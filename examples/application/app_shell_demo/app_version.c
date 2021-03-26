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
 * \brief ͨ��shell�����ȡSDK�汾
 *
 * - �������裺
 *   1. ��shell�������".test"��Ŀ¼������"version"��
 *
 * - ʵ������
 *   1. �����ϴ�ӡ��SDK�汾��Ϣ��
 *
 * \par Դ����
 * \snippet app_version.c app_version
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_version] */
#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
#include "aw_shell.h"
#include "aw_vdebug.h"
#include "aw_sdk_version.h"
#include "app_config.h"

#ifdef APP_VERSION
/******************************************************************************/
int app_version (int argc, char **argv, struct aw_shell_user *p_user)
{
    aw_kprintf("\r\n");
    aw_kprintf("AWorks SDK version: %s\r\n", aw_sdk_version_get());
    aw_kprintf("APP version:        v%2d.%02d %s %s\r\n", APP_VER >> 8, APP_VER & 0x00FF, __DATE__, __TIME__);
    return AW_OK;
}

/** [src_app_version] */
#endif /* APP_VERSION */
