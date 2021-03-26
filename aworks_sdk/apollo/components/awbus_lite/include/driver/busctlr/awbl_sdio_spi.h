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
 * \brief SDIO SPI mode driver
 *
 * \internal
 * \par modification history:
 * - 1.00 14-10-29  deo, first implementation
 * - 1.01 17-01-06  xdn, adaptive new sdio interface
 * \endinternal
 */

#ifndef __AWBL_SPI_SDIO_H
#define __AWBL_SPI_SDIO_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_sdiobus.h"
#include "aw_spi.h"

#define AWBL_SDIO_SPI_NAME            "awbl_sdio_spi"

#define _TRANS_TASK_STACK_SIZE        2048


/**
 * \brief spi sdio �豸ʵ��
 */
typedef struct awbl_spi_sdio_dev {
    /** \brief �̳��� AWBus SDIO ���������� */
    awbl_sdio_host_t    host;

    aw_spi_device_t     sdio_spi;

    uint32_t            speed;
    awbl_sdio_cmd_t    *p_cmd;
    AW_TASK_DECL(spi_sdio_trans_task,
            _TRANS_TASK_STACK_SIZE);
    AW_SEMB_DECL(sem_sync);

    void (*pfn_spi_cs)(
            aw_spi_device_t *p_dev,
            int              state);

} awbl_spi_sdio_dev_t;


/* sdio host �豸��Ϣ  */
typedef struct awbl_spi_sdio_dev_info {
    awbl_sdio_host_info_t   host_info;

    uint16_t                mode;
    int                     cs_pin;         /**< \brief ʹ�ܹܽ� */
    int                     pin_cd;         /**< \brief ��������ܽ� */
    uint32_t                max_speed;
    uint32_t                task_trans_prio;

    /** \brief ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init)(void);

    aw_bool_t              (*pfn_cd) (awbl_spi_sdio_dev_t *p_dev);

} awbl_spi_sdio_dev_info_t;

void awbl_sdio_spi_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_SPI_SDIO_H_ */
