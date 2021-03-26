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


/**
 * DOC: basic gatable clock which can gate and ungate it's ouput
 *
 * Traits of this clock:
 * prepare - clk_(un)prepare only ensures parent is (un)prepared
 * enable - clk_enable and clk_disable are functional & control gating
 * rate - inherits rate from parent.  No clk_set_rate support
 * parent - fixed parent.  No clk_set_parent support
 */

#define to_clk_gate(_hw) AW_CONTAINER_OF(_hw, struct clk_gate, hw)

/*
 * It works on following logic:
 *
 * For enabling clock, enable = 1
 *  set2dis = 1 -> clear bit    -> set = 0
 *  set2dis = 0 -> set bit  -> set = 1
 *
 * For disabling clock, enable = 0
 *  set2dis = 1 -> set bit  -> set = 1
 *  set2dis = 0 -> clear bit    -> set = 0
 *
 * So, result is always: enable xor set2dis.
 */
static void clk_gate_endisable(struct clk *hw, int enable)
{
    struct clk_gate *gate = to_clk_gate(hw);
    int set = gate->flags & CLK_GATE_SET_TO_DISABLE ? 1 : 0;

    aw_clk_reg_t reg;

    set ^= enable;

    if (NULL != gate->lock) {
        aw_spinlock_isr_take(gate->lock);
    }

    if (gate->flags & CLK_GATE_HIWORD_MASK) {
        reg = AW_BIT(gate->bit_idx + 16);
        if (set) {
            reg |= AW_BIT(gate->bit_idx);
        }
    } else {
        reg = clk_readl(gate->reg);

        if (set) {
            reg |= AW_BIT(gate->bit_idx);
        }
        else {
            reg &= ~AW_BIT(gate->bit_idx);
        }
    }

    clk_writel(reg, gate->reg);
    if (NULL != gate->lock) {
        aw_spinlock_isr_give(gate->lock);
    }
}

static aw_err_t clk_gate_enable(struct clk *hw)
{
    clk_gate_endisable(hw, 1);

    return 0;
}

static aw_err_t clk_gate_disable(struct clk *hw)
{
    clk_gate_endisable(hw, 0);
    return 0;
}

static int clk_gate_is_enabled(struct clk *hw)
{
    aw_clk_reg_t reg;
    struct clk_gate *gate = to_clk_gate(hw);

    reg = clk_readl(gate->reg);

    /* if a set bit disables this clk, flip it before masking */
    if (gate->flags & CLK_GATE_SET_TO_DISABLE)
        reg ^= AW_BIT(gate->bit_idx);

    reg &= AW_BIT(gate->bit_idx);

    return reg ? 1 : 0;
}

const struct clk_ops clk_gate_ops = {
    .enable = clk_gate_enable,
    .disable = clk_gate_disable,
};

/**
 * clk_register_gate - register a gate clock with the clock framework
 * @dev: device that is registering this clock
 * @name: name of this clock
 * @parent_name: name of this clock's parent
 * @flags: framework-specific flags for this clock
 * @reg: register address to control gating of this clock
 * @bit_idx: which bit in the register controls gating of this clock
 * @clk_gate_flags: gate-specific flags for this clock
 * @lock: shared register lock for this clock
 */
struct clk *clk_register_gate(struct clk_gate *gate,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        unsigned long flags,
        void *reg,
        uint8_t bit_idx,
        uint8_t clk_gate_flags,
        aw_spinlock_isr_t *lock)
{

    struct clk *clk = NULL;
    struct clk_init_data init;

    if (clk_gate_flags & CLK_GATE_HIWORD_MASK) {
        return NULL;
    }

    /* allocate the gate */


    init.name = name;
    init.ops = &clk_gate_ops;
    init.flags = flags | CLK_IS_BASIC;
    init.parent_name = parent_name;
    init.clk_id = clk_id;

    /* struct clk_gate assignments */
    gate->reg = reg;
    gate->bit_idx = bit_idx;
    gate->flags = clk_gate_flags;
    gate ->lock = lock;

    clk = clk_register_hw(&gate->hw,&init);

    return clk;
}


/* end of file */
