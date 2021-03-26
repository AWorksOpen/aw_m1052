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
 * \brief FLASH device manage sub system
 *
 * \internal
 * \par modification history:
 * - 1.00 17-4-15  vih, first implementation.
 * \endinternal
 */

#include "ftl/aw_ftl_core.h"

#ifdef  __AW_FTL_DEBUG
#include "aw_shell.h"
#define __printf(fmt, ...)   \
    aw_shell_printf_ext(aw_dbg_shell_user(), fmt, ##__VA_ARGS__)
#else
#define __printf(fmt, ...)
#endif

/*******************************************************************************
  块设备驱动函数
*******************************************************************************/

/**
 * \brief 块设备io control
 */
static int __ftl_blk_ioctl (struct aw_blk_dev *dev, int cmd, void *arg)
{
    int ret = 0;

    aw_ftl_dev_t *p_ftl = AW_CONTAINER_OF(dev, aw_ftl_dev_t, bdev);

    if (NULL == dev) {
        return -AW_EINVAL;
    }

    switch (cmd) {

    case AW_BD_SOFT_EJECT:
    case AW_BD_HARD_EJECT:

        break;

    case AW_BD_GET_BASE_NAME:
        ret = aw_blk_dev_name(dev, arg);
        break;

    case AW_BD_GET_BSIZE:
        *(uint32_t *)arg = p_ftl->sector_size;
        break;

    case AW_BD_GET_BLKS:
        *(uint32_t *)arg = p_ftl->data_sector_total;
        break;

    case AW_BD_SYNC:
        break;

    case AW_BD_TEST:
        break;

    case AW_BD_STACK_COMPLETE:
        break;

    case AW_BD_GET_ATTRIBUTE:
        *(uint32_t *)arg = 0;
        break;

    default:
        ret = -AW_ENOTSUP;
        break;
    }

    return ret;
}


/**
 * \brief 块设备io传输
 */
static int __ftl_blk_xfr (struct aw_blk_dev *dev, struct aw_blk_arp *bio)
{
    int      i;
    uint16_t nblocks;
    aw_err_t ret = AW_OK;

    if (NULL == dev ||
        NULL == bio) {
        return -AW_EINVAL;
    }

    aw_ftl_dev_t *p_ftl = AW_CONTAINER_OF(dev, aw_ftl_dev_t, bdev);

    /* 遍历所有block_io */
    for (; bio != NULL; bio = bio->p_next) {
        nblocks = bio->nbytes / (1 << p_ftl->log2_sector_size);

        /* block_io起始扇区编号超出dev所拥有扇区数量范围 */
        if (bio->blk_no >= p_ftl->data_sector_total) {
            bio->resid = bio->nbytes;
            __printf("FTL BLK: %s xfr error: access block %lu out of range (%lu)\n",
                     dev->name,
                     bio->blk_no,
                     bio->nbytes);

            if (bio->pfn_done) {
                bio->pfn_done(bio);
            }
            continue;
        }

        /* 要写入的扇区编号超过了设备的扇区个数，截断要写入的扇区个数 */
        if (p_ftl->data_sector_total < bio->blk_no + nblocks) {
            nblocks = p_ftl->data_sector_total - bio->blk_no;
        }

        /* 重新计算要操作的字节数 */
        bio->resid = bio->nbytes - nblocks * (1 << p_ftl->log2_sector_size);

        if (0 == nblocks) { /* 没有能操作的扇区 */
            bio->resid = bio->nbytes;
        } else if (bio->flags == AW_BD_ARQ_READ) { /* 读扇区操作 */
            __printf("FTL BLK: read sector %d\n", bio->blk_no);

            for (i = 0; i < nblocks; i++) {
                AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_WAIT_FOREVER);
                ret = aw_ftl_sector_read(p_ftl,
                                         bio->blk_no + i,
                                         (uint8_t *)bio->p_data +
                                         (i << p_ftl->log2_sector_size));
                AW_MUTEX_UNLOCK(p_ftl->op_mutex);

                if (AW_OK != ret) {
                    break;
                }
            }
        } else if (bio->flags == AW_BD_ARQ_WRITE) { /* 写扇区操作 */
            __printf("FTL BLK: write sector %d\n", bio->blk_no);

            for (i = 0; i < nblocks; i++) {
                AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_WAIT_FOREVER);
                ret = aw_ftl_sector_write(p_ftl,
                                          bio->blk_no + i,
                                          (uint8_t *)bio->p_data +
                                          (i << p_ftl->log2_sector_size));
                AW_MUTEX_UNLOCK(p_ftl->op_mutex);

                if (AW_OK != ret) {
                    ret = -AW_EIO;
                    break;
                }
            }
        }
        if (bio->pfn_done) {
            bio->pfn_done(bio);
        }
    }

    return ret;
}

aw_local void __ftl_blk_release (struct aw_blk_dev *p_dev)
{
    if (NULL == p_dev) {
        return;
    }

    aw_ftl_dev_t *p_ftl = AW_CONTAINER_OF(p_dev, aw_ftl_dev_t, bdev);

    aw_ftl_dev_put(p_ftl);
    p_ftl->blk_reg = AW_FALSE;
}


aw_err_t aw_ftl_blk_register (const char *p_ftl_path)
{
    int ret;
    struct aw_ftl_dev   *p_ftl;

    p_ftl = aw_ftl_dev_get(p_ftl_path);
    if (!p_ftl) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_WAIT_FOREVER);
    if (p_ftl->blk_reg ) {
        /* 如果已经注册过了则将刚刚get到的ftl释放，并返回OK  */
        ret = AW_OK;
        goto __exit;
    }

    /* 连接上层块设备驱动     */
    ret = aw_blk_dev_create(&p_ftl->bdev, p_ftl_path);
    if (AW_OK != ret) {
        __printf("FTL ERROR: fail to attach block device %s (ret:%d)\n", p_ftl_path, ret);
        goto __exit;
    }

    ret = aw_blk_dev_inject(&p_ftl->bdev);
    if (-AW_EEXIST == ret) {
        ret = AW_OK;
    } else if (AW_OK != ret) {
        aw_blk_dev_destroy(&p_ftl->bdev);
        __printf("FTL ERROR: aw_blk_dev_inject %s failed (ret:%d)\n", p_ftl_path, ret);
        goto __exit;
    }

    p_ftl->bdev.pfn_ioctl   = __ftl_blk_ioctl;
    p_ftl->bdev.pfn_areq    = __ftl_blk_xfr;
    p_ftl->bdev.pfn_release = __ftl_blk_release;

    p_ftl->blk_reg = AW_TRUE;

__exit:
    AW_MUTEX_UNLOCK(p_ftl->op_mutex);
    aw_ftl_dev_put(p_ftl);
    return ret;
}

aw_err_t aw_ftl_blk_unregister (const char *p_ftl_path)
{
    int ret;
    struct aw_ftl_dev   *p_ftl;

    p_ftl = aw_ftl_dev_get(p_ftl_path);
    if (p_ftl == NULL) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_WAIT_FOREVER);
    if ( !p_ftl->blk_reg ) {
        ret = AW_OK;
        goto __exit;
    }

    p_ftl->blk_reg = AW_FALSE;

    ret = aw_blk_dev_eject(&p_ftl->bdev);
    if (ret != AW_OK) {
        goto __exit;
    }

    ret = aw_blk_dev_destroy(&p_ftl->bdev);
    if (ret != AW_OK) {
        goto __exit;
    }

__exit:
    AW_MUTEX_UNLOCK(p_ftl->op_mutex);
    aw_ftl_dev_put(p_ftl);
    return ret;
}
