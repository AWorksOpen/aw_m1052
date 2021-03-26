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
#include "driver/rtwdt/awbl_imx10xx_rtwdt.h"
#include "aw_int.h"
#include "aw_clk.h"

/*******************************************************************************
  typedef
*******************************************************************************/

#define     RTWDT_FEED_KEY      (0xB480A602)
#define     RTWDT_UPDATE_KEY    (0xD928C520)

/* rtwdt 寄存器定义 */
typedef struct __imx10xx_rtwdt_regs {
    volatile uint32_t CS;       /**< Watchdog Control and Status Register, offset: 0x0 */
    volatile uint32_t CNT;      /**< Watchdog Counter Register, offset: 0x4 */
    volatile uint32_t TOVAL;    /**< Watchdog Timeout Value Register, offset: 0x8 */
    volatile uint32_t WIN;      /**< Watchdog Window Register, offset: 0xC */
} __imx10xx_rtwdt_regs_t;

/*******************************************************************************
  define
*******************************************************************************/

#define __IMX10xx_RTWDT_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_rtwdt_devinfo *p_devinfo = \
        (struct awbl_imx10xx_rtwdt_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define  __INFO_TO_IMX10xx_RTWDT(p_info) \
    AW_CONTAINER_OF(p_info, struct awbl_imx10xx_rtwdt_dev, info)

/******************************************************************************/

/** \brief watchdog feed */
aw_local void __imx10xx_rtwdt_feed (struct awbl_hwwdt_info *pwdt_info)
{

    struct awbl_imx10xx_rtwdt_dev *p_rtwdt_dev = __INFO_TO_IMX10xx_RTWDT(pwdt_info);
    __imx10xx_rtwdt_regs_t        *p_reg     = (__imx10xx_rtwdt_regs_t *)(p_rtwdt_dev->reg_base);

    p_reg->CNT = RTWDT_FEED_KEY;

}


/** \brief instance initializing stage 1 */
aw_local void  __imx10xx_rtwdt_inst_init (struct awbl_dev *p_dev)
{
    __IMX10xx_RTWDT_DEVINFO_DECL(p_devinfo, p_dev);
    struct awbl_imx10xx_rtwdt_dev  *p_rtwdt_dev = (struct awbl_imx10xx_rtwdt_dev *)p_dev;
    __imx10xx_rtwdt_regs_t         *p_reg       = (__imx10xx_rtwdt_regs_t *)(p_devinfo->reg_base);
    uint32_t                        cs_temp     = 0;
    uint32_t                        toval_temp  = 0;
    uint32_t                        clk;

    AW_INT_CPU_LOCK_DECL(key);

    p_rtwdt_dev->info.wdt_time = p_devinfo->wdt_time;
    p_rtwdt_dev->info.feed     = __imx10xx_rtwdt_feed;
    p_rtwdt_dev->reg_base      = p_devinfo->reg_base;
    p_rtwdt_dev->wdt_time_up   = 0;
    p_rtwdt_dev->wdt_time_down = 0;

    /* platform init (enable wdt clock) */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    /* register IMX10xx watchdog info */
    awbl_wdt_device_register(&p_rtwdt_dev->info);

    /* 计算CR寄存器的值 */
    cs_temp =  ((0) << 0) |                 /* stop mode */
               ((0) << 1) |                 /* wait mode */
               ((0) << 2) |                 /* debug mode */
               ((0) << 3) |                 /* fast test mode */
               ((1) << 5) |                 /* update */
               ((0) << 6) |                 /* interrupt */
               ((1) << 7) |                 /* enable rtwdt */
               ((1) << 8) |                 /* clock source */
               ((1) << 12) |                /* prescaler */
               ((1) << 13) |                /* cmd32 enable */
               ((0) << 15);                 /* window mode */

    /* 计算toval的值  */
    clk         = aw_clk_rate_get(p_devinfo->clk_id);
    toval_temp  = p_devinfo->wdt_time * clk / 256000;

    /* 关闭中断 */
    AW_INT_CPU_LOCK(key);   // 不能被打断的临界代码(执行应尽可能地短)

    /* rtwdt unlock */
    p_reg->CNT = RTWDT_UPDATE_KEY;

    /* 配置寄存器  */
    p_reg->TOVAL = toval_temp;
    p_reg->CS    = cs_temp;

    AW_INT_CPU_UNLOCK(key);

    /* 第一次喂狗 */
    p_reg->CNT = RTWDT_FEED_KEY;

}

/*******************************************************************************
  locals
*******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_imx10xx_rtwdt = {
    __imx10xx_rtwdt_inst_init, /* devInstanceInit */
    NULL,                    /* devInstanceInit2 */
    NULL                     /* devConnect */
};

/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_imx10xx_rtwdt = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    AWBL_IMX10XX_RTWDT_NAME,            /* p_drvname */
    &__g_awbl_drvfuncs_imx10xx_rtwdt,   /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

/******************************************************************************/
void awbl_imx10xx_rtwdt_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_imx10xx_rtwdt);
}

/* 看门狗参数设置 */
void awbl_imx10xx_rtwdt_parm_set (struct awbl_dev *p_dev,
                                  uint16_t         options,
                                  uint32_t         wdt_time_down,
                                  uint32_t         wdt_time_up)
{
    __IMX10xx_RTWDT_DEVINFO_DECL(p_devinfo, p_dev);
    __imx10xx_rtwdt_regs_t         *p_reg       = (__imx10xx_rtwdt_regs_t *)(p_devinfo->reg_base);
    struct awbl_imx10xx_rtwdt_dev  *p_rtwdt_dev = (struct awbl_imx10xx_rtwdt_dev *)p_dev;
    uint32_t                        cs_temp     = 0;
    uint32_t                        toval_temp  = 0;
    uint32_t                        winval_temp = 0;
    uint32_t                        clk;

    p_rtwdt_dev->wdt_time_down = wdt_time_down;
    p_rtwdt_dev->wdt_time_up = wdt_time_up;

    AW_INT_CPU_LOCK_DECL(key);

    clk         = aw_clk_rate_get(p_devinfo->clk_id);
    if (((options) & (AW_RTWDOG_PRESCALER_ENABLE))) {/* 分频*/
        toval_temp  = wdt_time_up * clk / 256000;
        winval_temp = wdt_time_down * clk /256000;
    } else {/* 不分频 */
        toval_temp  = wdt_time_up * clk / 1000;
        winval_temp = wdt_time_down * clk /1000;
    }

    /* 清除看门狗窗口模式，中断，分频标志位 */
    cs_temp = (p_reg->CS & (~(AW_RTWDOG_WINDOW_MODE_OPEN |
                              AW_RTWDOG_INTERRUPT_ENABLE |
                              AW_RTWDOG_PRESCALER_ENABLE)));
    /* 设置看门狗窗口模式，中断，分频标志位 */
    cs_temp |= options;

    /* 关闭中断 */
    AW_INT_CPU_LOCK(key);   // 不能被打断的临界代码(执行应尽可能地短)

    /* rtwdt unlock */
    p_reg->CNT = RTWDT_UPDATE_KEY;

    /* 配置寄存器  */
    p_reg->CS    = cs_temp;
    p_reg->TOVAL = toval_temp;
    p_reg->WIN   = winval_temp;

    /* 等待重新配置生效 */
    while(((p_reg->CS) & (AW_RTWDOG_RECONFIG_SUCESS)) == 0);
    AW_INT_CPU_UNLOCK(key);
}

/* 看门狗窗口上限、下限值获取 */
void awbl_imx10xx_rtwdt_parm_get (struct awbl_dev *p_dev,
                                  uint32_t        *wdt_time_down,
                                  uint32_t        *wdt_time_up)
{
    struct awbl_imx10xx_rtwdt_dev  *p_rtwdt_dev = (struct awbl_imx10xx_rtwdt_dev *)p_dev;
    *wdt_time_down = p_rtwdt_dev->wdt_time_down;
    *wdt_time_up   = p_rtwdt_dev->wdt_time_up;
}

/* 看门狗CNT寄存器值获取 */
void awbl_imx10xx_rtwdt_cnt_val_get (struct awbl_dev *p_dev,
                                     uint32_t        *cnt_val)
{
    __IMX10xx_RTWDT_DEVINFO_DECL(p_devinfo, p_dev);
    __imx10xx_rtwdt_regs_t         *p_reg       = (__imx10xx_rtwdt_regs_t *)(p_devinfo->reg_base);
    uint32_t                        clk;

    clk         = aw_clk_rate_get(p_devinfo->clk_id);
    if (p_reg->CS & AW_RTWDOG_PRESCALER_ENABLE) { /* 分频 */
        *cnt_val = p_reg->CNT * 256000 / clk;
        return;
    }
    *cnt_val = p_reg->CNT * 1000 / clk;
}



/* end of file */

