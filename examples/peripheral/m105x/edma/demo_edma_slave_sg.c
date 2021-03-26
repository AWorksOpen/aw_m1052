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
 * \brief edma����(slave_sg������� )
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_IMX1050_EDMA
 *      - AW_DEV_IMX1050_LPUART3
 *   2. ��UART3��TX��RX�̽ӡ�
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *
 * \par Դ����
 * \snippet demo_edma_slave_sg.c src_edma_slave_sg
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_edma_slave_sg EDMA(�Ի�����)
 * \copydoc demo_edma_slave_sg.c
 */

/** [src_edma_slave_sg] */
#include "aworks.h"
#include "aw_cache.h"
#include "aw_sem.h"
#include "string.h"
#include "aw_serial.h"
#include "aw_vdebug.h"
#include "driver/edma/aw_imx10xx_edma.h"

#define     UART_BAUD      19200

/* slave_sg����Ļص����� */
static void dma_slave_sg_callback (void *pVoid, unsigned int len)
{
    aw_semb_id_t            sem_id;

    sem_id = (aw_semb_id_t)pVoid;

    aw_semb_give(sem_id);
}

/*
 * /brief �������
 */
void demo_edma_slave_sg_entry (void)
{
    void                    *tx_chan    = NULL;
    void                    *rx_chan    = NULL;
    struct imx_dma_data      data       = {0};
    struct dma_slave_config  cfg        = {0};
    char                    *tx_buf, *rx_buf;
    struct scatterlist       tx_list;
    struct scatterlist       rx_list;
    int                      ret;
    uint32_t                 len        = 4096;
    aw_semb_id_t             rx_sem_id;
    aw_semb_id_t             tx_sem_id;
    AW_SEMB_DECL(rx_sem);
    AW_SEMB_DECL(tx_sem);

    AW_INFOF(("demo_edma_slave_sg_entry start...\r\n"));

    /* ��ʼ������3 */
    aw_serial_ioctl(2, SIO_MODE_SET, (void *)SIO_MODE_POLL);
    aw_serial_ioctl(2, SIO_BAUD_SET, (void *)UART_BAUD);        /* ���ò����� */

    /* �رմ���3�ķ���fifo */
    AW_REG_BIT_CLR32(0x4018C018, 18);
    AW_REG_BIT_CLR32(0x4018C018, 19);
    AW_REG_BIT_CLR32(0x4018C028, 7);
    AW_REG_BIT_CLR32(0x4018C028, 3);
    AW_REG_BIT_SET32(0x4018C018, 18);
    AW_REG_BIT_SET32(0x4018C018, 19);

    /* UARTʹ��dma���з��� */
    AW_REG_BIT_SET32(0x4018C010, 23);

    /* UARTʹ��dma���н��� */
    AW_REG_BIT_SET32(0x4018C010, 21);

    /* ������ջ����� */
    rx_buf = (char *)aw_cache_dma_align(len, 4);
    if (rx_buf == NULL) {
        AW_INFOF(("aw_cache_dma_align error\r\n"));
        return ;
    }

    /* ���뷢�ͻ����� */
    tx_buf = (char *)aw_cache_dma_align(len, 4);
    if (tx_buf == NULL) {
        AW_INFOF(("aw_cache_dma_align error\r\n"));
        return ;
    }

    /* ������� */
    memset(rx_buf,'a', len);
    memset(tx_buf,'5', len);

    ret = memcmp(rx_buf, tx_buf, len);
    if (ret == 0) {
        AW_INFOF(("rx_buf and tx_buf equal.\r\n"));
        return ;
    }

    /* �������ȼ�����һ������3�ķ���ͨ�� */
    data.peripheral_type = IMX_DMATYPE_LPUART3_TX;
    data.priority        = DMA_PRIO_2;
    tx_chan = imx10xx_edma_alloc_chan(&data);
    if (tx_chan == NULL) {
        AW_INFOF(("imx10xx_edma_alloc_chan error\r\n"));
        return ;
    }

    /* ���÷���ͨ�������� */
    cfg.direction       = DMA_MEM_TO_DEV;
    cfg.dst_addr_width  = DMA_SLAVE_BUSWIDTH_1_BYTE;
    cfg.src_addr_width  = DMA_SLAVE_BUSWIDTH_1_BYTE;
    cfg.dst_addr        = (dma_addr_t)(0x4018C01c);
    cfg.src_addr        = (dma_addr_t)0;
    ret = imx10xx_edma_control(tx_chan, DMA_SLAVE_CONFIG, (unsigned long)&cfg);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_control error.\r\n"));
        return ;
    }

    /* ׼��һ�����ʹ��� */
    tx_list.length = len;
    tx_list.dma_address = (dma_addr_t)tx_buf;
    ret = imx10xx_edma_prep_slave_sg(tx_chan, &tx_list, 1, DMA_MEM_TO_DEV);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_prep_slave_sg error.\r\n"));
        return ;
    }

    /* �������ȼ�����һ������3�Ľ���ͨ�� */
    data.peripheral_type = IMX_DMATYPE_LPUART3_RX;
    data.priority        = DMA_PRIO_1;
    rx_chan = imx10xx_edma_alloc_chan(&data);
    if (rx_chan == NULL) {
        AW_INFOF(("imx10xx_edma_alloc_chan error.\r\n"));
        return ;
    }

    /* ���ý���ͨ�������� */
    cfg.direction       = DMA_DEV_TO_MEM;
    cfg.dst_addr_width  = DMA_SLAVE_BUSWIDTH_1_BYTE;
    cfg.src_addr_width  = DMA_SLAVE_BUSWIDTH_1_BYTE;
    cfg.dst_addr        = (dma_addr_t)(0);
    cfg.src_addr        = (dma_addr_t)(0x4018C01c);
    ret = imx10xx_edma_control(rx_chan, DMA_SLAVE_CONFIG, (unsigned long)&cfg);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_control error\r\n"));
        return ;
    }

    /* ׼��һ�����մ��� */
    rx_list.dma_address = (dma_addr_t)rx_buf;
    rx_list.length = len;
    ret = imx10xx_edma_prep_slave_sg(rx_chan, &rx_list, 1, DMA_DEV_TO_MEM);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_prep_slave_sg error.\r\n"));
        return ;
    }

    rx_sem_id = AW_SEMB_INIT(rx_sem,AW_SEM_EMPTY,AW_SEM_Q_FIFO);
    tx_sem_id = AW_SEMB_INIT(tx_sem,AW_SEM_EMPTY,AW_SEM_Q_FIFO);

    /* ��ʼ���� */
    ret = imx10xx_edma_chan_start_async(rx_chan,
            dma_slave_sg_callback, (void *)rx_sem_id);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_chan_start_async error.\r\n"));
        return ;
    }

    /* ��ʼ���� */
    ret = imx10xx_edma_chan_start_async(tx_chan,
            dma_slave_sg_callback, (void *)tx_sem_id);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_chan_start_async error.\r\n"));
        return ;
    }

    ret = aw_semb_take(rx_sem_id, 10000);
    ret = aw_semb_take(tx_sem_id, 11000);

    /* �ر�UARTʹ��dma���з��� */
    AW_REG_BIT_CLR32(0x4018C010, 23);

    /* �ر�UARTʹ��dma���н��� */
    AW_REG_BIT_CLR32(0x4018C010, 21);

    /* �ͷŷ���ͨ�� */
    ret = imx10xx_edma_free_chan(tx_chan);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_free_chan error.\r\n"));
        return ;
    }

    /* �ͷŽ���ͨ�� */
    ret = imx10xx_edma_free_chan(rx_chan);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_free_chan error.\r\n"));
        return ;
    }

    ret = memcmp(rx_buf, tx_buf, len);
    if (ret != 0) {
        AW_INFOF(("data verify fail!\r\n"));
        return ;
    }

    aw_cache_dma_free(rx_buf);
    aw_cache_dma_free(tx_buf);

    AW_INFOF(("demo_edma_slave_sg_entry test OK.\r\n"));

    return ;
}

/** [src_edma_slave_sg] */

/* end of file */
