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
 * \brief ARMv7-M nvic
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-08  zen, first implemetation
 * \endinternal
 */
 
#include "rtk.h"
#include "aw_io.h"
#include "aw_exc.h"
#include "aw_bitops.h"
#include "arch/arm/armv7-m/armv7m_core.h"
#include "armv7m_context.h"
#include "string.h"

uint32_t armv7m_nvic_get_priority_count(void)
{
    return ( 1<< CONFIG_RTK_ARMV7M_NVIC_PRIO_BITS);
}

void armv7m_nvic_set_priority(ARMv7m_IRQn_t irqn,uint32_t prio)
{
    uint8_t             data;

    data = (uint8_t)(( prio << (8 - CONFIG_RTK_ARMV7M_NVIC_PRIO_BITS)) & 0xFF);

    if(irqn < 0) {
        irqn = 16 - (0 - irqn) - 4;
        /* set Priority for Cortex-M System Interrupts */
        writeb(data,&ARMV7M_SCB->SHP[irqn]);
    }
    else {
        /* set Priority for device specific Interrupts  */
        writeb(data,&ARMV7M_NVIC->IP[irqn]);
    }
}

uint32_t armv7m_nvic_get_priority(ARMv7m_IRQn_t irqn)
{
    uint8_t             data;
    uint32_t            prio;

    if(irqn < 0) {
        irqn = 16 - (0 - irqn) - 4;
        /* get Priority for Cortex-M System Interrupts */
        data = readb(&ARMV7M_SCB->SHP[irqn]);
    }
    else {
        /* get Priority for device specific Interrupts  */
        data = readb(&ARMV7M_NVIC->IP[irqn]);
    }

    prio = ((uint32_t)data) >> (8 - CONFIG_RTK_ARMV7M_NVIC_PRIO_BITS);
    return prio;
}


#define AIRCR_VECTKEY_MASK

void armv7m_nvic_set_prio_group(uint32_t prio_grouping)
{
    uint32_t            reg_value;

    /* ¶ÁÈ¡´óÐ¡¶Ë */
    reg_value = readl( &ARMV7M_SCB->AIRCR);
    reg_value &= (1<< 15);

    if (prio_grouping > 7) {
        prio_grouping = 7;
    }

    reg_value |= (prio_grouping << 8);
    reg_value |= (0x05FA << 16);
    writel(reg_value,&ARMV7M_SCB->AIRCR);
    rtk_arm_barrier_dsb();
}

uint32_t armv7m_nvic_get_prio_group(void)
{
    uint32_t            reg_value;

    reg_value = AW_REG_BITS_GET32(&ARMV7M_SCB->AIRCR,8,3);
    return reg_value;
}




void armv7m_nvic_irq_enable(ARMv7m_IRQn_t irqn)
{
    register uint32_t       mask;
    register uint32_t      *p_addr;
    if (irqn >= 0) {
        mask = ((uint32_t)irqn) >> 5;
        p_addr = (uint32_t *)&(ARMV7M_NVIC->ISER[mask]);
        mask = ( 1<< (irqn & 0x1F));
        writel(mask,p_addr);
        rtk_arm_barrier_dsb();
    }
}

void armv7m_nvic_irq_disable(ARMv7m_IRQn_t irqn)
{
    register uint32_t       mask;
    register uint32_t      *p_addr;
    if (irqn >= 0) {
        mask = ((uint32_t)irqn) >> 5;
        p_addr = (uint32_t *)&(ARMV7M_NVIC->ICER[mask]);
        mask = ( 1<< (irqn & 0x1F));
        writel(mask,p_addr);
        rtk_arm_barrier_dsb();
    }
}

void armv7m_nvic_irq_set_pending(ARMv7m_IRQn_t irqn)
{
    register uint32_t      *p_addr;
    if (irqn >= 0) {
        p_addr = (uint32_t *)&(ARMV7M_NVIC->STIR);
        writel(irqn,p_addr);
        rtk_arm_barrier_dsb();
    }
}

void armv7m_nvic_irq_clr_pending(ARMv7m_IRQn_t irqn)
{
    register uint32_t       mask;
    register uint32_t      *p_addr;
    if (irqn >= 0) {
        mask = ((uint32_t)irqn) >> 5;
        p_addr = (uint32_t *)&(ARMV7M_NVIC->ICPR[mask]);
        mask = ( 1<< (irqn & 0x1F));
        writel(mask,p_addr);
        rtk_arm_barrier_dsb();
    }
}




static uint32_t                     _g_isr_count;
static void *                      *_g_p_arg_start;
static uint32_t *                  _g_p_vector_start;

void armv7m_nvic_isr_runtime_init(
        uint32_t *p_vector_start,
        uint32_t length,
        void **p_arg_start,
        uint32_t arg_count)
{
    uint32_t            irq_count;
    uint32_t            exc_count;
    uint32_t            vector_count;
    uint32_t           *p_org_vector_start;
    RTK_CRITICAL_STATUS_DECL(old);

    irq_count =( (ARMV7M_SCnSCB->ICTR & 0xF) + 1 ) * 32;
    exc_count = irq_count + 16;
    vector_count = length / 4;

    if (NULL == p_vector_start || NULL == p_arg_start) {
        while(1);
    }

    if (( vector_count < exc_count) || (arg_count < irq_count))   {
        while(1);
    }

    if ((uint32_t)p_vector_start % 128 != 0) {
        while(1);
    }
    p_org_vector_start = (uint32_t *)readl(&ARMV7M_SCB->VTOR);
    memcpy(p_vector_start,p_org_vector_start,exc_count * 4);

    _g_isr_count = irq_count;
    _g_p_arg_start = p_arg_start;
    for (irq_count = 0;irq_count < _g_isr_count;irq_count++) {
        p_arg_start[irq_count] = NULL;
    }


    RTK_ENTER_CRITICAL(old);
    _g_p_vector_start = p_vector_start;
    rtk_arm_barrier_dsb();
    writel((uint32_t)p_vector_start,&ARMV7M_SCB->VTOR);
    rtk_arm_barrier_dsb();
    rtk_arm_barrier_isb();
    RTK_EXIT_CRITICAL(old);
}

int armv7m_nvic_irq_connect(
        ARMv7m_IRQn_t irqn,
        pfn_armv7m_nvic_isr_t isr_func,
        void *p_arg)
{
    RTK_CRITICAL_STATUS_DECL(old);
    uint32_t                    max_isr_count;

    max_isr_count = ( (ARMV7M_SCnSCB->ICTR & 0xF) + 1 ) * 32;

    if (irqn <0 || irqn > max_isr_count
            || NULL == isr_func) {
        return -AW_EINVAL;
    }

    RTK_ENTER_CRITICAL(old);
    _g_p_vector_start[irqn + 16] = (uint32_t)isr_func;
    _g_p_arg_start[irqn] = p_arg;
    rtk_arm_barrier_isb();
    RTK_EXIT_CRITICAL(old);

    return 0;
}

void armv7m_nvic_irq_disconnect(
        ARMv7m_IRQn_t irqn,
        pfn_armv7m_nvic_isr_t isr_func)
{
    RTK_CRITICAL_STATUS_DECL(old);
    uint32_t                    max_isr_count;

    max_isr_count = ( (ARMV7M_SCnSCB->ICTR & 0xF) + 1 ) * 32;

    if (irqn <0 || irqn > max_isr_count
            || NULL == isr_func ) {
        return ;
    }

    RTK_ENTER_CRITICAL(old);
    _g_p_vector_start[irqn + 16] = (uint32_t)armv7m_nvic_gen_int_handler;
    _g_p_arg_start[irqn] = NULL;
    rtk_arm_barrier_isb();
    RTK_EXIT_CRITICAL(old);
}

void armv7m_nvic_gen_int_handler(void)
{
    while(1);
}

ARMv7m_IRQn_t armv7m_nvic_get_active_irq(void)
{
    uint32_t            active_irq;
    int                 irq;

    active_irq = (readl(&ARMV7M_SCB->ICSR) & 0x1FF) ;

    irq = (int)(int32_t)(active_irq - 16);
    return (ARMv7m_IRQn_t)irq;
}

void * armv7m_nvic_get_args(unsigned int irqn)
{
    return _g_p_arg_start[irqn];
}
