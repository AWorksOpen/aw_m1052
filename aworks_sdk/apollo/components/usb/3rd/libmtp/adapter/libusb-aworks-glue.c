/*
 * \file libusb-glue.c
 * Low-level USB interface glue towards libusb.
 *
 * Copyright (C) 2005-2007 Richard A. Low <richard@wentnet.com>
 * Copyright (C) 2005-2012 Linus Walleij <triad@df.lth.se>
 * Copyright (C) 2006-2007 Marcus Meissner
 * Copyright (C) 2007 Ted Bullock
 * Copyright (C) 2008 Chris Bagwell <chris@cnpbagwell.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Created by Richard Low on 24/12/2005. (as mtp-utils.c)
 * Modified by Linus Walleij 2006-03-06
 *  (Notice that Anglo-Saxons use little-endian dates and Swedes
 *   use big-endian dates.)
 *
 */

//#include "config.h"
#include "libmtp.h"
#include "libusb-glue.h"
#include "device-flags.h"
#include "util.h"
#include "ptp.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ptp-pack.c"
#include "mtp_adapter.h"
#include "host/awbl_usbh.h"

/* USB�豸����*/
extern struct aw_list_head g_udev_list;

#ifndef USB_CLASS_PTP
#define USB_CLASS_PTP 6
#endif

/*
 * Ĭ��USB����ʱ���ȡ����ֵ���Ը�����Ҫ��д����Ӧʹ�ú����ֵ���Ա�����
 * ������USB���������ɡ�ԭʼֵ4000�����Խ��д��ļ����䣬���ң�����������
 * ����ʱ���ռ����ݡ����ߵ�ֵҲ������/�Ͽ����ɿ���
 */
#define USB_TIMEOUT_DEFAULT     20000
#define USB_TIMEOUT_LONG        60000
/* ��ȡUSB����ʱʱ��*/
static inline int get_timeout(PTP_USB* ptp_usb)
{
    /* ��Щ�豸��һЩ��������Ҫ��ʱ�䳬ʱ*/
    if (FLAG_LONG_TIMEOUT(ptp_usb)) {
        return USB_TIMEOUT_LONG;
    }
    return USB_TIMEOUT_DEFAULT;
}

/* USB ������Ϣ���ݽ׶η��� */
#ifndef USB_DP_HTD
#define USB_DP_HTD      (0x00 << 7) /* �������豸*/
#endif
#ifndef USB_DP_DTH
#define USB_DP_DTH      (0x01 << 7) /* �豸������*/
#endif

/* USB Feature selector HALT */
#ifndef USB_FEATURE_HALT
#define USB_FEATURE_HALT    0x00
#endif

/* MTP�豸����ṹ��*/
struct mtpdevice_list_struct {
  struct mtp_usb_device *libusb_device;    /* USB�豸�ṹ��*/
  PTPParams *params;                       /* PTP�����ṹ��*/
  PTP_USB *ptp_usb;                        /* PTP USB�ṹ��*/
  u32 bus_location;                        /* USB�豸�����ߵ�λ��*/
  struct mtpdevice_list_struct *next;      /* ָ����һ��MTP�豸�ṹ��ָ��*/
};
typedef struct mtpdevice_list_struct mtpdevice_list_t;

/* MTP�豸��*/
static const LIBMTP_device_entry_t mtp_device_table[] = {
#include "music-players.h"
};
/* MTP�豸���С*/
static const int mtp_device_table_size = sizeof(mtp_device_table) / sizeof(LIBMTP_device_entry_t);

static int usb_clear_stall_feature(PTP_USB* ptp_usb, int ep);
static int usb_get_endpoint_status(PTP_USB* ptp_usb, int ep, u16* status);


/**
 * ��ȡ֧�ֵ�USB�豸���б�
 *
 * @param devices Ҫ���ص�֧�ֵ�USB�豸�б�
 * @param numdevs ֧�ֵ�USB�豸�б������
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
 */
int LIBMTP_Get_Supported_Devices_List(LIBMTP_device_entry_t * * const devices, int * const numdevs) {
    *devices = (LIBMTP_device_entry_t *) & mtp_device_table;
    *numdevs = mtp_device_table_size;
    return 0;
}

/**
 * �µ�USB�豸׷�ӵ�USB MTP�豸��������
 * @param devlist    Ҫ��̬���ӵ�MTP USB�豸ָ������
 * @param newdevice  Ҫ��ӵ��µ�USB�豸
 *
 * @return  �ɹ�����MTP�豸����ʧ�ܷ���NULL
 */
static mtpdevice_list_t *append_to_mtpdevice_list(mtpdevice_list_t *devlist,
                          struct mtp_usb_device *newdevice,
                          u32 bus_location){
    mtpdevice_list_t *new_list_entry;

    new_list_entry = (mtpdevice_list_t *) mtp_malloc(sizeof(mtpdevice_list_t));
    if (new_list_entry == NULL) {
        return NULL;
    }
    /* ���USB�豸����Ϊһ������*/
    new_list_entry->libusb_device = newdevice;
    new_list_entry->bus_location = bus_location;
    new_list_entry->next = NULL;

    /* �ж�MTP�豸������û������MTP�豸�ڵ�*/
    if (devlist == NULL) {
        return new_list_entry;
    } else {
        mtpdevice_list_t *tmp = devlist;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = new_list_entry;
    }
    return devlist;
}

/**
 * �ͷŷ����USB MTP�豸�����MTP�豸�ڵ��ڴ�
 *
 * @param devlist ָ�����MTP���Ե�USB�豸��ָ��Ķ�̬����
 * @return ��
 */
static void free_mtpdevice_list(mtpdevice_list_t *devlist)
{
    mtpdevice_list_t *tmplist = devlist;

    if (devlist == NULL)
        return;
    while (tmplist != NULL) {
        mtpdevice_list_t *tmp = tmplist;
        tmplist = tmplist->next;
        mtp_free(tmp);
    }
    return;
}


/**
 * ���һ���豸�Ƿ���MTP�������������������gPhoto bug 1482084�н�������ϸ˵����һЩû�и��ӵ��ַ���
 * �Ĺٷ��ĵ���Microsoft��http://www.microsoft.com/whdc/system/bus/USB/USBFAQ_intermed.mspx#E3HAC
 * ����
 * @param dev         libusb��һ���豸�ṹ��
 * @return  ����豸����MTPЭ�鷵��1������0�򲻷���
 */
static int probe_device_descriptor(struct mtp_usb_device *dev)
{
    char *string_buf = NULL;
    unsigned char buf[1024], cmd;
    int i;
    int ret;
    struct awbl_usbh_interface *itf, *itf_temp;
    /* ��ָ���������Ƿ��ҵ�һЩ�Զ���ӿ�*/
    int found_vendor_spec_interface = 0;


    /* ��Ҫ��鲻̫���ܰ����κ�mtp�ӿڵ��豸���ڷ���һЩ��ֵ����ʱ�����������ж�*/
    if (!(dev->p_desc->bDeviceClass == AW_USB_CLASS_PER_INTERFACE ||
            dev->p_desc->bDeviceClass == AW_USB_CLASS_COMM ||
            dev->p_desc->bDeviceClass == USB_CLASS_PTP ||
            dev->p_desc->bDeviceClass == 0xEF || /* Intf. Association Desc.*/
            dev->p_desc->bDeviceClass == AW_USB_CLASS_VENDOR_SPEC)) {
        return 0;
    }

    /* ��������*/
    if (dev->cfg.p_desc) {
        /* ���豸�����Ϻͽӿ���ѭ����֧��ŵ����MTP���ֻ�(���ܻ��������ֻ�)��MTP�ӿ���������ͨ�����ַ�����MTP����
         * ����������ǳ��Ը���µġ�OS ��������(������)֮ǰ��⵽���ǵİ취��*/
        for (i = 0; i < dev->p_desc->bNumConfigurations; i++) {
            u8 j;
            /* ��������*/
            if(i > 0){
                ret = awbl_usbh_dev_cfg_set(dev, i + 1);
                if(ret != AW_OK){
                    return 0;
                }
            }
            /* �����ӿ�*/
            for (j = 0; j < dev->cfg.nfuns; j++) {
                /* �����ӿںͱ��ýӿ�*/
                aw_list_for_each_entry_safe(itf,
                                            itf_temp,
                                            &dev->cfg.funs[j].itfs,
                                            struct awbl_usbh_interface,
                                            node){
                    /* MTP�ӿ�ӵ�е����˵㣬���������˵��һ���ж϶˵㡣����̽���Ķ���*/
                    if (itf->p_desc->bNumEndpoints != 3)
                        continue;

                    /* �ҵ�һ���Զ���Ľӿڣ������*/
                    if (itf->p_desc->bInterfaceClass == AW_USB_CLASS_VENDOR_SPEC) {
                        found_vendor_spec_interface = 1;
                    }
                    /* �����������ڽӿ���������MTP���ַ��������磺"RIM MS/MTP"*/
                    string_buf = awbl_usbh_string_get(dev, itf->p_desc->iInterface);
                    if(string_buf != NULL){
                        if(strlen(string_buf) < 3)
                            continue;
                        if (strstr(string_buf, "MTP") != NULL) {
                            mtp_printf("Configuration %d, interface %d, altsetting %d:\n", i, itf->p_desc->bInterfaceNumber
                                        , itf->p_desc->bAlternateSetting);
                            mtp_printf("Interface description contains the string \"MTP\"\n");
                            mtp_printf("Device recognized as MTP, no further probing.\n");
                            return 1;
                        }
                    }
                }
            }
        }
    }
    else {
        if (dev->p_desc->bNumConfigurations)
            LIBMTP_INFO("dev->config is NULL in probe_device_descriptor yet dev->descriptor.bNumConfigurations > 0\n");
    }
    /* �豸��һ���Զ����豸����ӵ���Զ���ӿ�*/
    if (dev->p_desc->bDeviceClass == AW_USB_CLASS_VENDOR_SPEC ||
        found_vendor_spec_interface) {
        /* ��ȡ�����������*/
        ret = awbl_usbh_desc_get(dev, 0x03, 0xee, 0, sizeof(buf), buf);
        /* ���ʧ���ˣ���ֹͣ������˵��ֹͣ����˵���ⲻ��MTP�豸*/
        if (ret < 0) {
          /* �˵�0��Ĭ�ϵĿ��ƶ˵� */
            awbl_usbh_ep_halt_clr(&dev->ep0);
            return 0;
        }
        /* ��������������Ƿ�����10�ֽ�*/
        if (ret < 10) {
            return 0;
        }

        /* ����豸�Ƿ�ӵ��΢�������� */
        if (!((buf[2] == 'M') && (buf[4] == 'S') &&
          (buf[6] == 'F') && (buf[8] == 'T'))) {
            return 0;
        }

        /* ����豸�Ƿ���Ӧ������Ϣ1�����Ƿ����� */
        cmd = buf[16];
        ret = awbl_usbh_ctrl_sync(&dev->ep0, AW_USB_DIR_IN|AW_USB_REQ_TAG_DEVICE|AW_USB_REQ_TYPE_VENDOR,
                                cmd, 0, 4, sizeof(buf), buf, 5000, 0);
        if(ret > 0){
            mtp_printf("Microsoft device response to control message 1, CMD 0x%02x:\n", cmd);
        }
        /* ����������������豸Ҳ����MTP���д��� */
        if (ret <= 0x15) {
            /* TODO: ����д��󣬱��������ĳ�ַ�ʽ���û�֪�� */
            /* if(ret == -1) {} */
            return 0;
        }
        /* ����豸�Ƿ���һ��MTP���Ƿ���һ����Janus DRM֧�ֵ�USB�������洢�豸 */
        if ((buf[0x12] != 'M') || (buf[0x13] != 'T') || (buf[0x14] != 'P')) {
            return 0;
        }

        /* �������֮�����ǿ����ڴ���һ��MTP�豸*/
        /* ����Ƿ��豸��Ӧ������Ϣ2��������չ�豸������������豸ֻ��ʹ�����һ����ͬ����Ϣ��������Ӧ��
         * ��Щ��ʹ��pure garbage�����ǲ���������������ⲻ�Ǻ���Ҫ��*/
        ret = awbl_usbh_ctrl_sync(&dev->ep0, AW_USB_DIR_IN|AW_USB_REQ_TAG_DEVICE|AW_USB_REQ_TYPE_VENDOR,
                                cmd, 0, 5, sizeof(buf), buf, 5000, 0);
        if (ret > 0) {
            mtp_printf("Microsoft device response to control message 2, CMD 0x%02x:\n", cmd);
        }
        /* ����������������豸�Կ�����Ϣ2���� */
        if (ret == -1) {
          /* TODO: ʵ�ֻص��������ù������֪���������⣬��������������� */
            LIBMTP_ERROR("Potential MTP Device with VendorID:%04x and "
               "ProductID:%04x encountered an error responding to "
               "control message 2.\n"
               "Problems may arrise but continuing\n",
               dev->p_desc->idVendor, dev->p_desc->idProduct);
        } else if (ret == 0) {
            mtp_printf("Zero-length response to control message 2 (OK)\n");
        } else {
            mtp_printf("Device responds to control message 2 with some data.\n");
        }
        return 1;
    }
    return 0;
}

/**
 * �������ɨ�����������ӵ�usb�豸�������������֪�Ĺ�Ӧ�̺Ͳ�Ʒ��ʶ��ƥ�䣬��������ӵ�MTP�豸����mtp_device_list�С�
 * ͨ����free_mtpdevice_list(mtp_device_list)һ��ʹ�á�
 *
 * @param mtp_device_list  MTP�豸����
 * @return  LIBMTP_ERROR_NONE��ʾ���豸���ҵ����ʵ���ɨ������LIBMTP_ERROR_NO_DEVICE_ATTACHED��ʾ
 *          û���豸���ҵ���
 */
static LIBMTP_error_number_t get_mtp_usb_device_list(mtpdevice_list_t ** mtp_device_list){
    int i;
    int found = 0;
    struct mtp_usb_device *dev, *dev_temp;

    /* ����USB�����ϵ��豸*/
    aw_list_for_each_entry_safe(dev, dev_temp, &g_udev_list, struct mtp_usb_device, node){
        /* ���ȼ���ǲ�����֪��MTP�豸����֪��MTP�豸����̽�����ǵ�������*/
        for(i = 0; i < mtp_device_table_size; i++) {
            if(dev->p_desc->idVendor == mtp_device_table[i].vendor_id &&
              dev->p_desc->idProduct == mtp_device_table[i].product_id) {
                /* ׷��USB�豸��MTPdevice���� */
                *mtp_device_list = append_to_mtpdevice_list(*mtp_device_list, dev, 0);
                found = 1;
                break;
            }
        }
        /* �豸������֪��MTP�豸�����̽��һ�¸��豸�ǲ���һ��MTP*/
        if (!found) {
            if (probe_device_descriptor(dev)) {
                /* ׷��USB�豸��MTPdevice���� */
                *mtp_device_list = append_to_mtpdevice_list(*mtp_device_list, dev, 0);
            }
        }
    }
    /* ���û���ҵ��豸������������� */
    if(*mtp_device_list == NULL) {
      return LIBMTP_ERROR_NO_DEVICE_ATTACHED;
    }
    return LIBMTP_ERROR_NONE;
}

/**
 * ���ԭʼMTP�豸�������ͷ����ҵ����豸����
 *
 *  @param devices ָ�������ָ�룬�ñ����������ҵ���ԭʼ�豸�б������⵽���豸������0�����
 *         �ܻ᷵��NULL�����û����������ԭʼ�豸��Ӧ�ü򵥵� <code>free()</code> �������
 *         ���ͷ��ڴ档
 *  @param numdevs ָ��һ��������ָ�룬�������������б��е��豸�����������0��
 *  @return �ɹ�����0��ʧ�ܷ�������ֵ��
 */
LIBMTP_error_number_t LIBMTP_Detect_Raw_Devices(LIBMTP_raw_device_t ** devices,
        int * numdevs) {
    mtpdevice_list_t *devlist = NULL;
    mtpdevice_list_t *dev;
    LIBMTP_error_number_t ret;
    LIBMTP_raw_device_t *retdevs;
    int devs = 0;
    int i, j;

    /* ��ȡUSB�����ϵ�MTP�豸*/
    ret = get_mtp_usb_device_list(&devlist);
    /* û��MTP�豸*/
    if (ret == LIBMTP_ERROR_NO_DEVICE_ATTACHED) {
        *devices = NULL;
        *numdevs = 0;
        return ret;
    } else if (ret != LIBMTP_ERROR_NONE) {
        LIBMTP_ERROR("LIBMTP PANIC: get_mtp_usb_device_list() "
                "error code: %d on line %d\n", ret, __LINE__);
        return ret;
    }
    /* ��ȡMTP�豸����*/
    dev = devlist;
    while (dev != NULL) {
        devs++;
        dev = dev->next;
    }
    /* MTP�豸����Ϊ0*/
    if (devs == 0) {
        *devices = NULL;
        *numdevs = 0;
        return LIBMTP_ERROR_NONE;
    }
    /* ����һ��MTPԭʼ�豸����*/
    retdevs = (LIBMTP_raw_device_t *) mtp_malloc(sizeof(LIBMTP_raw_device_t) * devs);
    if (retdevs == NULL) {
        /* �ڴ治��*/
        *devices = NULL;
        *numdevs = 0;
        return LIBMTP_ERROR_MEMORY_ALLOCATION;
    }
    dev = devlist;
    i = 0;
    while (dev != NULL) {
        int device_known = 0;

        /* ����Ĭ���豸��Ϣ*/
        retdevs[i].device_entry.vendor = NULL;
        retdevs[i].device_entry.vendor_id = dev->libusb_device->p_desc->idVendor;
        retdevs[i].device_entry.product = NULL;
        retdevs[i].device_entry.product_id = dev->libusb_device->p_desc->idProduct;
        retdevs[i].device_entry.device_flags = 0x00000000U;
        /* ��MTP�豸����ƥ�䣬���Ƿ��ܶ�λһЩ������Զ�����Ϣ���豸��־*/
        for(j = 0; j < mtp_device_table_size; j++) {
            if(dev->libusb_device->p_desc->idVendor == mtp_device_table[j].vendor_id &&
                    dev->libusb_device->p_desc->idProduct == mtp_device_table[j].product_id) {
                device_known = 1;
                retdevs[i].device_entry.vendor = mtp_device_table[j].vendor;
                retdevs[i].device_entry.product = mtp_device_table[j].product;
                retdevs[i].device_entry.device_flags = mtp_device_table[j].device_flags;
                /* ������Ա֪�����豸*/
                LIBMTP_INFO("Device %d (VID=%04x and PID=%04x) is a %s %s.\n", i,
                            dev->libusb_device->p_desc->idVendor,
                            dev->libusb_device->p_desc->idProduct,
                            mtp_device_table[j].vendor,
                            mtp_device_table[j].product);
                break;
            }
        }
        /* δ֪�豸*/
        if (!device_known) {
            device_unknown(i, dev->libusb_device->p_desc->idVendor,
                         dev->libusb_device->p_desc->idProduct);
        }
        /* ���������ϵ�λ��*/
        retdevs[i].bus_location = dev->bus_location;
        /* �豸��ַ*/
        retdevs[i].devnum = dev->libusb_device->addr;
        i++;
        dev = dev->next;
    }
    /* ����MTPԭʼ�豸����*/
    *devices = retdevs;
    /* ����MTPԭʼ�豸����*/
    *numdevs = i;
    /* �ͷ�MTP�豸����*/
    free_mtpdevice_list(devlist);
    return LIBMTP_ERROR_NONE;
}

/**
 * ����������ӡ���ײ㵱ǰ�豸��USB��Ϣ
 *
 * @param ptp_usb Ҫ��ȡ��Ϣ��USB�豸
 */
void dump_usbinfo(PTP_USB *ptp_usb)
{
    struct mtp_usb_device *dev;

#ifdef LIBUSB_HAS_GET_DRIVER_NP
    char devname[0x10];
    int res;

    devname[0] = '\0';
    res = usb_get_driver_np(ptp_usb->handle, (int) ptp_usb->interface, devname, sizeof(devname));
    if (devname[0] != '\0') {
        LIBMTP_INFO("   Using kernel interface \"%s\"\n", devname);
    }
#endif
    dev = ptp_usb->usb_dev;
    LIBMTP_INFO("   bcdUSB: %d\r\n", dev->p_desc->bcdUSB);
    LIBMTP_INFO("   bDeviceClass: %d\r\n", dev->p_desc->bDeviceClass);
    LIBMTP_INFO("   bDeviceSubClass: %d\r\n", dev->p_desc->bDeviceSubClass);
    LIBMTP_INFO("   bDeviceProtocol: %d\r\n", dev->p_desc->bDeviceProtocol);
    LIBMTP_INFO("   idVendor: %04x\r\n", dev->p_desc->idVendor);
    LIBMTP_INFO("   idProduct: %04x\r\n", dev->p_desc->idProduct);
    LIBMTP_INFO("   IN endpoint maxpacket: %d bytes\r\n", ptp_usb->inep_maxpacket);
    LIBMTP_INFO("   OUT endpoint maxpacket: %d bytes\r\n", ptp_usb->outep_maxpacket);
    LIBMTP_INFO("   Raw device info:\r\n");
    LIBMTP_INFO("      Bus location: %d\r\n", ptp_usb->rawdevice.bus_location);
    LIBMTP_INFO("      Device number: %d\r\n", ptp_usb->rawdevice.devnum);
    LIBMTP_INFO("      Device entry info:\r\n");
    LIBMTP_INFO("         Vendor: %s\r\n", ptp_usb->rawdevice.device_entry.vendor);
    LIBMTP_INFO("         Vendor id: 0x%04x\r\n", ptp_usb->rawdevice.device_entry.vendor_id);
    LIBMTP_INFO("         Product: %s\r\n", ptp_usb->rawdevice.device_entry.product);
    LIBMTP_INFO("         Vendor id: 0x%04x\r\n", ptp_usb->rawdevice.device_entry.product_id);
    LIBMTP_INFO("         Device flags: 0x%08x\r\n", ptp_usb->rawdevice.device_entry.device_flags);
    (void) probe_device_descriptor(dev);
}

static void libusb_glue_debug (PTPParams *params, const char *format, ...)
{
    va_list args;

    va_start (args, format);
    if (params->debug_func!=NULL)
        params->debug_func (params->data, format, args);
    else
    {
        vfprintf (stderr, format, args);
        fprintf (stderr,"\n");
        fflush (stderr);
    }
    va_end (args);
}

static void libusb_glue_error (PTPParams *params, const char *format, ...)
{
    va_list args;

    va_start (args, format);
    if (params->error_func!=NULL)
        params->error_func (params->data, format, args);
    else
    {
        vfprintf (stderr, format, args);
        fprintf (stderr,"\n");
        fflush (stderr);
    }
    va_end (args);
}

/**
 * WMP/Windows��������ʹ���������㷨�����д��ʹ��䣺
 * 1.���ͷ���ָ��������(0x0C�ֽ�)�����򣬷��������sizeof(endpoint) - 0x0c
 * 2.����ʹ�÷ָ�ͷʱ�����͵�һ�����ݰ�ʱ����СΪsizeof(endpoint)������������3��
 * 3.�ظ�����0x10000�ֽڿ飬ֱ��ʣ���ֽ� < 0x10000�����ǳ�0x10000ΪCONTEXT_BLOCK_SIZE_1
 * 4.��ʣ����ֽ���sizeof(endpoint)����
 * 5.����ʣ���ֽڣ�����ոշ��͵��ֽ�Ϊsizeof(endpoint)����Ҫ����һ��0���Ȱ�
 *
 * ���⣬�����豸�е�USB�ӻ���������ȱ�ݣ�һЩ�豸�����޷����0���Ĳ�������˴����豸��0��������һЩ
 * ����ļ��ݡ�*/
#define CONTEXT_BLOCK_SIZE_1  0x3e00
#define CONTEXT_BLOCK_SIZE_2  0x200
#define CONTEXT_BLOCK_SIZE    CONTEXT_BLOCK_SIZE_1 + CONTEXT_BLOCK_SIZE_2

/**
 * PTPд����
 *
 * @param size Ҫ�������ݳ���
 * @param handler PTP���ݾ��
 * @param data PTP USB�ṹ��
 * @param readbytes ���ɹ������ݳ���
 * @param readzero �Ƿ��0��
 * @return �ɹ�����PTP_RC_OK
 */
static short ptp_read_func (unsigned long size, PTPDataHandler *handler,void *data,
                            unsigned long *readbytes, int readzero) {
    PTP_USB *ptp_usb = (PTP_USB *)data;
    unsigned long toread = 0;
    int result = 0;
    unsigned long curread = 0;
    unsigned char *bytes;
    int expect_terminator_byte = 0;
    unsigned long usb_inep_maxpacket_size;
    unsigned long context_block_size_1;
    unsigned long context_block_size_2;
    uint16_t ptp_dev_vendor_id = ptp_usb->rawdevice.device_entry.vendor_id;

    /* "iRiver" �豸����Ĵ���*/
    if (ptp_dev_vendor_id == 0x4102 || ptp_dev_vendor_id == 0x1006) {
        usb_inep_maxpacket_size = ptp_usb->inep_maxpacket;
        if (usb_inep_maxpacket_size == 0x400) {
            context_block_size_1 = CONTEXT_BLOCK_SIZE_1 - 0x200;
            context_block_size_2 = CONTEXT_BLOCK_SIZE_2 + 0x200;
        }
        else {
            context_block_size_1 = CONTEXT_BLOCK_SIZE_1;
            context_block_size_2 = CONTEXT_BLOCK_SIZE_2;
        }
    }
    /* �������Ŀ��С*/
    bytes = mtp_malloc(CONTEXT_BLOCK_SIZE);
    while (curread < size) {

        LIBMTP_USB_DEBUG_OFF("Remaining size to read: 0x%04lx bytes\n", size - curread);

        /* ��Ҫ�������ݷְ�*/
        if (size - curread < CONTEXT_BLOCK_SIZE)
        {
            /* ���һ��*/
            toread = size - curread;
            /* ��Ҫ��ȡ�����һ���ֽ���ָ���������*/
            if (readzero && FLAG_NO_ZERO_READS(ptp_usb) && (toread % ptp_usb->inep_maxpacket == 0)) {
                toread += 1;
                expect_terminator_byte = 1;
            }
        }
        else if (ptp_dev_vendor_id == 0x4102 || ptp_dev_vendor_id == 0x1006) {
            /* "iRiver" �豸���⴦��*/
            if (curread == 0)
                /* ��һ�������������һ����*/
                toread = context_block_size_1;
            else if (toread == context_block_size_1)
                toread = context_block_size_2;
            else if (toread == context_block_size_2)
                toread = context_block_size_1;
            else
                LIBMTP_INFO("unexpected toread size 0x%04x, 0x%04x remaining bytes\n",
                    (unsigned int) toread, (unsigned int) (size-curread));

        } else
            toread = CONTEXT_BLOCK_SIZE;

        LIBMTP_USB_DEBUG_OFF("Reading in 0x%04lx bytes\n", toread);
        /* USB����������*/
        result = awbl_usbh_trp_sync(ptp_usb->usb_dev->ep_in[ptp_usb->inep & 0x0f],
                                    NULL,
                                    (char*) bytes,
                                    toread,
                                    ptp_usb->timeout,
                                    0);

        LIBMTP_USB_DEBUG_OFF("Result of read: 0x%04x\n", result);

        if (result < 0) {
            return PTP_ERROR_IO;
        }

        LIBMTP_USB_DEBUG_OFF("<==USB IN\n");
        if (result == 0)
            LIBMTP_USB_DEBUG_OFF("Zero Read\n");
        else
            LIBMTP_USB_DATA(bytes, result, 16);

        /* ��ȥ�������һ���ֽ�*/
        if (expect_terminator_byte && result == toread)
        {
            LIBMTP_USB_DEBUG_OFF("<==USB IN\nDiscarding extra byte\n");

            result--;
        }
        /* �Ѷ����������ݷŵ�PTP���ݾ����*/
        int putfunc_ret = handler->putfunc(NULL, handler->priv, result, bytes);
        if (putfunc_ret != PTP_RC_OK)
          return putfunc_ret;
        /* ���µ�ǰ������ɵ�������*/
        ptp_usb->current_transfer_complete += result;
        curread += result;

        /* ���ûص�*/
        if (ptp_usb->callback_active) {
            if (ptp_usb->current_transfer_complete >= ptp_usb->current_transfer_total) {
                /* �رջص�*/
                ptp_usb->current_transfer_complete = ptp_usb->current_transfer_total;
                ptp_usb->callback_active = 0;
            }
            if (ptp_usb->current_transfer_callback != NULL) {
                int ret;
                ret = ptp_usb->current_transfer_callback(ptp_usb->current_transfer_complete,
                                                         ptp_usb->current_transfer_total,
                                                         ptp_usb->current_transfer_callback_data);
                if (ret != 0) {
                    return PTP_ERROR_CANCEL;
                }
            }
        }
        /* �����������ݱ�Ԥ�ڵ���*/
        if (result < toread)
            break;
    }
    /* �������ն��ɹ������ݳ���*/
    if (readbytes) *readbytes = curread;
    mtp_free (bytes);

    /* ��Ҫ��0��*/
    if (readzero && !FLAG_NO_ZERO_READS(ptp_usb) &&
        curread % ptp_usb->outep_maxpacket == 0) {
        char temp;
        int zeroresult = 0;

        LIBMTP_USB_DEBUG_OFF("<==USB IN\n");
        LIBMTP_USB_DEBUG_OFF("Zero Read\n");

        result = awbl_usbh_trp_sync(ptp_usb->usb_dev->ep_in[ptp_usb->inep & 0x0f],
                                    NULL,
                                    &temp,
                                    0,
                                    ptp_usb->timeout,
                                    0);


        if (zeroresult != 0)
            LIBMTP_INFO("LIBMTP panic: unable to read in zero packet, response 0x%04x", zeroresult);
    }
    return PTP_RC_OK;
}

/**
 * PTPд����
 *
 * @param size д���ݳ���
 * @param handler PTP���ݾ��
 * @param data PTP USB�ṹ��
 * @param written д�ɹ��ĳ���
 * @return �ɹ�����PTP_RC_OK
 */
static short ptp_write_func (unsigned long   size,
                             PTPDataHandler  *handler,
                             void            *data,
                             unsigned long   *written) {
    PTP_USB *ptp_usb = (PTP_USB *)data;
    unsigned long towrite = 0;
    int result = 0;
    unsigned long curwrite = 0;
    unsigned char *bytes;

    /* ����д�����ݿ����󳤶�*/
    bytes = mtp_malloc(CONTEXT_BLOCK_SIZE);
    if (!bytes) {
        return PTP_ERROR_IO;
    }
    while (curwrite < size) {
        unsigned long usbwritten = 0;
        towrite = size - curwrite;
        if (towrite > CONTEXT_BLOCK_SIZE) {
            towrite = CONTEXT_BLOCK_SIZE;
        } else {
            /* ʹ���ݰ��Ĵ�С��˵�������С���*/
            if (towrite > ptp_usb->outep_maxpacket && towrite % ptp_usb->outep_maxpacket != 0) {
                towrite -= towrite % ptp_usb->outep_maxpacket;
            }
        }
        /* ��ȡ����*/
        int getfunc_ret = handler->getfunc(NULL, handler->priv, towrite, bytes, &towrite);
        if (getfunc_ret != PTP_RC_OK)
            return getfunc_ret;
        while (usbwritten < towrite) {
            result = awbl_usbh_trp_sync(ptp_usb->usb_dev->ep_out[ptp_usb->outep],
                                        NULL,
                                        ((char*) bytes + usbwritten),
                                        towrite - usbwritten,
                                        ptp_usb->timeout,
                                        0);

            LIBMTP_USB_DEBUG_OFF("USB OUT==>\n");
            LIBMTP_USB_DATA(bytes + usbwritten, result, 16);

            if (result < 0) {
                return PTP_ERROR_IO;
            }

            ptp_usb->current_transfer_complete += result;
            curwrite += result;
            usbwritten += result;
        }
        /* ���ûص�*/
        if (ptp_usb->callback_active) {
            if (ptp_usb->current_transfer_complete >= ptp_usb->current_transfer_total) {
                /* �������µĸ��ºͽ��ܻص�*/
                ptp_usb->current_transfer_complete = ptp_usb->current_transfer_total;
                ptp_usb->callback_active = 0;
            }
            if (ptp_usb->current_transfer_callback != NULL) {
                int ret;
                ret = ptp_usb->current_transfer_callback(ptp_usb->current_transfer_complete,
                                                         ptp_usb->current_transfer_total,
                                                         ptp_usb->current_transfer_callback_data);
                if (ret != 0) {
                    return PTP_ERROR_CANCEL;
                }
            }
        }
        if (result < towrite) /* short writes happen */
            break;
    }
    mtp_free (bytes);
    if (written) {
        *written = curwrite;
    }
    /* ����������һ�δ��䣬�����Ҫ��д0*/
    if (ptp_usb->current_transfer_complete >= ptp_usb->current_transfer_total) {
        /* ����0���Ȱ�*/
        if ((towrite % ptp_usb->outep_maxpacket) == 0) {

            LIBMTP_USB_DEBUG("USB OUT==>\n");
            LIBMTP_USB_DEBUG("Zero Write\n");
            result = awbl_usbh_trp_sync(ptp_usb->usb_dev->ep_out[ptp_usb->outep],
                                        NULL,
                                        (char *) "x",
                                        0,
                                        ptp_usb->timeout,
                                        0);
        }
    }
    if (result < 0)
        return PTP_ERROR_IO;
    return PTP_RC_OK;
}

/* �ڴ����ݻ�ȡ/������*/
typedef struct {
    unsigned char *data;         /* ���ݻ���ָ��*/
    unsigned long  size;         /* ���ݳ���*/
    unsigned long  curoff;       /* ��ǰƫ��*/
} PTPMemHandlerPrivate;

/**
 * ���ڴ����ݾ�����ó�����
 *
 * @param params PTP�����ṹ��
 * @param private PTP�ڴ�˽�о��
 * @param wantlen ��Ҫ�ĳ���
 * @param data ��ȡ���ݵĻ���
 * @param gotlen ʵ�ʻ�õ����ݵĳ���
 * @return �ɹ�����PTP_RC_OK
 */
static u16 memory_getfunc(PTPParams* params, void* private,
                            unsigned long wantlen, unsigned char *data,
                            unsigned long *gotlen) {
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*) private;
    unsigned long tocopy = wantlen;

    if (priv->curoff + tocopy > priv->size)
        tocopy = priv->size - priv->curoff;
    /* �����ݸ��Ƶ�������ȥ*/
    memcpy(data, priv->data + priv->curoff, tocopy);
    /* ����ƫ��*/
    priv->curoff += tocopy;
    /* ʵ�ʸ������ݵĳ���*/
    *gotlen = tocopy;
    return PTP_RC_OK;
}

/**
 * �����ݷ����ڴ����ݾ����
 *
 * @param params PTP�����ṹ��
 * @param private PTP�ڴ�˽�о��
 * @param sendlen ��Ҫ�������ݵĳ���
 * @param data ��ȡ���ݵĻ���
 * @return �ɹ�����PTP_RC_OK
 */
static u16 memory_putfunc(PTPParams* params, void* private,
            unsigned long sendlen, unsigned char *data) {
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*) private;

    /* ��鵱ǰƫ�Ƽ���Ҫ��������ݳ����Ƿ������������*/
    if (priv->curoff + sendlen > priv->size) {
        /* ���·����ڴ�*/
        priv->data = mtp_realloc(priv->data, priv->curoff + sendlen);
        priv->size = priv->curoff + sendlen;
    }
    /* ��������*/
    memcpy(priv->data + priv->curoff, data, sendlen);
    priv->curoff += sendlen;
    return PTP_RC_OK;
}

/* ��ʼ���������ݵ�˽�о�� */
static u16 ptp_init_recv_memory_handler(PTPDataHandler *handler) {
    PTPMemHandlerPrivate* priv;
    priv = mtp_malloc (sizeof(PTPMemHandlerPrivate));
    handler->priv = priv;
    handler->getfunc = memory_getfunc;
    handler->putfunc = memory_putfunc;
    priv->data = NULL;
    priv->size = 0;
    priv->curoff = 0;
    return PTP_RC_OK;
}

/* ��ʼ���������ݵ�˽�о�� */
static u16 ptp_init_send_memory_handler(PTPDataHandler *handler,
                    unsigned char *data, unsigned long len) {
    PTPMemHandlerPrivate* priv;
    priv = mtp_malloc (sizeof(PTPMemHandlerPrivate));
    if (!priv)
        return PTP_RC_GeneralError;
    handler->priv = priv;
    handler->getfunc = memory_getfunc;
    handler->putfunc = memory_putfunc;
    priv->data = data;
    priv->size = len;
    priv->curoff = 0;
    return PTP_RC_OK;
}

/* �ͷŷ������ݾ��*/
static u16 ptp_exit_send_memory_handler (PTPDataHandler *handler) {
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*)handler->priv;
    mtp_free (priv);
    return PTP_RC_OK;
}

/* ��ȡ�������ݾ��������ݲ��ͷ����ݾ��*/
static u16 ptp_exit_recv_memory_handler (PTPDataHandler *handler,
    unsigned char **data, unsigned long *size) {
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*)handler->priv;
    *data = priv->data;
    *size = priv->size;
    mtp_free (priv);
    return PTP_RC_OK;
}

/**
 * PTP����������
 *
 * @param params PTP�����ṹ��
 * @param req PTP��������
 * @param dataphase ���ݽ׶�
 * @return �ɹ�����PTP_RC_OK
 */
u16 ptp_usb_sendreq (PTPParams* params, PTPContainer* req, int dataphase){
    u16 ret;
    PTPUSBBulkContainer usbreq;
    PTPDataHandler  memhandler;
    unsigned long written = 0;
    unsigned long towrite;

    LIBMTP_USB_DEBUG_OFF("REQUEST: 0x%04x, %s\n", req->Code, ptp_get_opcode_name(params, req->Code));

    /* �����ʵ���USB���� */
    usbreq.length   = AW_USB_CPU_TO_LE32(PTP_USB_BULK_REQ_LEN - (sizeof(u32)*(5-req->Nparam)));
    usbreq.type     = AW_USB_CPU_TO_LE16(PTP_USB_CONTAINER_COMMAND);
    usbreq.code     = AW_USB_CPU_TO_LE16(req->Code);
    usbreq.trans_id = AW_USB_CPU_TO_LE32(req->Transaction_ID);
    usbreq.payload.params.param1 = AW_USB_CPU_TO_LE32(req->Param1);
    usbreq.payload.params.param2 = AW_USB_CPU_TO_LE32(req->Param2);
    usbreq.payload.params.param3 = AW_USB_CPU_TO_LE32(req->Param3);
    usbreq.payload.params.param4 = AW_USB_CPU_TO_LE32(req->Param4);
    usbreq.payload.params.param5 = AW_USB_CPU_TO_LE32(req->Param5);
    /* ���͵���Ӧ�� */
    /* ���ݲ��������������󳤶�*/
    towrite = PTP_USB_BULK_REQ_LEN - (sizeof(u32)*(5 - req->Nparam));
    /* ��ʼ�������ڴ���*/
    ptp_init_send_memory_handler (&memhandler, (unsigned char*)&usbreq, towrite);
    /* PTPд*/
    ret = ptp_write_func(towrite, &memhandler, params->data, &written);
    /* �ͷŷ����ڴ���*/
    ptp_exit_send_memory_handler (&memhandler);
    if (ret != PTP_RC_OK && ret != PTP_ERROR_CANCEL) {
        ret = PTP_ERROR_IO;
    }
    if (written != towrite && ret != PTP_ERROR_CANCEL && ret != PTP_ERROR_IO) {
        libusb_glue_error (params,
            "PTP: request code 0x%04x sending req wrote only %ld bytes instead of %d",
            req->Code, written, towrite
        );
        ret = PTP_ERROR_IO;
    }
    return ret;
}

#ifndef ULONG_MAX
#define ULONG_MAX 0xFFFFFFFF
#endif

/**
 * PTP�������ݺ���
 *
 * @param params  PTP�����ṹ��
 * @param ptp     PTP��������
 * @param size    Ҫ�������ݵĴ�С
 * @param handler PTP���ݾ��
 *
 * @return �ɹ�����PTP_RC_OK
 */
u16 ptp_usb_senddata (PTPParams* params, PTPContainer* ptp,
                      u64 size, PTPDataHandler *handler) {
    u16 ret;
    int wlen, datawlen;
    unsigned long written;
    PTPUSBBulkContainer usbdata;
    u64 bytes_left_to_transfer;
    PTPDataHandler memhandler;
    unsigned long packet_size;
    PTP_USB *ptp_usb = (PTP_USB *) params->data;

    packet_size = ptp_usb->outep_maxpacket;


    LIBMTP_USB_DEBUG_OFF("SEND DATA PHASE\n");

    /* �������ʵ�USB����*/
    usbdata.length  = AW_USB_CPU_TO_LE32(PTP_USB_BULK_HDR_LEN + size);
    usbdata.type    = AW_USB_CPU_TO_LE16(PTP_USB_CONTAINER_DATA);
    usbdata.code    = AW_USB_CPU_TO_LE16(ptp->Code);
    usbdata.trans_id= AW_USB_CPU_TO_LE32(ptp->Transaction_ID);

    ((PTP_USB*)params->data)->current_transfer_complete = 0;
    ((PTP_USB*)params->data)->current_transfer_total = size + PTP_USB_BULK_HDR_LEN;

    if (params->split_header_data) {
        datawlen = 0;
        wlen = PTP_USB_BULK_HDR_LEN;
    } else {
        unsigned long gotlen;
        /* ��������ͷ�豸*/
        /* ������ݳ���*/
        datawlen = (size < PTP_USB_BULK_PAYLOAD_LEN_WRITE)?size:PTP_USB_BULK_PAYLOAD_LEN_WRITE;
        wlen = PTP_USB_BULK_HDR_LEN + datawlen;

        ret = handler->getfunc(params, handler->priv, datawlen, usbdata.payload.data, &gotlen);
        if (ret != PTP_RC_OK)
            return ret;
        if (gotlen != datawlen)
            return PTP_RC_GeneralError;
    }
    /* ��ʼ�������ڴ���*/
    ptp_init_send_memory_handler (&memhandler, (unsigned char *)&usbdata, wlen);
    /* ���͵�һ��������*/
    ret = ptp_write_func(wlen, &memhandler, params->data, &written);
    /* �ͷ��ڴ���*/
    ptp_exit_send_memory_handler (&memhandler);
    if (ret != PTP_RC_OK) {
        return ret;
    }
    if (size <= datawlen)
        return ret;
    /* û��������ʣ������*/
    bytes_left_to_transfer = size - datawlen;
    ret = PTP_RC_OK;
    while(bytes_left_to_transfer > 0) {
        unsigned long max_long_transfer = ULONG_MAX + 1 - packet_size;
        ret = ptp_write_func (bytes_left_to_transfer > max_long_transfer ? max_long_transfer : bytes_left_to_transfer,
            handler, params->data, &written);
        if (ret != PTP_RC_OK)
            break;
        if (written == 0) {
            ret = PTP_ERROR_IO;
            break;
        }
        bytes_left_to_transfer -= written;
    }
    if (ret != PTP_RC_OK && ret != PTP_ERROR_CANCEL)
        ret = PTP_ERROR_IO;
    return ret;
}

/**
 *  PTP USB��ȡ��
 *
 *  @param params PTP�����ṹ��
 *  @param packet PTP USB��������
 *  @param rlen ��ȡ���İ��ĳ���
 *  @return �ɹ�����PTP_RC_OK
 */
static u16 ptp_usb_getpacket(PTPParams *params,
        PTPUSBBulkContainer *packet, unsigned long *rlen){
    PTPDataHandler  memhandler;
    u16    ret;
    unsigned char   *x = NULL;
    unsigned long packet_size;
    PTP_USB *ptp_usb = (PTP_USB *) params->data;
    /* ��ȡ����˵��������С*/
    packet_size = ptp_usb->inep_maxpacket;

    /* �Ѿ�������Ӧ����ʹ�������Ӧ��(�����)*/
    if (params->response_packet_size > 0) {
        memcpy(packet, params->response_packet, params->response_packet_size);
        *rlen = params->response_packet_size;
        mtp_free(params->response_packet);
        params->response_packet = NULL;
        params->response_packet_size = 0;
        return PTP_RC_OK;
    }
    /* ��ʼ�������ڴ���*/
    ptp_init_recv_memory_handler (&memhandler);
    /* PTP��*/
    ret = ptp_read_func(packet_size, &memhandler, params->data, rlen, 0);
    /* ��ȡ���ݺ����ݳ��ȣ��ͷŽ����ڴ���*/
    ptp_exit_recv_memory_handler (&memhandler, &x, rlen);
    if (x) {
        /* ��������*/
        memcpy (packet, x, *rlen);
        mtp_free (x);
    }
    return ret;
}

/**
 * PTP��ȡ���ݺ���
 *
 * @param params PTP�����ṹ��
 * @param req PTP��������
 * @param handler ���ݾ��
 * @return �ɹ�����PTP_RC_OK
 */
u16 ptp_usb_getdata (PTPParams* params, PTPContainer* ptp, PTPDataHandler *handler){
    u16 ret;
    PTPUSBBulkContainer usbdata;
    PTP_USB *ptp_usb = (PTP_USB *) params->data;
    int putfunc_ret;

    LIBMTP_USB_DEBUG_OFF("GET DATA PHASE\n");

    memset(&usbdata, 0, sizeof(usbdata));
    do {
        unsigned long len, rlen;
        /* ��ȡ"��ȡ�豸��Ϣ"�����ݣ���ȡ�˵�������С������*/
        ret = ptp_usb_getpacket(params, &usbdata, &rlen);
        if (ret != PTP_RC_OK) {
            ret = PTP_ERROR_IO;
            break;
        }
        /* ����ǲ�����������*/
        if (AW_USB_CPU_TO_LE16(usbdata.type) != PTP_USB_CONTAINER_DATA) {
            ret = PTP_ERROR_DATA_EXPECTED;
            break;
        }
        /* �¼����벻һ��*/
        if (AW_USB_CPU_TO_LE16(usbdata.code) != ptp->Code) {
            /* �Ƿ���Ի�ͷ*/
            if (FLAG_IGNORE_HEADER_ERRORS(ptp_usb)) {
                libusb_glue_debug (params, "ptp2/ptp_usb_getdata: detected a broken "
                       "PTP header, code field insane, expect problems! (But continuing)");
                /* �޸�ͷ����Ϣ��ͨ����������ᱻ�ƻ�*/
                usbdata.code     = AW_USB_CPU_TO_LE16(ptp->Code);
                usbdata.trans_id = AW_USB_CPU_TO_LE32(ptp->Transaction_ID);
                ret = PTP_RC_OK;
            } else {
                ret = AW_USB_CPU_TO_LE16(usbdata.code);
                /* ���������ȫû�õķ��ش��룬����Ȼ�����ڻ�ȡ����ʱ����code�ֶ��еĴ�����롣Windows�ƺ���ȫ����
                 * ������ֶε�����*/
                if (ret < PTP_RC_Undefined || ret > PTP_RC_SpecificationOfDestinationUnsupported) {
                    libusb_glue_debug (params, "ptp2/ptp_usb_getdata: detected a broken "
                           "PTP header, code field insane.");
                    ret = PTP_ERROR_IO;
                }
                break;
            }
        }
        /* ���ȸպ��Ƕ˵�������С��˵����������*/
        if (rlen == ptp_usb->inep_maxpacket) {
            /* ������һ���ֵ����ݵ�PTP���ݾ���� */
            putfunc_ret = handler->putfunc(params, handler->priv,
                            rlen - PTP_USB_BULK_HDR_LEN, usbdata.payload.data);
            if (putfunc_ret != PTP_RC_OK)
                return putfunc_ret;

            /* ������ֱ����䵽��������ݾ�� */
            while (1) {
                unsigned long readdata;
                u16 xret;

                xret = ptp_read_func(0x20000000, handler,
                                    params->data,
                                    &readdata,
                                    0);
                if (xret != PTP_RC_OK)
                    return xret;
                if (readdata < 0x20000000)
                    break;
            }
            return PTP_RC_OK;
        }
        /* ��ȡ�������ݵĳ��ȴ���Ҫ����ĳ���*/
        if (rlen > AW_USB_CPU_TO_LE32(usbdata.length)) {
            /*
             * ����������Ӧ����������ĳ��ȴ���PTP_USB_BULK_HDR_LEN(����������һ��
             * �����İ�)����������Ϊ�����ʣ�����ݶ�������ʹ��ͬһ�����г����������ϵİ�����
             * Ҳ�������ã�����ֻ�ᱻ��������
             * iRiver�豸��һЩɢ���ֽڣ���Щ�ֽ��Ա�����*/
            unsigned int packlen = AW_USB_CPU_TO_LE32(usbdata.length);
            unsigned int surplen = rlen - packlen;

            if (surplen >= PTP_USB_BULK_HDR_LEN) {
                params->response_packet = mtp_malloc(surplen);
                memcpy(params->response_packet, (u8 *) &usbdata + packlen, surplen);
                params->response_packet_size = surplen;
            /* ���Զ�ȡ�����һ���ֽ� */
            } else if(!FLAG_NO_ZERO_READS(ptp_usb) &&
                  (rlen - AW_USB_CPU_TO_LE32(usbdata.length) == 1)) {
                libusb_glue_debug (params, "ptp2/ptp_usb_getdata: read %d bytes "
                     "too much, expect problems!",
                     rlen - AW_USB_CPU_TO_LE32(usbdata.length));
            }
            rlen = packlen;
        }
        /* ���ڴ����PTP�豸������rlen��512 == sizeof(usbdata)������MTP�豸�ķָ�ͷ�����ݿ�����12*/
        /* �����������ݵĳ��� */
        len = AW_USB_CPU_TO_LE32(usbdata.length) - PTP_USB_BULK_HDR_LEN;

        /* �Զ����ָ�ͷ/���� MTP�豸 */
        if (AW_USB_CPU_TO_LE32(usbdata.length) > 12 && (rlen == 12))
            params->split_header_data = 1;
        /* ������һ���ֵ����ݵ�PTP���ݾ���� */
        putfunc_ret = handler->putfunc(params, handler->priv, rlen - PTP_USB_BULK_HDR_LEN,
                           usbdata.payload.data);
        if (putfunc_ret != PTP_RC_OK)
            return putfunc_ret;

        if (FLAG_NO_ZERO_READS(ptp_usb) &&
            len + PTP_USB_BULK_HDR_LEN == ptp_usb->inep_maxpacket) {

            LIBMTP_USB_DEBUG_OFF("Reading in extra terminating byte\n");

            /* ��Ҫ��ȡ������ֽڲ�������*/
            int result = 0;
            char byte = 0;

            result = awbl_usbh_trp_sync(ptp_usb->usb_dev->ep_in[ptp_usb->inep & 0x0F],
                                        NULL,
                                        &byte,
                                        1,
                                        ptp_usb->timeout,
                                        0);

          if (result != 1)
              LIBMTP_INFO("Could not read in extra byte for %d bytes long file, return value 0x%04x\n", ptp_usb->inep_maxpacket, result);
        } else if (len + PTP_USB_BULK_HDR_LEN == ptp_usb->inep_maxpacket && params->split_header_data == 0) {
            int zeroresult = 0;
            char zerobyte = 0;


            LIBMTP_INFO("Reading in zero packet after header\n");

            zeroresult = awbl_usbh_trp_sync(ptp_usb->usb_dev->ep_in[ptp_usb->inep & 0x0F],
                                        NULL,
                                        &zerobyte,
                                        1,
                                        ptp_usb->timeout,
                                        0);

            if (zeroresult != 0)
              LIBMTP_INFO("LIBMTP panic: unable to read in zero packet, response 0x%04x", zeroresult);

        }
        /* �������ݣ�*/
        if (len + PTP_USB_BULK_HDR_LEN <= rlen) {
            break;
        }

        ret = ptp_read_func(len - (rlen - PTP_USB_BULK_HDR_LEN),
                                handler, params->data, &rlen, 1);

        if (ret != PTP_RC_OK) {
            break;
        }

    } while (0);
    return ret;
}

/**
 * PTP��ȡ��Ӧ
 *
 * @param params PTP�����ṹ��
 * @param resp PTP��������
 * @return �ɹ�����PTP_RC_OK
 */
u16 ptp_usb_getresp (PTPParams* params, PTPContainer* resp){
    u16 ret;
    unsigned long rlen;
    PTPUSBBulkContainer usbresp;
    PTP_USB *ptp_usb = (PTP_USB *)(params->data);


    LIBMTP_USB_DEBUG_OFF("RESPONSE: \r\n");

    memset(&usbresp, 0, sizeof(usbresp));
    /* ��ȡ��Ӧ, ��Ӧ�ñ�sizeof(usbresp)���� */
    ret = ptp_usb_getpacket(params, &usbresp, &rlen);
    /* ���²�����Ϊ����Samsung YP-U3�豸��������豸�ڷ�����Ӧ��ʱ�����һ���հ�֮����һ��
     * ֻ��������Ϊ2��0���ݵİ�(�����Կ���4��)*/
    while (ret == PTP_RC_OK && rlen < PTP_USB_BULK_HDR_LEN && usbresp.length == 0) {
        libusb_glue_debug (params, "ptp_usb_getresp: detected short response "
                            "of %d bytes, expect problems! (re-reading "
                            "response), rlen");
        ret = ptp_usb_getpacket(params, &usbresp, &rlen);
    }

    if (ret != PTP_RC_OK) {
        ret = PTP_ERROR_IO;
    } else if (AW_USB_CPU_TO_LE16(usbresp.type) != PTP_USB_CONTAINER_RESPONSE) {
        /* ���ص���Ӧ���������ͺ������Ĳ�һ��*/
        ret = PTP_ERROR_RESP_EXPECTED;
    } else if (AW_USB_CPU_TO_LE16(usbresp.code) != resp->Code) {
        ret = AW_USB_CPU_TO_LE16(usbresp.code);
    }

    LIBMTP_USB_DEBUG_OFF("%04x\n", ret);

    if (ret!=PTP_RC_OK) {
/*      libusb_glue_error (params,
        "PTP: request code 0x%04x getting resp error 0x%04x",
            resp->Code, ret);*/
        return ret;
    }
    /* ����һ��PTP���� */
    resp->Code = AW_USB_CPU_TO_LE16(usbresp.code);
    resp->SessionID = params->session_id;
    resp->Transaction_ID = AW_USB_CPU_TO_LE32(usbresp.trans_id);
    if (FLAG_IGNORE_HEADER_ERRORS(ptp_usb)) {
        if (resp->Transaction_ID != params->transaction_id - 1) {
            libusb_glue_debug (params, "ptp_usb_getresp: detected a broken "
                   "PTP header, transaction ID insane, expect "
                   "problems! (But continuing)");
            /* �޸�ͷ��*/
            resp->Transaction_ID = params->transaction_id - 1;
        }
    }
    resp->Param1 = AW_USB_CPU_TO_LE32(usbresp.payload.params.param1);
    resp->Param2 = AW_USB_CPU_TO_LE32(usbresp.payload.params.param2);
    resp->Param3 = AW_USB_CPU_TO_LE32(usbresp.payload.params.param3);
    resp->Param4 = AW_USB_CPU_TO_LE32(usbresp.payload.params.param4);
    resp->Param5 = AW_USB_CPU_TO_LE32(usbresp.payload.params.param5);
    return ret;
}

/**
 * PTPȡ����������
 *
 * @param params        PTP�����ṹ��
 * @param transactionid Ҫȡ��������ID
 *
 * @return �ɹ�����PTP_RC_OK
 */
u16 ptp_usb_control_cancel_request (PTPParams *params, u32 transactionid) {
    PTP_USB *ptp_usb = (PTP_USB *)(params->data);
    int ret;
    unsigned char buffer[6];
    struct mtp_usb_device* udev = ptp_usb->usb_dev;

    htod16a(&buffer[0], PTP_EC_CancelTransaction);
    htod32a(&buffer[2], transactionid);
    ret = awbl_usbh_ctrl_sync(&udev->ep0,
                              AW_USB_REQ_TAG_INTERFACE | AW_USB_REQ_TYPE_CLASS,
                              0xaa,
                              0x00,
                              0x04,
                              sizeof(buffer),
                              (char *) buffer,
                              ptp_usb->timeout, 0);
    if (ret < sizeof(buffer))
        return PTP_ERROR_IO;
    return PTP_RC_OK;
}


/**
 * ��ʼ��һ��PTP USB�ṹ��
 *
 * @param params PTP�����ṹ��
 * @param ptp_usb Ҫ��ʼ����PTP USB�ṹ��
 * @param dev USB�豸�ṹ��
 * @return �ɹ�����0
 */
static int init_ptp_usb(PTPParams* params, PTP_USB* ptp_usb, struct mtp_usb_device* dev){
    char buf[255];

    params->sendreq_func   = ptp_usb_sendreq;
    params->senddata_func  = ptp_usb_senddata;
    params->getresp_func   = ptp_usb_getresp;
    params->getdata_func   = ptp_usb_getdata;
    params->cancelreq_func = ptp_usb_control_cancel_request;
    params->data           = ptp_usb;
    params->transaction_id = 0;
    /*
     * This is hardcoded here since we have no devices whatsoever that are BE.
     * Change this the day we run into our first BE device (if ever).
     */
    /* �ֽ���*/
    params->byteorder = PTP_DL_LE;
    /* USB��ʱʱ��*/
    ptp_usb->timeout = get_timeout(ptp_usb);

    ptp_usb->usb_dev = dev;

    /*
     * ��������Ƿ�����Ϊ���ǲ���ʹ�õ��������ݡ�ֻ���ھ��Ա��������²����������á�
     */
    if (dev->cfg.p_desc->bConfigurationValue != ptp_usb->config) {
        if (awbl_usbh_dev_cfg_set(dev, dev->cfg.p_desc->bConfigurationValue) != AW_OK) {
            mtp_printf("error in usb set configuration - continuing anyway\r\n");
        }
    }

    if (FLAG_SWITCH_MODE_BLACKBERRY(ptp_usb)) {
        int ret;
        //FIXME : Only for BlackBerry Storm
        /* ʲô��˼���������л�ģʽ...
         * ��һ��������Ϣ�Ǿ��Ա�Ҫ��*/
        mtp_usleep(1000);
        ret = awbl_usbh_ctrl_sync(&dev->ep0,
                                  AW_USB_DIR_IN | AW_USB_REQ_TAG_DEVICE | AW_USB_REQ_TYPE_VENDOR,
                                  0xaa,
                                  0x00,
                                  0x04,
                                  0x40,
                                  buf,
                                  1000, 0);
        LIBMTP_USB_DEBUG("BlackBerry magic part 1:\n");
        LIBMTP_USB_DATA(buf, ret, 16);

        mtp_usleep(1000);
        /* ���������Ϣ�ķǱ�Ҫ��*/
        ret = awbl_usbh_ctrl_sync(&dev->ep0,
                                  AW_USB_DIR_IN | AW_USB_REQ_TAG_DEVICE | AW_USB_REQ_TYPE_VENDOR,
                                  0xa5,
                                  0x00,
                                  0x01,
                                  0x02,
                                  buf,
                                  1000, 0);
        LIBMTP_USB_DEBUG("BlackBerry magic part 2:\n");
        LIBMTP_USB_DATA(buf, ret, 16);
        mtp_usleep(1000);
        /* ���������Ϣ�ķǱ�Ҫ��*/
        ret = awbl_usbh_ctrl_sync(&dev->ep0,
                                  AW_USB_DIR_IN | AW_USB_REQ_TAG_DEVICE | AW_USB_REQ_TYPE_VENDOR,
                                  0xa8,
                                  0x00,
                                  0x01,
                                  0x05,
                                  buf,
                                  1000, 0);
        LIBMTP_USB_DEBUG("BlackBerry magic part 3:\n");
        LIBMTP_USB_DATA(buf, ret, 16);

        mtp_usleep(1000);
        /* ���������Ϣ�ķǱ�Ҫ��*/
        ret = awbl_usbh_ctrl_sync(&dev->ep0,
                                  AW_USB_DIR_IN | AW_USB_REQ_TAG_DEVICE | AW_USB_REQ_TYPE_VENDOR,
                                  0xa8,
                                  0x00,
                                  0x01,
                                  0x11,
                                  buf,
                                  1000, 0);
        LIBMTP_USB_DEBUG("BlackBerry magic part 4:\n");
        LIBMTP_USB_DATA(buf, ret, 16);

        mtp_usleep(1000);
    }
    return 0;
}

/* ����˵�ֹͣ״̬*/
static void clear_stall(PTP_USB* ptp_usb)
{
    u16 status;
    int ret;

    /* ����ж϶˵�״̬ */
    status = 0;
    ret = usb_get_endpoint_status(ptp_usb, ptp_usb->inep, &status);
    if (ret < 0) {
        perror ("inep: usb_get_endpoint_status()");
    } else if (status) {
        LIBMTP_INFO("Clearing stall on IN endpoint\n");
        ret = usb_clear_stall_feature(ptp_usb,ptp_usb->inep);
        if (ret < 0) {
            perror ("usb_clear_stall_feature()");
        }
    }

    /* ������==����˵�״̬ */
    status = 0;
    ret = usb_get_endpoint_status(ptp_usb,ptp_usb->outep,&status);
    if (ret < 0) {
        perror("outep: usb_get_endpoint_status()");
    } else if (status) {
        LIBMTP_INFO("Clearing stall on OUT endpoint\n");
        ret = usb_clear_stall_feature(ptp_usb,ptp_usb->outep);
        if (ret < 0) {
            perror("usb_clear_stall_feature()");
        }
    }

  /* TODO: do we need this for INTERRUPT (ptp_usb->intep) too? */
}

/**
 * �ر�USB
 *
 * @param params PTP�����ṹ��
 */
static void close_usb(PTP_USB* ptp_usb)
{
    if (!FLAG_NO_RELEASE_INTERFACE(ptp_usb)) {
        /*
         * Clear any stalled endpoints
         * On misbehaving devices designed for Windows/Mac, quote from:
         * http://www2.one-eyed-alien.net/~mdharm/linux-usb/target_offenses.txt
         * Device does Bad Things(tm) when it gets a GET_STATUS after CLEAR_HALT
         * (...) Windows, when clearing a stall, only sends the CLEAR_HALT command,
         * and presumes that the stall has cleared.  Some devices actually choke
         * if the CLEAR_HALT is followed by a GET_STATUS (used to determine if the
         * STALL is persistant or not).
         */
        clear_stall(ptp_usb);
#if 0
    // causes troubles due to a kernel bug in 3.x kernels before/around 3.8
    // Clear halts on any endpoints
    clear_halt(ptp_usb);
    // Added to clear some stuff on the OUT endpoint
    // TODO: is this good on the Mac too?
    // HINT: some devices may need that you comment these two out too.
#endif
        /* AWorkִ�����²�����������*/
        //awbl_usbh_endpoint_reset(ptp_usb->usb_dev, ptp_usb->outep);
    }
    if (FLAG_FORCE_RESET_ON_CLOSE(ptp_usb)) {
        /* ��Щ�豸���ϲ���ڶϿ����Ӻ�λ��ͬ��������Windows�Ӳ��Ͽ��豸�������
         * ���ʱ���ٻ���δ���豸��ִ���豸�ر���Ϊ�������������׳���
         * ��λ�����а���*/
        //usb_reset(ptp_usb->handle);
    }
}


/**
 * ��ȡ����MTP���豸�ӿںͶ˵�
 *
 * @param dev USB�豸�ṹ��
 * @param conf USB�豸�����ñ��
 * @param interface USB�豸���ýӿڱ��
 * @param altsetting USB�豸���ýӿڱ�ű������ú�
 * @param inep ����˵��
 * @param inep_maxpacket ����˵�������С
 * @param outep ����˵�
 * @param outep_maxpacket ����˵�������С
 * @param intep �ж϶˵��
 * @return �ɹ�����0
 */
static int find_interface_and_endpoints(struct mtp_usb_device *dev,
                                        u8 *conf,
                                        u8 *interface,
                                        u8 *altsetting,
                                        int* inep,
                                        int* inep_maxpacket,
                                        int* outep,
                                        int *outep_maxpacket,
                                        int* intep)
{
    u8 i;
    aw_err_t ret;
    struct awbl_usbh_interface *itf, *itf_temp;

    /* �����豸����*/
    for (i = 0; i < dev->p_desc->bNumConfigurations; i++) {
        /* ��������*/
        if(i > 0){
            ret = awbl_usbh_dev_cfg_set(dev, i + 1);
            if(ret != AW_OK){
                return -1;
            }
        }
        uint8_t j;
        *conf = dev->cfg.p_desc->bConfigurationValue;
        /* �������ýӿ�*/
        for (j = 0; j < dev->cfg.nfuns; j++) {
            u8 l;
            u8 no_ep;
            int found_inep = 0;
            int found_outep = 0;
            int found_intep = 0;
            struct awbl_usbh_endpoint *ep;
            /* �����ӿںͱ��ýӿ�*/
            aw_list_for_each_entry_safe(itf,
                                        itf_temp,
                                        &dev->cfg.funs[j].itfs,
                                        struct awbl_usbh_interface,
                                        node){
                /* MTP�豸Ӧ����3���˵㣬������Щû�еĽӿ�*/
                no_ep = itf->p_desc->bNumEndpoints;
                if (no_ep != 3)
                    continue;
                *interface = itf->p_desc->bInterfaceNumber;
                *altsetting = itf->p_desc->bAlternateSetting;
                ep = itf->eps;
                /* �������˵���ѭ���Զ�λ���������˵��һ���ж϶˵㣬������ܣ���ʧ�ܣ�Ȼ�����*/
                for (l = 0; l < no_ep; l++) {
                    if (ep[l].p_desc->bmAttributes == AW_USB_EP_TYPE_BULK) {
                        if ((ep[l].p_desc->bEndpointAddress & AW_USB_DIR_IN) ==
                                AW_USB_DIR_IN) {
                            *inep = ep[l].p_desc->bEndpointAddress;
                            *inep_maxpacket = ep[l].p_desc->wMaxPacketSize;
                            found_inep = 1;
                        }
                        if ((ep[l].p_desc->bEndpointAddress & AW_USB_DIR_IN) == 0) {
                            *outep = ep[l].p_desc->bEndpointAddress;
                            *outep_maxpacket = ep[l].p_desc->wMaxPacketSize;
                            found_outep = 1;
                        }
                    }else if (ep[l].p_desc->bmAttributes == AW_USB_EP_TYPE_INT) {
                        if ((ep[l].p_desc->bEndpointAddress & AW_USB_DIR_IN) ==
                                AW_USB_DIR_IN) {
                            *intep = ep[l].p_desc->bEndpointAddress;
                            found_intep = 1;
                        }
                    }
                    /* �ҵ��˵㣬���������ﷵ��*/
                    if (found_inep && found_outep && found_intep)
                      return 0;
                }
            }
        }
    }
    return -1;
}

/**
 * �˺�����������usbinfo�����һ��ԭʼ�豸��Ϊ����
 *
 * @param device  Ҫ������豸
 * @param usbinfo һ��ָ���µ�usbinfo��ָ��
 * @return һ���������
 */
LIBMTP_error_number_t configure_usb_device(LIBMTP_raw_device_t *device,
                       PTPParams *params, void **usbinfo){
    PTP_USB *ptp_usb;
    struct mtp_usb_device *libusb_device;
    u16 ret = 0;
    int found = 0;
    int err;
    struct mtp_usb_device *dev, *dev_temp;

    /* ����USB�����ϵ��豸�������Ƿ����ٴ��ҵ����ԭʼ�豸*/
    aw_list_for_each_entry_safe(dev, dev_temp, &g_udev_list, struct mtp_usb_device, node){
        if((dev->addr == device->devnum) && (dev->p_desc->idVendor == device->device_entry.vendor_id)
                && (dev->p_desc->idProduct == device->device_entry.product_id)){
            libusb_device = dev;
            found = 1;
            break;
        }
    }
    /* �豸��ʧ�� */
    if (!found) {
      return LIBMTP_ERROR_NO_DEVICE_ATTACHED;
    }
    /* ����ṹ��*/
    ptp_usb = (PTP_USB *) mtp_malloc(sizeof(PTP_USB));
    if (ptp_usb == NULL) {
        return LIBMTP_ERROR_MEMORY_ALLOCATION;
    }
    /* ��0�ռ� (�����豸 device_flagsΪ0) */
    memset(ptp_usb, 0, sizeof(PTP_USB));
    /* ��ֵԭʼ�豸 */
    memcpy(&ptp_usb->rawdevice, device, sizeof(LIBMTP_raw_device_t));
    /*
     * һЩ�豸��Ҫʼ�ռ���䡰OS��������������������
     */
    if (FLAG_ALWAYS_PROBE_DESCRIPTOR(ptp_usb)) {
      /* ����豸������*/
      (void) probe_device_descriptor(libusb_device);
    }
    /* Ѱ�ҷ���Ҫ��ĽӿںͶ˵� */
    err = find_interface_and_endpoints(libusb_device,
                                       &ptp_usb->config,
                                       &ptp_usb->interface,
                                       &ptp_usb->altsetting,
                                       &ptp_usb->inep,
                                       &ptp_usb->inep_maxpacket,
                                       &ptp_usb->outep,
                                       &ptp_usb->outep_maxpacket,
                                       &ptp_usb->intep);

    if (err) {
        LIBMTP_ERROR("LIBMTP PANIC: Unable to find interface & endpoints of device\n");
        return LIBMTP_ERROR_CONNECTING;
    }
    /* ����USB�汾�� */
    ptp_usb->bcdusb = libusb_device->p_desc->bcdUSB;

    /* ��ʼ���豸 */
    if (init_ptp_usb(params, ptp_usb, libusb_device) < 0) {
        LIBMTP_ERROR("LIBMTP PANIC: Unable to initialize device\n");
        return LIBMTP_ERROR_CONNECTING;
    }

    /* �򿪻Ự�����»������������ǹر��豸*/
    if ((ret = ptp_opensession(params, 1)) == PTP_ERROR_IO) {
        LIBMTP_ERROR("PTP_ERROR_IO: failed to open session, trying again after resetting USB interface\n");
        LIBMTP_ERROR("LIBMTP libusb: Attempt to reset device\n");
        /* ��ʧ�ܣ��ȹر�USB�豸*/
        close_usb(ptp_usb);

        /* �ٳ�ʼ��һ���豸*/
        if(init_ptp_usb(params, ptp_usb, libusb_device) <0) {
            LIBMTP_ERROR("LIBMTP PANIC: Could not init USB on second attempt\n");
            return LIBMTP_ERROR_CONNECTING;
        }

        /* �豸���³�ʼ��������һ�� */
        if ((ret = ptp_opensession(params, 1)) == PTP_ERROR_IO) {
            LIBMTP_ERROR("LIBMTP PANIC: failed to open session on second attempt\n");
            return LIBMTP_ERROR_CONNECTING;
        }
    }
    /* �������ID�Ƿ���Ч */
    if (ret == PTP_RC_InvalidTransactionID) {
        LIBMTP_ERROR("LIBMTP WARNING: Transaction ID was invalid, increment and try again\n");
        /* ???*/
        params->transaction_id += 10;
        /* �ٴδ򿪻Ự*/
        ret = ptp_opensession(params, 1);
    }
    if (ret != PTP_RC_SessionAlreadyOpened && ret != PTP_RC_OK) {
        LIBMTP_ERROR("LIBMTP PANIC: Could not open session! "
                     "(Return code %d)\n  Try to reset the device.\n", ret);
        return LIBMTP_ERROR_CONNECTING;
    }

    /* ������ɣ�����PTP USB�ṹ���ַ*/
    *usbinfo = (void *) ptp_usb;
    return LIBMTP_ERROR_NONE;
}

/**
 * �ر��豸
 *
 * @param ptp_usb PTP USB�豸
 * @param params  PTP����
 **/
void close_device (PTP_USB *ptp_usb, PTPParams *params)
{
    /* �رջỰ*/
    if (ptp_closesession(params)!=PTP_RC_OK)
        LIBMTP_ERROR("ERROR: Could not close session!\n");
    /* �ر�USB*/
    close_usb(ptp_usb);
}

/* ����USB�豸��ʱʱ��*/
void set_usb_device_timeout(PTP_USB *ptp_usb, int timeout)
{
    ptp_usb->timeout = timeout;
}

/* ��ȡUSB�豸��ʱʱ��*/
void get_usb_device_timeout(PTP_USB *ptp_usb, int *timeout)
{
    *timeout = ptp_usb->timeout;
}

/* �²�USB�ٶ�*/
int guess_usb_speed(PTP_USB *ptp_usb)
{
    int bytes_per_second;

    /*
     * ���ǲ�֪��ȷ�е��ٶȣ�����ֻ�ܸ�����Ϣ���ԵĲ²⣬��Ϣ������
     * http://en.wikipedia.org/wiki/USB#Transfer_rates
     * http://www.barefeats.com/usb2.html
     */
    switch (ptp_usb->bcdusb & 0xFF00) {
    case 0x0100:
        /* 1.x USB �汾�ٶ� 1MiB/s */
        bytes_per_second = 1*1024*1024;
        break;
    case 0x0200:
    case 0x0300:
    /* USB 2.0 �����ٶ� 18MiB/s */
    /* USB 3.0 won't be worse? */
    bytes_per_second = 18*1024*1024;
    break;
    default:
        /* Half-guess something? */
        bytes_per_second = 1*1024*1024;
    break;
    }
    return bytes_per_second;
}

/**
 * ����˵�ֹͣ����
 *
 * @param ptp_usb PTP USB�ṹ��
 * @param ep �˵��
 * @return �ɹ�����0
 */
static int usb_clear_stall_feature(PTP_USB* ptp_usb, int ep)
{
    return awbl_usbh_ctrl_sync(&ptp_usb->usb_dev->ep0,
                              AW_USB_DIR_OUT | AW_USB_REQ_TAG_ENDPOINT,
                              AW_USB_REQ_CLEAR_FEATURE,
                              USB_FEATURE_HALT,
                              ep,
                              0,
                              NULL,
                              ptp_usb->timeout, 0);
}

/**
 * ��ȡ�˵�״̬
 *
 * @param ptp_usb PTP USB�ṹ��
 * @param ep �˵��
 * @param status ���ص�״̬
 * @return �ɹ����ػ�ȡ�����ݵĳ���
 */
static int usb_get_endpoint_status(PTP_USB* ptp_usb, int ep, u16* status)
{
    return awbl_usbh_ctrl_sync(&ptp_usb->usb_dev->ep0,
                              AW_USB_DIR_IN | AW_USB_REQ_TAG_ENDPOINT,
                              AW_USB_REQ_GET_STATUS,
                              USB_FEATURE_HALT,
                              ep,
                              2,
                              (char *) status,
                              ptp_usb->timeout, 0);
}
