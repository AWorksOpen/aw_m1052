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
 * \brief �����㷨��׼�ӿ�
 *
 * ʹ�ñ��ӿ���Ҫ��������ͷ�ļ���
 * \code
 * #include "aw_ts_lib.h"
 * \endcode
 *
 *     ���ӿ���Դ�����ϵͳ�������Դ, Ϊ�������豸�ṩ���˲���
 *     ����У׼���㷨��ʵ��
 *
 * \par ��ʾ��
 * \code
 *
 * \endcode
 *
 * // �������ݴ���ӡ�����
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

/** \brief ��X�᷽�� */
#define AW_TS_LIB_COORD_X    0

/** \brief ��Y�᷽�� */
#define AW_TS_LIB_COORD_Y    1

/**
 * \brief �����
 */
typedef struct aw_ts_lib_point {
    int32_t x, y;
}aw_ts_lib_point_t;

/**
 * \brief ���У׼ϵ��
 */
typedef struct aw_ts_lib_factor {

    int64_t det;
    int64_t det_x1, det_x2, det_x3;
    int64_t det_y1, det_y2, det_y3;

}aw_ts_lib_factor_t;

/**
 * \brief ����У׼ϵ��
 */
typedef struct aw_ts_lib_minmax {
    int64_t min, max;
}aw_ts_lib_minmax_t;

/**
 * \brief ��ȡ����У׼���������,�������㼰���У׼
 */
typedef struct aw_ts_lib_calibration {
    aw_ts_lib_point_t  log[5];       /**< \brief ����������ֵ.      */
    aw_ts_lib_point_t  phy[5];       /**< \brief �����Ĳ���ֵ(����ֵ) */

    int32_t  cal_res_x;              /**< \brief У׼��x��ֱ���. */
    int32_t  cal_res_y;              /**< \brief У׼��y��ֱ���. */

}aw_ts_lib_calibration_t;

/*******************************************************************************
  �˲��㷨
*******************************************************************************/

/** \brief ����ֵ����ֵ�˲�
 *
 *  �㷨ԭ��
 *   (1) ��������NUM��, ����Сֵ��NUM�β�����������(ʹ��ð������, �γɽ�������).
 *   (2) ȡ�м���������, ���ֵ���ڷ�ֵ, ����. ��С�ڷ�ֵ, ��������������ľ�ֵ,
 *       ��Ϊ���β�������Чֵ.
 *
 *  \param   sample      ��������
 *           num         ��������
 *           threshold   ��ֵ
 *
 *  \return  �˲���Ĳ���ֵ
 *
 *  \error  -AW_EDOM  �м����������Ĳ�ֵ���ڷ�ֵ
 */
int aw_ts_lib_filter_median (uint16_t *sample, int num, int threshold);


/** \brief �����ڵľ�ֵ�˲�
 *
 *  �㷨ԭ��
 *   (1) ��������NUM��, ����Сֵ��NUM�β�����������(ʹ��ð������, �γɽ�������).
 *   (2) ȡ���ڳߴ�WIN_SIZE������, ���о�ֵ����, ��Ϊ���β�������Чֵ.
 *       (���ڳߴ�ָ���������м�����ɸ�����.)
 *
 *  \param   sample    ��������
 *           num       ��������
 *           win_size  ���ڳߴ�
 *
 *  \return  �˲���Ĳ���ֵ
 *
 *  \error -AW_EINVAL  ���ڳߴ������������, ���ڳߴ�С�ڵ�����
 */
int aw_ts_lib_filter_mean (uint16_t *sample, int num, int win_size);

/**********************************************************************
 *
 *   У׼�㷨
 *
 *   ʹ��Լ�� ���ȵ���У׼�ӿ�, ����ü���ӿ�
 *
 **********************************************************************/

/**
 * \brief ���㴥��У׼
 *
 * \param phy_minmax      X��Y����������С�Ĳ���ֵ
 * \param cal_xsize       X�����ϵķֱ���
 * \param cal_ysize       Y�����ϵķֱ���
 * \param cal             У׼����
 * \param threshold       ��֤��ֵ
 *
 * \return AW_OK     У׼���
 *
 * \error  -AW_EINVAL   �Ƿ���У׼����
 */
int aw_ts_lib_two_point_calibrate (aw_ts_lib_minmax_t      *phy_minmax,
                                   int                      cal_xsize,
                                   int                      cal_ysize,
                                   aw_ts_lib_calibration_t *cal,
                                   uint16_t                 threshold);

/**
 * \brief ���ݲ���ֵ��������ֵ (���øýӿ�ǰ, ���ȵ�������У׼�ӿ�)
 *
 * \param phy_minmax      X��Y����������С��A/Dת��ֵ
 * \param cal_xsize       X�����ϵķֱ���
 * \param cal_ysize       Y�����ϵķֱ���
 * \param phy_x           �ü�������x����Ĳ���ֵ
 * \param phy_y           �ü�������y����Ĳ���ֵ
 *
 * \return  ����У׼������ֵ
 */
aw_ts_lib_point_t aw_ts_lib_two_point_calc_coord (aw_ts_lib_minmax_t *p_phy_minmax,
                                                  int                 cal_xsize,
                                                  int                 cal_ysize,
                                                  int                 phy_x,
                                                  int                 phy_y);


/**
 * \brief ��㴥��У׼�㷨, ������linux TsLib
 *
 * \param p_factor    ָ��У׼ϵ����ָ��
 * \param pdata       У׼����
 * \param threshold   ��֤��ֵ
 *                    (���㷨������ɺ�, ������֤, 
 *                     ����ֵ��ԭֵ�Ĳ�ֵ��С����֤��ֵ, 
 *                     �㷨���ж�У׼�ɹ�.)
 *
 * \return AW_OK     У׼���
 *
 * \error  -AW_EDOM  ����ֵ��ԭֵ�Ĳ�ֵ������֤��ֵ
 */
int aw_ts_lib_five_point_calibrate (aw_ts_lib_factor_t      *p_factor,
                                    aw_ts_lib_calibration_t *cal,
                                    uint16_t                 threshold);


/**
 * \brief ���ݲ���ֵ��������ֵ (���øýӿ�ǰ, ���ȵ������У׼�ӿ�)
 *
 * \param p_factor    ָ��У׼ϵ����ָ��
 * \param phy_x       �ü������X���ϵĲ���ֵ
 * \param phy_y       �ü������Y���ϵĲ���ֵ
 *
 * \return log        ���ظõ������ֵ
 */
aw_ts_lib_point_t aw_ts_lib_five_point_calc_coord (aw_ts_lib_factor_t *p_factor,
                                                   int                 phy_x,
                                                   int                 phy_y);


#endif /* AW_TS_LIB_H_ */
