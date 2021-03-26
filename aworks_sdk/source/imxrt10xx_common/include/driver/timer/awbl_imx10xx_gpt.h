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
 * \brief iMX RT10xx GPT ����
 *
 * ������ʵ���˶�ʱ���������жϹ��ܣ��Լ�ƥ��Ͳ����ܡ�
 *
 * \internal
 * \par modification history
 * - 1.00 17-11-09  pea, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_GPT_H
#define __AWBL_IMX10xx_GPT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbus_lite.h"
#include "awbl_timer.h"
#include "aw_clk.h"
#include "driver/timer/imx10xx_gpt_regs.h"

#define AWBL_IMX10XX_GPT_DRV_NAME   "imx10xx_gpt"

/** \brief �豸������Ϣ */
typedef struct awbl_imx10xx_gpt_devinfo {
    uint32_t           alloc_byname;          /**< \brief �� 1 ��λ��ʾ�ö�ʱ������ͨ�����ƽ��з��� */
    uint32_t           reg_addr;              /**< \brief �Ĵ�������ַ */
    int                inum;                  /**< \brief �жϺ� */
    const aw_clk_id_t  clk_id;                /**< \brief ָ��ʱ�� ID �����ָ�� */
    void             (*pfn_plfm_init) (void); /**< \brief ƽ̨��ʼ�� */
} awbl_imx10xx_gpt_devinfo_t;

/** \brief �豸ʵ�� */
typedef struct awbl_imx10xx_gpt_dev {
    awbl_dev_t              dev;                   /**< \brief AWBus �豸ʵ���ṹ�� */
    struct awbl_timer       tmr;                   /**< \brief ��׼��ʱ���ṹ */
    imx10xx_gpt_regs_t     *p_reg;                 /**< \brief �Ĵ�������ַ */

    uint32_t                count;                 /**< \brief ��ǰ������ֵ */
    uint32_t                rollover_period;       /**< \brief ��ת���� */

    struct awbl_timer_param param;                 /**< \brief ��׼��ʱ������ */

    void                  (*pfn_isr)(void *p_arg); /**< \brief ��ʱ���жϻص����� */
    void                   *p_arg;                 /**< \brief ��ʱ���жϻص��������� */
    volatile int            enabled;               /**< \brief ��ʱ���Ƿ�ʹ�� */
} awbl_imx10xx_gpt_dev_t;

/**
 * \brief �� IMX10xx GPT ����ע�ᵽ AWBus ��ϵͳ��
 *
 * \note ������Ӧ�� aw_prj_early_init() �г�ʼ�����������н�ֹ���ò���ϵͳ��غ���
 */
void awbl_imx10xx_gpt_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_IMX10xx_GPT_H */

/* end of file */
