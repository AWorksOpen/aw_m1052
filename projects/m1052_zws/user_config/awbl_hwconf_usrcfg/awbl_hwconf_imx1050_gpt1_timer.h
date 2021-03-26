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
 * \brief AWBus-lite iMX1050 GPT1 TIMER 配置文件
 *
 * \internal
 * \par modification history
 * - 1.00 2017-11-10  pea, first implementation
 * \endinternal
 */

#ifndef  __AWBL_HWCONF_IMX1050_GPT1_TIMER_H
#define  __AWBL_HWCONF_IMX1050_GPT1_TIMER_H

#ifdef AW_DEV_IMX1050_GPT1_TIMER

#include "aw_int.h"
#include "driver/timer/awbl_imx10xx_gpt.h"

aw_local aw_const awbl_imx10xx_gpt_devinfo_t __g_imx1050_gpt1_param = {
    1,                              /* 0 分配一个符合指定要求的定时器，1 通过名称分配定时器 */
    IMX1050_GPT1_BASE,              /* 寄存器基地址 */
    INUM_GPT1,                      /* 中断号 */
    IMX1050_CLK_CG_GPT_SERIAL,      /* 时钟 ID */
};

/** \brief 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_gpt_dev __g_imx1050_gpt1_dev;

#define AWBL_HWCONF_IMX1050_GPT1    \
    {                               \
        AWBL_IMX10XX_GPT_DRV_NAME,  \
        IMX1050_GPT1_BUSID,         \
        AWBL_BUSID_PLB,             \
        0,                          \
        &__g_imx1050_gpt1_dev.dev,  \
        &__g_imx1050_gpt1_param     \
    },

#else
#define AWBL_HWCONF_IMX1050_GPT1
#endif  /* AW_DEV_IMX1050_GPT1 */

#endif  /* __AWBL_HWCONF_IMX1050_GPT1_TIMER_H */

/* end of file */
