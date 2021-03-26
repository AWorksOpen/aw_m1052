/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief �ⲿʹ�õ������ļ�
 *
 * �û��ڴ��ļ��ж�ģ�����ز�����������
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-24  zen, first implementation
 * \endinternal
 */

#ifndef __AWBUS_LITE_USRCFG_H
#define __AWBUS_LITE_USRCFG_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/**
 * \addtogroup grp_awbl_usrcfg
 * @{
 */

/**
 * \name AWBus ģ������ (lite)
 * @{
 */

/**
 * \brief  AWBus lite ֧�ֵ����������
 *
 * Ϊ�˱��ⲻ��Ҫ���ڴ����ģ��û����Խ��˲�������Ϊϵͳ��ʵ�ʻ�ʹ�õ�����������
 * ���磬���ֻ�õ����жϿ�������GPIO��������������Խ��˲�������Ϊ2��
 *
 * \attention ��Чֵ��ΧΪ 1 ~ 128
 */
#define AWBUS_LITE_DRIVER_MAX   15

/**
 * \brief AWBus lite ֧�ֵ��������������
 *
 * Ϊ�˱��ⲻ��Ҫ���ڴ����ģ��û����Խ��˲�������Ϊϵͳ��ʵ�ʻ�ʹ�õ�������������
 * ���磬���ֻ�õ���CPU�������� (����ʹ��)��I2C�����������ͣ�����Խ��˲�������Ϊ2��
 *
 * \attention ��Чֵ��ΧΪ 1 ~ 128
 */
#define AWBUS_LITE_BUSTYPE_MAX  3

/** @} */

/** @} grp_lpc11xx_gpio_usrcfg */

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AWBUS_LITE_USRCFG_H */

/* end of file */

