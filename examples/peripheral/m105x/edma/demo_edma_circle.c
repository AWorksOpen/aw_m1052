/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief edma例程(circle传输测试)
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_DEV_IMX1050_EDMA
 *      - AW_DEV_IMX1050_LPUART3
 *   2. 将UART3的TX和RX短接。
 *
 * - 实验现象：
 *   1. 串口打印调试信息。
 *
 * \par 源代码
 * \snippet demo_edma_circle.c src_edma_circle
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_edma_circle EDMA(循环传输)
 * \copydoc demo_edma_circle.c
 */

/** [src_edma_circle] */
#include "aworks.h"
#include "aw_mem.h"
#include "aw_cache.h"
#include "aw_sem.h"
#include "string.h"
#include "driver/edma/aw_imx10xx_edma.h"
#include "aw_vdebug.h"
#include "aw_serial.h"

#define     UART_BAUD      19200

/* circle传输的回调函数 */
static void edma_circle_callback (void *pVoid, unsigned int len)
{
    aw_semb_id_t sem_id;

    sem_id = (aw_semb_id_t)pVoid;

    aw_semb_give(sem_id);
}



/*
 * /brief circle传输测试入口
 */
void demo_edma_circle_entry (void)
{
    void                    *tx_chan    = NULL;
    void                    *rx_chan    = NULL;
    struct imx_dma_data      data       = {0};
    struct dma_slave_config  cfg        = {0};
    char                    *tx_circle_buf;
    char                    *rx_circle_buf;
    struct scatterlist       tx_list;
    int                      ret;
    uint32_t                 i          = 0;
    uint32_t                 len        = 1024;
    uint32_t                 period_len = 256;
    aw_semb_id_t             rx_sem_id;
    aw_semb_id_t             tx_sem_id;
    AW_SEMB_DECL(rx_sem);
    AW_SEMB_DECL(tx_sem);

    AW_INFOF(("demo_edma_circle_entry start...\r\n"));

    /* 初始化串口3 */
    aw_serial_ioctl(2, SIO_MODE_SET, (void *)SIO_MODE_POLL);
    aw_serial_ioctl(2, SIO_BAUD_SET, (void *)UART_BAUD);        /* 设置波特率 */

    /* 关闭串口3的发送fifo */
    AW_REG_BIT_CLR32(0x4018C018, 18);
    AW_REG_BIT_CLR32(0x4018C018, 19);
    AW_REG_BIT_CLR32(0x4018C028, 7);
    AW_REG_BIT_CLR32(0x4018C028, 3);
    AW_REG_BIT_SET32(0x4018C018, 18);
    AW_REG_BIT_SET32(0x4018C018, 19);

    /* UART使用dma进行发送 */
    AW_REG_BIT_SET32(0x4018C010, 23);

    /* UART使用dma进行接收 */
    AW_REG_BIT_SET32(0x4018C010, 21);

    /* 申请发送缓冲区 */
    tx_circle_buf = (char *)aw_cache_dma_align(len, len);
    if (tx_circle_buf == NULL) {
        AW_INFOF(("tx_circle_buf is NULL\r\n"));
        return ;
    }

    /* 申请接收缓冲区 */
    rx_circle_buf = (char *)aw_cache_dma_align(len, len);
    if (rx_circle_buf == NULL) {
        AW_INFOF(("rx_circle_buf is NULL\r\n"));
        return ;
    }

    /* 填充数据 */
    memset(tx_circle_buf, '4', len);
    memset(rx_circle_buf, '2', len);

    ret = memcmp(tx_circle_buf, rx_circle_buf, len);
    if (ret == 0) {
        AW_INFOF(("rx_circle_buf and tx_circle_buf equal!\r\n"));
        return ;
    }

    /* 根据优先级申请一个串口3的发送通道 */
    data.peripheral_type = IMX_DMATYPE_LPUART3_TX;
    data.priority        = DMA_PRIO_2;
    tx_chan = imx10xx_edma_alloc_chan(&data);
    if (tx_chan == NULL) {
        AW_INFOF(("tx_chan is NULL!\r\n"));
        return ;
    }

    /* 设置发送通道的属性 */
    cfg.direction       = DMA_MEM_TO_DEV;
    cfg.dst_addr_width  = DMA_SLAVE_BUSWIDTH_1_BYTE;
    cfg.src_addr_width  = DMA_SLAVE_BUSWIDTH_1_BYTE;
    cfg.dst_addr        = (dma_addr_t)(0x4018C01c);
    cfg.src_addr        = (dma_addr_t)0;
    ret = imx10xx_edma_control(tx_chan, DMA_SLAVE_CONFIG, (unsigned long)&cfg);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_control error!\r\n"));
        return ;
    }

    /* 准备一个发送传输 */
    tx_list.length = len;
    tx_list.dma_address = (dma_addr_t)tx_circle_buf;
    ret = imx10xx_edma_prep_slave_sg(tx_chan, &tx_list, 1, DMA_MEM_TO_DEV);
    if (ret != AW_OK) {
        return ;
    }

    /* 根据优先级申请一个串口3的接收通道 */
    data.peripheral_type = IMX_DMATYPE_LPUART3_RX;
    data.priority        = DMA_PRIO_3;
    rx_chan = imx10xx_edma_alloc_chan(&data);
    if (rx_chan == NULL) {
        AW_INFOF(("rx_chan is NULL!\r\n"));
        return ;
    }

    /* 设置接收通道的属性 */
    cfg.direction       = DMA_DEV_TO_MEM;
    cfg.dst_addr_width  = DMA_SLAVE_BUSWIDTH_1_BYTE;
    cfg.src_addr_width  = DMA_SLAVE_BUSWIDTH_1_BYTE;
    cfg.dst_addr        = (dma_addr_t)(0);
    cfg.src_addr        = (dma_addr_t)(0x4018C01c);
    ret = imx10xx_edma_control(rx_chan, DMA_SLAVE_CONFIG, (unsigned long)&cfg);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_control error!\r\n"));
        return ;
    }

    /* 准备一个接收传输 */
    ret = imx10xx_edma_prep_dma_cyclic(rx_chan,
            (dma_addr_t)rx_circle_buf, len, period_len,DMA_DEV_TO_MEM);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_prep_dma_cyclic error!\r\n"));
        return ;
    }

    rx_sem_id = AW_SEMB_INIT(rx_sem, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    /* 开始传输 */
    ret = imx10xx_edma_chan_start_async(rx_chan,
            edma_circle_callback, (void *)rx_sem_id);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_chan_start_async error!\r\n"));
        return ;
    }

    tx_sem_id = AW_SEMB_INIT(tx_sem,AW_SEM_EMPTY,AW_SEM_Q_FIFO);

    /* 开始传输 */
    ret = imx10xx_edma_chan_start_async(tx_chan,
            edma_circle_callback, (void *)tx_sem_id);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_chan_start_async error!\r\n"));
        return ;
    }

    for (i = 0; i < 4; i++) {
        ret = aw_semb_take(rx_sem_id, 1000);
    }

    ret = aw_semb_take(tx_sem_id, AW_SEM_WAIT_FOREVER);

    /* 关闭UART使用dma进行发送 */
    AW_REG_BIT_CLR32(0x4018C010, 23);

    /* 关闭UART使用dma进行接收 */
    AW_REG_BIT_CLR32(0x4018C010, 21);

    ret = imx10xx_edma_chan_stop(tx_chan);
    if (ret != AW_OK) {
        return ;
    }

    ret = imx10xx_edma_chan_stop(rx_chan);
    if (ret != AW_OK) {
        return ;
    }


    /* 释放发送通道 */
    ret = imx10xx_edma_free_chan(tx_chan);
    if (ret != AW_OK) {
        return ;
    }

    /* 释放接收通道 */
    ret = imx10xx_edma_free_chan(rx_chan);
    if (ret != AW_OK) {
        return ;
    }

    ret = memcmp(tx_circle_buf, rx_circle_buf, len);

    aw_cache_dma_free(tx_circle_buf);
    aw_cache_dma_free(rx_circle_buf);

    if (ret != AW_OK) {
        AW_INFOF((" data verify fail!\r\n"));
        return ;
    }

    AW_INFOF(("demo_edma_circle_entry test OK!\r\n"));
    return ;
}

/** [src_edma_circle] */

/* end of file */
