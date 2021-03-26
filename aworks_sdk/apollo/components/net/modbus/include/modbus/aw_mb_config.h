/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Modbus�û������ļ�
 *
 * \internal
 * \par modification history
 * - 1.03 2019-03-26  cml, use serial interface replace int serial.
 * - 1.02 2015-05-13  cod, Modify for aworks1.0, fix code style
 * - 1.01 2012-12-04  liuweiyun, Modify for aworks1.0, fix code style
 * - 1.00 2012-05-13  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_CONFIG_H /* { */
#define __AW_MB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef AIXO_AWORKS
#include "aw_modbus_autoconf.h"
#endif

/**
 * \addtogroup grp_aw_if_mb_config
 * \copydoc aw_mb_config.h
 * @{
 */

/** \note ���ļ���������ΪĬ�����ã����ɿ��ֱ���޸ĺ궨����Ч */

/** \brief �ӻ�ͨ��ģʽ���� */

/** \brief ����RTUģʽ */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_SERIAL_RTU_ENABLE
#define AW_MB_SERIAL_RTU_ENABLE    1
#else
#define AW_MB_SERIAL_RTU_ENABLE    0
#endif
#else
#define AW_MB_SERIAL_RTU_ENABLE    1
#endif

/** \brief ����ASCIIģʽ */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_SERIAL_ASCII_ENABLE
#define AW_MB_SERIAL_ASCII_ENABLE    1
#else
#define AW_MB_SERIAL_ASCII_ENABLE    0
#endif
#else
#define AW_MB_SERIAL_ASCII_ENABLE    1
#endif

/** \brief TCP/IPģʽ */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_TCP_ENABLE
#define AW_MB_TCP_ENABLE    1
#else
#define AW_MB_TCP_ENABLE    0
#endif
#else
#define AW_MB_TCP_ENABLE    1
#endif


/** \brief ��վͨ��ģʽ���� */

/** \brief ����RTUģʽ */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_MASTER_RTU_ENABLE
#define AW_MB_MASTER_RTU_ENABLE    1
#else
#define AW_MB_MASTER_RTU_ENABLE    0
#endif
#else
#define AW_MB_MASTER_RTU_ENABLE    1
#endif


/** \brief ����ASCIIģʽ */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_MASTER_RTU_ENABLE
#define AW_MB_MASTER_ASCII_ENABLE    1
#else
#define AW_MB_MASTER_ASCII_ENABLE    0
#endif
#else
#define AW_MB_MASTER_ASCII_ENABLE    1
#endif


/** \brief TCP/IPģʽ */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_MASTER_TCP_ENABLE
#define AW_MB_MASTER_TCP_ENABLE    1
#else
#define AW_MB_MASTER_TCP_ENABLE    0
#endif
#else
#define AW_MB_MASTER_TCP_ENABLE    1
#endif


/**
 * \brief �������ADU����
 * - �ó��Ȱ����ӻ���ַ�ͼ������ݳ���
 * - ����ĳЩӦ�ã����ܲ���Ҫ�ܳ���һ֡���ݣ����Կ����ʵ������Խ���RAMʹ��
 * - RTUģʽ����Ϊ4~256�� ASCIIģʽ����Ϊ3~255
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_MAX_SER_ADU_LENGTH)
#define AW_MB_MAX_SER_ADU_LENGTH    CONFIG_AW_MB_MAX_SER_ADU_LENGTH
#else
#define AW_MB_MAX_SER_ADU_LENGTH    256
#endif

/**
 * \brief TCPģʽ�����ADU����
 * - ����ĳЩӦ�ã����ܲ���Ҫ�ܳ���һ֡���ݣ����Կ����ʵ������Խ���RAMʹ��
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_MAX_TCP_ADU_LENGTH)
#define AW_MB_MAX_TCP_ADU_LENGTH    CONFIG_AW_MB_MAX_TCP_ADU_LENGTH
#else
#define AW_MB_MAX_TCP_ADU_LENGTH    263
#endif

/**
 * \brief RTUģʽT3.5ʱ�����ã���λ��΢�룩
 * - ��RTUģʽ�У������Ŷ�t3.5��֡���ӳ�ʱ�䣩����Ĵ����жϵĴ���
 *   �ڽϸߵ�ͨ�Ų������£��⽫����CPU�������أ�
 *   ��˵�������С�ڻ����19200λ/�������£������ϸ����������ʱ
 * - ���������ʴ���19200λ/��ʱ��Ӧ��ʹ�ù̶�ֵ������֡���ӳ�ʱ������Ϊ1.750ms��
 * - ��ʹ��ASCIIģʽʱ������Ҫ�Ըú��������
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_SERIAL_RTU_BAUDRATE_OVER_19200_FRAME_DELAY3_5_US)
#define AW_MB_SERIAL_RTU_BAUDRATE_OVER_19200_FRAME_DELAY3_5_US    CONFIG_AW_MB_SERIAL_RTU_BAUDRATE_OVER_19200_FRAME_DELAY3_5_US
#else
#define AW_MB_SERIAL_RTU_BAUDRATE_OVER_19200_FRAME_DELAY3_5_US    1750ul
#endif

/**
 * \brief ASCIIģʽ�ַ��䳬ʱ���ã���λ��ms��
 * ��ASCIIģʽ�У��ַ��䳬ʱʱ��û�й̶������Ը��ݾ�������������
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_ASCII_TIMEOUT_SEC)
#define AW_MB_ASCII_TIMEOUT_SEC    CONFIG_AW_MB_ASCII_TIMEOUT_SEC
#else
#define AW_MB_ASCII_TIMEOUT_SEC    1000
#endif

/**
 * \brief �û�����չע�������������
 * ����ʹ��aw_mb_slave_register_handler������ע�Ṧ�����������ʱ����ע��������
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_FUNC_CODE_HANDLER_MAX)
#define AW_MB_FUNC_CODE_HANDLER_MAX    CONFIG_AW_MB_FUNC_CODE_HANDLER_MAX
#else
#define AW_MB_FUNC_CODE_HANDLER_MAX    4
#endif

/**
 * \brief Ĭ�ϴ�վ���������������������Ĭ��������ʱ���ر����罨�������ӣ�
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_SLAVE_TCP_CONNECT_NUM)
#define AW_MB_SLAVE_TCP_CONNECT_NUM    CONFIG_AW_MB_SLAVE_TCP_CONNECT_NUM
#else
#define AW_MB_SLAVE_TCP_CONNECT_NUM    4
#endif

/**
 * \brief ��վ�ȴ���վӦ��ʱʱ�䣨���룩
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_MASTER_TIMEOUT_MS_RESPOND)
#define AW_MB_MASTER_TIMEOUT_MS_RESPOND    CONFIG_AW_MB_MASTER_TIMEOUT_MS_RESPOND
#else
#define AW_MB_MASTER_TIMEOUT_MS_RESPOND    200
#endif

#if ((AW_MB_MASTER_TIMEOUT_MS_RESPOND < 50)  ||\
     (AW_MB_MASTER_TIMEOUT_MS_RESPOND > 100000))
#error "AW_MB_MASTER_TIMEOUT_MS_RESPOND value must be in 50~100000ms"
#endif

/**
 * \brief ��վ�㲥ʱ��ת���ӳ�ʱ�䣨���룩
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_MASTER_DELAY_MS_CONVERT)
#define AW_MB_MASTER_DELAY_MS_CONVERT    CONFIG_AW_MB_MASTER_DELAY_MS_CONVERT
#else
#define AW_MB_MASTER_DELAY_MS_CONVERT    100
#endif

/**
 * \brief ��վ�û�����չע�������������
 * ����ʹ��aw_mb_master_funcode_register������ע�Ṧ�����������ʱ����ע��������
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_MASTER_FUNCODE_HANDLER_MAX)
#define AW_MB_MASTER_FUNCODE_HANDLER_MAX    CONFIG_AW_MB_MASTER_FUNCODE_HANDLER_MAX
#else
#define AW_MB_MASTER_FUNCODE_HANDLER_MAX   4
#endif

/**
 * \brief TCPģʽ�µ�Ԫ��ʾ��ʹ�ô�վ��ַʹ��
 * - ����ĳЩӦ�ã���Modbus���������ӵ�������·����ʹ�õ�Ԫ��ʾ��ʶ���վ��ַ
 * - ��ʹ�ܣ���Ԫ��ʾ������Ŀ���վ��ַʹ�ã�����ʹ�ܣ���Ԫ��ʾ��ֵĬ��Ϊ0xFF
 */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_MASTER_RTU_ENABLE
#define AW_MB_TCP_UID_AS_SLAVEADDR    1
#else
#define AW_MB_TCP_UID_AS_SLAVEADDR    0
#endif
#ifndef AXIO_AWORKS 
#define AW_MB_TCP_UID_AS_SLAVEADDR    1
#endif
#endif


/**
 * \brief ��վTCPģʽ�£�Ĭ�����ӳ�ʱʱ��(ms)
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_TCP_CONNECT_TIMEOUT)
#define AW_MB_TCP_CONNECT_TIMEOUT    CONFIG_AW_MB_TCP_CONNECT_TIMEOUT
#else
#define AW_MB_TCP_CONNECT_TIMEOUT   5000
#endif

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_config */

#endif /* } __AW_MB_CONFIG_H */


/* end of file */
