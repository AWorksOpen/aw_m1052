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
 * \brief AWorks ��׼I2C�ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_i2c.h
 *
 * \par ��ʾ��
 * \code
 * #include "aw_i2c.h"
 *
 * aw_i2c_device_t dev;                     // �豸����
 * uint8_t         read_buf[16];            // ��������
 *
 * // ���ɴӻ��豸
 * aw_i2c_mkdev(&dev,
 *              I2C0,
 *              0x50,
 *              AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);
 *
 * // д�����ݵ��������ӵ�ַ0��ʼ��16���ֽ�
 * aw_i2c_write(&dev, 0, &write_buf[0], 16);
 *
 * // �������ӵ�ַ0��ʼ��16���ֽ�
 * aw_i2c_read(&dev, 0, &read_buf[0], 16);
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.01 12-11-16  zen, add aw_i2c_async() and aw_i2c_sync()
 * - 1.00 12-07-27  orz, first implementation
 * \endinternal
 */

#ifndef __AW_I2C_H
#define __AW_I2C_H

#ifdef __cplusplus
extern "C" {
#endif
#include "aw_list.h"

/**
 * \addtogroup grp_aw_if_i2c
 * \copydoc aw_i2c.h
 * @{
 */

/**
 * \name I2C������Ʊ�־
 * @{
 * ����I2C����ĵ�ַ���͡���д���͡�������Ƶȱ�־
 *
 * ��Щ��־�ࡰ��֮��Ľ�����ڸ��ṹ���Ա
 * <code>aw_i2c_transfer::flags</code>��ֵ��δ��ʽ�����ı�־��ʹ��Ĭ��ֵ��
 * ���磬�������Щ��ֵ���õ���ͬ�Ŀ���Ч����
 * \code
 * aw_i2c_transfer_t trans;
 * trans.flags = 0;             // 7-bit�ӻ���ַ(Ĭ��)��д����(Ĭ��)
 * trans.flags = AW_I2C_M_WR;   // 7-bit�ӻ���ַ(Ĭ��)��д����
 * trans.flags = AW_I2C_M_RD;   // 7-bit�ӻ���ַ(Ĭ��)��������
 * trans.flags = AW_I2C_M_10BIT | AW_I2C_M_RD;   // 10-bit�ӻ���ַ��������
 *
 * trans.flags = AW_I2C_M_7BIT |
 *               AW_I2C_M_WR |
 *               AW_I2C_M_IGNORE_NAK;// 7-bit�ӻ���ַ��д������������Ӧ�����
 * \endcode
 *
 * \sa struct aw_i2c_transfer
 */
#define AW_I2C_M_7BIT          0x0000u     /**< \brief 7-bit�豸��ַ(Ĭ��) */
#define AW_I2C_M_10BIT         0x0001u     /**< \brief 10-bit�豸��ַ */
#define AW_I2C_M_WR            0x0000u     /**< \brief д����(Ĭ��) */
#define AW_I2C_M_RD            0x0002u     /**< \brief ������ */
#define AW_I2C_M_NOSTART       0x0004u     /**< \brief ������ start ����(ֻ��д��Ч) */
#define AW_I2C_M_REV_DIR_ADDR  0x0008u     /**< \brief ��д��־λ��ת */
#define AW_I2C_M_RECV_LEN      0x0010u     /**< \brief ��ʱ��֧�� */

/** \brief ����������Ӧ����� (�����ӻ���ַ��Ӧ���������Ӧ��) */
#define AW_I2C_M_IGNORE_NAK    0x0020u

/** \brief �ڶ������н�������ʱ������Ӧ�� */
#define AW_I2C_M_NO_RD_ACK     0x0040u

/** \brief I2C������Ʊ�־���� */
#define AW_I2C_M_MASK          0x00FFu

/** @} */


/**
 * \name I2C���豸���Ա�־
 * @{
 * ����I2C�ӻ���ַ���͡��������ӵ�ַ���ͺ��������ӵ�ַ��ȵȱ�־
 *
 * ���磬�������Щ��ֵ�����岻ͬ�Ĵ��豸���ԣ�
 * \code
 * aw_i2c_device_t dev;
 * dev.flags = 0;                    ��// 7-bit �ӻ���ַ�����ӵ�ַ
 * dev.flags = AW_I2C_SUBADDR_1BYTE;  // 7-bit �ӻ���ַ��1�ֽ��ӵ�ַ
 * dev.flags = AW_I2C_SUBADDR_2BYTE;  // 7-bit �ӻ���ַ��2�ֽ��ӵ�ַ��
 *                                   // �ӵ�ַ��λ��ַ�ȴ���
 *
 * dev.flags = AW_I2C_SUBADDR_2BYTE |
 *             AW_I2C_SUBADDR_LSB_FIRST; // 7-bit �ӻ���ַ��2�ֽ��ӵ�ַ��
 *                                      // �ӵ�ַ��λ�ֽ��ȴ���
 *
 * \endcode
 *
 * \sa struct aw_i2c_device
 */

/** \brief 7λ��ַģʽ(Ĭ��ģʽ) */
#define AW_I2C_ADDR_7BIT         AW_I2C_M_7BIT

/** \brief 10λ��ַģʽ */
#define AW_I2C_ADDR_10BIT        AW_I2C_M_10BIT

/** \brief �����豸����Ӧ�� */
#define AW_I2C_IGNORE_NAK        AW_I2C_M_IGNORE_NAK

/** \brief �������ӵ�ַ��λ�ֽ��ȴ���(Ĭ��)*/
#define AW_I2C_SUBADDR_MSB_FIRST 0x0000u

/** \brief �������ӵ�ַ��λ�ֽ��ȴ��� */
#define AW_I2C_SUBADDR_LSB_FIRST 0x0100u

#define AW_I2C_SUBADDR_NONE      0x0000u  /**< \brief ���ӵ�ַ(Ĭ��) */
#define AW_I2C_SUBADDR_1BYTE     0x1000u  /**< \brief �ӵ�ַ���1�ֽ� */
#define AW_I2C_SUBADDR_2BYTE     0x2000u  /**< \brief �ӵ�ַ���2�ֽ� */

/** \brief �ӱ�־��������ȡ���ӵ�ַ��� */
#define AW_I2C_SUBADDR_LEN_GET(flags)  (((flags) & 0xF000) >> 12)

/** @} */

/** \brief I2C���豸�����ṹ */
typedef struct aw_i2c_device {
    /** \brief �ӻ��豸 */
    uint8_t     busid;

    uint16_t    addr;       /**< \brief �ӻ��豸��ַ��7λ��10λ */
    uint16_t    flags;      /**< \brief �ӻ��豸���ԣ�����I2C���豸���Ա�־��*/
} aw_i2c_device_t;

/** \brief I2C ���� (�Ƽ�ʹ�� aw_i2c_mktrans() ���ñ����ݽṹ) */
typedef struct aw_i2c_transfer {
    uint16_t    addr;   /**< \brief ������ַ */
    uint16_t    flags;  /**< \brief ������Ʊ�־������I2C������Ʊ�־��*/
    uint8_t    *p_buf;  /**< \brief ���ݻ����� */
    size_t      nbytes; /**< \brief ���ݸ��� */
} aw_i2c_transfer_t;

/** \brief I2C ��Ϣ (�Ƽ�ʹ�� aw_i2c_mkmsg() ���ñ����ݽṹ) */
typedef struct aw_i2c_message {
    /** \brief ָ��ǰmessage�Ŀ����� */
    struct awbl_i2c_master *p_master;

    struct aw_i2c_transfer *p_transfers;    /**< \brief �����Ϣ�Ĵ��� */

    uint16_t                trans_num;      /**< \brief ������Ĵ������ */
    uint16_t                done_num;       /**< \brief �ɹ�����Ĵ������ */

    aw_pfuncvoid_t  pfunc_complete; /**< \brief ������ɻص����� */
    void           *p_arg;          /**< \brief ���ݸ� pfunc_complete �Ĳ��� */
    int             status;         /**< \brief ��Ϣ��״̬ */

    /** \brief msg�ڵ� */
    struct aw_list_head msg;
} aw_i2c_message_t;

struct aw_i2c_config {

    /** \brief i2c �ٶ�  */
    uint32_t speed;

    /** \brief ��䳬ʱ(ms)*/
    uint32_t timeout;

    /** \brief �ڿ�ʼ�µĴ���ǰ������Ҫ��С���ֿ��е�ʱ��(us) */
    uint32_t bus_idle_time;
};

/**
 * \brief ����I2C�豸�ṹ�����
 *
 * \param[out] p_dev  I2C�ӻ��豸������ָ��
 * \param[in]  busid  �ӻ��豸����
 * \param[in]  addr   �ӻ��豸��ַ
 * \param[in]  flags  �ӻ��豸��־������I2C���豸���Ա�־��
 *
 * \par ����
 * \code
 * #include "aw_i2c.h"
 *
 * aw_i2c_device_t cat1025; // CAT1025���豸�����ṹ
 *
 * // ��ʼ��CAT1025�豸�����ṹ��λ��I2C0���������豸��ַ0x50��
 * // �Ĵ�����ַΪ1�ֽڿ�
 * aw_i2c_mkdev(&cat1025,
 *              I2C0,
 *              0x50,
 *              AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);
 * \endcode
 */
aw_local aw_inline void aw_i2c_mkdev (aw_i2c_device_t *p_dev,
                                      uint8_t          busid,
                                      uint16_t         addr,
                                      uint16_t         flags)
{
    p_dev->busid = busid;
    p_dev->addr  = addr;
    p_dev->flags = flags;
}

/**
 * \brief ����I2C����ṹ�����
 *
 * \param[in]  p_trans   I2C����������ָ��
 * \param[in]  addr      Ŀ��������ַ
 * \param[in]  flags     ������Ʊ�־������I2C������Ʊ�־��
 * \param[out] p_buf     ���ݻ�����
 * \param[in]  nbytes    �������ݸ���
 *
 * \par ����
 * �� aw_i2c_mkmsg()
 */
aw_local aw_inline void aw_i2c_mktrans (aw_i2c_transfer_t *p_trans,
                                        uint16_t           addr,
                                        uint16_t           flags,
                                        uint8_t           *p_buf,
                                        size_t             nbytes)
{
    p_trans->addr   = addr;
    p_trans->flags  = flags;
    p_trans->p_buf  = p_buf;
    p_trans->nbytes = nbytes;
}

/**
 * \brief ����I2C��Ϣ�ṹ�����
 *
 * \param[in,out] p_msg             I2C��Ϣ������ָ��
 * \param[in]     p_transfers       �����Ϣ�ġ�I2C���䡱
 * \param[in]     trans_num         ��I2C���䡱�ĸ���
 * \param[in]     pfunc_complete    ������ɻص�����(������ aw_i2c_async() ʱ�˲�����Ч,
 *                                  ���� aw_i2c_sync()ʱ��������Ч)
 * \param[in]     p_arg             ���ݸ��ص������Ĳ���
 *
 * \par ����
 * \code
 * #include "aw_i2c.h"
 *
 * aw_i2c_message_t msg;        // ��Ϣ�����ṹ��
 * aw_i2c_transfer_t trans[2];  // ���������ṹ
 * uint8_t data0[16]            // ���ݻ����� 0
 * uint8_t data1[16];           // ���ݻ����� 1
 *
 * // ���ô��������ṹ��
 * aw_i2c_mktrans(&trans[0],    // ���������ṹ 0
 *                0x50,         // Ҫ�����Ŀ������(�ӻ�)��ַ
 *                AW_I2C_M_7BIT | AW_I2C_M_WR��// 7-bit�ӻ���ַ��д����
 *                1,            // ���ݳ��ȣ�1�ֽ�
 *                &data0[0]);   // ���ݻ����� 0
 *
 * aw_i2c_mktrans(&trans[1],    // ���������ṹ 1
 *                0x50,         // Ҫ�����Ŀ������(�ӻ�)��ַ
 *                AW_I2C_M_7BIT | AW_I2C_M_RD��// 7-bit�ӻ���ַ��������
 *                16,           // ���ݳ��ȣ�16�ֽ�
 *                &data1[0]);   // ���ݻ�����1
 *
 * // ������Ϣ�����ṹ��
 * aw_i2c_mkmsg(&msg,           // ��Ϣ�����ṹ
 *              &trans[0],      // �����Ϣ�Ĵ���
 *              2,              // �������
 *              my_callback,    // ������ɻص�����
 *              my_arg);        // ������ɻص������Ĳ���
 *
 * \endcode
 */
aw_local aw_inline void aw_i2c_mkmsg (aw_i2c_message_t  *p_msg,
                                      aw_i2c_transfer_t *p_transfers,
                                      uint16_t           trans_num,
                                      aw_pfuncvoid_t     pfunc_complete,
                                      void              *p_arg)
{
    p_msg->p_transfers    = p_transfers;
    p_msg->trans_num      = trans_num;
    p_msg->pfunc_complete = pfunc_complete;
    p_msg->p_arg          = p_arg;
    p_msg->status         = -AW_EISCONN;
}

/**
 * \brief ��I2C�ӻ�����
 *
 * \param[in]  p_dev       I2C�ӻ��豸������
 * \param[in]  subaddr     �������ӵ�ַ���Ӵ˵�ַ��ʼ��ȡ���ݣ�
 *                         ��\a p_dev->flags �����ӵ�ַ���Ϊ0
 *                         (#AW_I2C_SUBADDR_NONE), ��˲�����������
 * \param[out] p_buf       ���ݻ���������ȡ�����ݴ���ڴ�
 * \param[in]  nbytes      Ҫд������ݸ���
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a p_dev->flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIMEDOUT   ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a p_dev->flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * \note  �ýӿ���д���ַ���͵����ظ���ʼ�ź�Ȼ���ٶ����ݣ�������豸��֧���ظ���ʼ�źŵĶ��������ʧ��
 *
 * \par ����
 * \code
 * #include "aw_i2c.h"
 *
 * aw_i2c_device_t dev;                     // �豸����
 * uint8_t         read_buf[16];            // ��������
 *
 * aw_i2c_mkdev(&dev,
 *              I2C0,
 *              0x50,
 *              AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);
 *
 * aw_i2c_read(&dev, 0, &read_buf[0], 16);  // �������ӵ�ַ0��ʼ��16���ֽ�
 * \endcode
 */
aw_err_t aw_i2c_read(aw_i2c_device_t  *p_dev,
                     uint32_t          subaddr,
                     uint8_t          *p_buf,
                     size_t            nbytes);

/**
 * \brief д���ݵ�I2C�ӻ�
 *
 * \param[in]  p_dev       I2C�ӻ��豸������
 * \param[in]  subaddr     �������ӵ�ַ���Ӵ˵�ַ��ʼд�����ݣ�
 *                         ��\a p_dev->flags �����ӵ�ַ���Ϊ0
 *                         (#AW_I2C_SUBADDR_NONE), ��˲�����������
 * \param[in]  p_buf       ���ݻ�������Ҫд�����ݴ���ڴ�
 * \param[in]  nbytes      Ҫд������ݸ���
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a p_dev->flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIMEDOUT   ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a p_dev->flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * ע��:д�ӵ�ַ��д����֮����û��ֹͣ�źŵġ�
 *
 * \par ����
 * \code
 * #include "aw_i2c.h"
 *
 * aw_i2c_device_t dev;                     // �豸����
 * uint8_t         write_buf[16];           // д������
 *
 * aw_i2c_mkdev(&dev,
 *              I2C0,
 *              0x50,
 *              AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);
 *
 * // д�����ݵ��������ӵ�ַ0��ʼ��16���ֽ�
 * aw_i2c_write(&dev, 0, &write_buf[0], 16);
 * \endcode
 */
aw_err_t aw_i2c_write(aw_i2c_device_t  *p_dev,
                      uint32_t          subaddr,
                      const void       *p_buf,
                      size_t            nbytes);

/**
 * \brief ������Ϣ��ͬ��ģʽ
 *
 * ���첽�ķ�ʽ������Ϣ����Ϣ�Ĵ���״̬�ͽ����ӳ��\a p_msg->status��
 * ��Ϣ�������(�ɹ�����ʱ�����)ʱ���������\a p_msg->pfunc_complete ������
 * ����\a p_msg->p_arg��
 *
 * \param[in]       busid   Ҫ������I2C���߱��
 * \param[in,out]   p_msg   Ҫ�������Ϣ
 *
 * �����ķ���ֵ���£�
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a p_dev->flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIMEDOUT   ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a p_dev->flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * �ɹ�����Ĵ��������ӳ��\a p_msg->done_num
 *
 * ��Ϣ�Ĵ���״̬�ͽ����ӳ��\a p_msg->status:
 *
 *      \li  -AW_ENOTCONN       ��Ϣ��δ�Ŷ�
 *      \li  -AW_EISCONN        ��Ϣ�����ŶӴ���
 *      \li  -AW_EINPROGRESS    ��Ϣ���ڱ�����
 * ����Ϊ��Ϣ������ϵĽ�� (��ͨ��\a p_msg->done_num ��λ����Ĵ���)
 *      \li   AW_OK             ���д���ɹ�����
 *      \li  -AW_ENOTSUP        ĳ�δ���Ŀ��Ʊ�־��֧��
 *      \li  -AW_ETIMEDOUT      ��ʱ����Ϣδ���趨��ʱ���ڴ����ڿ�������devinfo��
 *                              ���ó�ʱʱ��
 *      \li  -AW_ENODEV         ĳ�δ�����Ӧ��
 *      \li  -AW_EIO            ������Ϣʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * \par ʾ��
 * \code
 *  #include "aw_i2c.h"
 *  #include "aw_sem.h"
 *
 *  aw_i2c_message_t  msg;
 *  aw_i2c_transfer_t trans[2];
 *  uint8_t           subaddr;
 *  uint8_t           data[EEPROM_PAGE_SIZE];
 *  int               i;
 *
 *  // ���岢��ʼ���ź���
 *  AW_SEMB_DECL_STATIC(sem_sync);
 *  AW_SEMB_INIT(sem_sync, 0, AW_SEM_Q_PRIORITY);
 *
 *  // дEEPROM�ӵ�ַ0x00��ʼ��һ��ҳ (ʱ����ο������ֲ�)
 *
 *  // ��1�����䷢���ӵ�ַ
 *  subaddr = 0x00;
 *  aw_i2c_mktrans(&trans[0],
 *                 EEPROM_ADDR,                 // Ŀ��������ַ
 *                 AW_I2C_M_7BIT | AW_I2C_M_WR, // 7-bit������ַ, д����
 *                 &subaddr,                    // �ӵ�ַ
 *                 1);                          // �ӵ�ַ���ȣ�1byte
 *
 *  // ��2�����䷢������
 *  for (i = 0; i < EEPROM_PAGE_SIZE; i++) {
 *      data[i] = i;
 *  }
 *  aw_i2c_mktrans(&trans[1],
 *                 EEPROM_ADDR,                 // Ŀ��������ַ (��������)
 *
 *                 // д����, ��������ʼ�źż��ӻ���ַ
 *                 AW_I2C_M_WR | AW_I2C_M_NOSTART,
 *
 *                 &data[0],                    // ���ݻ�����
 *                 EEPROM_PAGE_SIZE);           // ���ݸ���
 *
 *  // �����������Ϣ
 *  aw_i2c_mkmsg(&msg,
 *               &trans[0],
 *               2,
 *               eeprom_callback,
 *               &sem_sync);
 *
 *  // �������Ϣ���첽
 *  aw_i2c_async(I2C0, &msg);
 *
 *  // �ȴ���Ϣ�������
 *  aw_semb_take(&sem_sync, AW_SEM_WAIT_FOREVER);
 *
 *  // ��EEPROM�ӵ�ַ0x00��ʼ��һ��ҳ  (ʱ����ο������ֲ�)
 *
 *  // ��1�����䷢���ӵ�ַ
 *  subaddr = 0x00;
 *  aw_i2c_mktrans(&trans[0],
 *                 EEPROM_ADDR,                 // Ŀ��������ַ
 *                 AW_I2C_M_7BIT | AW_I2C_M_WR, // 7-bit������ַ, д����
 *                 &subaddr,                    // �ӵ�ַ
 *                 1);                          // �ӵ�ַ���ȣ�1byte
 *
 *  // ��2�������������
 *  for (i = 0; i < EEPROM_PAGE_SIZE; i++) {
 *      data[i] = 0;
 *  }
 *  aw_i2c_mktrans(&trans[1],
 *                 EEPROM_ADDR,                 // Ŀ��������ַ
 *                 AW_I2C_M_7BIT | AW_I2C_M_RD, // 7-bit������ַ, ������
 *                 &data[0],                    // ���ݻ�����
 *                 EEPROM_PAGE_SIZE);           // ���ݸ���
 *
 *  // �����������Ϣ
 *  aw_i2c_mkmsg(&msg,
 *               &trans[0],
 *               2,
 *               NULL,
 *               NULL);
 *
 *  // �������Ϣ��ͬ��
 *  aw_i2c_sync(EEPROM_BUSID, &msg);
 *
 *  // У�������Ƿ���ȷ
 *
 *  for (i = 0; i < EEPROM_PAGE_SIZE; i++) {
 *      if (data[i] != i) {
 *          return AW_ERROR;
 *      }
 *  }
 *
 *  return AW_OK;
 * \endcode
 */
aw_err_t aw_i2c_sync (uint8_t           busid,
                      aw_i2c_message_t *p_msg);

/**
 * \brief ������Ϣ���첽ģʽ
 *
 * ��ͬ���ķ�ʽ������Ϣ����Ϣ�Ĵ���״̬�ͽ����ӳ��\a p_msg->status��
 * ����\a p_msg->pfunc_complete ��\a p_msg->p_arg �ɿ�����ʹ�á�
 *
 * \param[in]       busid   Ҫ������I2C���߱��
 * \param[in,out]   p_msg   Ҫ�������Ϣ
 *
 * �����ķ���ֵ���£�
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a p_dev->flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIMEDOUT   ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a p_dev->flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * �ɹ�����Ĵ��������ӳ��\a p_msg->done_num
 *
 * ��Ϣ�Ĵ���״̬�ͽ����ӳ��\a p_msg->status ��
 *
 *      \li  -AW_ENOTCONN       ��Ϣ��δ�Ŷ�
 *      \li  -AW_EISCONN        ��Ϣ�����ŶӴ���
 *      \li  -AW_EINPROGRESS    ��Ϣ���ڱ�����
 * ����Ϊ��Ϣ������ϵĽ�� (��ͨ��\a p_msg->done_num ��λ����Ĵ���)
 *      \li   AW_OK              ���д���ɹ�����
 *      \li  -AW_ENOTSUP        ĳ�δ���Ŀ��Ʊ�־��֧��
 *      \li  -AW_ETIMEDOUT      ��ʱ����Ϣδ���趨��ʱ���ڴ���
 *      \li  -AW_ENODEV         ĳ�δ�����Ӧ��
 *      \li  -AW_EIO            ������Ϣʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 * \par ʾ��
 * ��ο� aw_i2c_async()
 */
aw_err_t aw_i2c_async(uint8_t           busid,
                      aw_i2c_message_t *p_msg);


/**
 * \brief ִ������д����
 *
 * \param[in]  p_dev       I2C�ӻ��豸������
 * \param[in]  p_buf0      ���ݻ�����0��Ҫд�����ݴ���ڴ�
 * \param[in]  n_tx0       ���ݻ�����0�����ݸ���
 * \param[in]  p_buf1      ���ݻ�����1��Ҫд�����ݴ���ڴ�
 * \param[in]  n_tx1       ���ݻ�����1�����ݸ���
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a p_dev->flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIMEDOUT   ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a p_dev->flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * ע��:����д�м���û��ֹͣ�źŵġ�
 */
aw_err_t aw_i2c_write_then_write (aw_i2c_device_t  *p_dev,
                                  const void       *p_buf0,
                                  size_t            n_tx0,
                                  const void       *p_buf1,
                                  size_t            n_tx1);

/**
 * \brief ��д�ٶ�
 *
 * \param[in]  p_dev         I2C�ӻ��豸������
 * \param[in]  p_txbuf       ָ���ӷ������ݻ�����
 * \param[in]  n_tx          �������ݻ���������
 * \param[out] p_rxbuf       �������ݻ�������Ҫ�������ݴ���ڴ�
 * \param[in]  n_rx          �������ݻ���������
 * \param[in]  is_send_stop  AW_TRUE  ��д���ַ����STOP����,�ٷ���START,
 *                           AW_FALSE д���ַ�󷢴�RESTART����
 *
 * \retval  AW_OK            �ɹ�
 * \retval -AW_ENXIO         δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL        ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP       \a p_dev->flags ��ĳЩ���Բ�֧��
 *                           (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIMEDOUT     ��ʱ
 * \retval -AW_ENODEV        �ӻ���Ӧ��(��\a p_dev->flags ������
 *                           #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO           ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 */
aw_err_t aw_i2c_write_then_read (aw_i2c_device_t  *p_dev,
                                 const void       *p_txbuf,
                                 size_t            n_tx,
                                 void             *p_rxbuf,
                                 size_t            n_rx,
                                 uint8_t           is_send_stop);


/**
 * \brief i2c�������ò���
 *
 * \param[in]  busid       Ҫ������I2C���߱��
 * \param[in]  p_cfg       ָ������������
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENOTSUP     ���ж������ļ����ô˽ӿ�
 * \retval -AW_ENXIO       �Ƿ���busid
 * \retval -AW_EINVAL      p_cfgΪ�ջ���p_cfgָ�����������Ч��
 */
aw_err_t aw_i2c_set_cfg (uint8_t busid, struct aw_i2c_config *p_cfg );


/**
 * \brief ��ȡi2c�����ò���
 *
 * \param[in]  busid       Ҫ������I2C���߱��
 * \param[out]  p_cfg      ָ���ȡ��������������
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENOTSUP     ���ж������ļ����ô˽ӿ�
 * \retval -AW_ENXIO       �Ƿ���busid
 * \retval -AW_EINVAL      p_cfgΪ��
 */
aw_err_t aw_i2c_get_cfg (uint8_t busid, struct aw_i2c_config *p_cfg);


/**
 * \brief i2cԭʼд(д��������ַ��, �Ϳ�ʼ����p_buf�е�����)
 *
 * \param[in]  p_dev         I2C�ӻ��豸������
 * \param[in]  p_buf         ָ�������ݻ�����
 * \param[in]  nbytes        �������ݻ���������
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a p_dev->flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIMEDOUT   ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a p_dev->flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 */
aw_err_t aw_i2c_raw_write (aw_i2c_device_t  *p_dev,
                           const void       *p_buf,
                           size_t            nbytes);


/**
 * \brief i2cԭʼ��(д��������ַ��, �ͽ������ϵ����ݶ���p_buf��)
 *
 * \param[in]  p_dev         I2C�ӻ��豸������
 * \param[out] p_buf         ָ��������ݻ�����
 * \param[in]  nbytes        �������ݻ���������
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a p_dev->flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIMEDOUT   ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a p_dev->flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 */
aw_err_t aw_i2c_raw_read ( aw_i2c_device_t   *p_dev,
                           void              *p_buf,
                           size_t             nbytes);
/** @} grp_aw_if_i2c */

#ifdef __cplusplus
}
#endif

#endif /* __AW_I2C_H */

/* end of file */
