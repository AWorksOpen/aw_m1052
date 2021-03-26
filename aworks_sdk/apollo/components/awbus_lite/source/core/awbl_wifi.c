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
 * \brief AWBus wifi interface implementation
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_wifi.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-07-11  phd, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_assert.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "aw_wifi.h"
#include "awbl_wifi.h"
#include "awbus_lite.h"

#include <string.h>

#ifndef ENDL
    #define ENDL                    "\r\n"
#endif

#define DEV_LOCK_DECL()             AW_SEMB_DECL_STATIC(__g_dev_list_mux)
#define DEV_LOCK_INIT()             AW_SEMB_INIT(__g_dev_list_mux, AW_SEM_FULL,\
                                        AW_SEM_Q_FIFO)
#define DEV_LOCK()                  AW_SEMB_TAKE(__g_dev_list_mux, \
                                        AW_SEM_WAIT_FOREVER)
#define DEV_UNLOCK()                AW_SEMB_GIVE(__g_dev_list_mux)

#define DEV_EVENT_CB_DECL()
#define DEV_EVENT_CB_INIT()
#define DEV_EVENT_CB_CALL(n, e)

#define DEV_LIST_FOR_EACH(iter)    \
    for (iter = __g_dev_list; iter != NULL; iter = iter->p_next)

/******************************************************************************/

/**
 *  Macro for checking for NULL MAC addresses
 */
#define NULL_MAC(a)  (((((unsigned char*)a)[0])==0)&& \
                      ((((unsigned char*)a)[1])==0)&& \
                      ((((unsigned char*)a)[2])==0)&& \
                      ((((unsigned char*)a)[3])==0)&& \
                      ((((unsigned char*)a)[4])==0)&& \
                      ((((unsigned char*)a)[5])==0))

/******************************************************************************/

#define OP_LOCK_WAIT_TIMEOUT    (10 * 1000)

/******************************************************************************/

#define __OPLOCK_INIT(o)  AW_SEMB_INIT(o->oplock, AW_SEM_FULL, AW_SEM_Q_FIFO)
#define __OPLOCK(o)       AW_SEMB_TAKE(o->oplock, AW_SEM_WAIT_FOREVER)
#define __OPTRYLOCK(o)    AW_SEMB_TAKE(o->oplock, \
                              aw_ms_to_ticks(OP_LOCK_WAIT_TIMEOUT))
#define __OPUNLOCK(o)     AW_SEMB_GIVE(o->oplock)

/******************************************************************************/

aw_local aw_err_t aw_wifi_scan_internal (awbl_wifi_service_t     *p_serv,
                                         aw_wifi_scan_callback_t  pfn_cb,
                                         void                    *p_arg,
                                         aw_wifi_ssid_t          *p_ssid,
                                         aw_wifi_mac_t           *p_bssid,
                                         aw_wifi_scan_type_t      type);

aw_local aw_err_t aw_wifi_scan_abort_internal (awbl_wifi_service_t *p_serv);

aw_local aw_err_t aw_wifi_get_ap_info_internal (awbl_wifi_service_t *p_serv,
                                                aw_wifi_ssid_t      *p_ssid,
                                                aw_wifi_mac_t       *p_bssid,
                                                aw_wifi_ap_info_t   *p_info);

/*******************************************************************************
  locals
*******************************************************************************/

aw_local awbl_wifi_service_t *__g_dev_list = NULL;
aw_local int                  __g_dev_cnt = 0;

DEV_EVENT_CB_DECL();

DEV_LOCK_DECL();

/**
 * \brief find out a service by id
 */
aw_inline aw_local awbl_wifi_service_t *__id_to_serv (aw_wifi_id_t id)
{
    return (awbl_wifi_service_t *) id;
}

aw_local aw_bool_t __is_exist (const awbl_wifi_service_t *p_dev)
{
    awbl_wifi_service_t *iter;

    DEV_LIST_FOR_EACH(iter) {
        if (iter == p_dev) {
            return AW_TRUE;
        }
    }
    return AW_FALSE;
}

aw_local void __dev_release (struct aw_refcnt *p_ref)
{
#if 0
    awbl_wifi_service_t *iter = (awbl_wifi_service_t *) AW_CONTAINER_OF(
            p_ref, aw_netif_t, refcnt);

    if (iter->p_servops->release) {
        iter->p_servops->release(iter);
    }
#else
    (void) p_ref;
#endif
}

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief 获取工作模式
 * \param id            设备 ID
 * \param p_mode        工作模式
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_opmode_get (aw_wifi_id_t id, aw_wifi_opmode_t *p_mode)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        if (p_mode) {
            *p_mode = p_serv->config.opmode;
            return AW_OK;
        }
        return AW_ERROR;
    }
    return -AW_ENODEV;
}

/**
 * \brief 设置工作模式
 * \param id            设备 ID
 * \param mode          工作模式
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_opmode_set (aw_wifi_id_t id, aw_wifi_opmode_t mode)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        p_serv->config.opmode = mode;
        return AW_OK;
    }
    return -AW_ENODEV;
}

/**
 * \brief 获取运行参数
 * \param id            设备 ID
 * \param p_config      运行参数
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_config_get (aw_wifi_id_t id, aw_wifi_config_t *p_config)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        if (p_config) {
            memcpy(p_config, &p_serv->config, sizeof(aw_wifi_opmode_t));
            return AW_OK;
        }
        return AW_ERROR;
    }
    return -AW_ENODEV;
}

/**
 * \brief 设置运行参数
 * \param id            设备 ID
 * \param p_config      运行参数
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_config_set (aw_wifi_id_t id, aw_wifi_config_t *p_config)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        if (p_config) {
            memcpy(&p_serv->config, p_config, sizeof(aw_wifi_config_t));
            return AW_OK;
        }
        return AW_ERROR;
    }
    return -AW_ENODEV;
}

/**
 * \brief 应用运行参数
 * \param id            设备 ID
 * \param force         强制重新配置，AP会关闭后重新开启，STA会重连
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_config_apply (aw_wifi_id_t id, aw_bool_t force)
{
    aw_err_t err = -AW_ENODEV;
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        if (AW_OK != __OPTRYLOCK(p_serv)) {
            AW_ERRF(("Can not take oplock" ENDL));
            return -AW_EAGAIN;
        }
        err = p_serv->p_servops->pfn_config_apply(p_serv->p_cookie,
                                                  &p_serv->config,
                                                  &p_serv->status,
                                                  force);
        __OPUNLOCK(p_serv);
    }
    return err;
}

aw_err_t aw_wifi_status_get (aw_wifi_id_t id, aw_wifi_status_t *p_status)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        if (p_status) {
            memcpy(p_status, &p_serv->status, sizeof(aw_wifi_status_t));
            return AW_OK;
        }
        return AW_ERROR;
    }
    return -AW_ENODEV;
}

aw_err_t aw_wifi_scan (aw_wifi_id_t             id,
                       aw_wifi_scan_callback_t  pfn_cb,
                       void                    *p_arg,
                       aw_wifi_ssid_t          *p_ssid,
                       aw_wifi_mac_t           *p_bssid,
                       aw_wifi_scan_type_t      type)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        return aw_wifi_scan_internal(p_serv,
                                     pfn_cb,
                                     p_arg,
                                     p_ssid,
                                     p_bssid,
                                     type);
    }
    return -AW_ENODEV;
}

aw_err_t aw_wifi_abort_scan (aw_wifi_id_t id)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        return aw_wifi_scan_abort_internal(p_serv);
    }
    return -AW_ENODEV;
}

aw_err_t aw_wifi_join (aw_wifi_id_t        id,
                       aw_wifi_mac_t      *p_bssid,
                       aw_wifi_ssid_t     *p_ssid,
                       aw_wifi_security_t  security,
                       aw_wifi_key_t      *p_key)
{
    aw_err_t err = -AW_ENODEV;
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    while (NULL != p_serv) {
        const awbl_wifi_servops_t *p_ops = p_serv->p_servops;

        if (AW_WIFI_STATE_STA_JOINED & p_serv->status.state) {
            return -AW_EPERM;
        }
        if (AW_OK != __OPTRYLOCK(p_serv)) {
            AW_ERRF(("Can not take oplock" ENDL));
            return -AW_EAGAIN;
        }
        err = p_ops->pfn_join(p_serv->p_cookie,
                              p_bssid,
                              p_ssid,
                              security,
                              p_key);
        if (AW_OK == err) {
            AW_BIT_SET_MASK(p_serv->status.state, AW_WIFI_STATE_STA_JOINED);
        } else {
            AW_BIT_CLR_MASK(p_serv->status.state, AW_WIFI_STATE_STA_JOINED);
            AW_ERRF(("Join failed!" ENDL));
        }
        break;
    }

    __OPUNLOCK(p_serv);
    return err;
}

aw_err_t aw_wifi_is_sta_joined (aw_wifi_id_t id, aw_bool_t *is_joined)
{
    awbl_wifi_service_t *p_serv;

    if (NULL == is_joined) {
        return -AW_EINVAL;
    }
    p_serv = __id_to_serv(id);
    if (p_serv) {
        if (AW_WIFI_STATE_STA_JOINED & p_serv->status.state) {
            *is_joined = AW_TRUE;
        } else {
            *is_joined = AW_FALSE;
        }
        return AW_OK;
    }
    return -AW_ENODEV;
}

aw_err_t aw_wifi_leave (aw_wifi_id_t id)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        aw_err_t err;

        if (!(AW_WIFI_STATE_STA_JOINED & p_serv->status.state)) {
            return -AW_EPERM;
        }
        err = p_serv->p_servops->pfn_leave(p_serv->p_cookie);
        if (AW_OK == err) {
            AW_BIT_CLR_MASK(p_serv->status.state, AW_WIFI_STATE_STA_JOINED);
        }
        return err;
    }
    return -AW_ENODEV;
}

aw_err_t aw_wifi_start_ap (aw_wifi_id_t        id,
                           aw_wifi_ssid_t     *p_ssid,
                           aw_wifi_security_t  security,
                           aw_wifi_key_t      *p_key,
                           uint8_t             ch)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);
    aw_err_t err = -AW_ENODEV;

    if (p_serv) {
        int hide_ssid = p_serv->config.ap.flags & AW_WIFI_AP_HIDDEN;

        if (AW_WIFI_STATE_AP_ON & p_serv->status.state) {
            return -AW_EPERM;
        }
        err = p_serv->p_servops->pfn_start_ap(p_serv->p_cookie,
                                              p_ssid,
                                              security,
                                              p_key,
                                              ch,
                                              hide_ssid);
        if (AW_OK == err) {
            AW_BIT_SET_MASK(p_serv->status.last_opmode, AW_WIFI_MODE_AP_ONLY);
            AW_BIT_SET_MASK(p_serv->status.state, AW_WIFI_STATE_AP_ON);
        }
    }
    return err;
}

aw_err_t aw_wifi_is_ap_on (aw_wifi_id_t id, aw_bool_t *is_on)
{
    awbl_wifi_service_t *p_serv;

    if (NULL == is_on) {
        return -AW_EINVAL;
    }
    p_serv = __id_to_serv(id);
    if (p_serv) {
        if (AW_WIFI_STATE_AP_ON & p_serv->status.state) {
            *is_on = AW_TRUE;
        } else {
            *is_on = AW_FALSE;
        }
        return AW_OK;
    }
    return -AW_ENODEV;
}

aw_err_t aw_wifi_stop_ap (aw_wifi_id_t id)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        aw_err_t err;

        if (!(AW_WIFI_STATE_AP_ON & p_serv->status.state)) {
            return -AW_EPERM;
        }
        err = p_serv->p_servops->pfn_stop_ap(p_serv->p_cookie);
        if (AW_OK == err) {
            AW_BIT_CLR_MASK(p_serv->status.state, AW_WIFI_STATE_AP_ON);
            AW_BIT_CLR_MASK(p_serv->status.last_opmode, AW_WIFI_MODE_AP_ONLY);
        }
        return err;
    }
    return -AW_ENODEV;
}

aw_err_t aw_wifi_deauth_sta (aw_wifi_id_t id, aw_wifi_mac_t *p_mac)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        return p_serv->p_servops->pfn_deauth_sta(p_serv->p_cookie, p_mac);
    }
    return -AW_ENODEV;
}

aw_err_t aw_wifi_get_sta_list (aw_wifi_id_t                      id,
                               aw_wifi_get_sta_result_handler_t  handler,
                               void                             *p_arg)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        return p_serv->p_servops->pfn_get_sta_list(p_serv->p_cookie,
                handler,
                p_arg);
    }
    return -AW_ENODEV;
}

aw_err_t aw_wifi_get_join_ap_info (aw_wifi_id_t id, aw_wifi_ap_info_t *p_info)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        return p_serv->p_servops->pfn_get_join_ap_info(p_serv->p_cookie, p_info);
    }
    return -AW_ENODEV;
}

aw_err_t aw_wifi_get_ap_info (aw_wifi_id_t       id,
                              aw_wifi_ssid_t    *p_ssid,
                              aw_wifi_mac_t     *p_bssid,
                              aw_wifi_ap_info_t *p_info)
{
    aw_err_t err = -AW_ENODEV;
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (AW_OK != __OPTRYLOCK(p_serv)) {
        AW_ERRF(("Can not take oplock" ENDL));
        return -AW_EAGAIN;
    }
    if (p_serv) {
        err = aw_wifi_get_ap_info_internal(p_serv, p_ssid, p_bssid, p_info);
    }
    __OPUNLOCK(p_serv);
    return err;
}

aw_err_t aw_wifi_link_status_get (aw_wifi_id_t         id,
                                  aw_wifi_interface_t  interface,
                                  uint32_t            *p_status)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        return p_serv->p_servops->pfn_get_link_status(p_serv->p_cookie,
                interface,
                p_status);
    }
    return -AW_ENODEV;
}

static aw_err_t aw_wifi_common_ioctl (awbl_wifi_service_t *p_serv,
                                      int                  ops,
                                      int                  name,
                                      void                *p_arg1,
                                      void                *p_arg2)
{
    aw_err_t err = -AW_ENOSYS;

    switch (ops) {

    case AW_WO_OPT_SET:
        switch (name) {

        case AW_WO_AP_HIDDEN: {
            int hide_ssid = (int) p_arg1;

            if (hide_ssid) {
                p_serv->config.ap.flags |= AW_WIFI_AP_HIDDEN;
            } else {
                p_serv->config.ap.flags &= ~AW_WIFI_AP_HIDDEN;
            }
            err = AW_OK;
        } break;
        }
        break;

    case AW_WO_OPT_GET:
        switch (name) {

        case AW_WO_AP_HIDDEN: {
            int *hide_ssid = (int *) p_arg1;

            if (p_serv->config.ap.flags & AW_WIFI_AP_HIDDEN) {
                hide_ssid = 1;
            } else {
                hide_ssid = 0;
            }
            err = AW_OK;
        } break;
        }
        break;
    }

    return err;
}

aw_err_t aw_wifi_ioctl (aw_wifi_id_t  id,
                        int           ops,
                        int           name,
                        void         *p_arg1,
                        void         *p_arg2)
{
    awbl_wifi_service_t *p_serv = __id_to_serv(id);

    if (p_serv) {
        int ret;

        ret = p_serv->p_servops->pfn_dev_ioctl(p_serv->p_cookie,
                                               ops,
                                               name,
                                               p_arg1,
                                               p_arg2);
        if (-AW_ENOSYS == ret) {
            return aw_wifi_common_ioctl(p_serv,
                                        ops,
                                        name,
                                        p_arg1,
                                        p_arg2);
        }
        return ret;
    }
    return -AW_ENODEV;
}

aw_local aw_err_t aw_wifi_scan_internal (awbl_wifi_service_t     *p_serv,
                                         aw_wifi_scan_callback_t  pfn_cb,
                                         void                    *p_arg,
                                         aw_wifi_ssid_t          *p_ssid,
                                         aw_wifi_mac_t           *p_bssid,
                                         aw_wifi_scan_type_t      type)
{
    return p_serv->p_servops->pfn_scan(p_serv->p_cookie,
                                       pfn_cb,
                                       p_arg,
                                       p_ssid,
                                       p_bssid,
                                       type);
}

aw_local aw_err_t aw_wifi_scan_abort_internal (awbl_wifi_service_t *p_serv)
{
    return p_serv->p_servops->pfn_abort_scan(p_serv->p_cookie);
}

aw_local aw_err_t aw_wifi_get_ap_info_internal (awbl_wifi_service_t *p_serv,
                                                aw_wifi_ssid_t      *p_ssid,
                                                aw_wifi_mac_t       *p_bssid,
                                                aw_wifi_ap_info_t   *p_info)
{
    aw_err_t err;
    uint32_t comp_level = 0;

    if (p_ssid != NULL && p_ssid->length != 0) {
        comp_level |= 0x01;
    }
    if (p_bssid != NULL && !NULL_MAC(p_bssid->octet)) {
        comp_level |= 0x02;
    } else {
        p_bssid = NULL;
    }

    if (0 == comp_level) {
        /* local ap config */
        aw_wifi_ap_config_t *ap = &p_serv->config.ap;

        p_info->ssid = ap->ssid;
        p_info->security = ap->security;
        p_info->channel = ap->channel;

        err = AW_OK;
    } else {
        err = p_serv->p_servops->pfn_get_ap_info(p_serv->p_cookie,
                                                 p_ssid,
                                                 p_bssid,
                                                 p_info);
    }

    return err;
}

/**
 * \brief add a wifi device to list
 */
aw_err_t awbl_wifi_add (awbl_wifi_service_t        *p_dev, 
                        const awbl_wifi_serv_inf_t *p_inf)
{
    aw_err_t res = -AW_EEXIST;

    DEV_LOCK();
    if (__is_exist(p_dev)) {
        DEV_UNLOCK();
        goto _exit;
    }
    DEV_UNLOCK();

    aw_refcnt_init(&p_dev->ref);

    __OPLOCK_INIT(p_dev);

    p_dev->p_servinf = p_inf;

    AW_DBGF(("wifi_add:%x,%s" ENDL, p_dev, p_dev->p_servinf->name));

    DEV_LOCK();
    p_dev->p_next = __g_dev_list;
    __g_dev_list = p_dev;
    __g_dev_cnt++;
    DEV_UNLOCK();

    res = AW_OK;

    DEV_EVENT_CB_CALL(p_dev, AWBL_DEV_EVENT_ADD);

_exit:

    return res;
}

/**
 * \brief remove a wifi device from list
 */
aw_err_t awbl_wifi_remove (awbl_wifi_service_t *p_dev)
{
    aw_err_t res = -AW_ENOENT;

    if (p_dev == NULL) {
        return -AW_EPERM;
    }

    do {
        DEV_LOCK();

        /* is it the first device? */
        if (p_dev == __g_dev_list) {
            __g_dev_list = p_dev->p_next;
            res = AW_OK;
        } else {
            /* look for further down the list */
            awbl_wifi_service_t *iter;
            DEV_LIST_FOR_EACH(iter) {
                if (iter->p_next == p_dev) {
                    iter->p_next = p_dev->p_next;
                    res = AW_OK;
                    DEV_UNLOCK();
                    break;
                }
            }
            if (iter == NULL) {
                DEV_UNLOCK();
                break; /* we didn't find match today */
            }
        }
        __g_dev_cnt--;
        AW_DBGF(("wifi_del:%x,%s" ENDL, p_dev, p_dev->p_servinf->name));

        DEV_UNLOCK();
        DEV_EVENT_CB_CALL(p_dev, AWBL_DEV_EVENT_DEL);
    } while (0);

    return res;
}

/**
 * \brief remove a wifi device from list
 */
aw_err_t awbl_wifi_remove_by_id (aw_wifi_id_t id)
{
    return awbl_wifi_remove(__id_to_serv(id));
}

/**
 * \brief get how much wifi devices on the list
 */
int awbl_wifi_get_num (void)
{
    return __g_dev_cnt;
}

/**
 * \brief open a wifi device by name
 */
aw_wifi_id_t aw_wifi_dev_open (const char *p_name)
{
    aw_wifi_id_t id = AW_WIFI_ID_INVALID;
    awbl_wifi_service_t *iter;

    DEV_LOCK();
    DEV_LIST_FOR_EACH(iter) {
        if (0 == strcmp(p_name, iter->p_servinf->name)) {
            aw_refcnt_get(&iter->ref);
            id = (aw_wifi_id_t) iter;
            break;
        }
    }
    DEV_UNLOCK();

    return id;
}

/**
 * \brief get the wifi device with the number. [no] start with 0
 */
aw_wifi_id_t aw_wifi_open_by_no (int no)
{
    awbl_wifi_service_t *id;
    int i;

    if (__g_dev_cnt <= no) {
        return AW_WIFI_ID_INVALID;
    }

    DEV_LOCK();
    for (id = __g_dev_list, i = 0; NULL != id && i < no; id = id->p_next, i++) {
    }
    DEV_UNLOCK();
    aw_refcnt_get(&id->ref);

    return (aw_wifi_id_t) id;
}

aw_err_t aw_wifi_dev_close (aw_wifi_id_t id)
{
    awbl_wifi_service_t *p_serv;

    if (AW_WIFI_ID_INVALID == id) {
        return -AW_ENODEV;
    }
    p_serv = __id_to_serv(id);
    aw_refcnt_put(&p_serv->ref, __dev_release);
    return AW_OK;
}

void aw_wifi_init (void)
{
    DEV_LOCK_INIT();
    DEV_EVENT_CB_INIT();
}

/* end of file */
