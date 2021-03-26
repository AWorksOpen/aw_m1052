/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus CAN bus type implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.02 17-09-02  anu, modify canfd and rngbuf.
 * - 1.01 17-07-25  zoe, support canfd and adapt old interface.
 * - 1.00 15-12-02  cae, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_rngbuf.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "aw_common.h"
#include "awbl_can.h"
#include "string.h"

awbl_can_service_t     *__gp_can_serv_head = NULL;
aw_can_app_callbacks_t  __g_app_call_back;

/******************************************************************************/
aw_err_t awbl_can_service_register (awbl_can_service_t *p_serv)
{
    if (NULL == p_serv->p_servfuncs) {
        return -AW_ENXIO;
    }

    p_serv->p_next = __gp_can_serv_head;
    __gp_can_serv_head = p_serv;
    return AW_OK;
}

/******************************************************************************/
awbl_can_service_t *__can_chn_to_serv (int chn)
{
    awbl_can_service_t *p_serv_cur = __gp_can_serv_head;

    while ((p_serv_cur != NULL)) {

        if (chn == p_serv_cur->p_info->chn)  {
            return p_serv_cur;
        }
        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}

/******************************************************************************/
aw_err_t awbl_can_service_init (awbl_can_service_t   *p_serv,
                                awbl_can_servinfo_t  *p_servinfo,
                                awbl_can_drv_funcs_t *p_servfuncs)
{

    p_serv->p_info      = p_servinfo;
    p_serv->p_servfuncs = p_servfuncs;

    aw_spinlock_isr_init(&p_serv->lock_dev, 0);
    AW_MUTEX_INIT(p_serv->dev_mux, AW_SEM_Q_PRIORITY );
    AW_INIT_LIST_HEAD(&p_serv->transfer_list);

    if ((p_serv->p_info->p_rxd_buf     != NULL) &&
        (p_serv->p_info->p_rxd_buf_ctr != NULL)) {
        aw_rngbuf_init(p_serv->p_info->p_rxd_buf_ctr,
                      (char*)p_serv->p_info->p_rxd_buf,
                       p_serv->p_info->rxd_buf_size *
                       p_serv->p_info->sizeof_msg + 1);
    }

    memset(&__g_app_call_back, 0x00, sizeof(__g_app_call_back));

    AW_SEMB_INIT(p_serv->rcv_start_sem,
                 AW_SEM_EMPTY,
                 AW_SEM_Q_PRIORITY);
    return AW_OK;
}

/******************************************************************************/
aw_static_inline void __can_transfer_in (
        awbl_can_service_t *p_serv, aw_can_transfer_t *p_transfer)
{
    aw_list_add_tail((struct aw_list_head *)(&p_transfer->ctlrdata),
                     &p_serv->transfer_list);
}

/******************************************************************************/
aw_static_inline aw_can_transfer_t * __can_transfer_out (
        awbl_can_service_t *p_serv)
{
    if (aw_list_empty_careful(&p_serv->transfer_list)) {
        return NULL;
    } else {
        struct aw_list_head *p_node = p_serv->transfer_list.next;
        aw_list_del(p_node);
        return aw_list_entry(p_node, aw_can_transfer_t, ctlrdata);
    }
}

/******************************************************************************/
aw_static_inline aw_err_t __can_transfer_delet (awbl_can_service_t *p_serv)
{
    aw_list_head_init(&p_serv->transfer_list);
    p_serv->p_curr_trans = NULL;
    p_serv->txd_busy = AW_FALSE;
    return AW_OK;
}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_stop (awbl_can_service_t *p_serv)
{
    if (p_serv->p_servfuncs->pfn_can_stop != NULL) {
        return p_serv->p_servfuncs->pfn_can_stop(p_serv);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;

}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_int_enable (
        awbl_can_service_t *p_serv, awbl_can_int_type_t int_mask)
{
    if (p_serv->p_servfuncs->pfn_can_int_enable != NULL) {
        return p_serv->p_servfuncs->pfn_can_int_enable(p_serv, int_mask);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_int_disable (
        awbl_can_service_t *p_serv, awbl_can_int_type_t int_mask)
{
    if (p_serv->p_servfuncs->pfn_can_int_disable != NULL) {
        return p_serv->p_servfuncs->pfn_can_int_disable(p_serv, int_mask);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_baud_set (
        awbl_can_service_t *p_serv, const aw_can_baud_param_t *p_baud)
{
    if (p_serv->p_servfuncs->pfn_can_baud_set != NULL) {
        return p_serv->p_servfuncs->pfn_can_baud_set(p_serv, p_baud);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_err_cnt_clr (awbl_can_service_t *p_serv)
{
    if (p_serv->p_servfuncs->pfn_can_err_cnt_clr != NULL) {
        return p_serv->p_servfuncs->pfn_can_err_cnt_clr(p_serv);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_err_cnt_get (
        awbl_can_service_t *p_serv, aw_can_err_cnt_t *p_err_reg)
{
    if (p_serv->p_servfuncs->pfn_can_err_cnt_get != NULL) {
        return p_serv->p_servfuncs->pfn_can_err_cnt_get(p_serv, p_err_reg);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_static_inline aw_can_bus_err_t __can_int_status_get (
        awbl_can_service_t  *p_serv,
        awbl_can_int_type_t *p_int_type,
        aw_can_bus_err_t    *p_bus_err)
{
    if (p_serv->p_servfuncs->pfn_can_int_status_get != NULL) {
        return p_serv->p_servfuncs->pfn_can_int_status_get(
                p_serv, p_int_type, p_bus_err);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_reg_msg_read (
        awbl_can_service_t *p_serv, aw_can_msg_t *p_msgs)
{
    if (p_serv->p_servfuncs->pfn_can_reg_msg_read != NULL) {
        return p_serv->p_servfuncs->pfn_can_reg_msg_read(p_serv, p_msgs);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_reg_msg_write (
        awbl_can_service_t *p_serv, aw_can_msg_t *p_msg)
{
    if (p_serv->p_servfuncs->pfn_can_reg_msg_write != NULL) {
        return p_serv->p_servfuncs->pfn_can_reg_msg_write(p_serv, p_msg);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_msg_tx_startup (awbl_can_service_t *p_serv)
{
    aw_can_err_t ret;
    
    /* if the controller is already launched, just return */
    AWBL_CAN_CONTROLLER_LOCK(p_serv);
    if (p_serv->txd_busy ){
        AWBL_CAN_CONTROLLER_UNLOCK(p_serv);
        return -AW_CAN_ERR_BUSY;
    }
    p_serv->txd_busy = AW_TRUE;
    p_serv->p_curr_trans = __can_transfer_out(p_serv);
    AWBL_CAN_CONTROLLER_UNLOCK(p_serv);

    ret = __can_reg_msg_write(p_serv,
                              (aw_can_msg_t*)p_serv->p_curr_trans->p_msgs);
    if (ret != AW_CAN_ERR_NONE) {
        AWBL_CAN_CONTROLLER_LOCK(p_serv);
        p_serv->txd_busy = AW_FALSE;
        AWBL_CAN_CONTROLLER_UNLOCK(p_serv);
    }
    
    return ret;
}

/******************************************************************************/
aw_local void __can_sync_complete (void *p_arg)
{
    if (p_arg) {
        aw_semb_give(p_arg);
    }
}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_msgs_stop_send (awbl_can_service_t *p_serv)
{
    if (p_serv->p_servfuncs->pfn_can_msg_snd_stop != NULL) {
        return p_serv->p_servfuncs->pfn_can_msg_snd_stop(p_serv);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_async (awbl_can_service_t *p_serv,
                                           aw_can_transfer_t  *p_transfer)
{
    aw_can_ctrl_type_t ctrl_type;

    uint32_t      i;
    aw_can_err_t  err;
    aw_can_msg_t *p_msgs = p_transfer->p_msgs;

    if (p_serv->chn_status != AWBL_CAN_CHN_START) {
        return -AW_CAN_ERR_CTRL_NOT_START;
    }

    /* check message is right */
    for (i = 0; i < p_transfer->msgs_num; i++) {
        ctrl_type = (p_msgs->flags & AW_CAN_MSG_FLAG_CTRL_TYPE_MASK) >>
                     AW_CAN_MSG_FLAG_CTRL_TYPE_SHIFT;
        if (p_serv->p_info->ctr_type == AW_CAN_CTLR_HS) {

            if (ctrl_type != AW_CAN_CTLR_HS) {
                return -AW_CAN_ERR_INCOMPATIBLE_CTRL_TYPE;
            }

        } else if (p_serv->p_info->ctr_type == AW_CAN_CTLR_FD) {

            if (ctrl_type != AW_CAN_CTLR_HS && ctrl_type != AW_CAN_CTLR_FD) {
                return -AW_CAN_ERR_INCOMPATIBLE_CTRL_TYPE;
            }

            if ((ctrl_type == AW_CAN_CTLR_FD) &&
                (p_msgs->flags & AW_CAN_MSG_FLAG_REMOTE)) {
                return -AW_CAN_ERR_ILLEGAL_MASK_VALUE;
            }

        } else {
            return -AW_CAN_ERR_INCOMPATIBLE_CTRL_TYPE;
        }

        if ((ctrl_type == AW_CAN_CTLR_HS) &&
            (p_msgs->flags &
            (AW_CAN_MSG_FLAG_ERROR |
             AW_CAN_MSG_FLAG_BRS   |
             AW_CAN_MSG_FLAG_ESI))) {
            return -AW_CAN_ERR_ILLEGAL_MASK_VALUE;
        }

        p_msgs = (aw_can_msg_t*)((uint8_t*)p_msgs + p_serv->p_info->sizeof_msg);
    }

    /* message is not queueing */
    p_transfer->status   = -AW_ENOTCONN;

    /* check if each transfer is valid */
    p_transfer->done_num = 0;

    /* add transfer to contoller's transfer list */
    AWBL_CAN_CONTROLLER_LOCK(p_serv);
    __can_transfer_in(p_serv, p_transfer);
    p_transfer->status = -AW_EISCONN;
    AWBL_CAN_CONTROLLER_UNLOCK(p_serv);

    /* launch controller */
    err =  __can_msg_tx_startup(p_serv);
    if (err == AW_CAN_ERR_NONE) {
        AWBL_CAN_CONTROLLER_LOCK(p_serv);
        if (p_serv->p_curr_trans != NULL) {
            p_serv->p_curr_trans->status = -AW_EINPROGRESS;
        }
        AWBL_CAN_CONTROLLER_UNLOCK(p_serv);
    }
    return err;
}

/******************************************************************************/
aw_static_inline aw_can_err_t __can_sync (awbl_can_service_t *p_serv,
                                          aw_can_transfer_t  *p_transfer,
                                          uint32_t            timeout)
{
    aw_can_err_t     err;
    aw_err_t         ret;

    /* allocate synchronization semaphore from stack */
    AW_SEMB_DECL(sem_sync);
    aw_semb_id_t sem_sync_id;

    /* initialize the stack binary semaphore */
    memset(&sem_sync,0,sizeof(sem_sync));
    sem_sync_id = AW_SEMB_INIT(sem_sync, 0, AW_SEM_Q_FIFO);
    if (sem_sync_id == 0) {
        AW_MUTEX_UNLOCK(p_serv->dev_mux);
        return -AW_CAN_ERR_NO_OP;
    }

    /* the synchronous callback */
    p_transfer->p_arg          = sem_sync_id;
    p_transfer->pfunc_complete = __can_sync_complete;

    /* send message synchronously */
    err = __can_async (p_serv, p_transfer);
    if (err != AW_CAN_ERR_NONE) {
        goto exit;
    }

    /* waiting for message transfer done */
    ret = aw_semb_take(sem_sync_id, timeout);

    /* timeout, take out the message from queue */
    if (ret == -AW_ETIME) {
        AWBL_CAN_CONTROLLER_LOCK(p_serv);
        p_transfer->status = -AW_ETIME;
        AWBL_CAN_CONTROLLER_UNLOCK(p_serv);
        err = __can_msgs_stop_send(p_serv);
        if (err == -AW_CAN_ERR_ABORT_TRANSMITTED) {
           p_transfer->done_num++;
        }
        AWBL_CAN_CONTROLLER_LOCK(p_serv);
        __can_transfer_delet(p_serv);
        AWBL_CAN_CONTROLLER_UNLOCK(p_serv);

        err = -AW_CAN_ERR_TRANSMITTED_TIMEOUT;
    }

exit:
    AWBL_CAN_CONTROLLER_LOCK(p_serv);
    p_transfer->p_arg = NULL;
    AWBL_CAN_CONTROLLER_UNLOCK(p_serv);
    aw_semb_terminate(sem_sync_id);
    AW_MUTEX_UNLOCK(p_serv->dev_mux);

    if (p_transfer->status == -AW_ENETDOWN) {
        err = -AW_CAN_ERR_BUS_FAULT;
    }
    return err;
}

/******************************************************************************/
aw_can_err_t aw_can_start (int chn)
{
    aw_can_err_t        ret    = 0;
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);

    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }

    if (p_serv->chn_status == AWBL_CAN_CHN_NONE) {
        return -AW_CAN_ERR_NOT_INITIALIZED;
    }

    if (p_serv->p_servfuncs->pfn_can_start != NULL) {
        ret = p_serv->p_servfuncs->pfn_can_start(p_serv);
    }

    if (ret == AW_CAN_ERR_NONE) {
        p_serv->chn_status = AWBL_CAN_CHN_START;
        return ret;
    }

    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_can_err_t aw_can_stop (int chn)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }
    p_serv->chn_status = AWBL_CAN_CHN_STOP;
    return __can_stop(p_serv);
}

/******************************************************************************/
aw_can_err_t aw_can_sleep (int chn)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }
    if (p_serv->p_servfuncs->pfn_can_sleep != NULL) {
        return p_serv->p_servfuncs->pfn_can_sleep(p_serv);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_can_err_t aw_can_wakeup (int chn)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);
    p_serv = __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }
    if (p_serv->p_servfuncs->pfn_can_wakeup != NULL) {
        return p_serv->p_servfuncs->pfn_can_wakeup(p_serv);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_can_err_t aw_can_baudrate_set (int chn, aw_can_baud_param_t *p_baud)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);

    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }

    if (p_serv->chn_status == AWBL_CAN_CHN_NONE) {
        return -AW_CAN_ERR_NOT_INITIALIZED;
    }

    return __can_baud_set(p_serv, p_baud);
}

/******************************************************************************/
aw_can_err_t aw_can_baudrate_get (int chn, aw_can_baud_param_t *p_baud)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);

    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }

    if (p_serv->chn_status == AWBL_CAN_CHN_NONE) {
        return -AW_CAN_ERR_NOT_INITIALIZED;
    }

    if (p_serv->p_servfuncs->pfn_can_baud_get != NULL) {
        return p_serv->p_servfuncs->pfn_can_baud_get(p_serv, p_baud);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_can_err_t aw_can_err_cnt_clr (int chn)
{
    awbl_can_service_t *p_serv= __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }
    return __can_err_cnt_clr(p_serv);
}

/******************************************************************************/
aw_can_err_t aw_can_err_cnt_get (int chn, aw_can_err_cnt_t *p_err_reg)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }
    return __can_err_cnt_get(p_serv, p_err_reg);
}

/******************************************************************************/
aw_can_err_t aw_can_filter_table_set (int chn, uint8_t *p_table, size_t length)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }
    if (p_serv->p_servfuncs->pfn_can_filter_table_set != NULL) {
        return p_serv->p_servfuncs->pfn_can_filter_table_set(
                p_serv, p_table, length);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_can_err_t aw_can_filter_table_get (
        int chn, uint8_t *p_table, size_t *p_length)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }
    if (p_serv->p_servfuncs->pfn_can_filter_table_get != NULL) {
        return p_serv->p_servfuncs->pfn_can_filter_table_get(
                p_serv, p_table, p_length);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_can_err_t aw_can_reg_write (
        int chn, uint32_t offset, uint8_t *p_data, size_t length)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }
    if (p_serv->p_servfuncs->pfn_can_reg_write != NULL) {
        return p_serv->p_servfuncs->pfn_can_reg_write(
                p_serv, offset, p_data, length);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_can_err_t aw_can_reg_read (
        int chn, uint32_t offset, uint8_t *p_data, size_t length)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }
    if (p_serv->p_servfuncs->pfn_can_reg_read != NULL) {
        return p_serv->p_servfuncs->pfn_can_reg_read(
                p_serv, offset, p_data, length);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_can_err_t aw_can_reg_msg_write (int chn, aw_can_msg_t *p_msg)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }
    if (p_serv->p_servfuncs->pfn_can_reg_msg_write != NULL) {
        return p_serv->p_servfuncs->pfn_can_reg_msg_write(p_serv, p_msg);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_can_err_t aw_can_reg_msg_read (int chn, aw_can_msg_t *p_msg)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }
    if (p_serv->p_servfuncs->pfn_can_reg_msg_read != NULL) {
            return p_serv->p_servfuncs->pfn_can_reg_msg_read(p_serv, p_msg);
    }
    return -AW_CAN_ERR_NONE_DRIVER_FUNC;
}


/******************************************************************************/
aw_can_err_t aw_can_init (int                        chn,
                          aw_can_work_mode_type_t    work_mode,
                          const aw_can_baud_param_t *p_baud,
                          aw_can_app_callbacks_t    *p_app_cb)
{
    aw_can_err_t        err;
    awbl_can_service_t *p_serv  = __can_chn_to_serv(chn);
    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }

    p_serv->chn_status     = AWBL_CAN_CHN_STOP;
    p_serv->p_app_cb_funcs = p_app_cb;

    if (p_app_cb == NULL) {
        p_serv->p_app_cb_funcs = &__g_app_call_back;
    }

    if ((p_app_cb == NULL)                               ||
        (p_serv->p_app_cb_funcs->proto_rx_cb    == NULL) ||
        (p_serv->p_app_cb_funcs->proto_tx_cb    == NULL) ||
        (p_serv->p_app_cb_funcs->bus_err_sta_cb == NULL) ||
        (p_serv->p_app_cb_funcs->dev_wakeup_cb  == NULL)) {

        if ((p_serv->p_info->p_rxd_buf     == NULL)  ||
            (p_serv->p_info->p_rxd_buf_ctr == NULL)) {
            return -AW_CAN_ERR_NOT_INITIALIZED;
        }
    }

    if (p_serv->p_servfuncs->pfn_can_reg_msg_read == NULL) {
        return -AW_CAN_ERR_NONE_DRIVER_FUNC;
    }

    if (p_serv->p_servfuncs->pfn_can_reg_msg_write == NULL) {
        return -AW_CAN_ERR_NONE_DRIVER_FUNC;
    }

    if (p_serv->p_servfuncs->pfn_can_int_status_get == NULL) {
        return -AW_CAN_ERR_NONE_DRIVER_FUNC;
    }

    if(p_serv->p_servfuncs->pfn_can_init != NULL) {
        p_serv->p_servfuncs->pfn_can_init(p_serv, work_mode);
    }

    /** \brief  Disables interrupts at the CAN controller */
    err = __can_int_disable (p_serv, AWBL_CAN_INT_ALL);
    if (err != AW_CAN_ERR_NONE){
        return err;
    }

    /** \brief Set baud rate */
    err = __can_baud_set(p_serv, p_baud);
    if (err != AW_CAN_ERR_NONE){
        return err;
    }

    /** \brief Clears error counters */
    __can_err_cnt_clr(p_serv);

    /** \brief  Enable interrupts at the CAN controller. */
    err = __can_int_enable(p_serv, AWBL_CAN_INT_ALL);

    if (err != AW_CAN_ERR_NONE){
        return err;
    }

    p_serv->chn_status = AWBL_CAN_CHN_INIT;

    return AW_CAN_ERR_NONE;
}

/******************************************************************************/
void aw_can_mkmsg (aw_can_transfer_t  *p_transfer,
                   aw_can_msg_t       *p_msgs,
                   uint16_t            msgs_num,
                   aw_pfuncvoid_t      pfunc_complete,
                   void               *p_arg)
{
   p_transfer->p_msgs         = p_msgs;
   p_transfer->msgs_num       = msgs_num;
   p_transfer->done_num       = 0;
   p_transfer->pfunc_complete = pfunc_complete;
   p_transfer->p_arg          = p_arg;
   p_transfer->status         = -AW_ENOTCONN;
}

/******************************************************************************/
aw_can_err_t aw_can_msgs_send_async (int chn, aw_can_transfer_t *p_transfer)
{

    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);

    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }

    if ((p_transfer           == NULL) ||
        (p_transfer->p_msgs   == NULL) ||
        (p_transfer->msgs_num == 0     )) {
        return -AW_CAN_ERR_INVALID_PARAMETER;
    }

     return __can_async(p_serv, p_transfer);
}

/******************************************************************************/
aw_can_err_t aw_can_msgs_send_sync (
        int chn, aw_can_transfer_t *p_transfer, uint32_t timeout)
{

    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);

    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }

    if ((timeout              == 0   ) ||
        (p_transfer           == NULL) ||
        (p_transfer->p_msgs   == NULL) ||
        (p_transfer->msgs_num == 0     )) {
         return -AW_CAN_ERR_INVALID_PARAMETER;
    }

    /* lock this function */
    AW_MUTEX_LOCK(p_serv->dev_mux, AW_SEM_WAIT_FOREVER);

    return __can_sync(p_serv, p_transfer, timeout);
}

/******************************************************************************/
aw_can_err_t aw_can_std_msgs_send (int               chn,
                                   aw_can_std_msg_t *p_msgs,
                                   uint32_t          msgs_num,
                                   uint32_t         *p_done_num,
                                   uint32_t          timeout)
{
    aw_can_err_t ret = AW_CAN_ERR_NONE;

    aw_can_transfer_t transfer = {0};

    transfer.p_msgs   = (aw_can_msg_t*)p_msgs;
    transfer.msgs_num = msgs_num;
    ret = aw_can_msgs_send_sync(chn, &transfer, timeout);
    *p_done_num = transfer.done_num;
    return ret;
}

/******************************************************************************/
aw_can_err_t aw_can_fd_msgs_send (int              chn,
                                  aw_can_fd_msg_t *p_msgs,
                                  uint32_t         msgs_num,
                                  uint32_t        *p_done_num,
                                  uint32_t         timeout)
{
    aw_can_err_t ret  = AW_CAN_ERR_NONE;
    aw_can_transfer_t transfer = {0};

    transfer.p_msgs   = (aw_can_msg_t*)p_msgs;
    transfer.msgs_num = msgs_num;
    ret = aw_can_msgs_send_sync(chn, &transfer, timeout);
    *p_done_num = transfer.done_num;
    return ret;
}

/******************************************************************************/
aw_can_err_t aw_can_msgs_stop_send (int chn)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);
    aw_can_err_t        ret    = AW_CAN_ERR_NONE;

    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }

    AWBL_CAN_CONTROLLER_LOCK(p_serv);
    if (p_serv->p_curr_trans != NULL) {
        if ((p_serv->p_curr_trans->status == -AW_EINPROGRESS) ||
            (p_serv->p_curr_trans->status == -AW_ENETDOWN)) {
            AWBL_CAN_CONTROLLER_UNLOCK(p_serv);
            __can_msgs_stop_send(p_serv);
        } else {
            AWBL_CAN_CONTROLLER_UNLOCK(p_serv);
        }
    } else {
        AWBL_CAN_CONTROLLER_UNLOCK(p_serv);
    }

    AWBL_CAN_CONTROLLER_LOCK(p_serv);
    ret = __can_transfer_delet(p_serv);
    AWBL_CAN_CONTROLLER_UNLOCK(p_serv);
    return ret;
}

/******************************************************************************/
uint32_t aw_can_rcv_msgs_num (int chn)
{
    uint32_t bytes_cnt;

    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);

    if (p_serv == NULL) {
        return 0;
    }
    bytes_cnt = aw_rngbuf_nbytes(p_serv->p_info->p_rxd_buf_ctr);

    return  (bytes_cnt / p_serv->p_info->sizeof_msg);
}

/******************************************************************************/
uint32_t aw_can_rcv_fifo_size (int chn)
{
    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);

    if (p_serv == NULL) {
        return 0;
    }
    return (p_serv->p_info->rxd_buf_size);
}

/******************************************************************************/
aw_can_err_t aw_can_msgs_rcv (
        int chn, aw_can_transfer_t *p_transfer, uint32_t timeout)
{
    uint32_t bytes_cnt = 0;

    awbl_can_service_t *p_serv = __can_chn_to_serv(chn);

    if (p_serv == NULL) {
        return -AW_CAN_ERR_ILLEGAL_CHANNEL_NO;
    }

    if ((p_serv->p_info->p_rxd_buf     == NULL)  ||
        (p_serv->p_info->rxd_buf_size  == 0   )  ||
        (p_serv->p_info->p_rxd_buf_ctr == NULL)) {

        return -AW_CAN_ERR_RXD_BUFF_UNUSED;
    }

    if (p_serv->chn_status != AWBL_CAN_CHN_START) {
        return -AW_CAN_ERR_CTRL_NOT_START;
    }

    if ((p_transfer           == NULL) ||
        (p_transfer->p_msgs   == NULL) ||
        (p_transfer->msgs_num == 0     )) {
         return -AW_CAN_ERR_INVALID_PARAMETER;
    }

    AW_SEMB_TAKE(p_serv->rcv_start_sem, timeout);

    bytes_cnt = aw_rngbuf_get(
            p_serv->p_info->p_rxd_buf_ctr,
            (char*)p_transfer->p_msgs,
            p_serv->p_info->sizeof_msg * p_transfer->msgs_num);
    p_transfer->done_num = bytes_cnt / p_serv->p_info->sizeof_msg;

    if (aw_rngbuf_nbytes(p_serv->p_info->p_rxd_buf_ctr)) {
        AW_SEMB_GIVE(p_serv->rcv_start_sem);
    }
    return AW_CAN_ERR_NONE;
}
/******************************************************************************/
aw_can_err_t aw_can_std_msgs_rcv (int               chn,
                                  aw_can_std_msg_t *p_msgs,
                                  uint32_t          msgs_num,
                                  uint32_t         *p_done_num,
                                  uint32_t          timeout)
{
    aw_can_err_t      ret      = AW_CAN_ERR_NONE;
    aw_can_transfer_t transfer = {NULL, 0, 0, NULL, NULL, 0, {NULL, NULL}};

    transfer.p_msgs   = (aw_can_msg_t*)p_msgs;
    transfer.msgs_num = msgs_num;

    ret = aw_can_msgs_rcv(chn, &transfer, timeout);

    *p_done_num = transfer.done_num;

    return ret;
}

/******************************************************************************/
aw_can_err_t aw_can_fd_msgs_rcv (int              chn,
                                 aw_can_fd_msg_t *p_msgs,
                                 uint32_t         msgs_num,
                                 uint32_t        *p_done_num,
                                 uint32_t         timeout)
{
    aw_can_err_t ret = AW_CAN_ERR_NONE;
    aw_can_transfer_t transfer = {0};

    transfer.p_msgs   = (aw_can_msg_t*)p_msgs;
    transfer.msgs_num = msgs_num;

    ret = aw_can_msgs_rcv(chn, &transfer, timeout);

    *p_done_num = transfer.done_num;
    return ret;
}

/******************************************************************************/
void awbl_can_isr_handler (awbl_can_service_t *p_serv)
{
    aw_can_fd_msg_t     can_rxd_msg = {{0},{0}};
    aw_can_msg_t       *p_can_txd_msg;
    aw_can_err_cnt_t    can_err_cnt = {0};
    aw_can_bus_err_t    can_bus_err =  0;
    aw_can_bus_status_t can_bus_sta =  0;

    awbl_can_int_type_t int_status  =  0;
    uint32_t rcvbytes = 0;

    while(1) {

        /** \brief get the interrupt status of can controller*/
        __can_int_status_get (p_serv, &int_status, &can_bus_err);

        if (int_status == AWBL_CAN_INT_NONE) {
            break;
        }

        /** \brief successful transmission interrupt */
        if (int_status & AWBL_CAN_INT_TX) {
            if ((NULL  == p_serv->p_app_cb_funcs->proto_tx_cb) ||
                (! p_serv->p_app_cb_funcs->proto_tx_cb(
                       p_serv->p_info->chn))) {
                if (p_serv->p_curr_trans != NULL) {
                    AWBL_CAN_CONTROLLER_LOCK(p_serv);
                    ++p_serv->p_curr_trans->done_num;
                    AWBL_CAN_CONTROLLER_UNLOCK(p_serv);

                    if (p_serv->p_curr_trans->status != -AW_ETIME) {
                        if (p_serv->p_curr_trans->done_num  <
                            p_serv->p_curr_trans->msgs_num) {

                            AWBL_CAN_CONTROLLER_LOCK(p_serv);
                            p_can_txd_msg = (aw_can_msg_t *)(
                                    (uint8_t *)p_serv->p_curr_trans->p_msgs +
                                     p_serv->p_curr_trans->done_num *
                                     p_serv->p_info->sizeof_msg);
                            AWBL_CAN_CONTROLLER_UNLOCK(p_serv);
                            __can_reg_msg_write(p_serv, p_can_txd_msg);

                        } else {

                            /** \brief notify the caller */
                            if (p_serv->p_curr_trans->pfunc_complete != NULL) {
                                p_serv->p_curr_trans->pfunc_complete(
                                        p_serv->p_curr_trans->p_arg);
                            }

                            AWBL_CAN_CONTROLLER_LOCK(p_serv);
                            p_serv->p_curr_trans->status = AW_OK;
                            /* \brief get out the next transfer */
                            p_serv->p_curr_trans = __can_transfer_out(p_serv);
                            AWBL_CAN_CONTROLLER_UNLOCK(p_serv);

                            if (p_serv->p_curr_trans != NULL){
                                __can_reg_msg_write(
                                        p_serv, p_serv->p_curr_trans->p_msgs);
                            } else {
                                p_serv->txd_busy = AW_FALSE;
                            }
                        }
                    }  else {
                        /** \brief notify the caller */
                        if (p_serv->p_curr_trans->pfunc_complete != NULL) {
                            p_serv->p_curr_trans->pfunc_complete(
                                    p_serv->p_curr_trans->p_arg);
                        }
                    }
                }
            }
        }

        if (int_status & AWBL_CAN_INT_STATUS) {
            if (int_status & AWBL_CAN_INT_BUS_OFF) {
                can_bus_sta = AW_CAN_BUS_STATUS_OFF;
            } else if (int_status & AWBL_CAN_INT_WARN) {
                can_bus_sta = AW_CAN_BUS_STATUS_WARN;
            } else if (int_status & AWBL_CAN_INT_ERROR_PASSIVE) {
                can_bus_sta = AW_CAN_BUS_STATUS_PASSIVE;
            } else if (int_status & AWBL_CAN_INT_ERROR) {
                can_bus_sta = AW_CAN_BUS_STATUS_ERROR;
            } else if (int_status & AWBL_CAN_INT_DATAOVER) {
                can_bus_sta = AW_CAN_BUS_STATUS_DATAOVER;
            } else if (int_status & AWBL_CAN_INT_WAKE_UP) {
                can_bus_sta = AW_CAN_BUS_STATUS_WAKE;
            }
        }

        /** \brief successful reception interrupt */
        if (int_status & AWBL_CAN_INT_RX) {

            /** \brief read message from can control */
            if (__can_reg_msg_read(p_serv, (aw_can_msg_t*)&can_rxd_msg) ==
                    AW_CAN_ERR_NONE) {

                /** \brief get time stamp */
                if (p_serv->p_app_cb_funcs->timestamp_get_cb != NULL) {
                    can_rxd_msg.can_msg.timestamp =
                        p_serv->p_app_cb_funcs->timestamp_get_cb(
                                can_rxd_msg.can_msg.timestamp);
                }

                /*
                 * If there is an input hook routine to call, call it now.
                 * The hook routine will return AW_TRUE if it doesn't want us
                 * to process the character further.
                 */
                if ((NULL  == p_serv->p_app_cb_funcs->proto_rx_cb) ||
                    (! p_serv->p_app_cb_funcs->proto_rx_cb(
                            p_serv->p_info->chn,
                            (aw_can_msg_t*)&can_rxd_msg))) {
                    AWBL_CAN_CONTROLLER_LOCK(p_serv);
                    rcvbytes = aw_rngbuf_put(p_serv->p_info->p_rxd_buf_ctr,
                                            (char *)&can_rxd_msg,
                                             p_serv->p_info->sizeof_msg);
                    AWBL_CAN_CONTROLLER_UNLOCK(p_serv);

                    if (rcvbytes != p_serv->p_info->sizeof_msg) {
                        if (NULL != p_serv->p_app_cb_funcs->bus_err_sta_cb) {
                            p_serv->p_app_cb_funcs->bus_err_sta_cb(
                                    p_serv->p_info->chn,
                                    AW_CAN_BUS_ERR_RCVBUFF_FULL,
                                    can_bus_sta);
                        }
                    }
                    AW_SEMB_GIVE(p_serv->rcv_start_sem);
                }
            }
        }

        /** \brief ERROR Warn / Passive /Bus off interrupt*/
        if (int_status & AWBL_CAN_INT_STATUS) {

            /*
             * If there is an input hook routine to call, call it now.
             * The hook routine will return AW_TRUE if it doesn't want us
             * to process the character further.
             */
            if (int_status & AWBL_CAN_INT_DATAOVER) {
                can_bus_err |= AW_CAN_BUS_ERR_DATAOVER;
            }
            if ((NULL  == p_serv->p_app_cb_funcs->bus_err_sta_cb) ||
                (! p_serv->p_app_cb_funcs->bus_err_sta_cb(
                       p_serv->p_info->chn, can_bus_err, can_bus_sta))) {
                can_rxd_msg.can_msg.flags  = AW_CAN_MSG_FLAG_ERROR;
                can_rxd_msg.can_msg.length = 0x03;

                /** \brief transmitting or receiving error interrupt */
                if (int_status & AWBL_CAN_INT_ERROR) {
                    can_rxd_msg.msgbuff[0] = AW_CAN_BUS_STATUS_ERROR;
                    can_rxd_msg.msgbuff[1] = can_bus_err;
                    can_rxd_msg.msgbuff[2] = 0x00;

                    AWBL_CAN_CONTROLLER_LOCK(p_serv);
                    rcvbytes = aw_rngbuf_put(p_serv->p_info->p_rxd_buf_ctr,
                                            (char*)&can_rxd_msg,
                                             p_serv->p_info->sizeof_msg);
                    AWBL_CAN_CONTROLLER_UNLOCK(p_serv);

                    if (rcvbytes != p_serv->p_info->sizeof_msg) {
                        if (NULL != p_serv->p_app_cb_funcs->bus_err_sta_cb) {
                            p_serv->p_app_cb_funcs->bus_err_sta_cb(
                                    p_serv->p_info->chn,
                                    AW_CAN_BUS_ERR_RCVBUFF_FULL,
                                    can_bus_sta);
                        }
                    }
                }

                /** \brief data over interrupt */
                if (int_status & AWBL_CAN_INT_DATAOVER) {
                    can_rxd_msg.msgbuff[0] = AW_CAN_BUS_STATUS_DATAOVER;
                    can_rxd_msg.msgbuff[1] = 0x00;
                    can_rxd_msg.msgbuff[2] = 0x00;

                    AWBL_CAN_CONTROLLER_LOCK(p_serv);
                    rcvbytes = aw_rngbuf_put(p_serv->p_info->p_rxd_buf_ctr,
                                            (char*)&can_rxd_msg,
                                             p_serv->p_info->sizeof_msg);
                    AWBL_CAN_CONTROLLER_UNLOCK(p_serv);

                    if (rcvbytes != p_serv->p_info->sizeof_msg) {
                        if (NULL != p_serv->p_app_cb_funcs->bus_err_sta_cb) {
                            p_serv->p_app_cb_funcs->bus_err_sta_cb(
                                    p_serv->p_info->chn,
                                    AW_CAN_BUS_ERR_RCVBUFF_FULL,
                                    can_bus_sta);
                        }
                    }
                }
                if (int_status & (AWBL_CAN_INT_WARN         |
                                  AW_CAN_BUS_STATUS_PASSIVE |
                                  AW_CAN_BUS_STATUS_OFF     )) {
                    can_rxd_msg.msgbuff[0] = can_bus_sta;
                    if (int_status & AWBL_CAN_INT_BUS_OFF) {
                        p_serv->txd_busy = AW_FALSE;
                        if (p_serv->p_curr_trans != NULL) {
                            p_serv->p_curr_trans->status = -AW_ENETDOWN;

                            /** \brief notify the caller */
                            if (p_serv->p_curr_trans->pfunc_complete != NULL) {
                                p_serv->p_curr_trans->pfunc_complete(
                                       p_serv->p_curr_trans->p_arg);
                            }
                        }
                    }
                    __can_err_cnt_get (p_serv, &can_err_cnt);
                    can_rxd_msg.msgbuff[1] = can_err_cnt.rx_err_cnt;
                    can_rxd_msg.msgbuff[2] = can_err_cnt.tx_err_cnt;

                    AWBL_CAN_CONTROLLER_LOCK(p_serv);
                    rcvbytes = aw_rngbuf_put(p_serv->p_info->p_rxd_buf_ctr,
                                            (char*)&can_rxd_msg,
                                             p_serv->p_info->sizeof_msg);
                    AWBL_CAN_CONTROLLER_UNLOCK(p_serv);

                    if (rcvbytes != p_serv->p_info->sizeof_msg) {

                        if (NULL != p_serv->p_app_cb_funcs->bus_err_sta_cb) {
                            p_serv->p_app_cb_funcs->bus_err_sta_cb(
                                    p_serv->p_info->chn,
                                    AW_CAN_BUS_ERR_RCVBUFF_FULL,
                                    can_bus_sta);
                        }
                    }
                }
            }

            /** \brief wake up interrupt*/
            if (int_status & AWBL_CAN_INT_WAKE_UP) {
                if ((NULL  == p_serv->p_app_cb_funcs->dev_wakeup_cb) ||
                    (! p_serv->p_app_cb_funcs->dev_wakeup_cb(
                           p_serv->p_info->chn))) {
                    can_rxd_msg.can_msg.flags  = AW_CAN_MSG_FLAG_ERROR;
                    can_rxd_msg.can_msg.length = 0x03;
                    can_rxd_msg.msgbuff[0]     = AW_CAN_BUS_STATUS_WAKE;
                    can_rxd_msg.msgbuff[1]     = 0x00;
                    can_rxd_msg.msgbuff[2]     = 0x00;

                    AWBL_CAN_CONTROLLER_LOCK(p_serv);
                    rcvbytes = aw_rngbuf_put(p_serv->p_info->p_rxd_buf_ctr,
                                            (char*)&can_rxd_msg,
                                             p_serv->p_info->sizeof_msg);
                    AWBL_CAN_CONTROLLER_UNLOCK(p_serv);

                    if (rcvbytes != p_serv->p_info->sizeof_msg) {

                        if (NULL != p_serv->p_app_cb_funcs->bus_err_sta_cb) {
                            p_serv->p_app_cb_funcs->bus_err_sta_cb(
                                    p_serv->p_info->chn,
                                    AW_CAN_BUS_ERR_RCVBUFF_FULL,
                                    can_bus_sta);
                        }
                    }
                }
            }
        }
    }
}
/* end of file */

