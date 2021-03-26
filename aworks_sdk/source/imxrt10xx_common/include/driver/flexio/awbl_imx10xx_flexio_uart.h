/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx FlexIO UART
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-02  pea, first implementation
 * \endinternal
 */

#ifndef __IMX10xx_FLEXIO_UART_H
#define __IMX10xx_FLEXIO_UART_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "aw_serial.h"
#include "aw_clk.h"
#include "awbl_sio.h"

#define AWBL_IMX10XX_FLEXIO_UART_DRV_NAME       "imx10xx_flexio_uart"

/** \brief iMX10xx FlexIO UART �豸ͨ�� */
typedef struct awbl_imx10xx_flexio_uart_chan {

    struct aw_sio_chan sio_chan;           /**< \brief ��׼����ͨ�� */
    uint8_t            channel_no;         /**< \brief ͨ���� */
    uint8_t            channel_mode;       /**< \brief �ж�ģʽ������ѯģʽ */
    uint16_t           options;            /**< \brief Ӳ��������Ϣ */

    aw_bool_t          is_tx_int_en;       /**< \brief �Ƿ�ʹ�ܷ����ж� */
    uint32_t           baudrate;           /**< \brief ������ */

    void *txget_arg;                       /**< \brief �������ݻ�ȡ�ص��������� */
    void *rxput_arg;                       /**< \brief �����������ûص��������� */
    int (*pfn_txchar_get)(void *, char *); /**< \brief �������ݻ�ȡ�ص����� */
    int (*pfn_rxchar_put)(void *, char );  /**< \brief �����������ûص����� */

    aw_spinlock_isr_t dev_lock;            /**< \brief ������ */

    struct awbl_imx10xx_flexio_uart_chan *p_next; /**< \brief ��һ��ͨ�� */

} awbl_imx10xx_flexio_uart_chan_t;

/** \brief �豸������Ϣ */
typedef struct awbl_imx10xx_flexio_uart_devinfo {

    int             inum;                /**< \brief Actual architectural vector */
    uint32_t        regbase;             /**< \brief Virt address of register set */
    aw_clk_id_t     async_parent_clk_id; /**< \brief clk id */
    aw_bool_t       is_doze_en;          /**< \brief �Ƿ�ʹ��doze */
    aw_bool_t       is_debug_en;         /**< \brief �Ƿ�ʹ��debug */
    aw_bool_t       is_fast_access_en;   /**< \brief �Ƿ�ʹ��fast_access */
    uint8_t         tx_pin_index;        /**< \brief TX ���ź� */
    uint8_t         rx_pin_index;        /**< \brief RX ���ź� */
    uint8_t         shifter_index[2];    /**< \brief ʹ�õ�Shifter�� */
    uint8_t         timer_index[2];      /**< \brief ʹ�õ�Timer�� */

    /** \brief ƽ̨��ʼ������ */
    void            (*pfn_plfm_init)(void);

} awbl_imx10xx_flexio_uart_dev_info_t;

/** \brief �豸ʵ�� */
typedef struct awbl_imx10xx_flexio_uart_dev {

    /** \brief AWBus �豸ʵ���ṹ�� */
    struct awbl_dev                      dev;

    /** \brief FlexIO UART ͨ�� */
    struct awbl_imx10xx_flexio_uart_chan chan;

} awbl_imx10xx_flexio_uart_dev_t;

/** \brief Flex UART����ע�� */
void awbl_imx10xx_flexio_uart_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __IMX10xx_FLEXIO_UART_H */

/* end of file */
