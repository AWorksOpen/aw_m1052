/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite ZSN603
 *
 *
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "zsn603"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_zsn603_devinfo
 *
 * \par 2.兼容设备
 *
 *  - 无
 *  -
 *  -
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_zsn603_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_adc
 *
 * \internal
 * \par modification history:
 * - 1.00 19-03-19 htf, first implementation
 * \endinternal
 */

#ifndef __AWBL_ZSN603_I2C__H
#define __AWBL_ZSN603_I2C__H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "aw_sem.h"
#include "zsn603.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"

#define AWBL_ZSN603_I2C_NAME   "zsn603_i2c"
/**
 * \brief ZSN603 设备信息
 */
typedef struct awbl_zsn603_i2c_devinfo {
    /** \brief ZSN603 lcoal address */
    uint8_t  local_address;
    /** \brief ZSN603 i2c int pin*/
    uint8_t  int_pin;
} awbl_zsn603_i2c_devinfo_t;


#define awbldev_get_zsn603_i2c_info(p_dev) \
            ((awbl_zsn603_i2c_devinfo_t *)AWBL_DEVINFO_GET(((awbl_dev_t *)(p_dev))))
/**
 * \brief ZSN603 设备实例
 */
typedef struct awbl_zsn603_i2c_dev {

    /** \brief 继承自 AWBus I2C device 设备 */
    struct awbl_i2c_device super;

    /** \brief zsn603设备 */
    zsn603_i2c_dev_t       dev;

    zsn603_i2c_devinfo_t   info;

} awbl_zsn603_i2c_dev_t;


/**
 * \brief zsn603(i2c mode) driver register
 */
void awbl_zsn603_i2c_drv_register (void);

/**
 * \brief zsn603(i2c mode) handle get
 */
zsn603_handle_t  awbl_zsn603_i2c_handle_get(int unit);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_ZMF169_ADC1_H */

/* end of file */
