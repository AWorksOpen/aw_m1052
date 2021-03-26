#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc usdhc1_clk_desc = {
        {
                IMX1050_CLK_PLL2_PFD2,
                IMX1050_CLK_PLL2_PFD0,
        },
        2,
        (void *)&CCM->CSCMR1,
        16,
        1
};

SIPMLE_MUX_CLK_DEFS(usdhc1_clk_sel,usdhc1_clk_desc)

struct clk * imx1050_clk_register_usdhc1_clk_sel(
        struct clk *p_usdhc1_clk_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_usdhc1_clk_sel,
            clk_id,
            name,
            &usdhc1_clk_sel_ops);
}
