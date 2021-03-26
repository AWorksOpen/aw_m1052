#include "clk_provider.h"
#include "imx1010_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1010_ccm_regs.h"
#include "imx1010_reg_base.h"

aw_const static struct simple_mux_desc lpspi_clk_sel_desc = {
        {
                IMX1010_CLK_PLL3_PFD1,
                IMX1010_CLK_PLL3_PFD0,
                IMX1010_CLK_PLL_SYS,
                IMX1010_CLK_PLL2_PFD2
        },
        4,
        (void *)&CCM->CBCMR,
        4,
        2
};

SIPMLE_MUX_CLK_DEFS(lpspi_clk_sel,lpspi_clk_sel_desc);

struct clk * imx1010_clk_register_lpspi_clk_sel(
        struct clk *p_lpspi_clk_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_lpspi_clk_sel,
            clk_id,
            name,
            &lpspi_clk_sel_ops);
}

aw_const static struct simple_div_clk_desc lpspi_podf_desc = {
        &CCM->CBCMR,
        26,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(lpspi_podf,lpspi_podf_desc)

struct clk * imx1010_clk_register_lpspi_podf(
        struct clk *p_lpspi_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_lpspi_podf,
            clk_id,
            name,
            parent_id,
            &lpspi_podf_ops);
}

