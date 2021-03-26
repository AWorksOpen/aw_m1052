/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief SPI从机演示例程（以直接操作寄存器的方式测试SPI的基本读写功能）
 *
 * - 操作步骤:
 *   1. 将主机 SPI 的 MOSI、MISO、SCK、CS 引脚用杜邦线和从机设备的对应引脚相连；
 *   2. 关闭AW_DEV_IMX1050_LPSPI3设备宏；
 *   3. __spi_slave_send函数为从机发送数据函数可调用该函数发送数据；
 *   4. __timer_callback为定时器的回调函数，当完成一次消息传输时会调用该回调。
 *
 * - 实验现象:
 *   1. 从机接收到主机的数据后，将收到的数据通过SPI发送回去；
 *   2. 第一次收到的数据时从机没有发生任何数据，收到的第二次数据，回复上一次主机
 *      发送的数据。即第一次传输不回复数据，下一次传输回复上一次主机发送的数据。
 *
 * - 备注：
 *   1. 主机和从机同时上电；
 *   2. 主机和丛机需要共地；
 *   3. 该丛机目前主要是作为配套例程，用于验证spi主机功能。
 *
 * \par 源代码
 * \snippet demo_spi_slave.c src_spi_slave
 *
 * \internal
 * \par History
 * - 1.00 20-04-16  licl, first implementation.
 * \endinternal
 */

#include "apollo.h"
#include "aworks.h"
#include "aw_led.h"
#include "aw_delay.h"
#include "imx1050_inum.h"
#include "aw_rt10xx_lpspi_slv.h"

#define SPI_DEV_BPW  32       /* SPI数据位数，取值只能是8、16、32 */
#define __TEST_LEN   256      /* 接收与发送数据的个数 */

uint8_t                 rx_buf[__TEST_LEN ] = {0};   /* 数据缓存 */
uint8_t                 tx_buf[__TEST_LEN ] = {0};   /* 数据缓存 */
imx10xx_lpspi_slv_dev_t lpspi3_slv_dev;              /* spi从设备实例 */

static int __rx_isr (void *p_arg)
{
    imx10xx_lpspi_slv_dev_t  *p_dev      = (imx10xx_lpspi_slv_dev_t *)p_arg;
    imx10xx_lpspi_slv_regs_t *p_hw_lpspi = p_dev->p_hw_lpspi;
    uint8_t                  *p_rx_buf   = p_dev->p_rx_buf;
    uint8_t                  *p_tx_buf   = p_dev->p_tx_buf;
    
    uint32_t status       = 0;
    uint32_t i            = 0;
    uint32_t rxfifo_index = 0;
    
    /* 关闭定时器 */
    aw_timer_stop(&p_dev->spi_slave_timer);
    
    /* 清中断标志位 */
    status = readl((void *)&p_hw_lpspi->sr);
    AW_REG_WRITE32(&p_hw_lpspi->sr, status);

    /* 获取 receive FIFO 中数据的数量 */
    int len = (AW_REG_READ32(&p_hw_lpspi->fsr) >> 16) & 0x1F;

    rxfifo_index      = p_dev->rx_nbytes;
    p_dev->rx_nbytes += len;


    if (lpspi3_slv_dev.bpw == 8) {
        /* 读取数据放入rx缓存 */
        for (i = rxfifo_index; i < p_dev->rx_nbytes; i++) {
            p_rx_buf[i] = readl((void *)&p_hw_lpspi->rdr);
        }
        /* 向管道中添加数据 */
        while(((AW_REG_READ32(&p_hw_lpspi->fsr) & 0x1f) < 15) && \
               (p_dev->tx_index < p_dev->tx_nbytes)) {

            AW_REG_WRITE32(&p_hw_lpspi->tdr, p_tx_buf[p_dev->tx_index]);
            p_dev->tx_index++;
        }
    } else if (lpspi3_slv_dev.bpw == 16) {
        /* 读取数据放入rx缓存 */
        for (i = rxfifo_index; i < p_dev->rx_nbytes; i++) {
            ((uint16_t *)p_rx_buf)[i] = (uint16_t)readl((void *)&p_hw_lpspi->rdr);
        }
        /* 向管道中添加数据 */
        while(((AW_REG_READ32(&p_hw_lpspi->fsr) & 0x1f) < 15) && \
               (p_dev->tx_index < p_dev->tx_nbytes)) {

            AW_REG_WRITE32(&p_hw_lpspi->tdr, ((uint16_t *)p_tx_buf)[p_dev->tx_index]);
            p_dev->tx_index++;
        }
    } else {
          /* 读取数据放入rx缓存 */
          for (i = rxfifo_index; i < p_dev->rx_nbytes; i++) {
              ((uint32_t *)p_rx_buf)[i] = (uint32_t)readl((void *)&p_hw_lpspi->rdr);
          }
          /* 向管道中添加数据 */
          while(((AW_REG_READ32(&p_hw_lpspi->fsr) & 0x1f) < 15) && \
                 (p_dev->tx_index < p_dev->tx_nbytes)) {

              AW_REG_WRITE32(&p_hw_lpspi->tdr, ((uint32_t *)p_tx_buf)[p_dev->tx_index]);
              p_dev->tx_index++;
          }
    }

    /* 启动定时器 超过2ms没有进中断 则代表本次传输完成 */
    aw_timer_start(&p_dev->spi_slave_timer, aw_ms_to_ticks(2));

    return AW_OK;
}

/* 定时器回调函数 当一次传输成功时会调用该函数 */
aw_local void __timer_callback (void *p_arg)
{
    imx10xx_lpspi_slv_dev_t  *p_dev      = (imx10xx_lpspi_slv_dev_t *)p_arg;
    uint8_t                  *p_rx_buf   = p_dev->p_rx_buf;
    uint8_t                   len        = p_dev->rx_nbytes;

    aw_timer_stop(&p_dev->spi_slave_timer); /* 关闭定时器 */

    p_dev->rx_nbytes = 0;

    /* 将收到的数据发送出去 */
    lpspi_slv_send(p_dev,p_rx_buf,len);

}

void lpspi3_slv_demo (void)
{
    lpspi3_slv_dev.p_rx_buf         = rx_buf;
    lpspi3_slv_dev.p_tx_buf         = tx_buf;
    lpspi3_slv_dev.pfn_isr_active   = __rx_isr;
    lpspi3_slv_dev.bpw              = SPI_DEV_BPW;
    lpspi3_slv_dev.p_arg            = &lpspi3_slv_dev;

    /* 引脚配置 */
    imx1050_lpspi3_slv_plfm_init();

    /* SPI从设备配置 */
    lpspi_slv_configuration(&lpspi3_slv_dev,
                            (uint32_t *)LPSPI3_BASE_ADDR,
                            INUM_LPSPI3,
                            lpspi3_slv_dev.bpw ,
                            AW_SPI_CPHA);
                            
    /* 软件定时器初始化 当完成一次传输会调用回调函数 */
    aw_timer_init(&lpspi3_slv_dev.spi_slave_timer,__timer_callback,&lpspi3_slv_dev);

    /* 使能 RxInterruput */
    lpspi_interrupts_enable((imx10xx_lpspi_slv_regs_t *)LPSPI3_BASE_ADDR, 0x02);

    while (1) {
        aw_led_toggle(0);
        aw_mdelay(500);
    }
}

/*end of file */
