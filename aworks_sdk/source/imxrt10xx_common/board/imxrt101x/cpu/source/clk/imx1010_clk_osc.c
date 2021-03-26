#include "clk_provider.h"
#include "imx1010_clk.h"
#include "imx1010_ccm_regs.h"
#include "imx1010_reg_base.h"
#include "aw_bitops.h"


struct clk * imx1010_clk_register_osc32k(
        struct clk_fixed_rate *p_osc32k,
        aw_clk_id_t clk_id,
        const char *name)
{
    return clk_register_fixed_rate(
            p_osc32k,
            clk_id,
            name,
            CLK_IS_ROOT,
            32768);
}


struct clk * imx1010_clk_register_osc24m(
        struct clk_fixed_rate *p_osc24m,
        aw_clk_id_t clk_id,
        const char *name)
{
    return clk_register_fixed_rate(
            p_osc24m,
            clk_id,
            name,
            CLK_IS_ROOT,
            24000000);
}
