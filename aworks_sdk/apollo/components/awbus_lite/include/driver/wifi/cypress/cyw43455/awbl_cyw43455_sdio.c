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
 * \brief AWBus cyw43455 sdio driver
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-17  mex, first implementation
 * \endinternal
 */


#include "driver/wifi/cypress/cyw43455/awbl_cyw43455_drv_common.h"
#include "driver/wifi/cypress/cyw43455/awbl_cyw43455_sdio.h"
#include "aw_mem.h"


#ifndef ENDL
    #define ENDL                    "\r\n"
#endif

aw_import aw_const awbl_wifi_servops_t __g_wifi_cyw43455_drv_srv_ops;

struct awbl_sdio_host                   *__gp_cyw43455_sdio_host = NULL;
awbl_sdio_cyw43455_wlan_dev_info_t      *__gp_cyw43455_sdio_info = NULL;
struct awbl_dev                         *__gp_cyw43455_sdio_dev  = NULL;

aw_local awbl_sdio_cyw43455_wlan_dev_t  *__gp_cyw43455_wlan_dev = NULL;

/*
 * sdio_cyw43455 driver
 */
aw_local aw_bool_t __sdio_cyw43455_drv_probe (struct awbl_dev *p_dev)
{
    return AW_TRUE;
}

aw_local void __sdio_cyw43455_inst_init1 (struct awbl_dev *p_awdev)
{
}

wwd_result_t awbl_43455_sdio_transfer (wwd_bus_transfer_direction_t  direction,
                                       int                           function,
                                       uint32_t                      address,
                                       uint16_t                      data_size,
                                       uint8_t                      *data)
{
    int ret;
    int blk_size = 0;
    uint8_t dir = 0;
    aw_bool_t  ram_flg;

    if (__gp_cyw43455_sdio_host == NULL) {
        return WWD_DOES_NOT_EXIST;
    }
    /* 
     * Note: this function had broken retry logic (never retried), which has 
     * been removed.Failing fast helps problems on the bus get brought to
     * light more quickly and preserves the original behavior.  
     */
    if (direction == BUS_READ) {
        dir = AWBL_SDIO_RD_DATA;
        ram_flg = AW_FALSE;
    } else {
        dir = AWBL_SDIO_WR_DATA;
        ram_flg = AW_TRUE;
    }

    if (function == 2) {
        blk_size = 512;
    } else {
        blk_size = 64;
    }

    if (data_size == (uint16_t) 1) {

        ret = aw_sdio_host_io_rw_direct(__gp_cyw43455_sdio_host,
                                        function,
                                        dir,
                                        ram_flg,
                                        address,
                                        data);
    } else {
        if (data_size >= blk_size) {
            int cnt = data_size / blk_size;
            if ((cnt * blk_size) < data_size) {
                cnt++;
            }

            if (direction == BUS_READ) {
                int i = 0;
                while(cnt > 0) {

                    ret = aw_sdio_host_io_rw_externed(__gp_cyw43455_sdio_host,
                                                      function,
                                                      dir,
                                                      AW_TRUE,
                                                      address + (blk_size * i),
                                                      data + (blk_size * i),
                                                      blk_size,
                                                      1,
                                                      AW_TRUE);
                    cnt--;
                    i++;

                }

            } else {

                ret = aw_sdio_host_io_rw_externed(__gp_cyw43455_sdio_host,
                                                  function,
                                                  dir,
                                                  AW_TRUE,
                                                  address,
                                                  data,
                                                  blk_size,
                                                  cnt,
                                                  AW_TRUE);
            }
        } else {

            ret = aw_sdio_host_io_rw_externed(__gp_cyw43455_sdio_host,
                                              function,
                                              dir,
                                              AW_FALSE,
                                              address,
                                              data,
                                              0,
                                              data_size,
                                              AW_TRUE);
        }
    }

    if (ret != AW_OK) {
        aw_kprintf("WWD_SDIO_BUS: %d trans failed:%d" ENDL, data_size, ret);
        return WWD_UNFINISHED;
    }

    return WWD_SUCCESS;
}

wwd_result_t awbl_43455_sdio_enable_hsp (void)
{
    int ret;
    struct awbl_sdio_host *p_host = __gp_cyw43455_sdio_host;

    /* 
     * wiced中保证在使用CMD53以前调用该函数，
     * 且在之前已经把IO卡的BUS_WIDTH设置为4位模式
     */
    ret = awbl_sdio_host_set_bus_width(p_host, AWBL_SDIO_BUS_WIDTH_4B);
    if (ret != AW_OK) {
        AW_INFOF(("SDIO: set bus width fail." ENDL));
    }
    awbl_sdio_host_set_speed(p_host, 35000000);
    return WWD_SUCCESS;
}

wwd_result_t awbl_43455_sdio_unmask_sdio_int (void)
{

    if (__gp_cyw43455_sdio_host == NULL || __gp_cyw43455_sdio_info == NULL) {
        return WWD_DOES_NOT_EXIST;
    }

    if (__gp_cyw43455_sdio_info->base.unmask_sdio_int != NULL) {
        __gp_cyw43455_sdio_info->base.unmask_sdio_int(__gp_cyw43455_sdio_host);
    }

    return WWD_SUCCESS;
}

wwd_result_t awbl_43455_sdio_enable_int (void)
{
    if (__gp_cyw43455_sdio_host == NULL || __gp_cyw43455_sdio_info == NULL) {
        return WWD_DOES_NOT_EXIST;
    }

    if (__gp_cyw43455_sdio_info->base.enable_sdio_int != NULL) {
        __gp_cyw43455_sdio_info->base.enable_sdio_int(__gp_cyw43455_sdio_host);
    }

    return WWD_SUCCESS;
}



aw_local void __irq_handler (void *parg)
{
    wwd_thread_notify_irq();
}

wwd_result_t awbl_43455_sdio_enable_oob_int (void)
{
    __INF_DECL(p_info, __gp_cyw43455_sdio_dev);
    int ret;
#if 0 
    AW_INFOF(("enable 43455 sdio irq." ENDL));
#endif 
    aw_gpio_trigger_connect(p_info->base.irq_pin, 
                            __irq_handler, 
                            __gp_cyw43455_sdio_dev);
    ret = aw_gpio_trigger_cfg(p_info->base.irq_pin, AW_GPIO_TRIGGER_RISE);
    ret = aw_gpio_trigger_on(p_info->base.irq_pin);
    (void) ret;
    return WWD_SUCCESS;
}

uint8_t awbl_43455_sdio_get_int_pin (void)
{
    return 1;
}

aw_local void __sdio_cyw43455_inst_init2 (struct awbl_dev *p_awdev)
{
    uint32_t model = 0;
    uint8_t  num   = 0;
    awbl_sdio_cyw43455_wlan_dev_t  *p_this = NULL;
    struct awbl_sdio_host          *p_host = NULL;

    AWBL_DEV_TO_FUNC_DECL(p_func, p_awdev);
    AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_host_info, p_func->p_card->p_host);
    __INF_DECL(p_dev_info, p_awdev);

    num = p_func->num;

    /* 静态设备 */
    if (p_host_info->connect_type == AWBL_SDIO_CON_DEV_STATIC) {
        aw_sdio_static_sdio_enum(p_func);
        p_this = (awbl_sdio_cyw43455_wlan_dev_t*) p_dev_info->super.p_drv;
        p_host = p_func->p_card->p_host;
        if (p_func != p_host->p_card->p_iofunc[num - 1]) {
            AW_INFOF(("sdio:function num is not match.\r\n"));
        }
        __gp_cyw43455_sdio_info = p_dev_info;

    /* 动态设备 */
    } else {
        p_this = (awbl_sdio_cyw43455_wlan_dev_t *)
                aw_mem_alloc(sizeof(awbl_sdio_cyw43455_wlan_dev_t));
        memset(p_this, 0x00, sizeof(*p_this));
        p_func->p_drv = (void *) p_this;
        p_this->p_func = p_func;
        p_dev_info->super.p_drv = p_this;
        p_host = p_func->p_card->p_host;
        __gp_cyw43455_sdio_info = (awbl_sdio_cyw43455_wlan_dev_info_t*)
                aw_mem_alloc(sizeof(awbl_sdio_cyw43455_wlan_dev_info_t));
        memset(__gp_cyw43455_sdio_info, 0x00, sizeof(*__gp_cyw43455_sdio_info));
        memcpy(&__gp_cyw43455_sdio_info->super, 
               &p_dev_info->super, 
               sizeof(p_dev_info->super));
        /* add base */
        extern awbl_cyw43455_cfg_info_t g_cyw43455_cfg_info;
        memcpy(&__gp_cyw43455_sdio_info->base, 
               &g_cyw43455_cfg_info, 
               sizeof(__gp_cyw43455_sdio_info->base));

        aw_sdio_func_refcnt_get(p_func);
    }

    AW_INFOF((ENDL "Found a %s cyw43455 wifi device" ENDL,
        p_host_info->connect_type == 
            AWBL_SDIO_CON_DEV_STATIC ? "static" : "dynamic"));

    __gp_cyw43455_sdio_host = p_host;
    __gp_cyw43455_sdio_dev  = p_awdev;
    __gp_cyw43455_wlan_dev  = p_this;

    p_this->base.netif.sta.interface = WWD_STA_INTERFACE;
    p_this->base.netif.ap.interface  = WWD_AP_INTERFACE;

    p_this->base.serv.p_servops = &__g_wifi_cyw43455_drv_srv_ops;
    p_this->base.serv.p_cookie = (void *) p_awdev;

    awbl_wifi_add(&p_this->base.serv, &p_dev_info->base.servinf);
    AW_BIT_SET_MASK(p_this->base.serv.status.state, 
                    AW_WIFI_STATE_EXIST | AW_WIFI_STATE_POWER);

    return;
}

aw_local void __sdio_cyw43455_inst_connect (struct awbl_dev *p_awdev)
{

/******************************************************************************/
    awbl_wiced_inst_connect(p_awdev);
    awbl_sdio_host_set_speed(__gp_cyw43455_sdio_host, 50000000);
}

aw_local aw_err_t __sdio_cyw43455_match_type (struct awbl_dev *p_awdev, 
                                              void            *p_arg)
{
    struct awbl_sdio_dev_info *p_info = (struct awbl_sdio_dev_info *) 
                                            p_awdev->p_devhcf->p_devinfo;
    AWBL_DEV_TO_FUNC_DECL(p_func, p_awdev);

    if (p_info->type  != AWBL_SDIO_TYPE_IO  ||
       ((p_info->class != AWBL_SDIO_CLASS_WLAN) &&
       (p_info->class != AWBL_SDIO_CLASS_NONE))) {
        return AW_ERROR;
    }

    if (p_info->vendor == AWBL_SDIO_CYW43455_WLAN_VID  &&
        p_info->device == AWBL_SDIO_CYW43455_WLAN_PID  &&
        p_func->num    == AWBL_SDIO_CYW43455_WLAN_NUM) {
        return AW_OK;
    }

    return AW_ERROR;
}


void awbl_cyw43455_sdio_release (aw_netif_t *p_ethif)
{
    if (__gp_cyw43455_sdio_info) {
        aw_mem_free(__gp_cyw43455_sdio_info);
        __gp_cyw43455_sdio_info = NULL;
    }
    if (__gp_cyw43455_wlan_dev) {
        aw_mem_free(__gp_cyw43455_wlan_dev);
        __gp_cyw43455_wlan_dev = NULL;
    }
    __gp_cyw43455_sdio_host = NULL;
    __gp_cyw43455_sdio_dev = NULL;
}


aw_local aw_err_t __sdio_cyw43455_del_card (struct awbl_dev *p_awdev)
{
    __DEV_DECL(p_this, p_awdev);
    AWBL_DEV_TO_FUNC_DECL(p_func, p_awdev);
    awbl_wifi_remove(&p_this->base.serv);
    aw_netif_remove(&p_this->base.netif.ap.ethif);
    aw_netif_remove(&p_this->base.netif.sta.ethif);
    wiced_wlan_connectivity_deinit(); /* TODO: run test */
    aw_sdio_func_refcnt_put(p_func);
    return AW_OK;
}

aw_local aw_const struct awbl_dev_method __g_sdio_cyw43455_methods[] = {
    AWBL_METHOD(awbl_sdio_match_type,  __sdio_cyw43455_match_type),
    AWBL_METHOD(awbl_drv_unlink,       __sdio_cyw43455_del_card),
    AWBL_METHOD_END
};

/**
 * \brief driver functions
 */
aw_local aw_const struct awbl_drvfuncs __g_sdio_cyw43455_drvfuncs = {
    __sdio_cyw43455_inst_init1,
    __sdio_cyw43455_inst_init2,
    __sdio_cyw43455_inst_connect
};

/**
 * \brief driver information
 */
aw_local aw_const struct awbl_sdio_dev_drvinfo __g_sdio_cyw43455_drv_registration = {
    {
        AWBL_VER_1,                     /* awb_ver */
        AWBL_BUSID_SDIO,                /* bus_id */
        AWBL_CYW43455_SDIO_NAME,        /* p_drvname */
        &__g_sdio_cyw43455_drvfuncs,    /* p_busfuncs */
        __g_sdio_cyw43455_methods,      /* p_methods */
        __sdio_cyw43455_drv_probe       /* pfunc_drv_probe */
    },
    AWBL_SDIO_TYPE_IO,
    {{
        AWBL_SDIO_CLASS_WLAN,           /* class */
        AWBL_SDIO_CYW43455_WLAN_VID,    /* vendor */
        AWBL_SDIO_CYW43455_WLAN_PID,    /* device */
        NULL                            /* driver_data */
    }}
};

void awbl_cyw43455_sdio_drv_register (void)
{
    (void) awbl_drv_register(
        (struct awbl_drvinfo *) &__g_sdio_cyw43455_drv_registration);
}

/* end of file */
