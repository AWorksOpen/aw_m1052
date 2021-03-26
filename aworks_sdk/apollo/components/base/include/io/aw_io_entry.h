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
 * \brief I/O entry manage
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-08  deo, first implementation
 * \endinternal
 */


#ifndef __AW_IO_ENTRY_H
#define __AW_IO_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_iosys_entry
 * @{
 */


#include "aw_io_device.h"



/**
 * \brief I/O �豸��ڽṹ��
 */
struct aw_io_entry {
    struct aw_iodev  *p_dev;
    void             *p_ent;
};


/**
 * \brief ��ʼ��IO��ڹ�����
 *
 * \param[in]        p_ents     ��ڽṹ��ָ��
 * \param[in]        count      �������
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_io_entry_init (struct aw_io_entry *p_ents, int count);


/**
 * \brief ��ȡ��ڵ�����
 *
 * \param[in]        p_ents     ��ڽṹ��ָ��
 *
 * \return ���� >=0 Ϊ������������򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_io_entry_to_index (struct aw_io_entry *p_entry);


/**
 * \brief ͨ��������ȡ���ָ��
 *
 * \param[in]        index     ��ڵ�����
 *
 * \return �ɹ��������ָ�룬���򷵻�NULL
 */
struct aw_io_entry *aw_io_index_to_entry (int index);


/**
 * \brief ����һ�����
 *
 * \return �ɹ��������ָ�룬���򷵻�NULL
 */
struct aw_io_entry *aw_io_entry_alloc (void);


/**
 * \brief �ͷ�һ��alloc���������
 *
 * \param[in]        p_ents     ��ڽṹ��ָ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_io_entry_free (struct aw_io_entry *p_entry);


/**
 * \brief ��������������Ч��
 *
 * \param[in]        index     ��ڵ�����
 *
 * \return ��Ч���� TRUE����Ч���� AW_FALSE
 */
aw_bool_t aw_io_entry_valid (int index);

/** @} grp_iosys_entry */

#ifdef __cplusplus
}
#endif

#endif /* __AW_IO_ENTRY_H */

/* end of file */
