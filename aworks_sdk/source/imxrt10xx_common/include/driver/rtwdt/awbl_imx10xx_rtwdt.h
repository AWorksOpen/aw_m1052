/*******************************************************************************
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
 * \brief AWBus IMX10xx on chip RTwatchdog interface header file
 *
 * use this module should include follows :
 * \code
 * #include "awbl_wdt.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-22  hsg, first implementation
 * \endinternal
 */

/**
 * \note RT看门狗与普通看门狗的区别之处在于它有窗口功能，用户必须在
 *       窗口值的上限（Toval寄存器值）与窗口值的下限（WIN寄存器的值）之间喂狗。
 *
 * \details
 * RT看门狗在开启中断后会在强制复位之前延迟256个总线时钟（约1.9us），
 * 以允许中断服务例程（ISR）执行任务（如分析堆栈以调试代码）。
 *
 * RTWDOG的时钟源选择的是LPO_CLK时钟源（32.768Khz）。
 * RTWDOG在第一阶段初始化中的默认配置是：关闭窗口模式，不分频，关闭中断（用户若需要使用窗口模式可以在主函数中调用
 * RTWDOG提供给外部的接口更改看门狗配置【需要包含头文件“driver/rtwdt/awbl_imx10xx_rtwdt.h”】）。
 * RT看门狗的超时公式：Time_out = TOVAL / Frtwdog / Pres
 * 其中：
 *     Time_out：RT看门狗超时时间（单位为ms）
 *     TOVAL：写入RT看门狗toval寄存器的值
 *     Frtwdog:RT看门狗计数时钟频率（单位为Khz）
 *     Pres：RT看门狗分频值（可以选择不分频或256分频），不分频时其值为1，分频时其值为256。
 *
 * 时钟源选择的是LPO_CLK时钟源时，根据以上公式，可以得到最小-最大超时时间（四舍五入），如下表所示：
 *    +---------+----------------+----------------+
      | PRES    | 最小超时                             | 最大超时                             |
      |         | TOVAL = 0x0001 | TOVAL = 0xFFFF |
      +=========+================+================+
      | 不分频            | 30.5us         | 2s             |
      +---------+----------------+----------------+
      | 256分频       | 7.83ms         | 512s           |
      +---------+----------------+----------------+
*/

#ifndef __AWBL_IMX10xx_RTWDT_H
#define __AWBL_IMX10xx_RTWDT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/*******************************************************************************
  includes
*******************************************************************************/
#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_wdt.h"

/*******************************************************************************
  define
*******************************************************************************/

/** \brief define IMX10xx watchdog's name */
#define  AWBL_IMX10XX_RTWDT_NAME  "imx10xx_rtwdt"

/*******************************************************************************
awbl_imx10xx_rtwdt_parm_set函数的options选项宏（可以为几个宏的或值）
*******************************************************************************/

#define  AW_RTWDOG_WINDOW_MODE_OPEN       ((1) << 15)    /**< \brief 打开看门狗窗口模式  */
#define  AW_RTWDOG_WINDOW_MODE_CLOSE      ((0) << 15)    /**< \brief 关闭看门狗窗口模式  */

#define  AW_RTWDOG_INTERRUPT_ENABLE       ((1) << 6)     /**< \brief 看门狗中断使能  */
#define  AW_RTWDOG_INTERRUPT_DISABLE      ((0) << 6)     /**< \brief 看门狗中断禁能  */

#define  AW_RTWDOG_PRESCALER_ENABLE       ((1) << 12)    /**< \brief 看门狗分频使能  */
#define  AW_RTWDOG_PRESCALER_DISABLE      ((0) << 12)    /**< \brief 看门狗分频禁能  */

/******************************************************************************/

#define  AW_RTWDOG_INTERRUPT_FLAG_MASK    ((1) << 14)    /**< \brief 看门狗中断标志  */
#define  AW_RTWDOG_RECONFIG_SUCESS        ((1) << 10)    /**< \brief 看门狗重新配置成功  */

/** \brief define IMX10xx watchdog's time info */
typedef struct  awbl_imx10xx_rtwdt_devinfo {
    uint32_t        reg_base;           /**< \brief RTWDT register base address */
    uint32_t        clk_id;             /**< \brief clk id */
    uint32_t        wdt_time;           /**< \brief RTWDT time(ms), use to init1 */
    const uint32_t  inum;               /**< \brief RTWDT inum */
    void (*pfn_plfm_init) (void);   /**< \brief platform init */
} awbl_imx10xx_rtwdt_devinfo_t;

/** \brief define IMX10xx watchdog's device */
typedef struct  awbl_imx10xx_rtwdt_dev {
    struct awbl_dev          dev;            /**< the AWBus device */
    struct awbl_hwwdt_info   info;           /**< hw wdt info */
    uint32_t                 reg_base;       /**< hw wdt reg_base */
    uint32_t                 wdt_time_up;    /**< hw wdt wdt_time_up(ms) 这个参数默认为0，只有在调用awbl_imx10xx_rtwdt_parm_set函数后才会进行赋值 */
    uint32_t                 wdt_time_down;  /**< hw wdt wdt_time_down(ms) 这个参数默认为0，只有在调用awbl_imx10xx_rtwdt_parm_set函数后才会进行赋值 */
} awbl_imx10xx_rtwdt_dev_t;

/**
 * \brief IMX10xx rtwdt driver register
 */
void awbl_imx10xx_rtwdt_drv_register (void);


/**
 * \brief 设置看门狗定时器是否使用窗口模式，开启看门狗 中断与否，以及设置其窗口值和超时值
 *
 * 注意：调用这个函数时一定要在options参数里面表面分频与否，否则驱动不知道如何计算toval的值
 *
 * param[in] p_dev   看门狗父亲设备的指针
 * param[in] options   需要设置的看门狗选项宏的或值（例如：AW_RTWDOG_PRESCALER_ENABLE |
 *                                             AW_OPEN_RTWDOG_WINDOW_MODE |
 *                                             AW_ENABLE_RTWDOG_INTERRUPT）
 *                     必须在options选项体现出来分频与否，窗口模式开关，中断使能与否
 * param[in] wdt_time_down   看门狗窗口值下限（单位ms）,如果不使用窗口模式，直接设置为0即可，其为win寄存器的毫秒值
 * param[in]  wdt_time_up   看门狗窗口值上限（不能为0，否则看门狗会一直复位,单位（ms）），其为toval寄存器的毫秒值
 *
 * \return 无
 */
void awbl_imx10xx_rtwdt_parm_set (struct awbl_dev *p_dev,
                                  uint16_t         options,
                                  uint32_t         wdt_time_down,
                                  uint32_t         wdt_time_up);

/**
 * 注意：需要在使能窗口模式后使用
 * \brief 看门狗当前已计数的计数值获取（返回值为已计数的毫秒数）
 *
 * param[in] p_dev   看门狗父亲设备的指针
 * param[in] cnt_val   用于存储获取的毫秒数
 *
 * \return 无
 */
void awbl_imx10xx_rtwdt_cnt_val_get (struct awbl_dev *p_dev,
                                     uint32_t        *cnt_val);

/**
 * 注意：只有在调用awbl_imx10xx_rtwdt_parm_set函数后才能获取，如果没有使用窗口模式忽略wdt_time_down的值
 * \brief 看门狗设置的窗口值下限和上限获取(单位：ms)
 *
 * param[in] p_dev   看门狗父亲设备的指针
 * param[in] wdt_time_down   窗口看门狗上限获取
 * param[in] wdt_time_up     窗口看门狗下限获取
 *
 * \return 无
 */
void awbl_imx10xx_rtwdt_parm_get (struct awbl_dev *p_dev,
                                  uint32_t        *wdt_time_down,
                                  uint32_t        *wdt_time_up);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_RTWDT_H */

/* end of file */
