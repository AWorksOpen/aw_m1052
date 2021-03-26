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
 * \brief AWorks RomFs dev
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-30  sdy, first implementation
 * \endinternal
 */

#ifndef __AW_ROMFS_DEV_H
#define __AW_ROMFS_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_list.h"

#define AW_RD_NAME_MAX  32

#define IMG_HEAD_VOL_MAX    32
#define IMG_HEAD_LEN_MAX    20 + IMG_HEAD_VOL_MAX

#define IMG_DATA_NAME_MAX   32
#define IMG_DATA_HEAD_MAX   16 + IMG_DATA_NAME_MAX

struct aw_romfs_dev;

struct aw_romfs_img_info {
    size_t  head_size;
    size_t  img_size;
};

/** \brief romfs driver operations */
struct aw_romfs_dev_ops {
    aw_err_t (*pfn_init) (struct aw_romfs_dev *p_dev);
    aw_err_t (*pfn_deinit) (struct aw_romfs_dev *p_dev);
    ssize_t  (*pfn_read) (struct aw_romfs_dev *p_dev,
                          off_t                from,
                          void                *buf,
                          size_t               len);
};

struct aw_romfs_dev {
    struct aw_list_head      list;
    char                     name[AW_RD_NAME_MAX];  /**< \brief device name */
    AW_MUTEX_DECL(           lock);                 /**< \brief device lock */
    
    /** \brief device use count */
    uint32_t                 ref_count;             
    uint8_t                 *buf;                  /**< \brief read img buf */
    void                    *dev_info;             /**< \brief device message */
    struct aw_romfs_img_info img_info;             /**< \brief img message */
    
    
    void                    *p_info;          /**< \brief device private data */

    const struct aw_romfs_dev_ops *ops;       /**< \brief device operations */
};

/**
 * \brief romfs dev链表初始化
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
aw_err_t aw_romfs_dev_lib_init (void);

/**
 * \brief 动态创建romfs dev, 并加入到链表
 *
 * \param[in]    name       设备名
 * \param[in]    dev_info   设备信息
 * \param[in]    info_size  设备信息大小
 * \param[in]    ops        设备操作函数
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
aw_err_t aw_romfs_dev_create (const char                    *name,
                              void                          *dev_info,
                              uint32_t                       info_size,
                              const struct aw_romfs_dev_ops *ops);

/**
 * \brief 销毁romfs dev
 *
 * \param[in]    p_dev      设备指针
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
aw_err_t aw_romfs_dev_destroy (struct aw_romfs_dev *p_dev);

/**
 * \brief 获取romfs dev
 *
 * \param[in]    name       设备名字
 *
 * \return 成功返回设备指针，否则返回值为NULL。
 */
struct aw_romfs_dev *aw_romfs_dev_get (const char *name);

/**
 * \brief 释放romfs dev
 *
 * \param[in]    p_dev      设备指针
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
aw_err_t aw_romfs_dev_put (struct aw_romfs_dev *p_dev);


aw_err_t aw_romfs_dev_init (struct aw_romfs_dev *p_dev);
aw_err_t aw_romfs_dev_deinit (struct aw_romfs_dev *p_dev);
ssize_t aw_romfs_dev_read (struct aw_romfs_dev *p_dev,
                           off_t                from,
                           void                *buf,
                           size_t               len);

#endif /* __AW_ROMFS_DEV_H */

/* end of file */
