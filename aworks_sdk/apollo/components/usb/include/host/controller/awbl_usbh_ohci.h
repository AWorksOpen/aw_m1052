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



#ifndef __AWBL_USBH_OHCI_H
#define __AWBL_USBH_OHCI_H



#include "aw_pool.h"
#include "aw_timer.h"
#include "host/awbl_usbh.h"


#define AWBL_USBH_OHCI_TASK_STACK_SIZE  (8192)

#define __MAX_INT_NUM 32


/*
 * The HCCA (Host Controller Communications Area)
 * is a 256 byte structure, must be 256-byte aligned.
 */
struct awbl_usbh_ohci_hcca {
    uint32_t  int_table [__MAX_INT_NUM];    /* periodic schedule */
    uint16_t  frame_no;                     /* current frame number */
    uint16_t  pad;
    uint32_t  done_head;                    /* info returned for an interrupt */
    uint8_t   reserved[116];
    uint8_t   aligned[4];                   /* spec only identifies 252 bytes :) */
};




/* USB host controller OHCI */
struct awbl_usbh_ohci {
    struct aw_pool              eds;
    struct aw_pool              tds;

    uint8_t                     evt;
    aw_usb_sem_handle_t         sem;
    aw_usb_mutex_handle_t       sync;
    aw_usb_mutex_handle_t       lock;
    aw_usb_task_handle_t        task;
    aw_timer_t                  guarder;

    aw_bool_t                   intr;
    aw_bool_t                   first;



    aw_bool_t                   sw_support;
    uint8_t                     nposts;
    struct awbl_usbh_ohci_hcca *hcca;
    uint32_t                    reg_base;

    /* periodic bandwidth */
    int                         bandwidth[__MAX_INT_NUM];
    int                         periodic_req;

    /* ED list */
    struct aw_list_head         ctrl_list;
    struct aw_list_head         bulk_list;
    struct aw_list_head         remove_list;

    void (*pfn_rh_change) (struct awbl_usbh_ohci *p_ohci, uint16_t change);
};




struct awbl_usbh_ohci_ed {
    uint32_t                    hwinfo;
    uint32_t                    hwtailp;
    uint32_t                    hwheadp;
    uint32_t                    hwnexted;

    struct aw_list_head         node;
    struct aw_list_head         td_list;
    struct awbl_usbh_ohci      *p_ohci;
    struct awbl_usbh_endpoint  *p_ep;
    struct awbl_usbh_ohci_td   *dummy_td;
    uint16_t                    frame_no;
    uint8_t                     phase;
    uint16_t                    interval;
    uint8_t                     state;
};




struct awbl_usbh_ohci_td {
    uint32_t                hwinfo;
    uint32_t                hwcbp;
    uint32_t                hwnexttd;
    uint32_t                hwbe;
    struct awbl_usbh_trp   *p_trp;
    size_t                  len;
    struct aw_list_head     node;
};






aw_err_t awbl_usbh_ohci_init(struct awbl_usbh_ohci *p_ohci,
                            uint32_t                reg_base,
                            uint32_t                task_prio,
                            int                     neds,
                            int                     ntds,
                            void (*pfn_rh_change) (
                                    struct awbl_usbh_ohci *p_ohci,
                                    uint16_t               change));

void awbl_usbh_ohci_irq (struct awbl_usbh_ohci *p_ohci);

aw_err_t awbl_usbh_ohci_ep_enable (struct awbl_usbh_ohci     *p_ohci,
                                   struct awbl_usbh_endpoint *p_ep);

aw_err_t awbl_usbh_ohci_ep_disable (struct awbl_usbh_ohci     *p_ohci,
                                    struct awbl_usbh_endpoint *p_ep);

aw_err_t awbl_usbh_ohci_request (struct awbl_usbh_ohci *p_ohci,
                                 struct awbl_usbh_trp  *p_trp);

aw_err_t awbl_usbh_ohci_cancel (struct awbl_usbh_ohci *p_ohci,
                                struct awbl_usbh_trp  *p_trp);


aw_err_t awbl_usbh_ohci_rh_change_check (struct awbl_usbh_ohci *p_ohci,
                                         uint16_t              *p_change);
aw_err_t awbl_usbh_ohci_rh_ctrl (struct awbl_usbh_ohci *p_ohci,
                                 uint8_t                type,
                                 uint8_t                req,
                                 uint16_t               val,
                                 uint16_t               idx,
                                 uint16_t               len,
                                 void                  *p_buf);


aw_err_t awbl_usbh_ohci_mem_init (struct awbl_usbh_ohci *p_ohci,
                                  int                    neds,
                                  int                    ntds);

void awbl_usbh_ohci_mem_uninit (struct awbl_usbh_ohci *p_ohci);
struct awbl_usbh_ohci_ed *
awbl_usbh_ohci_ed_alloc (struct awbl_usbh_ohci *p_ohci);
aw_err_t awbl_usbh_ohci_ed_free (struct awbl_usbh_ohci      *p_ohci,
                                 struct awbl_usbh_ohci_ed   *p_ed);
struct awbl_usbh_ohci_td *
awbl_usbh_ohci_td_alloc (struct awbl_usbh_ohci *p_ohci);
aw_err_t awbl_usbh_ohci_td_free (struct awbl_usbh_ohci     *p_ohci,
                                 struct awbl_usbh_ohci_td  *p_td);

#endif /* __AWBL_USBH_OHCI_H */
