/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
**** ***************************************************************************/

/**
 * \file
 * \brief iMX RT10xx Quadrature Decoder �м�����
 *
 * \internal
 * \par modification history:
 * - 1.00 18-03-21  mex, first implementation
 * \endinternal
 */


#ifndef __AWBL_ENC_SERVICE_H
#define __AWBL_ENC_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbus_lite.h"
#include "aw_errno.h"

struct awbl_enc_service;

/**
 * \brief ENC ������
 */
typedef struct awbl_enc_drv_funcs {


    /* ��ʼ�������� */
    aw_err_t (*pfunc_enc_init) (struct awbl_enc_service *p_serv);

    /* ��ȡλ������ */
    aw_err_t (*pfunc_get_position) (struct awbl_enc_service *p_serv,
                                    int                     *p_val);

    /* ��ȡλ�ò����� */
    aw_err_t (*pfunc_get_position_difference) (struct awbl_enc_service *p_serv,
                                               int16_t                 *p_val);

    /* ��ȡ�ۼ���תȦ�� */
    aw_err_t (*pfunc_get_revolution) (struct awbl_enc_service *p_serv,
                                      int16_t                 *p_val);
} awbl_enc_drv_funcs_t;



/**
 * \brief ENC ����
 */
typedef struct awbl_enc_service {

    /** \brief ָ����һ��ENC ���� */
    struct awbl_enc_service  *p_next;

    /** \brief ��ǰͨ���� */
    int                       chan;

    /** \brief ������ */
    awbl_enc_drv_funcs_t     *p_servfuncs;


    /** \brief �û��жϻص����� */
    aw_pfuncvoid_t            p_func;


    /** \brief �û��жϻص��������� */
    void                     *p_arg;

} awbl_enc_service_t;


/* ENC�����ʼ�� */
aw_err_t awbl_enc_service_init (awbl_enc_service_t   *p_serv,
                                awbl_enc_drv_funcs_t *p_servfuncs,
                                int                   chan);

/* ENC����ע�� */
aw_err_t awbl_enc_service_register (awbl_enc_service_t *p_serv);



#ifdef __cplusplus
}
#endif

#endif /* __AWBL_ENC_SERVICE_H */


/* end of file */
