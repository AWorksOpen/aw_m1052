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
 * \brief AWBus RTC �ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_rtc.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 13-09-05  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_RTC_H
#define __AWBL_RTC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_rtc.h"

/**
 * \brief AWBus RTC ����ʵ���������Ϣ
 */
struct awbl_rtc_servinfo {

    /** \brief RTC �豸��� */
    int rtc_id;
};

/**
 * \brief AWBus RTC ������
 */
struct awbl_rtc_servopts {

    /** \brief ��ȡ RTC ʱ�� */
    aw_err_t (*time_get)(void *p_cookie, aw_tm_t *p_tm);

    /** \brief ���� RTC ʱ�� */
    aw_err_t (*time_set)(void *p_cookie, aw_tm_t *p_tm);

    /** \brief RTC �豸����(����) */
    aw_err_t (*dev_ctrl)(void *p_cookie, int req, void *arg);

    /** \brief ��ȡ RTC ����ʱ�� */
    aw_err_t (*timespec_get)(void *p_cookie, aw_timespec_t *p_tv);

    /** \brief ���� RTC ����ʱ�� */
    aw_err_t (*timespec_set)(void *p_cookie, aw_timespec_t *p_tv);
};

/**
 * \brief AWBus RTC ����ʵ��
 */
struct awbl_rtc_service {

    /** \brief ָ����һ�� RTC ���� */
    struct awbl_rtc_service *p_next;

    /** \brief RTC ����������Ϣ */
    const struct awbl_rtc_servinfo *p_servinfo;

    /** \brief RTC �������غ��� */
    const struct awbl_rtc_servopts *p_servopts;

    /**
     * \brief RTC �����Զ��������
     *
     * �˲�����RTC�ӿ�ģ����������ʱ���������ã��ڵ��÷�����ʱ��Ϊ��һ��������
     */
    void *p_cookie;
};

/**
 * \brief AWBus RTC �ӿ�ģ���ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_init2() ֮�����
 */
void awbl_rtc_init(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus 	*/

#endif  /* __AWBL_RTC_H */

/* end of file */

