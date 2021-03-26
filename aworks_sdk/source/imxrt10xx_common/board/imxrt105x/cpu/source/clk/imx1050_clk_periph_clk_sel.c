#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc periph_clk_desc = {
        {
                IMX1050_CLK_PRE_PERIPH_CLK_SEL,
                IMX1050_CLK_PERIPH_CLK2_PODF,
        },
        2,
        (void *)&CCM->CBCDR,
        25,
        1
};

SIPMLE_MUX_CLK_DEFS(periph_clk_sel,periph_clk_desc)

struct clk * imx1050_clk_register_periph_clk_sel(
        struct clk *p_periph_clk,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_periph_clk,
            clk_id,
            name,
            &periph_clk_sel_ops);
}
