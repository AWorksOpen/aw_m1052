#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc semc_alt_desc = {
        {
                IMX1050_CLK_PLL2_PFD2,
                IMX1050_CLK_PLL3_PFD1,
        },
        2,
        (void *)&CCM->CBCDR,
        7,
        1
};

SIPMLE_MUX_CLK_DEFS(semc_alt_sel,semc_alt_desc);

struct clk * imx1050_clk_register_semc_alt_sel(
        struct clk *p_semc_alt_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_semc_alt_sel,
            clk_id,
            name,
            &semc_alt_sel_ops);
}

aw_const static struct simple_mux_desc semc_desc = {
        {
                IMX1050_CLK_PERIPH_CLK_SEL,
                IMX1050_CLK_SEMC_ALT_SEL,
        },
        2,
        (void *)&CCM->CBCDR,
        6,
        1
};

SIPMLE_MUX_CLK_DEFS(semc_sel,semc_desc);

struct clk * imx1050_clk_register_semc_sel(
        struct clk *p_semc_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_semc_sel,
            clk_id,
            name,
            &semc_sel_ops);
}

aw_const static struct simple_div_clk_desc semc_podf_desc = {
        &CCM->CBCDR,
        16,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(semc_podf,semc_podf_desc)

struct clk * imx1050_clk_register_semc_podf(
        struct clk *p_semc_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_semc_podf,
            clk_id,
            name,
            parent_id,
            &semc_podf_ops);
}

