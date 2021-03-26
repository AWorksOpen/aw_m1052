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
 * \brief GNSS(global navigation satellite system)接口
 *
 * \code

    #include "aw_gnss.h"
    #include "aw_vdebug.h"

    aw_local aw_err_t  __test_gnss_func(void)
    {
        aw_err_t  ret = AW_OK;

        float    a[2];
        char     buf[50];

        // 获取定位数据
        ret = aw_gnss_dat_get(0, AW_GNSS_RMC_POSITION, a, 8);
        if (ret != AW_OK) {
            AW_ERRF(("aw_gnss_get_pos err: %d\n",  ret));
            return  ret;
        } else {
            AW_INFOF(("gps local: %f, %f\n", a[0], a[1]));
        }

        // 获取速度
        ret = aw_gnss_dat_get(0, AW_GNSS_VTG_RATE, a, 4);
        if (ret != AW_OK) {
            AW_ERRF(("get vtg rate failed: %d\n",  ret));
        } else {
            AW_INFOF(("vtg rate: %f\n", a[0]));
        }

        // 获取海拔
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

        // 复位 GPS 模块
        ret = aw_gnss_ioctl(dev_id, AW_GNSS_SRESET, (void *)AW_GNSS_HOT_STARTUP);
        if (ret != AW_OK) {
            AW_ERRF(("aw_gnss_ioctl AW_GNSS_HOT_STARTUP err: %d\n",  ret));
            return;
        }

        // 启动 GPS 模块
        ret = aw_gnss_enable(dev_id);
        if (ret != AW_OK) {
            AW_ERRF(("aw_gnss_start err: %d\n",  ret));
            return;
        }

        // 设置双模输出
        ret = aw_gnss_ioctl(dev_id, AW_GNSS_SYS_FREQ, (void *)(AW_GNSS_BDS_B1 | AW_GNSS_GPS_L1));
        if (ret != AW_OK) {
            AW_ERRF(("aw_gnss_ioctl err: %d\n",  ret));
            return;
        }

        // 设置解码类型为RMC  VTG 和 GGA
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

#define AW_GNSS_GPS_L1                  (1)         /**< \brief 频段1575.42MHZ */
#define AW_GNSS_GPS_L2                  (1<<1)      /**< \brief 频段1227.6MHZ */
#define AW_GNSS_GPS_L5                  (1<<2)      /**< \brief 频段1176.45MHZ */

#define AW_GNSS_BDS_B1                  (1<<4)      /**< \brief 频段1561.098MHZ */
#define AW_GNSS_BDS_B2                  (1<<5)      /**< \brief 频段1207.14MHZ */
#define AW_GNSS_BDS_B3                  (1<<6)      /**< \brief 频段1268.52MHZ */

#define AW_GNSS_HOT_STARTUP             0x00        /**< \brief 热启动 */
#define AW_GNSS_WARM_STARTUP            0x01        /**< \brief 常规启动 */
#define AW_GNSS_COLD_STARTUP            0x95        /**< \brief 冷启动 */

#define AW_GNSS_NMEA_RMC               (0x01)       /**< \brief 解码类型为RMC */
#define AW_GNSS_NMEA_VTG               (0x01 << 1)  /**< \brief 解码类型为VTG */
#define AW_GNSS_NMEA_GGA               (0x01 << 2)  /**< \brief 解码类型为GGA */

/**
 * \brief ioctl的选项
 */
typedef enum aw_gnss_ioctl_req {
    AW_GNSS_SYS_FREQ = 0,           /**< \brief 系统的频段 */
    AW_GNSS_NMEA_VER,               /**< \brief nmea0183 协议版本 */
    AW_GNSS_DAT_OUT_RATE,           /**< \brief gnss device数据的输出速率(ms) */
    AW_GNSS_SRESET,                 /**< \brief 软件复位  */
    AW_GNSS_HRESET,                 /**< \brief 硬件复位  */
    AW_GNSS_NMEA_SELECT,            /**< \brief NMEA 解码的类型  */
    AW_GNSS_MAX_VALID_TIM,          /**< \brief 数据最大的有效时间(ms)  */
    /**
     * \brief 查询设备状态
     *        为AW_OK则设备就绪，否则设备繁忙
     */
    AW_GNSS_DEV_STATUS              
} aw_gnss_ioctl_req_t;

/**
 * \brief ioctl的选项
 */
typedef enum aw_gnss_data_type {

    /** \brief 获取的数据类型: VTG的速度 ， Km/h */
    AW_GNSS_VTG_RATE            = ((AW_GNSS_NMEA_VTG << 16) + 1),

    /** \brief 获取的数据类型: GGA的海拔高度, 单位M   */
    AW_GNSS_GGA_ALTITUDE        = ((AW_GNSS_NMEA_GGA << 16) + 1),

    /** \brief 获取的数据类型: RMC的定位   */
    AW_GNSS_RMC_POSITION        = ((AW_GNSS_NMEA_RMC << 16) + 1),

    /** \brief 获取的数据类型: RMC的UTC时间   */
    AW_GNSS_RMC_UTC_TIME        = ((AW_GNSS_NMEA_RMC << 16) + 2),

    /** \brief 获取的数据类型: RMC的地磁偏角   */
    AW_GNSS_RMC_ANGLE           = ((AW_GNSS_NMEA_RMC << 16) + 3),

    /** \brief 获取的数据类型: RMC的磁偏角方向   */
    AW_GNSS_RMC_ANGLE_DIRECT    = ((AW_GNSS_NMEA_RMC << 16) + 4),

    /** \brief 获取的数据类型: RMC的地面航向   */
    AW_GNSS_RMC_COURSE          = ((AW_GNSS_NMEA_RMC << 16) + 5),

    /** \brief 获取的数据类型: RMC数据的有效性   */
    AW_GNSS_RMC_VALID           = ((AW_GNSS_NMEA_RMC << 16) + 6)
} aw_gnss_data_type_t;


/**
 * \brief      gnss启动函数
 *
 * \param      id        设备id
 *
 * \return     aw_err_t 错误类型
 */
aw_err_t aw_gnss_enable (int id);

/**
 * \brief      gnss关闭函数
 *
 * \param      id        设备id
 *
 * \return     aw_err_t 错误类型
 */
aw_err_t aw_gnss_disable   (int id);

/**
 * \brief      gnss device control
 *
 * \param      id        设备id
 * \param      req       iotcl命令请求
 *              可用的请求及参数如下：
 *              @arg AW_GNSS_SYS_FREQ，选择系统的类型
 *                   AW_GNSS_GPS_L1
 *                   AW_GNSS_GPS_L2
 *                   AW_GNSS_BDS_B1
 *                   AW_GNSS_BDS_B2
 *                   AW_GNSS_BDS_B3
 *              @arg AW_GNSS_NMEA_VER，选择输出的nmea协议版本
 *                   0x30 - 在 NMEA 标准 version 3.0 基础上扩展北斗相关的语句
 *                   0x40 - 卫星定位总站定义的扩展 NMEA
 *                   0x41 - 《车载终端用北斗导航模块数据接口规范》中规定的NMEA 格式
 *                   0x42 - 输出双模和北斗gsa *
 *              @arg AW_GNSS_DAT_OUT_RATE，数据输出速率，单位ms
 *              @arg AW_GNSS_SRESET，软件复位
 *                   GNSS_HOT_STARTUP - 热启动
 *                   GNSS_CLOD_STARTUP - 冷启动
 *                   GNSS_WARM_STARTUP - 温启动
 *              @arg AW_GNSS_HRESET，硬件复位
 *                   GNSS_HOT_STARTUP
 *                   GNSS_COLD_STARTUP
 *                   GNSS_WARM_STARTUP
 *              @arg AW_GNSS_NMEA_SELECT, 设置解码类型
 *              @arg AW_GNSS_MAX_VALID_TIM, 设置数据的有效时间ms
 *              @arg AW_GNSS_DEV_STATUS, 查询设备状态
 *
 * \param      arg       ioctl命令请求参数，参数类型和取值取决于对应的请求命令
 *
 * \par 范例
 * \code
 *
 *
 * aw_gnss_ioctl(0, AW_GNSS_DAT_OUT_RATE, (void *)500); // 设置数据输出速率，单位ms
 * aw_gnss_ioctl(0, AW_GNSS_NMEA_VER, (void *)0x30);    // 选择输出的nmea协议版本
 * aw_gnss_ioctl(0, AW_GNSS_SRESET, (void *)GNSS_HOT_STARTUP);    // 软件热启动复位
 * aw_gnss_ioctl(0, AW_GNSS_MAX_VALID_TIM, (void *)500);    // 有效数据的保持500ms
 *
 * // 设置设备的频率为gps和北斗双模
 * aw_gnss_ioctl(0, AW_GNSS_SYS_FREQ, (void *)(AW_GNSS_GPS_L1 | AW_GNSS_BDS_B1));
 *
 * // 设置解码类型为RMC  VTG 和 GGA
 * aw_gnss_ioctl(0,
 *               AW_GNSS_NMEA_SELECT,
 *               (void *)(AW_GNSS_NMEA_RMC | AW_GNSS_NMEA_VTG | AW_GNSS_NMEA_GGA));
 *
 * //查询设备状态
 * aw_err_t dev_status;
 * aw_gnss_ioctl(0, AW_GNSS_DEV_STATUS, (void *)(&dev_status));
 * if (dev_status == AW_OK) {
 *     // 状态为AW_OK则设备就绪，否则设备繁忙
 * }
 *
 * \endcode
 *
 * \return     aw_err_t 错误类型
 */
aw_err_t aw_gnss_ioctl (int id, aw_gnss_ioctl_req_t req, void *arg);


/**
 * \brief      获取数据
 *
 * \param      id        设备id
 * \param      type      需要获取的数据类型
 * \param      ptr       数据的存放buf
 * \param      len       buf的长度
 *
 * \retval    AW_OK         操作成功
 * \retval    -AW_EBUSY     设备繁忙
 * \retval    -AW_EINVAL    参数无效
 * \retval    -AW_ENOMEM    参数 ptr 的长度不足以接收该数据
 * \retval    -AW_ENOMSG    没有获取到有效数据
 * \retval    -AW_ETIME     接收数据超时
 * \retval    -AW_EBADMSG   接收到的数据是不完整的
 * \retval    -AW_ENOTSUP   不支持获取这种类型的数据，
 *                          请确认是否已通过ioctl命令开启接收该类型的数据
 *
 * \par 范例
 * \code
 *
 * int ret;
 * float    a[2];
 * float    lon ,lat， rat, alt;
 * struct awbl_nmea_0183_utc_time  utc_time;
 *
 * ret = aw_gnss_dat_get(0, AW_GNSS_RMC_POSITION, a, 8);  //获取定位数据
 *
 * if (ret == AW_OK) {
 *     lon = a[0];  // 经度
 *     lat = a[1];  // 纬度
 * }
 *
 * ret = aw_gnss_dat_get(0, AW_GNSS_VTG_RATE, a, 4);  //获取速度
 *
 * if (ret == AW_OK) {
 *     rate = a[0];
 * }
 *
 * //获取RMC类型的UTC时间
 * ret = aw_gnss_dat_get(0, AW_GNSS_RMC_UTC_TIME, &utc_time, sizeof(utc_time));
 * if (ret == AW_OK) {
 *     rate = a[0];
 * }
 *
 * //获取海拔高度
 * ret = aw_gnss_dat_get(0, AW_GNSS_GGA_ALTITUDE, a, 4);
 * if (ret == AW_OK) {
 *     alt = a[0];
 * }
 *
 * // 获取地磁偏角
 * ret = aw_gnss_dat_get(0, AW_GNSS_RMC_ANGLE, a, 4);
 * if (ret != AW_OK) {
 *     AW_ERRF(("get angle failed: %d\n",  ret));
 * } else {
 *     AW_INFOF(("angle: %f\n", a[0]));
 * }
 *
 * // 获取RMC的磁偏角方向
 * ret = aw_gnss_dat_get(0, AW_GNSS_RMC_ANGLE_DIRECT, a, 4);
 * if (ret != AW_OK) {
 *     AW_ERRF(("get angle direct failed: %d\n",  ret));
 * } else {
 *     AW_INFOF(("angle direct: %f\n", a[0]));
 * }
 *
 * // RMC的地面航向
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

