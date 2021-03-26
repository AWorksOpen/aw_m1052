#include "clk_provider.h"
#include "imx1010_clk.h"
#include "simple_mux_clk.h"
#include "imx1010_ccm_regs.h"
#include "imx1010_reg_base.h"

aw_const static struct simple_mux_desc pre_periph_clk_desc = {
        {
                IMX1010_CLK_PLL_SYS,
                IMX1010_CLK_PLL3_PFD3,
                IMX1010_CLK_PLL2_PFD3,
        },
        4,
        (void *)&CCM->CBCMR,
        18,
        2
};

SIPMLE_MUX_CLK_DEFS(pre_periph_clk_sel,pre_periph_clk_desc)

struct clk * imx1010_clk_register_pre_periph_clk_sel(
        struct clk *arm_podf,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            arm_podf,
            clk_id,
            name,
            &pre_periph_clk_sel_ops);
}
