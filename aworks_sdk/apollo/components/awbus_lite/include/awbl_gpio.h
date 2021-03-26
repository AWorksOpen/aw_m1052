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
 * \brief AWBus GPIO 接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_gpio.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-29  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_GPIO_H
#define __AWBL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/**
 * \brief AWBus GPIO 服务实例的相关信息
 */
struct awbl_gpio_servinfo {

    /** \brief 支持的最小引脚编号 */
    int pin_min;

    /** \brief 支持的最大引脚编号 */
    int pin_max;

    /**
     *\brief 记录引脚状态的内存(每个引脚1个bit)
     *
     * 若驱动实现了服务函数 pfunc_gpio_toggle()则不必提供此内存，填写 NULL
     */
    uint8_t *p_pin_state;
};

/**
 * \brief AWBus GPIO 服务函数
 */
struct awbl_gpio_servfuncs {

    /** \brief 配置引脚功能 */
    aw_err_t (*pfunc_gpio_pin_cfg)(void *p_cookie, int pin, uint32_t flags);

    /** \brief 读取引脚当前的输入/输出值 */
    aw_err_t (*pfunc_gpio_get)(void *p_cookie, int pin);

    /** \brief 设置引脚的输出值 */
    aw_err_t (*pfunc_gpio_set)(void *p_cookie, int pin, int value);

    /** \brief 翻转引脚的输出值 */
    aw_err_t (*pfunc_gpio_toggle)(void *p_cookie, int pin);

    /** \brief 配置引脚触发条件 */
    aw_err_t (*pfunc_gpio_trigger_cfg)(void     *p_cookie,
                                       int       pin,
                                       uint32_t  flags);

    /** \brief 连接回调函数到引脚 */
    aw_err_t (*pfunc_gpio_trigger_connect)(void           *p_cookie,
                                           int             pin,
                                           aw_pfuncvoid_t  pfunc_callback,
                                           void           *p_arg);

    /** \brief 从引脚断开回调函数 */
    aw_err_t (*pfunc_gpio_trigger_disconnect)(void           *p_cookie,
                                              int             pin,
                                              aw_pfuncvoid_t  pfunc_callback,
                                              void           *p_arg);
    /** \brief 开启引脚的触发器 */
    aw_err_t (*pfunc_gpio_trigger_on)(void *p_cookie, int pin);

    /** \brief 关闭引脚的触发器 */
    aw_err_t (*pfunc_gpio_trigger_off)(void *p_cookie, int pin);

    /** \brie 申请一个引脚 */
    aw_err_t (*pfunc_gpio_pin_request)(void       *p_cookie, 
                                       const char *p_name,
                                       int         pin);

    /** \brie 释放一个引脚 */
    aw_err_t (*pfunc_gpio_pin_release)(void *p_cookie, int pin);
};

/**
 * \brief AWBus GPIO 服务实例
 */
struct awbl_gpio_service {

    /** \brief 指向下一个 GPIO 服务 */
    struct awbl_gpio_service *p_next;

    /** \brief GPIO 服务的相关信息 */
    const struct awbl_gpio_servinfo *p_servinfo;

    /** \brief GPIO 服务的相关函数 */
    const struct awbl_gpio_servfuncs *p_servfuncs;

    /**
     * \brief GPIO 服务自定义的数据
     *
     * 此参数在GPIO接口模块搜索服务时由驱动设置，在调用服务函数时作为第一个参数。
     */
    void *p_cookie;
};

/**
 * \brief AWBus GPIO 接口模块初始化
 *
 * \attention 本函数应当在 awbl_dev_init1() 之后、在 awbl_dev_init2() 之前调用
 */
void awbl_gpio_init(void);

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AWBL_GPIO_H */

/* end of file */
