#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_div_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_div_clk_desc periph_clk2_podf_desc = {
        &CCM->CBCDR,
        27,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(periph_clk2_podf,periph_clk2_podf_desc)

struct clk * imx1050_clk_register_periph_clk2_podf(
        struct clk *p_periph_clk2_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_periph_clk2_podf,
            clk_id,
            name,
            parent_id,
            &periph_clk2_podf_ops);
}
