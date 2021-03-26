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
 * \brief 
 *
 * \internal
 * \par modification history:
 * - 1.00 18-11-19  sni, first implement
 * \endinternal
 */

#ifndef         __GIC_A9_H__
#define         __GIC_A9_H__


void gic_init_non_secure(void);

void gic_enable_irq(uint32_t irqID, int isEnabled);

/** 
 * \brief Set whether a CPU will receive a particular interrupt.
 *
 * \param irqID      The interrupt number.
 * \param cpuNumber  The CPU number. The first CPU core is 0.
 * \param enableIt   Whether to send the interrupt to the specified CPU. 
 *                   Pass true to enable or false to disable.
 */
void gic_set_cpu_target(uint32_t irqID, unsigned cpuNumber, int enableIt);

/** 
 * \brief Set an interrupt's priority.
 *
 * \param irq_id    The interrupt number.
 * \param priority  The priority for the interrupt. In the range of 0 through 
 *                  0xff, with 0 being the highest priority.
 */
void gic_set_irq_priority(uint32_t irq_id, uint32_t priority);

uint32_t gic_get_irq_priority(uint32_t irq_id);
uint32_t gic_get_irq_priority_count(void);

void gic_set_irq_pending(uint32_t irq_id);






/** 
 * \brief Acknowledge starting of interrupt handling and get the interrupt number.
 *
 * Normally, this function is called at the beginning of the IRQ handler. It 
 * tells the GIC that you are starting to handle an interupt, and returns the 
 * number of the interrupt you need to handle. After the interrupt is handled, 
 * you should call gic_write_end_of_irq() to signal that the interrupt is 
 * completely handled.
 *
 * In some cases, a spurious interrupt might happen. One possibility is if 
 * another CPU handles the interrupt. When a spurious interrupt occurs, the 
 * end of the interrupt should be indicated but nothing else.
 *
 * \return The number for the highest priority interrupt available for the 
 *         calling CPU. If the return value is 1022 or 1023, a spurious 
 *         interrupt has occurred.
 */
uint32_t gicc_read_irq_ack(void);

/**
 * \brief Signal the end of handling an interrupt.
 *
 * \param irq_id The number of the interrupt for which handling has finished.
 */
void gicc_write_end_of_irq(uint32_t irq_id);
//@}






/* 设置指定的中断是否需要在secure状态下执行或者在non-secure状态执行*/
/* 此函数只能在secure状态下调用*/
void gic_set_irq_security(uint32_t irqID, int isSecure);


void gic_init_secure(void);
#endif
