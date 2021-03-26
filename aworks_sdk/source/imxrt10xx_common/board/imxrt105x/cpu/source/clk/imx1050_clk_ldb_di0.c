#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "simple_div_table_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc ldb_di0_clk_sel_desc = {
        {
                IMX1050_CLK_PLL_VIDEO,
                IMX1050_CLK_PLL2_PFD0,
                IMX1050_CLK_PLL2_PFD3,
                IMX1050_CLK_PLL2_PFD1,
                IMX1050_CLK_PLL3_PFD3,
        },
        5,
        (void *)&CCM->CS2CDR,
        9,
        3
};

SIPMLE_MUX_CLK_DEFS(ldb_di0_clk_sel,ldb_di0_clk_sel_desc);

struct clk * imx1050_clk_register_ldb_di0_clk_sel(
        struct clk *p_ldb_di0_clk_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_ldb_di0_clk_sel,
            clk_id,
            name,
            &ldb_di0_clk_sel_ops);
}

static aw_const struct clk_div_table __ldb_id0_div_table[] = {
        {.val = 0,.div = 4},
        {.val = 1,.div = 7},
        {.val = 0,.div = 0},
};

static aw_const struct simple_div_table_clk_desc ldb_di0_ipu_div_desc = {
        &__ldb_id0_div_table[0],
        (void *)&CCM->CSCMR2,
        10,
        1,
};

SIPMLE_DIV_TABLE_CLK_DEFS(ldb_di0_ipu_div,ldb_di0_ipu_div_desc)

struct clk * imx1050_clk_register_ldb_di0_ipu_div(
        struct clk *p_ldb_di0_ipu_div,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_ldb_di0_ipu_div,
            clk_id,
            name,
            parent_id,
            &ldb_di0_ipu_div_ops);

}
