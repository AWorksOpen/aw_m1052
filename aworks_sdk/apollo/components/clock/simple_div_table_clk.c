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

#include "simple_div_table_clk.h"
#include "aw_bitops.h"

#define div_mask(d) ((1 << ((d)->width)) - 1)
#define ULONG_MAX 0xFFFFFFFF

static unsigned int _get_table_div(const struct clk_div_table *table,
                            unsigned int val)
{
    const struct clk_div_table *clkt;

    for (clkt = table; clkt->div; clkt++)
        if (clkt->val == val)
            return clkt->div;
    return 0;
}

aw_clk_rate_t simple_div_table_clk_recalc_rate(
        struct clk *hw,
        aw_clk_rate_t parent_rate,
        aw_const struct simple_div_table_clk_desc *p_desc)
{
    uint32_t                data;

    data = AW_REG_BITS_GET32(p_desc->reg,p_desc->shift,p_desc->width);
    data = _get_table_div(p_desc->table,data);
    return parent_rate/data;
}


static int simple_div_table_bestdiv_index(
        struct clk *hw,
        unsigned long rate,
        unsigned long *best_parent_rate,
        aw_const struct simple_div_table_clk_desc *p_desc)
{
    unsigned long           diff ,r;
    unsigned long           div;
    int                     i;
    unsigned long           parent_rate = *best_parent_rate;
    unsigned long           best_table_index = 0;
    unsigned long           best_diff;

    div = p_desc->table[best_table_index].div;
    r = div * rate;
    best_diff = r > parent_rate?r -parent_rate:parent_rate -r;


    i = 1;
    while(1) {
        div = p_desc->table[i].div;
        if (0 == div) {
            break;
        }
        r = rate * div;
        diff =  r > parent_rate?r -parent_rate:parent_rate -r;
        if (diff < best_diff) {
            best_diff = diff;
            best_table_index = i;
        }
        i ++;
    }

    return best_table_index;
}

aw_clk_rate_t simple_div_table_clk_round_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t *prate,
        aw_const struct simple_div_table_clk_desc *p_desc)
{
    int i;
    i = simple_div_table_bestdiv_index(hw, rate, prate,p_desc);
    i = p_desc->table[i].div;

    return *prate / i;
}

aw_err_t simple_div_table_set_rate(
        struct clk *hw,
        unsigned long rate,
        unsigned long parent_rate,
        aw_const struct simple_div_table_clk_desc *p_desc)
{
    int             i;

    i = simple_div_table_bestdiv_index(hw, rate, &parent_rate,p_desc);
    i = p_desc->table[i].val;

    AW_REG_BITS_SET32(p_desc->reg,p_desc->shift,p_desc->width,i);
    return AW_OK;
}

/* end of file */