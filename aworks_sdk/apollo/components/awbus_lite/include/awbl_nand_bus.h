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
 * \brief NandBus head file
 *
 * define NandBus data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-16  deo, first implementation
 * - 2.00 17-3-23   vih, modify
 * \endinternal
 */
#ifndef __AWBL_NAND_BUS_H
#define __AWBL_NAND_BUS_H


#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aw_list.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "string.h"


/** \brief ����һ��awbl_nand_controller���͵�ָ����� */
#define AWBL_NANDBUS_DECL(p_dev, p_awdev) \
            struct awbl_nandbus *p_dev = \
                (struct awbl_nandbus *)AW_CONTAINER_OF( \
                    AW_CONTAINER_OF(p_awdev, struct awbl_busctlr, super), \
                    struct awbl_nandbus, \
                    awbus)

struct awbl_nandbus;

/** \brief ���������� */
typedef struct awbl_nandbus_opt {

    /** \brief �������� */
    uint8_t            type;
#define NAND_CMD       0x01   /**< \brief ���� */
#define NAND_ADDR      0x02   /**< \brief ��ַ */
#define NAND_RD_DATA   0x03   /**< \brief ��ȡ���� */
#define NAND_WR_DATA   0x04   /**< \brief д������ */

    /** \brief CE�Ƿ���ס */
    uint8_t            lock_ce;

    /** \brief ָ������������� */
    const void        *ptr;

    /** \brief �����������ݵĳ��� */
    uint32_t           len;

} awbl_nandbus_opt_t;





/** \brief ������������Ϣ */
struct awbl_nandbus_serv {

    /** \brief Ƭѡ */
    void (*pfn_select)  (struct awbl_nandbus        *p_bus,
                         void                       *p_info);

    /** \brief ���� */
    aw_err_t (*pfn_opt) (struct awbl_nandbus        *p_bus,
                         struct awbl_nandbus_opt    *p_opt);

    /** \brief NandоƬ������� */
    aw_bool_t (*pfn_ready) (struct awbl_nandbus     *p_bus,
                            void                    *priv);

    /** \brief ��������λ */
    void (*pfn_reset)   (struct awbl_nandbus        *p_bus);
};


struct awbl_nandbus {

    /** \brief ���߿����� */
    struct awbl_busctlr          awbus;

    /** \brief ����ID */
    uint8_t                      bus_index;

    struct awbl_nandbus_serv     serv;

    /** \brief �豸������ */
    aw_spinlock_isr_t            dev_lock;

    /** \brief �豸������ */
    AW_MUTEX_DECL(               dev_mux);

    /** \brief ��ǰ�������Ƿ����� */
    aw_bool_t                       lock;

    /** \brief ECCǿ��֧�� */
    uint8_t                      ecc_support;
#define AWBL_NAND_ECC_NONE       0x00   /**< \brief ��������֧��ECC���� */
#define AWBL_NAND_ECC_1BITS      0x01   /**< \brief ������֧��1λECC���� */
#define AWBL_NAND_ECC_4BITS      0x08   /**< \brief ������֧��4λECC���� */
#define AWBL_NAND_ECC_8BITS      0x80   /**< \brief ������֧��8λECC���� */

    void                        *priv;
};


/***************************Nand������ͨ�ýӿ�**********************************/

/**
 * \brief NandBus��ʼ��
 */
void awbl_nand_bus_init (void);


/**
 * \brief ����������
 * \param[in] p_controller  ���߿�����
 * \param[in] ecc_support   ECCǿ��֧��
 *
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_nandbus_create (struct awbl_nandbus *p_bus,
                              uint8_t              bus_index);


/**
 * \brief ���߿���������
 * \param[in] p_controller  ���߿�����
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_nandbus_lock (struct awbl_nandbus *p_bus);


/**
 * \brief ���߿�����ȥ��
 * \param[in] p_controller : ���߿�����
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_nandbus_unlock (struct awbl_nandbus *p_bus);


/**
 * \brief ���߿�������λ
 * \param[in] p_controller : ���߿�����
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_nandbus_reset (struct awbl_nandbus *p_bus);


/**
 * \brief Ƭѡ
 * \param[in] p_bus   bus
 * \param[in] p_chip  NandоƬ
 *
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_nandbus_select (struct awbl_nandbus *p_bus,
                              void                *p_arg);


/**
 * \brief ��NandоƬ���в���
 * \param[in] p_chip  NandоƬ
 * \param[in] p_opt   �����ṹ��
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_nandbus_opt (struct awbl_nandbus      *p_bus,
                           struct awbl_nandbus_opt  *p_opt);


/**
 * \brief ���NandоƬ�Ƿ�׼������
 * \param[in] p_chip  NandоƬ
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_bool_t awbl_nandbus_is_ready (struct awbl_nandbus *p_bus);


#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif /* __AWBL_NAND_BUS_H */

/* end of file */

