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
 * \brief AWBus �жϿ������ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_intcltr.h"
 * \endcode
 *
 * ��ģ����� AWBus �ϵ������жϿ�������
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-06  zen, first implemetation
 * \endinternal
 */

#ifndef __AWBL_INTCTLR_H
#define __AWBL_INTCTLR_H

#include "aw_int.h"

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/**
 * \brief AWBus intctlr �����������Ϣ
 */
struct awbl_intctlr_servinfo {

    /** \brief ֧�ֵ���ʼ�жϺ� */
    int inum_start;

    /** \brief ֧�ֵĽ����жϺ� */
    int inum_end;
};

/** \brief �ж����ӷ��������� */
typedef aw_err_t (*pfunc_int_connect_t)(
        void *p_cookie,
        int inum,
        aw_pfuncvoid_t pfunc_isr,
        void *p_arg);

/** \brief �ж϶Ͽ����ӷ��������� */
typedef aw_err_t (*pfunc_int_disconnect_t)(
        void *p_cookie,
        int inum,
        aw_pfuncvoid_t pfunc_isr,
        void *p_arg);

/** \brief �ж�ʹ�ܷ��������� */
typedef aw_err_t (*pfunc_int_enable_t)(
        void *p_cookie,
        int inum);

/** \brief �ж�ʹ�ܷ��������Ͷ��� */
typedef aw_err_t (*pfunc_int_disable_t)(
        void *p_cookie,
        int inum);

/** \brief �ж��������ӷ��������� */
typedef aw_err_t (*pfunc_int_vector_connect_t)(
        void *p_cookie,
        int inum,
        pfn_aw_int_vector_func_t pfunc_vector_isr);

typedef aw_err_t (*pfunc_int_vector_disconnect_t)(
        void *p_cookie,
        int inum,
        pfn_aw_int_vector_func_t pfunc_vector_isr);



typedef aw_err_t (*pfunc_int_priority_get_t)(
        void *p_cookie,
        int inum,
        int *p_cur_prio);

typedef aw_err_t (*pfunc_int_priority_range_get_t)(
        void *p_cookie,
        int inum,
        int *p_min_prio,int *p_max_prio);

typedef aw_err_t (*pfunc_int_priority_set_t)(
        void *p_cookie,
        int inum,
        int new_prio);

typedef aw_err_t (*pfunc_int_pend_t)(
        void *p_cookie,
        int inum);

/**
 * \brief AWBus intctlr ������
 */
struct awbl_intctlr_servfuncs {
    pfunc_int_connect_t             pfunc_int_connect;
    pfunc_int_disconnect_t          pfunc_int_disconnect;
    pfunc_int_enable_t              pfunc_int_enable;
    pfunc_int_disable_t             pfunc_int_disable;

    pfunc_int_vector_connect_t      pfunc_int_vector_connect;
    pfunc_int_vector_disconnect_t   pfunc_int_vector_disconnect;
    pfunc_int_priority_get_t        pfunc_int_prio_get;
    pfunc_int_priority_range_get_t  pfunc_int_prio_range_get;
    pfunc_int_priority_set_t        pfunc_int_prio_set;
    pfunc_int_pend_t                pfunc_int_pend;
};




/**
 * \brief AWBus �жϿ���������
 * �˷���ṹ��ע�ᵽ�жϿ������м�����������ͨ��
 * �м�㺯�����޸Ĵ˽ṹ�壬�������޸Ĵ˽ṹ��
 */
struct awbl_intctlr_service {

    /** \brief ָ����һ�� intctlr ���� */
    struct awbl_intctlr_service *p_next;

    /** \brief intctlr �����������Ϣ */
    const struct awbl_intctlr_servinfo *p_servinfo;

    /** \brief intctlr ����ķ����� */
    const struct awbl_intctlr_servfuncs *p_servfuncs;

    /**
     * \brief intctlr �����Զ��������
     *
     * �˲����� ע���жϿ�����ǰ���������ã�
     * �ڵ��÷�����ʱ��Ϊ��һ��������
     */
    void *p_cookie;
     /**
     * \brief �����˱����������м��ʹ�ã�
     * �˱�������Ҫ�������������豸���Ƴ�ʱ�������������м��ĳ�ͻ
     * ���ܴ�������һ�ֵ��͵��������Ӧ������ͨ���м������豸�Ĺ��ܣ�
     * �պ�ִ�е����ҵ��������������������豸������ʱ��ͻȻ�������ȼ�
     * ������Ҫ���Ƴ��豸�������ʱ���Ƴ��豸�����´ε��ȵ��м�������
     * ��������ʱ��ᷢ�����ϲ���������

     * �������Ƴ�������֮ǰ��Ӧ�õ���awbl_unregister_interrupt_controller��
     * �����ڲ�����ctlr_used_by_awbl��Ϊ0�������CPUһ��ʱ�䣬�ٴμ�飬
     * ֱ��ctlr_used_by_awblΪ0��Ȼ��ע���жϿ�������
     *
     * �豸�Ƴ������ڳɹ�����awbl_unregister_interrupt_controller֮ǰ��
     * ���ܻ���豸��������������������Ϊ�м������豸����Ҳ�����豸��
     */
    int             ctlr_used_by_awbl;
};

/**
 * \brief AWBus �жϿ����������ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_init1() ֮���� awbl_dev_init2() ֮ǰ����
 */
void awbl_intctlr_init(void);

/**
 * \brief ���ж���ϵͳע��һ���ն˿�����
 *
 * \param[in]   p_intctlr   Ҫע����жϿ�����
 *
 * \retval      AW_OK       �ɹ�
 * \retval      ����ֵ      ʧ�ܣ��Ǹ���AW������
 */
aw_err_t awbl_register_interrupt_controller(
    struct awbl_intctlr_service *p_intctlr);

/**
 * \brief ���ж���ϵͳ��ע��һ���ն˿�����
 *
 * \param[in]   p_intctlr   Ҫ��ע����жϿ�����
 *
 * \retval      AW_OK       �ɹ�
 * \retval      ����ֵ      ʧ�ܣ��Ǹ���AW������
 */
aw_err_t awbl_unregister_interrupt_controller(
    struct awbl_intctlr_service *p_intctlr);

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AWBL_INTCTLR_H */

/* end of file */
