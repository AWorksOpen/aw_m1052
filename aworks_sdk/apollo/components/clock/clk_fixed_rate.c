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

#include "apollo.h"
#include "clk_provider.h"
#include <string.h>

static aw_clk_rate_t clk_fixed_rate_recalc_rate(struct clk   *hw,
                                                aw_clk_rate_t parent_rate)
{
    struct clk_fixed_rate          *p_fixed_rate_clk;

    p_fixed_rate_clk = AW_CONTAINER_OF(hw, struct clk_fixed_rate, hw);
#ifdef CONFIG_CLK_NO_CACHE_RATE
    return p_fixed_rate_clk->fixed_rate;
#else 
    return hw->rate;
#endif      /* CONFIG_CLK_NO_CACHE_RATE */
}

static const struct clk_ops __clk_fixed_rate_ops = {
        .recalc_rate = clk_fixed_rate_recalc_rate,
};

struct clk *clk_register_fixed_rate(
        struct clk_fixed_rate *p_clk_fixed_rate,
        aw_clk_id_t clk_id,
        const char *name,
        unsigned long flags,
        unsigned long fixed_rate)
{
    struct clk_init_data        init = {0};
    struct clk                 *hw;


    init.name = name;
    init.clk_id = clk_id;
    init.parent_name = NULL;
    init.flags = flags;
    init.ops = &__clk_fixed_rate_ops;

    hw = clk_register_hw(&p_clk_fixed_rate->hw,&init);

#ifdef CONFIG_CLK_NO_CACHE_RATE
    p_clk_fixed_rate ->fixed_rate = fixed_rate;
#else
    hw->rate = fixed_rate;
#endif
    return hw;
}

static aw_clk_rate_t clk_user_fixed_rate_get_rate(
        struct clk *hw,
        aw_clk_rate_t parent_rate)
{
    struct clk_user_fixed_rate     *p_user_fixed_rate_clk;
    p_user_fixed_rate_clk = AW_CONTAINER_OF(hw, struct clk_user_fixed_rate, hw);

#ifdef CONFIG_CLK_NO_CACHE_RATE
    return p_user_fixed_rate_clk->rate;
#else 
    return hw->rate;
#endif
}

static aw_err_t clk_user_fixed_rate_set_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate)
{
    struct clk_user_fixed_rate     *p_user_fixed_rate_clk;
    p_user_fixed_rate_clk = AW_CONTAINER_OF(hw, struct clk_user_fixed_rate, hw);
#ifdef CONFIG_CLK_NO_CACHE_RATE
    p_user_fixed_rate_clk->rate = rate;
#else
    hw->rate = rate;
#endif
    return AW_OK;
}

static aw_clk_rate_t clk_user_fixed_rate_round_rate(
                        struct clk *hw,
                        aw_clk_rate_t desired_rate,
                        aw_clk_rate_t *best_parent_rate)
{
    struct clk_user_fixed_rate         *p_user_fixed_rate_clk;
    p_user_fixed_rate_clk = AW_CONTAINER_OF(hw, struct clk_user_fixed_rate, hw);

#ifdef CONFIG_CLK_NO_CACHE_RATE
    return p_user_fixed_rate_clk->rate;
#else 
    return p_user_fixed_rate_clk->hw.rate;
#endif
}



static const struct clk_ops __clk_user_fixed_rate_ops = {
        .set_rate = clk_user_fixed_rate_set_rate,
        .recalc_rate = clk_user_fixed_rate_get_rate,
#if 0 
        .round_rate = clk_user_fixed_rate_round_rate,
#endif 
};

struct clk *clk_register_user_fixed_rate(
        struct clk_user_fixed_rate *p_clk_user_fixed_rate,
        aw_clk_id_t clk_id,
        const char *name,
        unsigned long flags)
{
    struct clk_init_data        init = {0};
    struct clk                 *hw;


    init.name = name;
    init.clk_id = clk_id;
    init.parent_name = NULL;
    init.flags = flags;
    init.ops = &__clk_user_fixed_rate_ops;

    hw = clk_register_hw(&p_clk_user_fixed_rate->hw,&init);
    return hw;
}

/* end of file */
