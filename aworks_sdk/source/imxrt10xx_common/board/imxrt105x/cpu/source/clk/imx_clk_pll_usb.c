#include "clk_provider.h"
#include "imx1050_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"
#include "aw_bitops.h"

#define PLL_USB_LOCK_RATE (480 * 1000000)

static aw_clk_rate_t pll_usb_round_rate(
        struct clk *hw,
        aw_clk_rate_t desired_rate,
        aw_clk_rate_t *best_parent_rate)
{
    aw_clk_rate_t           parent_rate = *best_parent_rate;

    if (desired_rate < PLL_USB_LOCK_RATE) {
        return parent_rate;
    }

    return PLL_USB_LOCK_RATE;
}

static aw_clk_rate_t pll_usb_recalc_rate(
        volatile void *p_reg,
        aw_clk_rate_t parent_rate)
{
    int                         bypass;
    bypass = AW_REG_BIT_GET32(p_reg,16);
    if (bypass) {
        return parent_rate;
    }
    else {
        return PLL_USB_LOCK_RATE;
    }
}

static aw_clk_rate_t pll_usb1_recalc_rate(
        struct clk *hw,
        aw_clk_rate_t parent_rate)
{
    imx1050_ccm_analog_regs_t   *p_ccm_analog;
    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;
    return pll_usb_recalc_rate(&p_ccm_analog->PLL_USB1,parent_rate);
}

static aw_err_t pll_usb_set_rate(
        volatile void *p_reg,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate )
{
    uint32_t                    div = 0;
    int                         enable;

    enable = AW_REG_BIT_GET32(p_reg,13);

    // 设置PLL为bypass
    AW_REG_BIT_SET32(p_reg,16);
    // PLL power down
    AW_REG_BIT_CLR32(p_reg,12);
    // 等待 PLL unlock
    while (AW_REG_BIT_ISSET32(p_reg,31)) ;

    // 如果想要设置的频率太小，则需要直接使用父时钟频率
    if (rate < PLL_USB_LOCK_RATE) {
        return AW_OK;
    }


    // 设置div值
    AW_REG_BITS_SET32(p_reg,0,1,div);
    if (enable) {
        // 如果PLL已经输出使能，则首先需要将PLL上电，再取消bypass
        AW_REG_BIT_SET32(p_reg,12);
        // 等待 PLL lock
        while (!AW_REG_BIT_ISSET32(p_reg,31)) ;
    }

    // pll的bypass位取消即可
    AW_REG_BIT_CLR32(p_reg,16);
    return AW_OK;
}

static aw_err_t pll_usb1_set_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate)
{

    imx1050_ccm_analog_regs_t  *p_ccm_analog;
    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;

    return pll_usb_set_rate(&p_ccm_analog->PLL_USB1,rate,parent_rate);
}

static aw_err_t pll_usb_enable(volatile void *p_reg)
{
    int                         bypass;

    bypass = AW_REG_BIT_GET32(p_reg,16);

    // 如果PLL不是bypass模式，则需要先给pll上电
    if (!bypass) {
        AW_REG_BIT_SET32(p_reg,12);
        // 等待 PLL lock
        while (!AW_REG_BIT_ISSET32(p_reg,31)) ;
    }

    //
    AW_REG_BIT_SET32(p_reg,13);
    return AW_OK;
}

static aw_err_t pll_usb1_enable(struct clk *hw)
{
    imx1050_ccm_analog_regs_t  *p_ccm_analog;

    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;

    return pll_usb_enable(&p_ccm_analog->PLL_USB1);
}

static aw_err_t pll_usb_disable(volatile void *p_reg)
{

    AW_REG_BIT_CLR32(p_reg,13);

    // 禁用了pll输出后再使得PLL下电
    AW_REG_BIT_CLR32(p_reg,12);
    // 等待 PLL lock
    while (AW_REG_BIT_ISSET32(p_reg,31)) ;
    return AW_OK;
}

static aw_err_t pll_usb1_disable(struct clk *hw)
{
    imx1050_ccm_analog_regs_t  *p_ccm_analog;
    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;

    return pll_usb_disable(&p_ccm_analog->PLL_USB1);
}

static void pll_usb1_init(struct clk *hw)
{
    imx1050_ccm_regs_t         *p_ccm;
    p_ccm = (imx1050_ccm_regs_t *)IMX1050_CCM_BASE;

    AW_REG_BIT_CLR32(&p_ccm->CCSR,0);
}

aw_const static struct clk_ops pll_usb1_ops = {
        .init = pll_usb1_init,
        .set_rate = pll_usb1_set_rate,
        .round_rate = pll_usb_round_rate,
        .recalc_rate = pll_usb1_recalc_rate,
        .enable = pll_usb1_enable,
        .disable = pll_usb1_disable,
};

struct clk * imx1050_clk_register_pll_usb1(
        struct clk *p_pll_usb1,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll_usb1,
            clk_id,
            name,
            parent_id,
            &pll_usb1_ops);
}

static aw_clk_rate_t pll_usb2_recalc_rate(
        struct clk *hw,
        aw_clk_rate_t parent_rate)
{
    imx1050_ccm_analog_regs_t   *p_ccm_analog;
    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;
    return pll_usb_recalc_rate(&p_ccm_analog->PLL_USB2,parent_rate);
}

static aw_err_t pll_usb2_set_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate)
{

    imx1050_ccm_analog_regs_t  *p_ccm_analog;
    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;

    return pll_usb_set_rate(&p_ccm_analog->PLL_USB2,rate,parent_rate);
}

static aw_err_t pll_usb2_enable(struct clk *hw)
{
    imx1050_ccm_analog_regs_t  *p_ccm_analog;

    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;

    return pll_usb_enable(&p_ccm_analog->PLL_USB2);
}

static aw_err_t pll_usb2_disable(struct clk *hw)
{
    imx1050_ccm_analog_regs_t  *p_ccm_analog;
    p_ccm_analog = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;

    return pll_usb_disable(&p_ccm_analog->PLL_USB2);
}

aw_const static struct clk_ops pll_usb2_ops = {
        .set_rate = pll_usb2_set_rate,
        .round_rate = pll_usb_round_rate,
        .recalc_rate = pll_usb2_recalc_rate,
        .enable = pll_usb2_enable,
        .disable = pll_usb2_disable,
};

struct clk * imx1050_clk_register_pll_usb2(
        struct clk *p_pll_usb2,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll_usb2,
            clk_id,
            name,
            parent_id,
            &pll_usb2_ops);
}

