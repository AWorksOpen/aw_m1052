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


extern int __heap_end__;                                              /**< /brief Ӳ��ƽ̨ջ�� */
extern int __heap_start__;                                            /**< /brief Ӳ��ƽ̨ջ�� */

#define _stack          __heap_start__

#define MICROPY_HW_MCU_NAME             "imx28x board"             /**< /brief ���������� */
//#define MP_MACHINE_NIC_NAME[]            {"eth0"}                     /**< /brief �������� */

#define MP_MACHINE_GPIO_NUM_MAX          130                          /**< /brief Ӳ��ƽ̨���� GPIO ����*/
#define MP_MACHINE_INTE_NUM_MAX          267                          /**< /brief Ӳ��ƽ̨���� INT ����*/
#define MP_MACHINE_UART_NUM_MAX          5                            /**< /brief Ӳ��ƽ̨����  UART ����*/
#define MP_MACHINE_SPI_NUM_MAX           4                            /**< /brief Ӳ��ƽ̨���� SPI ����*/
#define MP_MACHINE_I2C_NUM_MAX           2                            /**< /brief Ӳ��ƽ̨���� I2C ����*/
#define MP_MACHINE_LED_NUM_MAX           1                            /**< /brief Ӳ��ƽ̨���� LED ����*/

#endif  /* __MPCONFIGBOARD_H__ */

