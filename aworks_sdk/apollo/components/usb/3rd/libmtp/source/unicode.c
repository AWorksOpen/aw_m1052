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
 * ���ڴ����ַ��������Ļ�������С(���ֽ�Ϊ��λ)
 */
#define STRING_BUFFER_LENGTH 1024

#ifdef HAVE_ICONV
/**
 * ��һ����˵�UTF-16 2�ֽ��ַ���ת��ΪUTF-8�ַ�����
 *
 * @param device ��ǰMTP�豸
 * @param device Ҫת����UTF-16 unicode�ַ���
 *
 * @return �ɹ�����һ��UTF-8�ַ���
 **/
char *utf16_to_utf8(LIBMTP_mtpdevice_t *device, const u16 *unicstr)
{
    PTPParams *params = (PTPParams *) device->params;
    char *stringp = (char *) unicstr;
    char loclstr[STRING_BUFFER_LENGTH * 3 + 1]; /* UTF-8����UCS2�ַ����3���ֽ�*/
    char *locp = loclstr;
    size_t nconv;
    size_t convlen = (ucs2_strlen(unicstr) + 1) * sizeof(u16); /* UCS-2��16λ��������ֹ��*/
    size_t convmax = STRING_BUFFER_LENGTH*3;

    loclstr[0]='\0';
    /* ����ת��*/
    nconv = iconv(params->cd_ucs2_to_locale, &stringp, &convlen, &locp, &convmax);
    if (nconv == (size_t) -1) {
        /* ������η��ز����ַ���*/
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
 * �������ɾ���κδ���0x7F�������ַ���Ȼ�����»����滻����UTF-8�У�>0x7F �������ַ�
 * ��ʾһ���������ַ������Ա�����ô������ֻ����С�ַ����Ĵ�С����˲���Ҫ������
 *
 * @param str ��Ҫת�����ַ���
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
            /* ��������0x7F�������ַ�*/
            while((u8) str[i] > 0x7FU) {
                i++;
            }
        } else {
            str[j] = str[i];
            i++;
        }
        j++;
    }
    /* �ַ�����ֹ*/
    str[j] = '\0';
}
