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
 * \brief AWBus SATA bus type implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.00 17-06-07  anu, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_vdebug.h"
#include "awbl_satabus.h"
#include "stdio.h"
#include "string.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "aw_cache.h"

#undef AW_INFOF
#define AW_INFOF(x) aw_kprintf("SATA_BUS:");aw_kprintf x
/* #define AW_INFOF(x) */

#define __SATABUS_DETECT_DEV_TIME       500  /* 检测设备时间 */
#define __SATABUS_DETECT_ONLINE_TIME    50   /* 检测连接状态时间 (消抖) */

#define __SATABUS_HOST_DECL(p_host) \
    struct awbl_satabus_host *p_host = NULL

#define __SATABUS_DEV_DECL(p_dev) \
    struct awbl_satabus_dev *p_dev = NULL

#define __SATABUS_HOST_INFO_DECL(p_hostinfo) \
    struct awbl_satabus_host_info *p_hostinfo = NULL

#define __SATABUS_HOST_GET(p_host, p_dev) \
    p_host = p_dev->p_host

#define __SATABUS_DEV_GET(p_dev, p_host, port_num) \
    p_dev = p_host->p_dev[port_num]

#define __SATABUS_HOST_INFO_GET(p_hostinfo, p_host) \
    p_hostinfo = p_host->p_info

/*******************************************************************************
  fuction
*******************************************************************************/
aw_local aw_bool_t __satabus_devmatch (const struct awbl_drvinfo *p_drv,
                                    struct awbl_dev           *p_adev);

/*******************************************************************************
  locals
*******************************************************************************/
/** \brief SATA register table */
aw_local aw_const struct awbl_bustype_info __g_satabus_info = {
    AWBL_BUSID_SATA,        /**< \brief bus_id */
    NULL,                   /**< \brief pfunc_bustype_init1 */
    NULL,                   /**< \brief pfunc_bustype_init2 */
    NULL,                   /**< \brief pfunc_bustype_connect */
    NULL,                   /**< \brief pfunc_bustype_newdev_present */
    __satabus_devmatch      /**< \brief pfunc_bustype_devmatch */
};

/*******************************************************************************
    内部函数
*******************************************************************************/
/******************************************************************************/
aw_local aw_bool_t __satabus_devmatch (const struct awbl_drvinfo *p_adrv,
                                       struct awbl_dev           *p_adev)
{
    struct awbl_satabus_dev_info *p_drv;
/*    struct awbl_satabus_dev      *p_dev; */

    /* check bus type */
    if (p_adev->p_devhcf->bus_type != AWBL_BUSID_SATA) {
        return AW_FALSE;
    }

    p_drv = AW_CONTAINER_OF(p_adrv, struct awbl_satabus_dev_info, super);
/*    p_dev = AW_CONTAINER_OF(p_adev, struct awbl_satabus_dev, super);*/

    if (p_drv->type >= AWBL_SATABUS_DEV_TYPE_ILLEGAL) {
        return AW_FALSE;
    }

    return AW_TRUE;
}

/******************************************************************************/
aw_local aw_err_t __satabus_dev_delete (struct awbl_satabus_host *p_host, 
                                        uint32_t                  port_num)
{
    aw_err_t ret = AW_OK;

    if (p_host->p_dev[port_num] != NULL) {
        ret = awbl_dev_remove_announce(&(p_host->p_dev[port_num]->super));
        if (ret != AW_OK) {
            AW_INFOF(("device delete failed %d.\n", ret));
        }
        aw_mem_free(p_host->p_dev[port_num]);
        p_host->p_dev[port_num] = NULL;
    }
    return ret;
}

/******************************************************************************/
aw_local aw_err_t __satabus_dev_create (struct awbl_satabus_host *p_host, 
                                        uint32_t                  port_num)
{
    __SATABUS_DEV_DECL(p_dev);
    __SATABUS_HOST_INFO_DECL(p_hostinfo);

    aw_err_t ret;

    __SATABUS_HOST_INFO_GET(p_hostinfo, p_host);

    if (p_host->p_dev[port_num]) {
        __satabus_dev_delete(p_host, port_num);
    }

    p_dev = aw_mem_alloc(sizeof(struct awbl_satabus_dev));
    if (p_dev == NULL) {
        AW_INFOF(("no memory for SATABUS device.\n"));
        return -AW_ENOMEM;
    }

    memset(p_dev, 0x00, sizeof(struct awbl_satabus_dev));

    /* initialize base information of SATABUS device */
    p_dev->p_host   = p_host;
    p_dev->port_num = port_num;
    sprintf(p_dev->name, "/dev/sata%d-D%d", p_hostinfo->bus_index, port_num);

    /* initialize AWorks device information */
    p_dev->devhcf.p_name    = p_dev->name;
    p_dev->devhcf.unit      = 0;
    p_dev->devhcf.bus_type  = AWBL_BUSID_SATA;
    p_dev->devhcf.bus_index = p_hostinfo->bus_index;
    p_dev->devhcf.p_dev     = &(p_dev->super);
    p_dev->devhcf.p_devinfo = NULL;

    /* register it */
    p_dev->super.p_parentbus = ((struct awbl_dev *)p_host)->p_subbus;
    p_dev->super.p_devhcf    = &(p_dev->devhcf);

    AW_SEMB_INIT(p_dev->opt_semb, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    ret = awbl_dev_announce(&(p_dev->super));
    if (ret == AW_OK) {
        p_host->p_dev[port_num] = p_dev;

        if (p_dev->super.p_driver == NULL) {
            AW_INFOF(("could not found a driver\n"));
        }
        return AW_OK;
    }

    aw_mem_free(p_dev);
    return ret;
}

/******************************************************************************/
aw_local void __satabus_detect_work (struct awbl_satabus_host *p_host)
{
    awbl_satabus_host_port_link_stat_t link_stat;

    uint32_t i;
    aw_err_t ret;

    for (i = 0; i < p_host->cap_info.port_max_num; i++) {

        if (p_host->p_drvfuns->pfn_port_link_stat_get == NULL) {
            continue;
        }

        memset(&link_stat, 0x00, sizeof(link_stat));
        ret = p_host->p_drvfuns->pfn_port_link_stat_get(p_host, i, &link_stat);
        if (ret != AW_OK) {
            continue;
        }

        if (link_stat.is_change) {
            __satabus_dev_delete(p_host, i);
            p_host->dev_created &= ~(0x01 << i);
        }

        if (link_stat.is_online && ((p_host->dev_created & (0x01 << i)) == 0)) { 
        
            /* 当前在线，设备未创建 */
            aw_mdelay(__SATABUS_DETECT_ONLINE_TIME);
            memset(&link_stat, 0x00, sizeof(link_stat));
            ret = p_host->p_drvfuns->pfn_port_link_stat_get(
                    p_host, i, &link_stat);
            if (ret != AW_OK || !link_stat.is_online) {
                continue;
            }

            ret = __satabus_dev_create(p_host, i);
            if (ret != AW_OK) {
                p_host->dev_created &= ~(0x01 << i);
                AW_INFOF(("device create failed %d.\n", ret));
            }
            p_host->dev_created |= (0x01 << i);
        } else if ((!link_stat.is_online) &&
                  ((p_host->dev_created & (0x01 << i)) != 0)) { 
                  
            /* 当前不在线，设备已创建 */
            __satabus_dev_delete(p_host, i);
            p_host->dev_created &= ~(0x01 << i);
        }
    }

    aw_delayed_work_start(&p_host->dwork_detect, __SATABUS_DETECT_DEV_TIME);
}

static aw_err_t __satabus_cmd_request_complete (
        struct awbl_satabus_cmd_req *p_req, void *p_arg)
{
    __SATABUS_DEV_DECL(p_dev);

    p_dev = p_arg;
    if (p_dev != NULL) {
        AW_SEMB_GIVE(p_dev->opt_semb);
    }

    return AW_OK;
}

/******************************************************************************/
static aw_err_t __satabus_result_get (awbl_satabus_dev_stat_t *p_dev_stat)
{
    aw_err_t ret;

    switch (p_dev_stat->host_stat) {
    case AWBL_SATABUS_HOST_STAT_NONE   : ret =  AW_OK    ; break;
    case AWBL_SATABUS_HOST_STAT_ERR    : ret = -AW_EIO   ; break;
    case AWBL_SATABUS_HOST_STAT_PLUG   : ret = -AW_ENODEV; break;
    case AWBL_SATABUS_HOST_STAT_REQC   : ret = -AW_EAGAIN; break;
    case AWBL_SATABUS_HOST_STAT_PRESET : ret = -AW_ENODEV; break;
    case AWBL_SATABUS_HOST_STAT_PSTOP  : ret = -AW_ENODEV; break;
    default                            : ret = -AW_EINVAL; break;
    }

    return ret;
}

/*******************************************************************************
    外部函数
*******************************************************************************/
/******************************************************************************/
aw_err_t awbl_satabus_cmd_request_sync (struct awbl_satabus_dev *p_dev, 
                                        awbl_satabus_cmd_req_t *p_req)
{
    __SATABUS_HOST_DECL(p_host);

    aw_err_t ret;

    if (p_dev == NULL || p_req == NULL) {
        return -AW_EINVAL;
    }

    __SATABUS_HOST_GET(p_host, p_dev);

    if (p_host == NULL) {
        return -AW_EINVAL;
    }

    if (p_host->p_drvfuns->pfn_cmd_request != NULL) {
        p_req->pfn_complete_cb = __satabus_cmd_request_complete;
        p_req->p_arg = p_dev;
        AW_SEMB_TAKE(p_dev->opt_semb, AW_SEM_NO_WAIT);
        ret = p_host->p_drvfuns->pfn_cmd_request(
                p_host, p_dev->port_num, p_req);
    } else {
        ret = -AW_ENOTSUP;
    }

    if (ret == AW_OK) {
        AW_SEMB_TAKE(p_dev->opt_semb, AW_SEM_WAIT_FOREVER);

        ret = __satabus_result_get(&p_req->dev_stat);
    }

    return ret;
}

/******************************************************************************/
aw_err_t awbl_satabus_cmd_request_async (struct awbl_satabus_dev *p_dev, 
                                         awbl_satabus_cmd_req_t  *p_req)
{
    __SATABUS_HOST_DECL(p_host);

    aw_err_t ret;

    if (p_dev == NULL || p_req == NULL) {
        return -AW_EINVAL;
    }

    __SATABUS_HOST_GET(p_host, p_dev);

    if (p_host == NULL) {
        return -AW_EINVAL;
    }

    if (p_host->p_drvfuns->pfn_cmd_request != NULL) {
        ret = p_host->p_drvfuns->pfn_cmd_request(
                p_host, p_dev->port_num, p_req);
    } else {
        ret = -AW_ENOTSUP;
    }

    return ret;
}

/******************************************************************************/
aw_err_t awbl_satabus_cmd_request_cancel (struct awbl_satabus_dev *p_dev, 
                                          awbl_satabus_cmd_req_t  *p_req)
{
    __SATABUS_HOST_DECL(p_host);

    aw_err_t ret;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    __SATABUS_HOST_GET(p_host, p_dev);

    if (p_host == NULL) {
        return -AW_EINVAL;
    }

    if (p_host->p_drvfuns->pfn_cmd_request_cancel != NULL) {
        ret = p_host->p_drvfuns->pfn_cmd_request_cancel(
                p_host, p_dev->port_num, p_req);
    } else {
        ret = -AW_ENOTSUP;
    }

    return ret;
}

/******************************************************************************/
aw_err_t awbl_satabus_create (struct awbl_satabus_host          *p_host,
                              struct awbl_satabus_host_info     *p_info,
                              struct awbl_satabus_host_drv_funs *p_drvfuns)
{
    if (p_host == NULL || p_info == NULL || p_drvfuns == NULL) {
        return -AW_EINVAL;
    }

    memset(p_host->p_dev,
           0x00,
           sizeof(struct awbl_satabus_host) - sizeof(struct awbl_busctlr));

    p_host->p_info = p_info;
    p_host->p_drvfuns = p_drvfuns;

    /* 
     *todo 初始化 由于是热插拔设备，所以不会枚举总线下面的设备，
     *     等检测到在进行枚举
     */
     
    /* initialize detect works */
    aw_delayed_work_init(&p_host->dwork_detect,
                         (void (*)(void *))__satabus_detect_work,
                         p_host);

    /* announce the controller */
    awbl_bus_announce(&p_host->bus_ctrl,
                      AWBL_BUSID_SATA);

    if (p_host->p_drvfuns->pfn_cap_info_get != NULL) {
        if (AW_OK !=
            p_host->p_drvfuns->pfn_cap_info_get(p_host, &p_host->cap_info)) {
            p_host->cap_info.port_max_num = AWBL_SATABUS_DEV_MAX_NUM;
        }
    } else {
        p_host->cap_info.port_max_num = AWBL_SATABUS_DEV_MAX_NUM;
    }

    /* start detect works */
    aw_delayed_work_start(&p_host->dwork_detect, __SATABUS_DETECT_DEV_TIME);

    return AW_OK;
}

/******************************************************************************/
void awbl_satabus_init (void)
{
    awbl_bustype_register(&__g_satabus_info);
}

/* end of file */

