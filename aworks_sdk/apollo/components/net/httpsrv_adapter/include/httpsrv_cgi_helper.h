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
    char    *filename;         /**< \brief 文件名称 */
    char    *name;             /**< \brief 输入信息 */
    char    *data;             /**< \brief 文件数据指针 */
    char    *boundary;         /**< \brief 分割线 */
    uint8_t  state;
    size_t   data_length;      /**< \brief 本次文件数据的大小 */
}httpsrv_cgi_file_t;
/**
 * \brief   初始化结构体
 *
 * \param[in]    ses_handle  ：
 * \param[out]   p_file      ： 文件信息结构体指针
 *
 * \return
 */
int httpsev_cgi_file_data_init(uint32_t ses_handle, httpsrv_cgi_file_t *p_file);

/**
 * \brief	获取上传文件的数据
 *
 * \param[in]    ses_handle  ：
 * \param[in]    len         ： 期望获取的数据大小
 * \param[out]   p_file      ： 文件信息结构体指针
 *
 * \return 0  : 还有下一次数据
 *         1  : 本次是最后一次数据
 *         -1 : 未知错误
 */
int httpsev_cgi_file_data_get(uint32_t            ses_handle, 
                              httpsrv_cgi_file_t *p_file, 
                              size_t              len);

#endif /* __HTTPSRV_CGI_HELPER_H_ */

/* end of file */
