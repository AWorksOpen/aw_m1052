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
 * \brief RAM disk implementation with block device interface.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-06-16 deo, first implementation.
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_vdebug.h"

#include "fs/aw_ram_disk.h"
#include <string.h>



/******************************************************************************/
static int __bd_ioctl (struct aw_blk_dev *p_dev, int cmd, void *arg)
{
    int                 ret = AW_OK;
    struct aw_ram_disk *p_disk;

    p_disk = AW_CONTAINER_OF(p_dev, struct aw_ram_disk, bd);

    switch (cmd) {
    case AW_BD_SYNC:
        break;

    case AW_BD_GET_BASE_NAME:
        strcpy(arg, p_disk->name);
        break;

    case AW_BD_TEST:
        break;

    case AW_BD_STACK_COMPLETE:
        break;

    case AW_BD_GET_ATTRIBUTE:
        *(int*)arg = AW_BD_ARRT_REMOVABLE;
        break;

    case AW_BD_GET_BSIZE:
        *(uint32_t*)arg = p_disk->bsize;
        break;

    case AW_BD_GET_BLKS:
        *(uint32_t*)arg = p_disk->nblks;
        break;

    default:
        ret = -AW_ENOTSUP;
        break;
    }
    return ret;
}




/******************************************************************************/
static int __bd_areq (struct aw_blk_dev *p_dev,
                      struct aw_blk_arp *p_arp)
{
    struct aw_ram_disk *p_disk;
    uint_t              len;

    p_disk = AW_CONTAINER_OF(p_dev, struct aw_ram_disk, bd);

    /* iterate through the chain, running each bio as we get it */
    for (; p_arp != NULL; p_arp = p_arp->p_next) {

        if (p_arp->blk_no >= p_disk->nblks) {
            p_arp->error = -AW_ENOSPC;
            p_arp->resid = p_arp->nbytes;
        } else {
            len = min((p_disk->bsize * p_disk->nblks), p_arp->nbytes);

            if (p_arp->flags & AW_BD_ARQ_READ) {
                memcpy(p_arp->p_data,
                       &p_disk->p_mem[p_arp->blk_no * p_disk->bsize],
                       len);
            } else {
                memcpy(&p_disk->p_mem[p_arp->blk_no * p_disk->bsize],
                        p_arp->p_data,
                       len);
            }
            p_arp->error = AW_OK;
            p_arp->resid = p_arp->nbytes - len;
        }

        if (p_arp->pfn_done) {
            p_arp->pfn_done(p_arp);
        }
    }

    return AW_OK;
}

/******************************************************************************/
static void __bd_release (struct aw_blk_dev *p_dev)
{
    struct aw_ram_disk *p_disk;

    p_disk = AW_CONTAINER_OF(p_dev, struct aw_ram_disk, bd);

    if (p_disk->pfn_release) {
        p_disk->pfn_release(p_disk);
    }
}

/******************************************************************************/
int aw_ram_disk_create (struct aw_ram_disk *p_disk,
                        const char         *name,
                        uint_t              bsize,
                        void               *p_mem,
                        uint_t              size,
                        void (*pfn_release) (struct aw_ram_disk *p_disk))
{
    int  ret;

    if ((p_disk == NULL) ||
        (bsize == 0) ||
        (size == 0) ||
        (p_mem == NULL)) {
        return -AW_EINVAL;
    }

    p_disk->p_mem       = p_mem;
    p_disk->name        = name;
    p_disk->bsize       = bsize;
    p_disk->nblks       = size / bsize;
    p_disk->pfn_release = pfn_release;

    ret = aw_blk_dev_create(&p_disk->bd, name);
    if (ret != AW_OK) {
        return ret;
    }

    p_disk->bd.pfn_ioctl   = __bd_ioctl;
    p_disk->bd.pfn_areq    = __bd_areq;
    p_disk->bd.pfn_release = __bd_release;

    ret = aw_blk_dev_inject(&p_disk->bd);
    if (ret != AW_OK) {
        aw_blk_dev_destroy(&p_disk->bd);
    }

    return ret;
}

/******************************************************************************/
int aw_ram_disk_destroy (struct aw_ram_disk *p_disk)
{
    aw_blk_dev_eject(&p_disk->bd);
    return aw_blk_dev_destroy(&p_disk->bd);
}

/* end of file */
