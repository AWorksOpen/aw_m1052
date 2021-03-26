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
 * \brief SMTP接口函数
 *
 * 用于使用SMTP协议，发送电子邮件
 *
 * \par 使用示例
 * \code
 *
 * int aw_main ()
 * {
 *     aw_smtp_client_t  smtp_client;
 *     aw_smtp_email_t   smtp_email;
 *     
 *     aw_smtp_mkclient(&smtp_client,
 *                      "sender@something.com",
 *                      "sender@something.com",
 *                      "password",
 *                      "192.168.40.10",
 *                       25,
 *                       smtp_buf,
 *                       256,
 *                       2 * 60 * 1000);
 *
 *     aw_smtp_client_init(&smtp_client);
 *
 *     aw_smtp_mkemail(&smtp_email,
 *                     "receiver@something.com",
 *                     "copy1@something.com,copy2@something.com",
 *                     "The Subject of this email!",
 *                     "Please call me right now!",
 *                      strlen("Please call me right now!")
 *                      NULL);
 *
 *     // send the email
 *     if (aw_smtp_email_send(&smtp_client, &smtp_email) != AW_OK) {
 *         aw_kprintf("Start send failed, please check! \r\n");
 *     }
 * }
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-01-21  tee, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "sys/socket.h"

#ifndef __AW_SMTP_H
#define __AW_SMTP_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief SMTP 客户端结构体
 * \note 用户不应直接操作其中的成员
 */
typedef struct aw_smtp_client {

    /** \brief 用户的邮箱地址（用于MAIL FROM字段） */
    char             *p_email_address;

    /** \brief 用户名（用于验证阶段）              */
    char             *p_username;

    /** \brief 密码（用于验证阶段）                */
    char             *p_password;

    /** \brief 服务器（可以是DNS服务器名或IP地址） */
    char             *p_server;

    /** \brief 服务器端口，通常为25                */
    uint16_t          server_port;

    /**
     * \brief 系统分配的内存空间
     * 建议大小在256字节以上，若API函数返回值为：-AM_ENOMEM，则应增大该空间
     */
    uint8_t          *p_buf;

    /** \brief 分配内存空间的大小                  */
    size_t            buf_len;

    /**
     * \brief 超时时间，当服务器在该段时间内无任何相应时，将返回超时错误
     *  一般将该值设置为： 2 * 60 * 1000 = 120000，即两分钟
     */
    uint32_t          timeout_ms;

} aw_smtp_client_t;

/**
 * \brief SMTP 电子邮件结构体
 * \note 用户不应直接操作其中的成员
 */
typedef struct aw_smtp_email {

    /**
     * \brief 收件人地址，可以是多个收件人，收件人与收件人之间使用','隔开
     *
     * 若仅发送给 abc\@zlg.cn，则该值为: "abc\@zlg.cn"
     * 若发送给abc\@zlg.cn 和 123\@zlg.cn，则该值为： "abc\@zlg.cn,123\@zlg.cn"
     * \note 字符串，务必以'\0'结尾
     */
    const char        *p_to_addresses;

    /**
     * \brief 抄送，可以抄送给多个人，它们之间使用','隔开
     *
     * 若仅抄送给 abc\@zlg.cn，则该值为: "abc\@zlg.cn"
     * 若抄送给abc\@zlg.cn 和 123\@zlg.cn，则该值为： "abc\@zlg.cn,123\@zlg.cn"
     * \note 字符串，务必以'\0'结尾
     */
    const char        *p_cc_addresses;


    /** \brief 主题，字符串，务必以'\0'结尾    */
    const char        *p_subject;

    /** \brief 正文内容（可以包含'\0'）        */
    const uint8_t     *p_body;

    /** \brief 正文的长度  */
    size_t             body_len;

    /** \brief 若主题和正文存在非ASCII文本，则该值指定非ASCII文本的编码格式   */
    const char        *p_text_encoding;

} aw_smtp_email_t;


/**
 * \brief 设置SMTP客户端结构体参数
 *
 * \param[in] p_account        待设置参数的客户端结构体
 * \param[in] p_email_address  用户的邮箱地址
 * \param[in] p_user_name      用户名，一般与邮箱地址相同
 * \param[in] p_password       密码
 * \param[in] p_server         SMTP服务器（可以是DNS服务器名或IP地址）
 * \param[in] server_port      服务器端口，一般为：25（SMTP知名端口号）
 * \param[in] encryption       邮件传输的加密方式
 * \param[in] p_buf            分配的内存空间，若遇到内存不足错误，应增大该空间
 * \param[in] buf_len          分配内存空间的大小，建议256字节以上
 * \param[in] timeout_ms       超时时间（单位ms）
 *
 * \return 无
 *
 * \note 使用 aw_smtp_account_init()初始化账户前，应先用本函数为各成员赋予必要
 * 的初值，邮箱地址、用户名、密码等只能使用ASCII码值。
 */
aw_local aw_inline void aw_smtp_mkclient (aw_smtp_client_t   *p_smtp_client,
                                          char               *p_email_address,
                                          char               *p_user_name,
                                          char               *p_password,
                                          char               *p_server,
                                          uint16_t            server_port,
                                          uint8_t            *p_buf,
                                          size_t              buf_len,
                                          uint32_t            timeout_ms)
{
    p_smtp_client->p_email_address  = p_email_address;
    p_smtp_client->p_username       = p_user_name;
    p_smtp_client->p_password       = p_password;
    p_smtp_client->p_server         = p_server;
    p_smtp_client->server_port      = server_port;
    p_smtp_client->p_buf            = p_buf;
    p_smtp_client->buf_len          = buf_len;
    p_smtp_client->timeout_ms       = timeout_ms;
}

/**
 * \brief 制作一封邮件（设置邮件结构体的各个参数）
 *
 * \param[in] p_email         待设置参数的邮件结构体
 * \param[in] p_to_addresses  收件人的邮箱地址，字符串，以'\0'结尾
 * \param[in] p_cc_addresses  抄送，字符串，以'\0'结尾
 * \param[in] p_subject       主题，字符串，以'\0'结尾
 * \param[in] p_body          邮件内容
 * \param[in] body_len        邮件内容长度，因此，邮件内容可以包含'\0'
 * \param[in] p_text_encode   指定文本的编码方式，该值用于支持非ASCII纯文本邮件
 *
 * \return 无
 *
 * \note 关于文本的编码格式p_text_encode，若主题和邮件内容中没有非ASCII字符，则
 * 该值无需设置，直接填充为NULL即可。若包含非ASCII文本，如（中文），则需要设置
 * 其编码格式，一般地，中文编码格式为”GBK"，因此，若无特殊情况，发送中文文本
 * 时，该值也无需设置，使用默认值"GBK"即可。
 */
aw_local aw_inline void aw_smtp_mkemail (aw_smtp_email_t   *p_email,
                                         const char        *p_to_addresses,
                                         const char        *p_cc_addresses,
                                         const char        *p_subject,
                                         const uint8_t     *p_body,
                                         size_t             body_len,
                                         const char        *p_text_encoding)
{
    p_email->p_to_addresses  = p_to_addresses;
    p_email->p_cc_addresses  = p_cc_addresses;
    p_email->p_subject       = p_subject;
    p_email->p_body          = p_body;
    p_email->body_len        = body_len;
    p_email->p_text_encoding = p_text_encoding;
}


/**
 * \brief 初始化一个SMTP客户端
 *
 * \param[in] p_account  指向需要初始化的SMTP客户端结构体
 *
 * \retval AW_OK       初始化成功，可以发送邮件
 * \retval -AW_EINVAL  结构体参数有误，邮箱地址、用户名、密码等只能使用ASCII码值
 *
 * \note 务必确保初始化前使用aw_smtp_mkclient()函数设置好必要参数的值
 */
aw_err_t aw_smtp_client_init (aw_smtp_client_t  *p_client);

/**
 * \brief 当一个SMTP客户端不再使用时，使用该函数解初始化
 *
 * \param[in] p_account  指向需要解初始化的SMTP客户端结构体
 *
 * \retval  AW_OK      解初始化成功
 * \retval -AW_EINVAL  参数有误
 */
aw_err_t aw_smtp_client_deinit (aw_smtp_client_t *p_client);

/**
 * \brief 发送一封电子邮件
 *
 * \param[in] p_account  指向发送者的SMTP客户端结构体
 * \param[in] p_mail     使用  aw_smtp_mkemail() 制作好的一封邮件
 *
 * \retval  AW_OK      发送邮件成功
 * \retval -AW_EINVAL  参数有误
 * \retval -AW_ENOMEM  内存不足，应增大为账户分配的内存
 */
aw_err_t aw_smtp_email_send (aw_smtp_client_t      *p_client,
                             const aw_smtp_email_t *p_email);

#ifdef __cplusplus
}
#endif

#endif /* __AW_SMTP_H */

/* end of file */
