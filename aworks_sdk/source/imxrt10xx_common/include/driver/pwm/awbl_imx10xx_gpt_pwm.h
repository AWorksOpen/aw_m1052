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
 * \brief iMX RT10xx GPT 驱动
 *
 * 本驱动实现了定时器的匹配输出功能。
 *
 * \internal
 * \par modification history
 * - 1.00 20-09-11  licl, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_GPT_PWM_H
#define __AWBL_IMX10xx_GPT_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_pwm.h"
#include "aw_sem.h"
#include "aw_clk.h"
#include "driver/timer/imx10xx_gpt_regs.h"

#define AWBL_IMX10XX_GPT_PWM_NAME   "imx10xx_gpt_pwm"

/**
 * \brief QTimer PWM 通道选择列表
 */
typedef enum __gpt_channel_sel
{
    GPT_Channel_0 = 0,    /**< \brief GPT Channel 0 */
    GPT_Channel_1,        /**< \brief GPT Channel 1 */
    GPT_Channel_2,        /**< \brief GPT Channel 2 */
    GPT_Channel_all,
} gpt_channel_sel_t;

/**
 * \brief iMX RT10xx GPT PWM 设备配置
 */
typedef struct awbl_imx10xx_gpt_pwm_devcfg {
    uint8_t            pid;             /**< \brief PWM ID */
    gpt_channel_sel_t  ch;              /**< \brief PWM对应的通道 */
    int                gpio;            /**< \brief PWM对应的引脚 */
    uint32_t           func;            /**< \brief PWM引脚功能 */
    uint32_t           dfunc;           /**< \brief 禁能PWM后的默认引脚功能 */
} awbl_imx10xx_gpt_pwm_devcfg_t;

/**
 * \brief iMX RT10xx GPT PWM 通道配置
 */
typedef struct {
    uint32_t period_num;
    AW_SEMB_DECL(sync_sem);
} awbl_imx10xx_gpt_pwm_channel_t;

/**
 * \brief iMX RT10xx  GPT PWM 设备信息
 */
typedef struct awbl_imx10xx_gpt_pwm_devinfo {
    uint32_t                 regbase;           /**< \brief PWM寄存器基地址 */
    uint32_t                 inum;              /**< \brief 中断号 */
    aw_clk_id_t              ref_clk_id;        /**< \brief PWM时钟ID */
    struct awbl_pwm_servinfo pwm_servinfo;      /**< \brief PWM服务相关信息 */

    /** \brief PWM配置信息 */
    aw_const struct awbl_imx10xx_gpt_pwm_devcfg *pwm_devcfg;
    uint8_t                  pnum;              /**< \brief 使用到的PWM通道数*/
    void (*pfunc_plfm_init)(void);              /** \brief 平台相关初始化：开启时钟、初始化引脚等操作 */
} awbl_imx10xx_gpt_pwm_devinfo_t;

/**
 * \brief 设备实例
 */
typedef struct awbl_imx10xx_gpt_pwm_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev super;

    /** \brief PWM 服务*/
    struct awbl_pwm_service pwm_serv;

    /** \brief PWM通道 */
    awbl_imx10xx_gpt_pwm_channel_t channel[3];

} awbl_imx10xx_gpt_pwm_dev_t;

/**
 * \brief 将 IMX10xx GPT PWM 驱动注册到 AWBus 子系统中
 *
 * \note 本函数应在 aw_prj_early_init() 中初始化，本函数中禁止调用操作系统相关函数
 */
void awbl_imx10xx_gpt_pwm_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_IMX10xx_GPT_PWM_H */

/* end of file */
