/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_SENSOR_LPS22HB_H
#define __AWBL_HWCONF_SENSOR_LPS22HB_H

#ifdef AW_DEV_SENSOR_LPS22HB
#include "driver/sensor/awbl_lps22hb.h"


/**
 * \addtogroup grp_aw_plfm_ext_lps22hb_usrcfg
 * @{
 */

/* 平台相关初始化 */
aw_local void __lps22hb_plfm_init (void)
{

    int lps22hb_gpios[] = {GPIO1_24};  /* NPD管脚 */

    if (aw_gpio_pin_request("lps22hb_gpios",
                             lps22hb_gpios,
                             AW_NELEMENTS(lps22hb_gpios)) == AW_OK) {
        aw_gpio_pin_cfg(lps22hb_gpios[0], AW_GPIO_INPUT);
    }
}

/* LPS22HB 设备信息 */
aw_local aw_const awbl_lps22hb_param_t __g_lps22hb_devinfo = {
    SENSOR_LPS22HB_START_ID,
    GPIO1_24,
    0x5C,
    __lps22hb_plfm_init,
};

/* lps22hb 设备实例内存静态分配 */
aw_local awbl_lps22hb_dev_t __g_lps22hb_dev0;

#define AWBL_HWCONF_LPS22HB                   \
    {                                         \
        AWBL_LPS22HB_NAME,                    \
        0,                                    \
        AWBL_BUSID_I2C,                       \
        IMX1050_LPI2C1_BUSID,                 \
        (struct awbl_dev *)&__g_lps22hb_dev0, \
        &__g_lps22hb_devinfo                  \
    },

/** @} */

#else
#define AWBL_HWCONF_LPS22HB
#endif

#endif
