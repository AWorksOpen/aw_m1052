/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/


#ifndef __AWBL_USBD_MS_H
#define __AWBL_USBD_MS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_usbd_ms
 * \copydoc aw_usbd_ms.h
 * @{
 */
#include "aw_usb_os.h"
#include "device/aw_usbd.h"
#include "fs/aw_blk_dev.h"


/** \brief �������洢���������ȼ��Ͷ�ջ��С */
#define AW_USBD_MS_TASK_PRIO    8
#define AW_USBD_MS_STACK_SIZE   2048



/** \brief �豸�߼���Ԫ */
struct aw_usbd_ms_lun {
    struct aw_blk_dev      *p_bd;      /**< \brief ���豸ָ�� */
    int                     num;       /**< \brief �߼���Ԫ�� */
    struct aw_list_head     node;      /**< \brief �߼���Ԫ����ڵ� */
    uint32_t                sd;        /**< \brief SCSI sense key */
    uint32_t                sd_inf;    /**< \brief SCSI sense��Ϣ */
    aw_bool_t               inf_valid; /**< \brief sense info�Ƿ���Ч */
    aw_bool_t               ro;        /**< \brief �Ƿ�ֻ�� */
    aw_bool_t               rm;        /**< \brief �Ƿ���Ƴ� */
    aw_bool_t               pv;        /**< \brief prevent��־ */
    uint_t                  bsize;     /**< \brief ���С */
    uint_t                  nblks;     /**< \brief ������ */
//    aw_usb_mutex_handle_t   lock;    //TODO �����汾��Ҫ������߳�ͨ������
    aw_usb_sem_handle_t     blk_sync;
    aw_usb_sem_handle_t     usb_sync;
};




/** \brief �������洢���豸 */
struct aw_usbd_ms {
    struct aw_usbd_fun      ufun;       /**< \brief USB���ܽӿ�*/
    struct aw_usbd_pipe     in;         /**< \brief IN���ݹܵ� */
    struct aw_usbd_pipe     out;        /**< \brief OUT���ݹܵ� */

    aw_usb_task_handle_t    ms_task;    /**< \brief �������洢������ */
    aw_usb_sem_handle_t     semb;
    aw_usb_mutex_handle_t   mutex;

    uint8_t                 nluns;      /**< \brief �߼���Ԫ���� */
    struct aw_list_head     luns;       /**< \brief �߼���Ԫ���� */
    void                   *p_ubuf;     /**< \brief USBD������ */
    void                   *p_bbuf;     /**< \brief LUN������ */
    uint32_t                buf_size;   /**< \brief ��������С */
    aw_bool_t               buf_auto;   /**< \brief �Ƿ�̬�����ڴ� */

    uint32_t                tag;        /**< \brief CBW/CSW��ǩ */
    uint32_t                dlen;       /**< \brief CBW���ݳ��� */
    uint32_t                cdlen;      /**< \brief CBW��Ч�ֽڳ��� */
    uint8_t                 cb[16];     /**< \brief CBW���� */
    uint8_t                 dir;        /**< \brief CBW��־ */
    uint8_t                 clen;       /**< \brief CBW����� */
    uint8_t                 sta;        /**< \brief CSW״̬ */
    struct aw_usbd_ms_lun  *p_lun;      /**< \brief ��ǰʹ�õ��߼���Ԫ */

};




/**
 * \brief ����һ���������洢�豸
 *
 * \param[in] p_obj     USBD����ָ��
 * \param[in] p_ms      �������洢������ָ��
 * \param[in] name      �������洢������
 * \param[in] p_buf     �洢�������׵�ַ��ΪNULL���ڲ���̬���룩
 * \param[in] buf_size  �洢�������С
 *
 * \retval  AW_OK       �����ɹ�
 * \retval  -AW_EINVAL  ��Ч����
 * \retval  -AW_ENOSPC  ����ռ��С
 * \retval  -AW_ENOMEM  �洢�ռ䲻��
 */
int aw_usbd_ms_create (struct aw_usbd      *p_obj,
                       struct aw_usbd_ms   *p_ms,
                       const char          *name,
                       void                *p_buf,
                       int                  buf_size);




/**
 * \brief ����һ���������洢�豸
 *
 * \param[in] p_ms      �������洢������ָ��
 *
 * \retval  AW_OK       ���ٳɹ�
 * \retval  -AW_EINVAL  ��Ч����
 * \retval  -AW_EBUSY   �豸��æ
 * \retval  -AW_ENOENT  �豸���ܲ�����
 */
int aw_usbd_ms_destroy (struct aw_usbd_ms  *p_ms);




/**
 * \brief ���һ���߼���Ԫ���������洢�豸
 *
 * \param[in] p_ms      �������洢������ָ��
 * \param[in] name      �߼���Ԫ����
 *
 * \retval  AW_OK       ��ӳɹ�
 * \retval  -AW_EINVAL  ��Ч����
 * \retval  -AW_ENODEV  �豸������
 * \retval  -AW_ENOMEM  �洢�ռ䲻��
 */
int aw_usbd_ms_lun_add (struct aw_usbd_ms  *p_ms,
                        const char         *name);


/** @} grp_aw_if_usbd_ms */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __AWBL_USBD_MS_H */
