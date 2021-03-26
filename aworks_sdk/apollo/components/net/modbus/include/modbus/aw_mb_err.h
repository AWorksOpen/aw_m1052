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
 * \brief modbus�����ͷ�ļ�.
 *
 * \internal
 * \par modification history
 * - 1.03 2019-03-26  cml, use serial interface replace int serial.
 * - 1.02 2015-05-18  cod, fix code style and comments, a little improve.
 * - 1.01 2013-03-29  liuweiyun, fix code style and comments, a little improve.
 * - 1.00 2012-05-03  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_ERR_H /* { */
#define __AW_MB_ERR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_mb_err
 * \copydoc aw_mb_err.h
 * @{
 */

/** \brief ����� */
typedef enum aw_mb_err {
    AW_MB_ERR_NOERR,                   /**< \brief û�д��� */
    AW_MB_ERR_ALLOC_FAIL,              /**< \brief �����ڴ�ʧ�� */
    AW_MB_ERR_ENOREG,                  /**< \brief �Ƿ�ע���ַ */
    AW_MB_ERR_EINVAL,                  /**< \brief �Ƿ����� */
    AW_MB_ERR_EPORTERR,                /**< \brief �˿ڴ��� */
    AW_MB_ERR_ENORES,                  /**< \brief ��Դ���� */
    AW_MB_ERR_EIO,                     /**< \brief I/O���� */
    AW_MB_ERR_ENOTSUP,                 /**< \brief ��֧�ָù��� */
    AW_MB_ERR_EILLSTATE,               /**< \brief Э��ջ�Ƿ�״̬ */
    AW_MB_ERR_ETIMEDOUT,               /**< \brief ��ʱ���� */
    AW_MB_ERR_ECRC,                    /**< \brief CRC����  */
    AW_MB_ERR_EFRAME_TICK,             /**< \brief ֡ʱ����� */
    AW_MB_ERR_EFRAME_LEN,              /**< \brief ֡���ȴ��� */
    AW_MB_ERR_EUNEXECPTION_FUNCTION,   /**< \brief ���ڴ����ܴ��� */
    AW_MB_ERR_ILLEGAL_FUNCTION,        /**< \brief �Ƿ����� */
    AW_MB_ERR_ILLEGAL_DATA_ADDRESS,    /**< \brief �Ƿ����ݵ�ַ */
    AW_MB_ERR_ILLEGAL_DATA_VALUE,      /**< \brief �Ƿ�����ֵ */
    AW_MB_ERR_SLAVE_DEVICE_FAILURE,    /**< \brief �ӻ��豸���� */
    AW_MB_ERR_ACKNOWLEDGE,             /**< \brief ȷ�� */
    AW_MB_ERR_SLAVE_BUSY,              /**< \brief �ӻ��豸æ */
    AW_MB_ERR_MEMORY_PARITY_ERROR,     /**< \brief �洢��ż�Բ��*/
    AW_MB_ERR_GATEWAY_PATH_FAILED,     /**< \brief ����·�������� */
    AW_MB_ERR_GATEWAY_TGT_FAILED,      /**< \brief ����Ŀ���豸��Ӧʧ�� */

    AW_MB_ERR_MODE_NO_SUPPORT,         /**< \brief ��֧�ֵ�ͨѶģʽ */
    AW_MB_ERR_THREAD_FAIL,             /**< \brief �̴߳���ʧ�� */
    AW_MB_ERR_OPEN_FILE,               /**< \brief ��ͨ���ļ�ʧ�� */
    AW_MB_ERR_UNKOWN_EXP,              /**< \brief δ֪�쳣 */

    AW_MB_ERR_MASTER_RCV_ERR,          /**< \brief ��վ�������ݴ���  */
} aw_mb_err_t;

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_err */

#endif /* } __AW_MB_ERR_H */

/* end of file */
