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
 * \brief core rigister for armv8m arch
 *
 * \internal
 * \par Modification history
 * - 1.00 2019-08-28, A.J, first implementation.
 * \endinternal
 */

#ifndef __ARMV8M_CORE_REGS_H__
#define __ARMV8M_CORE_REGS_H__

/** \brief System Control Space Base Address  */
#define ARMV8M_SCS_BASE         (0xE000E000UL)
/** \brief ITM Base Address                   */
#define ARMV8M_ITM_BASE         (0xE0000000UL)
/** \brief DWT Base Address                   */
#define ARMV8M_DWT_BASE         (0xE0001000UL)
/** \brief TPI Base Address                   */
#define ARMV8M_TPI_BASE         (0xE0040000UL)
/** \brief Core Debug Base Address            */
#define ARMV8M_CoreDebug_BASE   (0xE000EDF0UL)
/** \brief NVIC Base Address                  */
#define ARMV8M_NVIC_BASE        (ARMV8M_SCS_BASE + 0x0100UL)
/** \brief System Control Block Base Address  */
#define ARMV8M_SCB_BASE         (ARMV8M_SCS_BASE + 0x0D00UL)
/** \brief Floating Point Unit                */
#define ARMV8M_FPU_BASE         (ARMV8M_SCS_BASE + 0x0F30UL)

/* \brief 定义SCB结构 */
typedef struct
{
    /** \brief Offset: 0x000 (R/ )  CPUID Base Register*/
    volatile uint32_t CPUID;
    /** \brief Offset: 0x004 (R/W)  Interrupt Control and State Register*/
    volatile uint32_t ICSR;
   /** \brief Offset: 0x008 (R/W)  Vector Table Offset Register*/
    volatile uint32_t VTOR;
   /** \brief Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register*/
    volatile uint32_t AIRCR;
    /** \brief Offset: 0x010 (R/W)  System Control Register*/
    volatile uint32_t SCR;
    /** \brief Offset: 0x014 (R/W)  Configuration Control Register*/
    volatile uint32_t CCR;
    /** \brief Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15)*/
    volatile uint8_t  SHPR[12];
    /** \brief Offset: 0x024 (R/W)  System Handler Control and State Register*/
    volatile uint32_t SHCSR;
    /** \brief Offset: 0x028 (R/W)  Configurable Fault Status Register */
    volatile uint32_t CFSR;
    /** \brief Offset: 0x02C (R/W)  HardFault Status Register    */
    volatile uint32_t HFSR;
    /** \brief Offset: 0x030 (R/W)  Debug Fault Status Register*/
    volatile uint32_t DFSR;
    /** \brief Offset: 0x034 (R/W)  MemManage Fault Address Register */
    volatile uint32_t MMFAR;
    /** \brief Offset: 0x038 (R/W)  BusFault Address Register */
    volatile uint32_t BFAR;
    /** \brief Offset: 0x03C (R/W)  Auxiliary Fault Status Register */
    volatile uint32_t AFSR;
    /** \brief Offset: 0x040 (R/ )  Processor Feature Register*/
    volatile uint32_t PFR[2];
    /** \brief Offset: 0x048 (R/ )  Debug Feature Register */
    volatile uint32_t DFR;
    /** \brief Offset: 0x04C (R/ )  Auxiliary Feature Register*/
    volatile uint32_t AFR;
    /** \brief Offset: 0x050 (R/ )  Memory Model Feature Register*/
    volatile uint32_t MMFR[4];
    /** \brief Offset: 0x060 (R/ )  Instruction Set Attributes Register*/
    volatile uint32_t ISAR[6];
    volatile uint32_t RESERVED0[4];
     /** \brief Offset: 0x088 (R/W)  Coprocessor Access Control Register*/
    volatile uint32_t CPACR;
} armv8m_scb_t;

typedef struct
{
    uint32_t          RESERVED0[1];
    /** \brief Offset: 0x004 (R/ )  Interrupt Controller Type Register*/
    volatile uint32_t ICTR;
    /** \brief Offset: 0x008 (R/W)  Auxiliary Control Register */
    volatile uint32_t ACTLR;
} armv8m_scn_scb_t;

/**
 * \brief  Structure type to access the Nested Vectored Interrupt Controller (NVIC).
 */
typedef struct
{
    /** \brief Offset: 0x000 (R/W)  Interrupt Set Enable Register */
    volatile uint32_t ISER[16];
    volatile uint32_t RESERVED0[16];
    /** \brief Offset: 0x080 (R/W)  Interrupt Clear Enable Register */
    volatile uint32_t ICER[16];
    volatile uint32_t RSERVED1[16];
    /** \brief Offset: 0x100 (R/W)  Interrupt Set Pending Register */
    volatile uint32_t ISPR[16];
    volatile uint32_t RESERVED2[16];
    /** \brief Offset: 0x180 (R/W)  Interrupt Clear Pending Register */
    volatile uint32_t ICPR[16];
    volatile uint32_t RESERVED3[16];
    /** \brief Offset: 0x200 (R/W)  Interrupt Active bit Register */
    volatile uint32_t IABR[16];
    volatile uint32_t RESERVED4[16];
    /** \brief Offset: 0x280 (R/W)  Interrupt Target Non-secure Register */
    volatile uint32_t ITNS[16];
    volatile uint32_t RESERVED5[16];
    /** \brief Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide) */
    volatile uint8_t  IPR[480];
    volatile uint32_t RESERVED6[584];
    /** \brief Offset: 0xE00 ( /W)  Software Trigger Interrupt Register */
    volatile uint32_t STIR;
} armv8m_nvic_t;

/**
 * \brief  Structure type to access the Floating Point Unit (FPU).
 */
typedef struct
{
    uint32_t          RESERVED0[1];
    /** \brief Offset: 0x004 (R/W)  Floating-Point Context Control Register*/
    volatile uint32_t FPCCR;
    /** \brief Offset: 0x008 (R/W)  Floating-Point Context Address Register*/
    volatile uint32_t FPCAR;
    /** \brief Offset: 0x00C (R/W)  Floating-Point Default Status Control Register */
    volatile uint32_t FPDSCR;
    /** \brief Offset: 0x010 (R/ )  Media and FP Feature Register 0*/
    volatile uint32_t MVFR0;
    /** \brief Offset: 0x014 (R/ )  Media and FP Feature Register 1 */
    volatile uint32_t MVFR1;
    /** \brief Offset: 0x014 (R/ )  Media and FP Feature Register 2 */
    volatile uint32_t MVFR2;
} armv8m_fpu_t;

#define ARMV8M_SCB      ((armv8m_scb_t *)ARMV8M_SCB_BASE)
#define ARMV8M_NVIC     ((armv8m_nvic_t *)ARMV8M_NVIC_BASE)
#define ARMV8M_FPU      ((armv8m_fpu_t *)ARMV8M_FPU_BASE)

 /*\brief System control Register not in SCB */
#define ARMV8M_SCnSCB   ((armv8m_scn_scb_t *)ARMV8M_SCS_BASE)

#endif /* __ARMV8M_CORE_REGS_H__*/

/* end of file */
