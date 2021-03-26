/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file    异步请求响应处理功能
 * \brief
 *
 * \internal
 * \par modification history:
 * - 1.00 20-09-17 vih, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "aw_reqsched.h"

uint32_t aw_reqsched_create (aw_reqsched_hdr_t  pfn_req_hdr,
                             uint32_t           req_max_cache_num,
                             size_t             req_arg_size)
{
    return reqsched_create(pfn_req_hdr, req_max_cache_num, req_arg_size);
}
aw_err_t aw_reqsched_destroy (uint32_t reqid)
{
    return reqsched_destroy(reqid);
}
aw_err_t aw_reqsched_req_send (uint32_t  reqid,
                               void     *p_data,
                               size_t    size,
                               uint32_t *p_token)
{
    return reqsched_req_send(reqid, p_data, size, p_token);
}

aw_err_t aw_reqsched_rsp_send (uint32_t reqid, void *p_data)
{
    return reqsched_rsp_send(reqid, p_data);
}

aw_err_t aw_reqsched_rsp_tryget (uint32_t   reqid,
                                 uint32_t   token,
                                 void      *p_data,
                                 size_t     size)
{
    return reqsched_rsp_tryget(reqid, token, p_data, size);
}

int aw_reqsched_req_available (uint32_t   reqid)
{
    return reqsched_req_available(reqid);
}
/* end of file */
