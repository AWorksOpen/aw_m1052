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
 * \brief TODO文件描述
 *
 * 使用本服务需要包含头文件
 * \code
 * #include "awbl_cyw43455_platform_host.c"
 * \endcode
 *
 * \internal
 * \par Modification History
 * - 1.00 19-04-17  mex, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_gpio.h"
#include "aw_spinlock.h"

#include "driver/wifi/cypress/cyw43455/awbl_cyw43455_drv_common.h"
#include "driver/wifi/cypress/cyw43455/awbl_cyw43455_sdio.h"
#include "driver/wifi/cypress/cyw43455/private/awbl_cyw43455_sdio.h"
#include "wwd_buffer.h"
#include "wwd_constants.h"
#include "platform/wwd_sdio_interface.h"


/******************************************************************************
   HOST
 ******************************************************************************/
extern struct awbl_sdio_host              *__gp_cyw43455_sdio_host;
extern awbl_sdio_cyw43455_wlan_dev_info_t *__gp_cyw43455_sdio_info;
extern struct awbl_dev                    *__gp_cyw43455_sdio_dev;

wwd_result_t host_platform_init (void)
{
    if ((__gp_cyw43455_sdio_host == NULL) || (__gp_cyw43455_sdio_info == NULL)) {
        return WWD_DOES_NOT_EXIST;
    }
    /*
     * 此处保证模块处于不复位状态，wiced协议栈里面不再进行复位操作，
     * 因为设备在bus已经枚举完成
     */ 
    aw_gpio_pin_cfg(__gp_cyw43455_sdio_info->base.reset_pin, 
                    AW_GPIO_OUTPUT_INIT_HIGH);

    return WWD_SUCCESS;
}

wwd_result_t host_platform_bus_init (void)
{
    if (__gp_cyw43455_sdio_host == NULL) {
        return WWD_DOES_NOT_EXIST;
    }
    return WWD_SUCCESS;
}

void host_platform_power_wifi (wiced_bool_t power_enabled)
{
    return;
}

void host_platform_reset_wifi (wiced_bool_t reset_asserted)
{

    if (__gp_cyw43455_sdio_info == NULL) {
        return;
    }
#if 0 
    aw_gpio_set(__gp_cyw43455_sdio_info->base.reset_pin, 
               (AW_TRUE == reset_asserted) ? 0 : 1);
#endif
}

wwd_result_t host_platform_init_wlan_powersave_clock (void)
{
    return WWD_SUCCESS;
}

wwd_result_t host_platform_bus_enable_interrupt (void)
{
    return awbl_43455_sdio_enable_int();
}

wwd_result_t host_platform_bus_disable_interrupt (void)
{

    return WWD_SUCCESS;
}

wiced_bool_t host_platform_is_in_interrupt_context (void)
{
    return AW_TRUE;
}


/******************************************************************************
   NETWORK
 ******************************************************************************/

void host_network_process_ethernet_data (wiced_buffer_t  buffer, 
                                         wwd_interface_t interface)
{

    if (__gp_cyw43455_sdio_dev == NULL) {
        return;
    }
    awbl_wiced_eth_pkt_processor(__gp_cyw43455_sdio_dev, buffer, interface);

}


/******************************************************************************
   SDIO
 ******************************************************************************/
wwd_result_t host_platform_sdio_transfer (wwd_bus_transfer_direction_t direction,
                                          int                          function,
                                          uint32_t                     address,
                                          uint16_t                     data_size,
                                          uint8_t                      *data)
{
    return awbl_43455_sdio_transfer(direction, function, address, data_size, data);
}

wwd_result_t host_platform_sdio_enumerate (void)
{
    if (__gp_cyw43455_sdio_host == NULL) {
        return WWD_DOES_NOT_EXIST;
    }
    return WWD_SUCCESS;
}

wwd_result_t host_platform_enable_high_speed_sdio (void)
{
    return awbl_43455_sdio_enable_hsp();
}

wwd_result_t host_platform_unmask_sdio_interrupt (void)
{
    return awbl_43455_sdio_unmask_sdio_int();
}

wwd_result_t host_enable_oob_interrupt (void)
{
    return awbl_43455_sdio_enable_oob_int();
}

uint8_t host_platform_get_oob_interrupt_pin (void)
{
    return awbl_43455_sdio_get_int_pin();
}


/*******************************************************************************
  NIC
*******************************************************************************/
extern aw_err_t awbl_wiced_nic_set_link_up (awbl_dev_t            *p_dev,
                                            aw_wifi_interface_t    interface);
extern aw_err_t awbl_wiced_nic_set_link_down (awbl_dev_t          *p_dev,
                                              aw_wifi_interface_t  interface);

void host_platform_nic_link_up (uint8_t interface)
{

    if (__gp_cyw43455_sdio_dev == NULL) {
        return;
    }
    awbl_wiced_nic_set_link_up(__gp_cyw43455_sdio_dev, interface);
}

void host_platform_nic_link_down (uint8_t interface)
{
    if (__gp_cyw43455_sdio_dev == NULL) {
        return;
    }
    awbl_wiced_nic_set_link_down(__gp_cyw43455_sdio_dev, interface);
}


/*******************************************************************************
  FIRMWARE
*******************************************************************************/

/* 获取WIFI固件的大小 */
void host_platform_firmware_size (uint32_t *size_out)
{
    if (__gp_cyw43455_sdio_info == NULL) {
        return;
    }
    __gp_cyw43455_sdio_info->base.resource_hdls.fw.pfn_size(size_out);
}

/* 获取NVRAM配置信息的大小 */
void host_platform_cfg_nvram_size (uint32_t *size_out)
{
    if (__gp_cyw43455_sdio_info == NULL) {
        return;
    }
    __gp_cyw43455_sdio_info->base.resource_hdls.nv.pfn_size(size_out);
}

/* 获取CLM BLOB的大小 */
void host_platform_clm_blob_size (uint32_t *size_out)
{
    if (__gp_cyw43455_sdio_info == NULL) {
        return;
    }
    __gp_cyw43455_sdio_info->base.resource_hdls.clm.pfn_size(size_out);
}


/* 读WIFI固件数据 */
void host_platform_firmware_read (uint32_t  offset,
                                  void     *buffer,
                                  uint32_t  size_in,
                                  uint32_t *size_out)
{
    if (__gp_cyw43455_sdio_info == NULL) {
        return;
    }
    __gp_cyw43455_sdio_info->base.resource_hdls.fw.pfn_read(offset,
                                                            buffer, 
                                                            size_in, 
                                                            size_out);
}

/* 读NVRAM数据 */
void host_platform_cfg_nvram_read (uint32_t  offset,
                                   void     *buffer,
                                   uint32_t  size_in,
                                   uint32_t *size_out)
{
    if (__gp_cyw43455_sdio_info == NULL) {
        return;
    }
    __gp_cyw43455_sdio_info->base.resource_hdls.nv.pfn_read(offset,
                                                            buffer, 
                                                            size_in, 
                                                            size_out);
}

/* 读CLM BLOB数据 */
void host_platform_clm_blob_read (uint32_t  offset,
                                  void     *buffer,
                                  uint32_t  size_in,
                                  uint32_t *size_out)
{
    if (__gp_cyw43455_sdio_info == NULL) {
        return;
    }
    __gp_cyw43455_sdio_info->base.resource_hdls.clm.pfn_read(offset, 
                                                             buffer, 
                                                             size_in, 
                                                             size_out);
}


/* end of file */
