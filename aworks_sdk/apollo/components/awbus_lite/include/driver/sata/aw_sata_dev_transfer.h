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
 * \brief SATA dev transfer head file
 *
 * define SATA dev transfer data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 17-08-07  anu, first implementation
 * \endinternal
 */


#ifndef __AW_SATA_DEV_TRANSFER_H
#define __AW_SATA_DEV_TRANSFER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_satabus.h"

aw_err_t aw_sata_dev_transfer_request_sync (
        struct awbl_satabus_dev *p_this, awbl_satabus_cmd_req_t *p_req);

aw_err_t aw_sata_dev_transfer_request_async (
        struct awbl_satabus_dev *p_this, awbl_satabus_cmd_req_t *p_req);

aw_err_t aw_sata_dev_transfer_request_cancel (
        struct awbl_satabus_dev *p_this, awbl_satabus_cmd_req_t *p_req);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_SATA_DEV_TRANSFER_H */

/* end of file */
