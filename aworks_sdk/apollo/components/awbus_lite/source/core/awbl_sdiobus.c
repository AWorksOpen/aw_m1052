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
 * \brief AWBus SDIO bus type implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.00 14-08-08  deo, first implementation
 * - 1.01 17-01-06  xdn, code refactoring
 * - 1.02 17-09-15  xdn, modify mem_alloc bug
 * - 1.03 18-04-20  xdn, modify static device enum way
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "awbus_lite.h"
#include "aw_assert.h"
#include "string.h"
#include "aw_spinlock.h"
#include "aw_list.h"
#include "aw_sem.h"
#include "aw_delay.h"
#include "awbl_sdiobus.h"
#include "stdio.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "aw_gpio.h"
#include "aw_system.h"


#define __SDIO_SPI_SWAB32(x) ((uint32_t)(\
            (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) | \
            (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) | \
            (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) | \
            (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))

#define __SDIO_SYNC_TIMEOUT               10000

#define __SDIO_IO_NORMAL_SPEED            25000000

#define __SDIO_CMD53_ARG_FLAG_W           (1 << 31)
#define __SDIO_CMD53_ARG_FNUM(x)          (((x) & 0x7) << 28)
#define __SDIO_CMD53_ARG_BLKMD_BLK        (1 << 27)
#define __SDIO_CMD53_ARG_FLAG_OP_INCREM   (1 << 26)
#define __SDIO_CMD53_ARG_ADDR(x)          (((x) & 0x1FFFF) << 9)
#define __SDIO_CMD53_ARG_CNT(x)           (((x) & 0x1FF) << 0)

#define __SDIO_CMD52_ARG_FLAG_W           (1 << 31)
#define __SDIO_CMD52_ARG_FNUM(x)          (((x) & 0x7) << 28)
#define __SDIO_CMD52_ARG_RAM_FLG          (1 << 27)
#define __SDIO_CMD52_ARG_ADDR(x)          (((x) & 0x1FFFF) << 9)
#define __SDIO_CMD52_ARG_DATA(x)          (((x) & 0xFF) << 0)

#define __SDIO_CMD52_RSP_COM_CRC_ERROR    (1 << 15)
#define __SDIO_CMD52_RSP_ILLEGAL_COMMAND  (1 << 14)
#define __SDIO_CMD52_RSP_ERROR            (1 << 11)
#define __SDIO_CMD52_RSP_RFU              (1 << 10)
#define __SDIO_CMD52_RSP_FUNCTION_NUMBER  (1 << 9)
#define __SDIO_CMD52_RSP_OUT_OF_RANGE     (1 << 8)

#define __SDIO_CMD52_RSP_ALL_ERROR        0xCF00

#define __SDIO_OCCR_REG_ADDR_ABORT        0x06
#define __SDIO_OCCR_REG_ADDR_BUSCTRL      0x07
#define __SDIO_OCCR_REG_ADDR_BLKSIZE      0x10

#define __SDIO_OCCR_REG_ABORT_RES         (1 << 3)
#define __SDIO_OCCR_REG_ABORT_AS(x)       (((x) & 0x7) << 0)


#define __SDCARD_SPEED_CLASS              0x001F
#define __SDCARD_CLASS_0                  0x0001
#define __SDCARD_CLASS_2                  0x0002
#define __SDCARD_CLASS_4                  0x0004
#define __SDCARD_CLASS_6                  0x0008
#define __SDCARD_CLASS_10                 0x0010

#define __SDCARD_VERSION                  0x00E0
#define __SDCARD_SDV2X                    0x0020
#define __SDCARD_SDV1X                    0x0040
#define __SDCARD_MMC                      0x0080

#define __SDCARD_CAPACITY_VER             0x0300
#define __SDCARD_SDSC_MMC                 0x0100
#define __SDCARD_SDHC                     0x0200
#define __SDCARD_SDXC                     0x0400

#define __CSD_SPEC_VER_0                  0
#define __CSD_SPEC_VER_1                  1
#define __CSD_SPEC_VER_2                  2
#define __CSD_SPEC_VER_3                  3
#define __CSD_SPEC_VER_4                  4


/**
 * 简单的热插拔资源统计
 */
#define __SDIO_MEM_DBG_EN   0

#if __SDIO_MEM_DBG_EN
aw_local int32_t __g_sdio_mem_cnt = 0;
#if 0 
#define __MEM_ALLOC_SIZE(size) \
  aw_mem_alloc(size); __g_sdio_mem_cnt++; aw_kprintf("**ALLOC:%d\r\n", __LINE__)
#define __MEM_FREE_SUB(p) \
            if (p != NULL) {aw_mem_free(p); p = NULL; __g_sdio_mem_cnt--; \
            aw_kprintf("--FREE:%d\r\n", __LINE__);}
#define __MEM_FREE_THIS(p) \
            if (p != NULL) {aw_mem_free(p); __g_sdio_mem_cnt--; \
            aw_kprintf("--FREE:%d\r\n", __LINE__);}
#endif
#define __MEM_ALLOC_SIZE(size) aw_mem_alloc(size); __g_sdio_mem_cnt++
#define __MEM_FREE_SUB(p)      if (p != NULL) {aw_mem_free(p); p = NULL; __g_sdio_mem_cnt--;}
#define __MEM_FREE_THIS(p)     if (p != NULL) {aw_mem_free(p); __g_sdio_mem_cnt--;}
#define __MEM_ANALYSE_SHOW()   AW_INFOF(("SDIO_MEM_USED: %d\r\n", __g_sdio_mem_cnt))
#else
#define __MEM_ALLOC_SIZE(size) aw_mem_alloc(size)
#define __MEM_FREE_SUB(p)      if (p != NULL) {aw_mem_free(p); p = NULL;}
#define __MEM_FREE_THIS(p)     if (p != NULL) {aw_mem_free(p);}
#endif

#define __MEM_FREE_TUPLES(tuples)   \
    do {\
        if (tuples == NULL) {\
            break;\
        }\
         struct sdio_func_tuple  *p_index_1 = NULL;\
         struct sdio_func_tuple  *p_index_2 = NULL;\
         p_index_1 = tuples->next;\
         __MEM_FREE_SUB(tuples);\
         while (p_index_1 != NULL){\
             p_index_2 = p_index_1->next;\
             __MEM_FREE_THIS(p_index_1);\
             if (p_index_2 == NULL) {\
                 break;\
             }\
             p_index_1 = p_index_2->next;\
             __MEM_FREE_THIS(p_index_2);\
         };\
     } while(0);


#define __SDIO_MEMORY_PRINTF_INFO(p_sdcard)     \
    do {\
        awbl_sdio_mem_card_info_t *p_mem_info = &p_sdcard->mem_info;\
        AW_INFOF(("\r\n"));\
        AW_INFOF(("Found a new memory card:\r\n"));\
        AW_INFOF(("        Device Name: %s\r\n", p_sdcard->p_card->name));\
        AW_INFOF(("        Product name: %s\r\n", p_mem_info->cid.pnm));\
        AW_INFOF(("        Capacity: %d MB\r\n",\
                                     p_mem_info->csd.sector_cnt / 2048));\
        AW_INFOF(("        Manufacturing date: %04d-%02d\r\n",\
                                                p_mem_info->cid.year,\
                                                p_mem_info->cid.month));\
        AW_INFOF(("\r\n"));\
    } while(0)


/******************************************************************************/
aw_local aw_bool_t __sdiobus_devmatch (const struct awbl_drvinfo *p_drv,
                                       struct awbl_dev           *p_dev);

aw_local aw_err_t __sdcard_decode_scr (
    struct awbl_sdio_mem_card_info *p_mem_info,
    uint32_t                       *p_scr);

aw_local aw_err_t __sdcard_decode_ssr (
    struct awbl_sdio_mem_card_info *p_mem_info,
    uint32_t                       *p_ssr);

aw_local aw_err_t __sdio_func_init(struct awbl_sdio_device  *p_card,
                                   uint8_t                   fn);

aw_local aw_err_t __sdio_cistpl_funce_common (struct awbl_sdio_device   *p_card,
                                              struct awbl_sdio_func     *p_func,
                                              const uint8_t             *buf,
                                              uint8_t                    size);

aw_local aw_err_t __sdio_cistpl_funce_func (struct awbl_sdio_device   *p_card,
                                            struct awbl_sdio_func     *p_func,
                                            const uint8_t             *buf,
                                            uint8_t                    size);

aw_local aw_err_t __sdio_cistpl_vers_1 (struct awbl_sdio_device   *p_card,
                                        struct awbl_sdio_func     *p_func,
                                        const uint8_t             *buf,
                                        uint8_t                    size);

aw_local aw_err_t __sdio_cistpl_manfid (struct awbl_sdio_device   *p_card,
                                        struct awbl_sdio_func     *p_func,
                                        const uint8_t             *buf,
                                        uint8_t                    size);

aw_local aw_err_t __sdio_cistpl_funce (struct awbl_sdio_device   *p_card,
                                       struct awbl_sdio_func     *p_func,
                                       const uint8_t             *buf,
                                       uint8_t                    size);

aw_local aw_err_t __iocard_decode_fbr (struct awbl_sdio_device       *p_card,
                                       uint8_t                        fun_num,
                                       struct awbl_sdio_io_card_info *p_io_info,
                                       uint8_t                       *p_fbr);

aw_local void __sdio_scan_work (void *p_arg);

aw_local void __sdio_det_task(void *p_arg);

aw_err_t aw_sdio_dynamic_memory_enum (struct awbl_sdio_host *p_host,
                                      uint32_t               ocr,
                                      uint8_t                pre_type,
                                      aw_bool_t              dynamic);

aw_err_t aw_sdio_dynamic_sdio_enum (struct awbl_sdio_host *p_host,
                                    uint32_t               ocr,
                                    struct awbl_sdio_func *p_func);

aw_err_t aw_sdio_host_spi_read_ocr (struct awbl_sdio_host *p_host,
                                    uint8_t                highcap,
                                    uint32_t              *p_ocrp);

/******************************************************************************/
aw_local aw_err_t __sdiobus_dev_enum (struct awbl_dev *p_sdiodev)
{
    int i;
    struct awbl_dev           *p_dev  = NULL;
    struct awbl_sdio_dev_info *p_dev_info = NULL;
    struct awbl_sdio_host     *p_host = ( struct awbl_sdio_host *)p_sdiodev;
    const struct awbl_devhcf  *p_devhcf_list = awbl_devhcf_list_get();
    size_t                 devhcf_list_count = awbl_devhcf_list_count_get();
    struct awbl_sdio_host_info *p_info = \
            (struct awbl_sdio_host_info *)AWBL_DEVINFO_GET(p_sdiodev);

    for (i = 0; i < devhcf_list_count; i++) {
        if ((p_devhcf_list[i].bus_type  == AWBL_BUSID_SDIO) &&
            (p_devhcf_list[i].bus_index == p_info->bus_index)) {
            p_dev = p_devhcf_list[i].p_dev;
            if (p_dev == NULL) {
                return -AW_ENOMEM;
            }
            p_dev->p_parentbus = p_sdiodev->p_subbus;
            p_dev->p_devhcf = &p_devhcf_list[i];

            p_dev_info = (struct awbl_sdio_dev_info *)
                             p_dev->p_devhcf->p_devinfo;
            if (p_dev_info->type == AWBL_SDIO_TYPE_IO) {
                ((struct awbl_sdio_func *)p_dev)->p_card = p_host->p_card;
            } else {
                ((struct awbl_sdio_mem *)p_dev)->p_card = p_host->p_card;
            }
            if (p_dev_info->pfn_plfm_init) {
                p_dev_info->pfn_plfm_init();
            }
            awbl_dev_announce(p_dev);
        }
    }

    return AW_OK;
}

aw_local aw_bool_t __sdiobus_devmatch (const struct awbl_drvinfo *p_drv,
                                       struct awbl_dev           *p_dev)
{
    const struct awbl_dev_method *p_methods;
    if (NULL == p_drv ||
        NULL == p_dev) {
        return AW_FALSE;
    }
    if (p_dev->p_devhcf->bus_type != AWBL_BUSID_SDIO) {
        return AW_FALSE;
    }

    AWBL_DRVINFO_TO_CARDINFO_DECL(p_dev_drvinfo, p_drv);
    struct awbl_sdio_dev_info *p_dev_info = (struct awbl_sdio_dev_info *)
                                            p_dev->p_devhcf->p_devinfo;

    if (strcmp(p_dev->p_devhcf->p_name, p_drv->p_drvname) == 0) {
        return AW_TRUE;
    }

    /* MEMORY */
    if ((p_dev_info->type == AWBL_SDIO_TYPE_MMC) ||
        (p_dev_info->type == AWBL_SDIO_TYPE_SD)) {
        if ((p_dev_drvinfo->type != AWBL_SDIO_TYPE_MMC) &&
            (p_dev_drvinfo->type != AWBL_SDIO_TYPE_SD)) {
            return AW_FALSE;
        }

    /* IO */
    } else {
        if ((p_dev_drvinfo->type != AWBL_SDIO_TYPE_IO) &&
            (p_dev_drvinfo->type != AWBL_SDIO_TYPE_COMBO)) {
            return AW_FALSE;
        }
    }

    p_methods = p_drv->p_methods;
    while (1) {
        if (p_methods->dev_method_id == 0) {
            return AW_FALSE;
        }
        /* awbl_sdio_match_type only be called by dynamic device */
        if (AWBL_METHOD_CALL(awbl_sdio_match_type)
                == p_methods->dev_method_id) {

            if ((p_methods->pfunc_handler != NULL) &&
                (p_methods->pfunc_handler(p_dev, NULL)
                         == AW_OK)) {
                break;
            } else {
                return AW_FALSE;
            }
        }
        p_methods++;
    }

    return AW_TRUE;
}

aw_local aw_err_t __sdio_register_sdcard (struct awbl_sdio_mem *p_sdcard)
{
    if (NULL == p_sdcard ||
        NULL == p_sdcard->p_card->p_host) {
        return -AW_EINVAL;
    }
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_sdcard->p_card->p_host);
    p_sdcard->devinfo.type = AWBL_SDIO_TYPE_SD;
    memcpy(p_sdcard->name, 
           p_sdcard->p_card->name, 
           sizeof(p_sdcard->p_card->name));

    p_sdcard->devhcf.p_name     = (const char*)p_sdcard->name;
    p_sdcard->devhcf.unit       = 0;
    p_sdcard->devhcf.bus_type   = AWBL_BUSID_SDIO;
    p_sdcard->devhcf.bus_index  = p_info->bus_index;
    p_sdcard->devhcf.p_dev      = &(p_sdcard->super);
    p_sdcard->devhcf.p_devinfo  = (void*)(&p_sdcard->devinfo);
    p_sdcard->super.p_parentbus = ((struct awbl_dev *)
                                   p_sdcard->p_card->p_host)->p_subbus;
    p_sdcard->super.p_devhcf    = &(p_sdcard->devhcf);

    return awbl_dev_announce(&(p_sdcard->super));
}

aw_local aw_err_t __sdio_unregister_sdcard (struct awbl_sdio_mem *p_sdcard)
{
    aw_err_t ret = AW_OK;
    if (NULL == p_sdcard) {
        return -AW_EINVAL;
    }
    ret = awbl_dev_remove_announce(&p_sdcard->super);
    return ret;
}

aw_local aw_err_t __sdio_register_iofunc (struct awbl_sdio_func *p_func)
{
    if (NULL == p_func  ||
        NULL == p_func->p_card->p_host) {
        return -AW_EINVAL;
    }
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_func->p_card->p_host);
    p_func->devinfo.type   = AWBL_SDIO_TYPE_IO;
    p_func->devinfo.class  = p_func->class;
    p_func->devinfo.vendor = p_func->p_card->io_info.cis.tplmid_manf;
    p_func->devinfo.device = p_func->p_card->io_info.cis.tplmid_card;

    p_func->devhcf.p_name    = p_func->name;
    p_func->devhcf.unit      = 0;
    p_func->devhcf.bus_type  = AWBL_BUSID_SDIO;
    p_func->devhcf.bus_index = p_info->bus_index;
    p_func->devhcf.p_dev     = &(p_func->super);
    p_func->devhcf.p_devinfo = (void*)(&p_func->devinfo);
    p_func->super.p_parentbus = ((struct awbl_dev *)
                                 p_func->p_card->p_host)->p_subbus;
    p_func->super.p_devhcf    = &(p_func->devhcf);

    return awbl_dev_announce(&(p_func->super));
}


aw_local aw_err_t __sdio_unregister_iofunc (struct awbl_sdio_func *p_func)
{
    aw_err_t ret = AW_OK;
    if (NULL == p_func) {
        return -AW_EINVAL;
    }
    ret = awbl_dev_remove_announce(&p_func->super);
    return ret;
}


aw_local void __sdio_nouse_cb(struct aw_refcnt *p_ref)
{
    uint8_t i = 0;
    struct awbl_sdio_device *p_card = (struct awbl_sdio_device*)AW_CONTAINER_OF
                                        (p_ref, struct awbl_sdio_device, ref);
    struct awbl_sdio_host   *p_host = p_card->p_host;
    if (p_card != NULL) {
        AW_INFOF(("\r\n"));
        if (p_card->p_sdcard) {
            AW_INFOF(("Delete memory card: %s\r\n", p_card->p_sdcard->name));
            __MEM_FREE_SUB(p_card->p_sdcard->info);
            __MEM_FREE_TUPLES(p_card->p_sdcard->tuples);
            __MEM_FREE_SUB(p_card->p_sdcard);
        }
        for (i = 0; i < AWBL_SDIO_MAX_FUNC; i++) {
            if (p_card->p_iofunc[i]) {
                AW_INFOF(("Delete sdio card: %s\r\n", 
                          p_card->p_iofunc[i]->name));
                __MEM_FREE_SUB(p_card->p_iofunc[i]->info);
                __MEM_FREE_TUPLES(p_card->p_iofunc[i]->tuples);
                __MEM_FREE_SUB(p_card->p_iofunc[i]);
            }
        }
        AW_INFOF(("\r\n"));
        __MEM_FREE_SUB(p_card->info);
        __MEM_FREE_TUPLES(p_card->tuples);
        __MEM_FREE_SUB(p_host->p_card);
        }
}


aw_local void __sdio_scan_work (void *p_arg)
{
    aw_err_t err        = 0;
    uint32_t status     = 0;
    aw_bool_t   ret        = AW_FALSE;
    uint8_t  port_state = 0;
    uint32_t ocr        = 0;
    uint8_t  i          = 0;
    uint8_t  num_info   = 0;
    struct awbl_sdio_host    *p_host = (struct awbl_sdio_host *)p_arg;
    struct awbl_sdio_timeout_obj  timeout;

    AWBL_HOST_TO_HSOST_DRV_INFO_DECL(p_drvinfo, p_host);
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_hostinfo, p_host);
    err = awbl_sdio_host_get_port_status(p_host, &port_state);
    if (err) {
        goto out;
    }
    AW_MUTEX_LOCK(p_host->dev_mux, AW_SEM_WAIT_FOREVER);

    /**
     * 中断触发进入(可监测卡抖动)
     */
    if (p_host->changed) {
        p_host->changed = AW_FALSE;
        if ((p_host->p_card != NULL &&
            (!p_host->removed)  &&
            port_state == AWBL_SDIO_CD_PLUG)) {
            err = aw_sdio_host_send_status(p_host, p_host->p_card->rca, &status);
            /** 卡存在但是通信有误 -->直接移除 */
            if (err != AW_OK) {
                p_host->removed = AW_TRUE;
            } else {
            }
        }
    }

    /**
     * Device insert
     */
    if ((port_state == AWBL_SDIO_CD_PLUG) && (p_host->p_card == NULL)) {
        AW_INFOF(("\r\nInsert an sdio device,start to identify...\r\n"));
        aw_spinlock_isr_take(&(p_host->dev_lock));
        p_host->removed = AW_FALSE;
#ifdef SDIO_TRANS_ERRCNT_ENABLE
        p_host->err_cnt = 0;
#endif
        aw_spinlock_isr_give(&(p_host->dev_lock));
        err = aw_sdio_dynamic_dev_enumerate(p_host);
#if __SDIO_MEM_DBG_EN
        __MEM_ANALYSE_SHOW();
#endif
        if (err) {
            goto out;
        }

    /**
     * Device remove
     */
    }else if ((port_state == AWBL_SDIO_CD_STUBBS  ||
               p_host->removed)                   &&
              (p_host->p_card != NULL)) {
        if (p_host->state == AW_SDIO_DEV_STAT_DEVREGIST) {
            if (p_host->p_card->type == AWBL_SDIO_TYPE_MMC ||
                p_host->p_card->type == AWBL_SDIO_TYPE_SD  ||
                p_host->p_card->type == AWBL_SDIO_TYPE_COMBO) {
                __sdio_unregister_sdcard(p_host->p_card->p_sdcard);
                ret = aw_refcnt_put(&p_host->p_card->ref, __sdio_nouse_cb);

            } else if (p_host->p_card->type == AWBL_SDIO_TYPE_IO ||
                       p_host->p_card->type == AWBL_SDIO_TYPE_COMBO) {
                num_info = p_host->p_card->func_num;
                for (i = 0; i < num_info; i++) {
                    __sdio_unregister_iofunc(p_host->p_card->p_iofunc[i]);
                }
                ret = aw_refcnt_put(&p_host->p_card->ref, __sdio_nouse_cb);
            }
            p_host->state = AW_SDIO_DEV_STAT_UNINSTALL;
            aw_spinlock_isr_take(&(p_host->dev_lock));
            p_host->removed = AW_TRUE;
            aw_spinlock_isr_give(&(p_host->dev_lock));

        } else if (p_host->state == AW_SDIO_DEV_STAT_UNINSTALL) {
            p_host->p_card = NULL;
            AW_INFOF(("SDIO ERROR:have device not realse the sdio card.\r\n"));
        }
#if __SDIO_MEM_DBG_EN
        __MEM_ANALYSE_SHOW();
#endif
    }
out:
    AW_MUTEX_UNLOCK(p_host->dev_mux);
    return;
}


aw_local void __sdio_det_task(void *p_arg)
{
    struct awbl_sdio_host *p_host = (struct awbl_sdio_host *)p_arg;
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_host_info, p_host);

    AW_FOREVER {
        if (p_host_info->connect_type == AWBL_SDIO_CON_DEV_DYNAMIC) {
            __sdio_scan_work(p_arg);
            aw_mdelay(200);
        } else {
            AW_INFOF(("SDIO: invalid connect_type"));
            while(1);
        }
    }
}


aw_local void __sdio_sync_complete (void *p_arg)
{
    aw_assert(p_arg != NULL);
    if (p_arg) {
        aw_semb_give((aw_psp_semb_id_t)p_arg);
    }
}


aw_local aw_err_t __sdio_host_lock (struct awbl_sdio_host *p_host)
{
    if (p_host == NULL) {
        return -AW_EINVAL;
    }
    AW_MUTEX_LOCK(p_host->dev_mux, AW_SEM_WAIT_FOREVER);
    aw_spinlock_isr_take(&(p_host->dev_lock));
    p_host->lock = AW_TRUE;
    aw_spinlock_isr_give(&(p_host->dev_lock));
    return AW_OK;
}


aw_local aw_err_t __sdio_host_unlock (struct awbl_sdio_host *p_host)
{
    if (p_host == NULL) {
        return -AW_EINVAL;
    }
    aw_spinlock_isr_take(&(p_host->dev_lock));
    p_host->lock = AW_FALSE;
    aw_spinlock_isr_give(&(p_host->dev_lock));
    AW_MUTEX_UNLOCK(p_host->dev_mux);
    return AW_OK;
}


/* SD table */
aw_local const uint32_t __gp_tran_exp[] = {
    10000, 100000, 1000000, 10000000, 0, 0, 0, 0
};

aw_local const uint8_t __gp_tran_mant[] = {
    0,  10, 12, 13, 15, 20, 25, 30,
    35, 40, 45, 50, 55, 60, 70, 80,
};

aw_local const uint32_t __gp_sd_au_size[] = {
    0,
    (16*1024) / 512,
    (32*1024) / 512,
    (64*1024) / 512,
    (128*1024) / 512,
    (256*1024) / 512,
    (512*1024) / 512,
    (1024*1024) / 512,
    (2*1024*1024) / 512,
    (4*1024*1024) / 512,
    (8*1024*1024) / 512,
    (12*1024*1024) / 512,
    (16*1024*1024) / 512,
    (24*1024*1024) / 512,
    (32*1024*1024) / 512,
    (64*1024*1024) / 512,
};

typedef aw_err_t (tpl_parse_t) (struct awbl_sdio_device   *p_card,
                                struct awbl_sdio_func     *p_func,
                                const uint8_t             *buf,
                                uint8_t                    size);


struct cis_tpl {
    uint8_t      code;
    uint8_t      min_size;
    tpl_parse_t *parse;
};

/* Known TPL_CODEs table for CIS tuples */
aw_local const struct cis_tpl cis_tpl_list[] = {
    {    0x15,    3,    __sdio_cistpl_vers_1    },
    {    0x20,    4,    __sdio_cistpl_manfid    },
    {    0x21,    2,    /* cistpl_funcid */     },
    {    0x22,    0,    __sdio_cistpl_funce     },
    {    0x91,    2,    /* cistpl_sdio_std */   },
};

aw_local const struct cis_tpl cis_tpl_funce_list[] = {
    {    0x00,    4,    __sdio_cistpl_funce_common          },
    {    0x01,    0,    __sdio_cistpl_funce_func            },
    {    0x04,    1+1+6,    /* CISTPL_FUNCE_LAN_NODE_ID */  },
};

aw_local const uint8_t speed_val[16] =
    { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };

aw_local const uint32_t speed_unit[8] =
    { 10000, 100000, 1000000, 10000000, 0, 0, 0, 0 };



aw_local aw_err_t __sdio_sd_get_cid(struct awbl_sdio_host *p_host,
                                    uint32_t               ocr,
                                    uint32_t              *rocr,
                                    uint32_t              *p_cid)
{
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_hostinfo, p_host);
    aw_err_t err = 0;
    aw_sdio_host_go_idle_state(p_host);
    err = aw_sdio_host_send_if_cond (p_host, ocr);
    if (!err) {
        ocr |= AWBL_SD_OCR_CCS;
    }
    err = aw_sdio_host_send_app_op_cond(p_host, 0, ocr, rocr);
    if (err) {
        return err;
    }
    if (p_hostinfo->mode == AWBL_SDIO_SPI_M) {
        err = aw_sdio_send_cid(p_host->p_card, 0, p_cid);
    } else {
        err = aw_sdio_all_send_cid(p_host->p_card, p_cid);
    }
    return err;
}


aw_local aw_err_t __sdio_sd_setup_card(struct awbl_sdio_mem      *p_sdcard,
                                       awbl_sdio_mem_card_info_t *p_mem_info)
{
    aw_err_t err = 0;
    uint32_t ssr[16];
    uint32_t buf[16];
    struct awbl_sdio_device  *p_card = NULL;
    if (p_sdcard                 == NULL ||
        p_sdcard->p_card->p_host == NULL) {
        return -AW_EINVAL;
    }
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_hostinfo, p_sdcard->p_card->p_host);

    p_card = p_sdcard->p_card;
    err = aw_sdio_app_send_scr(p_card, p_card->rca, p_sdcard->raw_scr);
    if (err != AW_OK) {
        return err;
    }
    __sdcard_decode_scr(p_mem_info, p_sdcard->raw_scr);

    err = aw_sdio_app_sd_status(p_card, p_card->rca, ssr);
    if (err != AW_OK) {
        return err;
    }
    __sdcard_decode_ssr(p_mem_info, ssr);

    /* read switch */
    err = aw_sdio_sd_switch(p_card,
                            0,
                            0,
                            0,
                            (uint8_t *)buf);
    if (err != AW_OK) {
        return err;
    }
    /*For SPI enable CRC */
    if (p_hostinfo->mode == AWBL_SDIO_SPI_M) {
        if (p_hostinfo->crc_en) {
            err = aw_sdio_spi_set_crc(p_card, 1);
            if (err != AW_OK) {
                return err;
            }
        }
    }
    return AW_OK;
}


aw_local uint32_t __get_bits_str (uint32_t *p_str,
                                  uint32_t  start,
                                  uint8_t   len)
{
    uint32_t  mask;
    uint32_t  index;
    uint8_t   shift;
    uint32_t  value;

    if (NULL == p_str) {
        return -AW_EINVAL;
    }
    mask  = (int)((len < 32) ? (1 << len) : 0) - 1;
    index = start / 32;
    shift = start & 31;
    value = p_str[index] >> shift;

    if ((len + shift) > 32) {
        value |= p_str[index + 1] << (32 - shift);
    }
    value &= mask;
    return value;
}


aw_local aw_err_t __sdcard_decode_cid (
    struct awbl_sdio_mem_card_info *p_mem_info,
    uint32_t                       *p_cid)
{
    if (NULL == p_mem_info   ||
        NULL == p_cid) {
        return -AW_EINVAL;
    }
    memset(&(p_mem_info->cid), 0, sizeof(struct awbl_sdcard_cid));
    if (p_mem_info->attribute & __SDCARD_MMC) {
        switch (p_mem_info->csd.mmca_vsn) {
        case 0: /* MMC v1.0 - v1.2 */
        case 1: /* MMC v1.4 */
            p_mem_info->cid.mid    = __get_bits_str(p_cid, 104, 24);
            p_mem_info->cid.pnm[0] = __get_bits_str(p_cid, 96, 8);
            p_mem_info->cid.pnm[1] = __get_bits_str(p_cid, 88, 8);
            p_mem_info->cid.pnm[2] = __get_bits_str(p_cid, 80, 8);
            p_mem_info->cid.pnm[3] = __get_bits_str(p_cid, 72, 8);
            p_mem_info->cid.pnm[4] = __get_bits_str(p_cid, 64, 8);
            p_mem_info->cid.pnm[5] = __get_bits_str(p_cid, 56, 8);
            p_mem_info->cid.pnm[6] = __get_bits_str(p_cid, 48, 8);
            p_mem_info->cid.hwrev  = __get_bits_str(p_cid, 44, 4);
            p_mem_info->cid.fwrev  = __get_bits_str(p_cid, 40, 4);
            p_mem_info->cid.psn    = __get_bits_str(p_cid, 16, 24);
            p_mem_info->cid.month  = __get_bits_str(p_cid, 12, 4);
            p_mem_info->cid.year   = __get_bits_str(p_cid, 8, 4) + 1997;
            break;

        case 2: /* MMC v2.0 - v2.2 */
        case 3: /* MMC v3.1 - v3.3 */
        case 4: /* MMC v4 */
            p_mem_info->cid.mid    = __get_bits_str(p_cid, 120, 8);
            p_mem_info->cid.oid    = __get_bits_str(p_cid, 104, 16);
            p_mem_info->cid.pnm[0] = __get_bits_str(p_cid, 96, 8);
            p_mem_info->cid.pnm[1] = __get_bits_str(p_cid, 88, 8);
            p_mem_info->cid.pnm[2] = __get_bits_str(p_cid, 80, 8);
            p_mem_info->cid.pnm[3] = __get_bits_str(p_cid, 72, 8);
            p_mem_info->cid.pnm[4] = __get_bits_str(p_cid, 64, 8);
            p_mem_info->cid.pnm[5] = __get_bits_str(p_cid, 56, 8);
            p_mem_info->cid.prv    = __get_bits_str(p_cid, 48, 8);
            p_mem_info->cid.psn    = __get_bits_str(p_cid, 16, 32);
            p_mem_info->cid.month  = __get_bits_str(p_cid, 12, 4);
            p_mem_info->cid.year   = __get_bits_str(p_cid, 8, 4) + 1997;
            break;

        default:
            return -AW_EINVAL;
        }

    } else {
        p_mem_info->cid.mid    = __get_bits_str(p_cid, 120, 8);
        p_mem_info->cid.oid    = __get_bits_str(p_cid, 104, 16);
        p_mem_info->cid.pnm[0] = __get_bits_str(p_cid, 96, 8);
        p_mem_info->cid.pnm[1] = __get_bits_str(p_cid, 88, 8);
        p_mem_info->cid.pnm[2] = __get_bits_str(p_cid, 80, 8);
        p_mem_info->cid.pnm[3] = __get_bits_str(p_cid, 72, 8);
        p_mem_info->cid.pnm[4] = __get_bits_str(p_cid, 64, 8);
        p_mem_info->cid.prv    = __get_bits_str(p_cid, 56, 8);
        p_mem_info->cid.psn    = __get_bits_str(p_cid, 24, 32);
        p_mem_info->cid.year   = __get_bits_str(p_cid, 12, 8) + 2000;
        p_mem_info->cid.month  = __get_bits_str(p_cid, 8, 4);
    }
    return AW_OK;
}


aw_local aw_err_t __sdcard_decode_csd (
    struct awbl_sdio_mem_card_info *p_mem_info,
    uint32_t                       *p_csd)
{
    uint32_t e, m, r;
    uint8_t  structure;

    if (NULL == p_mem_info   ||
        NULL == p_csd) {
        return -AW_EINVAL;
    }
    structure = __get_bits_str(p_csd, 126, 2);
    if (p_mem_info->attribute & __SDCARD_MMC) {
        if (structure == 0) {
            /* 该版本无法识别 */
            return -AW_EINVAL;
        }
        p_mem_info->csd.mmca_vsn   = __get_bits_str(p_csd, 122, 4);

        m = __get_bits_str(p_csd, 99, 4);
        e = __get_bits_str(p_csd, 96, 3);
        p_mem_info->csd.max_tr_speed  = __gp_tran_exp[e] * __gp_tran_mant[m];
        p_mem_info->csd.cmd_class     = __get_bits_str(p_csd, 84, 12);

        p_mem_info->csd.sector_size = 512;
        m = __get_bits_str(p_csd, 62, 12);
        e = __get_bits_str(p_csd, 47, 3);
        r = __get_bits_str(p_csd, 80, 4);
        p_mem_info->csd.sector_cnt = ((1 + m) << (e + r - 7));

        e = __get_bits_str(p_csd, 22, 4);

        if (e >= 9) {
            m = __get_bits_str(p_csd, 42, 5);
            r = __get_bits_str(p_csd, 37, 5);
            p_mem_info->csd.block_size = (m + 1) * (r + 1);
            p_mem_info->csd.block_size <<= e - 9;
        }

    } else {
        switch (structure) {
            case 0:
                m = __get_bits_str(p_csd, 99, 4);
                e = __get_bits_str(p_csd, 96, 3);
                p_mem_info->csd.max_tr_speed = __gp_tran_exp[e] * 
                                               __gp_tran_mant[m];
                p_mem_info->csd.cmd_class = __get_bits_str(p_csd, 84, 12);

                p_mem_info->csd.sector_size = 512;
                m = __get_bits_str(p_csd, 62, 12);
                e = __get_bits_str(p_csd, 47, 3);
                r = __get_bits_str(p_csd, 80, 4);
                p_mem_info->csd.sector_cnt = ((1 + m) << (e + r - 7));


                if (__get_bits_str(p_csd, 46, 1)) {
                    p_mem_info->csd.block_size = 1;
                } else {
                    p_mem_info->csd.block_size = __get_bits_str(p_csd, 39, 7)+1;
                    p_mem_info->csd.block_size <<= 
                        (__get_bits_str(p_csd, 22, 4)-9);
                }

                break;

            case 1:
                m = __get_bits_str(p_csd, 99, 4);
                e = __get_bits_str(p_csd, 96, 3);
                p_mem_info->csd.max_tr_speed = __gp_tran_exp[e] * 
                                               __gp_tran_mant[m];
                p_mem_info->csd.cmd_class    = __get_bits_str(p_csd, 84, 12);

                p_mem_info->csd.sector_size = 512;
                m = __get_bits_str(p_csd, 48, 22);
                p_mem_info->csd.sector_cnt = (1 + m) << 10;
                p_mem_info->csd.block_size = 1;
                break;
            default:
                return -AW_EINVAL;
        }
    }
    return AW_OK;
}


aw_local aw_err_t __sdcard_decode_ext_csd(
    struct awbl_sdio_mem_card_info *p_mem_info,
    uint32_t                       *p_ext_csd)
{
    uint8_t *ptr = (uint8_t*)p_ext_csd;
    uint8_t csd_type = 0;

    if (ptr[192] > 8) {
        return -AW_EINVAL;
    }
    csd_type = ptr[196];
    if (ptr[192] >= 2) {
        /* if in sector mode */
        if (p_mem_info->ocr & (1<<30)) {
            p_mem_info->csd.sector_cnt = (uint32_t)ptr[212] << 0  |
                                         (uint32_t)ptr[213] << 8  |
                                         (uint32_t)ptr[214] << 16 |
                                         (uint32_t)ptr[215] << 24;
        }
    }
    return AW_OK;
}


aw_local aw_err_t __sdcard_decode_scr (
    struct awbl_sdio_mem_card_info *p_mem_info,
    uint32_t                       *p_scr)
{
    if (NULL == p_mem_info   ||
        NULL == p_scr) {
        return -AW_EINVAL;
    }
    p_mem_info->scr.structure = __get_bits_str(p_scr, 60, 4);
    if (p_mem_info->scr.structure != 0) {
        return -AW_EINVAL;
    }
    p_mem_info->scr.sda_vsn    = __get_bits_str(p_scr, 56, 4);
    p_mem_info->scr.bus_widths = __get_bits_str(p_scr, 48, 4);

    if (p_mem_info->scr.sda_vsn == 2) {
        p_mem_info->scr.sda_spec3 = __get_bits_str(p_scr, 47, 1);
    }
    if (__get_bits_str(p_scr, 55, 1)) {
        p_mem_info->scr.erase_value = 0xFF;
    } else {
        p_mem_info->scr.erase_value = 0x00;
    }
    if (p_mem_info->scr.sda_spec3) {
        p_mem_info->scr.cmds = __get_bits_str(p_scr, 32, 2);
    }

    return AW_OK;
}


aw_local aw_err_t __sdcard_decode_ssr (
    struct awbl_sdio_mem_card_info *p_mem_info,
    uint32_t                       *p_ssr)
{
    uint32_t au;
    uint32_t speed_calss = 0;

    if (NULL == p_mem_info   ||
        NULL == p_ssr) {
        return -AW_EINVAL;
    }
    au = __get_bits_str(p_ssr, 428, 4);
    speed_calss = __get_bits_str(p_ssr, 440, 8);  /* class10  0x04 */
    if (au) {
        if ((au <= 9) || (p_mem_info->scr.sda_spec3)) {
            p_mem_info->ssr.au_size = __gp_sd_au_size[au];
        } else {
            return -AW_EINVAL;
        }
    }
    return AW_OK;
}


/* todo 未使用，后期有硬件调试 */
aw_local aw_err_t __iocard_decode_fbr (struct awbl_sdio_device       *p_card,
                                       uint8_t                        fun_num,
                                       struct awbl_sdio_io_card_info *p_io_info,
                                       uint8_t                       *p_fbr)
{
    uint32_t bfr_addr = AWBL_SDIO_ADDR_INFO_FBR_F(fun_num);
    uint8_t bfr_data[18];
    memset(bfr_data, 0x00, sizeof(bfr_data));
    if (NULL == p_io_info) {
        return -AW_EINVAL;
    }
    aw_sdio_io_rw_direct(p_card,
                         0,
                         AWBL_SDIO_RD_DATA,
                         0,
                         bfr_addr,
                         bfr_data);

    p_io_info->fbr.interface          = bfr_data[0] & AWBL_SDIO_CLASS_MASK;
    p_io_info->fbr.ext_interface_code = bfr_data[1];
    p_io_info->fbr.csa_support        = bfr_data[0] & 0x40;
    p_io_info->fbr.csa_addr           = bfr_data[9] + bfr_data[10] * 256 + 
                                        bfr_data[11] * 65536;
    p_io_info->fbr.csa_access_window  = bfr_data[15];
    p_io_info->fbr.cis_addr           = bfr_data[12] + bfr_data[13] * 256 + 
                                        bfr_data[14] * 65536;
    p_io_info->fbr.blk_size           = bfr_data[16] + bfr_data[17] * 256;
    return AW_OK;
}


aw_local aw_err_t __sdio_read_cccr(struct awbl_sdio_device   *p_card,
                                   uint32_t                   ocr,
                                   struct awbl_sdio_cccr     *p_cccr)
{
    aw_err_t   ret;
    uint8_t    data;
    memset(p_cccr, 0x00, sizeof(*p_cccr));
    ret = aw_sdio_io_rw_direct(p_card,
                         0,
                         AWBL_SDIO_RD_DATA,
                         0,
                         AWBL_SDIO_ADDR_CCCR_REV,
                         &data);
    if (ret) {
        goto out;
    }
    p_cccr->cccr_vsn = data & 0x0f;
    if (p_cccr->cccr_vsn > SDIO_CCCR_REV_3_00) {
        return -AW_EINVAL;
    }
    p_cccr->sdio_vsn = (data & 0xf0) >> 4;
    ret = aw_sdio_io_rw_direct(p_card,
                         0,
                         AWBL_SDIO_RD_DATA,
                         0,
                         AWBL_SDIO_ADDR_CCCR_CAPABLITIES,
                         &data);

    if (ret) {
        goto out;
    }
    if (data & SDIO_CCCR_CAP_SMB)
        p_cccr->multi_block = 1;
    if (data & SDIO_CCCR_CAP_LSC)
        p_cccr->low_speed = 1;
    if (data & SDIO_CCCR_CAP_4BLS)
        p_cccr->wide_bus = 1;

    if (p_cccr->cccr_vsn >= SDIO_CCCR_REV_1_10) {
        ret = aw_sdio_io_rw_direct(p_card,
                             0,
                             AWBL_SDIO_RD_DATA,
                             0,
                             AWBL_SDIO_ADDR_CCCR_POWER_CONTROL,
                             &data);
        if (ret) {
            goto out;
        }
        if (data & 0x01)
            p_cccr->high_power = 1;
    }

    if (p_cccr->cccr_vsn >= SDIO_CCCR_REV_1_20) {
        ret = aw_sdio_io_rw_direct(p_card,
                             0,
                             AWBL_SDIO_RD_DATA,
                             0,
                             AWBL_SDIO_ADDR_CCCR_SPEED_CONTROL,
                             &data);
        if (ret) {
            goto out;
        }
        if (data & 0x01)
            p_cccr->high_speed = 1;
    }
out:
    return ret;
}


aw_local aw_err_t __sdio_cis_tpl_parse(struct awbl_sdio_device   *p_card,
                                       struct awbl_sdio_func     *p_func,
                                       const struct cis_tpl      *p_tpl,
                                       uint32_t                   tpl_cnt,
                                       uint8_t                    code,
                                       const uint8_t             *buf,
                                       uint8_t                    size)
{
    int i = 0;
    aw_err_t ret;

    if ((p_card == NULL) ||
        (p_tpl  == NULL) || (buf    == NULL)) {
        return -AW_EINVAL;
    }

    for (i = 0; i < tpl_cnt; i++, p_tpl++) {
        if (code == p_tpl->code){
            break;
        }
    }

    if (i < tpl_cnt) {
        if (size >= p_tpl->min_size) {
            if (p_tpl->parse)
                ret = p_tpl->parse(p_card, p_func, buf, size);
            else
                ret = -AW_EILSEQ;    /* known tuple, not parsed */
        } else {
            /* invalid tuple */
            ret = -AW_EINVAL;
        }
    } else {
        /* unknown tuple */
        ret = -AW_ENOENT;
    }

    return ret;
}


aw_local aw_err_t __sdio_cistpl_vers_1 (struct awbl_sdio_device   *p_card,
                                        struct awbl_sdio_func     *p_func,
                                        const uint8_t             *buf,
                                        uint8_t                    size)
{
    uint32_t i, nr_strings;
    const uint8_t **buffer;
    uint8_t  *string;

    /* 
     * Find all null-terminated (including zero length) strings in
     * the TPLLV1_INFO field. Trailing garbage is ignored. 
     */
    buf += 2;
    size -= 2;

    nr_strings = 0;
    for (i = 0; i < size; i++) {
        if (buf[i] == 0xff)
            break;
        if (buf[i] == 0)
            nr_strings++;
    }
    if (nr_strings == 0)
        return 0;

    size = i;

    buffer = __MEM_ALLOC_SIZE(sizeof(char*) * nr_strings + size);
    if (buffer == NULL) {
        AW_ERRF(("SDIO ERROR: vers_1 mem_alloc failed."));
        return -AW_ENOMEM;
    }
    memset(buffer, 0x00, sizeof(char*) * nr_strings + size);
    string = (char*)(buffer + nr_strings);
    for (i = 0; i < nr_strings; i++) {
        buffer[i] = string;
        strcpy(string, buf);
        string += strlen(string) + 1;
        buf += strlen(buf) + 1;
    }
    if (p_func) {
        p_func->num_info = nr_strings;
        p_func->info = (const uint8_t**)buffer;
    } else {
        p_card->num_info = nr_strings;
        p_card->info = (const uint8_t**)buffer;
    }
    return AW_OK;
}


aw_local aw_err_t __sdio_cistpl_manfid (struct awbl_sdio_device   *p_card,
                                        struct awbl_sdio_func     *p_func,
                                        const uint8_t             *buf,
                                        uint8_t                    size)
{
    uint32_t vendor, device;
    /* TPLMID_MANF */
    vendor = buf[0] | (buf[1] << 8);
    /* TPLMID_CARD */
    device = buf[2] | (buf[3] << 8);
    if (p_func) {
        p_func->cis.tplmid_manf = vendor;
        p_func->cis.tplmid_card = device;
    } else {
        p_card->io_info.cis.tplmid_manf = vendor;
        p_card->io_info.cis.tplmid_card = device;
    }

    return AW_OK;
}


aw_local aw_err_t __sdio_cistpl_funce (struct awbl_sdio_device   *p_card,
                                       struct awbl_sdio_func     *p_func,
                                       const uint8_t             *buf,
                                       uint8_t                    size)
{
    if (size < 1) {
        return -AW_EINVAL;
    }
    return __sdio_cis_tpl_parse(p_card,
                                p_func,
                                cis_tpl_funce_list,
                                AW_NELEMENTS(cis_tpl_funce_list),
                                buf[0],
                                buf,
                                size);
}


aw_local aw_err_t __sdio_cistpl_funce_common (struct awbl_sdio_device   *p_card,
                                              struct awbl_sdio_func     *p_func,
                                              const uint8_t             *buf,
                                              uint8_t                    size)
{
    /* Only valid for the common CIS (function 0) */
    if (p_func)
        return -AW_EINVAL;
    /* TPLFE_FN0_BLK_SIZE */
    p_card->io_info.cis.tplfe_max_blk_size = buf[1] | (buf[2] << 8);
    /* TPLFE_MAX_TRAN_SPEED */
    p_card->io_info.cis.tplfe_max_speed = speed_val[(buf[3] >> 3) & 15] *
                speed_unit[buf[3] & 7];
    return AW_OK;
}


aw_local aw_err_t __sdio_cistpl_funce_func (struct awbl_sdio_device   *p_card,
                                            struct awbl_sdio_func     *p_func,
                                            const uint8_t             *buf,
                                            uint8_t                    size)
{
    uint32_t vsn;
    uint32_t min_size;

    if (!p_func) {
        return -AW_EINVAL;
    }

    /* This tuple has a different length depending on the SDIO spec version. */
    vsn = p_card->io_info.cccr.sdio_vsn;
    min_size = (vsn == SDIO_SDIO_REV_1_00) ? 28 : 42;

    if (size == 28 && vsn == SDIO_SDIO_REV_1_10) {
        vsn = SDIO_SDIO_REV_1_00;
    } else if (size < min_size) {
        return -AW_EINVAL;
    }
    /* TPLFE_MAX_BLK_SIZE */
    p_func->cis.tplfe_max_blk_size = buf[12] | (buf[13] << 8);
    return AW_OK;
}


aw_local aw_err_t __sdio_read_cis(struct awbl_sdio_device   *p_card,
                                  struct awbl_sdio_func     *func)
{
    aw_err_t ret;
    struct sdio_func_tuple *this, **prev;
    uint32_t i   = 0;
    uint32_t ptr = 0;
    /*
     * Note that this works for the common CIS (function number 0) as
     * well as a function's CIS * since SDIO_CCCR_CIS and SDIO_FBR_CIS
     * have the same offset.
     */
    for (i = 0; i < 3; i++) {
        uint8_t x, fn;
        fn = func ? func->num : 0;
        ret = aw_sdio_io_rw_direct(p_card,
                             0,
                             AWBL_SDIO_RD_DATA,
                             0,
                             AWBL_SDIO_ADDR_CISPTR_FBR_F(fn) + i,
                             &x);
        if (ret) {
            return ret;
        }
        ptr |= x << (i * 8);
    }
    prev = func ? &func->tuples : &p_card->tuples;

    do {
        uint8_t tpl_code, tpl_link;
        ret = aw_sdio_io_rw_direct(p_card,
                             0,
                             AWBL_SDIO_RD_DATA,
                             0,
                             ptr++,
                             &tpl_code);
        if (ret) {
            break;
        }
        /* 0xff means we're done */
        if (tpl_code == 0xff) {
            break;
        }
        /* null entries have no link field or data */
        if (tpl_code == 0x00) {
            continue;
        }
        ret = aw_sdio_io_rw_direct(p_card,
                             0,
                             AWBL_SDIO_RD_DATA,
                             0,
                             ptr++,
                             &tpl_link);
        if (ret) {
            break;
        }
        /* a size of 0xff also means we're done */
        if (tpl_link == 0xff) {
            break;
        }
        this = __MEM_ALLOC_SIZE(sizeof(*this) + tpl_link);
        if (this == NULL) {
            AW_ERRF(("SDIO ERROR: cis_this mem_alloc failed."));
            return -AW_ENOMEM;
        }
        memset(this, 0x00, sizeof(*this) + tpl_link);

        for (i = 0; i < tpl_link; i++) {
            ret = aw_sdio_io_rw_direct(p_card,
                                 0,
                                 AWBL_SDIO_RD_DATA,
                                 0,
                                 ptr + i,
                                 &this->data[i]);
            if (ret) {
                break;
            }
        }
        if (ret) {
            __MEM_FREE_THIS(this);
            break;
        }

        ret = __sdio_cis_tpl_parse(p_card,
                                    func,
                                    cis_tpl_list,
                                    AW_NELEMENTS(cis_tpl_list),
                                    tpl_code,
                                    this->data,
                                    tpl_link);

        if (ret == -AW_EILSEQ || ret == -AW_ENOENT) {
            /*
             * The tuple is unknown or known but not parsed.
             * Queue the tuple for the function driver.
             */
            this->next = NULL;
            this->code = tpl_code;
            this->size = tpl_link;
            *prev = this;
            prev = &this->next;

            if (ret == -AW_ENOENT) {
                /* warn about unknown tuples */
                aw_kprintf("unknown CIS tuple 0x%02x (%u bytes)\n", 
                           tpl_code, 
                           tpl_link);
            }

            /* keep on analyzing tuples */
            ret = AW_OK;
        } else {
            /*
             * We don't need the tuple anymore if it was
             * successfully parsed by the SDIO core or if it is
             * not going to be queued for a driver.
             */
            __MEM_FREE_THIS(this);
        }

        ptr += tpl_link;
    } while (!ret);

    /*
     * Link in all unknown tuples found in the common CIS so that
     * drivers don't have to go digging in two places.
     */
#if 0
    if (func) {
        *prev = p_card->tuples;
    }
#endif 
    return ret;
}


aw_local aw_err_t __sdio_read_fbr(struct awbl_sdio_func *p_func)
{
    aw_err_t ret;
    unsigned char data;

    if ((p_func == NULL) || (p_func->p_card == NULL)) {
        return -AW_EINVAL;
    }

    ret = aw_sdio_io_rw_direct(p_func->p_card,
                         0,
                         AWBL_SDIO_RD_DATA,
                         0,
                         AWBL_SDIO_ADDR_INFO_FBR_F(p_func->num),
                         &data);
    if (ret) {
        goto out;
    }

    data &= 0x0f;
    if (data == 0x0f) {
        ret = aw_sdio_io_rw_direct(
                  p_func->p_card,
                  0,
                  AWBL_SDIO_RD_DATA,
                  0,
                  AWBL_SDIO_ADDR_EXTFUNC_CODE_FBR_F(p_func->num),
                 &data);

        if (ret)
            goto out;
    }
    p_func->class = data;  /* 0x07 wlan */
out:
    return ret;
}


aw_local aw_err_t __sdio_func_init(struct awbl_sdio_device  *p_card, uint8_t fn)
{
    aw_err_t err = 0;
    struct awbl_sdio_func *p_func = NULL;
    if ((p_card == NULL) || (fn > AWBL_SDIO_MAX_FUNC)) {
        return -AW_EINVAL;
    }

    p_func = __MEM_ALLOC_SIZE(sizeof(struct awbl_sdio_func));
    if (p_func == NULL) {
        AW_ERRF(("SDIO ERROR: p_func mem_alloc failed."));
        return -AW_ENOMEM;
    }
    memset(p_func, 0x00, sizeof(struct awbl_sdio_func));
    p_func->p_card = p_card;
    p_func->num    = fn;

    /* Get fun's class type */
    err = __sdio_read_fbr(p_func);
    if (err) {
        goto fail;
    }

    /* Get fun's CIS */
    err = __sdio_read_cis(p_card, p_func);
    if (err) {
        goto fail;
    }

    p_card->p_iofunc[fn - 1] = p_func;
    sprintf(p_func->name, "%s%d", p_func->p_card->name, p_func->num);

    return err;

fail:
    __MEM_FREE_SUB(p_func->info);
    __MEM_FREE_TUPLES(p_func->tuples);
    __MEM_FREE_THIS(p_func);
    return err;
}


aw_local aw_err_t __func_init_static(struct awbl_sdio_device  *p_card,
                                     struct awbl_sdio_func    *p_func)
{
    aw_err_t err   = 0;
    uint8_t  class = 0;
    struct awbl_sdio_dev_info *p_dev_info = NULL;
    if ((p_card == NULL) || (p_func->num > AWBL_SDIO_MAX_FUNC)) {
        return -AW_EINVAL;
    }

    p_dev_info = (struct awbl_sdio_dev_info *)
                            p_func->super.p_devhcf->p_devinfo;

    p_func->p_card = p_card;

    /**
     * Get fun's class type
     */
    err = __sdio_read_fbr(p_func);
    if (err) {
        goto fail;
    }

    /* compare the class */
    class = p_func->class;
    if ((class != AWBL_SDIO_CLASS_NONE) && (class != p_dev_info->class)) {
        goto fail;
    }

    /**
     * Get fun's CIS
     */
    err = __sdio_read_cis(p_card, p_func);
    if (err) {
        goto fail;
    }

    p_card->p_iofunc[p_func->num - 1] = p_func;

    return err;

fail:
    p_card->p_iofunc[p_func->num - 1] = NULL;
    return err;
}


aw_local void __sdio_host_dev_reset(struct awbl_sdio_host *p_host)
{
    /* host */
    awbl_sdio_host_reset(p_host);
    awbl_sdio_host_dummy_clks(p_host, 100);
    awbl_sdio_host_set_speed(p_host, 100000);
    awbl_sdio_host_set_bus_width(p_host, AWBL_SDIO_BUS_WIDTH_1B);

    /* dev */
    /* 先进行io reset将导致SPI模式下SD卡识别变慢  */
    aw_sdio_host_go_idle_state(p_host);
    aw_sdio_host_io_reset(p_host);
}


aw_local aw_err_t __sdio_memory_portion_init(awbl_sdio_memory_t *p_sdcard,
                                             uint32_t            ocr,
                                             uint8_t             pre_type)
{
    aw_err_t ret;
    uint32_t cid[4];
    uint32_t status;
    struct awbl_sdio_timeout_obj  timeout;

    if (p_sdcard == NULL) {
        return -AW_EINVAL;
    }

    struct awbl_sdio_host     *p_host     = p_sdcard->p_card->p_host;
    struct awbl_sdio_device   *p_card     = p_sdcard->p_card;
    awbl_sdio_mem_card_info_t *p_mem_info = &p_sdcard->mem_info;

    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_hostinfo, p_host);

    /* For spi read ocr */
    if (p_hostinfo->mode == AWBL_SDIO_SPI_M) {
        aw_sdio_host_go_idle_state(p_host);
        ret = aw_sdio_host_spi_read_ocr(p_host, 0, &ocr);
        if (ret) {
            goto fail;
        }
    }

    /* Initialization SDcard */
    aw_sdio_host_go_idle_state(p_host);

    /* check voltage and version */
    if (AW_OK == aw_sdio_host_send_if_cond (p_host, ocr)) {
        /* above V2.0 */
        p_mem_info->attribute |= __SDCARD_SDV2X;
        ocr |= AWBL_SD_OCR_CCS;
    } else {
        /* V1.0卡 */
        p_mem_info->attribute |= __SDCARD_SDV1X;
    }

    /* SD get ocr */
    if (pre_type == AWBL_SDIO_TYPE_SD) {
        awbl_sdio_timeout_set(&timeout, 1000);
        do {
            ret = aw_sdio_host_send_app_op_cond(p_host, 0, ocr, &ocr);
            if (ret != AW_OK) {
                aw_mdelay(1);
            }
        } while ((ret != AW_OK) && (!awbl_sdio_timeout(&timeout)) && 
                    !(p_host->removed));
        if (ret == AW_OK) {
            p_card->type = AWBL_SDIO_TYPE_SD;
        }

    /* MMC get ocr */
    } else if (pre_type == AWBL_SDIO_TYPE_MMC) {
        awbl_sdio_timeout_set(&timeout, 1000);
        do {
            ret = aw_sdio_host_send_op_cond(p_host, ocr | (1 << 30), &ocr);
            if (ret != AW_OK) {
                aw_mdelay(1);
            }
        } while ((ret != AW_OK) && (!awbl_sdio_timeout(&timeout)) && 
                    !(p_host->removed));

        if (ret == AW_OK) {
            p_card->type           = AWBL_SDIO_TYPE_MMC;
            p_mem_info->attribute &= ~__SDCARD_VERSION;
            p_mem_info->attribute |= __SDCARD_MMC;
        } else {
            goto fail;
        }
    }

    ocr &= 0x61ffffff;
    if (ocr & AWBL_SD_OCR_CCS) {
        p_mem_info->attribute |= __SDCARD_SDHC;
    } else {
        p_mem_info->attribute |= __SDCARD_SDSC_MMC;
    }

    /* voltage_witch */
    if ((p_hostinfo->mode != AWBL_SDIO_SPI_M) &&
        ((ocr & 0x41000000) == 0x41000000)) {
        ret = aw_sdio_voltage_witch(p_card);
        if (ret != AW_OK) {
            goto fail;
        }
    }

    /* Fetch CID from card. */
    if (p_hostinfo->mode == AWBL_SDIO_SPI_M) {
        ret = aw_sdio_send_cid(p_host->p_card, 0, cid);
    } else {
        ret = aw_sdio_all_send_cid(p_host->p_card, cid);
    }
    if (ret) {
        goto fail;
    }

    memcpy(p_sdcard->raw_cid, cid, sizeof(p_sdcard->raw_cid));
    p_mem_info->ocr = ocr;

    /* Get rca */
    if (p_hostinfo->mode != AWBL_SDIO_SPI_M){
        if (p_mem_info->attribute & __SDCARD_MMC) {
            p_mem_info->rca = 1;
            ret = aw_sdio_mmc_set_relative_addr(p_card, p_mem_info->rca);                                        
            if (ret != AW_OK) {
                goto fail;
            }
        } else {
            ret = aw_sdio_send_relative_addr(p_card, &(p_mem_info->rca));
            if (ret != AW_OK) {
                goto fail;
            }
        }
        p_card->rca = p_mem_info->rca;
    }

    /* Fetch CSD from card. */
    ret = aw_sdio_send_csd(p_card, p_card->rca, p_sdcard->raw_csd);
    if (ret) {
        goto fail;
    }
    ret = __sdcard_decode_csd(p_mem_info, p_sdcard->raw_csd);
    if (ret) {
        goto fail;
    }
    ret = __sdcard_decode_cid(p_mem_info, p_sdcard->raw_cid);
    if (ret) {
        goto fail;
    }

    /* Select card, as all following commands rely on that. */
    if (p_hostinfo->mode != AWBL_SDIO_SPI_M) {
        ret = aw_sdio_select_card(p_card, p_card->rca, 1);
        if (ret) {
            goto fail;
        }
    }

    /* get MMC expand CSD */
    if ((p_mem_info->attribute & __SDCARD_MMC) &&
        (p_mem_info->csd.mmca_vsn > __CSD_SPEC_VER_3)) {
        uint32_t p_ext_csd[128];
        ret = aw_sdio_mmc_send_ext_csd(
                                    p_card,
                                    p_ext_csd);
        if (ret == AW_OK) {

            ret = __sdcard_decode_ext_csd(
                                    p_mem_info,
                                    p_ext_csd);
            if (ret != AW_OK) {
                goto fail;
            }
        } else {
            goto fail;
        }
    }

    /* get the status of the card */
    ret = aw_sdio_send_status(p_card, p_card->rca, &status);
    if (ret != AW_OK) {
        goto fail;
    }
    if (p_hostinfo->mode != AWBL_SDIO_SPI_M) {
        if (AWBL_SD_R1_STATE_TRAN != AWBL_SD_R1_CURRENT_STATE(status)) {
            ret = AW_ERROR;
            goto fail;
        }
    } else {
        if (((AWBL_SD_SPI_R1_ALL_ERROR<<8) |
             AWBL_SD_SPI_R2_CC_ERROR |
             AWBL_SD_SPI_R2_CARD_ECC_ERROR) & status) {
            ret = AW_ERROR;
            goto fail;
        }
    }

    /* set sector size */
    if ((p_mem_info->attribute & __SDCARD_SDSC_MMC) ||
        (p_mem_info->attribute & __SDCARD_MMC)) {
        ret = aw_sdio_set_blocklen(p_card,
                                   p_mem_info->csd.sector_size);
        if (ret != AW_OK) {
            goto fail;
        }
    }

    /* get scr ssr  （MMC not have scr ssr） */
    if (!(p_mem_info->attribute & __SDCARD_MMC)) {
         /* get SCR */
        ret = aw_sdio_app_send_scr(p_card,
                                    p_mem_info->rca,
                                    p_sdcard->raw_scr);
         if (ret != AW_OK) {
             goto fail;
         }

         ret = __sdcard_decode_scr(p_mem_info, p_sdcard->raw_scr);
         if (ret != AW_OK) {
             goto fail;
         }

         uint32_t buf[16];
         /* get SSR */
         ret = aw_sdio_app_sd_status(p_card,
                                     p_mem_info->rca,
                                     buf);
         if (ret != AW_OK) {
             goto fail;
         }
         ret = __sdcard_decode_ssr(p_mem_info, buf);
         if (ret != AW_OK) {
             goto fail;
         }

         if (p_mem_info->scr.sda_vsn == 2) {
             p_mem_info->csd.block_size = p_mem_info->ssr.au_size;
         }

         /* switch */
         ret = aw_sdio_sd_switch(p_card,
                                 0,
                                 0,
                                 0,
                                 (uint8_t *)buf);
         if (ret != AW_OK) {
             goto fail;
         }
     } else {

     }

    /* For SPI enable CRC */
    if (p_hostinfo->mode == AWBL_SDIO_SPI_M) {
        if (p_hostinfo->crc_en) {
            ret = aw_sdio_spi_set_crc(p_card, 1);
            if (ret != AW_OK) {
                goto fail;
            }
        }
    }

    /* Switch bus width */
    if (!(p_mem_info->attribute & __SDCARD_MMC)) {
        if (p_hostinfo->mode == AWBL_SDIO_SD_4B_M) {
            if (p_mem_info->scr.bus_widths & AW_SD_SCR_BUS_WIDTH_4) {
                ret = aw_sdio_app_set_bus_width(
                                        p_card,
                                        p_mem_info->rca,
                                        AWBL_SDIO_BUS_WIDTH_4B);
                if (ret != AW_OK) {
                    goto fail;
                }
                ret = awbl_sdio_host_set_bus_width(
                                    p_host,
                                    AWBL_SDIO_BUS_WIDTH_4B);

                if (ret != AW_OK) {
                    goto fail;
                }
            }
        }

    } else {
        uint8_t width = 0;
        if (p_hostinfo->mode != AWBL_SDIO_SPI_M) {
            if (p_hostinfo->mode == AWBL_SDIO_SD_4B_M) {
                width = AWBL_SDIO_BUS_WIDTH_4B;
            } else if (p_hostinfo->mode == AWBL_SDIO_SD_8B_M) {
                width = AWBL_SDIO_BUS_WIDTH_8B;
            } else if (p_hostinfo->mode == AWBL_SDIO_SD_1B_M){
                width = AWBL_SDIO_BUS_WIDTH_1B;
            } else {
                ret = AW_ERROR;
                goto fail;
            }
            ret = aw_sdio_mmc_switch(p_card,
                                     0x03,
                                     0xBB,  /* 187 POWER_CLASS */
                                     0x08,
                                     1);
            if (ret != AW_OK) {
                goto fail;
            }

            /* switch high speed */
            ret = aw_sdio_mmc_switch(p_card,
                                     0x03,
                                     0xB9,  /* 185  HS_TIMING */
                                     0x01,
                                     1);
            if (ret != AW_OK) {
                goto fail;
            }

            /* switch bus width */
            ret  = aw_sdio_mmc_buswidth_switch(p_card, width);
#if 0 
            ret  = aw_sdio_mmc_switch(p_card,
                                      3,
                                      183,
                                      2,
                                      1);
#endif
            if (ret != AW_OK) {
                goto fail;
            }
            ret = awbl_sdio_host_set_bus_width(
                                p_host,
                                width);

            if (ret != AW_OK) {
                goto fail;
            }
        }
    }

    /* set bus speed */
    awbl_sdio_host_set_speed(p_host,
                             p_mem_info->csd.max_tr_speed);
    return AW_OK;
fail:
    return ret;
}


aw_local aw_err_t __sdio_io_portion_init(awbl_sdio_function_t *p_func,
                                         uint32_t              ocr)
{
    if (p_func == NULL) {
        return -AW_EINVAL;
    }
    /*  TODO 后期修改为单个function初始化实现 */
    return aw_sdio_dynamic_sdio_enum(p_func->p_card->p_host, ocr, p_func);
}



#ifdef _SPEED_TEST
aw_local void __sd_speed_test(struct awbl_sdio_device   *p_card)
{
#define NMTEST 2
    static uint8_t testbuf[NMTEST*1024] = {0};

    aw_err_t ret;
    {
    aw_tick_t ticks;
    uint32_t  i;
    float speed = 0.0;
    for (i = 0; i < sizeof(testbuf); i++) {
        testbuf[i] = 0x55 & 0xFF;
    }
    ticks = aw_sys_tick_get();


    ret = aw_sdio_write_multiple_block(
                        p_card,
                        0,
                        testbuf,
                        sizeof(testbuf) / 512,
                        512,
                        AW_TRUE);
    if (ret != 0) {
        AW_INFOF(("Test:write data to sectors failed.\r\n"));
        return;
    } else {
        ticks = aw_sys_tick_get() - ticks;
        AW_INFOF(("time:%d %d\r\n", ticks, aw_ticks_to_ms(ticks)));
        ticks = aw_ticks_to_ms(ticks);
        speed = NMTEST*1000.0/ticks;
        AW_INFOF(("Test:write data speed: %f MB/s\r\n",speed));
    }

    memset(testbuf, 0x00, sizeof(testbuf));
    ticks        = aw_sys_tick_get();

    ret = aw_sdio_read_multiple_block(
                        p_card,
                        0,
                        testbuf,
                        sizeof(testbuf) / 512,
                        512,
                        AW_TRUE);
    if (ret != 0) {
        AW_INFOF(("Test:read data from sectors failed.\r\n"));
    } else {
        ticks = aw_sys_tick_get() - ticks;
        AW_INFOF(("time:%d %d\r\n", ticks, aw_ticks_to_ms(ticks)));
        ticks = aw_ticks_to_ms(ticks);
        speed = NMTEST*1000.0/ticks;
        AW_INFOF(("Test:read data speed: %f MB/s\r\n",speed));

        uint32_t j;
        for (j = 0; j < sizeof(testbuf); j++) {
            if (testbuf[j] != (j & 0xFF)) {
                AW_INFOF(("Test:data verify failed at %d.\r\n", i));
                break;
            }
        }
    }

    }
}
#endif




/******************************************************************************/
aw_err_t awbl_sdio_host_reset (struct awbl_sdio_host *p_host)
{
    aw_err_t err;

    AWBL_HOST_TO_HSOST_DRV_INFO_DECL(p_drvinfo, p_host);


    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    err = __sdio_host_lock(p_host);
    if (AW_OK != err) {
        return err;
    }

    if (p_drvinfo->host_reset) {
        p_drvinfo->host_reset(p_host);
    } else {
        err = -AW_ENOTSUP;
    }

    __sdio_host_unlock(p_host);
    return err;
}

/******************************************************************************/
aw_err_t awbl_sdio_host_dummy_clks (struct awbl_sdio_host  *p_host,
                                    uint8_t                 clks)
{
    aw_err_t err;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    AWBL_HOST_TO_HSOST_DRV_INFO_DECL(p_drvinfo, p_host);

    err = __sdio_host_lock(p_host);
    if (AW_OK != err) {
        return err;
    }

    if (p_drvinfo->dummy_clks) {
        err = p_drvinfo->dummy_clks(p_host, clks);
    } else {
        err = -AW_ENOTSUP;
    }

    __sdio_host_unlock(p_host);

    return err;
}

/******************************************************************************/
aw_err_t awbl_sdio_host_set_speed (struct awbl_sdio_host *p_host,
                                   uint32_t               speed)
{
    aw_err_t err;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    AWBL_HOST_TO_HSOST_DRV_INFO_DECL(p_drvinfo, p_host);

    err = __sdio_host_lock(p_host);
    if (AW_OK != err) {
        return err;
    }

    if (p_drvinfo->set_speed) {
        err = p_drvinfo->set_speed(p_host, speed);
    } else {
        err = -AW_ENOTSUP;
    }
    __sdio_host_unlock(p_host);

    return err;
}

/******************************************************************************/
aw_err_t awbl_sdio_host_set_bus_width (struct awbl_sdio_host *p_host,
                                       awbl_sdio_bus_width_t  width)
{
    aw_err_t err;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    AWBL_HOST_TO_HSOST_DRV_INFO_DECL(p_drvinfo, p_host);

    err = __sdio_host_lock(p_host);
    if (AW_OK != err) {
        return err;
    }

    if (p_drvinfo->set_bus_width) {
        err = p_drvinfo->set_bus_width(p_host, width);
    } else {
        err = -AW_ENOTSUP;
    }
    __sdio_host_unlock(p_host);

    return err;
}

/******************************************************************************/
aw_err_t awbl_sdio_host_get_port_status (struct awbl_sdio_host *p_host,
                                         uint8_t               *p_status)
{
    aw_err_t err;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    AWBL_HOST_TO_HSOST_DRV_INFO_DECL(p_drvinfo, p_host);

    if (p_drvinfo->status_get) {
        err = p_drvinfo->status_get(p_host, p_status);
    } else {
        err = -AW_ENOTSUP;
    }

    return err;
}

/******************************************************************************/
aw_err_t awbl_sdio_host_async (struct awbl_sdio_host      *p_host,
                               struct awbl_sdio_msg       *p_msg,
                               void                       *p_arg,
                               void (*pfn_complete) (void *p_arg))
{
    if ((p_host == NULL) ||
        (p_msg == NULL)) {
        return -AW_EINVAL;
    }

    if (p_host->lock ) {
        return -AW_EBUSY;
    }

    AWBL_HOST_TO_HSOST_DRV_INFO_DECL(p_drvinfo, p_host);

    /* initialize */
    p_msg->p_arg    = p_arg;
    p_msg->pfn_complete = pfn_complete;
    p_msg->status   = -AW_ENOTCONN;
    /* add message into list */
    awbl_sdio_msg_in(p_host, p_msg);

    /* startup transfer */
    if (p_drvinfo->xfr) {
        return p_drvinfo->xfr(p_host);
    } else {
        return -AW_ENOTSUP;
    }
}

/******************************************************************************/
aw_err_t awbl_sdio_host_sync (struct awbl_sdio_host *p_host,
                              struct awbl_sdio_msg  *p_msg,
                              int                    timeout)
{
    aw_err_t     err;
    AW_SEMB_DECL(sem_sync);
    aw_semb_id_t sem_sync_id;

    if (NULL == p_host ||
        NULL == p_msg) {
        return -AW_EINVAL;
    }

    AWBL_HOST_TO_HSOST_DRV_INFO_DECL(p_drvinfo, p_host);
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_hostinfo, p_host);

    /* 加入设备在位判断 */
    if (p_host->removed) {
        return -AW_ENODEV;
    }

    (void)sem_sync;
    memset(&sem_sync,0,sizeof(sem_sync));
    sem_sync_id = AW_SEMB_INIT(sem_sync,
                               0,
                               AW_SEM_Q_FIFO);
    if (sem_sync_id == 0) {
        return -AW_ENOMEM;
    }

    /* send message and wait for complete */
    err = awbl_sdio_host_async(p_host,
                               p_msg,
                               sem_sync_id,
                               __sdio_sync_complete);
    if (err == AW_OK) {
        do {
            err = aw_semb_take(sem_sync_id, timeout);
            if (err == AW_OK) {
                err = p_msg->status;
                break;
            } else {
                AW_INFOF(("sdio: sync trans get semb error.\r\n"));
#if 0 
                if (p_drvinfo->pfn_cancel) {
                    p_drvinfo->pfn_cancel(p_host, p_msg);
                }
#endif 
            }
        } while (err != AW_OK);
    }

    /* release the SEM */
    aw_semb_terminate(sem_sync_id);

    /* trans error statistics */
#ifdef SDIO_TRANS_ERRCNT_ENABLE
    static void *p_temp = NULL;
    if (p_host->state == AW_SDIO_DEV_STAT_DEVREGIST) {
        if ((p_temp == (void *)p_host) && (err == AW_OK) && (p_host->err_cnt > 0)) {
            p_host->err_cnt = 0;
        }else if ((err != AW_OK) && (p_hostinfo->connect_type == 
                                         AWBL_SDIO_CON_DEV_DYNAMIC)) {
            aw_spinlock_isr_take(&(p_host->dev_lock));
            p_host->err_cnt++;
            if (p_host->err_cnt >= SDIO_TRANS_ERRCNT_LIMIT) {
                p_host->removed = AW_TRUE;
            }
            aw_spinlock_isr_give(&(p_host->dev_lock));
        }
        p_temp = (void *)p_host;
    }
#endif


    return err;
}

/* MMC + SD operation */
/******************************************************************************/
aw_err_t aw_sdio_go_idle_state (struct awbl_sdio_device *p_card)
{
    return aw_sdio_host_go_idle_state(p_card->p_host);
}

aw_err_t aw_sdio_host_go_idle_state (awbl_sdio_host_t *p_host)
{
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_host);

    struct awbl_sdio_msg  msg;
    struct awbl_sdio_cmd  cmd;
    uint32_t              rsp[4];
    aw_err_t              err;
    uint8_t               rsp_type;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    rsp_type = (p_info->mode == AWBL_SDIO_SPI_M) ?
                       AWBL_SDIO_RSP_R1 : AWBL_SDIO_RSP_NONE;

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD0,
                       0,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       rsp_type,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        if ((rsp[0] & (AWBL_SD_SPI_R1_ALL_ERROR)) != 0) {
            return -AW_EIO;
        }
    }
    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_send_if_cond (struct awbl_sdio_device *p_card,
                               uint32_t                 ocr)
{
    return aw_sdio_host_send_if_cond(p_card->p_host, ocr);
}

aw_err_t aw_sdio_host_send_if_cond (struct awbl_sdio_host *p_host,
                                       uint32_t                 ocr)
{
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_host);
    struct awbl_sdio_msg    msg;
    struct awbl_sdio_cmd    cmd;
    uint32_t                rsp[4];
    uint32_t                arg;
    aw_err_t                err;
    uint8_t                 pattern;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    arg = ((ocr & 0xFF8000) != 0) << 8 | 0xAA;

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD8,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R7,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        if ((rsp[0] & (AWBL_SD_SPI_R1_ALL_ERROR)) != 0) {
            return -AW_EIO;
        }
    }

    pattern = (p_info->mode == AWBL_SDIO_SPI_M) ?
                                rsp[1] : rsp[0];

    if (pattern != 0xAA) {
        return -AW_EIO;
    }
    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_mmc_send_ext_csd (struct awbl_sdio_device *p_card,
                                   uint32_t                *p_ext_csd)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg    msg;
    struct awbl_sdio_cmd    cmd;
    uint32_t                rsp[4];
    aw_err_t                err;
#if 0 
    uint8_t                 i;
    uint32_t                temp;
#endif 
    if (NULL == p_card ||
        NULL == p_ext_csd) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD8,
                       0,
                       AWBL_SDIO_RD_DATA,
                       p_ext_csd,
                       512,
                       1,
                       AWBL_SDIO_RSP_R1,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        if ((rsp[0] & (AWBL_SD_SPI_R1_ALL_ERROR)) != 0) {
            return -AW_EIO;
        }
    } else {
        if ((rsp[0] & 0xFFFF0000) != 0) {
            return -AW_EIO;
        }
    }
#if 0 
    for (i=0; i<64; i++) {
        temp           = __SDIO_SPI_SWAB32(p_ext_csd[i]);
        p_ext_csd[i]     = __SDIO_SPI_SWAB32(p_ext_csd[127-i]);
        p_ext_csd[127-i] = temp;
    }
#endif 
    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_send_app_op_cond (struct awbl_sdio_device *p_card,
                                   uint32_t                 rca,
                                   uint32_t                 ocr,
                                   uint32_t                *p_rocr)
{
    return aw_sdio_host_send_app_op_cond(p_card->p_host, rca, ocr, p_rocr);
}

aw_err_t aw_sdio_host_send_app_op_cond (struct awbl_sdio_host   *p_host,
                                        uint32_t                 rca,
                                        uint32_t                 ocr,
                                        uint32_t                *p_rocr)
{
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_host);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd1;
    struct awbl_sdio_cmd cmd2;
    uint32_t             rsp1[4];
    uint32_t             rsp2[4];
    aw_err_t             err;
    uint32_t             arg;
    uint8_t              rsp_type;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    arg = (p_info->mode != AWBL_SDIO_SPI_M) ?
                            (rca << 16) : 0;

    aw_sdio_config_cmd(&cmd1,
                       AWBL_SDIO_CMD55,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R1,
                       rsp1,
                       5);

    aw_sdio_cmd_add_tail(&msg, &cmd1);

    arg = (p_info->mode == AWBL_SDIO_SPI_M) ?
                            (ocr & (1 << 30)) : ocr;

    rsp_type = (p_info->mode == AWBL_SDIO_SPI_M) ? AWBL_SDIO_RSP_R1 : 
                    AWBL_SDIO_RSP_R3;

    aw_sdio_config_cmd(&cmd2,
                       AWBL_SDIO_ACMD41,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       rsp_type,
                       rsp2,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd2);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    /* check if enter APP CMD mode */
    if ((p_info->mode != AWBL_SDIO_SPI_M) &&
        !(rsp1[0] & AWBL_SD_R1_APP_CMD)) {
        return -AW_EIO;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        if (rsp2[0] & AWBL_SD_SPI_R1_IN_IDLE_STATE) {
            if (rsp2[0] & AWBL_SD_SPI_R1_ALL_ERROR) {
                return -AW_EIO;
            } else {
                err = ocr ? -AW_EBUSY : AW_OK;
                return err;
            }
        }
    } else {
        if ((!(rsp2[0] & 0x80000000)) && ocr) {
            return -AW_EBUSY;
        }
        if (p_rocr != NULL) {
            *p_rocr = rsp2[0];
        }
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_host_spi_read_ocr (struct awbl_sdio_host   *p_host,
                                    uint8_t                  highcap,
                                    uint32_t                *p_ocrp)
{
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_host);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;
    uint32_t             arg;

    if (NULL == p_host ||
        NULL == p_ocrp) {
        return -AW_EINVAL;
    }

    if (p_info->mode != AWBL_SDIO_SPI_M) {
        /* just SPI mode */
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    arg = highcap ? (1 << 30) : 0;

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD58,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R3,
                       rsp,
                       5);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    *p_ocrp = ((rsp[0]<<16) & 0xFF000000) |
               (rsp[0] & 0x00FF0000) |
              ((rsp[0]>>16) & 0x0000FF00) |
               (rsp[1] & 0x000000FF);

    return AW_OK;
}

aw_err_t aw_sdio_spi_read_ocr (struct awbl_sdio_device *p_card,
                               uint8_t                  highcap,
                               uint32_t                *p_ocrp)
{
    return aw_sdio_host_spi_read_ocr(p_card->p_host, highcap, p_ocrp);
}

/******************************************************************************/
aw_err_t aw_sdio_send_cid (struct awbl_sdio_device *p_card,
                           uint32_t                 rca,
                           uint32_t                *p_cid)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_cid) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);
    if (p_info->mode == AWBL_SDIO_SPI_M) {
        aw_sdio_config_cmd(&cmd,
                           AWBL_SDIO_CMD10,
                           0,
                           AWBL_SDIO_RD_DATA,
                           p_cid,
                           16,
                           1,
                           AWBL_SDIO_RSP_R1,
                           rsp,
                           0);
    } else {
        aw_sdio_config_cmd(&cmd,
                           AWBL_SDIO_CMD10,
                           (rca << 16),
                           AWBL_SDIO_NO_DATA,
                           NULL,
                           0,
                           0,
                           AWBL_SDIO_RSP_R2,
                           rsp,
                           0);
    }
    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        if ((rsp[0] & AWBL_SD_SPI_R1_ALL_ERROR) != 0) {
            return -AW_EIO;
        } else {
            uint8_t i;
            uint32_t temp;
            for (i=0; i<2; i++) {
                temp       = __SDIO_SPI_SWAB32(p_cid[i]);
                p_cid[i]   = __SDIO_SPI_SWAB32(p_cid[3-i]);
                p_cid[3-i] = temp;
            }
        }
    } else {
        memcpy(p_cid, rsp, 16);
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_all_send_cid (struct awbl_sdio_device *p_card,
                               uint32_t                *p_cid)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_cid) {
        return -AW_EINVAL;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        /* just SPI mode */
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD2,
                       0,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R2,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    memcpy(p_cid, rsp, 16);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_send_relative_addr (struct awbl_sdio_device *p_card,
                                     uint32_t                *p_rca)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_rca) {
        return -AW_EINVAL;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        /* just SPI mode */
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD3,
                       0,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R6,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    *p_rca = rsp[0]>>16;
    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_mmc_set_relative_addr (struct awbl_sdio_device *p_card,
                                        uint32_t                 rca)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        /* just SPI */
        return -AW_EINVAL;
    }

    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD3,
                       (rca << 16),
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R1,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_send_csd (struct awbl_sdio_device *p_card,
                           uint32_t                 rca,
                           uint32_t                *p_csd)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_csd) {
        return -AW_EINVAL;
    }


    aw_sdio_msg_init(&msg);
    if (p_info->mode == AWBL_SDIO_SPI_M) {
        aw_sdio_config_cmd(&cmd,
                           AWBL_SDIO_CMD9,
                           0,
                           AWBL_SDIO_RD_DATA,
                           p_csd,
                           16,
                           1,
                           AWBL_SDIO_RSP_R1,
                           rsp,
                           0);

    } else {
        aw_sdio_config_cmd(&cmd,
                           AWBL_SDIO_CMD9,
                           (rca << 16),
                           AWBL_SDIO_NO_DATA,
                           NULL,
                           0,
                           0,
                           AWBL_SDIO_RSP_R2,
                           rsp,
                           0);
    }
    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        if ((rsp[0] & AWBL_SD_SPI_R1_ALL_ERROR) != 0) {
            return -AW_EIO;
        } else {
            uint8_t  i;
            uint32_t temp;
            for (i = 0; i < 2; i++) {
                temp       = __SDIO_SPI_SWAB32(p_csd[i]);
                p_csd[i]   = __SDIO_SPI_SWAB32(p_csd[3-i]);
                p_csd[3-i] = temp;
            }
        }
    } else {
        memcpy(p_csd, rsp, 16);
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_select_card (struct awbl_sdio_device  *p_card,
                              uint32_t                  rca,
                              uint8_t                   select)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;
    uint32_t             arg;
    uint8_t              rsp_type;

    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        /* just SPI */
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    arg = select ? (rca << 16) : 0;

    rsp_type = select ? AWBL_SDIO_RSP_R1 : AWBL_SDIO_RSP_NONE;

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD7,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       rsp_type,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_app_send_scr (struct awbl_sdio_device  *p_card,
                               uint32_t                  rca,
                               uint32_t                 *p_scr)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd1;
    struct awbl_sdio_cmd cmd2;
    uint32_t             rsp1[4];
    uint32_t             rsp2[4];
    aw_err_t             err;
    uint32_t             arg;
    uint32_t             temp;

    if (NULL == p_card ||
        NULL == p_scr) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    arg = (p_info->mode != AWBL_SDIO_SPI_M) ? (rca << 16) : 0;

    aw_sdio_config_cmd(&cmd1,
                       AWBL_SDIO_CMD55,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R1,
                       rsp1,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd1);

    aw_sdio_config_cmd(&cmd2,
                       AWBL_SDIO_ACMD51,
                       0,
                       AWBL_SDIO_RD_DATA,
                       p_scr,
                       8,
                       1,
                       AWBL_SDIO_RSP_R1,
                       rsp2,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd2);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if ((p_info->mode != AWBL_SDIO_SPI_M) &&
        !(rsp1[0] & AWBL_SD_R1_APP_CMD)) {
        return -AW_EIO;
    }

    temp     = __SDIO_SPI_SWAB32(p_scr[0]);
    p_scr[0] = __SDIO_SPI_SWAB32(p_scr[1]);
    p_scr[1] = temp;

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_app_sd_status (struct awbl_sdio_device *p_card,
                                uint32_t                 rca,
                                uint32_t                *p_ssr)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    uint32_t             arg;
    aw_err_t             err;
    uint8_t              rsp_type;
    uint8_t              i;
    uint32_t             temp;


    if (NULL == p_card ||
        NULL == p_ssr) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    arg = (p_info->mode != AWBL_SDIO_SPI_M) ? (rca << 16) : 0;

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD55,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R1,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    aw_mdelay(20);

    aw_sdio_msg_init(&msg);

    rsp_type = (p_info->mode == AWBL_SDIO_SPI_M) ? AWBL_SDIO_RSP_R2 : 
                   AWBL_SDIO_RSP_R1;

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_ACMD13,
                       0,
                       AWBL_SDIO_RD_DATA,
                       p_ssr,
                       64,
                       1,
                       rsp_type,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if ((p_info->mode != AWBL_SDIO_SPI_M) &&
        !(rsp[0] & AWBL_SD_R1_APP_CMD)) {
        return -AW_EIO;
    }

    for (i=0; i<8; i++) {
        temp        = __SDIO_SPI_SWAB32(p_ssr[i]);
        p_ssr[i]    = __SDIO_SPI_SWAB32(p_ssr[15-i]);
        p_ssr[15-i] = temp;
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_sd_switch (struct awbl_sdio_device *p_card,
                            int                      mode,
                            int                      group,
                            uint8_t                  value,
                            uint8_t                 *p_resp)
{
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;
    uint32_t             arg;

    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);
    arg =  mode << 31 | 0x00FFFFFF;
    arg &= ~(0xF << (group * 4));
    arg |= value << (group * 4);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD6,
                       arg,
                       AWBL_SDIO_RD_DATA,
                       p_resp,
                       64,
                       1,
                       AWBL_SDIO_RSP_R1,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_mmc_switch (struct awbl_sdio_device *p_card,
                             uint8_t                  access,
                             uint8_t                  index,
                             uint8_t                  value,
                             uint8_t                  cmd_set)
{
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;
    uint32_t             arg;

    aw_sdio_msg_init(&msg);


    arg = (((uint32_t)access  & 0xFF) << 24) |
          (((uint32_t)index   & 0xFF) << 16) |
          (((uint32_t)value   & 0xFF) << 8) |
          (((uint32_t)cmd_set & 0x03) << 0);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD6,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R1B,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_mmc_buswidth_switch (struct awbl_sdio_device *p_card,
                                      awbl_sdio_bus_width_t    width)
{
    uint8_t value = (width & 0x3);

    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    return aw_sdio_mmc_switch(p_card, 3, 183, value, 0);
}

/******************************************************************************/
aw_err_t aw_sdio_mmc_bus_select_hs_ddr (struct awbl_sdio_device *p_card)
{
    aw_err_t ret = 0;

    /* switch high speed */
    ret = aw_sdio_mmc_switch(p_card,
                             0x03,
                             185,
                             0x01,
                             0);


    ret = aw_sdio_mmc_switch(p_card,
                             0x03, /* access */
                             183,  /* index */
                             0x06, /* value */
                             0);   /* set */

    return ret;

}

/******************************************************************************/
aw_err_t aw_sdio_spi_set_crc(struct awbl_sdio_device *p_card,
                             uint8_t                  crc_en)
{
    aw_err_t             err;

    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    err = aw_sdio_host_spi_set_crc(p_card->p_host, crc_en);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_app_set_bus_width (struct awbl_sdio_device *p_card,
                                    uint32_t                 rca,
                                    awbl_sdio_bus_width_t    width)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd1;
    struct awbl_sdio_cmd cmd2;
    uint32_t             rsp1[4];
    uint32_t             rsp2[4];
    uint32_t             arg;
    aw_err_t             err;

    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    arg = (p_info->mode != AWBL_SDIO_SPI_M) ? (rca << 16) : 0;

    aw_sdio_config_cmd(&cmd1,
                       AWBL_SDIO_CMD55,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R1,
                       rsp1,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd1);

    if ((p_info->mode == AWBL_SDIO_SD_1B_M) &&
        (width == AWBL_SDIO_BUS_WIDTH_1B)) {
        arg = 0;
    } else if ((p_info->mode == AWBL_SDIO_SD_4B_M) &&
               (width == AWBL_SDIO_BUS_WIDTH_4B)) {
        arg = 2;
    } else {
        return -AW_EINVAL;
    }

    aw_sdio_config_cmd(&cmd2,
                       AWBL_SDIO_ACMD6,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R1,
                       rsp2,
                       0);


    aw_sdio_cmd_add_tail(&msg, &cmd2);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if ((p_info->mode != AWBL_SDIO_SPI_M) &&
        !(rsp1[0] & AWBL_SD_R1_APP_CMD)) {
        return -AW_EIO;
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_voltage_witch (struct awbl_sdio_device *p_card)
{
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);


    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        return -AW_EINVAL;
    }
    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD11,
                       0,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R1,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (rsp[0] & AWBL_SD_R1_ERROR) {
        return -AW_EIO;
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_set_blocklen (struct awbl_sdio_device *p_card,
                               uint32_t                 blk_len)
{
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;

    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD16,
                       blk_len,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R1,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_read_single_block (struct awbl_sdio_device *p_card,
                                    uint32_t                 sector,
                                    uint8_t                 *p_buf,
                                    uint16_t                 sector_size)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_buf) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD17,
                       sector,
                       AWBL_SDIO_RD_DATA,
                       p_buf,
                       sector_size,
                       1,
                       AWBL_SDIO_RSP_R1,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        if ((rsp[0] & (AWBL_SD_SPI_R1_ALL_ERROR |
                       AWBL_SD_SPI_R1_IN_IDLE_STATE)) != 0) {
            return -AW_EIO;
        }
    } else {
        if ((rsp[0] & 0xFFFF0000) != 0) {
            return -AW_EIO;
        }
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_read_single_block_async (struct awbl_sdio_device *p_card,
                                          uint32_t                 sector,
                                          uint8_t                 *p_buf,
                                          uint16_t                 sector_size,
                                          uint32_t                *p_rsp,
                                          void                    *p_arg,
                                          void  (*pfn_complete) (void *p_arg))
{
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_buf  ||
        NULL == pfn_complete) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD17,
                       sector,
                       AWBL_SDIO_RD_DATA,
                       p_buf,
                       sector_size,
                       1,
                       AWBL_SDIO_RSP_R1,
                       p_rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_async(p_host, &msg, p_arg, pfn_complete);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_write_single_block (struct awbl_sdio_device *p_card,
                                     uint32_t                 sector,
                                     uint8_t                 *p_buf,
                                     uint16_t                 sector_size)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_buf) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD24,
                       sector,
                       AWBL_SDIO_WR_DATA,
                       p_buf,
                       sector_size,
                       1,
                       AWBL_SDIO_RSP_R1,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        if ((rsp[0] & (AWBL_SD_SPI_R1_IN_IDLE_STATE |
                       AWBL_SD_SPI_R1_ALL_ERROR)) != 0) {
            return -AW_EIO;
        }
    } else {
        if ((rsp[0] & 0xFFFF0000) != 0) {
            return -AW_EIO;
        }
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_write_single_block_async (struct awbl_sdio_device *p_card,
                                           uint32_t                 sector,
                                           uint8_t                 *p_buf,
                                           uint16_t                 sector_size,
                                           uint32_t                *p_rsp,
                                           void                    *p_arg,
                                           void (*pfn_complete) (void  *p_arg))
{
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_buf  ||
        NULL == pfn_complete) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD24,
                       sector,
                       AWBL_SDIO_WR_DATA,
                       p_buf,
                       sector_size,
                       1,
                       AWBL_SDIO_RSP_R1,
                       p_rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_async(p_host, &msg, p_arg, pfn_complete);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_read_multiple_block (struct awbl_sdio_device  *p_card,
                                      uint32_t                  sector,
                                      uint8_t                  *p_buf,
                                      uint32_t                  sector_cnt,
                                      uint16_t                  sector_size,
                                      uint8_t                   stop_en)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd1;
    struct awbl_sdio_cmd cmd2;
    uint32_t             rsp1[4];
    uint32_t             rsp2[4];
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_buf) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd1,
                       AWBL_SDIO_CMD18,
                       sector,
                       AWBL_SDIO_RD_DATA,
                       p_buf,
                       sector_size,
                       sector_cnt,
                       AWBL_SDIO_RSP_R1,
                       rsp1,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd1);

    if (stop_en) {
        aw_sdio_config_cmd(&cmd2,
                           AWBL_SDIO_CMD12,
                           0,
                           AWBL_SDIO_NO_DATA,
                           NULL,
                           0,
                           0,
                           AWBL_SDIO_RSP_R1,
                           rsp2,
                           5);

        aw_sdio_cmd_add_tail(&msg, &cmd2);
    }

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        if ((rsp1[0] & (AWBL_SD_SPI_R1_IN_IDLE_STATE |
                        AWBL_SD_SPI_R1_ALL_ERROR)) != 0) {
            return -AW_EIO;
        }
    } else {
        if ((rsp1[0] & 0xFFFF0000) != 0) {
            return -AW_EIO;
        }
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_read_multiple_block_async (struct awbl_sdio_device *p_card,
                                            uint32_t                 sector,
                                            uint8_t                 *p_buf,
                                            uint32_t                 sector_cnt,
                                            uint16_t                 sector_size,
                                            uint32_t                *p_rsp,
                                            uint8_t                  stop_en,
                                            void                    *p_arg,
                                            void  (*pfn_complete) (void *p_arg))
{
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd1;
    struct awbl_sdio_cmd cmd2;
    uint32_t             rsp2[4];
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_buf  ||
        NULL == pfn_complete) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd1,
                       AWBL_SDIO_CMD18,
                       sector,
                       AWBL_SDIO_RD_DATA,
                       p_buf,
                       sector_size,
                       sector_cnt,
                       AWBL_SDIO_RSP_R1,
                       p_rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd1);

    if (stop_en) {
        aw_sdio_config_cmd(&cmd2,
                           AWBL_SDIO_CMD12,
                           0,
                           AWBL_SDIO_NO_DATA,
                           NULL,
                           0,
                           0,
                           AWBL_SDIO_RSP_R1,
                           rsp2,
                           5);
        aw_sdio_cmd_add_tail(&msg, &cmd2);
    }

    err = awbl_sdio_host_async(p_host, &msg, p_arg, pfn_complete);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_write_multiple_block (struct awbl_sdio_device *p_card,
                                       uint32_t                 sector,
                                       uint8_t                 *p_buf,
                                       uint32_t                 sector_cnt,
                                       uint16_t                 sector_size,
                                       uint8_t                  stop_en)
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd1;
    struct awbl_sdio_cmd cmd2;
    uint32_t             rsp1[4];
    uint32_t             rsp2[4];
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_buf) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd1,
                       AWBL_SDIO_CMD25,
                       sector,
                       AWBL_SDIO_WR_DATA,
                       p_buf,
                       sector_size,
                       sector_cnt,
                       AWBL_SDIO_RSP_R1,
                       rsp1,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd1);

    if ((p_info->mode != AWBL_SDIO_SPI_M) && stop_en) {
        aw_sdio_config_cmd(&cmd2,
                           AWBL_SDIO_CMD12,
                           0,
                           AWBL_SDIO_NO_DATA,
                           NULL,
                           0,
                           0,
                           AWBL_SDIO_RSP_R1,
                           rsp2,
                           5);
        aw_sdio_cmd_add_tail(&msg, &cmd2);
    }

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        if ((rsp1[0] & (AWBL_SD_SPI_R1_IN_IDLE_STATE |
                        AWBL_SD_SPI_R1_ALL_ERROR)) != 0) {
            return -AW_EIO;
        }
    } else {
        if ((rsp1[0] & 0xFFFF0000) != 0) {
            return -AW_EIO;
        }
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_write_multiple_block_async (struct awbl_sdio_device *p_card,
                                             uint32_t                 sector,
                                             uint8_t                 *p_buf,
                                             uint32_t                 sector_cnt,
                                             uint16_t                 sector_size,
                                             uint32_t                *p_rsp,
                                             uint8_t                  stop_en,
                                             void                    *p_arg,
                                             void  (*pfn_complete) (void *p_arg))
{
    AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card);
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd1;
    struct awbl_sdio_cmd cmd2;
    uint32_t             rsp2[4];
    aw_err_t             err;

    if (NULL == p_card ||
        NULL == p_buf  ||
        NULL == pfn_complete) {
        return -AW_EINVAL;
    }


    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd1,
                       AWBL_SDIO_CMD25,
                       sector,
                       AWBL_SDIO_WR_DATA,
                       p_buf,
                       sector_size,
                       sector_cnt,
                       AWBL_SDIO_RSP_R1,
                       p_rsp,
                       0);
    aw_sdio_cmd_add_tail(&msg, &cmd1);

    if ((p_info->mode != AWBL_SDIO_SPI_M) && stop_en) {
        aw_sdio_config_cmd(&cmd2,
                           AWBL_SDIO_CMD12,
                           0,
                           AWBL_SDIO_NO_DATA,
                           NULL,
                           0,
                           0,
                           AWBL_SDIO_RSP_R1,
                           rsp2,
                           5);
        aw_sdio_cmd_add_tail(&msg, &cmd2);
    }

    err = awbl_sdio_host_async(p_host, &msg, p_arg, pfn_complete);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_stop_transmission (struct awbl_sdio_device *p_card)
{
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;

    if (NULL == p_card) {
        return -AW_EINVAL;
    }



    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD12,
                       0,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R1,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_send_op_cond (struct awbl_sdio_device *p_card,
                               uint32_t                 ocr,
                               uint32_t                *p_rocr)
{
    return aw_sdio_host_send_op_cond(p_card->p_host, ocr, p_rocr);
}

aw_err_t aw_sdio_host_send_op_cond (struct awbl_sdio_host *p_host,
                                    uint32_t               ocr,
                                    uint32_t              *p_rocr)
{
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_host);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;
    uint32_t             arg;
    uint8_t              rsp_type;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    arg = (p_info->mode == AWBL_SDIO_SPI_M) ? 0 : ocr;

    rsp_type = (p_info->mode == AWBL_SDIO_SPI_M) ? AWBL_SDIO_RSP_R1 : 
                    AWBL_SDIO_RSP_R3;

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD1,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       rsp_type,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        if (rsp[0] & AWBL_SD_SPI_R1_IN_IDLE_STATE) {
            err = ocr ? -AW_EBUSY : AW_OK;
            return err;
        }
    } else {
        if ((!(rsp[0] & 0x80000000)) && ocr) {
            return -AW_EBUSY;
        }
        if (p_rocr != NULL) {
            *p_rocr = rsp[0];
        }
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_send_status (struct awbl_sdio_device *p_card,
                              uint32_t                 rca,
                              uint32_t                *p_status)
{
    return aw_sdio_host_send_status(p_card->p_host, rca, p_status);
}

aw_err_t aw_sdio_host_send_status (struct awbl_sdio_host *p_host,
                                   uint32_t               rca,
                                   uint32_t              *p_status)
{
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_host);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;
    uint32_t             arg;
    uint8_t              rsp_type;

    if (NULL == p_host ||
        NULL == p_status) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    arg = (p_info->mode != AWBL_SDIO_SPI_M) ? (rca << 16) : 0;

    rsp_type = (p_info->mode == AWBL_SDIO_SPI_M) ? AWBL_SDIO_RSP_R2 : 
                    AWBL_SDIO_RSP_R1;

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD13,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       rsp_type,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode != AWBL_SDIO_SPI_M) {
        *p_status = rsp[0];
    } else {
        *p_status = __SDIO_SPI_SWAB32(rsp[0]) >> 16;
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_host_spi_set_crc(awbl_sdio_host_t      *p_host,
                                  uint8_t                crc_en)
{
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_host);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    if (p_info->mode != AWBL_SDIO_SPI_M) {
        /* 该命令只支持SPI */
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);
    aw_sdio_config_cmd(&cmd, AWBL_SDIO_CMD59, crc_en, AWBL_SDIO_NO_DATA, NULL,
                       0, 0, AWBL_SDIO_RSP_R1, rsp, 0);
    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);

    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_host_send_relative_addr (awbl_sdio_host_t *p_host,
                                          uint32_t         *p_rca)
{
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_host);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;

    if (NULL == p_host ||
        NULL == p_rca) {
        return -AW_EINVAL;
    }


    if (p_info->mode == AWBL_SDIO_SPI_M) {
        /* just SPI mode */
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD3,
                       0,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       AWBL_SDIO_RSP_R6,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);

    if (err != AW_OK) {
        return err;
    }

    *p_rca = rsp[0] >> 16;

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_host_select_card (awbl_sdio_host_t       *p_host,
                                   uint32_t                rca,
                                   uint8_t                 select)
{
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_host);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd;
    uint32_t             rsp[4];
    aw_err_t             err;
    uint32_t             arg;
    uint8_t              rsp_type;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
        /* just SPI */
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    arg = select ? (rca<<16) : 0;

    rsp_type = select ? AWBL_SDIO_RSP_R1 : AWBL_SDIO_RSP_NONE;

    aw_sdio_config_cmd(&cmd,
                       AWBL_SDIO_CMD7,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       rsp_type,
                       rsp,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_rw_direct (awbl_sdio_host_t  *p_host,
                                    uint8_t            func_num,
                                    uint8_t            opt,
                                    aw_bool_t          ram_flg,
                                    uint32_t           addr,
                                    uint8_t           *p_data)
{
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_host);
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd1;
    aw_err_t             err;
    uint32_t             rsp1[4];
    uint8_t              rsp_type;

    uint32_t             arg = 0;

    if (NULL == p_host ||
        NULL == p_data) {
        return -AW_EINVAL;
    }

    if (opt == AWBL_SDIO_WR_DATA) {
        arg |= __SDIO_CMD52_ARG_FLAG_W;
    }

    if (ram_flg ) {
        arg |= __SDIO_CMD52_ARG_RAM_FLG;
    }

    arg |= __SDIO_CMD52_ARG_ADDR(addr)      |
           __SDIO_CMD52_ARG_FNUM(func_num)  |
           __SDIO_CMD52_ARG_DATA((*p_data));


    aw_sdio_msg_init(&msg);
#if 0 
    rsp_type = (p_info->mode == AWBL_SDIO_SPI_M) ? AWBL_SDIO_RSP_R1 : 
        AWBL_SDIO_RSP_R5;
#endif 
    rsp_type = (p_info->mode == AWBL_SDIO_SPI_M) ? AWBL_SDIO_RSP_R5 : 
                    AWBL_SDIO_RSP_R5;
    aw_sdio_config_cmd(&cmd1,
                       AWBL_SDIO_CMD52,
                       arg,
                       AWBL_SDIO_NO_DATA,
                       NULL,
                       0,
                       0,
                       rsp_type,
                       rsp1,
                       0);

    aw_sdio_cmd_add_tail(&msg, &cmd1);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    if (p_info->mode == AWBL_SDIO_SPI_M) {
#if 0 
        if ((rsp1[0] & 0xff00)) {
            return -AW_EIO;
        }
#endif 
        *p_data = (rsp1[0] >> 8) & 0x00ff;
        return AW_OK;
    } else {
        if ((rsp1[0] & __SDIO_CMD52_RSP_ALL_ERROR)) {
            return -AW_EIO;
        }
    }

    if (opt == AWBL_SDIO_RD_DATA) {
        *p_data = rsp1[0] & 0Xff;
    }

    if (opt == AWBL_SDIO_WR_DATA && ram_flg ) {
       *p_data = rsp1[0] & 0Xff;
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_rw_externed (awbl_sdio_host_t  *p_host,
                                      uint8_t            func_num,
                                      uint8_t            opt,
                                      aw_bool_t          blk_flg,
                                      uint32_t           addr,
                                      uint8_t           *p_buf,
                                      size_t             blksize,
                                      size_t             blkorbytes,
                                      aw_bool_t          op_flg)
{
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd1;
    aw_err_t             err;
    uint32_t             rsp1[4];
    uint32_t             arg = 0;

    if (NULL == p_host           ||
        NULL == p_buf            ||
        AWBL_SDIO_NO_DATA == opt) {
        return -AW_EINVAL;
    }

    if (opt == AWBL_SDIO_WR_DATA) {
        arg |= __SDIO_CMD53_ARG_FLAG_W;
    }

    if (op_flg ) {
        arg |= __SDIO_CMD53_ARG_FLAG_OP_INCREM;
    }


    if (func_num > 0x7 || addr > 0x1FFFF) {
        /* TODO */
    }

    arg |= __SDIO_CMD53_ARG_FNUM(func_num) |
           __SDIO_CMD53_ARG_ADDR(addr)     |
           __SDIO_CMD53_ARG_CNT(blkorbytes);


    if (blk_flg ) {
        arg |= __SDIO_CMD53_ARG_BLKMD_BLK;
    } else  {
        blksize = blkorbytes;
        blkorbytes = 1;
    }  /* 1 001 1 0 10000000000000000 000000001 */

    aw_sdio_msg_init(&msg);

    aw_sdio_config_cmd(&cmd1,
                        AWBL_SDIO_CMD53,
                        arg,
                        opt,
                        p_buf,
                        blksize,
                        blkorbytes,
                        AWBL_SDIO_RSP_R5,
                        rsp1,
                        0);

    aw_sdio_cmd_add_tail(&msg, &cmd1);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }
/* 1 0000 00000000 */
    if ((rsp1[0] & __SDIO_CMD52_RSP_ALL_ERROR)) {
        return -AW_EIO;
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_go_idle_state (awbl_sdio_host_t *p_host)
{
    uint8_t data = __SDIO_OCCR_REG_ABORT_RES;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    return aw_sdio_host_io_rw_direct(p_host,
                                     0,
                                     AWBL_SDIO_WR_DATA,
                                     AW_TRUE,
                                     __SDIO_OCCR_REG_ADDR_ABORT,
                                    &data);
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_set_blocklen (awbl_sdio_host_t *p_host,
                                       uint8_t           func_num,
                                       uint32_t          blk_len)
{
    uint8_t  data = blk_len & 0xFF;
    aw_err_t ret;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    ret = aw_sdio_host_io_rw_direct(p_host,
                                    0,
                                    AWBL_SDIO_WR_DATA,
                                    AW_TRUE,
                                    __SDIO_OCCR_REG_ADDR_BLKSIZE + 
                                        func_num * 256,
                                   &data);

    if (AW_OK != ret) {
        return ret;
    }

    data = (blk_len >> 8) & 0xFF;

    return aw_sdio_host_io_rw_direct(p_host,
                                    0,
                                    AWBL_SDIO_WR_DATA,
                                    AW_TRUE,
                                    __SDIO_OCCR_REG_ADDR_BLKSIZE + 
                                        func_num * 256 + 1,
                                   &data);
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_app_set_bus_width (awbl_sdio_host_t       *p_host,
                                            awbl_sdio_bus_width_t   width)
{
    uint8_t data;

    if (NULL == p_host) {
        return -AW_EINVAL;
    }


    if (width == AWBL_SDIO_BUS_WIDTH_1B) {
        data = __SDIO_CMD52_ARG_DATA(0);
    } else if (width == AWBL_SDIO_BUS_WIDTH_4B) {
        data = __SDIO_CMD52_ARG_DATA(2);
    } else {
        return -AW_EINVAL;
    }

    return aw_sdio_host_io_rw_direct(p_host,
                                     0,
                                     AWBL_SDIO_WR_DATA,
                                     AW_TRUE,
                                     __SDIO_OCCR_REG_ADDR_BUSCTRL,
                                    &data);
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_send_app_op_cond (awbl_sdio_host_t *p_host,
                                           uint32_t          ocr,
                                           uint32_t         *p_rocr,
                                           uint8_t          *p_func_num)
{
    struct awbl_sdio_msg msg;
    struct awbl_sdio_cmd cmd1;
    aw_err_t             err;
    uint32_t             rsp1[4];
    uint32_t             arg;

    if (NULL == p_host  ||
        NULL == p_rocr  ) {
        return -AW_EINVAL;
    }

    aw_sdio_msg_init(&msg);

    arg = ocr;

    aw_sdio_config_cmd(&cmd1,
                        AWBL_SDIO_CMD5,
                        arg,
                        AWBL_SDIO_NO_DATA,
                        NULL,
                        0,
                        0,
                        AWBL_SDIO_RSP_R4,
                        rsp1,
                        0);

    aw_sdio_cmd_add_tail(&msg, &cmd1);

    err = awbl_sdio_host_sync(p_host, &msg, __SDIO_SYNC_TIMEOUT);
    if (err != AW_OK) {
        return err;
    }

    /*
     * funnum   io only            ocr
     * 010       0      00  0 111111111111111100000000
     *
     */

    if (p_rocr != NULL) {
        *p_rocr = rsp1[0] & 0xFFFFFF;
    }
    /*
     * r4: 0(C)  010(func num)  0(Memory Present)  00(Stuff)  0(S18A)  
     *     111111111111111100000000(OCR)
     *second time : 1(C) 010 0 00 0 111111111111111100000000
     *                            SPI 111111111111111100000101   1001000000000
     */
    /* 可以从RSP中读出是IO卡还是组合卡的信息，这里没有利用这个信息 */

    if (p_func_num) {
        *p_func_num = ((rsp1[0] >> 28) & 0x7);
    }
    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_transfer_abort (awbl_sdio_host_t      *p_host,
                                         uint8_t                func_num)
{
    uint8_t data = __SDIO_OCCR_REG_ABORT_AS(func_num);

    if (NULL == p_host) {
        return -AW_EINVAL;
    }

    return aw_sdio_host_io_rw_direct(p_host,
                                     0,
                                     AWBL_SDIO_WR_DATA,
                                     AW_TRUE,
                                     __SDIO_OCCR_REG_ADDR_ABORT,
                                    &data);
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_reset(awbl_sdio_host_t *p_host)
{
    aw_err_t ret;
    uint8_t  abort;

    /* SDIO Simplified Specification V2.0, 4.4 Reset for SDIO */
    ret = aw_sdio_host_io_rw_direct(p_host,
                                    0,
                                    AWBL_SDIO_RD_DATA,
                                    AW_TRUE,
                                    __SDIO_OCCR_REG_ADDR_ABORT,
                                   &abort);
    if (ret)
        abort = 0x08;
    else
        abort |= 0x08;

    ret = aw_sdio_host_io_rw_direct(p_host,
                                    0,
                                    AWBL_SDIO_WR_DATA,
                                    AW_TRUE,
                                    __SDIO_OCCR_REG_ADDR_ABORT,
                                   &abort);
    return ret;
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_read_multiple_block (awbl_sdio_host_t      *p_host,
                                              uint8_t               func_num,
                                              uint32_t              addr,
                                              uint8_t              *p_buf,
                                              uint16_t              sector_size,
                                              uint32_t              sector_cnt,
                                              
                                              /* 每次写入数据加1 */
                                              aw_bool_t             op_fix)
{
    if (NULL == p_host ||
        NULL == p_buf) {
        return -AW_EINVAL;
    }

    return aw_sdio_host_io_rw_externed (p_host,
                                        func_num,
                                        AWBL_SDIO_RD_DATA,
                                        AW_TRUE, /* blkorbyte */
                                        addr,
                                        p_buf,
                                        sector_size,
                                        sector_cnt,
                                        op_fix);
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_write_multiple_block (awbl_sdio_host_t *p_host,
                                               uint8_t           func_num,
                                               uint32_t          addr,
                                               uint8_t          *p_buf,
                                               uint16_t          sector_size,
                                               uint32_t          sector_cnt,
                                               aw_bool_t         op_fix)
{
    if (NULL == p_host ||
        NULL == p_buf) {
        return -AW_EINVAL;
    }

    return aw_sdio_host_io_rw_externed (p_host,
                                        func_num,
                                        AWBL_SDIO_WR_DATA,
                                        AW_TRUE, /* blkorbyte */
                                        addr,
                                        p_buf,
                                        sector_size,
                                        sector_cnt,
                                        op_fix);
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_read_bytes(awbl_sdio_host_t *p_host,
                                    uint8_t           func_num,
                                    uint32_t          addr,
                                    uint8_t          *p_buf,
                                    size_t            data_len,
                                    aw_bool_t         op_fix)
{
    if (NULL == p_host ||
        NULL == p_buf) {
        return -AW_EINVAL;
    }

    return aw_sdio_host_io_rw_externed (p_host,
                                        func_num,
                                        AWBL_SDIO_RD_DATA,
                                        AW_FALSE, /* blkorbyte */
                                        addr,
                                        p_buf,
                                        0,
                                        data_len,
                                        op_fix);
}

/******************************************************************************/
aw_err_t aw_sdio_host_io_write_bytes (awbl_sdio_host_t *p_host,
                                      uint8_t           func_num,
                                      uint32_t          addr,
                                      uint8_t          *p_buf,
                                      size_t            data_len,
                                      aw_bool_t         op_fix)
{
    if (NULL == p_host ||
        NULL == p_buf) {
        return -AW_EINVAL;
    }

    return aw_sdio_host_io_rw_externed (p_host,
                                        func_num,
                                        AWBL_SDIO_WR_DATA,
                                        AW_FALSE,/* blkorbyte */
                                        addr,
                                        p_buf,
                                        0,
                                        data_len,
                                        op_fix);
}


/* HOST-->CARD */
/******************************************************************************/
aw_err_t aw_sdio_io_rw_direct (struct awbl_sdio_device *p_card,
                               uint8_t                  func_num,
                               uint8_t                  opt,
                               aw_bool_t                ram_flg,
                               uint32_t                 addr,
                               uint8_t                 *p_data)
{
    if (NULL == p_card   ||
        NULL == p_data) {
        return -AW_EINVAL;
    }

    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    return aw_sdio_host_io_rw_direct(p_host,
                                     func_num,
                                     opt,
                                     ram_flg,
                                     addr,
                                     p_data);
}

/******************************************************************************/
aw_err_t aw_sdio_io_rw_externed (struct awbl_sdio_device *p_card,
                                 uint8_t                  func_num,
                                 uint8_t                  opt,
                                 aw_bool_t                blk_flg,/* blkorbyte */
                                 uint32_t                 addr,
                                 uint8_t                 *p_buf,
                                 size_t                   blksize,
                                 size_t                   blkorbytes,
                                 aw_bool_t                op_flg)
{
    if (NULL == p_card   ||
        NULL == p_buf) {
        return -AW_EINVAL;
    }

    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    return aw_sdio_host_io_rw_externed(p_host,
                                       func_num,
                                       opt,
                                       blk_flg,
                                       addr,
                                       p_buf,
                                       blksize,
                                       blkorbytes,
                                       op_flg);
}

/******************************************************************************/
aw_err_t aw_sdio_io_go_idle_state (struct awbl_sdio_device *p_card)
{
    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    return aw_sdio_host_io_go_idle_state(p_host);
}

/******************************************************************************/
aw_err_t aw_sdio_io_set_blocklen (struct awbl_sdio_device *p_card,
                                  uint8_t                  func_num,
                                  uint32_t                 blk_len)
{
    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    return aw_sdio_host_io_set_blocklen(p_host, func_num, blk_len);
}

/******************************************************************************/
aw_err_t aw_sdio_io_app_set_bus_width (struct awbl_sdio_device *p_card,
                                       awbl_sdio_bus_width_t    width)
{
    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    return aw_sdio_host_io_app_set_bus_width(p_host, width);
}

/******************************************************************************/
aw_err_t aw_sdio_io_send_app_op_cond (struct awbl_sdio_device *p_card,
                                      uint32_t                 ocr,
                                      uint32_t                *p_rocr,
                                      uint8_t                 *p_func_num)
{
   if (NULL == p_card   ||
        NULL == p_rocr   ||
        NULL == p_func_num) {
        return -AW_EINVAL;
    }

    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    return aw_sdio_host_io_send_app_op_cond(p_host, ocr, p_rocr, p_func_num);
}

/******************************************************************************/
aw_err_t aw_sdio_io_transfer_abort (struct awbl_sdio_device *p_card,
                                    uint8_t                  func_num)
{
    if (NULL == p_card) {
        return -AW_EINVAL;
    }

    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    return aw_sdio_host_io_transfer_abort(p_host, func_num);
}

/******************************************************************************/
aw_err_t aw_sdio_io_read_multiple_block (struct awbl_sdio_device *p_card,
                                         uint8_t                  func_num,
                                         uint32_t                 addr,
                                         uint8_t                 *p_buf,
                                         uint16_t                 sector_size,
                                         uint32_t                 sector_cnt,
                                         
                                         /* 每次写入数据加1 */
                                         aw_bool_t                op_fix)
{
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);

    if (NULL == p_card || NULL == p_buf) {
        return -AW_EINVAL;
    }

    return aw_sdio_host_io_read_multiple_block(p_host,
                                               func_num,
                                               addr,
                                               p_buf,
                                               sector_size,
                                               sector_cnt,
                                               op_fix);
}

/******************************************************************************/
aw_err_t aw_sdio_io_write_multiple_block (struct awbl_sdio_device *p_card,
                                          uint8_t                  func_num,
                                          uint32_t                 addr,
                                          uint8_t                 *p_buf,
                                          uint16_t                 sector_size,
                                          uint32_t                 sector_cnt,
                                          aw_bool_t                op_fix)
{
    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    if (NULL == p_card || NULL == p_buf) {
        return -AW_EINVAL;
    }

    return aw_sdio_host_io_write_multiple_block(p_host,
                                                func_num,
                                                addr,
                                                p_buf,
                                                sector_size,
                                                sector_cnt,
                                                op_fix);
}

/******************************************************************************/
aw_err_t aw_sdio_io_read_bytes(struct awbl_sdio_device *p_card,
                               uint8_t                  func_num,
                               uint32_t                 addr,
                               uint8_t                 *p_buf,
                               size_t                   data_len,
                               aw_bool_t                op_fix)
{

    if (NULL == p_card || NULL == p_buf) {
        return -AW_EINVAL;
    }

    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    return aw_sdio_host_io_read_bytes(p_host,
                                      func_num,
                                      addr,
                                      p_buf,
                                      data_len,
                                      op_fix);
}

/******************************************************************************/
aw_err_t aw_sdio_io_write_bytes (struct awbl_sdio_device *p_card,
                                 uint8_t                  func_num,
                                 uint32_t                 addr,
                                 uint8_t                 *p_buf,
                                 size_t                   data_len,
                                 aw_bool_t                op_fix)
{

    if (NULL == p_card || NULL == p_buf) {
        return -AW_EINVAL;
    }

    AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card);
    return aw_sdio_host_io_write_bytes(p_host,
                                       func_num,
                                       addr,
                                       p_buf,
                                       data_len,
                                       op_fix);
}




/******************************************************************************/
aw_err_t aw_sdio_mem_refcnt_get(struct awbl_sdio_mem *p_sdcard)
{
    aw_err_t ret = 0;
    AWBL_SDIO_MEM_TO_CARD_DECL(p_sdio, p_sdcard);

    ret = aw_refcnt_get(&p_sdio->ref);
    return ret;
}

/******************************************************************************/
aw_err_t aw_sdio_mem_refcnt_put(struct awbl_sdio_mem *p_sdcard)
{
    aw_err_t ret = 0;
    AWBL_SDIO_MEM_TO_CARD_DECL(p_sdio, p_sdcard);

    ret = aw_refcnt_put(&p_sdio->ref, __sdio_nouse_cb);
    return ret;
}

/******************************************************************************/
aw_err_t aw_sdio_func_refcnt_get(struct awbl_sdio_func *p_func)
{
    aw_err_t ret = 0;
    AWBL_SDIO_SDIO_TO_CARD_DECL(p_sdio, p_func);

    ret = aw_refcnt_get(&p_sdio->ref);
    return ret;
}

/******************************************************************************/
aw_err_t aw_sdio_func_refcnt_put(struct awbl_sdio_func *p_func)
{
    aw_err_t ret = 0;
    AWBL_SDIO_SDIO_TO_CARD_DECL(p_sdio, p_func);

    ret = aw_refcnt_put(&p_sdio->ref, __sdio_nouse_cb);
    return ret;
}

/******************************************************************************/
aw_err_t aw_sdio_static_memmory_enum (struct awbl_sdio_mem *p_sdcard)
{
    aw_err_t err;
    uint32_t ocr = 0;
    struct awbl_sdio_timeout_obj  timeout;
    struct awbl_sdio_host *p_host     = NULL;
    awbl_sdio_dev_info_t  *p_dev_info = NULL;

    p_host = p_sdcard->p_card->p_host;
    p_dev_info = (awbl_sdio_dev_info_t *)p_sdcard->super.p_devhcf->p_devinfo;

    if ((p_sdcard == NULL) || (p_dev_info == NULL) || (p_host == NULL)) {
        return -AW_EINVAL;
    }
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_hostinfo, p_host);

    p_sdcard->devhcf.p_devinfo = p_dev_info;
    p_host->p_card->p_sdcard   = (struct awbl_sdio_mem *)p_sdcard;
    ((struct awbl_sdio_mem *)p_sdcard)->p_card = p_host->p_card;
    memcpy(p_sdcard->p_card->name, p_sdcard->name, sizeof(p_sdcard->name));

    __sdio_host_dev_reset(p_host);
    aw_sdio_host_send_if_cond (p_host, p_hostinfo->ocr_valid);

    switch(p_dev_info->type) {
        case AWBL_SDIO_TYPE_MMC:
            goto __emmc;
            break;
        case AWBL_SDIO_TYPE_SD:
            goto __sd;
            break;
        default :
            AW_INFOF(("sdio: invalid memory type.\r\n"));
            return -AW_EINVAL;
    }

__sd:
    awbl_sdio_timeout_set(&timeout, 1000);
    do {
        err = aw_sdio_host_send_app_op_cond(p_host, 0, 0, &ocr);
        if (err != AW_OK) {
            aw_mdelay(1);
        }
    } while ((err != AW_OK) && (!awbl_sdio_timeout(&timeout)) && 
                !(p_host->removed));

    if (err != AW_OK) {
        AW_INFOF(("sdio: sdcard enum failed.\r\n"));
        return err;
    }
    err = __sdio_memory_portion_init(p_sdcard, ocr, AWBL_SDIO_TYPE_SD);
    goto __print;

__emmc:
    awbl_sdio_timeout_set(&timeout, 1000);
    do {
        err = aw_sdio_host_send_op_cond(p_host, 0, &ocr);
        if (err != AW_OK) {
            aw_mdelay(1);
        }
    } while ((err != AW_OK) && (!awbl_sdio_timeout(&timeout)) && 
                !(p_host->removed));

    if (err != AW_OK) {
        AW_INFOF(("sdio: emmc enum failed.\r\n"));
        return err;
    }
    err = __sdio_memory_portion_init(p_sdcard, ocr, AWBL_SDIO_TYPE_MMC);

__print:
    if (err == AW_OK) {
        __SDIO_MEMORY_PRINTF_INFO(p_sdcard);
    }
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_static_sdio_enum (struct awbl_sdio_func *p_func)
{
    aw_err_t err;
    uint32_t ocr = 0;
    struct awbl_sdio_timeout_obj  timeout;
    struct awbl_sdio_host *p_host = NULL;
    awbl_sdio_dev_info_t  *p_dev_info = NULL;

    p_host = p_func->p_card->p_host;
    p_dev_info = (awbl_sdio_dev_info_t *)p_func->super.p_devhcf->p_devinfo;

    if ((p_func == NULL) || (p_dev_info == NULL) || (p_host == NULL)) {
        return -AW_EINVAL;
    }
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_hostinfo, p_host);

    if ((p_dev_info->type != AWBL_SDIO_TYPE_IO) &&
        (p_dev_info->type != AWBL_SDIO_TYPE_COMBO)) {
        AW_INFOF(("sdio: invalid sdio type.\r\n"));
        return -AW_EINVAL;
    }

    p_func->devhcf.p_devinfo = p_dev_info;
    p_func->p_card = p_host->p_card;

    __sdio_host_dev_reset(p_host);
    aw_sdio_host_send_if_cond (p_host, p_hostinfo->ocr_valid);

    awbl_sdio_timeout_set(&timeout, 1000);
    do {
        err = aw_sdio_host_io_send_app_op_cond(p_host, 0, &ocr, NULL);
        if (err != AW_OK) {
            aw_mdelay(1);
        }
    } while ((err != AW_OK) && (!awbl_sdio_timeout(&timeout)) && 
                !(p_host->removed));

    if (err != AW_OK) {
        AW_INFOF(("sdio: function enum failed.\r\n"));
        return err;
    }
    err = __sdio_io_portion_init(p_func, ocr);
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_dynamic_dev_enumerate (struct awbl_sdio_host *p_host)
{
    aw_err_t err = AW_OK;
    uint32_t ocr = 0;
    struct awbl_sdio_timeout_obj  timeout;

    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_hostinfo, p_host);

    /* reset host */
    __sdio_host_dev_reset(p_host);
    aw_sdio_host_send_if_cond (p_host, p_hostinfo->ocr_valid);


    if (!aw_sdio_host_io_send_app_op_cond(p_host, 0, &ocr, NULL)) {
        err = AW_OK;
        goto atch_sdio;
    }
    if (!aw_sdio_host_send_app_op_cond(p_host, 0, 0, &ocr)) {
        err = AW_OK;
        goto atch_sd;
    }
    if (!aw_sdio_host_send_op_cond(p_host, 0, &ocr)) {
        err = AW_OK;
        goto atch_mmc;
    }


    /**
     * SDIO
     */
    awbl_sdio_timeout_set(&timeout, 1000);
    do {
        err = aw_sdio_host_io_send_app_op_cond(p_host, 0, &ocr, NULL);
        if (err != AW_OK) {
            aw_mdelay(1);
        }
    } while ((err != AW_OK) && (!awbl_sdio_timeout(&timeout)) && 
                !(p_host->removed));

atch_sdio:
    if (!err) {
        err = aw_sdio_dynamic_sdio_enum(p_host, ocr, NULL);
        if (err) {
            goto out;
        } else {
            return err;
        }
    }


    /**
     * SD
     */
    awbl_sdio_timeout_set(&timeout, 1000);
    do {
        err = aw_sdio_host_send_app_op_cond(p_host, 0, 0, &ocr);
        if (err != AW_OK) {
            aw_mdelay(1);
        }
    } while ((err != AW_OK) && (!awbl_sdio_timeout(&timeout)) && 
                !(p_host->removed));

atch_sd:
    if (!err) {
        err = aw_sdio_dynamic_memory_enum(p_host, 
                                          ocr, 
                                          AWBL_SDIO_TYPE_SD, 
                                          AW_TRUE);
        if (err) {
            goto out;
        } else {
            return err;
        }
    }


    /**
     * MMC
     */
    awbl_sdio_timeout_set(&timeout, 1000);
    do {
        err = aw_sdio_host_send_op_cond(p_host, 0, &ocr);
        if (err != AW_OK) {
            aw_mdelay(1);
        }
    } while ((err != AW_OK) && (!awbl_sdio_timeout(&timeout)) && 
                !(p_host->removed));

atch_mmc:
    if (!err) {
        err = aw_sdio_dynamic_memory_enum(p_host,
                                          ocr, 
                                          AWBL_SDIO_TYPE_MMC, 
                                          AW_TRUE);
        if (err) {
            goto out;
        } else {
            return err;
        }
    }

out:
    return err;
}

/******************************************************************************/
aw_err_t aw_sdio_dynamic_sdio_enum (struct awbl_sdio_host *p_host,
                                    uint32_t               ocr,
                                    struct awbl_sdio_func *p_func)
{
    aw_err_t ret       = 0;
    uint8_t  i         = 0;
    uint32_t rocr      = 0;
    uint8_t  funcs     = 0;
    uint32_t raw_cid[4];
    uint32_t raw_csd[4];
    uint32_t raw_scr[2];
    aw_bool_t   dynamic;

    struct awbl_sdio_device      *p_card = NULL;
    struct awbl_sdio_timeout_obj  timeout;
    struct awbl_sdio_io_card_info ioinfo;
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_hostinfo, p_host);

    awbl_sdio_mem_card_info_t *p_mem_info = NULL;
    struct awbl_sdio_mem      *p_sdcard   = NULL;

    dynamic = (p_func == NULL) ? AW_TRUE : AW_FALSE;

    if (dynamic) {
        p_card = __MEM_ALLOC_SIZE(sizeof(struct awbl_sdio_device));
        if (p_card == NULL) {
            AW_ERRF(("SDIO ERROR: attach_sdio p_card mem_alloc failed."));
            return -AW_ENOMEM;
        }
        memset(p_card, 0x00, sizeof(struct awbl_sdio_device));
        p_card->p_host = p_host;
        p_host->p_card = p_card;

        aw_refcnt_init(&p_card->ref);
    } else {
        if (p_host->p_card == NULL) {
            return -AW_EINVAL;
        }
        p_card  = p_host->p_card;
    }

    /* Inform the voltage */
    awbl_sdio_timeout_set(&timeout, 1000);
    do {
        ret = aw_sdio_host_io_send_app_op_cond(p_host, ocr, &rocr, &funcs);
        if (ret != AW_OK) {
            aw_mdelay(1);
        }
    } while ((ret != AW_OK) && (!awbl_sdio_timeout(&timeout)) && 
                 !(p_host->removed));
    if (ret != AW_OK) {
        goto failed;
    }

    /* For SPI, enable CRC */
    uint8_t crc_en = 0;
    if(p_hostinfo->mode == AWBL_SDIO_SPI_M) {
        crc_en = p_hostinfo->crc_en ? 1: 0;
        ret = aw_sdio_host_spi_set_crc(p_host, crc_en);
    }
    if (ret != AW_OK) {
        goto failed;
    }

#define AWBL_SDIO_R4_18V_PRESENT    (1 << 24)
#define AWBL_SDIO_R4_MEMORY_PRESENT (1 << 27)
    /* Judge combo or I/O only card */
    if (rocr & AWBL_SDIO_R4_MEMORY_PRESENT) {
        ret = __sdio_sd_get_cid(p_host, ocr & rocr, &rocr, raw_cid);
        if (ret == AW_OK) {
            p_card->type = AWBL_SDIO_TYPE_COMBO;
        }
    } else {
        p_card->type = AWBL_SDIO_TYPE_IO;
    }

    /* Send rca */
    if (p_hostinfo->mode != AWBL_SDIO_SPI_M) {
        ret = aw_sdio_send_relative_addr(p_card, &p_card->rca);
        p_card->io_info.rca = p_card->rca;
        if (ret != AW_OK) {
            goto failed;
        }
    }

    /* Decode sdcard */
    if (p_card->type == AWBL_SDIO_TYPE_COMBO) {
        if (dynamic) {
            p_sdcard = __MEM_ALLOC_SIZE(sizeof(struct awbl_sdio_mem));
            if (p_sdcard == NULL) {
                AW_ERRF(("SDIO ERROR: attach_sdio p_sdcard mem_alloc failed."));
                ret = -AW_ENOMEM;
                goto failed;
            }
            memset(p_sdcard, 0x00, sizeof(struct awbl_sdio_mem));
            p_card->p_sdcard = p_sdcard;
            p_sdcard->p_card = p_card;
            p_mem_info       = &p_sdcard->mem_info;

        } else {
            if (p_host->p_card == NULL      ||
                p_host->p_card->p_sdcard == NULL) {
                ret = -AW_EINVAL;
                goto failed;
            }
            p_card     = p_host->p_card;
            p_sdcard   = p_host->p_card->p_sdcard;
            p_mem_info = &p_sdcard->mem_info;
        }

        ret = aw_sdio_send_csd(p_card, p_card->rca, raw_csd);
        if (ret != AW_OK) {
            p_card->type = AWBL_SDIO_TYPE_IO;
        }
        __sdcard_decode_csd(p_mem_info, p_sdcard->raw_csd);
        __sdcard_decode_cid(p_mem_info, p_sdcard->raw_cid);
    }

    /* Select card */
    if (p_hostinfo->mode != AWBL_SDIO_SPI_M) {
        ret = aw_sdio_select_card(p_card, p_card->rca, 1);
        if (ret != AW_OK) {
            goto failed;
        }
    }

    /*Read CCCR */
    awbl_sdio_io_card_info_t *p_io_info = NULL;
    p_io_info = &p_card->io_info;
    memset(p_io_info, 0x00, sizeof(awbl_sdio_io_card_info_t));
    ret = __sdio_read_cccr(p_card, rocr, &p_io_info->cccr);
    if (ret != AW_OK) {
        __MEM_FREE_THIS(p_io_info);
        goto failed;
    }

    /* Read common CIS */
     ret = __sdio_read_cis(p_card, NULL);
    if (ret != AW_OK) {
        goto failed;
    }

    /* SD information */
    if (p_card->type == AWBL_SDIO_TYPE_COMBO) {
        ret = __sdio_sd_setup_card(p_sdcard, p_mem_info);
        if (ret != AW_OK) {
            p_card->type = AWBL_SDIO_TYPE_IO;
        }
    }

    /* create p_card name */
    if (dynamic) {
        sprintf(p_card->name, "/dev/sdio%d", p_hostinfo->bus_index);
    } else {
        memcpy(p_card->name, p_func->name, sizeof(p_func->name));
    }

    /* 动态设备 */
    if (dynamic) {
        p_card->func_num = 0;
        for (i = 0; i < funcs; i++, p_card->func_num++) {
            ret = __sdio_func_init(p_card, i + 1);
            if (ret != AW_OK) {
                goto failed;
            }
        }

    /* 静态设备 */
    } else {
        ret = __func_init_static(p_host->p_card, p_func);
        if (ret == AW_OK) {
            p_host->p_card->func_num++;
        }
    }

    {
        AW_INFOF(("\r\n"));
        AW_INFOF(("Found a new sdio card with %d functions:\r\n",
                    p_card->func_num));
        for (i = 0; i < AWBL_SDIO_MAX_FUNC; i++) {
            if (p_card->p_iofunc[i] != NULL) {
                AW_INFOF(("        Device Name: %s\r\n", 
                          p_card->p_iofunc[i]->name));
            }
        }
        AW_INFOF(("        Device VID:0x%x  PID:0x%x\r\n",
                  p_card->io_info.cis.tplmid_manf,
                  p_card->io_info.cis.tplmid_card));
        if (p_card->info != NULL) {
            for (i = 0; i < p_card->num_info; i++) {
                AW_INFOF(("          Device info%d: %s\r\n",i, p_card->info[i]));
            }
        }
        AW_INFOF(("\r\n"));
    }

    /* register SDcard and iofunc */
    if (dynamic) {
        if (p_card->type == AWBL_SDIO_TYPE_COMBO) {
            p_card->p_sdcard = p_sdcard;
            /* register  */
            ret = __sdio_register_sdcard(p_card->p_sdcard);
            if (ret != AW_OK) {
                p_card->type = AWBL_SDIO_TYPE_IO;
            } else {
                p_host->state = AW_SDIO_DEV_STAT_DEVREGIST;
            }
        }

        for (i = 0; i < AWBL_SDIO_MAX_FUNC; i++) {
            if (p_card->p_iofunc[i] != NULL) {
                ret = __sdio_register_iofunc(p_card->p_iofunc[i]);
                if (ret != AW_OK) {
                    goto failed;
                } else {
                    p_host->state = AW_SDIO_DEV_STAT_DEVREGIST;
                }
            }
        }
    }

    if (dynamic) {
        if (p_card->type == AWBL_SDIO_TYPE_IO) {
            if (p_sdcard != NULL) {
                __MEM_FREE_SUB(p_sdcard->info);
                __MEM_FREE_TUPLES(p_sdcard->tuples);
                __MEM_FREE_THIS(p_sdcard);
                p_card->p_sdcard = NULL;
                p_mem_info = NULL;
            }
        }
    }

    return ret;


failed:
    if (dynamic) {
        if (p_sdcard != NULL) {
            __MEM_FREE_SUB(p_sdcard->info);
            __MEM_FREE_TUPLES(p_sdcard->tuples);
            __MEM_FREE_THIS(p_sdcard);
            p_card->p_sdcard = NULL;
            p_mem_info = NULL;
        }
        if (p_card != NULL) {
            __MEM_FREE_SUB(p_card->info);
            __MEM_FREE_TUPLES(p_card->tuples);
            __MEM_FREE_THIS(p_card);
            p_host->p_card = NULL;
        }
    }
    return ret;
}

/******************************************************************************/
aw_err_t aw_sdio_dynamic_memory_enum (struct awbl_sdio_host *p_host,
                                      uint32_t               ocr,
                                      uint8_t                pre_type,
                                      aw_bool_t              dynamic)
{
    aw_err_t ret = 0;
    uint32_t cid[4];
    uint32_t status;
    struct awbl_sdio_timeout_obj  timeout;
    struct awbl_sdio_device   *p_card     = NULL;
    struct awbl_sdio_mem      *p_sdcard   = NULL;
    awbl_sdio_mem_card_info_t *p_mem_info = NULL;

    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_hostinfo, p_host);

    p_card = __MEM_ALLOC_SIZE(sizeof(struct awbl_sdio_device));
    if (p_card == NULL) {
        AW_ERRF(("SDIO ERROR: attach_memory p_card mem_alloc failed."));
        ret = -AW_ENOMEM;
        goto fail;
    }
    memset(p_card, 0x00, sizeof(struct awbl_sdio_device));
    p_card->p_host = p_host;
    p_host->p_card = p_card;
    aw_refcnt_init(&p_card->ref);

    p_sdcard = __MEM_ALLOC_SIZE(sizeof(struct awbl_sdio_mem));
    if (p_sdcard == NULL) {
        AW_ERRF(("SDIO ERROR: attach_memory p_sdcard mem_alloc failed."));
        ret = -AW_ENOMEM;
        goto fail;
    }
    memset(p_sdcard, 0x00, sizeof(struct awbl_sdio_mem));
    p_card->p_sdcard =  p_sdcard;
    p_sdcard->p_card =  p_card;
    p_mem_info       = &p_sdcard->mem_info;

    ret = __sdio_memory_portion_init(p_sdcard, ocr, pre_type);
    if (ret != AW_OK) {
        goto fail;
    }

    /* create p_card name */
    sprintf(p_card->name, "/dev/sd%d", p_hostinfo->bus_index);
    __SDIO_MEMORY_PRINTF_INFO(p_sdcard);

    ret = __sdio_register_sdcard(p_card->p_sdcard);
    if (ret != AW_OK) {
        goto fail;
    } else {
        p_host->state = AW_SDIO_DEV_STAT_DEVREGIST;
    }

/* #define _SPEED_TEST */
#ifdef _SPEED_TEST
    __sd_speed_test(p_card);
    /**
     * 28X 速度测试结果
     *
     * emmc(8 bit)  裸测：write 18MB/s  read  21MB/s
     *                 fs : write  8MB/s  read  14MB/s
     *
     * sdcard       裸测：write 11MB/s  read  12MB/s
     *                 fs : write  6MB/s  read  10MB/s
     *
     * mmc          裸测：write    4MB/s  read  12MB/s
     *                 fs : write  3.5MB/s  read   8MB/s
     */
#endif

ret:
    return ret;

fail:
    if (p_sdcard != NULL) {
        __MEM_FREE_SUB(p_sdcard->info);
        __MEM_FREE_TUPLES(p_sdcard->tuples);
        __MEM_FREE_THIS(p_sdcard);
        p_card->p_sdcard = NULL;
        p_mem_info = NULL;
    }
    if (p_card != NULL) {
        __MEM_FREE_SUB(p_card->info);
        __MEM_FREE_TUPLES(p_card->tuples);
        __MEM_FREE_THIS(p_card);
        p_host->p_card = NULL;
    }
    return ret;
}

/******************************************************************************/
aw_err_t awbl_sdio_host_create (struct awbl_sdio_host *p_host)
{
    if (p_host == NULL) {
        return -AW_EINVAL;
    }

    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_host_info, p_host);

    /* spin lock and mutex */
    aw_spinlock_isr_init(&(p_host->dev_lock), 0);
    AW_MUTEX_INIT(p_host->dev_mux, AW_SEM_Q_PRIORITY);
    AW_INIT_LIST_HEAD(&(p_host->msg_list));

    /* announce the controller */
    awbl_bus_announce((struct awbl_busctlr *)p_host,
                      AWBL_BUSID_SDIO);

    if (p_host_info->connect_type == AWBL_SDIO_CON_DEV_STATIC &&
        p_host->p_card            == NULL) {
        p_host->p_card = __MEM_ALLOC_SIZE(sizeof(struct awbl_sdio_device));
        if (p_host->p_card == NULL) {
            AW_ERRF(("SDIO ERROR: p_host->p_card mem_alloc failed."));
            return -AW_ENOMEM;
        }
        memset(p_host->p_card, 0x00, sizeof(struct awbl_sdio_device));
        p_host->p_card->p_host = p_host;
        __sdiobus_dev_enum((struct awbl_dev *)p_host);
    }

    if (p_host_info->connect_type == AWBL_SDIO_CON_DEV_DYNAMIC) {
        p_host->p_card = NULL;

        AW_TASK_INIT(p_host->detect_task,
                     "sdio det task",
                     SDIO_DETECT_TASK_PRIO,
                     SDIO_DETECT_TASK_STACK_SIZE,
                     __sdio_det_task,
                     (void *)p_host);

        AW_TASK_STARTUP(p_host->detect_task);
    }

    return AW_OK;
}


/******************************************************************************/
void awbl_sdio_port_change (struct awbl_sdio_host *p_host, uint8_t state)
{
    p_host->changed = AW_TRUE;

    /* todo 是否要立刻进入任务 */
    return;
}


/******************************************************************************/
void awbl_sdio_host_dynamic_reinit(struct awbl_sdio_host *p_host)
{
    if (p_host == NULL) {
        return;
    }
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_host_info, p_host);
    if (p_host_info->connect_type != AWBL_SDIO_CON_DEV_DYNAMIC) {
        return;
    }
    aw_spinlock_isr_take(&(p_host->dev_lock));
    p_host->removed = AW_TRUE;
    aw_spinlock_isr_give(&(p_host->dev_lock));
    return;
}


/******************************************************************************/
/** \brief SDIO register table */
aw_local aw_const struct awbl_bustype_info __gp_sdio_bustype = {
    AWBL_BUSID_SDIO,        /**< \brief bus_id */
    NULL,                   /**< \brief pfunc_bustype_init1 */
    NULL,                   /**< \brief pfunc_bustype_init2 */
    NULL,                   /**< \brief pfunc_bustype_connect */
    NULL,                   /**< \brief pfunc_bustype_newdev_present */
    __sdiobus_devmatch      /**< \brief pfunc_bustype_devmatch */
};


void awbl_sdiobus_init (void)
{
    aw_err_t err;
    err = awbl_bustype_register(&__gp_sdio_bustype);
    aw_assert(err == AW_OK);
}

/* end of file */
