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

#ifndef _AW_LFFS_TREE_H_
#define _AW_LFFS_TREE_H_


#ifdef __cplusplus
extern "C"{
#endif

#include "aw_lffs_core.h"
#include "aw_lffs_pool.h"
#include "aw_lffs_hash.h"


#define AW_LFFS_TYPE_DIR         0
#define AW_LFFS_TYPE_FILE        1
#define AW_LFFS_TYPE_DATA        2
#define AW_LFFS_TYPE_RESV        3
#define AW_LFFS_TYPE_INVALID    0xFF

typedef struct aw_lffs_node_info {
    int         type;
    const char *name;
} aw_lffs_node_info_t;

#define AW_LFFS_TYPE_NAME_MAP { \
    {AW_LFFS_TYPE_DIR, "DIR"}, \
    {AW_LFFS_TYPE_FILE, "FILE"}, \
    {AW_LFFS_TYPE_DATA, "DATA"}, \
    {AW_LFFS_TYPE_RESV, "RESV"}, \
    {AW_LFFS_TYPE_INVALID, "INVALID"} \
}

struct aw_lffs_free {    /**< \brief 12 bytes */
    struct aw_list_head node;

    uint16_t            block;
    union {
        uint16_t serial;            /**< \brief for suspended block list */
        uint8_t  need_check;        /**< \brief for erased block list */
    } u;
};

struct aw_lffs_dir {        /**< \brief 8 bytes */
    uint16_t block;
    uint16_t checksum;    /**< \brief check sum of dir name */
    uint16_t parent;
    uint16_t serial;
};


struct aw_lffs_file {    /**< \brief 12 bytes */
    uint16_t parent;
    uint16_t serial;
    uint16_t block;
    uint16_t checksum;   /**< \brief check sum of file name */
    uint32_t len;        /**< \brief file length total */
};

struct aw_lffs_fdata {    /**< \brief 10 bytes */
    uint16_t block;
    uint32_t len;        /**< \brief file data length on this block */
    
    /** \brief note：parent在这里需要定义在serial后，用作hash表关键字 */
    uint16_t serial;   
    uint16_t parent;
};



/**< \brief LFFS aw_lffs_tree_node_t (14 or 16 bytes) */
struct aw_lffs_tree_node {
    union {
        struct aw_lffs_free         free;
        struct aw_lffs_dir          dir;
        struct aw_lffs_file         file;
        struct aw_lffs_fdata        data;
    } u;
    struct aw_lffs_hash_node        hash_node;

#if (AW_LFFS_CONFIG_NAME_HASH_TABLE == 1)
    struct aw_lffs_hash_node        name_hash_node;
#endif /* (AW_LFFS_CONFIG_NAME_HASH_TABLE == 1) */
};

/** \brief special index num of empty node. */
#define AW_LFFS_EMPTY_NODE 0xffff    

#define AW_LFFS_ROOT_DIR_SERIAL    0       /**< \brief serial num of root dir */

/** \brief maximum dir|file serial number (lffs_TagStore#parent: 10 bits) */
#define AW_LFFS_MAX_LFFS_FSN            0x3ff    

/** \brief maximum file data block serial numbers (lffs_TagStore#serial: 14 bits) */
#define AW_LFFS_MAX_LFFS_FDN            0x3fff    

/** \brief parent of ROOT ? kidding me ... */
#define AW_LFFS_PARENT_OF_ROOT            0xfffd    

/** \brief invalid serial num */
#define AW_LFFS_INVALID_LFFS_SERIAL        0xffff    

#define AW_LFFS_DIR_NODE_HASH_MASK        0x1f
#define AW_LFFS_DIR_NODE_ENTRY_LEN        (AW_LFFS_DIR_NODE_HASH_MASK + 1)

#define AW_LFFS_FILE_NODE_HASH_MASK        0x3f
#define AW_LFFS_FILE_NODE_ENTRY_LEN        (AW_LFFS_FILE_NODE_HASH_MASK + 1)

#define AW_LFFS_DATA_NODE_HASH_MASK        0x1ff
#define AW_LFFS_DATA_NODE_ENTRY_LEN        (AW_LFFS_DATA_NODE_HASH_MASK + 1)

#if (AW_LFFS_CONFIG_NAME_HASH_TABLE == 1)
#define AW_LFFS_NAME_HASH_MASK     0x3f
#define AW_LFFS_NAME_ENTRY_LEN     (AW_LFFS_NAME_HASH_MASK + 1)
#endif /* (AW_LFFS_CONFIG_NAME_HASH_TABLE == 1) */

#define AW_LFFS_GET_NODE_BY_INDEX(idx, pool)  \
    ((aw_lffs_tree_node_t *)aw_lffs_pool_buf_get_by_index(pool, idx))
#define AW_LFFS_GET_NODE_INDEX(p, pool)        \
    ((uint16_t)aw_lffs_pool_buf_index_get(pool, (void *) p))

#define AW_LFFS_GET_FILE_HASH(serial)            \
    (serial & AW_LFFS_FILE_NODE_HASH_MASK)
#define AW_LFFS_GET_DIR_HASH(serial)            \
    (serial & AW_LFFS_DIR_NODE_HASH_MASK)
#define AW_LFFS_GET_DATA_HASH(parent, serial)    \
    ((parent + serial) & AW_LFFS_DATA_NODE_HASH_MASK)


struct aw_lffs_tree {
    struct aw_list_head  erased_list;      /**< \brief erased block list */
    int                  erased_count;     /**< \brief erased block counter */

    /* 
     * \brief suspended block list, this is just a staging zone 
     * that prevent the serial number of the block be re-used.
     */
    struct aw_list_head  suspend_list;     
                                       
    struct aw_list_head  bad_list;         /**< \brief bad block list */
    int                  bad_count;        /**< \brief bad block counter */

/*    uint16_t dir_entry[AW_LFFS_DIR_NODE_ENTRY_LEN]; */
/*    uint16_t file_entry[AW_LFFS_FILE_NODE_ENTRY_LEN]; */
/*    uint16_t data_entry[AW_LFFS_DATA_NODE_ENTRY_LEN]; */
    uint16_t max_serial;

    struct aw_lffs_hash_table   dir_hash;
    struct aw_list_head         dir_hash_list[AW_LFFS_DIR_NODE_ENTRY_LEN];

    struct aw_lffs_hash_table   file_hash;
    struct aw_list_head         file_hash_list[AW_LFFS_FILE_NODE_ENTRY_LEN];

    struct aw_lffs_hash_table   data_hash;
    struct aw_list_head         data_hash_list[AW_LFFS_DATA_NODE_ENTRY_LEN];

#if (AW_LFFS_CONFIG_NAME_HASH_TABLE == 1)
    struct aw_lffs_hash_table   name_hash;
    struct aw_list_head         name_hash_list[AW_LFFS_NAME_ENTRY_LEN];
#endif /* (AW_LFFS_CONFIG_NAME_HASH_TABLE == 1) */
};


aw_err_t aw_lffs_tree_init (aw_lffs_device_t *dev);
aw_err_t aw_lffs_tree_uninit (aw_lffs_device_t *dev);
aw_err_t aw_lffs_tree_build (aw_lffs_device_t *dev);
uint16_t aw_lffs_free_serial_find (aw_lffs_device_t *dev);

aw_lffs_tree_node_t * aw_lffs_tree_node_find (aw_lffs_device_t *dev,
                                              uint8_t           type,
                                              uint16_t          parent,
                                              uint16_t          serial);

aw_lffs_tree_node_t * aw_lffs_file_node_find (aw_lffs_device_t *dev, 
                                              uint16_t          serial);
aw_lffs_tree_node_t * aw_lffs_file_node_find_by_parent (aw_lffs_device_t *dev,
                                                        uint16_t          parent);
aw_lffs_tree_node_t * aw_lffs_dir_node_find (aw_lffs_device_t *dev, 
                                             uint16_t          serial);
aw_lffs_tree_node_t * aw_lffs_dir_node_find_by_parent (aw_lffs_device_t *dev, 
                                                       uint16_t          parent);
aw_lffs_tree_node_t * aw_lffs_file_node_find_by_name (aw_lffs_device_t *dev, 
                                                      const char       *name, 
                                                      uint32_t          len, 
                                                      uint16_t          sum, 
                                                      uint16_t          parent);
aw_lffs_tree_node_t * aw_lffs_dir_node_find_by_name (aw_lffs_device_t *dev, 
                                                     const char       *name,
                                                     uint32_t          len,
                                                     uint16_t          sum, 
                                                     uint16_t          parent);

int aw_lffs_data_node_cnt (aw_lffs_device_t *dev, uint16_t parent);

aw_err_t aw_lffs_data_node_truncate (aw_lffs_device_t   *dev,
                                     uint16_t            fserial,
                                     uint16_t            start_fdn,
                                     int                 cnt);


aw_lffs_tree_node_t * aw_lffs_data_node_find (aw_lffs_device_t *dev, 
                                              uint16_t          parent, 
                                              uint16_t          serial);


aw_lffs_tree_node_t * aw_lffs_dir_node_find_by_block (aw_lffs_device_t *dev, 
                                                      uint16_t          block);
aw_lffs_tree_node_t * aw_lffs_file_node_find_by_block (aw_lffs_device_t *dev,
                                                       uint16_t          block);
aw_lffs_tree_node_t * aw_lffs_data_node_find_by_block (aw_lffs_device_t *dev, 
                                                       uint16_t          block);
aw_lffs_tree_node_t * aw_lffs_erased_node_find_by_block (aw_lffs_device_t *dev, 
                                                         uint16_t          block);
aw_lffs_tree_node_t * aw_lffs_bad_node_find_by_block (aw_lffs_device_t *dev,
                                                      uint16_t          block);

void aw_lffs_suspend_node_add (aw_lffs_device_t *dev, aw_lffs_tree_node_t *node);
aw_lffs_tree_node_t * aw_lffs_suspend_node_find (aw_lffs_device_t *dev,
                                                 uint16_t          serial);
void aw_lffs_suspend_node_remove (aw_lffs_device_t *dev, aw_lffs_tree_node_t *node);

#define AW_LFFS_SEARCH_REGION_DIR        1
#define AW_LFFS_SEARCH_REGION_FILE        2
#define AW_LFFS_SEARCH_REGION_DATA        4
#define AW_LFFS_SEARCH_REGION_BAD        8
#define AW_LFFS_SEARCH_REGION_ERASED    16
aw_lffs_tree_node_t * aw_lffs_node_find_by_block (aw_lffs_device_t *dev,
                                                  uint16_t          block, 
                                                  int              *region);

aw_lffs_tree_node_t * aw_lffs_node_del_by_block (aw_lffs_device_t *dev,
                                                 uint16_t          block,
                                                 int              *region);

aw_bool_t aw_lffs_file_name_compare (aw_lffs_device_t *dev,
                                  const char *name,
                                  uint32_t len,
                                  uint16_t sum,
                                  aw_lffs_tree_node_t *node,
                                  int type);

aw_lffs_tree_node_t * aw_lffs_erased_node_get (aw_lffs_device_t *dev);
aw_lffs_tree_node_t * aw_lffs_erased_node_get_by_block (aw_lffs_device_t *dev, 
                                                        int               block);

aw_err_t aw_lffs_node_erase (aw_lffs_device_t *dev, aw_lffs_tree_node_t *node);

void aw_lffs_node_insert (aw_lffs_device_t    *dev, 
                          uint8_t              type, 
                          aw_lffs_tree_node_t *node);
                          
void aw_lffs_node_insert_to_erased_list_head (aw_lffs_device_t    *dev, 
                                              aw_lffs_tree_node_t *node);
                                              
void aw_lffs_node_insert_to_erased_list_tail (aw_lffs_device_t    *dev, 
                                              aw_lffs_tree_node_t *node);

void aw_lffs_node_insert_to_erased_list_tail_ex (aw_lffs_device_t    *dev, 
                                                 aw_lffs_tree_node_t *node, 
                                                 int                  need_check);
                                                 
void aw_lffs_node_insert_to_bad_block_list (aw_lffs_device_t    *dev, 
                                            aw_lffs_tree_node_t *node);

void aw_lffs_node_break_from_hash (aw_lffs_device_t    *dev, 
                                   uint8_t              type, 
                                   aw_lffs_tree_node_t *node);

void aw_lffs_node_block_set (uint8_t              type, 
                             aw_lffs_tree_node_t *node, 
                             uint16_t             block);


#if (AW_LFFS_CONFIG_NAME_HASH_TABLE == 1)
/**
 * \brief 通过名字的sum获取树结点，可以获取相同sum的下一个结点。
 *
 * \param[in]       dev           aw_lffs_device_t 设备指针
 * \param[in]       name_sum      名字的crc校验和
 * \param[in]       tree_node     当前树结点，通过该结点查找下一个结点，
 *                                为NULL则获取第一个结点
 *
 * \retval  NULL:  查找失败
 *          ELSE;  查找到的树结点指针
 */
aw_lffs_tree_node_t *aw_lffs_name_node_find (aw_lffs_device_t     *dev,
                                             uint16_t              name_sum,
                                             aw_lffs_tree_node_t  *tree_node);

aw_err_t aw_lffs_name_node_insert (aw_lffs_device_t     *dev,
                                   aw_lffs_tree_node_t  *node,
                                   uint16_t             *p_name_sum);

aw_err_t aw_lffs_name_node_del (aw_lffs_device_t     *dev,
                                aw_lffs_tree_node_t  *node);
#endif /* (AW_LFFS_CONFIG_NAME_HASH_TABLE == 1) */

#ifdef __cplusplus
}
#endif



#endif

/* end of file */
