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
 * \brief AWorks GPIO 标准接口
 *
 * 使用本服务需要包含头文件 aw_gpio.h
 *
 * 本服务提供了设置引脚功能模式，以及读写GPIO引脚输入输出电平的能力。
 *
 * \par 简单示例
 * \code
 * #include "aw_gpio.h"
 *
 * // 配置引脚 PIO0_11 为GPIO输入功能，上拉电阻使能
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
 *
 * val = aw_gpio_get(PIO0_11); //读取引脚 PIO0_11 的输入值
 *
 * // 配置引脚 PIO0_11 为GPIO输出功能
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_OUTPUT);
 *
 * aw_gpio_set(PIO0_11, 0);    //设置引脚 PIO0_11 的输出值为0(低电平)
 * val = aw_gpio_get(PIO0_11); //读取引脚 PIO0_11 当前的输出值
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.10 13-04-23  zen, add api aw_gpio_toggle()
 * - 1.00 12-08-23  zen, first implementation
 * \endinternal
 */

#ifndef __AW_GPIO_H
#define __AW_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_gpio
 * \copydoc aw_gpio.h
 * @{
 */

#include "aw_psp_gpio.h"
#include "aw_gpio_util.h"

/**
 * \name 引脚功能
 * @{
 */

/** \brief GPIO输入功能 */
#define AW_GPIO_INPUT             AW_GPIO_FUNCBITS_CODE(AW_GPIO_INPUT_VAL)

/** \brief GPIO输出功能 */
#define AW_GPIO_OUTPUT            AW_GPIO_FUNCBITS_CODE(AW_GPIO_OUTPUT_VAL)

/** \brief GPIO输出初始为高 */
#define AW_GPIO_OUTPUT_INIT_HIGH  AW_GPIO_FUNCBITS_CODE(AW_GPIO_OUTPUT_INIT_HIGH_VAL)

/** \brief GPIO输出初始为低 */
#define AW_GPIO_OUTPUT_INIT_LOW   AW_GPIO_FUNCBITS_CODE(AW_GPIO_OUTPUT_INIT_LOW_VAL)

/** @} */

/**
 * \name 引脚模式
 * @{
 */

/** \brief 引脚上拉电阻使能 */
#define AW_GPIO_PULL_UP         AW_GPIO_MODEBITS_CODE(AW_GPIO_PULL_UP_VAL)

/** \brief 引脚下拉电阻使能 */
#define AW_GPIO_PULL_DOWN       AW_GPIO_MODEBITS_CODE(AW_GPIO_PULL_DOWN_VAL)

/** \brief 引脚悬空(引脚上下拉电阻禁能) */
#define AW_GPIO_FLOAT           AW_GPIO_MODEBITS_CODE(AW_GPIO_FLOAT_VAL)

/** \brief 引脚开漏 */
#define AW_GPIO_OPEN_DRAIN      AW_GPIO_MODEBITS_CODE(AW_GPIO_OPEN_DRAIN_VAL)

/** \brief 引脚推挽输出 */
#define AW_GPIO_PUSH_PULL       AW_GPIO_MODEBITS_CODE(AW_GPIO_PUSH_PULL_VAL)

/** @} */

/**
 * \name 引脚触发条件
 * @{
 */

/** \brief 触发关闭 */
#define AW_GPIO_TRIGGER_OFF             0

/** \brief 高电平触发 */
#define AW_GPIO_TRIGGER_HIGH            1

/** \brief 低电平触发 */
#define AW_GPIO_TRIGGER_LOW             2

/** \brief 上升沿触发 */
#define AW_GPIO_TRIGGER_RISE            3

/** \brief 下降沿触发 */
#define AW_GPIO_TRIGGER_FALL            4

/** \brief 双边沿触发 */
#define AW_GPIO_TRIGGER_BOTH_EDGES      5

/** @} */

/**
 * \brief 配置引脚的功能、模式等参数
 *
 * 通过设置 \a flags 的值来达到配置引脚功能模式等参数的目的，\a flags 格式为：
 * (<b>功能|模式|平台定义</b>)
 *
 *  - “功能”定义:
 *      - #AW_GPIO_INPUT
 *      - #AW_GPIO_OUTPUT
 *  - “模式”定义:
 *      - #AW_GPIO_PULL_UP
 *      - #AW_GPIO_PULL_DOWN
 *      - #AW_GPIO_FLOAT
 *      - #AW_GPIO_OPEN_DRAIN
 *      - #AW_GPIO_PUSH_PULL
 *
 * “功能”和“模式”标志域合称为“通用定义”标志域。“通用定义”
 * 部分每个标志域都是可选的，未设置的标志域则保持当前配置不变，例如，
 * <code> aw_gpio_pin_cfg(PIO0_1, AW_GPIO_INPUT) </code>
 * 只会将引脚 PIO0_1 的“功能”设置为“GPIO输入”功能，而其“模式”
 * 保持当前的配置不变。
 *
 * 具有外部中断功能的引脚可以连接触发回调函数，当引脚的输入电平满足
 * “触发条件”并且“触发使能”开启时，将会调用连接到引脚上的回调函数。
 * 触发回调函数的连接请参考函数 aw_gpio_trigger_connect()。
 *
 * “平台定义”标志域由平台定义，实现平台特定参数的配置，如“平台定义”
 * 标志域与“通用定义”标志域有重复的定义，则前者的配置将取代后者的配置，
 * 例如，<code> aw_gpio_pin_cfg(PIO0_1, AW_GPIO_INPUT| PIO0_1_OUTPUT) </code>
 * 会将 PIO0_1 设置为“GPIO输出功能”。
 *
 * \par 实现相关：
 *  - 平台可以不完全支持“通用定义”标志域的定义
 *  - 推荐“平台定义”标志域的格式为
 *    (<b>引脚功能|引脚模式|引脚其它类型配置参数</b>)
 *
 * \attention 不推荐同时使用“通用定义”标志域和“平台定义”标志域
 *
 * \param[in] pin       引脚编号
 * \param[in] flags     配置标志，其格式请参考上面的详细描述
 *
 * \retval AW_OK        操作成功
 * \retval -AW_ENXIO    \a pin 不存在
 * \retval -AW_EIO      操作出错
 *
 * \par 示例：配置引脚功能 (使用“通用定义”标志域)
 * \code
 * #include "aw_gpio.h"
 *
 * // 配置引脚PIO0_1为GPIO输入功能
 * aw_gpio_pin_cfg(PIO0_1, AW_GPIO_INPUT);
 *
 * // 配置引脚PIO0_1为GPIO输入功能，并使能引脚上拉电阻
 * aw_gpio_pin_cfg(PIO0_1, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
 *
 * // 配置引脚PIO0_1为GPIO输出功能，并使能引脚上拉电阻
 * aw_gpio_pin_cfg(PIO0_1, AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);
 * \endcode
 */
aw_err_t aw_gpio_pin_cfg(int pin, uint32_t flags);


/**
 * \brief 申请一组引脚被某外设/模块使用
 *
 * \param[in] p_name  引脚被使用的外设/模块名
 * \param[in] p_pins  指向引脚列表
 * \param[in] num     引脚数
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  引脚被占用
 * \retval -AW_ENXIO  \a pin 不存在
 *
 * \par 示例
 * \code
 *
 * static const uart0_pins_tabe[] = {
 *     PIO0_0,
 *     PIO0_1
 * };
 * if (aw_gpio_pin_request("UART0",
 *                          uart0_pins_tabe,
 *                          AW_NELEMENTS(uart0_pins_tabe)) == AW_OK) {
 *
 *     aw_gpio_pin_cfg(PIO0_0, PIO0_0_USART0_RXD | PIO0_0_PULLUP);
 *     aw_gpio_pin_cfg(PIO0_1, PIO0_1_USART0_TXD | PIO0_1_PULLUP);
 *
 *  }
 *
 * \endcode
 */
aw_err_t aw_gpio_pin_request(const char *p_name, const int *p_pins, int num);


/**
 * \brief 释放一组引脚，清除每个引脚的使用记录。
 *
 * \param[in] p_pins  指向引脚列表
 * \param[in] num     引脚数
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_ENXIO  \a pin 不存在
 *
 * \par 示例
 * \code
 * static const uart0_pins_tabe[] = {
 *     PIO0_0,
 *     PIO0_1
 * };
 * aw_gpio_pin_release(uart0_pins_tabe, AW_NELEMENTS(uart0_pins_tabe));
 *
 * \endcode
 */
aw_err_t aw_gpio_pin_release(const int *p_pins, int num);

/**
 * \brief 读取GPIO引脚的输入值/输出值
 *
 * 读取引脚 \a pin 当前的输入值(\a pin 被配置为GPIO输入功能)或输出值
 * (\a pin 被配置为GPIO输出功能)
 *
 * \par 实现相关
 * 当引脚被配置为GPIO输出功能时， aw_gpio_get() 返回的可以是设置的输出值，
 * 也可以是引脚上的实际电平
 *
 * \param[in] pin   引脚编号
 *
 * \retval  0         \a pin 的输入值或输出值为0(低电平)
 * \retval  1         \a pin 的输入值或输出值为1(高电平)
 * \retval -AW_ENXIO  \a pin 不存在
 *
 * \par 示例
 * \code
 * #include "aw_gpio.h"
 *
 * int val;
 *
 * // 配置引脚 PIO0_11 为GPIO输入功能，上拉电阻使能
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
 *
 * val = aw_gpio_get(PIO0_11); //读取引脚 PIO0_11 的输入值
 *
 * // 配置引脚 PIO0_11 为GPIO输出功能
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_OUTPUT);
 *
 * val = aw_gpio_get(PIO0_11); //读取引脚 PIO0_11 当前的输出值
 * \endcode
 */
aw_err_t aw_gpio_get(int pin);

/**
 * \brief 设置GPIO引脚的输出值
 *
 * 设置引脚 \a pin 的输出值 (\a pin 需要被配置为GPIO输出功能)
 *
 * \param[in] pin   引脚编号
 * \param[in] value 引脚的输出值。为0时，引脚输出0(低电平)；为其它非0值时，
 *                  引脚输出1(高电平)
 *
 * \retval AW_OK      成功
 * \retval -AW_ENXIO  \a pin 不存在
 * \retval -AW_EIO    无法设置\a pin 的输出值 (例如，pin被配置为GPIO输入功能)
 *
 * \par 示例
 * \code
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_OUTPUT); // 设置 PIO0_11 为GPIO输出功能
 * aw_gpio_set(PIO0_11, 0);                  // PIO0_11 输出0 (低电平)
 * aw_gpio_set(PIO0_11, 1);                  // PIO0_11 输出1 (高电平)
 * \endcode
 */
aw_err_t aw_gpio_set(int pin, int value);

/**
 * \brief 翻转GPIO引脚的输出值
 *
 * 翻转GPIO引脚 \a pin 的输出值 (\a pin 需要被配置为GPIO输出功能):
 * 高电平变低电平; 低电平变高电平。
 *
 * \param[in] pin   引脚编号
 *
 * \retval  0         \a pin 被翻转为低电平
 * \retval  1         \a pin 被翻转为高电平
 * \retval -AW_ENXIO  \a pin 不存在
 * \retval -AW_EIO    无法设置\a pin 的输出值 (例如，pin被配置为GPIO输入功能)
 *
 * \par 示例
 * \code
 * #include "aw_gpio.h"
 *
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_OUTPUT); // 设置 PIO0_11 为GPIO输出功能
 * aw_gpio_toggle(PIO0_11);                  // 翻转 PIO0_11 的输出电平
 * \endcode
 */
aw_err_t aw_gpio_toggle(int pin);

/**
 * \brief 配置引脚的触发条件
 *
 * 具有外部中断功能的引脚可以连接回调函数，当引脚的输入电平/边沿满足“触发条件”
 * 并且触发开启时，将会调用连接到引脚上的回调函数。
 * 使用本函数可以配置引脚的“触发条件”，回调函数的连接请参考函数
 * aw_gpio_trigger_connect()，触发的开启关闭请参考函数
 * aw_gpio_trigger_on() 和 aw_gpio_trigger_off()。
 *
 * \param[in] pin       引脚编号
 * \param[in] flags     配置标志，支持的标志有:
 *                      - #AW_GPIO_TRIGGER_HIGH
 *                      - #AW_GPIO_TRIGGER_LOW
 *                      - #AW_GPIO_TRIGGER_RISE
 *                      - #AW_GPIO_TRIGGER_FALL
 *                      - #AW_GPIO_TRIGGER_BOTH_EDGES
 *
 * \retval AW_OK        操作成功
 * \retval -AW_ENXIO    \a pin 不存在
 * \retval -AW_ENOTSUP  传入了引脚不支持的配置
 *
 * \par 示例
 * 请参考 aw_gpio_trigger_connect()
 */
aw_err_t aw_gpio_trigger_cfg(int pin, uint32_t flags);

/**
 * \brief 连接一个回调函数到引脚
 *
 * 具有外部中断功能的引脚可以连接回调函数，当引脚的输入电平/边沿满足“触发条件”
 * 并且触发开启时，将会调用连接到引脚上的回调函数。使用本函数连接回调函数，
 * 使用函数 aw_gpio_trigger_on() 和 aw_gpio_trigger_off() 来分别开启和关闭触发，
 * 使用函数 aw_gpio_trigger_cfg() 设备触发类型。
 *
 * \attention 当调用本函数时，引脚被自动设置为外部中断功能。
 *
 * \par 实现相关
 * 若引脚没有外部中断功能，则不能连接回调函数，将返回错误 -ENOTSUP 。
 * 平台可以支持连接多个回调函数到同一个引脚上，当触发条件满足时，将会按照连接的
 * 先后顺序调用各个“回调函数”(并传递“回调参数”)。
 * 平台也可以只支持连接一个回调函数到一个引脚上，连接已经被连接的引脚将会返回
 * 错误 -ENOSPC ，必须先调用 aw_gpio_trigger_disconnect() 断开先前已连接的回调
 * 函数，才可以成功连接新的回调函数。
 *
 * \param[in]   pin             引脚编号
 * \param[in]   pfunc_callback  触发回调函数
 * \param[in]   p_arg           传递给回调函数的参数
 *
 * \retval      AW_OK          成功
 * \retval     -AW_ENXIO       \a pin 不存在
 * \retval     -AW_EFAULT      \a pfunc_callback 为 NULL
 * \retval     -AW_ENOSPC      空间不足
 * \retval     -AW_ENOTSUP     不支持
 *
 * \par 示例：常规用法
 * \code
 *  #include "aw_gpio.h"
 *
 *  void my_callback(void *p_arg)
 *  {
 *      int my_arg = (int)p_arg; //my_arg = 5
 *      //中断处理。。。
 *  }
 *
 *  // 连接回调函数到引脚
 *  aw_gpio_trigger_connect(PIO0_11, my_callback, (void *)5);
 *
 *  // 设置引脚触发类型，下降沿触发
 *  aw_gpio_trigger_cfg(PIO_11, AW_GPIO_TRIGGER_FALL);
 *
 *  // 开启引脚上的触发器
 *  aw_gpio_trigger_on(PIO0_11);
 * \endcode
 *
 * \par 示例：更新回调函数
 * 当实现只支持一个引脚连接一个回调函数时，按如下方式更新回调函数:
 * \code
 *  // 断开旧的回调函数
 *  aw_gpio_trigger_disconnect(PIO0_11, my_callback, (void *)5);
 *
 *  // 连接新的回调函数
 *  aw_gpio_trigger_connect(PIO0_11, new_callback, (void *)6);
 * \endcode
 *
 * \see aw_gpio_trigger_disconnect(), aw_gpio_trigger_on(), aw_gpio_trigger_off()
 */
aw_err_t aw_gpio_trigger_connect(int             pin,
                                 aw_pfuncvoid_t  pfunc_callback,
                                 void           *p_arg);

/**
 * \brief 断开引脚的一个回调函数
 *
 * 本函数从引脚断开一个“入口地址”为 \a pfunc_callback “参数为” \a p_arg
 * 的回调函数。
 *
 * \param[in]   pin             引脚编号
 * \param[in]   pfunc_callback  要断开的回调函数的“入口地址”
 * \param[in]   p_arg           要断开的回调函数的“参数”
 *
 * \retval     AW_OK        成功
 * \retval     -AW_ENXIO    \a pin 不存在
 * \retval     -AW_ENOENT   未找到指定的回调函数
 *
 * \par 示例
 * 请参考 aw_gpio_trigger_connect()
 *
 * \see aw_gpio_trigger_connect(), aw_gpio_trigger_on(), aw_gpio_trigger_off()
 */
aw_err_t aw_gpio_trigger_disconnect(int             pin,
                                    aw_pfuncvoid_t  pfunc_callback,
                                    void           *p_arg);

/**
 * \brief 关闭引脚上的触发器
 *
 * 本函数关闭所有连接到引脚的触发器
 *
 * \param[in]   pin     引脚编号
 *
 * \retval      AW_OK     成功
 * \retval     -AW_ENXIO  \a pin 不存在
 *
 * \par 示例
 * 请参考 aw_gpio_trigger_connect()
 *
 * \see aw_gpio_trigger_on()
 */
aw_err_t aw_gpio_trigger_off(int pin);

/**
 * \brief 开启引脚的触发
 *
 * 本函数开启引脚的触发
 *
 * \param[in]   pin     引脚编号
 *
 * \retval     AW_OK      成功
 * \retval     -AW_ENXIO  \a pin 不存在
 *
 * \par 示例
 * 请参考 aw_gpio_trigger_connect()
 *
 * \see aw_gpio_trigger_off()
 */
aw_err_t aw_gpio_trigger_on(int pin);

/** @} grp_aw_if_gpio */

#ifdef __cplusplus
}
#endif

#endif /* __AW_GPIO_H */

/* end of file */
