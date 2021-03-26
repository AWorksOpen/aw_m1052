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

#include "clk_provider.h"

/*
 * DOC: basic fixed multiplier and divider clock that cannot gate
 *
 * Traits of this clock:
 * prepare - clk_prepare only ensures that parents are prepared
 * enable - clk_enable only ensures that parents are enabled
 * rate - rate is fixed.  clk->rate = parent->rate / div * mult
 * parent - fixed parent.  No clk_set_parent support
 */

#define to_clk_fixed_factor(_hw) container_of(_hw, struct clk_fixed_factor, hw)

static unsigned long clk_factor_recalc_rate(struct clk *hw,
        unsigned long parent_rate)
{
    struct clk_fixed_factor *fix = to_clk_fixed_factor(hw);
    unsigned long long int rate;

    rate = (unsigned long long int)parent_rate * fix->mult;
    rate /=fix->div ;
    return (unsigned long)rate;
}

static aw_clk_rate_t clk_factor_round_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t *prate)
{
    struct clk_fixed_factor *fix = to_clk_fixed_factor(hw);

    if (__clk_get_flags(hw) & CLK_SET_RATE_PARENT) {
        unsigned long best_parent;

        best_parent = (rate / fix->mult) * fix->div;
        *prate = __clk_round_rate(__clk_get_parent(hw),
                best_parent);
    }

    return (*prate / fix->div) * fix->mult;
}

static int clk_factor_set_rate(struct clk *hw, unsigned long rate,
                unsigned long parent_rate)
{
    return 0;
}

const static struct clk_ops clk_fixed_factor_ops = {
    .round_rate = clk_factor_round_rate,
    .set_rate = clk_factor_set_rate,
    .recalc_rate = clk_factor_recalc_rate,
};

struct clk *clk_register_fixed_factor(
        struct clk_fixed_factor *fix,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        unsigned long flags,
        unsigned int mult, unsigned int div)
{

    struct clk_init_data    init;
    struct clk             *clk;

    /* struct clk_fixed_factor assignments */
    fix->mult = mult;
    fix->div = div;


    init.name = name;
    init.ops = &clk_fixed_factor_ops;
    init.flags = flags | CLK_IS_BASIC;
    init.parent_name = parent_name;
    init.clk_id = clk_id;


    clk = clk_register_hw(&fix->hw, &init);


    return clk;
}

/* end of file */
