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
 * \brief 通过shell命令操作SPI Flash
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - 对应平台的Flash设备宏
 *   2. 在shell界面进入".test"子目录后输入"sflash -w 偏移地址   数据"往指定偏移地址处写入数据，如：
 *      sflash -w 0 AWorks
 *   3. 输入"sflash -r 偏移地址 数据个数"从指定偏移地址处读出指定个数据，如：
 *      sflash -r 0 6
 *
 * - 备注：
 *   1. 定义的__APP_NVRAM_ITEM的名字必须与flash配置头文件中的存储段名称一致。
 *
 * \par 源代码
 * \snippet app_sflash.c app_sflash
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_sflash] */

#include "aworks.h"
#include "string.h"
#include "aw_shell.h"
#include "aw_nvram.h"
#include "aw_vdebug.h"
#include "app_config.h"
#include "aw_demo_config.h"

/* 定义存储段名称 */
#define __APP_NVRAM_ITEM    DE_FLEXSPI_NVRAM_NAME


#ifdef APP_NVRAM
/******************************************************************************/
int app_sflash (int argc, char **argv, struct aw_shell_user *p_user)
{
    int       offs, len;
    char      buf[255];
    aw_err_t  ret;

    if (aw_shell_args_get(p_user, "-w", 2) == AW_OK) {
        char *pdat;
        /* 获取写入偏移量和数据 */
        offs = aw_shell_long_arg(p_user, 1);
        pdat = aw_shell_str_arg(p_user, 2);
        if (pdat) {
            len = strlen(pdat);
        }

        /* 写入数据 */
        ret = aw_nvram_set(__APP_NVRAM_ITEM, 0, pdat, offs, len);
        if (ret != AW_OK) {
            aw_kprintf("SPI-Flash write failed %d.\n", ret);
        } else {
            aw_kprintf("SPI-Flash write ok(%d).\n", len);
        }

    } else if (aw_shell_args_get(p_user, "-r", 2) == AW_OK) {
        /* 获取读取偏移和数据长度 */
        offs = aw_shell_long_arg(p_user, 1);
        len  = aw_shell_long_arg(p_user, 2);
        len = min(sizeof(buf), len);

        /* 读取数据 */
        ret = aw_nvram_get(__APP_NVRAM_ITEM, 0, buf, offs, len);
        if (ret != AW_OK) {
            aw_kprintf("SPI-Flash read failed %d.\n", ret);
        } else {
            aw_kprintf("SPI-Flash read ok(%d).\n", len);
            buf[len] = '\0';
            aw_kprintf("data: %s\n", buf);
        }
    } else {
        return -AW_EINVAL;
    }

    return AW_OK;
}

/** [src_app_sflash] */
#endif /* APP_SFLASH */
