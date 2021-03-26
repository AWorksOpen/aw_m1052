#include "clk_provider.h"
#include "imx1020_clk.h"
#include "simple_mux_clk.h"
#include "imx1020_ccm_regs.h"
#include "imx1020_reg_base.h"

aw_const static struct simple_mux_desc usdhc2_clk_desc = {
        {
                IMX1020_CLK_PLL2_PFD2,
                IMX1020_CLK_PLL2_PFD0,
        },
        2,
        (void *)&CCM->CSCMR1,
        16,
        1
};

SIPMLE_MUX_CLK_DEFS(usdhc2_clk_sel,usdhc2_clk_desc)

struct clk * imx1020_clk_register_usdhc2_clk_sel(
        struct clk *p_usdhc2_clk_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_usdhc2_clk_sel,
            clk_id,
            name,
            &usdhc2_clk_sel_ops);
}
