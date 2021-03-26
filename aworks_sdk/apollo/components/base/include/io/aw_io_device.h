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
 * \brief I/O �豸�ṹ��
 */
struct aw_iodev {

    /**< \brief �豸�� */
    char                  name[AW_IODEV_NAME_MAX];
    struct aw_iodev      *p_next;       /**< \brief ��һ��io�豸ָ��  */
    struct aw_refcnt      ref;          /**< \brief ���ü���  */
    aw_bool_t                removed;   /**< \brief ���Ƴ���־ */
    AW_MUTEX_DECL(        mutex);       /**< \brief ������  */

    /** \brief ������� */
    int (*pfn_ent_create) (struct aw_iodev *p_dev,
                           const char      *name,
                           int              mode);

    /** \brief ɾ����� */
    int (*pfn_ent_delete) (struct aw_iodev *p_dev,
                           const char      *name);

    /** \brief ����� */
    void *(*pfn_ent_open) (struct aw_iodev *p_dev,
                           const char      *name,
                           int              oflag,
                           mode_t           mode,
                           int             *err);

    /** \brief �ر���� */
    int (*pfn_ent_close) (void *p_ent);

    /** \brief ������ */
    int (*pfn_ent_read) (void   *p_ent,
                         void   *p_buf,
                         size_t  nbyte);

    /** \brief д���� */
    int (*pfn_ent_write) (void        *p_ent,
                          const void  *p_buf,
                          size_t       nbyte);

    /** \brief io���� */
    int (*pfn_ent_ioctrl) (void    *p_ent,
                           int      cmd,
                           void    *p_arg);

    /** \brief �ͷ��豸 */
    void (*pfn_dev_release) (struct aw_iodev *p_dev);
};


/**
 * \brief ��ʼ��io�豸��
 */
int aw_iodev_lib_init (void);

/**
 * \brief ����һ��io�豸
 *
 * \param[in]        p_dev      �豸ָ��
 * \param[in]        name       �豸��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_iodev_create (struct aw_iodev *p_dev,
                     const char      *name);

/**
 * \brief ����һ��IO�豸
 *
 * \param[in]        p_dev      �豸ָ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_iodev_destroy (struct aw_iodev *p_dev);


/**
 * \brief ��ȡһ��IO�豸
 *
 * \param[in]        name      �豸��
 * \param[out]       tail      ָ��name�Ľ�����ַ
*
 * \return �ɹ�����io�豸ָ�룬���򷵻�NULL
 */
struct aw_iodev *aw_iodev_get (const char *name, const char **tail);


/**
 * \brief �Ż�һ��IO�豸
 *
 * \param[in]        p_dev      �豸ָ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_iodev_put (struct aw_iodev *p_dev);


/**
 * \brief ��ÿһ��IO�豸��ִ��һ�κ���ָ�� pfn
 *
 * \param[in]        pfn      ����ָ��
 * \param[in]        arg      ����
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_iodev_foreach (int   (*pfn)(struct aw_iodev *p_dev, void *arg),
                      void   *arg);


/**
 * \brief ����
 *
 * \param[in]        p_dev      io�豸ָ��
 * \param[in]        name       Ҫ�������������
 * \param[in]        mode       ģʽ
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_iodev_ent_create (struct aw_iodev *p_dev,
                         const char      *name,
                         mode_t           mode);


/**
 * \brief ɾ��
 *
 * \param[in]        p_dev      io�豸ָ��
 * \param[in]        name       Ҫɾ�����������
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_iodev_ent_delete (struct aw_iodev *p_dev,
                         const char      *name);


/**
 * \brief ��
 *
 * \param[in]        p_dev      io�豸ָ��
 * \param[in]        name       Ҫɾ�����������
 * \param[in]        flag       �򿪱�־
 * \param[in]        mode       ��ģʽ
 * \param[out]       err        ��������ֵ
 *
 * \return �ɹ����ض����ַ�����򷵻�NULL ��
 */
void * aw_iodev_ent_open (struct aw_iodev *p_dev,
                          const char      *name,
                          int              flag,
                          mode_t           mode,
                          int             *err);


/**
 * \brief �ر�
 *
 * \param[in]        p_dev      io�豸ָ��
 * \param[in ]       p_ent      Ҫ�رյĶ���ָ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_iodev_ent_close (struct aw_iodev *p_dev,
                        void            *p_ent);


/**
 * \brief ������
 *
 * \param[in]        p_dev      io�豸ָ��
 * \param[in]        p_ent      ����ָ��
 * \param[out]       p_buf      ����
 * \param[in]        nbyte      �ֽ���
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_iodev_ent_read (struct aw_iodev *p_dev,
                       void            *p_ent,
                       void            *p_buf,
                       size_t           nbyte);


/**
 * \brief д����
 *
 * \param[in]        p_dev      io�豸ָ��
 * \param[in]        p_ent      ����ָ��
 * \param[in]        p_buf      ����
 * \param[in]        nbyte      �ֽ���
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_iodev_ent_write (struct aw_iodev *p_dev,
                        void            *p_ent,
                        const void      *p_buf,
                        size_t           nbyte);


/**
 * \brief ����io��������
 *
 * \param[in]        p_dev      io�豸ָ��
 * \param[in]        p_ent      ����ָ��
 * \param[in]        cmd        ����
 * \param[out]       p_arg      ����ָ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
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
