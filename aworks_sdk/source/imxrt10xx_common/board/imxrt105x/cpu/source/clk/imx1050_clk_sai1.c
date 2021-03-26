#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc sai1_clk_sel_desc = {
        {
                IMX1050_CLK_PLL3_PFD2,
                IMX1050_CLK_PLL_VIDEO,
                IMX1050_CLK_PLL_AUDIO,
        },
        3,
        (void *)&CCM->CSCMR1,
        10,
        2
};

SIPMLE_MUX_CLK_DEFS(sai1_clk_sel,sai1_clk_sel_desc);

struct clk * imx1050_clk_register_sai1_clk_sel(
        struct clk *p_sai1_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_sai1_sel,
            clk_id,
            name,
            &sai1_clk_sel_ops);
}


aw_const static struct simple_div_clk_desc sai1_pred_desc = {
        &CCM->CS1CDR,
        6,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(sai1_pred,sai1_pred_desc)

struct clk * imx1050_clk_register_sai1_pred(
        struct clk *p_sai1_pred,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_sai1_pred,
            clk_id,
            name,
            parent_id,
            &sai1_pred_ops);
}

aw_const static struct simple_div_clk_desc sai1_podf_desc = {
        &CCM->CS1CDR,
        0,
        6,
        1,
        64,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(sai1_podf,sai1_podf_desc)

struct clk * imx1050_clk_register_sai1_podf(
        struct clk *p_sai1_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_sai1_podf,
            clk_id,
            name,
            parent_id,
            &sai1_podf_ops);
}

