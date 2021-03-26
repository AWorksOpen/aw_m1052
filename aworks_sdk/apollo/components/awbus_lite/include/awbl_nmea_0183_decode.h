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
 * \internal
 * \par modification history:
 * - 1.00 16-10-15  vih, first implementation.
 * \endinternal
 */

#ifndef __AWBL_NMEA_0183_DECODE_H
#define __AWBL_NMEA_0183_DECODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apollo.h"                     /* 此头文件必须被首先包含 */
#include "string.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "aw_gnss.h"


struct awbl_nmea_0183_utc_time {
    float sec;
    uint_t min;
    uint_t hour;

    uint_t day;
    uint_t mon;
    uint_t year;
};

/** \brief NMEA_0183数据块 */
struct awbl_nmea_0183_msg {

    /** \brief 数据类型 最后一个[]存放0 */
    char                             nmea_type[6];

    struct awbl_nmea_0183_utc_time   utc_time;

    /** \brief 定位状态是否有效，1：有效； 0：无效 */
    aw_bool_t                        pos_valid;

    /** \brief GPS定位状态: 0=未定位， 1=非差分定位， 2=差分定位 */
    char                             gps_status;

    /** \brief 检验值为$...*之间所有字符的异或值（16进制） */
    char                             chk_val;
    
    /** \brief 可视卫星数 */
    uint8_t                          view_satellites;

    /** \brief 单精度型纬度，南纬为负 */
    float                            latitude;

    /** \brief 单精度型经度，西经为负 */
    float                            longitude;

    /** \brief 地面速率（000.0~999.9节，前面的0也将被传输）*/
    float                            ground_rate;

    /** \brief 地面航向（000.0~359.9度，以真北为参考基准，前面的0也将被传输）*/
    float                            ground_course;

    /** \brief gmc的磁偏角 */
    float                            rmc_angle;

    /** \brief gmc的磁偏角方向 */
    float                            rmc_angle_direct;

    /** \brief gga的海拔高度, M */
    float                            gga_altitude;

    aw_bool_t                        gga_valid;

    /** \brief VTG的对地速度, Km/h*/
    float                            vtg_ground_rate;

    /** \brief VTG的模式,为0则数据无效 */
    char                             vtg_mode;

    /**
     * \brief GSA的定位模式指定状态
     *
     * M - 手动指定 2D 或 3D 定位
     * A - 自动切换 2D 或 3D 定位
     */
    char                             gsa_mode_stat;

    /**
     * \brief GSA的定位模式
     *
     * 1 - 未定位; 2 - 2D 定位; 3 - 3D 定位
     */
    uint32_t                         gsa_pos_mode;
};


/**
 * \brief NMEA-0183协议数据解码
 *
 * \param  p_this   用于存放nmea_msg结构体地址
 * \param  p_dat    要解码的数据地址
 * \param  p_nmea_type    协议类型(字符串)
 *           可用的参数如下：
 *           @arg  "$GPRMC"，gps定位信息
 *           @arg  "$GPGGA"
 *           @arg  "$GPGSA"，
 *           @arg  "$GPVTG"，
 *           @arg  "$GNGGA"，GPS/北斗定位信息，最大帧长72
 *           @arg  "$GNGSA"，当前卫星信息，最大帧长65
 *           @arg  "$GNGSV"，可见GPS卫星信息，最大帧长210
 *           @arg  "$BDGSV"，可见北斗卫星信息，最大帧长210
 *           @arg  "$GNRMC"，推荐定位信息，最大帧长70
 *           @arg  "$GNVTG"，地面速度信息，最大帧长34
 *           @arg  "$GNGLL"，大地坐标信息
 *           @arg  "$GNZDA"，当前时间（UTC）信息，北京时间比UTC早8个小时
 *
 * \return 错误类型
 */
aw_err_t awbl_nmea_0183_decode (struct awbl_nmea_0183_msg *p_this,
                                char                      *p_dat, 
                                char                      *p_nmea_type);

/**
 * \brief gps数据解码，此时数据已经读取到p_this里的msgbuf里了
 *
 * \param      p_msg           存放解码数据的msg
 * \param      nmea_type       需要解码的NMEA类型
 * \param      p_sta           将解码成功后的相应的标记设1
 *
 * \return     aw_err_t        有一种及一种以上解码成功则返回OK，无则返回error
 */
aw_err_t awbl_nmea_0183_dat_decode (struct awbl_nmea_0183_msg       *p_to_msg,
                                     uint8_t                        *src_buf,
                                     uint32_t                        sys_freq,
                                     uint32_t                        flag,
                                     uint32_t                       *p_sta);

aw_err_t awbl_nmea_0183_get_dat (struct awbl_nmea_0183_msg       *p_src_msg,
                                 aw_gnss_data_type_t              type,
                                 void                            *ptr,
                                 uint8_t                          len);

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_NMEA_0183_DECODE_H */

/* end of file */

