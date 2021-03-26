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
 * \brief romfs dev�����ʼ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
aw_err_t aw_romfs_dev_lib_init (void);

/**
 * \brief ��̬����romfs dev, �����뵽����
 *
 * \param[in]    name       �豸��
 * \param[in]    dev_info   �豸��Ϣ
 * \param[in]    info_size  �豸��Ϣ��С
 * \param[in]    ops        �豸��������
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
aw_err_t aw_romfs_dev_create (const char                    *name,
                              void                          *dev_info,
                              uint32_t                       info_size,
                              const struct aw_romfs_dev_ops *ops);

/**
 * \brief ����romfs dev
 *
 * \param[in]    p_dev      �豸ָ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
aw_err_t aw_romfs_dev_destroy (struct aw_romfs_dev *p_dev);

/**
 * \brief ��ȡromfs dev
 *
 * \param[in]    name       �豸����
 *
 * \return �ɹ������豸ָ�룬���򷵻�ֵΪNULL��
 */
struct aw_romfs_dev *aw_romfs_dev_get (const char *name);

/**
 * \brief �ͷ�romfs dev
 *
 * \param[in]    p_dev      �豸ָ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
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
