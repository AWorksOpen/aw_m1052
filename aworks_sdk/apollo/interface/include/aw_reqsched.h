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
 * \brief �첽������Ӧ�����ܣ������ں�ʱ�ϳ����첽����Ĺ�����������Ϻ���Ҫ�ظ���Ӧ���ݵĳ�����
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


/** \brief �� aw_reqsched_hdr_t ���͵Ļص������� p_data ����ת��Ϊ������� token  */
#define AW_REQSCHED_DATA_TO_TOKEN(p_data)  \
        (uint32_t)container_of(p_data, reqsched_arg_t, data)

/**
 * \brief ������ص����Ͷ���
 */
typedef reqsched_hdr_t aw_reqsched_hdr_t;

/**
 * \brief ������������ʼ��
 */
void aw_reqschd_lib_init (void);

/**
 * \brief ����һ�����������
 *
 * \param[in]   pfn_req_hdr         ������ص�
 * \param[in]   req_max_cache_num   ������󻺴�����
 * \param[in]   req_arg_size        ���������С
 *
 * \retval  0                       ����ʧ��
 * \retval  >0                      ����id
 */
uint32_t aw_reqsched_create (aw_reqsched_hdr_t  pfn_req_hdr,
                             uint32_t           req_max_cache_num,
                             size_t             req_arg_size);
/**
 * \brief   ����һ�����������
 *
 * \param[in]   reqid           �����������id
 *
 * \retval  AW_OK               �����ɹ�
 * \retval  -AW_ENOTSUP         ��֧�����ٹ���
 */
aw_err_t aw_reqsched_destroy (uint32_t reqid);

/**
 * \brief ����һ������
 *
 * \param[in]  reqid            �����id
 * \param[in]  p_data           �����������Ϊֻ�ṩ��һ�������������˸ò���Ӧ�����������Ӧ�Ľṹ��
 * \param[in]  size             ������С
 * \param[out] p_token          ���ڴ������token
 *
 * \retval  AW_OK               �����ɹ�
 */
aw_err_t aw_reqsched_req_send (uint32_t  reqid,
                               void     *p_data,
                               size_t    size,
                               uint32_t *p_token);
/**
 * \brief   ����һ����Ӧ
 *
 * \param[in]   reqid    �����id
 * \param[in]   p_data   ��Ӧ�������ò�������Ϊ������ص��Ĳ��� p_data;
 *                       ��Ϊ��������ʱ��ֻ�ṩ��һ�������������˸ò��������������Ӧ�Ľṹ��
 *
 * \retval  AW_OK               �����ɹ�
 */
aw_err_t aw_reqsched_rsp_send (uint32_t reqid, void *p_data);

/**
 * \brief   ���Ի�ȡ�������Ӧ
 *
 * \param[in]   reqid    �����id
 * \param[in]   token    ����token
 * \param[in]   p_data   �����Ӧ�ĵ�ַ����ʵ�������뷢������ʱ��ͬ
 * \param[in]   size     ��ȡ����Ӧ��С
 *
 * \retval  AW_OK                   �����ɹ�
 * \retval  -AW_EINPROGRESS         ���������ڴ���
 */
aw_err_t aw_reqsched_rsp_tryget (uint32_t   reqid,
                                 uint32_t   token,
                                 void      *p_data,
                                 size_t     size);

/**
 * \brief   ��ȡ���е����󻺴�����
 *
 * \param[in]   reqid    �����id
 *
 * \retval  >=0                     ��������
 * \retval  <0                      �鿴������
 */
int aw_reqsched_req_available (uint32_t   reqid);

#ifdef __cplusplus
}
#endif /* __cplusplus   */


#endif /* __AW_REQSCHED_H__ */
