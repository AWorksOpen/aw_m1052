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


#ifndef _AW_LFFS_FLASH_H_
#define _AW_LFFS_FLASH_H_


#ifdef __cplusplus
extern "C"{
#endif

#include "aw_lffs_core.h"


/** flash operation return code */
#define AW_LFFS_FLASH_NO_ERR       0       /**< \brief no error */

/** \brief bit-flip found, but corrected by ECC */
#define AW_LFFS_FLASH_ECC_OK       1       

/** \brief page spare area is not sealed properly (only for ReadPageWithLayout()) */
#define AW_LFFS_FLASH_NOT_SEALED   2       
#define AW_LFFS_FLASH_IO_ERR       -1      /**< \brief I/O error */
#define AW_LFFS_FLASH_ECC_FAIL     -2      /**< \brief ECC failed */
#define AW_LFFS_FLASH_BAD_BLK      -3      /**< \brief bad block */
#define AW_LFFS_FLASH_CRC_ERR      -4      /**< \brief CRC failed */
#define AW_LFFS_FLASH_UNKNOWN_ERR  -100    /**< \brief unkown error? */

#define AW_LFFS_FLASH_HAVE_ERR(e)      ((e) < 0)

#if defined(AW_LFFS_CONFIG_BAD_BLOCK_POLICY_STRICT)
# define AW_LFFS_FLASH_IS_BAD_BLOCK(e) \
    ((e) == AW_LFFS_FLASH_ECC_FAIL || (e) == AW_LFFS_FLASH_ECC_OK || \
     (e) == AW_LFFS_FLASH_BAD_BLK || (e) == AW_LFFS_FLASH_CRC_ERR)
#else
# define AW_LFFS_FLASH_IS_BAD_BLOCK(e) \
    ((e) == AW_LFFS_FLASH_ECC_FAIL || (e) == AW_LFFS_FLASH_BAD_BLK || \
     (e) == AW_LFFS_FLASH_CRC_ERR)
#endif



/** defines for page info (data length and data sum) */
#define AW_LFFS_PAGE_INFO_CLEAN    0xFFFFFFFF
#define AW_LFFS_PAGE_INFO_IOERR    0xDEADFFFF
#define AW_LFFS_PAGE_GET_LEN(info) (info & 0xFFFF)
#define AW_LFFS_PAGE_GET_DSUM(info) (info >> 16)
#define AW_LFFS_PAGE_MAKE_INFO(d_len, d_sum) ((d_sum << 16) | d_len)

/**
 * \struct lffs_StorageAttrSt
 * \brief lffs device storage attribute, provide by nand specific file
 */
struct aw_lffs_storage_attr {
    uint32_t total_blocks;        /**< \brief total blocks in this chip */
    
    /** \brief page data size (physical page data size, e.g. 512) */
    uint16_t page_data_size;      
    uint16_t pages_per_block;     /**< \brief pages per block */
    
    /** \brief page spare size (physical page spare size, e.g. 16) */
    uint8_t  spare_size;          
    uint8_t  block_status_offs; /**< \brief block status byte offset in spare */

    void *_private;         /**< \brief private data for storage attribute */
};


/**
 * \struct lffs_FlashOpsSt
 * \brief low level flash operations, should be implement in flash driver
 */
typedef struct aw_lffs_flash_ops {
    /**
     * Initilize flash driver, called once when LFFS initilize partition
     *
     * \return 0 if no error, return -1 when failed.
     *
     * \note This function is optional.
     */
    int (*pfn_flash_init)(aw_lffs_device_t *dev);

    /**
     * Release flash driver, called once when LFFS unmount partition
     *
     * \return 0 if no error, return -1 when failed.
     *
     * \note This function is optional.
     */
    int (*pfn_flash_release)(aw_lffs_device_t *dev);

    /**
     * Read a full nand page, driver do the layout.
     *
     * \param[out] ecc ecc of page data
     *   if ecc_opt is AW_LFFS_ECC_HW, flash driver must calculate and return
     *       ecc of data(if ecc != NULL).
     *   if ecc_opt is AW_LFFS_ECC_HW_AUTO, flash driver do ecc correction 
     *       before return data and flash driver do not need to return ecc.
     *   if ecc_opt is AW_LFFS_ECC_NONE or AW_LFFS_ECC_SOFT, flash driver do not
     *       need calculate data ecc and return ecc.
     *
     * \param[out] ecc_store ecc store on spare area
     *   if ecc_opt is AW_LFFS_ECC_NONE or AW_LFFS_ECC_HW_AUTO, do not need to 
     *       return ecc_store.
     *
     * \note flash driver must provide this function if layout_opt is 
     *       AW_LFFS_LAYOUT_FLASH.
     *       LFFS will use this function (if exist) prio to 'ReadPageSpare()'
     *
     * \return  #AW_LFFS_FLASH_NO_ERR: success
     *          #AW_LFFS_FLASH_IO_ERR: I/O error, expect retry ?
     *          #AW_LFFS_FLASH_ECC_FAIL: page data has flip bits and ecc correct 
     *              failed (when ecc_opt == AW_LFFS_ECC_HW_AUTO)
     *          #AW_LFFS_FLASH_ECC_OK: page data has flip bits and corrected by 
     *              ecc (when ecc_opt == AW_LFFS_ECC_HW_AUTO)
     *          #AW_LFFS_FLASH_BAD_BLK: if the block is a bad block (e.g., 
     *              the bad block mark byte is not 0xFF)
     *          #AW_LFFS_FLASH_NOT_SEALED: if the page spare is not sealed properly
     *
     * \note if data is NULL, do not return data; if ts is NULL, do not read tag; 
     *       if both data and ts are NULL,then read bad block mark and return 
     *       AW_LFFS_FLASH_BAD_BLK if bad block mark is not 0xFF.
     *
     * \note flash driver DO NOT need to do ecc correction for tag,
     *      LFFS will take care of tag ecc.
     */
    int (*pfn_page_read_with_layout)(aw_lffs_device_t     *dev,
                                      uint32_t             block,
                                      uint32_t             page,
                                      uint8_t*             data,
                                      int                  data_len,
                                      aw_lffs_tag_store_t *ts);


    /**
     * Write full page, flash driver do the layout for spare area.
     *
     * \note if layout_opt is AW_LFFS_LAYOUT_FLASH or ecc_opt is AW_LFFS_ECC_HW/
     *       AW_LFFS_ECC_HW_AUTO, flash driver MUST implement this function. 
     *       LFFS will use this function (if provided) prio to 'WritePage()'     
     *
     * \param[in] ecc ecc of data. if ecc_opt is AW_LFFS_ECC_SOFT and this 
     *            function is implemented, LFFS will calculate page data ecc and
     *            pass it to WritePageWithLayout().
     *            if ecc_opt is AW_LFFS_ECC_NONE/AW_LFFS_ECC_HW/
     *                AW_LFFS_ECC_HW_AUTO, LFFS pass ecc = NULL.
     *
     * \note If data == NULL && ts == NULL, driver should mark this block as a 
     *       'bad block'.
     *
     * \return  #AW_LFFS_FLASH_NO_ERR: success
     *          #AW_LFFS_FLASH_IO_ERR: I/O error, expect retry ?
     *          #AW_LFFS_FLASH_BAD_BLK: a bad block detected.
     */
    int (*pfn_page_write_with_layout)(aw_lffs_device_t *dev,
                                       uint32_t block,
                                       uint32_t page,
                                       const uint8_t *data,
                                       int data_len,
                                       const aw_lffs_tag_store_t *ts);

    /**
     * Check block status.
     *
     * \note flash driver may maintain a bad block table to speed up bad block 
     *       checking or it will require one or two read spare I/O to check 
     *       block status.
     *
     * \note if this function is not implented by driver, 
     *       LFFS check the block_status byte in spare.
     *
     * \return 1 if it's a bad block, 0 if it's not.
     */
    int (*pfn_block_is_bad)(aw_lffs_device_t *dev, uint32_t block);

    /**
     * Mark a new bad block.
     *
     * \note if this function is not implemented, LFFS mark bad block by call 
     *       'WritePage()/WritePageWithLayout()'
     *       with: data == NULL && spare == NULL && ts == NULL.
     *
     * \return 0 if success, otherwise return -1.
     */
    int (*pfn_bad_block_mark)(aw_lffs_device_t *dev, uint32_t block);

    /**
     * Erase a block, driver MUST implement this function.
     *
     * \return  #AW_LFFS_FLASH_NO_ERR: success
     *          #AW_LFFS_FLASH_IO_ERR: I/O error, expect retry ?
     *          #AW_LFFS_FLASH_BAD_BLK: a bad block detected.
     */
    int (*pfn_block_erase)(aw_lffs_device_t *dev, uint32_t block);

    /**
     * Check if the block pages are all clean.
     *
     * \note if this function is not implemented, LFFS will read all pages and 
     *       verify that all bits are set to 1'
     *
     * \return 0 if all pages are clean, otherwise return -1.
     */
    int (*pfn_erased_block_check)(aw_lffs_device_t *dev, uint32_t block);
} aw_lffs_flash_ops_t;

/** \brief read page spare and fill to tag */
int aw_lffs_flash_page_tag_read (aw_lffs_device_t *dev, 
                                 int               block, 
                                 int               page, 
                                 aw_lffs_tags_t   *tag);

/** \brief read page data to page buf and do ECC correct */
int aw_lffs_flash_page_read (aw_lffs_device_t *dev, 
                             int               block, 
                             int               page, 
                             aw_lffs_buf_t    *buf, 
                             aw_bool_t         skip_ecc);

/** \brief write page data and spare */
int aw_lffs_flash_page_combine_write (aw_lffs_device_t *dev, 
                                      int               block, 
                                      int               page, 
                                      aw_lffs_buf_t    *buf,
                                      aw_lffs_tags_t   *tag);

/** \brief Mark this block as bad block */
int aw_lffs_flash_bad_block_mark (aw_lffs_device_t *dev, int block);

/** \brief Is this block a bad block ? */
aw_bool_t aw_lffs_flash_is_bad_block (aw_lffs_device_t *dev, int block);

/** \brief Erase flash block */
int aw_lffs_flash_block_erase (aw_lffs_device_t *dev, int block);

/* check if the block pages are all clean */
aw_err_t aw_lffs_flash_block_is_erased (aw_lffs_device_t *dev, int block);

/**
 * get page head info
 *
 * \return #AW_LFFS_PAGE_INFO_IOERR if I/O error, otherwise return page info
 */
uint32_t aw_lffs_flash_page_info_get (aw_lffs_device_t *dev, int block, int page);

/** \brief load lffs_FileInfo from flash storage */
aw_err_t aw_lffs_flash_file_info_read (aw_lffs_device_t    *dev, 
                                       int                  block, 
                                       int                  page,
                                       aw_lffs_file_info_t *info);
/**
 * \brief Initialize LFFS flash interface
 */
aw_err_t aw_lffs_flash_interface_init (aw_lffs_device_t *dev);

/**
 * \brief Release LFFS flash interface
 */
aw_err_t aw_lffs_flash_interface_uninit (aw_lffs_device_t *dev);

#ifdef __cplusplus
}
#endif
#endif /* _AW_LFFS_FLASH_H_ */

/* end of file */



