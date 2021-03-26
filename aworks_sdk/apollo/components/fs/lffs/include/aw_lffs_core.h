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
 * \brief aw_lffs_core.h
 *
 * \internal
 * \par modification history:
 * - 170923, vih, first implementation.
 * \endinternal
 */

#ifndef _AW_LFFS_CORE_H_
#define _AW_LFFS_CORE_H_

#include "aw_lffs_config.h"

#ifdef __cplusplus
extern "C"{
#endif

/** \brief typdef */
typedef struct aw_lffs_block_info       aw_lffs_block_info_t;
typedef struct aw_lffs_device           aw_lffs_device_t;
typedef struct aw_lffs_pool             aw_lffs_pool_t;
typedef struct aw_lffs_buf              aw_lffs_buf_t;
typedef struct aw_lffs_tree_node        aw_lffs_tree_node_t;
typedef struct aw_lffs_tree             aw_lffs_tree_t;
typedef struct aw_lffs_mem_allocator    aw_lffs_mem_allocator_t;
typedef struct aw_lffs_storage_attr     aw_lffs_storage_attr_t;
typedef struct aw_lffs_tag_store        aw_lffs_tag_store_t;
typedef struct aw_lffs_tags             aw_lffs_tags_t;
typedef struct aw_lffs_object           aw_lffs_object_t;
typedef struct aw_lffs_file_info        aw_lffs_file_info_t;
typedef struct aw_lffs_object_info      aw_lffs_object_info_t;

#ifdef __cplusplus
}
#endif


#endif

/* end of file */
