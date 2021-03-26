/*******************************************************************************
*                                 AWorks
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
 * \brief FTPD�ļ�����Э������(�����)
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_FTPD
 *      - ����
 *      - ���Դ���
 *      - AW_COM_CONSOLE
 *   2. ��PC�������뿪��������0�������޸�IP��ַ����֤��������PC��ͬ��������
 *   3. ���ӵ��Դ��ڵ�PC
 *   4. ʹ��FTPǰ����Ҫ���洢���������ļ�ϵͳ�У���ʹ��FAT����SD�����õ�һ�����ص�
 *   5. ��PC���ļ�������������"ftp://������IP"����"ftp://172.16.18.10"��
 *      �ڵ����ĻỰ���У������˺ź������¼���˻�ΪAWorks������Ϊ1234
 *
 * - ʵ������
 *   1. ������PC�˷��ʿ�������SD���е��ļ���
 *
 * - ��ע��
 *   1. ��֤�����뿪���崦��ͬһ�����Σ�����ͨ���޸Ķ�Ӧ���ڵ������ļ����޸�ip��
 *      �����ļ�Ϊawbl_hwconf_xxx_enet.h��xxxΪƽ̨�ͺš�
 *
 * \par Դ����
 * \snippet demo_ftpd_server_auth.c src_ftpd_server_auth
 *
 * \internal
 * \par Modification history
 * - 1.00 16-04-07  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_ftpd_server_auth Ftpd�����(auth)
 * \copydoc demo_ftpd_server_auth.c
 */

/** [src_ftpd_server_auth] */
#include "aworks.h"
#include "aw_ftpd.h"
#include <string.h>

#define __FTPD_LISTEN_PORT     21
#define __FTPD_USER_NAME       "AWorks"
#define __FTPD_USER_PASSWORD   "1234"

aw_local aw_bool_t __ftpd_check_user (const char *user_name, const char *pass_word)
{
    if (0 == strcmp(user_name, __FTPD_USER_NAME)) {
        if (0 == strcmp(pass_word, __FTPD_USER_PASSWORD)) {
            return AW_TRUE;
        }
    }
    return AW_FALSE;
}


/**
 * \brief FTPD�������
 * \return ��
 */
void demo_ftpd_server_auth_entry (void)
{
    /* ���÷���·��Ϊ"/", �˿ں�Ϊ21��__FTPD_LISTEN_PORT������֤����   */
    aw_ftpd_init("/", __FTPD_LISTEN_PORT, 0, __ftpd_check_user);
}

/** [src_ftpd_server_auth] */

/* end of file */
