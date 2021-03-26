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
 * \brief 触摸屏 interface.
 *
 * \internal
 * \par modification history
 * - 1.01 17-02-08  anu, add MultiTouch.
 * - 1.01 16-10-24  anu, modify.
 * - 1.01 14-09-22  ops, add the implementation of abstract touch device.
 * - 1.00 14-07-25  ops, first implementation.
 * \endinternal
 */

#include "awbl_ts.h"
#include "aw_input.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_nvram.h"
#include "aw_spinlock.h"
#include <string.h>
#include "aw_assert.h"
#include "aw_sem.h"
#include "aw_mem.h"
#include "aw_md5.h"
#include "aw_ts.h"

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

/**
 * \brief 触摸校准信息
 */
typedef struct __awbl_ts_calc_info {

    char *name;                         /**< \brief 触摸关联的显示屏名字.  */
    int   vendor;                       /**< \brief 触摸关联的显示屏产商ID */
    int   product;                      /**< \brief 触摸关联的显示屏产品ID */

    int   x_res;                      /**< \brief 触摸关联显示屏的x轴分辨率. */
    int   y_res;                      /**< \brief 触摸关联显示屏的y轴分辨率. */

    /** \brief 校准系数.   */
    union {
        aw_ts_lib_factor_t factor;        /**< \brief 五点校准系数.  */
        aw_ts_lib_minmax_t xy_minmax[2];  /**< \brief 两点校准系数.  */
    };

}__awbl_ts_calc_info_t;

/** \brief  extern fuc */
extern int ts_calc_data_read  (void *p_cacl_info, int len);
extern int ts_calc_data_write (void *p_cacl_info, int len);


/** \brief  pointer to first ts service */
aw_local struct awbl_ts_service *p_serv_head;

/**
 * \brief find out a ts service who accept the id
 */
struct awbl_ts_service *__ts_id_to_serv (aw_ts_id id)
{
#if 0
    struct awbl_ts_service *p_serv_cur = p_serv_head;

    while ((p_serv_cur != NULL)) {

        if (id == p_serv_cur->p_service_info->ts_id) {

            return p_serv_cur;
        }

        p_serv_cur = p_serv_cur->p_next;
    }

#endif
    struct awbl_ts_service *p_serv_cur = (struct awbl_ts_service *)(id);

    if (NULL != p_serv_cur) {

       return p_serv_cur;
    }

    return NULL;
}

/**
 * \brief 触摸校准
 */
int aw_ts_calibrate (aw_ts_id ts_id, aw_ts_lib_calibration_t *p_calc_data)
{
    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    awbl_ts_service_info_t *p_info = NULL;
    int algorithm = 0;

    if (NULL == p_serv ) {
        return -AW_EINVAL;
    }

    p_info = p_serv ->p_service_info;

    if (p_info == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    algorithm = p_info->calc_algorithm;

    switch (algorithm) {

    case AW_TS_LIB_TWO_POINT: {

        if ((aw_ts_lib_two_point_calibrate(p_serv->xy_minmax,
                                           p_calc_data->cal_res_x,
                                           p_calc_data->cal_res_y,
                                           p_calc_data,
                                           p_info->calc_threshold)) != AW_OK) {
            AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
            return -AW_EINVAL;
        }

        break;
    }

    case AW_TS_LIB_FIVE_POINT: {

        if (aw_ts_lib_five_point_calibrate(&p_serv->factor,
                                            p_calc_data,
                                            p_info->calc_threshold) != AW_OK) {
            AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
            return -AW_EINVAL;
        }

        break;
    }

    default:
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_ENOTSUP;
    }

    /* 校准成功时记录这两个参数 */
    p_serv->cal_res_x = p_calc_data->cal_res_x;
    p_serv->cal_res_y = p_calc_data->cal_res_y;

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return AW_OK;

}

int aw_ts_calc_data_read (aw_ts_id ts_id)
{
    int ret = AW_OK;

    aw_md5_t md5;

    uint8_t md5_calc[16];         /* md5校验.   */

    struct __awbl_ts_calc_info *p_cacl_info = NULL;

    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    uint8_t calc_buf[sizeof(__awbl_ts_calc_info_t) + sizeof(md5_calc)];

    awbl_ts_service_info_t *p_info = NULL;

    if (NULL == p_serv ) {
        return -AW_EINVAL;
    }

    p_info = p_serv ->p_service_info;

    if (p_info == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    ret = ts_calc_data_read(calc_buf, sizeof(calc_buf));

    if (ret != AW_OK) {

        /* 数据读取出错，返回IO错误 */
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EIO;
    }

    p_cacl_info = (__awbl_ts_calc_info_t *)calc_buf;

    /* 名字不重要，重要的产商id及产品id */
    if (p_info->vendor  != p_cacl_info->vendor  ||
        p_info->product != p_cacl_info->product) {
        AW_ERRF(("the calc info not right!\n"));
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EINVAL;
    }

    aw_md5_init(&md5);

    /* 对数据进行校验 */
    aw_md5_update(&md5, (unsigned char *)p_cacl_info, sizeof(__awbl_ts_calc_info_t));
    aw_md5_final(md5_calc, &md5);

    if (0 != memcmp(md5_calc, &calc_buf[sizeof(__awbl_ts_calc_info_t)],
                                        sizeof(md5_calc))) {

        AW_ERRF(("the calc info md5 verify not right!\n"));
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EINVAL;
    }

    /* 填充相关系数 */
    p_serv->cal_res_x = p_cacl_info->x_res;
    p_serv->cal_res_y = p_cacl_info->y_res;
    p_serv->factor.det    = p_cacl_info->factor.det;
    p_serv->factor.det_x1 = p_cacl_info->factor.det_x1;
    p_serv->factor.det_x2 = p_cacl_info->factor.det_x2;
    p_serv->factor.det_x3 = p_cacl_info->factor.det_x3;
    p_serv->factor.det_y1 = p_cacl_info->factor.det_y1;
    p_serv->factor.det_y2 = p_cacl_info->factor.det_y2;
    p_serv->factor.det_y3 = p_cacl_info->factor.det_y3;

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return ret;
}

/*
 * \brief 加载触摸校准信息
 * \Remarks add by CYX at 2019.1.4
 */
int aw_ts_cal_data_load (aw_ts_id ts_id)
{
    int ret = AW_OK;

    aw_md5_t md5;

    uint8_t md5_calc[16];         /* md5校验.   */

    struct __awbl_ts_calc_info *p_cacl_info = NULL;

    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    uint8_t calc_buf[sizeof(__awbl_ts_calc_info_t) + sizeof(md5_calc)];

    awbl_ts_service_info_t *p_info = NULL;

    if (NULL == p_serv ) {
        return -AW_EINVAL;
    }

    p_info = p_serv ->p_service_info;

    if (p_info == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    ret = ts_calc_data_read(calc_buf, sizeof(calc_buf));

    if (ret != AW_OK) {

        /* 数据读取出错，返回IO错误 */
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EIO;
    }

    p_cacl_info = (__awbl_ts_calc_info_t *)calc_buf;

    /* 名字不重要，重要的产商id及产品id */
    if (p_info->vendor  != p_cacl_info->vendor  ||
        p_info->product != p_cacl_info->product) {
        AW_ERRF(("the calc info not right!\n"));
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EINVAL;
    }

    aw_md5_init(&md5);

    /* 对数据进行校验 */
    aw_md5_update(&md5, (unsigned char *)p_cacl_info,
                  sizeof(__awbl_ts_calc_info_t));
    aw_md5_final(md5_calc, &md5);

    if (0 != memcmp(md5_calc, &calc_buf[sizeof(__awbl_ts_calc_info_t)],
                                        sizeof(md5_calc))) {

        AW_ERRF(("the calc info md5 verify not right!\n"));
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EINVAL;
    }

    /* 填充相关系数 */
    p_serv->cal_res_x = p_cacl_info->x_res;
    p_serv->cal_res_y = p_cacl_info->y_res;
    p_serv->factor.det    = p_cacl_info->factor.det;
    p_serv->factor.det_x1 = p_cacl_info->factor.det_x1;
    p_serv->factor.det_x2 = p_cacl_info->factor.det_x2;
    p_serv->factor.det_x3 = p_cacl_info->factor.det_x3;
    p_serv->factor.det_y1 = p_cacl_info->factor.det_y1;
    p_serv->factor.det_y2 = p_cacl_info->factor.det_y2;
    p_serv->factor.det_y3 = p_cacl_info->factor.det_y3;

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return ret;
}

/**
 * \brief 保存触摸校准信息
 */
int aw_ts_calc_data_write (aw_ts_id ts_id)
{
    int ret = 0;

    aw_md5_t md5;

    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    struct __awbl_ts_calc_info cacl_info;

    uint8_t md5_calc[16];                 /* md5校验. */

    uint8_t calc_buf[sizeof(__awbl_ts_calc_info_t) + sizeof(md5_calc)];

    int algorithm  = 0;

    awbl_ts_service_info_t *p_info = NULL;

    if (NULL == p_serv ) {
        return -AW_EINVAL;
    }

    p_info = p_serv ->p_service_info;

    if (p_info == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    algorithm = p_info->calc_algorithm;

    /* 名字不重要，重要的产商id及产品id */
    cacl_info.name    = p_info->name;
    cacl_info.vendor  = p_info->vendor;
    cacl_info.product = p_info->product;
    cacl_info.x_res   = p_serv->cal_res_x;
    cacl_info.y_res   = p_serv->cal_res_y;

    aw_md5_init(&md5);

    switch (algorithm) {
    case AW_TS_LIB_TWO_POINT:{

        /* 记录校准系数 */
        cacl_info.xy_minmax[0].min = p_serv->xy_minmax[0].min;
        cacl_info.xy_minmax[0].max = p_serv->xy_minmax[0].max;
        cacl_info.xy_minmax[1].min = p_serv->xy_minmax[1].min;
        cacl_info.xy_minmax[1].max = p_serv->xy_minmax[1].max;

        break;
    }

    case AW_TS_LIB_FIVE_POINT: {

        /* 记录校准系数 */
        cacl_info.factor.det    = p_serv->factor.det;
        cacl_info.factor.det_x1 = p_serv->factor.det_x1;
        cacl_info.factor.det_x2 = p_serv->factor.det_x2;
        cacl_info.factor.det_x3 = p_serv->factor.det_x3;
        cacl_info.factor.det_y1 = p_serv->factor.det_y1;
        cacl_info.factor.det_y2 = p_serv->factor.det_y2;
        cacl_info.factor.det_y3 = p_serv->factor.det_y3;

        break;
    }

    default:
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_ENOTSUP;
    }


    /* 对保存的数据加入md5校验信息 */
    aw_md5_update(&md5, (unsigned char *)&cacl_info, sizeof(__awbl_ts_calc_info_t));
    aw_md5_final(md5_calc, &md5);

    memcpy((void *)calc_buf, (void *)&cacl_info, sizeof(__awbl_ts_calc_info_t));
    memcpy((void *)&calc_buf[sizeof(__awbl_ts_calc_info_t)], (void *)md5_calc,
                             sizeof(md5_calc));

    ret = ts_calc_data_write(calc_buf, sizeof(calc_buf));

    if (ret != AW_OK) {

        /* 数据读取出错，返回IO错误 */
        ret = -AW_EIO;
    }

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return ret;
}


/**
 * \brief 保存触摸校准信息
 * \Remarks add by CYX at 2019.1.4
 */
int aw_ts_cal_data_save (aw_ts_id ts_id)
{
    int ret = 0;

    aw_md5_t md5;

    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    struct __awbl_ts_calc_info cacl_info;

    uint8_t md5_calc[16];                 /* md5校验. */

    uint8_t calc_buf[sizeof(__awbl_ts_calc_info_t) + sizeof(md5_calc)];

    int algorithm  = 0;

    awbl_ts_service_info_t *p_info = NULL;

    if (NULL == p_serv ) {
        return -AW_EINVAL;
    }

    p_info = p_serv ->p_service_info;

    if (p_info == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    algorithm = p_info->calc_algorithm;

    /* 名字不重要，重要的产商id及产品id */
    cacl_info.name    = p_info->name;
    cacl_info.vendor  = p_info->vendor;
    cacl_info.product = p_info->product;
    cacl_info.x_res   = p_serv->cal_res_x;
    cacl_info.y_res   = p_serv->cal_res_y;

    aw_md5_init(&md5);

    switch (algorithm) {
    case AW_TS_LIB_TWO_POINT:{

        /* 记录校准系数 */
        cacl_info.xy_minmax[0].min = p_serv->xy_minmax[0].min;
        cacl_info.xy_minmax[0].max = p_serv->xy_minmax[0].max;
        cacl_info.xy_minmax[1].min = p_serv->xy_minmax[1].min;
        cacl_info.xy_minmax[1].max = p_serv->xy_minmax[1].max;

        break;
    }

    case AW_TS_LIB_FIVE_POINT: {

        /* 记录校准系数 */
        cacl_info.factor.det    = p_serv->factor.det;
        cacl_info.factor.det_x1 = p_serv->factor.det_x1;
        cacl_info.factor.det_x2 = p_serv->factor.det_x2;
        cacl_info.factor.det_x3 = p_serv->factor.det_x3;
        cacl_info.factor.det_y1 = p_serv->factor.det_y1;
        cacl_info.factor.det_y2 = p_serv->factor.det_y2;
        cacl_info.factor.det_y3 = p_serv->factor.det_y3;

        break;
    }

    default:
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_ENOTSUP;
    }


    /* 对保存的数据加入md5校验信息 */
    aw_md5_update(&md5, (unsigned char *)&cacl_info, sizeof(__awbl_ts_calc_info_t));
    aw_md5_final(md5_calc, &md5);

    memcpy((void *)calc_buf, (void *)&cacl_info, sizeof(__awbl_ts_calc_info_t));
    memcpy((void *)&calc_buf[sizeof(__awbl_ts_calc_info_t)], (void *)md5_calc,
                             sizeof(md5_calc));

    ret = ts_calc_data_write(calc_buf, sizeof(calc_buf));

    if (ret != AW_OK) {

        /* 数据读取出错，返回IO错误 */
        ret = -AW_EIO;
    }

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return ret;
}

/**
 * \brief 获取滤波后的采样值
 */
aw_local int __ts_filter_phys_get (struct awbl_ts_service *p_serv,
                                   struct aw_ts_state *sample_mt,
                                   int max_slots)
{
    int i = 0;
    int pressed_cnt = 0;

    awbl_ts_service_info_t * p_info= p_serv->p_service_info;

    if (p_serv       == NULL ||
        p_info       == NULL ||
        sample_mt    == NULL) {
        return 0;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);
    pressed_cnt = p_serv->drv_fun.pfn_xy_get(p_serv,
                                             (void *)p_serv->p_xsample,
                                             (void *)p_serv->p_ysample,
                                             max_slots,
                                             p_serv->p_service_info->sample_cnt);
    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    /* 按下的触摸点大于实际需要需要获取的点数，防止用户缓冲区溢出 */
    if (pressed_cnt > max_slots) {
        pressed_cnt = max_slots;
    }

    for (i = 0; i < pressed_cnt; i++) {

        uint16_t *p_xsample = (uint16_t *)p_serv->p_xsample[i];
        uint16_t *p_ysample = (uint16_t *)p_serv->p_ysample[i];

        sample_mt[i].x = aw_ts_lib_filter_mean(p_xsample,
                                               p_info->sample_cnt,
                                               p_info->filter_cnt);
        sample_mt[i].y = aw_ts_lib_filter_mean(p_ysample,
                                               p_info->sample_cnt,
                                               p_info->filter_cnt);
    }

    return pressed_cnt;
}

/*
 * \brief 设置x轴与y轴坐标关系
 */
void aw_ts_set_orientation (aw_ts_id ts_id, int flags)
{
    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    if (NULL == p_serv) {
        return ;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    p_serv->ts_orientation = flags;

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
}

/*
 * \brief 获取x轴与y轴坐标关系
 * \Remarks add by CYX at 2019.1.4
 */
int aw_ts_get_orientation (aw_ts_id ts_id)
{
	uint8_t temp;
    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    if (NULL == p_serv) {
        return -AW_EINVAL;
    }
    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    temp = p_serv->ts_orientation;

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return temp;
}

/*
 * \brief 获取x,y轴滤波后的采样值
 */
int aw_ts_get_phys (aw_ts_id ts_id, struct aw_ts_state *ts_sample, int max_slots)
{
    int ret = 0;
    int i = 0;

    int phy_x = 0;
    int phy_y = 0;

    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    if (NULL == p_serv) {
        return -AW_EINVAL;
    }

    /*　大于实际触摸点支持的个数 */
    if (max_slots > p_serv->p_service_info->multi_ts_cnt) {

        return  -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    ret = __ts_filter_phys_get(p_serv, ts_sample, max_slots);

    for (i = 0; i < ret; i++) {

        if (p_serv->ts_orientation == AW_TS_SWAP_XY) {

            phy_x = ts_sample[i].y;
            phy_y = ts_sample[i].x;

            /* 根据显示方向交换数据 */
            ts_sample[i].x = phy_x;
            ts_sample[i].y = phy_y;
        }

        ts_sample[i].pressed = 1;
        ts_sample[i].slot    = i;
    }

    /* 未检测到按下的触摸点按下状态缓冲区清0 */
    for (i = ret; i < max_slots; i++) {
        ts_sample[i].pressed = 0;
        ts_sample[i].slot    = 0;
    }

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return ret;
}

/**
 * \brief 触摸屏锁屏
 */
void aw_ts_lock (aw_ts_id ts_id, aw_bool_t lock)
{

    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    if (NULL == p_serv) {
        return ;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    p_serv->drv_fun.pfn_lock_set(p_serv, lock);

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
}

/**
 * \brief 触摸屏是否有按下
 */
int aw_ts_is_touch (aw_ts_id ts_id)
{
    int ret = 0;

    struct awbl_ts_service * p_serv = __ts_id_to_serv(ts_id);

    if (NULL == p_serv) {

        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    ret = p_serv->drv_fun.pfn_is_touch(p_serv);

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return ret;
}

/**
 * \brief 触摸屏是否需要校准
 * \Remarks change by CYX at 2019.1.4
 */
int aw_ts_cal_attribute_get (aw_ts_id ts_id)
{
    int ret = 0;

    struct awbl_ts_service * p_serv = __ts_id_to_serv(ts_id);

    if (NULL == p_serv) {

        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);
    ret =  p_serv->ts_cal_att;
    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return ret;
}

/**
 * \brief 触摸屏是否需要校准
 */
int aw_ts_calc_flag_get (aw_ts_id ts_id)
{
    int ret = 0;

    struct awbl_ts_service * p_serv = __ts_id_to_serv(ts_id);

    if (NULL == p_serv) {

        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);
    ret =  p_serv->ts_cal_att;
    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return ret;
}

/**
 * \brief 触摸状态更新
 */
int aw_ts_exec (aw_ts_id ts_id, struct aw_ts_state *ts_point, int max_slots)
{
    int i = 0;

    int pressed_cnt = 0;

    int mt_cnt = 0;

    int   phy_x = 0, phy_y = 0;

    aw_ts_lib_point_t coord = {0};

    struct aw_ts_state ts_sample[AW_MULTI_TS_CNT] = {0};

    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    awbl_ts_service_info_t *p_info = p_serv->p_service_info;

    if (NULL == p_serv || p_info == NULL) {

        return -AW_EINVAL;
    }

    /*　大于实际触摸点支持的个数 */
    if (max_slots > p_info->multi_ts_cnt) {

        return  -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    pressed_cnt = __ts_filter_phys_get(p_serv, ts_sample, max_slots);

    /* 未检测到有触摸点按下 */
    if (pressed_cnt == 0) {

        for (i = 0; i < max_slots; i++) {

            ts_point[i].pressed = 0;
            ts_point[i].slot    = 0;
        }

        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return  pressed_cnt;
    }

    for (i = 0; i < pressed_cnt; i++) {

        if (p_serv->ts_orientation == AW_TS_SWAP_XY) {
            phy_x = ts_sample[i].y;
            phy_y = ts_sample[i].x;

            /* 根据显示方向交换数据 */
            ts_sample[i].x = phy_x;
            ts_sample[i].y = phy_y;
        }

        /* 不需要校准，得到的数据即为坐标值 */
        if (p_serv->ts_cal_att == 0) {

            /* 不需要校准，得到的数据即为坐标值, 更新触摸状态  */
            ts_point[i].x       = ts_sample[i].x;
            ts_point[i].y       = ts_sample[i].y;
            ts_point[i].pressed = 1;
            ts_point[i].slot    = i;

        }
    }

    /* 不需要校准，得到的数据即为坐标值 */
    if (p_serv->ts_cal_att == 0) {

        goto exit;
    }

    /* 需要校准，得到的数据为采样值，需通过校准得到坐标值 */
    for (i = 0; i < pressed_cnt; i++) {
        #if 0
          /* 上下两次采样值相差在采样抖动的阀值范围内 */
         if ((abs(p_serv->x_last_sample[i] - ts_sample[i].x) <
             p_info->sample_threshold) &&
             (abs(p_serv->y_last_sample[i] - ts_sample[i].y) <
             p_info->sample_threshold)) {

             /* 认为是同一个点 */
             continue;

         }
        #endif

        if (p_info->calc_algorithm == AW_TS_LIB_TWO_POINT) {
            coord = aw_ts_lib_two_point_calc_coord(&p_serv->xy_minmax[0],
                                                    p_serv->cal_res_x,
                                                    p_serv->cal_res_y,
                                                    ts_sample[i].x,
                                                    ts_sample[i].y);


        } else if (p_info->calc_algorithm == AW_TS_LIB_FIVE_POINT) {
            coord = aw_ts_lib_five_point_calc_coord(&p_serv->factor,
                                                     ts_sample[i].x,
                                                     ts_sample[i].y);
        } else {
            AW_INFOF(("No selected calibration algorithm. \n"));
            AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
            return -AW_ENOTSUP;
        }


        if (coord.x < p_serv->cal_res_x &&
            coord.y < p_serv->cal_res_y &&
            coord.x > 0 &&
            coord.y > 0) {

            p_serv->x_last_sample[i] = ts_sample[i].x;
            p_serv->y_last_sample[i] = ts_sample[i].y;

            /* 更新该触摸点, 转换成坐标值  */
            ts_point[i].x = coord.x;
            ts_point[i].y = coord.y;
            ts_point[i].pressed = 1;
            ts_point[i].slot    = i;

        } else {
            /* 按下的点极度靠近边界，校准后出现偏差 */
            ts_point[i].pressed = 0;
            ts_point[i].slot    = 0;

            /* 记录校准后有偏差点的个数*/
            //mt_cnt++;
            /* mt_cnt++; */
        }
    }


exit:

   /* 未检测到按下的触摸点缓冲区按下状态清0 */
   for (i = pressed_cnt; i < max_slots; i++) {

        ts_point[i].pressed = 0;
        ts_point[i].slot    = 0;
    }

    /* todo 后续考虑输入子系统的多点触摸，当前仅支持一个触摸点的事件上报*/
    aw_input_report_coord(AW_INPUT_EV_ABS,
                          ABS_COORD,
                          ts_point[0].x,
                          ts_point[0].y,
                          0);

    aw_input_report_key(AW_INPUT_EV_KEY, BTN_TOUCH, ts_point[0].pressed);

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return (pressed_cnt - mt_cnt);
}

/** \brief 注册一个ts服务 */
aw_err_t awbl_ts_service_register (struct awbl_ts_service *p_serv)
{
    aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;

    if (p_serv == NULL) {
        return -AW_EINVAL;
    }

    aw_spinlock_isr_take(&lock);
    /* 插入链表 */
    p_serv->p_next = p_serv_head;
    p_serv_head = p_serv;

    aw_spinlock_isr_give(&lock);

    return AW_OK;
}


/** \brief 初始化一个ts服务 */
aw_err_t awbl_ts_service_init (struct awbl_ts_service      *p_serv,
                               struct awbl_ts_service_info *p_serv_info)
{
    int i = 0;

    if (p_serv == NULL || p_serv_info == NULL) {
        return -AW_EINVAL;
    }

    /* 当前触摸服务配置的触摸点数超过系统支持的最大触摸点数 */
    if (p_serv_info->multi_ts_cnt > AW_MULTI_TS_CNT) {
        return -AW_ENOTSUP;
    }

    /* 申请保存触摸点的缓冲区地址内存  */
    memset(p_serv, 0x00, sizeof(awbl_ts_service_t));

    p_serv->p_service_info = p_serv_info;

    /* 每个触摸点的采样缓冲区首地址  */
    for (i = 0; i < p_serv_info->multi_ts_cnt; i++) {
         p_serv->p_xsample[i] = (void *)aw_mem_align(sizeof(uint32_t) *
                                                     p_serv_info->sample_cnt, 4);
         p_serv->p_ysample[i] = (void *)aw_mem_align(sizeof(uint32_t) *
                                                     p_serv_info->sample_cnt, 4);
    }

    for (i = 0; i < p_serv_info->multi_ts_cnt; i++) {

         if ((p_serv->p_xsample[i] != NULL) && ( p_serv->p_ysample[i] != NULL)) {

             AW_INFOF(("xsample buffer[%d] malloc addr 0x%x\n",
                       i,
                       p_serv->p_xsample[i]));
             AW_INFOF(("ysample buffer[%d] malloc addr 0x%x\n",
                       i,
                       p_serv->p_ysample[i]));

         } else {

             AW_ERRF(("TS sample buffer[%d] malloc failed\n", i));

             while(1);
         }
    }

    /* 初始化ts_serv_mux互斥信号量 */
    AW_MUTEX_INIT(p_serv->ts_serv_mux, AW_SEM_Q_PRIORITY);

    return AW_OK;
}


/**
 * \brief 获取触摸服务的id
 * \Remarks change by CYX at 2019.1.4
 */
aw_ts_id aw_ts_serv_id_get (char *name, int vendor, int product)
{
    if (NULL == name) {
        return  NULL;
    }

    aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;
    /* 获取触摸屏服务链表*/
    struct awbl_ts_service *p_serv = p_serv_head;

    aw_spinlock_isr_take(&lock);

    /* 如果名字是系统默认的名字，则返回触摸屏服务链表第一个触摸屏服务*/
    if ((NULL != p_serv) && (strcmp(AW_TS_DEFAULT_NAME, name) == 0)){
        aw_spinlock_isr_give(&lock);
        return p_serv;
    }
    else{
        /* 链表不为空，则遍历触摸屏服务链表 ，找到和名字相同的触摸屏服务结构体 */
        while (NULL != p_serv) {
            if (strcmp(p_serv->p_service_info->name, name) == 0) {

                aw_spinlock_isr_give(&lock);
                return p_serv;
            }

            p_serv = p_serv->p_next;
        }
    }
    aw_spinlock_isr_give(&lock);

    return NULL;
}


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

/* end of file */

