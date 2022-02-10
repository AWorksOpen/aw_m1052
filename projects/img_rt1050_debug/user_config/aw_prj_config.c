
/*
 *  此文件由配置工具生产，请勿修改。
 *  现在作为一个例子。
 */
#undef  AW_IMG_PRJ_BUILD
#define AW_IMG_PRJ_BUILD

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_task.h"
#include "awbl_plb.h"
#include "aw_serial.h"
#include "aw_prj_params.h"
#include "aw_vdebug.h"
#include "aw_gpio.h"
#include "awbl_gpio.h"
#include "awbl_led.h"
#include "awbl_wdt.h"
#include "awbl_pwm.h"
#include "awbl_driver_if.h"
#include "aw_bsp_driver_if.h"
#include "mtd/aw_mtd.h"
#include "aw_sdk_version.h"

/* including c source files */
#include "all/aworks_startup.c"
#include "all/aw_psp_startup.c"
#include "all/aw_psp_usr_app_init.c"
#include "core/awbus_lite_cfg.c"

#include "os/aw_isr_defer_cfg.c"
#include "os/aw_nor_defer_cfg.c"
#include "awsa_cfg.c"

#include "aw_psp_shell_tools.h"

#ifdef AW_DEV_GNSS_UBLOX
#include "driver/gps/awbl_ublox.h"
#endif

#ifdef AW_COM_INPUT_EV
#include "core/awbus_input_cfg.c"
#endif

#ifdef AW_COM_LOG
#include "util/aw_loglib_cfg.c"
#endif

#ifdef AW_COM_FTL
#include "ftl/aw_ftl_cfg.c"
#endif

#ifdef AW_COM_NETWORK
#include "aw_net.h"
#endif  /* AW_COM_NETWORK */

#ifdef AW_COM_SHELL_SERIAL
#include "aw_shell_serial_cfg.c"
#endif  /* AW_COM_SHELL_SERIAL */

#if defined(AW_COM_SHELL_LUA) || defined(AW_COM_SHELL_LUAC)
#include "aw_lua_shell_init.h"
#endif

#if defined AW_COM_SHELL_MPY
#include "config/aw_python_shell_init.c"
#endif
#ifdef AW_COM_SHELL_TELNET
#include "config/aw_telnet_cfg.c"
#endif

#ifdef AW_COM_FTPD
#include "config/aw_ftpd_cfg.c"
#endif

#ifdef AW_COM_BLOCK_DEV
#include "fs/aw_blk_dev.h"
#endif  /* AW_DRV_BLOCK_DEV */

#ifdef AW_COM_I2C
#include "awbl_i2cbus.h"
#endif  /* AW_COM_I2C */

#ifdef AW_COM_FS_ROOTFS
#include "fs/aw_root_fs.h"
#endif  /* AW_COM_FS_ROOTFS */

#ifdef AW_COM_FS_FATFS
#include "fs/aw_fatfs_cfg.c"
#endif  /* AW_COM_FS_FATFS */

#ifdef AW_COM_FS_LFFS
#include "fs/aw_lffs_cfg.c"
#endif  /* AW_COM_FS_LFFS */

#ifdef AW_COM_FS_LITTLEFS
#include "fs/aw_littlefs_cfg.c"
#endif  /* AW_COM_FS_LFFS */

#ifdef AW_COM_FS_YAFFS
#include "fs/aw_yaffs_cfg.c"
#endif  /* AW_COM_FS_LFFS */

#ifdef AW_COM_EVENT
#include "util/aw_event_cfg.c"
#endif  /* AW_COM_EVENT */

#ifdef AW_COM_IO_SYSTEM
#include "io/aw_io_cfg.c"
#endif  /* AW_COM_IO_SYSTEM */

#ifdef AW_COM_MOUNT
#include "fs/aw_mount_cfg.c"
#endif  /* AW_COM_MOUNT */

//#if  defined(__CC_ARM) || defined(AW_DRV_IMX1050_TS) || defined(AW_DRV_BU21029MUV_TS) || defined(AW_DRV_FT5X06_TS)
#include "ts/aw_ts_calc_cfg.c"
//#endif

/* USB相关配置及特殊USB设备配置信息表*/
#if defined(AW_COM_USBH) || defined(AW_COM_USBD)
#include "awbl_usb_cfg.c"
#endif




#if defined(AW_COM_NETWORK) && defined(AW_NET_TOOLS)
    #include "aw_net_tools_cfg.h"
#endif  /* AW_NET_TOOLS */

#ifdef AW_COM_CANFESTIVAL
#include "config/aw_canfestival_cfg.c"
#endif

#ifdef AW_COM_CANOPEN
#if defined(AW_COM_CANOPEN_MST)
#include "mst/config/aw_cop_cfg.c"
#elif defined(AW_COM_CANOPEN_SLV)
#include "slv/config/aw_cop_cfg.c"
#else
#error "use AW_COM_CANOPEN must define AW_COM_CANOPEN_MST or AW_COM_CANOPEN_SLV at project"
#endif
#endif

#ifdef AW_DEV_I2C_ZSN603
#include "driver/rfid/awbl_zsn603_i2c.h"
#endif

#ifdef AW_DEV_UART_ZSN603
#include "driver/rfid/awbl_zsn603_uart.h"
#endif

#ifdef AW_COM_CONSOLE
#if AW_CFG_CONSOLE_SHOW_BANNER  != 0
static const char __g_aw_banner[] = {
    "\nAWorks for i.MX RT1050, build " __DATE__ "\n"
};
#endif  /* AW_CFG_CONSOLE_SHOW_BANNER */
#endif  /* AW_COM_CONSOLE */

#ifdef AW_COM_CONSOLE
#if AW_CFG_CONSOLE_SHOW_BANNER  != 0
void aw_console_show_banner (void)
{
    AW_LOGF(("%s\n", __g_aw_banner));
    AW_LOGF(("AWorks SDK Version is %s  \r\n", aw_sdk_version_get()));
}
#endif  /* AW_CFG_CONSOLE_SHOW_BANNER */
#endif  /* AW_COM_CONSOLE */

#if defined(AW_DEV_BU21029MUV) || \
    defined(AW_DEV_IMX1050_TS) || \
    defined(AW_DEV_FT5X06)
char *aw_get_ts_name (void)
{
    return (char *)SYS_TS_ID;
}
#else
char *aw_get_ts_name (void)
{
    return (char *)"none";
}
#endif

static void awbl_group_init (void)
{
    awbus_lite_init();
    awbl_plb_register();

    awbl_armcm_nvic_drv_register();
    awbl_imx1050_gpio_drv_register();

#ifdef AW_DRV_IMX1050_GPT_TIMER
    awbl_imx10xx_gpt_drv_register();
#endif

#ifdef AW_DRV_IMX1050_GPT_CAP
    awbl_imx10xx_gpt_cap_drv_register();
#endif

#ifdef AW_DRV_IMX1050_GPT_PWM
    awbl_imx10xx_gpt_pwm_drv_register();
#endif

#ifdef AW_DRV_IMX1050_PIT
    awbl_imx10xx_pit_drv_register();
#endif

#ifdef AW_DRV_IMX1050_LPUART
    awbl_imx10xx_lpuart_drv_register();
#endif

#ifdef AW_DRV_GPIO_LED
    awbl_led_gpio_drv_register();
#endif

#ifdef AW_DRV_GPIO_KEY
    awbl_gpio_key_drv_register();
#endif

#ifdef AW_DRV_IMX1050_ENET
    awbl_imx10xx_enet_drv_register();
    awbl_miibus_init();
    awbl_dp83848_phy_drv_register();
#endif

#ifdef AW_DRV_IMX1050_FLEXSPI
    awbl_imx1050_flexspi_drv_register();
#endif

#ifdef AW_DRV_IMX1050_LPSPI
    awbl_imx10xx_lpspi_drv_register();
#endif

#ifdef AW_DRV_IMX1050_WDT
    awbl_imx10xx_wdt_drv_register();
#endif /* AW_DRV_IMX1050_WDT */

#ifdef AW_DRV_IMX1050_RTWDT
    awbl_imx1050_rtwdt_drv_register();
#endif /* AW_DRV_IMX1050L_RTWDT */

#ifdef AW_DRV_IMX1050_ADC
    awbl_imx10xx_adc_drv_register();
#endif

#ifdef AW_DRV_IMX1050_USDHC
    awbl_sdiobus_init();
    awbl_sdcard_drv_register();
    awbl_imx10xx_sdio_drv_register();
#endif

#ifdef AW_DRV_IMX1050_EDMA
    awbl_imx10xx_edma_drv_register();
#endif

#ifdef AW_DRV_IMX1050_SAI
    awbl_imx1050_sai_drv_register();
#endif /* AW_DRV_IMX1050L_SAI */

#ifdef AW_DRV_IMX1050_MQS
    awbl_imx1050_mqs_drv_register();
#endif /* AW_DRV_IMX1050L_MQS */

#ifdef AW_DRV_IMX1050_LPI2C
    awbl_imx10xx_lpi2c_drv_register();
#endif

#ifdef AW_DRV_IMX1050_OCOTP
    awbl_imx1050_ocotp_register();
#endif


#ifdef AW_DRV_IMX1050_CAN
    awbl_imx10xx_flexcan_drv_register();
#endif


#ifdef AW_DRV_TFT_PANEL
    awbl_imx1050_tft_panel_drv_register();
#endif

#ifdef AW_DRV_FB
    awbl_imx1050_fb_drv_register();
#endif

#ifdef AW_DRV_FT5X06_TS
    awbl_ft5x06_drv_register();
#endif

#ifdef AW_DRV_BU21029MUV_TS
    awbl_bu21029muv_drv_register();
#endif

#ifdef AW_DRV_IMX1050_TS
    awbl_imx1050_touch_screen_drv_register();
#endif

#ifdef AW_DRV_IMX1050_EFLEX_PWM
    awbl_imx10xx_eflex_pwm_drv_register();
#endif

#ifdef AW_DRV_IMX1050_QTIMER_PWM
    awbl_imx10xx_qtimer_pwm_drv_register();
#endif

#ifdef AW_DRV_PWM_BUZZER
    awbl_pwm_buzzer_drv_register();
#endif

#ifdef AW_DRV_GPIO_I2C_0
    awbl_gpio_i2c_drv_register();
#endif

#ifdef AW_DRV_EXTEND_PCF85063
    awbl_pcf85063_drv_register();
#endif

#ifdef AW_DRV_EXTEND_PCF85263
    awbl_pcf85263_drv_register();
#endif

#ifdef AW_COM_NANDBUS
    void awbl_nand_bus_init(void);
    awbl_nand_bus_init();
#endif /* AW_COM_NANDBUS */

#ifdef AW_DRV_IMX1050_SEMC
    awbl_imx1050_semc_drv_register();
#endif /* AW_DRV_IMX1050_SEMC */

#ifdef AW_DRV_NANDFLASH
    void awbl_nand_drv_register(void);
    awbl_nand_drv_register();
    void awbl_imx1050_nand_platform_register (void);
    awbl_imx1050_nand_platform_register();
#endif /* AW_DRV_NANDFLASH */

#ifdef AW_DRV_IMX1050_USBH
    awbl_imx10xx_usbh_drv_register();
#endif

#ifdef AW_COM_USBH
    extern void awbl_usbh_bus_init (void);
    awbl_usbh_bus_init();
    awbl_usbh_hub_drv_register();
#endif

#ifdef AW_DRV_USBH_MASS_STORAGE
    extern void awbl_usbh_mass_storage_drv_register (int buffer_size);
    awbl_usbh_mass_storage_drv_register(AW_CFG_USB_MS_BUF_SIZE);
#endif
#ifdef AW_DRV_USBH_UVC
    extern void awbl_usbh_uvc_drv_register (void);
    awbl_usbh_uvc_drv_register();
#endif
#ifdef AW_DRV_USBH_CDC_SERIAL
    extern void awbl_usbh_cdc_serial_drv_register (void);
    awbl_usbh_cdc_serial_drv_register();
#endif


#ifdef AW_DRV_USBH_WIRELESS
    awbl_usbh_wireless_drv_register();
#endif

#ifdef AW_DRV_GPRS_SIM800
    extern void awbl_gprs_sim800_drv_register (void);
    awbl_gprs_sim800_drv_register();
#endif

#ifdef AW_DRV_GPRS_ME909S
    extern void awbl_gprs_me909s_drv_register (void);
    awbl_gprs_me909s_drv_register();
#endif

#ifdef AW_DRV_GPRS_U9300C
    extern void awbl_gprs_u9300c_drv_register (void);
    awbl_gprs_u9300c_drv_register();
#endif

#ifdef AW_DRV_GPRS_EC20
    extern void awbl_gprs_ec20_drv_register (void);
    awbl_gprs_ec20_drv_register();
#endif

#ifdef AW_DRV_FM175XX
    awbl_fm175xx_drv_register();
#endif

#ifdef AW_DRV_UBLOX
    awbl_ublox_drv_register();
#endif

#ifdef AW_DRV_IMX1050_USBD
    awbl_imx10xx_usbd_drv_register();
#endif

#ifdef AW_DRV_GPIO_WDT
    awbl_gpio_wdt_drv_register();
#endif

#ifdef AW_DRV_IMX1050_ENC1
    awbl_imx10xx_enc_drv_register();
#endif

#ifdef AW_DRV_IMX1050_TEMPMON
    awbl_imx10xx_tempmon_drv_register();
#endif

#ifdef AW_DRV_IMX1050_ACMP1
    awbl_imx10xx_acmp_drv_register();
#endif

#ifdef AW_DRV_CYW43362_WIFI_SDIO
    extern void awbl_cyw43362_sdio_drv_register (void);
    awbl_cyw43362_sdio_drv_register();
#endif

#ifdef AW_DRV_CYW43362_WIFI_SPI
    extern void awbl_cyw43362_spi_drv_register (void);
    awbl_cyw43362_spi_drv_register();
#endif

#ifdef AW_DRV_AWBL_SX127X
    extern void awbl_sx127x_drv_register (void);
    awbl_sx127x_drv_register();
#endif

#ifdef AW_DRV_OV7725
    awbl_ov7725_drv_register();
#endif

#ifdef AW_DRV_GC0308
    awbl_gc0308_drv_register();
#endif

#ifdef AW_DRV_IMX1050_CSI
    awbl_imx1050_csi_drv_register ();
#endif

#ifdef AW_DRV_IMX1050_RTC
    awbl_imx10xx_rtc_drv_register();
#endif

#ifdef AW_DRV_IMX1050_QTIMER_CAP
    awbl_imx10xx_qtimer_cap_drv_register();
#endif

#ifdef AW_DEV_CSM300X
    extern void awbl_csm300x_drv_register (void);
    awbl_csm300x_drv_register ();
#endif

#ifdef AW_DRV_IMX1050_QTIMER_TIMER
    extern void awbl_imx10xx_qtimer_timer_drv_register();
    awbl_imx10xx_qtimer_timer_drv_register();
#endif

#ifdef AW_DEV_I2C_ZSN603
    awbl_zsn603_i2c_drv_register();
#endif

#ifdef AW_DEV_UART_ZSN603
    awbl_zsn603_uart_drv_register();
#endif
/**
 * @\brief AW_Sensor
 */
#ifdef AW_DRV_SENSOR_HTS221
    extern void awbl_hts221_drv_register(void);
    awbl_hts221_drv_register();
#endif

#ifdef AW_DRV_SENSOR_SHTC1
    extern void awbl_shtc1_drv_register(void);
    awbl_shtc1_drv_register();
#endif

#ifdef AW_DRV_SENSOR_LIS3MDL
    extern void awbl_lis3mdl_drv_register(void);
    awbl_lis3mdl_drv_register();
#endif

#ifdef AW_DRV_SENSOR_BME280
    extern void awbl_bme280_drv_register(void);
    awbl_bme280_drv_register();
#endif

#ifdef AW_DRV_SENSOR_MMC5883MA
    extern void awbl_mmc5883ma_drv_register(void);
    awbl_mmc5883ma_drv_register();
#endif

#ifdef AW_DRV_SENSOR_BMA253
    extern void awbl_bma253_drv_register(void);
    awbl_bma253_drv_register();
#endif
#ifdef AW_DRV_SENSOR_BMP280
    extern void awbl_bmp280_drv_register(void);
    awbl_bmp280_drv_register();
#endif

#ifdef AW_DRV_SENSOR_LPS22HB
    extern void awbl_lps22hb_drv_register(void);
    awbl_lps22hb_drv_register();
#endif

#ifdef AW_DRV_SENSOR_LSM6DSL
    extern void awbl_lsm6dsl_drv_register(void);
    awbl_lsm6dsl_drv_register();
#endif

#ifdef AW_DRV_SENSOR_BH1730
    extern void awbl_bh1730_drv_register(void);
    awbl_bh1730_drv_register();
#endif

#ifdef AW_DRV_SENSOR_LTR553
    extern void awbl_ltr553_drv_register(void);
    awbl_ltr553_drv_register();
#endif

#ifdef AW_DRV_SENSOR_BMG160
    extern void awbl_bmg160_drv_register(void);
    awbl_bmg160_drv_register();
#endif

#ifdef AW_DRV_IMX1050_TIMESTAMP
    extern void awbl_imx1050_timestamp_drv_register(void);
    awbl_imx1050_timestamp_drv_register();
#endif
}

void aw_prj_early_init (void)
{
    awbl_group_init();

    awbl_intctlr_init();    /* AWBus IntCtlr service up */

#ifdef AW_COM_SERIAL
    aw_serial_init();
#endif  /* AW_COM_SERIAL */
    /* AWBus init phase 1 */
    awbl_dev_init1();

#ifdef AW_COM_LED
    awbl_led_init();        /* AWBus LED service up */
#endif

#ifdef AW_COM_PWM
    awbl_pwm_init();
#endif

#ifdef AW_COM_CAP
    awbl_cap_init();
#endif

#ifdef AW_COM_I2C
    awbl_i2cbus_init();
#endif

#ifdef AW_COM_SPI
    awbl_spibus_init();
#endif

#ifdef AW_COM_ADC
    awbl_adc_init();
#endif  /* AW_COM_ADC */

#ifdef AW_DEV_IMX1050_EDMA
    awbl_imx10xx_edma_init();
#endif

#ifdef AW_DEV_IMX1050_PXP
    awbl_imx1050_pxp_init();
#endif

#ifdef AW_DEV_CAMERA
    aw_camera_serve_init();
#endif


}

void aw_prj_task_level_init (void)
{
    /*
     * Initialize serial I/O for kernel output. after this call, kprintf()
     * is ready for use
     */
#ifdef AW_COM_CONSOLE
    aw_koutput_sio_init(AW_CFG_CONSOLE_COMID, AW_CFG_CONSOLE_BAUD_RATE);
#if AW_CFG_CONSOLE_SHOW_BANNER != 0
    aw_console_show_banner();
#endif  /* AW_CFG_CONSOLE_SHOW_BANNER */
#endif  /* AW_COM_CONSOLE */

    /* isr deferral library initialization */
    aw_isr_defer_lib_init();
    aw_nor_defer_lib_init();

#ifdef AW_COM_EVENT
    aw_event_lib_init();        /* event lib initialization */
#endif  /* AW_COM_EVENT */

#ifdef AW_COM_INPUT_EV
    awbl_input_ev_mgr_register();
#endif

#ifdef AW_COM_IO_SYSTEM
    aw_io_lib_init();
#endif  /* AW_COM_IO_SYSTEM */

#ifdef AW_COM_BLOCK_DEV
#ifndef AW_BLOCK_DEV_EVT_CNT
#define AW_BLOCK_DEV_EVT_CNT 4
#endif
    aw_blk_dev_lib_init(AW_BLOCK_DEV_EVT_CNT);    /* block device lib initialization */
#endif  /* AW_DRV_BLOCK_DEV */

#ifdef AW_COM_FS_ROOTFS
    aw_root_fs_init();
#endif  /* AW_COM_FS_ROOTFS */

#ifdef AW_COM_FS_FATFS
    aw_fatfs_lib_init();
#endif  /* AW_COM_FS_FATFS */

#ifdef AW_COM_MTD_DEV
    aw_mtd_lib_init();
#endif

#ifdef AW_COM_FS_LFFS
    aw_lffs_lib_init();
#endif  /* AW_COM_FS_LFFS */

#ifdef AW_COM_FS_LITTLEFS
    aw_littlefs_lib_init();
#endif  /* AW_COM_FS_LFFS */

#ifdef AW_COM_FS_YAFFS
    aw_yaffs_lib_init();
#endif

#ifdef AW_COM_MOUNT
    aw_mount_lib_init();
//   (void)aw_mount_table_add("/sd", "/dev/sd0", "vfat", 0);
#endif  /* AW_COM_MOUNT */

#if defined(AW_COM_USBH) || defined(AW_COM_USBD)
    extern void awbl_usb_cfg (void);
    awbl_usb_cfg();
#endif

#if defined(AW_DRV_IMX1050_TS) || \
    defined(AW_DRV_BU21029MUV_TS) || \
    defined(AW_DRV_FT5X06_TS)
    aw_ts_cfg_init();
#endif

#ifdef AW_COM_WDT
    awbl_wdt_init();            /* AWBus WDT service up */
#endif  /* AW_COM_WDT */

    /* system tick initialization */
    systick_init(AW_CFG_TICKS_PER_SECOND);

    /* AWBus init phase 2 */
    awbl_dev_init2();

#ifdef AW_COM_RTC
    awbl_rtc_init(); /* AWBus RTC service up */
#endif  /* AW_COM_RTC */

    aw_time_lib_init(aw_sys_clkrate_get(),0);


#ifdef AW_COM_SHELL_SERIAL
    extern aw_err_t aw_serial_shell_cfg();
    aw_serial_shell_cfg();

#ifdef AW_COM_SHELL_LUA
    aw_lua_shell_init();
#endif

#ifdef AW_COM_SHELL_LUAC
    aw_luac_shell_init();
#endif

#ifdef AW_COM_SHELL_MPY
    aw_python_shell_init();
#endif
#ifdef AW_SHELL_SYS_CMD
    extern void aw_io_shell_init (void);
    aw_io_shell_init();
#endif  /* AW_SHELL_SYS_CMD */
#endif  /* AW_COM_SERIAL */

#ifdef AW_COM_FTL
    aw_ftl_lib_init();
#endif

#ifdef AW_COM_LOG
    aw_loglib_cfg_init();
#endif

    /* AW_COM_NETWORK */
#ifdef AW_COM_NETWORK
    aw_net_init();

#if defined(AW_COM_SHELL_SERIAL) || defined(AW_COM_SHELL_TELNET)
#ifdef AW_NET_TOOLS
    aw_net_tools_register();
#endif
#endif
#endif

#ifdef AW_COM_SHELL_TELNET
    aw_telnet_shell_cfg();
#endif

#if defined(AW_COM_SHELL_SERIAL) || defined(AW_COM_SHELL_TELNET)
    aw_rtk_tools_register();
#endif

#ifdef AW_COM_LRNET
    extern void  aw_lrnet_init(void);
    aw_lrnet_init();
#endif

#ifdef AW_COM_GPRS
    extern void awbl_gprs_init (void);
    awbl_gprs_init();
    extern void sio_list_init(void);
    sio_list_init();
#endif

#ifdef AW_COM_GNSS
    awbl_gnss_init();
#endif

#ifdef AW_COM_AWSA
    awsa_cfg_init();
#endif

#ifdef AW_COM_CANFESTIVAL
    aw_canfestival_lib_init();
#endif

#ifdef AW_COM_CANOPEN
    aw_cop_lib_init();
#endif

    /* AWBus init phase 3 */
    awbl_dev_connect();

#if defined (__CC_ARM)
//    void aw_cplusplus_runtime_init(void);
//    aw_cplusplus_runtime_init();
#elif defined (__GNUC__)
    void aw_cplusplus_runtime_init(void);
    aw_cplusplus_runtime_init();
#endif

#ifdef AW_COM_SENSOR
    extern void awbl_sensor_init(void);
    awbl_sensor_init();
#endif

}


/* end of file */
