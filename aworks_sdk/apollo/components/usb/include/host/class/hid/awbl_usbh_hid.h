/************************************************
 * create by CYX at 12/9-2019
 * USB Human Interface Device Driver
 ************************************************/
#ifndef __AWBL_USBH_HID_H
#define __AWBL_USBH_HID_H
#include "host/awbl_usbh.h"
#include "aw_usb_os.h"

#define USB_HID_DEVICE_MATCH_INT_INFO AWBL_USBH_FUNDRV_MATCH_FUN_CLASS

/* USB HID��ӡ����*/
#define __USBHID_TRACE(info) \
            do { \
                AW_INFOF(("USBH-HID: ")); \
                AW_INFOF(info); \
            } while (0)

#ifdef USBHID_DEBUG
    #define __USBHID_TRACE_DEBUG  __USBHID_TRACE
    #define usbhid_printf  aw_kprintf
#else
    #define __USBHID_TRACE_DEBUG(info) \
            do { \
            } while (0)
#endif



/* USB HID�ӿ������Э�����*/
#define USB_INTERFACE_SUBCLASS_BOOT     1
#define USB_INTERFACE_PROTOCOL_KEYBOARD 1
#define USB_INTERFACE_PROTOCOL_MOUSE    2

/* USB HID����*/
enum usbhid_type {
    HID_TYPE_OTHER = 0,
    HID_TYPE_USBMOUSE,
    HID_TYPE_USBNONE
};

#define HID_STAT_ADDED      1
#define HID_STAT_PARSED     2

#define MAX_USBHID_BOOT_QUIRKS 4

#define HID_QUIRK_INVERT                        0x00000001
#define HID_QUIRK_NOTOUCH                       0x00000002
#define HID_QUIRK_IGNORE                        0x00000004
#define HID_QUIRK_NOGET                         0x00000008
#define HID_QUIRK_HIDDEV_FORCE                  0x00000010
#define HID_QUIRK_BADPAD                        0x00000020
#define HID_QUIRK_MULTI_INPUT                   0x00000040
#define HID_QUIRK_HIDINPUT_FORCE                0x00000080
#define HID_QUIRK_NO_EMPTY_INPUT                0x00000100
#define HID_QUIRK_NO_INIT_INPUT_REPORTS         0x00000200
#define HID_QUIRK_ALWAYS_POLL                   0x00000400
#define HID_QUIRK_SKIP_OUTPUT_REPORTS           0x00010000
#define HID_QUIRK_SKIP_OUTPUT_REPORT_ID         0x00020000
#define HID_QUIRK_NO_OUTPUT_REPORTS_ON_INTR_EP  0x00040000
#define HID_QUIRK_FULLSPEED_INTERVAL            0x10000000
#define HID_QUIRK_NO_INIT_REPORTS               0x20000000
#define HID_QUIRK_NO_IGNORE                     0x40000000
#define HID_QUIRK_NO_INPUT_SYNC                 0x80000000

struct awbl_usbh_hid_device;

/* USB HID�ײ���������*/
struct usbhid_ll_driver {
    int (*parse)(struct awbl_usbh_hid_device *hdev);
    /* ����ԭʼ�ı���������豸*/
    int (*raw_request) (struct awbl_usbh_hid_device *hdev, uint8_t reportnum,
                uint8_t *buf, uint32_t len, uint8_t rtype, int reqtype);
};

/* USB����ӿ��豸�ṹ��*/
struct awbl_usbh_hid_device {
    struct awbl_usbh_interface *itf;         /* �������豸�ӿ�*/
    int                        itfnum;       /* �������豸�ӿڱ��*/
    uint32_t                   vendor;       /* ��Ӧ�� ID */
    uint32_t                   product;      /* ���� ID */
    char                       name[128];    /* �豸����*/
    aw_spinlock_isr_t          lock;         /* FIFO������*/
    enum usbhid_type           type;         /* �豸���� (���, ...) */
    struct usbhid_ll_driver    *ll_driver;   /* �ײ���������*/
    uint8_t                    status;       /* ״̬*/
    uint32_t                   quirks;       /* ����*/
};
















#endif
