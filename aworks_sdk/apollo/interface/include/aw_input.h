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
 * \brief 输入子系统通用标准接口
 *
 *    通用输入接口包括鼠标、键盘、摇杆、触摸设备等
 *
 *    输入设备通常会发出多种输入事件, 例如, 鼠标发出相对坐标事件、按键事件, 键盘
 *    发出按键事件,
 *    触摸屏发出绝对坐标事件. 而输入子系统并不关心事件由哪个具体的设备发出, 它仅
 *    负责投递事件.
 *
 *    输入子系统在使用上遵循"谁关心谁订阅"的原则, 用户注册回调函数进系统, 在回调
 *    函数中, 将数据强制转换为所关心的数据类型即可.
 *
 * \par 使用说明
 * \code
 *
 * (1) 使用上报事件标准接口前, 请先确认已配置输入子系统(aw_prj_params.h)
 *
 * ......
 *
 * #define AW_COM_AWBL_INPUT_EV             // Input Service事件管理
 * #define AW_COM_AWBL_INPUT_EV_KEY         // Input Service按键事件
 * #define AW_COM_AWBL_INPUT_EV_ABS         // Input Service绝对事件
 *
 * ......
 *
 * (2) 初始化事件库, 以及向 AWorks平台注册输入子系统(aw_prj_config.c)
 * static void aw_prj_task_level_init( void )
 * {
 * ......
 *
 *   aw_event_lib_init();           // event lib initialization
 *
 * #ifdef AW_COM_AWBL_INPUT_EV
 *   awbl_input_ev_mgr_register();
 * #endif
 *
 * ......
 * }
 *
 * (3) 在任意处使用输入子系统标准接口前，请先包含aw_input.h头文件
 *
 * \endcode
 *
 * \par 简单示例
 * \code
 *
 * #include "aw_input.h"
 *
 * (1) 可在任意处调用上报接口, 上报数据（通常由按键驱动上报）
 *
 * aw_input_report_key(AW_INPUT_EV_KEY, KEY_1, 1);
 *
 * (2) 注册回调函数，接收上报数据（用户程序，用于处理输入事件）
 *
 * void key_proc0_cb (aw_input_event_t *p_input_data, void *p_usr_data)
 * {
 *     if (p_input_data->ev_type != AW_INPUT_EV_KEY) {
 *         return;
 *     }
 *
 *     aw_input_key_data_t *p_key_data = (aw_input_key_data_t *)p_input_data;
 *
 *     switch (key_data->key_code) {
 *
 *     case KEY_1 :
 *         if (p_key_data->key_state) {
 *             AW_INFOF(("KEY1 is down : %d.\n", p_key_data->key_state));
 *         } else {
 *             AW_INFOF(("KEY1 is up : %d.\n", p_key_data->key_state));
 *         }
 *         break;
 *         //...
 *     }
 * }
 *
 * void my_func (void)
 * {
 *     aw_local struct aw_input_handler input_hdlr;
 *
 *     aw_input_handler_register(&input_hdlr, key_proc0_cb, NULL);
 * }
 *
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.01 15-01-07  ops, encapsulate aw_input_handler and redefine how to 
 *                       register/unregister.
 * - 1.00 14-07-18  ops, first implementation.
 * \endinternal
 */

#ifndef __AW_INPUT_H
#define __AW_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_input
 * \copydoc aw_input.h
 * @{
 */

#include "aw_input_code.h"
#include "aw_types.h"
#include "aw_event.h"

/**
 * \name 输入事件类型
 * @{
 */

#define AW_INPUT_EV_KEY    0             /**< \brief 按键事件 */
#define AW_INPUT_EV_ABS    1             /**< \brief 绝对事件 */
#define AW_INPUT_EV_REL    2             /**< \brief 相对事件 */

/** @} */

/**
 * \brief 坐标定义
 */
typedef struct aw_input_pos {

    int                    x;            /**< \brief x轴坐标值 */
    int                    y;            /**< \brief y轴坐标值 */
    int                    z;            /**< \brief z轴坐标值 */
} aw_input_pos_t;

/**
 * \brief 输入事件
 */
typedef struct aw_input_event {
    /**
     * \brief 事件类型码
     * 当前支持的事件类型有：
     * - AW_INPUT_EV_KEY
     * - AW_INPUT_EV_ABS
     * - AW_INPUT_EV_REL
     */
    int                    ev_type;
} aw_input_event_t;

/** \brief 按键数据 */
typedef struct aw_input_key_data {

    aw_input_event_t       input_ev;     /**< \brief  事件类型 */
    int                    key_code;     /**< \brief  按键类型码 */
    
     /**
      * \brief  按键状态 
      *
      * 0   : 按键释放
      * 非0 : 按键按下。其值与具体驱动相关，不一定为1。对于部分可以感受按键压力
      *       的按键，可能通过该值区分按键的按下程度，值越大，按下的程度越大。
      *       对于普通的仅包含两种状态的按键，仅需通过该值是否为零来判断按键是否
      *       按下即可。
      */
    int                    key_state;
} aw_input_key_data_t;

/** \brief 指针输入设备数据 */
typedef struct aw_input_ptr_data {

    aw_input_event_t       input_ev;     /**< \brief  事件类型 */
    int                    ptr_code;     /**< \brief  指针输入设备类型码 */
    aw_input_pos_t         pos;          /**< \brief  坐标值 */
} aw_input_ptr_data_t;

/** \brief 统一的输入设备数据类型 */
typedef union aw_input_data {

    aw_input_key_data_t    key_data;     /**< \brief  按键 */
    aw_input_ptr_data_t    ptr_data;     /**< \brief  指针(鼠标) */
} aw_input_data_t;

/**
 * \brief 输入子系统数据接收回调函数类型.
 *
 * \param[in] p_input_data : 输入数据。实际数据类型应根据 p_input_data->input_ev
 *                           判定，如：类型为 AW_INPUT_EV_KEY，则对应的实际数据
 *                           类型为：aw_input_key_data_t。在回调函数中可将
 *                           p_input_data 强转为 aw_input_key_data_t* 类型使用
 * \param[in] p_usr_data   : 用户数据
 *
 * \return 无
 */
typedef void (*aw_input_cb_t) (aw_input_event_t *p_input_data, void *p_usr_data);

/**
 * \brief 输入事件处理器
 */
typedef struct aw_input_handler {

    struct event_handler  ev_handler;  /**< \brief 事件句柄 */
    aw_input_cb_t         pfn_cb;      /**< \brief 输入子系统数据接收回调函数 */
    void                 *p_usr_data;  /**< \brief 用户自定义数据 */
} aw_input_handler_t;

/*******************************************************************************
      输入子系统 （数据上报接口）
*******************************************************************************/

/**
 * \brief 上报一个按键事件到输入子系统
 *
 * \param[in] ev_type     事件类型码
 * \param[in] code        类型码
 * \param[in] key_state   按键状态: 0, 按键释放; 非0, 按键按下
 *
 * \retval   AW_OK      上报成功
 * \retval  -AW_ENODEV  未注册该事件
 */
int aw_input_report_key (int ev_type, int code, int key_state);

/**
 * \brief 上报一个坐标事件到输入子系统
 *
 * \param[in] ev_type     事件类型码
 * \param[in] code        类型码  (ABS_COORD : 绝对坐标, REL_COORD : 相对坐标)
 * \param[in] coord_x     X坐标
 * \param[in] coord_y     Y坐标
 * \param[in] coord_z     Z坐标
 *
 * \retval   AW_OK      上报成功
 * \retval  -AW_ENODEV  未注册该事件
 */
int aw_input_report_coord (int   ev_type,
                           int   code,
                           int   coord_x,
                           int   coord_y,
                           int   coord_z);

/*******************************************************************************
      输入子系统（注册/注销接口）
*******************************************************************************/

/**
 * \brief 注册一个输入事件处理器到输入子系统
 *
 * \param[in] p_input_handler  : 输入事件处理器实例
 * \param[in] pfn_cb           : 事件处理回调函数.
 * \param[in] p_usr_data       : 用户自定义数据, 如无, 则为NULL.
 *
 * \retval  AW_OK      成功注册
 * \retval -AW_EINVAL  当p_input_handler或p_cb_func为NULL时, 注册失败.
 */
aw_err_t aw_input_handler_register (aw_input_handler_t  *p_input_handler,
                                    aw_input_cb_t        pfn_cb,
                                    void                *p_usr_data);

/**
 * \brief 从输入子系统中注销指定的输入事件处理器
 *
 * \param[in] p_input_handler  :  待注销的输入事件处理器
 *
 * \retval  AW_OK      成功注销.
 * \retval -AW_EINVAL  当p_input_handler为NULL时, 注销失败.
 */
aw_err_t aw_input_handler_unregister (aw_input_handler_t *p_input_handler);


/** @} grp_aw_if_input */

#ifdef __cplusplus
}
#endif

#endif /* __AW_INPUT_H */

/* end of file */
