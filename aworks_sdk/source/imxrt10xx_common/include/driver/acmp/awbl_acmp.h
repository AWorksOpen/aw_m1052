/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2014 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx Analog Comparator 标准接口
 *
 * 使用本服务需要包含头文件
 * \code
 * #include awbl_enc.h
 * \endcode
 *
 * \par 简单示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 18-04-12 mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_ACMP_H
#define __AWBL_ACMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"


/**< \brief  VREF输入通道1 */
#define VREF_SOURCE_VIN1   1

/**< \brief  VREF输入通道2 */
#define VREF_SOURCE_VIN2   2

/**
 * \brief ACMP中断状态标志
 */
typedef enum {
    kCMP_OutputRisingEventFlag = 0,   /**< \brief Rising-edge on the comparison output has occurred. */
    kCMP_OutputFallingEventFlag = 1,  /**< \brief Falling-edge on the comparison output has occurred. */
} cmp_status_flags_t;


/**
 * \brief 初始化ACMP,并注册中断服务函数
 *
 * \param[in] acmp_id  ACMP模块ID (1 - 4)
 * \param[in] p_func   中断回调函数
 *
 * \retval   AW_OK      初始化成功
 * \retval   AW_ERROR   初始化失败
 * \retval   -AW_EINVAL 参数错误
 */
aw_err_t  awbl_acmp_init(uint8_t          acmp_id,
                         aw_pfuncvoid_t   p_func);

/**
 * \brief 配置DAC，设置参考电压
 *
 * \param[in] acmp_id  ACMP模块ID (1 - 4)
 * \param[in] vin      参考电压的输入通道
 * \param[in] vin_mv   参考电压的输入值(mv)
 * \param[in] vref_mv  参考电压的输出值(范围：vin_mv/64 ~ vin_mv)，
 *                     该参考电压可接入比较器
 *
 * \retval   AW_OK      成功
 * \retval   AW_ERROR   失败
 * \retval   -AW_EINVAL 参数错误
 */
aw_err_t awbl_acmp_config_dac (uint8_t     acmp_id,
                               uint8_t     vin,
                               uint16_t    vin_mv,
                               uint16_t    vref_mv);

/**
 * \brief 比较器的输入通道选择
 *
 * \param[in] acmp_id         ACMP模块ID (1 - 4)
 * \param[in] positive_chan   同相输入端输入通道
 * \param[in] negative_chan   反相输入端输入通道
 *
 * \retval   AW_OK      成功
 * \retval   AW_ERROR   失败
 * \retval   -AW_EINVAL 参数错误
 */
aw_err_t awbl_acmp_set_input_chan(uint8_t   acmp_id,
                                  uint32_t  positive_chan,
                                  uint16_t  negative_chan);

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_ACMP_H */

/* end of file */
