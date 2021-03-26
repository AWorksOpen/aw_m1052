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
 * \brief ATGM332D-5N模块中GPS的驱动 和 NMEA-0183协议解码驱动
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 20-09-03  vih, first implementation.
 * \endinternal
 */

#include "apollo.h"                     /* 此头文件必须被首先包含 */
#include "driver/gps/awbl_atgm332d.h"
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
            __kprintf("\nATGM332D MSG:\"%s\".\n"              \
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
#define __ATGM332D_DEV_DECL(p_this, p_dev) \
    struct awbl_atgm332d_dev *p_this = \
    (struct awbl_atgm332d_dev *)AW_CONTAINER_OF(p_dev, struct awbl_atgm332d_dev, super)


#define __ATGM332D_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_atgm332d_devinfo *p_devinfo = \
        (struct awbl_atgm332d_devinfo *)AWBL_DEVINFO_GET(p_dev)



/******************************************************************************/
aw_local aw_err_t __atgm332d_serv_get (struct awbl_dev *p_dev, void *p_arg);


aw_local void __atgm332d_inst (awbl_dev_t *p_dev);
aw_local void __atgm332d_inst_connect(awbl_dev_t *p_dev);

aw_local   aw_err_t __atgm332d_ioctl (void *p_cookie,
                                   aw_gnss_ioctl_req_t req, 
                                   void *arg);

aw_local   aw_err_t __atgm332d_end (void *p_cookie);
aw_local   aw_err_t __atgm332d_start (void *p_cookie);

/******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_atgm332d_drvfuncs = {
    NULL,
    NULL,
    __atgm332d_inst_connect
};

/* atgm332d service functions (must defined as aw_const) */
aw_local aw_const struct awbl_gnss_servfunc __g_atgm332d_servfunc = {
    __atgm332d_start,
    __atgm332d_end,
    __atgm332d_ioctl,
    NULL,
};


/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_gnss_serv_get);

aw_local aw_const struct awbl_dev_method __g_atgm332d_dev_methods[] = {
    AWBL_METHOD(awbl_gnss_serv_get, __atgm332d_serv_get),
    AWBL_METHOD_END
};

aw_local aw_const awbl_plb_drvinfo_t __g_atgm332d_drv_registration = {
    {
        AWBL_VER_1,                        /* awb_ver */
        AWBL_BUSID_PLB,                    /* bus_id */
        AWBL_ATGM332D_GNSS,                   /* p_drvname */
        &__g_atgm332d_drvfuncs,               /* p_busfuncs */
        &__g_atgm332d_dev_methods[0],         /* p_methods */
        NULL                               /* pfunc_drv_probe */
    }
};

/******************************************************************************/


/* set default configure parameter */
aw_local void __atgm332d_dev_set_dft_cfg_par (struct awbl_atgm332d_dev_cfg_par *p_cfg_par)
{
    p_cfg_par->baud     = 9600;
    p_cfg_par->reserved = 0;

    p_cfg_par->sys_mask = AW_GNSS_GPS_L1 ;  //0x11;
}


aw_local aw_inline int __the_dev_is_start (struct awbl_atgm332d_dev  *p_this)
{
    return p_this->gnss_serv.dev_is_enable = AW_TRUE;
}

/**
 * \brief 计算NMEA的数据的校验和，$....*之间字符串的异或值
 *
 * \param  p_start      数据的起始地址
 * \param  len          要使用的数据长度
 *
 * \return     返回校验值
 */
aw_local char __atgm332d_data_xor (char *p_start, uint32_t len)
{
    int i = 0;
    char temp = 0;

    temp = p_start[i++];
    while (i < len) {
        temp = temp ^ p_start[i++];
    }
    return temp;
}

aw_local aw_err_t __atgm332d_cfgprt (awbl_dev_t *p_dev,uint32_t baud)
{
    __ATGM332D_DEVINFO_DECL(p_devinfo, p_dev);
    __ATGM332D_DEV_DECL(p_this, p_dev);
    int ret;
    int len;
    char mask;
    uint8_t *buf;

    if (p_devinfo == 0) {
        return -AW_EINVAL;
    }

    sprintf(buf,"PCAS01,%d",baud);
    len = strlen(buf);

    mask = __atgm332d_data_xor (buf, len);

    snprintf(p_devinfo->com.buf,
         p_devinfo->com.size,
         "$PCAS01,%d*%x\r",
         baud,
         mask);

    p_this->cfg_par.baud = baud;

    ret = awbl_gnss_send_wait_res(&p_this->gnss_serv, p_devinfo->com.buf, 0, 0);

    return ret;
}

aw_local aw_err_t __atgm332d_cfgrate (awbl_dev_t *p_dev,uint32_t rate)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    int ret;
    int len;
    char mask;
    uint8_t *buf;
    __ATGM332D_DEV_DECL(p_this, p_dev);
    __ATGM332D_DEVINFO_DECL(p_devinfo, p_dev);

    sprintf(buf,"PCAS02,%d",rate);
    len = strlen(buf);

    mask = __atgm332d_data_xor (buf, len);

    snprintf(p_devinfo->com.buf,
             p_devinfo->com.size, 
             "$PCAS02,%d*%x\r",
             rate,
             mask);

    ret = awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                   p_devinfo->com.buf, 
                                   "OK", 
                                   2000);
    if (ret != AW_OK) {
        return ret;
    }

    return AW_OK;
}

aw_local aw_err_t __atgm332d_cfgsys (awbl_dev_t         *p_dev,
                                  uint32_t            sys_mask)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __ATGM332D_DEV_DECL(p_this, p_dev);
    __ATGM332D_DEVINFO_DECL(p_devinfo, p_dev);

    if (sys_mask == AW_GNSS_BDS_B1) {
        snprintf(p_devinfo->com.buf,
                 p_devinfo->com.size,
                 "$PCAS04,2*1B\r");
    } else if (sys_mask == AW_GNSS_GPS_L1) {
        snprintf(p_devinfo->com.buf,
                 p_devinfo->com.size,
                 "$PCAS04,1*18\r");
    } else {
    	return -AW_ENOTSUP;
    }

    p_this->cfg_par.sys_mask = sys_mask;

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}

aw_local aw_err_t __atgm332d_cfgsave (awbl_dev_t             *p_dev)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __ATGM332D_DEV_DECL(p_this, p_dev);
    __ATGM332D_DEVINFO_DECL(p_devinfo, p_dev);

    snprintf(p_devinfo->com.buf,
             p_devinfo->com.size,
             "$PCAS00*01\r");

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}

aw_local aw_err_t __atgm332d_reset (awbl_dev_t             *p_dev,
                                 uint32_t               type,
                                 uint32_t               clr_mask)
{
    if (p_dev == 0) {
        return -AW_EINVAL;
    }

    __ATGM332D_DEV_DECL(p_this, p_dev);
    __ATGM332D_DEVINFO_DECL(p_devinfo, p_dev);

    snprintf(p_devinfo->com.buf,
             p_devinfo->com.size,
             "$PCAS10,%d*%x\r", type, clr_mask);

    return awbl_gnss_send_wait_res(&p_this->gnss_serv, 
                                    p_devinfo->com.buf, 
                                    "OK", 
                                    2000);
}


/*******************************************************************************
  service functions 
*******************************************************************************/
aw_local  aw_err_t __atgm332d_start (void *p_cookie)
{
    __ATGM332D_DEV_DECL(p_this, p_cookie);
    __ATGM332D_DEVINFO_DECL(p_devinfo, p_cookie);
    int ret;
    uint8_t cnt = 0;

    if (!p_cookie) {
        return -AW_EINVAL;
    }

    ret = AW_OK;

    return ret;
}

aw_local  aw_err_t __atgm332d_end (void *p_cookie)
{
    __ATGM332D_DEV_DECL(p_this, p_cookie);
    int ret;

    if (!p_cookie) {
        return -AW_EINVAL;
    }

    /* 等待初始化完成 */
    while (p_this->gnss_serv.init_stat == AWBL_GNSS_INIT_WAIT) {
        aw_mdelay(10);
    }

    return ret;
}


aw_local aw_err_t __atgm332d_ioctl (void *p_cookie, aw_gnss_ioctl_req_t req, void *arg)
{
    __ATGM332D_DEV_DECL(p_this, p_cookie);
    __ATGM332D_DEVINFO_DECL(p_devinfo, p_cookie);
    awbl_dev_t * p_dev = (awbl_dev_t *)p_cookie;
    int  ret = AW_OK;

    uint32_t msg_rate = 0;
    uint32_t nav_rate = 0;

    if (!p_cookie) {
        return -AW_EINVAL;
    }

    switch (req) {

    case AW_GNSS_SYS_FREQ:
        __atgm332d_cfgsys(p_dev, (uint32_t)arg);
        p_this->gnss_serv.sys_freq = (uint32_t)arg;

        aw_mdelay(100);
        break;

    case AW_GNSS_NMEA_VER:
        ret = -AW_ENOTSUP;
        break;

    case AW_GNSS_DAT_OUT_RATE:
        p_this->data_rate = (uint32_t)arg;

        __atgm332d_cfgrate( p_dev,p_this->data_rate);
        break;

    case AW_GNSS_SRESET:
        p_this->gnss_serv.dev_is_enable = AW_FALSE;

        if ((uint32_t)arg == AW_GNSS_COLD_STARTUP) {
            __atgm332d_reset (p_dev, 2, 0x1E);
            aw_mdelay(30);
        } else if ((uint32_t)arg == AW_GNSS_HOT_STARTUP) {
            __atgm332d_reset (p_dev, 0, 0x1C);
            aw_mdelay(30);
        } else {
            ret = -AW_ENOTSUP;
        }

        __atgm332d_inst(p_dev);    /* 复位后重新初始化*/
        break;

    case AW_GNSS_HRESET:
        ret = -AW_ENOTSUP;
        break;

    default:
        ret = -AW_ENOTSUP;
    }

    if (__the_dev_is_start(p_this)) {
        __atgm332d_start(p_cookie);
    }

    return ret;
}


/******************************************************************************/
aw_local void __atgm332d_inst (awbl_dev_t *p_dev)
{
    __ATGM332D_DEV_DECL(p_this, p_dev);
    __ATGM332D_DEVINFO_DECL(p_devinfo, p_dev);
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

//    __atgm332d_cfgprt( p_dev,                        /* modify device baud*/
//                     p_devinfo->com.baud);

    if (p_this->gnss_serv.sys_freq) {
        p_this->cfg_par.sys_mask = p_this->gnss_serv.sys_freq;
    } else {
        p_this->gnss_serv.sys_freq = p_this->cfg_par.sys_mask;
    }

    p_this->gnss_serv.init_stat = AWBL_GNSS_INIT_OK;
}

aw_local void __atgm332d_inst_job_call (void *p_arg)
{
    __ATGM332D_DEV_DECL(p_this, p_arg);
    __ATGM332D_DEVINFO_DECL(p_devinfo, p_arg);

    if (!p_arg) {
        __ERR_MSG("p_arg == NULL");
    }

    __atgm332d_dev_set_dft_cfg_par(&p_this->cfg_par);

    p_this->gnss_serv.sys_freq  = 0;
    p_this->gnss_serv.valid_tim = 500;
    p_this->gnss_serv.begin_tim = 0;

    /* 初始化模块*/
    __atgm332d_inst(p_arg);
}

aw_local void __atgm332d_inst_connect(awbl_dev_t *p_dev)
{
    int ret;

    if (!p_dev) {
        __ERR_MSG("p_dev == NULL");
    }

    __ATGM332D_DEV_DECL(p_this, p_dev);

    p_this->gnss_serv.dev_is_enable = AW_FALSE;

    /* 初始化一个工作节点*/
    aw_defer_job_init(&p_this->djob, __atgm332d_inst_job_call, (void *)p_dev);

    /* 将一个工作节点添加至系统中断延迟作业中处理*/
    ret = aw_sys_defer_commit(AW_SYS_DEFER_TYPE_NOR, &p_this->djob);
    if (ret != AW_OK) {
        __ERR_MSG("defer jod commit failed");
    }
}

/**
 * \brief method "__gnss_serv_get" handler
 */
aw_local aw_err_t __atgm332d_serv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __ATGM332D_DEV_DECL(p_this, p_dev);
    __ATGM332D_DEVINFO_DECL(p_devinfo, p_dev);

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
    p_serv->p_servfunc  = &__g_atgm332d_servfunc;
    p_serv->p_cookie    = (void *)p_dev;

    *(struct awbl_gnss_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
void awbl_atgm332d_drv_register (void)
{
    int ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_atgm332d_drv_registration);
    if (ret != AW_OK) {
        return;
    }
}

/* end of file*/
