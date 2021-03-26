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
 * \brief FatFs time interface.
 *
 * FAT time is put in a double word, the format of the DWORD:
 * bit[31:25] Year from 1980 (0..127)
 * bit[24:21] Month (1..12)
 * bit[20:16] Day in month(1..31)
 * bit[15:11] Hour (0..23)
 * bit[10: 5] Minute (0..59)
 * bit[ 4: 0] Second / 2 (0..29)
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-07  deo, first implementation
 * \endinternal
 */
#include "apollo.h"
#include "aw_fatfs.h"
#include "aw_time.h"

/** @brief convert fat time to time_t */
time_t aw_fattime_to_time (DWORD fattime)
{
    struct tm tm;

    /* The fat time is local time, no need to be converted */
    tm.tm_sec  = (int)((fattime & 0x1F) << 1);
    tm.tm_min  = (int)((fattime >>  5) & 0x3F);
    tm.tm_hour = (int)((fattime >> 11) & 0x1F);
    tm.tm_mday = (int)((fattime >> 16) & 0x1F);
    tm.tm_mon  = (int)((fattime >> 21) & 0x0F) - 1;  /* month begin with 1 */
    tm.tm_year = (int)((fattime >> 25) & 0x7F) + 80; /* year begin with 1980 */

    return mktime(&tm); /* convert to time_t */
}

/** @brief convert time_t to fat time */
DWORD aw_time_to_fattime (time_t *t)
{
    struct tm tm;

    (void)gmtime_r(t, &tm);

    if (tm.tm_year < 80) {
        tm.tm_year = 0;
    } else {
        tm.tm_year -= 80; /* year begin with 1980 */
    }
    tm.tm_mon += 1;  /* tm month begin with 0 */

    return ((tm.tm_year << 25) |
            (tm.tm_mon  << 21) |
            (tm.tm_mday << 16) |
            (tm.tm_hour << 11) |
            (tm.tm_min  << 5)  |
            (tm.tm_sec  /  2));
}

/** @brief Get current time for FatFs */
DWORD get_fattime (void)
{
    time_t t = aw_time(NULL);

    return aw_time_to_fattime(&t);
}

/*end of file */

