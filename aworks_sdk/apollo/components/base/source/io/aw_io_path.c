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
 * \brief 工作路径管理
 *
 * \internal
 * \par modification history:
 * - 1.00 17-03-24  deo, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_task.h"
#include "io/aw_io_path.h"
#include "aw_mem.h"
#include "string.h"


aw_local char __g_pwd[AW_IO_PATH_LEN_MAX] = "/";




/******************************************************************************/
char *aw_io_pwd (void)
{
#if AW_TASK_TLS_EN

#else
    return __g_pwd;
#endif
}




/******************************************************************************/
void aw_io_pwd_set (char *path)
{
#if AW_TASK_TLS_EN

#else
    strcpy(__g_pwd, path);
#endif
}




/******************************************************************************/
static aw_err_t __path_make (char *cwd, const char *path)
{
    aw_err_t ret = 0;
    int      len = strlen(cwd) - 1;
    char     ch;

    /* is an absolute path */
    if ('/' == *path) {
        len = 0;
    } else if ('/' != cwd[len]) {
        /* add a '/' to end of path */
        cwd[++len] = '/';
    }

    while ((ch = *path++) != '\0') {
        if ('/' == ch) {
            if ('/' != cwd[len]) {
                cwd[++len] = '/';
            }
            continue;
        } else if ('.' != ch) {
            /* just '.' */
            cwd[++len] = ch;
            continue;
        }

        /* "./" */
        if ('/' == *path) {
            path++;
            continue;
        }

        /* ".x", x != '.' */
        if ('.' != *path) {
            cwd[++len] = '.';
            continue;
        }

        /* ".." or "../" */
        path++;
        if (('\0' == *path) || ('/' == *path)) {
            if (len < 2) {
                /* if we reach the root '/' */
                ret = -AW_EINVAL;
                break;
            }
            if ('/' == cwd[len]) {
                /* ignore the last '/' in path */
                len--;
            }

            while (cwd[len] != '/') {
                len--;
            }

        } else { /* invalid path */
            ret = -AW_EINVAL;
            break;
        }
    }

    /* remove the last '/' */
    if ((len > 0) && ('/' == cwd[len])) {
        cwd[len] = '\0';
    } else {
        cwd[len + 1] = '\0';
    }
    return ret;
}




/******************************************************************************/
aw_err_t aw_io_full_path (char *full, const char *path)
{
    char *pwd;

    if (path[0] != '/') {
        /* get work directory */
        pwd = aw_io_pwd();
        if (pwd == NULL) {
            return -AW_ENOENT;
        }
        strcpy(full, pwd);
    } else {
        full[0] = '/';
        full[1] = '\0';
    }

    __path_make(full, path);

    return AW_OK;
}









