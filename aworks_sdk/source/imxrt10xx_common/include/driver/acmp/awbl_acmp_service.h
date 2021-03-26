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
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx Analog Comparator �м�����
 *
 * \internal
 * \par modification history:
 * - 1.00 18-04-12  mex, first implementation
 * \endinternal
 */


#ifndef __AWBL_ACMP_SERVICE_H
#define __AWBL_ACMP_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbus_lite.h"
#include "aw_errno.h"

struct awbl_acmp_service;

/**
 * \brief ACMP ������
 */
typedef struct awbl_acmp_drv_funcs {

    /* ��ʼ��ACMP */
    aw_err_t (*pfunc_init) (struct awbl_acmp_service *p_serv);

    /* ����DAC */
    aw_err_t (*pfunc_config_dac) (struct awbl_acmp_service *p_serv,
                                  uint8_t                   vin,
                                  uint16_t                  vin_mv,
                                  uint16_t                  vref_mv);
    /* ���ñȽ�ͨ�� */
    aw_err_t (*pfunc_set_input_chan) (struct awbl_acmp_service *p_serv,
                                      uint32_t                  positive_chan,
                                      uint16_t                  negative_chan);

} awbl_acmp_drv_funcs_t;



/**
 * \brief ACMP ����
 */
typedef struct awbl_acmp_service {

    /** \brief ָ����һ��ACMP ���� */
    struct awbl_acmp_service  *p_next;

    /** \brief ��ǰ��ACMP ID */
    uint8_t                   acmp_id;

    /** \brief ������ */
    awbl_acmp_drv_funcs_t     *p_servfuncs;

    /** \brief �û��жϻص����� */
    aw_pfuncvoid_t             p_func;

} awbl_acmp_service_t;


/* ACMP�����ʼ�� */
aw_err_t awbl_acmp_service_init (awbl_acmp_service_t   *p_serv,
                                 awbl_acmp_drv_funcs_t *p_servfuncs,
                                 int                   chan);

/* ACMP����ע�� */
aw_err_t awbl_acmp_service_register (awbl_acmp_service_t *p_serv);



#ifdef __cplusplus
}
#endif

#endif /* __AWBL_ACMP_SERVICE_H */


/* end of file */
