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
 * \brief AWBus IMX10xx on chip watchdog driver source file
 *
 * \internal
 * \par Modification History
 * - 1.00 17-11-22  hsg, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "driver/wdt/awbl_imx10xx_wdt.h"

/*******************************************************************************
  typedef
*******************************************************************************/

typedef struct __imx10xx_wdt_regs {
    volatile uint16_t WCR;  /**< Watchdog Control Register, offset: 0x0 */
    volatile uint16_t WSR;  /**< Watchdog Service Register, offset: 0x2 */
    volatile uint16_t WRSR; /**< Watchdog Reset Status Register, offset: 0x4 */
    volatile uint16_t WICR; /**< Watchdog Interrupt Control Register, offset: 0x6 */
    volatile uint16_t WMCR; /**< Watchdog Miscellaneous Control Register, offset: 0x8 */
} __imx10xx_wdt_regs_t;

/*******************************************************************************
  define
*******************************************************************************/

#define __WDT_WDZST               0x01          /* Watchdog Low Power */
#define __WDT_WDBG                0x02          /* Watchdog DEBUG Enable */
#define __WDT_WDE                 0x04          /* enable wdt */
#define __WDT_WDT                 0x08          /* enable time_out */
#define __WDT_SRS                 0x10          /* Software Reset Signal */
#define __WDT_WT_SET(x)           ((x) << 8)    /* countdown timer count (0.5s per 1) */
#define __WDT_WSR_5555            0x5555        /* reload first cmd */
#define __WDT_WSR_AAAA            0xAAAA        /* reload second cmd */


#define __IMX10xx_WDT_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_wdt_devinfo *p_devinfo = \
        (struct awbl_imx10xx_wdt_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define  __INFO_TO_IMX10xx_WDT(p_info) \
    AW_CONTAINER_OF(p_info, struct awbl_imx10xx_wdt_dev, info)

/******************************************************************************/


/** \brief watchdog feed */
aw_local void __imx10xx_wdt_feed (struct awbl_hwwdt_info *pwdt_info)
{
    struct awbl_imx10xx_wdt_dev *p_wdt_dev = __INFO_TO_IMX10xx_WDT(pwdt_info);
    __imx10xx_wdt_regs_t        *p_reg     = (__imx10xx_wdt_regs_t *)(p_wdt_dev->reg_base);

    p_reg->WSR = __WDT_WSR_5555;    /* first reload */
    p_reg->WSR = __WDT_WSR_AAAA;    /* second reload */

}


/** \brief instance initializing stage 1 */
aw_local void  __imx10xx_wdt_inst_init (struct awbl_dev *p_dev)
{
    __IMX10xx_WDT_DEVINFO_DECL(p_devinfo, p_dev);
    struct awbl_imx10xx_wdt_dev  *p_wdt_dev = (struct awbl_imx10xx_wdt_dev *)p_dev;
    __imx10xx_wdt_regs_t         *p_reg     = (__imx10xx_wdt_regs_t *)(p_devinfo->reg_base);

    p_wdt_dev->info.wdt_time = p_devinfo->wdt_time;
    p_wdt_dev->info.feed     = __imx10xx_wdt_feed;
    p_wdt_dev->reg_base      = p_devinfo->reg_base;

    /* platform init (enable wdt clock) */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    /* register IMX10xx watchdog info */
    awbl_wdt_device_register(&p_wdt_dev->info);

    /* disable Power Down Counter of WDOG */
    p_reg->WMCR = 0;

    /* set iMX10xx watchdog watch time & open */
    p_reg->WCR = __WDT_WDZST | __WDT_WDBG | __WDT_WDE | __WDT_WDT | __WDT_SRS |
                 __WDT_WT_SET(p_devinfo->wdt_time / 500 - 1);
//    p_reg->WCR = 0x0134;


    /* load count once */
    p_reg->WSR = __WDT_WSR_5555;        /* first reload */
    p_reg->WSR = __WDT_WSR_AAAA;        /* second reload */
}

/*******************************************************************************
  locals
*******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_imx10xx_wdt = {
    __imx10xx_wdt_inst_init, /* devInstanceInit */
    NULL,                    /* devInstanceInit2 */
    NULL                     /* devConnect */
};

/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_imx10xx_wdt = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    AWBL_IMX10XX_WDT_NAME,              /* p_drvname */
    &__g_awbl_drvfuncs_imx10xx_wdt,     /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

/******************************************************************************/
void awbl_imx10xx_wdt_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_imx10xx_wdt);
}

/* end of file */

