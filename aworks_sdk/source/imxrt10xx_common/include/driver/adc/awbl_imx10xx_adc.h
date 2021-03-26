/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx adc driver
 *
 * \internal
 * \par Modification History
 * - 1.00 2017-11-02  mex, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_ADC_H
#define __AWBL_IMX10xx_ADC_H

#include "aworks.h"
#include "aw_gpio.h"
#include "awbl_adc.h"
#include "aw_spinlock.h"
#include "aw_isr_defer.h"
#include "awbus_lite.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief iMX RT10xx ADC 驱动名 */
#define AWBL_IMX10XX_ADC_NAME            "imx10xx_adc"

/** \brief iMX RT10xx ADC 模拟输入通道范围 */
#define IMX10xx_ADC_MIN_CH           0
#define IMX10xx_ADC_MAX_CH           15


/**
 * \brief iMX RT10xx adc 设备信息
 */
typedef struct awbl_imx10xx_adc_devinfo {
    struct awbl_adc_servinfo adc_servinfo;    /**< \brief ADC 服务相关信息 */
    uint32_t                 regbase;         /**< \brief ADC 寄存器基地址  */
    uint8_t                  inum;            /**< \brief ADC 中断号 */
    uint8_t                  bits;            /**< \brief ADC 采样位数 */
    uint32_t                 refmv;           /**< \brief ADC 参考电压 mV */
    uint32_t                 clk_id;          /**< \brief ADC 时钟ID */
    void (*pfn_plfm_init) (void);             /**< \brief 平台初始化函数 */
} awbl_imx10xx_adc_devinfo_t;

/**
 * \brief iMX RT10xx adc 设备实例
 */
typedef struct awbl_imx10xx_adc_dev {

    struct awbl_dev         super;             /**< \brief 继承自 AWBus 设备 */
    struct awbl_adc_service adc_serv;          /**< \brief ADC 服务 */

    aw_adc_channel_t        phy_chan;          /**< \brief 设备当前转换的通道 */

    aw_adc_buf_desc_t      *p_bufdesc;         /**< \brief 指向各个缓冲区描述 */
    uint32_t                bufdesc_num;       /**< \brief 缓冲区个数 */
    uint32_t                cur_bufdesc_id;    /**< \brief 正在处理的缓冲区ID */

    uint32_t                samples;           /**< \brief 缓冲区已完成的次数 */
    uint32_t                remain_count;      /**< \brief 剩余的序列转换次数 */
    aw_bool_t               is_stopped;        /**< \brief 标志是否被强制停止 */

    pfn_adc_complete_t      pfn_seq_complete;  /**< \brief 序列完成回调函数 */
    void                   *p_arg;             /**< \brief 回调函数参数 */
    volatile uint8_t        temp_conver_end;   /** \brief  温度ADC转换完成标志 */

    AW_MUTEX_DECL(dev_lock);                   /**< \brief 设备锁 */

} awbl_imx10xx_adc_dev_t;

/**
 * \brief iMX RT10xx ADC 驱动注册
 */
void awbl_imx10xx_adc_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_ADC_H */

/* end of file */
