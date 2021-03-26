
/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx Quadrature Decoder driver
 *
 * \internal
 * \par Modification History
 * - 1.00 2018-03-20  mex, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_ENC_H
#define __AWBL_IMX10xx_ENC_H

#include "aworks.h"
#include "aw_gpio.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "aw_gpio.h"
#include "driver/enc/awbl_enc_service.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief iMX RT10xx ENC 驱动名 */
#define AWBL_IMX10XX_ENC_NAME            "imx10xx_enc"

typedef enum __enc_index_trigger_mode
{

    /** \brief INDEX signal's trigger is disabled. */
    enc_index_trigger_disable = 0,

    /** \brief Use positive going edge-to-trigger initialization of position counters. */
    enc_index_trigger_rising_edge,

    /** \brief Use negative going edge-to-trigger initialization of position counters. */
    enc_index_trigger_falling_edge,

} enc_index_trigger_mode_t;



/**
 * \brief iMX RT10xx ENC 设备信息
 */
typedef struct awbl_imx10xx_enc_devinfo {

    uint32_t            regbase;    /**< \brief 寄存器基地址 */
    uint8_t             int_num;    /**< \brief 中断号 */

    int                 chan;        /** \brief 当前通道号 */

    /** \brief index触发设置 */
    enc_index_trigger_mode_t trigger_mode;

    /** \brief  PHASE、PHASEB、INDEX输入信号滤波器采样计数,设置范围0-7 */
    uint8_t             filter_count;

    /** \brief  输入信号滤波器采样周期，设置范围0-255
     *   滤波器延时计算方法：
     *   DELAY (IPBus clock cycles) = filter_sample_priod * (filter_count+3) + 2
     */
    uint16_t            filter_sample_priod;

    void (*pfn_plfm_init) (void);   /**< \brief 平台初始化函数 */
} awbl_imx10xx_enc_devinfo_t;



/**
 * \brief iMX RT10xx ENC 设备实例
 */
typedef struct awbl_imx10xx_enc_dev {

    struct awbl_dev         super;       /**< \brief 继承自 AWBus 设备 */
    struct awbl_enc_service enc_serv;    /**< \brief 设备服务 */

} awbl_imx10xx_enc_dev_t;

/**
 * \brief iMX RT10xx ENC 驱动注册
 */
void awbl_imx10xx_enc_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_ENC_H */

/* end of file */
