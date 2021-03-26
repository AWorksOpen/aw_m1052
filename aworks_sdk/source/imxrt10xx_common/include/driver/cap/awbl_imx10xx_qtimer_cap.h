/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief iMX RT10xx QTimer输入捕获驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 18-09-03  mex, first implemetation
 * \endinternal
 */
#ifndef __AWBL_IMX10xx_QTIMER_CAP_H
#define __AWBL_IMX10xx_QTIMER_CAP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_cap.h"
#include "awbl_plb.h"
#include "aw_clk.h"

/* 驱动名 */
#define AWBL_IMX10XX_QTIMER_CAP_NAME   "imx10xx_QTimer_cap"

/**
 * \brief QTimer PWM 通道选择列表
 */
typedef enum qtimer_channel_sel
{
    kQTMR_CAP_Channel_0 = 0,    /**< \brief QTimer CAP Channel 0 */
    kQTMR_CAP_Channel_1,        /**< \brief QTimer CAP Channel 1 */
    kQTMR_CAP_Channel_2,        /**< \brief QTimer CAP Channel 2 */
    kQTMR_CAP_Channel_3,        /**< \brief QTimer CAP Channel 3 */
    kQTMR_CAP_Channel_all,
} qtimer_channel_sel_t;


/**
 * \brief 时钟预分频选择列表
 */
typedef enum qtimer_primary_count_source {
    kQTMR_ClockCounter0InputPin = 0, /**< \brief Use counter 0 input pin */
    kQTMR_ClockCounter1InputPin,     /**< \brief Use counter 1 input pin */
    kQTMR_ClockCounter2InputPin,     /**< \brief Use counter 2 input pin */
    kQTMR_ClockCounter3InputPin,     /**< \brief Use counter 3 input pin */
    kQTMR_ClockCounter0Output,       /**< \brief Use counter 0 output */
    kQTMR_ClockCounter1Output,       /**< \brief Use counter 1 output */
    kQTMR_ClockCounter2Output,       /**< \brief Use counter 2 output */
    kQTMR_ClockCounter3Output,       /**< \brief Use counter 3 output */
    kQTMR_ClockDivide_1,             /**< \brief IP bus clock divide by 1 prescaler */
    kQTMR_ClockDivide_2,             /**< \brief IP bus clock divide by 2 prescaler */
    kQTMR_ClockDivide_4,             /**< \brief IP bus clock divide by 4 prescaler */
    kQTMR_ClockDivide_8,             /**< \brief IP bus clock divide by 8 prescaler */
    kQTMR_ClockDivide_16,            /**< \brief IP bus clock divide by 16 prescaler */
    kQTMR_ClockDivide_32,            /**< \brief IP bus clock divide by 32 prescaler */
    kQTMR_ClockDivide_64,            /**< \brief IP bus clock divide by 64 prescaler */
    kQTMR_ClockDivide_128            /**< \brief IP bus clock divide by 128 prescaler */
} qtimer_primary_count_source_t;


/**
 * \brief iMX RT10xx QTimer PWM 设备配置
 */
typedef struct awbl_imx10xx_qtimer_cap_devcfg {

    uint8_t              pid;          /**< \brief CAP ID */
    qtimer_channel_sel_t ch;           /**< \brief CAP对应的通道 */
    int                  gpio;         /**< \brief CAP对应的引脚 */
    uint32_t             func;         /**< \brief CAP引脚功能 */
} awbl_imx10xx_qtimer_cap_devcfg_t;


/**
 * \brief iMX RT10xx QTimer CAP 设备信息
 */
typedef struct awbl_imx10xx_qtimer_cap_devinfo {

    uint32_t                   regbase;           /**< \brief QTimer寄存器基地址 */
    aw_clk_id_t                ref_clk_id;        /**< \brief QTimer时钟ID */
    uint32_t                   inum;              /**< \brief 中断向量号 */

    /** \brief 时钟预分频 */
    qtimer_primary_count_source_t prescaler_div;

    /** \brief CAP 配置信息 */
    aw_const struct awbl_imx10xx_qtimer_cap_devcfg *cap_devcfg;
    uint8_t                    pnum;              /**< \brief 使用到的CAP通道数*/

    struct awbl_cap_servinfo   cap_servinfo;

    /** \breif platform initializing */
    void (*pfn_plfm_init) (void);

} awbl_imx10xx_qtimer_cap_devinfo_t;


/**
 * \brief iMX RT10xx QTimer capture device
 */
typedef struct awbl_imx10xx_qtimer_cap_dev {
    struct awbl_dev            dev;       /**< \brief 继承自 AWBus 设备 */
    struct awbl_cap_service    cap_serv;  /**< \brief 每个设备提供一个CAP服务给上层 */

    /** \brief To store maximum 8 channels callback functions */
    struct {
        cap_callback_t    callback_func;  /**< \brief callback function  */
        void              *p_arg;         /**< \brief The parameter for callback function */
    } callback_info[4];                   /**< \brief Most support 4 channel */

} awbl_imx10xx_qtimer_cap_dev_t;


/**
 * \brief iMX RT10xx QTimer cap driver register
 */
void awbl_imx10xx_qtimer_cap_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_QER_CAP_H */
