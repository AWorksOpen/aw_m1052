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

#include "simple_div_const_clk.h"
#include "aw_bitops.h"

#define div_mask(d) ((1 << ((d)->width)) - 1)
#define ULONG_MAX 0xFFFFFFFF

aw_clk_rate_t simple_div_const_clk_recalc_rate(
        struct clk *hw,
        aw_clk_rate_t parent_rate,
        uint32_t div)
{
    return parent_rate/div;
}

/* end of file */
