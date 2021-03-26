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
 * \brief 系统相关服务
 *
 * 只要包含了 apollo.h 就可以使用本服务。
 *
 * \par 简单示例:计算程序执行时间
 * \code
 *  #include "apollo.h"
 *
 *  int main(){
 *      unsigned int tick;
 *
 *      tick = aw_sys_tick_get();
 *      //  do something
 *      tick = aw_sys_tick_get() - tick;
 *      aw_kprintf("tick used=%d\n , time=%d ms",
 *                  tick,
 *                  aw_ticks_to_ms(tick));
 *      return 0;
 *  }
 * \endcode
 *
 * //更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 13-02-27  zen, first implementation
 * \endinternal
 */

#ifndef __AW_SYSTEM_H
#define __AW_SYSTEM_H

#include "aw_types.h"
#include "aw_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_system
 * \copydoc aw_system.h
 * @{
 */

/**
 * \brief 取得系统时钟节拍频率
 *
 * \return 系统时钟节拍频率, 类型为 unsigned long
 */
unsigned long aw_sys_clkrate_get (void);

/**
 * \brief 取得系统当前的节拍计数值,
 *
 * 系统启动后，节拍计数值从0开始，每过1个tick增加1 。系统节拍的频率可调用
 * aw_sys_clkrate_get() 获取。
 *
 * \return 系统当前的节拍计数值, 类型为 unsigned int
 *
 * \par 示例:计算程序执行时间
 * \code
 *  #include "apollo.h"
 *
 *  int main(){
 *      unsigned int tick;
 *
 *      tick = aw_sys_tick_get();
 *      //  do something
 *      tick = aw_sys_tick_get() - tick;
 *      aw_kprintf("tick used=%d\n , time=%d ms",
 *                  tick,
 *                  aw_ticks_to_ms(tick));
 *      return 0;
 *  }
 * \endcode
 *
 * // 更多内容待添加。。。
 */
aw_tick_t aw_sys_tick_get (void);

/**
 * \brief  计算两个时间tick的差值
 *
 * \param  __t0 t0时的节拍个数
 * \param  __t1 t1时的节拍个数
 *
 * \return 系统当前时间戳值(时间戳定时器的计数值)
 *
 * \note 不需要考虑反转的问题: 负数的表示机制能自动避免反转问题，如0xfffffff即表
 *       示无符号的最大值(32bit)，也表示有符号数的-1.
 */
static aw_inline aw_tick_t aw_sys_tick_diff(aw_tick_t __t0, aw_tick_t __t1)
{
    return __t1 - __t0;
}

/**
 * \brief 转换毫秒时间为系统时钟节拍个数
 * \param ms 毫秒时间
 * \return 毫秒时间对应的节拍个数
 * \note 毫秒数不足一个节拍的将补齐一个节拍。如一个节拍为5毫秒，则1~5毫秒都将
 *       转换为1个节拍。
 * \par 示例
 * \code
 * #include "apollo.h"
 *
 * aw_task_delay(aw_ms_to_ticks(500));    // 延时500ms
 * \endcode
 */
aw_tick_t aw_ms_to_ticks (unsigned int ms);

/**
 * \brief   转换系统时钟节拍个数为毫秒时间
 * \param   ticks 节拍个数
 * \return  系统时钟节拍数对应的毫秒时间
 *
 * \par 示例:计算程序执行时间
 * \code
 *  #include "apollo.h"
 *
 *  int main(){
 *      unsigned int tick;
 *
 *      tick = aw_sys_tick_get();
 *      //  do something
 *      tick = aw_sys_tick_get() - tick;
 *      aw_kprintf("tick used=%d\n , time=%d ms",
 *                  tick,
 *                  aw_ticks_to_ms(tick));
 *      return 0;
 *  }
 * \endcode
 */
unsigned int aw_ticks_to_ms (aw_tick_t ticks);

 
/**
 * \brief 获取当前处理器核ID
 *
 * \return 处理器核ID
 */
uint32_t aw_core_id_self(void);

/**
 * \brief 获取处理器本地地址对应的全局地址 (物理地址, DMA等总线主机外设使用此地
 *        址)
 *
 * \return 全局地址
 */
void *aw_global_addr_get(void *p_addr);

/** @} grp_aw_if_system */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif    /* __AW_SYSTEM_H    */

/* end of file */
