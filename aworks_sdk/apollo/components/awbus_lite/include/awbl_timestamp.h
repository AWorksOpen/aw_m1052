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
 * \brief 基于AWBus的硬件定时器驱动
 *
 * \internal
 * \par modification history
 * - 1.00 12-11-14  orz, first implementation
 * \endinternal
 */

#ifndef __AWBL_TIME_STAMP_H
#define __AWBL_TIME_STAMP_H

#include "aw_common.h"
#include "awbus_lite.h"

#ifdef __cplusplus
extern "C" {
#endif



/**
 * \name awbl_timestamp回调类型
 * @{
 */

struct awbl_timestamp;

/** \brief Get Tx Char callback */
typedef aw_timestamp_t  (*pfunc_timestamp_get_t)(
        struct awbl_timestamp *p_awbl_timestamp);



/** @} */

/**
 * \name serial device data structures
 * @{
 */

/** \brief serial io driver functions */
struct awbl_timestamp {
    pfunc_timestamp_get_t       fn_timestamp_get;
    aw_timestamp_freq_t         freq;
};

/** @} */

aw_err_t awbl_timestamp_dev_register(struct awbl_timestamp *p_awbl_timestamp);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_TIMER_H */

/* end of file */
