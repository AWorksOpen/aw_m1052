/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief memory management
 *
 * \internal
 * \par modification history:
 * - 1.00 18-06-06  lqy, first implementation
 * \endinternal
 */

#ifndef __MPCONFIGBOARD_H__
#define __MPCONFIGBOARD_H__


#include "imx1060_inum.h"
#include "imx1060_pin.h"


extern int __heap_end__;                                              /**< \brief 硬件平台栈顶  */
extern int __heap_start__;                                            /**< \brief 硬件平台栈底  */

#define _stack          __heap_start__

#define MICROPY_HW_MCU_NAME             "imxrt106x board"             /**< \brief 开发板名称  */
#define MP_MACHINE_NIC_NAME[]            {"eth0"}                     /**< \brief 网卡名字 */

#define MP_MACHINE_GPIO_NUM_MAX          IMX1060_GPIO_PIN_MAX         /**< \brief 硬件平台最大的 GPIO 数量 */
#define MP_MACHINE_INTE_NUM_MAX          INUM_GPIO5_02                 /**< \brief 硬件平台最大的 INT 数量 */
#define MP_MACHINE_UART_NUM_MAX          7                            /**< \brief 硬件平台最大的  UART 数量 */
#define MP_MACHINE_SPI_NUM_MAX           3                            /**< \brief 硬件平台最大的 SPI 数量 */

#define MP_MACHINE_I2C_NUM_MAX           4                            /**< \brief 硬件平台最大的 I2C 数量 */
#define MP_HW_I2C_FREQ_DEFAULT           200000
#define MP_HW_I2C_TIMEOUT_DEFAULT        1000

#define MP_MACHINE_LED_NUM_MAX           1                            /**< \brief 硬件平台最大的 LED 数量 */

#endif/* __MPCONFIGBOARD_H__ */
