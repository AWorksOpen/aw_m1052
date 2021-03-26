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
 
#include "rtk.h"
#include "rtk_io.h"
#include "gic_a9_registers.h"
#include "arch/arm/cortex-a9/gic_a9.h"

static inline uint32_t irq_get_register_offset(uint32_t irqID);
static inline uint32_t irq_get_bit_offset(uint32_t irqID);
static inline uint32_t irq_get_bit_mask(uint32_t irqID);


static inline uint32_t irq_get_register_offset(uint32_t irqID)
{
    return irqID / 32;
}

static inline uint32_t irq_get_bit_offset(uint32_t irqID)
{
    return irqID & 0x1f;
}

static inline uint32_t irq_get_bit_mask(uint32_t irqID)
{
    return 1 << irq_get_bit_offset(irqID);
}

static void gic_init_cpu_non_secure(void)
{
    /* disabile GIC CPU interface */
    writel(0,&GICC->CTLR);
    /* Init the GIC CPU interface. */
    writel(0x7F,&GICC->PMR);

    /* Disable preemption. */
    writel(7,&GICC->BPR);

    /* Enable signaling the CPU. */
    writel(GICC_CTLR_ENABLE_FLAG_NON_SECURE,&GICC->CTLR);
}

void gic_init_non_secure()
{
    int             i;

    /* First disable the distributor. */
    writel(0,&GICD->CTLR);

    /* Clear all pending interrupts. */
    for (i = 0; i < 32; i++) {
        writel(0xffffffff,&GICD->ICPENDRn[i]);
    }

    /* Init the GIC CPU interface. */
    gic_init_cpu_non_secure();

    /* Now enable the distributor. */
    writel( GICD_CTLR_ENABLE_FLAG_NON_SECURE,&GICD->CTLR);
}

static void gic_set_all_irq_non_secure(void)
{
    uint32_t            temp;
    uint32_t            i;

    /* 获取GICD中IGROUPRn寄存器的个数 */
    temp = readl(&GICD->TYPER);
    temp = ((0x1F & temp) + 1) ;

    for (i = 0; i< temp;i++) {
        writel(0xFFFFFFFF,&GICD->IGROUPRn[i]);
    }

}

void gic_init_secure(void)
{
    /*secure状态的初始化主要是将所有中断都配置为non-secure状态可以操作 */
    gic_set_all_irq_non_secure();
    /* secure状态下disable GICD */
    writel(3,&GICD->CTLR);
    writel(0x1F,&GICC->CTLR);
    writel(0xFF,&GICC->PMR);
}


void gic_enable_irq(uint32_t irqID, int isEnabled)
{
    uint32_t reg = irq_get_register_offset(irqID);
    uint32_t mask = irq_get_bit_mask(irqID);

    /* Select set-enable or clear-enable register based on enable flag. */
    if (isEnabled) {
        writel(mask,&GICD->ISENABLERn[reg]);
    }
    else {
        writel(mask,&GICD->ICENABLERn[reg]);
    }
}

void gic_set_cpu_target(uint32_t irqID, unsigned cpuNumber, aw_bool_t enableIt)
{
    /* Make sure the CPU number is valid. */
    uint8_t cpuMask = 1 << cpuNumber;
    uint8_t temp;

    temp = readb(&GICD->ITARGETSRn[irqID]);

    /*
     * Like the priority registers, the target registers are byte accessible, 
     * and the register struct has the them as a byte array, so we can just 
     * index directly by the interrupt ID.
     */
    if (enableIt) {
        temp |= cpuMask;
    }
    else {
        temp &= (uint8_t)(~cpuMask);
    }

    writeb(cpuMask,&GICD->ITARGETSRn[irqID]);
}


uint32_t gicc_read_irq_ack(void)
{
    return readl(&GICC->IAR);
}

void gicc_write_end_of_irq(uint32_t irqID)
{
    writel(irqID,&GICC->EOIR) ;
}
