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
 * \brief access and modification times structure
 *
 * \internal
 * \par modification history:
 * - 1.00 15-04-20 deo, first implementation.
 * \endinternal
 */

#ifndef AW_UTIME_H_
#define AW_UTIME_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_iosys_utime
 * @{
 */

#include "aworks.h"
#include <aw_types.h>

/**
 * \brief utime�ṹ��
 */
struct aw_utimbuf {
    time_t    actime;    /**< \brief ����ʱ��. */
    time_t    modtime;   /**< \brief �޸�ʱ��. */
};


/**
 * \brief �����ļ��ķ��ʺ��޸�ʱ��
 *
 * \param[in]         path     �ļ�·��
 * \param[in]         times    �ṹ��ָ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_utime(const char *path, struct aw_utimbuf *times);

/** @} grp_iosys_utime */

#ifdef __cplusplus
}
#endif

#endif /* AW_UTIME_H_ */
