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
 * \brief gpio spi driver
 *
 * AWBus spi bus controller simulated by GPIO
 *
 * \internal
 * \par modification history:
 * - 1.10 14-11-20  deo, first implementation
 * \endinternal
 */

#ifndef __AWBL_GPIO_SPI_H
#define __AWBL_GPIO_SPI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_spinlock.h"
#include "awbl_spibus.h"
#include "aw_hwtimer.h"
#include "aw_task.h"

#define AWBL_GPIO_SPI_NAME "awbl_gpio_spi"

/**
 * \brief gpio spi �豸��Ϣ
 */
struct awbl_gpio_spi_devinfo {

    /** \brief �̳��� AWBus SPI �������豸��Ϣ */
    struct awbl_spi_master_devinfo spi_master_devinfo;

    int                            sck;

    /***
     * \brief ��ֻʹ������ģʽʱ��ֻ������mosi���ź�,��miso��ֵΪ -1 ����
     *        ��ĳЩ�豸Ϊ����ģʽ��Ĭ��mosi������Ϊ������������
     */
    int                            mosi;
    int                            miso;

    uint8_t                        dummp_byte;

    aw_err_t (*hal_io_set) (int pin, int state);

    /** \brief ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init)(void);
};


/**
 * \brief gpio spi �豸�ṹ
 */
struct awbl_gpio_spi_dev {

    /** \brief �̳��� AWBus SPI ���������� */
    struct awbl_spi_master  super;

    uint8_t                 mode;
    uint8_t                 bpw;
    uint32_t                speed;
    uint32_t                dly;

    uint8_t                 sck_state;

    aw_err_t (*io_set) (int pin, int state);
};


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_GPIO_SPI_H */
