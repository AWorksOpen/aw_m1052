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

#ifndef         __SIMPLE_DIV_CLK_H__
#define         __SIMPLE_DIV_CLK_H__

#include "clk_provider.h"

struct simple_div_clk_desc {
    volatile void                  *reg;
    uint8_t                         shift;
    uint8_t                         width;
    uint32_t                        min_div;
    uint32_t                        max_div;
    int                             div_val_sub_one;

};

aw_clk_rate_t simple_div_clk_recalc_rate(
        struct clk                          *hw,
        aw_clk_rate_t                        parent_rate,
        aw_const struct simple_div_clk_desc *p_desc);

aw_clk_rate_t simple_div_clk_round_rate(
        struct clk                          *hw,
        aw_clk_rate_t                        rate,
        aw_clk_rate_t                       *prate,
        aw_const struct simple_div_clk_desc *p_desc);

aw_err_t simple_div_set_rate(
        struct clk                          *hw,
        unsigned long                        rate,
        unsigned long                        parent_rate,
        aw_const struct simple_div_clk_desc *p_desc);

#define     SIPMLE_DIV_CLK_DEFS(name,desc) \
    static aw_clk_rate_t name##_recalc_rate(\
        struct clk *hw,aw_clk_rate_t parent_rate) \
    { \
        return simple_div_clk_recalc_rate(hw,parent_rate,&desc); \
    } \
    static aw_clk_rate_t name##_round_rate(\
        struct clk *hw,aw_clk_rate_t rate,aw_clk_rate_t *prate) \
    { \
        return simple_div_clk_round_rate(hw,rate,prate,&desc); \
    } \
    static aw_err_t name##_set_rate(\
        struct clk *hw,unsigned long rate,unsigned long parent_rate) \
    { \
        return simple_div_set_rate(hw,rate,parent_rate,&desc); \
    }\
    static const struct clk_ops name##_ops = { \
        .recalc_rate = name##_recalc_rate, \
        .set_rate = name##_set_rate, \
        .round_rate = name##_round_rate, \
    };

#endif

/* end of file */
