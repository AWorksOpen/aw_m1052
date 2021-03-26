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
 * \brief U-BLOX7模块中GPS的驱动 和 NMEA-0183协议解码驱动
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-30  vih, first implementation.
 * \endinternal
 */

#include "apollo.h"                     /* 此头文件必须被首先包含 */
#include "driver/gps/awbl_ublox.h"
#include "aw_serial.h"
#include "aw_ioctl.h"
#include "aw_vdebug.h"
#include "awbl_plb.h"
#include "string.h"
#include "aw_gpio.h"
#include "aw_delay.h"

/******************************************************************************/
#if 0
#define __DEBUG_INFO_PRINTF              /**< \brief 开启打印调试信息 */
#endif
/******************************************************************************/
#ifdef __DEBUG_INFO_PRINTF

#define __kprintf aw_kprintf

#define __ERR_MSG(str)                               \
            __kprintf("\nU-BLOX MSG:\"%s\".\n"              \
                        "-------MSG: at file %s.\n"         \
                        "-------MSG: at line %d in %s()\n\n", \
                     str,                                 \
                     __FILE__,                            \
                     __LINE__,                            \
                     __FUNCTION__);

#else
#define __kprintf

#define __ERR_MSG

#endif

/******************************************************************************/
#define __UBLOX_DEV_DECL(p_this, p_dev) \
    struct awbl_ublox_dev *p_this = \
    (struct awbl_ublox_dev *)AW_CONTAINER_OF(p_dev, struct awbl_ublox_dev, super)


#define __UBLOX_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_ublox_devinfo *p_devinfo = \
        (struct awbl_ublox_devinfo *)AWBL_DEVINFO_GET(p_dev)



/******************************************************************************/
aw_local aw_err_t __ublox_serv_get (struct awbl_dev *p_dev, void *p_arg);


aw_local void __ublox_inst (awbl_dev_t *p_dev);
aw_local void __ublox_inst_connect(awbl_dev_t *p_dev);

aw_local   aw_err_t __ublox_ioctl (void *p_cookie, 
                                   aw_gnss_ioctl_req_t req, 
                                   void *arg);

aw_local   aw_err_t __ublox_end (void *p_cookie);
aw_local   aw_err_t __ublox_start (void *p_cookie);

/******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_ublox_drvfuncs = {
    NULL,
    NULL,
    __ublox_inst_connect
};

/* ublox service functions (must defined as aw_const) */
aw_local aw_const struct awbl_gnss_servfunc __g_ublox_servfunc = {
    __ublox_start,
    __ublox_end,
    __ublox_ioctl,
    NULL,
};


/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_gnss_serv_get);

aw_local aw_const struct awbl_dev_method __g_ublox_dev_methods[] = {
    AWBL_METHOD(awbl_gnss_serv_get, __ublox_serv_get),
    AWBL_METHOD_END
};

aw_local aw_const awbl_plb_drvinfo_t __g_ublox_drv_registration = {
    {
        AWBL_VER_1,                        /* awb_ver */
        AWBL_BUSID_PLB,                    /* bus_id */
        AWBL_UBLOX_GNSS,                   /* p_drvname */
        &__g_ublox_drvfuncs,               /* p_busfuncs */
        &__g_ublox_dev_methods[0],         /* p_methods */
        NULL                               /* pfunc_drv_probe */
    }
};

/******************************************************************************/


/* set default configure parameter */
aw_local void __ublox_dev_set_dft_cfg_par (struct awbl_ublox_dev_cfg_par *p_cfg_par)
{
    p_cfg_par->uart_num = 1;
    p_cfg_par->baud     = 9600;
    p_cfg_par->inProto  = 1;
    p_cfg_par->outProto = 3;
    p_cfg_par->reserved = 0;

    p_cfg_par->msg_type = 0;
    p_cfg_par->msg_id   = -1;
    p_cfg_par->msg_rate = 1;

    p_cfg_par->meas_rate = 1000;
    p_cfg_par->nav_rate  = 1000;
    p_cfg_par->correction_mask = 3;

    p_cfg_par->sys_mask = AW_GNSS_GPS_L1 ;  //0x11;
}


aw_local aw_inline int __the_dev_is_start (struct awbl_ublox_dev  *p_this)
{
    return p_this->gnss_serv.dev_is_enable = AW_TRUE;
}

/******************************************************************************/


aw_local void __calculate_msg_nav_rate (uint32_t *p_data_rate,
                                        uint32_t *p_msg_rate,
                                        uint32_t *p_nav_rate)
{
    if (*p_data_rate > 2000) {
        *p_data_rate = 2000;
        *p_msg_rate = 2;
        *p_nav_rate = 1000;

    } else if (*p_data_rate < 200) {
        *p_data_rate = 200;
        *p_msg_rate = 1;
        *p_nav_rate = 200;

    } else {
        if (*p_data_rate > 1000) {
            *p_nav_rate = 1000;
            *p_msg_rate = (*p_data_rate) / (*p_nav_rate);

        } else if (*p_data_rate > 500) {
            *p_nav_rate = 500;
            *p_msg_rate = (*p_data_rate) / (*p_nav_rate);
        } else if (*p_data_rate > 200) {
            *p_nav_rate = 200;
            *p_msg_rate = (*p_data_rate) / (*p_nav_rate);
        }
    }
}

aw_local aw_err_t __ublox_cfgprt (awbl_dev_t             *p_dev,
                                  int                     uart_num,
                                  uint32_t                reserved,
                                  uint32_t                baud,
                                  uint32_t                inProto,
                                  uint32_t                outProto)
{
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);
    __UBLOX_DEV_DECL(p_this, p_dev);
    int ret;

    if (p_devinfo == 0) {
        return -AW_EINVAL;
    }

    if (uart_num == -1) {
        snprintf(p_devinfo->com.buf, 
                 p_devinfo->com.size, 
                 "$CFGPRT,,%d,%d,%d,%d\r", 
                 reserved, 
                 baud, 
                 inProto, 
                 outProto);
    } else {
        snprintf(p_devinfo->com.buf, 
                 p_devinfo->com.size, 
                 "$CFGPRT,%d,%d,%d,%d,%d\r", 
                 uart_num, 
                 reserved, 
                 baud, 
                 inProto, 
                 outProto);
    }

    p_this->cfg_par.uart_num  = uart_num;
    p_this->cfg_par.baud      = baud;
    p_this->cfg_par.inProto   = inProto;
    p_this->cfg_par.outProto  = outProto;

    ret = awbl_gnss_send_wait_res(&p_this->gnss_serv, p_devinfo->com.buf, 0, 0);

    return ret;
}


aw_local aw_err_t __ublox_cfgmsg (awbl_dev_t             *p_dev,
                                  uint32_t                msg_type,
                                  char                    msg_id,
                                  uint32_t                rate)
{
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);
    __UBLOX_DEV_DECL(p_this, p_dev);

    if (p_devinfo == 0) {
        return -AW_EINVAL;
    }

    if (msg_id == -1) {
        snprintf(p_devinfo->com.buf,
                 p_devinfo->com.size,
                 "$CFGMSG,%d,,%d\r", msg_type, rate);
    } else {
        snprintf(p_devinfo->com.buf,
                 p_devinfo->com.size,
                 "$CFGMSG,%d,%d,%d\r", msg_type, msg_id, rate);
    }

    p_this->cfg_par.msg_type  = msg_type;
    p_this->cfg_par.msg_id    = msg_id;

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}

aw_local aw_err_t __ublox_cfgnav (awbl_dev_t             *p_dev,
                                  uint32_t                meas_rate,
                                  uint32_t                nav_rate,
                                  uint32_t                correction_mask)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    int ret;
    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);

    snprintf(p_devinfo->com.buf,
             p_devinfo->com.size, 
             "$CFGNAV,%d,%d,%d\r", 
             meas_rate, 
             nav_rate, 
             correction_mask);

    p_this->cfg_par.meas_rate = meas_rate;
    p_this->cfg_par.nav_rate  = nav_rate;
    p_this->cfg_par.correction_mask  = correction_mask;

    ret = awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                   p_devinfo->com.buf, 
                                   "OK", 
                                   2000);
    if (ret != AW_OK) {
        return ret;
    }

    p_this->data_rate = nav_rate * p_this->cfg_par.msg_rate;
    return AW_OK;
}

aw_local aw_err_t __ublox_cfgtp (awbl_dev_t             *p_dev,
                                 uint32_t                interval,
                                 uint32_t                length,
                                 uint32_t                flag,
                                 int                     ant_delay,
                                 int                     rf_delay,
                                 int                     usr_delay)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);

    snprintf(p_devinfo->com.buf,
             p_devinfo->com.size,
             "$CFGTP,%d,%d,%d,%d,%d,%d\r",
            interval,
            length,
            flag,
            ant_delay,
            rf_delay,
            usr_delay);

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}

aw_local aw_err_t __ublox_cfgem (awbl_dev_t             *p_dev,
                                 uint32_t                enable,
                                 uint32_t                palarity,
                                 uint32_t                clock_sync)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);

    snprintf(p_devinfo->com.buf,
             p_devinfo->com.size,
             "$CFGEM,%d,%d,%d\r", enable, palarity, clock_sync);

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}

aw_local aw_err_t __ublox_cfgnmea (awbl_dev_t             *p_dev,
                                   uint32_t                nmea_ver)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);

    snprintf(p_devinfo->com.buf,
             p_devinfo->com.size,
             "$CFGNMEA,%d\r", nmea_ver);

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}

aw_local aw_err_t __ublox_cfgsys (awbl_dev_t         *p_dev,
                                  uint32_t            sys_mask)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);

    snprintf(p_devinfo->com.buf,
             p_devinfo->com.size,
             "$CFGSYS,%d\r", sys_mask);

    p_this->cfg_par.sys_mask = sys_mask;

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}

aw_local aw_err_t __ublox_cfgdyn (awbl_dev_t             *p_dev,
                                  uint32_t                mask,
                                  uint32_t                dyn_model,
                                  uint32_t                static_hold_thresh)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);

    snprintf(p_devinfo->com.buf,
             p_devinfo->com.size,
             "$CFGDYN,%d,%d,%d\r", mask, dyn_model, static_hold_thresh);

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}

aw_local aw_err_t __ublox_cfgsave (awbl_dev_t             *p_dev,
                                   int                     mask)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);

    if (mask == -1) {
        snprintf(p_devinfo->com.buf,
                 p_devinfo->com.size,
                 "$CFGSAVE,\r");
    } else {
        snprintf(p_devinfo->com.buf,
                 p_devinfo->com.size,
                 "$CFGSAVE,%d\r", mask);
    }

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}

aw_local aw_err_t __ublox_cfgload (awbl_dev_t             *p_dev,
                                   int                     mask)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);

    if (mask == -1) {
        snprintf(p_devinfo->com.buf,
                 p_devinfo->com.size,"$CFGLOAD,\r");
    } else {
        snprintf(p_devinfo->com.buf,
                 p_devinfo->com.size,
                 "$CFGLOAD,%d\r", mask);
    }

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}

aw_local aw_err_t __ublox_cfgclr (awbl_dev_t             *p_dev,
                                  int                     mask)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);

    if (mask == -1) {
        snprintf(p_devinfo->com.buf,
                 p_devinfo->com.size,
                 "$CFGCLR,\r");
    } else {
        snprintf(p_devinfo->com.buf,
                 p_devinfo->com.size,
                 "$CFGCLR,%d\r", mask);
    }

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}


aw_inline aw_local aw_err_t __ublox_enter_cfg_mode (awbl_dev_t *p_dev)
{
    return __ublox_cfgmsg(p_dev, 0, -1, 0);
}

aw_inline aw_local aw_err_t __ublox_enter_data_mode (awbl_dev_t *p_dev)
{
    __UBLOX_DEV_DECL(p_this, p_dev);
    return __ublox_cfgmsg(p_dev, 0, -1, p_this->cfg_par.msg_rate);
}


aw_local aw_err_t __ublox_reset (awbl_dev_t             *p_dev,
                                 uint32_t               type,
                                 uint32_t               clr_mask)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);

    snprintf(p_devinfo->com.buf,
             p_devinfo->com.size,
             "$RESET,%d,%d\r", type, clr_mask);

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}


/*******************************************************************************
  service functions 
*******************************************************************************/
aw_local  aw_err_t __ublox_start (void *p_cookie)
{
    __UBLOX_DEV_DECL(p_this, p_cookie);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_cookie);
    int ret;
    uint8_t cnt = 0;

    if (!p_cookie) {
        return -AW_EINVAL;
    }

    /* 等待初始化完成 */
    while (p_this->gnss_serv.init_stat == AWBL_GNSS_INIT_WAIT) {
        aw_mdelay(10);
    }

    ret = __ublox_enter_data_mode((awbl_dev_t *)p_cookie);
    if (ret != AW_OK) {
        ret = -AW_EBUSY;
    }

    aw_mdelay(1000);

    return ret;
}

aw_local  aw_err_t __ublox_end (void *p_cookie)
{
    __UBLOX_DEV_DECL(p_this, p_cookie);
    int ret;

    if (!p_cookie) {
        return -AW_EINVAL;
    }

    /* 等待初始化完成 */
    while (p_this->gnss_serv.init_stat == AWBL_GNSS_INIT_WAIT) {
        aw_mdelay(10);
    }

    ret =  __ublox_enter_cfg_mode((awbl_dev_t *)p_cookie);
    if (ret != AW_OK) {
        ret = -AW_EBUSY;
    }

    return ret;
}


aw_local aw_err_t __ublox_ioctl (void *p_cookie, aw_gnss_ioctl_req_t req, void *arg)
{
    __UBLOX_DEV_DECL(p_this, p_cookie);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_cookie);
    awbl_dev_t * p_dev = (awbl_dev_t *)p_cookie;
    int  ret = AW_OK;

    uint32_t msg_rate = 0;
    uint32_t nav_rate = 0;

    if (!p_cookie) {
        return -AW_EINVAL;
    }

    /* 等待初始化完成 */
    while (p_this->gnss_serv.init_stat == AWBL_GNSS_INIT_WAIT) {
        aw_mdelay(10);
    }

    __ublox_enter_cfg_mode(p_dev);

    switch (req) {

    case AW_GNSS_SYS_FREQ:
        __ublox_cfgsys(p_dev, (uint32_t)arg);
        p_this->gnss_serv.sys_freq = (uint32_t)arg;

        aw_mdelay(100);
        __ublox_inst(p_dev);    /* 配置系统频率需要复位，复位后重新初始化*/
        break;

    case AW_GNSS_NMEA_VER:
        __ublox_cfgnmea(p_dev, (uint32_t)arg);
        break;

    case AW_GNSS_DAT_OUT_RATE:
        __calculate_msg_nav_rate((uint32_t *)&arg, &msg_rate, &nav_rate);
        p_this->data_rate = (uint32_t)arg;

        __ublox_cfgmsg( p_dev,
                         p_this->cfg_par.msg_type,
                         p_this->cfg_par.msg_id,
                         msg_rate);

        __ublox_cfgnav( p_dev,
                         p_this->cfg_par.meas_rate,
                         nav_rate,
                         p_this->cfg_par.correction_mask);
        break;

    case AW_GNSS_SRESET:
        p_this->gnss_serv.dev_is_enable = AW_FALSE;

        __ublox_reset (p_dev, 0, (uint32_t)arg);
        aw_mdelay(100);
        __ublox_inst(p_dev);    /* 复位后重新初始化*/
        break;

    case AW_GNSS_HRESET:
        p_this->gnss_serv.dev_is_enable = AW_FALSE;

        if ((uint32_t)arg == AW_GNSS_COLD_STARTUP) {
            aw_gpio_set(p_devinfo->rst_pin, 1);
            aw_mdelay(30);
            aw_gpio_set(p_devinfo->rst_pin, 1);
        } else {
            __ublox_reset (p_dev, 1, (uint32_t)arg);
            aw_mdelay(30);
        }

        __ublox_inst(p_dev);    /* 复位后重新初始化*/

        break;

    default:
        ret = -AW_ENOTSUP;
    }

    if (__the_dev_is_start(p_this)) {
        __ublox_start(p_cookie);
    }

    return ret;
}


/******************************************************************************/
aw_local void __ublox_inst (awbl_dev_t *p_dev)
{
    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);
    int ret;

    if (!p_dev) {
        __ERR_MSG("p_dev == NULL");
    }

    p_this->gnss_serv.init_stat = AWBL_GNSS_INIT_FAILED;

    /* default configure */
    ret = aw_serial_ioctl(  p_devinfo->com.id,
                            SIO_BAUD_SET,
                            (void *)9600);           /* set default baud*/
    if (ret != AW_OK) {
        return;
    }

    ret = aw_serial_ioctl(p_devinfo->com.id,
                          AW_TIOCRDTIMEOUT,
                          (void *)p_devinfo->com.timout);
    if (ret != AW_OK) {
        return;
    }

    if (1) { /* p_devinfo->com.baud != 9600*/

        __ublox_enter_cfg_mode(p_dev);

        __ublox_cfgprt( p_dev,                        /* modify device baud*/
                         p_this->cfg_par.uart_num,
                         0,
                         p_devinfo->com.baud,
                         p_this->cfg_par.inProto,
                         p_this->cfg_par.outProto);

        __ublox_enter_data_mode(p_dev);

        aw_serial_ioctl(p_devinfo->com.id,
                        SIO_BAUD_SET,
                        (void *)p_devinfo->com.baud); /* modify mcu baud*/
    }

    __ublox_enter_cfg_mode(p_dev);

    if (p_this->gnss_serv.sys_freq) {
        p_this->cfg_par.sys_mask = p_this->gnss_serv.sys_freq;
    } else {
        p_this->gnss_serv.sys_freq = p_this->cfg_par.sys_mask;
    }

    p_this->gnss_serv.init_stat = AWBL_GNSS_INIT_OK;
}

aw_local void __ublox_inst_job_call (void *p_arg)
{
    __UBLOX_DEV_DECL(p_this, p_arg);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_arg);

    if (!p_arg) {
        __ERR_MSG("p_arg == NULL");
    }

    aw_gpio_pin_request("ublox", &p_devinfo->rst_pin, 1);
    aw_gpio_pin_request("ublox", &p_devinfo->en_pin, 1);

    aw_gpio_pin_cfg(p_devinfo->rst_pin, AW_GPIO_OUTPUT|AW_GPIO_OUTPUT_INIT_HIGH);

    aw_gpio_set(p_devinfo->rst_pin, 1);
    aw_mdelay(10);

    aw_gpio_pin_cfg(p_devinfo->en_pin, AW_GPIO_OUTPUT|AW_GPIO_OUTPUT_INIT_HIGH);
    aw_gpio_set(p_devinfo->en_pin, 0);
    aw_mdelay(10);

    __ublox_dev_set_dft_cfg_par(&p_this->cfg_par);

    p_this->gnss_serv.sys_freq  = 0;
    p_this->gnss_serv.valid_tim = 500;
    p_this->gnss_serv.begin_tim = 0;

    /* 初始化模块*/
    __ublox_inst(p_arg);
}

aw_local void __ublox_inst_connect(awbl_dev_t *p_dev)
{
    int ret;

    if (!p_dev) {
        __ERR_MSG("p_dev == NULL");
    }

    __UBLOX_DEV_DECL(p_this, p_dev);

    p_this->gnss_serv.dev_is_enable = AW_FALSE;

    /* 初始化一个工作节点*/
    aw_defer_job_init(&p_this->djob, __ublox_inst_job_call, (void *)p_dev);

    /* 将一个工作节点添加至系统中断延迟作业中处理*/
    ret = aw_sys_defer_commit(AW_SYS_DEFER_TYPE_NOR, &p_this->djob);
    if (ret != AW_OK) {
        __ERR_MSG("defer jod commit failed");
    }
}

/**
 * \brief method "__gnss_serv_get" handler
 */
aw_local aw_err_t __ublox_serv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __UBLOX_DEV_DECL(p_this, p_dev);
    __UBLOX_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_gnss_service *p_serv = NULL;

    if (!p_dev) {
        return -AW_EINVAL;
    }

    /* 各个设备的gnss服务函数 */
    p_serv = &p_this->gnss_serv;

    p_this->gnss_serv.init_stat = AWBL_GNSS_INIT_WAIT;

    /* 对服务函数进行初始化  */
    p_serv->p_next      = NULL;
    p_serv->p_com       = &p_devinfo->com;
    p_serv->p_servinfo  = &p_devinfo->gnss_servinfo;
    p_serv->p_servfunc  = &__g_ublox_servfunc;
    p_serv->p_cookie    = (void *)p_dev;

    *(struct awbl_gnss_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
void awbl_ublox_drv_register (void)
{
    int ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_ublox_drv_registration);
    if (ret != AW_OK) {
        return;
    }
}

/* end of file*/
