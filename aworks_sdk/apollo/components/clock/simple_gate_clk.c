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

#include "simple_gate_clk.h"
#include "aw_bitops.h"

aw_err_t simple_gate_clk_enable(
        struct clk *hw,
        aw_const struct simple_gate_desc *p_desc)
{
    AW_REG_BIT_SET32(p_desc->reg,p_desc->bit);
    return AW_OK;
}

aw_err_t simple_gate_clk_disable(
        struct clk *hw,
        aw_const struct simple_gate_desc *p_desc)
{
    AW_REG_BIT_CLR32(p_desc->reg,p_desc->bit);
    return AW_OK;
}

/* end of file */