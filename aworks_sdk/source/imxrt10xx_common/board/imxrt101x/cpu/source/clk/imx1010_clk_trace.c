#include "clk_provider.h"
#include "imx1010_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1010_ccm_regs.h"
#include "imx1010_reg_base.h"

aw_const static struct simple_mux_desc trace_clk_sel_desc = {
        {
                IMX1010_CLK_PLL_SYS,
                IMX1010_CLK_PLL2_PFD2,
                IMX1010_CLK_PLL2_PFD0,
                IMX1010_CLK_PLL2_PFD1
        },
        4,
        (void *)&CCM->CBCMR,
        14,
        2
};

SIPMLE_MUX_CLK_DEFS(trace_clk_sel,trace_clk_sel_desc);

struct clk * imx1010_clk_register_trace_clk_sel(
        struct clk *p_trace_clk_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_trace_clk_sel,
            clk_id,
            name,
            &trace_clk_sel_ops);
}

aw_const static struct simple_div_clk_desc trace_podf_desc = {
        &CCM->CSCDR1,
        25,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(trace_podf,trace_podf_desc)

struct clk * imx1010_clk_register_trace_podf(
        struct clk *p_trace_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_trace_podf,
            clk_id,
            name,
            parent_id,
            &trace_podf_ops);
}

