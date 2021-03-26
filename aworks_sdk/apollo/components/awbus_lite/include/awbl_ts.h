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
 * \brief ������(touch screen)�м��
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

/** \brief struct awbl_ts_service pre declaration */
struct awbl_ts_service;


/** \brief ts service info */
typedef struct awbl_ts_service_info {

    /** \brief ������������ʾ������.  */
    char     *name;

    /** \brief ������������ʾ������ID */
    int      vendor;

    /** \brief ������������ʾ����ƷID */
    int      product;

    /** \brief ֧�ִ�����ĸ���.    */
    int      multi_ts_cnt;

    /** \brief ��������, ��СֵΪ1  */
    int      sample_cnt;

    /** \brief �˲�����, ���ܴ��ڲ�������, ��СֵΪ1 */
    int      filter_cnt;

    /** \brief ����ʱ��(�������ò�����ѡ).     */
    uint16_t debounce_ms;

    /** \brief �������޷�ֵ(�������ò�����ѡ).  */
    uint16_t sample_threshold;

    /** \brief У׼���޷�ֵ(�������ò�����ѡ). */
    uint16_t calc_threshold;

    /**
     * \brief У׼�㷨(�������ò�����ѡ)
     *        AW_TS_LIB_TWO_POINT : ����У׼
     *        AW_TS_LIB_FIVE_POINT: ���У׼
     */
    uint8_t  calc_algorithm;

} awbl_ts_service_info_t;

/** \brief ts service driver function */
typedef struct awbl_ts_drv_funcs {

    /* ��ȡX Y�����꣬���ҷ����ж��ٸ��㰴�� */
    int (*pfn_xy_get) (struct awbl_ts_service *p_serv,
                       void                   *p_x_sample[],
                       void                   *p_y_sample[],
                       int                     max_slots,
                       int                     count);

    /* �����Ƿ����� */
    void (*pfn_lock_set) (struct awbl_ts_service *p_serv, aw_bool_t lock);

    /* �Ƿ��� */
    int (*pfn_is_touch)  (struct awbl_ts_service *p_serv);

    /* ׼������X������  */
    void (*pfn_active_x) (struct awbl_ts_service *p_serv);

    /* ׼������Y������  */
    void (*pfn_active_y) (struct awbl_ts_service *p_serv);

    /* ����X�����ֵ  */
    void (*pfn_measure_x) (struct awbl_ts_service *p_serv,
                           void                   *p_sample[],
                           int                     max_slots,
                           int                     count);

    /* ����Y�����ֵ  */
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
 * \brief ��ʼ��һ��ts����
 *
 * \param[in]  p_serv      :  ָ��ts�����ָ��
 * \param[in]  p_serv_info :  ָ��ts������Ϣ��ָ��
 *
 * \return AW_OK           : ��ʼ���ɹ�
 *         -AW_EINVAL      : ��������
 *         -AW_ENOTSUP     : ��ǰ�������õĶ��ش�����������ϵͳ֧�ֵ������
 *                           ���� AW_MULTI_TS_CNT
 *
 */
aw_err_t awbl_ts_service_init (struct awbl_ts_service      *p_serv,
                               struct awbl_ts_service_info *p_serv_info);

/**
 * \brief ts����ע�ắ��
 *
 * \param[in]  p_serv  : ָ��ts�����ָ��
 *
 * \return AW_OK       : ע��ɹ�
 *         -AW_EINVAL  : ��������
 */
aw_err_t awbl_ts_service_register (struct awbl_ts_service  *p_serv);

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

#ifdef __cplusplus
}
#endif


#endif /* AWBL_TS_H_ */

/* end of file */
