/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef  __AWBL_HWCONF_IMX1050_MQS_H
#define  __AWBL_HWCONF_IMX1050_MQS_H

#ifdef AW_DEV_IMX1050_MQS

/*******************************************************************************
 MQS控制器配置信息
*******************************************************************************/
#include "aworks.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "driver/mqs/awbl_imx1050_mqs.h"
#include "aw_gpio.h"
#include "imx1050_pin.h"

#define __IMX1050_MQS_PAD_CTRL      IMX1050_PAD_CTL(                    \
                                        PUS_0_100K_Ohm_Pull_Down    |   \
                                        PKE_0_Pull_Keeper_Disabled  |   \
                                        PUE_0_Keeper                |   \
                                        Open_Drain_Disabled         |   \
                                        SPEED_1_medium_100MHz       |   \
                                        DSE_7_R0_7                  |   \
                                        SRE_0_Fast_Slew_Rate)


aw_local void __imx1050_mqs_plat_init (void)
{
    /* 引脚配置  */
    int mqs_gpios[] = {

#if 1
        /* 底板为M105x-EV-Board Rev.D 和 Rev.E 版本使用 */
        GPIO1_4,        /* MQS_RIGHT */
        GPIO1_5         /* MQS_LEFT */
#else
        /* 底板为M105x-EV-Board Rev.C版本使用 */
        GPIO2_0,        /* MQS_RIGHT */
        GPIO2_1         /* MQS_LEFT */
#endif
    };

    if (aw_gpio_pin_request("mqs_gpios",
                             mqs_gpios,
                             AW_NELEMENTS(mqs_gpios)) == AW_OK) {

        /* 配置功能引脚 */
#if 1
        /* 底板为M105x-EV-Board Rev.D 和 Rev.E 版本使用 */
        aw_gpio_pin_cfg(GPIO1_4, GPIO1_4_MQS_RIGHT | __IMX1050_MQS_PAD_CTRL);
        aw_gpio_pin_cfg(GPIO1_5, GPIO1_5_MQS_LEFT  | __IMX1050_MQS_PAD_CTRL);
#else
        /* 底板为M105x-EV-Board Rev.C版本使用 */
        aw_gpio_pin_cfg(GPIO2_0, GPIO2_0_MQS_RIGHT | __IMX1050_MQS_PAD_CTRL);
        aw_gpio_pin_cfg(GPIO2_1, GPIO2_1_MQS_LEFT  | __IMX1050_MQS_PAD_CTRL);
#endif

    }

    aw_clk_enable(IMX1050_CLK_CG_MQS);
}

/* 设备信息 */
aw_local aw_const struct awbl_imx1050_mqs_devinfo __g_imx1050_mqs_devinfo = {
     0x400AC008,                     /* IOMUX_GRP2 寄存器基地址 */
     IMX1050_CLK_SAI3_PODF,            /* 与MQS绑定的SAI时钟id */
     __imx1050_mqs_plat_init,        /* 平台相关初始化 */
     0,                              /* 声卡ID */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx1050_mqs_dev __g_imx1050_mqs_dev;

#define AWBL_HWCONF_IMX1050_MQS                     \
    {                                               \
        AWBL_IMX1050_MQS_NAME,                      \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx1050_mqs_dev,    \
        &__g_imx1050_mqs_devinfo                    \
    },

#else

#define AWBL_HWCONF_IMX1050_MQS

#endif  /* AW_DEV_IMX1050_MQS */

#endif /* __AWBL_HWCONF_IMX1050_MQS_H */

/* end of file */
