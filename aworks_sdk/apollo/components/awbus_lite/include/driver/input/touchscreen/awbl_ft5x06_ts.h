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
 * \brief ft5x06 touch screen controller
 *
 * \internal
 * \par modification history:
 * - 1.00 14-07-17  ops, first implemetation
 * \endinternal
 */
#ifndef AWBL_FT5X06_TS_H_
#define AWBL_FT5X06_TS_H_

#include "awbl_input.h"
#include "awbl_ts.h"
#include "aw_task.h"
#include "aw_i2c.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \name FT5x06�����豸I2C�ӻ���ַ���������л�����һλ�����0x70)
 * @{
 */
#define FT5x06_ADDRESS        (0x70 >> 1)

/** @} */

#define AWBL_FT5X06_TOUCHSCREEN_NAME            "ft5x06_touchscreen"


typedef struct awbl_ft5x06_ts_devinfo {
    /** \brief FT5x06�豸��ַ               */
    int  dev_addr;

    /** \brief ����FT5X06�豸�ĸ�λ���� */
    int   rst_pin;

    /** \brief ����FT5X06�豸���ж����� */
    int   int_pin;

    awbl_ts_service_info_t ts_serv_info;

}awbl_ft5x06_ts_devinfo_t;


typedef struct awbl_ft5x06_ts_dev {

    /** \brief �̳��� AWBus�豸 */
    struct awbl_dev super;

    /** \bried �̳���ts����.   */
    struct awbl_ts_service ts_serv;

    /** \brief ft5x06���豸�ṹ.*/
    aw_i2c_device_t i2c_ft5x06;

    /** \brief ��һ�°��±�־ */
    uint8_t is_debounce;

}awbl_ft5x06_ts_dev_t;

/**
 * \brief ft5x063 pwm driver register
 */
void awbl_ft5x06_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_FT5X06_TS_H_ */
