/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2014 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief System initialization configure
 *
 *
 * \internal
 * \par Modification history
 * - 1.00 2017-07-10  sni, first implementation.
 * \endinternal
 */
#include "aworks.h"
#include "string.h"
#include "imx1050_pmu.h"
#include "imx1050_at_code.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"
#include "imx1050_dcdc_regs.h"
#include "imx1050_xtal_osc24m.h"
#include "driver/flexspi/imx1050_flexspi_regs.h"

extern unsigned int aw_const g_pfnVectors;

#if defined(__CC_ARM)
extern unsigned int const Image$$ER_m_RELOCODE_ORIGIN$$Base;
extern unsigned int const Image$$ER_m_RELOCODE_START$$Base;
extern unsigned int const Image$$ER_m_RELOCODE_END$$Base;
#define RELODE_ORIGIN_ADDR ((uint32_t)&Image$$ER_m_RELOCODE_ORIGIN$$Base)
#define RELODE_START_ADDR  ((uint32_t)&Image$$ER_m_RELOCODE_START$$Base)
#define RELODE_END_ADDR    ((uint32_t)&Image$$ER_m_RELOCODE_END$$Base)
#elif defined(__GNUC__)
extern unsigned int relocode_origin;
extern unsigned int relocode_start;
extern unsigned int relocode_end;
#define RELODE_ORIGIN_ADDR ((uint32_t)&relocode_origin)
#define RELODE_START_ADDR  ((uint32_t)&relocode_start)
#define RELODE_END_ADDR    ((uint32_t)&relocode_end)
#endif

#define RELOAD_CODE_LENGTH    (RELODE_END_ADDR - RELODE_START_ADDR)

__IMX1050_RAM_USED__(static inline uint32_t readl_in_ram(const volatile void *addr));
__IMX1050_RAM_USED__(static inline void writel_in_ram(uint32_t val, volatile void *addr));
__IMX1050_RAM_NOINLINE__(static void reconfig_pll3_clock(void));

static inline uint32_t readl_in_ram(const volatile void *addr)
{
    return *((const volatile uint32_t *) addr);
}

static inline void writel_in_ram(uint32_t val, volatile void *addr)
{
    *((volatile uint32_t *)addr) = val;
}

static void reconfig_pll3_clock(void)
{
    imx1050_flexspi_regs_t     *p_flexspi_regs;
    imx1050_ccm_regs_t         *p_ccm_regs;
    imx1050_ccm_analog_regs_t  *p_ccm_analog_regs;
    uint32_t                    temp;
    int                         is_flexspi_enable = 0;

    p_flexspi_regs = (imx1050_flexspi_regs_t *)IMX1050_FLEXSPI_BASE;
    p_ccm_regs = (imx1050_ccm_regs_t *)IMX1050_CCM_BASE;
    p_ccm_analog_regs = (imx1050_ccm_analog_regs_t *)IMX1050_CCM_ANALOG_BASE;

    temp = readl_in_ram(&p_flexspi_regs->MCR0);
    if (!(temp & (0x1 << 1)) ) {
        is_flexspi_enable = 1;
    }

    // 如果flexspi被启用，首先我们需要等待其传输完成并禁用它
    if (is_flexspi_enable) {
        do {
            temp = readl_in_ram(&p_flexspi_regs->STS0);
            temp &= ((0x1 << 0) | (0x1 << 1));
            temp ^= ((0x1 << 0) | (0x1 << 1));
        } while(temp);

        temp = readl_in_ram(&p_flexspi_regs->MCR0);
        temp |= (0x1 << 1);
        writel_in_ram(temp,&p_flexspi_regs->MCR0);
    }

    // disable clock gate of flexspi
    temp = readl_in_ram(&p_ccm_regs->CCGR6);
    temp &= (~(0x3 << 10));
    writel_in_ram(temp,&p_ccm_regs->CCGR6);

    // 配置FLEXSPI_PODF = 4,so FLEXSPI_CLK_ROOT = 120M
    temp = readl_in_ram(&p_ccm_regs->CSCMR1);
//    temp &= (~(0x3 << 23));                           /* 小bug，flexspi的divide因子有3个位，这里只清除掉两个位 */
    temp &= (~(0x7 << 23));
    temp |= (0x3 << 23);
    temp &= (~(0x3 << 29));
    temp |= (0x3 << 29);
    writel_in_ram(temp,&p_ccm_regs->CSCMR1);

    temp = readl_in_ram(&p_ccm_analog_regs->PFD_480);

    // 降低PLL3_PFD0的时钟至480M
    // 480 * 18 /18 = 480;
    temp &= (~((unsigned int)0xFF << 0));
    temp |= 18 << 0;

    // PLL3_PFD1的时钟为540M
    // 480 * 18 /16 = 540;
    temp &= (~((unsigned int)0xFF << 8));
    temp |= 16 << 8;

    // PLL3_PFD2的时钟为508M
    // 480 * 18 /17 = 508;
    temp &= (~((unsigned int)0xFF << 16));
    temp |= 17 << 16;

    // PLL3_PFD3的时钟为454M
    // 480 * 18 /19 = 454;
    temp &= (~((unsigned int)0xFF << 24));
    temp |= 19 << 24;

    writel_in_ram(temp,&p_ccm_analog_regs->PFD_480);

    // 配置FLEXSPI_PODF = 4,so FLEXSPI_CLK_ROOT = 120M
    temp = readl_in_ram(&p_ccm_regs->CSCMR1);
    temp &= (~(0x7 << 23));

#ifdef AW_FLEXSPI_NAND
    temp |= (4 << 23);                              /* 100:divide by 5 = 96M*/
#else
    temp |= (3 << 23);                              /* 011:divide by 4 = 120M*/
#endif

    temp &= (~(0x3 << 29));
    temp |= (3 << 29);
    writel_in_ram(temp,&p_ccm_regs->CSCMR1);

    // 配置完成，重启FLEXSPI
    if (is_flexspi_enable) {
        // enable clock gate of flexspi
        temp = readl_in_ram(&p_ccm_regs->CCGR6);
        temp |= ((0x3 << 10));
        writel_in_ram(temp,&p_ccm_regs->CCGR6);
        
        //note DLLCR寄存器值若非0x100，则会导致低温环境下运行异常
        temp = ((0x1 << 8));
        writel_in_ram(temp,&p_flexspi_regs->DLLCR[0]);
        
        temp = readl_in_ram(&p_flexspi_regs->MCR0);
        temp &= (~(0x1 << 1));
        writel_in_ram(temp,&p_flexspi_regs->MCR0);
        temp = readl_in_ram(&p_flexspi_regs->MCR0);
        temp |= (1 << 0);
        writel_in_ram(temp,&p_flexspi_regs->MCR0);

        do {
            temp = readl_in_ram(&p_flexspi_regs->MCR0);
        }while (temp & (1 << 0));

        do {
            temp = readl_in_ram(&p_flexspi_regs->STS0);
            temp &= ((0x1 << 0) | (0x1 << 1));
            temp ^= ((0x1 << 0) | (0x1 << 1));
        } while(temp);
    }
}

static void dcdc_init(void)
{
    imx1050_dcdc_regs_t        *p_dcdc_regs;
    uint32_t                    temp;

    p_dcdc_regs = (imx1050_dcdc_regs_t *)IMX1050_DCDC_BASE;

    // 设置DCDC进入CCM模式，以便在高功耗的情形下，获得更高的效率
    temp = readl(&p_dcdc_regs->REG0);
    temp &= (~(1 << 26));
    temp |= (1 << 0);
    writel(temp,&p_dcdc_regs->REG0);

    temp = readl(&p_dcdc_regs->REG2);
    temp &= (~(7 << 9));
    temp |= (0x3 << 9);
    writel(temp,&p_dcdc_regs->REG2);

    /* Connect internal the load resistor */
    temp = readl(&p_dcdc_regs->REG1);
    temp |= (0x1 << 9);
    writel(temp,&p_dcdc_regs->REG1);

    /* Setting the VDD_SOC to 1.275V. It is necessary to config AHB to 600Mhz. */
    temp = readl(&p_dcdc_regs->REG3);
    temp &= (~0x1F);
    temp |= 0x13;
    writel(temp,&p_dcdc_regs->REG3);

    /* Waiting for DCDC_STS_DC_OK bit is asserted */
    do {
       temp = readl(&p_dcdc_regs->REG0);
    } while (!(temp & (1 << 31)));
}

/**
 * \brief The SystemInit() called after reset.
 */
void SystemInit()
{
    // 设置中断向量表
    writel((uint32_t)&g_pfnVectors,(volatile uint32_t *)0xE000ED08);

    pmu_init();
    dcdc_init();
    xtal_osc24M_init();

    // 从加载域拷贝到执行域
    if(RELOAD_CODE_LENGTH)
    {
        memcpy((void *)RELODE_START_ADDR,
               (void *)RELODE_ORIGIN_ADDR,
               (size_t)RELOAD_CODE_LENGTH);
    }

    aw_arm_barrier_dsb();

    //由于不同的启动方式导致pll3时钟的配置产生差异
    //所以这里需要重新配置pll3的时钟
    reconfig_pll3_clock();
}

/**
 * \brief System initialization
 * The function should be called in aw_bsp_early_init()
 */
void aw_bsp_system_init (void)
{
    return;
}

/* end of file */
