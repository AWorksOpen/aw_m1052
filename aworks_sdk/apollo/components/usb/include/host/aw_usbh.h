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


#ifndef AW_USB_AW_USBH_H_
#define AW_USB_AW_USBH_H_

#include "aw_list.h"
#include "host/awbl_usbh.h"

typedef void *  aw_usbh_dev_tree_t;

struct aw_usbh_dev_handle {
    struct aw_usb_device_desc desc;
    char                      name[32];
};

struct aw_usbh_cfg_handle {
    struct aw_usb_config_desc desc;
};

struct aw_usbh_func_handle {
    uint8_t         ifirst;
    uint8_t         nitfs;
    uint8_t         clss;
    uint8_t         sclss;
    uint8_t         proto;
};

struct aw_usbh_intf_handle {
    struct aw_usb_interface_desc desc;
};

struct aw_usbh_ep_handle {
    struct aw_usb_endpoint_desc desc;
};


/**
 * �������������е��豸����
 */
int aw_usbh_find_dev_num ();

/**
 * �����Ͼ���һ�ε��õ��豸�����仯
 */
int aw_usbh_dev_change_num ();


/**
 * ��usbh�豸��
 */
aw_usbh_dev_tree_t aw_usbh_dev_tree_open (int timeout);

/**
 * ������һ���豸
 */
aw_err_t aw_usbh_dev_find_next (aw_usbh_dev_tree_t         tree,
                                struct aw_usbh_dev_handle *p_dev_handle);

/**
 * �����豸����һ������
 */
aw_err_t aw_ubsh_func_find_next (struct aw_usbh_dev_handle  *p_dev_handle,
                                 struct aw_usbh_func_handle *p_func_handle);

/**
 * ���ҹ�������һ���ӿ�
 */
aw_err_t aw_ubsh_intf_find_next (struct aw_usbh_dev_handle  *p_dev_handle,
                                 struct aw_usbh_func_handle *p_func_handle,
                                 struct aw_usbh_intf_handle *p_intf_handle);

/**
 * ���ҽӿ�����һ���˵�
 */
aw_err_t aw_ubsh_ep_find_next (struct aw_usbh_dev_handle  *p_dev_handle,
                               struct aw_usbh_intf_handle *p_intf_handle,
                               struct aw_usbh_ep_handle   *p_ep_handle);

/**
 * �ر��豸��
 */
aw_err_t aw_usbh_dev_tree_close (aw_usbh_dev_tree_t tree);


/**
 * ��ѯ�豸�Ƿ����
 */
aw_bool_t aw_usbh_dev_is_connect (struct aw_usbh_dev_handle  *p_dev_handle);


/**
 * �鿴�����Ƿ��Ѿ���������ֻ���豸��ǰ������ʱ���ܽ��в���
 */
aw_err_t aw_usbh_func_is_driver_connected(
                                struct aw_usbh_dev_handle  *p_dev_handle,
                                struct aw_usbh_func_handle *p_func_handle,
                                aw_bool_t                  *p_ret);


/**
 * ���豸��ʹ�ý�������Ҫ����aw_usbh_device_close�ر��豸�������豸�ڴ��޷��ͷţ�
 */
aw_err_t aw_usbh_dev_open (struct aw_usbh_dev_handle *p_dev_handle);


/**
 * �ر��豸
 */
aw_err_t aw_usbh_dev_close (struct aw_usbh_dev_handle *p_dev_handle);


/**
 * �豸���ƴ���
 */
aw_err_t aw_usbh_ctrl_msg_sync (struct aw_usbh_dev_handle *p_dev_handle,
                                uint8_t                    reqtype,
                                uint8_t                    request,
                                uint16_t                   value,
                                uint16_t                   index,
                                uint16_t                   length,
                                void                      *p_data,
                                int                        timeout);

/**
 * �豸��������-д
 */
aw_err_t aw_usbh_dev_bulk_write (struct aw_usbh_dev_handle *p_dev_handle,
                                 uint8_t                    ep,
                                 void                      *p_data,
                                 int                        len,
                                 int                        timeout);

/**
 * �豸��������-��
 */
aw_err_t aw_usbh_dev_bulk_read (struct aw_usbh_dev_handle *p_dev_handle,
                                uint8_t                    ep,
                                void                      *p_data,
                                int                        len,
                                int                        timeout);

/**
 * �豸�жϴ���-д
 */
aw_err_t aw_usbh_dev_intr_write (struct aw_usbh_dev_handle *p_dev_handle,
                                 uint8_t                    ep,
                                 void                      *p_data,
                                 int                        len,
                                 int                        timeout);

/**
 * �豸�жϴ���-��
 */
aw_err_t aw_usbh_dev_intr_read (struct aw_usbh_dev_handle *p_dev_handle,
                                uint8_t                    ep,
                                void                      *p_data,
                                int                        len,
                                int                        timeout);


/******************************************************************************
 *  USB HOST��׼����
 ******************************************************************************/

/**
 * �˵㸴λ
 */
aw_err_t aw_usbh_ep_reset (struct aw_usbh_dev_handle *p_dev_handle, uint8_t ep);

/**
 * �˵�halt�������
 */
aw_err_t aw_usbh_ep_halt_clr (struct aw_usbh_dev_handle *p_dev_handle, uint8_t ep);

/**
 * �豸��������
 */
aw_err_t aw_usbh_cfg_set (struct aw_usbh_dev_handle *p_dev_handle, uint8_t cfg);

/**
 * ��������ȡ
 */
aw_err_t aw_usbh_desc_get (struct aw_usbh_dev_handle *p_dev_handle,
                           uint16_t                   type,
                           uint16_t                   value,
                           uint16_t                   index,
                           void                      *p_buf,
                           size_t                     len);

/**
 * �ַ�����ȡ
 */
aw_err_t aw_usbh_string_get (struct aw_usbh_dev_handle *p_dev_handle,
                             int                        index,
                             char                      *p_buf,
                             size_t                     len);

/**
 * �豸��������ȡ
 */
aw_err_t aw_usbh_dev_desc_get (struct aw_usbh_dev_handle *p_dev_handle,
                               struct aw_usb_device_desc *p_desc);

/**
 * ȫ��������������ȡ���ڲ�ʹ�ö�̬�ڴ���䣬
 * ʹ�ý�������Ҫʹ��aw_usbh_cfg_desc_free�����ڴ��ͷ�
 */
aw_err_t aw_usbh_cfg_desc_get (struct aw_usbh_dev_handle  *p_dev_handle,
                               struct aw_usb_config_desc **p_config,
                               uint8_t                     cfg);

/**
 * ȫ�������������ڴ��ͷ�
 */
aw_err_t aw_usbh_cfg_desc_free (struct aw_usb_config_desc *p_config);


#endif /* AW_USB_AW_USBH_H_ */
