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
 * \brief core function for armv7m arch
 *
 * \internal
 * \par Modification history
 * - 1.00 2018-08-17  sni, first implementation.
 * \endinternal
 */


#ifndef __ARMV7M_NVIC_H__
#define __ARMV7M_NVIC_H__

typedef enum IRQn
{
    /* armv7m Processor Exceptions Numbers */
    ARMv7m_NonMaskableInt_IRQn  = -14,      /**< \brief 2 Non Maskable Interrupt */
    ARMv7m_MemManagement_IRQn   = -12,      /**< \brief 4 Memory Management Interrupt */
    ARMv7m_BusFault_IRQn        = -11,      /**< \brief 5 Bus Fault Interrupt */
    ARMv7m_UsageFault_IRQn      = -10,      /**< \brief 6 Usage Fault Interrupt */
    ARMv7m_SVCall_IRQn          = -5,       /**< \brief 11 SV Call Interrupt */
    ARMv7m_DebugMonitor_IRQn    = -4,       /**< \brief 12 Debug Monitor Interrupt */
    ARMv7m_PendSV_IRQn          = -2,       /**< \brief 14 Pend SV Interrupt */
    ARMv7m_SysTick_IRQn         = -1,       /**< \brief 15 System Tick Interrupt */
    ARMv7m_MAX_IRQN             = 4096,
}ARMv7m_IRQn_t;

typedef void (*pfn_armv7m_nvic_isr_t)(void *p_arg);

uint32_t armv7m_nvic_get_priority_count(void);

void armv7m_nvic_set_priority(ARMv7m_IRQn_t irqn,uint32_t prio);
uint32_t armv7m_nvic_get_priority(ARMv7m_IRQn_t irqn);

uint32_t armv7m_nvic_get_prio_group(void);
void armv7m_nvic_set_prio_group(uint32_t prio_grouping);

void armv7m_nvic_irq_enable(ARMv7m_IRQn_t irqn);
void armv7m_nvic_irq_disable(ARMv7m_IRQn_t irqn);
void armv7m_nvic_irq_set_pending(ARMv7m_IRQn_t irqn);
void armv7m_nvic_irq_clr_pending(ARMv7m_IRQn_t irqn);
ARMv7m_IRQn_t armv7m_nvic_get_active_irq(void);

int armv7m_nvic_irq_connect(
        ARMv7m_IRQn_t irqn,
        pfn_armv7m_nvic_isr_t isr_func,
        void *p_arg);

void armv7m_nvic_irq_disconnect(
        ARMv7m_IRQn_t irqn,
        pfn_armv7m_nvic_isr_t isr_func);

void armv7m_nvic_isr_runtime_init(
        uint32_t *p_vector_start,
        uint32_t length,
        void **p_arg_start,
        uint32_t arg_count);

void armv7m_nvic_gen_int_handler(void);

void * armv7m_nvic_get_args(unsigned int irqn);

#endif      /* __ARMV7M_CORE_H__*/

/* end of file */
