#include "clk_provider.h"
#include "imx1020_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1020_ccm_regs.h"
#include "imx1020_reg_base.h"

#define IOMUXC_GPR_GPR1_REG_ADDR        0x400AC004

static void enet1_tx_ref_init(struct clk *hw)
{
    void                        *p_reg;
    uint32_t                    val;
    aw_clk_id_t                 parent_id;

    p_reg = (imx1020_ccm_regs_t *)IOMUXC_GPR_GPR1_REG_ADDR;
    val = readl(p_reg);
    if (val & (1 << 13)) {
        val &= ~(1 << 17);
        parent_id = IMX1020_CLK_ENET1_EX_TX_REF_IN;
    }
    else {
        val |= (1<< 17);
        parent_id = IMX1020_CLK_PLL_ENET;
    }

    hw->parent = __clk_find_by_id(parent_id);
    writel(val,p_reg);

}


static aw_err_t enet1_tx_ref_get_parent(struct clk *hw,aw_clk_id_t *p_parent_id)
{
    uint32_t                    val;
    aw_clk_id_t                 parent_id;
    void                        *p_reg;

    p_reg = (imx1020_ccm_regs_t *)IOMUXC_GPR_GPR1_REG_ADDR;
    val = readl(p_reg);
    if (val & (1 << 13)) {
        val &= ~(1 << 17);
        parent_id = IMX1020_CLK_ENET1_EX_TX_REF_IN;
    }
    else {
        val |= (1<< 17);
        parent_id = IMX1020_CLK_PLL_ENET;
    }

    * p_parent_id = parent_id;
    return AW_OK;

}
static aw_err_t enet1_tx_ref_set_parent(struct clk *hw,aw_clk_id_t parent_id)
{
    uint32_t                    val;
    void                        *p_reg;

    p_reg = (imx1020_ccm_regs_t *)IOMUXC_GPR_GPR1_REG_ADDR;
    val = readl(p_reg);
    if (IMX1020_CLK_ENET1_EX_TX_REF_IN == parent_id ) {
        val |= (1<<13);
        val &= ~(1 << 17);
    }
    else if (IMX1020_CLK_PLL_ENET == parent_id) {
        val &= ~(1<<13);
        val |= (1 << 17);
    }
    else {
        return -AW_ENOTSUP;
    }

    return AW_OK;
}

static const struct clk_ops __enet1_tx_ref_ops = {
        .init = enet1_tx_ref_init,
        .get_parent = enet1_tx_ref_get_parent,
        .set_parent = enet1_tx_ref_set_parent,
};

struct clk * imx1020_clk_register_enet1_tx_ref(
        struct clk *p_enet1_tx_ref,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_enet1_tx_ref,
            clk_id,
            name,
            &__enet1_tx_ref_ops);
}


