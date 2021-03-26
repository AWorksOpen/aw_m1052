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
 * \brief  NMEA-0183Э���������
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-10-15  vih, first implementation.
 * \endinternal
 */

#include "apollo.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
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
 * \brief ����NMEA�����ݵ�У��ͣ�$....*֮���ַ��������ֵ
 *
 * \param  p_start      ���ݵ���ʼ��ַ
 * \param  len          Ҫʹ�õ����ݳ���
 *
 * \return     ����У��ֵ
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
 * \brief ��ȡ�ַ�����������ŵ�ָ���������У�����������󣬻��Զ����������
 *
 * \param  p_src_dat    Դ����
 * \param  p_dst_buf    Ҫ��ŵ���Ŀ��buffer
 * \param  num          ������ţ�0~99��
 * \param  dst_max_size Ŀ��buffer�ĳ��ȣ�Ҫ����2��
 *
 * \retval  > 0             ���ػ�ȡ���ַ�������
 * \retval  -AW_EINVAL      ������Ч
 * \retval  -AW_EBADMSG     ���ݰ���Ч(���ݰ���","����û�дﵽָ��Ҫ��)
 * \retval  -AW_ENOMSG      û���ҵ���Ҫ�����ݣ�������λӦ����","��
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
    p_dst_buf[i] = 0;                              /* ���������0 */

    if (j == 0) {
        return -AW_ENOMSG;
    }

    return j;
}

/**
 * \brief GPRMC�������ݽ���   �������ܹ���12��','  
 *        ÿ��','�ź��һ��������Ҳ���Կղ���)
 *
 * \param[in]  p_this   nmea_msg�ṹ���ַ
 * \param[in]  buf      ���ݵ���ʼ��ַ
 *
 * \return
 */
aw_local aw_err_t __nmea_rmc_decode (struct awbl_nmea_0183_msg *p_this, 
                                     char *buf)
{
    int     len  = 0;
    char    tmp[50];
    uint8_t tmp_size = sizeof(tmp);
    char utc_time[11];          /**< \brief utcʱ�䣺hhmmss */
    char utc_date[7];           /**< \brief utc���ڣ�ddmmyy */

    /* ��2������Ϊ��λ״̬ A=��Ч��λ��V=��Ч��λ */
    len = __get_str_param(buf, tmp, 2, tmp_size);
    if (tmp[0] == 'A') {
        p_this->pos_valid = 1;
    } else {
        __ERR_WMSG("the data invalid", p_this->nmea_type);
        return -AW_EINVAL;
    }

    /* ��1������ΪUTCʱ�� */
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

    /* ��3������Ϊγ��ddmm.mmmm���ȷ֣���ʽ;  latitudu[0]�Ǵ��S��N�� */
    len = __get_str_param(buf, tmp, 3, tmp_size);
    p_this->latitude = strtof(tmp, 0);

    /* ��4������Ϊγ��S��N   �����latitudu[0]�� */
    len = __get_str_param(buf, tmp, 4, tmp_size);
    if (tmp[0] == 'S') {
        p_this->latitude = (-1) * p_this->latitude;
    }

    /* ��5������Ϊ����dddmm.mmmm���ȷ֣���ʽ ,longitude[0]�Ǵ��E��W�� */
    len = __get_str_param(buf, tmp, 5, tmp_size);
    p_this->longitude = strtof(tmp, 0);

    /* ��6������Ϊ����E��W   �����longitude[0] */
    len = __get_str_param(buf, tmp, 6, tmp_size);
    if (tmp[0] == 'W') {
        p_this->longitude = (-1) * p_this->longitude;
    }

    if (p_this->pos_valid) {
        __dm2dd_convert(&p_this->longitude, &p_this->latitude);
    }

    /* ��7������Ϊ�������� */
    len = __get_str_param(buf, tmp, 7, tmp_size);
    p_this->ground_rate = strtof(tmp, 0);

    /* ��8������Ϊ���溽�� */
    len = __get_str_param(buf, tmp, 8, tmp_size);
    p_this->ground_course = strtof(tmp, 0);

    /* ��9������ΪUTC���� */
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
 * \brief GPGSV�������ݽ��� (��������)
 *
 * \param[in]  p_this   nmea_msg�ṹ���ַ
 * \param[in]  buf      ���ݵ���ʼ��ַ
 *
 * \return
 */
aw_local aw_err_t __nmea_gsv_decode (struct awbl_nmea_0183_msg *p_this,
                                     char                      *buf)
{
    int      err         = 0;
    char     tmp[50];
    
    /* ��4������Ϊ���������� */
    err = __get_str_param(buf, tmp, 4, 50);
    if (err < 0) {
        return err;
    }
    p_this->view_satellites = strtol(tmp, 0, 10);
    
    return AW_OK;
}

/**
 * \brief GNGGA�������ݽ���
 *
 * \param[in]  p_this   nmea_msg�ṹ���ַ
 * \param[in]  buf      ���ݵ���ʼ��ַ
 *
 * \return
 */
aw_local aw_err_t __nmea_gga_decode (struct awbl_nmea_0183_msg *p_this, 
                                     char                      *buf)
{
    int      len         = 0;
    char     tmp[50];
    uint8_t  tmp_size = sizeof(tmp);
    char utc_time[11];          /**< \brief utcʱ�䣺hhmmss */
    char utc_date[7];           /**< \brief utc���ڣ�ddmmyy */

    (void)len;
    (void)utc_time;
    (void)utc_date;

    /* ��6������ΪGPS״̬ */
    len = __get_str_param(buf, tmp,   6, tmp_size);
    if (tmp[0] != '0') {
        p_this->gga_valid = 1;
    } else {
        p_this->gga_valid = 0;
        __ERR_WMSG("the data invalid", p_this->nmea_type);
        return -AW_EINVAL;
    }

    /* ��1������ΪUTCʱ��    hhmmss(ʱ����) */
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

    /* ��2������Ϊγ��ddmm.mmmm���ȷ֣���ʽ;  latitudu[0]�Ǵ��S��N�� */
    len = __get_str_param(buf, tmp, 2, tmp_size);
    p_this->latitude = strtof(tmp, 0);

    /* ��3������Ϊγ��S��N   �����latitudu[0] */
    len = __get_str_param(buf, tmp, 3, tmp_size);
    if (tmp[0] == 'S') {
        p_this->latitude = (-1) * p_this->latitude;
    }

    /* ��4������Ϊ����dddmm.mmmm���ȷ֣���ʽ ,longitude[0]�Ǵ��E��W�� */
    len = __get_str_param(buf, tmp, 4, tmp_size);
    p_this->longitude = strtof(tmp, 0);

    /* ��5������Ϊ����E��W   �����longitude[0] */
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

    /* ��1������Ϊ��λģʽָ��״̬ */
    err = __get_str_param(buf, tmp, 1, tmp_size);
    if (err < 0) {
        return err;
    }
    p_this->gsa_mode_stat = tmp[0];

    /* ��2������Ϊ��λģʽ */
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

    /* ��9������Ϊ��Ч��־ */
    err = __get_str_param(buf, tmp, 9, tmp_size);
    if (tmp[0] == 'N') {
        p_this->vtg_mode = 0;

        __ERR_WMSG("the data invalid", p_this->nmea_type);
        return -AW_EINVAL;
    } else {
        p_this->vtg_mode = tmp[0];
    }

    /* ��7������Ϊrate */
    err = __get_str_param(buf, tmp, 7, tmp_size);
    if (err > 0) {
        p_this->vtg_ground_rate = strtof(tmp, 0);
    }

    return AW_OK;
}
/******************************************************************************/

/**
 * \brief NMEA-0183Э�����ݽ���
 */
aw_err_t awbl_nmea_0183_decode (struct awbl_nmea_0183_msg *p_this, 
                                char                      *p_dat, 
                                char                      *p_nmea_type)
{
    aw_err_t err        = 0;
    char    *addr_start = 0;         /* Ҫ��������ݵ���ʼ��ַ */
    char    *addr_end   = 0;         /* Ҫ��������ݵ�ĩβ��ַ */
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

        /* ��start��end֮������Ƿ����������������� */
        p_tmp = strstr(addr_start + 1, "$");

        if (p_tmp > addr_end || !p_tmp) {
            break;
        }
    }

    if (addr_start == 0) {
        /* �����ʼ��ַΪ0  �򷵻ز����ڸ����͵����� */
        __ERR_WMSG("not find the data", p_nmea_type);
        return -AW_EINVAL;

    } else if (addr_end == 0) {
        __ERR_WMSG("not find end signal", p_nmea_type);
        return -AW_EBADMSG;
    }

    /* ��addr_start��λ��'$'����һ���ַ� */
    if (addr_start[0] == '$') {
        addr_start = addr_start + 1;
    }

    /* ������ݽṹ�壬��ֹ�в������� */
    memset(p_this, 0, sizeof(struct awbl_nmea_0183_msg));

    /* ����У��ֵ */
    p_this->chk_val = __nmea_data_xor(addr_start, (uint32_t)(addr_end - 
                                                             addr_start));

    /* �������ֵ����������addr_end[0]��'*',��������ֽڵ�У��ֵ��16���ƣ�*/
    if (p_this->chk_val != (char)strtol(addr_end + 1, 0, 16)) {
        p_this->chk_val = 0;

        __ERR_WMSG("verify falied, data is bad", p_nmea_type);
        return -AW_EBADMSG;
    }

    /* ��0����������ȡnmea������ */
    err = __get_str_param(addr_start, 
                          p_this->nmea_type, 
                          0,
                          sizeof(p_this->nmea_type));

    /* ��ʼ�������� */
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

        /* ��λ��־��Ч�������Ч */
        if ((p_src_msg->pos_valid ) && (p_src_msg->utc_time.year != 0)) {
            aw_tm_t   *p_tm = (aw_tm_t *)ptr;
            aw_time_t  time;

            p_tm->tm_sec    = p_src_msg->utc_time.sec;  /* ������ */
            p_tm->tm_min    = p_src_msg->utc_time.min;  /* ���÷� */
            p_tm->tm_hour   = p_src_msg->utc_time.hour; /* ����Сʱ */
            p_tm->tm_mday   = p_src_msg->utc_time.day;  /* ������ */
            
            /* �����£�ʵ���·�Ҫ��һ */
            p_tm->tm_mon    = p_src_msg->utc_time.mon-1;  
            
            /* �����꣬��1900������ */
            p_tm->tm_year   = p_src_msg->utc_time.year+100; 
            p_tm->tm_isdst  = -1;                       /* ����ʱ������ */

            return  aw_tm_to_time(p_tm, &time);
        }
    }

    __ERR_MSG("get data failed");
    return -AW_ENOMSG;
}

/* end of file */
