#include "clk_provider.h"
#include "imx1020_clk.h"
#include "simple_div_clk.h"
#include "imx1020_ccm_regs.h"
#include "imx1020_reg_base.h"

aw_const static struct simple_div_clk_desc arm_podf_desc = {
        &CCM->CACRR,
        0,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(arm_podf,arm_podf_desc)

struct clk * imx1020_clk_register_arm_podf(
        struct clk *arm_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            arm_podf,
            clk_id,
            name,
            parent_id,
            &arm_podf_ops);
}
