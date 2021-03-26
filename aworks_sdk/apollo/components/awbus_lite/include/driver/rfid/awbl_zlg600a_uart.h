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
 * \brief AWBus ZLG600A �ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_zlg600a.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-05-03  win, first implementation
 * \endinternal
 */

#ifndef __AWBL_ZLG600A_UART_H
#define __AWBL_ZLG600A_UART_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_zlg600a_uart
 * \copydetails awbl_zlg600a.h
 * @{
 */

/**
 * \defgroup  grp_awbl_zlg600a_uart_impl ʵ�����
 * \todo
 *
 */

/**
 * \defgroup  grp_awbl_zlg600a_uart_hwconf �豸����/����
 * \todo
 */

/** @} grp_awbl_drv_zlg600a_uart */

#include "aw_task.h"
#include "aw_sem.h"
#include "awbl_zlg600a.h"

/** \brief PCF8563 ������ */
#define AWBL_ZLG600A_UART_NAME          "zlg600a_uart"

#define __ZLG600A_UART_TASK_STACK_SIZE  512

/**
 * \brief ZLG600A �����豸��Ϣ
 */
typedef struct awbl_zlg600a_uart_devinfo {

    /** \brief ZLG600A ����������Ϣ  */
    struct awbl_zlg600a_servinfo zlg600a_servinfo;

    uint8_t   frame_fmt;    /**< \brief Ĭ��ʹ�õ�֡��ʽ */
    int       baudrate;     /**< \brief ������ */
    uint8_t   addr;         /**< \brief Ĭ�ϴ���ĵ�ַ      */
    uint8_t   mode;         /**< \brief ͨ��ģʽ */

    /** \brief ��ʱ����λms�� */
    int       timeout;

    uint8_t *p_txbuf;      /**< \brief ���ͻ����� */
    uint32_t txbuf_size;   /**< \brief ���ͻ�������С */

    /** ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init)(void);
} awbl_zlg600a_uart_devinfo_t;

/**
 * \brief ZLG600A�����豸�ṹ�嶨��
 */
typedef struct awbl_zlg600a_uart_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev dev;

    /** \brief ��Ƭ����ṹ�� */
    awbl_zlg600a_service_t serv;

    uint8_t  addr;         /**< \brief Ĭ�ϴ���ĵ�ַ      */

} awbl_zlg600a_uart_dev_t;

/**
 * \brief ZLG600A UART driver register
 *        awbl_serail_init()��Ҫ��awbl_dev_connect()����֮ǰ��ʼ��
 */
void awbl_zlg600a_uart_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_ZLG600A_UART_H */

/* end of file */
