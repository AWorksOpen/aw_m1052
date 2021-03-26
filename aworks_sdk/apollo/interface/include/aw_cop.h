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
 * \brief CANopen�������ӻ������ļ�
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-31  anu, modify
 * - 1.00 15-11-16  cae, first implementation
 * \endinternal
 */

#ifndef __AW_COP_H
#define __AW_COP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_cop
 * \copydoc aw_cop.h
 * @{
 */

#include "aw_types.h"

/**
 * \name CANopen ��Ϣ
 * \anchor grp_aw_cop_max_slaves
 * @{
 */

#define AW_COP_MAX_SLAVES         127    /**< \brief ���ӻ����� */

/** @} */

/**
 * \name CANopen �ڵ�
 * \anchor grp_aw_cop_node_id
 * @{
 */

#define AW_COP_NODE_ID_MAX        0x7F   /**< \brief ���ڵ�ID */
#define AW_COP_NODE_ID_INVAILD    0x00   /**< \brief ��Ч�ڵ�ID */
#define AW_COP_NODE_ID_ALL        0x00   /**< \brief ���нڵ�ID */

/** @} */

/**
 * \name CANopen ID
 * \anchor grp_aw_cop_id
 * @{
 */

#define AW_COP_ID_SYNC_CLEAR    0x80000080L /**< \brief ���ͬ������ID */
#define AW_COP_ID_SYNC          0x00000080L /**< \brief ͬ������ID */
#define AW_COP_ID_TIME_STAMP    0x100       /**< \brief ʱ�������ID */
#define AW_COP_ID_SDO_SND       0x580       /**< \brief �������ݶ�����ID */
#define AW_COP_ID_SDO_RCV       0x600       /**< \brief �������ݶ�����ID */

/** @} */

/**
 * \name CANopen �������
 * \anchor grp_aw_cop_err_t
 * @{
 */

typedef int aw_cop_err_t;               /**< \brief ������� */
#define AW_COP_ERR_NO_ERROR         0   /**< \brief û�д��� */
#define AW_COP_ERR_LOADLIB          11  /**< \brief װ��DLLʧ�� */
#define AW_COP_ERR_GETPROC          12  /**< \brief ��ȡ������ַʧ�� */
#define AW_COP_ERR_OPENED           13  /**< \brief �豸�Ѿ����� */
#define AW_COP_ERR_NOTEXIST         14  /**< \brief �豸������ */
#define AW_COP_ERR_INITFAIL         15  /**< \brief ��ʼ���豸ʧ�� */
#define AW_COP_ERR_STARTDEV         16  /**< \brief ����CANopen�豸ʧ�� */
#define AW_COP_ERR_NOTOPEN          17  /**< \brief �豸û�д� */
#define AW_COP_ERR_INVALIDPARAM     18  /**< \brief ��Ч���� */
#define AW_COP_ERR_INVALIDHANDLE    19  /**< \brief ��Ч���豸��� */
#define AW_COP_ERR_CLOSEDEV         20  /**< \brief �޷��ر��豸 */
#define AW_COP_ERR_INSTALLDRIVER    22  /**< \brief ������װ����ȷ */
#define AW_COP_ERR_BUFFERTOOSMALL   23  /**< \brief �洢�ռ�̫С */
#define AW_COP_ERR_INTERNAL         29  /**< \brief δ֪�ڲ����� */
#define AW_COP_ERR_TIMEOUT          30  /**< \brief �ȴ���ʱ */
#define AW_COP_ERR_SLAVEEXIST       31  /**< \brief ��վ�Ѵ��� */
#define AW_COP_ERR_SLAVENOTEXIST    32  /**< \brief ��վ������ */
#define AW_COP_ERR_SLAVETABFUL      33  /**< \brief ��վע������ */
#define AW_COP_ERR_SENDFAILED       34  /**< \brief ��������ʧ�� */
#define AW_COP_ERR_NODATA           35  /**< \brief û������ */
#define AW_COP_ERR_GETSTATUS        36  /**< \brief ��ȡ��վ״̬ʧ�� */
#define AW_COP_ERR_NOTIMPLEMENT     37  /**< \brief ����û��ʵ�� */
#define AW_COP_ERR_NOTSTARTED       38  /**< \brief δ����CANopen */
#define AW_COP_ERR_SDOABORT         39  /**< \brief SDO ������ֹ */
#define AW_COP_ERR_PDOTABFULL       40  /**< \brief PDO ע������ */
#define AW_COP_ERR_PDONOTREGISTER   41  /**< \brief PDO δע�� */
#define AW_COP_ERR_CHNLNOTINIT      42  /**< \brief δ��ʼ��ͨ������ */
#define AW_COP_ERR_QUEUECREATE      43  /**< \brief ��������ʧ�� */
#define AW_COP_ERR_NOSEGCONT        44  /**< \brief �������� */
#define AW_COP_ERR_NOMEM            45  /**< \brief �޿����ڴ� */
#define AW_COP_ERR_OFFSET           46  /**< \brief ƫ�ƴ��� */
#define AW_COP_ERR_LENGTH           47  /**< \brief ���ȴ��� */

/** @} */

/** \brief CANopen PDO�������ݶ�����Ϣ */
typedef struct aw_cop_pdo_msg {

    uint8_t  node_id;          /**< \brief ��վ��ַ */
    uint32_t pdo_id;           /**< \brief PDO����  */
    uint8_t  pdo_len;          /**< \brief PDO���ݳ��� */
    uint8_t  pdo_data[8];      /**< \brief PDO���� */

} aw_cop_pdo_msg_t;

/** \brief CANopen ������������ */
typedef struct aw_cop_emcy_msg {

    uint8_t  node_id;          /**< \brief ��վ��ַ */
    uint16_t err_code;         /**< \brief ����������� */
    uint8_t  err_reg_code;     /**< \brief ��ǰ����Ĵ���ֵ */
    uint8_t  err_specific[5];  /**< \brief �Զ��������� */

} aw_cop_emcy_msg_t;

/** \brief CANopen ʱ�� */
typedef struct aw_cop_time {

    uint16_t year;             /**< \brief �� */
    uint8_t  month;            /**< \brief �� */
    uint8_t  day;              /**< \brief �� */
    uint8_t  hour;             /**< \brief Сʱ */
    uint8_t  minute;           /**< \brief ���� */
    uint8_t  second;           /**< \brief �� */
    uint16_t millisecond;      /**< \brief ���� */

} aw_cop_time_t;

/** \brief CANopen ������ */
typedef uint32_t aw_cop_baud_t;
#define AW_COP_BAUD_1M      1000  /**< \brief 1M ������ */
#define AW_COP_BAUD_800K    800   /**< \brief 800K ������ */
#define AW_COP_BAUD_500K    500   /**< \brief 500K ������ */
#define AW_COP_BAUD_250K    250   /**< \brief 250K ������ */
#define AW_COP_BAUD_125K    125   /**< \brief 125K ������ */
#define AW_COP_BAUD_100K    100   /**< \brief 100K ������ */
#define AW_COP_BAUD_50K     50    /**< \brief 50K ������ */
#define AW_COP_BAUD_20K     20    /**< \brief 20K ������ */
#define AW_COP_BAUD_10K     10    /**< \brief 10K ������ */

/** @} grp_aw_if_cop */

#ifdef __cplusplus
}
#endif

#endif /* __AW_COP_H */

/* end of file */
