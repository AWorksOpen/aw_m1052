/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief the definition of consts and struct of clock controler module in imx1010
 *
 * \par 1.兼容设备
 *
 *  - IMX1010
 *
 * \internal
 * \par modification history
 * - 1.00 2020-04-29  cml, first implementation
 * \endinternal
 */

#ifndef __IMX1010_PMU_REGS_H__
#define __IMX1010_PMU_REGS_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \addtogroup grp_imx1010_bsp IMX1010 BSP支持
 */


/**
 * \addtogroup grp_imx1010_pmu_regs DCDC模块寄存器
 * @{
 */

typedef struct {
    uint8_t RESERVED_0[272];
    uint32_t REG_1P1;                   /**< Regulator 1P1 Register, offset: 0x110 */
    uint32_t REG_1P1_SET;               /**< Regulator 1P1 Register, offset: 0x114 */
    uint32_t REG_1P1_CLR;               /**< Regulator 1P1 Register, offset: 0x118 */
    uint32_t REG_1P1_TOG;               /**< Regulator 1P1 Register, offset: 0x11C */
    uint32_t REG_3P0;                   /**< Regulator 3P0 Register, offset: 0x120 */
    uint32_t REG_3P0_SET;               /**< Regulator 3P0 Register, offset: 0x124 */
    uint32_t REG_3P0_CLR;               /**< Regulator 3P0 Register, offset: 0x128 */
    uint32_t REG_3P0_TOG;               /**< Regulator 3P0 Register, offset: 0x12C */
    uint32_t REG_2P5;                   /**< Regulator 2P5 Register, offset: 0x130 */
    uint32_t REG_2P5_SET;               /**< Regulator 2P5 Register, offset: 0x134 */
    uint32_t REG_2P5_CLR;               /**< Regulator 2P5 Register, offset: 0x138 */
    uint32_t REG_2P5_TOG;               /**< Regulator 2P5 Register, offset: 0x13C */
    uint32_t REG_CORE;                  /**< Digital Regulator Core Register, offset: 0x140 */
    uint32_t REG_CORE_SET;              /**< Digital Regulator Core Register, offset: 0x144 */
    uint32_t REG_CORE_CLR;              /**< Digital Regulator Core Register, offset: 0x148 */
    uint32_t REG_CORE_TOG;              /**< Digital Regulator Core Register, offset: 0x14C */
    uint32_t MISC0;                     /**< Miscellaneous Register 0, offset: 0x150 */
    uint32_t MISC0_SET;                 /**< Miscellaneous Register 0, offset: 0x154 */
    uint32_t MISC0_CLR;                 /**< Miscellaneous Register 0, offset: 0x158 */
    uint32_t MISC0_TOG;                 /**< Miscellaneous Register 0, offset: 0x15C */
    uint32_t MISC1;                     /**< Miscellaneous Register 1, offset: 0x160 */
    uint32_t MISC1_SET;                 /**< Miscellaneous Register 1, offset: 0x164 */
    uint32_t MISC1_CLR;                 /**< Miscellaneous Register 1, offset: 0x168 */
    uint32_t MISC1_TOG;                 /**< Miscellaneous Register 1, offset: 0x16C */
    uint32_t MISC2;                     /**< Miscellaneous Control Register, offset: 0x170 */
    uint32_t MISC2_SET;                 /**< Miscellaneous Control Register, offset: 0x174 */
    uint32_t MISC2_CLR;                 /**< Miscellaneous Control Register, offset: 0x178 */
    uint32_t MISC2_TOG;                 /**< Miscellaneous Control Register, offset: 0x17C */
}imx1010_pmu_regs_t;

/** @} end of grp_imx1010_pmu_regs */

/** @} end of grp_imx1010_bsp */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __IMX1010_PMU_REGS_H__ */

/* end of file */
