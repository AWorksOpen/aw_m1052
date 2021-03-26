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



#ifndef __AW_USBD_CANDTU_H
#define __AW_USBD_CANDTU_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/**
 * \addtogroup grp_aw_if_usbd_candtu
 * \copydoc aw_usbd_candtu.h
 * @{
 */
#include "aw_usb_os.h"
#include "device/aw_usbd.h"


/** \brief �������ȼ��Ͷ�ջ��С */
#define AW_USBD_TASK_PRIO                  8
#define AW_USBD_CANDTU_STACK_SIZE          4096
#define AW_USBD_CANDTU_USBCAN_STACK_SIZE   2048

/** \brief �����б������־ */
#define AW_USBD_CANDTU_CMD_END      {0, 0, 0, 0, NULL}


struct aw_usbd_candtu;


/** \brief candtu����� */
struct aw_usbd_candtu_cmd {
    uint8_t     start;      /**< \brief ��ʼ��־ */
    uint8_t     cmd;        /**< \brief ���� */
    uint8_t     info;       /**< \brief ���� */
    uint8_t     res;        /**< \brief ���� */
    uint32_t    len;        /**< \brief ���ݳ��� */
};


/** \brief candtuӦ���ʽ���� */
struct aw_usbd_candtu_ack {
    uint8_t     start;      /**< \brief ��ʼ��־ */
    uint8_t     ack;        /**< \brief ���� */
    uint8_t     status;     /**< \brief �����־�� 0 OK, 1 FAIL, 2 BUSY */
    uint8_t     res;        /**< \brief ���� */
    uint32_t    len;        /**< \brief ���ݳ��� */
};



/** \brief candtu������Ϣ���� */
struct aw_usbd_candtu_cmd_info {
    uint8_t         cmd;                /**< \brief ���� */
    uint8_t         info;               /**< \brief ��Ϣ */
    uint32_t        len;                /**< \brief ���� */
    uint8_t         flag;               /**< \brief ��־ */
#define AW_USBD_CMD_CHECH_CMD   0x01
#define AW_USBD_CMD_CHECH_INFO  0x02
#define AW_USBD_CMD_CHECH_LEN   0x04

    /** \brief ����ִ�к��� */
    int           (*pf_do) (struct aw_usbd_candtu     *p_cdtu,
                            struct aw_usbd_candtu_cmd *p_cmd,
                            void                      *p_buf,
                            int                        buf_len);
};



/** \brief candtu usbcan���ܽṹ���� */
struct aw_usbd_candtu_usbcan {
    struct aw_usbd_pipe  in;            /**< \brief IN���ݹܵ� */
    struct aw_usbd_pipe  out;           /**< \brief OUT���ݹܵ� */

    aw_usb_task_handle_t    usbcan_task;
    aw_usb_sem_handle_t     semb;

    aw_bool_t            buf_auto;      /**< \brief �Ƿ��ڴ涯̬���� */
    void                *p_rxbuf;       /**< \brief ���ݽ��ջ��� */
    void                *p_txbuf;       /**< \brief ���ݷ��ͻ��� */
    int                  rxsize;        /**< \brief ���ݽ��ջ�������С */
    int                  txsize;        /**< \brief ���ݷ��ͻ�������С */

    /** \brief ��Ϣ���ͺ���  */
    int                (*put_msg) (struct aw_usbd_candtu_usbcan *p_candtu,
                                   void                         *p_msg,
                                   int                           len,
                                   void                         *p_buf,
                                   int                           buf_size,
                                   uint32_t                      timeout);
};



/** \brief candtu�Զ�����ṹ���� */
struct aw_usbd_candtu {
    struct aw_usbd_fun                     ufun;        /**< \brief ���ܽӿ� */
    struct aw_usbd_pipe                    cin;         /**< \brief IN���ݹܵ� */
    struct aw_usbd_pipe                    cout;        /**< \brief OUT���ݹܵ� */

    void                                  *p_buf;       /**< \brief ���ݻ����� */
    uint32_t                               buf_size;    /**< \brief ��������С */
    aw_bool_t                              buf_auto;    /**< \brief �Ƿ�̬�����ڴ� */

    const struct aw_usbd_candtu_cmd_info  *p_cmd_tab;   /**< \brief �����б�ָ�� */

    aw_usb_task_handle_t                   cdtu_task;
    aw_usb_sem_handle_t                    semb;
    aw_usb_mutex_handle_t                  mutex;

    struct aw_usbd_candtu_usbcan          *p_usbcan;    /**< \brief usbcan���� */
};




/**
 * \brief ����candtu�Զ����豸
 *
 * \param[in] p_cdtu    candtu�豸ָ��
 * \param[in] p_ctab    �����б�
 * \param[in] p_buf     �������׵�ַ��ΪNULL���Զ���̬���䣩
 * \param[in] size      ��������С
 * \param[in] evt_cb    �¼��ص�
 *
 * \retval  AW_OK       �����ɹ�
 * \retval  -AW_EINVAL  ��Ч����
 * \retval  -AW_ENOMEM  �洢�ռ䲻��
 */
int aw_usbd_candtu_create (struct aw_usbd_candtu                 *p_cdtu,
                           const struct aw_usbd_candtu_cmd_info  *p_ctab,
                           void                                  *p_buf,
                           uint32_t                               size,
                           void (*evt_cb) (struct aw_usbd_candtu*, int));



/**
 * \brief ����candtu usbcan����
 *
 * \param[in] p_cdtu    candtu�豸ָ��
 * \param[in] p_usbcan  uabcan�豸ָ��
 * \param[in] p_buf     �������׵�ַ��ΪNULL���Զ���̬���䣩
 * \param[in] size      ��������С
 * \param[in] put_msg   ��Ϣ���ͺ���
 *
 * \retval  AW_OK       �����ɹ�
 * \retval  -AW_EINVAL  ��Ч����
 * \retval  -AW_EEXIST  �豸�Ѵ���
 * \retval  -AW_ENOMEM  �洢�ռ䲻��
 */
int aw_usbd_candtu_usbcan_create (struct aw_usbd_candtu         *p_cdtu,
                                  struct aw_usbd_candtu_usbcan  *p_usbcan,
                                  void                          *p_buf,
                                  uint32_t                       size,
                                  int (*put_msg) (struct aw_usbd_candtu_usbcan *p_candtu,
                                                  void                         *p_msg,
                                                  int                           len,
                                                  void                         *p_buf,
                                                  int                           buf_size,
                                                  uint32_t                      timeout));



/**
 * \brief ����candtu�豸
 *
 * \param[in] p_cdtu    candtu�豸ָ��
 *
 * \return  ��
 */
void aw_usbd_candtu_destroy (struct aw_usbd_candtu  *p_cdtu);



/**
 * \brief candtu�����ϴ�
 *
 * \param[in] p_cdtu    candtu�豸ָ��
 * \param[in] p_data    �ϴ������׵�ַ
 * \param[in] blk_size  ���ݿ��С
 * \param[in] nblks     ������
 * \param[in] timeout   ���ͳ�ʱ�����룩
 *
 * \retval  AW_OK       �ϴ��ɹ�
 * \retval  -AW_EINVAL  ��Ч����
 * \retval  -AW_ENOMEM  �洢�ռ䲻��
 */
int aw_usbd_candtu_usbcan_upload (struct aw_usbd_candtu  *p_cdtu,
                                  void                   *p_data,
                                  uint32_t                blk_size,
                                  uint32_t                nblks,
                                  int                     timeout);

/** @} grp_aw_if_usbd_candtu */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_USBD_CANDTU_H */

/* end of file */
