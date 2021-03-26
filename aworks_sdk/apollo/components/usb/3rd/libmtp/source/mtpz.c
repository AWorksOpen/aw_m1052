/**
 * \file mtpz.c
 *
 * Copyright (C) 2011-2012 Sajid Anwar <sajidanwar94@gmail.com>
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
 * This file provides mtp zune cryptographic setup interfaces.
 * It is also used with Windows Phone 7, but Microsoft/Nokiad seem
 * to have discontinued MTPZ on Windows Phone 8.
 *
 * DISCLAIMER:
 *
 * The intention of this implementation is for users to be able
 * to interoperate with their devices, i.e. copy music to them in
 * operating systems other than Microsoft Windows, so it can be
 * played back on the device. We do not provide encryption keys
 * and constants in libmtp, we never will. You have to have these
 * on file in your home directory in $HOME/.mtpz-data, and we suggest
 * that you talk to Microsoft about providing the proper numbers if
 * you want to use this facility.
 */
#include "util.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

static char *MTPZ_ENCRYPTION_KEY;
static unsigned char *MTPZ_PUBLIC_EXPONENT;
static unsigned char *MTPZ_MODULUS;
static unsigned char *MTPZ_PRIVATE_KEY;
static char *MTPZ_CERTIFICATES;

/* 从fgets中去掉尾随的换行符*/
static char *fgets_strip(char * str, int num, FILE * stream)
{
    char *result = str;

    if ((result = fgets(str, num, stream)))
    {
        size_t newlen = strlen(result);

        if (result[newlen - 1] == '\n')
            result[newlen - 1] = '\0';
    }

    return result;
}

static char *hex_to_bytes(char *hex, size_t len)
{
    if (len % 2)
        return NULL;

    char *bytes = mtp_malloc(len / 2);
    unsigned int u;
    int i = 0;

    while (i < len && sscanf(hex + i, "%2x", &u) == 1)
    {
        bytes[i / 2] = u;
        i += 2;
    }

    return bytes;
}

int mtpz_loaddata(void)
{
#ifdef AWORKS
    char *home = mtp_get_home_path();
#else
    char *home = getenv("HOME");
#endif

    int ret = -1;
    if (!home)
    {
        LIBMTP_ERROR("Unable to determine user's home directory, MTPZ disabled.\n");
        return -1;
    }

    int plen = strlen(home) + strlen("/.mtpz-data") + 1;
    char path[plen];
    sprintf(path, "%s/.mtpz-data", home);

    FILE *fdata = fopen(path, "r");
    if (!fdata)
        return ret;

    /* 长度应该只有6个字符，但fgets将遇到换行符并停止*/
    MTPZ_PUBLIC_EXPONENT = (unsigned char *)fgets_strip((char *)mtp_malloc(8), 8, fdata);
    if (!MTPZ_PUBLIC_EXPONENT)
    {
        LIBMTP_ERROR("Unable to read MTPZ public exponent from ~/.mtpz-data, MTPZ disabled.\n");
        goto cleanup;
    }

    /* 长度应该只有33个字符，但fgets将遇到换行符并停止*/
    char *hexenckey = fgets_strip((char *)mtp_malloc(35), 35, fdata);
    if (!hexenckey)
    {
        LIBMTP_ERROR("Unable to read MTPZ encryption key from ~/.mtpz-data, MTPZ disabled.\n");
        goto cleanup;
    }

    MTPZ_ENCRYPTION_KEY = hex_to_bytes(hexenckey, strlen(hexenckey));
    if (!MTPZ_ENCRYPTION_KEY)
    {
        LIBMTP_ERROR("Unable to read MTPZ encryption key from ~/.mtpz-data, MTPZ disabled.\n");
        goto cleanup;
    }

    /* 长度应该只有256个字符，但fgets将遇到换行符并停止*/
    MTPZ_MODULUS = (unsigned char *)fgets_strip((char *)mtp_malloc(260), 260, fdata);
    if (!MTPZ_MODULUS)
    {
        LIBMTP_ERROR("Unable to read MTPZ modulus from ~/.mtpz-data, MTPZ disabled.\n");
        goto cleanup;
    }

    /* 长度应该只有256个字符，但fgets将遇到换行符并停止*/
    MTPZ_PRIVATE_KEY = (unsigned char *)fgets_strip((char *)mtp_malloc(260), 260, fdata);
    if (!MTPZ_PRIVATE_KEY)
    {
        LIBMTP_ERROR("Unable to read MTPZ private key from ~/.mtpz-data, MTPZ disabled.\n");
        goto cleanup;
    }

    /* 长度应该只有1258个字符，但fgets将遇到换行符并停止*/
    char *hexcerts = fgets_strip((char *)mtp_malloc(1260), 1260, fdata);
    if (!hexcerts)
    {
        LIBMTP_ERROR("Unable to read MTPZ certificates from ~/.mtpz-data, MTPZ disabled.\n");
        goto cleanup;
    }

    MTPZ_CERTIFICATES = hex_to_bytes(hexcerts, strlen(hexcerts));
    if (!MTPZ_CERTIFICATES)
    {
        LIBMTP_ERROR("Unable to parse MTPZ certificates from ~/.mtpz-data, MTPZ disabled.\n");
        goto cleanup;
    }
    /* 没有错误则返回0*/
    ret = 0;
cleanup:
    fclose(fdata);
    return ret;
}
