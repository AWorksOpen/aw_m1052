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
 * \file
 * \brief 异步请求响应处理功能，适用于耗时较长需异步处理的工作，处理完毕后需要回复响应数据的场景；
 *
 * \internal
 * \par modification history:
 * - 1.00 20-09-17  vih, first implementation
 * \endinternal
 */

#ifndef __AW_REQSCHED_H__
#define __AW_REQSCHED_H__

#include "aw_common.h"
#include "reqsched.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/** \brief 将 aw_reqsched_hdr_t 类型的回调函数的 p_data 参数转换为该请求的 token  */
#define AW_REQSCHED_DATA_TO_TOKEN(p_data)  \
        (uint32_t)container_of(p_data, reqsched_arg_t, data)

/**
 * \brief 请求处理回调类型定义
 */
typedef reqsched_hdr_t aw_reqsched_hdr_t;

/**
 * \brief 请求调度器库初始化
 */
void aw_reqschd_lib_init (void);

/**
 * \brief 创建一个请求调度器
 *
 * \param[in]   pfn_req_hdr         请求处理回调
 * \param[in]   req_max_cache_num   请求最大缓存数量
 * \param[in]   req_arg_size        请求参数大小
 *
 * \retval  0                       创建失败
 * \retval  >0                      请求id
 */
uint32_t aw_reqsched_create (aw_reqsched_hdr_t  pfn_req_hdr,
                             uint32_t           req_max_cache_num,
                             size_t             req_arg_size);
/**
 * \brief   销毁一个请求调度器
 *
 * \param[in]   reqid           请求调度器的id
 *
 * \retval  AW_OK               操作成功
 * \retval  -AW_ENOTSUP         不支持销毁功能
 */
aw_err_t aw_reqsched_destroy (uint32_t reqid);

/**
 * \brief 发出一个请求
 *
 * \param[in]  reqid            请求的id
 * \param[in]  p_data           请求参数，因为只提供了一个请求参数，因此该参数应包含请求和响应的结构；
 * \param[in]  size             参数大小
 * \param[out] p_token          用于存放请求token
 *
 * \retval  AW_OK               操作成功
 */
aw_err_t aw_reqsched_req_send (uint32_t  reqid,
                               void     *p_data,
                               size_t    size,
                               uint32_t *p_token);
/**
 * \brief   发出一个响应
 *
 * \param[in]   reqid    请求的id
 * \param[in]   p_data   响应参数，该参数必须为请求处理回调的参数 p_data;
 *                       因为发出请求时，只提供了一个请求参数，因此该参数包含请求和响应的结构；
 *
 * \retval  AW_OK               操作成功
 */
aw_err_t aw_reqsched_rsp_send (uint32_t reqid, void *p_data);

/**
 * \brief   尝试获取请求的响应
 *
 * \param[in]   reqid    请求的id
 * \param[in]   token    请求token
 * \param[in]   p_data   存放响应的地址，其实际类型与发出请求时相同
 * \param[in]   size     获取的响应大小
 *
 * \retval  AW_OK                   操作成功
 * \retval  -AW_EINPROGRESS         该请求正在处理
 */
aw_err_t aw_reqsched_rsp_tryget (uint32_t   reqid,
                                 uint32_t   token,
                                 void      *p_data,
                                 size_t     size);

/**
 * \brief   获取空闲的请求缓存数量
 *
 * \param[in]   reqid    请求的id
 *
 * \retval  >=0                     空闲数量
 * \retval  <0                      查看错误码
 */
int aw_reqsched_req_available (uint32_t   reqid);

#ifdef __cplusplus
}
#endif /* __cplusplus   */


#endif /* __AW_REQSCHED_H__ */
