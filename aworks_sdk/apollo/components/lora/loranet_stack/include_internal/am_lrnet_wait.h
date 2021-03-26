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
 * \brief   LoRaNet Wait Module Interface
 *
 * \internal
 * \par modification history:
 * - 2017-05-23 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_WAIT_H
#define __AM_LRNET_WAIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet_internal.h"

typedef struct {
    am_lora_bool_t finish_flg;
    void  *p_arg;
    void  *p_wait;
} am_lrnet_wait_t;

void am_lrnet_wait_init (am_lrnet_wait_t *p_wait, void *p_arg);

void am_lrnet_wait_finish (am_lrnet_wait_t *p_wait);

am_lora_bool_t am_lrnet_wait_is_done (am_lrnet_wait_t *p_wait);

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_WAIT_H */

/* end of file */
