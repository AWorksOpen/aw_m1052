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
 * \brief YAFFS adapter header file
 *
 * \internal
 * \par modification history:
 * - 1.00 2016-05-05  cyl, first implementation.
 * \endinternal
 */


#ifndef __AW_YAFFS_H
#define __AW_YAFFS_H


#ifdef __cplusplus
extern "C" {
#endif


#include "apollo.h"
#include "yaffsfs.h"
#include "yaffs_guts.h"
#include "io/aw_io_device.h"
#include "mtd/aw_mtd.h"


/** \brief YAFFS����Ϣ */
typedef struct aw_yaffs_volume {

    /** \brief IO�豸 */
    struct aw_iodev     io_dev;

    /** \brief yaffs�豸 */
    struct yaffs_dev    yfs_dev;

    /** \brief MTD�豸 */
    struct aw_mtd_info *mtd;

} aw_yaffs_volume_t;


/** \brief YAFFS file descriptor */
typedef struct aw_yaffs_file {

    struct aw_yaffs_volume  *volume; /**< \brief YAFFS volume */

    int                      oflags; /**< \brief flags */

    union {
        int                  fdesc;  /**< \brief YAFFS file descriptor */
        yaffs_DIR           *dir;    /**< \brief yaffs dir */
    };
    
    /** \brief file path */
    char                     path[YAFFS_MAX_NAME_LENGTH + 1]; 

} aw_yaffs_file_t;


/** \brief YAFFS�س�ʼ������ */
typedef struct aw_yaffs_init_param {

    /** \brief YAFFS volume�� */
    struct aw_yaffs_volume *vol_pool;

    /** \brief YAFFS�ļ������ */
    struct aw_yaffs_file   *fil_pool;

    /** \brief YAFFS volume�ش�С */
    unsigned int vol_pool_size;

    /** \brief YAFFS�ļ�����ش�С */
    unsigned int fil_pool_size;

} aw_yaffs_init_param_t;


int aw_yaffs_init (const struct aw_yaffs_init_param *init);
void aw_yaffs_mtd_init (struct aw_yaffs_volume *vol);
void aw_yaffs_mtd_erase (struct aw_yaffs_volume *vol);


#ifdef __cplusplus
}
#endif


#endif /* __AW_YAFFS_H */

/* end of file */
