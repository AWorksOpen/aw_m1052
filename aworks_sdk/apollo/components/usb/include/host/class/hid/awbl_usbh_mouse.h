/************************************************
 * create by CYX at 12/9-2019
 * USB Human Interface Device Driver
 ************************************************/
#ifndef __AWBL_USBH_MOUSE_H
#define __AWBL_USBH_MOUSE_H
#include "host/awbl_usbh.h"
#include "aw_usb_os.h"
#include "aw_input.h"
#include "aw_input_code.h"

#define USB_MOUSE_DEVICE_MATCH_INT_INFO AWBL_USBH_FUNDRV_MATCH_FUN_CLASS

/* USB HID��ӡ����*/
#define __USBMOUSE_TRACE(info) \
            do { \
                AW_INFOF(("USBH-MOUSE: ")); \
                AW_INFOF(info); \
            } while (0)

#ifdef USBMOUSE_DEBUG
    #define __USBMOUSE_TRACE_DEBUG  __USBMOUSE_TRACE
    #define usbmouse_printf  aw_kprintf
#else
    #define __USBMOUSE_TRACE_DEBUG(info) \
            do { \
            } while (0)
#endif

///* ����¼�*/
//#define BTN_LEFT        0x110       /* ���*/
//#define BTN_RIGHT       0x111       /* �Ҽ�*/
//#define BTN_MIDDLE      0x112       /* �м��*/
//#define BTN_SIDE        0x113       /* ��߼�*/
//#define BTN_EXTRA       0x114       /* ���ⰴ��*/
//
//#define REL_COORD       0x01        /* �������*/
//#define REL_WHEEL       0x08        /* ����*/


/* USB���ṹ��*/
struct awl_usbh_mouse{
    struct awbl_usbh_function *p_fun;    /* ��ص�USB���ܽṹ��*/
    char name[128];                      /* �������*/
    struct awbl_usbh_trp *irqtrp;        /* �жϴ��������*/
    char *data;                          /* ������ݻ���*/
};












#endif
