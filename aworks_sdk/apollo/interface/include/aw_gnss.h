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
 * \brief GNSS(global navigation satellite system)�ӿ�
 *
 * \code

    #include "aw_gnss.h"
    #include "aw_vdebug.h"

    aw_local aw_err_t  __test_gnss_func(void)
    {
        aw_err_t  ret = AW_OK;

        float    a[2];
        char     buf[50];

        // ��ȡ��λ����
        ret = aw_gnss_dat_get(0, AW_GNSS_RMC_POSITION, a, 8);
        if (ret != AW_OK) {
            AW_ERRF(("aw_gnss_get_pos err: %d\n",  ret));
            return  ret;
        } else {
            AW_INFOF(("gps local: %f, %f\n", a[0], a[1]));
        }

        // ��ȡ�ٶ�
        ret = aw_gnss_dat_get(0, AW_GNSS_VTG_RATE, a, 4);
        if (ret != AW_OK) {
            AW_ERRF(("get vtg rate failed: %d\n",  ret));
        } else {
            AW_INFOF(("vtg rate: %f\n", a[0]));
        }

        // ��ȡ����
        ret = aw_gnss_dat_get(0, AW_GNSS_GGA_ALTITUDE, a, 4);
        if (ret != AW_OK) {
            AW_ERRF(("get gga altitude failed: %d\n",  ret));
        } else {
            AW_INFOF(("altitude: %f\n", a[0]));
        }

        return ret;
    }

    void  test_gnss(void)
    {
        aw_err_t  ret = AW_OK;

        int  dev_id = 0;

        // ��λ GPS ģ��
        ret = aw_gnss_ioctl(dev_id, AW_GNSS_SRESET, (void *)AW_GNSS_HOT_STARTUP);
        if (ret != AW_OK) {
            AW_ERRF(("aw_gnss_ioctl AW_GNSS_HOT_STARTUP err: %d\n",  ret));
            return;
        }

        // ���� GPS ģ��
        ret = aw_gnss_enable(dev_id);
        if (ret != AW_OK) {
            AW_ERRF(("aw_gnss_start err: %d\n",  ret));
            return;
        }

        // ����˫ģ���
        ret = aw_gnss_ioctl(dev_id, AW_GNSS_SYS_FREQ, (void *)(AW_GNSS_BDS_B1 | AW_GNSS_GPS_L1));
        if (ret != AW_OK) {
            AW_ERRF(("aw_gnss_ioctl err: %d\n",  ret));
            return;
        }

        // ���ý�������ΪRMC  VTG �� GGA
        ret = aw_gnss_ioctl(dev_id,
                            AW_GNSS_NMEA_SELECT,
                            (void *)(AW_GNSS_NMEA_RMC | AW_GNSS_NMEA_VTG | AW_GNSS_NMEA_GGA));
        if (ret != AW_OK) {
            AW_ERRF(("aw_gnss_ioctl AW_GNSS_NMEA_SELECT err: %d\n",  ret));
            return;
        }

        __test_gnss_func();
    }

 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-29  vih, first implementation.
 * \endinternal
 */

#ifndef __AW_GNSS_H
#define __AW_GNSS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_gnss
 * \copydoc aw_gnss.h
 * @{
 */

#include "aworks.h"

#define AW_GNSS_GPS_L1                  (1)         /**< \brief Ƶ��1575.42MHZ */
#define AW_GNSS_GPS_L2                  (1<<1)      /**< \brief Ƶ��1227.6MHZ */
#define AW_GNSS_GPS_L5                  (1<<2)      /**< \brief Ƶ��1176.45MHZ */

#define AW_GNSS_BDS_B1                  (1<<4)      /**< \brief Ƶ��1561.098MHZ */
#define AW_GNSS_BDS_B2                  (1<<5)      /**< \brief Ƶ��1207.14MHZ */
#define AW_GNSS_BDS_B3                  (1<<6)      /**< \brief Ƶ��1268.52MHZ */

#define AW_GNSS_HOT_STARTUP             0x00        /**< \brief ������ */
#define AW_GNSS_WARM_STARTUP            0x01        /**< \brief �������� */
#define AW_GNSS_COLD_STARTUP            0x95        /**< \brief ������ */

#define AW_GNSS_NMEA_RMC               (0x01)       /**< \brief ��������ΪRMC */
#define AW_GNSS_NMEA_VTG               (0x01 << 1)  /**< \brief ��������ΪVTG */
#define AW_GNSS_NMEA_GGA               (0x01 << 2)  /**< \brief ��������ΪGGA */

/**
 * \brief ioctl��ѡ��
 */
typedef enum aw_gnss_ioctl_req {
    AW_GNSS_SYS_FREQ = 0,           /**< \brief ϵͳ��Ƶ�� */
    AW_GNSS_NMEA_VER,               /**< \brief nmea0183 Э��汾 */
    AW_GNSS_DAT_OUT_RATE,           /**< \brief gnss device���ݵ��������(ms) */
    AW_GNSS_SRESET,                 /**< \brief �����λ  */
    AW_GNSS_HRESET,                 /**< \brief Ӳ����λ  */
    AW_GNSS_NMEA_SELECT,            /**< \brief NMEA ���������  */
    AW_GNSS_MAX_VALID_TIM,          /**< \brief ����������Чʱ��(ms)  */
    /**
     * \brief ��ѯ�豸״̬
     *        ΪAW_OK���豸�����������豸��æ
     */
    AW_GNSS_DEV_STATUS              
} aw_gnss_ioctl_req_t;

/**
 * \brief ioctl��ѡ��
 */
typedef enum aw_gnss_data_type {

    /** \brief ��ȡ����������: VTG���ٶ� �� Km/h */
    AW_GNSS_VTG_RATE            = ((AW_GNSS_NMEA_VTG << 16) + 1),

    /** \brief ��ȡ����������: GGA�ĺ��θ߶�, ��λM   */
    AW_GNSS_GGA_ALTITUDE        = ((AW_GNSS_NMEA_GGA << 16) + 1),

    /** \brief ��ȡ����������: RMC�Ķ�λ   */
    AW_GNSS_RMC_POSITION        = ((AW_GNSS_NMEA_RMC << 16) + 1),

    /** \brief ��ȡ����������: RMC��UTCʱ��   */
    AW_GNSS_RMC_UTC_TIME        = ((AW_GNSS_NMEA_RMC << 16) + 2),

    /** \brief ��ȡ����������: RMC�ĵش�ƫ��   */
    AW_GNSS_RMC_ANGLE           = ((AW_GNSS_NMEA_RMC << 16) + 3),

    /** \brief ��ȡ����������: RMC�Ĵ�ƫ�Ƿ���   */
    AW_GNSS_RMC_ANGLE_DIRECT    = ((AW_GNSS_NMEA_RMC << 16) + 4),

    /** \brief ��ȡ����������: RMC�ĵ��溽��   */
    AW_GNSS_RMC_COURSE          = ((AW_GNSS_NMEA_RMC << 16) + 5),

    /** \brief ��ȡ����������: RMC���ݵ���Ч��   */
    AW_GNSS_RMC_VALID           = ((AW_GNSS_NMEA_RMC << 16) + 6)
} aw_gnss_data_type_t;


/**
 * \brief      gnss��������
 *
 * \param      id        �豸id
 *
 * \return     aw_err_t ��������
 */
aw_err_t aw_gnss_enable (int id);

/**
 * \brief      gnss�رպ���
 *
 * \param      id        �豸id
 *
 * \return     aw_err_t ��������
 */
aw_err_t aw_gnss_disable   (int id);

/**
 * \brief      gnss device control
 *
 * \param      id        �豸id
 * \param      req       iotcl��������
 *              ���õ����󼰲������£�
 *              @arg AW_GNSS_SYS_FREQ��ѡ��ϵͳ������
 *                   AW_GNSS_GPS_L1
 *                   AW_GNSS_GPS_L2
 *                   AW_GNSS_BDS_B1
 *                   AW_GNSS_BDS_B2
 *                   AW_GNSS_BDS_B3
 *              @arg AW_GNSS_NMEA_VER��ѡ�������nmeaЭ��汾
 *                   0x30 - �� NMEA ��׼ version 3.0 ��������չ������ص����
 *                   0x40 - ���Ƕ�λ��վ�������չ NMEA
 *                   0x41 - �������ն��ñ�������ģ�����ݽӿڹ淶���й涨��NMEA ��ʽ
 *                   0x42 - ���˫ģ�ͱ���gsa *
 *              @arg AW_GNSS_DAT_OUT_RATE������������ʣ���λms
 *              @arg AW_GNSS_SRESET�������λ
 *                   GNSS_HOT_STARTUP - ������
 *                   GNSS_CLOD_STARTUP - ������
 *                   GNSS_WARM_STARTUP - ������
 *              @arg AW_GNSS_HRESET��Ӳ����λ
 *                   GNSS_HOT_STARTUP
 *                   GNSS_COLD_STARTUP
 *                   GNSS_WARM_STARTUP
 *              @arg AW_GNSS_NMEA_SELECT, ���ý�������
 *              @arg AW_GNSS_MAX_VALID_TIM, �������ݵ���Чʱ��ms
 *              @arg AW_GNSS_DEV_STATUS, ��ѯ�豸״̬
 *
 * \param      arg       ioctl��������������������ͺ�ȡֵȡ���ڶ�Ӧ����������
 *
 * \par ����
 * \code
 *
 *
 * aw_gnss_ioctl(0, AW_GNSS_DAT_OUT_RATE, (void *)500); // ��������������ʣ���λms
 * aw_gnss_ioctl(0, AW_GNSS_NMEA_VER, (void *)0x30);    // ѡ�������nmeaЭ��汾
 * aw_gnss_ioctl(0, AW_GNSS_SRESET, (void *)GNSS_HOT_STARTUP);    // �����������λ
 * aw_gnss_ioctl(0, AW_GNSS_MAX_VALID_TIM, (void *)500);    // ��Ч���ݵı���500ms
 *
 * // �����豸��Ƶ��Ϊgps�ͱ���˫ģ
 * aw_gnss_ioctl(0, AW_GNSS_SYS_FREQ, (void *)(AW_GNSS_GPS_L1 | AW_GNSS_BDS_B1));
 *
 * // ���ý�������ΪRMC  VTG �� GGA
 * aw_gnss_ioctl(0,
 *               AW_GNSS_NMEA_SELECT,
 *               (void *)(AW_GNSS_NMEA_RMC | AW_GNSS_NMEA_VTG | AW_GNSS_NMEA_GGA));
 *
 * //��ѯ�豸״̬
 * aw_err_t dev_status;
 * aw_gnss_ioctl(0, AW_GNSS_DEV_STATUS, (void *)(&dev_status));
 * if (dev_status == AW_OK) {
 *     // ״̬ΪAW_OK���豸�����������豸��æ
 * }
 *
 * \endcode
 *
 * \return     aw_err_t ��������
 */
aw_err_t aw_gnss_ioctl (int id, aw_gnss_ioctl_req_t req, void *arg);


/**
 * \brief      ��ȡ����
 *
 * \param      id        �豸id
 * \param      type      ��Ҫ��ȡ����������
 * \param      ptr       ���ݵĴ��buf
 * \param      len       buf�ĳ���
 *
 * \retval    AW_OK         �����ɹ�
 * \retval    -AW_EBUSY     �豸��æ
 * \retval    -AW_EINVAL    ������Ч
 * \retval    -AW_ENOMEM    ���� ptr �ĳ��Ȳ����Խ��ո�����
 * \retval    -AW_ENOMSG    û�л�ȡ����Ч����
 * \retval    -AW_ETIME     �������ݳ�ʱ
 * \retval    -AW_EBADMSG   ���յ��������ǲ�������
 * \retval    -AW_ENOTSUP   ��֧�ֻ�ȡ�������͵����ݣ�
 *                          ��ȷ���Ƿ���ͨ��ioctl��������ո����͵�����
 *
 * \par ����
 * \code
 *
 * int ret;
 * float    a[2];
 * float    lon ,lat�� rat, alt;
 * struct awbl_nmea_0183_utc_time  utc_time;
 *
 * ret = aw_gnss_dat_get(0, AW_GNSS_RMC_POSITION, a, 8);  //��ȡ��λ����
 *
 * if (ret == AW_OK) {
 *     lon = a[0];  // ����
 *     lat = a[1];  // γ��
 * }
 *
 * ret = aw_gnss_dat_get(0, AW_GNSS_VTG_RATE, a, 4);  //��ȡ�ٶ�
 *
 * if (ret == AW_OK) {
 *     rate = a[0];
 * }
 *
 * //��ȡRMC���͵�UTCʱ��
 * ret = aw_gnss_dat_get(0, AW_GNSS_RMC_UTC_TIME, &utc_time, sizeof(utc_time));
 * if (ret == AW_OK) {
 *     rate = a[0];
 * }
 *
 * //��ȡ���θ߶�
 * ret = aw_gnss_dat_get(0, AW_GNSS_GGA_ALTITUDE, a, 4);
 * if (ret == AW_OK) {
 *     alt = a[0];
 * }
 *
 * // ��ȡ�ش�ƫ��
 * ret = aw_gnss_dat_get(0, AW_GNSS_RMC_ANGLE, a, 4);
 * if (ret != AW_OK) {
 *     AW_ERRF(("get angle failed: %d\n",  ret));
 * } else {
 *     AW_INFOF(("angle: %f\n", a[0]));
 * }
 *
 * // ��ȡRMC�Ĵ�ƫ�Ƿ���
 * ret = aw_gnss_dat_get(0, AW_GNSS_RMC_ANGLE_DIRECT, a, 4);
 * if (ret != AW_OK) {
 *     AW_ERRF(("get angle direct failed: %d\n",  ret));
 * } else {
 *     AW_INFOF(("angle direct: %f\n", a[0]));
 * }
 *
 * // RMC�ĵ��溽��
 * ret = aw_gnss_dat_get(0, AW_GNSS_RMC_COURSE, a, 4);
 * if (ret != AW_OK) {
 *     AW_ERRF(("get course failed: %d\n",  ret));
 * } else {
 *     AW_INFOF(("course: %f\n", a[0]));
 * }
 *
 * \endcode
 *
 */
aw_err_t aw_gnss_dat_get   (int                    id,
                            aw_gnss_data_type_t    type,
                            void                  *ptr,
                            uint8_t                len);

/** @}  grp_aw_if_gnss */

#ifdef __cplusplus
}
#endif

#endif  /* __AW_GNSS_H */

/* end of file */

