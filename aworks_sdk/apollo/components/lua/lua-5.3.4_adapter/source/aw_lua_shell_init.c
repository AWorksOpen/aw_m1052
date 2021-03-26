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
 *  Lua ��������
 *
 * \internal
 * \par modification history:
 * - 1.00 17-12-15 imp, first implemetation
 * \endinternal
 */

#include <stdlib.h>

#include "aw_common.h"
#include "aw_shell.h"

#define __LUA  "lua"

#if !defined(LUA_ARGS_NUM)
#define  LUA_ARGS_NUM     20  /* �����д�����������ֵ */ 
#endif 

/*
 * Ҫ�� lua.c �� main() ��ڳ��򣬸ĳ� lua_main()��
 */ 
extern int lua_main (int argc, char **argv);

/*
 * AWorks �� shell �������������еĵ�һ����������Ҫ����һ���µ�
 * �����б� new_argv[]��
 */ 
int aw_lua (int argc, char **argv)
{
    static char * const p_progname = __LUA;
    char  *new_argv[LUA_ARGS_NUM+2];  /* +2��Ϊ�˴�ų�������NULL */
    int    i;
    int    ret;

    if(argc > LUA_ARGS_NUM) {    /* �����̶ܹ������Ĳ����� */ 
        argc = LUA_ARGS_NUM;
    }
    for(i = 0; i < argc; ++i) {  /* ���� argv ��ĳ�Ա�� */ 
        new_argv[i+1] = argv[i];
    }
    new_argv[0] = p_progname;  /* ��������������ǰ�档 */ 
    new_argv[argc+1] = NULL;   /* �Ÿ� NULL�� */  
    argc++;                    /* ������������˸��������� */ 
    
    ret = lua_main(argc, new_argv);
    return (ret == EXIT_SUCCESS) ? AW_OK : AW_ERROR;
}


static int __lua (int argc, char *argv[], struct aw_shell_user *sh)
{
    (void)sh;
    return aw_lua(argc, argv);
}

static const struct aw_shell_cmd __g_lua_shell_cmd_tbl[] = {
    {__lua,   __LUA,    "lua command:lua file.lua"},
};

/*
 * ע�����
 */ 
void aw_lua_shell_init (void)
{
    static struct aw_shell_cmd_list cl;

    AW_SHELL_REGISTER_CMDS(&cl, __g_lua_shell_cmd_tbl);
}

/* end of file */
