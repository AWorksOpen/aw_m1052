#include "clk_provider.h"
#include "imx1020_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1020_ccm_regs.h"
#include "imx1020_reg_base.h"

aw_const static struct simple_mux_desc spdif0_clk_sel_desc = {
        {
                IMX1020_CLK_PLL_AUDIO,
                IMX1020_CLK_PLL3_PFD2,
                IMX1020_CLK_INVALID,
                IMX1020_CLK_PLL_USB1,
        },
        4,
        (void *)&CCM->CDCDR,
        14,
        2
};

SIPMLE_MUX_CLK_DEFS(spdif0_clk_sel,spdif0_clk_sel_desc);

struct clk * imx1020_clk_register_spdif0_clk_sel(
        struct clk *p_spdif0_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_spdif0_sel,
            clk_id,
            name,
            &spdif0_clk_sel_ops);
}


aw_const static struct simple_div_clk_desc spdif0_pred_desc = {
        &CCM->CDCDR,
        25,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(spdif0_pred,spdif0_pred_desc)

struct clk * imx1020_clk_register_spdif0_pred(
        struct clk *p_spdif0_pred,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_spdif0_pred,
            clk_id,
            name,
            parent_id,
            &spdif0_pred_ops);
}

aw_const static struct simple_div_clk_desc spdif0_podf_desc = {
        &CCM->CDCDR,
        22,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(spdif0_podf,spdif0_podf_desc)

struct clk * imx1020_clk_register_spdif0_podf(
        struct clk *p_spdif0_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_spdif0_podf,
            clk_id,
            name,
            parent_id,
            &spdif0_podf_ops);
}

