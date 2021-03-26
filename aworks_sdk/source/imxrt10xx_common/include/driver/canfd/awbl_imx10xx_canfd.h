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
 * \brief iMX RT10xx FLEXCANFD
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-29  hsg, first implementation
 * \endinternal
 */

#include "awbl_can.h"
#include "awbus_lite.h"

#ifndef __AWBL_IMX10xx_CANFD_H
#define __AWBL_IMX10xx_CANFD_H

#define AWBL_IMX10XX_CANFD_NAME    "imx10xx_canfd"


#define CANFD_AND_CAN_SWITCH_DYNAMICALLY

/** \brief imx10xx canfd initialization structure for plb based AWBus devices */
typedef struct awbl_imx10xx_canfd_dev_info {

    awbl_can_servinfo_t  servinfo;
    uint32_t             reg_base;
    uint8_t              int_num;

    void   (*pfn_plfm_init)(void);
} awbl_imx10xx_canfd_dev_info_t;

/** \brief imx10xx canfd device channel structure for awbus lite */
typedef struct awbl_imx10xx_canfd_dev {

    /**< \brief device */
    struct awbl_dev  dev;

     /** \brief driver functions, always goes first */
    struct awbl_can_service can_serv;

    /**< \brief 用于记录发送类型 */
    uint8_t sendtype;

    /** \brief 用于记录当前滤波表的长度 */
    uint32_t filter_table_len;

#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
    /** \brief 用于记录当前控制器的类型 */
    uint8_t ctrl_type;
#endif
} awbl_imx10xx_canfd_dev_t;

/**
 * \brief iMX RT10xx CANFD 驱动注册
 */
void awbl_imx10xx_flexcanfd_drv_register (void);


#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
/**
 * \brief canfd控制器类型动态切换函数
 *
 * \note 使用此接口时需要注意，只有在配置文件(如：awbl_hwconf_imx1064_flexcan3.h)中将CANFD控制器
 *       默认配置为CANFD控制器时，才可以调用此接口将此控制器在CANFD与CAN之间动态切换。
 *       如果在配置文件中默认将CANFD控制器配置为CAN控制器时，调用此函数做动态切换是将返回失败。
 *
 * \param[in] chn   CAN通道号
 *
 * \param[in] type  要切换的控制器类型
 *
 * \return  AW_CAN_ERR_NONE                 切换成功
 *         -AW_CAN_ERR_NO_DEV               设备不存在 或 此控制器默认是CAN控制器，无法做切换
 *         -AW_CAN_ERR_ILLEGAL_CTRL_TYPE    type参数错误
 */
aw_can_err_t imxrt10xx_canfd_ctrl_type_switch (int chn, uint8_t type);
#endif

#endif /* __AWBL_IMX10xx_CANFD_H */





