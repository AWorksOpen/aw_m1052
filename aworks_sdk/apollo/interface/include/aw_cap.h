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
 * \brief CAP (capture)输入捕获标准服务
 *
 * 使用本服务需要包含以下头文件:
 * \code
 * #include "aw_cap.h"
 * \endcode
 *
 * \par 使用示例
 * \code
 * #include "aw_cap.h"
 *
 * void callback(uint32_t count)
 * {
 *    static unsigned int num = 0;
 *    static unsigned int count1;
 *    unsigned int period_ns;
 *
 *    if (num == 0) {
 *         count1 = count;        // 第一次捕获得到的值 
 *         num = 1;
 *    } else {
 *         // 得到两次上升沿之间的时间，即波形的周期  
 *         aw_cap_count_to_time(0,count1,count,&period_ns); 
 *         aw_kprintf("测得波形的周期为:%d ns\n",period_ns);
 *    }
 *    count1 = count;            //  保存值              
 * }
 * void aw_main()
 * {
 *     aw_cap_config(0,AW_CAP_TRIGGER_RISE,callback);
 *     aw_cap_enable(0);
 * }
 *  
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 14-06-05  tee, first implementation
 * \endinternal
 */

#ifndef __AW_CAP_H
#define __AW_CAP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_capture
 * \copydoc aw_cap.h
 * @{
 */

/** \brief 配置输入捕获由上升沿触发 */
#define AW_CAP_TRIGGER_RISE          0x01
    
/** \brief 下降沿触发 */
#define AW_CAP_TRIGGER_FALL          0x02

/** \brief 双边沿触发 */   
#define AW_CAP_TRIGGER_BOTH_EDGES    0x04


/**
 * \brief 设置CAP的回调函数
 *
 * \param[in] cap_id 输入捕获通道号
 * \param[in] pfunc_callback 捕获回调函数，当捕获事件发生时，将由驱动调用此函数，
 *            由此得到捕获值.回调函数第一个参数p_arg是用户参数，第二个参数count是
 *            捕获值。
 * \param[in] p_arg 用户参数，传递给回调函数的第一个参数
 * \param[in] flags 触发捕获的边沿类型设置
 *                   - AW_CAP_TRIGGER_RISE           上升沿触发捕获   
 *                   - AW_CAP_TRIGGER_FALL           下降沿触发捕获
 *                   - AW_CAP_TRIGGER_BOTH_EDGES     双边沿触发捕获
 *
 * \retval      AW_OK      成功
 * \retval     -AW_ENXIO   cap_id 为不存在的通道号
 * \retval     -AW_EINVAL  无效参数
 *
 */
aw_err_t aw_cap_config(int           cap_id, 
                       unsigned int  flags,
                       void (*pfunc_callback)(void *p_arg,unsigned int count),
                       void         *p_arg);

/**
 * \brief 使能CAP设备，使能后，定时器开始计时
 *
 * \param[in] cap_id 输入捕获通道号
 *
 * \retval      AW_OK      成功
 * \retval     -AW_ENXIO   cap_id 为不存在的通道号
 * \retval     -AW_EINVAL  无效参数
 */
aw_err_t aw_cap_enable(int cap_id);

/**
 * \brief 复位CAP设备，复位后，定时器count值复位为0
 *
 * \param[in] cap_id 输入捕获通道号
 *
 * \retval      AW_OK      成功
 * \retval     -AW_ENXIO   cap_id 为不存在的通道号
 * \retval     -AW_EINVAL  无效参数
 */
aw_err_t aw_cap_reset(int cap_id);

/**
 * \brief 禁用CAP设备
 *
 * \param[in] cap_id 输入捕获通道号
 *
 * \retval      AW_OK      成功
 * \retval     -AW_ENXIO   cap_id 为不存在的通道号
 * \retval     -AW_EINVAL  无效参数
 */
aw_err_t aw_cap_disable(int cap_id);

/**
 * \brief 将两次捕获的捕获值转换为对应的时间值
 *
 * \param[in]   cap_id      输入捕获通道号
 * \param[in]   count1      第一次捕获得到的值
 * \param[in]   count2      第二次捕获得到的值
 * \param[out]  p_time_ns   计算得到的时间值(单位:纳秒)
 *
 * \retval AW_OK     成功
 */
aw_err_t aw_cap_count_to_time(int           cap_id, 
                              unsigned int  count1,
                              unsigned int  count2,
                              unsigned int *p_time_ns);

/**
 * @}
 */
                              
#ifdef __cplusplus
}
#endif

#endif

/* end of file */
