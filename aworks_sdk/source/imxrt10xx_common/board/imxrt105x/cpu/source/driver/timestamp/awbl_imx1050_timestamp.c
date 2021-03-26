/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT1052 时间戳
 *
 * 本驱动使用cyccnt实现时间戳
 *
 * \internal
 * \par modification history
 * - 1.00 20-04-04  wk, first implementation
 * \endinternal
 */

/*****************************************************************************/

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_common.h"
#include "aw_clk.h"
#include "aw_timer.h"
#include "awbl_timestamp.h"
#include "driver/timestamp/awbl_imx1050_timestamp.h"

#define __TIMESTAMP_DEV_DECL(p_this, p_dev)   \
          struct awbl_imx1050_timestamp_dev *p_this = \
          (struct awbl_imx1050_timestamp_dev *)p_dev

#define __TIMESTAMP_DEVINFO_DECL(p_devinfo, p_dev)   \
          struct awbl_imx1050_timestamp_devinfo *p_devinfo = \
          (struct awbl_imx1050_timestamp_devinfo *)AWBL_DEVINFO_GET(p_dev)


#define __REG_MODIFY32_SET(addr, value) \
        do { \
            uint32_t val = readl((volatile void __iomem *)(addr)); \
            val |= value; \
            writel(val, (volatile void __iomem *)(addr)); \
        } while (0)

#define __REG_MODIFY32_CLR(addr, value) \
        do { \
            uint32_t val = readl((volatile void __iomem *)(addr)); \
            val &= value; \
            writel(val, (volatile void __iomem *)(addr)); \
        } while (0)

/*****************************************************************************/

#define DHCSR_ADDR      0xE000EDF0
#define DWT_CTRL_ADDR   0xE0001000
#define DWT_CYCCNT_ADDR 0xE0001004

/* timestamp */
struct awbl_timestamp __g_imx1050_timestamp = {NULL, 0};

static struct {
    uint32_t    prev_count;         /**< \brief CPU周期记录器. */

    uint64_t    microsecond;        /**< \brief 微秒计数器. */

    uint32_t    sys_freq;           /**< \brief 系统频率. */
} __g_timestamp_record ;

/* \brief 更新系统的时间戳
 *
 * \note  请确保在小于半个溢出周期的间隔内进行更新（调用）
 */
aw_local aw_timestamp_t __timestamp_update_count ()
{
    uint32_t cur_count, diff;
//    aw_timestamp_t timestamp = 0;
    RTK_CRITICAL_STATUS_DECL(old);

    RTK_ENTER_CRITICAL(old);

    /* 获取CPU周期计数，并与上一次的值对比 */
    cur_count = readl(DWT_CYCCNT_ADDR);
    diff = cur_count - __g_timestamp_record.prev_count;

    /* 对溢出进行处理，并除以CPU频率（单位：M）以获取微秒级别的时间差 */
    diff += (diff>>31) * (uint32_t)(-1);
    diff = (diff / __g_timestamp_record.sys_freq);

    /* 更新CPU周期记录， */
    __g_timestamp_record.prev_count  += diff * __g_timestamp_record.sys_freq;
    __g_timestamp_record.microsecond += diff;

    RTK_EXIT_CRITICAL(old);

    return __g_timestamp_record.microsecond ;
}

aw_local
aw_timestamp_t  __timestamp_get(struct awbl_timestamp *p_awbl_timestamp)
{
    return __timestamp_update_count();
}

aw_local void __timestamp_update (void *arg)
{
    __TIMESTAMP_DEV_DECL(p_this, arg);
    __TIMESTAMP_DEVINFO_DECL(p_devinfo, p_this);

    __timestamp_update_count();
    aw_timer_start(&p_this->p_timer,
                   aw_ms_to_ticks(p_devinfo->update_period));

    return ;
}



aw_local void __timestamp_inst_init (awbl_dev_t *p_dev)
{
    return;
}

aw_local void __timestamp_inst_init2 (awbl_dev_t *p_dev)
{
    __TIMESTAMP_DEV_DECL(p_this, p_dev);
    __TIMESTAMP_DEVINFO_DECL(p_devinfo, p_this);

    p_devinfo->pfn_plfm_init();

    __g_imx1050_timestamp.fn_timestamp_get = __timestamp_get;

    if (p_devinfo->freq < 1) {
        __g_imx1050_timestamp.freq = 1;
    } else if (p_devinfo->freq > MAX_TIMESTAMP_FREQUENCY) {
        __g_imx1050_timestamp.freq = MAX_TIMESTAMP_FREQUENCY;
    } else {
        __g_imx1050_timestamp.freq = p_devinfo->freq;
    }
    
    /* 初始化时间戳记录变量 */
    __g_timestamp_record.sys_freq    = aw_clk_rate_get(IMX1050_CLK_ARM_PODF) / __g_imx1050_timestamp.freq;
    __g_timestamp_record.prev_count  = 0;
    __g_timestamp_record.microsecond = 0;

    /* 使能CPU周期计数器 */
    __REG_MODIFY32_SET(DHCSR_ADDR, 1<<24);
    __REG_MODIFY32_SET(DWT_CTRL_ADDR, 1<<0);
    writel(0, DWT_CYCCNT_ADDR);
    return;
}

aw_local void __timestamp_inst_connect(awbl_dev_t *p_dev)
{
    __TIMESTAMP_DEV_DECL(p_this, p_dev);
    __TIMESTAMP_DEVINFO_DECL(p_devinfo, p_this);

    awbl_timestamp_dev_register(&__g_imx1050_timestamp);

    aw_timer_init (&p_this->p_timer, __timestamp_update, (void *)p_this);

    /* 启动软件定时器，用于更新时间戳 */
    aw_timer_start(&p_this->p_timer,
                   aw_ms_to_ticks(p_devinfo->update_period));

    return;
}

aw_const struct awbl_drvfuncs __g_timestamp_awbl_drvfuncs = {
    __timestamp_inst_init,        /* devInstanceInit */
    __timestamp_inst_init2,       /* devInstanceInit2 */
    __timestamp_inst_connect      /* devConnect */
};

/**
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_timestamp_drv_registration = {
    AWBL_VER_1,                                 /* awb_ver */
    AWBL_BUSID_PLB,                             /* bus_id */
    AWBL_IMX1050_TIMESTAMP_NAME,                /* p_drvname*/
    &__g_timestamp_awbl_drvfuncs,               /* p_busfuncs */
    NULL,                                       /* p_methods */
    NULL                                        /* pfunc_drv_probe */
};


void awbl_imx1050_timestamp_drv_register (void)
{
    awbl_drv_register(&__g_timestamp_drv_registration);
}

