/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx PIT ����
 *
 * ������ʵ���˶�ʱ���������жϹ��ܡ�
 *
 * \internal
 * \par modification history
 * - 1.00 17-11-13  pea, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_PIT_H
#define __AWBL_IMX10xx_PIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbus_lite.h"
#include "awbl_timer.h"
#include "aw_clk.h"
#include "driver/timer/imx10xx_pit_regs.h"

#define AWBL_IMX10XX_PIT_DRV_NAME   "imx10xx_pit"

/** \brief �豸������Ϣ */
typedef struct awbl_imx10xx_pit_devinfo {
    uint32_t           alloc_byname;   /**< \brief �� 1 ��λ��ʾ�ö�ʱ������ͨ�����ƽ��з��� */
    uint32_t           regbase;        /**< \brief �Ĵ�������ַ */
    int                inum;           /**< \brief �жϺ� */
    const aw_clk_id_t  clk_id;         /**< \brief ָ��ʱ�� ID �����ָ�� */

    /** \brief ָ��ƽ̨��ʼ��������ָ�� */
    void             (*pfn_plfm_init) (void);
} awbl_imx10xx_pit_devinfo_t;

/** \brief �豸ʵ�� */
typedef struct awbl_imx10xx_pit_dev {
    awbl_dev_t              dev;                   /**< \brief AWBus �豸ʵ���ṹ�� */
    struct awbl_timer       tmr;                   /**< \brief ��׼��ʱ���ṹ */

    uint32_t                count;                 /**< \brief ��ǰ������ֵ */
    uint32_t                rollover_period;       /**< \brief ��ת���� */

    struct awbl_timer_param param;                 /**< \brief ��׼��ʱ������ */

    void                  (*pfn_isr)(void *p_arg); /**< \brief ��ʱ���жϻص����� */
    void                   *p_arg;                 /**< \brief ��ʱ���жϻص��������� */
    volatile int            enabled;               /**< \brief ��ʱ���Ƿ�ʹ�� */
} awbl_imx10xx_pit_dev_t;

/**
 * \brief �� IMX10xx PIT ����ע�ᵽ AWBus ��ϵͳ��
 *
 * \note ������Ӧ�� aw_prj_early_init() �г�ʼ�����������н�ֹ���ò���ϵͳ��غ���
 */
void awbl_imx10xx_pit_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_IMX10xx_PIT_H */

/* end of file */
