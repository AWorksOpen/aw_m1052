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
 * \brief mount table related stuff
 *
 * \internal
 * \par modification history:
 * - 170923, vih, first implementation.
 * \endinternal
 */


#ifndef _AW_LFFS_MTB_H_
#define _AW_LFFS_MTB_H_


#ifdef __cplusplus
extern "C"{
#endif


#include "aw_lffs_core.h"


typedef struct aw_lffs_mount_table {
    
    /** \brief LFFS 'device' - core internal data structure for partition */
    aw_lffs_device_t            *dev;      
    
    /** \brief partition start block */
    int                          start_block;        
    
    /*
     * \brief partition end block ( if < 0, reserve space form the end 
     * of storage) 
     */
    int                          end_block;          
    
    /** \brief mount point */
    const char                  *mount_point;      
/*    struct aw_lffs_mount_table  *prev; */
/*    struct aw_lffs_mount_table  *next; */
    struct aw_list_head          node;
} aw_lffs_mount_table_t;

/** \brief Register mount entry, will be put at 'unmounted' list */
int aw_lffs_mount_table_reister (aw_lffs_mount_table_t *mtb);

/** \brief Remove mount entry from the list */
int aw_lffs_mount_table_unreister (aw_lffs_mount_table_t *mtb);

/** \brief mount partition */
int aw_lffs_mount (const char *mount);

/** \brief unmount parttion */
int aw_lffs_unmount (const char *mount);

/** \brief get mounted entry list */
aw_lffs_mount_table_t * aw_lffs_mounted_mtb_get (void);

/** \brief get unmounted entry list */
aw_lffs_mount_table_t * aw_lffs_unmounted_mtb_get (void);

/** \brief get matched mount point from absolute path */
int aw_lffs_matched_mount_point_len (const char *path);

/** \brief get lffs device from mount point */
aw_lffs_device_t * aw_lffs_device_get (const char *mount);

/** \brief get lffs device from mount point */
aw_lffs_device_t * aw_lffs_device_get_ex (const char *mount, int len);

/** \brief get mount point name from lffs device */
const char * aw_lffs_mount_point_get (aw_lffs_device_t *dev);

/** \brief down crease lffs device references by lffs_GetDeviceXXX () */
void aw_lffs_device_put (aw_lffs_device_t *dev);

#if (AW_LFFS_CONFIG_ENABLE_INFO_TABLE == 1)
aw_err_t aw_lffs_info_table_download (aw_lffs_device_t *dev);
aw_err_t aw_lffs_info_table_delay_update (aw_lffs_device_t *dev, 
                                          uint32_t          ms_delay);
#else
#define aw_lffs_info_table_download()
#define aw_lffs_info_table_delay_update()
#endif /* AW_LFFS_CONFIG_ENABLE_INFO_TABLE */

#ifdef __cplusplus
}
#endif
#endif

/* end of file */
