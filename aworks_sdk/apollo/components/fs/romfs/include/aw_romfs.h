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
 * \brief AWorks RomFs adapter layer
 *
 * \internal
 * \par modification history:
 * - 1.00 18-03-06  sdy, first implementation
 * \endinternal
 */

#ifndef __AW_ROMFS_H
#define __AW_ROMFS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_romfs_ios
 * @{
 */
#include "apollo.h"
#include "aw_shell.h"
#include "aw_vdebug.h"
#include "aw_sem.h"
#include "io/aw_io_device.h"
#include "fs/aw_blk_dev.h"
#include "string.h"
#include "io/aw_fcntl.h"
#include "aw_romfs_dev.h"

#define ROMFS_MSG(info) \
            do { \
                aw_shell_color_set(AW_DBG_SHELL_IO, AW_SHELL_FCOLOR_RED); \
                AW_INFOF(("%s(%d): ", __ROMFS_FILE, __LINE__)); \
                aw_shell_color_set(AW_DBG_SHELL_IO, AW_SHELL_FCOLOR_DEFAULT); \
                AW_INFOF(info); \
            } while (0)

/** \brief RomFs volume descriptor */
struct aw_romfs_volume {
    
    /** \brief base on I/O device to AWorks */
    struct aw_iodev       io_dev;       
    struct aw_romfs_dev  *romfs_dev;    /**< \brief mount to block device */
    char                  mpoint[16];   /**< \brief mount point in ROMFS */
    uint32_t              file_count;   /**< \brief number of the file in use */
};

struct __romfs_file_info
{
    
    /** \brief pointer to volume descriptor */
    char     name[IMG_DATA_NAME_MAX];   
    int      attr;                      /**< \brief file type 1==dir */
    int      data_ptr;                  /**< \brief data offset addr */
    uint32_t size;                      /**< \brief file size */
};

/** \brief RomFs device file descriptor */
struct aw_romfs_file {
    
    /** \brief pointer to volume descriptor */
    struct aw_romfs_volume   *volume;       
    int                       oflags;       /**< \brief flags */
    struct __romfs_file_info  file_info;    /**< \brief file info */
    
    /**< \brief The current position in the stream. */
    volatile int              location;     
};


/**
 * \brief AWorks RomFs initialize
 *
 * \param[in]  p_vol_mem    : memory for volumes
 * \param[in]  vmem_size    : size of p_vol_mem
 * \param[in]  p_fil_mem    : memory for files
 * \param[in]  fmem_size    : size of p_fil_mem
 */
int aw_romfs_init (void         *p_vol_mem,
                   unsigned int  vmem_size,
                   void         *p_fil_mem,
                   unsigned int  fmem_size);


#ifdef __cplusplus
}
#endif


#endif /* __AW_ROMFS_H */

/* end of file */
