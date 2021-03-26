/**
 * \file unicode.c
 *
 * This file contains general Unicode string manipulation functions.
 * It mainly consist of functions for converting between UCS-2 (used on
 * the devices) and UTF-8 (used by several applications).
 *
 * For a deeper understanding of Unicode encoding formats see the
 * Wikipedia entries for
 * <a href="http://en.wikipedia.org/wiki/UTF-16/UCS-2">UTF-16/UCS-2</a>
 * and <a href="http://en.wikipedia.org/wiki/UTF-8">UTF-8</a>.
 *
 * Copyright (C) 2005-2009 Linus Walleij <triad@df.lth.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

//#include "config.h"

#include <stdlib.h>
#include <string.h>
#ifdef HAVE_ICONV
#include "iconv.h"
//#else
//#error "libmtp unicode.c needs fixing to work without iconv()!"
#endif
#include "libmtp.h"
#include "unicode.h"
#include "util.h"
#include "ptp.h"

/**
 * 用于创建字符串副本的缓冲区大小(以字节为单位)
 */
#define STRING_BUFFER_LENGTH 1024

#ifdef HAVE_ICONV
/**
 * 将一个大端的UTF-16 2字节字符串转换为UTF-8字符串。
 *
 * @param device 当前MTP设备
 * @param device 要转换的UTF-16 unicode字符串
 *
 * @return 成功返回一个UTF-8字符串
 **/
char *utf16_to_utf8(LIBMTP_mtpdevice_t *device, const u16 *unicstr)
{
    PTPParams *params = (PTPParams *) device->params;
    char *stringp = (char *) unicstr;
    char loclstr[STRING_BUFFER_LENGTH * 3 + 1]; /* UTF-8编码UCS2字符最多3个字节*/
    char *locp = loclstr;
    size_t nconv;
    size_t convlen = (ucs2_strlen(unicstr) + 1) * sizeof(u16); /* UCS-2是16位宽，包括终止符*/
    size_t convmax = STRING_BUFFER_LENGTH*3;

    loclstr[0]='\0';
    /* 进行转换*/
    nconv = iconv(params->cd_ucs2_to_locale, &stringp, &convlen, &locp, &convmax);
    if (nconv == (size_t) -1) {
        /* 无论如何返回部分字符串*/
        *locp = '\0';
    }
    loclstr[STRING_BUFFER_LENGTH*3] = '\0';
    // Strip off any BOM, it's totally useless...
    if ((u8) loclstr[0] == 0xEFU && (u8) loclstr[1] == 0xBBU && (u8) loclstr[2] == 0xBFU) {
        return strdup(loclstr+3);
    }
    return strdup(loclstr);
}

#endif

/**
 * 这个函数删除任何大于0x7F的连续字符，然后用下划线替换。在UTF-8中，>0x7F 的连续字符
 * 表示一个单独的字符，所以必须这么做。它只会缩小字符串的大小，因此不需要拷贝。
 *
 * @param str 需要转换的字符串
 **/
void strip_7bit_from_utf8(char *str)
{
    int i,j,k;
    i = 0;
    j = 0;
    k = strlen(str);
    while (i < k) {
        if ((u8) str[i] > 0x7FU) {
            str[j] = '_';
            i++;
            /* 跳过大于0x7F的连续字符*/
            while((u8) str[i] > 0x7FU) {
                i++;
            }
        } else {
            str[j] = str[i];
            i++;
        }
        j++;
    }
    /* 字符串终止*/
    str[j] = '\0';
}
