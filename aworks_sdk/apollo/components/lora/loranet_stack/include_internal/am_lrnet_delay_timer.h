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
 * \brief   LoRaNet Delay Timer Interface
 *
 * \internal
 * \par modification history:
 * - 2017-05-23 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_DELAY_TIMER_H
#define __AM_LRNET_DELAY_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet_internal.h"

typedef struct {
    am_lpsoftimer_t  timer;
    am_sm_active_t  *p_active;
    uint32_t         extra_arg;

#ifdef AM_LRNET_SRCFG_DEBUG
    const char      *p_str_name;
#endif
} am_lrnet_delay_timer_t;

void am_lrnet_delay_timer_init (am_lrnet_delay_timer_t *p_timer,
                                am_sm_active_t         *p_active);

#ifdef AM_LRNET_SRCFG_DEBUG
void am_lrnet_delay_timer_set_name (am_lrnet_delay_timer_t *p_timer,
                                    const char             *p_name);
#else
am_static_inline
void am_lrnet_delay_timer_set_name (am_lrnet_delay_timer_t *p_timer,
                                    const char             *p_name)
{
    (void) p_timer;
    (void) p_name;
}
#endif

void am_lrnet_delay_timer_set (am_lrnet_delay_timer_t *p_timer,
                               uint32_t                ms,
                               uint32_t                arg);

void am_lrnet_delay_timer_stop (am_lrnet_delay_timer_t *p_time);

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_DELAY_TIMER_H */

/* end of file */
