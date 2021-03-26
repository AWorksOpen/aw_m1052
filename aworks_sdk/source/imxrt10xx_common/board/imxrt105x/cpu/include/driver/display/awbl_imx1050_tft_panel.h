/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-20126 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief  TFT屏显示面板通用驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-21  mex, first implemetation
 * \endinternal
 */

#ifndef __AWBL_IMX1050_TFT_PANEL_H
#define __AWBL_IMX1050_TFT_PANEL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "driver/display/awbl_imx1050_lcdif.h"

#define AWBL_IMX1050_TFT_PANEL_NAME            "tft_panel"


typedef struct awbl_imx1050_tft_panel_devinfo {

    awbl_imx1050_panel_info_t       panel_info;
    awbl_imx1050_dotclk_timing_t    timing;
    void (*pfunc_plfm_init)(void);    /**< \brief 平台相关初始化：开启时钟、初始化引脚 */

} awbl_imx1050_tft_panel_devinfo_t;

typedef struct awbl_imx1050_tft_panel {

    awbl_dev_t           dev;
    awbl_imx1050_panel_t panel;
    awbl_pwm_it_dev_t    pwm_it;

} awbl_imx1050_tft_panel_t;

/**
 * \brief TFT显示屏驱动注册
 */
void awbl_imx1050_tft_panel_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_IMX1050_TFT_PANEL_H */

/* end of file */

