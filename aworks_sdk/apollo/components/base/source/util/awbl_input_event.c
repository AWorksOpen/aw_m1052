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
 * \brief Event Manager for Input Subsystem.
 *
 * \internal
 * \par modification history:
 * - 1.01 15-01-07  ops, encapsulate aw_input_handler and 
 *                       redefine how to register/unregister.
 * - 1.00 14-09-09  ops, first implementation.
 * \endinternal
 */
#include "aw_input.h"
#include "aw_event.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_pool.h"

#include <string.h>

#include "awbl_input.h"

/*******************************************************************************
  Local variables
*******************************************************************************/

aw_local union aw_input_data     __g_input_data[2];
aw_local struct aw_pool          __g_input_pool;

aw_local struct event_category   __g_category_input;
aw_local struct event_handler    __g_handler_cat_input;

aw_local struct event_type       __g_input_event;

aw_local aw_bool_t               __g_is_ev_init;
aw_local aw_bool_t               __g_is_ev_key_init;
aw_local aw_bool_t               __g_is_ev_abs_init;
aw_local aw_bool_t               __g_is_ev_rel_init;

/*******************************************************************************
    Local functions
*******************************************************************************/

aw_local void __input_event_manager(struct event_type *p_evt_type,
                                    void              *p_evt_data,
                                    void              *p_usr_data)
{
    /* 
     * 执行event_type的handler之前, 先执行该category的handler
     * input category的handler暂时不处理任何事务, 预留给未来使用 
     */
}

/******************************************************************************/
aw_local void __input_event_handle(void *p_data)
{

    aw_event_raise(&__g_input_event, p_data, EVENT_PROC_FLAG_SYNC);

    aw_pool_item_return(&__g_input_pool, p_data);
}

/******************************************************************************/
aw_local void __input_event_func (struct event_type *p_evt_type,
                                  void              *p_evt_data,
                                  void              *p_hdl_data)
{

    struct aw_input_handler *p_hdlr = (struct aw_input_handler *)p_hdl_data;

    p_hdlr->pfn_cb(p_evt_data, p_hdlr->p_usr_data);
}

/*******************************************************************************
    Public functions
*******************************************************************************/
void awbl_input_event_init(void)
{
    if (NULL == __g_input_pool.p_start) {
        aw_pool_init(&__g_input_pool,
                      __g_input_data,
                      sizeof(__g_input_data),
                      sizeof(__g_input_data[0]));
    }

    /* 初始化一个事件类别 : Input类别 */
    aw_event_category_init(&__g_category_input);

    /* 为Input类别初始化一个handler */
    aw_event_handler_init(&__g_handler_cat_input, __input_event_manager,
                          "cat_input", EVENT_HANDLER_FLAG_NONE);

    /* 把handler与Input类别绑定 */
    aw_event_category_handler_register(&__g_category_input,
                                       &__g_handler_cat_input);

    aw_event_init(&__g_input_event);
    aw_event_category_event_register(&__g_category_input, &__g_input_event);

    __g_is_ev_init = AW_TRUE;
}

/******************************************************************************/
void awbl_input_event_key_init(void)
{
    __g_is_ev_key_init = AW_TRUE;
}

/******************************************************************************/
void awbl_input_event_abs_init(void)
{
    __g_is_ev_abs_init = AW_TRUE;
}

/******************************************************************************/
void awbl_input_event_rel_init(void)
{
    __g_is_ev_rel_init = AW_TRUE;
}

/******************************************************************************/
int awbl_input_report_key (int ev_type, int code, int is_pressed)
{
    union aw_input_data *pdata;

    if (__g_is_ev_init && __g_is_ev_key_init) {

        if ((pdata = aw_pool_item_get(&__g_input_pool)) == NULL) {

            return -AW_ENOMEM;
        }

        (void)memset(pdata, 0, sizeof(*pdata));

        pdata->key_data.input_ev.ev_type = ev_type;
        pdata->key_data.key_code         = code;
        pdata->key_data.key_state        = is_pressed;

        __input_event_handle((void *)pdata);

        return AW_OK;
    }

    return -AW_ENODEV;
}

/******************************************************************************/
int awbl_input_report_coord (int   ev_type,
                             int   code,
                             int   coord_x,
                             int   coord_y,
                             int   coord_z)
{
    union aw_input_data *pdata;

    if ((__g_is_ev_init && __g_is_ev_abs_init) ||
        (__g_is_ev_init && __g_is_ev_rel_init)) {

        if ((pdata = aw_pool_item_get(&__g_input_pool)) == NULL) {

            return -AW_ENOMEM;
        }

        (void)memset(pdata, 0, sizeof(*pdata));

        pdata->ptr_data.input_ev.ev_type = ev_type;
        pdata->ptr_data.ptr_code         = code;
        pdata->ptr_data.pos.x            = coord_x;
        pdata->ptr_data.pos.y            = coord_y;
        pdata->ptr_data.pos.z            = coord_z;

        __input_event_handle((void *)pdata);

        return AW_OK;
    }

    return -AW_ENODEV;
}

/******************************************************************************/
aw_err_t awbl_input_handler_register (aw_input_handler_t  *p_input_handler,
                                      aw_input_cb_t        pfn_cb,
                                      void                *p_usr_data)
{
    if (NULL == p_input_handler || NULL == pfn_cb) {

        return -AW_EINVAL;
    }

    p_input_handler->pfn_cb           = pfn_cb;
    p_input_handler->p_usr_data       = p_usr_data;

    p_input_handler->ev_handler.data  = (void *)p_input_handler;
    p_input_handler->ev_handler.flags = EVENT_HANDLER_FLAG_NONE;
    p_input_handler->ev_handler.func  = __input_event_func;

    aw_event_handler_init(&p_input_handler->ev_handler,
                           p_input_handler->ev_handler.func,
                           p_input_handler->ev_handler.data,
                           p_input_handler->ev_handler.flags);

    aw_event_handler_register(&__g_input_event, &p_input_handler->ev_handler);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_input_handler_unregister (aw_input_handler_t *p_input_handler)
{
    if (NULL == p_input_handler) {

        AW_INFOF(("Input System : input handler is invalid. \n"));
        return -AW_EINVAL;
    }

    if (-AW_EINVAL == aw_event_handler_unregister(&__g_input_event,
                                               &p_input_handler->ev_handler)) {

        AW_INFOF(("Input System : input handler unregister failed. \n"));
        return -AW_EINVAL;
    }

    AW_INFOF(("Input System : input handler unregister succeed. \n"));

    return AW_OK;
}

/* end of file */
