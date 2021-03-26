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


#ifndef __AW_USBD_H
#define __AW_USBD_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_usbd
 * \copydoc aw_usbd.h
 * @{
 */
#include "aw_list.h"
#include "aw_usb_common.h"
#include "aw_usb_mem.h"
#include "aw_usb_os.h"
#include "device/awbl_usbd.h"


/** \brief �ܵ������� */
#define AW_PIPE_DIR_IN      AW_USB_DIR_IN       /**< \brief IN : to host */
#define AW_PIPE_DIR_OUT     AW_USB_DIR_OUT      /**< \brief OUT: to device */


/** \brief �ܵ����Ͷ��� */
#define AW_PIPE_TYPE_CTRL   AW_USB_EP_TYPE_CTRL     /**< \brief ���ƹܵ� */
#define AW_PIPE_TYPE_ISO    AW_USB_EP_TYPE_ISO      /**< \brief ��ʱ�ܵ� */
#define AW_PIPE_TYPE_BULK   AW_USB_EP_TYPE_BULK     /**< \brief �����ܵ� */
#define AW_PIPE_TYPE_INT    AW_USB_EP_TYPE_INT      /**< \brief �жϹܵ� */


/** \brief USBD����ṹ���� */
struct aw_usbd_trans {
    struct aw_usbd_pipe     *p_pipe;    /**< \brief �ܵ�ָ�� */
    struct awbl_usbd_trans   tr;        /**< \brief ����ṹ */
};




/** \brief USBD�ܵ���Ϣ�ṹ���� */
struct aw_usbd_pipe_info {
    uint8_t  num;       /**< \brief �˵��ַ    0Ϊ�Զ����� */
    uint8_t  dir;       /**< \brief �˵㷽��    1:IN 0:OUT */
    uint8_t  attr;      /**< \brief �������ͺͲ�����Ϣ */
    uint8_t  inter;     /**< \brief �жϺ͵�ʱ�˵�ķ���ʱ�� */
    uint16_t mps;       /**< \brief �˵�ÿ������������������ֽ��� */
};




/** \brief USBD�ܵ����Ͷ��� */
struct aw_usbd_pipe {
    struct awbl_usbd_ep             *p_hw;      /**< \brief �˵�ָ�� */
    const struct aw_usbd_pipe_info  *p_info;    /**< \brief �ܵ���Ϣָ�� */
    struct aw_usbd_fun              *p_fun;     /**< \brief �ӿ�ָ�� */
    uint8_t                          addr;      /**< \brief �ܵ���ַ */
    struct aw_list_head              node;      /**< \brief �ڵ� */
    aw_usb_sem_handle_t              sync;
};



/* �ӿ�����������*/
struct awbl_usbd_iad {
    uint8_t  first_if;      /* ��һ���ӿڱ��*/
    uint8_t  if_count;      /* �ӿ�����*/
    uint8_t  fun_class;     /* �ӿ���*/
    uint8_t  fun_subclass;  /* �ӿ�����*/
    uint8_t  fun_protocol;  /* �ӿ�Э��*/
    uint8_t  ifunc;
    aw_bool_t used;         /* �Ƿ�ʹ��*/
};


/** \brief USBD�ӿ���Ϣ */
struct aw_usbd_fun_info {
    uint8_t       clss;         /**< \brief ����� */
    uint8_t       sub_clss;     /**< \brief ������� */
    uint8_t       protocol;     /**< \brief Э����� */

    /** \brief ������ûص� */
    int (*pfn_alt_set) (struct aw_usbd_fun  *p_fun,
                        aw_bool_t            set);

    /** \brief setup���ƴ���ص� */
    int (*pfn_ctrl_setup) (struct aw_usbd_fun    *p_fun,
                           struct aw_usb_ctrlreq *p_setup,
                           void                  *p_buf,
                           int                    buf_len);
};




/** \brief USBD�ӿڽṹ���� */
struct aw_usbd_fun {
    struct aw_usbd                *p_obj;      /**< \brief USBD�豸ָ�� */
    const char                    *name;       /**< \brief �ӿ����� */
    const struct aw_usbd_fun_info *p_info;     /**< \brief �ӿ� ��Ϣ */
    uint8_t                        num;        /**< \brief �ӿڱ�� */
    uint8_t                        alt;        /**< \brief ������ñ�� */
    uint8_t                        num_alt;    /**< \brief ֻ������0��Ч*/
    struct aw_usbd_fun            *p_next_alt; /**< \brief ���һ��ΪNULL*/
    aw_bool_t                      need_iad;
    uint8_t                       *p_spec_des; /**< \brief ���Զ���������*/
    uint8_t                        spec_size;  /**< \brief ���Զ�����������С*/
    aw_bool_t                      set;        /**< \brief �Ƿ����� */
    uint8_t                        neps;       /**< \brief �˵����� */
    struct aw_list_head            pipes;      /**< \brief �ܵ����� */
    struct aw_list_head            node;       /**< \brief �ڵ� */
    uint8_t                        strid;      /**< \brief �ַ������� */
};


/* �ӿ������������������*/
#define AW_USBD_MAX_IAD_NUM     10
/** \brief USBD���ýṹ���� */
struct aw_usbd_cfg {
    struct aw_usbd        *p_obj;                     /**< \brief USBD�豸ָ�� */
    struct awbl_usbd_iad   iads[AW_USBD_MAX_IAD_NUM]; /**< \brief �ӿ�����������*/
    struct aw_list_head    itfs;                      /**< \brief �ӿ����� */
    uint8_t                nitfs;                     /**< \brief �ӿ����� */
    uint8_t                value;                     /**< \brief ���ñ�� */
    struct aw_list_head    node;                      /**< \brief �ڵ� */
};




/** \brief USBD��Ϣ�ṹ���� */
struct aw_usbd_info {
    uint8_t          clss;          /**< \brief ����� */
    uint8_t          sub_clss;      /**< \brief ������� */
    uint8_t          protocol;      /**< \brief Э����� */
    uint8_t          mps0;          /**< \brief �˵�0�����ߴ� */
    uint16_t         bcd;           /**< \brief �豸�汾�� */
    uint16_t         vid;           /**< \brief ����ID */
    uint16_t         pid;           /**< \brief ��ƷID */
    const char      *manufacturer;  /**< \brief �������ַ������� */
    const char      *product;       /**< \brief ��Ʒ�ַ������� */
    const char      *serial;        /**< \brief ���к��ַ������� */
};




/** \brief USBD����ṹ���� */
struct aw_usbd {
    struct awbl_usbd           *p_usbd;     /**< \brief USBD�豸ָ�� */
    const struct aw_usbd_info  *p_info;     /**< \brief �豸��Ϣ */
    struct aw_usbd_cfg         *p_cur_cfg;  /**< \brief ��ǰ����ָ�� */
    struct aw_usbd_cfg          def_cfg;    /**< \brief Ĭ������ */
    struct aw_list_head         cfgs;       /**< \brief �������� */
    uint8_t                     ncfgs;      /**< \brief �������� */
    const struct aw_usbd_cb    *p_cb;       /**< \brief �豸��ص������б� */
    aw_usb_mutex_handle_t       lock;
    uint8_t                     status;     /**< \brief �豸״̬ */
    uint8_t                     strid_mf;   /**< \brief �����ַ������� */
    uint8_t                     strid_pd;   /**< \brief ��Ʒ�ַ������� */
    uint8_t                     strid_sn;   /**< \brief ���к��ַ������� */
};




/** \brief USB�豸��ص������б� */
struct aw_usbd_cb {

    /** \brief ���������ӵ��豸 */
    int (*pfn_link) (struct aw_usbd *p_obj);

    /** \brief ���������豸�Ͽ����� */
    int (*pfn_unlink) (struct aw_usbd *p_obj);

    /** \brief ���յ�SETUP�� */
    int (*pfn_ctrl_setup) (struct aw_usbd        *p_obj,
                           struct aw_usb_ctrlreq *p_setup,
                           void                  *p_buf,
                           int                    buf_len);

    /** \brief ���û�������� */
    int (*pfn_config_set) (struct aw_usbd      *p_obj,
                           struct aw_usbd_cfg  *p_cfg,
                           aw_bool_t            set);

    /** \brief ���߹��� */
    void (*pfn_suspend) (struct aw_usbd *p_obj);

    /** \brief ���߻ָ� */
    void (*pfn_resume) (struct aw_usbd  *p_obj);

    /** \brief ���߶Ͽ����� */
    void (*pfn_disconnect) (struct aw_usbd *p_obj);
};




/**
 * \brief USB device�����ʼ������
 *
 * \param[in] p_obj      USBD����ָ��
 * \param[in] p_info     USBD�豸��Ϣ�ṹָ��
 * \param[in] p_cb       USBD�ص������б�ָ��
 * \param[in] controller USB device����������
 *
 * \retval  AW_OK       ��ʼ���ɹ�
 * \retval  -AW_EINVAL  ��Ч����
 * \retval  -AW_ENODEV  �豸������
 * \retval  -AW_ENOMEM  �洢�ռ䲻��
 */
int aw_usbd_init (struct aw_usbd             *p_obj,
                  const struct aw_usbd_info  *p_info,
                  const struct aw_usbd_cb    *p_cb,
                  const char                 *controller);



/**
 * \brief USB device��������
 *
 * \param[in] p_obj      USBD����ָ��
 *
 * \retval  AW_OK       �����ɹ�
 * \retval  -AW_EINVAL  ��Ч����
 * \retval  -AW_EBUSY   �豸æ
 * \retval  -AW_ENOMEM  �洢�ռ䲻��
 */
int aw_usbd_cfg_create (struct aw_usbd  *p_obj);




/**
 * \brief USB device��ʼ���ӿ�
 *
 * \param[in] p_fun      USBD�ӿ�ָ��
 * \param[in] name       �ӿ�����
 * \param[in] p_info     �ӿ���Ϣָ��
 *
 * \retval  AW_OK       ��ʼ���ɹ�
 * \retval  -AW_EINVAL  ��Ч����
 */
int aw_usbd_fun_init (struct aw_usbd_fun            *p_fun,
                      const char                    *name,
                      const struct aw_usbd_fun_info *p_info);




/**
 * \brief ��ӽӿڵ�ָ������
 *
 * \param[in] p_obj      USBD����ָ��
 * \param[in] value      ���ñ��
 * \param[in] p_fun      USBD�ӿ�ָ��
 *
 * \retval  AW_OK       ��ӳɹ�
 * \retval  -AW_EBUSY   �豸æ
 * \retval  -AW_ENOENT  ��Ӧ���ò�����
 * \retval  -AW_EINVAL  ��Ч����
 */
int aw_usbd_fun_add (struct aw_usbd     *p_obj,
                     uint8_t             value,
                     struct aw_usbd_fun *p_fun);




/*
 * ��Ҫ��aw_usbd_fun_add����ã���ΪĳЩspec_des��Ҫʹ�ýӿںŵ���Ϣ
 */
int aw_usbd_fun_spec_desc_add (struct aw_usbd_fun *p_fun,
                               uint8_t            *p_desc,
                               uint8_t             size);

/**
 *  ���iad����������
 *  ��Ҫ��aw_usbd_fun_add����øú���
 */
int aw_usbd_fun_iad_create(struct aw_usbd_fun *p_fun,
                           uint8_t             cfg_value,
                           uint8_t             count);



/**
 * Ϊ�ӿ����altsetting
 * 
 */
int aw_usbd_fun_altset_add(struct aw_usbd_fun *p_def_fun,
                           struct aw_usbd_fun *p_alt_fun,
                           uint8_t             cfg_value,
                           uint8_t             alt_value);


/**
 * \brief ��ָ�������Ƴ�һ���ӿ�
 *
 * \param[in] p_obj      USBD����ָ��
 * \param[in] value      ���ñ��
 * \param[in] p_fun      USBD�ӿ�ָ��
 *
 * \retval  AW_OK       �Ƴ��ɹ�
 * \retval  -AW_EBUSY   �豸æ
 * \retval  -AW_ENOENT  ��Ӧ���ò�����
 * \retval  -AW_EINVAL  ��Ч����
 */
int aw_usbd_fun_del (struct aw_usbd     *p_obj,
                     uint8_t             value,
                     struct aw_usbd_fun *p_fun);




/**
 * \brief ���������һ���ܵ���ָ���ӿ�
 *
 * \param[in] p_fun      USBD�ӿ�ָ��
 * \param[in] p_pipe     USBD�ܵ�ָ��
 * \param[in] p_info     USBD�ܵ���Ϣ
 *
 * \retval  AW_OK       ������ӳɹ�
 * \retval  -AW_EBUSY   �豸æ
 * \retval  -AW_EINVAL  ��Ч����
 */
int aw_usbd_pipe_create (struct aw_usbd_fun              *p_fun,
                         struct aw_usbd_pipe             *p_pipe,
                         const struct aw_usbd_pipe_info  *p_info);



/**
 * \brief ����USB�豸
 *
 * \param[in] p_obj      USB�豸����ָ��
 *
 * \retval  AW_OK       �����ɹ�
 * \retval  -AW_ENODEV  �豸������
 * \retval  -AW_EINVAL  ��Ч����
 */
int aw_usbd_start (struct aw_usbd *p_obj);



/**
 * \brief ֹͣUSB�豸
 *
 * \param[in] p_obj      USB�豸����ָ��
 *
 * \retval  AW_OK       ֹͣ�ɹ�
 * \retval  -AW_ENODEV  �豸������
 * \retval  -AW_EINVAL  ��Ч����
 */
int aw_usbd_stop (struct aw_usbd *p_obj);




/**
 * \brief ���һ������ṹ
 *
 * \param[in] p_trans   USB����ṹָ��
 * \param[in] p_pipe    USB�ܵ�ָ��
 * \param[in] p_buf     �����ڴ��׵�ַ
 * \param[in] len       �����ֽڳ���
 * \param[in] flag      flasg��־
 * \param[in] complete  ���ݴ�����ɻص�
 * \param[in] p_arg     �ص���������
 *
 * \return  ��
 */
void aw_usbd_fill_trans (
            struct aw_usbd_trans  *p_trans,
            struct aw_usbd_pipe   *p_pipe,
            void                  *p_buf,
            size_t                 len,
            int                    flag,
            void                 (*complete) (void *p_arg),
            void                  *p_arg);




/**
 * \brief USB�����첽����
 *
 * \param[in] p_obj      USB�豸����ָ��
 * \param[in] p_trans    USB����ṹָ��
 *
 * \retval  AW_OK         �����ύ���
 * \retval  -AW_ENODEV    �豸������
 * \retval  -AW_EINVAL    ��Ч����
 * \retval  -AW_ESHUTDOWN usb�豸û������
 */
int aw_usbd_trans_async (struct aw_usbd        *p_obj,
                         struct aw_usbd_trans  *p_trans);




/**
 * \brief ȡ��USB���ݴ���
 *
 * \param[in] p_obj      USB�豸����ָ��
 * \param[in] p_trans    USB����ṹָ��
 *
 * \retval  AW_OK         ����ȡ���ɹ�
 * \retval  -AW_ENODEV    �豸������
 * \retval  -AW_EINVAL    ��Ч����
 * \retval  -AW_ESHUTDOWN usb�豸û������
 */
int aw_usbd_trans_cancel (struct aw_usbd        *p_obj,
                          struct aw_usbd_trans  *p_trans);




/**
 * \brief USB����ͬ������
 *
 * \param[in] p_obj      USB�豸����ָ��
 * \param[in] p_pipe     USB�ܵ�ָ��
 * \param[in] p_buf      �����ڴ��׵�ַ
 * \param[in] len        �����ֽڳ���
 * \param[in] flag       flasg��־
 * \param[in] timeout    ���䳬ʱʱ�䣨���룩
 *
 * \retval  -AW_ENODEV    �豸������
 * \retval  -AW_EINVAL    ��Ч����
 * \retval  -AW_ESHUTDOWN usb�豸û������
 * \retval  >= 0          �Ѵ�����ֽ���
 */
int aw_usbd_trans_sync (struct aw_usbd       *p_obj,
                        struct aw_usbd_pipe  *p_pipe,
                        void                 *p_buf,
                        size_t                len,
                        int                   flag,
                        int                   timeout);




/**
 * \brief USB���ú����STALL���ְ�
 *
 * \param[in] p_obj      USB�豸����ָ��
 * \param[in] p_pipe     USB�ܵ�ָ��
 * \param[in] set        �Ƿ�����
 *
 * \retval  AW_OK         ���óɹ�
 * \retval  -AW_EINVAL    ��Ч����
 */
int aw_usbd_pipe_stall (struct aw_usbd       *p_obj,
                        struct aw_usbd_pipe  *p_pipe,
                        aw_bool_t             set);




/**
 * \brief USB�ܵ���λ
 *
 * \param[in] p_obj      USB�豸����ָ��
 * \param[in] p_pipe     USB�ܵ�ָ��
 *
 * \retval  AW_OK         ��λ�ɹ�
 * \retval  -AW_EINVAL    ��Ч����
 */
int aw_usbd_pipe_reset (struct aw_usbd       *p_obj,
                        struct aw_usbd_pipe  *p_pipe);




/**
 * \brief �ж�USB�豸�Ƿ���Ч
 *
 * \param[in] p_obj      USB�豸����ָ��
 *
 * \retval  AW_TRUE      �豸��Ч
 * \retval  AW_FALSE     �豸��Ч
 */
aw_bool_t aw_usbd_valid (struct aw_usbd  *p_obj);




/**
 * \brief �ж�USB�ӿڹ����Ƿ���Ч
 *
 * \param[in] p_fun      USB�ӿڶ���ָ��
 *
 * \retval  AW_TRUE      �ӿڹ�����Ч
 * \retval  AW_FALSE     �ӿڹ�����Ч
 */
aw_bool_t aw_fun_valid (struct aw_usbd_fun  *p_fun);


/** @} grp_aw_if_usbd */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_USBD_H */

/* end of file */
