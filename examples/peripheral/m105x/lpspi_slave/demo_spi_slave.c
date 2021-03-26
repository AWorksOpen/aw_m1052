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
 * \brief SPI�ӻ���ʾ���̣���ֱ�Ӳ����Ĵ����ķ�ʽ����SPI�Ļ�����д���ܣ�
 *
 * - ��������:
 *   1. ������ SPI �� MOSI��MISO��SCK��CS �����öŰ��ߺʹӻ��豸�Ķ�Ӧ����������
 *   2. �ر�AW_DEV_IMX1050_LPSPI3�豸�ꣻ
 *   3. __spi_slave_send����Ϊ�ӻ��������ݺ����ɵ��øú����������ݣ�
 *   4. __timer_callbackΪ��ʱ���Ļص������������һ����Ϣ����ʱ����øûص���
 *
 * - ʵ������:
 *   1. �ӻ����յ����������ݺ󣬽��յ�������ͨ��SPI���ͻ�ȥ��
 *   2. ��һ���յ�������ʱ�ӻ�û�з����κ����ݣ��յ��ĵڶ������ݣ��ظ���һ������
 *      ���͵����ݡ�����һ�δ��䲻�ظ����ݣ���һ�δ���ظ���һ���������͵����ݡ�
 *
 * - ��ע��
 *   1. �����ʹӻ�ͬʱ�ϵ磻
 *   2. �����ʹԻ���Ҫ���أ�
 *   3. �ôԻ�Ŀǰ��Ҫ����Ϊ�������̣�������֤spi�������ܡ�
 *
 * \par Դ����
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

#define SPI_DEV_BPW  32       /* SPI����λ����ȡֵֻ����8��16��32 */
#define __TEST_LEN   256      /* �����뷢�����ݵĸ��� */

uint8_t                 rx_buf[__TEST_LEN ] = {0};   /* ���ݻ��� */
uint8_t                 tx_buf[__TEST_LEN ] = {0};   /* ���ݻ��� */
imx10xx_lpspi_slv_dev_t lpspi3_slv_dev;              /* spi���豸ʵ�� */

static int __rx_isr (void *p_arg)
{
    imx10xx_lpspi_slv_dev_t  *p_dev      = (imx10xx_lpspi_slv_dev_t *)p_arg;
    imx10xx_lpspi_slv_regs_t *p_hw_lpspi = p_dev->p_hw_lpspi;
    uint8_t                  *p_rx_buf   = p_dev->p_rx_buf;
    uint8_t                  *p_tx_buf   = p_dev->p_tx_buf;
    
    uint32_t status       = 0;
    uint32_t i            = 0;
    uint32_t rxfifo_index = 0;
    
    /* �رն�ʱ�� */
    aw_timer_stop(&p_dev->spi_slave_timer);
    
    /* ���жϱ�־λ */
    status = readl((void *)&p_hw_lpspi->sr);
    AW_REG_WRITE32(&p_hw_lpspi->sr, status);

    /* ��ȡ receive FIFO �����ݵ����� */
    int len = (AW_REG_READ32(&p_hw_lpspi->fsr) >> 16) & 0x1F;

    rxfifo_index      = p_dev->rx_nbytes;
    p_dev->rx_nbytes += len;


    if (lpspi3_slv_dev.bpw == 8) {
        /* ��ȡ���ݷ���rx���� */
        for (i = rxfifo_index; i < p_dev->rx_nbytes; i++) {
            p_rx_buf[i] = readl((void *)&p_hw_lpspi->rdr);
        }
        /* ��ܵ���������� */
        while(((AW_REG_READ32(&p_hw_lpspi->fsr) & 0x1f) < 15) && \
               (p_dev->tx_index < p_dev->tx_nbytes)) {

            AW_REG_WRITE32(&p_hw_lpspi->tdr, p_tx_buf[p_dev->tx_index]);
            p_dev->tx_index++;
        }
    } else if (lpspi3_slv_dev.bpw == 16) {
        /* ��ȡ���ݷ���rx���� */
        for (i = rxfifo_index; i < p_dev->rx_nbytes; i++) {
            ((uint16_t *)p_rx_buf)[i] = (uint16_t)readl((void *)&p_hw_lpspi->rdr);
        }
        /* ��ܵ���������� */
        while(((AW_REG_READ32(&p_hw_lpspi->fsr) & 0x1f) < 15) && \
               (p_dev->tx_index < p_dev->tx_nbytes)) {

            AW_REG_WRITE32(&p_hw_lpspi->tdr, ((uint16_t *)p_tx_buf)[p_dev->tx_index]);
            p_dev->tx_index++;
        }
    } else {
          /* ��ȡ���ݷ���rx���� */
          for (i = rxfifo_index; i < p_dev->rx_nbytes; i++) {
              ((uint32_t *)p_rx_buf)[i] = (uint32_t)readl((void *)&p_hw_lpspi->rdr);
          }
          /* ��ܵ���������� */
          while(((AW_REG_READ32(&p_hw_lpspi->fsr) & 0x1f) < 15) && \
                 (p_dev->tx_index < p_dev->tx_nbytes)) {

              AW_REG_WRITE32(&p_hw_lpspi->tdr, ((uint32_t *)p_tx_buf)[p_dev->tx_index]);
              p_dev->tx_index++;
          }
    }

    /* ������ʱ�� ����2msû�н��ж� ������δ������ */
    aw_timer_start(&p_dev->spi_slave_timer, aw_ms_to_ticks(2));

    return AW_OK;
}

/* ��ʱ���ص����� ��һ�δ���ɹ�ʱ����øú��� */
aw_local void __timer_callback (void *p_arg)
{
    imx10xx_lpspi_slv_dev_t  *p_dev      = (imx10xx_lpspi_slv_dev_t *)p_arg;
    uint8_t                  *p_rx_buf   = p_dev->p_rx_buf;
    uint8_t                   len        = p_dev->rx_nbytes;

    aw_timer_stop(&p_dev->spi_slave_timer); /* �رն�ʱ�� */

    p_dev->rx_nbytes = 0;

    /* ���յ������ݷ��ͳ�ȥ */
    lpspi_slv_send(p_dev,p_rx_buf,len);

}

void lpspi3_slv_demo (void)
{
    lpspi3_slv_dev.p_rx_buf         = rx_buf;
    lpspi3_slv_dev.p_tx_buf         = tx_buf;
    lpspi3_slv_dev.pfn_isr_active   = __rx_isr;
    lpspi3_slv_dev.bpw              = SPI_DEV_BPW;
    lpspi3_slv_dev.p_arg            = &lpspi3_slv_dev;

    /* �������� */
    imx1050_lpspi3_slv_plfm_init();

    /* SPI���豸���� */
    lpspi_slv_configuration(&lpspi3_slv_dev,
                            (uint32_t *)LPSPI3_BASE_ADDR,
                            INUM_LPSPI3,
                            lpspi3_slv_dev.bpw ,
                            AW_SPI_CPHA);
                            
    /* �����ʱ����ʼ�� �����һ�δ������ûص����� */
    aw_timer_init(&lpspi3_slv_dev.spi_slave_timer,__timer_callback,&lpspi3_slv_dev);

    /* ʹ�� RxInterruput */
    lpspi_interrupts_enable((imx10xx_lpspi_slv_regs_t *)LPSPI3_BASE_ADDR, 0x02);

    while (1) {
        aw_led_toggle(0);
        aw_mdelay(500);
    }
}

/*end of file */
