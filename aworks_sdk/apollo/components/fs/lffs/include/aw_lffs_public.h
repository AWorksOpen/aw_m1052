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
 * \brief public data structures for lffs
 *
 * \internal
 * \par modification history:
 * - 170923, vih, first implementation.
 * \endinternal
 */

#ifndef _AW_LFFS_PUBLIC_H_
#define _AW_LFFS_PUBLIC_H_


#ifdef __cplusplus
extern "C"{
#endif

#include "aw_lffs_core.h"

#ifndef AW_NELEMENTS
#define AW_NELEMENTS(ar)  (sizeof(ar) / sizeof(ar[0]))
#endif

#ifndef offsetof
#define offsetof(T, x) ((size_t) &((T *)0)->x)
#endif

#ifndef container_of
#define container_of(p, T, x) ((T *)((char *)(p) - offsetof(T,x)))
#endif

/**
 * \def AW_LFFS_CONFIG_LFFS_DBG_SHOW_LINE_NUM
 * \brief show function and line number in debug message
 */
#define AW_LFFS_CONFIG_LFFS_DBG_SHOW_LINE_NUM

/**
 * \def MAX_FILENAME_LENGTH
 * \note Be careful: it's part of the physical format 
 *       (see: aw_lffs_file_info_tSt.name)!!DO NOT CHANGE IT AFTER FILE SYSTEM 
 *       IS FORMATED!!
 */
#define AW_LFFS_MAX_FILENAME_LENGTH         128

/** \brief 8-bits attr goes to lffs_dirent::d_type */

/** \brief attribute for directory */
#define AW_LFFS_FILE_ATTR_DIR       (1 << 7)    

/** \brief writable */
#define AW_LFFS_FILE_ATTR_WRITE     (1 << 0)    


/**
 * \structure aw_lffs_file_info_tSt
 * \brief file/dir entry info in physical storage format
 */
struct aw_lffs_file_info {
    uint32_t attr;               /**< \brief file/dir attribute */
    uint32_t create_time;
    uint32_t last_modify;
    uint32_t access;
    uint32_t reserved;
    uint32_t name_len;           /**< \brief length of file/dir name */
    char name[AW_LFFS_MAX_FILENAME_LENGTH];
};


/**
 * \struct lffs_ObjectInfoSt
 * \brief object info
 */
struct aw_lffs_object_info {
    aw_lffs_file_info_t info;
    uint32_t            len;                /**< \brief length of file */
    uint16_t            serial;             /**< \brief object serial num */
};


/**
 * \struct lffs_TagStoreSt
 * \brief lffs tag, 12 bytes, will be store in page spare area.
 */
struct aw_lffs_tag_store {
    uint32_t valid:8;        /**< \brief 0: valid, 1: invalid */
    uint32_t dirty:1;        /**< \brief 0: dirty, 1: clear */
    
    /*
     *< \brief block type: #AW_LFFS_TYPE_DIR, #AW_LFFS_TYPE_FILE, 
     *  #AW_LFFS_TYPE_DATA 
     */
    uint32_t type:2;         
    uint32_t block_ts:2;     /**< \brief time stamp of block; */
    uint32_t data_len:12;    /**< \brief length of page data */
    uint32_t reserved0:7;

    uint32_t serial:14;      /**< \brief serial number */
    uint32_t parent:10;      /**< \brief parent's serial number */
    uint32_t page_id:6;      /**< \brief page id */
    uint32_t reserved1:2;

    uint32_t reserved:4;     /**< \brief reserved, for LFFS2 */
    uint32_t tag_ecc:12;     /**< \brief tag ECC */
    uint32_t bad_verify:1;
    uint32_t reserved2:15;
};



#define AW_LFFS_TAG_ECC_DEFAULT (0x5BE) /**< \brief 12-bit '1' */


/**
 * \struct aw_lffs_tags_tSt
 */
struct aw_lffs_tags {
    struct aw_lffs_tag_store s;   /**< \brief store must be the first member */

    /** \brief data_sum for file or dir name */
    uint16_t data_sum;

    /** \brief internal used */
    uint8_t seal_byte;         /**< \brief seal byte. */
};

/**
 * \struct aw_lffs_mini_header
 * \brief the mini header resides on the head of page data
 */
typedef struct aw_lffs_mini_header {
    uint8_t status;
    uint8_t reserved;
    uint16_t crc;
} aw_lffs_mini_header_t;

#if (AW_LFFS_CONFIG_ENABLE_INFO_TABLE == 1)
typedef enum {
    AW_LFFS_MTB_TAG_CLR     = 0,
    AW_LFFS_MTB_TAG_HEADER  = 0xFF,
    AW_LFFS_MTB_TAG_BODY    = 0X55,
} aw_lffs_mtb_tag_stat_t;

struct aw_lffs_mtb_info_table_tag {
    aw_lffs_mtb_tag_stat_t  stat;
    uint16_t                len;
    uint32_t                flag;
#define AW_LFFS_MTB_INFO_TABLE_FLAG    0x7D5EF17D
};
#endif /* AW_LFFS_CONFIG_ENABLE_INFO_TABLE == 1 */

/** \brief aw_lffs_tags_tSt.valid */
#define AW_LFFS_TAG_VALID       0xBE
#define AW_LFFS_TAG_INVALID     1

/** \brief aw_lffs_tags_tSt.dirty */
#define AW_LFFS_TAG_DIRTY       0
#define AW_LFFS_TAG_CLEAR       1

/** \brief tag is dirty if first 4 bytes not all 0xFF */
#define AW_LFFS_TAG_IS_DIRTY(tag) (*((uint32_t *) &((tag)->s)) != 0xFFFFFFFF)    
#define AW_LFFS_TAG_IS_VALID(tag) ((tag)->s.valid == AW_LFFS_TAG_VALID)
#define AW_LFFS_TAG_IS_SEALED(tag) ((tag)->seal_byte != 0xFF)

#define AW_LFFS_TAG_IS_GOOD(tag) \
    (AW_LFFS_TAG_IS_SEALED(tag) && AW_LFFS_TAG_IS_VALID(tag))

#define AW_LFFS_TAG_VALID_BIT(tag) (tag)->s.valid
#define AW_LFFS_TAG_DIRTY_BIT(tag) (tag)->s.dirty
#define AW_LFFS_TAG_SERIAL(tag) (tag)->s.serial
#define AW_LFFS_TAG_PARENT(tag) (tag)->s.parent
#define AW_LFFS_TAG_PAGE_ID(tag) (tag)->s.page_id
#define AW_LFFS_TAG_DATA_LEN(tag) (tag)->s.data_len
#define AW_LFFS_TAG_TYPE(tag) (tag)->s.type
#define AW_LFFS_TAG_BLOCK_TS(tag) (tag)->s.block_ts
#define AW_LFFS_SEAL_TAG(tag) (tag)->seal_byte = 0

int aw_lffs_first_block_time_stamp_get (void);
int aw_lffs_next_block_time_stamp_get (int prev);
aw_bool_t aw_lffs_is_src_newer_than_obj (int src, int obj);


/********************************** debug & error *************************************/
#define AW_LFFS_TENDSTR "\n"

/** \breif 消息类型 */
#define AW_LFFS_MSG_ALWAYS          0x00000001
#define AW_LFFS_MSG_NORMAL          0x00000002
#define AW_LFFS_MSG_DETAIL          0x00000004
#define AW_LFFS_MSG_IO              0x00000008
#define AW_LFFS_MSG_ASSERT          0x00000010
#define AW_LFFS_MSG_ALLOCATE        0x00000020
#define AW_LFFS_MSG_GC              0x00000040
#define AW_LFFS_MSG_BLOCK_INFO      0x00000080
#define AW_LFFS_MSG_BAD_BLOCK       0x00000100
#define AW_LFFS_MSG_UTILS           0x00000200
#define AW_LFFS_MSG_TREE            0x00000400
#define AW_LFFS_MSG_BUF             0x00000800
#define AW_LFFS_MSG_FLASH           0x00001000
#define AW_LFFS_MSG_POOL            0x00002000
#define AW_LFFS_MSG_MTB             0x00004000
#define AW_LFFS_MSG_INIT            0x00008000
#define AW_LFFS_MSG_FS              0x00010000
#define AW_LFFS_MSG_FLASH_RW        0x00020000
#define AW_LFFS_MSG_MTD             0x00040000

#define AW_LFFS_MSG_UNDEF           0X20000000
#define AW_LFFS_MSG_ERROR           0x40000000
#define AW_LFFS_MSG_BUG             0x80000000

//#define AW_LFFS_MSG_SCAN            0x00000008
//#define AW_LFFS_MSG_BAD_BLOCKS      0x00000010
//#define AW_LFFS_MSG_ERASE           0x00000020
//#define AW_LFFS_MSG_GC              0x00000040
//#define AW_LFFS_MSG_WRITE           0x00000080
//#define AW_LFFS_MSG_TRACING         0x00000100
//#define AW_LFFS_MSG_DELETION        0x00000200
//#define AW_LFFS_MSG_BUFFERS         0x00000400
//#define AW_LFFS_MSG_NANDACCESS      0x00000800
//#define AW_LFFS_MSG_GC_DETAIL       0x00001000
//#define AW_LFFS_MSG_SCAN_DEBUG      0x00002000
//#define AW_LFFS_MSG_MTD             0x00004000

//
//#define AW_LFFS_MSG_VERIFY          0x00010000
//#define AW_LFFS_MSG_VERIFY_NAND     0x00020000
//#define AW_LFFS_MSG_VERIFY_FULL     0x00040000
//#define AW_LFFS_MSG_VERIFY_ALL      0x000f0000
//
//#define AW_LFFS_MSG_SYNC            0x00100000
//#define AW_LFFS_MSG_BACKGROUND      0x00200000
//#define AW_LFFS_MSG_LOCK            0x00400000
//#define AW_LFFS_MSG_MOUNT           0x00800000
//
//#define AW_LFFS_MSG_ERROR           0x40000000
//#define AW_LFFS_MSG_BUG             0x80000000

#define AW_LFFS_MSG_COLOR_RED       0X00
#define AW_LFFS_MSG_COLOR_DEFAULT   0X01


struct aw_lffs_debug;
aw_err_t aw_lffs_debug_init (struct aw_lffs_debug *ops, uint32_t mask);
void aw_lffs_debug_mask_set (uint32_t mask);
void aw_lffs_debug_message (uint32_t mask, 
                            const char *prefix, 
                            const char *suffix, 
                            int         line, 
                            const char *errFmt, ...);
void aw_lffs_assert_call (const char *file, int line, const char *msg, ...);
void aw_lffs_debug_color_set (int color);

//#define _COMPILER_DO_NOT_SUPPORT_MACRO_VALIST_REPLACE_

#ifdef _COMPILER_DO_NOT_SUPPORT_MACRO_VALIST_REPLACE_

void aw_lffs_msg_raw(uint32_t mask, const char *fmt, ...);
void aw_lffs_msg(uint32_t mask, const char *fmt, ...);
void aw_lffs_err_msg(uint32_t mask, const char *fmt, ...);
aw_bool_t aw_lffs_assert(aw_bool_t expr, const char *fmt, ...);

#else
#define aw_lffs_msg_raw(mask, fmt, ...)  \
    aw_lffs_debug_message(mask, NULL, NULL, -1, fmt, ##__VA_ARGS__)

#define aw_lffs_msg(mask, fmt, ...)  \
    aw_lffs_debug_message(mask, PFX, AW_LFFS_TENDSTR, __LINE__, fmt, ##__VA_ARGS__)

#if 0 
#define aw_lffs_err_msg(mask, fmt, ...)  \
    do {\
        if ((mask) & g_aw_lffs_trace_mask) {\
            aw_lffs_debug_color_set(AW_LFFS_MSG_COLOR_RED);\
            aw_lffs_msg_raw(1, " \n%s", PFX);\
            aw_lffs_debug_color_set(AW_LFFS_MSG_COLOR_DEFAULT);\
            aw_lffs_msg_raw(1, fmt, ##__VA_ARGS__); \
            aw_lffs_msg_raw(1, ".\n%sat file %s"\
                               ".\n%sat line %d in %s()\n\n", \
                                  PFX, __FILE__, \
                                  PFX, __LINE__, \
                                  __FUNCTION__); \
        }\
    } while(0)
#endif 
#define aw_lffs_err_msg(mask, fmt, ...)  \
    do {\
        if ((mask) & g_aw_lffs_trace_mask) {\
            aw_lffs_debug_color_set(AW_LFFS_MSG_COLOR_RED);\
            aw_lffs_msg_raw(1, "%s(%d): ", PFX, __LINE__);\
            aw_lffs_debug_color_set(AW_LFFS_MSG_COLOR_DEFAULT);\
            aw_lffs_msg_raw(1, fmt, ##__VA_ARGS__); \
            aw_lffs_msg_raw(1, "\n"); \
        }\
    } while(0)

#define aw_lffs_assert(expr, msg, ...)  \
        ((expr) ? AW_TRUE : \
        (aw_lffs_assert_call(__FILE__, __LINE__, msg, ## __VA_ARGS__), AW_FALSE))

#endif

extern  uint32_t g_aw_lffs_trace_mask;


/********************************** NAND **********************************************/
/* NAND flash specific file must implement these interface */
aw_err_t aw_lffs_page_spare_load (aw_lffs_device_t *dev, 
                                  int               block, 
                                  int               page, 
                                  aw_lffs_tags_t   *tag);
aw_err_t aw_lffs_page_spare_write (aw_lffs_device_t *dev, 
                                   int               block, 
                                   int               page,
                                   aw_lffs_tags_t   *tag);
aw_err_t aw_lffs_page_valid_make (aw_lffs_device_t *dev, 
                                  int               block, 
                                  int               page, 
                                  aw_lffs_tags_t   *tag);
aw_bool_t aw_lffs_block_is_bad (aw_lffs_device_t *dev, aw_lffs_block_info_t *bc);

/********************************** Public defines *****************************/
/**
 * \def AW_LFFS_ALL_PAGES
 * \brief AW_LFFS_ALL_PAGES if this value presented, that means the objects are 
 *  all pages in the block
 */
#define AW_LFFS_ALL_PAGES (0xffff)

/**
 * \def AW_LFFS_INVALID_PAGE
 * \brief macro for invalid page number
 */
#define AW_LFFS_INVALID_PAGE   (0xfffe)

/**
 * \def AW_LFFS_INVALID_BLOCK
 * \brief macro for invalid block number
 */
#define AW_LFFS_INVALID_BLOCK  (0xfffe)

aw_err_t aw_lffs_new_block (aw_lffs_device_t *dev, 
                            uint16_t          block, 
                            aw_lffs_tags_t   *tag, 
                            aw_lffs_buf_t    *buf);
                            
aw_err_t aw_lffs_block_recover (aw_lffs_device_t     *dev, 
                                aw_lffs_block_info_t *old,
                                uint16_t             newBlock);
                                
aw_err_t aw_lffs_page_recover (aw_lffs_device_t *dev,
                               aw_lffs_block_info_t *bc,
                               uint16_t oldPage,
                               uint16_t newPage,
                               aw_lffs_buf_t *buf);
                               
int aw_lffs_free_page_find (aw_lffs_device_t *dev, aw_lffs_block_info_t *bc);

uint16_t aw_lffs_best_page_find (aw_lffs_device_t     *dev,
                                 aw_lffs_block_info_t *bc, 
                                 uint16_t              page);
                                 
uint16_t aw_lffs_first_free_page_find (aw_lffs_device_t     *dev, 
                                       aw_lffs_block_info_t *bc, 
                                       uint16_t              pageFrom);
                                       
uint16_t aw_lffs_valid_page_find (aw_lffs_device_t     *dev, 
                                  aw_lffs_block_info_t *bc,
                                  uint16_t              page);

uint8_t aw_lffs_sum8_make (const void *p, int len);

uint16_t aw_lffs_sum16_make (const void *p, int len);

aw_err_t aw_lffs_new_file_create (aw_lffs_device_t     *dev,
                                  uint16_t              parent,
                                  uint16_t              serial, 
                                  aw_lffs_block_info_t *bc, 
                                  aw_lffs_file_info_t  *fi);

int aw_lffs_file_length_get (aw_lffs_device_t     *dev,
                             aw_lffs_block_info_t *bc,
                             uint8_t               type);
                             
aw_bool_t aw_lffs_page_is_erased (aw_lffs_device_t     *dev,
                                  aw_lffs_block_info_t *bc,
                                  uint16_t              page);
                                  
int aw_lffs_free_page_num_get (aw_lffs_device_t *dev, aw_lffs_block_info_t *bc);

aw_bool_t aw_lffs_data_block_regu_is_full (aw_lffs_device_t     *dev,
                                           aw_lffs_block_info_t *bc);
                                           
aw_bool_t aw_lffs_block_is_used (aw_lffs_device_t *dev, aw_lffs_block_info_t *bc);

aw_bool_t aw_lffs_block_pages_is_all_used (aw_lffs_device_t     *dev, 
                                           aw_lffs_block_info_t *bc);

int aw_lffs_block_time_stamp_get (aw_lffs_device_t     *dev, 
                                  aw_lffs_block_info_t *bc);

int aw_lffs_device_used_num_get (aw_lffs_device_t *dev);

int aw_lffs_device_free_num_get (aw_lffs_device_t *dev);

int aw_lffs_device_num_get (aw_lffs_device_t *dev);

aw_err_t aw_lffs_mini_header_load (aw_lffs_device_t           *dev, 
                                   int                         block, 
                                   uint16_t                    page, 
                                   struct aw_lffs_mini_header *header);


/* some functions from lffs_fd.c */
void aw_lffs_fd_signature_increase (void);
aw_err_t aw_lffs_dir_entry_init (void);
aw_err_t aw_lffs_dir_entry_uninit (void);
aw_lffs_pool_t * aw_lffs_dir_entry_pool_get (void);
int aw_lffs_dir_entry_all_put (aw_lffs_device_t *dev);

uint16_t aw_lffs_newest_valid_page_find (aw_lffs_device_t     *dev,
                                         aw_lffs_block_info_t *bc,
                                         uint16_t              page_id);
/************************************************************************/
/*  init functions                                                      */
/************************************************************************/
aw_err_t aw_lffs_device_init (aw_lffs_device_t *dev);
aw_err_t aw_lffs_device_uninit (aw_lffs_device_t *dev);

aw_err_t aw_lffs_file_system_objects_init (void);
aw_err_t aw_lffs_file_system_objects_uninit (void);


#ifdef __cplusplus
}
#endif
#endif  /* _LFFS_PUBLIC_H_ */

/* end of file */
