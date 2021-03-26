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
 * \brief AWBus-lite lpc11xx ADC
 *
 * LPC13XX ADC0 为 LPC13XX 系列MCU的片上ADC0，有如下特性:
 *
 * - 10-bit 逐次逼近AD转换器
 * - 8个输入通道
 * - 测量范围 (0~3.6V)，不能超过VDD的电压
 * - 10-bit 转换时间 >= 2.44us
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "lpc11xx_adc"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_lpc11xx_adc_devinfo
 *
 * \par 2.兼容设备
 *
 *  - NXP LPC11XX 系列MCU
 *  - NXP LPC13XX 系列MCU
 *  - 其它与LPC11XX GPIO兼容的设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_lpc11xx_adc_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_adc
 *
 * \internal
 * \par modification history:
 * - 1.01 13-05-13  zyr, add jobq support, 
 *                   modify the way to get APB CLK
 * - 1.00 12-11-05 zyr, first implementation
 * \endinternal
 */

#ifndef __AWBL_LPC11XX_ADC_H
#define __AWBL_LPC11XX_ADC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/**
 * \addtogroup grp_awbl_drv_lpc11xx_adc
 * \copydetails awbl_lpc11xx_adc.h
 * @{
 */

/**
 * \defgroup  grp_awbl_lpc11xx_adc_hwconf 设备定义/配置
 *
 * LPC11XX ADC 硬件设备的定义如下面的代码所示，用户在定义目标系统的硬件资源时，
 * 可以直接将这段代码嵌入到 awbus_lite_hwconf_usrcfg.c 中对应的位置，然后对设备
 * 信息进行必要的配置(可配置的项目用【x】标识)。
 *
 * \note 有多少个ADC，就将下面的代码嵌入多少次，注意将变量修改为不同的名字。
 *
 * \include  hwconf_usrcfg_adc_lpc11xx.c
 * \note 以下配置项中带【用户配置】标识的，表示用户在开发应用时可以配置此参数。
 *
 * - 【1】平台相关初始化函数 \n
 * 在平台初始化函数中完成使能ADC时钟等与平台相关的操作。
 *
 * - 【2】ADC外设时钟频率获取函数 \n
 * 平台相关，将被驱动程序调用，用于获取外设时钟。
 *
 * - 【3】【4】AD通道编号 \n
 * ADC的起始通道编号和结束通道编号，整个系统的AD通道编号分配不能有重叠，系统的
 * AD通道编号分配请参考 \ref grp_aw_plfm_adc_chans 。
 *
 * - 【5】寄存器基地址 \n
 * 请参考MCU的用户手册填写此参数。
 *
 * - 【6】中断号 \n
 * 请参考MCU的用户手册填写此参数。
 *
 * - 【7】ADC位数 \n
 * 可设置的值为 3~10，运行时不可以修改ADC的转换位数，只能在这里设置。
 *
 * - 【8】AD转换速率 \n
 * 请参考MCU的用户手册填写此参数，不能大于芯片允许的最大速率。
 *
 * - 【9】 LPC11XX ADC 单元号
 * 为每个LPC11XX ADC 分配不同的单元号
 */

/** @} grp_awbl_drv_lpc11xx_gpio */

#include "awbl_adc.h"
#include "aw_spinlock.h"
#include "aw_isr_defer.h"

#define AWBL_LPC11XX_ADC_NAME   "lpc11xx_adc"

/*******************************************************************************
  defines
*******************************************************************************/
#if 0
#define AWBL_CONFIG_LPC11XX_ADC_JOBQ   /**< \brief Use job queue for this driver */
#endif


struct awbl_lpc11xx_adc_drv_ext {

    /** \brief current callback function */
    pfn_convert_callback_t  pfn_convert_cb;
    void                   *p_arg;
    aw_adc_val_t           *p_buf;
    aw_adc_channel_t        phy_ch;            /* 当前转换的通道  */

#ifdef AWBL_CONFIG_LPC11XX_ADC_JOBQ
    /** \brief adc result */
    uint32_t digital_val;
    /** \brief Job queue for ISR */
    struct aw_isr_defer_job  isr_jobq;
#endif
};

/**
 * \brief lpc11xx ADC 设备信息
 */
typedef struct awbl_lpc11xx_adc_devinfo {

    /** \brief ADC 服务相关信息 */
    struct awbl_adc_servinfo adc_servinfo;

    /** \brief ADC 寄存器基地址 */
    uint32_t regbase;

    /** \brief ADC 中断号      */
    uint32_t intnum;

    /** \brief ADC 位数        */
    uint8_t bits;
    
    /** \brief 参考电压,单位mV */
    uint32_t vref;

    /** \brief ADC 时钟 */
    uint32_t clk;

    /** \brief BSP function to get apb clock */
    uint32_t (*pfunc_plfm_apbclk_get) (void);

    /** \brief BSP function to pre initialize the device */
    void (*pfunc_plfm_init) (void);
} awbl_lpc11xx_adc_devinfo_t;

#define awbldev_get_lpc11xx_adc_info(p_dev) \
          ((struct awbl_lpc11xx_adc_devinfo *)AWBL_DEVINFO_GET(((awbl_dev_t *)(p_dev))))

/**
 * \brief lpc11xx ADC 设备实例
 */
typedef struct awbl_lpc11xx_adc_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev super;

    /** \brief ADC 服务 */
    struct awbl_adc_service adc_serv;

    /** \brief 设备扩展 */
    struct awbl_lpc11xx_adc_drv_ext ext;
} awbl_lpc11xx_adc_dev_t;

/**
 * \brief lpc11xx driver register
 */
void awbl_lpc11xx_adc_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_LPC11XX_ADC_H */

/* end of file */
