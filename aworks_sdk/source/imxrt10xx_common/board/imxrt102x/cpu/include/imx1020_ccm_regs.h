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
 * \brief the definition of consts and struct of clock controler module in imx1020
 *
 * \par 1.兼容设备
 *
 *  - imx1020
 *
 * \internal
 * \par modification history
 * - 1.00 2018-09-07  mex, first implementation
 * \endinternal
 */

#ifndef __IMX1020_CCM_REGS_H
#define __IMX1020_CCM_REGS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus*/

/**
 * \addtogroup grp_imx1020_bsp IMX1020 BSP支持
 */


/**
 * \addtogroup grp_imx1020_ccm_regs 时钟控制模块寄存器描述
 * @{
 */

typedef struct {
    volatile uint32_t   CCR;            /**< CCM Control Register, offset: 0x0 */
    uint8_t             RESERVED_0[4];
    volatile uint32_t   CSR;            /**< CCM Status Register, offset: 0x8 */
    volatile uint32_t   CCSR;           /**< CCM Clock Switcher Register, offset: 0xC */
    volatile uint32_t   CACRR;          /**< CCM Arm Clock Root Register, offset: 0x10 */
    volatile uint32_t   CBCDR;          /**< CCM Bus Clock Divider Register, offset: 0x14 */
    volatile uint32_t   CBCMR;          /**< CCM Bus Clock Multiplexer Register, offset: 0x18 */
    volatile uint32_t   CSCMR1;         /**< CCM Serial Clock Multiplexer Register 1, offset: 0x1C */
    volatile uint32_t   CSCMR2;         /**< CCM Serial Clock Multiplexer Register 2, offset: 0x20 */
    volatile uint32_t   CSCDR1;         /**< CCM Serial Clock Divider Register 1, offset: 0x24 */
    volatile uint32_t   CS1CDR;         /**< CCM Clock Divider Register, offset: 0x28 */
    volatile uint32_t   CS2CDR;         /**< CCM Clock Divider Register, offset: 0x2C */
    volatile uint32_t   CDCDR;          /**< CCM D1 Clock Divider Register, offset: 0x30 */
    uint8_t             RESERVED_1[4];
    volatile uint32_t   CSCDR2;         /**< CCM Serial Clock Divider Register 2, offset: 0x38 */
    volatile uint32_t   CSCDR3;         /**< CCM Serial Clock Divider Register 3, offset: 0x3C */
    uint8_t             RESERVED_2[8];
    volatile  uint32_t  CDHIPR;         /**< CCM Divider Handshake In-Process Register, offset: 0x48 */
    uint8_t             RESERVED_3[8];
    volatile uint32_t   CLPCR;          /**< CCM Low Power Control Register, offset: 0x54 */
    volatile uint32_t   CISR;           /**< CCM Interrupt Status Register, offset: 0x58 */
    volatile uint32_t   CIMR;           /**< CCM Interrupt Mask Register, offset: 0x5C */
    volatile uint32_t   CCOSR;          /**< CCM Clock Output Source Register, offset: 0x60 */
    volatile uint32_t   CGPR;           /**< CCM General Purpose Register, offset: 0x64 */
    volatile uint32_t   CCGR0;          /**< CCM Clock Gating Register 0, offset: 0x68 */
    volatile uint32_t   CCGR1;          /**< CCM Clock Gating Register 1, offset: 0x6C */
    volatile uint32_t   CCGR2;          /**< CCM Clock Gating Register 2, offset: 0x70 */
    volatile uint32_t   CCGR3;          /**< CCM Clock Gating Register 3, offset: 0x74 */
    volatile uint32_t   CCGR4;          /**< CCM Clock Gating Register 4, offset: 0x78 */
    volatile uint32_t   CCGR5;          /**< CCM Clock Gating Register 5, offset: 0x7C */
    volatile uint32_t   CCGR6;          /**< CCM Clock Gating Register 6, offset: 0x80 */
    uint8_t             RESERVED_4[4];
    volatile uint32_t   CMEOR;          /**< CCM Module Enable Overide Register, offset: 0x88 */
} imx1020_ccm_regs_t;

typedef struct
{
    uint8_t RESERVED_0[16];
    volatile uint32_t PLL_USB1;                          /**< Analog USB1 480MHz PLL Control Register, offset: 0x10 */
    volatile uint32_t PLL_USB1_SET;                      /**< Analog USB1 480MHz PLL Control Register, offset: 0x14 */
    volatile uint32_t PLL_USB1_CLR;                      /**< Analog USB1 480MHz PLL Control Register, offset: 0x18 */
    volatile uint32_t PLL_USB1_TOG;                      /**< Analog USB1 480MHz PLL Control Register, offset: 0x1C */
    uint8_t RESERVED_1[16];
    volatile uint32_t PLL_SYS;                           /**< Analog System PLL Control Register, offset: 0x30 */
    volatile uint32_t PLL_SYS_SET;                       /**< Analog System PLL Control Register, offset: 0x34 */
    volatile uint32_t PLL_SYS_CLR;                       /**< Analog System PLL Control Register, offset: 0x38 */
    volatile uint32_t PLL_SYS_TOG;                       /**< Analog System PLL Control Register, offset: 0x3C */
    volatile uint32_t PLL_SYS_SS;                        /**< 528MHz System PLL Spread Spectrum Register, offset: 0x40 */
    uint8_t RESERVED_2[12];
    volatile uint32_t PLL_SYS_NUM;                       /**< Numerator of 528MHz System PLL Fractional Loop Divider Register, offset: 0x50 */
    uint8_t RESERVED_3[12];
    volatile uint32_t PLL_SYS_DENOM;                     /**< Denominator of 528MHz System PLL Fractional Loop Divider Register, offset: 0x60 */
    uint8_t RESERVED_4[12];
    volatile uint32_t PLL_AUDIO;                         /**< Analog Audio PLL control Register, offset: 0x70 */
    volatile uint32_t PLL_AUDIO_SET;                     /**< Analog Audio PLL control Register, offset: 0x74 */
    volatile uint32_t PLL_AUDIO_CLR;                     /**< Analog Audio PLL control Register, offset: 0x78 */
    volatile uint32_t PLL_AUDIO_TOG;                     /**< Analog Audio PLL control Register, offset: 0x7C */
    volatile uint32_t PLL_AUDIO_NUM;                     /**< Numerator of Audio PLL Fractional Loop Divider Register, offset: 0x80 */
    uint8_t RESERVED_5[12];
    volatile uint32_t PLL_AUDIO_DENOM;                   /**< Denominator of Audio PLL Fractional Loop Divider Register, offset: 0x90 */
    uint8_t RESERVED_6[76];
    volatile uint32_t PLL_ENET;                          /**< Analog ENET PLL Control Register, offset: 0xE0 */
    volatile uint32_t PLL_ENET_SET;                      /**< Analog ENET PLL Control Register, offset: 0xE4 */
    volatile uint32_t PLL_ENET_CLR;                      /**< Analog ENET PLL Control Register, offset: 0xE8 */
    volatile uint32_t PLL_ENET_TOG;                      /**< Analog ENET PLL Control Register, offset: 0xEC */
    volatile uint32_t PFD_480;                           /**< 480MHz Clock (PLL3) Phase Fractional Divider Control Register, offset: 0xF0 */
    volatile uint32_t PFD_480_SET;                       /**< 480MHz Clock (PLL3) Phase Fractional Divider Control Register, offset: 0xF4 */
    volatile uint32_t PFD_480_CLR;                       /**< 480MHz Clock (PLL3) Phase Fractional Divider Control Register, offset: 0xF8 */
    volatile uint32_t PFD_480_TOG;                       /**< 480MHz Clock (PLL3) Phase Fractional Divider Control Register, offset: 0xFC */
    volatile uint32_t PFD_528;                           /**< 528MHz Clock (PLL2) Phase Fractional Divider Control Register, offset: 0x100 */
    volatile uint32_t PFD_528_SET;                       /**< 528MHz Clock (PLL2) Phase Fractional Divider Control Register, offset: 0x104 */
    volatile uint32_t PFD_528_CLR;                       /**< 528MHz Clock (PLL2) Phase Fractional Divider Control Register, offset: 0x108 */
    volatile uint32_t PFD_528_TOG;                       /**< 528MHz Clock (PLL2) Phase Fractional Divider Control Register, offset: 0x10C */
    uint8_t RESERVED_7[64];
    volatile uint32_t MISC0;                             /**< Miscellaneous Register 0, offset: 0x150 */
    volatile uint32_t MISC0_SET;                         /**< Miscellaneous Register 0, offset: 0x154 */
    volatile uint32_t MISC0_CLR;                         /**< Miscellaneous Register 0, offset: 0x158 */
    volatile uint32_t MISC0_TOG;                         /**< Miscellaneous Register 0, offset: 0x15C */
    volatile uint32_t MISC1;                             /**< Miscellaneous Register 1, offset: 0x160 */
    volatile uint32_t MISC1_SET;                         /**< Miscellaneous Register 1, offset: 0x164 */
    volatile uint32_t MISC1_CLR;                         /**< Miscellaneous Register 1, offset: 0x168 */
    volatile uint32_t MISC1_TOG;                         /**< Miscellaneous Register 1, offset: 0x16C */
    volatile uint32_t MISC2;                             /**< Miscellaneous Register 2, offset: 0x170 */
    volatile uint32_t MISC2_SET;                         /**< Miscellaneous Register 2, offset: 0x174 */
    volatile uint32_t MISC2_CLR;                         /**< Miscellaneous Register 2, offset: 0x178 */
    volatile uint32_t MISC2_TOG;                         /**< Miscellaneous Register 2, offset: 0x17C */
} imx1020_ccm_analog_regs_t;

/** @} end of grp_imx1020_ccm_regs */

/** @} end of grp_imx1020_bsp */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX1020_CCM_REGS_H */

/* end of file */
