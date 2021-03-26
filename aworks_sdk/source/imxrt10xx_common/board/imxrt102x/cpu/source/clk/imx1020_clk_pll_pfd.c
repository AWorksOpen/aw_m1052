#include "clk_provider.h"
#include "imx1020_clk.h"
#include "simple_div_table_clk.h"
#include "simple_gate_clk.h"
#include "imx1020_ccm_regs.h"
#include "imx1020_reg_base.h"
#include "aw_bitops.h"

struct imx_pfd_clk_descripter
{
    volatile void  *reg;
    int             start_bit;
};

#define PFD_FRAC_NUM_BITS       6
#define PFD_FRAC_STABLE_BIT     PFD_FRAC_NUM_BITS
#define PFD_FRAc_GATE_BIT       (PFD_FRAC_STABLE_BIT + 1)

static aw_clk_rate_t imx_pfd_clk_recalc_rate(
        aw_clk_rate_t parent_rate,
        aw_const struct imx_pfd_clk_descripter *p_desc)
{
    uint32_t                frac;

    frac = AW_REG_BITS_GET32(p_desc->reg,p_desc->start_bit,PFD_FRAC_NUM_BITS);
    return (aw_clk_rate_t)((18 * (uint64_t)parent_rate )/frac);
}

static aw_err_t imx_pfd_clk_enable(
        aw_const struct imx_pfd_clk_descripter *p_desc)
{
    AW_REG_BIT_CLR32(p_desc->reg,
            p_desc->start_bit + PFD_FRAc_GATE_BIT);
    return AW_OK;
}

static aw_err_t imx_pfd_clk_disable(
        aw_const struct imx_pfd_clk_descripter *p_desc)
{
    AW_REG_BIT_SET32(p_desc->reg,
            p_desc->start_bit + PFD_FRAc_GATE_BIT);
    return AW_OK;
}

aw_err_t imx_pfd_clk_set_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate,
        aw_const struct imx_pfd_clk_descripter *p_desc)
{
    uint32_t            tmp;
    uint64_t            t64;

    t64 = parent_rate;
    t64*=18;
    t64 /= rate;
    if (t64 < 12) {
        t64 = 12;
    }
    if (t64 > 35) {
        t64 = 35;
    }
    tmp = (uint32_t)t64;
    AW_REG_BITS_SET32(p_desc->reg,p_desc->start_bit,6,tmp);
    return AW_OK;
}

#define IMX_PDF_CLK_DECL(name,desc) \
    static aw_err_t name##_enable(struct clk *hw) \
    { \
        return imx_pfd_clk_enable(&desc); \
    } \
    static aw_err_t name##_disable(struct clk *hw) \
    { \
        return imx_pfd_clk_disable(&desc); \
    } \
    static aw_clk_rate_t name##_recalc_rate(struct clk *hw,aw_clk_rate_t parent_rate) \
    { \
        return imx_pfd_clk_recalc_rate(parent_rate,&desc); \
    } \
    static aw_err_t name##_set_rate(struct clk *hw,aw_clk_rate_t rate,aw_clk_rate_t parent_rate) \
    { \
        return imx_pfd_clk_set_rate(hw,rate,parent_rate,&desc); \
    } \
    static const struct clk_ops name##_ops = { \
        .enable = name##_enable, \
        .disable = name##_disable, \
        .recalc_rate = name##_recalc_rate, \
        .set_rate = name##_set_rate,\
    };



aw_const static struct imx_pfd_clk_descripter pll2_pfd0_desc = {
        &CCM_ANAOLG->PFD_528,
        0
};

IMX_PDF_CLK_DECL(pll2_pfd0,pll2_pfd0_desc)

struct clk * imx1020_clk_register_pll2_pfd0(
        struct clk *p_pll2_pfd0,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll2_pfd0,
            clk_id,
            name,
            parent_id,
            &pll2_pfd0_ops);
}

aw_const static struct imx_pfd_clk_descripter pll2_pfd1_desc = {
        &CCM_ANAOLG->PFD_528,
        8
};

IMX_PDF_CLK_DECL(pll2_pfd1,pll2_pfd1_desc)

struct clk * imx1020_clk_register_pll2_pfd1(
        struct clk *p_pll2_pfd1,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll2_pfd1,
            clk_id,
            name,
            parent_id,
            &pll2_pfd1_ops);
}

aw_const static struct imx_pfd_clk_descripter pll2_pfd2_desc = {
        &CCM_ANAOLG->PFD_528,
        16
};

IMX_PDF_CLK_DECL(pll2_pfd2,pll2_pfd2_desc)

struct clk * imx1020_clk_register_pll2_pfd2(
        struct clk *p_pll2_pfd2,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll2_pfd2,
            clk_id,
            name,
            parent_id,
            &pll2_pfd2_ops);
}

aw_const static struct imx_pfd_clk_descripter pll2_pfd3_desc = {
        &CCM_ANAOLG->PFD_528,
        24
};

IMX_PDF_CLK_DECL(pll2_pfd3,pll2_pfd3_desc)

struct clk * imx1020_clk_register_pll2_pfd3(
        struct clk *p_pll2_pfd3,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll2_pfd3,
            clk_id,
            name,
            parent_id,
            &pll2_pfd3_ops);
}
///////////////////////////////////////////////////////////////////////////////

aw_const static struct imx_pfd_clk_descripter pll3_pfd0_desc = {
        &CCM_ANAOLG->PFD_480,
        0
};

IMX_PDF_CLK_DECL(pll3_pfd0,pll3_pfd0_desc)

struct clk * imx1020_clk_register_pll3_pfd0(
        struct clk *p_pll3_pfd0,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll3_pfd0,
            clk_id,
            name,
            parent_id,
            &pll3_pfd0_ops);
}


aw_const static struct imx_pfd_clk_descripter pll3_pfd1_desc = {
        &CCM_ANAOLG->PFD_480,
        8
};

IMX_PDF_CLK_DECL(pll3_pfd1,pll3_pfd1_desc)

struct clk * imx1020_clk_register_pll3_pfd1(
        struct clk *p_pll3_pfd1,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll3_pfd1,
            clk_id,
            name,
            parent_id,
            &pll3_pfd1_ops);
}


aw_const static struct imx_pfd_clk_descripter pll3_pfd2_desc = {
        &CCM_ANAOLG->PFD_480,
        16
};

IMX_PDF_CLK_DECL(pll3_pfd2,pll3_pfd2_desc)

struct clk * imx1020_clk_register_pll3_pfd2(
        struct clk *p_pll3_pfd2,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll3_pfd2,
            clk_id,
            name,
            parent_id,
            &pll3_pfd2_ops);
}

aw_const static struct imx_pfd_clk_descripter pll3_pfd3_desc = {
        &CCM_ANAOLG->PFD_480,
        24
};

IMX_PDF_CLK_DECL(pll3_pfd3,pll3_pfd3_desc)

struct clk * imx1020_clk_register_pll3_pfd3(
        struct clk *p_pll3_pfd3,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_pll3_pfd3,
            clk_id,
            name,
            parent_id,
            &pll3_pfd3_ops);
}
