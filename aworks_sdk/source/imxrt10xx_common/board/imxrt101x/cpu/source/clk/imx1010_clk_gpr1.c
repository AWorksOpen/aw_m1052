#include "clk_provider.h"
#include "imx1010_clk.h"
#include "simple_div_table_clk.h"
#include "simple_gate_clk.h"
#include "imx1010_ccm_regs.h"
#include "imx1010_reg_base.h"
#include "aw_bitops.h"

#define IOMUXC_GPR_GPR1_REG_ADDR        0x400AC004

aw_const static struct simple_gate_desc __enet_ipg_desc = {
        (void *)IOMUXC_GPR_GPR1_REG_ADDR,
        23
};


SIPMLE_GATE_CLK_DEFS(__enet_ipg,__enet_ipg_desc);




struct clk * imx1010_clk_register_enet_ipg(
        struct clk *p_enet_ipg,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_enet_ipg,
            clk_id,
            name,
            parent_id,
            &__enet_ipg_ops);
}


aw_const static struct simple_gate_desc __cm7_ahb_desc = {
        (void *)IOMUXC_GPR_GPR1_REG_ADDR,
        31
};


SIPMLE_GATE_CLK_DEFS(__cm7_ahb,__cm7_ahb_desc);




struct clk * imx1010_clk_register_cm7_ahb(
        struct clk *p_cm7_ahb,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_cm7_ahb,
            clk_id,
            name,
            parent_id,
            &__cm7_ahb_ops);
}
