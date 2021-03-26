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
 * \brief ���Ź���׼�ӿ�
 *
 * ��ģ���ṩӲ����ʱ������
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_wdt.h
 *
 * �������ṩ�˶�������Ź���ӡ�ι���Ľӿڣ����ϵͳδ�ڹ涨ʱ����
 * ����ι������ô�ͻᵼ��ϵͳ��λ��
 *
 * ע�⣺��ϵͳ�й涨��Ӳ�����Ź���ʱ��������1ms��
 *
 * \par ��ʾ��
 * \code
 *
 * // ���ÿ��Ź�ʱ��Ϊ1200ms
 * struct awbl_wdt   wdt_dev;
 * aw_wdt_add(&wdt_dev, 1200);
 *
 * // ι��
 * aw_wdt_feed(&wdt_dev);
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-08-27  may, first implementation
 * \endinternal
 */


#ifndef __AW_WDT_H
#define __AW_WDT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "aw_timer.h"

/**
 * \brief ���Ź��豸
 */
struct awbl_wdt {

    /** \brief ���Ź�ʱ�� */
    uint32_t   period_ms;

    /** \brief ���ڿ��Ź���ʱ�������ʱ�� */
    aw_timer_t sftimer;
};

/**
 * \addtogroup grp_aw_if_watchdog
 * \copydoc aw_wdt.h
 * @{
 */

/**
 * \brief ��ӿ��Ź��������ÿ��Ź�ʱ��
 *
 * \param[in] p_wdt   ���Ź��豸
 * \param[in] t_ms    ���Ź�ʱ��(ms)
 *
 * \retval   AW_OK       �ɹ�
 * \retval  -AW_EINVAL   \e p_wdt ֵ��Ч
 * \retval  -AW_EPERM    ����������
 */
aw_err_t  aw_wdt_add (struct awbl_wdt  *p_wdt, uint32_t  t_ms);

/**
 * \brief ִ��ι������
 *
 * \param[in] p_wdt    ���Ź��豸
 *
 * \retval  AW_OK        �ɹ�
 * \retval  -AW_EINVAL   \e p_wdt ֵ��Ч
 * \retval  -AW_EPERM    ����������
 */
aw_err_t  aw_wdt_feed (struct awbl_wdt  *p_wdt);

/** @} grp_aw_if_watchdog */

#ifdef __cplusplus
}
#endif

#endif /* __AW_WDT_H */

/* end of file */
