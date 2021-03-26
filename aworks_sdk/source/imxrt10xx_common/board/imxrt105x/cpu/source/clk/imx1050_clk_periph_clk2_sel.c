#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc periph_clk2_desc = {
        {
                IMX1050_CLK_PLL_USB1,
                IMX1050_CLK_OSC_24M,
        },
        2,
        (void *)&CCM->CBCMR,
        12,
        2
};

SIPMLE_MUX_CLK_DEFS(periph_clk2_sel,periph_clk2_desc)

struct clk * imx1050_clk_register_periph_clk2_sel(
        struct clk *p_periph_clk2,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_periph_clk2,
            clk_id,
            name,
            &periph_clk2_sel_ops);
}
