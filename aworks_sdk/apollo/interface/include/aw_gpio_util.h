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
 * \brief GPIO 标准接口工具头文件
 *
 * 本文件提供了与GPIO接口相关的辅助宏等工具软件
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
#define AW_GPIO_FUNCBITS_START          0   /**< \brief GPIO功能码起始位 */
#define AW_GPIO_FUNCBITS_LEN            3   /**< \brief GPIO功能码长度 */
#define AW_GPIO_FUNC_INVALID_VAL        0x0 /**< \brief 无效功能码值 */
#define AW_GPIO_INPUT_VAL               0x1 /**< \brief GPIO输入功能 */
#define AW_GPIO_OUTPUT_VAL              0x2 /**< \brief GPIO输出功能 */
#define AW_GPIO_OUTPUT_INIT_HIGH_VAL    0x3 /**< \brief GPIO初始输出高 */
#define AW_GPIO_OUTPUT_INIT_LOW_VAL     0x4 /**< \brief GPIO初始输出低 */

/** \brief 设置GPIO功能码 */
#define AW_GPIO_FUNCBITS_CODE(value) AW_SBF((value), AW_GPIO_FUNCBITS_START)

/** \brief 获取GPIO功能码值 */
#define AW_GPIO_FUNCBITS_GET(data)   AW_BITS_GET((data), \
                                                 AW_GPIO_FUNCBITS_START, \
                                                 AW_GPIO_FUNCBITS_LEN)

/** \brief 设置GPIO功能码为无效值 */
#define AW_GPIO_FUNC_INVALID    AW_GPIO_FUNCBITS_CODE(AW_GPIO_FUNC_INVALID_VAL)


/** \brief bit[5:3] mode code */

#define AW_GPIO_MODEBITS_START      3   /**< \brief GPIO模式起始位 */
#define AW_GPIO_MODEBITS_LEN        3   /**< \brief GPIO模式位长度 */
#define AW_GPIO_MODE_INVALID_VAL    0x0 /**< \brief GPIO模式无效值 */
#define AW_GPIO_PULL_UP_VAL         0x1 /**< \brief GPIO上拉模式 */
#define AW_GPIO_PULL_DOWN_VAL       0x2 /**< \brief GPIO下拉模式 */
#define AW_GPIO_FLOAT_VAL           0x3 /**< \brief GPIO浮空模式 */
#define AW_GPIO_OPEN_DRAIN_VAL      0x4 /**< \brief GPIO开漏模式 */
#define AW_GPIO_PUSH_PULL_VAL       0x5 /**< \brief GPIO推挽模式 */

/** \brief 设置GPIO模式 */
#define AW_GPIO_MODEBITS_CODE(value) AW_SBF((value), AW_GPIO_MODEBITS_START)

/** \brief 获取GPIO模式值 */
#define AW_GPIO_MODEBITS_GET(data)   AW_BITS_GET((data), \
                                                 AW_GPIO_MODEBITS_START, \
                                                 AW_GPIO_MODEBITS_LEN)

/** \brief 设置GPIO模式为无效值 */
#define AW_GPIO_MODE_INVALID    AW_GPIO_MODEBITS_CODE(AW_GPIO_MODE_INVALID_VAL)

/* bit[11:6] reserved */

/* bit[31:12] defined by platform */

#define AW_GPIO_PLFMBITS_START      12 /**< \brief GPIO平台相关起始位 */
#define AW_GPIO_PLFMBITS_LEN        20 /**< \brief GPIO平台相关位长度 */

/** \brief 设置GPIO平台功能码 */
#define AW_GPIO_PLFMBITS_SET(data, value) AW_BITS_SET((data), \
                                                      AW_GPIO_PLFMBITS_START, \
                                                      AW_GPIO_PLFMBITS_LEN, \
                                                      (value))

/** \brief 获取GPIO平台功能码 */
#define AW_GPIO_PLFMBITS_GET(data)  AW_BITS_GET((data), \
                                                AW_GPIO_PLFMBITS_START, \
                                                AW_GPIO_PLFMBITS_LEN)

#ifdef __cplusplus
}
#endif

#endif /* __AW_GPIO_UTIL_H */

/* end of file */
