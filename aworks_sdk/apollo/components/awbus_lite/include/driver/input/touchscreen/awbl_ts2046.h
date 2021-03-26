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
 * \brief touch screen controller
 *
 * \internal
 * \par modification history:
 * - 1.01 16-08-18  anu, first implemetation
 * \endinternal
 */
#ifndef __AWBL_TS2046_H
#define __AWBL_TS2046_H

#include "awbl_input.h"
#include "awbl_ts.h"
#include "awbus_lite.h"
#include "aw_spi.h"
#include "aw_sem.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#define AWBL_TS2046_NAME            "ts2046"

struct awbl_ts2046_spi_info {

    int spi_busid;
    int spi_cs_pin;
    int spi_speed;

};

typedef struct awbl_ts2046_devinfo {

    struct awbl_ts2046_spi_info spi_info;

    awbl_ts_service_info_t ts_serv_info;

    uint8_t detect_interval;

    int detect_pin;  /**< \brief 中断引脚，判断是否按下 */

    int touch_ohm; /**< \brief 触摸屏阻抗 单位 Ω */
    
    void (*pfn_plfm_init)(void);

} awbl_ts2046_devinfo_t;

typedef struct awbl_ts2046_val {
    uint16_t y;
    uint16_t x;
    uint16_t z1;
    uint16_t z2;
} awbl_ts2046_val_t;

typedef struct awbl_ts2046_dev {

    /** \bried 继承自Touch SCreen服务 */
    struct awbl_ts_service ts_serv;

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev adev;

    aw_spi_device_t sdev;

    awbl_ts2046_devinfo_t *p_info;

    aw_bool_t is_pressed; /**< \brief 是否处于按下状态 */

    /** \brief 第一下按下标志 */
    uint8_t is_debounce;

    awbl_ts2046_val_t *p_sample_buf;

} awbl_ts2046_dev_t;

/**
 * \brief ts driver register
 */
void awbl_touch_screen_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_TS2046_H */
