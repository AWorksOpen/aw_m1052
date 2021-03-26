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
 * \brief 热插拔组件接口
 *
 * 使用本模块需要包含头文件 aw_hotplug_event.h
 *
 * \par 简单示例
 * \code
 *
 * #include "aw_hotplug_event.h"
 *
 * aw_hotplug_event_t event;
 *
 * static int ref_callback (int stat, void *p_arg)
 * {
 *     if (AW_HOTPLUG_EVT_INSTALL == stat) {
 *         aw_kprintf("refcnt test:sdcard install!\n");
 *     } else if (AW_HOTPLUG_EVT_INSTALL == stat){
 *         aw_kprintf("refcnt test:sdcard remove!\n");
 *     } else {
 *         aw_kprintf("refcnt test:err\n");
 *     }
 *
 *     return AW_OK;
 * }
 *
 * void test_hotplug (void)
 * {
 *     int stat = 0;
 *
 *     aw_hotplug_event_register(&event,
 *                               "/dev/sd0",
 *                               AW_HOTPLUG_EVT_INSTALL |
 *                               AW_HOTPLUG_EVT_REMOVE,
 *                               ref_callback,
 *                               NULL);
 *
 *     if (AW_HOTPLUG_EVT_INSTALL != aw_hotplug_stat_get(&event, &stat)) {
 *         stat = AW_HOTPLUG_EVT_INSTALL;
 *         aw_hotplug_event_wait(&event, stat, 10000);
 *     }

 *     while (1) {
 *         aw_mdelay(100);
 *     }
 * }
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-08-08 bob, first implementation.
 * \endinternal
 */

#ifndef __AW_HOTPLUG_EVT_H
#define __AW_HOTPLUG_EVT_H

/**
 * \addtogroup grp_aw_if_hotplug_event
 * \copydoc aw_hotplug_event.h
 * @{
 */

#include "aw_types.h"
#include "aw_common.h"
#include "aw_list.h"
#include "aw_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief 设备插入事件（状态） */
#define AW_HOTPLUG_EVT_INSTALL    0x01

/** \brief 设备移除事件（状态） */
#define AW_HOTPLUG_EVT_REMOVE     0x02

/** \brief 所有事件（状态） */
#define AW_HOTPLUG_EVT_ALL        0xFFFFFFFF

/**
 * \brief 热插拔事件结构体
 */
typedef struct aw_hotplug_event {

    /** \brief 事件节点 */
    struct aw_list_head node;

    /** \brief 事件监听的设备名 */
    const char         *p_devname;

    /** \brief 事件标志 */
    int                 event_flgs;

    /** \brief 事件回调函数 */
    int               (*pfn_callback)(int event_flag, void *p_arg);

    /** \brief 回调函数参数 */
    void               *p_arg;
} aw_hotplug_event_t;


/**
 * \brief 热插拔设备结构体
 */
typedef struct aw_hotplug_dev {

    /** \brief 设备节点 */
    struct aw_list_head     node;

    /** \brief 设备名 */
    const char             *p_devname;

    /** \brief 热插拔设备状态 */
    int                     stat;
} aw_hotplug_dev_t;


/**
 * \brief 注册热插拔事件
 *
 * \param[in]  p_event        : 指向热插拔事件
 * \param[in]  p_name         : 热插拔事件监听的设别名
 * \param[in]  event_flgs     : 事件标志
 * \param[in]  pfn_callback   : 事件回调函数
 * \param[in]  p_arg          : 回调函数参数
 *
 * \retval AW_OK              : 初始化成功
 * \retval -AW_EINVAL         : 参数无效
 */
aw_err_t aw_hotplug_event_register (aw_hotplug_event_t *p_event,
                                    const char         *p_name,
                                    int                 event_flgs,
                                    int               (*pfn_callback)(int   flag,
                                                                      void *p_arg),
                                    void               *p_arg);

/**
 * \brief 取消注册热插拔事件
 *
 * \param[in]  p_event        : 指向热插拔事件
 *
 * \return 无
 */
void aw_hotplug_event_unregister (aw_hotplug_event_t *p_event);

/**
 * \brief 热插拔事件添加事件监听
 *
 * \param[in]      p_event        : 指向热插拔事件
 * \param[in,out]  p_event_flgs   : 添加需要监听的事件
 *
 * \retval AW_OK              : 初始化成功
 * \retval -AW_EINVAL         : 参数无效
 *
 * \note p_event_flgs指针返回添加事件后监听的事件集
 */
aw_err_t aw_hotplug_event_add (aw_hotplug_event_t *p_event,
                               int                *p_event_flgs);

/**
 * \brief 热插拔事件删除事件监听
 *
 * \param[in]      p_event        : 指向热插拔事件
 * \param[in,out]  p_event_flgs   : 删除需要监听的事件
 *
 * \retval AW_OK              : 初始化成功
 * \retval -AW_EINVAL         : 参数无效
 *
 * \note p_event_flgs指针返回删除事件后监听的事件集
 */
aw_err_t aw_hotplug_event_del (aw_hotplug_event_t *p_event,
                               int                *p_event_flgs);

/**
 * \brief 等待某个事件（状态）发生
 *
 * \param[in]  p_event        : 指向热插拔事件
 * \param[in]  stat           : 事件（状态）
 * \param[in]  time_out       : 超时
 *
 * \retval AW_OK              : 初始化成功
 * \retval -AW_EINVAL         : 参数无效
 * \retval -AW_ENOMEM         : 内存空间不足
 * \retval -AW_ETIME          : 超时
 */
aw_err_t aw_hotplug_event_wait (aw_hotplug_event_t *p_event,
                                int                 stat,
                                int                 time_out);

/**
 * \brief 获取热插拔设备当前状态
 *
 * \param[in]  p_event        : 指向热插拔事件
 * \param[out] p_stat         : 设备状态
 *
 * \retval AW_OK              : 初始化成功
 * \retval -AW_EINVAL         : 参数无效
 */
aw_err_t aw_hotplug_stat_get (aw_hotplug_event_t* p_event,
                              int                *p_stat);

/**
 * \brief 热插拔设备注册
 *
 * \param[in]  p_dev          : 热插拔设备
 * \param[in]  p_name         : 设备名
 * \param[in]  flags          : 支持的事件
 *
 * \retval AW_OK              : 初始化成功
 * \retval -AW_EINVAL         : 参数无效
 * \retval -AW_EEXIST         : 设备已存在
 */
aw_err_t aw_hotplug_dev_register (aw_hotplug_dev_t *p_dev,
                                  const char       *p_name,
                                  int               flags);

/**
 * \brief 热插拔设备取消注册
 *
 * \param[in]  p_dev          : 热插拔设备
 *
 * \retval AW_OK              : 初始化成功
 * \retval -AW_EINVAL         : 参数无效
 */
aw_err_t aw_hotplug_dev_unregister (aw_hotplug_dev_t *p_dev);

/**
 * \brief 驱动发送事件给热插拔组件
 *
 * \param[in]  p_dev          : 热插拔设备
 * \param[in]  event_flgs     : 发送的事件
 *
 * \retval AW_OK              : 初始化成功
 * \retval -AW_EINVAL         : 参数无效
 */
aw_err_t aw_hotplug_event_send (aw_hotplug_dev_t *p_dev,
                                int               event_flgs);

/**
 * \brief 热插拔组件初始化
 *
 * \retval 无
 */
void aw_hotplug_init(void);

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_hotplug_event */

#endif /* } __AW_HOTPLUG_EVT_H */

