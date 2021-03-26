#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc pre_periph_clk_desc = {
        {
                IMX1050_CLK_PLL_SYS,
                IMX1050_CLK_PLL2_PFD2,
                IMX1050_CLK_PLL2_PFD0,
                IMX1050_CLK_ARM_PODF,
        },
        4,
        (void *)&CCM->CBCMR,
        18,
        2
};

SIPMLE_MUX_CLK_DEFS(pre_periph_clk_sel,pre_periph_clk_desc)

struct clk * imx1050_clk_register_pre_periph_clk_sel(
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
