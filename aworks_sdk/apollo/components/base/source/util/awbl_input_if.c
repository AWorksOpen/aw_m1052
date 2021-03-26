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
 * \brief input interface.
 *
 * \internal
 * \par modification history
 * - 1.01 15-01-07  ops, encapsulate aw_input_handler and 
 *                       redefine how to register/unregister.
 * - 1.00 14-07-18  ops, first implementation.
 * \endinternal
 */

#include "aw_input.h"
#include "awbl_input.h"

/**
 * \addtogroup awbl_if_input
 * @{
 */

int aw_input_report_key (int ev_type, int code, int is_pressed)
{
    return awbl_input_report_key(ev_type, code, is_pressed);
}

int aw_input_report_coord (int ev_type, int code, int coord_x, int coord_y, int coord_z)
{
    return awbl_input_report_coord(ev_type, code, coord_x, coord_y, coord_z);
}

aw_err_t aw_input_handler_register (aw_input_handler_t  *p_input_handler,
                                    aw_input_cb_t        pfn_cb,
                                    void                *p_usr_data)
{
    return awbl_input_handler_register(p_input_handler, pfn_cb, p_usr_data);
}

aw_err_t aw_input_handler_unregister (aw_input_handler_t  *p_input_handler)
{
    return awbl_input_handler_unregister(p_input_handler);
}

/** @} */

/* end of file */
