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
 * \brief LORA SX127x �豸��Ϣ
 */
typedef struct awbl_sx127x_devinfo {

    int     dev_id;                 /**< \brief �豸 ID */

    /**
     * \brief ��ƵӲ��IO������Ϣ
     */
    int     reset;                  /**< \brief RESET ���ź� */
    int     dio0;                   /**< \brief DIO0 ���ź� */
    int     dio1;                   /**< \brief DIO1 ���ź� */
    int     dio2;                   /**< \brief DIO2 ���ź� */
    int     dio3;                   /**< \brief DIO3 ���ź� */
    int     dio4;                   /**< \brief DIO4 ���ź� */
    int     dio5;                   /**< \brief DIO5 ���ź� */

    aw_bool_t  use_gpio_spi;        /**< \brief �Ƿ�ʹ���ڲ� GPIO SPI */

    int     mosi;                   /**< \brief SPI MOSI */
    int     miso;                   /**< \brief SPI MISO */
    int     sck;                    /**< \brief SPI CLK */
    int     cs;                     /**< \brief SPI CS */

    int       dio_task_prio;        /**< \brief DIO �������ȼ� */
    int       spi_num;              /**< \brief ʹ�õ�SPI�豸�� */
    uint32_t  spi_speed;            /**< \brief SPI���� */

    void (*pfn_plfm_init)(void *p_arg);     /**< \brief ƽ̨��ʼ������ */
    void (*pfn_plfm_deinit)(void *p_arg);   /**< \brief ƽ̨ȥ��ʼ������ */
} awbl_sx127x_devinfo_t;

/**
 * \brief LORA SX127x ����
 */
struct  awbl_sx127x_service {
    int                 id;
    am_sx127x_handle_t  handle;

    struct  awbl_sx127x_service  *p_next;
};

/**
 * \brief LORA SX127x �豸�ṹ��
 */
typedef struct awbl_sx127x_dev {
    am_sx127x_dev_t         dev;        /**< \brief sx127x �豸��� */
    struct awbl_dev         super;      /**< \brief �̳��� AWBus �豸 */
    aw_spi_device_t         spi_dev;    /**< \brief spi �豸��� */
    awbl_sx127x_devinfo_t  *p_devinfo;  /**< \brief �豸��Ϣ */
    am_sx127x_pfnvoid_t    *p_dio_irq;  /**< \brief DIO�жϺ�����(��6��DIO) */

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
 * \brief ��ȡ��Ӧid��sx127x�豸
 *
 * \param[in] id �豸��
 *
 * \retval !NULL   �ɹ���ȡָ�� id ���豸
 * \retval  NULL   ��ȡʧ��
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
