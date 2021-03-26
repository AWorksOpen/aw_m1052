#include "aworks.h"
#include "clk_provider.h"
#include "imx1020_clk.h"
#include "aw_bitops.h"

#define CLK_BUG_ON(x) if((x) ) {while(1);};

#define IMX_REGISTER_CLK_IN(name,id) \
        static struct clk_fixed_rate    name; \
        clk = imx1020_clk_register_##name( \
            &name,\
            (id),\
            #name); \
    CLK_BUG_ON(NULL == clk); \

#define IMX_REGISTER_CLK(name,id,parent_id) \
        static struct clk    name; \
        clk = imx1020_clk_register_##name( \
            &name,\
            (id),\
            #name, \
            parent_id); \
    CLK_BUG_ON(NULL == clk); \

#define IMX_REGISTER_CLK_NO_PARENT(name,id) \
        static struct clk    name; \
        clk = imx1020_clk_register_##name( \
            &name,\
            (id),\
            #name); \
    CLK_BUG_ON(NULL == clk); \

#define REGISTER_GATE2(name,id,parent_id) \
        static struct clk    name; \
        clk = imx1020_clk_register_gate2( \
            &name,\
            (id),\
            #name, \
            parent_id); \
    CLK_BUG_ON(NULL == clk); \


static struct clk      *__g_all_clk[IMX1020_CLK_MAX_COUNT] = {NULL};

extern void user_clock_init(void);

extern void Reset_Handler(void);
int aw_is_code_run_in_xip_mode(void)
{
    uint32_t            x = (uint32_t)&Reset_Handler;
    if (x >= 0x60000000 && x < 0x7F800000) {
        return 1;
    }
    return 0;
}

void aw_bsp_clock_init()
{
    struct clk         *clk;
    uint32_t            rate;
    aw_clk_id_t         clk_id;

    aw_clk_init(__g_all_clk,IMX1020_CLK_MAX_COUNT);

    IMX_REGISTER_CLK_IN(osc32k,IMX1020_CLK_OSC_32K);
    IMX_REGISTER_CLK_IN(osc24m,IMX1020_CLK_OSC_24M);

    IMX_REGISTER_CLK(pll_sys,IMX1020_CLK_PLL_SYS,IMX1020_CLK_OSC_24M);
    IMX_REGISTER_CLK(pll_usb1,IMX1020_CLK_PLL_USB1,IMX1020_CLK_OSC_24M);
    IMX_REGISTER_CLK(pll_audio,IMX1020_CLK_PLL_AUDIO,IMX1020_CLK_OSC_24M);
    IMX_REGISTER_CLK(pll_enet,IMX1020_CLK_PLL_ENET,IMX1020_CLK_OSC_24M);


    IMX_REGISTER_CLK(pll2_pfd0,IMX1020_CLK_PLL2_PFD0,IMX1020_CLK_PLL_SYS);
    IMX_REGISTER_CLK(pll2_pfd1,IMX1020_CLK_PLL2_PFD1,IMX1020_CLK_PLL_SYS);
    IMX_REGISTER_CLK(pll2_pfd2,IMX1020_CLK_PLL2_PFD2,IMX1020_CLK_PLL_SYS);
    IMX_REGISTER_CLK(pll2_pfd3,IMX1020_CLK_PLL2_PFD3,IMX1020_CLK_PLL_SYS);

    IMX_REGISTER_CLK(pll3_pfd0,IMX1020_CLK_PLL3_PFD0,IMX1020_CLK_PLL_USB1);
    IMX_REGISTER_CLK(pll3_pfd1,IMX1020_CLK_PLL3_PFD1,IMX1020_CLK_PLL_USB1);
    IMX_REGISTER_CLK(pll3_pfd2,IMX1020_CLK_PLL3_PFD2,IMX1020_CLK_PLL_USB1);
    IMX_REGISTER_CLK(pll3_pfd3,IMX1020_CLK_PLL3_PFD3,IMX1020_CLK_PLL_USB1);


    IMX_REGISTER_CLK(arm_podf,IMX1020_CLK_ARM_PODF,IMX1020_CLK_PLL_ENET);

///////////////////////////////////////////////////////////////////////////////
    IMX_REGISTER_CLK_NO_PARENT(pre_periph_clk_sel,IMX1020_CLK_PRE_PERIPH_CLK_SEL);
    IMX_REGISTER_CLK_NO_PARENT(periph_clk2_sel,IMX1020_CLK_PERIPH_CLK2_SEL);
    IMX_REGISTER_CLK(periph_clk2_podf,IMX1020_CLK_PERIPH_CLK2_PODF,IMX1020_CLK_PERIPH_CLK2_SEL);

    IMX_REGISTER_CLK_NO_PARENT(periph_clk_sel,IMX1020_CLK_PERIPH_CLK_SEL);
    IMX_REGISTER_CLK(ahb_podf,IMX1020_CLK_AHB_PODF,IMX1020_CLK_PERIPH_CLK_SEL);
    IMX_REGISTER_CLK(ipg_podf,IMX1020_CLK_IPG_PODF,IMX1020_CLK_AHB_PODF);

    IMX_REGISTER_CLK_NO_PARENT(perclk_clk_sel,IMX1020_CLK_PERCLK_CLK_SEL);
    IMX_REGISTER_CLK(perclk_podf,IMX1020_CLK_PERCLK_PODF,IMX1020_CLK_PERCLK_CLK_SEL);

////////////////////////////////////////////////////////////////////////////////
    IMX_REGISTER_CLK_NO_PARENT(usdhc1_clk_sel,IMX1020_CLK_USDHC1_CLK_SEL);
    IMX_REGISTER_CLK(usdhc1_podf,IMX1020_CLK_USDHC1_PODF,IMX1020_CLK_USDHC1_CLK_SEL);

    IMX_REGISTER_CLK_NO_PARENT(usdhc2_clk_sel,IMX1020_CLK_USDHC2_CLK_SEL);
    IMX_REGISTER_CLK(usdhc2_podf,IMX1020_CLK_USDHC2_PODF,IMX1020_CLK_USDHC2_CLK_SEL);

    IMX_REGISTER_CLK_NO_PARENT(semc_alt_sel,IMX1020_CLK_SEMC_ALT_SEL);
    IMX_REGISTER_CLK_NO_PARENT(semc_sel,IMX1020_CLK_SEMC_SEL);
    IMX_REGISTER_CLK(semc_podf,IMX1020_CLK_SEMC_PODF,IMX1020_CLK_SEMC_SEL);
////////////////////////////////////////////////////////////////////////////////
    IMX_REGISTER_CLK(pll3_div_4,IMX1020_CLK_PLL3_DIV_4,IMX1020_CLK_PLL_USB1);
    IMX_REGISTER_CLK(pll3_div_6,IMX1020_CLK_PLL3_DIV_6,IMX1020_CLK_PLL_USB1);
    IMX_REGISTER_CLK(pll3_div_8,IMX1020_CLK_PLL3_DIV_8,IMX1020_CLK_PLL_USB1);
////////////////////////////////////////////////////////////////////////////////

    IMX_REGISTER_CLK_NO_PARENT(flexspi_clk_sel,IMX1020_CLK_FLEXSPI_CLK_SEL);
    IMX_REGISTER_CLK(flexspi_podf,IMX1020_CLK_FLEXSPI_PODF,IMX1020_CLK_FLEXSPI_CLK_SEL);

    IMX_REGISTER_CLK_NO_PARENT(enc_clk_sel,IMX1020_CLK_ENC_CLK_SEL);
    IMX_REGISTER_CLK(enc_pred,IMX1020_CLK_ENC_PRED,IMX1020_CLK_ENC_CLK_SEL);
    IMX_REGISTER_CLK(enc_podf,IMX1020_CLK_ENC_PODF,IMX1020_CLK_ENC_PRED);
////////////////////////////////////////////////////////////////////////////////
    IMX_REGISTER_CLK_NO_PARENT(lpspi_clk_sel,IMX1020_CLK_LPSPI_CLK_SEL);
    IMX_REGISTER_CLK(lpspi_podf,IMX1020_CLK_LPSPI_PODF,IMX1020_CLK_LPSPI_CLK_SEL);

    IMX_REGISTER_CLK_NO_PARENT(trace_clk_sel,IMX1020_CLK_TRACE_CLK_SEL);
    IMX_REGISTER_CLK(trace_podf,IMX1020_CLK_TRACE_PODF,IMX1020_CLK_TRACE_CLK_SEL);
////////////////////////////////////////////////////////////////////////////////
    IMX_REGISTER_CLK_NO_PARENT(sai1_clk_sel,IMX1020_CLK_SAI1_CLK_SEL);
    IMX_REGISTER_CLK(sai1_pred,IMX1020_CLK_SAI1_PRED,IMX1020_CLK_SAI1_CLK_SEL);
    IMX_REGISTER_CLK(sai1_podf,IMX1020_CLK_SAI1_PODF,IMX1020_CLK_SAI1_PRED);

    IMX_REGISTER_CLK_NO_PARENT(sai2_clk_sel,IMX1020_CLK_SAI2_CLK_SEL);
    IMX_REGISTER_CLK(sai2_pred,IMX1020_CLK_SAI2_PRED,IMX1020_CLK_SAI2_CLK_SEL);
    IMX_REGISTER_CLK(sai2_podf,IMX1020_CLK_SAI2_PODF,IMX1020_CLK_SAI2_PRED);

    IMX_REGISTER_CLK_NO_PARENT(sai3_clk_sel,IMX1020_CLK_SAI3_CLK_SEL);
    IMX_REGISTER_CLK(sai3_pred,IMX1020_CLK_SAI3_PRED,IMX1020_CLK_SAI3_CLK_SEL);
    IMX_REGISTER_CLK(sai3_podf,IMX1020_CLK_SAI3_PODF,IMX1020_CLK_SAI3_PRED);
////////////////////////////////////////////////////////////////////////////////
    IMX_REGISTER_CLK_NO_PARENT(lpi2c_clk_sel,IMX1020_CLK_LPI2C_CLK_SEL);
    IMX_REGISTER_CLK(lpi2c_podf,IMX1020_CLK_LPI2C_PODF,IMX1020_CLK_LPI2C_CLK_SEL);

    IMX_REGISTER_CLK_NO_PARENT(can_clk_sel,IMX1020_CLK_CAN_CLK_SEL);
    IMX_REGISTER_CLK(can_podf,IMX1020_CLK_CAN_PODF,IMX1020_CLK_CAN_CLK_SEL);

    IMX_REGISTER_CLK_NO_PARENT(uart_clk_sel,IMX1020_CLK_UART_CLK_SEL);
    IMX_REGISTER_CLK(uart_podf,IMX1020_CLK_UART_PODF,IMX1020_CLK_UART_CLK_SEL);


////////////////////////////////////////////////////////////////////////////////
    IMX_REGISTER_CLK_NO_PARENT(spdif0_clk_sel,IMX1020_CLK_SPDIF0_CLK_SEL);
    IMX_REGISTER_CLK(spdif0_pred,IMX1020_CLK_SPDIF0_PRED,IMX1020_CLK_SPDIF0_CLK_SEL);
    IMX_REGISTER_CLK(spdif0_podf,IMX1020_CLK_SPDIF0_PODF,IMX1020_CLK_SPDIF0_PRED);

    IMX_REGISTER_CLK_NO_PARENT(flexio1_clk_sel,IMX1020_CLK_FLEXIO1_CLK_SEL);
    IMX_REGISTER_CLK(flexio1_pred,IMX1020_CLK_FLEXIO1_PRED,IMX1020_CLK_FLEXIO1_CLK_SEL);
    IMX_REGISTER_CLK(flexio1_podf,IMX1020_CLK_FLEXIO1_PODF,IMX1020_CLK_FLEXIO1_PRED);

    IMX_REGISTER_CLK_NO_PARENT(flexio2_clk_sel,IMX1020_CLK_FLEXIO2_CLK_SEL);
    IMX_REGISTER_CLK(flexio2_pred,IMX1020_CLK_FLEXIO2_PRED,IMX1020_CLK_FLEXIO2_CLK_SEL);
    IMX_REGISTER_CLK(flexio2_podf,IMX1020_CLK_FLEXIO2_PODF,IMX1020_CLK_FLEXIO2_PRED);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(aips_tz1, IMX1020_CLK_CG_AIPS_TZ1,IMX1020_CLK_AHB_CLK_ROOT);
    REGISTER_GATE2(aips_tz2, IMX1020_CLK_CG_AIPS_TZ2,IMX1020_CLK_AHB_CLK_ROOT);
    REGISTER_GATE2(mqs, IMX1020_CLK_CG_MQS,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(flexspi_exsc, IMX1020_CLK_CG_FLEXSPI_EXSC,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(sim_m_clk_r, IMX1020_CLK_CG_SIM_M_CLK_R,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(dcp, IMX1020_CLK_CG_DCP,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(lpuart3, IMX1020_CLK_CG_LPUART3,IMX1020_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(can1, IMX1020_CLK_CG_CAN1,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(can1_serial, IMX1020_CLK_CG_CAN1_SERIAL,IMX1020_CLK_CAN_CLK_ROOT);
    REGISTER_GATE2(can2, IMX1020_CLK_CG_CAN2,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(can2_serial, IMX1020_CLK_CG_CAN2_SERIAL,IMX1020_CLK_CAN_CLK_ROOT);
    REGISTER_GATE2(trace, IMX1020_CLK_CG_TRACE,IMX1020_CLK_TRACE_CLK_ROOT);
    REGISTER_GATE2(gpt2_bus, IMX1020_CLK_CG_GPT2_BUS,IMX1020_CLK_PERCLK_CLK_ROOT);
    REGISTER_GATE2(gpt2_serial, IMX1020_CLK_CG_GPT2_SERIAL,IMX1020_CLK_PERCLK_CLK_ROOT);
    REGISTER_GATE2(lpuart2, IMX1020_CLK_CG_LPUART2,IMX1020_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(gpio2, IMX1020_CLK_CG_GPIO2,IMX1020_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(lpspi1, IMX1020_CLK_CG_LPSPI1,IMX1020_CLK_LPSPI_CLK_ROOT);
    REGISTER_GATE2(lpspi2, IMX1020_CLK_CG_LPSPI2,IMX1020_CLK_LPSPI_CLK_ROOT);
    REGISTER_GATE2(lpspi3, IMX1020_CLK_CG_LPSPI3,IMX1020_CLK_LPSPI_CLK_ROOT);
    REGISTER_GATE2(lpspi4, IMX1020_CLK_CG_LPSPI4,IMX1020_CLK_LPSPI_CLK_ROOT);
    REGISTER_GATE2(adc2, IMX1020_CLK_CG_ADC2,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(enet, IMX1020_CLK_CG_ENET,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(pit, IMX1020_CLK_CG_PIT,IMX1020_CLK_PERCLK_CLK_ROOT);
    REGISTER_GATE2(adc1, IMX1020_CLK_CG_ADC1,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(semc_exsc, IMX1020_CLK_CG_SEMC_EXSC,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(gpt_bus, IMX1020_CLK_CG_GPT_BUS,IMX1020_CLK_PERCLK_CLK_ROOT);
    REGISTER_GATE2(gpt_serial, IMX1020_CLK_CG_GPT_SERIAL,IMX1020_CLK_PERCLK_CLK_ROOT);
    REGISTER_GATE2(lpuart4, IMX1020_CLK_CG_LPUART4,IMX1020_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(gpio1, IMX1020_CLK_CG_GPIO1,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(csu, IMX1020_CLK_CG_CSU,IMX1020_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(ocram_exsc,IMX1020_CLK_CG_OCRAM_EXSC,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(iomuxc_snvs,IMX1020_CLK_CG_IOMUXC_SNVS,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(lpi2c1,IMX1020_CLK_CG_LPI2C1,IMX1020_CLK_LPI2C_CLK_ROOT);
    REGISTER_GATE2(lpi2c2,IMX1020_CLK_CG_LPI2C2,IMX1020_CLK_LPI2C_CLK_ROOT);
    REGISTER_GATE2(lpi2c3,IMX1020_CLK_CG_LPI2C3,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(ocotp,IMX1020_CLK_CG_OCOTP,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(xbar1,IMX1020_CLK_CG_XBAR1,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(xbar2,IMX1020_CLK_CG_XBAR2,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(gpio3,IMX1020_CLK_CG_GPIO3,IMX1020_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(lpuart5,IMX1020_CLK_CG_LPUART5,IMX1020_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(semc,IMX1020_CLK_CG_SEMC,IMX1020_CLK_SEMC_CLK_ROOT);
    REGISTER_GATE2(lpuart6,IMX1020_CLK_CG_LPUART6,IMX1020_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(aoi1,IMX1020_CLK_CG_AOI1,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(ewm,IMX1020_CLK_CG_EWM,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(wdog1,IMX1020_CLK_CG_WDOG1,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(flex_ram,IMX1020_CLK_CG_FLEX_RAM,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(acmp1,IMX1020_CLK_CG_ACMP1,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(acmp2,IMX1020_CLK_CG_ACMP2,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(acmp3,IMX1020_CLK_CG_ACMP3,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(acmp4,IMX1020_CLK_CG_ACMP4,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(ocram,IMX1020_CLK_CG_OCRAM,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(iomuxc_snvs_gpr,IMX1020_CLK_CG_IOMUXC_SNVS_GPR,IMX1020_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(sim_m7_clk_r, IMX1020_CLK_CG_SIM_M7_CLK_R,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(iomuxc, IMX1020_CLK_CG_IOMUXC,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(iomuxc_gpr, IMX1020_CLK_CG_IOMUXC_GPR,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(bee, IMX1020_CLK_CG_BEE,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(sim_m7, IMX1020_CLK_CG_SIM_M7,IMX1020_CLK_AHB_CLK_ROOT);
    REGISTER_GATE2(sim_m, IMX1020_CLK_CG_SIM_M,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(sim_ems, IMX1020_CLK_CG_SIM_EMS,IMX1020_CLK_AHB_CLK_ROOT);
    REGISTER_GATE2(pwm1, IMX1020_CLK_CG_PWM1,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(pwm2, IMX1020_CLK_CG_PWM2,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(enc1, IMX1020_CLK_CG_ENC1,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(enc2, IMX1020_CLK_CG_ENC2,IMX1020_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////

    REGISTER_GATE2(rom, IMX1020_CLK_CG_ROM,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(flexio1, IMX1020_CLK_CG_FLEXIO1,IMX1020_CLK_FLEXIO1_CLK_ROOT);
    REGISTER_GATE2(wdog3, IMX1020_CLK_CG_WDOG3,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(dma, IMX1020_CLK_CG_DMA,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(kpp, IMX1020_CLK_CG_KPP,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(wdog2, IMX1020_CLK_CG_WDOG2,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(aips_tz4, IMX1020_CLK_CG_AIPS_TZ4,IMX1020_CLK_AHB_CLK_ROOT);
    REGISTER_GATE2(spdif, IMX1020_CLK_CG_SPDIF,IMX1020_CLK_SPDIF0_CLK_ROOT);
    REGISTER_GATE2(sai1, IMX1020_CLK_CG_SAI1,IMX1020_CLK_SAI1_CLK_ROOT);
    REGISTER_GATE2(sai2, IMX1020_CLK_CG_SAI2,IMX1020_CLK_SAI2_CLK_ROOT);
    REGISTER_GATE2(sai3, IMX1020_CLK_CG_SAI3,IMX1020_CLK_SAI3_CLK_ROOT);
    REGISTER_GATE2(lpuart1, IMX1020_CLK_CG_LPUART1,IMX1020_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(lpuart7, IMX1020_CLK_CG_LPUART7,IMX1020_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(snvs_hp, IMX1020_CLK_CG_SNVS_HP,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(snvs_lp, IMX1020_CLK_CG_SNVS_LP,IMX1020_CLK_IPG_CLK_ROOT);

////////////////////////////////////////////////////////////////////////////////
    REGISTER_GATE2(usboh3, IMX1020_CLK_CG_USBOH3,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(usdhc1, IMX1020_CLK_CG_USDHC1,IMX1020_CLK_USDHC1_CLK_ROOT);
    REGISTER_GATE2(usdhc2, IMX1020_CLK_CG_USDHC2,IMX1020_CLK_USDHC2_CLK_ROOT);
    REGISTER_GATE2(dcdc, IMX1020_CLK_CG_DCDC,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(flexspi, IMX1020_CLK_CG_FLEXSPI,IMX1020_CLK_FLEXSPI_CLK_ROOT);
    REGISTER_GATE2(trng, IMX1020_CLK_CG_TRNG,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(lpuart8, IMX1020_CLK_CG_LPUART8,IMX1020_CLK_UART_CLK_ROOT);
    REGISTER_GATE2(aips_tz3, IMX1020_CLK_CG_AIPS_TZ3,IMX1020_CLK_AHB_CLK_ROOT);
    REGISTER_GATE2(sim_per, IMX1020_CLK_CG_SIM_PER,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(anadig, IMX1020_CLK_CG_ANADIG,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(lpi2c4, IMX1020_CLK_CG_LPI2C4,IMX1020_CLK_LPI2C_CLK_ROOT);
    REGISTER_GATE2(timer1, IMX1020_CLK_CG_TIMER1,IMX1020_CLK_IPG_CLK_ROOT);
    REGISTER_GATE2(timer2, IMX1020_CLK_CG_TIMER2,IMX1020_CLK_IPG_CLK_ROOT);

    static struct clk_user_fixed_rate    external_enet_tx_ref_in;
    clk = clk_register_user_fixed_rate(
            &external_enet_tx_ref_in,
            IMX1020_CLK_ENET1_EX_TX_REF_IN,
            "external_enet_tx_ref_in",
            CLK_IS_ROOT);
    CLK_BUG_ON(NULL == clk);

    static struct clk                   enet_tx_ref;
    clk = imx1020_clk_register_enet1_tx_ref(
            &enet_tx_ref,
            IMX1020_CLK_ENET1_TX_REF,
            "enet_tx_ref");
    CLK_BUG_ON(NULL == clk);

    IMX_REGISTER_CLK(enet_ipg,IMX1020_CLK_ENET_IPG,IMX1020_CLK_IPG_CLK_ROOT);
    IMX_REGISTER_CLK(cm7_ahb,IMX1020_CLK_CM7_AHB,IMX1020_CLK_AHB_CLK_ROOT);

    /* 锁定semc的时钟配置 */
    if (!AW_REG_BIT_ISSET32(0x402f0000,1)) {
        clk = __g_all_clk[IMX1020_CLK_CG_SEMC];
        while(NULL != clk) {
            __clk_set_flags(clk,CLK_CANNOT_CHANGE);
            clk = clk->parent;
        }
        aw_clk_enable(IMX1020_CLK_CG_SEMC);
    }


    /* 如果代码是运行在XIP模式下，则需要锁定 */
    if (aw_is_code_run_in_xip_mode()) {
        clk = __g_all_clk[IMX1020_CLK_CG_FLEXSPI];
        while(NULL != clk) {
            __clk_set_flags(clk,CLK_CANNOT_CHANGE);
            clk = clk->parent;
        }
        aw_clk_enable(IMX1020_CLK_CG_FLEXSPI);
    }

    rate = aw_clk_rate_get(IMX1020_CLK_CG_FLEXSPI);

    int imxrt1020_cpu_freq_is_500M(void);

    // 在调整CPU时钟之前，首先将IPG时钟设置为AHB时钟的1/4
    rate = aw_clk_rate_get(IMX1020_CLK_AHB_CLK_ROOT);
    aw_clk_rate_set(IMX1020_CLK_IPG_CLK_ROOT,rate / 4);

    /* 配置CPU时钟为396M */
    clk_id = aw_clk_parent_get(IMX1020_CLK_PRE_PERIPH_CLK_SEL);
    if (IMX1020_CLK_PLL2_PFD3 == clk_id) {

        // 配置PLL_ENET为500M
        // AHB_CLK_ROOT为250M
        // 并且选择CPU时钟从PLL_ENET来
        aw_clk_rate_set(IMX1020_CLK_PLL_ENET,500000000);
        aw_clk_rate_set(IMX1020_CLK_ARM_PODF,250000000);
        aw_clk_enable(IMX1020_CLK_ARM_PODF);

        // 先把periph切换到periph_clk2，避免直接将父时钟切换到arm_podf时带来的不稳定
        aw_clk_parent_set(IMX1020_CLK_PERIPH_CLK_SEL,IMX1020_CLK_PERIPH_CLK2_PODF);
        aw_clk_parent_set(IMX1020_CLK_PRE_PERIPH_CLK_SEL,IMX1020_CLK_ARM_PODF);
        aw_clk_parent_set(IMX1020_CLK_PERIPH_CLK_SEL,IMX1020_CLK_PRE_PERIPH_CLK_SEL);
        aw_clk_rate_set(IMX1020_CLK_AHB_CLK_ROOT,250000000);
    }
    else {
        // 默认CPU时钟从PLL_ENET来
    }

    // 配置PLL2输出528M,PFD都输出396M
    aw_clk_rate_set(IMX1020_CLK_PLL_SYS,528000000);
    aw_clk_rate_set(IMX1020_CLK_PLL2_PFD0,396000000);
    aw_clk_rate_set(IMX1020_CLK_PLL2_PFD1,396000000);
    aw_clk_rate_set(IMX1020_CLK_PLL2_PFD2,396000000);
    aw_clk_rate_set(IMX1020_CLK_PLL2_PFD3,396000000);
    aw_clk_enable(IMX1020_CLK_PLL2_PFD3);

    // 此刻设置CPU时钟为396M
    // 先把periph切换到periph_clk2，避免直接将父时钟切换到pll2_pfd3时带来的不稳定
    aw_clk_parent_set(IMX1020_CLK_PERIPH_CLK_SEL,IMX1020_CLK_PERIPH_CLK2_PODF);
    aw_clk_parent_set(IMX1020_CLK_PRE_PERIPH_CLK_SEL,IMX1020_CLK_PLL2_PFD3);
    aw_clk_parent_set(IMX1020_CLK_PERIPH_CLK_SEL,IMX1020_CLK_PRE_PERIPH_CLK_SEL);
    aw_clk_rate_set(IMX1020_CLK_AHB_CLK_ROOT,396000000);

    if (IMX1020_CLK_PLL2_PFD3 == clk_id) {
        aw_clk_disable(IMX1020_CLK_ARM_PODF);
    }

    if (imxrt1020_cpu_freq_is_500M() ) {
        // 如果CPU频率为500M，那么就只能使用PLL_ENET
        aw_clk_rate_set(IMX1020_CLK_PLL_ENET,500000000);
        aw_clk_rate_set(IMX1020_CLK_ARM_PODF,500000000);
        aw_clk_enable(IMX1020_CLK_ARM_PODF);

        // 先把periph切换到periph_clk2，避免直接将父时钟切换到arm_podf时带来的不稳定
        aw_clk_parent_set(IMX1020_CLK_PERIPH_CLK_SEL,IMX1020_CLK_PERIPH_CLK2_PODF);
        aw_clk_parent_set(IMX1020_CLK_PRE_PERIPH_CLK_SEL,IMX1020_CLK_ARM_PODF);
        aw_clk_parent_set(IMX1020_CLK_PERIPH_CLK_SEL,IMX1020_CLK_PRE_PERIPH_CLK_SEL);
        aw_clk_rate_set(IMX1020_CLK_AHB_CLK_ROOT,500000000);
        aw_clk_disable(IMX1020_CLK_PLL2_PFD3);

        aw_clk_enable(IMX1020_CLK_AHB_CLK_ROOT);
        aw_clk_disable(IMX1020_CLK_ARM_PODF);
    }
    else {
        aw_clk_enable(IMX1020_CLK_AHB_CLK_ROOT);
        aw_clk_disable(IMX1020_CLK_PLL2_PFD3);
    }

    rate = aw_clk_rate_get(IMX1020_CLK_AHB_CLK_ROOT);
    rate = aw_clk_rate_get(IMX1020_CLK_IPG_CLK_ROOT);


    rate = aw_clk_parent_set(IMX1020_CLK_PERCLK_CLK_SEL,IMX1020_CLK_OSC_24M);
    rate = aw_clk_rate_get(IMX1020_CLK_PERCLK_CLK_SEL);

    /* 设置pll2 pfds */
    aw_clk_rate_set(IMX1020_CLK_PLL2_PFD0,352000000);
    aw_clk_rate_set(IMX1020_CLK_PLL2_PFD1,594000000);
    aw_clk_rate_set(IMX1020_CLK_PLL2_PFD2,396000000);
    aw_clk_rate_set(IMX1020_CLK_PLL2_PFD3,297000000);

    /* 设置pll3 */
    aw_clk_rate_set(IMX1020_CLK_PLL_USB1,480000000);

    aw_clk_enable(IMX1020_CLK_CM7_AHB);
    aw_clk_enable(IMX1020_CLK_CG_SIM_PER);
    //aw_clk_enable(IMX1020_CLK_CG_AIPS_TZ3);
    aw_clk_enable(IMX1020_CLK_CG_ANADIG);
    aw_clk_enable(IMX1020_CLK_CG_DCDC);

    aw_clk_enable(IMX1020_CLK_CG_SIM_M7);
    aw_clk_enable(IMX1020_CLK_CG_IOMUXC_GPR);
    aw_clk_enable(IMX1020_CLK_CG_IOMUXC);

    aw_clk_enable(IMX1020_CLK_CG_OCRAM);
    aw_clk_enable(IMX1020_CLK_CG_OCOTP);
    aw_clk_enable(IMX1020_CLK_CG_TRACE);

    aw_clk_enable(IMX1020_CLK_CG_AIPS_TZ1);
    aw_clk_enable(IMX1020_CLK_CG_AIPS_TZ2);
    aw_clk_enable(IMX1020_CLK_CG_AIPS_TZ3);
    aw_clk_enable(IMX1020_CLK_CG_AIPS_TZ4);

//    aw_clk_enable(IMX1020_CLK_CG_PWM2);
//    aw_clk_enable(IMX1020_CLK_CG_PWM1);
//    aw_clk_enable(IMX1020_CLK_CG_SIM_EMS);
//    aw_clk_enable(IMX1020_CLK_CG_SIM_M);
//    aw_clk_enable(IMX1020_CLK_CG_TSC_DIG);
    user_clock_init();

#if 0
    for (i = IMX1020_CLK_MAX_COUNT - 1;i>=0;i--) {
        clk = __g_all_clk[i];
        if (NULL != clk) {
            if (NULL != clk->ops) {
                if ( NULL != clk->ops->disable) {
                    if (0 == clk->ref_count ) {
                        clk->ops->disable(clk);
                    }
                }
            }
        }
    }
#endif

    /* 串口时钟为osc时钟 */
    aw_clk_parent_set(IMX1020_CLK_UART_CLK_SEL,IMX1020_CLK_OSC_24M);
    return ;
}

