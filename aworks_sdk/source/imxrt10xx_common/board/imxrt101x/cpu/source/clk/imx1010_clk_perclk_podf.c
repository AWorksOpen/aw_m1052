#include "clk_provider.h"
#include "imx1010_clk.h"
#include "simple_div_clk.h"
#include "imx1010_ccm_regs.h"
#include "imx1010_reg_base.h"

aw_const static struct simple_div_clk_desc perclk_podf_desc = {
        &CCM->CSCMR1,
        0,
        6,
        1,
        64,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(perclk_podf,perclk_podf_desc)

struct clk * imx1010_clk_register_perclk_podf(
        struct clk *p_perclk_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_perclk_podf,
            clk_id,
            name,
            parent_id,
            &perclk_podf_ops);
}
