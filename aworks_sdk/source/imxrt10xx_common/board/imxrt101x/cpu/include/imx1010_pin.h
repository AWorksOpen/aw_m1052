/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT1010 引脚定义.
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-10, mex, first implementation.
 * \endinternal
 */

#ifndef __IMX1010_GPIO_PIN_H
#define __IMX1010_GPIO_PIN_H

#include "driver/gpio/awbl_imx1010_gpio_private.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @{ */

/******************************************************************************/

/**
 * \name iMX RT1010 引脚编号定义
 * @{
 */

#define GPIO1_0     (0)     /* GPIO_00 */
#define GPIO1_1     (1)     /* GPIO_01 */
#define GPIO1_2     (2)     /* GPIO_02 */
#define GPIO1_3     (3)     /* GPIO_03 */
#define GPIO1_4     (4)     /* GPIO_04 */
#define GPIO1_5     (5)     /* GPIO_05 */
#define GPIO1_6     (6)     /* GPIO_06 */
#define GPIO1_7     (7)     /* GPIO_07 */
#define GPIO1_8     (8)     /* GPIO_08 */
#define GPIO1_9     (9)     /* GPIO_09 */
#define GPIO1_10    (10)    /* GPIO_10 */
#define GPIO1_11    (11)    /* GPIO_11 */
#define GPIO1_12    (12)    /* GPIO_12 */
#define GPIO1_13    (13)    /* GPIO_13 */
#define GPIO1_14    (14)    /* GPIO_AD_00 */
#define GPIO1_15    (15)    /* GPIO_AD_01 */
#define GPIO1_16    (16)    /* GPIO_AD_02 */
#define GPIO1_17    (17)    /* GPIO_AD_03 */
#define GPIO1_18    (18)    /* GPIO_AD_04 */
#define GPIO1_19    (19)    /* GPIO_AD_05 */
#define GPIO1_20    (20)    /* GPIO_AD_06 */
#define GPIO1_21    (21)    /* GPIO_AD_07 */
#define GPIO1_22    (22)    /* GPIO_AD_08 */
#define GPIO1_23    (23)    /* GPIO_AD_09 */
#define GPIO1_24    (24)    /* GPIO_AD_10 */
#define GPIO1_25    (25)    /* GPIO_AD_11 */
#define GPIO1_26    (26)    /* GPIO_AD_12 */
#define GPIO1_27    (27)    /* GPIO_AD_13 */
#define GPIO1_28    (28)    /* GPIO_AD_14 */

#define GPIO2_0     (32)    /* GPIO_SD_00 */
#define GPIO2_1     (33)    /* GPIO_SD_01 */
#define GPIO2_2     (34)    /* GPIO_SD_02 */
#define GPIO2_3     (35)    /* GPIO_SD_03 */
#define GPIO2_4     (36)    /* GPIO_SD_04 */
#define GPIO2_5     (37)    /* GPIO_SD_05 */
#define GPIO2_6     (38)    /* GPIO_SD_06 */
#define GPIO2_7     (39)    /* GPIO_SD_07 */
#define GPIO2_8     (40)    /* GPIO_SD_08 */
#define GPIO2_9     (41)    /* GPIO_SD_09 */
#define GPIO2_10    (42)    /* GPIO_SD_10 */
#define GPIO2_11    (43)    /* GPIO_SD_11 */
#define GPIO2_12    (44)    /* GPIO_SD_12 */
#define GPIO2_13    (45)    /* GPIO_SD_13 */
#define GPIO2_14    (46)    /* GPIO_SD_14 */

#define GPIO5_0     (47)    /* PMIC_ON_REQ */



/** @} */

/**
 * \name iMX RT1010 引脚编号最大值和引脚总数
 * @{
 */

#define IMX1010_GPIO_PIN_MAX       GPIO5_0
#define IMX1010_GPIO_PINS         ((GPIO5_0 - GPIO1_0) + 1)

/** @} */

/** @} grp_imx_rt1010_pins */

#ifdef __cplusplus
}
#endif

#endif /* __IMX1010_GPIO_PIN_H */

/* end of file */
