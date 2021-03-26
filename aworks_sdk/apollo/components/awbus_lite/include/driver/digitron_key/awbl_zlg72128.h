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
 * \brief ZLG72128 接口头文件
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-11  pea, first implementation
 * \endinternal
 */

#ifndef __AWBL_ZLG72128_H
#define __AWBL_ZLG72128_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "aw_sem.h"
#include "aw_i2c.h"
#include "aw_pool.h"
#include "aw_input.h"
#include "awbus_lite.h"
#include "awbl_scanner.h"
#include "awbl_digitron.h"

#define AWBL_ZLG72128_NAME      "awbl_zlg72128"

/**
 * \brief 实际使用的行列宏定义
 *
 * 设备信息中的 key_use_row_flags 与 key_use_col_flags 必须根据实际情况
 * 配置，比如实际使用了第 0 行和第 3 行，则 key_use_row_flags 的值应当配置为
 * AWBL_ZLG72128_KEY_ROW_0 | AWBL_ZLG72128_KEY_ROW_3
 */
#define AWBL_ZLG72128_KEY_ROW_0  (1 << 0) /**< \brief 使用第 0 行 */
#define AWBL_ZLG72128_KEY_ROW_1  (1 << 1) /**< \brief 使用第 1 行 */
#define AWBL_ZLG72128_KEY_ROW_2  (1 << 2) /**< \brief 使用第 2 行 */
#define AWBL_ZLG72128_KEY_ROW_3  (1 << 3) /**< \brief 使用第 3 行（功能按键行）*/

#define AWBL_ZLG72128_KEY_COL_0  (1 << 0) /**< \brief 使用第 0 列 */
#define AWBL_ZLG72128_KEY_COL_1  (1 << 1) /**< \brief 使用第 1 列 */
#define AWBL_ZLG72128_KEY_COL_2  (1 << 2) /**< \brief 使用第 2 列 */
#define AWBL_ZLG72128_KEY_COL_3  (1 << 3) /**< \brief 使用第 3 列 */
#define AWBL_ZLG72128_KEY_COL_4  (1 << 4) /**< \brief 使用第 4 列 */
#define AWBL_ZLG72128_KEY_COL_5  (1 << 5) /**< \brief 使用第 5 列 */
#define AWBL_ZLG72128_KEY_COL_6  (1 << 6) /**< \brief 使用第 6 列 */
#define AWBL_ZLG72128_KEY_COL_7  (1 << 7) /**< \brief 使用第 7 列 */

/**
 * \brief 普通按键（第 1 行 ~ 第 3 行所有按键）键值宏定义
 *
 * 列举出了 awbl_zlg72128_key_cb_t 回调函数中的 key_val 可能的一系列值，可以
 * 使用 key_val 值与这一系列宏值作相应对比，以判断是哪一按键按下。宏的命名规则是
 * AWBL_ZLG72128_KEY_x_y，其中，x 表示哪一行（1 ~ 3），y 表示哪一列（1 ~ 8)
 */
#define  AWBL_ZLG72128_KEY_1_1     1     /**< \brief 第 1 排第 1 个按键 */
#define  AWBL_ZLG72128_KEY_1_2     2     /**< \brief 第 1 排第 2 个按键 */
#define  AWBL_ZLG72128_KEY_1_3     3     /**< \brief 第 1 排第 3 个按键 */
#define  AWBL_ZLG72128_KEY_1_4     4     /**< \brief 第 1 排第 4 个按键 */
#define  AWBL_ZLG72128_KEY_1_5     5     /**< \brief 第 1 排第 5 个按键 */
#define  AWBL_ZLG72128_KEY_1_6     6     /**< \brief 第 1 排第 6 个按键 */
#define  AWBL_ZLG72128_KEY_1_7     7     /**< \brief 第 1 排第 7 个按键 */
#define  AWBL_ZLG72128_KEY_1_8     8     /**< \brief 第 1 排第 8 个按键 */

#define  AWBL_ZLG72128_KEY_2_1     9     /**< \brief 第 2 排第 1 个按键 */
#define  AWBL_ZLG72128_KEY_2_2     10    /**< \brief 第 2 排第 2 个按键 */
#define  AWBL_ZLG72128_KEY_2_3     11    /**< \brief 第 2 排第 3 个按键 */
#define  AWBL_ZLG72128_KEY_2_4     12    /**< \brief 第 2 排第 4 个按键 */
#define  AWBL_ZLG72128_KEY_2_5     13    /**< \brief 第 2 排第 5 个按键 */
#define  AWBL_ZLG72128_KEY_2_6     14    /**< \brief 第 2 排第 6 个按键 */
#define  AWBL_ZLG72128_KEY_2_7     15    /**< \brief 第 2 排第 7 个按键 */
#define  AWBL_ZLG72128_KEY_2_8     16    /**< \brief 第 2 排第 8 个按键 */

#define  AWBL_ZLG72128_KEY_3_1     17    /**< \brief 第 3 排第 1 个按键 */
#define  AWBL_ZLG72128_KEY_3_2     18    /**< \brief 第 3 排第 2 个按键 */
#define  AWBL_ZLG72128_KEY_3_3     19    /**< \brief 第 3 排第 3 个按键 */
#define  AWBL_ZLG72128_KEY_3_4     20    /**< \brief 第 3 排第 4 个按键 */
#define  AWBL_ZLG72128_KEY_3_5     21    /**< \brief 第 3 排第 5 个按键 */
#define  AWBL_ZLG72128_KEY_3_6     22    /**< \brief 第 3 排第 6 个按键 */
#define  AWBL_ZLG72128_KEY_3_7     23    /**< \brief 第 3 排第 7 个按键 */
#define  AWBL_ZLG72128_KEY_3_8     24    /**< \brief 第 3 排第 8 个按键 */

/**
 * \brief 功能按键（第 4 行所有按键）编号宏定义
 *
 * 功能按键从第 4 行开始，从左往右，依次定义为 F0 ~ F7，其状态分别与按键回调函数
 * 中的 funkey_val 值的 bit0 ~ bit7 对应。0 为按下，1 为释放。由于 funkey_val 是 8 个
 * 功能键的组合值，可以直接使用这个 8 位数值实现较为复杂的按键控制。若只需要简单
 * 的检测其中某个功能键的状态，可以使用 #AWBL_ZLG72128_FUNKEY_CHECK() 宏检查某一功
 * 能键的状态。#AWBL_ZLG72128_FUNKEY_CHECK() 宏的 funkey_num 参数为 0 ~ 7，建议使用
 * 此处定义的宏值：#AWBL_ZLG72128_FUNKEY_0 ~ #AWBL_ZLG72128_FUNKEY_7
 */
#define  AWBL_ZLG72128_FUNKEY_0     0     /**< \brief 功能键 1 */
#define  AWBL_ZLG72128_FUNKEY_1     1     /**< \brief 功能键 2 */
#define  AWBL_ZLG72128_FUNKEY_2     2     /**< \brief 功能键 3 */
#define  AWBL_ZLG72128_FUNKEY_3     3     /**< \brief 功能键 4 */
#define  AWBL_ZLG72128_FUNKEY_4     4     /**< \brief 功能键 4 */
#define  AWBL_ZLG72128_FUNKEY_5     5     /**< \brief 功能键 5 */
#define  AWBL_ZLG72128_FUNKEY_6     6     /**< \brief 功能键 6 */
#define  AWBL_ZLG72128_FUNKEY_7     7     /**< \brief 功能键 7 */

/**
 * \brief 数码管显示移位的方向
 *
 * 用于 awbl_zlg72128_digitron_shift() 函数的 dir 参数。
 */
#define  AWBL_ZLG72128_DIGITRON_SHIFT_RIGHT   0   /**< \brief 右移 */
#define  AWBL_ZLG72128_DIGITRON_SHIFT_LEFT    1   /**< \brief 左移 */

/**
 * \brief 功能按键（第 4 行所有按键）状态检测宏
 *
 * 功能按键从第 4 行开始，从左往右，依次定义为 F0 ~ F7，其状态分别与按键回调函数
 * 中的 funkey_val 值的 bit0 ~ bit7 对应。0 为按下，1 为释放。由于 funkey_val 是 8 个
 * 功能键的组合值，可以直接使用这个 8 位数值实现较为复杂的按键控制。若只需要简单
 * 的检测其中某个功能键的状态，可以使用该宏检查某一功能键的状态。
 *
 * \param[in] funkey_val 按键回调函数中获取到的功能键键值
 * \param[in] funkey_num 功能键编号，有效值：0 ~ 7，分别对应 F0 ~ F7，建议使用
 *                       宏值 #AWBL_ZLG72128_FUNKEY_0 ~ #AWBL_ZLG72128_FUNKEY_7
 *
 * \retval AW_TRUE  对应功能键当前是按下状态
 * \retval AW_FALSE 对应功能键当前是释放状态
 */
#define  AWBL_ZLG72128_FUNKEY_CHECK(funkey_val, funkey_num)  \
               (((funkey_val) & (1 << ((funkey_num) & 0x07))) ? AW_FALSE : AW_TRUE)

/**
 * \brief 数码管段
 *
 * 用于 awbl_zlg72128_digitron_seg_ctrl() 段控制函数的 seg 参数。
 */
#define  AWBL_ZLG72128_DIGITRON_SEG_A    0        /**< \brief a  段 */
#define  AWBL_ZLG72128_DIGITRON_SEG_B    1        /**< \brief b  段 */
#define  AWBL_ZLG72128_DIGITRON_SEG_C    2        /**< \brief c  段 */
#define  AWBL_ZLG72128_DIGITRON_SEG_D    3        /**< \brief d  段 */
#define  AWBL_ZLG72128_DIGITRON_SEG_E    4        /**< \brief e  段 */
#define  AWBL_ZLG72128_DIGITRON_SEG_F    5        /**< \brief f  段 */
#define  AWBL_ZLG72128_DIGITRON_SEG_G    6        /**< \brief g  段 */
#define  AWBL_ZLG72128_DIGITRON_SEG_DP   7        /**< \brief dp 段 */

/**
 * \brief 定义按键回调函数类型
 *
 * 可以使用 awbl_zlg72128_key_cb_set() 函数设置该回调函数至系统之中，当有按键
 * 事件发生时，调用回调函数
 *
 * \param[in] p_arg      设置回调函数时设定的用户自定义参数
 *
 * \param[in] key_val    键值，从第 1 行开始，从左至右顺序编号。第 1 行第 1 个按键
 *                       键值为 1，第 1 行第 2 个按键键值为 2，以此类推。建议使用
 *                       该值直接与 AWBL_ZLG72128_KEY_*_*(#AWBL_ZLG72128_KEY_1_1)
 *                       比较，以判断是哪一按键按下。特殊地，0 表示无任何普通
 *                       按键按下
 *
 * \param[in] repeat_cnt 按键连击次数，普通按键支持连击，该值仅用于辅助修饰
 *                       key_val0 对应的普通按键的连击次数，与 funkey_val 表征
 *                       的功能键没有关系。0 表示首次按键，未连击。其余值表
 *                       示识别到的连击次数。当按键按住时间超过 2s 后，开始连
 *                       续有效，连续有效的时间间隔约 200ms，即每隔 200ms，
 *                       repeat_cnt 加 1，并调用一次该函数，直到按键释放。该值
 *                       最大为 255，若连击次数大于 255 后，则将进入持续连击阶段，
 *                       即以没有时间间隔来调用回调函数
 *
 * \param[in] funkey_val 功能键键值。共计 8 位，分别与第 4 行各个按键对应，最高位
 *                       与第 4 行第 1 个按键对应，次高位与第 4 行第 2 个按键对应，最
 *                       低位与第 4 行第 8 个按键对应。位值为 0 表明对应键按下，位
 *                       值为 1 表明对应键未按下。只要该值发生改变，就会调用设置
 *                       的回调函数。可以使用宏：#AWBL_ZLG72128_FUNKEY_CHECK() 来
 *                       判断对应的功能键是否按下
 *
 * \note 基于功能键和普通键，很容易实现组合键应用，类似 PC 机上的 Alt、Shift 等按键。如
 *       经常用到的一个功能，编辑代码时，TAB 键是右移，如果按住 Shift 键，则 TAB 键
 *       变为左移。可以很容易实现类似的功能，如功能键 0 来当做 Shift 键，普通按键 0 当做
 *       TAB 键。在回调函数中的处理示例代码如：
 *
 * \code
 *  // 首先应使用 awbl_zlg72128_key_cb_set() 函数将该函数注册进系统，以便在合适的时
 *  // 候调用该回调函数
 *  static void zlg72128_key_callback (void    *p_arg,
 *                                     uint8_t  key_val,
 *                                     uint8_t  repeat_cnt,
 *                                     uint8_t  funkey_val)
 *
 *  {
 *       if (keyval == AWBL_ZLG72128_KEY_1_1) {
 *
 *           // 功能键按下，左移
 *           if (AWBL_ZLG72128_FUNKEY_CHECK(fun_keyval, AWBL_ZLG72128_FUNKEY_0)) {
 *
 *           } else {  // 功能键未按下，正常右移
 *
 *           }
 *      }
 *  }
 * \endcode
 */
typedef void (*awbl_zlg72128_key_cb_t) (void    *p_arg,
                                        uint8_t  key_val,
                                        uint8_t  repeat_cnt,
                                        uint8_t  funkey_val);

/**
 * \brief ZLG72128 消息
 */
typedef struct awbl_zlg72128_msg {
    aw_pool_id_t      pool_id;   /**< \brief 内存池句柄 */
    aw_i2c_message_t  i2c_msg;   /**< \brief I2C 消息 */
    aw_i2c_transfer_t trans[2];  /**< \brief I2C 传输 */
    uint8_t           buf[13];   /**< \brief 缓冲区 */
} awbl_zlg72128_msg_t;

/**
 * \brief ZLG72128 设备信息结构体
 */
typedef struct awbl_zlg72128_devinfo {

    /** \brief 数码管显示器 ID */
    int                             digitron_disp_id;

    /** \brief ZLG72128 复位引脚，不使用时，如，固定为高电平时，设置为 -1 */
    int                             rst_pin;

    /** \brief ZLG72128 中断引脚，不使用时，设置为 -1，使用轮询方式读取按键状态 */
    int                             int_pin;

    /** \brief 仅当 int_pin 为 -1 时，该参数有效，指定按键状态读取间隔 */
    uint16_t                        interval_ms;

    /** \brief 实际使用的数码管个数 */
    uint8_t                         digitron_num;

    /** \brief 一个闪烁周期内，点亮的时间，如 500ms */
    uint16_t                        blink_on_time;

    /** \brief 一个闪烁周期内，熄灭的时间，如 500ms */
    uint16_t                        blink_off_time;

    /** \brief 实际使用的行标志 */
    uint8_t                         key_use_row_flags;

    /** \brief 实际使用的列标志 */
    uint8_t                         key_use_col_flags;

    /** \brief 按键编码， 其大小为 key_rows * key_cols */
    const int                      *p_key_codes;

} awbl_zlg72128_devinfo_t;

/**
 * \brief ZLG72128 设备结构体
 */
typedef struct awbl_zlg72128_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev                      dev;

    /** \brief 数码管设备 */
    struct awbl_digitron_dev             digitron_dev;

    /** \brief 数码管矩阵配置信息 */
    struct awbl_digitron_disp_param      disp_param;

    /** \brief 功能键值，将功能键转换为通用按键使用 */
    volatile uint8_t                     f_key;

    /** \brief 实际使用的按键行数 */
    uint8_t                              num_rows;

    /** \brief 实际使用按键的列数 */
    uint8_t                              num_cols;

    /** \brief I2C 从机设备 */
    aw_i2c_device_t                      i2c_dev;

    /** \brief 用于 I2C 数据传输 */
    aw_i2c_transfer_t                    trans[2];

    /** \brief 消息，内部使用 */
    aw_i2c_message_t                     msg;

    /** \brief 状态 */
    volatile uint8_t                     state;

    /** \brief 保存普通键回调函数 */
    awbl_zlg72128_key_cb_t               pfn_key_cb;

    /** \brief 保存回调函数参数 */
    void                                *pfn_key_arg;

    /** \brief 键值相关的 4 个寄存器 */
    uint8_t                              key_buf[4];

    /** \brief 扫描器设备 */
    struct awbl_scanner_dev              scan_dev;

    /** \brief 扫描器配置参数 */
    struct awbl_scanner_param            scan_param;

    /** \brief 扫描器回调函数句柄 */
    struct awbl_scan_cb_handler          scan_cb_handler;

    /** \brief 内存池对象 */
    aw_pool_t                            pool;

    /** \brief 内存池句柄 */
    aw_pool_id_t                         pool_id;

    /** \brief 内存池句柄 */
    awbl_zlg72128_msg_t                  bulk[10];

} awbl_zlg72128_dev_t;

/**
 * \brief ZLG72128 句柄定义
 */
typedef awbl_zlg72128_dev_t *awbl_zlg72128_handle_t;

/**
 * \brief 按键回调函数注册
 *
 * 当有按键事件发生时，将调用此处设置的回调函数。按键事件包括以下 3 种情况：
 *    1. 有普通按键按下
 *    2. 普通按键一直按下，处于连击状态
 *    3. 任一功能按键状态发生变化，释放->按下、按下->释放
 *
 * \param[in] id         设备号
 * \param[in] pfn_key_cb 按键回调函数
 * \param[in] p_arg      回调函数的参数
 *
 * \retval  AW_OK     设置回调函数成功
 * \retval -AW_EINVAL 参数错误
 */
aw_err_t awbl_zlg72128_key_cb_set (int                     id,
                                   awbl_zlg72128_key_cb_t  pfn_key_cb,
                                   void                   *p_arg);

/**
 * \brief 设置数码管闪烁时间，即当设定某位闪烁时点亮持续的时间和熄灭持续的时间
 *
 *     出厂设置为：亮和灭的时间均为500ms。时间单位为ms，有效的时间为：
 *  150、200、250、……、800、850、900，即 150ms ~ 900ms，且以50ms为间距。
 *  若时间不是恰好为这些值，则会设置一个最接近的值。
 *
 * \param[in] id     设备号
 * \param[in] on_ms  点亮持续的时间，有效值为 150ms ~ 900ms，且以50ms为间距
 * \param[in] off_ms 熄灭持续的时间，有效值为 150ms ~ 900ms，且以50ms为间距
 *
 * \retval  AW_OK     设置成功
 * \retval -AW_EINVAL 无效参数
 * \retval -AW_EIO    设置失败，数据传输错误
 *
 * \note 仅设置闪烁时间并不能立即看到闪烁，还必须打开某位的闪烁开关后方能看到
 * 闪烁现象。awbl_zlg72128_digitron_flash_ctrl()
 */
aw_err_t awbl_zlg72128_digitron_flash_time_cfg (int      id,
                                                uint16_t on_ms,
                                                uint16_t off_ms);

/**
 * \brief 控制（打开或关闭）数码管闪烁，默认均不闪烁
 *
 * \param[in] id       设备号
 * \param[in] ctrl_val 控制值，位0 ~ 11分别对应从左至右的数码管。值为1时闪烁，
 *                     值为0时不闪烁。
 *
 * \retval  AW_OK     设置成功
 * \retval -AW_EINVAL 无效参数
 * \retval -AW_EIO    设置失败，数据传输错误
 */
aw_err_t awbl_zlg72128_digitron_flash_ctrl (int id, uint16_t ctrl_val);

/**
 * \brief 控制数码管的显示属性（打开显示或关闭显示）
 *
 *     默认情况下，所有数码管均处于打开显示状态，则将按照12位数码管扫描，实际中，
 *  可能需要显示的位数并不足12位，此时，即可使用该函数关闭某些位的显示。
 *
 * \param[in] id       设备号
 * \param[in] ctrl_val 控制值，位0 ~ 位11分别对应从左至右的数码管。
 *                     值为1关闭显示，值为0打开显示
 *
 * \retval  AW_OK     设置成功
 * \retval -AW_EINVAL 无效参数
 * \retval -AW_EIO    设置失败，数据传输错误
 *
 * \note 这与设置段码为熄灭段码不同，设置段码为熄灭段码时，数码管同样被扫描，同
 * 样需要在每次扫描时填充相应的段码。使用该函数关闭某些位的显示后，是从根本上不
 * 再扫描该位。
 */
aw_err_t awbl_zlg72128_digitron_disp_ctrl (int id, uint16_t ctrl_val);

/**
 * \brief 控制数码管的扫描位数
 *
 *     在使用过程中，如果不需要 12 位数码管显示，从最高位开始裁剪，同时将数码扫描位
 * 数设置成相应的数码管位数。
 *
 * \param[in] id       设备号
 * \param[in] ctrl_val 控制值，位0 ~ 位11分别对应从左至右的数码管。
 *                     值为1关闭显示，值为0打开显示
 *
 * \retval  AW_OK     设置成功
 * \retval -AW_EINVAL 无效参数
 * \retval -AW_EIO    设置失败，数据传输错误
 */
aw_err_t awbl_zlg72128_digitron_scan_ctrl (int id, uint8_t ctrl_val);

/**
 * \brief 直接设置数码管显示的内容（段码）
 *
 *      当用户需要显示一些自定义的特殊图形时，可以使用该函数，直接设置段码。一
 *  般来讲，ZLG72128 已经提供了常见的10种数字和21种字母的直接显示，用户不必使用
 *  此函数直接设置段码，可以直接使用相关函数显示数字或字母
 *      为方便快速设置多个数码管位显示的段码，本函数可以一次连续写入多个数码管
 *  显示的段码。
 *
 * \param[in] id        设备号
 * \param[in] start_pos 本次设置段码起始位置，有效值 0 ~ 11
 * \param[in] p_buf     段码存放的缓冲区
 * \param[in] num       本次设置的数码管段码的个数
 *
 * \retval  AW_OK     设置成功
 * \retval -AW_EINVAL 无效参数
 * \retval -AW_EIO    设置失败，数据传输错误
 *
 * \note 若 start_pos + num 的值超过 12 ，则多余的缓冲区内容被丢弃。
 */
aw_err_t awbl_zlg72128_digitron_dispbuf_set (int      id,
                                             uint8_t  start_pos,
                                             uint8_t *p_buf,
                                             uint8_t  num);

/**
 * \brief 获取数码管显示的内容（段码）
 *
 * \param[in] id        设备号
 * \param[in] start_pos 本次设置段码起始位置，有效值 0 ~ 11
 * \param[in] p_buf     段码存放的缓冲区
 * \param[in] num       本次设置的数码管段码的个数
 *
 * \retval  AW_OK     设置成功
 * \retval -AW_EINVAL 无效参数
 * \retval -AW_EIO    设置失败，数据传输错误
 */
aw_err_t awbl_zlg72128_digitron_dispbuf_get (int      id,
                                             uint8_t  start_pos,
                                             uint8_t *p_buf,
                                             uint8_t  num);

/**
 * \brief 设置数码管显示的字符
 *
 *      ZLG72128已经提供了常见的10种数字和21种字母的直接显示。若需要显示不支持的
 *  图形，可以直接使用 awbl_zlg72128_digitron_dispbuf_set() 函数直接设置段码。
 *
 * \param[in] id         设备号
 * \param[in] pos        本次显示的位置，有效值 0 ~ 11
 * \param[in] ch         显示的字符，支持字符 0123456789AbCdEFGHiJLopqrtUychT
 *                       以及空格（无显示）
 * \param[in] is_dp_disp 是否显示小数点，TRUE:显示; AW_FALSE:不显示
 * \param[in] is_flash   该位是否闪烁，TRUE:闪烁; AW_FALSE:不闪烁
 *
 * \retval  AW_OK      设置成功
 * \retval -AW_EINVAL  无效参数
 * \retval -AW_ENOTSUP 不支持的字符
 * \retval -AW_EIO     设置失败，数据传输错误
 */
aw_err_t awbl_zlg72128_digitron_disp_char (int        id,
                                           uint8_t    pos,
                                           char       ch,
                                           aw_bool_t  is_dp_disp,
                                           aw_bool_t  is_flash);

/**
 * \brief 从指定位置开始显示一个字符串
 *
 * \param[in] id          设备号
 * \param[in] start_pos   字符串起始位置
 * \param[in] p_str       显示的字符串
 * \param[in] flash_flags 字符串对应的字符是否闪烁，第 0 位代表字符串中的第
 *                        一个字符是否闪烁，，第 1 位代表字符串中的第二个字
 *                        符是否闪烁，以此类推
 *
 * \retval  AW_OK      设置成功
 * \retval -AW_EINVAL  无效参数
 * \retval -AW_ENOTSUP 不支持的字符
 * \retval -AW_EIO     设置失败，数据传输错误
 *
 * \note 若遇到不支持的字符，该字符位将不显示
 */
aw_err_t awbl_zlg72128_digitron_disp_str (int         id,
                                          uint8_t     start_pos,
                                          const char *p_str,
                                          uint8_t     flash_flags);
/**
 * \brief 设置数码管显示的数字（0 ~ 9）
 *
 * \param[in] id         设备号
 * \param[in] pos        本次显示的位置，有效值 0 ~ 11
 * \param[in] num        显示的数字，0 ~ 9
 * \param[in] is_dp_disp 是否显示小数点，TRUE:显示; AW_FALSE:不显示
 * \param[in] is_flash   该位是否闪烁，TRUE:闪烁; AW_FALSE:不闪烁
 *
 * \retval  AW_OK     设置成功
 * \retval -AW_EINVAL 无效参数
 * \retval -AW_EIO    设置失败，数据传输错误
 */
aw_err_t awbl_zlg72128_digitron_disp_num (int        id,
                                          uint8_t    pos,
                                          uint8_t    num,
                                          aw_bool_t  is_dp_disp,
                                          aw_bool_t  is_flash);

/**
 * \brief 直接控制段的点亮或熄灭
 *
 *     为了更加灵活的控制数码管显示，提供了直接控制某一段的属性（亮或灭）。
 *
 * \param[in] id    设备号
 * \param[in] pos   控制段所处的位，有效值 0 ~ 11
 * \param[in] seg   段，有效值 0 ~ 7，分别对应：a,b,c,d,e,f,g,dp
 *                  建议使用下列宏：
 *                    - AWBL_ZLG72128_DIGITRON_SEG_A
 *                    - AWBL_ZLG72128_DIGITRON_SEG_B
 *                    - AWBL_ZLG72128_DIGITRON_SEG_C
 *                    - AWBL_ZLG72128_DIGITRON_SEG_D
 *                    - AWBL_ZLG72128_DIGITRON_SEG_E
 *                    - AWBL_ZLG72128_DIGITRON_SEG_F
 *                    - AWBL_ZLG72128_DIGITRON_SEG_G
 *                    - AWBL_ZLG72128_DIGITRON_SEG_DP
 * \param[in] is_on 是否点亮该段，TRUE:点亮; AW_FALSE:熄灭
 *
 * \retval  AW_OK     设置成功
 * \retval -AW_EINVAL 无效参数
 * \retval -AW_EIO    设置失败，数据传输错误
 */
aw_err_t awbl_zlg72128_digitron_seg_ctrl (int        id,
                                          uint8_t    pos,
                                          char       seg,
                                          aw_bool_t  is_on);

/**
 * \brief 显示移位命令
 *
 *      使当前所有数码管的显示左移或右移，并可指定是否是循环移动。如果不是循环
 *   移位，则移位后，右边空出的位（左移）或左边空出的位（右移）将不显示任何内容。
 *   若是循环移动，则空出的位将会显示被移除位的内容。
 *
 * \param[in] id        设备号
 * \param[in] dir       移位方向
 *                        - AWBL_ZLG72128_DIGITRON_SHIFT_LEFT   左移
 *                        - AWBL_ZLG72128_DIGITRON_SHIFT_RIGHT  右移
 *
 * \param[in] is_cyclic 是否是循环移位，TRUE，循环移位；FALSE，不循环移位
 * \param[in] num       本次移位的位数，有效值 0（不移动） ~ 11，其余值无效
 *
 * \retval  AW_OK     设置成功
 * \retval -AW_EINVAL 无效参数
 * \retval -AW_EIO    设置失败，数据传输错误
 *
 * \note 关于移位的说明
 *
 *   实际中，可能会发现移位方向相反，这是由于可能存在两种硬件设计：
 *   1、 最右边为0号数码管，从左至右为：11、10、9、8、7、6、5、4、3、2、 1、 0
 *   2、 最左边为0号数码管，从左至右为： 0、 1、2、3、4、5、6、7、8、9、10、11
 *   这主要取决于硬件设计时 COM0 ~ COM11 引脚所对应数码管所处的物理位置。
 *
 *       此处左移和右移的概念是以《数据手册》中典型应用电路为参考的，即最右边
 *   为0号数码管。那么左移和右移的概念分别为：
 *   左移：数码管0显示切换到1，数码管1显示切换到2，……，数码管10显示切换到11
 *   右移：数码管11显示切换到10，数码管1显示切换到2，……，数码管10显示切换到11
 *
 *   可见，若硬件电路设计数码管位置是相反的，则移位效果可能也恰恰是相反的，此处
 * 只需要稍微注意即可。
 */
aw_err_t awbl_zlg72128_digitron_shift (int        id,
                                       uint8_t    dir,
                                       aw_bool_t  is_cyclic,
                                       uint8_t    num);

/**
 * \brief 复位命令，将数码管的所有 LED 段熄灭
 *
 * \param[in] id 设备号
 *
 * \retval  AW_OK     设置成功
 * \retval -AW_EINVAL 无效参数
 * \retval -AW_EIO    设置失败，数据传输错误
 */
aw_err_t awbl_zlg72128_digitron_disp_reset (int id);

/**
 * \brief 测试命令，所有 LED 段以 0.5S 的速率闪烁，用于测试数码管显示是否正常
 *
 * \param[in] id 设备号
 *
 * \retval  AW_OK     设置成功
 * \retval -AW_EINVAL 无效参数
 * \retval -AW_EIO    设置失败，数据传输错误
 */
aw_err_t awbl_zlg72128_digitron_disp_test (int id);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_ZLG72128_H */

/* end of file */
