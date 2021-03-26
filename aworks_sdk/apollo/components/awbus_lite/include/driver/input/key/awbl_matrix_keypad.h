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
 * \brief 基于标准GPIO驱动的矩阵键盘驱动
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "matrix-kp"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct matrix_keypad_par
 *
 * \par 2.兼容设备
 *
 *  - 任何可以使用标准GPIO接口访问的通用矩阵键盘
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_drv_input_matrix_keypad_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_input_key
 *
 * \internal
 * \par modification history
 * - 1.00 13-02-26  orz, first implementation.
 * \endinternal
 */

#ifndef __AWBL_MATRIX_KEYPAD_H
#define __AWBL_MATRIX_KEYPAD_H

#include "aw_common.h"
#include "awbus_lite.h"
#include "awbl_input.h"
#include "awbl_digitron.h"
#include "aw_types.h"
#include "aw_delayed_work.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_input_matrix_keypad
 * \copydetails awbl_matrix_keypad.h
 * @{
 */

/**
 * \defgroup  grp_awbl_drv_input_matrix_keypad_hwconf 设备定义/配置
 *
 * 通用矩阵键盘设备的定义如下面的代码所示，用户在定义目标系统的硬件资源时，
 * 可以直接将这段代码嵌入到 awbus_lite_hwconf_usrcfg.c 中对应的位置，然后对设备
 * 信息进行必要的配置(可配置的项目用【x】标识)。
 *
 * \note 有多少个矩阵键盘设备，就将下面的代码嵌入多少次，注意将变量修改为不同
 *       的名字。
 *
 * \include  hwconf_usrcfg_matrix_keypad.c
 * \note 以下配置项中带【用户配置】标识的，表示用户在开发应用时可以配置此参数。
 *
 * - 【1】矩阵键盘的行GPIO编号表 \n
 * 在这里依次填入与矩阵键盘的“行”线相连的GPIO的编号
 *
 * - 【2】矩阵键盘的列GPIO编号表 \n
 * 在这里依次填入与矩阵键盘的“列”线相连的GPIO的编号
 *
 * - 【3】矩阵键盘键值映射表 \n
 *      - 在这里填入矩阵键盘每个按键对应的键值，键值可使用自定义的值
 *        （如ASCII码）或 aw_input_key.h 头文件中定义的标准键值，推荐使用后者
 *      - 对应关系为从左到右，从上到下。如2x2的键盘，0行0列对应映射表的元素0，
 *        0行1列对应映射表的元素1，1行0列对应映射表的元素2，依次类推
 *
 * - 【4】矩阵键盘平台相关的一些初始化 \n
 * 这些初始化主要包括设置与键盘行列线相连的GPIO的上下拉电阻或特别设计的电路的
 * 特殊初始化等等。 \n
 * 如低电平有效的键盘需要设置上拉电阻，而如果硬件设计了上拉电阻，则不需要设置
 *
 * - 【5】矩阵键盘的“行”数 \n
 * 行数必须和行GPIO编号表【1】中元素个数相同
 *
 * - 【6】矩阵键盘的“列”数 \n
 * 列数必须和列GPIO编号表【2】中元素个数相同
 *
 * - 【7】按键是否为低电平有效 \n
 * 若按键按下为低电平有效则这里填true，否则填false
 *
 * - 【8】中断模式下去抖动延时时间 \n
 * 这个时间用于可中断的键盘消抖，产生按键中断后延时再扫描键盘，一般取10~20毫秒
 *
 * - 【9】定时扫描间隔时间，为0则表示使用中断方式，不为0则使用定时扫描方式 \n
 * 这个时间用于不可中断的键盘，不能产生中断的键盘使用定时扫描的方式扫描按键，
 * 这个时间即为定时扫描的间隔时间，建议为10~20毫秒
 *
 * - 【10】GPIO的“簇”中断号 \n
 * 有些芯片的多个GPIO“共享”同一个中断号，若使用了这种方式，这里填它们的共享
 * 中断号，否则填0
 *
 * - 【11】和矩阵键盘设备复用“行”GPIO的数码管显示设备的GPIO有效极性 \n
 * 这个驱动允许矩阵键盘设备和数码管显示设备复用行GPIO，这里指定其有效极性
 *
 * - 【12】和矩阵键盘设备复用“列”GPIO的数码管显示设备的GPIO有效极性 \n
 * 这个驱动允许矩阵键盘设备和数码管显示设备复用列GPIO，这里指定其有效极性
 *
 * - 【13】和矩阵键盘设备复用GPIO的数码管显示设备的设备实例 \n
 * 若没有复用GPIO，则填NULL
 */

/** @} */

/** @brief name of matrix keyboard driver */
#define MATRIX_KEYPAD_NAME      "matrix-kp"

/** @brief max matrix keyboard rows and columns */
#define MATRIX_MAX_COLS         16
#define MATRIX_MAX_ROWS         16

/** @brief get matrix keyboard scan code */
#define MATRIX_SCAN_CODE(row, col, num_col) (((row) * (num_col)) + (col))

/******************************************************************************/

/**
 * \brief matrix keyboard platform parameters
 * \note -  if scan_interval is not 0, the keypad driver will use a timer to
 *          scan the keypad in period, otherwise will use GPIO interrupt.
 *       - if <digitron> is set, the key pad is scanning in a digitron display
 *         device's scanning black time.
 */
struct matrix_keypad_par {
    void (*plat_init)(void);    /**< \brief platform initialization */

    const uint16_t  *row_gpios; /**< \brief row GPIO number table */
    const uint16_t  *col_gpios; /**< \brief column GPIO number table */
    const uint16_t  *keymap;    /**< \brief key map */

    uint8_t num_row;        /**< \brief number of keyboard rows */
    uint8_t num_col;        /**< \brief number of keyboard columns */
    uint8_t active_low;     /**< \brief gpio polarity */
    uint8_t debounce_ms;    /**< \brief key debounce interval in ms */
    uint8_t scan_interval;  /**< \brief keypad scanning interval in ms */
    uint8_t clustered_irq;  /**< \brief irq that all row/column GPIOs bundled */

    uint8_t dd_row_active_low; /**< \brief polarity of complex row GPIOs */
    uint8_t dd_col_active_low; /**< \brief polarity of complex column GPIOs */

    struct awbl_digitron_dev *digitron; /** \brief the digitron display device */
};

/** \brief get keyboard ops from device */
#define dev_get_matrix_keypad_par(p_dev)    ((void *)AWBL_DEVINFO_GET(p_dev))

/** \brief matrix keyboard structure */
struct matrix_keypad {
    struct awbl_dev        dev;     /**< \brief AWBus device, always go first */
    struct aw_delayed_work work;    /**< \brief timer for scan keypad */

    uint16_t dd_scan_count_max;     /**< \brief scan count maxim in digitron */
    uint16_t dd_scan_count;         /**< \brief scan counter in digitron */

    uint16_t last_key_state[MATRIX_MAX_COLS]; /**< \brief last state of keys */
};

/** \brief convert a awbl_dev pointer to matrix_keypad pointer */
#define dev_to_matrix_keypad(p_dev)     ((struct matrix_keypad *)(p_dev))

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_MATRIX_KEYPAD_H */

/* end of file */
