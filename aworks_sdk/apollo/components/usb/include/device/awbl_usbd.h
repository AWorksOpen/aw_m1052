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



#ifndef __AWBL_USBD_H
#define __AWBL_USBD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbus_lite.h"
#include "aw_list.h"
#include "aw_spinlock.h"
#include "aw_usb_common.h"
#include "aw_usb_os.h"





/** \brief event for controller to report */
#define AWBL_USBD_EVT_CONNECT           0x00000001           /* connect */
#define AWBL_USBD_EVT_DISCONNECT        0x00000002           /* disconnect */
#define AWBL_USBD_EVT_BUS_RST           0x00000004           /* bus reset */
#define AWBL_USBD_EVT_SUSPENDED         0x00000008           /* suspended */
#define AWBL_USBD_EVT_RESUME            0x00000010           /* resume */
#define AWBL_USBD_EVT_SETUP_PKT         0x00000020           /* setup packet */
#define AWBL_USBD_EVT_UPDATE_SPEED      0x00000040           /* update speed */
#define AWBL_USBD_EVT_ERROR             0x00000080           /* error */
#define AWBL_USBD_EVT_DC_STOP           0x00000100           /* Device controller stop*/



/** \brief controller state */
#define AWBL_USBD_ST_NOTATTACHED       0           /* disconnect */
#define AWBL_USBD_ST_ATTACHED          1           /* connected but not be enumerated */
#define AWBL_USBD_ST_POWERED           2           /* power up */
#define AWBL_USBD_ST_DEFAULT           3           /* default  */
#define AWBL_USBD_ST_ADDRESS           4           /* set address */
#define AWBL_USBD_ST_CONFIGURED        5           /* configure */
#define AWBL_USBD_ST_SUSPENDED         6           /* suspended */




/** \brief controller speed */
#define AWBL_USBD_SPEED_UNKNOWN        0
#define AWBL_USBD_SPEED_LOW            1           /* usb 1.1 */
#define AWBL_USBD_SPEED_FULL           2           /* usb 1.1 */
#define AWBL_USBD_SPEED_HIGH           3           /* usb 2.0 */
#define AWBL_USBD_SPEED_WIRELESS       4           /* wireless (usb 2.5) */
#define AWBL_USBD_SPEED_SUPER          5           /* usb 3.0 */




/** 传输标志*/
#define AWBL_USBD_SHORT_NOT_OK      0x00000001     /* 短包错误标志*/
#define AWBL_USBD_ZERO_PACKET       0x00000002     /* 用一个短包来结束一次传输*/
#define AWBL_USBD_NO_INTERRUPT      0x00000004     /* 不需要中断，除非错误*/




/** \brief EP type support flag */
#define AWBL_USBD_EP_SUPPORT_CTRL    0x01
#define AWBL_USBD_EP_SUPPORT_ISO     0x02
#define AWBL_USBD_EP_SUPPORT_BULK    0x04
#define AWBL_USBD_EP_SUPPORT_INT     0x08
#define AWBL_USBD_EP_SUPPORT_ALL     0x0F




/** \brief EP max packet size limit */
#define AWBL_USBD_EP_MPS_NO_LIMT     0x00




/** \brief controller configuration flag */
#define AWBL_USBD_CFG_SELF_POWERED     0x0200
#define AWBL_USBD_CFG_REMOTE_WAKEUP    0x0400
#define AWBL_USBD_CFG_MAX_POWER        0x01FF
#define AWBL_USBD_CFG_MAX_POWER_MA(v)  (v & 0x1FF)


/** \brief usbd task info */
#define AWBL_USBD_TASK_STACK_SIZE       2048
#define AWBL_USBD_TASK_PRIO             0


struct awbl_usbd_event {
    aw_spinlock_isr_t          lock;
    aw_usb_sem_handle_t        semb;
    uint32_t                   event;
};


/** \brief USB device transmission */
struct awbl_usbd_trans {
    struct awbl_usbd_ep  *p_hw;
    void                 *p_buf;
    void                 *p_buf_dma;
    size_t                len;
    void                (*complete) (void *p_arg);
    void                 *p_arg;
    size_t                act_len;            /* length of data that actually transfer */
    int                   status;             /* result */
    int                   flag;
};




/** USB设备端点*/
struct awbl_usbd_ep {
    uint8_t                 ep_addr;       /* 端点地址*/
    uint8_t                 type_support;  /* 支持类型*/
    uint8_t                 cur_type;      /* 当前类型*/
    uint16_t                mps_limt;      /* 最大包大小限制*/
    uint16_t                cur_mps;       /* 当前最大包大小*/
    struct aw_list_head     node;          /* 端点节点*/
    aw_bool_t               enable;        /* 使能标志位*/
    aw_bool_t               used;          /* 使用标志位*/
};




/** \brief USB device EndPoint0 */
struct awbl_usbd_ep0 {
    struct awbl_usbd       *p_usbd;
    struct awbl_usbd_ep    *p_hw;
    struct awbl_usbd_trans  req;
};




/** \brief USB device controller */
struct awbl_usbd {
    struct awbl_dev               awdev;
    struct aw_usbd               *p_obj;
    const struct awbl_usbd_cb    *p_cb;
    aw_usb_task_handle_t          usbd_task;
    aw_usb_mutex_handle_t         mutex;
    aw_usb_sem_handle_t           start;
    aw_usb_sem_handle_t           done;
    aw_spinlock_isr_t             lock;
    aw_bool_t                     run;
    uint8_t                       state;
    uint8_t                       resume_state;
    uint8_t                       speed;
    uint8_t                       addr;
    uint8_t                       mps0;
    struct aw_list_head           ep_list;
    struct awbl_usbd_ep0          ep0_in;
    struct awbl_usbd_ep0          ep0_out;
    aw_bool_t                     need_sta;
    struct aw_list_head           node;
    struct awbl_usbd_event        event;
    struct aw_usb_ctrlreq         setup;
};




/** \brief controller information */
struct awbl_usbd_info {
    const char  *name;
    uint32_t     flag;
};




/** \brief controller driver */
struct awbl_usbd_drv {
    /* AWorks driver */
    struct awbl_drvinfo awdrv;

    /* controller reset */
    void (*reset) (struct awbl_usbd *p_usbd);

    /* controller start */
    aw_err_t (*run) (struct awbl_usbd *p_usbd);

    /* controller stop */
    aw_err_t (*stop) (struct awbl_usbd *p_usbd);

    /* transfer request */
    aw_err_t (*transfer_req) (struct awbl_usbd       *p_usbd,
                              struct awbl_usbd_trans *p_trans);

    /* transfer cancel */
    aw_err_t (*transfer_cancel) (struct awbl_usbd       *p_usbd,
                                 struct awbl_usbd_trans *p_trans);

    /* enable EndPoint */
    aw_err_t (*ep_enable) (struct awbl_usbd    *p_usbd,
                           struct awbl_usbd_ep *p_ep);

    /* disable EndPoint */
    aw_err_t (*ep_disable) (struct awbl_usbd    *p_usbd,
                            struct awbl_usbd_ep *p_ep);

    /* reset EndPoint,
     * think that clear all EP status and config,
     * stack must enable it again if had already enable */
    aw_err_t (*ep_reset) (struct awbl_usbd    *p_usbd,
                          struct awbl_usbd_ep *p_ep);

    /* set USB address */
    aw_err_t (*set_address) (struct awbl_usbd  *p_usbd,
                             uint8_t            addr);

    /* set config for some controller */
    aw_err_t (*set_config) (struct awbl_usbd  *p_usbd,
                            aw_bool_t          set);

    /* get or set EndPoint's status */
    aw_err_t (*ep_status) (struct awbl_usbd    *p_usbd,
                           uint8_t              ep_addr,
                           aw_bool_t            is_set,
                           uint16_t            *status);

    /* set or clear EndPoint's feature */
    aw_err_t (*ep_feature) (struct awbl_usbd    *p_usbd,
                            uint8_t              ep_addr,
                            aw_bool_t            is_set);

    /* wake up controller */
    aw_err_t (*wakeup)(struct awbl_usbd *p_usbd);

    /* pull up or disconnect resistance on D+/D- */
    aw_err_t (*pullup)(struct awbl_usbd *p_usbd, aw_bool_t on);
};




/** \brief controller driver */
struct awbl_usbd_cb {
    /* received a control setup packet */
    int (*ctrl_setup) (struct awbl_usbd      *p_usbd,
                       struct aw_usb_ctrlreq *p_setup,
                       void                  *p_buf,
                       int                    buf_len);

    /* bus reset */
    void (*reset) (struct awbl_usbd *p_usbd);

    /* bus suspend */
    void (*suspend) (struct awbl_usbd *p_usbd);

    /* bus resume */
    void (*resume) (struct awbl_usbd  *p_usbd);

    /* bus disconnect */
    void (*disconnect) (struct awbl_usbd *p_usbd);
};




/** \brief create a controller by controller driver */
int awbl_usbd_create (struct awbl_usbd *p_usbd);




/** \brief register a EP by controller driver */
int awbl_usbd_ep_register (struct awbl_usbd    *p_usbd,
                           struct awbl_usbd_ep *p_ep,
                           uint8_t              ep_addr,
                           uint8_t              type_support,
                           uint16_t             mps_limt);




/** \brief report a event by controller driver */
int awbl_usbd_event_handle (struct awbl_usbd  *p_usbd,
                            uint32_t           event,
                            void              *arg);




/** \brief complete a transmission */
void awbl_usbd_trans_done (struct awbl_usbd_trans *p_trans, int status);




/** \brief allocate a EP from controller */
struct awbl_usbd_ep *awbl_usbd_ep_alloc (struct awbl_usbd  *p_usbd,
                                         uint8_t            addr,
                                         uint8_t            type,
                                         uint16_t           mps);




/** \brief free a EP */
int awbl_usbd_ep_free (struct awbl_usbd     *p_usbd,
                       struct awbl_usbd_ep  *p_ep);




/** \brief enable a EP */
int awbl_usbd_ep_enable (struct awbl_usbd    *p_usbd,
                         struct awbl_usbd_ep *p_ep);




/** \brief disable a EP */
int awbl_usbd_ep_disable (struct awbl_usbd    *p_usbd,
                          struct awbl_usbd_ep *p_ep);




/** \brief reaset a EP */
int awbl_usbd_ep_reset (struct awbl_usbd    *p_usbd,
                        struct awbl_usbd_ep *p_ep);




/** \brief EP feature set/clean, by STALL */
int awbl_usbd_ep_feature (struct awbl_usbd    *p_usbd,
                          struct awbl_usbd_ep *p_ep,
                          aw_bool_t            set);




/** \brief find a controller by name */
struct awbl_usbd *awbl_usbd_find_dev (const char *name);




/** \brief link a object to the controller and then start it */
int awbl_usbd_start (struct awbl_usbd            *p_usbd,
                     struct aw_usbd              *p_obj,
                     const struct awbl_usbd_cb   *p_cb);




/** \brief stop the controller and then unlink any object linked */
int awbl_usbd_stop (struct awbl_usbd  *p_usbd);




/** \brief submit a transmission */
int awbl_usbd_trans_req (struct awbl_usbd        *p_usbd,
                         struct awbl_usbd_trans  *p_trans);




/** \brief cancel a transmission */
int awbl_usbd_trans_cancel (struct awbl_usbd        *p_usbd,
                            struct awbl_usbd_trans  *p_trans);




/** \brief get controller's attribute */
int awbl_usbd_attribute_get (struct awbl_usbd  *p_usbd,
                             uint32_t          *attr);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBD_H */
