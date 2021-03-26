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
 * \brief iMX RT10xx Quadrature Decoder通用接口
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
 * - 1.00 18-03-21 mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_ENC_H
#define __AWBL_ENC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"


/**
 * \brief 初始化编码器
 *
 * \param[in] chan   通道
 * \param[in] p_func 中断回调函数
 * \param[in] p_arg  中断回调函数的参数
 *
 * \retval   AW_OK      初始化成功
 * \retval   AW_ERROR   初始化失败
 * \retval   -AW_EINVAL 参数错误
 */
aw_err_t  awbl_enc_init(uint32_t         chan,
                        aw_pfuncvoid_t   p_func,
                        void            *p_arg);



/**
 * \brief 获得编码器的位置
 *
 * \param[in] chan  通道
 * \param[in] p_val 数据读缓存
 *
 * \retval   AW_OK      读取成功
 * \retval   AW_ERROR   读取失败
 * \retval   -AW_EINVAL 参数错误
 */
aw_err_t  awbl_enc_get_position(uint32_t   chan,
                                int       *p_val);



/**
 * \brief 获得编码器的位置差值(本次位置和上次位置的差值)
 *
 * \param[in] chan  通道
 * \param[in] p_val 数据读缓存
 *
 * \retval   AW_OK      读取成功
 * \retval   AW_ERROR   读取失败
 * \retval   -AW_EINVAL 参数错误
 */
aw_err_t  awbl_enc_get_position_difference(uint32_t   chan,
                                           int16_t   *p_val);



/**
 * \brief 获得编码器的旋转圈数
 *
 * \param[in] chan  通道
 * \param[in] p_val 数据读缓存
 *
 * \retval   AW_OK      读取成功
 * \retval   AW_ERROR   读取失败
 * \retval   -AW_EINVAL 参数错误
 */
aw_err_t  awbl_enc_get_revolution(uint32_t   chan,
                                  int16_t   *p_val);



#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_ENC_H */

/* end of file */
