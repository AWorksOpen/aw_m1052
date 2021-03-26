/*******************************************************************************
*                                 AWorks
*                       ---------------------------
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
 * \brief AWorks user application initialization entry file.
 *
 * \internal
 * \par History
 * - 1.02 2019-05-06  cat, add lcd vram
 * - 1.01 2017-06-30  sni, first implementation for imxrt150x
 * \endinternal
 */

/*******************************************************************************
  headers
*******************************************************************************/

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_clk.h"
#include "string.h"
#include "arch/arm/armv7-m/armv7m_mpu.h"
#include "arch/arm/armv7-m/armv7m_cache.h"

/*******************************************************************************
  implementation
*******************************************************************************/

aw_import const struct awbl_devhcf *awbl_devhcf_list_get(void)
{
    extern aw_const struct awbl_devhcf g_awbl_devhcf_list[];

    return g_awbl_devhcf_list;
}

aw_import size_t awbl_devhcf_list_count_get(void)
{
    extern aw_const size_t g_awbl_devhcf_list_count;

    return g_awbl_devhcf_list_count;
}


/**
 * \brief user application initialization entry
 */
int aw_prj_usr_app_init (void)
{
    extern int aw_main (void);

    /* add user application initialization code here */
    return aw_main();
}

/******************************************************************************/
#if defined (__CC_ARM)



extern  unsigned int const Image$$ER_m_reset$$Base;             /*  __rom_start__ */
extern  unsigned int const Image$$ER_CODE_END_SCT$$Base ;       /*  __rom_end__  */

extern  unsigned int  const Image$$ER_RAM_START_SCT$$Base;      /*  __ram_start__ */
extern  unsigned int  const Image$$ER_RAM_END_SCT$$Base;        /*  __ram_end__ */

extern  unsigned int  const Image$$ER_DMA_HEAP_START$$Base;     /*  __heap_dma_start__  */
extern  unsigned int  const Image$$ER_DMA_HEAP_END$$Base;       /*  __heap_dma_end__  */

extern  unsigned int  const Image$$ER_HEAP_START$$Base;         /*  __heap_start__  */
extern  unsigned int  const Image$$ER_HEAP_END$$Base;           /*  __heap_end__  */

extern  unsigned int  const Image$$ER_VRAM_START$$Base;         /*  __vram_start__  */
extern  unsigned int  const Image$$ER_VRAM_END$$Base;           /*  __vram_end__  */ 

extern  unsigned int  const Image$$ER_FLEXSPI_DRV_START$$Base;  /*  __flexspi_drv_start__  */
extern  unsigned int  const Image$$ER_FLEXSPI_DRV_END$$Base;    /*  __flexspi_drv_end__  */

extern  unsigned int  const Image$$ER_m_RELOCODE_START$$Base;   /* __relocode_start__ */
extern  unsigned int  const Image$$ER_relocode_end$$Base;       /* __relocode_end__ */

extern  unsigned int  const Image$$ER_FLASH_START$$Base;        /* __flash_start__ */
extern  unsigned int  const Image$$ER_FLASH_END$$Base;          /* __flash_end__ */

#define ROM_START               ( (uint32_t)&Image$$ER_m_reset$$Base)
#define ROM_END                 ( ((uint32_t)&Image$$ER_CODE_END_SCT$$Base) - 1)

#define RAM_START               ( (uint32_t)&Image$$ER_RAM_START_SCT$$Base)
#define RAM_END                 ( ((uint32_t)&Image$$ER_RAM_END_SCT$$Base) - 1)

#define HEAP_DMA_START          ( (uint32_t)&Image$$ER_DMA_HEAP_START$$Base)
#define HEAP_DMA_END            ( ((uint32_t)&Image$$ER_DMA_HEAP_END$$Base) - 1)

#define HEAP_START              ( (uint32_t)&Image$$ER_HEAP_START$$Base)
#define HEAP_END                ( ((uint32_t)&Image$$ER_HEAP_END$$Base) - 1)

#define FLEXSPI_DRV_START       ( (uint32_t)&Image$$ER_FLEXSPI_DRV_START$$Base)
#define FLEXSPI_DRV_END         ( ((uint32_t)&Image$$ER_FLEXSPI_DRV_END$$Base) - 1)

#define VRAM_START              ( (uint32_t)&Image$$ER_VRAM_START$$Base)
#define VRAM_END                ( ((uint32_t)&Image$$ER_VRAM_END$$Base) - 1)

#define RELOCODE_START          ((uint32_t)&Image$$ER_m_RELOCODE_START$$Base)
#define RELOCODE_END            (((uint32_t)&Image$$ER_relocode_end$$Base) - 1)

#define FLASH_START             ((uint32_t)&Image$$ER_FLASH_START$$Base)
#define FLASH_END               ((uint32_t)&Image$$ER_FLASH_END$$Base - 1)

#elif defined (__GNUC__)
extern int __rom_start__;
extern int __rom_end__;
extern int __ram_start__;
extern int __ram_end__;
extern int __heap_start__;
extern int __heap_end__;
extern int __heap_dma_start__;
extern int __heap_dma_end__;
extern int __flexspi_drv_start__;
extern int __flexspi_drv_end__;
extern int __lcd_vram_start__;
extern int __lcd_vram_end__;
extern int __relocode_start__;
extern int __relocode_end__;
extern int __flash_start__;
extern int __flash_end__;

#define ROM_START               ( (uint32_t)&__rom_start__ )
#define ROM_END                 ( ((uint32_t)&__rom_end__) - 1)

#define RAM_START               ( ((uint32_t)&__ram_start__) )
#define RAM_END                 ( ((uint32_t)&__ram_end__) - 1)

#define HEAP_DMA_START          ( ((uint32_t)&__heap_dma_start__))
#define HEAP_DMA_END            ( ((uint32_t)&__heap_dma_end__) - 1)

#define HEAP_START              ( ((uint32_t)&__heap_start__))
#define HEAP_END                ( ((uint32_t)&__heap_end__) - 1)

#define FLEXSPI_DRV_START       ( ((uint32_t)&__flexspi_drv_start__))
#define FLEXSPI_DRV_END         ( ((uint32_t)&__flexspi_drv_end__) - 1)

#define VRAM_START              ( ((uint32_t)&__lcd_vram_start__))
#define VRAM_END                ( ((uint32_t)&__lcd_vram_end__) - 1)

#define RELOCODE_START          ((uint32_t)&__relocode_start__)
#define RELOCODE_END            (((uint32_t)&__relocode_end__) - 1)

#define FLASH_START             ((uint32_t)&__flash_start__)
#define FLASH_END               ((uint32_t)&__flash_end__ - 1)

#endif



/* Flexspi AHB读写映射基地址 */
#define __FLEXSPI_AMBA_BASE         (0x60000000UL)

static const struct mpu_region_desc __g_mpu_regions[] = {

        // NAND
        {
                0xA0000000,
                0XC0000000 - 1,
                kStronglyOrdered,
                kNonshareable,
                kRWAccess,
                kNonExecutable,
        },

        // heap
        {
                HEAP_START,
                HEAP_END,
                kOuterInner_WB_NOWA,
                kNonshareable,
                kRWAccess,
                kNonExecutable,
        },

        // ROM
        {
                ROM_START,
                ROM_END,
                kOuterInner_WB_NOWA,
                kNonshareable,
                kROAccess,
                kExecutable,
        },

        // RAM
        {
                RAM_START,
                RAM_END,
                kOuterInner_WB_NOWA,
                kNonshareable,
                kRWAccess,
                kNonExecutable,
        },

        // dma heap
        {
                HEAP_DMA_START,
                HEAP_DMA_END,
                kStronglyOrdered,
                kShareable,
                kRWAccess,
                kNonExecutable,
        },

        // devices
        {
                0x40000000,
                0x403FFFFF,
                kStronglyOrdered,
                kNonshareable,
                kRWAccess,
                kNonExecutable,
        },
        {
                0xE0000000,
                0xE0FFFFFF,
                kStronglyOrdered,
                kShareable,
                kRWAccess,
                kNonExecutable,
        },

        // Flexspi driver
        {
                FLEXSPI_DRV_START,
                FLEXSPI_DRV_END,
                kOuterInner_WB_NOWA,
                kNonshareable,
                kRWAccess,
                kExecutable,
        },

        // FlexSPI AHB read and write
        {
                FLASH_START,
                FLASH_END,
                kOuterInner_WT_NOWA,
                kNonshareable,
                kRWAccess,
                kNonExecutable,
        },

        // lcd vram
        {
                VRAM_START,
                VRAM_END,
                kOuterInner_WT_NOWA,
                kNonshareable,
                kRWAccess,
                kNonExecutable,
        },

        //reload code
        {
                RELOCODE_START,
                RELOCODE_END,
                kOuterInner_WB_NOWA,
                kNonshareable,
                kROAccess,
                kExecutable,
        }
};



static const struct mpu_region_desc __g_mpu_regions_flexspi_xip[] = {

        // NAND
        {
                0xA0000000,
                0XC0000000 - 1,
                kStronglyOrdered,
                kNonshareable,
                kRWAccess,
                kNonExecutable,
        },

        // heap
        {
                HEAP_START,
                HEAP_END,
                kOuterInner_WB_NOWA,
                kNonshareable,
                kRWAccess,
                kNonExecutable,
        },

        // ROM
        {
                ROM_START,
                ROM_END,
                kOuterInner_WB_NOWA,
                kNonshareable,
                kROAccess,
                kExecutable,
        },

        // RAM
        {
                RAM_START,
                RAM_END,
                kOuterInner_WB_NOWA,
                kNonshareable,
                kRWAccess,
                kNonExecutable,
        },

        // dma heap
        {
                HEAP_DMA_START,
                HEAP_DMA_END,
                kStronglyOrdered,
                kShareable,
                kRWAccess,
                kNonExecutable,
        },

        // devices
        {
                0x40000000,
                0x403FFFFF,
                kStronglyOrdered,
                kNonshareable,
                kRWAccess,
                kNonExecutable,
        },
        {
                0xE0000000,
                0xE0FFFFFF,
                kStronglyOrdered,
                kShareable,
                kRWAccess,
                kNonExecutable,
        },

        // Flexspi driver
        {
                FLEXSPI_DRV_START,
                FLEXSPI_DRV_END,
                kOuterInner_WB_NOWA,
                kNonshareable,
                kRWAccess,
                kExecutable,
        },

        // FlexSPI AHB read and write
        {
                FLASH_START,
                FLASH_END,
                kOuterInner_WT_NOWA,
                kNonshareable,
                kROAccess,
                kNonExecutable,
        },

        // lcd vram
        {
                VRAM_START,
                VRAM_END,
                kOuterInner_WT_NOWA,
                kNonshareable,
                kRWAccess,
                kNonExecutable,
        },
        
        //reload code
        {
                RELOCODE_START,
                RELOCODE_END,
                kOuterInner_WB_NOWA,
                kNonshareable,
                kROAccess,
                kExecutable,
        }
};


void target_init (void)
{
    armv7m_icache_disable();
    armv7m_dcache_disable();

    /* 判断是否是Flexspi XIP 执行 */
    if ((((uint32_t)&target_init) >  __FLEXSPI_AMBA_BASE) &&
        (((uint32_t)&target_init) < __FLEXSPI_AMBA_BASE + 0x4000000)) {

        aw_hw_mpu_init(__g_mpu_regions_flexspi_xip,
                      AW_NELEMENTS(__g_mpu_regions_flexspi_xip));

    } else {
        aw_hw_mpu_init(__g_mpu_regions,AW_NELEMENTS(__g_mpu_regions));
    }

    armv7m_icache_enable();
    armv7m_dcache_enable();
}

void user_clock_init(void)
{
    /*
     * 默认配置下，CPU时钟528MHz，为ARM PLL的二分之一。
     * 如果当前CPU支持600MHz，只需将ARM PLL的时钟修改为1200MHz(CPU时钟将被改为600MHz)。
     *
     * 方法：
     *     运行“aw_clk_rate_set(IMX1050_CLK_PLL_ARM, 1200000000)”
     */
//    aw_clk_rate_set(IMX1050_CLK_PLL_ARM, 1200000000);

    aw_clk_parent_set(IMX1050_CLK_CAN_CLK_SEL, IMX1050_CLK_PLL3_DIV_6);

    /* 设置Video PLL为96MHz, LCDIF的时钟源来自Video PLL */
    aw_clk_rate_set(IMX1050_CLK_PLL_VIDEO, 96000000);
    aw_clk_enable(IMX1050_CLK_PLL_VIDEO);
    aw_clk_parent_set(IMX1050_CLK_LCDIF1_PRE_CLK_SEL, IMX1050_CLK_PLL_VIDEO);
}



/* end of file */
