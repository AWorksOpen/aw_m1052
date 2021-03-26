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
 * 查找总线上所有的设备数量
 */
int aw_usbh_find_dev_num ();

/**
 * 总线上距上一次调用的设备数量变化
 */
int aw_usbh_dev_change_num ();


/**
 * 打开usbh设备树
 */
aw_usbh_dev_tree_t aw_usbh_dev_tree_open (int timeout);

/**
 * 查找下一个设备
 */
aw_err_t aw_usbh_dev_find_next (aw_usbh_dev_tree_t         tree,
                                struct aw_usbh_dev_handle *p_dev_handle);

/**
 * 查找设备中下一个功能
 */
aw_err_t aw_ubsh_func_find_next (struct aw_usbh_dev_handle  *p_dev_handle,
                                 struct aw_usbh_func_handle *p_func_handle);

/**
 * 查找功能中下一个接口
 */
aw_err_t aw_ubsh_intf_find_next (struct aw_usbh_dev_handle  *p_dev_handle,
                                 struct aw_usbh_func_handle *p_func_handle,
                                 struct aw_usbh_intf_handle *p_intf_handle);

/**
 * 查找接口中下一个端点
 */
aw_err_t aw_ubsh_ep_find_next (struct aw_usbh_dev_handle  *p_dev_handle,
                               struct aw_usbh_intf_handle *p_intf_handle,
                               struct aw_usbh_ep_handle   *p_ep_handle);

/**
 * 关闭设备树
 */
aw_err_t aw_usbh_dev_tree_close (aw_usbh_dev_tree_t tree);


/**
 * 查询设备是否存在
 */
aw_bool_t aw_usbh_dev_is_connect (struct aw_usbh_dev_handle  *p_dev_handle);


/**
 * 查看功能是否已经绑定驱动，只有设备当前无驱动时才能进行操作
 */
aw_err_t aw_usbh_func_is_driver_connected(
                                struct aw_usbh_dev_handle  *p_dev_handle,
                                struct aw_usbh_func_handle *p_func_handle,
                                aw_bool_t                  *p_ret);


/**
 * 打开设备（使用结束了需要调用aw_usbh_device_close关闭设备，否则设备内存无法释放）
 */
aw_err_t aw_usbh_dev_open (struct aw_usbh_dev_handle *p_dev_handle);


/**
 * 关闭设备
 */
aw_err_t aw_usbh_dev_close (struct aw_usbh_dev_handle *p_dev_handle);


/**
 * 设备控制传输
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
 * 设备批量传输-写
 */
aw_err_t aw_usbh_dev_bulk_write (struct aw_usbh_dev_handle *p_dev_handle,
                                 uint8_t                    ep,
                                 void                      *p_data,
                                 int                        len,
                                 int                        timeout);

/**
 * 设备批量传输-读
 */
aw_err_t aw_usbh_dev_bulk_read (struct aw_usbh_dev_handle *p_dev_handle,
                                uint8_t                    ep,
                                void                      *p_data,
                                int                        len,
                                int                        timeout);

/**
 * 设备中断传输-写
 */
aw_err_t aw_usbh_dev_intr_write (struct aw_usbh_dev_handle *p_dev_handle,
                                 uint8_t                    ep,
                                 void                      *p_data,
                                 int                        len,
                                 int                        timeout);

/**
 * 设备中断传输-读
 */
aw_err_t aw_usbh_dev_intr_read (struct aw_usbh_dev_handle *p_dev_handle,
                                uint8_t                    ep,
                                void                      *p_data,
                                int                        len,
                                int                        timeout);


/******************************************************************************
 *  USB HOST标准操作
 ******************************************************************************/

/**
 * 端点复位
 */
aw_err_t aw_usbh_ep_reset (struct aw_usbh_dev_handle *p_dev_handle, uint8_t ep);

/**
 * 端点halt特性清除
 */
aw_err_t aw_usbh_ep_halt_clr (struct aw_usbh_dev_handle *p_dev_handle, uint8_t ep);

/**
 * 设备配置设置
 */
aw_err_t aw_usbh_cfg_set (struct aw_usbh_dev_handle *p_dev_handle, uint8_t cfg);

/**
 * 描述符获取
 */
aw_err_t aw_usbh_desc_get (struct aw_usbh_dev_handle *p_dev_handle,
                           uint16_t                   type,
                           uint16_t                   value,
                           uint16_t                   index,
                           void                      *p_buf,
                           size_t                     len);

/**
 * 字符串获取
 */
aw_err_t aw_usbh_string_get (struct aw_usbh_dev_handle *p_dev_handle,
                             int                        index,
                             char                      *p_buf,
                             size_t                     len);

/**
 * 设备描述符获取
 */
aw_err_t aw_usbh_dev_desc_get (struct aw_usbh_dev_handle *p_dev_handle,
                               struct aw_usb_device_desc *p_desc);

/**
 * 全部配置描述符获取，内部使用动态内存分配，
 * 使用结束后需要使用aw_usbh_cfg_desc_free进行内存释放
 */
aw_err_t aw_usbh_cfg_desc_get (struct aw_usbh_dev_handle  *p_dev_handle,
                               struct aw_usb_config_desc **p_config,
                               uint8_t                     cfg);

/**
 * 全部配置描述符内存释放
 */
aw_err_t aw_usbh_cfg_desc_free (struct aw_usb_config_desc *p_config);


#endif /* AW_USB_AW_USBH_H_ */
