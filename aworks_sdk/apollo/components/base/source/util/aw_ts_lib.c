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
 * ���ӿ�Ϊ�������豸�ṩ���˲�������ת�����㷨��ʵ��
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
 * - 1.00 14-09-12  ops, first implementation.
 * \endinternal
 */

#include "aw_ts_lib.h"
#include "aw_types.h"
#include "aw_common.h"
#include "aw_vdebug.h"

#include <stdlib.h>


/*******************************************************************************

   У׼�㷨

   1. aw_ts_lib_two_point_calibrate   - ����У׼
   2. aw_ts_lib_five_point_calibrate  - ���У׼

   �˲��㷨

   1. aw_ts_lib_filter_median  �� ����ֵ����ֵ�˲�
   2. aw_ts_lib_filter_mean    �� �����ڵľ�ֵ�˲�

*******************************************************************************/


/*******************************************************************************
  ����У׼�㷨
******************************************************************************/

int aw_ts_lib_two_point_calibrate (aw_ts_lib_minmax_t      *phy_minmax,
                                   int                      cal_xsize,
                                   int                      cal_ysize,
                                   aw_ts_lib_calibration_t *cal,
                                   uint16_t                 threshold)
{
    int i = 0;
    int log_origin      = 0;                 /* ����ԭ��   */

    int lcd_x_boundary   = cal_xsize - 1;    /* ����߽�ֵ */

    int lcd_y_boundary   = cal_ysize - 1;    /* ����߽�ֵ */

    aw_ts_lib_point_t auth_point = {0};

    int temp = 0;

    aw_ts_lib_minmax_t xy_minmax[2] = {0};

    xy_minmax[0].min = cal->phy[0].x + ((cal->phy[0].y - cal->phy[0].x) * \
                                (log_origin - cal->log[0].x)) / \
                                (cal->log[0].y - cal->log[0].x);
    xy_minmax[0].max = cal->phy[0].x + ((cal->phy[0].y - cal->phy[0].x) * \
                                (lcd_x_boundary - cal->log[0].x)) / \
                                (cal->log[0].y - cal->log[0].x);


    xy_minmax[1].min = cal->phy[1].x + ((cal->phy[1].y - cal->phy[1].x) * \
                                (log_origin - cal->log[1].x)) / \
                                (cal->log[1].y - cal->log[1].x);
    xy_minmax[1].max = cal->phy[1].x + ((cal->phy[1].y - cal->phy[1].x) * \
                                (lcd_y_boundary - cal->log[1].x)) / \
                                (cal->log[1].y - cal->log[1].x);

    for (i = 0; i < 2; i++) {

        if (xy_minmax[i].min > xy_minmax[i].max) {
            /* �������� */
            temp = xy_minmax[i].max;
            xy_minmax[i].max = xy_minmax[i].min;
            xy_minmax[i].min = temp;

        }
    }

    auth_point.x = (abs(cal->phy[0].x - xy_minmax[0].min) * cal_xsize) / \
                    (abs(xy_minmax[0].max - xy_minmax[0].min));
    auth_point.y = (abs(cal->phy[0].y - xy_minmax[1].min) * cal_ysize) / \
                    (abs(xy_minmax[1].max - xy_minmax[1].min));

    if ((abs(auth_point.x - cal->log[0].x) > threshold) && \
        (abs(auth_point.y - cal->log[0].y) > threshold)) {

         AW_INFOF(("two point calibration failed, the calculation error is larger than AUTH_THRESHOLD. \n"));

         return -AW_EDOM;

     } else {

         for (i = 0; i < 2; i++) {

             phy_minmax[i].min = xy_minmax[i].min;
             phy_minmax[i].max = xy_minmax[i].max;
         }

         /* У׼�ɹ� */
         AW_INFOF(("two point calibration success. \n"));

         return AW_OK;

     }

    return AW_OK;
}


aw_ts_lib_point_t aw_ts_lib_two_point_calc_coord (aw_ts_lib_minmax_t *p_phy_minmax,
                                                  int                 cal_xsize,
                                                  int                 cal_ysize,
                                                  int                 phy_x,
                                                  int                 phy_y)
{
    aw_ts_lib_point_t log = {0, 0};


    log.x = (abs(phy_x - p_phy_minmax[0].min) * cal_xsize) / \
            (abs(p_phy_minmax[0].max - p_phy_minmax[0].min));
    log.y = (abs(phy_y - p_phy_minmax[1].min) * cal_ysize) / \
            (abs(p_phy_minmax[1].max - p_phy_minmax[1].min));

    return log;
}



/*******************************************************************************
  ���У׼�㷨 
*******************************************************************************/
int aw_ts_lib_five_point_calibrate (aw_ts_lib_factor_t      *p_factor,
                                    aw_ts_lib_calibration_t *cal,
                                    uint16_t                 threshold)
{
    int j;
    float n, x, y, x2, y2, xy, z, zx, zy;
    float det, a, b, c, e, f, i;
    float scaling = 65536.0f;

    aw_ts_lib_factor_t factor_tmp;

    aw_ts_lib_point_t auth_point1, auth_point2, auth_point3;

    /* Get sums for matrix */
    n = x = y = x2 = y2 = xy = 0;
    for (j = 0; j < 5; j++) {
        n += 1.0;
        x += (float)cal->phy[j].x;
        y += (float)cal->phy[j].y;
        x2 += (float)(cal->phy[j].x * cal->phy[j].x);
        y2 += (float)(cal->phy[j].y * cal->phy[j].y);
        xy += (float)(cal->phy[j].x * cal->phy[j].y);
    }

    /* Get determinant of matrix -- check if determinant is too small */
    det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
    if (det < 0.1 && det > -0.1) {
        aw_kprintf("ts_calibrate: determinant is too small, Calibration failed -- %f\n", det);
        return AW_ERROR;
    }

    /* Get elements of inverse matrix */
    a = (x2*y2 - xy*xy)/det;
    b = (xy*y - x*y2)/det;
    c = (x*xy - y*x2)/det;
    e = (n*y2 - y*y)/det;
    f = (x*y - n*xy)/det;
    i = (n*x2 - x*x)/det;

    /* Get sums for x calibration */
    z = zx = zy = 0;
    for (j = 0; j < 5; j++) {
        z += (float)cal->log[j].x;
        zx += (float)(cal->log[j].x * cal->phy[j].x);
        zy += (float)(cal->log[j].x * cal->phy[j].y);
    }

    /* Now multiply2010 out to get the calibration for framebuffer x coord */

    factor_tmp.det_x3 = (int64_t)((a*z + b*zx + c*zy)*(scaling));
    factor_tmp.det_x1 = (int64_t)((b*z + e*zx + f*zy)*(scaling));
    factor_tmp.det_x2 = (int64_t)((c*z + f*zx + i*zy)*(scaling));


    /* Get sums for y calibration */
    z = zx = zy = 0;
    for (j = 0; j < 5; j++) {
        z += (float)cal->log[j].y;
        zx += (float)(cal->log[j].y * cal->phy[j].x);
        zy += (float)(cal->log[j].y * cal->phy[j].y);
    }

    /* Now multiply out to get the calibration for framebuffer y coord */
    factor_tmp.det_y3 = (int64_t)((a*z + b*zx + c*zy)*(scaling));
    factor_tmp.det_y1 = (int64_t)((b*z + e*zx + f*zy)*(scaling));
    factor_tmp.det_y2 = (int64_t)((c*z + f*zx + i*zy)*(scaling));


    /* If we got here, we're OK, so assign scaling to a[6] and return */
    factor_tmp.det    = (int64_t)scaling;

    /* ʹ����������֤У׼ϵ�� */
    auth_point1.x = ((int64_t)cal->phy[3].x * \
                    factor_tmp.det_x1 + cal->phy[3].y * \
                    factor_tmp.det_x2 + factor_tmp.det_x3) / factor_tmp.det;
    auth_point1.y = ((int64_t)cal->phy[3].x * \
                    factor_tmp.det_y1 + cal->phy[3].y * \
                    factor_tmp.det_y2 + factor_tmp.det_y3) / factor_tmp.det;

    auth_point2.x = ((int64_t)cal->phy[4].x * \
                    factor_tmp.det_x1 + cal->phy[4].y * \
                    factor_tmp.det_x2 + factor_tmp.det_x3) / factor_tmp.det;
    auth_point2.y = ((int64_t)cal->phy[4].x * \
                    factor_tmp.det_y1 + cal->phy[4].y * \
                    factor_tmp.det_y2 + factor_tmp.det_y3) / factor_tmp.det;

    auth_point3.x = ((int64_t)cal->phy[1].x * \
                    factor_tmp.det_x1 + cal->phy[1].y * \
                    factor_tmp.det_x2 + factor_tmp.det_x3) / factor_tmp.det;
    auth_point3.y = ((int64_t)cal->phy[1].x * \
                    factor_tmp.det_y1 + cal->phy[1].y * \
                    factor_tmp.det_y2 + factor_tmp.det_y3) / factor_tmp.det;

    if ((abs(auth_point1.x - cal->log[3].x) > \
            threshold && abs(auth_point1.y - cal->log[3].y) > threshold) ||
           (abs(auth_point2.x - cal->log[4].x) > threshold && \
            abs(auth_point2.y - cal->log[4].y) > threshold) ||
           (abs(auth_point3.x - cal->log[1].x) > threshold && \
            abs(auth_point3.y - cal->log[1].y) > threshold)) {

        AW_INFOF(("Five point calibration failed, the calculation error is larger than AUTH_THRESHOLD. \n"));

        return -AW_EDOM;

    } else {

        p_factor->det      =  factor_tmp.det;
        p_factor->det_x1   =  factor_tmp.det_x1;
        p_factor->det_x2   =  factor_tmp.det_x2;
        p_factor->det_x3   =  factor_tmp.det_x3;
        p_factor->det_y1   =  factor_tmp.det_y1;
        p_factor->det_y2   =  factor_tmp.det_y2;
        p_factor->det_y3   =  factor_tmp.det_y3;

        /* У׼�ɹ� */
        AW_INFOF(("Five point calibration success. \n"));

        return AW_OK;

    }
}


aw_ts_lib_point_t aw_ts_lib_five_point_calc_coord (aw_ts_lib_factor_t *p_factor, int phy_x, int phy_y)
{
    aw_ts_lib_point_t log = {0, 0};

    log.x = (p_factor->det_x3 + p_factor->det_x1 * \
            (int64_t)phy_x + p_factor->det_x2 * phy_y) / p_factor->det;
    log.y = (p_factor->det_y3 + p_factor->det_y1 * \
            (int64_t)phy_x + p_factor->det_y2 * phy_y) / p_factor->det;

    return log;
}



/*******************************************************************************************/

/**
 * \brief ����ֵ����ֵ�˲�
 */
int aw_ts_lib_filter_median (uint16_t *sample, int num, int threshold)
{
    int result = 0, i = 0, j = 0;
    int min = 0, tmp;

    for (i = 0; i < num - 1; i++) {

        min = i;

        for (j = i + 1; j < num; j++) {

            if (sample[i] > sample[j]) {

                min = j;
            }
        }

        tmp = sample[i];
        sample[i] = sample[min];
        sample[min] = tmp;

    }

    if (abs(sample[(num / 2) + 1] - sample[(num / 2)]) > threshold) {
        return -AW_EDOM;
    }

    result = (sample[(num / 2) + 1] + sample[(num / 2)]) / 2;

    return result;
}

/**
 * \brief �����ڵľ�ֵ�˲�
 */
int aw_ts_lib_filter_mean (uint16_t *sample, int num, int win_size)
{
    int result = 0, i = 0, j = 0;
    int min = 0, tmp;

    /* ���ڳߴ粻�ô����������� , ��������� */
    if (win_size > num && win_size < 0 && win_size == 0) {

        return -AW_EINVAL;
    }

    for (i = 0; i < num - 1; i++) {

        min = i;

        for (j = i + 1; j < num; j++) {

            if (sample[i] > sample[j]) {

                min = j;

            }
        }

        /* ��С�������� */
        tmp = sample[i];
        sample[i] = sample[min];
        sample[min] = tmp;
    }

    /* ȡ�м�win_size������, ȥ��������С��num - win_size������ */
    for (i = (num - win_size) / 2; i < ((num - win_size) / 2 + win_size); i++) {

        result += sample[i];
    }

    result = result / (win_size);

    return result;
}
/* end of file*/

