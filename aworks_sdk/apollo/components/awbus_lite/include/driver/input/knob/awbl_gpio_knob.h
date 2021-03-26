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
 * \brief 基于标准GPIO驱动的旋钮驱动
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "gpio-knob"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct gpio_knob_data
 *
 * \par 2.兼容设备
 *
 *  - 任何可以使用标准GPIO接口访问的旋钮
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_drv_input_gpio_knob_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_input_knob
 *
 * \internal
 * \par modification history
 * - 130819, orz, first implementation.
 * \endinternal
 */

#ifndef __AWBL_GPIO_KNOB_H /* { */
#define __AWBL_GPIO_KNOB_H

#include "aw_common.h"
#include "awbus_lite.h"
#include "awbl_input.h"
#include "aw_types.h"
#include "aw_delayed_work.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_input_gpio_knob
 * \copydetails awbl_gpio_knob.h
 * @{
 */

/** @} */

/** @brief name of GPIO knob driver */
#define GPIO_KNOB_DEV_NAME       "gpio-knob"

/** @brief max knobs a gpio-knob device can handle */
#define GPIO_KNOB_MAX_KNOBS      32

/******************************************************************************/
struct __gpio_knob_map {
    uint16_t gpio;  /**< \brief GPIO connected to the knob input */
    uint16_t key;   /**  \brief Key value of this input */
};

struct gpio_knob_map {
    struct __gpio_knob_map kmap[2]; /**< \brief knob A and B input map */
};

/**
 * \brief GPIO knob platform parameters
 * \note -  if scan_interval is not 0, the keypad driver will use a timer to
 *          scan the keypad in period, otherwise will use GPIO interrupt.
 */
struct gpio_knob_data {
    void (*plat_init) (void);

    const struct gpio_knob_map *knobmap; /**< \brief knob map */

    aw_bool_t    active_low;/**< \brief gpio polarity */
    uint8_t num_knobs;      /**< \brief number of knobs */
    uint8_t debounce_ms;    /**< \brief knob debounce interval in ms */
    uint8_t scan_interval;  /**< \brief knob scanning interval in ms */
    uint8_t clustered_irq;  /**< \brief irq that all knob GPIOs bundled*/
};
#define to_gpio_knob_data(dev)       ((void *)AWBL_DEVINFO_GET(dev))

/** \brief GPIO knob structure */
struct gpio_knob_dev {
    struct awbl_dev        dev;             /**< \brief AWBus device */
    struct aw_delayed_work work;            /**< \brief knob scan delayed work*/
    uint32_t               knob_state[2];   /**< \brief last state of knobs A */
    uint8_t                event[GPIO_KNOB_MAX_KNOBS];
};
#define to_gpio_knob(dev)            ((struct gpio_knob_dev *)(dev))

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_GPIO_KNOB_H */

/* end of file */
