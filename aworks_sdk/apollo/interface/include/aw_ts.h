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
 * \brief 触摸屏ts标准接口
 *
 * 使用本服务需要包含以下头文件:
 * \code
 * #include "aw_ts.h"
 * \endcode
 *
 * \note
 *
 *  (1) 抽象触摸设备与具体的触摸控制器无关, 它为GUI提供通用的触摸操作, 可直接替
 *      换GUI的触摸系统.
 *
 *      例如, 使用 AWorks 抽象触摸设备替换emwin的触摸系统
 *
 *      GUI_PID_STATE        th;             // emwin指针输入设备状态结构体
 *      struct aw_ts_state   ts_state;       // apollo触摸状态结构体
 *
 *      int ts_id = 0;
 *
 *      // 该接口必须最先调用，获取触摸服务的id
 *      ts_id = aw_ts_serv_id_get("480x272", 0x12345678, 0x12345678);
 *
 *      AW_FOREVER {
 *
 *          aw_ts_exec(ts_id, &ts_state, 1);  // 获取一个触摸点的数据(单点触摸)
 *
 *          th.x = ts_state.x;
 *          th.y = ts_state.y;
 *          th.Pressed = ts_state.pressed;
 *
 *          GUI_TOUCH_StoreStateEx(&th);      // 把结果存储进emwin
 *
 *          aw_mdelay(10);
 *      }
 *
 *  (2) 直接控制Touch Screen Controller的标准接口
 *      aw_ts_lock(aw_bool_t lock)  — 触摸屏锁屏 (AW_TRUE: 锁屏, AW_FALSE: 解锁)
 *
 *
 * \internal
 * \par modification history
 * - 1.01 17-02-08  anu, add MultiTouch.
 * - 1.01 14-09-22  ops, add the implementation of abstract ts device.
 * - 1.00 14-07-25  ops, first implementation.
 * \endinternal
 */
#ifndef __AW_TS_H
#define __AW_TS_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \addtogroup grp_aw_if_ts
 * \copydoc aw_ts.h
 * @{
 */

#include "aworks.h"
#include "aw_ts_lib.h"

/** \brief 触摸校准算法类型：两点校准  */
#define AW_TS_LIB_TWO_POINT    0

/** \brief 触摸校准算法类型：五点校准  */
#define AW_TS_LIB_FIVE_POINT   1


/** \brief 交换X、Y轴坐标数据 */
#define AW_TS_SWAP_XY         (1 << 2)

/** \brief 最大支持多少点的触摸 */
#define AW_MULTI_TS_CNT        5

#define AW_TS_DEFAULT_NAME   "default_tc"


/** \brief  空类型指针别名. */
typedef  void *aw_ts_id;

/**
 * \brief 触摸点状态
 */
struct aw_ts_state {
    int32_t x;         /**< \brief  x方向值 */
    int32_t y;         /**< \brief  y方向值 */
    int32_t slot;      /**< \brief  在当前触摸中代表第几个触摸点  */
    uint8_t pressed;   /**< \brief  是否触摸, 1 : 触摸, 0 : 未触摸 */
};

/**
 * \brief 获取触摸服务的id
 *
 * \param[in]  name     当前触摸服务的名字
 * \param[in]  vendor   当前触摸服务关联显示屏的厂商id(没有使用)
 *
 * \param[in]  product  当前触摸服务关联显示屏的产品id(没有使用)
 *
 * \return 触摸服务的id
 *
 * \note 使用触摸服务提供的接口，该接口必须最先被调用
 *
 * \Remarks change by CYX at 2019.1.4
 */
aw_ts_id aw_ts_serv_id_get (char *name, int vendor, int product);

/**
 * \brief 触摸校准
 *
 * \param[in]  ts_id      :  触摸服务id
 * \param[in]  p_calc_data:  获取得到的数据，用于校准
 *
 * \return  AW_OK         : 校准成功
 *         -AW_EINVAL     : 参数错误
 *         -AW_ENOTSUP    : 不支持的校正算法
 *
 */
int aw_ts_calibrate (aw_ts_id ts_id, aw_ts_lib_calibration_t *p_calc_data);

/**
 * \brief 保存触摸校准信息(旧)
 * \param[in]  ts_id      : 触摸服务id
 *
 *
 * \return  AW_OK         : 校准信息保存成功
 *         -AW_EINVAL     : 参数有误，未找到对应的触摸服务
 *         -AW_ENOTSUP    : 不支持校正算法
 *         -AW_EIO        : 写入触摸校准信息失败
 */
int aw_ts_calc_data_write (aw_ts_id ts_id);

/**
 * \brief 保存触摸校准信息(推荐使用)
 * \param[in]  ts_id      : 触摸服务id
 *
 *
 * \return  AW_OK         : 校准信息保存成功
 *         -AW_EINVAL     : 参数有误，未找到对应的触摸服务
 *         -AW_ENOTSUP    : 不支持校正算法
 *         -AW_EIO        : 写入触摸校准信息失败
 *
 * \Remarks change by CYX at 2019.1.4
 */
int aw_ts_cal_data_save (aw_ts_id ts_id);


/**
 * \brief 读取触摸校准信息(旧)
 * \param[in] ts_id      : 触摸服务id
 *
 * \return  AW_OK        : 校准信息读取成功
 *         -AW_EINVAL    : 参数有误，未找到对应的触摸服务
 *         -AW_EIO       : 写入触摸校准信息失败
 */
int aw_ts_calc_data_read (aw_ts_id ts_id);

/**
 * \brief 加载触摸校准信息(推荐使用)
 * \param[in] ts_id      : 触摸服务id
 *
 * \return  AW_OK        : 校准信息读取成功
 *         -AW_EINVAL    : 参数有误，未找到对应的触摸服务
 *         -AW_EIO       : 写入触摸校准信息失败
 *
 * \Remarks change by CYX at 2019.1.4
 */
int aw_ts_cal_data_load (aw_ts_id ts_id);


/**
 * \brief 设置触摸坐标轴方向
 *
 * \param[in] ts_id : 触摸服务id
 * \param[in] flags : 显示标记
 *            AW_TS_LIB_SWAP_XY : 交换坐标轴
 *
 * \return 无
 *
 * \note   触摸坐标轴方向默认为横屏显示
 */
void aw_ts_set_orientation (aw_ts_id ts_id, int flags);

/**
 * \brief 获取触摸坐标轴方向
 *
 * \param[in] ts_id : 触摸服务id
 *
 * \return AW_TS_LIB_SWAP_XY : 交换坐标轴
 *
 * \note   触摸坐标轴方向默认为横屏显示
 *
 * \Remarks add by CYX at 2019.1.4
 */
int aw_ts_get_orientation (aw_ts_id ts_id);


/**
 * \brief 触摸屏锁屏，禁止触摸
 * \param[in] ts_id: 触摸服务id
 * \param[in] lock : AW_TRUE : 锁屏 / AW_FALSE: 解锁
 *
 * \return 无
 */
void aw_ts_lock (aw_ts_id ts_id, aw_bool_t lock);

/**
 * \brief 触摸屏是否有按下
 *
 * \param[in] ts_id: 触摸服务id
 *
 * \return -AW_EINVAL: 参数错误
 *          0        : 触摸屏没有按下;
 *          1        : 触摸屏按下
 */
int aw_ts_is_touch (aw_ts_id ts_id);

/**
 * \brief 触摸屏是否需要校准(推荐使用)
 *
 * \param[in] ts_id: 触摸服务id
 *
 * \return -AW_EINVAL: 参数错误
 *          0        : 不需要校准;
 *          1        : 需要校准;
 *
 * \Remarks change by CYX at 2019.1.4
 */
int aw_ts_cal_attribute_get (aw_ts_id ts_id);

/**
 * \brief 触摸屏是否需要校准(旧)
 *
 * \param[in] ts_id: 触摸服务id
 *
 * \return -AW_EINVAL: 参数错误
 *          0        : 不需要校准;
 *          1        : 需要校准;
 */
int aw_ts_calc_flag_get (aw_ts_id ts_id);


/**
 * \brief 更新触摸状态信息，若有触摸点按下，返回触摸点的坐标值
 *
 * \param[in] ts_id    : 触摸服务id
 * \param[in] ts_point : 触摸状态缓冲区指针
 * \param[in] max_slots: 触摸状态缓冲区的个数，其值决定想要获取多少个触摸点数据
 *
 * \return -AW_EINVAL  : 参数有误，未找到对应的触摸服务
 *         -AW_ENOTSUP : 当前触摸服务不支持获取max_slots个多点触摸的数据
 *         others      : 当前触摸实际按下的触摸点数
 *
 * \note 该接口需要定期调用, 对触摸屏进行轮询, 以便不断更新触摸的状态
 *       用户传入传入触摸状态缓冲区的个数需等于max_slots
 */
int aw_ts_exec (aw_ts_id ts_id, struct aw_ts_state *ts_point, int max_slots);

/**
 * \brief 返回按下触摸点在滤波之后的采样值
 *
 * \param[in] ts_id     : 触摸服务id
 * \param[in] ts_sample : 触摸状态缓冲区指针
 * \param[in] max_slots : 触摸状态缓冲区的个数，其值决定想要获取多少个触摸点数据
 *
 * \return -AW_EINVAL   : 参数有误，未找到对应的触摸服务
 *         -AW_ENOTSUP  : 当前触摸服务不支持获取max_slots个多点触摸的数据
 *         others       : 当前触摸实际按下的触摸点数
 */
int aw_ts_get_phys (aw_ts_id ts_id, struct aw_ts_state *ts_sample, int max_slots);


/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AW_TS_H */

/* end of file */
