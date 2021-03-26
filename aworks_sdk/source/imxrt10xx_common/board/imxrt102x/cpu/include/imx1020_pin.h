/*******************************************************************************
*                                 AWorks
*                       ----------------------------
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
 * \brief iMX RT1020 引脚定义.
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-10, mex, first implementation.
 * \endinternal
 */

#ifndef __IMX1020_GPIO_PIN_H
#define __IMX1020_GPIO_PIN_H

#include "driver/gpio/awbl_imx1020_gpio_private.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @} */

/******************************************************************************/

/**
 * \name iMX RT1020 引脚编号定义
 * @{
 */

#define GPIO1_0     (0)
#define GPIO1_1     (1)
#define GPIO1_2     (2)
#define GPIO1_3     (3)
#define GPIO1_4     (4)
#define GPIO1_5     (5)
#define GPIO1_6     (6)
#define GPIO1_7     (7)
#define GPIO1_8     (8)
#define GPIO1_9     (9)
#define GPIO1_10    (10)
#define GPIO1_11    (11)
#define GPIO1_12    (12)
#define GPIO1_13    (13)
#define GPIO1_14    (14)
#define GPIO1_15    (15)
#define GPIO1_16    (16)
#define GPIO1_17    (17)
#define GPIO1_18    (18)
#define GPIO1_19    (19)
#define GPIO1_20    (20)
#define GPIO1_21    (21)
#define GPIO1_22    (22)
#define GPIO1_23    (23)
#define GPIO1_24    (24)
#define GPIO1_25    (25)
#define GPIO1_26    (26)
#define GPIO1_27    (27)
#define GPIO1_28    (28)
#define GPIO1_29    (29)
#define GPIO1_30    (30)
#define GPIO1_31    (31)

#define GPIO2_0     (32)
#define GPIO2_1     (33)
#define GPIO2_2     (34)
#define GPIO2_3     (35)
#define GPIO2_4     (36)
#define GPIO2_5     (37)
#define GPIO2_6     (38)
#define GPIO2_7     (39)
#define GPIO2_8     (40)
#define GPIO2_9     (41)
#define GPIO2_10    (42)
#define GPIO2_11    (43)
#define GPIO2_12    (44)
#define GPIO2_13    (45)
#define GPIO2_14    (46)
#define GPIO2_15    (47)
#define GPIO2_16    (48)
#define GPIO2_17    (49)
#define GPIO2_18    (50)
#define GPIO2_19    (51)
#define GPIO2_20    (52)
#define GPIO2_21    (53)
#define GPIO2_22    (54)
#define GPIO2_23    (55)
#define GPIO2_24    (56)
#define GPIO2_25    (57)
#define GPIO2_26    (58)
#define GPIO2_27    (59)
#define GPIO2_28    (60)
#define GPIO2_29    (61)
#define GPIO2_30    (62)
#define GPIO2_31    (63)

#define GPIO3_0     (64)
#define GPIO3_1     (65)
#define GPIO3_2     (66)
#define GPIO3_3     (67)
#define GPIO3_4     (68)
#define GPIO3_5     (69)
#define GPIO3_6     (70)
#define GPIO3_7     (71)
#define GPIO3_8     (72)
#define GPIO3_9     (73)
#define GPIO3_10    (74)   /* 此引脚不存在，仅作占位 */
#define GPIO3_11    (75)   /* 此引脚不存在，仅作占位 */
#define GPIO3_12    (76)   /* 此引脚不存在，仅作占位 */
#define GPIO3_13    (77)
#define GPIO3_14    (78)
#define GPIO3_15    (79)
#define GPIO3_16    (80)
#define GPIO3_17    (81)
#define GPIO3_18    (82)
#define GPIO3_19    (83)
#define GPIO3_20    (84)
#define GPIO3_21    (85)
#define GPIO3_22    (86)
#define GPIO3_23    (87)
#define GPIO3_24    (88)
#define GPIO3_25    (89)
#define GPIO3_26    (90)
#define GPIO3_27    (91)
#define GPIO3_28    (92)
#define GPIO3_29    (93)
#define GPIO3_30    (94)
#define GPIO3_31    (95)

#define GPIO5_0     (96)
#define GPIO5_1     (97)
#define GPIO5_2     (98)


/** @} */

/**
 * \name iMX RT1020 引脚编号最大值和引脚总数
 * @{
 */

#define IMX1020_GPIO_PIN_MAX       GPIO5_2
#define IMX1020_GPIO_PINS         ((GPIO5_2 - GPIO1_0) + 1)

/** @} */

/** @} grp_imx_rt1020_pins */

#ifdef __cplusplus
}
#endif

#endif /* __IMX1020_GPIO_PIN_H */

/* end of file */
