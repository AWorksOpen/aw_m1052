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
 * \brief AWBus-lite iMX1050 PIT 配置文件
 *
 * \internal
 * \par modification history
 * - 1.00 2017-11-14  pea, first implementation
 * \endinternal
 */

#ifndef  __AWBL_HWCONF_IMX1050_PIT_H
#define  __AWBL_HWCONF_IMX1050_PIT_H

#ifdef AW_DEV_IMX1050_PIT

#include "aw_int.h"
#include "driver/timer/awbl_imx10xx_pit.h"

/** \brief PIT 设备信息 */
aw_local aw_const awbl_imx10xx_pit_devinfo_t __g_imx1050_pit_devinfo = {
    1,                              /* 0 分配一个符合指定要求的定时器，1 通过名称分配定时器 */
    IMX1050_PIT_BASE,               /* 寄存器基地址 */
    INUM_PIT,                       /* 中断号 */
    IMX1050_CLK_PERCLK_CLK_ROOT,    /* 时钟 ID */
    NULL                            /* 无平台初始化函数 */
};

/** \brief 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_pit_dev __g_imx1050_pit_dev;

#define AWBL_HWCONF_IMX1050_PIT     \
    {                               \
        AWBL_IMX10XX_PIT_DRV_NAME,  \
        IMX1050_PIT_BUSID,          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &__g_imx1050_pit_dev.dev,   \
        &__g_imx1050_pit_devinfo    \
    },

#else
#define AWBL_HWCONF_IMX1050_PIT

#endif  /* AW_DEV_IMX1050_PIT */

#endif  /* __AWBL_HWCONF_IMX1050_PIT_H */

/* end of file */
