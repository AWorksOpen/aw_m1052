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
 * \brief SD CARD driver head file
 *
 * define SD CARD driver data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 14-11-14  deo, first implementation
 * - 1.00 17-01-06  xdn, code refactoring
 * \endinternal
 */


#ifndef __AWBL_SDCARD_H
#define __AWBL_SDCARD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "fs/aw_blk_dev.h"
#include "awbl_sdiobus.h"
#include "mtd/aw_mtd.h"
#include "aw_hotplug_event.h"

#define AWBL_SDCARD_NAME    "awbl_sdcard_driver"



/** \brief SD CARD struct */
typedef struct awbl_sdcard_dev {
    struct awbl_sdio_mem      *p_sd_mem;
    struct aw_list_head        node;
    struct aw_blk_dev          bd_dev;
    aw_bool_t                  removed;
    AW_MUTEX_DECL (            mutex);
    struct aw_refcnt           ref;

    struct awbl_sdio_mem_card_info *p_mem_info;
} awbl_sdcard_dev_t;



/** \brief SD card event */
struct awbl_sdcard_event {
    struct aw_list_head    node;
    const char            *p_name;
    void                 (*pfn_cb)(int event, void *arg);
    void                  *p_arg;
#define AW_SDCARD_EVT_INSTALL   0x01
#define AW_SDCARD_EVT_REMOVE    0x02
};



/**
 * \brief sdcard driver register
 */
void awbl_sdcard_drv_register (void);

/**
 * \brief write/read sdcard sector
 */
aw_err_t awbl_sdcard_rw_sector (struct awbl_sdcard_dev *p_sd,
                                uint32_t                sect_no,
                                uint8_t                *p_data,
                                uint32_t                sect_cnt,
                                aw_bool_t               read);

/**
 * \brief SD open
 * \param name   name
 */
struct awbl_sdcard_dev *awbl_sdcard_open (const char *p_name);

/**
 * \brief SD card close
 * \param name   name
 */
aw_err_t awbl_sdcard_close (const char *p_name);

/**
 * \brief SD card is insert ?
 * \param name   name
 */
aw_bool_t awbl_sdcard_is_insert (const char *name);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */
