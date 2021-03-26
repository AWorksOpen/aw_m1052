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
 * \brief  NMEA-0183协议解码驱动
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-10-15  vih, first implementation.
 * \endinternal
 */

#include "apollo.h"                     /* 此头文件必须被首先包含 */
#include "awbl_nmea_0183_decode.h"
#include "aw_time.h"
#include "aw_vdebug.h"
#include <string.h>

/******************************************************************************/
//#define __DEBUG_INFO_PRINTF
/******************************************************************************/
#ifndef __DEBUG_INFO_PRINTF
#define __kprintf
#define __ERR_MSG(str)
#define __ERR_WMSG(str, who)

#else
#define __kprintf aw_kprintf

#define __ERR_FORMAT(str, who)                                                 \
            __kprintf( "\nNMEA MSG:\"%s\"(%s).\n"                              \
                         "-----MSG: at file %s.\n"                             \
                         "-----MSG: at line %d in %s()\n\n",                   \
                        str,                                                   \
                        who,                                                   \
                        __FILE__,                                              \
                        __LINE__,                                              \
                        __FUNCTION__);

#define __ERR_MSG(str)       __ERR_FORMAT(str, NULL)
#define __ERR_WMSG(str, who) __ERR_FORMAT(str, who)

#endif

/******************************************************************************/
struct __nmea_0183_decode_info {
    char *p_type;
    int (*pfn_decode) (struct awbl_nmea_0183_msg *p_this, char *buf);
};
/******************************************************************************/
aw_local aw_err_t __nmea_rmc_decode (struct awbl_nmea_0183_msg *p_this, 
                                     char *buf);
aw_local aw_err_t __nmea_gsv_decode (struct awbl_nmea_0183_msg *p_this, 
                                     char *buf);
aw_local aw_err_t __nmea_gga_decode (struct awbl_nmea_0183_msg *p_this, 
                                     char *buf);
aw_local aw_err_t __nmea_vtg_decode (struct awbl_nmea_0183_msg *p_this, 
                                     char *buf);
aw_local aw_err_t __nmea_gsa_decode (struct awbl_nmea_0183_msg *p_this, 
                                     char *buf);

/******************************************************************************/
aw_local struct __nmea_0183_decode_info __g_nmea_decode_lib[] = {
    {"RMC",  __nmea_rmc_decode},
    {"GGA",  __nmea_gga_decode},
    {"VTG",  __nmea_vtg_decode},
    {"GSV",  __nmea_gsv_decode},
    {"GSA",  __nmea_gsa_decode},
};

aw_local uint32_t __g_nmea_decode_lib_num = AW_NELEMENTS(__g_nmea_decode_lib);
/******************************************************************************/
aw_local aw_err_t __dm2dd_convert (float *to_lon,   float *to_lat)
{
    if (to_lon == 0 || to_lat == 0) {
        __ERR_MSG("parameter invalid");
        return -AW_EINVAL;
    }

    int    n = 0;
    float tmp = 0;

    float  from_lon = *to_lon;
    float  from_lat = *to_lat;

    from_lon = from_lon / 100;
    from_lat = from_lat / 100;

    n = (int)from_lon;

    tmp = from_lon - n;
    tmp = tmp / 60;

    *to_lon = n + tmp * 100 ;

    n = (int)from_lat;

    tmp = from_lat - n;
    tmp = tmp / 60;

    *to_lat = n + tmp * 100 ;

    return AW_OK;
}

/**
 * \brief 计算NMEA的数据的校验和，$....*之间字符串的异或值
 *
 * \param  p_start      数据的起始地址
 * \param  len          要使用的数据长度
 *
 * \return     返回校验值
 */
aw_local char __nmea_data_xor (char *p_start, uint32_t len)
{
    int i = 0;
    char temp = 0;

    temp = p_start[i++];
    while (i < len) {
        temp = temp ^ p_start[i++];
    }
    return temp;
}

/**
 * \brief 获取字符串参数，存放到指定的数组中，接收完参数后，会自动加入结束符
 *
 * \param  p_src_dat    源数据
 * \param  p_dst_buf    要存放到的目的buffer
 * \param  num          参数序号（0~99）
 * \param  dst_max_size 目的buffer的长度（要大于2）
 *
 * \retval  > 0             返回获取的字符串长度
 * \retval  -AW_EINVAL      参数无效
 * \retval  -AW_EBADMSG     数据包无效(数据包的","数量没有达到指定要求)
 * \retval  -AW_ENOMSG      没有找到需要的数据（该数据位应该是","）
 */
aw_local int __get_str_param (char    *p_src_dat, 
                              char    *p_dst_buf, 
                              uint32_t num, 
                              uint32_t dst_max_size)
{
    int i = 0;
    int j = 0;

    if (dst_max_size < 2) {
        __ERR_MSG("buf not enough");
        return -AW_EINVAL;
    }

    if (NULL == p_src_dat) {
        return  -AW_EINVAL;
    }

    while (i < num) {
        if ((p_src_dat = strstr(p_src_dat, ",")) != 0) {
            i++;
            p_src_dat++;
        } else {
            return -AW_EBADMSG;
        }
    }

    for (i = 0, j = 0; j < dst_max_size - 1; j++, i++) {
        if (p_src_dat[i] == ',') {
            break;
        }
        p_dst_buf[j] = p_src_dat[i];
    }
    p_dst_buf[i] = 0;                              /* 加入结束符0 */

    if (j == 0) {
        return -AW_ENOMSG;
    }

    return j;
}

/**
 * \brief GPRMC类型数据解码   该类型总共有12个','  
 *        每个','号后跟一个参数（也可以空参数)
 *
 * \param[in]  p_this   nmea_msg结构体地址
 * \param[in]  buf      数据的起始地址
 *
 * \return
 */
aw_local aw_err_t __nmea_rmc_decode (struct awbl_nmea_0183_msg *p_this, 
                                     char *buf)
{
    int     len  = 0;
    char    tmp[50];
    uint8_t tmp_size = sizeof(tmp);
    char utc_time[11];          /**< \brief utc时间：hhmmss */
    char utc_date[7];           /**< \brief utc日期：ddmmyy */

    /* 第2个参数为定位状态 A=有效定位，V=无效定位 */
    len = __get_str_param(buf, tmp, 2, tmp_size);
    if (tmp[0] == 'A') {
        p_this->pos_valid = 1;
    } else {
        __ERR_WMSG("the data invalid", p_this->nmea_type);
        return -AW_EINVAL;
    }

    /* 第1个参数为UTC时间 */
    len = __get_str_param(buf, utc_time, 1, sizeof(utc_time));
    if (len >= 6) {
        p_this->utc_time.hour = (utc_time[0] - '0') * 10 + (utc_time[1] - '0');
        p_this->utc_time.min  = (utc_time[2] - '0') * 10 + (utc_time[3] - '0');
        p_this->utc_time.sec  = strtof(utc_time + 4, 0);
    } else {
        p_this->utc_time.hour = 0;
        p_this->utc_time.min  = 0;
        p_this->utc_time.sec  = 0;
    }

    /* 第3个参数为纬度ddmm.mmmm（度分）格式;  latitudu[0]是存放S或N的 */
    len = __get_str_param(buf, tmp, 3, tmp_size);
    p_this->latitude = strtof(tmp, 0);

    /* 第4个参数为纬度S或N   存放于latitudu[0]、 */
    len = __get_str_param(buf, tmp, 4, tmp_size);
    if (tmp[0] == 'S') {
        p_this->latitude = (-1) * p_this->latitude;
    }

    /* 第5个参数为经度dddmm.mmmm（度分）格式 ,longitude[0]是存放E或W的 */
    len = __get_str_param(buf, tmp, 5, tmp_size);
    p_this->longitude = strtof(tmp, 0);

    /* 第6个参数为经度E或W   存放于longitude[0] */
    len = __get_str_param(buf, tmp, 6, tmp_size);
    if (tmp[0] == 'W') {
        p_this->longitude = (-1) * p_this->longitude;
    }

    if (p_this->pos_valid) {
        __dm2dd_convert(&p_this->longitude, &p_this->latitude);
    }

    /* 第7个参数为地面速率 */
    len = __get_str_param(buf, tmp, 7, tmp_size);
    p_this->ground_rate = strtof(tmp, 0);

    /* 第8个参数为地面航向 */
    len = __get_str_param(buf, tmp, 8, tmp_size);
    p_this->ground_course = strtof(tmp, 0);

    /* 第9个参数为UTC日期 */
    len = __get_str_param(buf, utc_date, 9, sizeof(utc_date));
    if (len == 6) {
        p_this->utc_time.day   = (utc_date[0] - '0') * 10 + (utc_date[1] - '0');
        p_this->utc_time.mon   = (utc_date[2] - '0') * 10 + (utc_date[3] - '0');
        p_this->utc_time.year  = (utc_date[4] - '0') * 10 + (utc_date[5] - '0');
    } else {
        p_this->utc_time.day   = 0;
        p_this->utc_time.mon   = 0;
        p_this->utc_time.year  = 0;
    }

    len = __get_str_param(buf, tmp, 10, tmp_size);
    p_this->rmc_angle = strtof(tmp, 0);

    len = __get_str_param(buf, tmp, 11, tmp_size);
    p_this->rmc_angle_direct = tmp[0];

    return AW_OK;
}

/**
 * \brief GPGSV类型数据解码 (可视卫星)
 *
 * \param[in]  p_this   nmea_msg结构体地址
 * \param[in]  buf      数据的起始地址
 *
 * \return
 */
aw_local aw_err_t __nmea_gsv_decode (struct awbl_nmea_0183_msg *p_this,
                                     char                      *buf)
{
    int      err         = 0;
    char     tmp[50];
    
    /* 第4个参数为可视卫星数 */
    err = __get_str_param(buf, tmp, 4, 50);
    if (err < 0) {
        return err;
    }
    p_this->view_satellites = strtol(tmp, 0, 10);
    
    return AW_OK;
}

/**
 * \brief GNGGA类型数据解码
 *
 * \param[in]  p_this   nmea_msg结构体地址
 * \param[in]  buf      数据的起始地址
 *
 * \return
 */
aw_local aw_err_t __nmea_gga_decode (struct awbl_nmea_0183_msg *p_this, 
                                     char                      *buf)
{
    int      len         = 0;
    char     tmp[50];
    uint8_t  tmp_size = sizeof(tmp);
    char utc_time[11];          /**< \brief utc时间：hhmmss */
    char utc_date[7];           /**< \brief utc日期：ddmmyy */

    (void)len;
    (void)utc_time;
    (void)utc_date;

    /* 第6个参数为GPS状态 */
    len = __get_str_param(buf, tmp,   6, tmp_size);
    if (tmp[0] != '0') {
        p_this->gga_valid = 1;
    } else {
        p_this->gga_valid = 0;
        __ERR_WMSG("the data invalid", p_this->nmea_type);
        return -AW_EINVAL;
    }

    /* 第1个参数为UTC时间    hhmmss(时分秒) */
//    len = __get_str_param(buf, utc_time, 1, sizeof(utc_time));
//    if (len == 6) {
//        p_this->utc_time.hour = (utc_time[0] - '0') * 10 + (utc_time[1] - '0');
//        p_this->utc_time.min  = (utc_time[2] - '0') * 10 + (utc_time[3] - '0');
//        p_this->utc_time.sec  = strtof(utc_time + 4, 0);
//    } else {
//        p_this->utc_time.hour = 0;
//        p_this->utc_time.min  = 0;
//        p_this->utc_time.sec  = 0;
//    }

    /* 第2个参数为纬度ddmm.mmmm（度分）格式;  latitudu[0]是存放S或N的 */
    len = __get_str_param(buf, tmp, 2, tmp_size);
    p_this->latitude = strtof(tmp, 0);

    /* 第3个参数为纬度S或N   存放于latitudu[0] */
    len = __get_str_param(buf, tmp, 3, tmp_size);
    if (tmp[0] == 'S') {
        p_this->latitude = (-1) * p_this->latitude;
    }

    /* 第4个参数为经度dddmm.mmmm（度分）格式 ,longitude[0]是存放E或W的 */
    len = __get_str_param(buf, tmp, 4, tmp_size);
    p_this->longitude = strtof(tmp, 0);

    /* 第5个参数为经度E或W   存放于longitude[0] */
    len = __get_str_param(buf, tmp, 5, tmp_size);
    if (tmp[0] == 'W') {
        p_this->longitude = (-1) * p_this->longitude;
    }

    len = __get_str_param(buf, tmp,  9, tmp_size);
    p_this->gga_altitude = strtof(tmp, 0);

    return AW_OK;
}

aw_local aw_err_t __nmea_gsa_decode (struct awbl_nmea_0183_msg *p_this, 
                                     char                      *buf)
{
    int      err         = 0;
    char     tmp[50];
    uint8_t  tmp_size = sizeof(tmp);

    /* 第1个参数为定位模式指定状态 */
    err = __get_str_param(buf, tmp, 1, tmp_size);
    if (err < 0) {
        return err;
    }
    p_this->gsa_mode_stat = tmp[0];

    /* 第2个参数为定位模式 */
    err = __get_str_param(buf, tmp, 2, tmp_size);
    if (err < 0) {
        return err;
    }
    p_this->gsa_pos_mode = strtol(tmp, 0, 0);

    return AW_OK;
}

aw_local aw_err_t __nmea_vtg_decode (struct awbl_nmea_0183_msg *p_this, 
                                     char                      *buf)
{
    int      err         = 0;
    char     tmp[50];
    uint8_t  tmp_size = sizeof(tmp);

    /* 第9个参数为有效标志 */
    err = __get_str_param(buf, tmp, 9, tmp_size);
    if (tmp[0] == 'N') {
        p_this->vtg_mode = 0;

        __ERR_WMSG("the data invalid", p_this->nmea_type);
        return -AW_EINVAL;
    } else {
        p_this->vtg_mode = tmp[0];
    }

    /* 第7个参数为rate */
    err = __get_str_param(buf, tmp, 7, tmp_size);
    if (err > 0) {
        p_this->vtg_ground_rate = strtof(tmp, 0);
    }

    return AW_OK;
}
/******************************************************************************/

/**
 * \brief NMEA-0183协议数据解码
 */
aw_err_t awbl_nmea_0183_decode (struct awbl_nmea_0183_msg *p_this, 
                                char                      *p_dat, 
                                char                      *p_nmea_type)
{
    aw_err_t err        = 0;
    char    *addr_start = 0;         /* 要解码的数据的起始地址 */
    char    *addr_end   = 0;         /* 要解码的数据的末尾地址 */
    char    *p_tmp      = 0;
    uint32_t i          = 0;

    if (p_this == 0 || p_dat == 0 || p_nmea_type == 0) {
        return -AW_EINVAL;
    }

    p_tmp = p_dat;

    while (1) {
        addr_start = strstr(p_tmp, p_nmea_type);
        if (NULL == addr_start) {
            break;
        }
        addr_end   = strstr(addr_start, "*");

        /* 在start和end之间查找是否还有其他的数据类型 */
        p_tmp = strstr(addr_start + 1, "$");

        if (p_tmp > addr_end || !p_tmp) {
            break;
        }
    }

    if (addr_start == 0) {
        /* 如果起始地址为0  则返回不存在该类型的数据 */
        __ERR_WMSG("not find the data", p_nmea_type);
        return -AW_EINVAL;

    } else if (addr_end == 0) {
        __ERR_WMSG("not find end signal", p_nmea_type);
        return -AW_EBADMSG;
    }

    /* 将addr_start定位到'$'的下一个字符 */
    if (addr_start[0] == '$') {
        addr_start = addr_start + 1;
    }

    /* 清空数据结构体，防止有残留数据 */
    memset(p_this, 0, sizeof(struct awbl_nmea_0183_msg));

    /* 计算校验值 */
    p_this->chk_val = __nmea_data_xor(addr_start, (uint32_t)(addr_end - 
                                                             addr_start));

    /* 如果检验值不相等则出错，addr_end[0]是'*',后跟两个字节的校验值（16进制）*/
    if (p_this->chk_val != (char)strtol(addr_end + 1, 0, 16)) {
        p_this->chk_val = 0;

        __ERR_WMSG("verify falied, data is bad", p_nmea_type);
        return -AW_EBADMSG;
    }

    /* 第0个参数，提取nmea的类型 */
    err = __get_str_param(addr_start, 
                          p_this->nmea_type, 
                          0,
                          sizeof(p_this->nmea_type));

    /* 开始解析数据 */
    for (i = 0; i < __g_nmea_decode_lib_num; i++) {
        if (strstr(p_this->nmea_type, __g_nmea_decode_lib[i].p_type) != 0) {
            if (__g_nmea_decode_lib[i].pfn_decode) {
                err = __g_nmea_decode_lib[i].pfn_decode(p_this, addr_start);
            }

            break;
        }
        if (i == __g_nmea_decode_lib_num - 1) {
            __ERR_WMSG("not support the nmea type decode", p_nmea_type);
            return -AW_ENOTSUP;
        }
    }

    return err;
}

aw_err_t awbl_nmea_0183_dat_decode (struct awbl_nmea_0183_msg      *p_to_msg,
                                    uint8_t                        *src_buf,
                                    uint32_t                        sys_freq,
                                    uint32_t                        nmea_type,
                                    uint32_t                       *p_sta)
{
    if (p_to_msg == 0) {
        return -AW_EINVAL;
    }

    int ret = AW_ERROR;
    int err = AW_ERROR;
    char type_buf[6];

    struct awbl_nmea_0183_msg msg;

    if (sys_freq == (AW_GNSS_GPS_L1 | AW_GNSS_BDS_B1)) {
        strcpy(type_buf, "GN");
    } else if (sys_freq == AW_GNSS_BDS_B1) {
        strcpy(type_buf, "BD");
    } else  {                  /* default  (sys_freq == AW_GNSS_GPS_L1) */
        strcpy(type_buf, "GP");
    }

    *p_sta = 0;

    if (nmea_type & AW_GNSS_NMEA_RMC) {
        sprintf(type_buf + 2, "RMC");
        ret = awbl_nmea_0183_decode(&msg, (char *)src_buf, type_buf);

        if (ret == AW_OK) {
            *p_sta |= AW_GNSS_NMEA_RMC;

            p_to_msg->pos_valid     = msg.pos_valid;
            p_to_msg->longitude     = msg.longitude;
            p_to_msg->latitude      = msg.latitude;
            p_to_msg->ground_course = msg.ground_course;
            p_to_msg->rmc_angle     = msg.rmc_angle;
            p_to_msg->utc_time      = msg.utc_time;

            err = AW_OK;
        }
    }

    if (nmea_type & AW_GNSS_NMEA_VTG) {
        sprintf(type_buf + 2, "VTG");
        ret = awbl_nmea_0183_decode(&msg, (char *)src_buf, type_buf);

        if (ret == AW_OK) {
            *p_sta |= AW_GNSS_NMEA_VTG;

            p_to_msg->vtg_mode        = msg.vtg_mode;
            p_to_msg->vtg_ground_rate = msg.vtg_ground_rate;

            err = AW_OK;
        }
    }

    if (nmea_type & AW_GNSS_NMEA_GGA) {
        sprintf(type_buf + 2, "GGA");
        ret = awbl_nmea_0183_decode(&msg, (char *)src_buf, type_buf);

        if (ret == AW_OK) {
            *p_sta |= AW_GNSS_NMEA_GGA;

            p_to_msg->gga_altitude = msg.gga_altitude;

            err = AW_OK;
        }
    }

    return err;
}

aw_err_t awbl_nmea_0183_get_dat (struct awbl_nmea_0183_msg     *p_src_msg,
                                 aw_gnss_data_type_t            type,
                                 void                          *ptr,
                                 uint8_t                        len)
{
    if (p_src_msg == NULL) {
        return -AW_EINVAL;
    }

    if (AW_GNSS_VTG_RATE == type) {
        if (len < sizeof(float)) {
            __ERR_MSG("data buffer not enough");
            return -AW_ENOMEM;
        }

        if (p_src_msg->vtg_mode) {
            *((float *)ptr) = p_src_msg->vtg_ground_rate;
            return AW_OK;
        }

    } else if (AW_GNSS_RMC_POSITION == type) {
        if (len < (2 * sizeof(float))) {
            __ERR_MSG("data buffer not enough");
            return -AW_ENOMEM;
        }

        if (p_src_msg->pos_valid) {
            memcpy(ptr, &p_src_msg->longitude, 4);
            memcpy((void *)((char *)ptr + 4), &p_src_msg->latitude, 4);
            return AW_OK;
        }

    } else if (AW_GNSS_GGA_ALTITUDE == type) {
        if (len < (sizeof(float))) {
            __ERR_MSG("data buffer not enough");
            return -AW_ENOMEM;
        }

        if (p_src_msg->gga_altitude) {
            *((float *)ptr) = p_src_msg->gga_altitude;
            return AW_OK;
        }

    } else if (AW_GNSS_RMC_COURSE == type) {
        if (len < (sizeof(float))) {
            __ERR_MSG("data buffer not enough");
            return -AW_ENOMEM;
        }

        if (p_src_msg->ground_course) {
            *((float *)ptr) = p_src_msg->ground_course;
            return AW_OK;
        }

    } else if (AW_GNSS_RMC_ANGLE == type) {
        if (len < (sizeof(float))) {
           __ERR_MSG("data buffer not enough");
            return -AW_ENOMEM;
        }

        if (p_src_msg->rmc_angle) {
            *((float *)ptr) = p_src_msg->rmc_angle;
            return AW_OK;
        }

    } else if (AW_GNSS_RMC_VALID == type) {
        if (len < (sizeof(aw_bool_t))) {
           __ERR_MSG("data buffer not enough");
            return -AW_ENOMEM;
        }

        if (p_src_msg->pos_valid) {
            *((aw_bool_t *)ptr) = p_src_msg->pos_valid;
            return AW_OK;
        }

    } else if (AW_GNSS_RMC_UTC_TIME == type) {
        if (len < 16) {
           __ERR_MSG("data buffer not enough");
            return -AW_ENOMEM;
        }

        /* 定位标志有效且年份有效 */
        if ((p_src_msg->pos_valid ) && (p_src_msg->utc_time.year != 0)) {
            aw_tm_t   *p_tm = (aw_tm_t *)ptr;
            aw_time_t  time;

            p_tm->tm_sec    = p_src_msg->utc_time.sec;  /* 设置秒 */
            p_tm->tm_min    = p_src_msg->utc_time.min;  /* 设置分 */
            p_tm->tm_hour   = p_src_msg->utc_time.hour; /* 设置小时 */
            p_tm->tm_mday   = p_src_msg->utc_time.day;  /* 设置日 */
            
            /* 设置月，实际月份要减一 */
            p_tm->tm_mon    = p_src_msg->utc_time.mon-1;  
            
            /* 设置年，距1900的年数 */
            p_tm->tm_year   = p_src_msg->utc_time.year+100; 
            p_tm->tm_isdst  = -1;                       /* 夏令时不可用 */

            return  aw_tm_to_time(p_tm, &time);
        }
    }

    __ERR_MSG("get data failed");
    return -AW_ENOMSG;
}

/* end of file */
