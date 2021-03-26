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
 * \brief 通过shell命令获取SDK版本
 *
 * - 操作步骤：
 *   1. 在shell界面进入".test"子目录后输入"version"。
 *
 * - 实验现象
 *   1. 界面上打印出SDK版本信息。
 *
 * \par 源代码
 * \snippet app_version.c app_version
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_version] */
#include "aworks.h"                     /* 此头文件必须被首先包含 */
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
