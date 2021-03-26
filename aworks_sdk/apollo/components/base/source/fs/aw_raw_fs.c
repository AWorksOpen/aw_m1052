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
 * \brief raw block device file system.
 *
 * \internal
 * \par modification history:
 * - 1.00 17-02-28  deo, first implementation
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_assert.h"
#include "aw_mem.h"
#include "fs/aw_raw_fs.h"
#include "fs/aw_fs_type.h"
#include "io/aw_fcntl.h"
#include "aw_pool.h"
#include "string.h"



#define __RAWFS_TIMEOUT   AW_WAIT_FOREVER

#define __RAWFS_TRACE(info) \
            do { \
                AW_INFOF(("%s line %d: ", __FILE__, __LINE__)); \
                AW_INFOF(info); \
            } while (0)

/*******************************************************************************
    local variables
*******************************************************************************/
static struct aw_pool    __g_vol_pool;
static struct aw_pool    __g_fil_pool;
AW_MUTEX_DECL_STATIC(    __g_fs_lock);




/******************************************************************************/
static struct aw_raw_fs_volume *__volume_alloc (void)
{
    struct aw_raw_fs_volume *p_vol;

    p_vol = (struct aw_raw_fs_volume *)aw_pool_item_get(&__g_vol_pool);
    if (p_vol) {
        (void)memset(p_vol, 0, sizeof(*p_vol));
    }
    return p_vol;
}




/******************************************************************************/
static void __volume_free (struct aw_raw_fs_volume *p_vol)
{
    int  ret;
    ret = aw_pool_item_return(&__g_vol_pool, p_vol);
    aw_assert(0 == ret);
}




/******************************************************************************/
static struct aw_raw_fs_file *__file_alloc (void)
{
    struct aw_raw_fs_file *p_fil;

    p_fil = (struct aw_raw_fs_file *)aw_pool_item_get(&__g_fil_pool);
    if (p_fil) {
        (void)memset(p_fil, 0, sizeof(*p_fil));
    }
    return p_fil;
}




/******************************************************************************/
static void __file_free (struct aw_raw_fs_file *p_fil)
{
    int  ret;
    ret = aw_pool_item_return(&__g_fil_pool, p_fil);
    aw_assert(0 == ret);
}




/******************************************************************************/
static void __raw_fs_block_xfer_done (struct aw_blk_arp *p_arp)
{
    (void)aw_semb_give((aw_semb_id_t)p_arp->priv);
}




/******************************************************************************/
static int __disk_xfer (struct aw_raw_fs_file   *p_fil,
                        const void              *p_buf,
                        uint_t                   blk_no,
                        uint_t                   nblks,
                        uint_t                   flags)
{
    struct aw_blk_arp  arp;
    int                ret;
    uint8_t            retry = 5;
    uint_t             bsize;

    ret = aw_blk_dev_ioctl(p_fil->p_vol->p_bd,
                           AW_BD_GET_BSIZE,
                           &bsize);
    if (ret != AW_OK) {
        return ret;
    }

__retry:
    AW_SEMB_TAKE(p_fil->semb, AW_SEM_NO_WAIT);
    arp.p_dev    = p_fil->p_vol->p_bd;
    arp.blk_no   = blk_no;
    arp.nbytes   = bsize * nblks;
    arp.p_data   = (void *)p_buf;
    arp.resid    = 0;
    arp.error    = AW_OK;
    arp.flags    = flags;
    arp.priv     = p_fil->semb_id;
    arp.pfn_done = __raw_fs_block_xfer_done;
    arp.p_next   = NULL;

    ret = aw_blk_dev_areq(p_fil->p_vol->p_bd, &arp);
    AW_SEMB_TAKE(p_fil->semb, AW_SEM_WAIT_FOREVER);

    if ((ret == AW_OK) && (arp.error != AW_OK)) {
        ret = arp.error;
    }

    if (ret != AW_OK) {
        __RAWFS_TRACE(("block device transfer failed:%d, retry %d\n", ret, retry));
        if ((ret != -AW_ENODEV) && (retry--)) {
            goto __retry;
        }
    }

    return ret;
}




/******************************************************************************/
static void *__raw_fs_open (struct aw_iodev *p_dev,
                            const char      *path,
                            int              oflag,
                            mode_t           mode,
                            int             *err)
{
    struct aw_raw_fs_volume *p_vol;
    struct aw_raw_fs_file   *p_fil = NULL;

    /* check for open of other than raw volume (no-null filename) */
    if ((NULL == path) || (EOS != path[0])) {
        return NULL;
    }

    p_vol = AW_CONTAINER_OF(p_dev,
                            struct aw_raw_fs_volume,
                            iodev);

    p_fil = __file_alloc();
    if (p_fil == NULL) {
        *err = -AW_ENOMEM;
        return NULL;
    }
    memset(p_fil,0,sizeof(*p_fil));
    
    p_fil->p_vol   = p_vol;
    p_fil->oflags  = oflag;
    p_fil->semb_id = NULL;

    p_fil->p_buf = aw_mem_alloc(p_vol->bsize);
    if (p_fil->p_buf == NULL) {
        goto __failed;
    }

    p_fil->semb_id = AW_SEMB_INIT(p_fil->semb, 0, AW_SEM_Q_PRIORITY);
    if (p_fil->semb_id == NULL) {
        goto __failed;
    }

    if (AW_MUTEX_INIT(p_fil->lock, AW_SEM_Q_PRIORITY) == NULL) {
        goto __failed;
    }

    *err = AW_OK;
    return p_fil;

__failed:
    if (p_fil) {
        if (p_fil->p_buf == NULL) {
            aw_mem_free(p_fil->p_buf);
        }
        if (p_fil->semb_id) {
            AW_SEMB_TERMINATE(p_fil->semb);
        }
        __file_free(p_fil);
    }
    *err = -AW_ENOMEM;
    return NULL;
}




/******************************************************************************/
static int __raw_fs_close (void *pfd)
{
    struct aw_raw_fs_file   *p_fil;
    int                      ret;

    p_fil = (struct aw_raw_fs_file *)pfd;

    ret = aw_blk_dev_ioctl(p_fil->p_vol->p_bd, AW_BD_SYNC, NULL);
    if (ret != AW_OK) {
        __RAWFS_TRACE(("block device SYNC failed %d.\n", ret));
    }

    if (p_fil->p_buf == NULL) {
        aw_mem_free(p_fil->p_buf);
    }

    if (p_fil->semb_id) {
        AW_SEMB_TERMINATE(p_fil->semb);
    }

    AW_MUTEX_TERMINATE(p_fil->lock);

    __file_free(p_fil);

    return AW_OK;
}




/******************************************************************************/
static ssize_t __raw_fs_read (void *pfd, void *buf, size_t nbyte)
{
    struct aw_raw_fs_file   *p_fil;
    struct aw_raw_fs_volume *p_vol;
    int                      ret;
    uint_t                   offs, blk_no, nblks;

    p_fil = (struct aw_raw_fs_file *)pfd;
    p_vol = p_fil->p_vol;

    if ((NULL == p_fil) || (p_vol == NULL)) {
        return -AW_EINVAL;
    }

    /* make sure the file isn't opened in write-only mode */
    if ((p_fil->oflags & O_ACCMODE) == O_WRONLY) {
        return -AW_EPERM;
    }

    offs   = p_fil->offset % p_vol->bsize;
    blk_no = p_fil->offset / p_vol->bsize;

    AW_MUTEX_LOCK(p_fil->lock, AW_SEM_WAIT_FOREVER);

    if (offs) {
        ret = __disk_xfer(p_fil,
                          p_fil->p_buf,
                          blk_no,
                          1,
                          AW_BD_ARQ_READ);
        if (ret != AW_OK) {
            goto __exit;
        }

        ret = p_vol->bsize - offs;
        memcpy(buf, (uint8_t *)p_fil->p_buf + offs, ret);
        buf    = (uint8_t *)buf + ret;
        nbyte -= ret;
        blk_no++;
    }

    if (nbyte) {
        nblks  = nbyte / p_vol->bsize;

        ret = __disk_xfer(p_fil,
                          buf,
                          blk_no,
                          nblks,
                          AW_BD_ARQ_READ);
        if (ret != AW_OK) {
            goto __exit;
        }

        buf     = (uint8_t *)buf + p_vol->bsize * nblks;
        nbyte  -= p_vol->bsize * nblks;
        blk_no += nblks;
    }

    if (nbyte) {
        ret = __disk_xfer(p_fil,
                          p_fil->p_buf,
                          blk_no,
                          1,
                          AW_BD_ARQ_READ);
        if (ret != AW_OK) {
            goto __exit;
        }

        memcpy(buf, p_fil->p_buf, nbyte);
    }

__exit:
    AW_MUTEX_UNLOCK(p_fil->lock);
    return ret;
}




/******************************************************************************/
static ssize_t __raw_fs_write (void *pfd, const void *buf, size_t nbyte)
{
    struct aw_raw_fs_file   *p_fil;
    struct aw_raw_fs_volume *p_vol;
    int                      ret;
    uint_t                   offs, blk_no, nblks;

    p_fil = (struct aw_raw_fs_file *)pfd;
    p_vol = p_fil->p_vol;

    if ((NULL == p_fil) || (p_vol == NULL)) {
        return -AW_EINVAL;
    }

    /* make sure the file isn't opened in read-only mode */
    if ((p_fil->oflags & O_ACCMODE) == O_RDONLY) {
        return -AW_EPERM;
    }

    offs   = p_fil->offset % p_vol->bsize;
    blk_no = p_fil->offset / p_vol->bsize;

    AW_MUTEX_LOCK(p_fil->lock, AW_SEM_WAIT_FOREVER);

    if (offs) {
        ret = __disk_xfer(p_fil,
                          p_fil->p_buf,
                          blk_no,
                          1,
                          AW_BD_ARQ_READ);
        if (ret != AW_OK) {
            goto __exit;
        }

        ret = p_vol->bsize - offs;
        memcpy((uint8_t *)p_fil->p_buf + offs, buf, ret);

        ret = __disk_xfer(p_fil,
                          p_fil->p_buf,
                          blk_no,
                          1,
                          AW_BD_ARQ_WRITE);
        if (ret != AW_OK) {
            goto __exit;
        }

        buf    = (uint8_t *)buf + ret;
        nbyte -= ret;
        blk_no++;
    }

    if (nbyte) {
        nblks  = nbyte / p_vol->bsize;

        ret = __disk_xfer(p_fil,
                          buf,
                          blk_no,
                          nblks,
                          AW_BD_ARQ_WRITE);
        if (ret != AW_OK) {
            goto __exit;
        }

        buf     = (uint8_t *)buf + p_vol->bsize * nblks;
        nbyte  -= p_vol->bsize * nblks;
        blk_no += nblks;
    }

    if (nbyte) {
        ret = __disk_xfer(p_fil,
                          p_fil->p_buf,
                          blk_no,
                          1,
                          AW_BD_ARQ_READ);
        if (ret != AW_OK) {
            goto __exit;
        }

        memcpy((uint8_t *)p_fil->p_buf, buf, nbyte);

        ret = __disk_xfer(p_fil,
                          p_fil->p_buf,
                          blk_no,
                          1,
                          AW_BD_ARQ_WRITE);
        if (ret != AW_OK) {
            goto __exit;
        }
    }

__exit:
    AW_MUTEX_UNLOCK(p_fil->lock);
    return ret;
}




/******************************************************************************/
static int __raw_fs_ioctl (void *pfd, int cmd, void *arg)
{
    return AW_OK;
}




/******************************************************************************/
static void __raw_fs_release (struct aw_iodev *p_dev)
{
    struct aw_raw_fs_volume *p_vol = AW_CONTAINER_OF(p_dev,
                                                     struct aw_raw_fs_volume,
                                                     iodev);
    (void)__volume_free(p_vol);
}




/******************************************************************************/
static void *__raw_fs_dev_attach (const char *vol, const char *dev)
{
    struct aw_raw_fs_volume *p_vol = NULL;
    struct aw_blk_dev       *p_bd = NULL;
    int                      ret;

    /* lock */
    ret = AW_MUTEX_LOCK(__g_fs_lock, __RAWFS_TIMEOUT);
    if (ret != AW_OK) {
        return NULL;
    }

    /* get block device */
    p_bd = aw_blk_dev_get(dev);
    if (p_bd == NULL) {
        goto __failed;
    }

    /* get a volume and register it */
    p_vol = __volume_alloc();
    if (p_vol == NULL) {
        __RAWFS_TRACE(("allocate volume failed.\n"));
        goto __failed;
    }

    ret = aw_blk_dev_ioctl(p_vol->p_bd, AW_BD_GET_BSIZE, &p_vol->bsize);
    if (ret != AW_OK) {
        goto __failed;
    }

    ret = aw_blk_dev_ioctl(p_vol->p_bd, AW_BD_GET_BLKS, &p_vol->nblks);
    if (ret != AW_OK) {
        goto __failed;
    }

    p_vol->p_bd = p_bd;

    /* add the FAT volume to I/O system */
    ret = aw_iodev_create(&p_vol->iodev, vol);
    if (ret != AW_OK) {
        __RAWFS_TRACE(("%s add to I/O failed.\n", vol));
        goto __failed;
    }

    p_vol->iodev.pfn_ent_open    = __raw_fs_open;
    p_vol->iodev.pfn_ent_close   = __raw_fs_close;
    p_vol->iodev.pfn_ent_read    = __raw_fs_read;
    p_vol->iodev.pfn_ent_write   = __raw_fs_write;
    p_vol->iodev.pfn_ent_ioctrl  = __raw_fs_ioctl;
    p_vol->iodev.pfn_dev_release = __raw_fs_release;

    goto __exit;

__failed:
    if (p_vol) {
        __volume_free(p_vol);
    }

    if (p_bd) {
        /* put block device */
        ret = aw_blk_dev_put(p_bd);
        if (ret != AW_OK) {
            __RAWFS_TRACE(("put block device failed: %d.\n", ret));
        }
    }
    p_vol = NULL;

__exit:
    AW_MUTEX_UNLOCK(__g_fs_lock);
    return p_vol;
}




/******************************************************************************/
static int __raw_fs_dev_detach (void *vol)
{
    struct aw_raw_fs_volume *p_vol = (struct aw_raw_fs_volume *)vol;
    int                      ret;

    /* lock */
    ret = AW_MUTEX_LOCK(__g_fs_lock, __RAWFS_TIMEOUT);
    if (ret != AW_OK) {
        return ret;
    }

    /* put block device */
    ret = aw_blk_dev_put(p_vol->p_bd);
    if (ret != AW_OK) {
        __RAWFS_TRACE(("%s put block device failed: %d.\n",
                       __FUNCTION__,
                       ret));
    }
    p_vol->p_bd = NULL;

    /* remove the volume from I/O system */
    ret = aw_iodev_destroy(&p_vol->iodev);
    if (ret != AW_OK) {
        __RAWFS_TRACE(("%s delete volume from I/O system failed %d.\n",
                       __FUNCTION__,
                       ret));
    }

    AW_MUTEX_UNLOCK(__g_fs_lock);
    return AW_OK;
}




/******************************************************************************/
int aw_raw_fs_init (void         *p_vol_mem,
                    unsigned int  vmem_size,
                    void         *p_fil_mem,
                    unsigned int  fmem_size)
{
    static struct aw_fs_type fs;

    if (aw_pool_init(&__g_vol_pool,
                     p_vol_mem,
                     vmem_size,
                     sizeof(struct aw_raw_fs_volume)) == NULL) {
        return -AW_ENOENT;
    }

    if (aw_pool_init(&__g_fil_pool,
                     p_fil_mem,
                     fmem_size,
                     sizeof(struct aw_raw_fs_file)) == NULL) {
        return -AW_ENOENT;
    }

    if (AW_MUTEX_INIT(__g_fs_lock, AW_SEM_Q_PRIORITY) == NULL) {
        return -AW_ENOMEM;
    }

    /* register FAT file system */
    fs.fs_dev_attach = __raw_fs_dev_attach;
    fs.fs_dev_detach = __raw_fs_dev_detach;
    fs.fs_make_fs    = NULL;
    fs.fs_name       = "rawfs";
    return aw_fs_type_register(&fs);
}


