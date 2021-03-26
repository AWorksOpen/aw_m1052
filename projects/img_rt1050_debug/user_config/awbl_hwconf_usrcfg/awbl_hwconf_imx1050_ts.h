/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX1050_TS_H
#define __AWBL_HWCONF_IMX1050_TS_H

#ifdef AW_DEV_IMX1050_TS

#include "driver/input/touchscreen/awbl_imx1050_ts.h"
#include "aw_ts.h"



aw_local void __imx1050_ts_plfm_init (void)
{
    aw_clk_enable(IMX1050_CLK_CG_TSC_DIG);
}


aw_local awbl_imx1050_ts_devinfo_t __g_imx1050_ts_devinfo = {
        IMX1050_TS_CTRL_BASE,     /* 寄存器基地址 */
        1,                        /* x_plus_ch  : ADC channel 1 */
        2,                        /* x_minus_ch : ADC channel 2 */
        3,                        /* y_plus_ch  : ADC channel 3 */
        4,                        /* y_minus_ch : ADC channel 4 */
        {
            "imx105x-ts",                    /**< \brief 触摸关联的显示屏名字       */
            0x01,                         /**< \brief 触摸关联的显示屏产商ID */
            0x01,                         /**< \brief 触摸关联的显示屏产品ID */
            5,                            /**< \brief 支持最多触摸点的个数.    */
            35,                           /**< \brief 采样个数, 最小值为1  */
            8,                            /**< \brief 滤波个数, 不能大于采样个数, 最小值为1 */
            60,                           /**< \brief 消抖时长      */
            120,                          /**< \brief 采样门限阀值,用于消抖,根据关联不同的显示屏可做适配调整. */
            5,                            /**< \brief 校准门限阀值,用于验证校准系数,根据关联不同的显示屏可做适配调整. */
            AW_TS_LIB_FIVE_POINT,         /**< \brief 五点校准算法   */
        },
       __imx1050_ts_plfm_init     /* 平台初始化 */
};

/* 设备实例内存静态分配 */
aw_local awbl_imx1050_ts_dev_t __g_imx1050_ts;

#define AWBL_HWCONF_IMX1050_TS          \
    {                                   \
        AWBL_IMX1050_TOUCHSCREEN_NAME,  \
        0,                              \
        AWBL_BUSID_PLB,                 \
        0,                              \
        &(__g_imx1050_ts.adev),         \
        &(__g_imx1050_ts_devinfo)       \
    },

#else
#define AWBL_HWCONF_IMX1050_TS

#endif  /* AW_DEV_IMX1050_TS */

#endif  /* __AWBL_HWCONF_IMX1050_TS_H */

/* end of file */
