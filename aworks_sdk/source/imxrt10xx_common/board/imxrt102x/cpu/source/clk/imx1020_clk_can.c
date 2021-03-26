#include "clk_provider.h"
#include "imx1020_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1020_ccm_regs.h"
#include "imx1020_reg_base.h"

aw_const static struct simple_mux_desc can_clk_sel_desc = {
        {
                IMX1020_CLK_PLL3_DIV_8,
                IMX1020_CLK_OSC_24M,
                IMX1020_CLK_PLL3_DIV_6
        },
        3,
        (void *)&CCM->CSCMR2,
        8,
        2
};

SIPMLE_MUX_CLK_DEFS(can_clk_sel,can_clk_sel_desc);

struct clk * imx1020_clk_register_can_clk_sel(
        struct clk *p_can_clk_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_can_clk_sel,
            clk_id,
            name,
            &can_clk_sel_ops);
}

aw_const static struct simple_div_clk_desc can_podf_desc = {
        &CCM->CSCMR2,
        2,
        6,
        1,
        64,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(can_podf,can_podf_desc)

struct clk * imx1020_clk_register_can_podf(
        struct clk *p_can_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_can_podf,
            clk_id,
            name,
            parent_id,
            &can_podf_ops);
}

