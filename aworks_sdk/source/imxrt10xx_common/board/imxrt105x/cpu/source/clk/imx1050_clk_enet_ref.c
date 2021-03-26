#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_div_table_clk.h"
#include "simple_gate_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"
#include "aw_bitops.h"

#define PLL_ENET_LOCK_RATE (500 * 1000000)

static aw_clk_rate_t enet_25m_recalc_rate(
        struct clk *hw,
        aw_clk_rate_t parent_rate)
{
    return 25000000;
}

aw_const static struct clk_ops enet_25m_ref_ops = {
        .recalc_rate = enet_25m_recalc_rate,
};


struct clk * imx1050_clk_register_enet_25m_ref(
        struct clk *p_enet_25m_ref,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(p_enet_25m_ref,clk_id,name,parent_id,&enet_25m_ref_ops);
}


aw_const static struct clk_div_table __enet_ref_div_table[]= {
        {0,20},     //val = 0,div = 20
        {1,10},     //val = 1,div = 10
        {2, 5},     //val = 2,div = 5
        {3, 4},     //val = 3,div = 4
};

aw_const static struct simple_div_table_clk_desc __enet1_ref_div_table_desc = {
        __enet_ref_div_table,
        (void *)&CCM_ANAOLG->PLL_ENET,
        0,
        2,
};

SIPMLE_DIV_TABLE_CLK_DEFS(enet1_ref_div,__enet1_ref_div_table_desc);

aw_const static struct simple_div_table_clk_desc __enet2_ref_div_table_desc = {
        __enet_ref_div_table,
        (void *)&CCM_ANAOLG->PLL_ENET,
        2,
        2,
};

SIPMLE_DIV_TABLE_CLK_DEFS(enet2_ref_div,__enet2_ref_div_table_desc);

aw_const static struct simple_gate_desc __enet1_ref_gate = {
        (void *)&CCM_ANAOLG->PLL_ENET,
        13
};

aw_const static struct simple_gate_desc __enet2_ref_gate = {
        (void *)&CCM_ANAOLG->PLL_ENET,
        20
};

SIPMLE_GATE_CLK_DEFS(enet1_ref_gate,__enet1_ref_gate);
SIPMLE_GATE_CLK_DEFS(enet2_ref_gate,__enet2_ref_gate);

aw_const static struct clk_ops enet1_ref_ops = {
        .round_rate = enet1_ref_div_round_rate,
        .set_rate = enet1_ref_div_set_rate,
        .recalc_rate = enet1_ref_div_recalc_rate,
        .enable = enet1_ref_gate_enable,
        .disable =enet1_ref_gate_disable,
};

aw_const static struct clk_ops enet2_ref_ops = {
        .round_rate = enet2_ref_div_round_rate,
        .set_rate = enet2_ref_div_set_rate,
        .recalc_rate = enet2_ref_div_recalc_rate,
        .enable = enet2_ref_gate_enable,
        .disable =enet2_ref_gate_disable,
};

struct clk * imx1050_clk_register_enet1_ref(
        struct clk *p_enet1_ref,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_enet1_ref,
            clk_id,
            name,
            parent_id,
            &enet1_ref_ops);
}

struct clk * imx1050_clk_register_enet2_ref(
        struct clk *p_enet2_ref,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_enet2_ref,
            clk_id,
            name,
            parent_id,
            &enet2_ref_ops);
}
