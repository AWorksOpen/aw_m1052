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
#ifndef __USB_AW_USBD_VPRINTER_H
#define __USB_AW_USBD_VPRINTER_H
#include "device/aw_usbd.h"
#include "aw_usb_os.h"


/**\brief USB �豸�����ӡ����ӡ����*/
#define __USBD_PRINTER_TRACE(info, ...) \
            do { \
                AW_INFOF(("USBD-PRINTER:"info, ##__VA_ARGS__)); \
            } while (0)


#ifdef USBD_PRINTER_DEBUG
    #define __USBD_PRINTER_TRACE_DEBUG  __USBD_PRINTER_TRACE
    #define usbd_printer_printf  aw_kprintf
#else
    #define __USBD_PRINTER_TRACE_DEBUG(info) \
            do { \
            } while (0)
#endif

#define AW_GET_DEVICE_ID       0
#define AW_GET_PORT_STATUS     1
#define AW_SOFT_RESET          2

#define AW_PRINTER_NOT_ERROR     0x08
#define AW_PRINTER_SELECTED      0x10
#define AW_PRINTER_PAPER_EMPTY   0x20

#define AW_PNP_STRING_LEN        1024


/**\brief USB�豸�����ӡ���ṹ��*/
typedef struct aw_usbd_virtual_printer {
    struct aw_usbd_fun          fun;
    struct aw_usbd_pipe         in;              /**\brief ����˵�*/
    struct aw_usbd_pipe         out;             /**\brief ����˵�*/
    char                        name[32];
    uint8_t                     intf_num;        /**\brief �ӿڱ��*/

    aw_usb_mutex_handle_t       mutex;
    aw_usb_sem_handle_t         semb;            /**\brief �ź���*/
    struct aw_list_head         node;            /**\brief ��ǰ�豸�ڵ�*/
    char                        pnp_string[AW_PNP_STRING_LEN];
    uint8_t                     printer_status;  /**\brief ��ӡ��״̬*/
}aw_usbd_vp_t;









/*******************************��������*************************************/
aw_err_t aw_usbd_printer_create(struct aw_usbd     *p_obj,
                                aw_usbd_vp_t *p_printer,
                                const char         *p_name);
aw_err_t aw_usbd_printer_destory(struct aw_usbd *p_obj,aw_usbd_vp_t *p_printer);
aw_err_t aw_usbd_cdc_vp_read_sync (aw_usbd_vp_t           *p_vp,
                                   void                   *p_buf,
                                   size_t                  nbytes,
                                   int                    timeout);
#endif
