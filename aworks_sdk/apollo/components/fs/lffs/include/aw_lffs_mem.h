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

#ifndef _AW_LFFS_MEM_H_
#define _AW_LFFS_MEM_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "aw_lffs_core.h"
#include "aw_lffs_pool.h"

/** \brief lffs memory allocator */
struct aw_lffs_mem_allocator {
    
    /** \brief init memory allocator, setup buffer sizes */
    aw_err_t (*init)    (struct aw_lffs_device *dev);            
    
    /** \brief release memory allocator (for dynamic memory allocation) */
    aw_err_t (*release) (struct aw_lffs_device *dev);        

    /** \brief allocate memory (for dynamic memory allocation) */
    void *   (*malloc)  (struct aw_lffs_device *dev, unsigned int size); 
    
    /** \brief free memory (for dynamic memory allocation) */
    aw_err_t (*free)    (struct aw_lffs_device *dev, void *p);   

    void * blockinfo_pool_buf;          /**< \brief block info cache buffers */
    void * pagebuf_pool_buf;            /**< \brief page buffers */
    void * tree_nodes_pool_buf;         /**< \brief tree nodes buffer */
//    void * spare_pool_buf;            /**< \brief spare buffers */

    int blockinfo_pool_size;        /**< \brief block info cache buffers size */
    int pagebuf_pool_size;          /**< \brief page buffers size */
    int tree_nodes_pool_size;       /**< \brief tree nodes buffer size */
//    int spare_pool_size;          /**< \brief spare buffer pool size */

    aw_lffs_pool_t tree_pool;
//    aw_lffs_pool_t spare_pool;


    /*
     * \brief spare size consumed by LFFS, calculated by LFFS according to the 
     * layout informatio
     */
    int spare_data_size;                

    /** \brief for static memory allocator */
    char *buf_start;
    int buf_size;
    int pos;
};

void aw_lffs_mem_allocator_setup (aw_lffs_mem_allocator_t *allocator);

#ifdef __cplusplus
}
#endif


#endif

/* end of file */

