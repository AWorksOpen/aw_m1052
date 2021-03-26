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
 * \brief Input Service Configuration.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-01  ops, first implementation.
 * \endinternal
 */

#include "aw_msgq.h"
#include "aw_input.h"
#include "awbl_input.h"

#ifdef AXIO_AWORKS
#include "aw_event_autoconf.h"
void awbl_input_ev_mgr_register(void)
{
    awbl_input_event_init();

#ifdef CONFIG_AW_COM_INPUT_EV_KEY
    awbl_input_event_key_init();

#endif /* AW_COM_AWBL_INPUT_EV_KEY */

#ifdef CONFIG_AW_COM_INPUT_EV_ABS
    awbl_input_event_abs_init();

#endif /* AW_COM_AWBL_INPUT_ABS */

#ifdef CONFIG_AW_COM_INPUT_EV_REL
    awbl_input_event_rel_init();

#endif /* AW_COM_AWBL_INPUT_EV_REL */  
}
#else

void awbl_input_ev_mgr_register(void)
{
    awbl_input_event_init();
    
#ifdef AW_COM_INPUT_EV_KEY
    awbl_input_event_key_init();
#endif /* AW_COM_AWBL_INPUT_EV_KEY */

#ifdef AW_COM_INPUT_EV_ABS
    awbl_input_event_abs_init();
#endif /* AW_COM_AWBL_INPUT_ABS */

#ifdef AW_COM_INPUT_EV_REL
    awbl_input_event_rel_init();
#endif /* AW_COM_AWBL_INPUT_EV_REL */

} 

#endif


