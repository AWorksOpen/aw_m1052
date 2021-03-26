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


extern int __heap_end__;                                              /**< /brief 硬件平台栈顶 */
extern int __heap_start__;                                            /**< /brief 硬件平台栈底 */

#define _stack          __heap_start__

#define MICROPY_HW_MCU_NAME             "imx28x board"             /**< /brief 开发板名称 */
//#define MP_MACHINE_NIC_NAME[]            {"eth0"}                     /**< /brief 网卡名字 */

#define MP_MACHINE_GPIO_NUM_MAX          130                          /**< /brief 硬件平台最大的 GPIO 数量*/
#define MP_MACHINE_INTE_NUM_MAX          267                          /**< /brief 硬件平台最大的 INT 数量*/
#define MP_MACHINE_UART_NUM_MAX          5                            /**< /brief 硬件平台最大的  UART 数量*/
#define MP_MACHINE_SPI_NUM_MAX           4                            /**< /brief 硬件平台最大的 SPI 数量*/
#define MP_MACHINE_I2C_NUM_MAX           2                            /**< /brief 硬件平台最大的 I2C 数量*/
#define MP_MACHINE_LED_NUM_MAX           1                            /**< /brief 硬件平台最大的 LED 数量*/

#endif  /* __MPCONFIGBOARD_H__ */

