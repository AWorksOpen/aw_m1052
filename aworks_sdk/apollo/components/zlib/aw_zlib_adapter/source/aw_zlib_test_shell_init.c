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
 * \brief Zlib ���Դ�������
 *
 * \internal
 * \par Modification History
 * - 1.00 18-03-05  imp, first implementation
 * \endinternal
 */


#include "aw_shell.h"

/******************************************************************************/
extern int aw_zlib_test_minizip(int argc, char *argv[]);

#define __ARGS_NUM  10
#define __TEST_MINIZIP  "test_minizip"

/*
 * AWorks �� shell �������������еĵ�һ����������Ҫ����һ���µ�
 * �����б� new_argv[]��
 */ 
static int __test_minizip (int argc, char *argv[], struct aw_shell_user *sh)
{
    (void)sh;
    static char * const p_progname = __TEST_MINIZIP;
    char  *new_argv[__ARGS_NUM+2];  /* +2��Ϊ�˴�ų�������NULL */
    int    i;
    int    ret;

    if(argc > __ARGS_NUM) {    /* �����̶ܹ������Ĳ����� */ 
        argc = __ARGS_NUM;
    }
    for(i = 0; i < argc; ++i) {  /* ���� argv ��ĳ�Ա�� */ 
        new_argv[i+1] = argv[i];
    }
    new_argv[0] = p_progname;  /* ��������������ǰ�档 */ 
    new_argv[argc+1] = NULL;   /* �Ÿ� NULL�� */  
    argc++;                    /* ������������˸��������� */ 
    
    ret = aw_zlib_test_minizip (argc, new_argv);
    return (ret == 0) ? AW_OK : AW_ERROR;
}

static const struct aw_shell_cmd __g_test_minizip_shell_cmd_tbl[] = {
    {__test_minizip, __TEST_MINIZIP, ""},
};


/******************************************************************************/
extern int aw_zlib_test_example(int argc, char *argv[]);
#define __TEST_EXAMPLE  "test_example"

static int __test_example (int argc, char *argv[], struct aw_shell_user *sh)
{
    (void)sh;
    static char * const p_progname = __TEST_EXAMPLE;
    char  *new_argv[__ARGS_NUM+2];  /* +2��Ϊ�˴�ų�������NULL */
    int    i;
    int    ret;

    if(argc > __ARGS_NUM) {    /* �����̶ܹ������Ĳ����� */ 
        argc = __ARGS_NUM;
    }
    for(i = 0; i < argc; ++i) {  /* ���� argv ��ĳ�Ա�� */ 
        new_argv[i+1] = argv[i];
    }
    new_argv[0] = p_progname;  /* ��������������ǰ�档 */ 
    new_argv[argc+1] = NULL;   /* �Ÿ� NULL�� */  
    argc++;                    /* ������������˸��������� */ 
    
    ret = aw_zlib_test_example (argc, new_argv);
    return (ret == 0) ? AW_OK : AW_ERROR;
}

static const struct aw_shell_cmd __g_test_example_shell_cmd_tbl[] = {
    {__test_example, __TEST_EXAMPLE, "<output.gz> "},
};


/******************************************************************************/
extern int aw_zlib_test_infcover(int argc, char *argv[]);
#define __TEST_INFCOVER  "test_infcover"

static int __test_infcover (int argc, char *argv[], struct aw_shell_user *sh)
{
    (void)sh;
    static char * const p_progname = __TEST_INFCOVER;
    char  *new_argv[__ARGS_NUM+2];  /* +2��Ϊ�˴�ų�������NULL */
    int    i;
    int    ret;

    if(argc > __ARGS_NUM) {    /* �����̶ܹ������Ĳ����� */ 
        argc = __ARGS_NUM;
    }
    for(i = 0; i < argc; ++i) {  /* ���� argv ��ĳ�Ա�� */ 
        new_argv[i+1] = argv[i];
    }
    new_argv[0] = p_progname;  /* ��������������ǰ�档 */ 
    new_argv[argc+1] = NULL;   /* �Ÿ� NULL�� */  
    argc++;                    /* ������������˸��������� */ 
    
    ret = aw_zlib_test_infcover (argc, new_argv);
    return (ret == 0) ? AW_OK : AW_ERROR;
}

static const struct aw_shell_cmd __g_test_infcover_shell_cmd_tbl[] = {
    {__test_infcover, __TEST_INFCOVER, ""},
};

/******************************************************************************/


/*
 * ע�����
 */ 
void aw_zlib_test_shell_init (void)
{
    static struct aw_shell_cmd_list cl1;
    static struct aw_shell_cmd_list cl2;
    static struct aw_shell_cmd_list cl3;

    AW_SHELL_REGISTER_CMDS(&cl1, __g_test_minizip_shell_cmd_tbl);
    AW_SHELL_REGISTER_CMDS(&cl2, __g_test_example_shell_cmd_tbl);
    AW_SHELL_REGISTER_CMDS(&cl3, __g_test_infcover_shell_cmd_tbl);
}

/* end of file */
