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
 * 主要是提供给 Lua 的 REPL 用于接收和回显字符串。
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
 * \brief 立即发送一个字符。
 */
static int dump_fputc(int c, FILE *stream)
{
    fputc(c, stream);
    fflush(stream);
    return 0;
}

/**
 * \brief 立即发送一行字符串。
 */
static int dump_fputs(const char *s, FILE *stream)
{
    fputs(s, stream);
    fflush(stream);
    return 0;
}

/**
 * \brief 轮询获取一个字符。
 */
static int dump_fgetc(FILE *stream)
{
    (void)stream;
    struct aw_shell_user *sh = aw_dbg_shell_user();

    while(1) { /* 等待成功获取一个字符。 */
        int ret = aw_shell_getc(sh);
        if(ret > 0) {
            return ret;
        }
    }
}

/**
 * \brief 阻塞获取一行字符。
 */
char *dump_fgets(char *s, int n, FILE *stream)
{
    char  *str = s;
    char  *end = s + n - 1;

    if(n <= 0) {   /* 缓冲区大小不足，返回 */
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
        case '\r': /* 回车，返回 */
        case '\n':
            dump_fputc('\n', stdout);
            *str = '\0';
            return s;
        case '\b': /* 退格，减少一个已获取的字符 */
            if(str != s){
                dump_fputs("\b \b", stdout);
                str--;
            }
            break;
        case '\3':  /* Ctrl-C 处理 */
            return NULL;
        default: 
            if (isprint(ret)) { /* 可读字符，添加到缓冲区 */
                dump_fputc(ret, stdout);
                *str = (char)ret;
                str++;
            }
        }
    }
}

/* end of file */
