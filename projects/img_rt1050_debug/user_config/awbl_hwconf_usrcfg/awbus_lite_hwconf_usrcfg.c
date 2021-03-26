/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite 硬件配置文件
 */

/*******************************************************************************
  headers
*******************************************************************************/

#undef  AW_IMG_PRJ_BUILD
#define AW_IMG_PRJ_BUILD


#include "aworks.h"
#include "awbus_lite.h"
#include "aw_gpio.h"
#include "aw_delay.h"
#include "aw_prj_params.h"
#include "aw_clk.h"
#include "aw_bitops.h"
#include "imx1050_inum.h"
#include "imx1050_pin.h"
#include "imx1050_reg_base.h"

#include "awbl_hwconf_imx1050_nvic.h"
#include "awbl_hwconf_imx1050_lpuart1.h"
#include "awbl_hwconf_imx1050_lpuart2.h"
#include "awbl_hwconf_imx1050_lpuart3.h"
#include "awbl_hwconf_imx1050_lpuart4.h"
#include "awbl_hwconf_imx1050_lpuart5.h"
#include "awbl_hwconf_imx1050_lpuart6.h"
#include "awbl_hwconf_imx1050_lpuart7.h"
#include "awbl_hwconf_imx1050_lpuart8.h"
#include "awbl_hwconf_imx1050_gpio.h"
#include "awbl_hwconf_gpio_led.h"
#include "awbl_hwconf_gpio_key.h"
#include "awbl_hwconf_imx1050_flexspi.h"
#include "awbl_hwconf_imx1050_lpspi1.h"
#include "awbl_hwconf_imx1050_lpspi2.h"
#include "awbl_hwconf_imx1050_lpspi3.h"
#include "awbl_hwconf_imx1050_lpspi4.h"
#include "awbl_hwconf_imx1050_rtwdt.h"
#include "awbl_hwconf_imx1050_wdt1.h"
#include "awbl_hwconf_imx1050_gpt1_timer.h"
#include "awbl_hwconf_imx1050_gpt1_cap.h"
#include "awbl_hwconf_imx1050_gpt1_pwm.h"
#include "awbl_hwconf_imx1050_gpt2_timer.h"
#include "awbl_hwconf_imx1050_gpt2_cap.h"
#include "awbl_hwconf_imx1050_gpt2_pwm.h"
#include "awbl_hwconf_imx1050_pit.h"
#include "awbl_hwconf_imx1050_adc1.h"
#include "awbl_hwconf_imx1050_sdio_sd0.h"
#include "awbl_hwconf_imx1050_sdio_sd1.h"
#include "awbl_hwconf_imx1050_edma.h"
#include "awbl_hwconf_imx1050_sai1.h"
#include "awbl_hwconf_imx1050_sai3.h"
#include "awbl_hwconf_imx1050_mqs.h"
#include "awbl_hwconf_imx1050_lpi2c1.h"
#include "awbl_hwconf_imx1050_lpi2c2.h"
#include "awbl_hwconf_imx1050_ocotp.h"
#include "awbl_hwconf_imx1050_flexcan1.h"
#include "awbl_hwconf_imx1050_flexcan2.h"
#include "awbl_hwconf_imx1050_enet.h"
#include "awbl_hwconf_dp83848_phy.h"
#include "awbl_hwconf_imx1050_adc2.h"
#include "awbl_hwconf_tft_4_3.h"
#include "awbl_hwconf_ap_tft7_0.h"
#include "awbl_hwconf_lcd_tm070rdh12_24b.h"
#include "awbl_hwconf_lcd_640480w056tr.h"
#include "awbl_hwconf_lcd_800600w080tr.h"
#include "awbl_hwconf_fb.h"
#include "awbl_hwconf_imx1050_ts.h"
#include "awbl_hwconf_bu21029muv.h"
#include "awbl_hwconf_ft5x06.h"
#include "awbl_hwconf_imx1050_eflex_pwm4.h"
#include "awbl_hwconf_imx1050_eflex_pwm1.h"
#include "awbl_hwconf_imx1050_qtimer1_pwm.h"
#include "awbl_hwconf_imx1050_qtimer1_cap.h"
#include "awbl_hwconf_imx1050_qtimer1_timer0.h"
#include "awbl_hwconf_imx1050_qtimer2_pwm.h"
#include "awbl_hwconf_imx1050_qtimer2_cap.h"
#include "awbl_hwconf_imx1050_qtimer2_timer0.h"
#include "awbl_hwconf_imx1050_qtimer3_pwm.h"
#include "awbl_hwconf_imx1050_qtimer3_cap.h"
#include "awbl_hwconf_imx1050_qtimer3_timer0.h"
#include "awbl_hwconf_imx1050_qtimer4_pwm.h"
#include "awbl_hwconf_imx1050_qtimer4_cap.h"
#include "awbl_hwconf_imx1050_qtimer4_timer0.h"
#include "awbl_hwconf_buzzer_pwm.h"
#include "awbl_hwconf_gpio_i2c0.h"
#include "awbl_hwconf_pcf85063.h"
#include "awbl_hwconf_imx1050_semc.h"
#include "awbl_hwconf_imx1050_nandflash.h"
#include "awbl_hwconf_imx1050_usbh1.h"
#include "awbl_hwconf_fm175xx_spi.h"
#include "awbl_hwconf_imx1050_usbd.h"
#include "awbl_hwconf_imx1050_usbh2.h"
#include "awbl_hwconf_gpio_wdt.h"
#include "awbl_hwconf_imx1050_rtc.h"
#include "awbl_hwconf_imx1050_timestamp.h"

#include "awbl_hwconf_imx1050_enc1.h"
#include "awbl_hwconf_imx1050_tempmon.h"
#include "awbl_hwconf_imx1050_acmp1.h"
#include "awbl_hwconf_cyw43362_wifi_spi.h"

#include "awbl_hwconf_sx127x.h"

#include "awbl_hwconf_imx1050_csi.h"
#include "awbl_hwconf_ov7725.h"
#include "awbl_hwconf_gc0308.h"

#include "awbl_hwconf_gprs_me909s.h"
#include "awbl_hwconf_gprs_sim800.h"
#include "awbl_hwconf_gprs_u9300c.h"
#include "awbl_hwconf_gprs_ec20.h"
#include "awbl_hwconf_pcf85263.h"
#include "awbl_hwconf_ublox.h"

#include "awbl_hwconf_csm300x.h"

#include "awbl_hwconf_sensor_hts221.h"
#include "awbl_hwconf_sensor_shtc1.h"
#include "awbl_hwconf_sensor_lis3mdl.h"
#include "awbl_hwconf_sensor_lps22hb.h"
#include "awbl_hwconf_sensor_lsm6dsl.h"
#include "awbl_hwconf_sensor_bme280.h"
#include "awbl_hwconf_sensor_bmp280.h"
#include "awbl_hwconf_sensor_bma253.h"
#include "awbl_hwconf_sensor_mmc5883ma.h"
#include "awbl_hwconf_sensor_ltr553.h"
#include "awbl_hwconf_sensor_bh1730.h"
#include "awbl_hwconf_sensor_bmg160.h"

#include "awbl_hwconf_imx1050_zsn603_i2c.h"
#include "awbl_hwconf_imx1050_zsn603_uart.h"
#include "awbl_hwconf_cyw43362_wifi_sdio.h"

/*******************************************************************************
  globals
*******************************************************************************/

/* 硬件设备列表 */
aw_const struct awbl_devhcf g_awbl_devhcf_list[] = {
    /* nvic */
    AWBL_HWCONF_IMX1050_NVIC

    /* GPIO */
    AWBL_HWCONF_IMX1050_GPIO

    /* LED */
    AWBL_HWCONF_GPIO_LED

    /* KEY */
    AWBL_HWCONF_GPIO_KEY

    /* FlexSPI */
    AWBL_HWCONF_IMX1050_FLEXSPI

    /* LPSPI1 */
    AWBL_HWCONF_IMX1050_LPSPI1

    /* LPSPI2 */
    AWBL_HWCONF_IMX1050_LPSPI2

    /* LPSPI3 */
    AWBL_HWCONF_IMX1050_LPSPI3

    /* LPSPI4 */
    AWBL_HWCONF_IMX1050_LPSPI4

    /* RTWDOG */
    AWBL_HWCONF_IMX1050_RTWDT

    /* WDOG1 */
    AWBL_HWCONF_IMX1050_WDT1

    /* GPT1 TIMER */
    AWBL_HWCONF_IMX1050_GPT1

    /* GPT1 CAP */
    AWBL_HWCONF_IMX1050_GPT1_CAP

    /* GPT1 PWM */
    AWBL_HWCONF_IMX1050_GPT1_PWM

    /* GPT2 TIMER */
    AWBL_HWCONF_IMX1050_GPT2

    /* GPT2 CAP */
    AWBL_HWCONF_IMX1050_GPT2_CAP

    /* GPT2 PWM */
    AWBL_HWCONF_IMX1050_GPT2_PWM

    /* PIT */
    AWBL_HWCONF_IMX1050_PIT

    /* ADC */
    AWBL_HWCONF_IMX1050_ADC1

    /* SDIO */
    AWBL_HWCONF_IMX1050_SDIO_SSP0

    AWBL_HWCONF_IMX1050_SDIO_SSP1

    /* eDMA */
    AWBL_HWCONF_IMX1050_EDMA

    /* lpuart1 */
    AWBL_HWCONF_IMX1050_LPUART1

    /* lpuart2 */
    AWBL_HWCONF_IMX1050_LPUART2

    /* lpuart3 */
    AWBL_HWCONF_IMX1050_LPUART3

    /* lpuart4 */
    AWBL_HWCONF_IMX1050_LPUART4

    /* lpuart5 */
    AWBL_HWCONF_IMX1050_LPUART5

    /* lpuart6 */
    AWBL_HWCONF_IMX1050_LPUART6

    /* lpuart7 */
    AWBL_HWCONF_IMX1050_LPUART7

    /* lpuart8 */
    AWBL_HWCONF_IMX1050_LPUART8

    /* SAI1 */
    AWBL_HWCONF_IMX1050_SAI1

    /* SAI3 */
    AWBL_HWCONF_IMX1050_SAI3

    /* MQS */
    AWBL_HWCONF_IMX1050_MQS

    /* LPI2C1 */
    AWBL_HWCONF_IMX1050_LPI2C1

    /* LPI2C2 */
    AWBL_HWCONF_IMX1050_LPI2C2

    /* OCOTP */
    AWBL_HWCONF_IMX1050_OCOTP
    /* CAN1 */
    AWBL_HWCONF_IMX1050_CAN1

    /* CAN2 */
    AWBL_HWCONF_IMX1050_CAN2

    /* ENET */
    AWBL_HWCONF_IMX1050_ENET

    /* PHY */
    AWBL_HWCONF_DP83848_PHY

    /* ADC2 */
    AWBL_HWCONF_IMX1050_ADC2

    /* TFT-4.3(A、CAP)显示屏 */
    AWBL_HWCONF_TFT_4_3

    /* AP_TFT7_0显示屏 */
    AWBL_HWCONF_AP_TFT7_0

    /*LCD-TM070RDH12-24B显示屏 */
    AWBL_HWCONF_LCD_TM070RDH12_24B

    /* LCD-640480W056TR显示屏 */
    AWBL_HWCONF_LCD_640480W056TR

    /* LCD-800600W080TR显示屏 */
    AWBL_HWCONF_LCD_800600W080TR

    /* frame buffer */
    AWBL_HWCONF_FB

    /* 触摸屏 */
    AWBL_HWCONF_IMX1050_TS

    /* i2c电阻触摸屏 */
    AWBL_HWCONF_BU21029MUV

    /* i2c电容触摸屏 */
    AWBL_HWCONF_FT5X06

    /* eFlexPWM4　*/
    AWBL_HWCONF_IMX1050_EFLEX_PWM4

    /* eFlexPWM1　*/
    AWBL_HWCONF_IMX1050_EFLEX_PWM1

    /* QTimer1 PWM */
    AWBL_HWCONF_IMX1050_QTIMER1_PWM

    /* QTimer2 PWM */
    AWBL_HWCONF_IMX1050_QTIMER2_PWM

    /* QTimer3 PWM */
    AWBL_HWCONF_IMX1050_QTIMER3_PWM

    /* QTimer4 PWM */
    AWBL_HWCONF_IMX1050_QTIMER4_PWM

    /* PWM蜂鸣器 */
    AWBL_HWCONF_BUZZER_PWM

    /* GPIO I2C0 */
    AWBL_HWCONF_GPIO_I2C0

    /* PCF85063 */
    AWBL_HWCONF_PCF85063

    /* SEMC */
    AWBL_HWCONF_IMX1050_SEMC

    /* NAND FLASH*/
    AWBL_HWCONF_IMX1050_NANDFLASH

    /* USB HOST 1 */
    AWBL_HWCONF_IMX1050_USBH1

    /* USB HOST 2 */
    AWBL_HWCONF_IMX1050_USBH2

    /* Mifare卡 */
    AWBL_HWCONF_FM175XX_0

    /* USB device */
    AWBL_HWCONF_IMX1050_USBD

    /* GPIO看门狗 */
    AWBL_HWCONF_GPIO_WDT

    /* ENC */
    AWBL_HWCONF_IMX1050_ENC1

    /* Temperature Monitor */
    AWBL_HWCONF_IMX1050_TEMPMON

    /* Analog Comparator 1 */
    AWBL_HWCONF_IMX1050_ACMP1

    /* SPI WIFI */
    AWBL_HWCONF_CYW43362_WIFI_SPI

    AWBL_HWCONF_CYW43362_WIFI_SDIO

    /* LoRa sx127x */
    AWBL_HWCONF_SX127X

    /* CSI */
    AWBL_HWCONF_IMX1050_CSI

    /* OV7725 */
    AWBL_HWCONF_OV7725

    /* GC0308 */
    AWBL_HWCONF_GC0308
    
    /* timestamp */
    AWBL_HWCONF_IMX1050_TIMESTAMP

    AWBL_HWCONF_GPRS_ME909S
    AWBL_HWCONF_GPRS_SIM800
    AWBL_HWCONF_GPRS_U9300C
	AWBL_HWCONF_GPRS_EC20

    /* PCF85263 RTC */
    AWBL_HWCONF_PCF85263

    /* SNVS HP RTC */
    AWBL_HWCONF_IMX1050_RTC

    /* QTimer1 CAP */
    AWBL_HWCONF_IMX1050_QTIMER1_CAP

    /* QTimer2 CAP */
    AWBL_HWCONF_IMX1050_QTIMER2_CAP

    /* QTimer3 CAP */
    AWBL_HWCONF_IMX1050_QTIMER3_CAP

    /* QTimer4 CAP */
    AWBL_HWCONF_IMX1050_QTIMER4_CAP

    /* gps设备 */
    AWBL_HWCONF_UBLOX

    /* UART转 CAN总线设备 */
    AWBL_HWCONF_CSM300X

    /* QTimer1 TIMER0 */
    AWBL_HWCONF_IMX1050_QTIMER1_TIMER0

    /* QTimer2 TIMER0 */
    AWBL_HWCONF_IMX1050_QTIMER2_TIMER0

    /* QTimer3 TIMER0 */
    AWBL_HWCONF_IMX1050_QTIMER3_TIMER0

    /* QTimer4 TIMER0 */
    AWBL_HWCONF_IMX1050_QTIMER4_TIMER0

    /* Sensor HTS221 */
    AWBL_HWCONF_HTS221

    /* Sensor SHTC1 */
    AWBL_HWCONF_SHTC1

    /* Sensor LIS3MDL */
    AWBL_HWCONF_LIS3MDL

    /* Sensor LPS22HB */
    AWBL_HWCONF_LPS22HB

    /* Sensor LSM6DSL */
    AWBL_HWCONF_LSM6DSL

    /* Sensor BME280 */
    AWBL_HWCONF_BME280

    /* Sensor BMA253 */
    AWBL_HWCONF_BMA253

    /* Sensor BMP280 */
    AWBL_HWCONF_BMP280

    /* Sensor MMC5883MA */
    AWBL_HWCONF_MMC5883MA

    /* Sensor LTR553 */
    AWBL_HWCONF_LTR553

    /* Sensor BH1730 */
    AWBL_HWCONF_BH1730

    /* Sensor BMG160 */
    AWBL_HWCONF_BMG160

    /* ZSN603 UART */
    AWBL_HWCONF_ZSN603_UART

    /* ZSN603 I2C */
    AWBL_HWCONF_ZSN603_I2C
};

/* 硬件设备列表成员个数 */
aw_const size_t g_awbl_devhcf_list_count = AW_NELEMENTS(g_awbl_devhcf_list);

/* end of file */
