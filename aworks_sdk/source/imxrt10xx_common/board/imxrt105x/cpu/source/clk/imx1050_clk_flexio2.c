#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc flexio2_clk_sel_desc = {
        {
                IMX1050_CLK_PLL_AUDIO,
                IMX1050_CLK_PLL3_PFD2,
                IMX1050_CLK_PLL_VIDEO,
                IMX1050_CLK_PLL_USB1,
        },
        4,
        (void *)&CCM->CSCMR2,
        19,
        2
};

SIPMLE_MUX_CLK_DEFS(flexio2_clk_sel,flexio2_clk_sel_desc);

struct clk * imx1050_clk_register_flexio2_clk_sel(
        struct clk *p_flexio2_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_flexio2_sel,
            clk_id,
            name,
            &flexio2_clk_sel_ops);
}


aw_const static struct simple_div_clk_desc flexio2_pred_desc = {
        &CCM->CS1CDR,
        9,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(flexio2_pred,flexio2_pred_desc)

struct clk * imx1050_clk_register_flexio2_pred(
        struct clk *p_flexio2_pred,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_flexio2_pred,
            clk_id,
            name,
            parent_id,
            &flexio2_pred_ops);
}

aw_const static struct simple_div_clk_desc flexio2_podf_desc = {
        &CCM->CS1CDR,
        25,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(flexio2_podf,flexio2_podf_desc)

struct clk * imx1050_clk_register_flexio2_podf(
        struct clk *p_flexio2_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_flexio2_podf,
            clk_id,
            name,
            parent_id,
            &flexio2_podf_ops);
}

