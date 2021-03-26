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
 * \brief AWBus 中断控制器接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_intcltr.h"
 * \endcode
 *
 * 本模块管理 AWBus 上的所有中断控制器。
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-06  zen, first implemetation
 * \endinternal
 */

#ifndef __AWBL_INTCTLR_H
#define __AWBL_INTCTLR_H

#include "aw_int.h"

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/**
 * \brief AWBus intctlr 服务的配置信息
 */
struct awbl_intctlr_servinfo {

    /** \brief 支持的起始中断号 */
    int inum_start;

    /** \brief 支持的结束中断号 */
    int inum_end;
};

/** \brief 中断连接服务函数类型 */
typedef aw_err_t (*pfunc_int_connect_t)(
        void *p_cookie,
        int inum,
        aw_pfuncvoid_t pfunc_isr,
        void *p_arg);

/** \brief 中断断开连接服务函数类型 */
typedef aw_err_t (*pfunc_int_disconnect_t)(
        void *p_cookie,
        int inum,
        aw_pfuncvoid_t pfunc_isr,
        void *p_arg);

/** \brief 中断使能服务函数类型 */
typedef aw_err_t (*pfunc_int_enable_t)(
        void *p_cookie,
        int inum);

/** \brief 中断使能服务函数类型定义 */
typedef aw_err_t (*pfunc_int_disable_t)(
        void *p_cookie,
        int inum);

/** \brief 中断向量连接服务函数类型 */
typedef aw_err_t (*pfunc_int_vector_connect_t)(
        void *p_cookie,
        int inum,
        pfn_aw_int_vector_func_t pfunc_vector_isr);

typedef aw_err_t (*pfunc_int_vector_disconnect_t)(
        void *p_cookie,
        int inum,
        pfn_aw_int_vector_func_t pfunc_vector_isr);



typedef aw_err_t (*pfunc_int_priority_get_t)(
        void *p_cookie,
        int inum,
        int *p_cur_prio);

typedef aw_err_t (*pfunc_int_priority_range_get_t)(
        void *p_cookie,
        int inum,
        int *p_min_prio,int *p_max_prio);

typedef aw_err_t (*pfunc_int_priority_set_t)(
        void *p_cookie,
        int inum,
        int new_prio);

typedef aw_err_t (*pfunc_int_pend_t)(
        void *p_cookie,
        int inum);

/**
 * \brief AWBus intctlr 服务函数
 */
struct awbl_intctlr_servfuncs {
    pfunc_int_connect_t             pfunc_int_connect;
    pfunc_int_disconnect_t          pfunc_int_disconnect;
    pfunc_int_enable_t              pfunc_int_enable;
    pfunc_int_disable_t             pfunc_int_disable;

    pfunc_int_vector_connect_t      pfunc_int_vector_connect;
    pfunc_int_vector_disconnect_t   pfunc_int_vector_disconnect;
    pfunc_int_priority_get_t        pfunc_int_prio_get;
    pfunc_int_priority_range_get_t  pfunc_int_prio_range_get;
    pfunc_int_priority_set_t        pfunc_int_prio_set;
    pfunc_int_pend_t                pfunc_int_pend;
};




/**
 * \brief AWBus 中断控制器服务
 * 此服务结构在注册到中断控制器中间层后，驱动除非通过
 * 中间层函数来修改此结构体，否则不能修改此结构体
 */
struct awbl_intctlr_service {

    /** \brief 指向下一个 intctlr 服务 */
    struct awbl_intctlr_service *p_next;

    /** \brief intctlr 服务的配置信息 */
    const struct awbl_intctlr_servinfo *p_servinfo;

    /** \brief intctlr 服务的服务函数 */
    const struct awbl_intctlr_servfuncs *p_servfuncs;

    /**
     * \brief intctlr 服务自定义的数据
     *
     * 此参数在 注册中断控制器前由驱动设置，
     * 在调用服务函数时作为第一个参数。
     */
    void *p_cookie;
     /**
     * \brief 表征此变量现在由中间层使用，
     * 此变量的主要作用是用于在设备被移除时，处理驱动和中间层的冲突
     * 可能存在这样一种典型的情况，即应用正在通过中间层调用设备的功能，
     * 刚好执行到查找到本控制器，即将调用设备函数的时候，突然更高优先级
     * 的任务要求移除设备，如果这时候移除设备，则下次调度到中间层调用设
     * 备函数的时候会发生意料不到的问题

     * 驱动在移除控制器之前，应该调用awbl_unregister_interrupt_controller，
     * 函数内部发现ctlr_used_by_awbl不为0，则放弃CPU一段时间，再次检查，
     * 直到ctlr_used_by_awbl为0，然后反注册中断控制器、
     *
     * 设备移除函数在成功调用awbl_unregister_interrupt_controller之前，
     * 不能获得设备锁否则极易引发死锁，因为中间层调用设备函数也会获得设备锁
     */
    int             ctlr_used_by_awbl;
};

/**
 * \brief AWBus 中断控制器服务初始化
 *
 * \attention 本函数应当在 awbl_dev_init1() 之后、在 awbl_dev_init2() 之前调用
 */
void awbl_intctlr_init(void);

/**
 * \brief 向中断子系统注册一个终端控制器
 *
 * \param[in]   p_intctlr   要注册的中断控制器
 *
 * \retval      AW_OK       成功
 * \retval      其他值      失败，是负的AW错误码
 */
aw_err_t awbl_register_interrupt_controller(
    struct awbl_intctlr_service *p_intctlr);

/**
 * \brief 从中断子系统反注册一个终端控制器
 *
 * \param[in]   p_intctlr   要反注册的中断控制器
 *
 * \retval      AW_OK       成功
 * \retval      其他值      失败，是负的AW错误码
 */
aw_err_t awbl_unregister_interrupt_controller(
    struct awbl_intctlr_service *p_intctlr);

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AWBL_INTCTLR_H */

/* end of file */
