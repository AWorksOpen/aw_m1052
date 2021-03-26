#include "aworks.h"
#include "clk_provider.h"
#include "imx1010_clk.h"
#include "aw_bitops.h"

#define CLK_BUG_ON(x) if((x) ) {while(1);};

#define IMX_REGISTER_CLK_IN(name, id) \
        static struct clk_fixed_rate    name; \
        clk = imx1010_clk_register_##name( \
            &name,\
            (id),\
            #name); \
    CLK_BUG_ON(NULL == clk); \

#define IMX_REGISTER_CLK(name, id, parent_id) \
        static struct clk    name; \
        clk = imx1010_clk_register_##name( \
            &name,\
            (id),\
            #name, \
            parent_id); \
    CLK_BUG_ON(NULL == clk); \

#define IMX_REGISTER_CLK_NO_PARENT(name, id) \
        static struct clk    name; \
        clk = imx1010_clk_register_##name( \
            &name,\
            (id),\
            #name); \
    CLK_BUG_ON(NULL == clk); \

#define REGISTER_GATE2(name, id, parent_id) \
        static struct clk    name; \
        clk = imx1010_clk_register_gate2( \
            &name,\
            (id),\
            #name, \
            parent_id); \
    CLK_BUG_ON(NULL == clk); \


static struct clk   *__g_all_clk[IMX1010_CLK_MAX_COUNT] = {NULL};

extern void user_clock_init (void);
extern void Reset_Handler (void);

int aw_is_code_run_in_xip_mode (void)
{
    uint32_t x = (uint32_t)&Reset_Handler;
    if (x >= 0x60000000 && x < 0x7F800000) {
        return 1;
    }
    return 0;
}

void aw_bsp_clock_init ()
{
    struct clk  *clk;
    uint32_t    rate;
    aw_clk_id_t clk_id;

    aw_clk_init(__g_all_clk, IMX1010_CLK_MAX_COUNT);

    IMX_REGISTER_CLK_IN(osc32k, IMX1010_CLK_OSC_32K);
    IMX_REGISTER_CLK_IN(osc24m, IMX1010_CLK_OSC_24M);

    IMX_REGISTER_CLK(pll_sys, IMX1010_CLK_PLL_SYS, IMX1010_CLK_OSC_24M);        //PLL2
    IMX_REGISTER_CLK(pll_usb1, IMX1010_CLK_PLL_USB1, IMX1010_CLK_OSC_24M);      //PLL3
    IMX_REGISTER_CLK(pll_audio, IMX1010_CLK_PLL_AUDIO, IMX1010_CLK_OSC_24M);    //PLL4

    IMX_REGISTER_CLK(pll2_pfd0, IMX1010_CLK_PLL2_PFD0, IMX1010_CLK_PLL_SYS);
    IMX_REGISTER_CLK(pll2_pfd1, IMX1010_CLK_PLL2_PFD1, IMX1010_CLK_PLL_SYS);
    IMX_REGISTER_CLK(pll2_pfd2, IMX1010_CLK_PLL2_PFD2, IMX1010_CLK_PLL_SYS);
    IMX_REGISTER_CLK(pll2_pfd3, IMX1010_CLK_PLL2_PFD3, IMX1010_CLK_PLL_SYS);

    IMX_REGISTER_CLK(pll3_pfd0, IMX1010_CLK_PLL3_PFD0, IMX1010_CLK_PLL_USB1);
    IMX_REGISTER_CLK(pll3_pfd1, IMX1010_CLK_PLL3_PFD1, IMX1010_CLK_PLL_USB1);
    IMX_REGISTER_CLK(pll3_pfd2, IMX1010_CLK_PLL3_PFD2, IMX1010_CLK_PLL_USB1);
    IMX_REGISTER_CLK(pll3_pfd3, IMX1010_CLK_PLL3_PFD3, IMX1010_CLK_PLL_USB1);

///////////////////////////////////////////////////////////////////////////////

    IMX_REGISTER_CLK_NO_PARENT(pre_periph_clk_sel, IMX1010_CLK_PRE_PERIPH_CLK_SEL);

    IMX_REGISTER_CLK_NO_PARENT(periph_clk_sel, IMX1010_CLK_PERIPH_CLK_SEL);
    IMX_REGISTER_CLK(ahb_podf, IMX1010_CLK_AHB_PODF, IMX1010_CLK_PERIPH_CLK_SEL);
    IMX_REGISTER_CLK(ipg_podf, IMX1010_CLK_IPG_PODF, IMX1010_CLK_AHB_PODF);

    IMX_REGISTER_CLK_NO_PARENT(perclk_clk_sel, IMX1010_CLK_PERCLK_CLK_SEL);
    IMX_REGISTER_CLK(perclk_podf, IMX1010_CLK_PERCLK_PODF, IMX1010_CLK_PERCLK_CLK_SEL);

////////////////////////////////////////////////////////////////////////////////

    IMX_REGISTER_CLK(pll3_div_4, IMX1010_CLK_PLL3_DIV_4, IMX1010_CLK_PLL_USB1);
    IMX_REGISTER_CLK(pll3_div_6, IMX1010_CLK_PLL3_DIV_6, IMX1010_CLK_PLL_USB1);
    IMX_REGISTER_CLK(pll3_div_8, IMX1010_CLK_PLL3_DIV_8, IMX1010_CLK_PLL_USB1);

////////////////////////////////////////////////////////////////////////////////

    IMX_REGISTER_CLK_NO_PARENT(flexspi_clk_sel, IMX1010_CLK_FLEXSPI_CLK_SEL);
    IMX_REGISTER_CLK(flexspi_podf, IMX1010_CLK_FLEXSPI_PODF, IMX1010_CLK_FLEXSPI_CLK_SEL);

////////////////////////////////////////////////////////////////////////////////

    IMX_REGISTER_CLK_NO_PARENT(lpspi_clk_sel, IMX1010_CLK_LPSPI_CLK_SEL);
    IMX_REGISTER_CLK(lpspi_podf, IMX1010_CLK_LPSPI_PODF, IMX1010_CLK_LPSPI_CLK_SEL);

    IMX_REGISTER_CLK_NO_PARENT(trace_clk_sel, IMX1010_CLK_TRACE_CLK_SEL);
    IMX_REGISTER_CLK(trace_podf, IMX1010_CLK_TRACE_PODF, IMX1010_CLK_TRACE_CLK_SEL);

////////////////////////////////////////////////////////////////////////////////

    IMX_REGISTER_CLK_NO_PARENT(sai1_clk_sel, IMX1010_CLK_SAI1_CLK_SEL);
    IMX_REGISTER_CLK(sai1_pred, IMX1010_CLK_SAI1_PRED, IMX1010_CLK_SAI1_CLK_SEL);
    IMX_REGISTER_CLK(sai1_podf, IMX1010_CLK_SAI1_PODF, IMX1010_CLK_SAI1_PRED);

    IMX_REGISTER_CLK_NO_PARENT(sai3_clk_sel, IMX1010_CLK_SAI3_CLK_SEL);
    IMX_REGISTER_CLK(sai3_pred, IMX1010_CLK_SAI3_PRED, IMX1010_CLK_SAI3_CLK_SEL);
    IMX_REGISTER_CLK(sai3_podf, IMX1010_CLK_SAI3_PODF, IMX1010_CLK_SAI3_PRED);

////////////////////////////////////////////////////////////////////////////////

    IMX_REGISTER_CLK_NO_PARENT(lpi2c_clk_sel, IMX1010_CLK_LPI2C_CLK_SEL);
    IMX_REGISTER_CLK(lpi2c_podf, IMX1010_CLK_LPI2C_PODF, IMX1010_CLK_LPI2C_CLK_SEL);

    IMX_REGISTER_CLK_NO_PARENT(uart_clk_sel, IMX1010_CLK_UART_CLK_SEL);
    IMX_REGISTER_CLK(uart_podf, IMX1010_CLK_UART_PODF, IMX1010_CLK_UART_CLK_SEL);

////////////////////////////////////////////////////////////////////////////////
    IMX_REGISTER_CLK_NO_PARENT(spdif0_clk_sel, IMX1010_CLK_SPDIF0_CLK_SEL);
    IMX_REGISTER_CLK(spdif0_pred, IMX1010_CLK_SPDIF0_PRED, IMX1010_CLK_SPDIF0_CLK_SEL);
    IMX_REGISTER_CLK(spdif0_podf, IMX1010_CLK_SPDIF0_PODF, IMX1010_CLK_SPDIF0_PRED);

    IMX_REGISTER_CLK_NO_PARENT(flexio1_clk_sel, IMX1010_CLK_FLEXIO1_CLK_SEL);
    IMX_REGISTER_CLK(flexio1_pred, IMX1010_CLK_FLEXIO1_PRED, IMX1010_CLK_FLEXIO1_CLK_SEL);
    IMX_REGISTER_CLK(flexio1_podf, IMX1010_CLK_FLEXIO1_PODF, IMX1010_CLK_FLEXIO1_PRED);

    IMX_REGISTER_CLK_NO_PARENT(flexio2_clk_sel, IMX1010_CLK_FLEXIO2_CLK_SEL);
    IMX_REGISTER_CLK(flexio2_pred, IMX1010_CLK_FLEXIO2_PRED, IMX1010_CLK_FLEXIO2_CLK_SEL);
    IMX_REGISTER_CLK(flexio2_podf, IMX1010_CLK_FLEXIO2_PODF, IMX1010_CLK_FLEXIO2_PRED);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(aips_tz1, IMX1010_CLK_CG_AIPS_TZ1, IMX1010_CLK_AHB_CLK_ROOT);
    REGISTER_GATE2(aips_tz2, IMX1010_CLK_CG_AIPS_TZ2, IMX1010_CLK_AHB_CLK_ROOT);
    REGISTER_GATE2(mqs, IMX1010_CLK_CG_MQS, IMX1010_CLK_SAI3_CLK_ROOT);
    REGISTER_GATE2(flexspi_exsc, IMX1010_CLK_CG_FLEXSPI_EXSC, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(sim_m_clk_r, IMX1010_CLK_CG_SIM_M_CLK_R, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(dcp, IMX1010_CLK_CG_DCP, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(lpuart3, IMX1010_CLK_CG_LPUART3, IMX1010_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(trace, IMX1010_CLK_CG_TRACE, IMX1010_CLK_TRACE_CLK_ROOT);
    REGISTER_GATE2(gpt2_bus, IMX1010_CLK_CG_GPT2_BUS, IMX1010_CLK_PERCLK_CLK_ROOT);
    REGISTER_GATE2(gpt2_serial, IMX1010_CLK_CG_GPT2_SERIAL, IMX1010_CLK_PERCLK_CLK_ROOT);
    REGISTER_GATE2(lpuart2, IMX1010_CLK_CG_LPUART2, IMX1010_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(gpio2, IMX1010_CLK_CG_GPIO2, IMX1010_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(lpspi1, IMX1010_CLK_CG_LPSPI1, IMX1010_CLK_LPSPI_CLK_ROOT);
    REGISTER_GATE2(lpspi2, IMX1010_CLK_CG_LPSPI2, IMX1010_CLK_LPSPI_CLK_ROOT);
    REGISTER_GATE2(pit, IMX1010_CLK_CG_PIT, IMX1010_CLK_PERCLK_CLK_ROOT);
    REGISTER_GATE2(adc1, IMX1010_CLK_CG_ADC1, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(gpt_bus, IMX1010_CLK_CG_GPT1_BUS, IMX1010_CLK_PERCLK_CLK_ROOT);
    REGISTER_GATE2(gpt_serial, IMX1010_CLK_CG_GPT1_SERIAL, IMX1010_CLK_PERCLK_CLK_ROOT);
    REGISTER_GATE2(lpuart4, IMX1010_CLK_CG_LPUART4, IMX1010_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(gpio1, IMX1010_CLK_CG_GPIO1, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(csu, IMX1010_CLK_CG_CSU, IMX1010_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(ocram_exsc, IMX1010_CLK_CG_OCRAM_EXSC, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(iomuxc_snvs, IMX1010_CLK_CG_IOMUXC_SNVS, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(lpi2c1, IMX1010_CLK_CG_LPI2C1, IMX1010_CLK_LPI2C_CLK_ROOT);
    REGISTER_GATE2(lpi2c2, IMX1010_CLK_CG_LPI2C2, IMX1010_CLK_LPI2C_CLK_ROOT);
    REGISTER_GATE2(ocotp, IMX1010_CLK_CG_OCOTP, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(xbar1, IMX1010_CLK_CG_XBAR1, IMX1010_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(aoi1, IMX1010_CLK_CG_AOI1, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(ewm, IMX1010_CLK_CG_EWM, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(wdog1, IMX1010_CLK_CG_WDOG1, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(flex_ram, IMX1010_CLK_CG_FLEX_RAM, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(ocram, IMX1010_CLK_CG_OCRAM, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(iomuxc_snvs_gpr, IMX1010_CLK_CG_IOMUXC_SNVS_GPR, IMX1010_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(sim_m7_clk_r, IMX1010_CLK_CG_SIM_M7_CLK_R, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(iomuxc, IMX1010_CLK_CG_IOMUXC, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(iomuxc_gpr, IMX1010_CLK_CG_IOMUXC_GPR, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(sim_m7, IMX1010_CLK_CG_SIM_M7, IMX1010_CLK_AHB_CLK_ROOT);
    REGISTER_GATE2(sim_m, IMX1010_CLK_CG_SIM_M, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(sim_ems, IMX1010_CLK_CG_SIM_EMS, IMX1010_CLK_AHB_CLK_ROOT);
    REGISTER_GATE2(pwm1, IMX1010_CLK_CG_PWM1, IMX1010_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(rom, IMX1010_CLK_CG_ROM, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(flexio1, IMX1010_CLK_CG_FLEXIO1, IMX1010_CLK_FLEXIO1_CLK_ROOT);
    REGISTER_GATE2(wdog3, IMX1010_CLK_CG_WDOG3, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(dma, IMX1010_CLK_CG_DMA, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(kpp, IMX1010_CLK_CG_KPP, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(wdog2, IMX1010_CLK_CG_WDOG2, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(spdif, IMX1010_CLK_CG_SPDIF, IMX1010_CLK_SPDIF0_CLK_ROOT);
    REGISTER_GATE2(sai1, IMX1010_CLK_CG_SAI1, IMX1010_CLK_SAI1_CLK_ROOT);
    REGISTER_GATE2(sai3, IMX1010_CLK_CG_SAI3, IMX1010_CLK_SAI3_CLK_ROOT);
    REGISTER_GATE2(lpuart1, IMX1010_CLK_CG_LPUART1, IMX1010_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(snvs_hp, IMX1010_CLK_CG_SNVS_HP, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(snvs_lp, IMX1010_CLK_CG_SNVS_LP, IMX1010_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////
    REGISTER_GATE2(usboh3, IMX1010_CLK_CG_USBOH3, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(dcdc, IMX1010_CLK_CG_DCDC, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(flexspi, IMX1010_CLK_CG_FLEXSPI, IMX1010_CLK_FLEXSPI_CLK_ROOT);
    REGISTER_GATE2(trng, IMX1010_CLK_CG_TRNG, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(sim_per, IMX1010_CLK_CG_SIM_PER, IMX1010_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(anadig, IMX1010_CLK_CG_ANADIG, IMX1010_CLK_IPG_CLK_ROOT);

    /* 如果代码是运行在XIP模式下，则需要锁定 */
    if (aw_is_code_run_in_xip_mode()) {
        clk = __g_all_clk[IMX1010_CLK_CG_FLEXSPI];
        while (NULL != clk) {
            __clk_set_flags(clk,CLK_CANNOT_CHANGE);
            clk = clk->parent;
        }
        aw_clk_enable(IMX1010_CLK_CG_FLEXSPI);
    }

    rate = aw_clk_rate_get(IMX1010_CLK_CG_FLEXSPI);

    // 在调整CPU时钟之前，首先将IPG时钟设置为AHB时钟的1/4
    rate = aw_clk_rate_get(IMX1010_CLK_AHB_CLK_ROOT);
    aw_clk_rate_set(IMX1010_CLK_IPG_CLK_ROOT, rate / 4);

    /* 配置CPU时钟为528M */
    aw_clk_rate_set(IMX1010_CLK_PLL_SYS, 528000000);

    // ahb时钟为528m
    aw_clk_parent_set(IMX1010_CLK_PRE_PERIPH_CLK_SEL, IMX1010_CLK_PLL_SYS);
    aw_clk_parent_set(IMX1010_CLK_PERIPH_CLK_SEL, IMX1010_CLK_PRE_PERIPH_CLK_SEL);
    aw_clk_rate_set(IMX1010_CLK_AHB_CLK_ROOT, 528000000);

    rate = aw_clk_rate_get(IMX1010_CLK_AHB_CLK_ROOT);
    aw_clk_enable(IMX1010_CLK_AHB_CLK_ROOT);

    /* ipg为132 */
    aw_clk_rate_set(IMX1010_CLK_IPG_CLK_ROOT, 132000000);
    rate = aw_clk_rate_get(IMX1010_CLK_IPG_CLK_ROOT);
    aw_clk_parent_set(IMX1010_CLK_PERCLK_CLK_SEL, IMX1010_CLK_OSC_24M);
    rate = aw_clk_rate_get(IMX1010_CLK_PERCLK_CLK_SEL);

    /* 设置pll2 pfds */
    aw_clk_rate_set(IMX1010_CLK_PLL2_PFD0, 352000000);
    aw_clk_rate_set(IMX1010_CLK_PLL2_PFD1, 594000000);
    aw_clk_rate_set(IMX1010_CLK_PLL2_PFD2, 396000000);
    aw_clk_rate_set(IMX1010_CLK_PLL2_PFD3, 297000000);

    /* 设置pll3 */
    aw_clk_rate_set(IMX1010_CLK_PLL_USB1, 480000000);

    aw_clk_enable(IMX1010_CLK_CG_SIM_PER);
    aw_clk_enable(IMX1010_CLK_CG_ANADIG);
    aw_clk_enable(IMX1010_CLK_CG_DCDC);

    aw_clk_enable(IMX1010_CLK_CG_SIM_M7);
    aw_clk_enable(IMX1010_CLK_CG_IOMUXC_GPR);
    aw_clk_enable(IMX1010_CLK_CG_IOMUXC);

    aw_clk_enable(IMX1010_CLK_CG_OCRAM);
    aw_clk_enable(IMX1010_CLK_CG_OCOTP);
    aw_clk_enable(IMX1010_CLK_CG_TRACE);

    aw_clk_enable(IMX1010_CLK_CG_AIPS_TZ1);
    aw_clk_enable(IMX1010_CLK_CG_AIPS_TZ2);

    user_clock_init();

    /* 串口时钟为osc时钟 */
    aw_clk_parent_set(IMX1010_CLK_UART_CLK_SEL, IMX1010_CLK_OSC_24M);
    return ;
}

/* end of file */
