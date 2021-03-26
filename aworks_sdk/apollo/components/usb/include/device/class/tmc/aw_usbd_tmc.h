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


#ifndef __AW_USBD_TMC_H
#define __AW_USBD_TMC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \addtogroup grp_aw_if_usbd_tmc
 * \copydoc aw_usbd_tmc.h
 * @{
 */
#include "device/aw_usbd.h"
#include "aw_list.h"
#include "aw_usb_os.h"

/* Thread required */
#define AW_USBD_TMC_STACK_SIZE      1024


/* These are all required */
#define USBD_TMC_REQ_INITIATE_ABORT_BULK_OUT             1
#define USBD_TMC_REQ_CHECK_ABORT_BULK_OUT_STATUS         2
#define USBD_TMC_REQ_INITIATE_ABORT_BULK_IN              3
#define USBD_TMC_REQ_CHECK_ABORT_BULK_IN_STATUS          4
#define USBD_TMC_REQ_INITIATE_CLEAR                      5
#define USBD_TMC_REQ_CHECK_CLEAR_STATUS                  6
#define USBD_TMC_REQ_GET_CAPABILITIES                    7
#define USBD_TMC_REQ_INDICATOR_PULSE                     64

/* USB TMC status values */
#define USBD_TMC_STATUS_SUCCESS                          1
#define USBD_TMC_STATUS_PENDING                          2
#define USBD_TMC_STATUS_FAILED                           0x80
#define USBD_TMC_STATUS_TRANSFER_NOT_IN_PROGRESS         0x81
#define USBD_TMC_STATUS_SPLIT_NOT_IN_PROGRESS            0x82
#define USBD_TMC_STATUS_SPLIT_IN_PROGRESS                0x83

#define USBD_TMC_INTERFACE_CAPABILITY_INDICATOR_PULSE    (1<<2)
#define USBD_TMC_INTERFACE_CAPABILITY_TALK_ONLY          (1<<1)
#define USBD_TMC_INTERFACE_CAPABILITY_LISTEN_ONLY        (1<<0)
#define USBD_TMC_DEVICE_CAPABILITY_TERMCHAR              (1<<0)

/* MsgId values */
#define USBD_TMC_MSGID_OUT_DEV_DEP_MSG_OUT                 1
#define USBD_TMC_MSGID_OUT_REQUEST_DEV_DEP_MSG_IN          2
#define USBD_TMC_MSGID_IN_DEV_DEP_MSG_IN                   2
#define USBD_TMC_MSGID_OUT_VENDOR_SPECIFIC_OUT             126
#define USBD_TMC_MSGID_OUT_REQUEST_VENDOR_SPECIFIC_IN      127
#define USBD_TMC_MSGID_IN_VENDOR_SPECIFIC_IN               127

/* MSG SIZE */
#define USBD_TMC_MSG_HEADER_SIZE                           12


typedef struct aw_usbd_tmc_bulk_header {
    uint8_t msg_id;
    uint8_t b_tag;
    uint8_t b_tag_inverse;
    uint8_t reserved;

    union {
        struct _dev_dep_msg_out {
            uint32_t size;
            uint8_t  attributes;
            uint8_t  reserved[3];
        } dev_dep_msg_out;

        struct _req_dev_dep_msg_in {
            uint32_t size;
            uint8_t  attributes;
            uint8_t  term_char;
            uint8_t  reserved[2];
        } req_dev_dep_msg_in;

        struct _dev_dep_msg_in {
            uint32_t size;
            uint8_t  attributes;
            uint8_t  reserved[3];
        } dev_dep_msg_in;

        struct _vendor_specific_out {
            uint32_t size;
            uint8_t  reserved[4];
        } vendor_specific_out;

        struct _req_vendor_specific_in {
            uint32_t size;
            uint8_t  reserved[4];
        } req_vendor_specific_in;

        struct _vendor_specific_in {
            uint32_t size;
            uint8_t  reserved[4];
        } vendor_specific_in;

        uint8_t raw[8];
    } command_specific;

    uint8_t  payload[0];
} __attribute__((packed)) aw_usbd_tmc_bulk_header_t;


#define USBD_TMC_CMD_LIST_END          {NULL, NULL}

typedef aw_err_t (*aw_usbd_tmc_cmd_callback_t) (void *);

typedef struct aw_usbd_tmc_command {
    const char                *cmd;
    aw_usbd_tmc_cmd_callback_t callback;
}aw_usbd_tmc_command_t;


/** \brief usb tmc object */
typedef struct aw_usbd_tmc {
    struct aw_usbd_fun          fun;
    struct aw_usbd_pipe         int_in;
    struct aw_usbd_pipe         int_out;
    uint8_t                     name[32];
    uint8_t                     b_tag;

    aw_usb_task_handle_t        tmc_task;
    aw_usb_sem_handle_t         semb;
    aw_usb_sem_handle_t         rec_semb;
    aw_usb_mutex_handle_t       mutex;

    struct aw_list_head         node;
}aw_usbd_tmc_t;


aw_err_t aw_usbd_tmc_create(struct aw_usbd        *p_obj,
                            struct aw_usbd_tmc    *p_tmc,
                            const char            *p_name);
/** @} grp_aw_if_usbd_tmc */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __AW_USBD_TMC_H */

/* end of file */
