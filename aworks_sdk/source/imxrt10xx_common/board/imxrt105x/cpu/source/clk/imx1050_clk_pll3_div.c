#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_div_const_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

SIPMLE_DIV_CONST_CLK_DEFS(pll3_div_4,4)

struct clk * imx1050_clk_register_pll3_div_4(
        struct clk *p_pll3_div4,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll3_div4,
            clk_id,
            name,
            parent_id,
            &pll3_div_4_ops);
}

SIPMLE_DIV_CONST_CLK_DEFS(pll3_div_6,6)

struct clk * imx1050_clk_register_pll3_div_6(
        struct clk *p_pll3_div6,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll3_div6,
            clk_id,
            name,
            parent_id,
            &pll3_div_6_ops);
}

SIPMLE_DIV_CONST_CLK_DEFS(pll3_div_8,8)

struct clk * imx1050_clk_register_pll3_div_8(
        struct clk *p_pll3_div8,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll3_div8,
            clk_id,
            name,
            parent_id,
            &pll3_div_8_ops);
}
