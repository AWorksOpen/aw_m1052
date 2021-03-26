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
 * \brief 触摸屏(touch screen)中间件
 *
 * \internal
 * \par modification history
 * - 1.02 18-04-20  nwt, change.
 * - 1.01 17-02-08  anu, add MultiTouch.
 * - 1.01 14-09-12  ops, add algorithm operator for ts_dev.
 * - 1.00 14-07-25  ops, first implementation.
 * \endinternal
 */
#ifndef AWBL_TS_H_
#define AWBL_TS_H_

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "aw_ts.h"

#ifdef __cplusplus
extern "C" {
#endif


/* 使用匿名联合体段结束 */
#if defined(__CC_ARM) || defined(__ARMCOMPILER_VERSION)
    #if defined(__CC_ARM) /* ARM编译器 版本5 支持 */
        #pragma push
        #pragma anon_unions
    #endif
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)

  /* 默认使能匿名联合体 */
#elif defined(__TMS470__)

  /* 默认使能匿名联合体 */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler t
#endif

/** \brief struct awbl_ts_service pre declaration */
struct awbl_ts_service;


/** \brief ts service info */
typedef struct awbl_ts_service_info {

    /** \brief 触摸关联的显示屏名字.  */
    char     *name;

    /** \brief 触摸关联的显示屏产商ID */
    int      vendor;

    /** \brief 触摸关联的显示屏产品ID */
    int      product;

    /** \brief 支持触摸点的个数.    */
    int      multi_ts_cnt;

    /** \brief 采样个数, 最小值为1  */
    int      sample_cnt;

    /** \brief 滤波个数, 不能大于采样个数, 最小值为1 */
    int      filter_cnt;

    /** \brief 消抖时长(电容屏该参数可选).     */
    uint16_t debounce_ms;

    /** \brief 采样门限阀值(电容屏该参数可选).  */
    uint16_t sample_threshold;

    /** \brief 校准门限阀值(电容屏该参数可选). */
    uint16_t calc_threshold;

    /**
     * \brief 校准算法(电容屏该参数可选)
     *        AW_TS_LIB_TWO_POINT : 两点校准
     *        AW_TS_LIB_FIVE_POINT: 五点校准
     */
    uint8_t  calc_algorithm;

} awbl_ts_service_info_t;

/** \brief ts service driver function */
typedef struct awbl_ts_drv_funcs {

    /* 获取X Y轴坐标，并且返回有多少个点按下 */
    int (*pfn_xy_get) (struct awbl_ts_service *p_serv,
                       void                   *p_x_sample[],
                       void                   *p_y_sample[],
                       int                     max_slots,
                       int                     count);

    /* 设置是否锁屏 */
    void (*pfn_lock_set) (struct awbl_ts_service *p_serv, aw_bool_t lock);

    /* 是否按下 */
    int (*pfn_is_touch)  (struct awbl_ts_service *p_serv);

    /* 准备测量X轴坐标  */
    void (*pfn_active_x) (struct awbl_ts_service *p_serv);

    /* 准备测量Y轴坐标  */
    void (*pfn_active_y) (struct awbl_ts_service *p_serv);

    /* 测量X轴采样值  */
    void (*pfn_measure_x) (struct awbl_ts_service *p_serv,
                           void                   *p_sample[],
                           int                     max_slots,
                           int                     count);

    /* 测量Y轴采样值  */
    void (*pfn_measure_y) (struct awbl_ts_service *p_serv,
                           void                   *p_sample[],
                           int                     max_slots,
                           int                     count);
} awbl_ts_drv_funcs_t;

/** \brief ts service */
typedef struct awbl_ts_service {

    struct awbl_ts_service *p_next;           /**< \brief next ts service    */

    awbl_ts_service_info_t *p_service_info;   /**< \brief ts service info    */

    awbl_ts_drv_funcs_t     drv_fun;          /**< \brief ts service drv fuc */

    /** \brief calibrate factor  */
    union {

        /** \brief five point calibrate factor */
        aw_ts_lib_factor_t factor;

        /** \brief two point calibrate factor  */
        aw_ts_lib_minmax_t xy_minmax[2];
    };

    /** \brief  Screen resolution at the time when calibration was performed */
    uint32_t  cal_res_x;
    uint32_t  cal_res_y;

    /** \brief ts orientation, default horizontal screen display   */
    uint8_t   ts_orientation;


    /**
     * \brief ts cal att
     *
     *  0: don't need calibration
     *  1: need calibration
     */
    uint8_t   ts_cal_att;

    void     *p_xsample[AW_MULTI_TS_CNT];    /**< \brief x sample addr buffer */
    void     *p_ysample[AW_MULTI_TS_CNT];    /**< \brief y sample addr buffer */

    /** \brief x last sample data buffer */
    uint32_t  x_last_sample[AW_MULTI_TS_CNT];

    /** \brief y last sample data buffer */
    uint32_t  y_last_sample[AW_MULTI_TS_CNT];

    AW_MUTEX_DECL(ts_serv_mux);               /**< \brief ts serv mux sem */

} awbl_ts_service_t;


/**
 * \brief 初始化一个ts服务
 *
 * \param[in]  p_serv      :  指向ts服务的指针
 * \param[in]  p_serv_info :  指向ts服务信息的指针
 *
 * \return AW_OK           : 初始化成功
 *         -AW_EINVAL      : 参数错误
 *         -AW_ENOTSUP     : 当前触摸配置的多重触摸点数超过系统支持的最大触摸
 *                           点数 AW_MULTI_TS_CNT
 *
 */
aw_err_t awbl_ts_service_init (struct awbl_ts_service      *p_serv,
                               struct awbl_ts_service_info *p_serv_info);

/**
 * \brief ts服务注册函数
 *
 * \param[in]  p_serv  : 指向ts服务的指针
 *
 * \return AW_OK       : 注册成功
 *         -AW_EINVAL  : 参数错误
 */
aw_err_t awbl_ts_service_register (struct awbl_ts_service  *p_serv);

/* 使用匿名联合体段结束 */
#if defined(__CC_ARM) || defined(__ARMCOMPILER_VERSION)
    #if defined(__CC_ARM) /* ARM编译器 版本5 支持 */
        #pragma pop
    #endif
#elif defined(__ICCARM__)

  /* 允许匿名联合体使能 */
#elif defined(__GNUC__)

  /* 默认使用匿名联合体 */
#elif defined(__TMS470__)

  /* 默认使用匿名联合体 */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler t
#endif

#ifdef __cplusplus
}
#endif


#endif /* AWBL_TS_H_ */

/* end of file */
