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
 * \brief ������ interface.
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

/* ʹ������������ν��� */
#if defined(__CC_ARM) || defined(__ARMCOMPILER_VERSION)
    #if defined(__CC_ARM) /* ARM������ �汾5 ֧�� */
        #pragma push
        #pragma anon_unions
    #endif
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)

  /* Ĭ��ʹ������������ */
#elif defined(__TMS470__)

  /* Ĭ��ʹ������������ */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler t
#endif

/**
 * \brief ����У׼��Ϣ
 */
typedef struct __awbl_ts_calc_info {

    char *name;                         /**< \brief ������������ʾ������.  */
    int   vendor;                       /**< \brief ������������ʾ������ID */
    int   product;                      /**< \brief ������������ʾ����ƷID */

    int   x_res;                      /**< \brief ����������ʾ����x��ֱ���. */
    int   y_res;                      /**< \brief ����������ʾ����y��ֱ���. */

    /** \brief У׼ϵ��.   */
    union {
        aw_ts_lib_factor_t factor;        /**< \brief ���У׼ϵ��.  */
        aw_ts_lib_minmax_t xy_minmax[2];  /**< \brief ����У׼ϵ��.  */
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
 * \brief ����У׼
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

    /* У׼�ɹ�ʱ��¼���������� */
    p_serv->cal_res_x = p_calc_data->cal_res_x;
    p_serv->cal_res_y = p_calc_data->cal_res_y;

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return AW_OK;

}

int aw_ts_calc_data_read (aw_ts_id ts_id)
{
    int ret = AW_OK;

    aw_md5_t md5;

    uint8_t md5_calc[16];         /* md5У��.   */

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

        /* ���ݶ�ȡ��������IO���� */
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EIO;
    }

    p_cacl_info = (__awbl_ts_calc_info_t *)calc_buf;

    /* ���ֲ���Ҫ����Ҫ�Ĳ���id����Ʒid */
    if (p_info->vendor  != p_cacl_info->vendor  ||
        p_info->product != p_cacl_info->product) {
        AW_ERRF(("the calc info not right!\n"));
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EINVAL;
    }

    aw_md5_init(&md5);

    /* �����ݽ���У�� */
    aw_md5_update(&md5, (unsigned char *)p_cacl_info, sizeof(__awbl_ts_calc_info_t));
    aw_md5_final(md5_calc, &md5);

    if (0 != memcmp(md5_calc, &calc_buf[sizeof(__awbl_ts_calc_info_t)],
                                        sizeof(md5_calc))) {

        AW_ERRF(("the calc info md5 verify not right!\n"));
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EINVAL;
    }

    /* ������ϵ�� */
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
 * \brief ���ش���У׼��Ϣ
 * \Remarks add by CYX at 2019.1.4
 */
int aw_ts_cal_data_load (aw_ts_id ts_id)
{
    int ret = AW_OK;

    aw_md5_t md5;

    uint8_t md5_calc[16];         /* md5У��.   */

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

        /* ���ݶ�ȡ��������IO���� */
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EIO;
    }

    p_cacl_info = (__awbl_ts_calc_info_t *)calc_buf;

    /* ���ֲ���Ҫ����Ҫ�Ĳ���id����Ʒid */
    if (p_info->vendor  != p_cacl_info->vendor  ||
        p_info->product != p_cacl_info->product) {
        AW_ERRF(("the calc info not right!\n"));
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EINVAL;
    }

    aw_md5_init(&md5);

    /* �����ݽ���У�� */
    aw_md5_update(&md5, (unsigned char *)p_cacl_info,
                  sizeof(__awbl_ts_calc_info_t));
    aw_md5_final(md5_calc, &md5);

    if (0 != memcmp(md5_calc, &calc_buf[sizeof(__awbl_ts_calc_info_t)],
                                        sizeof(md5_calc))) {

        AW_ERRF(("the calc info md5 verify not right!\n"));
        AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);
        return -AW_EINVAL;
    }

    /* ������ϵ�� */
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
 * \brief ���津��У׼��Ϣ
 */
int aw_ts_calc_data_write (aw_ts_id ts_id)
{
    int ret = 0;

    aw_md5_t md5;

    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    struct __awbl_ts_calc_info cacl_info;

    uint8_t md5_calc[16];                 /* md5У��. */

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

    /* ���ֲ���Ҫ����Ҫ�Ĳ���id����Ʒid */
    cacl_info.name    = p_info->name;
    cacl_info.vendor  = p_info->vendor;
    cacl_info.product = p_info->product;
    cacl_info.x_res   = p_serv->cal_res_x;
    cacl_info.y_res   = p_serv->cal_res_y;

    aw_md5_init(&md5);

    switch (algorithm) {
    case AW_TS_LIB_TWO_POINT:{

        /* ��¼У׼ϵ�� */
        cacl_info.xy_minmax[0].min = p_serv->xy_minmax[0].min;
        cacl_info.xy_minmax[0].max = p_serv->xy_minmax[0].max;
        cacl_info.xy_minmax[1].min = p_serv->xy_minmax[1].min;
        cacl_info.xy_minmax[1].max = p_serv->xy_minmax[1].max;

        break;
    }

    case AW_TS_LIB_FIVE_POINT: {

        /* ��¼У׼ϵ�� */
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


    /* �Ա�������ݼ���md5У����Ϣ */
    aw_md5_update(&md5, (unsigned char *)&cacl_info, sizeof(__awbl_ts_calc_info_t));
    aw_md5_final(md5_calc, &md5);

    memcpy((void *)calc_buf, (void *)&cacl_info, sizeof(__awbl_ts_calc_info_t));
    memcpy((void *)&calc_buf[sizeof(__awbl_ts_calc_info_t)], (void *)md5_calc,
                             sizeof(md5_calc));

    ret = ts_calc_data_write(calc_buf, sizeof(calc_buf));

    if (ret != AW_OK) {

        /* ���ݶ�ȡ��������IO���� */
        ret = -AW_EIO;
    }

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return ret;
}


/**
 * \brief ���津��У׼��Ϣ
 * \Remarks add by CYX at 2019.1.4
 */
int aw_ts_cal_data_save (aw_ts_id ts_id)
{
    int ret = 0;

    aw_md5_t md5;

    struct awbl_ts_service *p_serv = __ts_id_to_serv(ts_id);

    struct __awbl_ts_calc_info cacl_info;

    uint8_t md5_calc[16];                 /* md5У��. */

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

    /* ���ֲ���Ҫ����Ҫ�Ĳ���id����Ʒid */
    cacl_info.name    = p_info->name;
    cacl_info.vendor  = p_info->vendor;
    cacl_info.product = p_info->product;
    cacl_info.x_res   = p_serv->cal_res_x;
    cacl_info.y_res   = p_serv->cal_res_y;

    aw_md5_init(&md5);

    switch (algorithm) {
    case AW_TS_LIB_TWO_POINT:{

        /* ��¼У׼ϵ�� */
        cacl_info.xy_minmax[0].min = p_serv->xy_minmax[0].min;
        cacl_info.xy_minmax[0].max = p_serv->xy_minmax[0].max;
        cacl_info.xy_minmax[1].min = p_serv->xy_minmax[1].min;
        cacl_info.xy_minmax[1].max = p_serv->xy_minmax[1].max;

        break;
    }

    case AW_TS_LIB_FIVE_POINT: {

        /* ��¼У׼ϵ�� */
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


    /* �Ա�������ݼ���md5У����Ϣ */
    aw_md5_update(&md5, (unsigned char *)&cacl_info, sizeof(__awbl_ts_calc_info_t));
    aw_md5_final(md5_calc, &md5);

    memcpy((void *)calc_buf, (void *)&cacl_info, sizeof(__awbl_ts_calc_info_t));
    memcpy((void *)&calc_buf[sizeof(__awbl_ts_calc_info_t)], (void *)md5_calc,
                             sizeof(md5_calc));

    ret = ts_calc_data_write(calc_buf, sizeof(calc_buf));

    if (ret != AW_OK) {

        /* ���ݶ�ȡ��������IO���� */
        ret = -AW_EIO;
    }

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return ret;
}

/**
 * \brief ��ȡ�˲���Ĳ���ֵ
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

    /* ���µĴ��������ʵ����Ҫ��Ҫ��ȡ�ĵ�������ֹ�û���������� */
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
 * \brief ����x����y�������ϵ
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
 * \brief ��ȡx����y�������ϵ
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
 * \brief ��ȡx,y���˲���Ĳ���ֵ
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

    /*������ʵ�ʴ�����֧�ֵĸ��� */
    if (max_slots > p_serv->p_service_info->multi_ts_cnt) {

        return  -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    ret = __ts_filter_phys_get(p_serv, ts_sample, max_slots);

    for (i = 0; i < ret; i++) {

        if (p_serv->ts_orientation == AW_TS_SWAP_XY) {

            phy_x = ts_sample[i].y;
            phy_y = ts_sample[i].x;

            /* ������ʾ���򽻻����� */
            ts_sample[i].x = phy_x;
            ts_sample[i].y = phy_y;
        }

        ts_sample[i].pressed = 1;
        ts_sample[i].slot    = i;
    }

    /* δ��⵽���µĴ����㰴��״̬��������0 */
    for (i = ret; i < max_slots; i++) {
        ts_sample[i].pressed = 0;
        ts_sample[i].slot    = 0;
    }

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return ret;
}

/**
 * \brief ����������
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
 * \brief �������Ƿ��а���
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
 * \brief �������Ƿ���ҪУ׼
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
 * \brief �������Ƿ���ҪУ׼
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
 * \brief ����״̬����
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

    /*������ʵ�ʴ�����֧�ֵĸ��� */
    if (max_slots > p_info->multi_ts_cnt) {

        return  -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_serv->ts_serv_mux, AW_SEM_WAIT_FOREVER);

    pressed_cnt = __ts_filter_phys_get(p_serv, ts_sample, max_slots);

    /* δ��⵽�д����㰴�� */
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

            /* ������ʾ���򽻻����� */
            ts_sample[i].x = phy_x;
            ts_sample[i].y = phy_y;
        }

        /* ����ҪУ׼���õ������ݼ�Ϊ����ֵ */
        if (p_serv->ts_cal_att == 0) {

            /* ����ҪУ׼���õ������ݼ�Ϊ����ֵ, ���´���״̬  */
            ts_point[i].x       = ts_sample[i].x;
            ts_point[i].y       = ts_sample[i].y;
            ts_point[i].pressed = 1;
            ts_point[i].slot    = i;

        }
    }

    /* ����ҪУ׼���õ������ݼ�Ϊ����ֵ */
    if (p_serv->ts_cal_att == 0) {

        goto exit;
    }

    /* ��ҪУ׼���õ�������Ϊ����ֵ����ͨ��У׼�õ�����ֵ */
    for (i = 0; i < pressed_cnt; i++) {
        #if 0
          /* �������β���ֵ����ڲ��������ķ�ֵ��Χ�� */
         if ((abs(p_serv->x_last_sample[i] - ts_sample[i].x) <
             p_info->sample_threshold) &&
             (abs(p_serv->y_last_sample[i] - ts_sample[i].y) <
             p_info->sample_threshold)) {

             /* ��Ϊ��ͬһ���� */
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

            /* ���¸ô�����, ת��������ֵ  */
            ts_point[i].x = coord.x;
            ts_point[i].y = coord.y;
            ts_point[i].pressed = 1;
            ts_point[i].slot    = i;

        } else {
            /* ���µĵ㼫�ȿ����߽磬У׼�����ƫ�� */
            ts_point[i].pressed = 0;
            ts_point[i].slot    = 0;

            /* ��¼У׼����ƫ���ĸ���*/
            //mt_cnt++;
            /* mt_cnt++; */
        }
    }


exit:

   /* δ��⵽���µĴ����㻺��������״̬��0 */
   for (i = pressed_cnt; i < max_slots; i++) {

        ts_point[i].pressed = 0;
        ts_point[i].slot    = 0;
    }

    /* todo ��������������ϵͳ�Ķ�㴥������ǰ��֧��һ����������¼��ϱ�*/
    aw_input_report_coord(AW_INPUT_EV_ABS,
                          ABS_COORD,
                          ts_point[0].x,
                          ts_point[0].y,
                          0);

    aw_input_report_key(AW_INPUT_EV_KEY, BTN_TOUCH, ts_point[0].pressed);

    AW_MUTEX_UNLOCK(p_serv->ts_serv_mux);

    return (pressed_cnt - mt_cnt);
}

/** \brief ע��һ��ts���� */
aw_err_t awbl_ts_service_register (struct awbl_ts_service *p_serv)
{
    aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;

    if (p_serv == NULL) {
        return -AW_EINVAL;
    }

    aw_spinlock_isr_take(&lock);
    /* �������� */
    p_serv->p_next = p_serv_head;
    p_serv_head = p_serv;

    aw_spinlock_isr_give(&lock);

    return AW_OK;
}


/** \brief ��ʼ��һ��ts���� */
aw_err_t awbl_ts_service_init (struct awbl_ts_service      *p_serv,
                               struct awbl_ts_service_info *p_serv_info)
{
    int i = 0;

    if (p_serv == NULL || p_serv_info == NULL) {
        return -AW_EINVAL;
    }

    /* ��ǰ�����������õĴ�����������ϵͳ֧�ֵ���������� */
    if (p_serv_info->multi_ts_cnt > AW_MULTI_TS_CNT) {
        return -AW_ENOTSUP;
    }

    /* ���뱣�津����Ļ�������ַ�ڴ�  */
    memset(p_serv, 0x00, sizeof(awbl_ts_service_t));

    p_serv->p_service_info = p_serv_info;

    /* ÿ��������Ĳ����������׵�ַ  */
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

    /* ��ʼ��ts_serv_mux�����ź��� */
    AW_MUTEX_INIT(p_serv->ts_serv_mux, AW_SEM_Q_PRIORITY);

    return AW_OK;
}


/**
 * \brief ��ȡ���������id
 * \Remarks change by CYX at 2019.1.4
 */
aw_ts_id aw_ts_serv_id_get (char *name, int vendor, int product)
{
    if (NULL == name) {
        return  NULL;
    }

    aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;
    /* ��ȡ��������������*/
    struct awbl_ts_service *p_serv = p_serv_head;

    aw_spinlock_isr_take(&lock);

    /* ���������ϵͳĬ�ϵ����֣��򷵻ش��������������һ������������*/
    if ((NULL != p_serv) && (strcmp(AW_TS_DEFAULT_NAME, name) == 0)){
        aw_spinlock_isr_give(&lock);
        return p_serv;
    }
    else{
        /* ����Ϊ�գ�������������������� ���ҵ���������ͬ�Ĵ���������ṹ�� */
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


/* ʹ������������ν��� */
#if defined(__CC_ARM) || defined(__ARMCOMPILER_VERSION)
    #if defined(__CC_ARM) /* ARM������ �汾5 ֧�� */
        #pragma pop
    #endif
#elif defined(__ICCARM__)

  /* ��������������ʹ�� */
#elif defined(__GNUC__)

  /* Ĭ��ʹ������������ */
#elif defined(__TMS470__)

  /* Ĭ��ʹ������������ */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler t
#endif

/* end of file */

