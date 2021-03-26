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

#include "simple_mux_clk.h"
#include "aw_bitops.h"

aw_err_t simple_mux_clk_get_parent(
                    struct clk *hw,
                    aw_clk_id_t *p_parent_id,
                    aw_const struct simple_mux_desc *p_desc)
{
    uint32_t                data;

    data = AW_REG_BITS_GET32(p_desc->reg,p_desc->shift,p_desc->len);
    * p_parent_id = p_desc->parents_id[data];
    return AW_OK;
}

void simple_mux_clk_init(struct clk *hw,aw_const struct simple_mux_desc *p_desc)
{
    aw_clk_id_t         parent_id;
    simple_mux_clk_get_parent(hw,&parent_id,p_desc);
    hw->parent = __clk_find_by_id(parent_id);
}

aw_err_t simple_mux_clk_set_parent(
                    struct clk *hw,
                    aw_clk_id_t parent_id,
                    aw_const struct simple_mux_desc *p_desc)
{
    uint32_t        data;
    aw_err_t        err = - AW_EINVAL;
    /* 首先检查parent_id是否合法 */
    for (data = 0;data<p_desc->num_parents;data++) {
        if (p_desc->parents_id[data] == parent_id) {
            err = 0;
            break;
        }
    }

    if (0 != err) {
        return err;
    }

    AW_REG_BITS_SET32(p_desc->reg,p_desc->shift,p_desc->len,data);
    return AW_OK;
}

/* end of file */
