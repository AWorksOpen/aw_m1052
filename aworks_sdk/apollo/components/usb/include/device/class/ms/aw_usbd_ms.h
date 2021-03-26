/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/


#ifndef __AWBL_USBD_MS_H
#define __AWBL_USBD_MS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_usbd_ms
 * \copydoc aw_usbd_ms.h
 * @{
 */
#include "aw_usb_os.h"
#include "device/aw_usbd.h"
#include "fs/aw_blk_dev.h"


/** \brief 大容量存储器任务优先级和堆栈大小 */
#define AW_USBD_MS_TASK_PRIO    8
#define AW_USBD_MS_STACK_SIZE   2048



/** \brief 设备逻辑单元 */
struct aw_usbd_ms_lun {
    struct aw_blk_dev      *p_bd;      /**< \brief 块设备指针 */
    int                     num;       /**< \brief 逻辑单元号 */
    struct aw_list_head     node;      /**< \brief 逻辑单元链表节点 */
    uint32_t                sd;        /**< \brief SCSI sense key */
    uint32_t                sd_inf;    /**< \brief SCSI sense信息 */
    aw_bool_t               inf_valid; /**< \brief sense info是否有效 */
    aw_bool_t               ro;        /**< \brief 是否只读 */
    aw_bool_t               rm;        /**< \brief 是否可移除 */
    aw_bool_t               pv;        /**< \brief prevent标志 */
    uint_t                  bsize;     /**< \brief 块大小 */
    uint_t                  nblks;     /**< \brief 块数量 */
//    aw_usb_mutex_handle_t   lock;    //TODO 后续版本需要处理多线程通信问题
    aw_usb_sem_handle_t     blk_sync;
    aw_usb_sem_handle_t     usb_sync;
};




/** \brief 大容量存储器设备 */
struct aw_usbd_ms {
    struct aw_usbd_fun      ufun;       /**< \brief USB功能接口*/
    struct aw_usbd_pipe     in;         /**< \brief IN数据管道 */
    struct aw_usbd_pipe     out;        /**< \brief OUT数据管道 */

    aw_usb_task_handle_t    ms_task;    /**< \brief 大容量存储器任务 */
    aw_usb_sem_handle_t     semb;
    aw_usb_mutex_handle_t   mutex;

    uint8_t                 nluns;      /**< \brief 逻辑单元个数 */
    struct aw_list_head     luns;       /**< \brief 逻辑单元链表 */
    void                   *p_ubuf;     /**< \brief USBD缓冲区 */
    void                   *p_bbuf;     /**< \brief LUN缓冲区 */
    uint32_t                buf_size;   /**< \brief 缓冲区大小 */
    aw_bool_t               buf_auto;   /**< \brief 是否动态分配内存 */

    uint32_t                tag;        /**< \brief CBW/CSW标签 */
    uint32_t                dlen;       /**< \brief CBW数据长度 */
    uint32_t                cdlen;      /**< \brief CBW有效字节长度 */
    uint8_t                 cb[16];     /**< \brief CBW命令 */
    uint8_t                 dir;        /**< \brief CBW标志 */
    uint8_t                 clen;       /**< \brief CBW命令长度 */
    uint8_t                 sta;        /**< \brief CSW状态 */
    struct aw_usbd_ms_lun  *p_lun;      /**< \brief 当前使用的逻辑单元 */

};




/**
 * \brief 创建一个大容量存储设备
 *
 * \param[in] p_obj     USBD对象指针
 * \param[in] p_ms      大容量存储器对象指针
 * \param[in] name      大容量存储器名称
 * \param[in] p_buf     存储器缓存首地址（为NULL则内部动态申请）
 * \param[in] buf_size  存储器缓存大小
 *
 * \retval  AW_OK       创建成功
 * \retval  -AW_EINVAL  无效参数
 * \retval  -AW_ENOSPC  缓存空间过小
 * \retval  -AW_ENOMEM  存储空间不足
 */
int aw_usbd_ms_create (struct aw_usbd      *p_obj,
                       struct aw_usbd_ms   *p_ms,
                       const char          *name,
                       void                *p_buf,
                       int                  buf_size);




/**
 * \brief 销毁一个大容量存储设备
 *
 * \param[in] p_ms      大容量存储器对象指针
 *
 * \retval  AW_OK       销毁成功
 * \retval  -AW_EINVAL  无效参数
 * \retval  -AW_EBUSY   设备正忙
 * \retval  -AW_ENOENT  设备功能不存在
 */
int aw_usbd_ms_destroy (struct aw_usbd_ms  *p_ms);




/**
 * \brief 添加一个逻辑单元到大容量存储设备
 *
 * \param[in] p_ms      大容量存储器对象指针
 * \param[in] name      逻辑单元名称
 *
 * \retval  AW_OK       添加成功
 * \retval  -AW_EINVAL  无效参数
 * \retval  -AW_ENODEV  设备不存在
 * \retval  -AW_ENOMEM  存储空间不足
 */
int aw_usbd_ms_lun_add (struct aw_usbd_ms  *p_ms,
                        const char         *name);


/** @} grp_aw_if_usbd_ms */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __AWBL_USBD_MS_H */
