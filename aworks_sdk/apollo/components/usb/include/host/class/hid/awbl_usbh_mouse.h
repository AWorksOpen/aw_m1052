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

/* USB HID打印函数*/
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

///* 鼠标事件*/
//#define BTN_LEFT        0x110       /* 左键*/
//#define BTN_RIGHT       0x111       /* 右键*/
//#define BTN_MIDDLE      0x112       /* 中间键*/
//#define BTN_SIDE        0x113       /* 侧边键*/
//#define BTN_EXTRA       0x114       /* 额外按键*/
//
//#define REL_COORD       0x01        /* 相对坐标*/
//#define REL_WHEEL       0x08        /* 滚轮*/


/* USB鼠标结构体*/
struct awl_usbh_mouse{
    struct awbl_usbh_function *p_fun;    /* 相关的USB功能结构体*/
    char name[128];                      /* 鼠标名字*/
    struct awbl_usbh_trp *irqtrp;        /* 中断传输请求包*/
    char *data;                          /* 鼠标数据缓存*/
};












#endif
