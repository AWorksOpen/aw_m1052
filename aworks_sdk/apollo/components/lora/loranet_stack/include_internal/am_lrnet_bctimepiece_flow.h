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
 * \brief   LoRaNet SM-BcTimePiece Moudle - Workflow Interface
 *
 * \internal
 * \par modification history:
 * - 2017-05-23 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_BCTIMEPIECE_FLOW_H
#define __AM_LRNET_BCTIMEPIECE_FLOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet_internal.h"

typedef struct {

    am_lrnet_sm_bctimepiece_flow_t super;

    /*  */
    am_lrnet_lcmd_gfetch_t        gfetch;
    am_lrnet_lcmd_gfetchexeack_t  exeack;
} am_lrnet_flow_gfetch_t;

/** \brief up-cast */
#define AM_LRNET_FLOW_GETCH_TO_FLOW(p)   (&(p)->super)

/** \brief down-cast */
#define AM_LRNET_FLOW_GETCH_FROM_FLOW(p)  \
            (AM_LORA_CONTAINER_OF((p), am_lrnet_flow_gfetch_t, super))

int am_lrnet_flow_gfetch_init (am_lrnet_flow_gfetch_t       *p_flow,
                               const am_lrnet_lcmd_gfetch_t *p_gfetch);

void am_lrnet_flow_gfetch_free (am_lrnet_flow_gfetch_t *p_flow);



typedef struct {
    am_lrnet_sm_bctimepiece_flow_t super;

    am_lrnet_lcmd_detect_t         detect;
    am_lrnet_lcmd_detectexeack_t   exeack;
} am_lrnet_flow_detect_t;

/** \brief up-cast */
#define AM_LRNET_FLOW_DETECT_TO_FLOW(p)   (&(p)->super)

/** \brief down-cast */
#define AM_LRNET_FLOW_DETECT_FROM_FLOW(p)  \
            (AM_LORA_CONTAINER_OF((p), am_lrnet_flow_detect_t, super))

int am_lrnet_flow_detect_init (am_lrnet_flow_detect_t       *p_flow,
                               const am_lrnet_lcmd_detect_t *p_detect);

void am_lrnet_flow_detect_free (am_lrnet_flow_detect_t *p_flow);

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_BCTIMEPIECE_FLOW_H */

/* end of file */
