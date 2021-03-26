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
 * Lua ��������
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
#define  LUAC_ARGS_NUM     20  /* �����д�����������ֵ */ 
#endif 

/*
** Ҫ�� luac.c �� main() ��ڳ��򣬸ĳ� luac_main()��
**/ 
extern int luac_main (int argc, char **argv);

int aw_luac (int argc, char **argv)
{
    static char * const p_progname = __LUAC;
    char  *new_argv[LUAC_ARGS_NUM+2];  /* +2��Ϊ�˴�ų�������NULL */
    int    i;
    int    ret;

    if(argc > LUAC_ARGS_NUM) {   /* �����̶ܹ������Ĳ����� */ 
        argc = LUAC_ARGS_NUM;
    }
    for(i = 0; i < argc; ++i) {  /* ���� argv ��ĳ�Ա�� */ 
        new_argv[i+1] = argv[i];
    }
    new_argv[0] = p_progname;  /* ��������������ǰ�档 */ 
    new_argv[argc+1] = NULL;   /* �Ÿ� NULL�� */  
    argc++;                    /* ������������˸��������� */ 
    
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
* ע�����
*/ 
void aw_luac_shell_init (void)
{
    static struct aw_shell_cmd_list cl;

    AW_SHELL_REGISTER_CMDS(&cl, __g_luac_shell_cmd_tbl);
}


/* end of file */
