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
 * \brief NS16550 compatible UART plb interface driver support
 *
 * ���������������� 16550 ��׼���ݵ�UART����
 *
 * \par 1.������Ϣ
 *
 *  - ������:    "ns16550"
 *  - ��������:  AWBL_BUSID_PLB
 *  - �豸��Ϣ:  struct awbl_ns16550_plb_chan_param
 *
 * \par 2.�����豸
 *
 *  - ���� 16550 ��׼���ݵ�UART����
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_ns16550_plb_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_serial
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-28  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_NS16550_PLB_H
#define __AWBL_NS16550_PLB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_ns16550
 * \copydetails awbl_ns16550_plb.h
 * @{
 */

/**
 * \defgroup  grp_awbl_ns16550_plb_hwconf �豸����/����
 *
 * ns16550 �����豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ��
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 *
 * \note �ж��ٸ�16550�����豸���ͽ�����Ĵ���Ƕ����ٴΣ�ע�⽫�����޸�Ϊ��ͬ��
 *       ���֡�
 *
 * \include  hwconf_usrcfg_ns16550_plb.c
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1��ƽ̨��س�ʼ������ \n
 * ��ƽ̨��ʼ�����������ʹ�ܴ���ʱ�ӡ���ʼ��Ӳ�����ŵ���ƽ̨��صĲ�����
 *
 * - ��2�����ڵ��жϺ� \n
 * ��ο�ϵͳ���жϺŷ�����д�˲������� \ref grp_aw_plfm_inums
 *
 * - ��3���豸�Ĵ�������ʼ��ַ \n
 * �����оƬ�����ֲ�ȷ���豸�ļĴ�����ʼ��ַ
 *
 * - ��4���豸�Ĵ��������λֵ \n
 * 16C550����UART�豸�ļĴ�����СΪ8λ���ڲ�ͬ��ʵ�����в�ͬ�ļ��������󲿷�
 * 32λSoC�ϵ�16C550����UART�ļĴ�����ַΪ4�ֽڶ��룬�����Ϊ4 \n
 * ����Ϊ��Ч�����ʹ�õ��Ǽ������λֵ��������4����λֵΪ2��4 == (1 << 2))��
 * ���Ϊ2����λֵΪ1�����Ϊ1����λֵΪ0
 *
 * - ��5��UART�豸��FIFO��С \n
 * ����������ֲ�ȷ��FIFO�Ĵ�С����λΪ�ֽ�
 *
 * - ��6��UART�豸������ʱ��Ƶ�� \n
 * �������ʱ�ӿ������ⲿ�ľ��������ṩ��������ϵͳ��Ƶ���ṩ����ο������
 * ��·��SoC�ֲ��Լ�ϵͳʱ�����õ���ȷ��
 *
 * - ��7��UART�����ʷ�Ƶֵ���㺯�� \n
 *  - һ��ʹ�������Դ��Ĳ����ʼ��㺯��������ʹ��������㺯��
 *  - ��������豸������Ĳ����ʼ��㷽ʽ�����С�����������ã������ʹ�øú�����
 *    ����ԭ��Ϊ�� \n
 *    uint32_t uart_divisor(uint32_t xtal, uint32_t baudrate);
 *      - \a xtal     ΪUART�豸������ʱ��Ƶ�� \n
 *      - \a baudrate Ϊ�����Ĳ����� \n
 *      - ����ֵΪ��Ƶֵ
 *
 * - ��8���豸��Ԫ�� \n
 * �����Ԫ�ż������豸�ġ����ںš�
 *
 */

/** @} */

#include "aw_common.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "awbl_access.h"

#include "driver/serial/awbl_ns16550.h"

/*******************************************************************************
    types
*******************************************************************************/

/** \brief ns16550 plb channel */
struct awbl_ns16550_plb_chan {
    struct awbl_dev          dev;       /**< \brief spi device data */
    struct awbl_ns16550_chan nschan;    /**< \brief NS16550 channel data */
};

/** \brief ns16550 plb channel parameter */
struct awbl_ns16550_plb_chan_param {

    /** \brief NS16550 parameter, always go first */
    struct awbl_ns16550_chan_param  nsparam;
};


#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_NS16550_PLB_H */

/* end of file */
