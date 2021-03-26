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

#include "apollo.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
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

/** \brief NMEA_0183���ݿ� */
struct awbl_nmea_0183_msg {

    /** \brief �������� ���һ��[]���0 */
    char                             nmea_type[6];

    struct awbl_nmea_0183_utc_time   utc_time;

    /** \brief ��λ״̬�Ƿ���Ч��1����Ч�� 0����Ч */
    aw_bool_t                        pos_valid;

    /** \brief GPS��λ״̬: 0=δ��λ�� 1=�ǲ�ֶ�λ�� 2=��ֶ�λ */
    char                             gps_status;

    /** \brief ����ֵΪ$...*֮�������ַ������ֵ��16���ƣ� */
    char                             chk_val;
    
    /** \brief ���������� */
    uint8_t                          view_satellites;

    /** \brief ��������γ�ȣ���γΪ�� */
    float                            latitude;

    /** \brief �������;��ȣ�����Ϊ�� */
    float                            longitude;

    /** \brief �������ʣ�000.0~999.9�ڣ�ǰ���0Ҳ�������䣩*/
    float                            ground_rate;

    /** \brief ���溽��000.0~359.9�ȣ����汱Ϊ�ο���׼��ǰ���0Ҳ�������䣩*/
    float                            ground_course;

    /** \brief gmc�Ĵ�ƫ�� */
    float                            rmc_angle;

    /** \brief gmc�Ĵ�ƫ�Ƿ��� */
    float                            rmc_angle_direct;

    /** \brief gga�ĺ��θ߶�, M */
    float                            gga_altitude;

    aw_bool_t                        gga_valid;

    /** \brief VTG�ĶԵ��ٶ�, Km/h*/
    float                            vtg_ground_rate;

    /** \brief VTG��ģʽ,Ϊ0��������Ч */
    char                             vtg_mode;

    /**
     * \brief GSA�Ķ�λģʽָ��״̬
     *
     * M - �ֶ�ָ�� 2D �� 3D ��λ
     * A - �Զ��л� 2D �� 3D ��λ
     */
    char                             gsa_mode_stat;

    /**
     * \brief GSA�Ķ�λģʽ
     *
     * 1 - δ��λ; 2 - 2D ��λ; 3 - 3D ��λ
     */
    uint32_t                         gsa_pos_mode;
};


/**
 * \brief NMEA-0183Э�����ݽ���
 *
 * \param  p_this   ���ڴ��nmea_msg�ṹ���ַ
 * \param  p_dat    Ҫ��������ݵ�ַ
 * \param  p_nmea_type    Э������(�ַ���)
 *           ���õĲ������£�
 *           @arg  "$GPRMC"��gps��λ��Ϣ
 *           @arg  "$GPGGA"
 *           @arg  "$GPGSA"��
 *           @arg  "$GPVTG"��
 *           @arg  "$GNGGA"��GPS/������λ��Ϣ�����֡��72
 *           @arg  "$GNGSA"����ǰ������Ϣ�����֡��65
 *           @arg  "$GNGSV"���ɼ�GPS������Ϣ�����֡��210
 *           @arg  "$BDGSV"���ɼ�����������Ϣ�����֡��210
 *           @arg  "$GNRMC"���Ƽ���λ��Ϣ�����֡��70
 *           @arg  "$GNVTG"�������ٶ���Ϣ�����֡��34
 *           @arg  "$GNGLL"�����������Ϣ
 *           @arg  "$GNZDA"����ǰʱ�䣨UTC����Ϣ������ʱ���UTC��8��Сʱ
 *
 * \return ��������
 */
aw_err_t awbl_nmea_0183_decode (struct awbl_nmea_0183_msg *p_this,
                                char                      *p_dat, 
                                char                      *p_nmea_type);

/**
 * \brief gps���ݽ��룬��ʱ�����Ѿ���ȡ��p_this���msgbuf����
 *
 * \param      p_msg           ��Ž������ݵ�msg
 * \param      nmea_type       ��Ҫ�����NMEA����
 * \param      p_sta           ������ɹ������Ӧ�ı����1
 *
 * \return     aw_err_t        ��һ�ּ�һ�����Ͻ���ɹ��򷵻�OK�����򷵻�error
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

