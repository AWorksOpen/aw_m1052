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
 * \par 1.ºÊ»›…Ë±∏
 *
 *  - IMX1020
 *
 * \internal
 * \par modification history
 * - 1.00 2020-04-29  cml, first implementation
 * \endinternal
 */

#include "aw_common.h"
#include "imx1020_pmu.h"
#include "imx1020_reg_base.h"
#include "imx1020_pmu_regs.h"
#include "imx1020_xtal_osc24m.h"
#include "imx1020_xtal_osc24m_regs.h"

void pmu_init(void)
{
    imx1020_pmu_regs_t             *p_pmu_regs;

    p_pmu_regs = (imx1020_pmu_regs_t *)IMX1020_PMU_BASE;

    /* Enable FET ODRIVE */
    writel( (1 << 29),&p_pmu_regs-> REG_CORE_SET);
    /* Connect vdd_high_in and connect vdd_snvs_in */
    writel( (1 << 12),&p_pmu_regs-> MISC0_CLR);

    pmu_bandgap_on();
    pmu_enable_regular_ldo();
    pmu_disable_weak_ldo();
}

void pmu_bandgap_on(void)
{
    imx1020_pmu_regs_t             *p_pmu_regs;
    uint32_t                        temp;
    imx1020_xtal_osc24m_regs_t     *p_osc24m_regs;

    p_pmu_regs = (imx1020_pmu_regs_t *)IMX1020_PMU_BASE;
    p_osc24m_regs = (imx1020_xtal_osc24m_regs_t *)IMX1020_XTALOSC24M_BASE;

    /* Turn on regular bandgap and wait for stable */
    writel((1 <<0),&p_pmu_regs->MISC0_CLR);
    do {
        temp = readl(&p_pmu_regs->MISC0);
    }while( !(temp & (1 << 7)));

    /* Low power band gap disable */
    writel((1 << 5),&p_osc24m_regs->LOWPWR_CTRL_CLR);
}


void pmu_enable_regular_ldo(void)
{
    imx1020_pmu_regs_t             *p_pmu_regs;

    p_pmu_regs = (imx1020_pmu_regs_t *)IMX1020_PMU_BASE;

    /*  Enable Regular LDO 2P5 and 1P1 */
    writel( (1 << 0),&p_pmu_regs-> REG_2P5_SET);
    writel( (1 << 0),&p_pmu_regs-> REG_1P1_SET);
}

void pmu_disable_regular_ldo(void)
{
    imx1020_pmu_regs_t             *p_pmu_regs;

    p_pmu_regs = (imx1020_pmu_regs_t *)IMX1020_PMU_BASE;

    /* Disable Regular LDO 2P5 and 1P1 */
    writel( (1 << 0),&p_pmu_regs-> REG_2P5_CLR);
    writel( (1 << 0),&p_pmu_regs-> REG_1P1_CLR);
}

void pmu_disable_weak_ldo(void)
{
    imx1020_pmu_regs_t             *p_pmu_regs;

    p_pmu_regs = (imx1020_pmu_regs_t *)IMX1020_PMU_BASE;

    /* Disable Weak LDO 2P5 and 1P1 */
    writel( (1 << 18),&p_pmu_regs-> REG_2P5_CLR);
    writel( (1 << 18),&p_pmu_regs-> REG_1P1_CLR);
}

