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

#ifndef _AW_LFFS_OS_H_
#define _AW_LFFS_OS_H_


#ifdef __cplusplus
extern "C"{
#endif

#include "aworks.h"
#include "stdarg.h"


#define AW_LFFS_TASK_ID_NOT_EXIST   -1

typedef void * aw_lffs_sem_t;
#define AW_LFFS_SEM_NOT_INITED  (NULL)

typedef struct aw_lffs_debug {
    void (*output) (const char *msg);
    void (*vprintf)(const char *fmt, va_list args);
} aw_lffs_debug_t;

void aw_lffs_debug_setup(void);

/* OS specific functions */
int aw_lffs_sem_create(aw_lffs_sem_t *sem);
int aw_lffs_sem_take(aw_lffs_sem_t sem);
int aw_lffs_sem_give(aw_lffs_sem_t sem);
int aw_lffs_sem_delete(aw_lffs_sem_t *sem);

int aw_lffs_taskid_get(void);   /* get current task id */
unsigned int aw_lffs_cur_datetime_get(void);

#ifdef __cplusplus
}
#endif


#endif   /* _AW_LFFS_OS_H_ */

/* end of file */
