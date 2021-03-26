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
 * ��Ҫ���ṩ�� Lua �� REPL ���ڽ��պͻ����ַ�����
 *
 * \internal
 * \par modification history:
 * - 1.00 17-12-15 imp, first implemetation
 * \endinternal
 */

#include <stdio.h>
#include <ctype.h>

#include "aw_delay.h"
#include "aw_shell.h"

/**
 * \brief ��������һ���ַ���
 */
static int dump_fputc(int c, FILE *stream)
{
    fputc(c, stream);
    fflush(stream);
    return 0;
}

/**
 * \brief ��������һ���ַ�����
 */
static int dump_fputs(const char *s, FILE *stream)
{
    fputs(s, stream);
    fflush(stream);
    return 0;
}

/**
 * \brief ��ѯ��ȡһ���ַ���
 */
static int dump_fgetc(FILE *stream)
{
    (void)stream;
    struct aw_shell_user *sh = aw_dbg_shell_user();

    while(1) { /* �ȴ��ɹ���ȡһ���ַ��� */
        int ret = aw_shell_getc(sh);
        if(ret > 0) {
            return ret;
        }
    }
}

/**
 * \brief ������ȡһ���ַ���
 */
char *dump_fgets(char *s, int n, FILE *stream)
{
    char  *str = s;
    char  *end = s + n - 1;

    if(n <= 0) {   /* ��������С���㣬���� */
        return s;
    } 
    while(1){
        int  ret;

        if(str == end){   
            *str = '\0';
            return s;
        }
        ret = dump_fgetc(stream);
        switch(ret) {
        case '\r': /* �س������� */
        case '\n':
            dump_fputc('\n', stdout);
            *str = '\0';
            return s;
        case '\b': /* �˸񣬼���һ���ѻ�ȡ���ַ� */
            if(str != s){
                dump_fputs("\b \b", stdout);
                str--;
            }
            break;
        case '\3':  /* Ctrl-C ���� */
            return NULL;
        default: 
            if (isprint(ret)) { /* �ɶ��ַ�����ӵ������� */
                dump_fputc(ret, stdout);
                *str = (char)ret;
                str++;
            }
        }
    }
}

/* end of file */
