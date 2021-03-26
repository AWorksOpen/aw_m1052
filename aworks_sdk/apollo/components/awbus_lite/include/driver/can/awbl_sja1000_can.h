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
 * \brief sja1000 can驱动
 *
 * 仅实现SJA1000单路CAN
 * \internal
 * \par modification history:
 * - 1.00 16-04-17  lnk, first implementation.
 * \endinternal
 */


#ifndef AWBL_SJA1000_CAN_H_
#define AWBL_SJA1000_CAN_H_

#define AWBL_SJA1000_CAN_NAME "sja1000_can"

#include "apollo.h"
#include "awbl_can.h"
#include "aw_sem.h"
#include "awbl_eim_bus.h"

/*******************************************************************************
    types
*******************************************************************************/
/** \brief SJA1000 设备 */
typedef struct awbl_sja1000_can_dev {

    /**< \brief device */
    struct awbl_eim_device dev;
    uint8_t cs_select;

    /** \brief driver functions, always goes first */
    struct awbl_can_service can_serv;

    /** \brief 用于记录发送类型 */
    uint8_t send_type;

    /** \brief 用于上一时刻的警告状态  */
    awbl_can_int_type_t last_ewarn_stat;

    /** \brief 用于上一时刻的错误被动状态  */
    awbl_can_int_type_t last_epassive_stat;

    /** \brief 用于上一时刻的总线关闭状态  */
    awbl_can_int_type_t last_ebusoff_stat;

} awbl_sja1000_can_dev_t;

/** \brief SJA1000 设备信息 */
typedef struct awbl_sja1000_can_dev_info {

    awbl_can_servinfo_t  servinfo;        /**< \brief can 服务 */

    uint8_t cs_select;                    /**< \brief CS 选择 */

    int     pin_rst;                      /**< \brief rst 引脚编号 */
    int     pin_int;                      /**< \brief int 引脚编号 */
    
    void (*pfn_plfm_init)(void);          /**< \brief plfm 平台初始化 */
} awbl_sja1000_can_dev_info_t;

/** \brief SJA1000 驱动注册 */
void awbl_sja1000_can_drv_register(void);

#endif /* AWBL_SJA1000_CAN_H_ */
