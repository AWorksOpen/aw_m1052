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
 * \brief I/O device
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-08  deo, first implementation
 * \endinternal
 */
#ifndef __AW_IODEV_H
#define __AW_IODEV_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_iosys_device
 * @{
 */

#include "aw_list.h"
#include "aw_sem.h"
#include "aw_refcnt.h"

#define AW_IODEV_NAME_MAX    32


/**
 * \brief I/O 设备结构体
 */
struct aw_iodev {

    /**< \brief 设备名 */
    char                  name[AW_IODEV_NAME_MAX];
    struct aw_iodev      *p_next;       /**< \brief 下一个io设备指针  */
    struct aw_refcnt      ref;          /**< \brief 引用计数  */
    aw_bool_t                removed;   /**< \brief 被移除标志 */
    AW_MUTEX_DECL(        mutex);       /**< \brief 互斥锁  */

    /** \brief 创建入口 */
    int (*pfn_ent_create) (struct aw_iodev *p_dev,
                           const char      *name,
                           int              mode);

    /** \brief 删除入口 */
    int (*pfn_ent_delete) (struct aw_iodev *p_dev,
                           const char      *name);

    /** \brief 打开入口 */
    void *(*pfn_ent_open) (struct aw_iodev *p_dev,
                           const char      *name,
                           int              oflag,
                           mode_t           mode,
                           int             *err);

    /** \brief 关闭入口 */
    int (*pfn_ent_close) (void *p_ent);

    /** \brief 读数据 */
    int (*pfn_ent_read) (void   *p_ent,
                         void   *p_buf,
                         size_t  nbyte);

    /** \brief 写数据 */
    int (*pfn_ent_write) (void        *p_ent,
                          const void  *p_buf,
                          size_t       nbyte);

    /** \brief io控制 */
    int (*pfn_ent_ioctrl) (void    *p_ent,
                           int      cmd,
                           void    *p_arg);

    /** \brief 释放设备 */
    void (*pfn_dev_release) (struct aw_iodev *p_dev);
};


/**
 * \brief 初始化io设备库
 */
int aw_iodev_lib_init (void);

/**
 * \brief 创建一个io设备
 *
 * \param[in]        p_dev      设备指针
 * \param[in]        name       设备名
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_iodev_create (struct aw_iodev *p_dev,
                     const char      *name);

/**
 * \brief 销毁一个IO设备
 *
 * \param[in]        p_dev      设备指针
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_iodev_destroy (struct aw_iodev *p_dev);


/**
 * \brief 获取一个IO设备
 *
 * \param[in]        name      设备名
 * \param[out]       tail      指向name的结束地址
*
 * \return 成功返回io设备指针，否则返回NULL
 */
struct aw_iodev *aw_iodev_get (const char *name, const char **tail);


/**
 * \brief 放回一个IO设备
 *
 * \param[in]        p_dev      设备指针
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_iodev_put (struct aw_iodev *p_dev);


/**
 * \brief 让每一个IO设备都执行一次函数指针 pfn
 *
 * \param[in]        pfn      函数指针
 * \param[in]        arg      参数
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_iodev_foreach (int   (*pfn)(struct aw_iodev *p_dev, void *arg),
                      void   *arg);


/**
 * \brief 创建
 *
 * \param[in]        p_dev      io设备指针
 * \param[in]        name       要创建对象的名字
 * \param[in]        mode       模式
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_iodev_ent_create (struct aw_iodev *p_dev,
                         const char      *name,
                         mode_t           mode);


/**
 * \brief 删除
 *
 * \param[in]        p_dev      io设备指针
 * \param[in]        name       要删除对象的名字
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_iodev_ent_delete (struct aw_iodev *p_dev,
                         const char      *name);


/**
 * \brief 打开
 *
 * \param[in]        p_dev      io设备指针
 * \param[in]        name       要删除对象的名字
 * \param[in]        flag       打开标志
 * \param[in]        mode       打开模式
 * \param[out]       err        操作错误值
 *
 * \return 成功返回对象地址，否则返回NULL 。
 */
void * aw_iodev_ent_open (struct aw_iodev *p_dev,
                          const char      *name,
                          int              flag,
                          mode_t           mode,
                          int             *err);


/**
 * \brief 关闭
 *
 * \param[in]        p_dev      io设备指针
 * \param[in ]       p_ent      要关闭的对象指针
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_iodev_ent_close (struct aw_iodev *p_dev,
                        void            *p_ent);


/**
 * \brief 读数据
 *
 * \param[in]        p_dev      io设备指针
 * \param[in]        p_ent      对象指针
 * \param[out]       p_buf      缓存
 * \param[in]        nbyte      字节数
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_iodev_ent_read (struct aw_iodev *p_dev,
                       void            *p_ent,
                       void            *p_buf,
                       size_t           nbyte);


/**
 * \brief 写数据
 *
 * \param[in]        p_dev      io设备指针
 * \param[in]        p_ent      对象指针
 * \param[in]        p_buf      缓存
 * \param[in]        nbyte      字节数
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_iodev_ent_write (struct aw_iodev *p_dev,
                        void            *p_ent,
                        const void      *p_buf,
                        size_t           nbyte);


/**
 * \brief 发送io控制命令
 *
 * \param[in]        p_dev      io设备指针
 * \param[in]        p_ent      对象指针
 * \param[in]        cmd        命令
 * \param[out]       p_arg      参数指针
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_iodev_ent_ioctrl (struct aw_iodev *p_dev,
                         void            *p_ent,
                         int              cmd,
                         void            *p_arg);

/** @} grp_iosys_device */

#ifdef __cplusplus
}
#endif

#endif /* __AW_IODEV_H */

/* end of file */
