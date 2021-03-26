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
 * \brief AWBus cyw wiced adapter
 *
 * \code
 * #include "awbl_cyw43362_common.h"
 * \endcode
 *
 * \internal
 * \par Modification History
 * - 1.00 19-11-12  phd, first implementation
 * \endinternal
 */

#ifndef __AWBL_CYW_WICED_COMMON_H
#define __AWBL_CYW_WICED_COMMON_H

#include "aw_common.h"


#include "WICED/WWD/include/wwd_constants.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \addtogroup grp_awbl_cyw_wiced_common
 * \copydoc awbl_cyw_wiced_common.h
 * @{
 */

aw_err_t awbl_wiced_pre_power_on(wiced_country_code_t country_code, uint32_t *roam_param);
aw_err_t awbl_wiced_power_on(wiced_country_code_t country_code, uint32_t *roam_param);

/** @} grp_awbl_cyw_wiced_common */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __AWBL_CYW_WICED_COMMON_H */

/* end of file */
