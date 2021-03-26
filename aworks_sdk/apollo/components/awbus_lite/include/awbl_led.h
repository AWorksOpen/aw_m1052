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
 * - 1.00 14-11-22  ops, first implementation
 * \endinternal
 */

#ifndef __AWBL_LED_H
#define __AWBL_LED_H

#include "aw_types.h"
#include "aw_list.h"
#include "awbus_lite.h"

#ifdef __cplusplus
extern "C" {
#endif

struct awbl_led_servinfo {

    int start_id;    /**< \brief 本LED服务的起始LED编号 */

    int end_id;      /**< \brief 本LED服务的结束LED编号 */
};

struct awbl_led_servfuncs {

    aw_err_t (*pfn_led_set)   (void *p_cookie, int id, aw_bool_t on);

    aw_err_t (*pfn_led_toggle)(void *p_cookie, int id);

};

struct awbl_led_service {

    struct awbl_led_service *p_next;

    const struct awbl_led_servinfo *p_servinfo;

    const struct awbl_led_servfuncs *p_servfuncs;

    void *p_cookie;

};

void awbl_led_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LED_H */

/* end of file */
