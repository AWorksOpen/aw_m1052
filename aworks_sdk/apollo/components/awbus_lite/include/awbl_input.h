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
 * \brief Input device model.
 *
 * \internal
 * \par modification history
 * - 1.01 15-01-07  ops, encapsulate aw_input_handler and redefine 
 *                       how to register/unregister.
 * - 1.00 14-08-29  ops, first implementation.
 * \endinternal
 */

#ifndef __AWBL_INPUT_H
#define __AWBL_INPUT_H

#include "aw_types.h"
#include "aw_input.h"
#include "aw_msgq.h"

#ifdef __cplusplus
extern "C" {
#endif

void awbl_input_event_init(void);
void awbl_input_event_key_init(void);
void awbl_input_event_abs_init(void);
void awbl_input_event_rel_init(void);

int  awbl_input_report_key (int ev_type, int code, int key_state);

int  awbl_input_report_coord (int ev_type,
                              int code,
                              int coord_x,
                              int coord_y,
                              int coord_z);

aw_err_t awbl_input_handler_register (aw_input_handler_t   *p_input_handler,
                                      aw_input_cb_t         pfn_cb,
                                      void                 *p_usr_data);

aw_err_t awbl_input_handler_unregister (aw_input_handler_t *p_input_handler);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_INPUT_H */

/* end of file */
