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
 * \brief SMTP�ӿں���
 *
 * ����ʹ��SMTPЭ�飬���͵����ʼ�
 *
 * \par ʹ��ʾ��
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
 * \brief SMTP �ͻ��˽ṹ��
 * \note �û���Ӧֱ�Ӳ������еĳ�Ա
 */
typedef struct aw_smtp_client {

    /** \brief �û��������ַ������MAIL FROM�ֶΣ� */
    char             *p_email_address;

    /** \brief �û�����������֤�׶Σ�              */
    char             *p_username;

    /** \brief ���루������֤�׶Σ�                */
    char             *p_password;

    /** \brief ��������������DNS����������IP��ַ�� */
    char             *p_server;

    /** \brief �������˿ڣ�ͨ��Ϊ25                */
    uint16_t          server_port;

    /**
     * \brief ϵͳ������ڴ�ռ�
     * �����С��256�ֽ����ϣ���API��������ֵΪ��-AM_ENOMEM����Ӧ����ÿռ�
     */
    uint8_t          *p_buf;

    /** \brief �����ڴ�ռ�Ĵ�С                  */
    size_t            buf_len;

    /**
     * \brief ��ʱʱ�䣬���������ڸö�ʱ�������κ���Ӧʱ�������س�ʱ����
     *  һ�㽫��ֵ����Ϊ�� 2 * 60 * 1000 = 120000����������
     */
    uint32_t          timeout_ms;

} aw_smtp_client_t;

/**
 * \brief SMTP �����ʼ��ṹ��
 * \note �û���Ӧֱ�Ӳ������еĳ�Ա
 */
typedef struct aw_smtp_email {

    /**
     * \brief �ռ��˵�ַ�������Ƕ���ռ��ˣ��ռ������ռ���֮��ʹ��','����
     *
     * �������͸� abc\@zlg.cn�����ֵΪ: "abc\@zlg.cn"
     * �����͸�abc\@zlg.cn �� 123\@zlg.cn�����ֵΪ�� "abc\@zlg.cn,123\@zlg.cn"
     * \note �ַ����������'\0'��β
     */
    const char        *p_to_addresses;

    /**
     * \brief ���ͣ����Գ��͸�����ˣ�����֮��ʹ��','����
     *
     * �������͸� abc\@zlg.cn�����ֵΪ: "abc\@zlg.cn"
     * �����͸�abc\@zlg.cn �� 123\@zlg.cn�����ֵΪ�� "abc\@zlg.cn,123\@zlg.cn"
     * \note �ַ����������'\0'��β
     */
    const char        *p_cc_addresses;


    /** \brief ���⣬�ַ����������'\0'��β    */
    const char        *p_subject;

    /** \brief �������ݣ����԰���'\0'��        */
    const uint8_t     *p_body;

    /** \brief ���ĵĳ���  */
    size_t             body_len;

    /** \brief ����������Ĵ��ڷ�ASCII�ı������ֵָ����ASCII�ı��ı����ʽ   */
    const char        *p_text_encoding;

} aw_smtp_email_t;


/**
 * \brief ����SMTP�ͻ��˽ṹ�����
 *
 * \param[in] p_account        �����ò����Ŀͻ��˽ṹ��
 * \param[in] p_email_address  �û��������ַ
 * \param[in] p_user_name      �û�����һ���������ַ��ͬ
 * \param[in] p_password       ����
 * \param[in] p_server         SMTP��������������DNS����������IP��ַ��
 * \param[in] server_port      �������˿ڣ�һ��Ϊ��25��SMTP֪���˿ںţ�
 * \param[in] encryption       �ʼ�����ļ��ܷ�ʽ
 * \param[in] p_buf            ������ڴ�ռ䣬�������ڴ治�����Ӧ����ÿռ�
 * \param[in] buf_len          �����ڴ�ռ�Ĵ�С������256�ֽ�����
 * \param[in] timeout_ms       ��ʱʱ�䣨��λms��
 *
 * \return ��
 *
 * \note ʹ�� aw_smtp_account_init()��ʼ���˻�ǰ��Ӧ���ñ�����Ϊ����Ա�����Ҫ
 * �ĳ�ֵ�������ַ���û����������ֻ��ʹ��ASCII��ֵ��
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
 * \brief ����һ���ʼ��������ʼ��ṹ��ĸ���������
 *
 * \param[in] p_email         �����ò������ʼ��ṹ��
 * \param[in] p_to_addresses  �ռ��˵������ַ���ַ�������'\0'��β
 * \param[in] p_cc_addresses  ���ͣ��ַ�������'\0'��β
 * \param[in] p_subject       ���⣬�ַ�������'\0'��β
 * \param[in] p_body          �ʼ�����
 * \param[in] body_len        �ʼ����ݳ��ȣ���ˣ��ʼ����ݿ��԰���'\0'
 * \param[in] p_text_encode   ָ���ı��ı��뷽ʽ����ֵ����֧�ַ�ASCII���ı��ʼ�
 *
 * \return ��
 *
 * \note �����ı��ı����ʽp_text_encode����������ʼ�������û�з�ASCII�ַ�����
 * ��ֵ�������ã�ֱ�����ΪNULL���ɡ���������ASCII�ı����磨���ģ�������Ҫ����
 * ������ʽ��һ��أ����ı����ʽΪ��GBK"����ˣ�����������������������ı�
 * ʱ����ֵҲ�������ã�ʹ��Ĭ��ֵ"GBK"���ɡ�
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
 * \brief ��ʼ��һ��SMTP�ͻ���
 *
 * \param[in] p_account  ָ����Ҫ��ʼ����SMTP�ͻ��˽ṹ��
 *
 * \retval AW_OK       ��ʼ���ɹ������Է����ʼ�
 * \retval -AW_EINVAL  �ṹ��������������ַ���û����������ֻ��ʹ��ASCII��ֵ
 *
 * \note ���ȷ����ʼ��ǰʹ��aw_smtp_mkclient()�������úñ�Ҫ������ֵ
 */
aw_err_t aw_smtp_client_init (aw_smtp_client_t  *p_client);

/**
 * \brief ��һ��SMTP�ͻ��˲���ʹ��ʱ��ʹ�øú������ʼ��
 *
 * \param[in] p_account  ָ����Ҫ���ʼ����SMTP�ͻ��˽ṹ��
 *
 * \retval  AW_OK      ���ʼ���ɹ�
 * \retval -AW_EINVAL  ��������
 */
aw_err_t aw_smtp_client_deinit (aw_smtp_client_t *p_client);

/**
 * \brief ����һ������ʼ�
 *
 * \param[in] p_account  ָ�����ߵ�SMTP�ͻ��˽ṹ��
 * \param[in] p_mail     ʹ��  aw_smtp_mkemail() �����õ�һ���ʼ�
 *
 * \retval  AW_OK      �����ʼ��ɹ�
 * \retval -AW_EINVAL  ��������
 * \retval -AW_ENOMEM  �ڴ治�㣬Ӧ����Ϊ�˻�������ڴ�
 */
aw_err_t aw_smtp_email_send (aw_smtp_client_t      *p_client,
                             const aw_smtp_email_t *p_email);

#ifdef __cplusplus
}
#endif

#endif /* __AW_SMTP_H */

/* end of file */
