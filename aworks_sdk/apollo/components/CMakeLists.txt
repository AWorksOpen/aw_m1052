


install(DIRECTORY awbus_lite/include/
  DESTINATION apollo/components/awbus_lite/include
  )

install(DIRECTORY awbus_lite/config/
  DESTINATION apollo/components/awbus_lite/config
  )


set(awbus_lite_core
  awbus_lite/source/core/awbl_adc.c
  awbus_lite/source/core/awbl_buzzer.c
  awbus_lite/source/core/awbl_can.c
  awbus_lite/source/core/awbl_cap.c
  awbus_lite/source/core/awbl_crc.c
  awbus_lite/source/core/awbl_crctable.c
  awbus_lite/source/core/awbl_dac.c
  awbus_lite/source/core/awbl_digitron.c
  awbus_lite/source/core/awbl_digitron_if.c
  awbus_lite/source/core/awbl_gpio.c
  awbus_lite/source/core/awbl_gpio_hc595.c
  awbus_lite/source/core/awbl_i2cbus.c
  awbus_lite/source/core/awbl_input_event.c
  awbus_lite/source/core/awbl_input_if.c
  awbus_lite/source/core/awbl_intctlr.c
  awbus_lite/source/core/awbl_intensity.c
  awbus_lite/source/core/awbl_led.c
  awbus_lite/source/core/awbl_miibus.c
  awbus_lite/source/core/awbl_nandbus.c
  awbus_lite/source/core/awbl_nandflash_ctlr.c
  awbus_lite/source/core/awbl_nandflash_ctlr_std_drvs.c
  awbus_lite/source/core/awbl_nic_manage.c
  awbus_lite/source/core/awbl_nvram.c
  awbus_lite/source/core/awbl_pwm.c
  awbus_lite/source/core/awbl_rtc.c
  awbus_lite/source/core/awbl_scanner.c
  awbus_lite/source/core/awbl_sdiobus.c
  awbus_lite/source/core/awbl_serial.c
  awbus_lite/source/core/awbl_spibus.c
  awbus_lite/source/core/awbl_timer.c
  awbus_lite/source/core/awbl_timestamp.c
  awbus_lite/source/core/awbl_touch_screen.c
  awbus_lite/source/core/awbl_upp.c
  awbus_lite/source/core/awbl_wdt.c
  awbus_lite/source/core/awbl_wifi.c
  )


set(awbus_lite_driver_easy_arm_imx28x
  awbus_lite/source/driver/adc/awbl_imx28_hsadc.c
  awbus_lite/source/driver/adc/awbl_imx28_lradc.c
  awbus_lite/source/driver/busctlr/awbl_imx28_ssp.c
  awbus_lite/source/driver/busctlr/awbl_imx28_sdio.c
  awbus_lite/source/driver/busctlr/awbl_imx28_i2c.c
  awbus_lite/source/driver/busctlr/awbl_imx28_gpmi.c
  awbus_lite/source/driver/dcp/awbl_imx28_dcp.c
  awbus_lite/source/driver/dma/awbl_imx28_dma.c
  awbus_lite/source/driver/flexcan/awbl_imx28_can.c
  awbus_lite/source/driver/input/touchscreen/awbl_imx28_ts.c
  awbus_lite/source/driver/intctlr/awbl_imx28_intc.c
  awbus_lite/source/driver/nvram/awbl_imx28_ocotp.c
  awbus_lite/source/driver/rtc/awbl_imx283_rtc.c
  awbus_lite/source/driver/serial/awbl_imx28x_duart.c
  awbus_lite/source/driver/serial/awbl_imx28_auart.c
  awbus_lite/source/driver/timer/awbl_imx28_timer.c
  awbus_lite/source/driver/usb/awbl_imx28_usbd.c
  awbus_lite/source/driver/usb/awbl_imx28_usbh.c
  awbus_lite/source/driver/wdt/awbl_imx283_wdt.c
  )
# fixme: 这里的驱动本不应该依赖apollo/cpu里面的头文件，但是283依赖了
set(awbus_lite_driver_include_dir_easy_arm_imx28x
    ../cpu/freescale/imx28x/include 
    ../components/usb/include
	)
  
  
set(awbus_lite_driver_ti_c674x
  awbus_lite/source/driver/busctlr/awbl_c674x_emifa.c
  awbus_lite/source/driver/busctlr/awbl_c674x_i2c.c
  awbus_lite/source/driver/busctlr/awbl_c674x_sdio.c
  awbus_lite/source/driver/usb/awbl_c674x_usbh.c
  awbus_lite/source/driver/pwm/awbl_c674x_pwm.c
  awbus_lite/source/driver/dma/awbl_ti_edma3.c
  )

set(awbus_lite_driver
  awbus_lite/source/driver/board/awbl_apl_d01.c
  awbus_lite/source/driver/board/awbl_panel_pm1000.c
  awbus_lite/source/driver/board/awbl_tiny_hmi.c
  awbus_lite/source/driver/busctlr/awbl_gpio_i2c.c
  awbus_lite/source/driver/busctlr/awbl_gpio_spi.c
  awbus_lite/source/driver/busctlr/awbl_sdio_spi.c
  awbus_lite/source/driver/buzzer/awbl_buzzer_dc.c
  awbus_lite/source/driver/buzzer/awbl_buzzer_pwm.c
  awbus_lite/source/driver/digitron/awbl_digitron_seg8_ascii.c
  awbus_lite/source/driver/digitron/awbl_hc595_digitron.c
#  awbl_gprs_sim800: 这个驱动依赖 以太网 aw_lwip_1.4.x 
  awbus_lite/source/driver/gprs/awbl_gprs_sim800.c
  awbus_lite/source/driver/gprs/gb2312_unicode_table.c
  awbus_lite/source/driver/input/key/awbl_gpio_key.c
  awbus_lite/source/driver/input/key/awbl_hc595_matrix_key.c
  awbus_lite/source/driver/input/key/awbl_matrix_keypad.c
  awbus_lite/source/driver/input/knob/awbl_gpio_knob.c
  awbus_lite/source/driver/intctlr/awbl_armcm_nvic.c
  awbus_lite/source/driver/intensity/awbl_pwm_it.c
  awbus_lite/source/driver/led/awbl_gpio_led.c
  awbus_lite/source/driver/led/awbl_hc595_digi_led.c
  awbus_lite/source/driver/led/awbl_hc595_led.c
  awbus_lite/source/driver/mprinter/awbl_zytp.c
  awbus_lite/source/driver/nand/awbl_nandflash.c
  awbus_lite/source/driver/net/awbl_emac_ax88796.c
  awbus_lite/source/driver/norflash/awbl_nor_flash.c
  awbus_lite/source/driver/norflash/awbl_spi_flash.c
  awbus_lite/source/driver/nvram/awbl_ep24cxx.c
  awbus_lite/source/driver/phy/awbl_generic_phy.c
  awbus_lite/source/driver/phy/awbl_dp83848_phy.c
  awbus_lite/source/driver/relay/awbl_relay.c
  awbus_lite/source/driver/relay/awbl_relay_dc.c
  awbus_lite/source/driver/rtc/awbl_jyrtc1.c
  awbus_lite/source/driver/rtc/awbl_pcf8563.c
  awbus_lite/source/driver/rtc/awbl_pcf85063.c
  awbus_lite/source/driver/rtc/awbl_rx8025sa.c
  awbus_lite/source/driver/rtc/awbl_rx8025t.c
  awbus_lite/source/driver/rtc/awbl_ds1302.c
  awbus_lite/source/driver/sdiocard/aw_sdcard.c
  awbus_lite/source/driver/serial/awbl_ns16550.c
  awbus_lite/source/driver/serial/awbl_ns16550_plb.c
  awbus_lite/source/driver/serial/awbl_sc16is7xx_i2c.c
  awbus_lite/source/driver/serial/awbl_sc16is7xx_spi.c
  awbus_lite/source/driver/thermistor/awbl_temp.c
  awbus_lite/source/driver/thermistor/awbl_therm_ntc.c
  awbus_lite/source/driver/upp/awbl_ti_upp.c
  awbus_lite/source/driver/wdt/awbl_gpio_wdt.c
  awbus_lite/source/driver/rfid/awbl_zlg600a.c
  awbus_lite/source/driver/rfid/awbl_zlg600a_i2c.c
  awbus_lite/source/driver/rfid/awbl_zlg600a_uart.c  
  )

set(awbus_lite_methods
  awbus_lite/source/methods/awbl_adc_methods.c
  awbus_lite/source/methods/awbl_capserv_get.c
  awbus_lite/source/methods/awbl_crcserv_get.c
  awbus_lite/source/methods/awbl_dac_methods.c
  awbus_lite/source/methods/awbl_dmaserv_get.c
  awbus_lite/source/methods/awbl_gpioserv_get.c
  awbus_lite/source/methods/awbl_icserv_get.c
  awbus_lite/source/methods/awbl_ledserv_get.c
  awbus_lite/source/methods/awbl_mii_phy_mode_setget.c
  awbus_lite/source/methods/awbl_mii_phy_rw.c
  awbus_lite/source/methods/awbl_mii_update_stat.c
  awbus_lite/source/methods/awbl_net_manage_methods.c
  awbus_lite/source/methods/awbl_nvram_get.c
  awbus_lite/source/methods/awbl_nvram_set.c
  awbus_lite/source/methods/awbl_pwmserv_get.c
  awbus_lite/source/methods/awbl_remove_dev.c
  awbus_lite/source/methods/awbl_rtcserv_get.c
  awbus_lite/source/methods/awbl_sdio_methods.c
  awbus_lite/source/methods/awbl_serial_methods.c
  awbus_lite/source/methods/awbl_timer_methods.c
  )

set(awbus_lite_include
  awbus_lite/include/
  awbus_lite/config/
  )


install(DIRECTORY base/include
  DESTINATION apollo/components/base
  )

install(DIRECTORY base/config
  DESTINATION apollo/components/base
  )

set(aw_base_rtk
#  base/source/util/aw_extimer.c
  )

set(aw_base
  base/source/eio/aw_eio.c

  base/source/fs/aw_blk_dev.c
  base/source/fs/aw_fs_type.c
  base/source/fs/aw_mkfs.c
  base/source/fs/aw_mount.c
  base/source/fs/aw_ram_disk.c
  base/source/fs/aw_raw_fs.c
  base/source/fs/aw_root_fs.c

  base/source/io/aw_dirent.c
  base/source/io/aw_io_device.c
  base/source/io/aw_io_entry.c
  base/source/io/aw_io_lib.c
  base/source/io/aw_io_shell.c
  base/source/io/aw_stat.c
  base/source/io/aw_statvfs.c
  base/source/io/aw_unistd.c
  base/source/io/aw_utime.c
  
  base/source/mtd/aw_mtd.c
  base/source/mtd/aw_mtd_part.c
  base/source/mtd/aw_mtd_ram.c
  
  base/source/os/aw_delay.c
  base/source/os/aw_delayed_work.c
  base/source/os/aw_isr_defer.c
  base/source/os/aw_job_queue.c
  base/source/os/aw_koutput.c
  base/source/os/aw_koutput_sio.c
  base/source/os/aw_kprintf.c
  base/source/os/aw_mallocator.c
  base/source/os/aw_memheap.c
  base/source/os/aw_pool_lib.c
  base/source/os/aw_slab.c
  base/source/os/aw_task2.c
  base/source/os/aw_time.c
  base/source/os/aw_time_keeper.c
  base/source/os/aw_ty.c
  base/source/os/aw_slab.c
  
  base/source/user/aw_user.c
  
  base/source/util/aw_event.c
  base/source/util/aw_flag_grp.c
  base/source/util/aw_flash_eeprom.c
  base/source/util/aw_hotplug_event.c
  base/source/util/aw_logger_seiral.c
  base/source/util/aw_loglib.c
  base/source/util/aw_md5.c
  base/source/util/aw_mprinter.c
  base/source/util/aw_pbuf.c
  base/source/util/aw_pipe.c
  base/source/util/aw_ring_buffer.c
  base/source/util/aw_sftimer_lib.c
  base/source/util/aw_soft_bch.c
  base/source/util/aw_ts_lib.c
  )
  
set(aw_printf_full_function
  base/source/os/aw_snprintf_full.c
)

set(aw_printf_mini
  base/source/os/aw_vfprintf_do.c
  base/source/os/aw_vsnprintf.c
  base/source/os/aw_snprintf.c
)

# printf版本选择
awgui_list(AWORKS_PRINTF_VERSION full_function mini )

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/awbus_lite/source/core/awbus_lite.c )
  add_library( awbl_core STATIC
    awbus_lite/source/core/awbus_lite.c
    awbus_lite/source/core/awbl_access.c
    awbus_lite/source/core/awbl_plb.c
    )
  target_link_libraries(awbl_core PUBLIC aworks)
  target_include_directories(awbl_core PUBLIC
    ${awbus_lite_include}
    )
  message(STATUS "libawbl_core:rebuild")
else()
  add_library(awbl_core INTERFACE )
  set(awbl_core_interface_lib_easy_arm_imx28x ${AWORKS_ROOT}/apollo/lib/arm926ejs_rtk_gcc_debug/libawbl_core.a )
  set(awbl_core_interface_lib_ti_c674x ${AWORKS_ROOT}/apollo/lib/tic674_dspbios5_debug/libawbl_core.lib )
  set(awbl_core_interface_lib_imx6ul ${AWORKS_ROOT}/apollo/lib/cortex_a9_rtk_gcc_soft/libawbl_core.a )
  set(awbl_core_interface_lib_easy_arm_17xxm3 ${AWORKS_ROOT}/apollo/lib/cortex_m3_rtk_gcc_debug/libawbl_core.a )
 # set(awbl_core_interface_lib_imx6ul ${AWORKS_ROOT}/build/sdks/imx6ul_sdk/projects_eclipse/img_imx6ul_debug/libawbl_core.a )
  message(STATUS "libawbl_core:${awbl_core_interface_lib_${AWORKS_BSP_NAME}}")
  target_link_libraries(awbl_core INTERFACE ${awbl_core_interface_lib_${AWORKS_BSP_NAME}}  )
  target_include_directories(awbl_core INTERFACE ${awbus_lite_include} )
  install(FILES ${awbl_core_interface_lib_${AWORKS_BSP_NAME}} DESTINATION ${AWORKS_LIB_INSTALL_DIR} )
endif()

add_library(aw_common STATIC
  ${awbus_lite_core}
  ${common_HEADERS}
  ${awbus_lite_methods}
  ${awbus_lite_driver}
  # CPU相关驱动
  ${awbus_lite_driver_${AWORKS_BSP_NAME}}
  ${aw_base}
  ${aw_base_${AWORKS_PSP_NAME}}
  ${aw_printf_${AWORKS_PRINTF_VERSION}}
  )

target_include_directories(aw_common PUBLIC
  ${awbus_lite_include}
  #fixme: 这是特别为283准备的，它不应该依赖cpu目录里面的头文件，需要移除hadc驱动
  ${awbus_lite_driver_include_dir_${AWORKS_BSP_NAME}}  
  base/include
  base/config
  shell/include
  ${AWORKS_SDKROOT}/include/config/
  )
target_link_libraries(aw_common PUBLIC aworks aw_usb aw_lwip_1.4.x)
#target_compile_options(aw_common PRIVATE -O0 ) 
# set_source_files_properties(
#  awbus_lite/source/core/awbl_nandbus.c
#  awbus_lite/source/driver/nand/awbl_nandflash.c
#  base/source/mtd/aw_mtd.c
#  base/source/mtd/aw_mtd_part.c
#  base/source/mtd/aw_mtd_ram.c
#      
#  base/source/io/aw_dirent.c
#  base/source/io/aw_io_core.c
#  base/source/io/aw_io_cwd.c
#  base/source/io/aw_io_dev.c
#  base/source/io/aw_io_fd.c
#  base/source/io/aw_io_shell.c
#  base/source/io/aw_io_shell_ramdisk.c
#  base/source/io/aw_stat.c
#  base/source/io/aw_statvfs.c
#  base/source/io/aw_unistd.c
#  base/source/io/aw_utime.c
  
#  base/source/fs/aw_block_dev.c
#  base/source/fs/aw_dump_fs.c
#  base/source/fs/aw_fs_monitor.c
#  base/source/fs/aw_fs_type.c
#  base/source/fs/aw_mkfs.c
#  base/source/fs/aw_mount.c
#  base/source/fs/aw_ram_disk.c
#  base/source/fs/aw_raw_fs.c
#  base/source/fs/aw_root_fs.c
#  ${awbus_lite_core}
#  ${awbus_lite_driver}
#  # CPU相关驱动
#  ${awbus_lite_driver_${AWORKS_BSP_NAME}}
#  ${aw_base}
#  ${aw_base_${AWORKS_PSP_NAME}}
#   awbus_lite/source/driver/busctlr/awbl_c674x_emifa.c
#    PROPERTIES COMPILE_FLAGS  "-O0"
#   )

# set_target_properties(awbus_lite_core PROPERTIES VERSION 1.0 )

install(TARGETS aw_common awbl_core
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION ${AWORKS_LIB_INSTALL_DIR}
  RUNTIME DESTINATION bin
  )


if (AWORKS_CRP_EN)
    target_link_libraries( aw_common PRIVATE aw_crp)
    message("using crp module")
endif()

  
# include(CMakePackageConfigHelpers)

# write_basic_package_version_file(
#   "${CMAKE_CURRENT_BINARY_DIR}/awbus_lite_core/awbus_lite_coreConfigVersion.cmake"
#   VERSION 1.0
#   COMPATIBILITY AnyNewerVersion
# )

# export(EXPORT awbus_lite_core_ex FILE
#   "${CMAKE_CURRENT_BINARY_DIR}/awbus_lite_core/awbus_lite_core_ex.cmake"
#   )

# # configure_file(cmake/awbus_lite_coreConfig.cmake
# #   "${CMAKE_CURRENT_BINARY_DIR}/awbus_lite_core/awbus_lite_coreConfig.cmake"
# #   COPYONLY
# #   )

# install(EXPORT awbus_lite_core_ex
#   FILE awbus_lite_core_ex.cmake
#   DESTINATION lib/cmake/awbus_lite_core
#   )

# install( FILES
#   cmake/awbus_lite_coreConfig.cmake
#   "${CMAKE_CURRENT_BINARY_DIR}/awbus_lite_core/awbus_lite_coreConfig.cmake"
#   DESTINATION lib/cmake/awbus_lite_core
#   COMPONENT Devel
#   )
