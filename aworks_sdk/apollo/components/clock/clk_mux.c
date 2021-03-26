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
 * DOC: basic adjustable multiplexer clock that cannot gate
 *
 * Traits of this clock:
 * prepare - clk_prepare only ensures that parents are prepared
 * enable - clk_enable only ensures that parents are enabled
 * rate - rate is only affected by parent switching.  No clk_set_rate support
 * parent - parent is adjustable through clk_set_parent
 */

#define to_clk_mux(_hw) AW_CONTAINER_OF(_hw, struct clk_mux, hw)


static aw_err_t clk_mux_get_parent_hw(struct clk *hw,struct clk **p_parent)
{
    struct clk_mux     *mux = to_clk_mux(hw);
    int                 num_parents = mux->num_parents;
    aw_clk_reg_t        val;
    aw_err_t            err = 0;

    *p_parent  = NULL;
    /*
     * FIXME need a mux-specific flag to determine if val is bitwise or numeric
     * e.g. sys_clkin_ck's clksel field is 3 bits wide, but ranges from 0x1
     * to 0x7 (index starts at one)
     * OTOH, pmd_trace_clk_mux_ck uses a separate bit for each clock, so
     * val = 0x4 really means "bit 2, index starts at bit 0"
     */
    val = clk_readl(mux->reg) >> mux->shift;
    val &= mux->mask;

    if (val && (mux->flags & CLK_MUX_INDEX_BIT))
        val = ffs(val) - 1;

    if (val && (mux->flags & CLK_MUX_INDEX_ONE))
        val--;

    if (val >= num_parents) {
        err = -AW_EINVAL;
        goto cleanup;
    }
    *p_parent = mux->parents[val];
cleanup:
    return err;
}

static aw_err_t clk_mux_get_parent(struct clk *hw,aw_clk_id_t *p_parent_id)
{
    aw_err_t            err = 0;
    struct clk         *parent;


    err = clk_mux_get_parent_hw(hw,&parent);
    if (AW_OK == err) {
        *p_parent_id = __clk_get_id(parent);
    }

    return err;
}



static void clk_mux_init(struct clk *hw)
{
    struct clk         *parent;
    aw_err_t            err;

    err = clk_mux_get_parent_hw(hw,&parent);
    if (AW_OK == err) {
        hw->parent = parent;
    }
}

static aw_err_t clk_mux_set_parent(struct clk *hw,aw_clk_id_t parent)
{
    struct clk_mux *mux = to_clk_mux(hw);
    uint32_t        val;
    aw_err_t        err = -AW_EINVAL;
    uint32_t        index;

    /* 首先查找看本mux clock是否支持指定的父时钟 */
    for (index = 0; index < mux->num_parents;index ++) {
        if (parent == __clk_get_id(mux->parents[index] ) ) {
            err = 0;
            break;
        }
    }
    if (0 != err ) {
        return err;
    }

    if (mux->flags & CLK_MUX_INDEX_BIT) {
        index = (1 << ffs(index));
    }

    if (mux->flags & CLK_MUX_INDEX_ONE) {
        index++;
    }

    if (NULL != mux->lock) {
        aw_spinlock_isr_take(mux->lock);
    }

    if (mux->flags & CLK_MUX_HIWORD_MASK) {
        val = mux->mask << (mux->shift + 16);
    }
    else {
        val = clk_readl(mux->reg);
        val &= ~(mux->mask << mux->shift);
    }
    val |= index << mux->shift;
    clk_writel(val, mux->reg);
    if (NULL != mux->lock) {
        aw_spinlock_isr_give(mux->lock);
    }
    return 0;
}

const struct clk_ops clk_mux_ops = {
        .init = clk_mux_init,
        .get_parent = clk_mux_get_parent,
        .set_parent = clk_mux_set_parent,
#if 0 
    .determine_rate = __clk_mux_determine_rate,
#endif
};

static const struct clk_ops clk_mux_ro_ops = {
        .init = clk_mux_init,
        .get_parent = clk_mux_get_parent,
};




struct clk *clk_register_mux(
        struct clk_mux *mux,
        aw_clk_id_t clk_id,
        const char *name,
        const char **parent_names,
        uint8_t num_parents,
        unsigned long flags,
        void *reg,
        uint8_t shift,
        uint8_t width,
        uint8_t clk_mux_flags,
        aw_spinlock_isr_t *lock)
{
    uint32_t                    mask = AW_BIT(width) - 1;
    struct clk_init_data        init;
    struct clk                 *hw = NULL;
    int                         i;

    /* 检查参数 */
    if (num_parents <= 1|| 0== width || shift > 32) {
        return NULL;
    }

    if (num_parents > CONFIG_CLK_MAX_PARENTS_NUM) {
        return NULL;
    }

    if (clk_mux_flags & CLK_MUX_HIWORD_MASK) {
        if (width + shift > 16) {
            return NULL;
        }
    }


    init.name = name;
    if (clk_mux_flags & CLK_MUX_READ_ONLY)
        init.ops = &clk_mux_ro_ops;
    else
        init.ops = &clk_mux_ops;
    init.flags = flags | CLK_IS_BASIC | CLK_IS_BASIC_MUX;
    init.parent_name = NULL;
    init.clk_id = clk_id;

    /* struct clk_mux assignments */
    mux->reg = reg;
    mux->shift = shift;
    mux->mask = mask;
    mux->flags = clk_mux_flags;
    mux->num_parents = num_parents;
    mux->lock = lock;
    for (i = 0;i< num_parents;i++) {
        hw = __clk_find_by_name(parent_names[i]);
        if (NULL == hw) {
            return NULL;
        }
        mux->parents[i] = hw;
    }

    hw = clk_register_hw(&mux->hw,&init);

    return hw;
}

/* end of file */

