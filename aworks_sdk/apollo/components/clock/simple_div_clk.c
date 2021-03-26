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

#include "simple_div_clk.h"
#include "aw_bitops.h"

#define div_mask(d) ((1 << ((d)->width)) - 1)
#define ULONG_MAX 0xFFFFFFFF

aw_clk_rate_t simple_div_clk_recalc_rate(
        struct clk *hw,
        aw_clk_rate_t parent_rate,
        aw_const struct simple_div_clk_desc *p_desc)
{
    uint32_t                data;

    data = AW_REG_BITS_GET32(p_desc->reg,p_desc->shift,p_desc->width);
    if (p_desc->div_val_sub_one) {
        data ++;
    }

    return parent_rate/data;
}


static int _get_div(
        unsigned long parent_rate,
        unsigned long rate,
        aw_const struct simple_div_clk_desc *p_desc)
{
    uint32_t        div;
    div = parent_rate / rate;
    if (rate * div < parent_rate) {
        div ++;
    }
    
    if(div < p_desc->min_div) {
        div = p_desc->min_div;
    }

    if(div > p_desc->max_div) {
        div = p_desc->max_div;
    }

    return div;
}

aw_clk_rate_t simple_div_clk_round_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t *prate,
        aw_const struct simple_div_clk_desc *p_desc)
{
    uint32_t                div;
    div = _get_div((*prate),rate,p_desc);
    return (*prate) / div;
}

aw_err_t simple_div_set_rate(
        struct clk *hw,
        unsigned long rate,
        unsigned long parent_rate,
        aw_const struct simple_div_clk_desc *p_desc)
{
    uint32_t                div;

    div = _get_div(parent_rate,rate,p_desc);
    if (p_desc->div_val_sub_one) {
        div --;
    }

    AW_REG_BITS_SET32(p_desc->reg,p_desc->shift,p_desc->width,div);
    return AW_OK;
}

/* end of file */

