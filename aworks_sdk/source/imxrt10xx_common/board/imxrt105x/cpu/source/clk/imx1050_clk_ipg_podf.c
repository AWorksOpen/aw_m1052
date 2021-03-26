#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_div_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_div_clk_desc ipg_podf_desc = {
        &CCM->CBCDR,
        8,
        2,
        1,
        4,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(ipg_podf,ipg_podf_desc)

struct clk * imx1050_clk_register_ipg_podf(
        struct clk *p_ipg_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_ipg_podf,
            clk_id,
            name,
            parent_id,
            &ipg_podf_ops);
}
