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
 * \brief lffs page buffers manipulations
 *
 * \internal
 * \par modification history:
 * - 170923, vih, first implementation.
 * \endinternal
 */

#ifndef _AW_LFFS_BUF_H
#define _AW_LFFS_BUF_H

#include "aw_lffs_core.h"

#ifdef __cplusplus
extern "C"{
#endif

/** \brief set aw_lffs_buf_St::ref_count to this for a 'cloned' buffer */
#define AW_LFFS_CLONE_BUF_MARK      0xffff      

/** \brief for aw_lffs_buf_St::mark */
#define AW_LFFS_BUF_EMPTY      0     /**< \brief buffer is empty */
#define AW_LFFS_BUF_VALID      1     /**< \brief buffer is holding valid data */
#define AW_LFFS_BUF_DIRTY      2     /**< \brief buffer data is modified */

/** for aw_lffs_buf_St::ext_mark */

/** \brief the last page of file (when truncating a file) */
#define AW_LFFS_BUF_EXT_MARK_TRUNC_TAIL 1  

/** \brief lffs page buffer */
struct aw_lffs_buf{
    struct aw_list_head     node;
    struct aw_list_head     dirty_node;
    
    /** \brief #LFFS_TYPE_DIR or #LFFS_TYPE_FILE or #LFFS_TYPE_DATA */
    uint8_t type;                            
    uint8_t ext_mark;                        /**< \brief extension mark. */
    uint16_t parent;                         /**< \brief parent serial */
    uint16_t serial;                         /**< \brief serial */
    uint16_t page_id;                        /**< \brief page id */
    
    /** \brief #LFFS_BUF_EMPTY or #LFFS_BUF_VALID, or #LFFS_BUF_DIRTY ? */
    uint16_t mark;                           
    
    /** \brief reference counter, or #CLONE_BUF_MARK for a cloned buffer */
    uint16_t ref_count;                      
    uint16_t data_len;                       /**< \brief length of data */
    uint16_t check_sum;                      /**< \brief checksum field */
    uint8_t *data;                           /**< \brief data buffer */
    uint8_t *header;                         /**< \brief header */
};

#define aw_lffs_buf_is_free(buf) (buf->ref_count == 0 ? AW_TRUE : AW_FALSE)

/** \brief initialize page buffers */
aw_err_t aw_lffs_buf_init (struct aw_lffs_device *dev, 
                           int                    buf_max,
                           int                    dirty_buf_max);

/** \brief release page buffers */
aw_err_t aw_lffs_buf_release_all (struct aw_lffs_device *dev);

/** \brief find the page buffer, move to link list head if found */
aw_lffs_buf_t * aw_lffs_buf_get (struct aw_lffs_device *dev, 
                                 uint16_t               parent, 
                                 uint16_t               serial, 
                                 uint16_t               page_id);
                                 
aw_lffs_buf_t *aw_lffs_buf_get_ex (struct aw_lffs_device *dev, 
                                   uint8_t                type, 
                                   aw_lffs_tree_node_t   *node, 
                                   uint16_t               page_id,
                                   int                    oflag);

/** \brief alloc a new page buffer */
aw_lffs_buf_t *aw_lffs_buf_new (struct aw_lffs_device *dev,
                                uint8_t                type, 
                                uint16_t               parent, 
                                uint16_t               serial, 
                                uint16_t               page_id);

/** \brief find the page buffer (not affect the reference counter) */
aw_lffs_buf_t * aw_lffs_buf_find (aw_lffs_device_t *dev, 
                                  uint16_t          parent,
                                  uint16_t          serial,
                                  uint16_t          page_id);

/** \brief find the page buffer from #start (not affect the reference counter) */
aw_lffs_buf_t * aw_lffs_buf_find_ex (aw_lffs_device_t *dev,
                                     aw_lffs_buf_t    *start,
                                     uint16_t          parent, 
                                     uint16_t          serial, 
                                     uint16_t          page_id);

/*
 * \brief put page buffer back to pool, called in pair with #lffs_Get,
 * #lffs_GetEx or #aw_lffs_buf_New 
 */
aw_err_t aw_lffs_buf_put (aw_lffs_device_t *dev, aw_lffs_buf_t *buf);

/** \brief increase buffer references */
void aw_lffs_buf_ref_increase (aw_lffs_buf_t *buf);

/** \brief decrease buffer references */
void aw_lffs_buf_ref_decrease (aw_lffs_buf_t *buf);

/** \brief write data to a page buffer */
aw_err_t aw_lffs_buf_write (struct aw_lffs_device *dev, 
                            aw_lffs_buf_t         *buf, 
                            void                  *data, 
                            uint32_t               ofs,
                            uint32_t               len);

/** \brief read data from a page buffer */
aw_err_t aw_lffs_buf_read (struct aw_lffs_device *dev, 
                           aw_lffs_buf_t *buf, 
                           void          *data, 
                           uint32_t       ofs, 
                           uint32_t       len);

/*
 * \brief mark buffer as #LFFS_BUF_EMPTY if ref_count == 0, 
 * and discard all data it holds 
 */
void aw_lffs_buf_mark_empty (aw_lffs_device_t *dev, aw_lffs_buf_t *buf);

/** \brief if there is no free dirty group slot, flush the most dirty group */
aw_err_t aw_lffs_buf_flush (struct aw_lffs_device *dev);
aw_err_t aw_lffs_buf_flush_ex (struct aw_lffs_device *dev, 
                               aw_bool_t              force_block_recover);

/** \brief flush dirty group */
aw_err_t aw_lffs_group_flush (struct aw_lffs_device *dev, 
                              uint16_t               parent, 
                              uint16_t               serial);
aw_err_t aw_lffs_group_flush_ex (struct aw_lffs_device *dev,
                                 uint16_t               parent, 
                                 uint16_t               serial,
                                 aw_bool_t              force_block_recover);

/** \brief find free dirty group slot */
int aw_lffs_free_group_slot_find (struct aw_lffs_device *dev);

/** \brief find the dirty group slot */
int aw_lffs_group_slot_find (struct aw_lffs_device *dev, 
                             uint16_t               parent, 
                             uint16_t               serial);

/** \brief lock dirty group */
aw_err_t aw_lffs_group_lock (struct aw_lffs_device *dev, int slot);

/** \brief unlock dirty group */
aw_err_t aw_lffs_group_unlock (struct aw_lffs_device *dev, int slot);

/** \brief flush most dirty group */
aw_err_t aw_lffs_most_dirty_group_flush (struct aw_lffs_device *dev);

/** \brief flush all groups under the same parent number */
aw_err_t aw_lffs_buf_flush_by_parent (struct aw_lffs_device *dev,
                                      uint16_t               parent);

/** \brief flush all page buffers */
aw_err_t aw_lffs_buf_all_flush (struct aw_lffs_device *dev);

/** \brief no one holding any page buffer ? safe to release page buffers */
aw_bool_t aw_lffs_buf_is_all_free (struct aw_lffs_device *dev);

/** \brief are all page buffer marked with #LFFS_BUF_EMPTY ? */
aw_bool_t aw_lffs_buf_is_all_empty (struct aw_lffs_device *dev);

/** \brief mark all page buffer as #LFFS_BUF_EMPTY */
aw_err_t aw_lffs_buf_set_all_empty (struct aw_lffs_device *dev);

/** \brief clone a page buffer */
aw_lffs_buf_t * aw_lffs_clone_buf_get (struct aw_lffs_device *dev,
                                       aw_lffs_buf_t         *buf);

/** \brief release a cloned page buffer, call in pair with #aw_lffs_buf_Clone */
aw_err_t aw_lffs_clone_buf_put (aw_lffs_device_t *dev, aw_lffs_buf_t *buf);

/** \brief showing page buffers info, for debug only */
void aw_lffs_buf_inspect (aw_lffs_device_t *dev);

#ifdef __cplusplus
}
#endif


#endif  /* _AW_LFFS_BUF_H */

/* end of file */


