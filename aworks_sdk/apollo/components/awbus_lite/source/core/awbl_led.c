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
 * \brief LED device management.
 *
 * \internal
 * \par modification history
 * - 1.00 14-11-22  ops, first implementation.
 * \endinternal
 */

/**
 * @addtogroup awbl_led
 * @{
 */

#include "apollo.h"
#include "aw_led.h"
#include "aw_common.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "aw_errno.h"
#include "aw_compiler.h"

#include "awbl_led.h"

AWBL_METHOD_IMPORT(awbl_ledserv_get);

struct awbl_led_service *__gp_led_serv_head = NULL;

aw_local aw_err_t __led_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t   pfn_led_serv = NULL;
    struct awbl_led_service *p_led_serv = NULL;

    pfn_led_serv = awbl_dev_method_get(p_dev,
                                       AWBL_METHOD_CALL(awbl_ledserv_get));

    if (pfn_led_serv != NULL) {

        pfn_led_serv(p_dev, &p_led_serv);

        if (p_led_serv != NULL) {

            struct awbl_led_service **pp_serv_cur = &__gp_led_serv_head;

            while (*pp_serv_cur != NULL) {

                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_led_serv;
            p_led_serv->p_next = NULL;
        }
    }

    return AW_OK;
}

aw_err_t __led_serv_alloc (void)
{
    __gp_led_serv_head = NULL;

    awbl_dev_iterate(__led_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

struct awbl_led_service * __led_id_to_serv (int id)
{
    struct awbl_led_service *p_serv_cur = __gp_led_serv_head;

    while ((p_serv_cur != NULL)) {

        if ((id >= p_serv_cur->p_servinfo->start_id) &&
            (id <= p_serv_cur->p_servinfo->end_id)) {

            return p_serv_cur;
        }

        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}


void awbl_led_init (void)
{
    __led_serv_alloc();
}

aw_err_t aw_led_set (int id, aw_bool_t on)
{
    struct awbl_led_service *p_serv = __led_id_to_serv(id);

    if (p_serv == NULL) {

        return -AW_ENODEV;
    }

    return p_serv->p_servfuncs->pfn_led_set(p_serv->p_cookie, id, on);
}

aw_err_t aw_led_toggle (int id)
{
    struct awbl_led_service *p_serv = __led_id_to_serv(id);

    if (p_serv == NULL) {

        return -AW_ENODEV;
    }

    return p_serv->p_servfuncs->pfn_led_toggle(p_serv->p_cookie, id);
}

aw_err_t aw_led_on (int id)
{
    return aw_led_set(id, AW_TRUE);
}

/** \brief turn off led */
aw_err_t aw_led_off (int id)
{
    return aw_led_set(id, AW_FALSE);
}

/** @} */

/* end of file */
