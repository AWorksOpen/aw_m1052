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
 * Lua 编译器。
 *
 * \internal
 * \par modification history:
 * - 1.00 17-12-15 imp, first implemetation
 * \endinternal
 */
#include <stdlib.h>

#include "aw_common.h"
#include "aw_shell.h"
#define __LUAC      "luac"

#if !defined(LUAC_ARGS_NUM)
#define  LUAC_ARGS_NUM     20  /* 命令行传入参数的最大值 */ 
#endif 

/*
** 要求 luac.c 的 main() 入口程序，改成 luac_main()。
**/ 
extern int luac_main (int argc, char **argv);

int aw_luac (int argc, char **argv)
{
    static char * const p_progname = __LUAC;
    char  *new_argv[LUAC_ARGS_NUM+2];  /* +2是为了存放程序名和NULL */
    int    i;
    int    ret;

    if(argc > LUAC_ARGS_NUM) {   /* 最多接受固定个数的参数。 */ 
        argc = LUAC_ARGS_NUM;
    }
    for(i = 0; i < argc; ++i) {  /* 后移 argv 里的成员。 */ 
        new_argv[i+1] = argv[i];
    }
    new_argv[0] = p_progname;  /* 将程序名放在最前面。 */ 
    new_argv[argc+1] = NULL;   /* 放个 NULL。 */  
    argc++;                    /* 传入参数，多了个程序名。 */ 
    
    ret = luac_main(argc, new_argv);
    return (ret == EXIT_SUCCESS) ? AW_OK : AW_ERROR; 
}


static int __luac (int argc, char *argv[], struct aw_shell_user *sh)
{
    (void)sh;
    return aw_luac(argc, argv);
}

static const struct aw_shell_cmd __g_luac_shell_cmd_tbl[] = {
    {__luac,   __LUAC,    "luac command:luac file.lua"},
};

/*
* 注册命令。
*/ 
void aw_luac_shell_init (void)
{
    static struct aw_shell_cmd_list cl;

    AW_SHELL_REGISTER_CMDS(&cl, __g_luac_shell_cmd_tbl);
}


/* end of file */
