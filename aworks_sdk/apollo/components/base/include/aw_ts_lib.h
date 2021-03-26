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
 * \brief 触摸算法标准接口
 *
 * 使用本接口需要包含以下头文件：
 * \code
 * #include "aw_ts_lib.h"
 * \endcode
 *
 *     本接口针对触摸子系统的误差来源, 为抽象触摸设备提供了滤波、
 *     坐标校准等算法的实现
 *
 * \par 简单示例
 * \code
 *
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.01 14-10-14  ops, add five point calibration algorithm.
 * - 1.00 14-09-12  ops, first implementation
 * \endinternal
 */

#ifndef AW_TS_LIB_H_
#define AW_TS_LIB_H_

#include "aworks.h"
#include "aw_types.h"

/** \brief 单X轴方向 */
#define AW_TS_LIB_COORD_X    0

/** \brief 单Y轴方向 */
#define AW_TS_LIB_COORD_Y    1

/**
 * \brief 坐标点
 */
typedef struct aw_ts_lib_point {
    int32_t x, y;
}aw_ts_lib_point_t;

/**
 * \brief 五点校准系数
 */
typedef struct aw_ts_lib_factor {

    int64_t det;
    int64_t det_x1, det_x2, det_x3;
    int64_t det_y1, det_y2, det_y3;

}aw_ts_lib_factor_t;

/**
 * \brief 两点校准系数
 */
typedef struct aw_ts_lib_minmax {
    int64_t min, max;
}aw_ts_lib_minmax_t;

/**
 * \brief 获取用于校准的相关数据,兼容两点及五点校准
 */
typedef struct aw_ts_lib_calibration {
    aw_ts_lib_point_t  log[5];       /**< \brief 五个点的坐标值.      */
    aw_ts_lib_point_t  phy[5];       /**< \brief 五个点的采样值(测量值) */

    int32_t  cal_res_x;              /**< \brief 校准的x轴分辨率. */
    int32_t  cal_res_y;              /**< \brief 校准的y轴分辨率. */

}aw_ts_lib_calibration_t;

/*******************************************************************************
  滤波算法
*******************************************************************************/

/** \brief 带阀值的中值滤波
 *
 *  算法原理
 *   (1) 连续采样NUM次, 按大小值对NUM次采样进行排列(使用冒泡排序, 形成降序数列).
 *   (2) 取中间两个样本, 如差值大于阀值, 则丢弃. 如小于阀值, 则计算两个样本的均值,
 *       作为本次采样的有效值.
 *
 *  \param   sample      样本集合
 *           num         样本数量
 *           threshold   阀值
 *
 *  \return  滤波后的采样值
 *
 *  \error  -AW_EDOM  中间两个样本的差值大于阀值
 */
int aw_ts_lib_filter_median (uint16_t *sample, int num, int threshold);


/** \brief 带窗口的均值滤波
 *
 *  算法原理
 *   (1) 连续采样NUM次, 按大小值对NUM次采样进行排列(使用冒泡排序, 形成降序数列).
 *   (2) 取窗口尺寸WIN_SIZE个数据, 进行均值计算, 作为本次采样的有效值.
 *       (窗口尺寸指的是数列中间的若干个数据.)
 *
 *  \param   sample    样本集合
 *           num       样本数量
 *           win_size  窗口尺寸
 *
 *  \return  滤波后的采样值
 *
 *  \error -AW_EINVAL  窗口尺寸大于样本数量, 窗口尺寸小于等于零
 */
int aw_ts_lib_filter_mean (uint16_t *sample, int num, int win_size);

/**********************************************************************
 *
 *   校准算法
 *
 *   使用约束 ：先调用校准接口, 后调用计算接口
 *
 **********************************************************************/

/**
 * \brief 两点触摸校准
 *
 * \param phy_minmax      X与Y方向最大和最小的采样值
 * \param cal_xsize       X方向上的分辨率
 * \param cal_ysize       Y方向上的分辨率
 * \param cal             校准数据
 * \param threshold       验证阀值
 *
 * \return AW_OK     校准完成
 *
 * \error  -AW_EINVAL   非法的校准方向
 */
int aw_ts_lib_two_point_calibrate (aw_ts_lib_minmax_t      *phy_minmax,
                                   int                      cal_xsize,
                                   int                      cal_ysize,
                                   aw_ts_lib_calibration_t *cal,
                                   uint16_t                 threshold);

/**
 * \brief 根据采样值计算坐标值 (调用该接口前, 请先调用两点校准接口)
 *
 * \param phy_minmax      X与Y方向最大和最小的A/D转换值
 * \param cal_xsize       X方向上的分辨率
 * \param cal_ysize       Y方向上的分辨率
 * \param phy_x           该计算点的在x方向的采样值
 * \param phy_y           该计算点的在y方向的采样值
 *
 * \return  返回校准的坐标值
 */
aw_ts_lib_point_t aw_ts_lib_two_point_calc_coord (aw_ts_lib_minmax_t *p_phy_minmax,
                                                  int                 cal_xsize,
                                                  int                 cal_ysize,
                                                  int                 phy_x,
                                                  int                 phy_y);


/**
 * \brief 五点触摸校准算法, 来自于linux TsLib
 *
 * \param p_factor    指向校准系数的指针
 * \param pdata       校准数据
 * \param threshold   验证阀值
 *                    (该算法计算完成后, 进行验证, 
 *                     计算值与原值的差值需小于验证阀值, 
 *                     算法才判定校准成功.)
 *
 * \return AW_OK     校准完成
 *
 * \error  -AW_EDOM  计算值与原值的差值大于验证阀值
 */
int aw_ts_lib_five_point_calibrate (aw_ts_lib_factor_t      *p_factor,
                                    aw_ts_lib_calibration_t *cal,
                                    uint16_t                 threshold);


/**
 * \brief 根据采样值计算坐标值 (调用该接口前, 请先调用五点校准接口)
 *
 * \param p_factor    指向校准系数的指针
 * \param phy_x       该计算点在X轴上的采样值
 * \param phy_y       该计算点在Y轴上的采样值
 *
 * \return log        返回该点的坐标值
 */
aw_ts_lib_point_t aw_ts_lib_five_point_calc_coord (aw_ts_lib_factor_t *p_factor,
                                                   int                 phy_x,
                                                   int                 phy_y);


#endif /* AW_TS_LIB_H_ */
