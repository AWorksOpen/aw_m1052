#include "clk_provider.h"
#include "imx1050_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"
#include "aw_bitops.h"

#define PLL_SYS_LOCK_RATE (528 * 1000000)

static aw_clk_rate_t pll_sys_round_rate(
        struct clk *hw,
        aw_clk_rate_t desired_rate,
        aw_clk_rate_t *best_parent_rate)
{
    aw_clk_rate_t           parent_rate = *best_parent_rate;

    if (desired_rate < PLL_SYS_LOCK_RATE) {
        return parent_rate;
    }

    return PLL_SYS_LOCK_RATE;
}


static aw_clk_rate_t pll_sys_recalc_rate(
        struct clk *hw,
        aw_clk_rate_t parent_rate)
{
    imx1050_ccm_analog_regs_t   *p_ccm_analog;
    int                         bypass;

    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;

    bypass = AW_REG_BIT_GET32(&p_ccm_analog->PLL_SYS,16);

    if (bypass) {
        return parent_rate;
    }
    else {
        return PLL_SYS_LOCK_RATE;
    }
}

static aw_err_t pll_sys_set_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate)
{
    uint32_t                    div = 1;
    imx1050_ccm_analog_regs_t  *p_ccm_analog;
    int                         enable;

    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;

    enable = AW_REG_BIT_GET32(&p_ccm_analog->PLL_SYS,13);

    // ����PLLΪbypass
    AW_REG_BIT_SET32(&p_ccm_analog->PLL_SYS,16);
    // PLL power down
    AW_REG_BIT_SET32(&p_ccm_analog->PLL_SYS,12);
    // �ȴ� PLL unlock
    while (AW_REG_BIT_ISSET32(&p_ccm_analog->PLL_SYS,31)) ;

    // �����Ҫ���õ�Ƶ��̫С������Ҫֱ��ʹ�ø�ʱ��Ƶ��
    if (rate < PLL_SYS_LOCK_RATE) {
        return AW_OK;
    }


    // ����divֵ
    AW_REG_BITS_SET32(&p_ccm_analog->PLL_SYS,0,1,div);


    if (enable) {
        // ���PLL�Ѿ����ʹ�ܣ���������Ҫ��PLL�ϵ磬��ȡ��bypass
        AW_REG_BIT_CLR32(&p_ccm_analog->PLL_SYS,12);
        // �ȴ� PLL lock
        while (!AW_REG_BIT_ISSET32(&p_ccm_analog->PLL_SYS,31)) ;
    }

    // pll��bypassλȡ������
    AW_REG_BIT_CLR32(&p_ccm_analog->PLL_SYS,16);
    return AW_OK;
}

static aw_err_t pll_sys_enable(struct clk *hw)
{
    imx1050_ccm_analog_regs_t  *p_ccm_analog;
    int                         bypass;

    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;
    bypass = AW_REG_BIT_GET32(&p_ccm_analog->PLL_SYS,16);

    // ���PLL����bypassģʽ������Ҫ�ȸ�pll�ϵ�
    if (!bypass) {
        AW_REG_BIT_CLR32(&p_ccm_analog->PLL_SYS,12);
        // �ȴ� PLL lock
        while (!AW_REG_BIT_ISSET32(&p_ccm_analog->PLL_SYS,31)) ;
    }

    //
    AW_REG_BIT_SET32(&p_ccm_analog->PLL_SYS,13);
    return AW_OK;
}

static aw_err_t pll_sys_disable(struct clk *hw)
{
    imx1050_ccm_analog_regs_t  *p_ccm_analog;

    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;

    AW_REG_BIT_CLR32(&p_ccm_analog->PLL_SYS,13);

    // ������pll�������ʹ��PLL�µ�
    AW_REG_BIT_SET32(&p_ccm_analog->PLL_SYS,12);
    // �ȴ� PLL lock
    while (AW_REG_BIT_ISSET32(&p_ccm_analog->PLL_SYS,31)) ;
    return AW_OK;
}

aw_const static struct clk_ops pll_arm_ops = {
        .set_rate = pll_sys_set_rate,
        .round_rate = pll_sys_round_rate,
        .recalc_rate = pll_sys_recalc_rate,
        .enable = pll_sys_enable,
        .disable = pll_sys_disable,
};


struct clk * imx1050_clk_register_pll_sys(
        struct clk *p_pll_sys,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(p_pll_sys,clk_id,name,parent_id,&pll_arm_ops);
}
