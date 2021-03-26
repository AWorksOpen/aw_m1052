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
 * \brief EimBus head file
 *
 * define EimBus data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-17  deo, first implementation
 * \endinternal
 */
#ifndef __AWBL_EIM_BUS_H
#define __AWBL_EIM_BUS_H


#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus */

#include "aw_list.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "string.h"

/** \biref �õ�EIM�ӻ��豸�ĸ������� */
#define AWBL_EIM_PARENT_GET(p_dev) \
    ((struct awbl_eim_master *) \
        (((struct awbl_dev *)(p_dev))->p_parentbus->p_ctlr))

struct awbl_eim_master;
struct awbl_eimbus_opt;

/** \biref AWBus EIM ���߿����� (EIM ����) �豸��Ϣ (ƽ̨���ò���) */
struct awbl_eim_master_devinfo {

    /** \brief ����������Ӧ�����߱�� */
    uint8_t     bus_index;
};

/** \biref AWBus EIM ���߿����� (EIM ����) �豸��Ϣ (�������ò���) */
struct awbl_eim_master_devinfo2 {
    /** \brief �������� */
    void (*pfn_eim_config) (struct awbl_eim_master *p_dev,
                            uint8_t                 cs_select,
                            void                   *p_info);

    /** \brief ����ʱ�� */
    void (*pfn_eim_timing)  (struct awbl_eim_master *p_master,
                             uint8_t                 cs_select,
                             void                   *p_info);

    /** \brief д���� */
    void  (*pfn_eim_write)  (struct awbl_eim_master   *p_master,
                             uint8_t                   cs_select,
                             uint32_t                  addr,
                             uint16_t                  data);

     /** \brief ������ */
    uint16_t  (*pfn_eim_read)  (struct awbl_eim_master   *p_master,
                                uint8_t                   cs_select,
                                uint32_t                  addr);
};

/** \brief AWBus EIM ���߿����� (EIM����) ʵ�� */
struct awbl_eim_master {

    /** \brief ���߿����� */
    struct awbl_busctlr          super;     /**< \brief �̳��� AWBus ���߿����� */

    /** \brief EIM �����������Ϣ (�������ò���) */
    const struct awbl_eim_master_devinfo2 *p_devinfo2;

    /** \brief �豸������ */
    AW_MUTEX_DECL(               dev_mux);

    /** \brief ��ǰ�������Ƿ����� */
    aw_bool_t                    lock;

    void                        *priv;
};

/* \biref EIM bus �ӻ��豸����ע����Ϣ�ṹ�� */
typedef struct awbl_eim_drvinfo {
    struct awbl_drvinfo super;      /**< \brief �̳��� AWBus �豸������Ϣ */
} awbl_eim_drvinfo_t;

/** \brief AWBus EIM �����豸 (EIM�ӻ�) ʵ�� */
struct awbl_eim_device {
    struct awbl_dev super;          /**< \brief �̳��� AWBus �豸 */
};

struct awbl_eimbus_opt {

    /** \brief �������� */
    uint8_t      type;

    /** \brief ָ������������� */
    const void  *ptr;
};
/*******************************************************************************
  Eim������ͨ�ýӿ�
*******************************************************************************/

/**
 * \brief EimBus��ʼ��
 */
void awbl_eim_bus_init(void);


/**
 * \brief ����������
 * 
 * param[in] p_master : ���߿�����
 *
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_eimbus_create (struct awbl_eim_master *p_master);


/**
 * \brief ���߿���������
 *
 * \param[in] p_dev : eim�豸
 *
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_eimbus_lock (struct awbl_eim_device *p_dev);


/**
 * \brief ���߿�����ȥ��
 *
 * \param[in] p_dev : eim�豸
 *
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_eimbus_unlock (struct awbl_eim_device *p_dev);

/**
 * \brief ��������
 * \param[in] p_dev     :  eim�豸
 * \param[in] cs_select :  CS ѡ��
 * \param[in] p_info    :  ������Ϣ
 *
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_eimbus_config (struct awbl_eim_device *p_dev,
                             uint8_t                 cs_select,
                             void                   *p_info);

/**
 * \brief ����ʱ��
 * \param[in] p_dev     :  eim�豸
 * \param[in] cs_select :  CS ѡ��
 * \param[in] p_info    :  ʱ����Ϣ
 *
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_eimbus_timing (struct awbl_eim_device *p_dev,
                             uint8_t                 cs_select,
                             void                   *p_info);

/**
 * \brief д����
 *
 * \param[in] p_dev     :  eim�豸
 * \param[in] cs_select :  CS ѡ��
 * \param[in] addr      :  ��Ե�ַ
 * \param[in] data      :  д��Ϣ
 *
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
aw_err_t awbl_eim_write (struct awbl_eim_device *p_dev,
                         uint8_t                 cs_select,
                         uint32_t                addr,
                         uint16_t                data);

/**
 * \brief ������
 *
 * \param[in] p_dev     :  eim�豸
 * \param[in] cs_select :  CS ѡ��
 * \param[in] addr      :  ��Ե�ַ
 *
 * \return AW_OK��ʾ�����ɹ���������ʾʧ�ܡ�
 */
uint8_t awbl_eim_read (struct awbl_eim_device *p_dev,
                       uint8_t                 cs_select,
                       uint32_t                addr);

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif /* __AWBL_EIM_BUS_H */
