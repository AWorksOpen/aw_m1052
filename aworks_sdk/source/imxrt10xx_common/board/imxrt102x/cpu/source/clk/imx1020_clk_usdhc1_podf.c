#include "clk_provider.h"
#include "imx1020_clk.h"
#include "simple_div_clk.h"
#include "imx1020_ccm_regs.h"
#include "imx1020_reg_base.h"

aw_const static struct simple_div_clk_desc usdhc1_podf_desc = {
        &CCM->CSCDR1,
        11,
        3,
        1,
        8,
        1,  //����ֵҪ��һ
};

SIPMLE_DIV_CLK_DEFS(usdhc1_podf,usdhc1_podf_desc)

struct clk * imx1020_clk_register_usdhc1_podf(
        struct clk *p_usdhc1_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_usdhc1_podf,
            clk_id,
            name,
            parent_id,
            &usdhc1_podf_ops);
}
