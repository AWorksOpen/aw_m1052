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
 * \brief 基于AWBus的时间戳定时器实现
 *
 * \internal
 * \par modification history
 * - 1.00 13-12-23  zen, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_hwtimer.h"
#include "awbl_timer.h"
#include "aw_timestamp.h"
#include "awbl_timestamp.h"
#include <string.h>

static aw_timestamp_t _default_timestamp_get(
        struct awbl_timestamp *p_awbl_timestamp)
{
    return 0;
}

static aw_const struct awbl_timestamp   _default_timstamp = {
        _default_timestamp_get,
        1,
};
static struct awbl_timestamp           *g_p_awbl_timestamp =
        (struct awbl_timestamp *)&_default_timstamp;

aw_timestamp_t aw_timestamp_get(void)
{
    /* aw_timestamp_get往往是一个要很高效率的场合，所以实现的时候不带参数检查 */
    return g_p_awbl_timestamp->fn_timestamp_get(g_p_awbl_timestamp);
}

aw_timestamp_freq_t aw_timestamp_freq_get(void)
{
    /* aw_timestamp_get往往是一个要很高效率的场合，所以实现的时候不带参数检查 */
    return g_p_awbl_timestamp->freq;
}

aw_err_t awbl_timestamp_dev_register(struct awbl_timestamp *p_awbl_timestamp)
{
    if ((struct awbl_timestamp *)&_default_timstamp == g_p_awbl_timestamp) {
        g_p_awbl_timestamp = p_awbl_timestamp;
        return AW_OK;
    }
    return AW_EEXIST;
}

uint32_t aw_timestamps_to_us(aw_timestamp_t stamps)
{
    aw_timestamp_freq_t     freq = aw_timestamp_freq_get();
    uint32_t                a,factor = 1;

    /* 为了方便计算中取整，首先计算出频率中可以被10指数幂的因子 */
    freq = aw_timestamp_freq_get();
    while (0 ==(freq % 10) ) {
        freq /= 10;
        factor *= 10;
    }

    /* 先尽量精简factor，以减少计算时的溢出 */
    a = 1000000;
    if (a > factor) {
        a = a/factor;
        factor = 1;
    }
    else {
        factor = factor/a;
        a = 1;
    }

    stamps *= a;
    freq *= factor;

    /* 四舍五入的处理 */
    stamps += (freq / 2);
    return (uint32_t)(stamps /freq);
}


uint32_t aw_timestamps_to_ns(aw_timestamp_t stamps)
{
    aw_timestamp_freq_t     freq = aw_timestamp_freq_get();
    uint32_t                a,factor = 1;

    /* 为了方便计算中取整，首先计算出频率中可以被10指数幂的因子 */
    freq = aw_timestamp_freq_get();
    while (0 ==(freq % 10) ) {
        freq /= 10;
        factor *= 10;
    }

    /* 先尽量精简factor，以减少计算时的溢出 */
    a = 1000000000;
    if (a > factor) {
        a = a/factor;
        factor = 1;
    }
    else {
        factor = factor/a;
        a = 1;
    }

    stamps *= a;
    freq *= factor;

    /* 四舍五入的处理 */
    stamps += (freq / 2);
    return (uint32_t)(stamps /freq);
}

/* end of file */
