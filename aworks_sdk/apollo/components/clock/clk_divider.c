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
#include <string.h>

/*
 * DOC: basic adjustable divider clock that cannot gate
 *
 * Traits of this clock:
 * prepare - clk_prepare only ensures that parents are prepared
 * enable - clk_enable only ensures that parents are enabled
 * rate - rate is adjustable.  clk->rate = parent->rate / divisor
 * parent - fixed parent.  No clk_set_parent support
 */

#define to_clk_divider(_hw) container_of(_hw, struct clk_divider, hw)
#define div_mask(d) ((1 << ((d)->width)) - 1)
#define ULONG_MAX 0xFFFFFFFF

static aw_bool_t is_power_of_2(unsigned long n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

static unsigned int _get_table_maxdiv(const struct clk_div_table *table)
{
    unsigned int maxdiv = 0;
    const struct clk_div_table *clkt;

    for (clkt = table; clkt->div; clkt++)
        if (clkt->div > maxdiv)
            maxdiv = clkt->div;
    return maxdiv;
}

static unsigned int _get_maxdiv(struct clk_divider *divider)
{
    if (divider->flags & CLK_DIVIDER_ONE_BASED) {
        return div_mask(divider);
    }
    if (divider->flags & CLK_DIVIDER_POWER_OF_TWO) {
        return 1 << div_mask(divider);
    }
    if (divider->table) {
        return _get_table_maxdiv(divider->table);
    }
    return div_mask(divider) + 1;
}

static unsigned int _get_table_div(const struct clk_div_table *table,
                            unsigned int val)
{
    const struct clk_div_table *clkt;

    for (clkt = table; clkt->div; clkt++)
        if (clkt->val == val)
            return clkt->div;
    return 0;
}

static unsigned int _get_div(struct clk_divider *divider, unsigned int val)
{
    if (divider->flags & CLK_DIVIDER_ONE_BASED)
        return val;
    if (divider->flags & CLK_DIVIDER_POWER_OF_TWO)
        return 1 << val;
    if (divider->table)
        return _get_table_div(divider->table, val);
    return val + 1;
}

static unsigned int _get_table_val(const struct clk_div_table *table,
                            unsigned int div)
{
    const struct clk_div_table *clkt;

    for (clkt = table; clkt->div; clkt++)
        if (clkt->div == div)
            return clkt->val;
    return 0;
}

static unsigned int _get_val(struct clk_divider *divider, unsigned int div)
{
    if (divider->flags & CLK_DIVIDER_ONE_BASED)
        return div;
    if (divider->flags & CLK_DIVIDER_POWER_OF_TWO)
        return ffs(div);
    if (divider->table)
        return  _get_table_val(divider->table, div);
    return div - 1;
}

static aw_clk_rate_t clk_divider_recalc_rate(struct clk *hw,
        aw_clk_rate_t parent_rate)
{
    struct clk_divider     *divider = to_clk_divider(hw);
    unsigned int            div, val;

    val = clk_readl(divider->reg) >> divider->shift;
    val &= div_mask(divider);

    div = _get_div(divider, val);
    if (!div) {
        return parent_rate;
    }

    return parent_rate / div;
}

/*
 * The reverse of DIV_ROUND_UP: The maximum number which
 * divided by m is r
 */
#define MULT_ROUND_UP(r, m) ((r) * (m) + (m) - 1)

static aw_bool_t _is_valid_table_div(const struct clk_div_table *table,
                             unsigned int div)
{
    const struct clk_div_table *clkt;

    for (clkt = table; clkt->div; clkt++)
        if (clkt->div == div)
            return AW_TRUE;
    return AW_FALSE;
}

static aw_bool_t _is_valid_div(struct clk_divider *divider, unsigned int div)
{
    if (divider->flags & CLK_DIVIDER_POWER_OF_TWO)
        return is_power_of_2(div);
    if (divider->table)
        return _is_valid_table_div(divider->table, div);
    return AW_TRUE;
}

static int clk_divider_bestdiv(struct clk *hw, unsigned long rate,
        unsigned long *best_parent_rate)
{
    struct clk_divider *divider = to_clk_divider(hw);
    int i, bestdiv = 0;
    unsigned long parent_rate, best = 0, now, maxdiv;
    unsigned long parent_rate_saved = *best_parent_rate;

    if (!rate)
        rate = 1;

    maxdiv = _get_maxdiv(divider);

    if (!(__clk_get_flags(hw) & CLK_SET_RATE_PARENT)) {
        parent_rate = *best_parent_rate;
        bestdiv = AW_DIV_ROUND_UP (parent_rate, rate);
        bestdiv = bestdiv == 0 ? 1 : bestdiv;
        bestdiv = bestdiv > maxdiv ? maxdiv : bestdiv;
        return bestdiv;
    }

    /*
     * The maximum divider we can use without overflowing
     * unsigned long in rate * i below
     */
    maxdiv = min(ULONG_MAX / rate, maxdiv);

    for (i = 1; i <= maxdiv; i++) {
        if (!_is_valid_div(divider, i))
            continue;
        if (rate * i == parent_rate_saved) {
            /*
             * It's the most ideal case if the requested rate can be
             * divided from parent clock without needing to change
             * parent rate, so return the divider immediately.
             */
            *best_parent_rate = parent_rate_saved;
            return i;
        }
        parent_rate = __clk_round_rate(__clk_get_parent(hw),
                MULT_ROUND_UP(rate, i));
        now = parent_rate / i;
        if (now <= rate && now > best) {
            bestdiv = i;
            best = now;
            *best_parent_rate = parent_rate;
        }
    }

    if (!bestdiv) {
        bestdiv = _get_maxdiv(divider);
        *best_parent_rate = __clk_round_rate(__clk_get_parent(hw), 1);
    }

    return bestdiv;
}

static aw_clk_rate_t clk_divider_round_rate(struct clk *hw, aw_clk_rate_t rate,
                aw_clk_rate_t *prate)
{
    int div;
    div = clk_divider_bestdiv(hw, rate, prate);

    return *prate / div;
}

static int clk_divider_set_rate(struct clk *hw, unsigned long rate,
                unsigned long parent_rate)
{
    struct clk_divider *divider = to_clk_divider(hw);
    unsigned int div, value;
    uint32_t val;

    div = parent_rate / rate;
    value = _get_val(divider, div);

    if (value > div_mask(divider))
        value = div_mask(divider);

    if (NULL != divider->lock) {
        aw_spinlock_isr_take(divider->lock);
    }
    if (divider->flags & CLK_DIVIDER_HIWORD_MASK) {
        val = div_mask(divider) << (divider->shift + 16);
    } else {
        val = clk_readl(divider->reg);
        val &= ~(div_mask(divider) << divider->shift);
    }
    val |= value << divider->shift;
    clk_writel(val, divider->reg);
    if (NULL != divider->lock) {
        aw_spinlock_isr_give(divider->lock);
    }
    return 0;
}

const struct clk_ops clk_divider_ops = {
    .recalc_rate = clk_divider_recalc_rate,
    .round_rate = clk_divider_round_rate,
    .set_rate = clk_divider_set_rate,
};


static struct clk *_init_divider(
        struct clk_divider *div,
        const char *name,
        aw_clk_id_t clk_id,
        const char *parent_name,
        unsigned long flags,
        void *reg,
        uint8_t shift,
        uint8_t width,
        uint8_t clk_divider_flags,
        const struct clk_div_table *table,
        aw_spinlock_isr_t *lock)
{
    struct clk          *clk;
    struct clk_init_data    init;

    if (clk_divider_flags & CLK_DIVIDER_HIWORD_MASK) {
        if (width + shift > 16) {
            return NULL;
        }
    }

    init.name = name;
    init.ops = &clk_divider_ops;
    init.flags = flags | CLK_IS_BASIC;
    init.clk_id = clk_id;
    init.parent_name = parent_name ;


    /* struct clk_divider assignments */
    div->reg = reg;
    div->shift = shift;
    div->width = width;
    div->flags = clk_divider_flags;
    div->table = table;
    div->lock = lock;

    /* register the clock */
    clk = clk_register_hw(&div->hw,&init);
    return clk;
}

/**
 * clk_register_divider - register a divider clock with the clock framework
 * @dev: device registering this clock
 * @name: name of this clock
 * @parent_name: name of clock's parent
 * @flags: framework-specific flags
 * @reg: register address to adjust divider
 * @shift: number of bits to shift the bitfield
 * @width: width of the bitfield
 * @clk_divider_flags: divider-specific flags for this clock
 * @lock: shared register lock for this clock
 */
struct clk *clk_register_divider(
        struct clk_divider *p_clk_div,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        unsigned long flags,
        void *reg,
        uint8_t shift,
        uint8_t width,
        uint8_t clk_divider_flags,
        aw_spinlock_isr_t *lock)
{
    return _init_divider(p_clk_div, name, clk_id,parent_name, flags, reg, shift,
            width, clk_divider_flags, NULL,lock);
}

/**
 * clk_register_divider_table - register a table based divider clock with
 * the clock framework
 * @dev: device registering this clock
 * @name: name of this clock
 * @parent_name: name of clock's parent
 * @flags: framework-specific flags
 * @reg: register address to adjust divider
 * @shift: number of bits to shift the bitfield
 * @width: width of the bitfield
 * @clk_divider_flags: divider-specific flags for this clock
 * @table: array of divider/value pairs ending with a div set to 0
 * @lock: shared register lock for this clock
 */
struct clk *clk_register_divider_table(
        struct clk_divider *p_clk_div,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        unsigned long flags,
        void *reg,
        uint8_t shift,
        uint8_t width,
        uint8_t clk_divider_flags,
        const struct clk_div_table *table,
        aw_spinlock_isr_t *lock)
{
    return _init_divider(p_clk_div, name, clk_id,parent_name, flags, reg, shift,
            width, clk_divider_flags, table,lock);
}

/* end of file */
