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
 * \brief Freescale i.MXRT10xx QTimer PWM外设驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-12  mex, first implemetation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_QTIMER_PWM_H
#define __AWBL_IMX10xx_QTIMER_PWM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include  "awbus_lite.h"
#include  "awbl_pwm.h"
#include  "aw_sem.h"

#define AWBL_IMX10XX_QTIMER_PWM_NAME      "imx10xx_qtimer_pwm"


/**
 * \brief QTimer PWM 通道选择列表
 */
typedef enum _qtmr_channel_sel
{
    kQTMR_Channel_0 = 0,    /**< \brief TMR Channel 0 */
    kQTMR_Channel_1,        /**< \brief TMR Channel 1 */
    kQTMR_Channel_2,        /**< \brief TMR Channel 2 */
    kQTMR_Channel_3,        /**< \brief TMR Channel 3 */
    kQTMR_Channel_all,
} qtmr_channel_sel_t;


/**
 * \brief iMX RT10xx QTimer PWM 设备配置
 */
typedef struct awbl_imx10xx_qtimer_pwm_devcfg {

    uint8_t            pid;             /**< \brief PWM ID */
    qtmr_channel_sel_t ch;              /**< \brief PWM对应的通道 */
    int                gpio;            /**< \brief PWM对应的引脚 */
    uint32_t           func;            /**< \brief PWM引脚功能 */
    uint32_t           dfunc;           /**< \brief 禁能PWM后的默认引脚功能 */

} awbl_imx10xx_qtimer_pwm_devcfg_t;

/**
 * \brief iMX RT10xx QTimer PWM 通道配置
 */
typedef struct {
    uint32_t period_num;
    AW_SEMB_DECL(sync_sem);
} awbl_imx10xx_qtimer_pwm_channel_t;

/**
 * \brief iMX RT10xx  QTimer PWM 设备信息
 */
typedef struct awbl_imx10xx_qtimer_pwm_devinfo {
    uint32_t                 regbase;            /**< \brief PWM寄存器基地址 */
    uint32_t                 inum;               /**< \brief 中断号 */
    aw_clk_id_t              ref_clk_id;         /**< \brief PWM时钟ID */
    struct awbl_pwm_servinfo pwm_servinfo;       /**< \brief PWM服务相关信息 */

    /** \brief PWM配置信息 */
    aw_const struct awbl_imx10xx_qtimer_pwm_devcfg *pwm_devcfg;
    uint8_t                  pnum;               /**< \brief 使用到的PWM通道数*/

    /** \brief 平台相关初始化：开启时钟、初始化引脚等操作 */
    void (*pfunc_plfm_init)(void);

} awbl_imx10xx_qtimer_pwm_devinfo_t;


/**
 * \brief iMX RT10xx eFlexPWM 设备实例
 */
typedef struct awbl_imx10xx_qtimer_pwm_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev super;

    /** \brief PWM 服务*/
    struct awbl_pwm_service pwm_serv;

    /** \brief Qtimer3 PWM通道 */
    awbl_imx10xx_qtimer_pwm_channel_t channel[4];

} awbl_imx10xx_qtimer_pwm_dev_t;


/**
 * \brief iMX RT10xx  QTimer PWM driver register
 */
void awbl_imx10xx_qtimer_pwm_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_QTIMER_PWM_H */

/* end of file */
