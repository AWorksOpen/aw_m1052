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

#ifndef __AWBL_SX127X_H
#define __AWBL_SX127X_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup awbl_sx127x
 * @copydoc awbl_sx127x.h
 * @{
 */

#include "aworks.h"
#include "am_sx127x.h"
#include "aw_task.h"
#include "aw_msgq.h"
#include "aw_sem.h"
#include "aw_spi.h"
#include "awbus_lite.h"

#define SX127X_DEV_NAME  "sx127x_device"

/**
 * \brief LORA SX127x 设备信息
 */
typedef struct awbl_sx127x_devinfo {

    int     dev_id;                 /**< \brief 设备 ID */

    /**
     * \brief 射频硬件IO配置信息
     */
    int     reset;                  /**< \brief RESET 引脚号 */
    int     dio0;                   /**< \brief DIO0 引脚号 */
    int     dio1;                   /**< \brief DIO1 引脚号 */
    int     dio2;                   /**< \brief DIO2 引脚号 */
    int     dio3;                   /**< \brief DIO3 引脚号 */
    int     dio4;                   /**< \brief DIO4 引脚号 */
    int     dio5;                   /**< \brief DIO5 引脚号 */

    aw_bool_t  use_gpio_spi;        /**< \brief 是否使用内部 GPIO SPI */

    int     mosi;                   /**< \brief SPI MOSI */
    int     miso;                   /**< \brief SPI MISO */
    int     sck;                    /**< \brief SPI CLK */
    int     cs;                     /**< \brief SPI CS */

    int       dio_task_prio;        /**< \brief DIO 任务优先级 */
    int       spi_num;              /**< \brief 使用的SPI设备号 */
    uint32_t  spi_speed;            /**< \brief SPI速率 */

    void (*pfn_plfm_init)(void *p_arg);     /**< \brief 平台初始化函数 */
    void (*pfn_plfm_deinit)(void *p_arg);   /**< \brief 平台去初始化函数 */
} awbl_sx127x_devinfo_t;

/**
 * \brief LORA SX127x 服务
 */
struct  awbl_sx127x_service {
    int                 id;
    am_sx127x_handle_t  handle;

    struct  awbl_sx127x_service  *p_next;
};

/**
 * \brief LORA SX127x 设备结构体
 */
typedef struct awbl_sx127x_dev {
    am_sx127x_dev_t         dev;        /**< \brief sx127x 设备句柄 */
    struct awbl_dev         super;      /**< \brief 继承自 AWBus 设备 */
    aw_spi_device_t         spi_dev;    /**< \brief spi 设备句柄 */
    awbl_sx127x_devinfo_t  *p_devinfo;  /**< \brief 设备信息 */
    am_sx127x_pfnvoid_t    *p_dio_irq;  /**< \brief DIO中断函数表(共6个DIO) */

    struct  awbl_sx127x_service  service;

    AW_TASK_DECL(task, 2048);           /**< \brief DIO task */
    AW_MSGQ_DECL(msgq, 2, 4);           /**< \brief DIO msgq */
    AW_MUTEX_DECL(mutex);               /**< \brief gpio spi mutex */
} awbl_sx127x_dev_t;


typedef awbl_sx127x_dev_t *awbl_sx127x_handle_t;

/******************************************************************************/
/** \brief convert a awbl_dev pointer to dc_buzzer pointer */
#define dev_to_sx127x(p_dev)    AW_CONTAINER_OF(p_dev, \
                                                struct awbl_sx127x_dev, \
                                                super)

/******************************************************************************/
void awbl_sx127x_drv_register (void);

/**
 * \brief 获取对应id的sx127x设备
 *
 * \param[in] id 设备号
 *
 * \retval !NULL   成功获取指定 id 的设备
 * \retval  NULL   获取失败
 */
am_sx127x_dev_t *awbl_sx127x_get_handle_by_id(int  id);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __AWBL_SX127X_H */

/* end of file */
