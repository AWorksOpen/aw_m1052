#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc sai2_clk_sel_desc = {
        {
                IMX1050_CLK_PLL3_PFD2,
                IMX1050_CLK_PLL_VIDEO,
                IMX1050_CLK_PLL_AUDIO
        },
        3,
        (void *)&CCM->CSCMR1,
        12,
        2
};

SIPMLE_MUX_CLK_DEFS(sai2_clk_sel,sai2_clk_sel_desc);

struct clk * imx1050_clk_register_sai2_clk_sel(
        struct clk *p_sai2_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_sai2_sel,
            clk_id,
            name,
            &sai2_clk_sel_ops);
}


aw_const static struct simple_div_clk_desc sai2_pred_desc = {
        &CCM->CS2CDR,
        6,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(sai2_pred,sai2_pred_desc)

struct clk * imx1050_clk_register_sai2_pred(
        struct clk *p_sai2_pred,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_sai2_pred,
            clk_id,
            name,
            parent_id,
            &sai2_pred_ops);
}

aw_const static struct simple_div_clk_desc sai2_podf_desc = {
        &CCM->CS2CDR,
        0,
        6,
        1,
        64,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(sai2_podf,sai2_podf_desc)

struct clk * imx1050_clk_register_sai2_podf(
        struct clk *p_sai2_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_sai2_podf,
            clk_id,
            name,
            parent_id,
            &sai2_podf_ops);
}

