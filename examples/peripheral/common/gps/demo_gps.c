/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief GPS例程
 *
 * - 操作步骤：
 *   1. 需要在 aw_prj_params.h 头文件里使能GPS设备，即：
 *    - AW_DEV_GNSS_CNT226 或  AW_DEV_GNSS_UBLOX (具体的见模块型号)
 *    - 对应平台的串口设备宏 (cnt226或ublox使用的串口设备)
 *    - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 例程配置完GPS后，连续读取GPS信息，并打印至串口中
 *
 * - 备注：
 *   1. 速率：GPS设备在静止状态速率为0，返回无效参数，获取速率会失败；设备移动起来会获取到相应速率。
 *   2. 平台支持的GPS设备目前有cnt226，ublox,对应设备的配置请查看相应的初始化文件awbl_hwconf_xxxxx.h
 *
 * \par 源代码
 * \snippet demo_gps.c src_gps
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-09  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_gps GPS定位
 * \copydoc demo_gps.c
 */

/** [src_gps] */
#include "aworks.h"
#include "aw_gnss.h"
#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_time.h"

#define __UBLOX_MODE_SET        0
#define __CNT226_MODE_SET       1

#define __GNSS_MODE_SET         __UBLOX_MODE_SET


aw_local aw_err_t  __gps_info_get (void)
{
    aw_err_t    ret = AW_OK;
    float       value[2];
    aw_tm_t     tm;
    aw_bool_t   is_ok = AW_FALSE;

    /* 由于 GPS 模块获取数据较慢，一般都是要重试多次，才有稳定数据 */
    ret = aw_gnss_dat_get(0, AW_GNSS_RMC_VALID, &is_ok, sizeof(is_ok));
    if (ret != AW_OK) {
        AW_ERRF(("get gps data valid err: %d\n",  ret));
        return  ret;
    }

    /* GPS 数据有效，打印具体数据信息 */
    if (is_ok) {

        AW_INFOF(("\n\n************ GPS Data ****************\n"));

        /* 获取定位数据 */
        ret = aw_gnss_dat_get(0, AW_GNSS_RMC_POSITION, value, 8);
        if (ret != AW_OK) {
            AW_ERRF(("aw_gnss_get_pos err: %d\n",  ret));
            return  ret;
        } else {

            /* 经度, 纬度 */
            AW_INFOF(("gps local: %f, %f\n", value[0], value[1]));
        }

        /* 获取海拔 */
        ret = aw_gnss_dat_get(0, AW_GNSS_GGA_ALTITUDE, value, 4);
        if (ret != AW_OK) {
            AW_ERRF(("get gga altitude failed: %d\n",  ret));
        } else {
            AW_INFOF(("altitude: %f\n", value[0]));
        }

        /* 获取RMC类型的UTC时间，中国时间为东8区 */
        ret = aw_gnss_dat_get(0, AW_GNSS_RMC_UTC_TIME, &tm, sizeof(tm));
        if (ret != AW_OK) {
            AW_ERRF(("get utc time failed: %d\n",  ret));
        } else {
            AW_INFOF(("UTC time: %d-%d-%d %02d:%02d:%02d\n",
                    tm.tm_year+1900,
                    tm.tm_mon+1,
                    tm.tm_mday,
                    tm.tm_hour,
                    tm.tm_min,
                    tm.tm_sec));
        }

        /* RMC的地面航向 */
        ret = aw_gnss_dat_get(0, AW_GNSS_RMC_COURSE, value, 4);
        if (ret != AW_OK) {
            AW_ERRF(("get course failed: %d\n",  ret));
        } else {
            AW_INFOF(("course: %f\n", value[0]));
        }

        /* 获取速度 */
        ret = aw_gnss_dat_get(0, AW_GNSS_VTG_RATE, value, 4);
        if (ret != AW_OK) {
            AW_ERRF(("get vtg rate failed: %d\n",  ret));
        } else {
            AW_INFOF(("vtg rate: %f\n", value[0]));
        }

        AW_INFOF(("**************************************\n\n"));
    }

    return ret;
}

/**
 * \brief GPS任务入口
 * \return 无
 */
void demo_gps_entry (void)
{
    aw_err_t  ret = AW_OK;

    int  dev_id = 0;
    int  i      = 0;

    /* 复位 GPS 模块（由于GPS初始化较慢，因而需要等待一段时间，重试多次） */
    /* AW_GNSS_HRESET: 硬复位，时间较长 */
    for (i = 0; i < 20; i++) {
        ret = aw_gnss_ioctl(dev_id, AW_GNSS_HRESET, (void *)AW_GNSS_HOT_STARTUP);
        if (ret == AW_OK) {
            break;
        }
        aw_mdelay(500);
    }
    if (i == 20) {
        AW_ERRF(("aw_gnss_ioctl AW_GNSS_HOT_STARTUP err: %d\n",  ret));
        return;
    }

    /* 启动 GPS 模块 */
    ret = aw_gnss_enable(dev_id);
    if (ret != AW_OK) {
        AW_ERRF(("aw_gnss_start err: %d\n",  ret));
        return;
    }

    /* 设置GPS模式输出 */
#if __GNSS_MODE_SET == __CNT226_MODE_SET
    ret = aw_gnss_ioctl(dev_id, AW_GNSS_SYS_FREQ, (void *)(AW_GNSS_BDS_B1 | AW_GNSS_GPS_L1));
#endif

#if __GNSS_MODE_SET == __CNT226_UBLOX_SET
    ret = aw_gnss_ioctl(dev_id, AW_GNSS_SYS_FREQ, (void *)(AW_GNSS_GPS_L1));
#endif

    if (ret != AW_OK) {
        AW_ERRF(("aw_gnss_ioctl err: %d\n",  ret));
        return;
    }

    /* 设置解码类型为RMC  VTG 和 GGA */
    ret = aw_gnss_ioctl(dev_id,
                        AW_GNSS_NMEA_SELECT,
                        (void *)(AW_GNSS_NMEA_RMC | AW_GNSS_NMEA_VTG | AW_GNSS_NMEA_GGA));
    if (ret != AW_OK) {
        AW_ERRF(("aw_gnss_ioctl AW_GNSS_NMEA_SELECT err: %d\n",  ret));
        return;
    }

    /*
     * 连续读取 GPS 信息
     */
    for (;;) {
        __gps_info_get();
        aw_mdelay(2000);
    }

}

/** [src_gps] */

/* end of file */
