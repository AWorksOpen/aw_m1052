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
 * \brief iMX RT10xx Analog Comparator 中间服务层
 *
 * \internal
 * \par modification history:
 * - 1.00 18-04-12  mex, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "awbus_lite.h"
#include "driver/acmp/awbl_acmp_service.h"
#include "driver/acmp/awbl_acmp.h"

static awbl_acmp_service_t *__gp_acmp_serv_head = NULL;


/******************************************************************************/

aw_err_t awbl_acmp_service_register (awbl_acmp_service_t *p_serv)
{
    if (NULL == p_serv->p_servfuncs) {
        return -AW_ENXIO;
    }

    p_serv->p_next = __gp_acmp_serv_head;
    __gp_acmp_serv_head = p_serv;
    return AW_OK;
}


/******************************************************************************/

aw_local awbl_acmp_service_t * __acmp_chan_to_serv (int acmp_id)
{
    awbl_acmp_service_t *p_serv_cur = __gp_acmp_serv_head;

    while ((p_serv_cur != NULL)) {

        if (acmp_id == p_serv_cur->acmp_id)  {
            return p_serv_cur;
        }
        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}


/******************************************************************************/

aw_err_t awbl_acmp_service_init (awbl_acmp_service_t   *p_serv,
                                 awbl_acmp_drv_funcs_t *p_servfuncs,
                                 int                    acmp_id)
{

    p_serv->p_servfuncs = p_servfuncs;
    p_serv->acmp_id = acmp_id;

    return AW_OK;
}


/******************************************************************************/

aw_err_t  awbl_acmp_init(uint8_t          acmp_id,
                         aw_pfuncvoid_t   p_func)
{
    awbl_acmp_service_t *p_serv = __acmp_chan_to_serv(acmp_id);

    if (p_serv == NULL) {
        return -AW_EINVAL;
    }

    if (p_serv->p_servfuncs->pfunc_init != NULL) {

        p_serv->p_func = p_func;
        return p_serv->p_servfuncs->pfunc_init(p_serv);
    }

    return AW_ERROR;
}

/******************************************************************************/

aw_err_t awbl_acmp_config_dac (uint8_t     acmp_id,
                               uint8_t     vin,
                               uint16_t    vin_mv,
                               uint16_t    vref_mv)
{

    awbl_acmp_service_t *p_serv = __acmp_chan_to_serv(acmp_id);

    if (p_serv == NULL) {
        return -AW_EINVAL;
    }

    if (p_serv->p_servfuncs->pfunc_config_dac != NULL) {
        return p_serv->p_servfuncs->pfunc_config_dac(p_serv, vin, vin_mv, vref_mv);
    }

    return AW_OK;
}


/******************************************************************************/

aw_err_t awbl_acmp_set_input_chan(uint8_t   acmp_id,
                                  uint32_t  positive_chan,
                                  uint16_t  negative_chan)
{
    awbl_acmp_service_t *p_serv = __acmp_chan_to_serv(acmp_id);

    if (p_serv == NULL) {
        return -AW_EINVAL;
    }

    if (p_serv->p_servfuncs->pfunc_set_input_chan != NULL) {
        return p_serv->p_servfuncs->pfunc_set_input_chan(p_serv,
                                                         positive_chan,
                                                         negative_chan);
    }

    return AW_OK;
}


/* end of file */
