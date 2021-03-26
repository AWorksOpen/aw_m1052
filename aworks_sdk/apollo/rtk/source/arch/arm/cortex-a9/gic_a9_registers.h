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
 * \brief a9 gic
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-08  zen, first implemetation
 * \endinternal
 */

#ifndef         __GIC_PL390_REGISTER_H__
#define         __GIC_PL390_REGISTER_H__

#define ARMV7A_PRIVATE_PERIPH_BASE      0x00A00000
#define GICD_BASE_OFFSET                0x1000
#define GICC_BASE_OFFSET                0x100

/**
 * \brief GIC Distributor register struct
 */
struct _gicd_registers
{
    uint32_t CTLR;              /**< \brief Distributor Control Register. */
    uint32_t TYPER;             /**< \brief Interrupt Controller Type Register.*/
    uint32_t IIDR;              /**< \brief Distributor Implementer Identification Register.*/
    uint32_t _reserved0[29];
    uint32_t IGROUPRn[32];      /**< \brief Interrupt Group Registers.*/
    uint32_t ISENABLERn[32];    /**< \brief Interrupt Set-Enable Registers.*/
    uint32_t ICENABLERn[32];    /**< \brief Interrupt Clear-Enable Registers.*/
    uint32_t ISPENDRn[32];      /**< \brief Interrupt Set-Pending Registers.*/
    uint32_t ICPENDRn[32];      /**< \brief Interrupt Clear-Pending Registers.*/
    uint32_t ICDABRn[32];       /**< \brief Active Bit Registers.
    uint32_t _reserved2[32];

    /** \brief Interrupt Priority Registers. (Byte accessible)*/
    uint8_t IPRIORITYRn[255 * sizeof(uint32_t)];  
    uint32_t _reserved3;

    /** \brief Interrupt Processor Targets Registers. (Byte accessible)*/
    uint8_t ITARGETSRn[255 * sizeof(uint32_t)];   
    uint32_t _reserved4;
    uint32_t ICFGRn[64];        /**< \brief Interrupt Configuration Registers.*/
    uint32_t _reserved5[128];
    uint32_t SGIR;              /**< \brief Software Generated Interrupt Register*/
};

#define GICD_CTLR_GRP0_ENABLE_FLAG_SECURE       (1 << 0)
#define GICD_CTLR_GRP1_ENABLE_FLAG_SECURE       (1 << 1)
#define GICD_CTLR_ENABLE_FLAG_NON_SECURE        (1 << 0)

/** \brief GIC CPU interface registers.*/
/** \brief Uses the GICv2 register names. Does not include GICv2 registers.*/
struct _gicc_registers
{
    uint32_t CTLR;  /**< \brief CPU Interface Control Register.*/
    uint32_t PMR;   /**< \brief Interrupt Priority Mask Register.*/
    uint32_t BPR;   /**< \brief Binary Point Register.*/
    uint32_t IAR;   /**< \brief Interrupt Acknowledge Register.*/
    uint32_t EOIR;  /**< \brief End of Interrupt Register.*/
    uint32_t RPR;   /**< \brief Running Priority Register.*/
    uint32_t HPPIR; /**< \brief Highest Priority Pending Interrupt Register.*/
    uint32_t ABPR;  /**< \brief Aliased Binary Point Register. (only visible with a secure access)*/
    uint32_t _reserved[55];
    uint32_t IIDR;  /**< \brief CPU Interface Identification Register.*/
};

#define GICC_CTLR_GRP0_ENABLE_FLAG_SECURE       (1 << 0)
#define GICC_CTLR_GRP1_ENABLE_FLAG_SECURE       (1 << 1)
#define GICC_CTLR_ENABLE_FLAG_NON_SECURE        (1 << 0)

#define GICC    ((struct _gicc_registers *)(ARMV7A_PRIVATE_PERIPH_BASE+GICC_BASE_OFFSET))
#define GICD    ((struct _gicd_registers *)(ARMV7A_PRIVATE_PERIPH_BASE+GICD_BASE_OFFSET))


#endif


