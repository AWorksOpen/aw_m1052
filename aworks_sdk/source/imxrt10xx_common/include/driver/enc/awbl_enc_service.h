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
 * \brief iMX RT10xx Quadrature Decoder 中间服务层
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
 * \brief ENC 服务函数
 */
typedef struct awbl_enc_drv_funcs {


    /* 初始化编码器 */
    aw_err_t (*pfunc_enc_init) (struct awbl_enc_service *p_serv);

    /* 读取位置数据 */
    aw_err_t (*pfunc_get_position) (struct awbl_enc_service *p_serv,
                                    int                     *p_val);

    /* 读取位置差数据 */
    aw_err_t (*pfunc_get_position_difference) (struct awbl_enc_service *p_serv,
                                               int16_t                 *p_val);

    /* 读取累计旋转圈数 */
    aw_err_t (*pfunc_get_revolution) (struct awbl_enc_service *p_serv,
                                      int16_t                 *p_val);
} awbl_enc_drv_funcs_t;



/**
 * \brief ENC 服务
 */
typedef struct awbl_enc_service {

    /** \brief 指向下一个ENC 服务 */
    struct awbl_enc_service  *p_next;

    /** \brief 当前通道号 */
    int                       chan;

    /** \brief 服务函数 */
    awbl_enc_drv_funcs_t     *p_servfuncs;


    /** \brief 用户中断回掉函数 */
    aw_pfuncvoid_t            p_func;


    /** \brief 用户中断回掉函数参数 */
    void                     *p_arg;

} awbl_enc_service_t;


/* ENC服务初始化 */
aw_err_t awbl_enc_service_init (awbl_enc_service_t   *p_serv,
                                awbl_enc_drv_funcs_t *p_servfuncs,
                                int                   chan);

/* ENC服务注册 */
aw_err_t awbl_enc_service_register (awbl_enc_service_t *p_serv);



#ifdef __cplusplus
}
#endif

#endif /* __AWBL_ENC_SERVICE_H */


/* end of file */
