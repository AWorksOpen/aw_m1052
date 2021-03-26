#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_div_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_div_clk_desc usdhc2_podf_desc = {
        &CCM->CSCDR1,
        11,
        3,
        1,
        8,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(usdhc2_podf,usdhc2_podf_desc)

struct clk * imx1050_clk_register_usdhc2_podf(
        struct clk *p_usdhc2_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_usdhc2_podf,
            clk_id,
            name,
            parent_id,
            &usdhc2_podf_ops);
}
