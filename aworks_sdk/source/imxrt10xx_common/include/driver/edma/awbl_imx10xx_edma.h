/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief imx10xx eDMA driver header file
 *
 *   eEMA ����ʵ����ΧI/O�豸���ڲ����ⲿ��������ݴ���
 * \internal
 * \par Modification History
 * - 1.00 17-11-07  hsg, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_EDMA_H
#define __AWBL_IMX10xx_EDMA_H

#include "aworks.h"
#include "driver/edma/aw_imx10xx_edma.h"
#include "aw_sem.h"
#include "aw_task.h"
#include "driver/edma/awbl_imx10xx_edma_service.h"
#include "driver/edma/imx10xx_edma_regs.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

struct edma_engine;

struct edma_channel {
    struct edma_engine             *edma;              /* ָ��eDMA�豸�ṹ��  */
    uint32_t                        alloced;            /* ��ͨ���Ƿ������־ */
    uint32_t                        channel;            /* ͨ���� */
    enum dma_transfer_direction     direction;          /* ���䷽�� */
    enum edma_peripheral_type       peripheral_type;    /* �������� */
    dma_addr_t                      base_addr;          /* ����һ����ַ���ڼ��㴫�����Ŀ(���ڷ�ѭ��ģʽ) */
    enum dma_slave_buswidth         src_width;          /* Դ��ַ��� */
    enum dma_slave_buswidth         dst_width;          /* Ŀ���ַ��� */
    enum dma_status                 status;             /* �����״̬ */

    uint32_t                        tcd_usd;            /* ������TCD�洢�ռ�  */
    void                            *p_tcd;              /* TCD�洢�ռ��ָ�� */

    uint32_t                        circle;             /* �Ƿ�Ϊѭ������  */

    uint32_t                        count;              /* �������Ŀ  */

    enum dma_chan_cfg_status        cfg;                /* ͨ������״̬  */

    pfn_dma_callback_t              fn_callback;        /* �ص����� */
    void                           *param;              /* ���ûص������Ĳ��� */

    AW_MUTEX_DECL(chan_lock);

};

#define     DMA_INTR_TASK_STACK_SIZE    512

/**
 * \brief EDMA �豸�ṹ��
 */
typedef struct edma_engine {
    struct awbl_dev                 dev;
    struct edma_channel             channel[MAX_EDMA_CHANNELS];

    AW_TASK_DECL(dma_intr_task, DMA_INTR_TASK_STACK_SIZE);   /* DMA�ж����� */
    AW_SEMB_DECL(dma_intr_semb);                            /* �����Ƿ���dma�жϲ��� */

    AW_MUTEX_DECL(dev_lock);
    struct awbl_imx10xx_edma_service edma_serv;

} awbl_imx10xx_edma_dev_t;

/**
 * \brief imx10xx eDMA  �豸��Ϣ
 */
typedef struct awbl_imx10xx_edma_devinfo {

    /** \brief edma�Ĵ�������ַ */
    uint32_t regbase;

    /** \brief dmamux�Ĵ�������ַ  */
    uint32_t dmamux_regbase;

    /** \brief eEMA �жϺ���� */
    const uint32_t *inum;

    /** \brief �жϺų��� */
    uint32_t inum_len;

    uint32_t err_inum;

    /** \brief ƽ̨��س�ʼ�� */
    void (*pfunc_plfm_init)(void);
} awbl_imx10xx_edma_devinfo_t;


void awbl_imx10xx_edma_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */


