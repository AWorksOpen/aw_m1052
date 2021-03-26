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
 * \brief the definition of consts and struct of clock controler module in imx1050
 *
 * \par 1.兼容设备
 *
 *  - IMX1050
 *
 * \internal
 * \par modification history
 * - 1.00 2017-09-01  sni, first implementation
 * \endinternal
 */
#include "aworks.h"
#include "imx1050_ccm.h"
#include "imx1050_reg_base.h"
#include "imx1050_xtal_osc24m.h"
#include "imx1050_xtal_osc24m_regs.h"
#include "arch\arm\armv7-m\armv7m_core_regs.h"

#define SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY (600000000UL)

static void xtal_delay_atleast_us(uint32_t delay_us)
{
    armv7m_dwt_regs_t        *DWT;
    armv7m_core_debug_regs_t *CoreDebug;
    uint64_t                  count  = 0U;
    uint32_t                  period = SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY / 1000000;
    uint32_t                  temp;

    DWT = (armv7m_dwt_regs_t *)ARMV7M_DWT_BASE;
    CoreDebug = (armv7m_core_debug_regs_t *)ARMV7M_CoreDebug_BASE;

    /* Make sure the DWT trace fucntion is enabled. */
    temp = readl( &CoreDebug->DEMCR);
    if ( !(temp & (1 << 24)) ) {
        temp |= (1 << 24);
        writel(temp,&CoreDebug->DEMCR);
    }

    /* If CYCCENT has already been enabled, read directly, otherwise, need enable it. */
    temp = readl(&DWT->CTRL);
    if (!(temp & (1 << 0))) {
        temp |= (1 << 0);
        writel(temp,&DWT->CTRL);
    }

    /* Calculate the count ticks. */
    count = (uint64_t)readl(&DWT->CYCCNT);
    count += (uint64_t)period * delay_us;

    if (count > 0xFFFFFFFFUL)
    {
        count -= 0xFFFFFFFFUL;
        /* wait for cyccnt overflow. */
        do {
            temp = readl(&DWT->CYCCNT);
        } while(count < temp);
    }

    /* Wait for cyccnt reach count value. */
    do {
        temp = readl(&DWT->CYCCNT);
    } while(count > temp);
}

void xtal_osc24M_init(void)
{
    imx1050_xtal_osc24m_regs_t     *p_osc24m_regs;
    uint32_t                        temp;


    p_osc24m_regs = (imx1050_xtal_osc24m_regs_t *)IMX1050_XTALOSC24M_BASE;

    // 初始化xtal内置的RC 24M
    /* Enable RC OSC. It needs at least 4ms to be stable, so self tuning need to be enabled. */
    writel((1 << 0),&p_osc24m_regs->LOWPWR_CTRL_SET);
    /* Configure RC OSC */
    temp = (0x4 << 24) |    // RC_OSC_PROG_CUR
            (0x2 << 16) |   // HYST_MINUS
            (0xA7 << 4) |   // RC_OSC_PROG
            (0x1 << 1)  |   // ENABLE
            (0x1 << 0);     // START
    writel(temp,&p_osc24m_regs->OSC_CONFIG0);

    temp = (0x40 << 20)         //COUNT_RC_CUR
            | (0x2DC << 0);     //COUNT_RC_TRG
    writel(temp,&p_osc24m_regs->OSC_CONFIG1);

    /* Take some delay */
    xtal_delay_atleast_us(4000);
    /* Add some hysteresis */
    temp = readl(&p_osc24m_regs->OSC_CONFIG0);
    temp &= (~((0xF << 12) | (0xF << 16) ) );
    temp |= (0x3 << 12)     // HYST_PLUS
            | (0x3 << 16);  // HYST_MINUS
    writel(temp,&p_osc24m_regs->OSC_CONFIG0);

    /* Set COUNT_1M_TRG */
    temp = readl(&p_osc24m_regs->OSC_CONFIG2);
    temp &= (~(0xFFF << 0));
    temp |= (0x2d7 << 0);
    writel(temp,&p_osc24m_regs->OSC_CONFIG2);

    /* Hardware requires to read OSC_CONFIG0 or OSC_CONFIG1 to make OSC_CONFIG2 write work */
    temp = readl(&p_osc24m_regs->OSC_CONFIG1);
    writel(temp,&p_osc24m_regs->OSC_CONFIG1);

    // 上电初始化之后，需要打开1MHz的输出
    // 确保能够正常进入STOP模式
    temp = readl(&p_osc24m_regs->OSC_CONFIG2);
    temp |= (1 << 16);
    writel(temp,&p_osc24m_regs->OSC_CONFIG2);

    /* Use free 1MHz clock output. */
    temp = readl(&p_osc24m_regs->OSC_CONFIG2);
    temp &= (~( 1 << 17)) ;
    writel(temp,&p_osc24m_regs->OSC_CONFIG2);

    ccm_init_external_clk();
    ccm_clock_handshake_wait();
    xtal_delay_atleast_us(4000);
    xtal_osc24M_switch_osc_to_xtal();
    xtal_osc24M_deinit_rc_24m();
}

void xtal_osc24M_switch_osc_to_xtal(void)
{
    imx1050_xtal_osc24m_regs_t     *p_osc24m_regs;

    p_osc24m_regs = (imx1050_xtal_osc24m_regs_t *)IMX1050_XTALOSC24M_BASE;
    writel((1 << 4),&p_osc24m_regs->LOWPWR_CTRL_CLR);
}


void xtal_osc24M_switch_osc_to_rc(void)
{
    imx1050_xtal_osc24m_regs_t     *p_osc24m_regs;

    p_osc24m_regs = (imx1050_xtal_osc24m_regs_t *)IMX1050_XTALOSC24M_BASE;
    writel((1 << 4),&p_osc24m_regs->LOWPWR_CTRL_SET);
}

void xtal_osc24M_deinit_rc_24m(void)
{
    imx1050_xtal_osc24m_regs_t     *p_osc24m_regs;

    p_osc24m_regs = (imx1050_xtal_osc24m_regs_t *)IMX1050_XTALOSC24M_BASE;
    writel((1 << 0),&p_osc24m_regs->LOWPWR_CTRL_CLR);
}


