/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Stock Co.,  Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 
 *
 * \internal
 * \par Modification history
 * - 1.00 20-09-18  win, first implementation.
 * \endinternal
 */

#ifndef __AWBL_WM8960_H
#define __AWBL_WM8960_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_i2c.h"
#include "awbl_i2cbus.h"

#include "soc/awsa_soc_internal.h"

/**
 * \brief 驱动名称
 */
#define AWBL_WM8960_NAME "awbl_wm8960_codec"

#define AWBL_WM8960_REG_LINVOL            0x00
#define AWBL_WM8960_REG_RINVOL            0x01
#define AWBL_WM8960_REG_LOUT1             0x02
#define AWBL_WM8960_REG_ROUT1             0x03
#define AWBL_WM8960_REG_CLOCK1            0x04
#define AWBL_WM8960_REG_DACCTL1           0x05
#define AWBL_WM8960_REG_DACCTL2           0x06
#define AWBL_WM8960_REG_IFACE1            0x07
#define AWBL_WM8960_REG_CLOCK2            0x08
#define AWBL_WM8960_REG_IFACE2            0x09
#define AWBL_WM8960_REG_LDAC              0x0a
#define AWBL_WM8960_REG_RDAC              0x0b

#define AWBL_WM8960_REG_RESET             0x0f
#define AWBL_WM8960_REG_3D                0x10
#define AWBL_WM8960_REG_ALC1              0x11
#define AWBL_WM8960_REG_ALC2              0x12
#define AWBL_WM8960_REG_ALC3              0x13
#define AWBL_WM8960_REG_NOISEG            0x14
#define AWBL_WM8960_REG_LADC              0x15
#define AWBL_WM8960_REG_RADC              0x16
#define AWBL_WM8960_REG_ADDCTL1           0x17
#define AWBL_WM8960_REG_ADDCTL2           0x18
#define AWBL_WM8960_REG_POWER1            0x19
#define AWBL_WM8960_REG_POWER2            0x1a
#define AWBL_WM8960_REG_ADDCTL3           0x1b
#define AWBL_WM8960_REG_APOP1             0x1c
#define AWBL_WM8960_REG_APOP2             0x1d

#define AWBL_WM8960_REG_LINPATH           0x20
#define AWBL_WM8960_REG_RINPATH           0x21
#define AWBL_WM8960_REG_LOUTMIX           0x22

#define AWBL_WM8960_REG_ROUTMIX           0x25
#define AWBL_WM8960_REG_MONOMIX1          0x26
#define AWBL_WM8960_REG_MONOMIX2          0x27
#define AWBL_WM8960_REG_LOUT2             0x28
#define AWBL_WM8960_REG_ROUT2             0x29
#define AWBL_WM8960_REG_MONO              0x2a
#define AWBL_WM8960_REG_INBMIX1           0x2b
#define AWBL_WM8960_REG_INBMIX2           0x2c
#define AWBL_WM8960_REG_BYPASS1           0x2d
#define AWBL_WM8960_REG_BYPASS2           0x2e
#define AWBL_WM8960_REG_POWER3            0x2f
#define AWBL_WM8960_REG_ADDCTL4           0x30
#define AWBL_WM8960_REG_CLASSD1           0x31

#define AWBL_WM8960_REG_CLASSD3           0x33
#define AWBL_WM8960_REG_PLL1              0x34
#define AWBL_WM8960_REG_PLL2              0x35
#define AWBL_WM8960_REG_PLL3              0x36
#define AWBL_WM8960_REG_PLL4              0x37

/**
 * \brief 耳机输出的放电电阻
 */
enum awbl_wm8960_dres {
    AWBL_WM8960_DRES_400R = 0,
    AWBL_WM8960_DRES_200R,
    AWBL_WM8960_DRES_600R,
    AWBL_WM8960_DRES_150R,

    AWBL_WM8960_DRES_MAX = AWBL_WM8960_DRES_150R,
};

/**
 * \brief WM8960 Clock dividers
 */
enum awbl_wm8960_clk_div {
    AWBL_WM8960_CLK_SYSCLKDIV,
    AWBL_WM8960_CLK_DACDIV,
    AWBL_WM8960_CLK_OPCLKDIV,
    AWBL_WM8960_CLK_DCLKDIV,
    AWBL_WM8960_CLK_TOCLKSEL,
};

#define AWBL_WM8960_SYSCLK_DIV_1          (0 << 1)
#define AWBL_WM8960_SYSCLK_DIV_2          (2 << 1)

#define AWBL_WM8960_SYSCLK_MCLK           (0 << 0)
#define AWBL_WM8960_SYSCLK_PLL            (1 << 0)

#define AWBL_WM8960_DAC_DIV_1             (0 << 3)
#define AWBL_WM8960_DAC_DIV_1_5           (1 << 3)
#define AWBL_WM8960_DAC_DIV_2             (2 << 3)
#define AWBL_WM8960_DAC_DIV_3             (3 << 3)
#define AWBL_WM8960_DAC_DIV_4             (4 << 3)
#define AWBL_WM8960_DAC_DIV_5_5           (5 << 3)
#define AWBL_WM8960_DAC_DIV_6             (6 << 3)

#define AWBL_WM8960_DCLK_DIV_1_5          (0 << 6)
#define AWBL_WM8960_DCLK_DIV_2            (1 << 6)
#define AWBL_WM8960_DCLK_DIV_3            (2 << 6)
#define AWBL_WM8960_DCLK_DIV_4            (3 << 6)
#define AWBL_WM8960_DCLK_DIV_6            (4 << 6)
#define AWBL_WM8960_DCLK_DIV_8            (5 << 6)
#define AWBL_WM8960_DCLK_DIV_12           (6 << 6)
#define AWBL_WM8960_DCLK_DIV_16           (7 << 6)

#define AWBL_WM8960_TOCLK_F19             (0 << 1)
#define AWBL_WM8960_TOCLK_F21             (1 << 1)

#define AWBL_WM8960_OPCLK_DIV_1           (0 << 0)
#define AWBL_WM8960_OPCLK_DIV_2           (1 << 0)
#define AWBL_WM8960_OPCLK_DIV_3           (2 << 0)
#define AWBL_WM8960_OPCLK_DIV_4           (3 << 0)
#define AWBL_WM8960_OPCLK_DIV_5_5         (4 << 0)
#define AWBL_WM8960_OPCLK_DIV_6           (5 << 0)

/**
 * \brief PLL divisors
 */
struct awbl_wm8960_pll_div {
    uint32_t pre_div;
    uint32_t n;
    uint32_t k;
};

/**
 * \brief wm8960设备信息
 */
struct awbl_wm8960_devinfo {
    const char            *p_codec_name;   /**< \brief 名称 */
    uint8_t                addr;           /**< \brief i2c从机地址(0x34) */
    const char            *p_dais_name[1]; /**< \brief 是播放+录音 */
    aw_bool_t              capless;        /**< \brief 耳机输出为无电容模式 */
    enum awbl_wm8960_dres  dres;           /**< \brief 耳机输出的放电电阻 */
};

/**
 * \brief wm8960设备
 */
struct awbl_wm8960_dev {

    /** \brief 继承自 AWBus I2C device 设备 */
    struct awbl_i2c_device       super;

    /** \biref codec 设备 */
    struct awsa_soc_codec        codec;

    /** \brief ctl element */
    struct awsa_core_ctl_elem    ctl_elem[34];

    /** \brief widget */
    struct awsa_soc_dapm_widget  widget[30];

    /** \brief ctl element */
    struct awsa_core_ctl_elem    widget_ctl_elem[14];

    /** \brief path */
    struct awsa_soc_dapm_path    path[34];

    /** \brief widget capless */
    struct awsa_soc_dapm_widget  widget_capless;

    /** \brief path capless */
    struct awsa_soc_dapm_path    path_capless[4];

    /** \brief ctl element */
    struct awsa_core_ctl_elem    out_ctl_elem[2];

    /** \brief dai 设备 */
    struct awsa_soc_dai          dai[1];

    uint32_t                     cache_dirty;    /**< \brief 寄存器修改标志 */
    aw_spinlock_isr_t            lock;           /**< \brief 读写锁 */
    uint16_t                     reg_cache[56];  /**< \brief 寄存器缓存 */

    struct awsa_soc_dapm_widget *p_lout1;        /**< \brief LOUT1 */
    struct awsa_soc_dapm_widget *p_rout1;        /**< \brief ROUT1 */
    struct awsa_soc_dapm_widget *p_out3;         /**< \brief OUT3 */

    struct awbl_wm8960_pll_div   pll_div;        /**< \brief pll */

    uint32_t                     rate;           /**< \brief rate */
    aw_bool_t                    deemph;         /**< \brief deemph */

    aw_bool_t                    probed;         /**< \brief probe标志 */
};

/**
 * \brief 驱动注册
 */
void awbl_wm8960_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_WM8960_H */

/* end of file */
