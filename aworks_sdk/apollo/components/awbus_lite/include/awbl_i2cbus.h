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
 * \brief AWBus I2C���߽ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_i2cbus.h"
 * \endcode
 * ��ģ��Ϊ I2C ���ߵľ����ʵ��
 *
 * �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.01 18-11-02  sls, update i2c arch
 * - 1.00 12-10-10  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_I2CBUS_H
#define __AWBL_I2CBUS_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aw_list.h"
#include "aw_i2c.h"         
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "aw_task.h"

/** \brief I2C �첽����ʹ�õ�ջ  */
#define AWBL_I2C_ASYNC_TASK_STACK_SIZE (4096)

/** \brief �������ȼ�   */
#define AWBL_I2C_ASYNC_TASK_PRIO       AW_TASK_SYS_PRIORITY(6)

/** \brief ���õȴ� */
#define AWBL_I2C_WAITFOREVER           (AW_SEM_WAIT_FOREVER)

/** \brief ����֧�ֵĹ��� */
#define AWBL_FEATURE_I2C_7BIT          0x0001u  /**< \brief 7-bit�豸��ַ֧�� */
#define AWBL_FEATURE_I2C_10BIT         0x0002u  /**< \brief 10-bit�豸��ַ֧�� */
#define AWBL_FEATURE_I2C_REV_DIR_ADDR  0x0004u  /**< \brief ��д��־λ��ת֧�� */

/** \brief ����������Ӧ����� (�����ӻ���ַ��Ӧ���������Ӧ��)֧�� */
#define AWBL_FEATURE_I2C_IGNORE_NAK    0x0008u

/** \brief �ڶ������н�������ʱ������Ӧ��֧�� */
#define AWBL_FEATURE_I2C_NO_RD_ACK     0x0010u


/** \biref �õ�I2C�ӻ��豸�ĸ������� */
#define AWBL_I2C_PARENT_GET(p_dev) \
    ((struct awbl_i2c_master *) \
        (((struct awbl_dev *)(p_dev))->p_parentbus->p_ctlr))

/** \biref �õ�I2C�ӻ��豸�ĸ���������� */
#define AWBL_I2C_PARENT_BUSID_GET(p_dev) \
        (((struct awbl_i2c_master_devinfo *) \
            AWBL_DEVINFO_GET( \
                ((struct awbl_dev *)(p_dev))->p_parentbus->p_ctlr))->bus_index)

/** \brief ����д�Ƿ��ֹͣ�ź�  */
#define AWBL_I2C_WRITE_NO_STOP      (0)
#define AWBL_I2C_WRITE_STOP         (1)

/** \brief ���ƶ��Ƿ���ظ���ʼ�ź�  */
#define AWBL_I2C_READ_START         (0)
#define AWBL_I2C_READ_RESTART       (1)

typedef struct awbl_trans_buf {
    uint8_t *p_buf;
    uint8_t  buf_size;
} awbl_trans_buf_t;


struct awbl_i2c_master;

/** \biref AWBus I2C ���߿����� (I2C ����) �豸��Ϣ (ƽ̨���ò���) */
struct awbl_i2c_master_devinfo {

    /** \brief ����������Ӧ�����߱�� */
    uint8_t     bus_index;

    /** \brief �����������ٶ� */
    uint32_t    speed;

    /** \brief ��ʱ, ��λ ms�����õȴ��� #AWBL_I2C_WAITFOREVER */
    int         timeout;
};


/** \biref AWBus I2C ���߿����� (I2C ����) �豸��Ϣ (�������ò���) */
struct awbl_i2c_master_devinfo2 {

    /** \brief ������������(֧�ֵġ�I2Cģʽ,�� awbl_i2c.h)  */
    uint16_t    features;

    /**
     * \brief i2cд����
     *
     * \param[in]  p_master         i2c������ָ��
     * \param[in]  chip_addr        i2c���������豸��ַ
     * \param[in]  p_trans_buf      д���ݻ������׵�ַ
     * \param[in]  trans_buf_num    д���ݻ���������
     * \param[in]  stop_ctl         ֹͣ�ź�����(AWBL_I2C_WRITE_STOP����ֹͣ�ź�,
     *                                       AWBL_I2C_WRITE_NO_STOP����ֹͣ�ź�)
     * \param[in]  dr_flag          ���ݸ������㴦���һЩ��־����
     *                              (��: AW_I2C_M_IGNORE_NAK,
     *                                   AW_I2C_M_NO_RD_ACK,
     *                                   AW_I2C_M_REV_DIR_ADDR,
     *                                   AW_I2C_M_10BIT��)
     */
    aw_err_t (*pfunc_i2c_write) (struct awbl_i2c_master  *p_master,
                                 uint16_t                 chip_addr,
                                 struct awbl_trans_buf   *p_trans_buf,
                                 uint32_t                 trans_buf_num,
                                 uint8_t                  stop_ctl,
                                 uint16_t                 dr_flag);

    /**
     * \brief i2c������
     *
     * \param[in]       p_master  i2c������ָ��
     * \param[in]       chip_addr i2c���������豸��ַ
     * \param[in]       p_trans_buf ��¼����Ļ������ͻ������Ĵ�С
     * \param[in]       start_ctl ��ʼ�źſ���, ����start�ź�,����restart�ź�(
     *                            AWBL_I2C_READ_START������ʼ�ź�,
     *                            AWBL_I2C_READ_RESTART �����ظ���ʼ�ź�)
     * \param[in]       dr_flag   ���ݸ������㴦���һЩ��־����
     *                                    (��: AW_I2C_M_IGNORE_NAK,
     *                                         AW_I2C_M_NO_RD_ACK,
     *                                         AW_I2C_M_REV_DIR_ADDR,
     *                                         AW_I2C_M_10BIT��)
     */
    aw_err_t  (*pfunc_i2c_read) (struct awbl_i2c_master *p_master,
                                 uint16_t                chip_addr,
                                 struct awbl_trans_buf  *p_trans_buf,
                                 uint8_t                 start_ctl,
                                 uint16_t                dr_flag);

    /**
     * \brief i2c����
     *
     * \param[in]       p_master  i2c������ָ��
     * \param[in]       p_cfg     ָ�����ýṹ
     */
    aw_err_t  (*pfunc_i2c_set_cfg) (struct awbl_i2c_master *p_master,
                                    struct aw_i2c_config   *p_cfg);


    /**
     * \brief ��ȡi2c����
     *
     * \param[in]       p_master  i2c������ָ��
     * \param[out]      p_cfg     ȡ����ǰ����
     */
    aw_err_t (*pfunc_i2c_get_cfg)(struct awbl_i2c_master *p_master,
                                  struct aw_i2c_config   *p_cfg);

};


/** \biref I2C bus �ӻ��豸����ע����Ϣ�ṹ�� */
typedef struct awbl_i2c_drvinfo {
    struct awbl_drvinfo super;  /**< \brief �̳��� AWBus �豸������Ϣ */
} awbl_i2c_drvinfo_t;

/** \brief AWBus I2C �����豸 (I2C�ӻ�) ʵ�� */
struct awbl_i2c_device {
    struct awbl_dev super;      /**< \brief �̳��� AWBus �豸 */
};

/** \brief AWBus I2C ���߿����� (I2C����) ʵ�� */
struct awbl_i2c_master {
    /** \brief �̳��� AWBus ���߿����� */
    struct awbl_busctlr super;

    /** \brief ָ����һ�� I2C ������ */
    struct awbl_i2c_master *p_next;

    /** \brief I2C �����������Ϣ (�������ò���) */
    const struct awbl_i2c_master_devinfo2 *p_devinfo2;

    /** \brief �豸������ */
    AW_MUTEX_DECL(dev_mux);
};


/**
 * \brief AWBus i2c ����ģ���ʼ������
 *
 * \attention ������Ӧ���� awbus_lite_init() ֮��awbl_dev_init1() ֮ǰ����
 */
void awbl_i2cbus_init(void);

/**
 * \brief ���� AWBus I2C ����ʵ���Լ�ö�������ϵ��豸
 *
 * ���ӿ��ṩ�� AWBus I2C ���߿�����(�����豸)����ʹ��
 *
 */
aw_err_t awbl_i2cbus_create(struct awbl_i2c_master *p_master);

/**
 * \brief ������Ϣ���첽ģʽ
 *
 * ���ӿ��ṩ�� AWBus I2C ���豸����ʹ��
 *
 * ���첽�ķ�ʽ������Ϣ����Ϣ�Ĵ���״̬�ͽ����ӳ�� p_msg->status��
 * ��Ϣ�������(�ɹ�����ʱ�����)ʱ��������� p_msg->pfunc_complete ������
 * ���� p_msg->p_arg��
 *
 * \param[in]       p_dev   AWBus I2C ���豸ʵ��
 * \param[in,out]   p_msg   Ҫ�������Ϣ
 *
 * �����ķ���ֵ���£�
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a p_dev->flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * �ɹ�����Ĵ��������ӳ�� p_msg->done_num
 *
 * ��Ϣ�Ĵ���״̬�ͽ����ӳ�� p_msg->status ��
 *
 *      \li  -ENOTCONN       ��Ϣ��δ�Ŷ�
 *      \li  -EISCONN        ��Ϣ�����ŶӴ���
 *      \li  -EINPROGRESS    ��Ϣ���ڱ�����
 * ����Ϊ��Ϣ������ϵĽ�� (��ͨ�� p_msg->done_num ��λ����Ĵ���)
 *      \li   AW_OK          ���д���ɹ�����
 *      \li  -ENOTSUP        ĳ�δ���Ŀ��Ʊ�־��֧��
 *      \li  -ETIME          ��ʱ����Ϣδ���趨��ʱ���ڴ����ڿ�������devinfo��
 *                           ���ó�ʱʱ��
 *      \li  -ENODEV         ĳ�δ�����Ӧ��
 *      \li  -EIO            ������Ϣʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 */
aw_err_t awbl_i2c_async(struct awbl_i2c_device *p_dev,
                        struct aw_i2c_message  *p_msg);

/**
 * \brief ������Ϣ��ͬ��ģʽ
 *
 * ���ӿ��ṩ�� AWBus I2C ���豸����ʹ��
 *
 * ��ͬ���ķ�ʽ������Ϣ����Ϣ�Ĵ���״̬�ͽ����ӳ�� p_msg->status��
 * ���� p_msg->pfunc_complete �� p_msg->p_arg �ɿ�����ʹ�á�
 *
 * \param[in]       p_dev   AWBus I2C ���豸ʵ��
 * \param[in,out]   p_msg   Ҫ�������Ϣ
 *
 * �����ķ���ֵ���£�
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a p_dev->flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * �ɹ�����Ĵ��������ӳ�� p_msg->done_num
 *
 * ��Ϣ�Ĵ���״̬�ͽ����ӳ�� p_msg->status ��
 *
 *      \li  -ENOTCONN       ��Ϣ��δ�Ŷ�
 *      \li  -EISCONN        ��Ϣ�����ŶӴ���
 *      \li  -EINPROGRESS    ��Ϣ���ڱ�����
 * ����Ϊ��Ϣ������ϵĽ�� (��ͨ�� p_msg->done_num ��λ����Ĵ���)
 *      \li   AW_OK           ���д���ɹ�����
 *      \li  -ENOTSUP        ĳ�δ���Ŀ��Ʊ�־��֧��
 *      \li  -ETIME          ��ʱ����Ϣδ���趨��ʱ���ڴ���
 *      \li  -ENODEV         ĳ�δ�����Ӧ��
 *      \li  -EIO            ������Ϣʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 */
aw_err_t awbl_i2c_sync(struct awbl_i2c_device *p_dev,
                       struct aw_i2c_message  *p_msg);

/**
 * \brief ��I2C�ӻ�����
 *
 * ���ӿ��ṩ�� AWBus I2C ���豸����ʹ��
 *
 * \param[in]  p_dev    AWBus I2C�ӻ��豸ʵ���� �� \ref struct awbl_i2c_device
 * \param[in]  flags    �ӻ��豸���ԣ�����I2C���豸���Ա�־ (aw_i2c.h)��
 * \param[in]  addr     �ӻ��豸��ַ��7λ��10λ
 * \param[in]  subaddr  �������ӵ�ַ���Ӵ˵�ַ��ʼ��ȡ���ݣ��� flags �����ӵ�ַ
 *                      ���Ϊ0 (AW_I2C_SUBADDR_NONE), ��˲�����������
 * \param[out] p_buf    ���ݻ���������ȡ�����ݴ���ڴ�
 * \param[in]  nbytes   Ҫд������ݸ���
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a p_dev->flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * ע��:д�ӵ�ַ�Ͷ�֮�����õ��ظ���ʼ�źţ�û��ֹͣ�źš�
 *
 * \par ����
 * \code
 *  uint8_t read_buf[16];
 *
 *  awbl_i2c_read((struct awbl_i2c_device *)p_dev,  // AWBus ���豸ʵ��
 *                 AW_I2C_ADDR_7BIT |
 *                 AW_I2C_SUBADDR_1BYTE,  // 7-bit ������ַ 1-byte �������ӵ�ַ
 *                 0x50,                  // ������ַ��0x50
 *                 0,                     // �������ӵ�ַ��0x00
 *                 p_buf,                 // ��������
 *                 16);                   // ��ȡ���ݳ��ȣ�16�ֽ�
 * \endcode
 */
aw_err_t awbl_i2c_read(struct awbl_i2c_device *p_dev,
                       uint16_t                flags,
                       uint16_t                addr,
                       uint32_t                subaddr,
                       void                   *p_buf,
                       size_t                  nbytes);

/**
 * \brief д���ݵ�I2C�ӻ�
 *
 * ���ӿ��ṩ�� AWBus I2C ���豸����ʹ��
 *
 * \param[in]  p_dev    AWBus I2C�ӻ��豸ʵ���� �� \ref struct awbl_i2c_device
 * \param[in]  flags    �ӻ��豸���ԣ�����I2C���豸���Ա�־ (aw_i2c.h)��
 * \param[in]  addr     �ӻ��豸��ַ��7λ��10λ
 * \param[in]  subaddr  �������ӵ�ַ���Ӵ˵�ַ��ʼ��ȡ���ݣ��� flags �����ӵ�ַ
 *                      ���Ϊ0 (AW_I2C_SUBADDR_NONE), ��˲�����������
 * \param[out] p_buf    ���ݻ�������Ҫд������ݴ���ڴ�
 * \param[in]  nbytes   Ҫд������ݸ���
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a p_dev->flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * ע��: д�ӵ�ַ��д�����м���û��ֹͣ�źŵ�
 *
 * \par ����
 * \code
 *  uint8_t write_buf[16];
 *
 *  awbl_i2c_write((struct awbl_i2c_device *)p_dev,  // AWBus ���豸ʵ��
 *                  AW_I2C_ADDR_7BIT |
 *                  AW_I2C_SUBADDR_1BYTE,  // 7-bit ������ַ 1-byte �������ӵ�ַ
 *                  0x50,                  // ������ַ��0x50
 *                  0,                     // �������ӵ�ַ��0x00
 *                  p_buf,                 // ��������
 *                  16);                   // ��ȡ���ݳ��ȣ�16�ֽ�
 * \endcode
 */
aw_err_t awbl_i2c_write(struct awbl_i2c_device *p_dev,
                        uint16_t                flags,
                        uint16_t                addr,
                        uint32_t                subaddr,
                        const void             *p_buf,
                        size_t                  nbytes);

/**
 * \brief ԭʼд��I2C�ӻ�(�����������ڵ��ӵ�ַ)
 *
 * \param[in]  p_dev    AWBus I2C�ӻ��豸ʵ���� �� \ref struct awbl_i2c_device
 * \param[in]  addr     �ӻ��豸��ַ��7λ��10λ
 * \param[in]  flags    �ӻ��豸���ԣ�����I2C���豸���Ա�־ (aw_i2c.h)��
 * \param[out] p_buf    ���ݻ�������Ҫд������ݴ���ڴ�
 * \param[in]  nbytes   Ҫд������ݸ���
 *
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a p_dev->flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * \par ����
 * \code
 *  uint8_t write_buf[16];
 *
 *  awbl_i2c_raw_write((struct awbl_i2c_device *)p_dev,  // AWBus ���豸ʵ��
 *                      0x50,                 // ������ַ��0x50
 *                      AW_I2C_ADDR_7BIT      // 7λ�ӵ�ַ
 *                      p_buf,                // ��������
 *                      16);                  // ��ȡ���ݳ��ȣ�16�ֽ�
 * \endcode
 */
aw_err_t awbl_i2c_raw_write (struct awbl_i2c_device  *p_dev,
                             uint16_t                 addr,
                             uint16_t                 flags,
                             const void              *p_buf,
                             size_t                   nbytes);


/**
 * \brief ԭʼ��I2C�ӻ�(�����������ڵ��ӵ�ַ)
 *
 * \param[in]  p_dev    AWBus I2C�ӻ��豸ʵ���� �� \ref struct awbl_i2c_device
 * \param[in]  addr     �ӻ��豸��ַ��7λ��10λ
 * \param[in]  flags    �ӻ��豸���ԣ�����I2C���豸���Ա�־ (aw_i2c.h)��
 * \param[out] p_buf    ���ݻ�������Ҫ���������ݴ���ڴ�
 * \param[in]  nbytes   Ҫ�������ݸ���
 *
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a p_dev->flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 */
aw_err_t awbl_i2c_raw_read (struct awbl_i2c_device   *p_dev,
                            uint16_t                  addr,
                            uint16_t                  flags,
                            void                     *p_buf,
                            size_t                    nbytes);


/**
 * \brief I2Cд���
 *
 * \param[in]  p_dev         AWBus I2C�ӻ��豸ʵ���� �� \ref struct awbl_i2c_device
 * \param[in]  addr          �ӻ��豸��ַ��7λ��10λ
 * \param[in]  flags         �ӻ��豸���ԣ�����I2C���豸���Ա�־ (aw_i2c.h)��
 * \param[in]  p_txbuf       �������ݻ�����
 * \param[in]  n_tx          �������ݻ���������
 * \param[in]  p_rxbuf       �������ݻ�����
 * \param[in]  n_rx          �������ݻ���������
 * \param[in]  is_send_stop  AW_TRUE  ��д���ַ����STOP����,�ٷ���START,
 *                           AW_FALSE д���ַ�󷢴�RESTART����
 *
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a p_dev->flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 */
aw_err_t awbl_i2c_write_then_read (struct awbl_i2c_device  *p_dev,
                                   uint16_t                 addr,
                                   uint16_t                 flags,
                                   const void              *p_txbuf,
                                   size_t                   n_tx,
                                   void                    *p_rxbuf,
                                   size_t                   n_rx,
                                   uint8_t                  is_send_stop);


/**
 * \brief I2Cд��д
 *
 * \param[in]  p_dev     AWBus I2C�ӻ��豸ʵ���� �� \ref struct awbl_i2c_device
 * \param[in]  addr      �ӻ��豸��ַ��7λ��10λ
 * \param[in]  flags     �ӻ��豸���ԣ�����I2C���豸���Ա�־ (aw_i2c.h)��
 * \param[in]  p_buf0    ���ݻ�����0��Ҫд�����ݴ���ڴ�
 * \param[in]  n_tx0     ���ݻ�����0�ĳ���
 * \param[out] p_buf1    ���ݻ�����1��Ҫд�����ݴ���ڴ�
 * \param[in]  n_tx1     ���ݻ�����1�ĳ���
 *
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_ENXIO       δ�ҵ�\a p_dev->masterid ��ָ��������
 * \retval -AW_EINVAL      ��������nbytes Ϊ 0,p_bufΪ�յ�
 * \retval -AW_ENOTSUP     \a flags ��ĳЩ���Բ�֧��
 *                         (����, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       ��ʱ
 * \retval -AW_ENODEV      �ӻ���Ӧ��(��\a p_dev->flags ������
 *                         #AW_I2C_IGNORE_NAK, �򲻻�����˴���)
 * \retval -AW_EIO         ��д����ʱ�������ߴ��������ٲ�ʧ�ܹ���ȴ���
 *
 * \note  ע��:����д�м���û��ֹͣ�źŵ�
 */
aw_err_t awbl_i2c_write_then_write (struct awbl_i2c_device   *p_dev,
                                    uint16_t                  addr,
                                    uint16_t                  flags,
                                    const void               *p_buf0,
                                    size_t                    n_tx0,
                                    const void               *p_buf1,
                                    size_t                    n_tx1);

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AWBL_I2CBUS_H */

/* end of file */
