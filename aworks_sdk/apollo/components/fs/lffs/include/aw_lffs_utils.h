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
 * \brief
 *
 * \internal
 * \par modification history:
 * - 170923, vih, first implementation.
 * \endinternal
 */


#ifndef _AW_LFFS_UTILS_H_
#define _AW_LFFS_UTILS_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "aw_lffs_device.h"


/* get LFFS disk version, if fail, return 0 */
int aw_lffs_version_get(struct aw_lffs_device *dev);

void aw_lffs_global_fslock_init(void);
void aw_lffs_global_fslock_release(void);
void aw_lffs_global_fslock_lock(void);
void aw_lffs_global_fslock_unlock(void);

aw_err_t aw_lffs_device_format(aw_lffs_device_t *dev, aw_bool_t force);

#ifdef __cplusplus
}
#endif

typedef void aw_lffs_dump_msg_cb_t (aw_lffs_device_t *dev, const char *fmt, ...);

void aw_lffs_device_dump(aw_lffs_device_t *dev, aw_lffs_dump_msg_cb_t *dump);

#endif /* _AW_LFFS_UTILS_H_ */

/* end of file */
