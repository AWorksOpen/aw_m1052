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

/* USB设备链表*/
extern struct aw_list_head g_udev_list;

#ifndef USB_CLASS_PTP
#define USB_CLASS_PTP 6
#endif

/*
 * 默认USB请求超时长度。这个值可以根据需要重写，但应使用合理的值，以便大多数
 * 常见的USB请求可以完成。原始值4000不足以进行大文件传输，而且，播放器可以
 * 花点时间收集数据。更高的值也是连接/断开更可靠。
 */
#define USB_TIMEOUT_DEFAULT     20000
#define USB_TIMEOUT_LONG        60000
/* 获取USB请求超时时间*/
static inline int get_timeout(PTP_USB* ptp_usb)
{
    /* 有些设备在一些操作上需要长时间超时*/
    if (FLAG_LONG_TIMEOUT(ptp_usb)) {
        return USB_TIMEOUT_LONG;
    }
    return USB_TIMEOUT_DEFAULT;
}

/* USB 控制信息数据阶段方向 */
#ifndef USB_DP_HTD
#define USB_DP_HTD      (0x00 << 7) /* 主机到设备*/
#endif
#ifndef USB_DP_DTH
#define USB_DP_DTH      (0x01 << 7) /* 设备到主机*/
#endif

/* USB Feature selector HALT */
#ifndef USB_FEATURE_HALT
#define USB_FEATURE_HALT    0x00
#endif

/* MTP设备链表结构体*/
struct mtpdevice_list_struct {
  struct mtp_usb_device *libusb_device;    /* USB设备结构体*/
  PTPParams *params;                       /* PTP参数结构体*/
  PTP_USB *ptp_usb;                        /* PTP USB结构体*/
  u32 bus_location;                        /* USB设备在总线的位置*/
  struct mtpdevice_list_struct *next;      /* 指向下一个MTP设备结构体指针*/
};
typedef struct mtpdevice_list_struct mtpdevice_list_t;

/* MTP设备表*/
static const LIBMTP_device_entry_t mtp_device_table[] = {
#include "music-players.h"
};
/* MTP设备表大小*/
static const int mtp_device_table_size = sizeof(mtp_device_table) / sizeof(LIBMTP_device_entry_t);

static int usb_clear_stall_feature(PTP_USB* ptp_usb, int ep);
static int usb_get_endpoint_status(PTP_USB* ptp_usb, int ep, u16* status);


/**
 * 获取支持的USB设备的列表
 *
 * @param devices 要返回的支持的USB设备列表
 * @param numdevs 支持的USB设备列表的数量
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Get_Supported_Devices_List(LIBMTP_device_entry_t * * const devices, int * const numdevs) {
    *devices = (LIBMTP_device_entry_t *) & mtp_device_table;
    *numdevs = mtp_device_table_size;
    return 0;
}

/**
 * 新的USB设备追加到USB MTP设备的链表中
 * @param devlist    要动态链接的MTP USB设备指针链表
 * @param newdevice  要添加的新的USB设备
 *
 * @return  成功返回MTP设备链表，失败返回NULL
 */
static mtpdevice_list_t *append_to_mtpdevice_list(mtpdevice_list_t *devlist,
                          struct mtp_usb_device *newdevice,
                          u32 bus_location){
    mtpdevice_list_t *new_list_entry;

    new_list_entry = (mtpdevice_list_t *) mtp_malloc(sizeof(mtpdevice_list_t));
    if (new_list_entry == NULL) {
        return NULL;
    }
    /* 填充USB设备，作为一个复制*/
    new_list_entry->libusb_device = newdevice;
    new_list_entry->bus_location = bus_location;
    new_list_entry->next = NULL;

    /* 判断MTP设备链表有没有其他MTP设备节点*/
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
 * 释放分配给USB MTP设备链表的MTP设备节点内存
 *
 * @param devlist 指向具有MTP属性的USB设备的指针的动态链表
 * @return 无
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
 * 检查一个设备是否有MTP描述符。这个描述符在gPhoto bug 1482084中进行了详细说明，一些没有附加的字符串
 * 的官方文档由Microsoft在http://www.microsoft.com/whdc/system/bus/USB/USBFAQ_intermed.mspx#E3HAC
 * 发布
 * @param dev         libusb的一个设备结构体
 * @return  如果设备符合MTP协议返回1，返回0则不符合
 */
static int probe_device_descriptor(struct mtp_usb_device *dev)
{
    char *string_buf = NULL;
    unsigned char buf[1024], cmd;
    int i;
    int ret;
    struct awbl_usbh_interface *itf, *itf_temp;
    /* 这指明了我们是否找到一些自定义接口*/
    int found_vendor_spec_interface = 0;


    /* 不要检查不太可能包含任何mtp接口的设备，在发现一些奇怪的组合时更新这个这个判断*/
    if (!(dev->p_desc->bDeviceClass == AW_USB_CLASS_PER_INTERFACE ||
            dev->p_desc->bDeviceClass == AW_USB_CLASS_COMM ||
            dev->p_desc->bDeviceClass == USB_CLASS_PTP ||
            dev->p_desc->bDeviceClass == 0xEF || /* Intf. Association Desc.*/
            dev->p_desc->bDeviceClass == AW_USB_CLASS_VENDOR_SPEC)) {
        return 0;
    }

    /* 存在配置*/
    if (dev->cfg.p_desc) {
        /* 在设备配置上和接口上循环。支持诺基亚MTP的手机(可能还有其他手机)在MTP接口描述符中通常有字符串“MTP”，
         * 这就是在我们尝试更深奥的“OS 描述符”(见下文)之前检测到它们的办法。*/
        for (i = 0; i < dev->p_desc->bNumConfigurations; i++) {
            u8 j;
            /* 更改配置*/
            if(i > 0){
                ret = awbl_usbh_dev_cfg_set(dev, i + 1);
                if(ret != AW_OK){
                    return 0;
                }
            }
            /* 遍历接口*/
            for (j = 0; j < dev->cfg.nfuns; j++) {
                /* 遍历接口和备用接口*/
                aw_list_for_each_entry_safe(itf,
                                            itf_temp,
                                            &dev->cfg.funs[j].itfs,
                                            struct awbl_usbh_interface,
                                            node){
                    /* MTP接口拥有单个端点，两个批量端点和一个中断端点。不用探测别的东西*/
                    if (itf->p_desc->bNumEndpoints != 3)
                        continue;

                    /* 找到一个自定义的接口，标记它*/
                    if (itf->p_desc->bInterfaceClass == AW_USB_CLASS_VENDOR_SPEC) {
                        found_vendor_spec_interface = 1;
                    }
                    /* 接下来我们在接口名中搜索MTP子字符串，例如："RIM MS/MTP"*/
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
    /* 设备是一个自定义设备并且拥有自定义接口*/
    if (dev->p_desc->bDeviceClass == AW_USB_CLASS_VENDOR_SPEC ||
        found_vendor_spec_interface) {
        /* 读取特殊的描述符*/
        ret = awbl_usbh_desc_get(dev, 0x03, 0xee, 0, sizeof(buf), buf);
        /* 如果失败了，则停止，清除端点的停止，并说明这不是MTP设备*/
        if (ret < 0) {
          /* 端点0是默认的控制端点 */
            awbl_usbh_ep_halt_clr(&dev->ep0);
            return 0;
        }
        /* 检查描述符长度是否至少10字节*/
        if (ret < 10) {
            return 0;
        }

        /* 检查设备是否拥有微软描述符 */
        if (!((buf[2] == 'M') && (buf[4] == 'S') &&
          (buf[6] == 'F') && (buf[8] == 'T'))) {
            return 0;
        }

        /* 检查设备是否响应控制信息1或者是否有误 */
        cmd = buf[16];
        ret = awbl_usbh_ctrl_sync(&dev->ep0, AW_USB_DIR_IN|AW_USB_REQ_TAG_DEVICE|AW_USB_REQ_TYPE_VENDOR,
                                cmd, 0, 4, sizeof(buf), buf, 5000, 0);
        if(ret > 0){
            mtp_printf("Microsoft device response to control message 1, CMD 0x%02x:\n", cmd);
        }
        /* 如果满足条件，则设备也不是MTP或有错误 */
        if (ret <= 0x15) {
            /* TODO: 如果有错误，标记它并以某种方式让用户知道 */
            /* if(ret == -1) {} */
            return 0;
        }
        /* 检查设备是否是一个MTP或是否是一个有Janus DRM支持的USB大容量存储设备 */
        if ((buf[0x12] != 'M') || (buf[0x13] != 'T') || (buf[0x14] != 'P')) {
            return 0;
        }

        /* 在这个点之后，我们可能在处理一个MTP设备*/
        /* 检查是否设备响应控制信息2，这是扩展设备参数。大多数设备只会使用与第一个相同的消息副本来响应，
         * 有些则使用pure garbage。我们不会分析结果，因此这不是很重要。*/
        ret = awbl_usbh_ctrl_sync(&dev->ep0, AW_USB_DIR_IN|AW_USB_REQ_TAG_DEVICE|AW_USB_REQ_TYPE_VENDOR,
                                cmd, 0, 5, sizeof(buf), buf, 5000, 0);
        if (ret > 0) {
            mtp_printf("Microsoft device response to control message 2, CMD 0x%02x:\n", cmd);
        }
        /* 如果满足这条件，设备对控制消息2出错 */
        if (ret == -1) {
          /* TODO: 实现回调函数，让管理程序知道存在问题，并对问题进行描述 */
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
 * 这个函数扫描总线上连接的usb设备，如果它们与已知的供应商和产品标识符匹配，则将它们添加到MTP设备链表mtp_device_list中。
 * 通常和free_mtpdevice_list(mtp_device_list)一起使用。
 *
 * @param mtp_device_list  MTP设备链表
 * @return  LIBMTP_ERROR_NONE表示有设备被找到，适当的扫描链表。LIBMTP_ERROR_NO_DEVICE_ATTACHED表示
 *          没有设备被找到。
 */
static LIBMTP_error_number_t get_mtp_usb_device_list(mtpdevice_list_t ** mtp_device_list){
    int i;
    int found = 0;
    struct mtp_usb_device *dev, *dev_temp;

    /* 遍历USB总线上的设备*/
    aw_list_for_each_entry_safe(dev, dev_temp, &g_udev_list, struct mtp_usb_device, node){
        /* 首先检查是不是已知的MTP设备。已知的MTP设备不会探测它们的描述符*/
        for(i = 0; i < mtp_device_table_size; i++) {
            if(dev->p_desc->idVendor == mtp_device_table[i].vendor_id &&
              dev->p_desc->idProduct == mtp_device_table[i].product_id) {
                /* 追加USB设备到MTPdevice链表 */
                *mtp_device_list = append_to_mtpdevice_list(*mtp_device_list, dev, 0);
                found = 1;
                break;
            }
        }
        /* 设备不在已知的MTP设备表里，则探测一下该设备是不是一个MTP*/
        if (!found) {
            if (probe_device_descriptor(dev)) {
                /* 追加USB设备到MTPdevice链表 */
                *mtp_device_list = append_to_mtpdevice_list(*mtp_device_list, dev, 0);
            }
        }
    }
    /* 如果没有找到设备，则在这里结束 */
    if(*mtp_device_list == NULL) {
      return LIBMTP_ERROR_NO_DEVICE_ATTACHED;
    }
    return LIBMTP_ERROR_NONE;
}

/**
 * 检测原始MTP设备描述符和返回找到的设备链表
 *
 *  @param devices 指向变量的指针，该变量将保存找到的原始设备列表。如果检测到的设备数量是0，这可
 *         能会返回NULL。当用户用完了这个原始设备，应该简单的 <code>free()</code> 这个变量
 *         以释放内存。
 *  @param numdevs 指向一个整数的指针，该整数将保存列表中的设备数。这可能是0。
 *  @return 成功返回0，失败返回其他值。
 */
LIBMTP_error_number_t LIBMTP_Detect_Raw_Devices(LIBMTP_raw_device_t ** devices,
        int * numdevs) {
    mtpdevice_list_t *devlist = NULL;
    mtpdevice_list_t *dev;
    LIBMTP_error_number_t ret;
    LIBMTP_raw_device_t *retdevs;
    int devs = 0;
    int i, j;

    /* 获取USB总线上的MTP设备*/
    ret = get_mtp_usb_device_list(&devlist);
    /* 没有MTP设备*/
    if (ret == LIBMTP_ERROR_NO_DEVICE_ATTACHED) {
        *devices = NULL;
        *numdevs = 0;
        return ret;
    } else if (ret != LIBMTP_ERROR_NONE) {
        LIBMTP_ERROR("LIBMTP PANIC: get_mtp_usb_device_list() "
                "error code: %d on line %d\n", ret, __LINE__);
        return ret;
    }
    /* 获取MTP设备数量*/
    dev = devlist;
    while (dev != NULL) {
        devs++;
        dev = dev->next;
    }
    /* MTP设备数量为0*/
    if (devs == 0) {
        *devices = NULL;
        *numdevs = 0;
        return LIBMTP_ERROR_NONE;
    }
    /* 申请一个MTP原始设备链表*/
    retdevs = (LIBMTP_raw_device_t *) mtp_malloc(sizeof(LIBMTP_raw_device_t) * devs);
    if (retdevs == NULL) {
        /* 内存不够*/
        *devices = NULL;
        *numdevs = 0;
        return LIBMTP_ERROR_MEMORY_ALLOCATION;
    }
    dev = devlist;
    i = 0;
    while (dev != NULL) {
        int device_known = 0;

        /* 分配默认设备信息*/
        retdevs[i].device_entry.vendor = NULL;
        retdevs[i].device_entry.vendor_id = dev->libusb_device->p_desc->idVendor;
        retdevs[i].device_entry.product = NULL;
        retdevs[i].device_entry.product_id = dev->libusb_device->p_desc->idProduct;
        retdevs[i].device_entry.device_flags = 0x00000000U;
        /* 与MTP设备表作匹配，看是否能定位一些额外的自定义信息和设备标志*/
        for(j = 0; j < mtp_device_table_size; j++) {
            if(dev->libusb_device->p_desc->idVendor == mtp_device_table[j].vendor_id &&
                    dev->libusb_device->p_desc->idProduct == mtp_device_table[j].product_id) {
                device_known = 1;
                retdevs[i].device_entry.vendor = mtp_device_table[j].vendor;
                retdevs[i].device_entry.product = mtp_device_table[j].product;
                retdevs[i].device_entry.device_flags = mtp_device_table[j].device_flags;
                /* 开发人员知道该设备*/
                LIBMTP_INFO("Device %d (VID=%04x and PID=%04x) is a %s %s.\n", i,
                            dev->libusb_device->p_desc->idVendor,
                            dev->libusb_device->p_desc->idProduct,
                            mtp_device_table[j].vendor,
                            mtp_device_table[j].product);
                break;
            }
        }
        /* 未知设备*/
        if (!device_known) {
            device_unknown(i, dev->libusb_device->p_desc->idVendor,
                         dev->libusb_device->p_desc->idProduct);
        }
        /* 保存总线上的位置*/
        retdevs[i].bus_location = dev->bus_location;
        /* 设备地址*/
        retdevs[i].devnum = dev->libusb_device->addr;
        i++;
        dev = dev->next;
    }
    /* 返回MTP原始设备链表*/
    *devices = retdevs;
    /* 返回MTP原始设备数量*/
    *numdevs = i;
    /* 释放MTP设备链表*/
    free_mtpdevice_list(devlist);
    return LIBMTP_ERROR_NONE;
}

/**
 * 这个函数会打印出底层当前设备的USB信息
 *
 * @param ptp_usb 要获取信息的USB设备
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
 * WMP/Windows可能正在使用这样的算法来进行大型传输：
 * 1.如果头被分割，发送命令(0x0C字节)，否则，发送命令加sizeof(endpoint) - 0x0c
 * 2.仅当使用分割头时，发送第一个数据包时最大大小为sizeof(endpoint)。否则跳到第3点
 * 3.重复发送0x10000字节块，直至剩余字节 < 0x10000，我们称0x10000为CONTEXT_BLOCK_SIZE_1
 * 4.将剩余的字节以sizeof(endpoint)发送
 * 5.发送剩余字节，如果刚刚发送的字节为sizeof(endpoint)，则还要发送一个0长度包
 *
 * 此外，由于设备中的USB从机控制器的缺陷，一些设备根本无法完成0读的操作，因此处理设备的0读操作有一些
 * 特殊的兼容。*/
#define CONTEXT_BLOCK_SIZE_1  0x3e00
#define CONTEXT_BLOCK_SIZE_2  0x200
#define CONTEXT_BLOCK_SIZE    CONTEXT_BLOCK_SIZE_1 + CONTEXT_BLOCK_SIZE_2

/**
 * PTP写函数
 *
 * @param size 要读的数据长度
 * @param handler PTP数据句柄
 * @param data PTP USB结构体
 * @param readbytes 读成功的数据长度
 * @param readzero 是否读0包
 * @return 成功返回PTP_RC_OK
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

    /* "iRiver" 设备特殊的处理*/
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
    /* 读的最大的块大小*/
    bytes = mtp_malloc(CONTEXT_BLOCK_SIZE);
    while (curread < size) {

        LIBMTP_USB_DEBUG_OFF("Remaining size to read: 0x%04lx bytes\n", size - curread);

        /* 把要读的数据分包*/
        if (size - curread < CONTEXT_BLOCK_SIZE)
        {
            /* 最后一包*/
            toread = size - curread;
            /* 需要读取额外的一个字节以指明传输结束*/
            if (readzero && FLAG_NO_ZERO_READS(ptp_usb) && (toread % ptp_usb->inep_maxpacket == 0)) {
                toread += 1;
                expect_terminator_byte = 1;
            }
        }
        else if (ptp_dev_vendor_id == 0x4102 || ptp_dev_vendor_id == 0x1006) {
            /* "iRiver" 设备特殊处理*/
            if (curread == 0)
                /* 第一个包但不是最后一个包*/
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
        /* USB批量读数据*/
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

        /* 减去最后额外的一个字节*/
        if (expect_terminator_byte && result == toread)
        {
            LIBMTP_USB_DEBUG_OFF("<==USB IN\nDiscarding extra byte\n");

            result--;
        }
        /* 把读出来的数据放到PTP数据句柄里*/
        int putfunc_ret = handler->putfunc(NULL, handler->priv, result, bytes);
        if (putfunc_ret != PTP_RC_OK)
          return putfunc_ret;
        /* 更新当前传输完成的数据量*/
        ptp_usb->current_transfer_complete += result;
        curread += result;

        /* 调用回调*/
        if (ptp_usb->callback_active) {
            if (ptp_usb->current_transfer_complete >= ptp_usb->current_transfer_total) {
                /* 关闭回调*/
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
        /* 读出来的数据比预期的少*/
        if (result < toread)
            break;
    }
    /* 设置最终读成功的数据长度*/
    if (readbytes) *readbytes = curread;
    mtp_free (bytes);

    /* 需要读0包*/
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
 * PTP写函数
 *
 * @param size 写数据长度
 * @param handler PTP数据句柄
 * @param data PTP USB结构体
 * @param written 写成功的长度
 * @return 成功返回PTP_RC_OK
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

    /* 单次写的数据块的最大长度*/
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
            /* 使数据包的大小与端点最大包大小相等*/
            if (towrite > ptp_usb->outep_maxpacket && towrite % ptp_usb->outep_maxpacket != 0) {
                towrite -= towrite % ptp_usb->outep_maxpacket;
            }
        }
        /* 获取数据*/
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
        /* 调用回调*/
        if (ptp_usb->callback_active) {
            if (ptp_usb->current_transfer_complete >= ptp_usb->current_transfer_total) {
                /* 发送最新的更新和禁能回调*/
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
    /* 如果这是最后一次传输，如果需要则写0*/
    if (ptp_usb->current_transfer_complete >= ptp_usb->current_transfer_total) {
        /* 发送0长度包*/
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

/* 内存数据获取/放入句柄*/
typedef struct {
    unsigned char *data;         /* 数据缓存指针*/
    unsigned long  size;         /* 数据长度*/
    unsigned long  curoff;       /* 当前偏移*/
} PTPMemHandlerPrivate;

/**
 * 从内存数据句柄中拿出数据
 *
 * @param params PTP参数结构体
 * @param private PTP内存私有句柄
 * @param wantlen 想要的长度
 * @param data 获取数据的缓存
 * @param gotlen 实际获得的数据的长度
 * @return 成功返回PTP_RC_OK
 */
static u16 memory_getfunc(PTPParams* params, void* private,
                            unsigned long wantlen, unsigned char *data,
                            unsigned long *gotlen) {
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*) private;
    unsigned long tocopy = wantlen;

    if (priv->curoff + tocopy > priv->size)
        tocopy = priv->size - priv->curoff;
    /* 把数据复制到缓存里去*/
    memcpy(data, priv->data + priv->curoff, tocopy);
    /* 更新偏移*/
    priv->curoff += tocopy;
    /* 实际复制数据的长度*/
    *gotlen = tocopy;
    return PTP_RC_OK;
}

/**
 * 把数据放入内存数据句柄中
 *
 * @param params PTP参数结构体
 * @param private PTP内存私有句柄
 * @param sendlen 想要放入数据的长度
 * @param data 获取数据的缓存
 * @return 成功返回PTP_RC_OK
 */
static u16 memory_putfunc(PTPParams* params, void* private,
            unsigned long sendlen, unsigned char *data) {
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*) private;

    /* 检查当前偏移加上要加入的数据长度是否大于数据总量*/
    if (priv->curoff + sendlen > priv->size) {
        /* 重新分配内存*/
        priv->data = mtp_realloc(priv->data, priv->curoff + sendlen);
        priv->size = priv->curoff + sendlen;
    }
    /* 拷贝数据*/
    memcpy(priv->data + priv->curoff, data, sendlen);
    priv->curoff += sendlen;
    return PTP_RC_OK;
}

/* 初始化接收数据的私有句柄 */
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

/* 初始化发送数据的私有句柄 */
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

/* 释放发送数据句柄*/
static u16 ptp_exit_send_memory_handler (PTPDataHandler *handler) {
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*)handler->priv;
    mtp_free (priv);
    return PTP_RC_OK;
}

/* 获取接收数据句柄里的数据并释放数据句柄*/
static u16 ptp_exit_recv_memory_handler (PTPDataHandler *handler,
    unsigned char **data, unsigned long *size) {
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*)handler->priv;
    *data = priv->data;
    *size = priv->size;
    mtp_free (priv);
    return PTP_RC_OK;
}

/**
 * PTP发送请求函数
 *
 * @param params PTP参数结构体
 * @param req PTP请求容器
 * @param dataphase 数据阶段
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_usb_sendreq (PTPParams* params, PTPContainer* req, int dataphase){
    u16 ret;
    PTPUSBBulkContainer usbreq;
    PTPDataHandler  memhandler;
    unsigned long written = 0;
    unsigned long towrite;

    LIBMTP_USB_DEBUG_OFF("REQUEST: 0x%04x, %s\n", req->Code, ptp_get_opcode_name(params, req->Code));

    /* 建立适当的USB容器 */
    usbreq.length   = AW_USB_CPU_TO_LE32(PTP_USB_BULK_REQ_LEN - (sizeof(u32)*(5-req->Nparam)));
    usbreq.type     = AW_USB_CPU_TO_LE16(PTP_USB_CONTAINER_COMMAND);
    usbreq.code     = AW_USB_CPU_TO_LE16(req->Code);
    usbreq.trans_id = AW_USB_CPU_TO_LE32(req->Transaction_ID);
    usbreq.payload.params.param1 = AW_USB_CPU_TO_LE32(req->Param1);
    usbreq.payload.params.param2 = AW_USB_CPU_TO_LE32(req->Param2);
    usbreq.payload.params.param3 = AW_USB_CPU_TO_LE32(req->Param3);
    usbreq.payload.params.param4 = AW_USB_CPU_TO_LE32(req->Param4);
    usbreq.payload.params.param5 = AW_USB_CPU_TO_LE32(req->Param5);
    /* 发送到响应器 */
    /* 根据参数数量调整请求长度*/
    towrite = PTP_USB_BULK_REQ_LEN - (sizeof(u32)*(5 - req->Nparam));
    /* 初始化发送内存句柄*/
    ptp_init_send_memory_handler (&memhandler, (unsigned char*)&usbreq, towrite);
    /* PTP写*/
    ret = ptp_write_func(towrite, &memhandler, params->data, &written);
    /* 释放发送内存句柄*/
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
 * PTP发送数据函数
 *
 * @param params  PTP参数结构体
 * @param ptp     PTP请求容器
 * @param size    要发送数据的大小
 * @param handler PTP数据句柄
 *
 * @return 成功返回PTP_RC_OK
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

    /* 构建合适的USB容器*/
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
        /* 所有摄像头设备*/
        /* 检查数据长度*/
        datawlen = (size < PTP_USB_BULK_PAYLOAD_LEN_WRITE)?size:PTP_USB_BULK_PAYLOAD_LEN_WRITE;
        wlen = PTP_USB_BULK_HDR_LEN + datawlen;

        ret = handler->getfunc(params, handler->priv, datawlen, usbdata.payload.data, &gotlen);
        if (ret != PTP_RC_OK)
            return ret;
        if (gotlen != datawlen)
            return PTP_RC_GeneralError;
    }
    /* 初始化发送内存句柄*/
    ptp_init_send_memory_handler (&memhandler, (unsigned char *)&usbdata, wlen);
    /* 发送第一部分数据*/
    ret = ptp_write_func(wlen, &memhandler, params->data, &written);
    /* 释放内存句柄*/
    ptp_exit_send_memory_handler (&memhandler);
    if (ret != PTP_RC_OK) {
        return ret;
    }
    if (size <= datawlen)
        return ret;
    /* 没问题则发送剩余数据*/
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
 *  PTP USB获取包
 *
 *  @param params PTP参数结构体
 *  @param packet PTP USB批量容器
 *  @param rlen 获取到的包的长度
 *  @return 成功返回PTP_RC_OK
 */
static u16 ptp_usb_getpacket(PTPParams *params,
        PTPUSBBulkContainer *packet, unsigned long *rlen){
    PTPDataHandler  memhandler;
    u16    ret;
    unsigned char   *x = NULL;
    unsigned long packet_size;
    PTP_USB *ptp_usb = (PTP_USB *) params->data;
    /* 获取输入端点的最大包大小*/
    packet_size = ptp_usb->inep_maxpacket;

    /* 已经存在响应包，使用这个响应包(虚拟读)*/
    if (params->response_packet_size > 0) {
        memcpy(packet, params->response_packet, params->response_packet_size);
        *rlen = params->response_packet_size;
        mtp_free(params->response_packet);
        params->response_packet = NULL;
        params->response_packet_size = 0;
        return PTP_RC_OK;
    }
    /* 初始化接收内存句柄*/
    ptp_init_recv_memory_handler (&memhandler);
    /* PTP读*/
    ret = ptp_read_func(packet_size, &memhandler, params->data, rlen, 0);
    /* 获取数据和数据长度，释放接收内存句柄*/
    ptp_exit_recv_memory_handler (&memhandler, &x, rlen);
    if (x) {
        /* 复制数据*/
        memcpy (packet, x, *rlen);
        mtp_free (x);
    }
    return ret;
}

/**
 * PTP获取数据函数
 *
 * @param params PTP参数结构体
 * @param req PTP请求容器
 * @param handler 数据句柄
 * @return 成功返回PTP_RC_OK
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
        /* 读取"获取设备信息"的数据，读取端点最大包大小的数据*/
        ret = ptp_usb_getpacket(params, &usbdata, &rlen);
        if (ret != PTP_RC_OK) {
            ret = PTP_ERROR_IO;
            break;
        }
        /* 检查是不是数据容器*/
        if (AW_USB_CPU_TO_LE16(usbdata.type) != PTP_USB_CONTAINER_DATA) {
            ret = PTP_ERROR_DATA_EXPECTED;
            break;
        }
        /* 事件代码不一致*/
        if (AW_USB_CPU_TO_LE16(usbdata.code) != ptp->Code) {
            /* 是否忽略坏头*/
            if (FLAG_IGNORE_HEADER_ERRORS(ptp_usb)) {
                libusb_glue_debug (params, "ptp2/ptp_usb_getdata: detected a broken "
                       "PTP header, code field insane, expect problems! (But continuing)");
                /* 修复头部信息，通常这两个域会被破坏*/
                usbdata.code     = AW_USB_CPU_TO_LE16(ptp->Code);
                usbdata.trans_id = AW_USB_CPU_TO_LE32(ptp->Transaction_ID);
                ret = PTP_RC_OK;
            } else {
                ret = AW_USB_CPU_TO_LE16(usbdata.code);
                /* 这过滤了完全没用的返回代码，但仍然可以在获取数据时返回code字段中的错误代码。Windows似乎完全忽略
                 * 了这个字段的内容*/
                if (ret < PTP_RC_Undefined || ret > PTP_RC_SpecificationOfDestinationUnsupported) {
                    libusb_glue_debug (params, "ptp2/ptp_usb_getdata: detected a broken "
                           "PTP header, code field insane.");
                    ret = PTP_ERROR_IO;
                }
                break;
            }
        }
        /* 长度刚好是端点最大包大小，说明还有数据*/
        if (rlen == ptp_usb->inep_maxpacket) {
            /* 拷贝第一部分的数据到PTP数据句柄里 */
            putfunc_ret = handler->putfunc(params, handler->priv,
                            rlen - PTP_USB_BULK_HDR_LEN, usbdata.payload.data);
            if (putfunc_ret != PTP_RC_OK)
                return putfunc_ret;

            /* 将数据直接填充到传输的数据句柄 */
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
        /* 读取到的数据的长度大于要请求的长度*/
        if (rlen > AW_USB_CPU_TO_LE32(usbdata.length)) {
            /*
             * 缓冲多余的响应包，如果它的长度大于PTP_USB_BULK_HDR_LEN(即它可能是一个
             * 完整的包)，否则将其作为错误的剩余数据丢弃。即使在同一事务中出现两个以上的包，这
             * 也会起作用，它们只会被迭代处理
             * iRiver设备有一些散乱字节，这些字节仍被丢弃*/
            unsigned int packlen = AW_USB_CPU_TO_LE32(usbdata.length);
            unsigned int surplen = rlen - packlen;

            if (surplen >= PTP_USB_BULK_HDR_LEN) {
                params->response_packet = mtp_malloc(surplen);
                memcpy(params->response_packet, (u8 *) &usbdata + packlen, surplen);
                params->response_packet_size = surplen;
            /* 忽略读取额外的一个字节 */
            } else if(!FLAG_NO_ZERO_READS(ptp_usb) &&
                  (rlen - AW_USB_CPU_TO_LE32(usbdata.length) == 1)) {
                libusb_glue_debug (params, "ptp2/ptp_usb_getdata: read %d bytes "
                     "too much, expect problems!",
                     rlen - AW_USB_CPU_TO_LE32(usbdata.length));
            }
            rlen = packlen;
        }
        /* 对于大多数PTP设备，这里rlen是512 == sizeof(usbdata)。对于MTP设备的分割头和数据可能是12*/
        /* 计算完整数据的长度 */
        len = AW_USB_CPU_TO_LE32(usbdata.length) - PTP_USB_BULK_HDR_LEN;

        /* 自动检测分割头/数据 MTP设备 */
        if (AW_USB_CPU_TO_LE32(usbdata.length) > 12 && (rlen == 12))
            params->split_header_data = 1;
        /* 拷贝第一部分的数据到PTP数据句柄里 */
        putfunc_ret = handler->putfunc(params, handler->priv, rlen - PTP_USB_BULK_HDR_LEN,
                           usbdata.payload.data);
        if (putfunc_ret != PTP_RC_OK)
            return putfunc_ret;

        if (FLAG_NO_ZERO_READS(ptp_usb) &&
            len + PTP_USB_BULK_HDR_LEN == ptp_usb->inep_maxpacket) {

            LIBMTP_USB_DEBUG_OFF("Reading in extra terminating byte\n");

            /* 需要读取额外的字节并丢弃它*/
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
        /* 所有数据？*/
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
 * PTP获取响应
 *
 * @param params PTP参数结构体
 * @param resp PTP请求容器
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_usb_getresp (PTPParams* params, PTPContainer* resp){
    u16 ret;
    unsigned long rlen;
    PTPUSBBulkContainer usbresp;
    PTP_USB *ptp_usb = (PTP_USB *)(params->data);


    LIBMTP_USB_DEBUG_OFF("RESPONSE: \r\n");

    memset(&usbresp, 0, sizeof(usbresp));
    /* 读取响应, 不应该比sizeof(usbresp)更长 */
    ret = ptp_usb_getpacket(params, &usbresp, &rlen);
    /* 以下操作是为兼容Samsung YP-U3设备，当这个设备在发送响应的时候会在一个空包之后发送一个
     * 只包含长度为2的0数据的包(最多可以看到4个)*/
    while (ret == PTP_RC_OK && rlen < PTP_USB_BULK_HDR_LEN && usbresp.length == 0) {
        libusb_glue_debug (params, "ptp_usb_getresp: detected short response "
                            "of %d bytes, expect problems! (re-reading "
                            "response), rlen");
        ret = ptp_usb_getpacket(params, &usbresp, &rlen);
    }

    if (ret != PTP_RC_OK) {
        ret = PTP_ERROR_IO;
    } else if (AW_USB_CPU_TO_LE16(usbresp.type) != PTP_USB_CONTAINER_RESPONSE) {
        /* 返回的响应的容器类型和期望的不一样*/
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
    /* 建立一个PTP容器 */
    resp->Code = AW_USB_CPU_TO_LE16(usbresp.code);
    resp->SessionID = params->session_id;
    resp->Transaction_ID = AW_USB_CPU_TO_LE32(usbresp.trans_id);
    if (FLAG_IGNORE_HEADER_ERRORS(ptp_usb)) {
        if (resp->Transaction_ID != params->transaction_id - 1) {
            libusb_glue_debug (params, "ptp_usb_getresp: detected a broken "
                   "PTP header, transaction ID insane, expect "
                   "problems! (But continuing)");
            /* 修复头部*/
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
 * PTP取消控制请求
 *
 * @param params        PTP参数结构体
 * @param transactionid 要取消的事务ID
 *
 * @return 成功返回PTP_RC_OK
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
 * 初始化一个PTP USB结构体
 *
 * @param params PTP参数结构体
 * @param ptp_usb 要初始化的PTP USB结构体
 * @param dev USB设备结构体
 * @return 成功返回0
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
    /* 字节序*/
    params->byteorder = PTP_DL_LE;
    /* USB超时时间*/
    ptp_usb->timeout = get_timeout(ptp_usb);

    ptp_usb->usb_dev = dev;

    /*
     * 检查配置是否设置为我们不想使用的其他内容。只有在绝对必须的情况下才能设置配置。
     */
    if (dev->cfg.p_desc->bConfigurationValue != ptp_usb->config) {
        if (awbl_usbh_dev_cfg_set(dev, dev->cfg.p_desc->bConfigurationValue) != AW_OK) {
            mtp_printf("error in usb set configuration - continuing anyway\r\n");
        }
    }

    if (FLAG_SWITCH_MODE_BLACKBERRY(ptp_usb)) {
        int ret;
        //FIXME : Only for BlackBerry Storm
        /* 什么意思？可能是切换模式...
         * 第一个控制信息是绝对必要的*/
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
        /* 这个控制信息的非必要的*/
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
        /* 这个控制信息的非必要的*/
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
        /* 这个控制信息的非必要的*/
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

/* 清除端点停止状态*/
static void clear_stall(PTP_USB* ptp_usb)
{
    u16 status;
    int ret;

    /* 检查中断端点状态 */
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

    /* 检查除数==输出端点状态 */
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
 * 关闭USB
 *
 * @param params PTP参数结构体
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
        /* AWork执行以下操作会有问题*/
        //awbl_usbh_endpoint_reset(ptp_usb->usb_dev, ptp_usb->outep);
    }
    if (FLAG_FORCE_RESET_ON_CLOSE(ptp_usb)) {
        /* 有些设备真的喜欢在断开连接后复位。同样，由于Windows从不断开设备，因此在
         * 设计时很少或重未在设备上执行设备关闭行为，而且往往容易出错。
         * 复位可能有帮助*/
        //usb_reset(ptp_usb->handle);
    }
}


/**
 * 获取符合MTP的设备接口和端点
 *
 * @param dev USB设备结构体
 * @param conf USB设备的配置编号
 * @param interface USB设备配置接口编号
 * @param altsetting USB设备配置接口编号备用设置号
 * @param inep 输入端点号
 * @param inep_maxpacket 输入端点最大包大小
 * @param outep 输出端点
 * @param outep_maxpacket 输出端点最大包大小
 * @param intep 中断端点号
 * @return 成功返回0
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

    /* 遍历设备配置*/
    for (i = 0; i < dev->p_desc->bNumConfigurations; i++) {
        /* 更改配置*/
        if(i > 0){
            ret = awbl_usbh_dev_cfg_set(dev, i + 1);
            if(ret != AW_OK){
                return -1;
            }
        }
        uint8_t j;
        *conf = dev->cfg.p_desc->bConfigurationValue;
        /* 遍历配置接口*/
        for (j = 0; j < dev->cfg.nfuns; j++) {
            u8 l;
            u8 no_ep;
            int found_inep = 0;
            int found_outep = 0;
            int found_intep = 0;
            struct awbl_usbh_endpoint *ep;
            /* 遍历接口和备用接口*/
            aw_list_for_each_entry_safe(itf,
                                        itf_temp,
                                        &dev->cfg.funs[j].itfs,
                                        struct awbl_usbh_interface,
                                        node){
                /* MTP设备应该有3个端点，忽略哪些没有的接口*/
                no_ep = itf->p_desc->bNumEndpoints;
                if (no_ep != 3)
                    continue;
                *interface = itf->p_desc->bInterfaceNumber;
                *altsetting = itf->p_desc->bAlternateSetting;
                ep = itf->eps;
                /* 在三个端点上循环以定位两个批量端点和一个中断端点，如果不能，则失败，然后继续*/
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
                    /* 找到端点，所以在这里返回*/
                    if (found_inep && found_outep && found_intep)
                      return 0;
                }
            }
        }
    }
    return -1;
}

/**
 * 此函数将参数和usbinfo分配给一个原始设备作为输入
 *
 * @param device  要分配的设备
 * @param usbinfo 一个指向新的usbinfo的指针
 * @return 一个错误代码
 */
LIBMTP_error_number_t configure_usb_device(LIBMTP_raw_device_t *device,
                       PTPParams *params, void **usbinfo){
    PTP_USB *ptp_usb;
    struct mtp_usb_device *libusb_device;
    u16 ret = 0;
    int found = 0;
    int err;
    struct mtp_usb_device *dev, *dev_temp;

    /* 遍历USB总线上的设备，看看是否还能再次找到这个原始设备*/
    aw_list_for_each_entry_safe(dev, dev_temp, &g_udev_list, struct mtp_usb_device, node){
        if((dev->addr == device->devnum) && (dev->p_desc->idVendor == device->device_entry.vendor_id)
                && (dev->p_desc->idProduct == device->device_entry.product_id)){
            libusb_device = dev;
            found = 1;
            break;
        }
    }
    /* 设备消失了 */
    if (!found) {
      return LIBMTP_ERROR_NO_DEVICE_ATTACHED;
    }
    /* 分配结构体*/
    ptp_usb = (PTP_USB *) mtp_malloc(sizeof(PTP_USB));
    if (ptp_usb == NULL) {
        return LIBMTP_ERROR_MEMORY_ALLOCATION;
    }
    /* 清0空间 (包括设备 device_flags为0) */
    memset(ptp_usb, 0, sizeof(PTP_USB));
    /* 赋值原始设备 */
    memcpy(&ptp_usb->rawdevice, device, sizeof(LIBMTP_raw_device_t));
    /*
     * 一些设备需要始终检测其“OS描述符”才能正常工作
     */
    if (FLAG_ALWAYS_PROBE_DESCRIPTOR(ptp_usb)) {
      /* 检测设备描述符*/
      (void) probe_device_descriptor(libusb_device);
    }
    /* 寻找符合要求的接口和端点 */
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
    /* 复制USB版本号 */
    ptp_usb->bcdusb = libusb_device->p_desc->bcdUSB;

    /* 初始化设备 */
    if (init_ptp_usb(params, ptp_usb, libusb_device) < 0) {
        LIBMTP_ERROR("LIBMTP PANIC: Unable to initialize device\n");
        return LIBMTP_ERROR_CONNECTING;
    }

    /* 打开会话，以下机制适用于忘记关闭设备*/
    if ((ret = ptp_opensession(params, 1)) == PTP_ERROR_IO) {
        LIBMTP_ERROR("PTP_ERROR_IO: failed to open session, trying again after resetting USB interface\n");
        LIBMTP_ERROR("LIBMTP libusb: Attempt to reset device\n");
        /* 打开失败，先关闭USB设备*/
        close_usb(ptp_usb);

        /* 再初始化一次设备*/
        if(init_ptp_usb(params, ptp_usb, libusb_device) <0) {
            LIBMTP_ERROR("LIBMTP PANIC: Could not init USB on second attempt\n");
            return LIBMTP_ERROR_CONNECTING;
        }

        /* 设备重新初始化，再试一遍 */
        if ((ret = ptp_opensession(params, 1)) == PTP_ERROR_IO) {
            LIBMTP_ERROR("LIBMTP PANIC: failed to open session on second attempt\n");
            return LIBMTP_ERROR_CONNECTING;
        }
    }
    /* 检查事务ID是否有效 */
    if (ret == PTP_RC_InvalidTransactionID) {
        LIBMTP_ERROR("LIBMTP WARNING: Transaction ID was invalid, increment and try again\n");
        /* ???*/
        params->transaction_id += 10;
        /* 再次打开会话*/
        ret = ptp_opensession(params, 1);
    }
    if (ret != PTP_RC_SessionAlreadyOpened && ret != PTP_RC_OK) {
        LIBMTP_ERROR("LIBMTP PANIC: Could not open session! "
                     "(Return code %d)\n  Try to reset the device.\n", ret);
        return LIBMTP_ERROR_CONNECTING;
    }

    /* 配置完成，返回PTP USB结构体地址*/
    *usbinfo = (void *) ptp_usb;
    return LIBMTP_ERROR_NONE;
}

/**
 * 关闭设备
 *
 * @param ptp_usb PTP USB设备
 * @param params  PTP参数
 **/
void close_device (PTP_USB *ptp_usb, PTPParams *params)
{
    /* 关闭会话*/
    if (ptp_closesession(params)!=PTP_RC_OK)
        LIBMTP_ERROR("ERROR: Could not close session!\n");
    /* 关闭USB*/
    close_usb(ptp_usb);
}

/* 设置USB设备超时时间*/
void set_usb_device_timeout(PTP_USB *ptp_usb, int timeout)
{
    ptp_usb->timeout = timeout;
}

/* 获取USB设备超时时间*/
void get_usb_device_timeout(PTP_USB *ptp_usb, int *timeout)
{
    *timeout = ptp_usb->timeout;
}

/* 猜测USB速度*/
int guess_usb_speed(PTP_USB *ptp_usb)
{
    int bytes_per_second;

    /*
     * 我们不知道确切的速度，所以只能根据信息粗略的猜测，信息来自于
     * http://en.wikipedia.org/wiki/USB#Transfer_rates
     * http://www.barefeats.com/usb2.html
     */
    switch (ptp_usb->bcdusb & 0xFF00) {
    case 0x0100:
        /* 1.x USB 版本速度 1MiB/s */
        bytes_per_second = 1*1024*1024;
        break;
    case 0x0200:
    case 0x0300:
    /* USB 2.0 正常速度 18MiB/s */
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
 * 清除端点停止特性
 *
 * @param ptp_usb PTP USB结构体
 * @param ep 端点号
 * @return 成功返回0
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
 * 获取端点状态
 *
 * @param ptp_usb PTP USB结构体
 * @param ep 端点号
 * @param status 返回的状态
 * @return 成功返回获取的数据的长度
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
