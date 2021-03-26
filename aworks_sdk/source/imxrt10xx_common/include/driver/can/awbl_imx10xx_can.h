/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
*
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx FLEXCAN
 *
 * \internal
 * \par modification history:
 * - 1.00 17-12-14  mex, first implementation
 * \endinternal
 */

#include "awbl_can.h"

#ifndef __AWBL_IMX10xx_CAN_H
#define __AWBL_IMX10xx_CAN_H

#define AWBL_IMX10XX_CAN_NAME    "imx10xx_can"


/** \brief imx10xx can initialization structure for plb based AWBus devices */
typedef struct awbl_imx10xx_can_dev_info {

    awbl_can_servinfo_t  servinfo;
    uint32_t             reg_base;
    uint8_t              int_num;

    void   (*pfn_plfm_init)(void);
} awbl_imx10xx_can_dev_info_t;

/** \brief imx10xx can device channel structure for awbus lite */
typedef struct awbl_imx10xx_can_dev {

    /**< \brief device */
    struct awbl_dev  dev;

     /** \brief driver functions, always goes first */
    struct awbl_can_service can_serv;

    /**< \brief 用于记录发送类型 */
    uint8_t sendtype;

    /**< \brief 用来记录当前的滤波个数 */
    uint32_t filtercnt;

} awbl_imx10xx_can_dev_t;

/**
 * \brief iMX RT10xx CAN 驱动注册
 */
void awbl_imx10xx_flexcan_drv_register (void);

#endif /* __AWBL_IMX10xx_CAN_H */
