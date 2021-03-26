/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief AWBus IMX10xx on chip watchdog interface header file
 *
 * use this module should include follows :
 * \code
 * #include "awbl_wdt.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-22  hsg, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_WDT_H
#define __AWBL_IMX10xx_WDT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/*******************************************************************************
  includes
*******************************************************************************/
#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_wdt.h"

/*******************************************************************************
  define
*******************************************************************************/

/** \brief define IMX10XX watchdog's name */
#define  AWBL_IMX10XX_WDT_NAME  "imx10xx_wdt"

/** \brief define IMX10xx watchdog's time info */
typedef struct  awbl_imx10xx_wdt_devinfo {
    uint32_t    reg_base;           /**< \brief WDT register base address */
    uint32_t    wdt_time;           /**< \brief watchdog time (ms)*/
    void (*pfn_plfm_init) (void);   /**< \brief platform init */
} awbl_imx10xx_wdt_devinfo_t;

/** \brief define IMX10xx watchdog's device */
typedef struct  awbl_imx10xx_wdt_dev {
    struct awbl_dev          dev;    /**< the AWBus device */
    struct awbl_hwwdt_info   info;   /**< hw wdt info */
    uint32_t                 reg_base;
} awbl_imx10xx_wdt_dev_t;

/**
 * \brief IMX10xx wdt driver register
 */
void awbl_imx10xx_wdt_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_WDT_H */

/* end of file */
