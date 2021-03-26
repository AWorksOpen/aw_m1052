#include "clk_provider.h"
#include "imx1020_clk.h"
#include "imx1020_ccm_regs.h"
#include "imx1020_reg_base.h"
#include "aw_bitops.h"



#define AV_PLL_MIN_DIV          27
#define AV_PLL_MAX_DIV          54
#define AV_PLL_MAX_NUM          0x3FFFFFFF

#define REG_NUM_OFFSET          0x10
#define REG_DENOM_OFFSET        0x20

static void pll_audio_init(struct clk *hw)
{
    imx1020_ccm_analog_regs_t  *p_ccm_analog;

    // 设置AUDIO POST DIV和AUDIO DIV为1
    p_ccm_analog = (imx1020_ccm_analog_regs_t *)IMX1020_CCM_ANALOG_BASE;

    // Audio post div,0x2 代表除1
    AW_REG_BITS_SET32(&p_ccm_analog->PLL_AUDIO,19,2,0x2);
    // Audio div,0x0代表除0
    AW_REG_BITS_SET32(&p_ccm_analog->MISC2,23,1,0x0);
    AW_REG_BITS_SET32(&p_ccm_analog->MISC2,15,1,0x0);
    // 使用osc作为bypass
    AW_REG_BITS_SET32(&p_ccm_analog->PLL_AUDIO,14,2,0x0);
    AW_REG_BIT_SET32(&p_ccm_analog->PLL_AUDIO,16);
}

typedef uint32_t (*pfn_get_func_div)(void);

aw_const static uint32_t pll_av_post_div_tab[] = {
        4,2,1,0
};

static aw_clk_rate_t pll_av_recalc_rate(
        volatile void *p_pll_reg,
        aw_clk_rate_t parent_rate,
        pfn_get_func_div fn_get_func_div)
{
    uint32_t            num;
    uint32_t            denom;
    uint32_t            pll_div;
    uint32_t            post_div;
    uint32_t            func_div;
    int                 bypass;
    uint64_t            t;

    func_div = fn_get_func_div();
    bypass = AW_REG_BIT_GET32(p_pll_reg,16);
    if (bypass) {
        // bypass模式，直接返回父时钟
        return parent_rate / func_div;
    }

    post_div = AW_REG_BITS_GET32(p_pll_reg,19,2);
    post_div = pll_av_post_div_tab[post_div];
    num = readl((volatile void *)((char *)p_pll_reg + REG_NUM_OFFSET));
    denom = readl((volatile void *)((char *)p_pll_reg + REG_DENOM_OFFSET));
    pll_div = AW_REG_BITS_GET32(p_pll_reg,0,7);

    t = parent_rate;
    t = (t * num)/denom;
    t += parent_rate * pll_div;

    return (aw_clk_rate_t)t/(post_div * func_div);
}

static aw_err_t pll_av_disable(volatile void *p_pll_reg)
{
    AW_REG_BIT_CLR32(p_pll_reg,13);

    // 禁用了pll输出后再使得PLL下电
    AW_REG_BIT_SET32(p_pll_reg,12);
    // 等待 PLL lock
    while (AW_REG_BIT_ISSET32(p_pll_reg,31)) ;
    return AW_OK;
}

static aw_err_t pll_av_enable(volatile void *p_pll_reg)
{
    int                         bypass;

    bypass = AW_REG_BIT_GET32(p_pll_reg,16);

    // 如果PLL不是bypass模式，则需要先给pll上电
    if (!bypass) {
        AW_REG_BIT_CLR32(p_pll_reg,12);
        // 等待 PLL lock
        while (!AW_REG_BIT_ISSET32(p_pll_reg,31)) ;
    }

    //
    AW_REG_BIT_SET32(p_pll_reg,13);
    return AW_OK;
}

struct pll_av_postdiv_table {
    int     div;
    int     post_val;   // 设置为
    int     func_val;
};

static aw_const struct pll_av_postdiv_table __pll_postdiv_table[] = {
        {1,2,0},    //div = 1,post_div=1,func_div=1,post_val = 0x2,func_val = 0x00
        {2,1,0},    //div = 2,post_div=2,func_div=1,post_val = 0x1,func_val = 0x00
        {4,0,0},    //div = 4,post_div=4,func_div=1,post_val = 0x0,func_val = 0x00
        {8,0,0x1},  //div = 8,post_div=4,func_div=2,post_val = 0x0,func_val = 0x01
        {16,0,0x3}, //div = 16,post_div=4,func_div=4,post_val = 0x0,func_val = 0x03
};

static aw_const struct clk_div_table    __bypass_post_div_table[] = {
        {1,0},  //div = 1,val = 0
        {2,1},  //div = 2,val = 1
        {4,3},  //div = 4,val = 3
};

static int __get_pll_av_max_div()
{
    int         i;
    int         max_div = __pll_postdiv_table[0].div;
    for (i =1 ;i < AW_NELEMENTS(__pll_postdiv_table);i++) {
        if (max_div < __pll_postdiv_table[i].div ) {
            max_div = __pll_postdiv_table[i].div;
        }

    }
    return max_div;
}

static int __get_bypass_max_div()
{
    int         i;
    int         max_div = __bypass_post_div_table[0].div;
    for (i =1 ;i < AW_NELEMENTS(__bypass_post_div_table);i++) {
        if (max_div < __bypass_post_div_table[i].div ) {
            max_div = __bypass_post_div_table[i].div;
        }

    }
    return max_div;
}

aw_const struct clk_div_table * __find_suitable_bypass_div(
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate)
{
    aw_const struct clk_div_table  *p_table;
    int                             i;
    uint32_t                        min_diff = ~(-1);
    uint32_t                        diff;
    aw_const struct clk_div_table  *p_suit_table = &__bypass_post_div_table[0];

    for (i = 0 ;i < AW_NELEMENTS(__bypass_post_div_table);i++) {
            p_table = &__bypass_post_div_table [i];
            diff = parent_rate / p_table->div;
            if (diff <= rate) {
                diff = rate - diff;
                if (min_diff > diff) {
                    min_diff = diff;
                    p_suit_table = p_table;
                }
            }
    }

    return p_suit_table;
}


static aw_clk_rate_t __pll_av_round_rate_internal(
        aw_clk_rate_t desired_rate,
        aw_const aw_clk_rate_t parent_rate,
        uint32_t *p_num,
        uint32_t *p_denom,
        uint32_t *p_pll_div,
        uint32_t *p_post_val,
        uint32_t *p_func_val,
        int      *p_bypass)
{
    uint32_t                rate;
    const uint32_t          pll_min_rate
                        = (AV_PLL_MIN_DIV * parent_rate)/__get_pll_av_max_div();
    const uint32_t          pll_max_rate
                        = (AV_PLL_MAX_DIV * parent_rate);
    const uint32_t          bypass_max_rate = parent_rate;
    const uint32_t          bypass_min_rate = parent_rate /__get_bypass_max_div();

    // 适配输入的频率，预防过大或过小的情况
    if (desired_rate > pll_max_rate) {
        desired_rate = pll_max_rate;
    }

    if (desired_rate < pll_min_rate) {
        if (desired_rate > bypass_max_rate) {
            desired_rate = bypass_max_rate;
        }
        else if (desired_rate < bypass_min_rate) {
            desired_rate = bypass_min_rate;
        }
        aw_const struct clk_div_table  *p_table;
        p_table = __find_suitable_bypass_div(desired_rate,parent_rate);
        * p_num = 1;
        * p_denom = parent_rate;
        * p_pll_div = AV_PLL_MIN_DIV;
        * p_post_val = 0x0;
        * p_func_val = p_table->val;
        * p_bypass = 1;
        desired_rate = parent_rate/p_table->div;
        return desired_rate;
    }
    else {
            uint32_t                                num = 0;
            uint32_t                                denom = parent_rate;
            uint32_t                                post_div;
            uint32_t                                pll_div;
            aw_const struct pll_av_postdiv_table   *p_table;
            int                                     i;

            for (i = 0; i < AW_NELEMENTS(__pll_postdiv_table);i++ ) {
                p_table = &__pll_postdiv_table[i];
                if ( (desired_rate * p_table->div) >=
                        (AV_PLL_MIN_DIV * parent_rate) ) {
                    post_div = p_table->div;
                    break;
                }
            }

            rate = desired_rate * p_table->div;
            pll_div = rate / parent_rate;
            num = rate % parent_rate;

            * p_num = num;
            * p_denom = denom;
            * p_pll_div = pll_div;
            * p_post_val = p_table->post_val;
            * p_func_val = p_table->func_val;
            * p_bypass = 0;

            return desired_rate;
    }
}



typedef void (*pfn_set_func_div)(uint32_t func_div);

static aw_err_t pll_av_set_rate(
        volatile void *p_pll_reg,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate,
        pfn_set_func_div fn_set_func_div)
{
    uint32_t                    num;
    uint32_t                    denom;
    uint32_t                    pll_div;
    uint32_t                    post_val;
    uint32_t                    func_val;
    int                         bypass;
    int                         enable;

    enable = AW_REG_BIT_GET32(p_pll_reg,13);

    // 首先设置PLL为bypass模式，以便设定pll频率
    AW_REG_BIT_SET32(p_pll_reg,16);
    // PLL power down
    AW_REG_BIT_SET32(p_pll_reg,12);
    // 等待 PLL unlock
    while (AW_REG_BIT_ISSET32(p_pll_reg,31)) ;

     __pll_av_round_rate_internal(
            rate,
            parent_rate,
            &num,
            &denom,
            &pll_div,
            &post_val,
            &func_val,
            &bypass);

     // 设置func_val
    fn_set_func_div(func_val);
    if (bypass) {
        return AW_OK;
     }

    // 设置div值
    AW_REG_BITS_SET32(p_pll_reg,0,7,pll_div);
    // 设置num和denom
    writel(num,(volatile void *)((char *)p_pll_reg + REG_NUM_OFFSET));
    writel(denom,(volatile void *)((char *)p_pll_reg + REG_DENOM_OFFSET));
    // 设置post val
    AW_REG_BITS_SET32(p_pll_reg,19,2,post_val);

    if (enable) {
        // 如果PLL已经输出使能，则首先需要将PLL上电，再取消bypass
        AW_REG_BIT_CLR32(p_pll_reg,12);
        // 等待 PLL lock
        while (!AW_REG_BIT_ISSET32(p_pll_reg,31)) ;
    }

    // pll的bypass位取消即可
    AW_REG_BIT_CLR32(p_pll_reg,16);
    return AW_OK;
}

aw_const static uint32_t pll_av_func_div_table[] = {
        0x1,0x2,0x1,0x4
};

static uint32_t pll_audio_get_audio_div()
{
    uint32_t            div_val = 0;

    imx1020_ccm_analog_regs_t  *p_ccm_analog;
    p_ccm_analog = (imx1020_ccm_analog_regs_t *)IMX1020_CCM_ANALOG_BASE;

    if (AW_REG_BIT_ISSET32(&p_ccm_analog->MISC2,15) ) {
        div_val |= 0x1;
    }

    if (AW_REG_BIT_ISSET32(&p_ccm_analog->MISC2,23) ) {
        div_val |= 0x2;
    }

    return pll_av_func_div_table[div_val];

}

static aw_clk_rate_t pll_audio_recalc_rate(
        struct clk *hw,
        aw_clk_rate_t parent_rate)
{
    imx1020_ccm_analog_regs_t  *p_ccm_analog;

    p_ccm_analog = (imx1020_ccm_analog_regs_t *)IMX1020_CCM_ANALOG_BASE;
    return pll_av_recalc_rate(
            &p_ccm_analog->PLL_AUDIO,
            parent_rate,
            pll_audio_get_audio_div);
}



static aw_err_t pll_audio_enable(struct clk *hw)
{
    imx1020_ccm_analog_regs_t  *p_ccm_analog;

    p_ccm_analog = (imx1020_ccm_analog_regs_t *)IMX1020_CCM_ANALOG_BASE;
    return pll_av_enable(&p_ccm_analog->PLL_AUDIO);
}

static aw_err_t pll_audio_disable(struct clk *hw)
{
    imx1020_ccm_analog_regs_t  *p_ccm_analog;

    p_ccm_analog = (imx1020_ccm_analog_regs_t *)IMX1020_CCM_ANALOG_BASE;
    return pll_av_disable(&p_ccm_analog->PLL_AUDIO);
}

static void pll_audio_set_audio_div_val(uint32_t div_val)
{
    imx1020_ccm_analog_regs_t  *p_ccm_analog;
    p_ccm_analog = (imx1020_ccm_analog_regs_t *)IMX1020_CCM_ANALOG_BASE;

    if (div_val & 0x1) {
        AW_REG_BIT_SET32(&p_ccm_analog->MISC2,15);
    }
    else {
        AW_REG_BIT_CLR32(&p_ccm_analog->MISC2,15);
    }

    if (div_val & 0x2) {
        AW_REG_BIT_SET32(&p_ccm_analog->MISC2,23);
    }
    else {
        AW_REG_BIT_CLR32(&p_ccm_analog->MISC2,23);
    }
}

static aw_err_t pll_audio_set_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate)
{
    imx1020_ccm_analog_regs_t  *p_ccm_analog;

    p_ccm_analog = (imx1020_ccm_analog_regs_t *)IMX1020_CCM_ANALOG_BASE;
    return pll_av_set_rate(&p_ccm_analog->PLL_AUDIO,
            rate,parent_rate,pll_audio_set_audio_div_val);
}


static aw_clk_rate_t pll_av_round_rate(
        struct clk *hw,
        aw_clk_rate_t desired_rate,
        aw_clk_rate_t *best_parent_rate)
{
    aw_clk_rate_t           parent_rate = *best_parent_rate;
    uint32_t                num;
    uint32_t                denom;
    uint32_t                div;
    uint32_t                post_val;
    uint32_t                func_val;
    int                     bypass;

    return __pll_av_round_rate_internal(
            desired_rate,
            parent_rate,
            &num,
            &denom,
            &div,
            &post_val,
            &func_val,
            &bypass
            );
}

aw_const static struct clk_ops pll_audio_ops = {
        .init = pll_audio_init,
        .recalc_rate = pll_audio_recalc_rate,
        .enable = pll_audio_enable,
        .disable = pll_audio_disable,
        .round_rate = pll_av_round_rate,
        .set_rate = pll_audio_set_rate,
};


struct clk * imx1020_clk_register_pll_audio(
        struct clk *p_pll_audio,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(p_pll_audio,clk_id,name,parent_id,&pll_audio_ops);
}



static uint32_t pll_video_get_video_div()
{
    uint32_t            div_val = 0;

    imx1020_ccm_analog_regs_t  *p_ccm_analog;
    p_ccm_analog = (imx1020_ccm_analog_regs_t *)IMX1020_CCM_ANALOG_BASE;

    div_val = AW_REG_BITS_GET32(&p_ccm_analog->MISC2,30,2);
    return pll_av_func_div_table[div_val];

}



static void pll_video_set_video_div_val(uint32_t div_val)
{
    imx1020_ccm_analog_regs_t  *p_ccm_analog;
    p_ccm_analog = (imx1020_ccm_analog_regs_t *)IMX1020_CCM_ANALOG_BASE;

    AW_REG_BITS_SET32(&p_ccm_analog->MISC2,30,2,div_val);
}


