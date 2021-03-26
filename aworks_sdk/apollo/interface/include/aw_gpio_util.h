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
 * \brief GPIO ��׼�ӿڹ���ͷ�ļ�
 *
 * ���ļ��ṩ����GPIO�ӿ���صĸ�����ȹ������
 *
 * \internal
 * \par modification history:
 * - 1.00 12-08-23  zen, first implementation
 * \endinternal
 */

#ifndef __AW_GPIO_UTIL_H
#define __AW_GPIO_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/** \brief bit[2:0] function code */
#define AW_GPIO_FUNCBITS_START          0   /**< \brief GPIO��������ʼλ */
#define AW_GPIO_FUNCBITS_LEN            3   /**< \brief GPIO�����볤�� */
#define AW_GPIO_FUNC_INVALID_VAL        0x0 /**< \brief ��Ч������ֵ */
#define AW_GPIO_INPUT_VAL               0x1 /**< \brief GPIO���빦�� */
#define AW_GPIO_OUTPUT_VAL              0x2 /**< \brief GPIO������� */
#define AW_GPIO_OUTPUT_INIT_HIGH_VAL    0x3 /**< \brief GPIO��ʼ����� */
#define AW_GPIO_OUTPUT_INIT_LOW_VAL     0x4 /**< \brief GPIO��ʼ����� */

/** \brief ����GPIO������ */
#define AW_GPIO_FUNCBITS_CODE(value) AW_SBF((value), AW_GPIO_FUNCBITS_START)

/** \brief ��ȡGPIO������ֵ */
#define AW_GPIO_FUNCBITS_GET(data)   AW_BITS_GET((data), \
                                                 AW_GPIO_FUNCBITS_START, \
                                                 AW_GPIO_FUNCBITS_LEN)

/** \brief ����GPIO������Ϊ��Чֵ */
#define AW_GPIO_FUNC_INVALID    AW_GPIO_FUNCBITS_CODE(AW_GPIO_FUNC_INVALID_VAL)


/** \brief bit[5:3] mode code */

#define AW_GPIO_MODEBITS_START      3   /**< \brief GPIOģʽ��ʼλ */
#define AW_GPIO_MODEBITS_LEN        3   /**< \brief GPIOģʽλ���� */
#define AW_GPIO_MODE_INVALID_VAL    0x0 /**< \brief GPIOģʽ��Чֵ */
#define AW_GPIO_PULL_UP_VAL         0x1 /**< \brief GPIO����ģʽ */
#define AW_GPIO_PULL_DOWN_VAL       0x2 /**< \brief GPIO����ģʽ */
#define AW_GPIO_FLOAT_VAL           0x3 /**< \brief GPIO����ģʽ */
#define AW_GPIO_OPEN_DRAIN_VAL      0x4 /**< \brief GPIO��©ģʽ */
#define AW_GPIO_PUSH_PULL_VAL       0x5 /**< \brief GPIO����ģʽ */

/** \brief ����GPIOģʽ */
#define AW_GPIO_MODEBITS_CODE(value) AW_SBF((value), AW_GPIO_MODEBITS_START)

/** \brief ��ȡGPIOģʽֵ */
#define AW_GPIO_MODEBITS_GET(data)   AW_BITS_GET((data), \
                                                 AW_GPIO_MODEBITS_START, \
                                                 AW_GPIO_MODEBITS_LEN)

/** \brief ����GPIOģʽΪ��Чֵ */
#define AW_GPIO_MODE_INVALID    AW_GPIO_MODEBITS_CODE(AW_GPIO_MODE_INVALID_VAL)

/* bit[11:6] reserved */

/* bit[31:12] defined by platform */

#define AW_GPIO_PLFMBITS_START      12 /**< \brief GPIOƽ̨�����ʼλ */
#define AW_GPIO_PLFMBITS_LEN        20 /**< \brief GPIOƽ̨���λ���� */

/** \brief ����GPIOƽ̨������ */
#define AW_GPIO_PLFMBITS_SET(data, value) AW_BITS_SET((data), \
                                                      AW_GPIO_PLFMBITS_START, \
                                                      AW_GPIO_PLFMBITS_LEN, \
                                                      (value))

/** \brief ��ȡGPIOƽ̨������ */
#define AW_GPIO_PLFMBITS_GET(data)  AW_BITS_GET((data), \
                                                AW_GPIO_PLFMBITS_START, \
                                                AW_GPIO_PLFMBITS_LEN)

#ifdef __cplusplus
}
#endif

#endif /* __AW_GPIO_UTIL_H */

/* end of file */
