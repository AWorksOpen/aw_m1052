#include "clk_provider.h"
#include "imx1020_clk.h"
#include "simple_mux_clk.h"
#include "imx1020_ccm_regs.h"
#include "imx1020_reg_base.h"

aw_const static struct simple_mux_desc perclk_clk_desc = {
        {
                IMX1020_CLK_IPG_PODF,
                IMX1020_CLK_OSC_24M,
        },
        2,
        (void *)&CCM->CSCMR1,
        6,
        1
};

SIPMLE_MUX_CLK_DEFS(perclk_clk_sel,perclk_clk_desc)

struct clk * imx1020_clk_register_perclk_clk_sel(
        struct clk *p_perclk_clk,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_perclk_clk,
            clk_id,
            name,
            &perclk_clk_sel_ops);
}
