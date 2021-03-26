#include "clk_provider.h"
#include "imx1020_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1020_ccm_regs.h"
#include "imx1020_reg_base.h"

static volatile void *__get_ccgr_reg(aw_clk_id_t clk_id)
{
    int                                 index;
    static volatile void * aw_const     __ccgr_regs[] = {
            &CCM->CCGR0,
            &CCM->CCGR1,
            &CCM->CCGR2,
            &CCM->CCGR3,
            &CCM->CCGR4,
            &CCM->CCGR5,
            &CCM->CCGR6
    };

    index = (clk_id - IMX1020_CLK_CG_AIPS_TZ1) / 16;
    if ( index >= AW_NELEMENTS(__ccgr_regs)) {
        while(1);
    }

    return __ccgr_regs[index];
}

static aw_err_t imx_gate2_clk_enable(struct clk * hw)
{
    int                 bit;
    aw_clk_id_t         clk_id;
    volatile void      *reg;

    clk_id = __clk_get_id(hw);
    bit = clk_id - IMX1020_CLK_CG_AIPS_TZ1;
    bit %= 16;
    bit *= 2;

    reg = __get_ccgr_reg(clk_id);
    AW_REG_BITS_SET32(reg,bit,2,0x3);
    return AW_OK;
}

static aw_err_t imx_gate2_clk_disable(struct clk * hw)
{
    int                 bit;
    aw_clk_id_t         clk_id;
    volatile void      *reg;

    clk_id = __clk_get_id(hw);
    bit = clk_id - IMX1020_CLK_CG_AIPS_TZ1;
    bit %= 16;
    bit *= 2;

    reg = __get_ccgr_reg(clk_id);
    AW_REG_BITS_SET32(reg,bit,2,0x0);
    return AW_OK;
}

aw_const static struct clk_ops gate2_ops = {
        .enable = imx_gate2_clk_enable,
        .disable = imx_gate2_clk_disable,
};



struct clk * imx1020_clk_register_gate2(
        struct clk *p_gate2,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_gate2,
            clk_id,
            name,
            parent_id,
            &gate2_ops);
}

