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
 * \brief SD CARD driver source file
 *
 * define SD CARD driver
 *
 * \internal
 * \par modification history:
 * - 1.00 14-11-14  deo, first implementation
 * - 1.01 17-01-06  xdn, code refactoring
 * - 1.02 18-04-20  xdn, modify static device enum method
 * \endinternal
 */


/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_spinlock.h"
#include "aw_mem.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "driver/sdiocard/awbl_sdcard.h"
#include "string.h"
#include "aw_vdebug.h"
#include "stdio.h"
#include "aw_delay.h"

/*******************************************************************************
  macro define
*******************************************************************************/
#define __SD_TRACE(info)    AW_INFOF(info)
#define __SD_MUTEX_TIMEOUT  5000

#define __SDCARD_SPEED_CLASS    0x001F
#define __SDCARD_CLASS_0        0x0001
#define __SDCARD_CLASS_2        0x0002
#define __SDCARD_CLASS_4        0x0004
#define __SDCARD_CLASS_6        0x0008
#define __SDCARD_CLASS_10       0x0010

#define __SDCARD_VERSION        0x00E0
#define __SDCARD_SDV2X          0x0020
#define __SDCARD_SDV1X          0x0040
#define __SDCARD_MMC            0x0080

#define __SDCARD_CAPACITY_VER   0x0300
#define __SDCARD_SDSC_MMC       0x0100
#define __SDCARD_SDHC           0x0200
#define __SDCARD_SDXC           0x0400


#define __CSD_SPEC_VER_0      0
#define __CSD_SPEC_VER_1      1
#define __CSD_SPEC_VER_2      2
#define __CSD_SPEC_VER_3      3
#define __CSD_SPEC_VER_4      4


/* global variable */
aw_local struct aw_list_head            __gp_sd_list;
AW_MUTEX_DECL_STATIC(                   __gp_sd_mutex);


/**
 * \brief lock SD card
 */
aw_local void __sd_lock(void)
{
   AW_MUTEX_LOCK(__gp_sd_mutex, AW_SEM_WAIT_FOREVER);
}

/**
 * \brief unlock SD card
 */
aw_local void __sd_unlock(void)
{
    AW_MUTEX_UNLOCK(__gp_sd_mutex);
}


/******************************************************************************/
aw_err_t awbl_sdcard_rw_sector (struct awbl_sdcard_dev *p_sd,
                                uint32_t                sect_no,
                                uint8_t                *p_data,
                                uint32_t                sect_cnt,
                                aw_bool_t               read)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_sd->p_sd_mem->p_card);

    struct awbl_sdio_timeout_obj timeout;
    uint32_t                     addr;
    uint32_t                     status = 0;
    aw_err_t                     ret = AW_OK;

    if (p_sd->removed) {
        return -AW_ENODEV;
    }

    if ((p_sd->p_mem_info->attribute & __SDCARD_SDHC) ||
        (p_sd->p_mem_info->attribute & __SDCARD_SDXC)   ) {
        addr = sect_no;
    } else {
        addr = sect_no << 9;
    }


    if (p_info->mode != AWBL_SDIO_SPI_M) {

        awbl_sdio_timeout_set(&timeout, 5000);

        while (1) {
            if (awbl_sdio_timeout(&timeout)) {
                __SD_TRACE(("RW sector timeout.\r\n"));
                return -AW_ETIME;
            }

            if (p_sd->removed) {
                return -AW_ENODEV;
            }

            /* get status of the card */
            ret = aw_sdio_send_status(p_sd->p_sd_mem->p_card,
                                      p_sd->p_mem_info->rca,
                                      &status);
            if (ret != AW_OK){
                return ret;
            }

            /* device had ready for transmit */
            if ((status & AWBL_SD_R1_READY_FOR_DATA) &&
                (AWBL_SD_R1_CURRENT_STATE(status) == AWBL_SD_R1_STATE_TRAN)) {
                break;
            }

            /* device stand by, select it */
            if (AWBL_SD_R1_CURRENT_STATE(status) == AWBL_SD_R1_STATE_STBY) {
                ret = aw_sdio_select_card(p_sd->p_sd_mem->p_card,
                                          p_sd->p_mem_info->rca,
                                          1);
                if (ret != AW_OK){
                    return ret;
                }
                continue;
            }

            /* device in transmit state, stop it */
            if ((AWBL_SD_R1_CURRENT_STATE(status) == AWBL_SD_R1_STATE_DATA) ||
                (AWBL_SD_R1_CURRENT_STATE(status) == AWBL_SD_R1_STATE_RCV)) {
                ret = aw_sdio_stop_transmission(p_sd->p_sd_mem->p_card);
                if (ret != AW_OK){
                    return ret;
                }
            }
        }
    }

    if (read) {
        /* read sector */
        if (sect_cnt) {
            ret = aw_sdio_read_multiple_block(
                                p_sd->p_sd_mem->p_card,
                                addr,
                                p_data,
                                sect_cnt,
                                p_sd->p_mem_info->csd.sector_size,
                                AW_TRUE);
        } else {
            ret = aw_sdio_read_single_block(
                                p_sd->p_sd_mem->p_card,
                                addr,
                                p_data,
                                p_sd->p_mem_info->csd.sector_size);
        }

        if (ret != AW_OK) {
            __SD_TRACE(("read sector fail %d.\r\n", ret));
            return ret;
        }
    } else {
        /* write sector */
        if (sect_cnt) {
            ret = aw_sdio_write_multiple_block(
                                p_sd->p_sd_mem->p_card,
                                addr,
                                p_data,
                                sect_cnt,
                                p_sd->p_mem_info->csd.sector_size,
                                AW_TRUE);
        } else {
            ret = aw_sdio_write_single_block(
                                p_sd->p_sd_mem->p_card,
                                addr,
                                p_data,
                                p_sd->p_mem_info->csd.sector_size);
        }

        if (ret != AW_OK) {
            __SD_TRACE(("write sector fail %d.\r\n", ret));
            return ret;
        }

        status = 0;

        /* get result */
        awbl_sdio_timeout_set(&timeout, 5000);
        while (1) {
            if (awbl_sdio_timeout(&timeout)) {
                __SD_TRACE(("write sector timeout.\r\n"));
                return -AW_ETIME;
            }

            if (p_sd->removed) {
                return -AW_ENODEV;
            }

            /* get status of the card */
            ret = aw_sdio_send_status(p_sd->p_sd_mem->p_card,
                                      p_sd->p_mem_info->rca,
                                      &status);
            if (ret != AW_OK){
                return ret;
            }

            if (p_info->mode == AWBL_SDIO_SPI_M) {
                if ((status & (AWBL_SD_SPI_R2_ERROR | AWBL_SD_SPI_R2_CC_ERROR))) {
                    ret = -AW_EIO;
                }
                break;
            } else {
                if ((status & AWBL_SD_R1_READY_FOR_DATA) &&
                    (AWBL_SD_R1_CURRENT_STATE(status) != AWBL_SD_R1_STATE_PRG)) {
                    break;
                }
            }
        }
    }

    return ret;
}


aw_local void __sd_nouse_cb(struct aw_refcnt *p_ref)
{
    aw_err_t ret;
    struct awbl_sdcard_dev *p_sd = (struct awbl_sdcard_dev*)
                                    AW_CONTAINER_OF(p_ref,
                                                    struct awbl_sdcard_dev,
                                                    ref);
    AW_MUTEX_TERMINATE(p_sd->mutex);

    ret = aw_sdio_mem_refcnt_put(p_sd->p_sd_mem);

    aw_list_del(&p_sd->node);

    aw_mem_free(p_sd);
}

/**
 * \brief IO control for block device
 */
aw_local int __bd_ioctl (struct aw_blk_dev   *dev,
                         int                  cmd,
                         void                *arg)
{
    int                 ret = AW_OK;
    uint32_t            status;

    struct awbl_sdcard_dev *p_sd =AW_CONTAINER_OF(dev,
                                                  struct awbl_sdcard_dev,
                                                  bd_dev);

    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info,
                                     p_sd->p_sd_mem->p_card);


    /* lock device */
    ret = AW_MUTEX_LOCK(p_sd->mutex, __SD_MUTEX_TIMEOUT);
    if (ret != AW_OK) {
        return ret;
    }

    if (p_sd->removed || (p_sd->p_sd_mem == NULL)) {
        /* device had removed */
        AW_MUTEX_UNLOCK(p_sd->mutex);
        return -AW_ENODEV;
    }

    ret = AW_OK;

    switch (cmd) {
    case AW_BD_SYNC:
        break;

    case AW_BD_GET_BASE_NAME:
        strcpy(arg, p_sd->p_sd_mem->name);
        break;

    case AW_BD_TEST:
        /* get the status of the card */
        ret = aw_sdio_send_status(p_sd->p_sd_mem->p_card,
                                  p_sd->p_mem_info->rca,
                                  &status);
        if (ret == AW_OK) {
            if (p_info->mode == AWBL_SDIO_SPI_M) {
                if (((AWBL_SD_SPI_R1_ALL_ERROR<<8) |
                     AWBL_SD_SPI_R2_CC_ERROR |
                     AWBL_SD_SPI_R2_CARD_ECC_ERROR) & status) {
                    ret = -AW_EIO;
                }
            } else {
                if (AWBL_SD_R1_STATE_TRAN != AWBL_SD_R1_CURRENT_STATE(status)) {
                    ret = -AW_EIO;
                }
            }
        }
        break;

    case AW_BD_STACK_COMPLETE:
        break;

    case AW_BD_GET_ATTRIBUTE:
        *(int*)arg = AW_BD_ARRT_REMOVABLE;
        break;

    case AW_BD_GET_BSIZE:
        *(uint32_t*)arg = p_sd->p_mem_info->csd.sector_size;
        break;

    case AW_BD_GET_BLKS:
        *(uint32_t*)arg = p_sd->p_mem_info->csd.sector_cnt;
        break;

    default:
        ret = -AW_ENOTSUP;
        break;
    }

    AW_MUTEX_UNLOCK(p_sd->mutex);
    return ret;
}

/**
 * \brief transfer for block device
 */
aw_local int __bd_areq (struct aw_blk_dev *dev,
                        struct aw_blk_arp *p_arp)
{
    struct awbl_sdcard_dev *p_sd;
    int                     ret = AW_OK;
    uint_t                  nblks;

    p_sd = AW_CONTAINER_OF(dev,
                            struct awbl_sdcard_dev,
                            bd_dev);

    /* lock device */
    ret = AW_MUTEX_LOCK(p_sd->mutex, __SD_MUTEX_TIMEOUT);
    if (ret != AW_OK) {
        return ret;
    }

    if (p_sd->removed || (p_sd->p_sd_mem == NULL)) {
        /* device had removed */
        AW_MUTEX_UNLOCK(p_sd->mutex);
        return -AW_ENODEV;
    }

    /* iterate through the chain, running each bio as we get it */
    for (; p_arp != NULL; p_arp = p_arp->p_next) {

        if (p_arp->blk_no >= p_sd->p_mem_info->csd.sector_cnt) {
            /* out of range */
            p_arp->error = -AW_ENOSPC;
            p_arp->resid = p_arp->nbytes;
        } else {
            nblks = p_arp->nbytes / p_sd->p_mem_info->csd.sector_size;
            nblks = min(nblks, 
                        (p_sd->p_mem_info->csd.sector_cnt - p_arp->blk_no));

            if (nblks) {
                ret = awbl_sdcard_rw_sector(p_sd,
                                            p_arp->blk_no,
                                            p_arp->p_data,
                                            nblks,
                                            (p_arp->flags & AW_BD_ARQ_READ));
            }

            p_arp->error = ret;
            if (ret == AW_OK) {
                p_arp->resid = p_arp->nbytes - 
                              (nblks * p_sd->p_mem_info->csd.sector_size);
            } else {
                p_arp->resid = p_arp->nbytes;
            }
        }

        /* done */
        if (p_arp->pfn_done) {
            p_arp->pfn_done(p_arp);
        }
    }

    AW_MUTEX_UNLOCK(p_sd->mutex);
    return AW_OK;
}

/**
 * \brief dump for block device
 */
aw_local void __bd_release (struct aw_blk_dev   *dev)
{
    struct awbl_sdcard_dev  *p_sd   = NULL;
    aw_err_t                 ret;

    p_sd = AW_CONTAINER_OF(dev,
                            struct awbl_sdcard_dev,
                            bd_dev);

    ret = aw_refcnt_put(&p_sd->ref, __sd_nouse_cb);
}


/**
 * \brief SD card probe
 */
aw_local aw_bool_t __sd_drv_probe (struct awbl_dev *p_dev)
{
    return AW_TRUE;
}

/**
 * \brief device init1
 */
aw_local void __sd_init1 (struct awbl_dev *p_awdev)
{
    return ;
}

/**
 * \brief device init2
 */
aw_local void __sd_init2 (struct awbl_dev *p_awdev)
{
    aw_err_t                  ret;
    struct awbl_sdcard_dev   *p_sd   = NULL;
    struct awbl_sdio_mem     *p_card = NULL;

    p_card = AW_CONTAINER_OF(p_awdev, struct awbl_sdio_mem, super);

    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_host_info, p_card->p_card->p_host);

    awbl_sdio_dev_info_t *p_dev_info = (awbl_sdio_dev_info_t *)
                                        p_awdev->p_devhcf->p_devinfo;

    if (p_host_info->connect_type == AWBL_SDIO_CON_DEV_STATIC) {
        aw_sdio_static_memmory_enum(p_card);
        p_sd = (struct awbl_sdcard_dev *)p_dev_info->p_drv;
        p_sd->p_mem_info = &p_card->mem_info;

    } else if (p_host_info->connect_type == AWBL_SDIO_CON_DEV_DYNAMIC) {
        p_sd = aw_mem_alloc(sizeof(struct awbl_sdcard_dev));
        memset(p_sd, 0, sizeof(*p_sd));
        p_sd->p_sd_mem   = p_card;
        p_sd->p_mem_info = &p_card->mem_info;
        p_sd->removed = AW_FALSE;

        aw_refcnt_init(&p_sd->ref);
        ret = aw_sdio_mem_refcnt_get(p_card);

    } else {
        AW_INFOF(("SDIO_ERROR: sdcard_init2 error\r\n"));
        return;
    }

    p_card->p_drv = (void*)p_sd;

    AW_MUTEX_INIT(p_sd->mutex, AW_SEM_Q_PRIORITY);

    ret = aw_blk_dev_create(&p_sd->bd_dev, p_card->name);
    if (ret != AW_OK) {
        AW_MUTEX_TERMINATE(p_sd->mutex);
        __SD_TRACE(("block create failed %d.\n", ret));
        goto __failed;
    }

    p_sd->bd_dev.pfn_ioctl   = __bd_ioctl;
    p_sd->bd_dev.pfn_areq    = __bd_areq;
    p_sd->bd_dev.pfn_release = __bd_release;

    ret = aw_blk_dev_inject(&p_sd->bd_dev);
    if (ret != AW_OK) {
        __SD_TRACE(("block device inject failed %d.\n", ret));
    }

    ret = aw_refcnt_get(&p_sd->ref);

    /* lock device */
    ret = AW_MUTEX_LOCK(p_sd->mutex, __SD_MUTEX_TIMEOUT);
    if (ret != AW_OK) {
        return;
    }
    aw_list_add(&p_sd->node, &__gp_sd_list);
    AW_MUTEX_UNLOCK(p_sd->mutex);
    return;

__failed:
    __SD_TRACE(("SDcard initialize failed.\n"));
    if (p_sd) {
        aw_mem_free(p_sd);
    }
    return;
}


/**
 * \brief device connet
 */
aw_local void __sd_connect (struct awbl_dev *p_awdev)
{
    return ;
}

/**
 * \brief device type match
 */
aw_local aw_err_t __sd_match (struct awbl_dev *p_awdev, void *p_arg)
{
    AWBL_DEV_TO_MEM_DECL(p_memory, p_awdev);
    AWBL_SDIO_MEM_TO_CARD_DECL(p_card, p_memory);

    if (p_card->type == AWBL_SDIO_TYPE_SD ||
        p_card->type == AWBL_SDIO_TYPE_MMC) {
        return AW_OK;
    } else {
        return AW_ERROR;
    }

    return AW_OK;
}

/**
 * \brief delete card device
 */
aw_local aw_err_t __sd_remove (struct awbl_dev *p_awdev)
{
    struct awbl_sdcard_dev   *p_sd   = NULL;
    struct awbl_sdio_mem     *p_card = NULL;
    aw_err_t                  ret    = AW_OK;

    p_card   = AW_CONTAINER_OF(p_awdev, struct awbl_sdio_mem, super);
    p_sd = (struct awbl_sdcard_dev*)p_card->p_drv;

    /* lock device must sync */
    ret = AW_MUTEX_LOCK(p_sd->mutex, __SD_MUTEX_TIMEOUT);
    if (ret != AW_OK) {
        __SD_TRACE(("lock device failed %d when removing.\n", ret));
    }

    /* mark removed */
    p_sd->removed = AW_TRUE;

    if (ret == AW_OK) {
        AW_MUTEX_UNLOCK(p_sd->mutex);
    }

    /* eject block device from system */
    ret = aw_blk_dev_eject(&p_sd->bd_dev);
    if (ret != AW_OK) {
        __SD_TRACE(("block device eject failed %d. \n", ret));
    }

    /* destroy block device */
    ret = aw_blk_dev_destroy(&p_sd->bd_dev);
    if (ret != AW_OK) {
        __SD_TRACE(("block device destroy failed %d. \n", ret));
    }

    ret = aw_refcnt_put(&p_sd->ref, __sd_nouse_cb);

    return AW_OK;
}

/**
 * \brief driver method
 */
aw_local aw_const struct awbl_dev_method __g_sdcard_methods[] = {
        AWBL_METHOD(awbl_sdio_match_type,  __sd_match),
        AWBL_METHOD(awbl_drv_unlink,       __sd_remove),
        AWBL_METHOD_END
};

/**
 * \brief driver functions
 */
aw_local aw_const struct awbl_drvfuncs __g_sd_drvfuncs = {
        __sd_init1,
        __sd_init2,
        __sd_connect
};

/**
 * \brief driver information
 */
aw_local aw_const struct awbl_sdio_dev_drvinfo __g_sd_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_SDIO,                        /* bus_id */
        AWBL_SDCARD_NAME,                       /* p_drvname */
        &__g_sd_drvfuncs,                       /* p_busfuncs */
        __g_sdcard_methods,                     /* p_methods */
        __sd_drv_probe                          /* pfunc_drv_probe */
    },
    AWBL_SDIO_TYPE_SD,
    0
};

/******************************************************************************/
void awbl_sdcard_drv_register (void)
{
    aw_err_t ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_sd_drv_registration);

    if (ret == AW_OK) {
        aw_list_head_init(&__gp_sd_list);
        AW_MUTEX_INIT(__gp_sd_mutex, AW_SEM_Q_PRIORITY);
    } else {
        AW_INFOF(("__SDCARD driver register failed: %d \n", ret));
    }
    return;
}


/******************************************************************************/
struct awbl_sdcard_dev *awbl_sdcard_open (const char *name)
{
    aw_err_t ret;
    awbl_sdcard_dev_t *p_sdcard;

    /* search for all SDCARDs */
    __sd_lock();
    aw_list_for_each_entry(p_sdcard,
                           &__gp_sd_list,
                           awbl_sdcard_dev_t,
                           node) {
        if (strcmp(name, p_sdcard->p_sd_mem->name) == 0) {
            if (p_sdcard->removed) {
                return NULL;
            }
            /* found it */
            ret = aw_sdio_mem_refcnt_get(p_sdcard->p_sd_mem);
            __sd_unlock();
            return p_sdcard;
        }
    }
    __sd_unlock();
    return NULL;
}


/******************************************************************************/
aw_err_t awbl_sdcard_close (const char *name)
{
    aw_err_t ret;
    awbl_sdcard_dev_t *p_sdcard;

    /* search for all SDCARDs */
    __sd_lock();
    aw_list_for_each_entry(p_sdcard,
                           &__gp_sd_list,
                           awbl_sdcard_dev_t,
                           node) {
        if (strcmp(name, p_sdcard->p_sd_mem->name) == 0) {
            if (p_sdcard->removed) {
                return -AW_ENXIO;
            }
            /* found it */
            ret = aw_sdio_mem_refcnt_put(p_sdcard->p_sd_mem);
            __sd_unlock();
            return AW_OK;
        }
    }
    __sd_unlock();

    return -AW_ENXIO;
}



/******************************************************************************/
aw_bool_t awbl_sdcard_is_insert (const char *name)
{
    awbl_sdcard_dev_t *p_sdcard;

    /* search for all SDCARDs */
    __sd_lock();

    if ((__gp_sd_list.prev == NULL && __gp_sd_list.next == NULL) || name == NULL) {
        return AW_FALSE;
    }

    aw_list_for_each_entry(p_sdcard,
                           &__gp_sd_list,
                           awbl_sdcard_dev_t,
                           node) {
        if (strncmp(name,
                    p_sdcard->p_sd_mem->name,
                    sizeof(p_sdcard->p_sd_mem->name)) == 0) {
            /* found it */
            __sd_unlock();
            return AW_TRUE;
        }
    }
    __sd_unlock();
    return AW_FALSE;
}

/* end of file */
