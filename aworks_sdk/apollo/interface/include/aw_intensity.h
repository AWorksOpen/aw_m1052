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
 * \brief INTENSITY (ǿ�ȵ���) ��׼����
 *
 * \brief ΪLCD��LED��BUZZER���豸�ṩǿ�ȵ��ڷ���
 *
 * ʹ�ñ�������Ҫ��������ͷ�ļ�:
 * \code
 * #include "aw_intensity.h"
 * \endcode
 *
 * \par ʹ��ʾ��
 * \code
 *
 * ��ʹ��intensity��׼�ӿ�ǰ, ����þ���intensity�豸�����Ĺ��캯����
 * ���磬��HW480272��������У�ʹ�û���PWM��intensity�豸��������
 *
 * aw_local aw_err_t __panel_pan_backlight (awbl_mxs_panel_t *p_panel, 
 *                                          int               pwm_num,
 *                                          int               percent)
 * {
 *  ......
 *  aw_it_dev_t *p_it = NULL;
 *
 *  p_it = awbl_pwm_it_ctor(&p_this->pwm_it, 100, pwm_num, 10000000);
 *  aw_it_level_set(p_it, 80);
 *  ......
 * }
 *
 * \endcode
 *
 * \par ʹ��˵��
 *
 * \internal
 * \par modification history
 * - 1.01 14-11-01  ops, redefine the intensity device by using new specification.
 * - 1.00 14-07-03  ops, first implementation
 * \endinternal
 */

#ifndef __AW_INTENSITY_H
#define __AW_INTENSITY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_intensity
 * \copydoc aw_intensity.h
 * @{
 */
#include "apollo.h"

/**
 * \brief abstract intensity device
 */
typedef struct aw_it_dev {

    /** \brief ��ȡǿ�ȵȼ� */
    int (*pfn_it_level_get)(struct aw_it_dev *p_dev);

    /** \brief ����ǿ�ȵȼ� */
    int (*pfn_it_level_set)(struct aw_it_dev *p_dev, int level);

    /** \brief ��ǰǿ�� */
    int current_level;

    /** \brief �ȼ���Χ (0, max_level) */
    int max_level;

}aw_it_dev_t;

/**
 * \brief ��ȡ�豸���ǿ��ֵ
 */
aw_static_inline int aw_it_max_level_get(aw_it_dev_t *p_dev)
{
    if (NULL == p_dev) {
        return -AW_EINVAL;
    }

    return p_dev->max_level;
}

/**
 * \brief ����ǿ��ֵ
 *
 * \param level   ǿ�ȵȼ�
 *
 * \return error  level�����豸�ȼ���Χ
 */
aw_err_t aw_it_level_set(aw_it_dev_t *p_dev, int level);

/**
 * \brief ��ȡ��ǰǿ��ֵ
 *
 * \return ��ǰǿ��ֵ
 */
int aw_it_level_get(aw_it_dev_t *p_dev);

/** @}  grp_aw_if_intensity */

#ifdef __cplusplus
}
#endif  /* __cplusplus 	*/

#endif /* __AW_INTENSITY_H */

/* end of file */
