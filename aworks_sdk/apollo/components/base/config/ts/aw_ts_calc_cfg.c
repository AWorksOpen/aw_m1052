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
 * \brief 触摸屏校准数据存储及分析
 *
 * \internal
 * \par modification history
 * - 1.00 18-03-22  nwt, first implementation.
 * \endinternal
 */
#include "aworks.h"
#include "aw_ts.h"
#include "aw_nvram.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "string.h"


#define __MOUNT_NAME "/syscfg"
#define __FILE_NAME  __MOUNT_NAME"/cal_data"
#define __DEV_NAME   "/dev/mtd-syscfg"

//#ifdef AW_DEV_IMX6UL_TS
//#define __FS_EN 1
//#else
//#define __FS_EN 0
//#endif

#if defined AW_DEV_IMX6UL_TS || \
    defined AW_DEV_TS2046
#define __FS_EN 1
#else
#define __FS_EN 0
#endif

#ifndef SYS_TS_ID
#define SYS_TS_ID
#endif

#if defined (AW_DEV_TFT_4_3)
aw_local const char *__g_ts_dev = "TFT_4_3"SYS_TS_ID;
#elif defined (AW_DEV_AP_TFT7_0)
aw_local const char *__g_ts_dev = "AP_TFT7_0"SYS_TS_ID;
#elif defined (AW_DEV_LCD_TM070RDH12_24B)
aw_local const char *__g_ts_dev = "LCD_TM070RDH12_24B"SYS_TS_ID;
#elif defined (AW_DEV_LCD_640480W056TR)
aw_local const char *__g_ts_dev = "LCD_640480W056TR"SYS_TS_ID;
#elif defined (AW_DEV_LCD_800600W080TR)
aw_local const char *__g_ts_dev = "LCD_800600W080TR"SYS_TS_ID;
#elif defined (AW_DEV_HW800480F)
aw_local const char *__g_ts_dev = "TS800X480"SYS_TS_ID;
#elif defined (AW_DEV_HW480272F)
aw_local const char *__g_ts_dev = "TS480X272"SYS_TS_ID;
#else
aw_local const char *__g_ts_dev = "TSnone"SYS_TS_ID;
#endif



#if __FS_EN
aw_local void __fs_init (void)
{
    aw_err_t                ret;
    int                     retry = 3;
    struct aw_fs_format_arg arg = {"nand", 4096, 0};
    static aw_bool_t           init = AW_FALSE;

    if (init) {
        return;
    }

    do {
        ret = aw_mount(__MOUNT_NAME, __DEV_NAME, "lffs", 0);
        if (ret != AW_OK) {
            ret = aw_make_fs(__DEV_NAME, "lffs", &arg);
            if (ret != AW_OK) {
                return;
            }
        }
    } while ((ret != AW_OK) && retry--);

    if (ret == AW_OK) {
        init = AW_TRUE;
    }
}
#endif


#ifdef AW_COM_SHELL_SERIAL
#include "aw_shell.h"

aw_local int __ts_cmd_clr (int argc, char *argv[], struct aw_shell_user *sh)
{
    aw_err_t ret;
    char  invalid = '\0';
#if __FS_EN
    int fd = -1;
    fd = aw_open(__FILE_NAME, O_RDWR | O_CREAT | O_TRUNC, 7777);
    if (fd >= 0) {
        ret = aw_write(fd, &invalid, 1);
        if (ret == 1) {
            ret = AW_OK;
        } else {
            ret = -AW_EIO;
        }
    }

    if (fd >= 0) {
        aw_close(fd);
    }
#else
    return aw_nvram_set("calc_data", 0, &invalid, 0, 1);
#endif
}

aw_shell_cmd_item_t __g_ts_shell_cmds[] = {
    {__ts_cmd_clr, "tsclr", "clean touch calibrate data saved"},
};

aw_shell_cmd_list_t __g_ts_shell_obj;

aw_local void __ts_shell_init (void)
{
    static aw_bool_t init = AW_FALSE;

    if (init) {
        return;
    }

    aw_shell_register_cmds(&__g_ts_shell_obj,
                           &__g_ts_shell_cmds[0],
                           &__g_ts_shell_cmds[AW_NELEMENTS(__g_ts_shell_cmds)]);

    init = AW_TRUE;
}
#endif



int ts_calc_data_read (void *p_cacl_info, int len)
{
    int  ret = AW_OK, flag_len;
    char buf[64];

#if __FS_EN
    int fd = -1;
#endif



    flag_len = strlen(__g_ts_dev);

#if __FS_EN
    fd = aw_open(__FILE_NAME, O_RDONLY, 7777);
    if (fd >= 0) {
        ret = aw_read(fd, buf, flag_len);
        if (ret == flag_len) {
            if (strncmp(buf, __g_ts_dev, flag_len) == 0) {
                ret = aw_read(fd, p_cacl_info, len);
                if (ret != len) {
                    ret = -AW_EIO;
                } else {
                    ret = AW_OK;
                }
            } else {
                ret = -AW_EBADF;
            }
        } else {
            ret = -AW_EIO;
        }
    }

    if (fd >= 0) {
        aw_close(fd);
    }
#else
    ret = aw_nvram_get("calc_data", 0, buf, 0, flag_len);
    if (strncmp(buf, __g_ts_dev, flag_len)) {
        return -AW_EINVAL;
    }
    ret = aw_nvram_get("calc_data", 0, (char *)p_cacl_info, flag_len, len);
#endif

    return ret;
}


int ts_calc_data_write (void *p_cacl_info, int len)
{
    int ret = AW_OK, flag_len;

#if __FS_EN
    int fd = -1;
#endif

    flag_len = strlen(__g_ts_dev);

#if __FS_EN
    fd = aw_open(__FILE_NAME, O_RDWR | O_CREAT | O_TRUNC, 7777);
    if (fd >= 0) {
        ret = aw_write(fd, __g_ts_dev, flag_len);
        if (ret == flag_len) {
            ret = aw_write(fd, p_cacl_info, len);
            if (ret != len) {
                ret = -AW_EIO;
            } else {
                ret = AW_OK;
            }
        } else {
            ret = -AW_EIO;
        }
    }

    if (fd >= 0) {
        aw_close(fd);
    }
#else
    ret = aw_nvram_set("calc_data", 0, (char *)__g_ts_dev, 0, flag_len);
    if (ret != AW_OK) {
        return ret;
    }

    ret = aw_nvram_set("calc_data", 0, (char *)p_cacl_info, flag_len, len);
#endif

    return ret;
}



void aw_ts_cfg_init (void)
{
#ifdef AW_COM_SHELL_SERIAL
    __ts_shell_init();
#endif

#if __FS_EN
    __fs_init();
#endif
}


