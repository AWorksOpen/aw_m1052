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
 * \brief iMX RT10xx Quadrature Decoder 中间服务层
 *
 * \internal
 * \par modification history:
 * - 1.00 18-03-21  mex, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "awbus_lite.h"
#include "driver/enc/awbl_enc_service.h"
#include "driver/enc/awbl_enc.h"

static awbl_enc_service_t *__gp_enc_serv_head = NULL;


/******************************************************************************/

aw_err_t awbl_enc_service_register (awbl_enc_service_t *p_serv)
{
    if (NULL == p_serv->p_servfuncs) {
        return -AW_ENXIO;
    }

    p_serv->p_next = __gp_enc_serv_head;
    __gp_enc_serv_head = p_serv;
    return AW_OK;
}


/******************************************************************************/

aw_local awbl_enc_service_t * __enc_chan_to_serv (int chan)
{
    awbl_enc_service_t *p_serv_cur = __gp_enc_serv_head;

    while ((p_serv_cur != NULL)) {

        if (chan == p_serv_cur->chan)  {
            return p_serv_cur;
        }
        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}


/******************************************************************************/

aw_err_t awbl_enc_service_init (awbl_enc_service_t   *p_serv,
                                awbl_enc_drv_funcs_t *p_servfuncs,
                                int                   chan)
{

    p_serv->p_servfuncs = p_servfuncs;
    p_serv->chan = chan;

    return AW_OK;
}

/******************************************************************************/

aw_err_t  awbl_enc_init(uint32_t         chan,
                        aw_pfuncvoid_t   p_func,
                        void            *p_arg)
{
    awbl_enc_service_t *p_serv = __enc_chan_to_serv(chan);

    if (p_serv == NULL) {
        return -AW_EINVAL;
    }

    if (p_serv->p_servfuncs->pfunc_enc_init != NULL) {

        p_serv->p_arg = p_arg;
        p_serv->p_func = p_func;
        return p_serv->p_servfuncs->pfunc_enc_init(p_serv);
    }

    return AW_ERROR;
}


/******************************************************************************/

aw_err_t  awbl_enc_get_position(uint32_t   chan,
                                int       *p_val)
{
    awbl_enc_service_t *p_serv = __enc_chan_to_serv(chan);

    if (p_serv == NULL || p_val == NULL) {
        return -AW_EINVAL;
    }

    if (p_serv->p_servfuncs->pfunc_get_position != NULL) {
        return p_serv->p_servfuncs->pfunc_get_position(p_serv, p_val);
    }

    return AW_ERROR;
}


/******************************************************************************/

aw_err_t  awbl_enc_get_position_difference(uint32_t   chan,
                                           int16_t   *p_val)
{
    awbl_enc_service_t *p_serv = __enc_chan_to_serv(chan);

    if (p_serv == NULL || p_val == NULL) {
        return -AW_EINVAL;
    }

    if (p_serv->p_servfuncs->pfunc_get_position_difference != NULL) {
        return p_serv->p_servfuncs->pfunc_get_position_difference(p_serv, p_val);
    }

    return AW_ERROR;
}

/******************************************************************************/

aw_err_t  awbl_enc_get_revolution(uint32_t   chan,
                                  int16_t   *p_val)
{
    awbl_enc_service_t *p_serv = __enc_chan_to_serv(chan);

    if (p_serv == NULL || p_val == NULL) {
        return -AW_EINVAL;
    }

    if (p_serv->p_servfuncs->pfunc_get_revolution != NULL) {
        return p_serv->p_servfuncs->pfunc_get_revolution(p_serv, p_val);
    }

    return AW_ERROR;
}

/* end of file */
