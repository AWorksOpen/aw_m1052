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
#ifndef         __SIMPLE_GATE_CLK_H__
#define         __SIMPLE_GATE_CLK_H__

#include "clk_provider.h"

struct simple_gate_desc {
    void           *reg;
    uint8_t         bit;
};

aw_err_t simple_gate_clk_enable(
        struct clk *hw,
        aw_const struct simple_gate_desc *p_desc);
aw_err_t simple_gate_clk_disable(
        struct clk *hw,
        aw_const struct simple_gate_desc *p_desc);

#define     SIPMLE_GATE_CLK_DEFS(name,desc) \
    static aw_err_t name##_enable(struct clk *hw) \
    { \
        return simple_gate_clk_enable(hw,&desc); \
    } \
    static aw_err_t name##_disable(struct clk *hw) \
    { \
        return simple_gate_clk_disable(hw,&desc); \
    } \
    static const struct clk_ops name##_ops = { \
        .enable = name##_enable, \
        .disable = name##_disable, \
    };


#endif

/* end of file */