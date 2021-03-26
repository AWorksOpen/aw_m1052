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
 * \brief 硬件定时器操作标准接口
 *
 * 本模块提供硬件定时器服务。
 * 使用本服务需要包含头文件 aw_hwtimer.h
 *
 * \note 系统的硬件定时器资源有限， 若对定时的精度要求不是特别高，推荐使用
 * \ref grp_aw_if_timer 代替。
 *
 * \par 使用示例
 * \code
 *
 * static aw_hwtimer_handle_t  mytmr;  // 定时器句柄
 *
 * // 定时器中断回调
 * void my_timer_isr (void *p_arg)
 * {
 *     ; // 中断执行内容
 * }
 * // 申请一个输入频率100000Hz，最小频率1Hz，最大频率8000Hz能中断，
 * // 能自动重装计数值的定时器
 * mytmr = aw_hwtimer_alloc(100000,
 *                          1,
 *                          8000,
 *                          AW_HWTIMER_CAN_INTERRUPT | AW_HWTIMER_AUTO_RELOAD,
 *                          &my_timer_isr,
 *                          &count);
 * if (AW_HWTIMER_NULL != mytmr) {
 *     ; // 申请成功，做一些其它事情
 *
 *     // 当定时器不再使用时，释放该定时器
 *     aw_hwtimer_release(mytmr);
 * }
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 12-11-14  orz, first implementation
 * \endinternal
 */

#ifndef __AW_HWTIMER_H
#define __AW_HWTIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_hwtimer
 * \copydoc aw_hwtimer.h
 * @{
 */

#include "aw_common.h"

/**
 * \name 定时器特性
 * \brief 最低字节代表定时器位数，如定时器位数为32，则最低字节就为32.
          次最低字节代表了定时器的通道数目，如通道为4，次最低字节即为4
          其他部分各个位代表了一种定时器特性,因此，最大支持16种定时器特性
          AW_HWTIMER_FEATURE(0) ~ AW_HWTIMER_FEATURE(15)
 * @{
 */

/** \brief  最低字节代表定时器的位数 */
#define AW_HWTIMER_SIZE(size)              ((uint32_t)(size & 0x00fful))

/** \brief  根据特性得到定时器的位数 */
#define AW_HWTIMER_SIZE_GET(features)      (features & 0x00fful)

/** \brief  根据特性得到定时器的最大计数值 */
#define AW_HWTIMER_MAXCOUNT_GET(features)  ((uint32_t)((1ul << AW_HWTIMER_SIZE_GET(features)) - 1))

/** \brief  定时器的通道数目 */
#define AW_HWTIMER_CHAN_NUM(num)           (((uint32_t)num << 8) & 0xff00ul)

/** \brief  根据特性得到定时器的位数 */
#define AW_HWTIMER_CHAN_NUM_GET(features)  ((features & 0x00ff00ul) >> 8)


/** \brief  定时器的特性从第8位开始 */
#define  AW_HWTIMER_FEATURE(bit)           ((uint32_t)(1ul<< (bit + 16)))

/** \brief 定时器可产生中断 */
#define AW_HWTIMER_CAN_INTERRUPT             AW_HWTIMER_FEATURE(0)

/** \brief 定时器可立即读当前计数值 */
#define AW_HWTIMER_INTERMEDIATE_COUNT        AW_HWTIMER_FEATURE(1)

/** \brief 定时器不能被禁止 */
#define AW_HWTIMER_CANNOT_DISABLE            AW_HWTIMER_FEATURE(2)

/** \brief 当读取定时器当前计数值时定时器会停止计数 */
#define AW_HWTIMER_STOP_WHILE_READ           AW_HWTIMER_FEATURE(3)

/** \brief 定时器中断后能自动重装计数初值 */
#define AW_HWTIMER_AUTO_RELOAD               AW_HWTIMER_FEATURE(4)

/** \brief 定时器的计数器是自由运行的，不能修改它的计数翻转值 */
#define AW_HWTIMER_CANNOT_MODIFY_ROLLOVER    AW_HWTIMER_FEATURE(5)

/** \brief 定时器不能支持最小到最大频率之间的所有频率，只支持该区间的部分频率 */
#define AW_HWTIMER_CANNOT_SUPPORT_ALL_FREQS  AW_HWTIMER_FEATURE(6)

/** @} */

/**
 * \name  定时器通道是否按照名字（即指定按名字、单元号、通道号）分配
 * \brief 设置了某个通道是否按照名字分配
 *
 * @{
 */

/** \brief  指定了某个通道按照名字分配 */
#define AW_HWTIMER_CHAN_ALLOC_BY_NAME(ch)            ((uint32_t)(1ul << ch))

/** \brief 指定了该定时器按照名字分配，对于只有一个通道的定时器使用此设置 */
#define AW_HWTIMER_ALLOC_BY_NAME                     ((uint32_t)(0xFFFFFFFF))

/** \brief 指定了该定时器按照名字分配，对于只有一个通道的定时器使用此设置 */
#define AW_HWTIMER_NOT_ALLOC_BY_NAME                  ((uint32_t)(0))

/** @} */



/** \brief 空定时器句柄，用于判断定时器句柄是否有效 */
#define AW_HWTIMER_NULL     ((void *)0)

/** \brief 定时器句柄类型 */
typedef void *aw_hwtimer_handle_t;

/**
 * \brief 获取定时器的特性
 *
 * 这个接口用于获取定时器的最小频率、最大频率和特性标志。如果不需要某个获取某个
 * 特性值，则把该特性参数设为NULL
 *
 * \param[in] timer        要查询的定时器句柄
 * \param[in] p_min_freq   保存定时器最小频率的指针
 * \param[in] p_max_freq   保存定时器最大频率的指针
 * \param[in] p_features   保存定时器特性的指针
 *
 * \retval AW_OK       没有错误
 * \retval -AW_EINVAL  无效的定时器句柄
 */
extern aw_err_t aw_hwtimer_features_get (aw_hwtimer_handle_t timer,
                                         uint32_t    *p_min_freq,
                                         uint32_t    *p_max_freq,
                                         uint32_t    *p_features);

/**
 * \brief 从系统分配一个符合指定要求的定时器
 *
 * \param[in] freq      期望的定时器输入频率（频率越高精度越高）
 * \param[in] min_freq  期望至少能达到的最小频率
 * \param[in] max_freq  期望至少能达到的最大频率（硬件无倍频的情况下max_freq ≦ freq）
 * \param[in] features  定时器特性
 * \param[in] pfunc_isr 定时器中断服务函数
 * \param[in] p_arg     定时器中断服务函数参数
 *
 * \note 如果分配成功，则实际的定时器的输入频率不一定等于freq，
 *      最小频率 ≦ min_freq，最大频率 ≧ max_freq，
 *      实际参数可通过 aw_hwtimer_features_get() 获取
 *
 * \return 定时器句柄，AW_HWTIMER_NULL为无效的定时器句柄
 */
extern aw_hwtimer_handle_t aw_hwtimer_alloc (uint32_t  freq,
                                             uint32_t  min_freq,
                                             uint32_t  max_freq,
                                             uint32_t  features,
                                             void    (*pfunc_isr) (void *p_arg),
                                             void     *p_arg);
/**
 * \brief 从系统分配一个符合指定名字和设备单元号的定时器
 *
 * \param[in] pname     定时器的名字
 * \param[in] uint      定时器的设备单元号
 * \param[in] time_no   定时器号码
 * \param[in] pfunc_isr 定时器中断服务函数
 * \param[in] p_arg     定时器中断服务函数参数
 *
 * \return 定时器句柄，AW_HWTIMER_NULL为无效的定时器句柄
 */
extern aw_hwtimer_handle_t aw_hwtimer_alloc_byname (const char *pname,
                                                    uint8_t     uint,
                                                    uint8_t     time_no,
                                                    void      (*pfunc_isr) (void *p_arg),
                                                    void       *p_arg);

/**
 * \brief 释放定时器
 *
 * \param[in] timer 要释放的定时器句柄
 *
 * \return AW_OK 为成功，其它错误码为失败
 */
extern aw_err_t aw_hwtimer_release (aw_hwtimer_handle_t timer);

/**
 * \brief 使能定时器
 *
 * \param[in] timer        要使能的定时器句柄
 * \param[in] frequency_hz 定时中断频率
 *
 * \return AW_OK 为成功，其它错误码为失败
 */
extern aw_err_t aw_hwtimer_enable (aw_hwtimer_handle_t timer, uint32_t frequency_hz);



/**
 * \brief 禁能定时器
 *
 * \param[in] timer 要禁止的定时器句柄
 *
 * \return AW_OK 为成功，其它错误码为失败
 */
extern aw_err_t aw_hwtimer_disable (aw_hwtimer_handle_t timer);

/**
 * \brief 获取定时器当前计数值
 *
 * \param[in]     timer    定时器句柄
 * \param[in,out] p_count  保存定时器计数值的指针
 *
 * \return AW_OK 为成功，其它错误码为失败
 */
extern aw_err_t aw_hwtimer_count_get (aw_hwtimer_handle_t timer, uint32_t *p_count);



/**
 * \brief 获取定时器翻转值
 *
 * \param[in]     timer    定时器句柄
 * \param[in,out] p_count  保存定时器翻转值的指针
 *
 * \return AW_OK 为成功，其它错误码为失败
 */
extern aw_err_t aw_hwtimer_rollover_get (aw_hwtimer_handle_t timer, uint32_t *p_count);


/**
 * \brief 获取定时器翻转值
 *
 * \param[in]     timer       定时器句柄
 * \param[in,out] p_clk_freq  输出时钟频率
 *
 * \return AW_OK 为成功，其它错误码为失败
 */
extern aw_err_t aw_hwtimer_clk_freq_get (aw_hwtimer_handle_t timer, uint32_t *p_clk_freq);


/**
 * \brief 获取定时器翻转值
 *
 * \param[in]     timer    定时器句柄
 * \param[in,out] p_count  保存定时器翻转值的指针
 *
 * \return AW_OK 为成功，其它错误码为失败
 */
extern aw_err_t aw_hwtimer_rollover_get64 (aw_hwtimer_handle_t timer, uint64_t *p_count);

/**
 * \brief 使能一个64位定时器
 *
 * \param[in] timer        要使能的定时器句柄
 * \param[in] frequency_hz 定时中断频率
 *
 * \return AW_OK 为成功，其它错误码为失败
 */
extern aw_err_t aw_hwtimer_enable64 (aw_hwtimer_handle_t timer, uint64_t frequency_hz);

/**
 * \brief 获取定时器当前计数值
 *
 * \param[in]     timer    定时器句柄
 * \param[in,out] p_count  保存定时器计数值的指针
 *
 * \return AW_OK 为成功，其它错误码为失败
 */
extern aw_err_t aw_hwtimer_count_get64 (aw_hwtimer_handle_t timer, uint64_t *p_count);

/**
 * \brief 将定时器的定时时间(ms)转换为频率
 *
 * \param[in] ms 定时器句柄
 *
 * \return 根据时间转换得到的频率
 */
extern uint32_t aw_hwtimer_ms_to_freq (uint32_t ms);

/**
 * \brief 将定时器的定时时间(us)转换为频率
 *
 * \param[in] us 定时器句柄
 *
 * \return 根据时间转换得到的频率
 */
extern uint32_t aw_hwtimer_us_to_freq (uint32_t us);

/** @} grp_aw_if_hwtimer */

#ifdef __cplusplus
}
#endif

#endif /* __AW_HWTIMER_H */

/* end of file */
