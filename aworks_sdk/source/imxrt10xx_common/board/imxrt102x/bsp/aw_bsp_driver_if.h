/*******************************************************************************
*                                 AWorks
*                       ----------------------------
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
 * \brief AWBus driver register and initializing interfaces.
 *
 * \internal
 * \par modification history
 * - 1.00 13-03-07  orz, first implementation.
 * \endinternal
 */
 
#ifndef __AW_BSP_DRIVER_IF_H
#define __AW_BSP_DRIVER_IF_H

#include "aw_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup aw_bsp_driver_if
 * @{
 */
 
aw_export void awbl_imx1020_gpio_drv_register (void);
aw_export void awbl_imx10xx_gpt_drv_register (void);
aw_export void awbl_imx10xx_gpt_cap_drv_register (void);
aw_export void awbl_imx10xx_gpt_pwm_drv_register (void);
aw_export void awbl_imx10xx_pit_drv_register (void);
aw_export void awbl_imx10xx_lpuart_drv_register (void);
aw_export void awbl_led_gpio_drv_register (void);
aw_export void awbl_imx10xx_enet_drv_register (void);
aw_export void awbl_imx10xx_lpspi_drv_register (void);
aw_export void awbl_imx10xx_wdt_drv_register (void);
aw_export void awbl_imx10xx_rtwdt_drv_register (void);
aw_export void awbl_imx10xx_adc_drv_register (void);
aw_export void awbl_imx10xx_sdio_drv_register (void);
aw_export void awbl_imx10xx_edma_drv_register (void);
aw_export void awbl_imx10xx_lpi2c_drv_register (void);
aw_export void awbl_imx10xx_flexcan_drv_register (void);
aw_export void awbl_imx10xx_eflex_pwm_drv_register (void);
aw_export void awbl_imx10xx_qtimer_cap_drv_register (void);
aw_export void awbl_imx10xx_qtimer_pwm_drv_register (void);
aw_export void awbl_imx10xx_qtimer_timer_drv_register (void);
aw_export void awbl_imx10xx_usbd_drv_register (void);
aw_export void awbl_imx10xx_usbh_drv_register (void);
aw_export void awbl_imx10xx_edma_init (void);
aw_export void awbl_imx10xx_rtc_drv_register (void);
aw_export void awbl_imx10xx_enc_drv_register (void);
aw_export void awbl_imx10xx_tempmon_drv_register (void);
aw_export void awbl_imx1020_flexspi_drv_register (void);
aw_export void awbl_imx1020_sai_drv_register (void);
aw_export void awbl_imx1020_mqs_drv_register (void);
aw_export void awbl_imx1020_ocotp_register (void);
/** @} */

 
 
 
#ifdef __cplusplus
}
#endif

#endif /* __AW_BSP_DRIVER_IF_H */

/* end of file */
