/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#include "aworks.h"
#include "stdio.h"
#include "string.h"
#include "awbus_lite.h"
#include "awbl_plb.h"

#include "aw_gpio.h"
#include "aw_sem.h"
#include "aw_mem.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_assert.h"
#include "aw_gprs.h"

#include "driver/gps/awbl_gnss_gprs.h"
/******************************************************************************/
#define PFX  "GNSS Drv(%d)"
#define ENDL "\r\n\t"

#define __LOG_FCOLOR_RED        "\x1B[31m"
#define __LOG_FCOLOR_GREEN      "\x1B[32m"

#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {\
         __log_out(color, PFX, __LINE__, fmt, ##__VA_ARGS__);\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[" PFX "] " fmt "\n", __LINE__, ##__VA_ARGS__)

#define __LOG_RES(is_true, mask, fmt, ...) \
    if (is_true) {\
        __LOG_BASE(mask, __LOG_FCOLOR_GREEN, "[ OK ] [" PFX "] " fmt "\n", \
                     __LINE__, ##__VA_ARGS__);\
    } else {\
        __LOG_BASE(1, __LOG_FCOLOR_RED,\
                     "[ ER ] [%s:%d] " fmt "\n", \
                     __func__, __LINE__, ##__VA_ARGS__);\
    }

/******************************************************************************/
#define __GNSS_GPRS_DEV_DECL(p_this, p_dev) \
    struct awbl_gnss_gprs_dev *p_this = \
    (struct awbl_gnss_gprs_dev *)AW_CONTAINER_OF(p_dev, struct awbl_gnss_gprs_dev, super)


#define __GNSS_GPRS_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_gnss_gprs_devinfo *p_devinfo = \
        (struct awbl_gnss_gprs_devinfo *)AWBL_DEVINFO_GET(p_dev)

/******************************************************************************/
aw_local  aw_err_t __gnss_gprs_start (void *p_cookie);
aw_local  aw_err_t __gnss_gprs_end (void *p_cookie);

/******************************************************************************/
aw_local awbl_gnss_gprs_dev_t *__gp_gnss_list = NULL;

/******************************************************************************/
/* 获取当前时间*/
aw_local uint32_t __cur_ms_time_get (void)
{
    return aw_ticks_to_ms(aw_sys_tick_get());
}

/******************************************************************************/
#ifdef __VIH_DEBUG
#include "aw_shell.h"
static void __log_out ( void         *p_color,
                        const char   *func_name,
                        int           line,
                        const char   *fmt, ...)
{
    char        log_buf[256 + 1];
    uint16_t    len;
    va_list     ap;

    va_start( ap, fmt );
    len = aw_vsnprintf((char *)log_buf, sizeof(log_buf), fmt, ap);
    va_end( ap );
    if (len >= sizeof(log_buf)) {
        aw_assert(len < sizeof(log_buf) - 1);
    }

    aw_shell_printf_with_color(AW_DBG_SHELL_IO, p_color, func_name, log_buf);
}
#else
#define __log_out(fmt, ...)
#endif  /* __VIH_DEBUG */

/******************************************************************************/
/* 通过 gprs id 来获取 gnss_gprs 设备 */
aw_local awbl_gnss_gprs_dev_t * __gnss_dev_get_by_id (int gprs_devid)
{
    awbl_gnss_gprs_dev_t        *p_gnss = __gp_gnss_list;
    awbl_gnss_gprs_devinfo_t    *p_devinfo;

    while (p_gnss) {
        p_devinfo = (awbl_gnss_gprs_devinfo_t *)AWBL_DEVINFO_GET(&p_gnss->super);
        if (p_devinfo->gprs_dev_id == gprs_devid) {
            return p_gnss;
        }
        p_gnss = p_gnss->p_next;
    }

    return NULL;
}

/******************************************************************************/
aw_local aw_err_t __gnss_gprs_soft_rest (void  *p_cookie)
{
    aw_err_t ret = AW_OK;

    ret = __gnss_gprs_end(p_cookie);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __gnss_gprs_start(p_cookie);
    if (ret != AW_OK) {
        return ret;
    }
    return AW_OK;
}
/******************************************************************************/
aw_local void __gprs_evt_handler (int devid, aw_gprs_evt_t *p_evt)
{
    awbl_gnss_gprs_dev_t        *p_this = __gnss_dev_get_by_id(devid);
    struct awbl_gnss_service    *p_serv = &p_this->gnss_serv;
    aw_err_t                     ret = -AW_ERROR;

    aw_assert(p_this);

    /* The callback is called by the GPRS module middle layer and does not
     * allow blocking for too long. */

    switch (p_evt->type) {
        case AW_GPRS_EVT_TYPE_PWR_ON: {
            p_this->gnss_sts = AWBL_GNSS_GPRS_STS_PWR_ON;
            __LOG_PFX(1, "GPRS reported an event of power on.");

            /**
             * 如果之前使用 aw_gnss_enable 成功，又未调用  disable 关闭，则
             * gprs 上电后，开启 gnss 功能
             */
            if (p_serv->dev_is_enable) {
                ret = __gnss_gprs_start(p_serv->p_cookie);
                __LOG_RES(ret == AW_OK, 1,
                          "Automatically restart GNSS function of GPRS.");
            }

        } break;

        case AW_GPRS_EVT_TYPE_PWR_OFF_PRE: {
            p_this->gnss_sts = AWBL_GNSS_GPRS_STS_PWR_OFF_PRE;

            /* Since the GPRS module handles the status of the GNSS function
             * automatically, there is no need to perform aw_gnss_disable. */

            /* Gets the GNSS device lock, and if successfully, the device is idle. */
            ret = AW_MUTEX_LOCK(p_serv->op_mutex, AW_NO_WAIT);
            if (ret == AW_OK) {
                /* Return OK and then the GPRS module will begin to perform
                 * the power off operation.*/
                p_evt->u.pwr_off_pre.retval = AW_OK;
                __LOG_PFX(1, "Receive a power request shutdown event reported by"
                          " GPRS module, return the permission.");
            } else {
                p_evt->u.pwr_off_pre.retval = -AW_EBUSY;
                __LOG_PFX(1, "Receive a power request shutdown event reported by"
                          " GPRS module, return rejection " ENDL
                          "because GNSS device is currently busy.");
            }
            AW_MUTEX_UNLOCK(p_serv->op_mutex);

        } break;

        case AW_GPRS_EVT_TYPE_PWR_OFF: {
            p_this->gnss_sts = AWBL_GNSS_GPRS_STS_PWR_OFF;
            __LOG_PFX(1, "GPRS reported an event off power off.");
        } break;

        case AW_GPRS_EVT_TYPE_RESET_PRE: {
            p_this->gnss_sts = AWBL_GNSS_GPRS_STS_PWR_OFF_PRE;

            /* Gets the GNSS device lock, and if successfully, the device is idle. */
            ret = AW_MUTEX_LOCK(p_serv->op_mutex, AW_NO_WAIT);
            if (ret == AW_OK) {
                p_evt->u.reset_pre.retval = AW_OK;
                __LOG_PFX(1, "Receive a reset request event reported by GPRS "
                         "module, return the " ENDL "permitted operation." );
            } else {
                p_evt->u.reset_pre.retval = -AW_EBUSY;
                __LOG_PFX(1, "Receive a reset request event reported by GPRS "
                          "module, return reject because GNSS " ENDL
                          "device is currently busy.");
            }
            AW_MUTEX_UNLOCK(p_serv->op_mutex);
        } break;

        case AW_GPRS_EVT_TYPE_RESET_COMPLETED: {
            p_this->gnss_sts = AWBL_GNSS_GPRS_STS_PWR_ON;
            __LOG_PFX(1, "GPRS reported an event for the module to complete the reset.");

            /**
             * 如果之前使用 aw_gnss_enable 成功，又未调用  disable 关闭，则
             * gprs 上电后，开启 gnss 功能
             */
            if (p_serv->dev_is_enable) {
                ret = __gnss_gprs_start(p_serv->p_cookie);
                __LOG_RES(ret == AW_OK, 1,
                          "Automatically restart GNSS function of GPRS.");
            }
        } break;
        default:
            break;
    }
}

/******************************************************************************/
/**
 * gnss service 回调函数实现
 */

/* 启动 EC20 GNSS 功能*/
aw_local  aw_err_t __gnss_gprs_start (void *p_cookie)
{
    __GNSS_GPRS_DEV_DECL(        p_this, p_cookie);
    __GNSS_GPRS_DEVINFO_DECL(    p_devinfo, p_cookie);
    aw_err_t                     ret;
    struct awbl_gnss_service    *p_serv = &p_this->gnss_serv;
    aw_gprs_ioctl_args_t         arg;

    if ((!p_cookie) || (&p_this->gnss_serv == NULL)) {
        return -AW_EINVAL;
    }

    /* 等待初始化完成 */
    while (p_serv->init_stat == AWBL_GNSS_INIT_WAIT) {
        aw_mdelay(10);
    }

#if 0     /* 该驱动中不控制 gprs 模块的电源，由外部控制   */
    /* 启动 gprs 模式  */
    ret = aw_gprs_power_on(p_devinfo->gprs_dev_id);
    if (ret != AW_OK) {
        return ret;
    }
#endif

    /*
     * 设置 gprs 事件回调，在 gprs 被掉电时，gps 相关的状态也要设置为 off;
     * devinfo 中不提供回调，由 ioctl 添加
     */
    aw_assert(p_devinfo->gnss_cfg.pfn_evt_cb == NULL);
    arg.pfn_evt_handler = __gprs_evt_handler;
    ret = aw_gprs_ioctl(p_devinfo->gprs_dev_id, AW_GPRS_EVT_CB_SET, &arg);
    /* 不成功且不为已添加，则失败退出   */
    if (ret != AW_OK) {
        if (ret == -AW_EEXIST) {
            __LOG_PFX(1, "The callback function has been set and "
                          "cannot be set repeatedly!");
        } else {
            __LOG_PFX(1, "Failed to set callback function of GPRS event."
                          "(ret:%d)", ret);
            return ret;
        }
    }

    /* 检查 gprs 模块是否已上电，未上电则退出  */
    if (p_this->gnss_sts != AWBL_GNSS_GPRS_STS_PWR_ON) {
        return -AW_EPERM;
    }

    /* 使能 gprs 的 gnss 功能   */
    ret = aw_gprs_gnss_enable(p_devinfo->gprs_dev_id, &p_devinfo->gnss_cfg);
    if (ret != AW_OK) {
        __LOG_RES(AW_FALSE, 1, "GNSS function of GPRS failed to turn on.(ret:%d)", ret);
        return ret;
    }
    __LOG_PFX(1, "GNSS function of GPRS has been turned on successfully.");

    p_this->gnss_sts = AWBL_GNSS_GPRS_STS_RUNNING;

    return AW_OK;
}

/* 关闭 EC20 GNSS 功能*/
aw_local  aw_err_t __gnss_gprs_end (void *p_cookie)
{
    __GNSS_GPRS_DEV_DECL(        p_this, p_cookie);
    __GNSS_GPRS_DEVINFO_DECL(    p_devinfo, p_cookie);
    aw_err_t                     ret;
    struct awbl_gnss_service    *p_serv = &p_this->gnss_serv;

    if ((!p_cookie) || (&p_this->gnss_serv == NULL)) {
        return -AW_EINVAL;
    }

    /* 等待初始化完成 */
    while (p_serv->init_stat == AWBL_GNSS_INIT_WAIT) {
        aw_mdelay(10);
    }

    /* 检查 gprs 模块是否已上电，未上电则退出  */
    if (p_this->gnss_sts != AWBL_GNSS_GPRS_STS_PWR_ON) {
        ret = AW_OK;
        goto exit;
    }
    /* 检查 gnss 功能是否已使能，没有则退出  */
    if (p_this->gnss_sts != AWBL_GNSS_GPRS_STS_RUNNING) {
        ret = AW_OK;
        goto exit;
    }

    ret = aw_gprs_gnss_disable(p_devinfo->gprs_dev_id);
    if (ret != AW_OK) {
        goto exit;
    }

    /* 关闭 gnss 功能后，gprs 模块仍然是已上电状态  */
    p_this->gnss_sts = AWBL_GNSS_GPRS_STS_PWR_ON;


exit:
    if (ret == AW_OK) {
        __LOG_PFX(1, "GNSS function of GPRS has been turned off successfully.");
    } else {
        __LOG_RES(AW_FALSE, 1, "GNSS function of GPRS failed to turn off.(ret:%d)", ret);
    }
    return AW_OK;
}

/* EC20 GNSS IO control
 * p_cookie ：GNSS 服务参数
 * req      ：具体请求
 * arg      ：参数
 * 返回                   ：成功返回AW_OK*/
aw_local  aw_err_t __gnss_gprs_ioctl (void                  *p_cookie,
                                      aw_gnss_ioctl_req_t    req,
                                      void                  *arg)
{
    aw_err_t ret = AW_OK;
    __GNSS_GPRS_DEV_DECL(p_this, p_cookie);
    struct awbl_gnss_service *p_serv = &p_this->gnss_serv;

    if ((!p_cookie) || (&p_this->gnss_serv == NULL)) {
        return -AW_EINVAL;
    }

    /* 检查 gprs 模块是否已上电，未上电则退出  */
    if (p_this->gnss_sts != AWBL_GNSS_GPRS_STS_PWR_ON) {
        return -AW_EBUSY;
    }

    ret = AW_OK;
    switch (req) {
        case AW_GNSS_SYS_FREQ:
            p_serv->sys_freq = (uint32_t)arg;
            break;
        case AW_GNSS_SRESET:
            ret = __gnss_gprs_soft_rest(p_cookie);
            break;
        default:
            /* 其它选项返回 不支持 */
            ret = -AW_ENOTSUP;
            break;
    }
    return ret;
}

/* GNSS 获取数据
 * p_cookie ：GNSS 服务参数
 * type     ：数据类型
 * ptr      ：数据缓存
 * len      ：数据缓存长度
 * 返回                   ：成功返回AW_OK*/
aw_local aw_err_t __gnss_gprs_dat_get (void                  *p_cookie,
                                       aw_gnss_data_type_t    type,
                                       void                  *ptr,
                                       uint8_t                len)
{

    int i;
    __GNSS_GPRS_DEV_DECL(        p_this, p_cookie);
    __GNSS_GPRS_DEVINFO_DECL(    p_devinfo, p_cookie);
    uint8_t                      read_buf[1024];
    uint32_t                     use_tim = 0;
    uint32_t                     flag_sta = 0;
    aw_err_t                     ret = AW_ERROR;
    aw_bool_t                    reflesh_req = 0;
    aw_bool_t                    is_timeover = AW_TRUE;
    int                          read_len;
    struct awbl_gnss_service    *p_serv = &p_this->gnss_serv;

    if(p_serv == NULL){
        return -AW_EINVAL;
    }

    if (p_this->gnss_sts != AWBL_GNSS_GPRS_STS_RUNNING) {
        return -AW_EPERM;
    }

    /* 结束计时，并判断是否超过有效时间，是则清空data */
    use_tim = (__cur_ms_time_get() - p_serv->begin_tim);

    is_timeover = (uint32_t)use_tim >= p_serv->valid_tim ? AW_TRUE : AW_FALSE;

    read_len = 0;
    if (reflesh_req || is_timeover) {
        memset(&p_serv->data, 0, sizeof(p_serv->data));
        memset(read_buf, 0, sizeof(read_buf));

        for(i = 0;i < 3;i++){
            read_len = aw_gprs_gnss_nmeasrc_get(p_devinfo->gprs_dev_id,
                                                read_buf,
                                                1024,
                                                500);
            if(read_len > 0){
                break;
            }
        }
    }

    ret = -AW_ERROR;
    if (read_len > 0) {
        ret = awbl_nmea_0183_dat_decode(&p_serv->data,
                                         read_buf,
                                         p_serv->sys_freq,
                                         p_serv->func_flag,
                                         &flag_sta);
    }
    if (ret == AW_OK) {
        /* 获取到数据则重置数据有效时间   */
        p_serv->begin_tim = __cur_ms_time_get();
    } else if (is_timeover) {
        return -AW_ETIME;
    }
    ret = awbl_nmea_0183_get_dat(&p_serv->data, type, ptr, len);

    return ret;
}

/******************************************************************************/
/* GPS 设备初始化
 * p_dev：基本设备类  */
aw_local void __gnss_gprs_inst_connect (awbl_dev_t *p_dev)
{
    __GNSS_GPRS_DEV_DECL(     p_this, p_dev);
    struct awbl_gnss_service *p_serv = &p_this->gnss_serv;

    aw_assert(p_serv);

    /* GNSS 服务状态*/
    p_serv->init_stat = AWBL_GNSS_INIT_OK;
    /* GNSS 服务系统频率*/
    p_serv->sys_freq  = AW_GNSS_GPS_L1;
    /* GNSS 服务有效时间*/
    p_serv->valid_tim = 500;
    /* GNSS 服务起始时间*/
    p_serv->begin_tim = 0;
}


/******************************************************************************/

/* gnss_gprs service functions (must defined as aw_const) */
aw_local aw_const struct awbl_gnss_servfunc __g_gnss_gprs_servfunc = {
    __gnss_gprs_start,
    __gnss_gprs_end,
    __gnss_gprs_ioctl,
    __gnss_gprs_dat_get,
};

/**
 * \brief method "__gnss_serv_get" handler
 */
aw_local aw_err_t __gnss_gprs_serv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __GNSS_GPRS_DEV_DECL(        p_this, p_dev);
    __GNSS_GPRS_DEVINFO_DECL(    p_devinfo, p_dev);
    struct awbl_gnss_service    *p_serv = NULL;

    if (!p_dev) {
        return -AW_EINVAL;
    }
    p_this->p_next  = __gp_gnss_list;
    __gp_gnss_list = p_this;

    /* 各个设备的gnss服务函数 */
    p_serv = &p_this->gnss_serv;

    p_this->gnss_serv.init_stat = AWBL_GNSS_INIT_WAIT;

    /* 对服务函数进行初始化  */
    p_serv->p_next      = NULL;
    p_serv->p_com       = NULL;
    p_serv->p_servinfo  = &p_devinfo->gnss_servinfo;
    p_serv->p_servfunc  = &__g_gnss_gprs_servfunc;
    p_serv->p_cookie    = (void *)p_dev;

    *(struct awbl_gnss_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_gnss_gprs_drvfuncs = {
    NULL,
    NULL,
    __gnss_gprs_inst_connect
};

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_gnss_serv_get);

aw_local aw_const struct awbl_dev_method __g_gnss_gprs_dev_methods[] = {
    AWBL_METHOD(awbl_gnss_serv_get, __gnss_gprs_serv_get),
    AWBL_METHOD_END
};

aw_local aw_const awbl_plb_drvinfo_t __g_gnss_gprs_drv_registration = {
    {
        AWBL_VER_1,                        /* awb_ver */
        AWBL_BUSID_PLB,                    /* bus_id */
        AWBL_GNSS_GPRS,                   /* p_drvname */
        &__g_gnss_gprs_drvfuncs,               /* p_busfuncs */
        &__g_gnss_gprs_dev_methods[0],         /* p_methods */
        NULL                               /* pfunc_drv_probe */
    }
};
/******************************************************************************/
void awbl_gnss_gprs_drv_register (void)
{
    int ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_gnss_gprs_drv_registration);
    if (ret != AW_OK) {
        return;
    }
}






