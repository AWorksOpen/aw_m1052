#include "clk_provider.h"
#include "imx1020_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1020_ccm_regs.h"
#include "imx1020_reg_base.h"

aw_const static struct simple_mux_desc enc_clk_sel_desc = {
        {
                IMX1020_CLK_PLL2_PFD0,
                IMX1020_CLK_PLL_SYS,
                IMX1020_CLK_PLL_USB1,
                IMX1020_CLK_PLL2_PFD2,
                IMX1020_CLK_PLL3_PFD3
        },
        5,
        (void *)&CCM->CS2CDR,
        15,
        3
};

SIPMLE_MUX_CLK_DEFS(enc_clk_sel,enc_clk_sel_desc);

struct clk * imx1020_clk_register_enc_clk_sel(
        struct clk *p_enc_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_enc_sel,
            clk_id,
            name,
            &enc_clk_sel_ops);
}


aw_const static struct simple_div_clk_desc enc_pred_desc = {
        &CCM->CS2CDR,
        18,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(enc_pred,enc_pred_desc)

struct clk * imx1020_clk_register_enc_pred(
        struct clk *p_enc_pred,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_enc_pred,
            clk_id,
            name,
            parent_id,
            &enc_pred_ops);
}

aw_const static struct simple_div_clk_desc enc_podf_desc = {
        &CCM->CS2CDR,
        21,
        6,
        1,
        64,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(enc_podf,enc_podf_desc)

struct clk * imx1020_clk_register_enc_podf(
        struct clk *p_enc_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_enc_podf,
            clk_id,
            name,
            parent_id,
            &enc_podf_ops);
}

