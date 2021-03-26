#include "clk_provider.h"
#include "imx1010_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1010_ccm_regs.h"
#include "imx1010_reg_base.h"

aw_const static struct simple_mux_desc sai3_clk_sel_desc = {
        {
                IMX1010_CLK_PLL3_PFD2,
                IMX1010_CLK_INVALID,
                IMX1010_CLK_PLL_AUDIO
        },
        3,
        (void *)&CCM->CSCMR1,
        14,
        2
};

SIPMLE_MUX_CLK_DEFS(sai3_clk_sel,sai3_clk_sel_desc);

struct clk * imx1010_clk_register_sai3_clk_sel(
        struct clk *p_sai3_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_sai3_sel,
            clk_id,
            name,
            &sai3_clk_sel_ops);
}


aw_const static struct simple_div_clk_desc sai3_pred_desc = {
        &CCM->CS1CDR,
        22,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(sai3_pred,sai3_pred_desc)

struct clk * imx1010_clk_register_sai3_pred(
        struct clk *p_sai3_pred,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_sai3_pred,
            clk_id,
            name,
            parent_id,
            &sai3_pred_ops);
}

aw_const static struct simple_div_clk_desc sai3_podf_desc = {
        &CCM->CS1CDR,
        16,
        6,
        1,
        64,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(sai3_podf,sai3_podf_desc)

struct clk * imx1010_clk_register_sai3_podf(
        struct clk *p_sai3_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_sai3_podf,
            clk_id,
            name,
            parent_id,
            &sai3_podf_ops);
}

