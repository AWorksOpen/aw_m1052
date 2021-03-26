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

#ifndef         __SIMPLE_DIV_CONST_CLK_H__
#define         __SIMPLE_DIV_CONST_CLK_H__

#include "clk_provider.h"


aw_clk_rate_t simple_div_const_clk_recalc_rate(
        struct clk *hw,
        aw_clk_rate_t parent_rate,
        uint32_t div);

#define     SIPMLE_DIV_CONST_CLK_DEFS(name,div) \
    static aw_clk_rate_t name##_recalc_rate(\
        struct clk *hw,aw_clk_rate_t parent_rate) \
    { \
        return simple_div_const_clk_recalc_rate(hw,parent_rate,(div) ); \
    } \
    static const struct clk_ops name##_ops = { \
        .recalc_rate = name##_recalc_rate, \
    };

#endif

/* end of file */
