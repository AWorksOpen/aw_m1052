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
 * \brief 基于标准GPIO驱动的按键驱动
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "gpio-key"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct gpio_key_data
 *
 * \par 2.兼容设备
 *
 *  - 任何可以使用标准GPIO接口访问的按键
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_drv_input_gpio_key_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_input_key
 *
 * \internal
 * \par modification history
 * - 1.00 13-03-11  orz, first implementation.
 * \endinternal
 */

#ifndef __AWBL_GPIO_KEY_H
#define __AWBL_GPIO_KEY_H

#include "aw_common.h"
#include "awbus_lite.h"
#include "awbl_input.h"
#include "aw_types.h"
#include "aw_delayed_work.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_input_gpio_key
 * \copydetails awbl_gpio_key.h
 * @{
 */

/**
 * \defgroup  grp_awbl_drv_input_gpio_key_hwconf 设备定义/配置
 *
 * 通用按键输入设备的定义如下面的代码所示，用户在定义目标系统的硬件资源时，
 * 可以直接将这段代码嵌入到 awbus_lite_hwconf_usrcfg.c 中对应的位置，然后对设备
 * 信息进行必要的配置(可配置的项目用【x】标识)。
 *
 * \note 有多少个按键输入设备，就将下面的代码嵌入多少次，注意将变量修改为不同
 *       的名字。
 *
 * \include  hwconf_usrcfg_gpio_key.c
 * \note 以下配置项中带【用户配置】标识的，表示用户在开发应用时可以配置此参数。
 *
 * - 【1】 按键对应的GPIO编号和键值映射表 \n
 * 每个按键对应一个GPIO和一个键值，键值推荐使用 aw_input_key.h 中定义的标准键值
 *
 * - 【2】 按键按下时是否为低电平有效 \n
 * 按键低电平有效填true，否则填false
 *
 * - 【3】 按键中断扫描方式下延时消抖时间 \n
 * 消抖时间一般为10~20毫秒
 *
 * - 【4】 定时扫描方式下的扫描间隔时间 \n
 * 为0表示使用中断模式，非0则为定时扫描的间隔时间。注意如果GPIO按键不支持中断则
 * 不能使用中断模式
 *
 * - 【5】 中断模式下的GPIO“簇”中断号 \n
 * 这个参数用于支持某些芯片中一些GPIO“共享”一个中断的情况，为0表示不使用
 */

/** @} */

/** \brief name of GPIO key driver */
#define GPIO_KEY_DEV_NAME       "gpio-key"

/** \brief max keys a gpio-key device can handle */
#define GPIO_KEY_MAX_KEYS       32

/******************************************************************************/
struct gpio_key_map {
    uint16_t gpio;  /**< \brief GPIO connected to the key */
    uint16_t key;   /**< \brief key value */
};

/**
 * \brief GPIO key platform parameters
 * \note -  if scan_interval is not 0, the keypad driver will use a timer to
 *          scan the keypad in period, otherwise will use GPIO interrupt.
 */
struct gpio_key_data {
    void (*plat_init) (void);

    const struct gpio_key_map *keymap; /**< \brief key map */

    aw_bool_t    active_low;/**< \brief gpio polarity */
    uint8_t num_keys;       /**< \brief number of keys */
    uint8_t debounce_ms;    /**< \brief key debounce interval in ms */
    uint8_t scan_interval;  /**< \brief key scanning interval in ms */
    uint8_t clustered_irq;  /**< \brief irq that all key GPIOs bundled*/
};
#define to_gpio_key_data(dev)       ((void *)AWBL_DEVINFO_GET(dev))

/** \brief GPIO key structure */
struct gpio_key_dev {
    struct awbl_dev        dev;         /**< \brief AWBus device */
    struct aw_delayed_work work;        /**< \brief key scan delayed work */
    uint32_t               key_state;   /**< \brief last state of keys */
};
#define to_gpio_key(dev)            ((struct gpio_key_dev *)(dev))

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_GPIO_KEY_H */

/* end of file */
