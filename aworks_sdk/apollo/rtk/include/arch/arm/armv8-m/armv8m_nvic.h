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
 * \brief core function for armv8m arch
 *
 * \internal
 * \par Modification history
 * - 1.10 2019-08-28, A.J, armv8-m nvic implementation.
 * - 1.00 2018-08-17  sni, first implementation.
 * \endinternal
 */

#ifndef __ARMV8M_NVIC_H__
#define __ARMV8M_NVIC_H__

typedef enum IRQn
{
    /* armv8m Processor Exceptions Numbers */
    ARMv8m_NonMaskableInt_IRQn  = -14,      /**< \brief 2 Non Maskable Interrupt */
    ARMv8m_MemManagement_IRQn   = -12,      /**< \brief 4 Memory Management Interrupt */
    ARMv8m_BusFault_IRQn        = -11,      /**< \brief 5 Bus Fault Interrupt */
    ARMv8m_UsageFault_IRQn      = -10,      /**< \brief 6 Usage Fault Interrupt */
    ARMv8m_SVCall_IRQn          = -5,       /**< \brief 11 SV Call Interrupt */
    ARMv8m_DebugMonitor_IRQn    = -4,       /**< \brief 12 Debug Monitor Interrupt */
    ARMv8m_PendSV_IRQn          = -2,       /**< \brief 14 Pend SV Interrupt */
    ARMv8m_SysTick_IRQn         = -1,       /**< \brief 15 System Tick Interrupt */
    ARMv8m_MAX_IRQN             = 4096,
}ARMv8m_IRQn_t;

typedef void (*pfn_armv8m_nvic_isr_t)(void *p_arg);

uint32_t armv8m_nvic_get_priority_count(void);

void armv8m_nvic_set_priority(ARMv8m_IRQn_t irqn, uint32_t prio);
uint32_t armv8m_nvic_get_priority(ARMv8m_IRQn_t irqn);

uint32_t armv8m_nvic_get_prio_group(void);
void armv8m_nvic_set_prio_group(uint32_t prio_grouping);

void armv8m_nvic_irq_enable(ARMv8m_IRQn_t irqn);
void armv8m_nvic_irq_disable(ARMv8m_IRQn_t irqn);
void armv8m_nvic_irq_set_pending(ARMv8m_IRQn_t irqn);
void armv8m_nvic_irq_clr_pending(ARMv8m_IRQn_t irqn);
ARMv8m_IRQn_t armv8m_nvic_get_active_irq(void);

int armv8m_nvic_irq_connect(
        ARMv8m_IRQn_t irqn,
        pfn_armv8m_nvic_isr_t isr_func,
        void *p_arg);

void armv8m_nvic_irq_disconnect(
        ARMv8m_IRQn_t irqn,
        pfn_armv8m_nvic_isr_t isr_func);

void armv8m_nvic_isr_runtime_init(
        uint32_t *p_vector_start,
        uint32_t length,
        void **p_arg_start,
        uint32_t arg_count);

void armv8m_nvic_gen_int_handler(void);

void * armv8m_nvic_get_args(unsigned int irqn);

#endif /* __ARMV8M_NVIC_H__*/

/* end of file */
