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
 * \brief 通用管道缓冲区
 *
 * 使用本服务需要包含头文件 aw_pipe.h
 *
 * \par 简单示例
 * \code
 *
 * #include "apollo.h"
 * #include "aw_pipe.h"
 *
 * AW_PIPE_DECL(pipe);
 *
 * char buffer[60];
 *
 * uint8_t data = 0;
 *
 * size_t ncount = 0;
 *
 * aw_pipe_init(&pipe, buffer, 60);      // 初始化管道缓冲区
 *
 *
 * aw_pipe_putbyte(&pipe, 'a', AW_SEM_WAIT_FOREVER);    // 向管道中写入一个字符
 * aw_pipe_getbyte(&pipe, &data, AW_SEM_WAIT_FOREVER);  // 从管道中读取1个字符
 *
 * aw_pipe_write(&pipe, "12345678", 8, &ncount, AW_SEM_WAIT_FOREVER);  // 向管道写入8个字符
 * aw_pipe_read(&pipe, &data, 1, &ncount, AW_SEM_WAIT_FOREVER);        // 从管道中读取1个字符
 *
 * aw_pipe_terminate(&pipe);             // 销毁管道
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history
 * - 1.00 15-05-14  dav, create file
 * \endinternal
 */

#ifndef AW_PIPE_H_
#define AW_PIPE_H_

#include "aw_common.h"
#include "aw_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_pipe
 * \copydoc aw_pipe.h
 * @{
 */

/**
 * \brief 定义管道结构体
 */
#define AW_PIPE_DECL(pipe)  struct aw_pipe pipe;

/**
 * \brief 管道数据管理结构
 * \note 不要直接操作本结构的成员
 */
struct aw_pipe {
    int    wr;                      /**< \brief 缓冲区当前写入偏移 */
    int    rd;                      /**< \brief 缓冲区 当前读取偏移 */

    int    unread;                  /**< \brief 管道中存在未读取的数据数量 */
    int    len;                     /**< \brief 管道缓冲区长度 */
    char   *buf;                    /**< \brief 缓冲区 */

    AW_SEMC_DECL( sem_rd );         /**< \brief 读操作信号量 */
    AW_SEMC_DECL( sem_wr );         /**< \brief 写操作信号量 */
    AW_MUTEX_DECL( mutex );         /**< \brief 操作互斥量 */
};

typedef struct aw_pipe *aw_pipe_t;

/**
 * \brief 初始化并打开管道
 *
 * \param p_pipe        要操作的管道
 * \param p_buf         初始化传入的缓冲区
 * \param nbytes        初始化的缓冲区大小
 *
 * \return 返回0表示初始化成功，否则表示初始化失败
 */

aw_err_t aw_pipe_init(aw_pipe_t p_pipe, void *p_buf, size_t nbytes);

/**
 * \brief 从管道中读取数据
 *
 * \param p_pipe        要操作的管道
 * \param p_buf         存放获取数据的缓冲
 * \param nbytes        要获取的数据个数
 * \param p_readbytes   已读取到的数据个数
 * \param timeout       信号量超时时间
 *
 * \return 成功获取的数据个数
 */

aw_err_t aw_pipe_read(aw_pipe_t p_pipe, 
                      void     *p_buf, size_t nbytes, 
                      size_t   *p_readbytes, 
                      int       timeout);

/**
 * \brief 向管道写入若干字节
 *
 * \param p_pipe        要操作的管道
 * \param p_buf         要存放到管道缓冲区的数据缓冲
 * \param nbytes        要存放到管道缓冲区的数据个数
 * \param p_writebytes  成功写入的字节数目
 * \param timeout       信号量超时时间
 *
 * \return 成功存放的数据个数
 */
aw_err_t aw_pipe_write(aw_pipe_t   p_pipe, 
                       const void *p_buf, 
                       size_t      nbytes, 
                       size_t     *p_writebytes,
                       int         timeout);


/**
 * \brief 从管道中取出一个字节
 *
 * \param p_pipe        要操作的管道
 * \param p_data        存放获取数据的缓冲
 * \param timeout       信号量超时时间
 *
 * \return 成功获取的数据个数
 */

aw_err_t aw_pipe_getbyte(aw_pipe_t p_pipe, uint8_t *p_data, int timeout);

/**
 * \brief 向管道写入一个字节
 *
 * \param p_pipe      要操作的管道
 * \param data        要存放到缓冲区的数据字节
 * \param timeout     信号量超时时间
 *
 * \return 成功存放的数据个数
 */

aw_err_t aw_pipe_putbyte(aw_pipe_t p_pipe, const uint8_t data, int timeout);

/**
 * \brief 关闭管道
 *
 * \param p_pipe      要操作的管道
 *
 * \return 打开状态
 */

/** @} */

aw_err_t aw_pipe_terminate(aw_pipe_t p_pipe);

#ifdef __cplusplus
}
#endif

#endif /* AW_PIPE_H_ */

/* end of file */
