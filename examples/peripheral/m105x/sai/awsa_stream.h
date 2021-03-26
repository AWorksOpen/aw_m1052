/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief data stream api
 *
 * \internal
 * \par modification history:
 * - 1.00 16-08-09  win, first implemetation
 * \endinternal
 */

#ifndef __AWSA_STREAM_H
#define __AWSA_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "string.h"

/**
 * \brief 流类型
 */
typedef enum awsa_stream_type {
    AWSA_STREAM_TYPE_FILE = 0,
    AWSA_STREAM_TYPE_MEMORY = 1,
} awsa_stream_type_t;

/**
 * \brief 流结构体
 */
typedef struct awsa_stream {
    awsa_stream_type_t  type;
    uint8_t            *p_buf;
    int                 pos;
    int                 mem_len;
    int                 fd;
} awsa_stream_t;

/**
 * \brief 初始化内存流对象
 *
 * \param[in] p_stream : 流对象结构体
 * \param[in] p_buf    : 流对象对应的内存缓冲区
 * \param[in] len      : 流长度
 *
 * \retval 0 : 成功
 */
int awsa_stream_mem_init (awsa_stream_t *p_stream,
                          const uint8_t *p_buf,
                          int            len);

/**
 * \brief 初始化文件流对象
 *
 * \param[in] p_stream : 流对象结构体
 * \param[in] p_file   : 流对象对应的文件缓冲区
 * \param[in] oflag    :
 *
 * \retval 0 : 成功
 */
int awsa_stream_file_init (awsa_stream_t *p_stream,
                           const char    *p_file,
                           int            oflag,
                           mode_t         mode);

/**
 * \brief 读取流中数据
 *
 * \param[in] p_stream : 流对象结构体
 * \param[in] p_buf    : 存放从流中读出数据的缓冲区
 * \param[in] nbyte    : 需要读出长度
 *
 * \return 读出数据的真实长度
 */
ssize_t awsa_stream_read (awsa_stream_t *p_stream,
                          void          *p_buf,
                          size_t         nbyte);

/**
 * \brief 移动流读写操作起始位置
 *
 * \param[in] p_stream : 流对象结构体
 * \param[in] offset   : 偏移值
 * \param[in] whence   : SEEK_SET, 参数offset即为新的读写位置
 *                       SEEK_CUR, 以目前的读写位置往后增加offset个位移量
 *                       SEEK_END, 将读写位置指向文件尾后再增加offset个位移量
 *                       当whence值为SEEK_CUR或SEEK_END时，参数offset允许负值的出现
 *
 * \retval 0 : 成功
 */
off_t awsa_stream_lseek (awsa_stream_t *p_stream,
                         off_t          offset,
                         int            whence);

/**
 * \brief 关闭流
 * \param[in] p_stream : 流对象结构体
 * \retval 0 : 成功
 */
void awsa_stream_close (awsa_stream_t *p_stream);

/**
 * \brief 往流中写入数据
 *
 * \param[in] p_stream : 流对象结构体
 * \param[in] p_buf    : 存放需要写入的数据
 * \param[in] nbyte    : 需要写入的长度
 *
 * \return 写入数据的真实长度
 */
ssize_t awsa_stream_write (awsa_stream_t *p_stream,
                           const void    *p_buf,
                           size_t         nbyte);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_STREAM_H */

/* end of file */
