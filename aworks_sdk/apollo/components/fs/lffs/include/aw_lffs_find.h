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


#ifndef _AW_LFFS_FIND_H_
#define _AW_LFFS_FIND_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "aw_lffs_core.h"

typedef struct aw_lffs_findinfo {

    aw_lffs_device_t    *dev;       /**< \brief the device to be searched */
    uint16_t             serial;    /**< \brief the dir serial number */

    /**
     * \brief
     *      step:   0 - working on dir entries,
     *              1 - working on file entries,
     *              2 - stoped.
     */
    int                  step;
    aw_lffs_tree_node_t *work;      /**< \brief working node, internal used. */
    int                  pos;       /**< \brief current position. */
} aw_lffs_findinfo_t;


aw_err_t aw_lffs_object_info_get (aw_lffs_object_t      *obj, 
                                  aw_lffs_object_info_t *info, 
                                  int                   *err);
aw_err_t aw_lffs_findinfo_open (aw_lffs_findinfo_t *find_handle,
                                aw_lffs_object_t   *dir);
aw_err_t aw_lffs_findinfo_open_ex (aw_lffs_findinfo_t *f, 
                                   aw_lffs_device_t   *dev, 
                                   int                 dir);
aw_err_t aw_lffs_first_object_info_find (aw_lffs_object_info_t *info,
                                         aw_lffs_findinfo_t    *find_handle);
aw_err_t aw_lffs_next_object_info_find (aw_lffs_object_info_t *info, 
                                        aw_lffs_findinfo_t    *find_handle);
aw_err_t aw_lffs_findinfo_rewind (aw_lffs_findinfo_t *find_handle);
aw_err_t aw_lffs_findinfo_close (aw_lffs_findinfo_t * find_handle);


#ifdef __cplusplus
}
#endif


#endif

/* end of file */
