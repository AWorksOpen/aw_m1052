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
 * \brief 抽象扫描器类定义
 *
 *    抽象扫描器以scan_interval的时间执行一次扫描行为, 在一次扫描行为中, 
 *    扫描scan_cnt次, 每次扫描, 依次调用回调函数管理链表中已注册的回调函数.
 *
 *    每个扫描器对象内部均维护一条回调函数管理链表, 使用者通过
 *    awbl_scan_cb_list_add/awbl_scan_cb_list_del进行增删.
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-04  ops, first implementation.
 * \endinternal
 */
#ifndef AWBL_SCANNER_H_
#define AWBL_SCANNER_H_

#include "aw_delayed_work.h"
#include "aw_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief 扫描器回调函数原型
 */
typedef void scan_cb_func_t (void *p_arg);


/**
 * \brief 扫描器回调函数句柄
 */
struct awbl_scan_cb_handler {

    scan_cb_func_t      *pfn_scan_cb;

    struct aw_list_head  list_elm;
};

/**
 * \brief 扫描器配置参数
 */
struct awbl_scanner_param {

    /**
     * \brief 扫描间隔
     */
    uint16_t  scan_interval;

    /**
     * \brief 在一次扫描周期内, 扫描的次数
     */
    uint16_t  scan_cnt;
};

/**
 * \brief 扫描器设备
 */
struct awbl_scanner_dev {

    /**
     * \brief 扫描器的配置参数
     */
    struct awbl_scanner_param *p_par;

    /**
     * \brief 延时作业
     */
    struct aw_delayed_work  work;

    /**
     * \brief 扫描索引
     */
    uint8_t   scan_index;

    /**
     * \brief 扫描使能标记, 使能: AW_TRUE, 禁能: AW_FALSE
     */
    aw_bool_t    scan_enable;

    /**
     * \brief 暂停时间(ms)
     */
    uint16_t  pause_time;

    /**
     * \brief 回调管理链表头
     */
    struct aw_list_head  cb_list_head;

    /**
     * \brief 启动扫描
     *
     * \param  p_scan  扫描器对象.
     */
    void (*pfn_scanner_start)(struct awbl_scanner_dev *p_scan);

    /**
     * \brief 暂停扫描(暂停指定时长后, 继续按原扫描间隔扫描)
     *
     * \param  p_scan      扫描器对象.
     * \param  pause_time  暂停时长
     */
    void (*pfn_scanner_pause)(struct awbl_scanner_dev *p_scan, 
                              uint16_t                 pause_time);

    /**
     * \brief 停止扫描
     *
     * \param  p_scan  扫描器对象.
     */
    void (*pfn_scanner_stop)(struct awbl_scanner_dev *p_scan);

};

aw_local aw_inline void awbl_scan_cb_list_add(
    struct awbl_scanner_dev     *p_scan,
    struct awbl_scan_cb_handler *p_cb_handler)
{
    aw_list_add_tail(&p_cb_handler->list_elm, &p_scan->cb_list_head);
}

aw_local aw_inline void awbl_scan_cb_list_del(
    struct awbl_scan_cb_handler *p_cb_handler)
{
    aw_list_del(&p_cb_handler->list_elm);
}

void awbl_scanner_start(struct awbl_scanner_dev *p_scan);

void awbl_scanner_pause(struct awbl_scanner_dev *p_scan, uint16_t pause_time);

void awbl_scanner_stop(struct awbl_scanner_dev *p_scan);

void awbl_scanner_dev_ctor(struct awbl_scanner_dev   *p_scan, 
                           struct awbl_scanner_param *p_par);

#ifdef __cplusplus
}
#endif

#endif /* AWBL_HC595_SCANNER_H_ */

/* end of file */
