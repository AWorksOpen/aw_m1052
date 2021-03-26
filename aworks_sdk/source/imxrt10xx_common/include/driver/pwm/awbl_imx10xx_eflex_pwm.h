/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief Freescale i.MXRT10xx eFlexPWM外设驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-09  mex, first implemetation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_EFLEX_PWM_H
#define __AWBL_IMX10xx_EFLEX_PWM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include  "awbus_lite.h"
#include  "awbl_pwm.h"
#include  "aw_sem.h"


#define AWBL_IMX10XX_EFLEX_PWM_NAME      "imx10xx_eflex_pwm"


/**
 * \brief PWM 通道
 */
typedef enum _pwm_channels
{
    kPWM_PwmB = 0U,
    kPWM_PwmA,
    kPWM_PwmX
} pwm_channels_t;


/**
 * \brief PWM 子模块
 */
typedef enum _pwm_submodule
{
    kPWM_Module_0 = 0,   /**< \brief  Submodule 0 */
    kPWM_Module_1,       /**< \brief  Submodule 1 */
    kPWM_Module_2,       /**< \brief  Submodule 2 */
    kPWM_Module_3,       /**< \brief  Submodule 3 */
    kPWM_Module_all,     /**< \brief  The toal numbers of submodules */
} pwm_submodule_t;


/**
 * \brief PWM operation mode
 */
typedef enum _pwm_mode
{
    kPWM_SignedCenterAligned = 0U,   /**< \brief  Signed center-aligned */
    kPWM_CenterAligned,              /**< \brief  Unsigned cente-aligned */
    kPWM_SignedEdgeAligned,          /**< \brief  Signed edge-aligned */
    kPWM_EdgeAligned                /**< \brief  Unsigned edge-aligned */
} pwm_mode_t;

/**
 * \brief iMX RT10xx eFlexPWM 设备配置
 */
typedef struct awbl_imx10xx_eflex_pwm_devcfg {

    uint8_t          pid;             /**< \brief PWM ID */
    pwm_submodule_t  sub_modedle;     /**< \brief PWM对应的子模块 */
    pwm_channels_t   pwm_channel;     /**< \brief PWM对应通道 */
    pwm_mode_t       pwm_mode;        /**< \brief PWM模式 */
    int              gpio;            /**< \brief PWM对应的引脚 */
    uint32_t         func;            /**< \brief PWM引脚功能 */
    uint32_t         dfunc;           /**< \brief 禁能PWM后的默认引脚功能 */
    uint32_t         inum;            /**< \brief 中断号 */

} awbl_imx10xx_eflex_pwm_devcfg_t;

typedef struct {
    uint32_t period_num;
    AW_SEMB_DECL(sync_sem);
} awbl_imx10xx_eflex_pwm_channel_t;

/**
 * \brief iMX RT10xx eFlexPWM 设备信息
 */
typedef struct awbl_imx10xx_eflex_pwm_devinfo {
    uint32_t                 regbase;            /**< \brief PWM寄存器基地址 */
    aw_clk_id_t              ref_clk_id;         /**< \brief PWM时钟ID */
    struct awbl_pwm_servinfo pwm_servinfo;       /**< \brief PWM服务相关信息 */

    /** \brief PWM配置信息 */
    aw_const struct awbl_imx10xx_eflex_pwm_devcfg *pwm_devcfg;
    uint8_t                  pnum;               /**< \brief 使用到的PWM通道数*/

    /** \brief 平台相关初始化：开启时钟、初始化引脚等操作 */
    void (*pfunc_plfm_init)(void);

} awbl_imx10xx_eflex_pwm_devinfo_t;


/**
 * \brief iMX RT10xx eFlexPWM 设备实例
 */
typedef struct awbl_imx10xx_eflex_pwm_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev super;

    /** \brief PWM 服务*/
    struct awbl_pwm_service pwm_serv;

    awbl_imx10xx_eflex_pwm_channel_t channel[4];

} awbl_imx10xx_eflex_pwm_dev_t;


/**
 * \brief iMX RT10xx eFlexPWM driver register
 */
void awbl_imx10xx_eflex_pwm_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_EFLEX_PWM_H */

/* end of file */
