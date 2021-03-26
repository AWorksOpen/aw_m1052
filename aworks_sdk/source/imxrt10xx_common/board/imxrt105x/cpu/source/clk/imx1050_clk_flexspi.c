#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc flexspi_clk_sel_desc = {
        {
                IMX1050_CLK_SEMC_PODF,
                IMX1050_CLK_PLL_USB1,
                IMX1050_CLK_PLL2_PFD2,
                IMX1050_CLK_PLL3_PFD0
        },
        4,
        (void *)&CCM->CSCMR1,
        29,
        2
};

SIPMLE_MUX_CLK_DEFS(flexspi_clk_sel,flexspi_clk_sel_desc);

struct clk * imx1050_clk_register_flexspi_clk_sel(
        struct clk *p_flexspi_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_flexspi_sel,
            clk_id,
            name,
            &flexspi_clk_sel_ops);
}

aw_const static struct simple_div_clk_desc flexspi_podf_desc = {
        &CCM->CSCMR1,
        23,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(flexspi_podf,flexspi_podf_desc)

struct clk * imx1050_clk_register_flexspi_podf(
        struct clk *p_flexspi_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_flexspi_podf,
            clk_id,
            name,
            parent_id,
            &flexspi_podf_ops);
}

