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
 * \brief modbus������Ϣ
 *
 * \internal
 * \par modification history
 * - 1.03 2019-03-26  cml, use serial interface replace int serial.
 * - 1.02 2015-05-18  cod, fix code style and comments, a little improve.
 * - 1.01 2013-03-29  liuweiyun, fix code style and comments, a little improve.
 * - 1.00 2012-05-03  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_COMM_H /* { */
#define __AW_MB_COMM_H

#include "modbus/aw_mb_compiler.h"

/**
 * \addtogroup grp_aw_if_mb_comm
 * \copydoc aw_mb_comm.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

/**
 * \name Modbus��ַ
 * @{
 */
#define AW_MB_ADDRESS_BROADCAST    0   /**< \brief Modbus�㲥��ַ */
#define AW_MB_ADDRESS_MIN          1   /**< \brief ��С�ӻ���ַ */
#define AW_MB_ADDRESS_MAX          247 /**< \brief ���ӻ���ַ */
/** @} */

/**
 * \name ���ù�����
 * @{
 */
#define AW_MB_FUNC_NONE                           0   /**< \brief ��ʹ�õĹ����� */
#define AW_MB_FUNC_READ_COILS                     1   /**< \brief ����Ȧ������ */
#define AW_MB_FUNC_READ_DISCRETE_INPUTS           2   /**< \brief ����ɢ�������� */
#define AW_MB_FUNC_READ_HOLDING_REGISTER          3   /**< \brief ������Ĵ��������� */
#define AW_MB_FUNC_READ_INPUT_REGISTER            4   /**< \brief ������Ĵ��������� */
#define AW_MB_FUNC_WRITE_SINGLE_COIL              5   /**< \brief д������Ȧ������ */
#define AW_MB_FUNC_WRITE_REGISTER                 6   /**< \brief д�����Ĵ��������� */
#define AW_MB_FUNC_WRITE_MULTIPLE_COILS          15   /**< \brief д����߹����� */
#define AW_MB_FUNC_WRITE_MULTIPLE_REGISTERS      16   /**< \brief д����Ĵ��������� */
#define AW_MB_FUNC_READWRITE_MULTIPLE_REGISTERS  23   /**< \brief ��д����Ĵ��������� */
#define AW_MB_FUNC_OTHER_REPORT_SLAVEID          17   /**< \brief ��ȡ�ӻ���Ϣ */

#define AW_MB_FUNC_ERROR                         128  /**< \brief �쳣������ */
/** @} */

/**
 * \name PDU defines
 * @{
 */
#define AW_MB_PDU_SIZE_MAX       253 /**< \brief ���PDU���� */
#define AW_MB_PDU_SIZE_MIN       1   /**< \brief �����볤�� */
#define AW_MB_PDU_FUNC_OFF       0   /**< \brief ��������PDU��ƫ�� */
#define AW_MB_PDU_DATA_OFF       1   /**< \brief ������PDU��ƫ�� */
/** @} */

/**
 * \name Modbus TCP MBAP defines
 * @{
 */
#define AW_MB_TCP_TID            0    /**< \brief �������ʶ�� */
#define AW_MB_TCP_PID            2    /**< \brief Э���ʶ�� */
#define AW_MB_TCP_LEN            4    /**< \brief ���� */
#define AW_MB_TCP_UID            6    /**< \brief ��Ԫ��ʶ�� */
#define AW_MB_TCP_FUNC           7    /**< \brief ������ */
/** @} */

/**
 * \name PID define
 * @{
 */
#define AW_MB_TCP_PROTOCOL_ID    0    /**< \brief Э���ʶ�� ID*/
/** @} */

/**
 * \name  TCP pseudo address define
 * @{
 */
#define AW_MB_TCP_PSEUDO_ADDRESS 255       /**< \brief Modbus TCPα��ַ */
/** @} */
/******************************************************************************/

/** \brief �쳣���� */
typedef enum aw_mb_exception {
    AW_MB_EXP_NONE                 = 0x00,     /**< \brief û���쳣 */
    AW_MB_EXP_ILLEGAL_FUNCTION     = 0x01,     /**< \brief �Ƿ����� */
    AW_MB_EXP_ILLEGAL_DATA_ADDRESS = 0x02,     /**< \brief �Ƿ����ݵ�ַ */
    AW_MB_EXP_ILLEGAL_DATA_VALUE   = 0x03,     /**< \brief �Ƿ�����ֵ */
    AW_MB_EXP_SLAVE_DEVICE_FAILURE = 0x04,     /**< \brief �ӻ��豸���� */
    AW_MB_EXP_ACKNOWLEDGE          = 0x05,     /**< \brief ȷ�� */
    AW_MB_EXP_SLAVE_BUSY           = 0x06,     /**< \brief �ӻ��豸æ */
    AW_MB_EXP_MEMORY_PARITY_ERROR  = 0x08,     /**< \brief �洢��ż�Բ��*/
    AW_MB_EXP_GATEWAY_PATH_FAILED  = 0x0A,     /**< \brief ����·�������� */
    AW_MB_EXP_GATEWAY_TGT_FAILED   = 0x0B      /**< \brief ����Ŀ���豸��Ӧʧ�� */
} aw_mb_exception_t;

/** \brief ������żУ��ģʽ */
enum aw_mb_parity {
    AW_MB_PAR_NONE,    /**< \brief ��У�� */
    AW_MB_PAR_ODD,     /**< \brief ��У�� */
    AW_MB_PAR_EVEN     /**< \brief żУ�� */
};

/** \brief ͨ��ģʽ���� */
enum aw_mb_mode {
    AW_MB_RTU,         /**< \brief ����RTUģʽ�Ĵ���MODBUS   */
    AW_MB_ASCII,       /**< \brief ����ASCIIģʽ�Ĵ���MODBUS */
    AW_MB_TCP,         /**< \brief ����TCP/IPЭ��ջ��MODBUS  */
};

/******************************************************************************/

/** \brief ���ڴ��ڴ�վ��MODBUS����ʼ������ */
struct aw_mb_serial_param {
    uint8_t           slave_addr;    /**< \brief �ӻ���ַ */
    enum aw_mb_parity parity;        /**< \brief ����У��ģʽ */
    uint32_t           port;          /**< \brief ���ں� */
    uint32_t          baud_rate;     /**< \brief ���ڲ����� */
};

/** \brief ���ڴ��ڵ�MODBUS��վ����ʼ������ */
typedef struct aw_mb_master_serial_params {
    uint32_t          port;          /**< \brief ���ں� */
    enum aw_mb_parity parity;        /**< \brief ����У��ģʽ */
    uint32_t          baud_rate;     /**< \brief ���ڲ����� */
} aw_mb_master_serial_params_t;

/** \brief Modbus TCP�ͻ��˵ĳ�ʼ������ */
typedef struct aw_mb_tcp_client_params {
    char    *ip_addr;      /**< \brief Ŀ��IP��ַ���硰192.168.28.111�� */
    uint16_t port;         /**< \brief Ŀ��˿� */
} aw_mb_tcp_client_params_t;

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_comm */

#endif /* } __AW_MB_COMM_H */

/* end of file */
