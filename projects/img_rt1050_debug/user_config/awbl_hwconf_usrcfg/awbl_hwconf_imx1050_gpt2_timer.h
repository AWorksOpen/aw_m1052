/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite iMX1050 GPT2 TIMER �����ļ�
 *
 * \internal
 * \par modification history
 * - 1.00 2017-11-10  pea, first implementation
 * \endinternal
 */

#ifndef  __AWBL_HWCONF_IMX1050_GPT2_TIMER_H
#define  __AWBL_HWCONF_IMX1050_GPT2_TIMER_H

#ifdef AW_DEV_IMX1050_GPT2_TIMER

#include "aw_int.h"
#include "driver/timer/awbl_imx10xx_gpt.h"

/** \brief GPT2 �豸��Ϣ */
aw_local aw_const awbl_imx10xx_gpt_devinfo_t __g_imx1050_gpt2_param = {
    1,                              /* 0 ����һ������ָ��Ҫ��Ķ�ʱ����1 ͨ�����Ʒ��䶨ʱ�� */
    IMX1050_GPT2_BASE,              /* �Ĵ�������ַ */
    INUM_GPT2,                      /* �жϺ� */
    IMX1050_CLK_CG_GPT2_SERIAL,     /* ʱ�� ID */
};

/** \brief �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx10xx_gpt_dev __g_imx1050_gpt2_dev;

#define AWBL_HWCONF_IMX1050_GPT2    \
    {                               \
        AWBL_IMX10XX_GPT_DRV_NAME,  \
        IMX1050_GPT2_BUSID,         \
        AWBL_BUSID_PLB,             \
        0,                          \
        &__g_imx1050_gpt2_dev.dev,  \
        &__g_imx1050_gpt2_param     \
    },

#else
#define AWBL_HWCONF_IMX1050_GPT2
#endif  /* AW_DEV_IMX1050_GPT2 */

#endif  /* __AWBL_HWCONF_IMX1050_GPT2_TIMER_H */

/* end of file */