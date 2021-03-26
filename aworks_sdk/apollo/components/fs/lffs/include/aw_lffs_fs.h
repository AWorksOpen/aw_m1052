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


#ifndef _AW_LFFS_FS_H_
#define _AW_LFFS_FS_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "aw_lffs_core.h"

/** file object */
struct aw_lffs_object {
    /******* objects manager ********/
    int          dev_lock_count;
    int          dev_get_count;

    /******** init level 0 ********/
    
    /** \brief pointer to the start of name, for open or create */
    const char  *name;                  
    uint32_t     name_len;          /**< \brief name length */
    uint16_t     sum;               /**< \brief sum of name */
    struct aw_lffs_device *dev;     /**< \brief lffs device */
    uint32_t     oflag;
    uint8_t      type;
    uint16_t     head_pages;        /**< \brief data pages on file head block */
    uint16_t     parent;

    /******* init level 1 ********/
    
    /** \brief file entry node in tree */
    struct aw_lffs_tree_node *node;                     
    uint16_t                  serial;

    /******* output ******/
    int      err;                       /**< \brief error number */

    /******* current *******/
    uint32_t pos;                       /**< \brief current position in file */

    /***** others *******/
    aw_bool_t attr_loaded;              /**< \brief attributes loaded ? */
    aw_bool_t open_succ;                /**< \brief U_TRUE or U_FALSE */

};


#define aw_lffs_object_err_get(obj) ((obj)->err)
#define aw_lffs_object_err_clr(obj) do { (obj)->err = AW_OK; } while (0)

struct aw_lffs_pool *aw_lffs_object_pool_get (void);

aw_err_t aw_lffs_object_init (void);
aw_err_t aw_lffs_object_uninit (void);
int aw_lffs_object_all_put (struct aw_lffs_device *dev);
aw_lffs_object_t * aw_lffs_object_get (void);
void aw_lffs_object_put (aw_lffs_object_t *obj);
int aw_lffs_object_index_get (aw_lffs_object_t *obj);
aw_lffs_object_t * aw_lffs_object_get_by_index (int idx);

/**
 * \brief Re-initialize an object.
 * should call this function if you want to re-use an object.
 */
aw_err_t aw_lffs_object_reinit (aw_lffs_object_t *obj);

aw_err_t aw_lffs_object_parse (aw_lffs_object_t *obj, const char *name);

aw_err_t aw_lffs_object_create_ex (aw_lffs_object_t      *obj, 
                                   struct aw_lffs_device *dev,
                                   int                    dir, 
                                   const char            *name,
                                   int                    name_len, 
                                   int                    oflag);
aw_err_t aw_lffs_object_open_ex (aw_lffs_object_t      *obj, 
                                 struct aw_lffs_device *dev,
                                 int                    dir,
                                 const char            *name,
                                 int                    name_len, 
                                 int                    oflag);

aw_err_t aw_lffs_object_open (aw_lffs_object_t *obj, 
                              const char       *fullname, 
                              int               oflag);
aw_err_t aw_lffs_object_truncate (aw_lffs_object_t *obj, uint32_t remain);

aw_err_t aw_lffs_object_create (aw_lffs_object_t *obj, 
                                const char       *fullname,
                                int               oflag);

aw_err_t aw_lffs_object_close (aw_lffs_object_t *obj);
int aw_lffs_object_write (aw_lffs_object_t *obj, const void *data, int len);
int aw_lffs_object_read (aw_lffs_object_t *obj, void *data, int len);
long aw_lffs_object_seek (aw_lffs_object_t *obj, long offset, int origin);
int aw_lffs_cur_time_get (aw_lffs_object_t *obj);
int aw_lffs_end_of_file (aw_lffs_object_t *obj);
aw_err_t aw_lffs_object_flush (aw_lffs_object_t *obj);

aw_err_t aw_lffs_object_rename (const char *old_name, 
                                const char *new_name, 
                                int        *err);
aw_err_t aw_lffs_object_delete (const char * name, int *err);

int aw_lffs_free_object_handlers_get (void);


#ifdef __cplusplus
}
#endif


#endif

/* end of file */
