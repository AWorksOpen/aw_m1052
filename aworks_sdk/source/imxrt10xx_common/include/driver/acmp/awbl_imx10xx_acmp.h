
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

#ifndef __AWBL_IMX10xx_ACMP_H
#define __AWBL_IMX10xx_ACMP_H

#include "aworks.h"
#include "aw_gpio.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "aw_gpio.h"
#include "driver/acmp/awbl_acmp_service.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief iMX RT10xx ACMP 驱动名 */
#define AWBL_IMX10XX_ACMP_NAME            "imx10xx_acmp"



/**
 * \brief iMX RT10xx ACMP 设备信息
 */
typedef struct awbl_imx10xx_acmp_devinfo {

    uint32_t            regbase;    /**< \brief 寄存器基地址 */
    uint8_t             int_num;    /**< \brief 中断号 */
    uint8_t             acmp_id;    /**< \brief ACMP的ID号(1~ 4) */
    void (*pfn_plfm_init) (void);   /**< \brief 平台初始化函数 */
} awbl_imx10xx_acmp_devinfo_t;


/**
 * \brief iMX RT10xx ACMP 设备实例
 */
typedef struct awbl_imx10xx_acmp_dev {

    struct awbl_dev          super;        /**< \brief 继承自 AWBus 设备 */
    struct awbl_acmp_service acmp_serv;    /**< \brief 设备服务 */

} awbl_imx10xx_acmp_dev_t;

/**
 * \brief iMX RT10xx ACMP 驱动注册
 */
void awbl_imx10xx_acmp_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_ACMP_H */

/* end of file */
