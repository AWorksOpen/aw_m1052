/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus driver register and initializing interfaces.
 *
 * \internal
 * \par modification history
 * - 1.00 13-03-07  orz, first implementation.
 * \endinternal
 */

#ifndef __AWBL_DRIVER_IF_H
#define __AWBL_DRIVER_IF_H

#include "aw_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup awbl_driver_if
 * @{
 */
int awbl_input_subsystem_register (void);

aw_export void systick_init(unsigned int);

aw_export void awbus_lite_init(void);

aw_export int awbl_plb_register(void);

aw_export void awbl_armcm_nvic_drv_register(void);
aw_export void awbl_armv8m_nvic_drv_register(void);

aw_export void awbl_ep24cxx_drv_register(void);
aw_export void awbl_pcf8563_drv_register(void);

aw_export void awbl_ns16550_plb_drv_register(void);
aw_export void awbl_sc16is7xx_i2c_drv_register(void);
aw_export void awbl_sc16is7xx_spi_drv_register(void);

aw_export void awbl_gpio_key_drv_register (void);
aw_export void awbl_gpio_knob_drv_register(void);
aw_export void awbl_dc_buzzer_drv_register (void);
aw_export void awbl_pwm_buzzer_drv_register (void);
aw_export void awbl_dc_relay_drv_register (void);
aw_export void awbl_led_gpio_drv_register (void);
aw_export void awbl_ntc_therm_drv_register (void);
aw_export void awbl_gpio_i2c_drv_register(void);
aw_export void awbl_sata_disk_drv_register(void);
aw_export void awbl_sata_cdrom_drv_register(void);

aw_export void awbl_lpc11xx_gpio_drv_register(void);
aw_export void awbl_lpc11xx_i2c_drv_register(void);
aw_export void awbl_lpc11xx_ssp_drv_register(void);
aw_export void awbl_lpc11xx_adc_drv_register(void);
aw_export void awbl_lpc11xx_timer32_drv_register(void);
aw_export void awbl_lpc11xx_timer16_drv_register(void);
aw_export void awbl_lpc11xx_pwm_drv_register(void);
aw_export void awbl_lpc11xx_cap_drv_register(void);

aw_export void awbl_ametal_gpio_drv_register(void);

aw_export void awbl_ametal_spi_drv_register(void);
aw_export void awbl_ametal_i2c_drv_register(void);
aw_export void awbl_ametal_crc_drv_register(void);

aw_export void awbl_lpc54xxx_i2c_drv_register(void);
aw_export void awbl_lpc54xxx_rtc_drv_register(void);
aw_export void awbl_lpc54xxx_crc_drv_register(void);
aw_export void awbl_lpc54xxx_ssp_drv_register(void);
aw_export void awbl_lpc54xxx_adc_drv_register(void);
aw_export void awbl_lpc54xxx_dma_drv_register(void);
aw_export void awbl_lpc54xxx_timer_std_drv_register(void);
aw_export void awbl_lpc54xxx_timer_mrt_drv_register(void);
aw_export void awbl_lpc54xxx_timer_utk_drv_register(void);
aw_export void awbl_lpc54xxx_timer_rit_drv_register(void);
aw_export void awbl_lpc54xxx_timer_sct_drv_register(void);
aw_export void awbl_lpc54xxx_pwm_std_drv_register(void);
aw_export void awbl_lpc54xxx_pwm_sct_drv_register(void);
aw_export void awbl_lpc54xxx_uart_drv_register(void);
aw_export void awbl_lpc54xxx_wwdt_drv_register(void);
aw_export void awbl_lpc54xxx_cap_std_drv_register(void);

aw_export void awbl_i2cbus_init(void);
aw_export void awbl_spibus_init(void);
aw_export void awbl_intctlr_init(void);
aw_export void awbl_gpio_init(void);
aw_export void awbl_adc_init(void);
aw_export void awbl_rtc_init(void);
aw_export void awbl_wdt_init(void);
aw_export void awbl_pwm_init(void);
aw_export void awbl_cap_init(void);
aw_export void awbl_sdiobus_init(void);
aw_export void awbl_nandbus_init(void);
aw_export void awbl_miibus_init(void);
aw_export void awbl_usbhbus_init(void);
aw_export void awbl_satabus_init(void);
aw_export void awbl_eim_bus_init(void);

aw_export void  imx28_clock_init (void);

aw_export aw_err_t aw_time_lib_init (unsigned int clkrate, int bk_rtc_id);
aw_export aw_err_t awbl_timestamp_lib_init(const char *pname,
                                           uint8_t     uint,
                                           uint8_t     timer_no);

aw_export void aw_isr_defer_lib_init(void);
aw_export void aw_koutput_sio_init (int com, int baud);


aw_export void awbl_lpc43xx_gpio_drv_register(void);

aw_export void awbl_c674x_intc_drv_register(void);
aw_export void awbl_c674x_gpio_drv_register(void);
aw_export void awbl_c674x_i2c_drv_register(void);
aw_export void awbl_c674x_emifa_drv_register(void);
aw_export void awbl_c674x_usbh_drv_register (void);
aw_export void awbl_c674x_usbh0_drv_register (void);
aw_export void awbl_c674x_sata_drv_register (void);

aw_export void awbl_lpc12xx_gpio_drv_register (void);

aw_export void awbl_lpc17xx_gpio_drv_register(void);

aw_export void awbl_ti_edma3_drv_register(void);

aw_export void awbl_tic6000_intc_drv_register(void);
aw_export void awbl_tic6000_cic_drv_register(void);

aw_export void awbl_c6657_gpio_drv_register(void);
aw_export void awbl_keystone_spi_drv_register(void);

aw_export void awbl_imx28_intc_drv_register(void);
aw_export void awbl_imx28_timer_drv_register (void);
aw_export void awbl_imx28_i2c_drv_register (void);
aw_export void awbl_imx28x_gpio_drv_register (void);
aw_export void awbl_imx28_ocotp_register (void);
aw_export void awbl_imx28_ssp_drv_register (void);
aw_export void awbl_imx28_auart_drv_register (void);
aw_export void awbl_imx28_lradc_drv_register (void);
aw_export void awbl_imx28_hsadc_drv_register (void);
aw_export void awbl_imx28x_duart_drv_register (void);

aw_export void awbl_imx283_rtc_drv_register (void);
aw_export void awbl_mxs_hw480272f_drv_register (void);
aw_export void awbl_mxs_emwin_fb_drv_register (void);
aw_export void awbl_imx28_dma_drv_register (void);
aw_export void awbl_imx28_emac_drv_register (void);

aw_export void awbl_gpio_wdt_drv_register(void);
aw_export void awbl_imx283_wdt_drv_register(void);
aw_export void awbl_imx28_switch_drv_register(void);

aw_export void awbl_panel_pm1000_drv_register(void);
aw_export void awbl_tiny_hmi_drv_register (void);
aw_export void awbl_apl_d01_drv_register (void);

aw_export void awbl_spi_flash_drv_register(void);
aw_export void awbl_nor_flash_drv_register (void);
aw_export void awbl_sdio_spi_drv_register (void);
aw_export void awbl_sdcard_drv_register (void);
aw_export void awbl_gpio_spi_drv_register (void);
aw_export void awbl_imx28_sdio_drv_register (void);
aw_export void awbl_imx28_gpmi_drv_register (void);
aw_export void awbl_nandflash_drv_register (void);

aw_export void awbl_imx28_usbh_drv_register (void);
aw_export void awbl_usbh_device_drv_register (void);
aw_export void awbl_usbh_hub_drv_register (void);
aw_export void awbl_usbh_ms_drv_register (void);
aw_export void awbl_usbh_cdc_drv_register (void);
aw_export void awbl_pl2303_drv_register (void);
aw_export void awbl_usbh_wireless_drv_register (void);

aw_export void awbl_imx28_usbd_drv_register (void);

aw_export void awbl_generic_phy_drv_register(void);
aw_export void awbl_dp83848_phy_drv_register(void);
aw_export void awbl_dp83848ep_phy_drv_register(void);
aw_export void awbl_led_init(void);
aw_export void awbl_ametal_uart_drv_register(void);
aw_export void awbl_ametal_timer_drv_register(void);
aw_export void awbl_ametal_pwm_drv_register(void);
aw_export void awbl_lpc5410x_dma_drv_register(void);
aw_export void awbl_ametal_rtc_drv_register(void);
aw_export void awbl_ametal_wwdt_drv_register(void);
aw_export void awbl_ametal_adc_drv_register(void);
aw_export void awbl_ametal_cap_drv_register(void);

aw_export void awbl_imx283_saif_drv_register(void);
aw_export void awbl_imx28_can_drv_register (void);

aw_export void awbl_jyrtc1_drv_register(void);
aw_export void awbl_pcf8563_drv_register(void);
aw_export void awbl_pcf85063_drv_register(void);
aw_export void awbl_pcf85263_drv_register(void);
aw_export void awbl_rx8025sa_drv_register(void);
aw_export void awbl_rx8025t_drv_register(void);
aw_export void awbl_ds1302_drv_register(void);

aw_export void awbl_c674x_sdio_drv_register (void);
aw_export void awbl_ne2000_drv_register(void);
aw_export void awbl_ti_upp_drv_register(void);
aw_export void awbl_c674x_emac_drv_register (void);

aw_export void awbl_matrix_keypad_drv_register(void);
aw_export void awbl_gpio_i2c_drv_register (void);

aw_export void awbl_zlg600a_init(void);
aw_export void awbl_zlg600a_i2c_drv_register(void);
aw_export void awbl_zlg600a_uart_drv_register(void);

aw_export void awbl_gnss_init (void);
aw_export void awbl_cnt226_drv_register(void);

aw_export void awbl_miniport_digitron_drv_register(void);
aw_export void awbl_miniport_miniport_hc595_led_drv_register(void);
aw_export void awbl_miniport_hc595_digitron_drv_register(void);
aw_export void awbl_miniport_key_hc595_digitron_drv_register (void);
aw_export void awbl_miniport_key_digitron_drv_register (void);
aw_export void awbl_fm175xx_drv_register(void);
aw_export void awbl_ublox_drv_register (void);

aw_export void awbl_zlg72128_if_drv_register(void);
aw_export void awbl_tps08u_drv_register(void);
aw_export void awbl_tps02r_drv_register (void);

aw_export void awbl_gprs_sim800_drv_register (void);
aw_export void awbl_gprs_me909s_drv_register (void);
aw_export void awbl_gprs_u9300c_drv_register (void);
aw_export void awbl_ublox_drv_register (void);

aw_export void awbl_sja1000_can_drv_register (void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_DRIVER_IF_H */

/* end of file */
