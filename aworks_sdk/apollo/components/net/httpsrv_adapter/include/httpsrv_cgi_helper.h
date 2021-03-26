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
 * \brief httpsrv_cgi_helper.h
 *
 * \internal
 * \par History
 * - 17-10-18, chenminghao, First implementation.
 * \endinternal
 */

#ifndef __HTTPSRV_CGI_HELPER_H_
#define __HTTPSRV_CGI_HELPER_H_

#include <stddef.h>
#include <stdint.h>

size_t httpsrv_extract_uri_parameters (char *p_param_names[],
                                       char *p_param_vals[],
                                       char *p_param,
                                       int   max_param_cnt);
int httpsrv_find_cgi_parameter (const char *p_target, 
                                char       *p_param[], 
                                int         max_param_cnt);
size_t httpsrv_raw_url_decode (char *p_encoded, size_t len);
size_t httpsrv_raw_url_encode (char const *p_decoded, 
                               char const *p_encoded, 
                               size_t      len);


#define HTTPSRV_CGI_FILE_BUFFER_SIZE   (256)

typedef struct _httpsrv_cgi_file {
    char    *filename;         /**< \brief �ļ����� */
    char    *name;             /**< \brief ������Ϣ */
    char    *data;             /**< \brief �ļ�����ָ�� */
    char    *boundary;         /**< \brief �ָ��� */
    uint8_t  state;
    size_t   data_length;      /**< \brief �����ļ����ݵĴ�С */
}httpsrv_cgi_file_t;
/**
 * \brief   ��ʼ���ṹ��
 *
 * \param[in]    ses_handle  ��
 * \param[out]   p_file      �� �ļ���Ϣ�ṹ��ָ��
 *
 * \return
 */
int httpsev_cgi_file_data_init(uint32_t ses_handle, httpsrv_cgi_file_t *p_file);

/**
 * \brief	��ȡ�ϴ��ļ�������
 *
 * \param[in]    ses_handle  ��
 * \param[in]    len         �� ������ȡ�����ݴ�С
 * \param[out]   p_file      �� �ļ���Ϣ�ṹ��ָ��
 *
 * \return 0  : ������һ������
 *         1  : ���������һ������
 *         -1 : δ֪����
 */
int httpsev_cgi_file_data_get(uint32_t            ses_handle, 
                              httpsrv_cgi_file_t *p_file, 
                              size_t              len);

#endif /* __HTTPSRV_CGI_HELPER_H_ */

/* end of file */
