/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_OV7725_H
#define __AWBL_HWCONF_OV7725_H


#ifdef  AW_DEV_OV7725

#include "driver/camera/awbl_ov7725.h"

aw_local void __ov7725_plfm_init (void)
{
    int ov7725_ctr_gpios[] = { GPIO1_8, GPIO1_9};

    if (aw_gpio_pin_request("ov7725_ctr_gpios",
            ov7725_ctr_gpios, AW_NELEMENTS(ov7725_ctr_gpios)) == AW_OK) {

        /* csi_rst */
        aw_gpio_pin_cfg(GPIO1_8,  AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PUSH_PULL);

        /* csi_en */
        aw_gpio_pin_cfg(GPIO1_9,  AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PUSH_PULL);

    }
        
    aw_gpio_set(GPIO1_9, 1);
    aw_gpio_set(GPIO1_8, 0);
    aw_mdelay(2);

    aw_gpio_set(GPIO1_9, 0);
    aw_mdelay(2);

    aw_gpio_set(GPIO1_8, 1);
    aw_mdelay(30);
};

/* ov7725 device infomation */
aw_local aw_const struct awbl_ov7725_devinfo __g_ov7725_devinfo = {
        0,                                             /* 摄像头控制器对应的ID号  */
        1,                                             /* 摄像头IIC控制器 */
        AW_RESOLUTION_SET(320, 240),                   /* 默认video模式的分辨率 */
        0,                                             /* 默认photo模式的分辨率 */
        RGB565,                                        /* 像素格式  */
        24000000,                                      /* 摄像头芯片的输入时钟频率  */
        25,                                            /* 帧率，目前只支持30、25 */
        (AW_CAM_HREF_ACTIVE_HIGH |
                AW_CAM_DATA_LATCH_ON_RISING_EDGE|
                AW_CAM_VSYNC_ACTIVE_LOW),             /* 时钟极性的设置  */
        4,                                             /* 预览buffer个数应为 >=3 */
        "ov7725",                                      /* 摄像头名  */
        __ov7725_plfm_init
};

/* allocate wdt1 device instance RAM */
aw_local struct awbl_ov7725_dev __g_ov7725_dev;

#define  AWBL_HWCONF_OV7725          \
    {                                \
        AWBL_OV7725_NAME,            \
        0,                           \
        AWBL_BUSID_PLB,              \
        0,                           \
        (struct awbl_dev *)&__g_ov7725_dev, \
        &(__g_ov7725_devinfo)  \
    },

#else
#define AWBL_HWCONF_OV7725

#endif  /* AW_DEV_OV7725 */


#endif /* __AWBL_HWCONF_OV7725_H */
