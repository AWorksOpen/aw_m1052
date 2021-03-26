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
 * \brief  软件定时器库
 *
 * 使用本服务需要包含头文件 aw_sftimer_lib.h
 *
 * 使用软件定时器库，需要外部提供周期性的服务例程，来调用 aw_sftimer_group_tick()
 * 来充当软件定时器库的“时钟”。
 * 
 * 软定时器组使用“桶”来存放定时器，当定时器组需要处理大量的定时器时，
 * 建议增大“桶”的大小，用于降低在某次周期服务例程执行大量工作的可能性。
 *
 * 假设使用硬件定时器提供软件定时器库的“时钟”：
 *
 * \par 初始化软定时器组
 * \code
 * #define  BUCKET_SIZE 6      // 桶的个数
 *
 * // 软定时器组的“时钟”频率，在这里等于硬件定时器中断服务例程的频率
 * #define  CLKRATE     1000   
 *
 * static struct aw_sftimer_group  my_sftimer_group;
 * static struct aw_sftimer_bucket my_bucket[BUCKET_SIZE];
 * 
 * static void sftimer_group_init(void)
 * {
 *     aw_sftimer_group_init(&my_sftimer_group,
 *                           &my_bucket[0],
 *                            BUCKET_SIZE,
 *                            CLKRATE);
 * }
 * \endcode 
 *
 * \par 设置硬件定时器中断服务例程
 * \code
 * static aw_hwtimer_handle_t my_hwtimer;
 * 
 * static void hwtimer_isr (void *p_arg)
 * {
 *     aw_sftimer_group_tick(&my_sftimer_group);
 * }
 *
 * void hwtimer_init(void)
 * {
 *     mytimer = aw_hwtimer_alloc(10000,                   
 *                                1,                      
 *                                1000000,                
 *                                AW_HWTIMER_AUTO_RELOAD, 
 *                                hwtimer_isr,            
 *                               (void *)0);          
 *
 *
 *     // 硬件定时器中断服务例程的执行频率
 *     aw_hwtimer_enable(my_hwtimer, CLKRATE);
 * }
 * \endcode
 *
 * \par 操作软件定时器
 * \code
 * #define SFTIMER_MS  1000  // 软定时器的定时时间，毫秒
 *
 * static struct aw_sftimer  my_sftimer;
 *
 * static void sftimer_handler(void *arg)
 * {
 *     // 执行定时操作
 *
 *     // 重启定时器
 *     aw_sftimer_restart(&my_sftimer);
 * }
 * 
 * static void sftimer_init(void)
 * {
 *     // 初始化定时器
 *     aw_sftimer_init(&my_sftimer,
 *                     &my_sftimer_group,
 *                      sftimer_handler，
 *                     (void *)0);
 * 
 *     // 启动定时器
 *     aw_sftimer_start_ms(&my_sftimer, 
 *                          SFTIMER_MS);
 * }
 * \endcode 
 *
 * aw_sftimer_start() 使用的滴答数，跟初始化软定时器组时传入的 \e clkrate 有关，
 * 所以不能调用 aw_ms_to_ticks() 来获取滴答数，要自行转换。
 * 推荐使用 aw_sftimer_start_ms() ，内部会自行将时间转换成滴答数。
 *
 * \internal
 * \par modification history:
 * - 1.10 13-03-21  zen, refine the description
 * - 1.01 13-02-22  orz, fix the defines and code style.
 * - 1.00 12-10-23  liangyaozhan, first implementation.
 * \endinternal
 */

#ifndef __AW_SFTIMER_LIB_H
#define __AW_SFTIMER_LIB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_list.h"

/**
 * \addtogroup grp_aw_if_sftimer_lib 
 * \copydoc aw_sftimer_lib.h
 * @{
 */

/** \brief 软定时器桶的定义 */
struct aw_sftimer_bucket {

    /** \brief 提供软件定时器链表入口 */
    struct aw_list_head list_head;
};

/** \brief 软定时器组的定义 */
struct aw_sftimer_group {

    /** \brief 软定时器组的桶 */
    struct aw_sftimer_bucket *p_buckets;

    /** \brief 桶个数，用于散列软定时器，和充当定时因子 */
    size_t                    bucket_cnt;

    /** \brief 当前的桶 */
    unsigned int              cur_bucket;

    /** \brief “时钟”频率，即外部提供定时服务的频率 */
    unsigned int              clkrate;
};


/**
 * \brief 软定时器的定义
 *
 * 用户不应该直接访问这些成员。
 */
struct aw_sftimer {

    /** \brief 链表节点 */
    struct aw_list_head listnode; 

    /** \brief 桶索引 */  
    uint32_t            bucket;     

    /** \brief 剩余计数，用于软定时器组定时 */
    uint32_t            round;      

    /** \brief 到期时间，用于重启软定时器 */
    uint32_t            expires;    

    /** 
     * \brief 软定时器执行的函数 
     * 
     * \param[in] arg 用户参数
     */
    void (*func)(void *arg);        

    /** \brief 软定时器执行函数的参数  */
    void  *arg;                    

    /** \brief 所属定时器组 */
    struct aw_sftimer_group *p_timer_grp;
};

/** 
 * \brief 软定时器组周期处理例程
 *
 * 一般放在外部提供的周期服务里执行。
 * 执行的频率等同于软定时器组的时钟频率。
 * 
 * \param[in] p_timer_grp      需要处理的软定时器组
 *
 * \sa aw_sftimer_group_init()
 */
void aw_sftimer_group_tick (struct aw_sftimer_group *p_timer_grp);


/** 
 * \brief 初始化软定时器组
 *
 * \e clkrate 是 aw_sftimer_group_tick() 所执行的频率。
 * 
 * \param[in] p_timer_grp      软定时器组
 * \param[in] p_buckets        桶的起始地址
 * \param[in] bucket_cnt       桶的个数
 * \param[in] clkrate          软定时器组的定时频率
 *
 * \retval AW_OK              初始化成功
 * \retval -AW_EINVAL         参数无效
 */
aw_err_t aw_sftimer_group_init (struct aw_sftimer_group  *p_timer_grp,
                                struct aw_sftimer_bucket *p_buckets,
                                size_t                    bucket_cnt,
                                unsigned int              clkrate);

/** 
 * \brief 初始化软定时器  
 * 
 * \param[in] p_timer      软定时器
 * \param[in] p_timer_grp  软定时器组
 * \param[in] func         定时执行的函数
 * \param[in] arg          定时执行的函数参数
 *
 * \retval AW_OK          初始化成功
 * \retval -AW_EINVAL     参数无效
 */
aw_err_t aw_sftimer_init (struct aw_sftimer *p_timer,
                          struct aw_sftimer_group *p_timer_grp,
                          void     (*func) (void *arg),
                          void      *arg);

/** 
 * \brief 开启软定时器 
 * 
 * \param[in] p_timer 软定时器
 * \param[in] ticks   定时时间(滴答数)
 *
 * \retval AW_OK       操作成功
 * \retval -AW_EINVAL  参数无效
 */
aw_err_t aw_sftimer_start (struct aw_sftimer *p_timer,
                           unsigned int       ticks);


/** 
 * \brief 定时执行的函数 
 * 
 * \param[in] p_timer   软定时器
 * \param[in] ms        定时时间(毫秒)
 *
 * \retval AW_OK       操作成功
 * \retval -AW_EINVAL  参数无效
 */
aw_err_t aw_sftimer_start_ms (struct aw_sftimer *p_timer,
                              unsigned int       ms);


/** 
 * \brief 重启软定时器
 * 
 * \param[in] p_timer   软定时器
 *
 * \retval AW_OK       操作成功
 * \retval -AW_EINVAL  参数无效
 */
aw_err_t aw_sftimer_restart (struct aw_sftimer *p_timer);


/** 
 * \brief 停止软定时器
 * 
 * \param[in] p_timer   软定时器
 *
 * \retval AW_OK       操作成功
 * \retval -AW_EINVAL  参数无效
 */
aw_err_t aw_sftimer_stop (struct aw_sftimer *p_timer);


/** @} grp_aw_if_sftimer_lib */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_SFTIMER_LIB_H */

/* end of file */
