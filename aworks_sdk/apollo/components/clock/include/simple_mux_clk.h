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
#ifndef         __SIMPLE_MUX_CLK_H__
#define         __SIMPLE_MUX_CLK_H__

#include "clk_provider.h"

struct simple_mux_desc {
    aw_clk_id_t     parents_id[CONFIG_CLK_MAX_PARENTS_NUM];
    int             num_parents;
    void           *reg;
    uint8_t         shift;
    uint8_t         len;
};

aw_err_t simple_mux_clk_get_parent(
                    struct clk *hw,
                    aw_clk_id_t *p_parent_id,
                    aw_const struct simple_mux_desc *p_desc);

aw_err_t simple_mux_clk_set_parent(
                    struct clk *hw,
                    aw_clk_id_t parent,
                    aw_const struct simple_mux_desc *p_desc);

void simple_mux_clk_init(struct clk *hw,aw_const struct simple_mux_desc *p_desc);

#define     SIPMLE_MUX_CLK_DEFS(name,desc) \
    static void name##clk_init(struct clk *hw) \
    { \
        simple_mux_clk_init(hw,& (desc) ); \
    } \
    static aw_err_t name##_get_parent(struct clk *hw,aw_clk_id_t *p_parent_id) \
    { \
        return simple_mux_clk_get_parent(hw,p_parent_id,&desc); \
    } \
    static aw_err_t name##_set_parent(struct clk *hw,aw_clk_id_t parent_id) \
    { \
        return simple_mux_clk_set_parent(hw,parent_id,&desc); \
    } \
    static const struct clk_ops name##_ops = { \
        .init = name##clk_init, \
        .set_parent = name##_set_parent, \
        .get_parent = name##_get_parent, \
    };


#endif

/* end of file */
