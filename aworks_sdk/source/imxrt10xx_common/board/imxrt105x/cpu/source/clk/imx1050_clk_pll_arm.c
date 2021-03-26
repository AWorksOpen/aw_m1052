#include "clk_provider.h"
#include "imx1050_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"
#include "aw_bitops.h"

#define PLL_ARM_MIN_RATE (24*54/2*1000000)
#define PLL_ARM_MAX_RATE (24*108/2*1000000)

static aw_clk_rate_t pll_arm_round_rate(
        struct clk *hw,
        aw_clk_rate_t desired_rate,
        aw_clk_rate_t *best_parent_rate)
{
    aw_clk_rate_t           parent_rate = *best_parent_rate;
    aw_clk_rate_t           rate;
    uint32_t                div;

    if (desired_rate < PLL_ARM_MIN_RATE) {
        return parent_rate;
    }

    div = (desired_rate * 2)/parent_rate;
    if (div > 108) {
        div = 108;
    }
    rate = (parent_rate * div) / 2;

    return rate;
}


static aw_clk_rate_t pll_arm_recalc_rate(
        struct clk *hw,
        aw_clk_rate_t parent_rate)
{
    uint32_t                    div;
    imx1050_ccm_analog_regs_t   *p_ccm_analog;
    int                         bypass;

    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;

    div = AW_REG_BITS_GET32(&p_ccm_analog->PLL_ARM,0,7);
    bypass = AW_REG_BIT_GET32(&p_ccm_analog->PLL_ARM,16);

    if (bypass) {
        return parent_rate;
    }
    else {
        return parent_rate * div /2;
    }
}

static aw_err_t pll_arm_set_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate)
{
    uint32_t                    div;
    imx1050_ccm_analog_regs_t  *p_ccm_analog;
    int                         enable;

    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;


    // ���ȼ������Ҫ���õ�divֵ
    div = (rate * 2)/parent_rate;
    if (div > 108) {
        div = 108;
    }

    enable = AW_REG_BIT_GET32(&p_ccm_analog->PLL_ARM,13);

    // ����PLLΪbypass
    AW_REG_BIT_SET32(&p_ccm_analog->PLL_ARM,16);
    // PLL power down
    AW_REG_BIT_SET32(&p_ccm_analog->PLL_ARM,12);
    // �ȴ� PLL unlock
    while (AW_REG_BIT_ISSET32(&p_ccm_analog->PLL_ARM,31)) ;

    // �����Ҫ���õ�Ƶ��̫С������Ҫֱ��ʹ�ø�ʱ��Ƶ��
    if (rate < PLL_ARM_MIN_RATE) {
        return AW_OK;
    }

    // ����divֵ
    AW_REG_BITS_SET32(&p_ccm_analog->PLL_ARM,0,7,div);


    if (enable) {
        // ���PLL�Ѿ����ʹ�ܣ���������Ҫ��PLL�ϵ磬��ȡ��bypass
        AW_REG_BIT_CLR32(&p_ccm_analog->PLL_ARM,12);
        // �ȴ� PLL lock
        while (!AW_REG_BIT_ISSET32(&p_ccm_analog->PLL_ARM,31)) ;
    }

    // pll��bypassλȡ������
    AW_REG_BIT_CLR32(&p_ccm_analog->PLL_ARM,16);
    return AW_OK;
}

static aw_err_t pll_arm_enable(struct clk *hw)
{
    imx1050_ccm_analog_regs_t  *p_ccm_analog;
    int                         bypass;

    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;
    bypass = AW_REG_BIT_GET32(&p_ccm_analog->PLL_ARM,16);

    // ���PLL����bypassģʽ������Ҫ�ȸ�pll�ϵ�
    if (!bypass) {
        AW_REG_BIT_CLR32(&p_ccm_analog->PLL_ARM,12);
        // �ȴ� PLL lock
        while (!AW_REG_BIT_ISSET32(&p_ccm_analog->PLL_ARM,31)) ;
    }

    //
    AW_REG_BIT_SET32(&p_ccm_analog->PLL_ARM,13);
    return AW_OK;
}

static aw_err_t pll_arm_disable(struct clk *hw)
{
    imx1050_ccm_analog_regs_t  *p_ccm_analog;

    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;
    AW_REG_BIT_CLR32(&p_ccm_analog->PLL_ARM,13);
    // ������pll�������ʹ��PLL�µ�
    AW_REG_BIT_SET32(&p_ccm_analog->PLL_ARM,12);
    // �ȴ� PLL lock
    while (AW_REG_BIT_ISSET32(&p_ccm_analog->PLL_ARM,31)) ;
    return AW_OK;
}

aw_const static struct clk_ops pll_arm_ops = {
        .set_rate = pll_arm_set_rate,
        .round_rate = pll_arm_round_rate,
        .recalc_rate = pll_arm_recalc_rate,
        .enable = pll_arm_enable,
        .disable = pll_arm_disable,
};


struct clk * imx1050_clk_register_pll_arm(
        struct clk *p_pll_arm,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    struct clk_init_data        init = {0};
    struct clk                 *hw;

    hw = __clk_find_by_id(parent_id);
    init.name = name;
    init.clk_id = clk_id;
    init.parent_name = hw->name;
    init.flags = 0;
    init.ops = &pll_arm_ops;

    hw = clk_register_hw(p_pll_arm,&init);
    return hw;
}
