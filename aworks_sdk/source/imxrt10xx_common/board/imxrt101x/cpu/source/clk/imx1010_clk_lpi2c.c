#include "clk_provider.h"
#include "imx1010_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1010_ccm_regs.h"
#include "imx1010_reg_base.h"

aw_const static struct simple_mux_desc lpi2c_clk_sel_desc = {
        {
                IMX1010_CLK_PLL3_DIV_8,
                IMX1010_CLK_OSC_24M
        },
        2,
        (void *)&CCM->CSCDR2,
        18,
        1
};

SIPMLE_MUX_CLK_DEFS(lpi2c_clk_sel,lpi2c_clk_sel_desc);

struct clk * imx1010_clk_register_lpi2c_clk_sel(
        struct clk *p_lpi2c_clk_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_lpi2c_clk_sel,
            clk_id,
            name,
            &lpi2c_clk_sel_ops);
}

aw_const static struct simple_div_clk_desc lpi2c_podf_desc = {
        &CCM->CSCDR2,
        19,
        6,
        1,
        64,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(lpi2c_podf,lpi2c_podf_desc)

struct clk * imx1010_clk_register_lpi2c_podf(
        struct clk *p_lpi2c_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_lpi2c_podf,
            clk_id,
            name,
            parent_id,
            &lpi2c_podf_ops);
}

