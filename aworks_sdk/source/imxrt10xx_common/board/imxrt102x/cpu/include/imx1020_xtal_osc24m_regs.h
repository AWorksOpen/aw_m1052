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
 *  - IMX1020
 *
 * \internal
 * \par modification history
 * - 1.00 2020-04-28  cml, first implementation
 * \endinternal
 */

#ifndef __IMX1020_XTAL_OSC24M_REGS_H__
#define __IMX1020_XTAL_OSC24M_REGS_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus*/

/**
 * \addtogroup grp_imx1020_bsp IMX1020 BSP支持
 */


/**
 * \addtogroup grp_imx1020_xtal_osc24_regs 晶振模块寄存器
 * @{
 */

typedef struct {
    uint8_t     RESERVED_0[336];
    uint32_t    MISC0;              /**< Miscellaneous Register 0, offset: 0x150 */
    uint32_t    MISC0_SET;          /**< Miscellaneous Register 0, offset: 0x154 */
    uint32_t    MISC0_CLR;          /**< Miscellaneous Register 0, offset: 0x158 */
    uint32_t    MISC0_TOG;          /**< Miscellaneous Register 0, offset: 0x15C */
    uint8_t     RESERVED_1[272];
    uint32_t    LOWPWR_CTRL;        /**< XTAL OSC (LP) Control Register, offset: 0x270 */
    uint32_t    LOWPWR_CTRL_SET;    /**< XTAL OSC (LP) Control Register, offset: 0x274 */
    uint32_t    LOWPWR_CTRL_CLR;    /**< XTAL OSC (LP) Control Register, offset: 0x278 */
    uint32_t    LOWPWR_CTRL_TOG;    /**< XTAL OSC (LP) Control Register, offset: 0x27C */
    uint8_t     RESERVED_2[32];
    uint32_t    OSC_CONFIG0;        /**< XTAL OSC Configuration 0 Register, offset: 0x2A0 */
    uint32_t    OSC_CONFIG0_SET;    /**< XTAL OSC Configuration 0 Register, offset: 0x2A4 */
    uint32_t    OSC_CONFIG0_CLR;    /**< XTAL OSC Configuration 0 Register, offset: 0x2A8 */
    uint32_t    OSC_CONFIG0_TOG;    /**< XTAL OSC Configuration 0 Register, offset: 0x2AC */
    uint32_t    OSC_CONFIG1;        /**< XTAL OSC Configuration 1 Register, offset: 0x2B0 */
    uint32_t    OSC_CONFIG1_SET;    /**< XTAL OSC Configuration 1 Register, offset: 0x2B4 */
    uint32_t    OSC_CONFIG1_CLR;    /**< XTAL OSC Configuration 1 Register, offset: 0x2B8 */
    uint32_t    OSC_CONFIG1_TOG;    /**< XTAL OSC Configuration 1 Register, offset: 0x2BC */
    uint32_t    OSC_CONFIG2;        /**< XTAL OSC Configuration 2 Register, offset: 0x2C0 */
    uint32_t    OSC_CONFIG2_SET;    /**< XTAL OSC Configuration 2 Register, offset: 0x2C4 */
    uint32_t    OSC_CONFIG2_CLR;    /**< XTAL OSC Configuration 2 Register, offset: 0x2C8 */
    uint32_t    OSC_CONFIG2_TOG;    /**< XTAL OSC Configuration 2 Register, offset: 0x2CC */
}imx1020_xtal_osc24m_regs_t;

typedef struct
{
    uint32_t CTRL;                  /*!< Offset: 0x000 (R/W)  Control Register */
    uint32_t CYCCNT;                /*!< Offset: 0x004 (R/W)  Cycle Count Register */
    uint32_t CPICNT;                /*!< Offset: 0x008 (R/W)  CPI Count Register */
    uint32_t EXCCNT;                /*!< Offset: 0x00C (R/W)  Exception Overhead Count Register */
    uint32_t SLEEPCNT;              /*!< Offset: 0x010 (R/W)  Sleep Count Register */
    uint32_t LSUCNT;                /*!< Offset: 0x014 (R/W)  LSU Count Register */
    uint32_t FOLDCNT;               /*!< Offset: 0x018 (R/W)  Folded-instruction Count Register */
    uint32_t PCSR;                  /*!< Offset: 0x01C (R/ )  Program Counter Sample Register */
    uint32_t COMP0;                 /*!< Offset: 0x020 (R/W)  Comparator Register 0 */
    uint32_t MASK0;                 /*!< Offset: 0x024 (R/W)  Mask Register 0 */
    uint32_t FUNCTION0;             /*!< Offset: 0x028 (R/W)  Function Register 0 */
    uint32_t RESERVED0[1U];
    uint32_t COMP1;                 /*!< Offset: 0x030 (R/W)  Comparator Register 1 */
    uint32_t MASK1;                 /*!< Offset: 0x034 (R/W)  Mask Register 1 */
    uint32_t FUNCTION1;             /*!< Offset: 0x038 (R/W)  Function Register 1 */
    uint32_t RESERVED1[1U];
    uint32_t COMP2;                 /*!< Offset: 0x040 (R/W)  Comparator Register 2 */
    uint32_t MASK2;                 /*!< Offset: 0x044 (R/W)  Mask Register 2 */
    uint32_t FUNCTION2;             /*!< Offset: 0x048 (R/W)  Function Register 2 */
    uint32_t RESERVED2[1U];
    uint32_t COMP3;                 /*!< Offset: 0x050 (R/W)  Comparator Register 3 */
    uint32_t MASK3;                 /*!< Offset: 0x054 (R/W)  Mask Register 3 */
    uint32_t FUNCTION3;             /*!< Offset: 0x058 (R/W)  Function Register 3 */
    uint32_t RESERVED3[981U];
    uint32_t LAR;                   /*!< Offset: 0xFB0 (  W)  Lock Access Register */
    uint32_t LSR;                   /*!< Offset: 0xFB4 (R  )  Lock Status Register */
} armv7m_dwt_regs_t;

typedef struct
{
    uint32_t DHCSR;                 /*!< Offset: 0x000 (R/W)  Debug Halting Control and Status Register */
    uint32_t DCRSR;                 /*!< Offset: 0x004 ( /W)  Debug Core Register Selector Register */
    uint32_t DCRDR;                 /*!< Offset: 0x008 (R/W)  Debug Core Register Data Register */
    uint32_t DEMCR;                 /*!< Offset: 0x00C (R/W)  Debug Exception and Monitor Control Register */
} armv7m_core_debug_regs_t;


/** @} end of grp_imx1020_xtal_osc24_regs */

/** @} end of grp_imx1020_bsp */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX1020_XTAL_OSC24M_REGS_H__ */

/* end of file */
