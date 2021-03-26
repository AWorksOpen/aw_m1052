/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx USB device controller driver head file
 *
 * \internal
 * \par modification history:
 * - 1.00 18-03-13  mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_USBD_H
#define __AWBL_IMX10xx_USBD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "device/awbl_usbd.h"
#include "aw_pool.h"
#include "aw_task.h"
#include "aw_sem.h"

#define AWBL_IMX10XX_USBD_DRV_NAME    "awbl_imx10xx_usbd"

#define  __USB_DEVICE_TASK_STACK_SIZE     1024


/** \brief imx10xx USB device EndPoint Queue Head (dQH) */
struct awbl_imx10xx_usbd_qh {
    uint32_t    info1;
#define __DQH_MULT_POS              30
#define __DQH_MULT_MASK             (3<<__DQH_MULT_POS)
#define __DQH_ZLT_POS               29
#define __DQH_ZLT_MASK              (1<<__DQH_ZLT_POS)
#define __DQH_MAX_PKT_POS           16
#define __DQH_MAX_PKT_MASK          (0x7FF<<__DQH_MAX_PKT_POS)
#define __DQH_MAX_PKT(qh)           (((qh)&__DQH_MAX_PKT_MASK)>>__DQH_MAX_PKT_POS)
#define __DQH_ISO_POS               15
#define __DQH_ISO_MASK              (1<<__DQH_ISO_POS)

    uint32_t    curr_dtd;
    uint32_t    next_dtd;
#define __DQH_NEXT_TERMINATE_MASK   1u

    uint32_t    info2;
#define __DQH_IOC_POS               15
#define __DQH_IOC_MASK              (1<<__DQH_IOC_POS)
#define __DQH_MULTO_POS             10
#define __DQH_MULTO_MASK            (3<<__DQH_MULTO_POS)
#define __DQH_STA_HALT_MASK         (1<<6)
#define __DQH_STA_ACTIVE_MASK       (1<<7)

    uint32_t    buff0;
#define __DQH_CURRENT_OFFSET_MASK   0x00000FFF
    uint32_t    buff1;
    uint32_t    buff2;
    uint32_t    buff3;
    uint32_t    buff4;
    uint32_t    reserved1;
    uint8_t     setup_buf[8];
    uint32_t    reserved2[4];
    /** \brief must be aligned on 64-byte boundaries. */
};

/** \brief imx10xx USB device transfer descriptor (dDT) */
struct awbl_imx10xx_usbd_td {
    uint32_t                    next;
#define __DTD_NEXT_TERMINATE_MASK       1u
#define __DTD_ADDR_MASK                 0xFFFFFFE0
    uint32_t                    token;
#define __DQH_TOTAL_BYTES_POS           16
#define __DQH_TOTAL_BYTES_MASK          (0x7FFF<<__DQH_TOTAL_BYTES_POS)
#define __DQH_TOTAL_BYTES(tkn)          (((tkn)&__DQH_TOTAL_BYTES_MASK)>>__DQH_TOTAL_BYTES_POS)
#define __DTD_IOC_POS                   15
#define __DTD_IOC_MASK                  (1<<__DTD_IOC_POS)
#define __DTD_STA_ACTIVE_POS            7
#define __DTD_STA_ACTIVE_MASK           (1<<__DTD_STA_ACTIVE_POS)
#define __DTD_STA_HALTED_POS            6
#define __DTD_STA_HALTED_MASK           (1<<__DTD_STA_HALTED_POS)
#define __DTD_STA_DATA_BUFF_ERR_POS     5
#define __DTD_STA_DATA_BUFF_ERR_MASK    (1<<__DTD_STA_DATA_BUFF_ERR_POS)
#define __DTD_STA_TRANSACTION_ERR_POS   3
#define __DTD_STA_TRANSACTION_ERR_MASK  (1<<__DTD_STA_TRANSACTION_ERR_POS)
    uint32_t                    buf[5];
    void                        *addr_dma;
    size_t                      len;
    struct aw_list_head         node;
    struct awbl_usbd_trans      *p_trans;
};

struct awbl_imx10xx_usbd_ep {
    struct awbl_usbd_ep         ep;
    struct awbl_imx10xx_usbd      *p_usbd;
    struct awbl_imx10xx_usbd_qh   *p_qh;
    struct aw_list_head         td_list;
    uint8_t                     state;
#define __EP_ST_ENABLE  0x00
#define __EP_ST_DISABLE 0x01
#define __EP_ST_RESET   0x02
};



/** \brief imx10xx USB device controller struct */
struct awbl_imx10xx_usbd {
    struct awbl_usbd            usbd;

    /** \brief EPs (IN and OUT) */
    struct awbl_imx10xx_usbd_ep   *eps;
    struct awbl_imx10xx_usbd_qh   *qhs;
    struct aw_pool              td_pool;

    /** \brief number of EPs (IN and OUT,include EP0) */
    int                         neps;

    aw_spinlock_isr_t           lock;

    /** \brief task signal */
    AW_SEMB_DECL(usbd_sem_sync);

    /** \brief task define */
    AW_TASK_DECL(__usb_device_task, __USB_DEVICE_TASK_STACK_SIZE);

};

/** \brief imx10xx USB device controller information */
struct awbl_imx10xx_usbd_info {
    struct awbl_usbd_info usbd_info;

    /** \brief controller register base */
    uint32_t    usb_regbase;

    /** \brief phy register base */
    uint32_t    phy_regbase;

    /** \brief IRQ number */
    int         inum;

    /** \brief USB device task  priority */
    uint32_t    task_prio;

    uint32_t    td_max;

    void (*pfunc_plfm_init)(void);
};


void awbl_imx10xx_usbd_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_USBD_H */
