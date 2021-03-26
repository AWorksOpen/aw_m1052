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
 * \brief FatFs AWorks adapter layer
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-07  deo, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_sem.h"
#include "aw_fatfs.h"
#include "aw_vdebug.h"

#include "diskio.h"
#include "ffconf.h"
#include "ff.h"
#include "string.h"



/******************************************************************************/
static void            *__gp_disk_num[FF_VOLUMES] = {0};
AW_MUTEX_DECL_STATIC(   __g_disk_num_lock);




/******************************************************************************/
static void __fatfs_block_xfer_done (struct aw_blk_arp *p_arp)
{
    (void)aw_semb_give((aw_semb_id_t)p_arp->priv);
}




/******************************************************************************/
static DRESULT __disk_xfer (struct aw_blk_dev *p_bd,
                            const BYTE        *buff,
                            DWORD              sector,
                            UINT               count,
                            aw_bool_t             read)
{
    struct aw_blk_arp  arp;
    int                ret;
    AW_SEMB_DECL(      bio_sem);
    aw_semb_id_t       semc_id = NULL;
    uint8_t            retry = 5;
    uint_t             bsize;

    ret = aw_blk_dev_ioctl(p_bd,
                           AW_BD_GET_BSIZE,
                           &bsize);
    if (ret != AW_OK) {
        return RES_PARERR;
    }

    memset(&bio_sem,0,sizeof(bio_sem));
    semc_id = AW_SEMB_INIT(bio_sem, 0, AW_SEM_Q_PRIORITY);
    if (semc_id == NULL) {
        return RES_ERROR;
    }

__retry:
    AW_SEMB_TAKE(bio_sem, AW_SEM_NO_WAIT);
    arp.p_dev    = p_bd;
    arp.blk_no   = sector;
    arp.nbytes   = bsize * count;
    arp.p_data   = (void *)buff;
    arp.resid    = 0;
    arp.error    = AW_OK;
    arp.flags    = read ? AW_BD_ARQ_READ : AW_BD_ARQ_WRITE;
    arp.priv     = semc_id;
    arp.pfn_done = __fatfs_block_xfer_done;
    arp.p_next   = NULL;

    ret = aw_blk_dev_areq(p_bd, &arp);
    if (ret == AW_OK) {
        AW_SEMB_TAKE(bio_sem, AW_SEM_WAIT_FOREVER);
        ret = arp.error;
    }

    if (ret != AW_OK) {
        AW_LOGF(("fat disk %s failed:%d, retry %d\n", read ? "read" : "write", 
                ret, retry));
        if ((ret != -AW_ENODEV) && (retry--)) {
            goto __retry;
        }
    }

    AW_SEMB_TERMINATE(bio_sem);

    return (ret != AW_OK) ? RES_ERROR : RES_OK;
}




/******************************************************************************/
int aw_fatfs_disk_lib_init (void)
{
    if (AW_MUTEX_INIT(__g_disk_num_lock, AW_SEM_Q_PRIORITY) == NULL) {
        return -AW_ENOMEM;
    }
    return AW_OK;
}




/******************************************************************************/
int aw_fatfs_disk_num_get (struct aw_blk_dev *p_bd)
{
    int  i;

    AW_MUTEX_LOCK(__g_disk_num_lock, AW_SEM_WAIT_FOREVER);

    for (i = 0; i < AW_NELEMENTS(__gp_disk_num); i++) {
        if (NULL == __gp_disk_num[i]) {
            __gp_disk_num[i] = p_bd;
            AW_MUTEX_UNLOCK(__g_disk_num_lock);
            return i;
        }
    }
    AW_MUTEX_UNLOCK(__g_disk_num_lock);
    return -AW_ENOSPC;
}




/******************************************************************************/
int aw_fatfs_disk_num_put (int disk)
{
    if (disk >= AW_NELEMENTS(__gp_disk_num)) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(__g_disk_num_lock, AW_SEM_WAIT_FOREVER);
    __gp_disk_num[disk] = NULL;
    AW_MUTEX_UNLOCK(__g_disk_num_lock);

    return AW_OK;
}




/******************************************************************************/
DSTATUS disk_initialize (BYTE pdrv)
{
    return disk_status(pdrv);
}




/******************************************************************************/
DSTATUS disk_status (BYTE pdrv)
{
    struct aw_blk_dev *p_bd;
    int                ret;

    if (pdrv >= AW_NELEMENTS(__gp_disk_num)) {
        return STA_NODISK;
    }

    p_bd = (struct aw_blk_dev *)__gp_disk_num[pdrv];

    if (p_bd == NULL) {
        return STA_NODISK;
    }

    ret = aw_blk_dev_ioctl(p_bd, AW_BD_TEST, NULL);
    if ((0 != ret) && (-AW_ENOTSUP != ret)) {
        return STA_NOINIT;
    }
    return 0;
}




/******************************************************************************/
DRESULT disk_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    struct aw_blk_dev *p_bd;

    if (pdrv >= AW_NELEMENTS(__gp_disk_num)) {
        return RES_PARERR;
    }

    p_bd = (struct aw_blk_dev *)__gp_disk_num[pdrv];
    if (p_bd == NULL) {
        return RES_PARERR;
    }

    return __disk_xfer(p_bd, buff, sector,  count, AW_TRUE);
}




/******************************************************************************/
DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    struct aw_blk_dev *p_bd;
    uint_t             tmp;

    if (pdrv >= AW_NELEMENTS(__gp_disk_num)) {
        return RES_PARERR;
    }

    p_bd = (struct aw_blk_dev *)__gp_disk_num[pdrv];
    if (p_bd == NULL) {
        return RES_PARERR;
    }

    (void)aw_blk_dev_ioctl(p_bd, AW_BD_GET_ATTRIBUTE, &tmp);
    if (tmp & AW_BD_ARRT_READ_ONLY) {
        return RES_WRPRT;
    }
    return __disk_xfer(p_bd, buff, sector,  count, AW_FALSE);
}




/******************************************************************************/
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
    struct aw_blk_dev *p_bd;
    uint_t             tmp;
    DRESULT            res;

    if (pdrv >= AW_NELEMENTS(__gp_disk_num)) {
        return RES_PARERR;
    }

    p_bd = (struct aw_blk_dev *)__gp_disk_num[pdrv];
    if (p_bd == NULL) {
        return RES_PARERR;
    }

    res = RES_OK;
    switch(cmd) {
    case CTRL_SYNC:
        /* Flush disk cache (for write functions) */
        (void)aw_blk_dev_ioctl(p_bd, AW_BD_GET_ATTRIBUTE, &tmp);
        if (tmp & AW_BD_ARRT_READ_ONLY) {
            res = RES_WRPRT;
        } else {
            (void)aw_blk_dev_ioctl(p_bd, AW_BD_SYNC, NULL);
        }
        break;

    case GET_SECTOR_COUNT:
        /* Get drive capacity in unit of sector (DWORD) */
        (void)aw_blk_dev_ioctl(p_bd, AW_BD_GET_BLKS, &tmp);
        *(DWORD *)buff = tmp;
        break;

    case GET_SECTOR_SIZE:
        /* Get sector size (for (_MAX_SS >= 1024)) */
        (void)aw_blk_dev_ioctl(p_bd, AW_BD_GET_BSIZE, &tmp);
        *(WORD *)buff = tmp;
        break;

    case GET_BLOCK_SIZE:
        /* Get erase block size in unit of sectors (DWORD)*/
        *(DWORD *)buff = 1;
        break;

    default:
        res = RES_PARERR;
        break;
    }
    return res;
}


/* end of file */
