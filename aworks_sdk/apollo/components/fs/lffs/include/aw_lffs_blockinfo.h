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
 * \brief block information cache system manipulations
 *
 * \internal
 * \par modification history:
 * - 170923, vih, first implementation.
 * \endinternal
 */

#ifndef _AW_LFFS_BLOCKINFO_H_
#define _AW_LFFS_BLOCKINFO_H_

#include "aw_lffs_core.h"
#include "aw_lffs_public.h"

#ifdef __cplusplus
extern "C"{
#endif
/**
 * \struct aw_lffs_page_spare
 * \brief this structure is for storing lffs tag and more.
 */

typedef struct aw_lffs_page_spare {
    struct aw_lffs_tags  tag;            /**< \brief page tag */
    int                  expired:1;
} aw_lffs_page_spare_t;


/**
 * \struct aw_lffs_block_info
 * \brief block information data. Block info is frequently accessed,
          LFFS use a cache system to speed up block info access.
 */
struct aw_lffs_block_info {
    struct aw_list_head        node;
    uint16_t                   block;     /**< \brief block number */
    struct aw_lffs_page_spare *spares;    /**< \brief page spare info array */
    
    /** \brief how many pages expired in this block ? */
    int                        expired_count;                 

    /*
     * \brief reference counter, it's safe to reuse this block memory 
     *  when the counter is 0. 
     */
    int                        ref_count;                       
};

/** \brief get tag from block info */
#if 0 
#define AW_LFFS_GET_TAG(bc, page) (&(bc)->spares[page].tag)
#endif 
#define aw_lffs_page_tag_get(bc, page) (&(bc)->spares[page].tag)


/** \brief initialize block info caches */
aw_err_t aw_lffs_block_info_cache_init (aw_lffs_device_t *dev, 
                                        int               maxCachedBlocks);

/** \brief release block info caches */
aw_err_t aw_lffs_block_info_cache_uninit (aw_lffs_device_t *dev);

/** \brief load page spare to block info cache */
aw_err_t aw_lffs_block_info_load (aw_lffs_device_t     *dev,
                                  aw_lffs_block_info_t *work,
                                  int                   page);

/** \brief find block info cache */
aw_lffs_block_info_t * aw_lffs_block_info_find (aw_lffs_device_t *dev,
                                                int               block);

/** \brief get block info cache, load it on demand */
aw_lffs_block_info_t * aw_lffs_block_info_get (aw_lffs_device_t *dev, int block);

/*
 * \brief put info cache back to pool,
 *  should be called with #aw_lffs_block_info_tGet in pairs. 
 */
void aw_lffs_block_info_put (aw_lffs_device_t *dev, aw_lffs_block_info_t *p);

/** \brief explicitly expire a block info cache */
void aw_lffs_block_info_expire (aw_lffs_device_t     *dev, 
                                aw_lffs_block_info_t *p, 
                                int                   page);

/** \brief no one hold any block info cache ? safe to release block info caches */
aw_bool_t aw_lffs_block_info_is_all_free (aw_lffs_device_t *dev);

/** \brief explicitly expire all block info caches */
void aw_lffs_block_info_all_expire (aw_lffs_device_t *dev);

/** \brief This will init block info cache for an erased block - all '0xFF' */
void aw_lffs_block_info_erase (aw_lffs_device_t *dev, aw_lffs_block_info_t *p);

#ifdef __cplusplus
}
#endif


#endif

/* end of file */
