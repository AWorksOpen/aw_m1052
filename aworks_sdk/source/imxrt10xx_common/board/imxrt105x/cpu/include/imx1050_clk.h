#ifndef         __IMX1050_CLK_H__
#define         __IMX1050_CLK_H__

#define IMX_CLK_DECL_IN(name) \
    struct clk * imx1050_clk_register_##name( \
        struct clk_fixed_rate *p_##name, \
        aw_clk_id_t clk_id, \
        const char *name); \


#define IMX_CLK_DECL(name) \
    struct clk * imx1050_clk_register_##name( \
        struct clk *p_##name, \
        aw_clk_id_t clk_id, \
        const char *name, \
        aw_clk_id_t parent_id); \

#define IMX_CLK_DECL_NO_PARENT(name) \
    struct clk * imx1050_clk_register_##name( \
        struct clk *p_##name, \
        aw_clk_id_t clk_id, \
        const char *name); \

// æß’Ò
IMX_CLK_DECL_IN(osc32k);
IMX_CLK_DECL_IN(osc24m);

//pllœ‡πÿ
IMX_CLK_DECL(pll_arm);
IMX_CLK_DECL(pll_sys);
IMX_CLK_DECL(pll_usb1);
IMX_CLK_DECL(pll_audio);
IMX_CLK_DECL(pll_video);
IMX_CLK_DECL(pll_enet);
IMX_CLK_DECL(pll_usb2);

IMX_CLK_DECL(enet1_ref);
IMX_CLK_DECL(enet2_ref);
IMX_CLK_DECL(enet_25m_ref);

// PFDS
IMX_CLK_DECL(pll2_pfd0);
IMX_CLK_DECL(pll2_pfd1);
IMX_CLK_DECL(pll2_pfd2);
IMX_CLK_DECL(pll2_pfd3);

IMX_CLK_DECL(pll3_pfd0);
IMX_CLK_DECL(pll3_pfd1);
IMX_CLK_DECL(pll3_pfd2);
IMX_CLK_DECL(pll3_pfd3);

///////////////////////////////////////////////////////////////////////////////
IMX_CLK_DECL(arm_podf);
IMX_CLK_DECL_NO_PARENT(pre_periph_clk_sel);
IMX_CLK_DECL_NO_PARENT(periph_clk2_sel);
IMX_CLK_DECL(periph_clk2_podf);
IMX_CLK_DECL_NO_PARENT(periph_clk_sel);
IMX_CLK_DECL(ahb_podf);
IMX_CLK_DECL(ipg_podf);
IMX_CLK_DECL_NO_PARENT(perclk_clk_sel);
IMX_CLK_DECL(perclk_podf);



////////////////////////////////////////////////////////////////////////////////
IMX_CLK_DECL_NO_PARENT(usdhc1_clk_sel);
IMX_CLK_DECL(usdhc1_podf);
IMX_CLK_DECL_NO_PARENT(usdhc2_clk_sel);
IMX_CLK_DECL(usdhc2_podf);
IMX_CLK_DECL_NO_PARENT(semc_alt_sel);
IMX_CLK_DECL_NO_PARENT(semc_sel);
IMX_CLK_DECL(semc_podf);
////////////////////////////////////////////////////////////////////////////////
IMX_CLK_DECL(pll3_div_4);
IMX_CLK_DECL(pll3_div_6);
IMX_CLK_DECL(pll3_div_8);
////////////////////////////////////////////////////////////////////////////////
IMX_CLK_DECL_NO_PARENT(csi_clk_sel);
IMX_CLK_DECL(csi_podf);
IMX_CLK_DECL_NO_PARENT(flexspi_clk_sel);
IMX_CLK_DECL(flexspi_podf);
IMX_CLK_DECL_NO_PARENT(enc_clk_sel);
IMX_CLK_DECL(enc_pred);
IMX_CLK_DECL(enc_podf);
////////////////////////////////////////////////////////////////////////////////
IMX_CLK_DECL_NO_PARENT(sai1_clk_sel);
IMX_CLK_DECL(sai1_pred);
IMX_CLK_DECL(sai1_podf);

IMX_CLK_DECL_NO_PARENT(sai2_clk_sel);
IMX_CLK_DECL(sai2_pred);
IMX_CLK_DECL(sai2_podf);

IMX_CLK_DECL_NO_PARENT(sai3_clk_sel);
IMX_CLK_DECL(sai3_pred);
IMX_CLK_DECL(sai3_podf);
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
IMX_CLK_DECL_NO_PARENT(lpspi_clk_sel);
IMX_CLK_DECL(lpspi_podf);

IMX_CLK_DECL_NO_PARENT(trace_clk_sel);
IMX_CLK_DECL(trace_podf);
////////////////////////////////////////////////////////////////////////////////
IMX_CLK_DECL_NO_PARENT(lpi2c_clk_sel);
IMX_CLK_DECL(lpi2c_podf);

IMX_CLK_DECL_NO_PARENT(can_clk_sel);
IMX_CLK_DECL(can_podf);

IMX_CLK_DECL_NO_PARENT(uart_clk_sel);
IMX_CLK_DECL(uart_podf);

////////////////////////////////////////////////////////////////////////////////
IMX_CLK_DECL_NO_PARENT(lcdif1_pre_clk_sel);
IMX_CLK_DECL(lcdif1_pred);
IMX_CLK_DECL(lcdif1_podf);
IMX_CLK_DECL_NO_PARENT(ldb_di0_clk_sel);
IMX_CLK_DECL(ldb_di0_ipu_div)
IMX_CLK_DECL_NO_PARENT(ldb_di1_clk_sel);
IMX_CLK_DECL(ldb_di1_ipu_div)
IMX_CLK_DECL_NO_PARENT(lcdif1_clk_sel);


IMX_CLK_DECL_NO_PARENT(spdif0_clk_sel);
IMX_CLK_DECL(spdif0_pred);
IMX_CLK_DECL(spdif0_podf);

////////////////////////////////////////////////////////////////////////////////
IMX_CLK_DECL_NO_PARENT(flexio1_clk_sel);
IMX_CLK_DECL(flexio1_pred);
IMX_CLK_DECL(flexio1_podf);

IMX_CLK_DECL_NO_PARENT(flexio2_clk_sel);
IMX_CLK_DECL(flexio2_pred);
IMX_CLK_DECL(flexio2_podf);


////////////////////////////////////////////////////////////////////////////////
IMX_CLK_DECL_NO_PARENT(enet1_tx_ref);
IMX_CLK_DECL(enet_ipg);
IMX_CLK_DECL(cm7_ahb);


struct clk * imx1050_clk_register_gate2(
        struct clk *p_gate2,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id);
////////////////////////////////////////////////////////////////////////////////
struct clk * imx_clk_register(
        struct clk *p_clk,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id,
        aw_const struct clk_ops *p_ops);

struct clk * imx_mux_clk_register(
        struct clk *p_clk,
        aw_clk_id_t clk_id,
        const char *name,
        aw_const struct clk_ops *p_ops);

#define CCM_ANAOLG      ((imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE)
#define CCM             ((imx1050_ccm_regs_t *)IMX1050_CCM_BASE)
#endif
