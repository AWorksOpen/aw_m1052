#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc lcdif1_pre_clk_sel_desc = {
        {
                IMX1050_CLK_PLL_SYS,
                IMX1050_CLK_PLL3_PFD3,
                IMX1050_CLK_PLL_VIDEO,
                IMX1050_CLK_PLL2_PFD0,
                IMX1050_CLK_PLL2_PFD1,
                IMX1050_CLK_PLL3_PFD1,
        },
        5,
        (void *)&CCM->CSCDR2,
        15,
        3
};

SIPMLE_MUX_CLK_DEFS(lcdif1_pre_clk_sel,lcdif1_pre_clk_sel_desc);

struct clk * imx1050_clk_register_lcdif1_pre_clk_sel(
        struct clk *p_lcdif1_pre_clk_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_lcdif1_pre_clk_sel,
            clk_id,
            name,
            &lcdif1_pre_clk_sel_ops);
}

aw_const static struct simple_div_clk_desc lcdif1_pred_desc = {
        &CCM->CSCDR2,
        12,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(lcdif1_pred,lcdif1_pred_desc)

struct clk * imx1050_clk_register_lcdif1_pred(
        struct clk *p_lcdif1_pred,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_lcdif1_pred,
            clk_id,
            name,
            parent_id,
            &lcdif1_pred_ops);
}


aw_const static struct simple_div_clk_desc lcdif1_podf_desc = {
        &CCM->CBCMR,
        23,
        3,
        1,
        8,
        1,  //设置值要减一
};


SIPMLE_DIV_CLK_DEFS(lcdif1_podf,lcdif1_podf_desc)

struct clk * imx1050_clk_register_lcdif1_podf(
        struct clk *p_lcdif1_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_lcdif1_podf,
            clk_id,
            name,
            parent_id,
            &lcdif1_podf_ops);
}

aw_const static struct simple_mux_desc lcdif1_clk_sel_desc = {
        {
                IMX1050_CLK_LCDIF1_PRED,
                -1,
                -1,
                IMX1050_CLK_LDB_DI0_IPU_DIV,
                IMX1050_CLK_LDB_DI1_IPU_DIV,
        },
        5,
        (void *)&CCM->CSCDR2,
        9,
        3
};

SIPMLE_MUX_CLK_DEFS(lcdif1_clk_sel,lcdif1_clk_sel_desc);

struct clk * imx1050_clk_register_lcdif1_clk_sel(
        struct clk *p_lcdif1_clk_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_lcdif1_clk_sel,
            clk_id,
            name,
            &lcdif1_clk_sel_ops);
}

