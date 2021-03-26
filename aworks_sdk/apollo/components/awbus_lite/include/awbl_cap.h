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
 * \brief Freescale i.MX283 cap����
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_cap.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-10-14  tee, first implementation
 * \endinternal
 */

#ifndef __AWBL_CAP_H
#define __AWBL_CAP_H

#include "aw_errno.h"
#include "aw_cap.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*cap_callback_t) (void *p_arg,unsigned int count); 
         
/**
 * \brief AWBus cap ����ʵ���������Ϣ
 */
struct awbl_cap_servinfo {

    /** \brief ֧�ֵ���Сcapͨ��id�� */
    int cap_min_id;

    /** \brief ֧�ֵ����capͨ��id�� */
    int cap_max_id;
};

/**
 * \brief AWBus cap ������
 */
struct awbl_cap_servopts {

    /** \brief ����cap��Ч�������غͻص����� */
    aw_err_t (*pfunc_cap_config) (void           *p_cookie,
                                  int             cap_id,
                                  unsigned int    flags,
                                  cap_callback_t  p_func_callback,
                                  void           *p_arg);

    /** \brief ʹ��cap */
    aw_err_t (*pfunc_cap_enable) (void *p_cookie,
                                  int   cap_id);

    /** \brief ����cap */
    aw_err_t (*pfunc_cap_disable) (void *p_cookie,
                                   int   cap_id);
    /** \brief ��λcap */
    aw_err_t (*pfunc_cap_reset) (void *p_cookie,
                                 int   cap_id);
    
    aw_err_t (*pfunc_cap_count_to_time)(void         *p_cookie,
                                        int           cap_id,
                                        unsigned int  count1,     /* ��һ�β���õ���ֵ */
                                        unsigned int  count2,     /* �ڶ��β���õ���ֵ */
                                        unsigned int *p_time_ns);
};

/**
 * \brief AWBus cap ����ʵ��
 */
struct awbl_cap_service {

    /** \brief ָ����һ�� cap ���� */
    struct awbl_cap_service *p_next;

    /** \brief CAP ����������Ϣ */
    const struct awbl_cap_servinfo *p_servinfo;

    /** \brief CAP �������غ��� */
    const struct awbl_cap_servopts *p_servfuncs;

    /**
     * \brief cap �����Զ��������
     *
     * �˲�����cap�ӿ�ģ����������ʱ���������ã��ڵ��÷�����ʱ��Ϊ��һ��������
     */
    void *p_cookie;
};

/**
 * \brief AWBus cap �ӿ�ģ���ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_init2() ֮�����
 */
void awbl_cap_init(void);


#ifdef __cplusplus
}
#endif

#endif
