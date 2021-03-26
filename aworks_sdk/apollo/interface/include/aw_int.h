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
 * \brief 中断标准接口
 *
 * 使用本服务需要包含头文件
 * \code
 * #include aw_int.h
 * \endcode
 *
 * 本服务提供了操作系统中断的接口。中断号由平台定义。
 * 其它服务可调用本模块来完成CPU中断的开关，
 * 中断服务函数的连接、断开、使能、禁能的操作。
 *
 * \par 简单示例
 * \code
 *  #include "apollo.h"
 *  #include "aw_int.h"
 *
 *  void my_isr(void *p_arg)
 *  {
 *      int my_arg = (int)p_arg; //my_arg = 5
 *      //中断处理。。。
 *  }
 *  aw_int_connect(INUM_ADC, my_isr, (void *)5); //连接ADC的中断服务函数
 *  aw_int_enable(INUM_ADC);    // 开启ADC中断
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 12-08-27  zen, first implementation
 * \endinternal
 */

#ifndef __AW_INT_H
#define __AW_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_int
 * \copydoc aw_int.h
 * @{
 */
#include "aw_types.h"
#include "aw_psp_int.h"

/**
 * \brief 声明CPU LOCK
 * 本宏定义一个变量用以保存 CPU 最后的中断状态
 * \sa AW_INT_CPU_LOCK_DECL_STATIC() AW_INT_CPU_LOCK() AW_INT_CPU_UNLOCK()
 */
#define AW_INT_CPU_LOCK_DECL(flags) \
    AW_PSP_INT_CPU_LOCK_DECL(flags)

/**
 * \brief 声明CPU LOCK (静态)
 * 本宏定义一个变量用以保存 CPU 最后的中断状态
 * \sa AW_INT_CPU_LOCK_DECL() AW_INT_CPU_LOCK() AW_INT_CPU_UNLOCK()
 */
#define AW_INT_CPU_LOCK_DECL_STATIC(flags) \
    AW_PSP_INT_CPU_LOCK_DECL_STATIC(flags)

/**
 * \brief 关闭本地CPU的中断
 *
 * 本函数可以关闭调用此函数的任务或ISR所在CPU的中断。
 * AW_INT_CPU_LOCK() 与 AW_INT_CPU_UNLOCK() 支持嵌套调用，里层的
 * AW_INT_CPU_UNLOCK() 不会立即开启CPU的中断，只有最外层的 AW_INT_CPU_UNLOCK()
 * 才会立即开启中断。
 *
 * \attention
 *  - CPU中断被关闭的时间应尽可能地短
 *  - 本函数应当总是与 AW_INT_CPU_UNLOCK() 配对调用
 *  - 应当避免使用开关CPU中断作为驱动的互斥手段，推荐使用
 *            \ref grp_aw_if_spinlock 代替之
 *  - 本函数无法关闭CPU的“不可屏蔽中断”
 *
 * \par 示例:常规用法
 * \code
 * int key;
 *
 * AW_INT_CPU_LOCK_DECL(key)
 *
 * AW_INT_CPU_LOCK(key);
 * // 不能被打断的临界代码(执行应尽可能地短)
 * AW_INT_CPU_UNLOCK(key);
 * \endcode
 *
 * \par 示例:嵌套
 * \code
 * AW_INT_CPU_LOCK_DECL(key0);
 * AW_INT_CPU_LOCK_DECL(key1);
 * AW_INT_CPU_LOCK_DECL(key2);
 *
 * AW_INT_CPU_LOCK(key0);   //CPU的中断被立即关闭
 * AW_INT_CPU_LOCK(key1);   //CPU的中断继续保持关闭
 * AW_INT_CPU_LOCK(key2);   //CPU的中断继续保持关闭
 *
 * AW_INT_CPU_UNLOCK(key2);    //CPU的中断继续保持关闭
 * AW_INT_CPU_UNLOCK(key1);    //CPU的中断继续保持关闭
 * AW_INT_CPU_UNLOCK(key0);    //CPU的中断被立即开启
 * \endcode
 *
 * \see AW_INT_CPU_UNLOCK()
 */
#define AW_INT_CPU_LOCK(key)    AW_PSP_INT_CPU_LOCK(key)

/**
 * \brief 打开本地CPU的中断
 *
 * 本函数可以打开调用此函数的任务或ISR所在CPU的中断。
 *
 * \attention 本函数应当总是与 AW_INT_CPU_LOCK() 配对调用
 *
 * \see AW_INT_CPU_LOCK()
 */
#define AW_INT_CPU_UNLOCK(key)  AW_PSP_INT_CPU_UNLOCK(key)

/**
 * \brief 判断是否在中断的上下文中执行
 *
 * \retval AW_FALSE 当前不在中断上下文执行
 * \retval 其他值   当前在中断上下文中执行
 */
#define AW_INT_CONTEXT()    AW_PSP_INT_CONTEXT()

/**
 * \brief 判断当前是否运行在CPU fault处理环境
 *
 * 典型的ARM fault环境为data abort，prefetch abort
 * \retval AW_FALSE     当前不在fault环境中执行
 * \retval 其他值       当前在fault环境中执行
 */
#define AW_FAULT_CONTEXT()    AW_PSP_FAULT_CONTEXT()

/**
 * \brief 连接一个C函数到一个硬件中断
 *
 * 本函数连接一个指定的C函数到一个指定的中断上。当中断号为 \a inum 的中断发生时，
 * 将会在特权模式下调用该C函数( \a pfunc_isr )，并传递参数 \a p_arg 。
 *
 * \par 实现相关
 * 平台可以支持连接多个C函数到同一个中断上，当中断发生时，将会按照连接的先后顺序
 * 调用这些C函数。平台也可以只支持连接一个C函数，连接已经被连接的中断将会返回
 * 空间不足错误(-ENOSPC)，必须先依次调用 aw_int_disable()、 aw_int_disconnect()
 * 禁止并断开先前已连接的C函数，才可以成功连接新的C函数。
 *
 * \attention   除非确定中断 \a inum 已经被禁止(中断 \a inum 默认是被禁止的)，
 *              否则, 在本函数之前应当先调用 aw_int_disable() 禁止对应的中断。
 *
 * \param[in]   inum        中断号，由平台定义
 * \param[in]   pfunc_isr   要调用的C函数(ISR, 中断服务函数)，可以为任何普通的
 *                          C代码，但是，它不能调用某些会阻塞的函数，
 *                          并且应当保证函数的执行尽可能地短
 * \param[in]   p_arg       要传递给C函数的参数
 *
 * \retval      AW_OK       成功
 * \retval     -AW_EINVAL   参数错误
 * \retval     -AW_ENOTSUP  不支持
 *
 * \par 示例：常规用法
 * \code
 *  void my_isr(void *p_arg)
 *  {
 *      int my_arg = (int)p_arg; //my_arg = 5
 *      //中断处理。。。
 *  }
 *  aw_int_connect(INUM_ADC, my_isr, (void *)5); //连接ADC的中断服务函数
 * \endcode
 *
 * \par 示例：更新中断服务函数(ISR)
 * 当实现只支持“1个中断连接1个ISR”时，按如下方式更新ISR:
 * \code
 *  aw_int_disable(INUM_ADC);                        // 禁止中断
 *  aw_int_disconnect(INUM_ADC, my_isr, (void *)5);  // 断开旧的ISR
 *  aw_int_connect(INUM_ADC, my_isr_new, (void *)6); // 连接新的ISR
 *  aw_int_enable(INUM_ADC);                         // 开启中断
 * \endcode
 *
 * \see aw_int_disconnect()
 */
aw_err_t aw_int_connect(int             inum,
                        aw_pfuncvoid_t  pfunc_isr,
                        void           *p_arg);

/**
 * \brief 断开一个C函数到一个硬件中断的连接
 *
 * 本函数从一个中断向量中断开一个参数为 \a p_arg 的C函数。
 *
 * \attention
 *  - 应当保证 \a pfunc_isr 和 \a p_arg 与先前传递给 aw_int_connect() 的参数相同。
 *
 * \param[in]   inum        中断号
 * \param[in]   pfunc_isr   要断开的C函数
 * \param[in]   p_arg       要断开的C函数的参数
 *
 * \retval      AW_OK       成功
 * \retval     -AW_EINVAL   参数错误
 * \retval     -AW_ENOTSUP  不支持
 * \retval     -AW_ENOENT   未找到参数为 \a p_arg 的 \a pfunc_isr
 *
 * \see aw_int_connect()
 */
aw_err_t aw_int_disconnect(int             inum,
                           aw_pfuncvoid_t  pfunc_isr,
                           void           *p_arg);

/**
 * \brief 使能指定的中断
 *
 * 本函数使能由\a inum 指定的中断
 *
 * \param[in]   inum        中断号
 *
 * \retval      AW_OK       成功
 * \retval     -AW_EINVAL   参数错误
 * \retval     -AW_ENOTSUP  不支持
 *
 * \see aw_int_disable()
 */
aw_err_t aw_int_enable(int inum);

/**
 * \brief 禁能指定的中断
 *
 * 本函数禁能由\a inum 指定的中断
 *
 * \param[in]   inum        中断号
 *
 * \retval      AW_OK       成功
 * \retval     -AW_EINVAL   参数错误,即错误的中断号
 * \retval     -AW_ENOTSUP  不支持
 *
 * \see aw_int_enable()
 */
aw_err_t aw_int_disable(int inum);

/**
 * \brief 定义连接到中断向量表的函数类型
 */
typedef void (*pfn_aw_int_vector_func_t)(void);


/**
 * \brief 连接一个C函数到中断向量表
 *
 * 为了提高中断处理效率，aw_int_vector_connect提供了一个直接连接到中断向量表
 * 的方法，CPU进入中断后，将直接在特权模式下执行指定的C函数
 * \par 实现相关
 * 此方法并不一定在所有的平台上实现，在一个平台上也不一定所有的中断都实现
 * 通常，这只在cortex-m3之类的平台上实现，CPU进入中断直接执行对应的ISR,中断参数
 * 需要自己维护，在特别需要效率的平台上实现。
 *
 * \attention   除非确定中断 \a inum 已经被禁止(中断 \a inum 默认是被禁止的)，
 *              否则, 在本函数之前应当先调用 aw_int_disable() 禁止对应的中断。
 *
 * \param[in]   inum            中断号，由平台定义
 * \param[in]   pfunc_vector    要调用的C函数(ISR, 中断服务函数)，可以为任何普通的
 *                          C代码，但是，它不能调用某些会阻塞的函数，
 *                          并且应当保证函数的执行尽可能地短
 * \retval      AW_OK       成功
 * \retval     -AW_EINVAL   参数错误
 * \retval     -AW_ENOTSUP  不支持
 *
 * \par 示例：常规用法
 * \code
 *  void my_isr(void)
 *  {
 *      //中断处理。。。
 *  }
 *  aw_int_vector_connect(INUM_ADC, my_isr);        //连接ADC的中断向量表函数
 * \endcode
 *
 * \par 示例：更新中断向量表函数(ISR)
 * 当实现只支持“1个中断连接1个ISR”时，按如下方式更新ISR:
 * \code
 *  aw_int_disable(INUM_ADC);                       // 禁止中断
 *  aw_int_vector_disconnect(INUM_ADC, my_isr);     // 断开旧的ISR
 *  aw_int_vector_connect(INUM_ADC, my_isr_new);    // 连接新的ISR
 *  aw_int_enable(INUM_ADC);                        // 开启中断
 * \endcode
 */
aw_err_t aw_int_vector_connect(int                      inum,
                               pfn_aw_int_vector_func_t pfunc_vector);

/**
 * \brief 断开一个C函数到一个硬件中断向量表的连接
 *
 * 本函数将中断向量表中对应中断的函数设置为系统默认函数
 *
 * \attention
 *  - 应当保证 \a pfunc_isr 与先前传递给 aw_int_vector_connect() 的参数相同。
 *
 * \param[in]   inum        中断号
 * \param[in]   pfunc_isr   要断开的C函数
 *
 * \retval      AW_OK       成功
 * \retval     -AW_EINVAL   参数错误
 * \retval     -AW_ENOTSUP  不支持
 * \see aw_int_vector_connect()
 */
aw_err_t aw_int_vector_disconnect(int                      inum,
                                  pfn_aw_int_vector_func_t pfunc_vector);



/**
 * \brief 获取一个中断当前的优先级
 * \param[in]   inum        中断号
 * \param[in]   p_cur_prio  用于输出此中断当前的优先级
 *
 * \retval      AW_OK       成功
 * \retval     -AW_EINVAL   参数错误
 */
aw_err_t aw_int_priority_get(int inum,int *p_cur_prio);

/**
 * \brief 获取一个中断的可设置优先级范围
 *
 * \param[in]   inum        中断号
 * \param[in]   p_min_prio  输出数值上最小的优先级
 * \param[in]   p_max_prio  输出数值上最大的优先级
 *
 * \retval      AW_OK       成功
 * \retval     -AW_EINVAL   参数错误
 * \retval     -AW_ENOTSUP  不支持
 * \attention 根据平台的不同，数值小的优先级可能实际更高，也可能实际更低
 */
aw_err_t aw_int_priority_range_get(int inum,int *p_min_prio,int *p_max_prio);

/**
 * \brief 设置一个中断优先级
 * \param[in]   inum        中断号
 * \param[in]   new_prio    新的中断优先级
 *
 * \retval      AW_OK       成功
 * \retval     -AW_EINVAL   参数错误
 * \retval     -AW_ENOTSUP  不支持
 */
aw_err_t aw_int_priority_set(int inum,int new_prio);

/**
 * \brief 手工触发一个中断
 *
 * 有时候，虽然硬件没有产生中断，但是我们可能需要其ISR执行
 * 手工触发中断将是一个比直接调用其ISR好很多的选择，手工触发中断后，如果当前
 * 没有禁止中断，且优先级满足要求，对应中断的ISR将立即执行
 *
 * \param[in]   inum        中断号
 *
 * \retval      AW_OK       成功
 * \retval     -AW_EINVAL   参数错误
 * \retval     -AW_ENOTSUP  不支持
 */
aw_err_t aw_int_pend(int inum);

/** @}  grp_aw_if_int */

#ifdef __cplusplus
}
#endif  /* __cplusplus 	*/

#endif  /* __AW_INT_H */

/* end of file */
