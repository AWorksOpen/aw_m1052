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
 * \brief CanFestival CAN��������
 *
 * \internal
 * \par modification history:
 * - 1.00 18-10-08  anu, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_can.h"
#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_delay.h"
#include "aw_spinlock.h"
#include "aw_mem.h"
#include "aw_canfestival_cfg.h"

#define __COP_RECV_TASK_STACK_SIZE     2048

#define __COP_CAN_RECV_MSG_TIMEOUT     10    /**< \brief CAN���Ľ��ճ�ʱʱ�� */
#define __COP_CAN_SEND_MSG_TIMEOUT     300   /**< \brief CAN���ķ��ͳ�ʱʱ�� */

#define __COP_MSG(x) AW_INFOF(("CAN : ")); AW_INFOF(x)

/** \brief canopen ʵ�� */
typedef struct __cop_inst {
    aw_bool_t     used; /**< \brief ��ʵ���Ƿ��Ѿ���ʹ�� */
    int           chn;  /**< \brief ��ʵ����Ӧ��CANͨ�� */
    
    /** \brief Э��ջ�������� */
    AW_TASK_DECL (recv_task, __COP_RECV_TASK_STACK_SIZE); 
    CO_Data      *p_data;                     /**< \brief Э��ջ���� */

    volatile aw_can_bus_status_t status;      /**< \brief CAN����״̬ */
} __cop_inst_t;

/******************************************************************************/
static uint32_t __can_timestamp_get_cb (uint32_t timestamp);
static aw_bool_t __can_err_sta_cb (
    int                 chn,
    aw_can_bus_err_t    err,
    aw_can_bus_status_t status);
static __cop_inst_t *__cop_inst_find_by_chn (int chn);

static aw_can_app_callbacks_t __g_can_app_callback_fun = {
    __can_timestamp_get_cb,
    NULL,
    NULL,
    __can_err_sta_cb,
    NULL
};

static aw_canfestival_brt_list_t *__gp_cop_brt_list = NULL; /* �������б� */
static __cop_inst_t              *__gp_cop_inst = NULL;     /* ʵ����� */
static uint32_t                   __g_cop_inst_cnt = 0;     /* ʵ������ */

/**
 * \brief ʱ�����ȡ�Ļص�����
 *
 * \param[in] timestamp ��ǰ���ĵ�ʱ���
 *
 * \return �û��޸ĺ��ʱ���
 */
static uint32_t __can_timestamp_get_cb (uint32_t timestamp)
{
    return timestamp;
}

/**
 * \brief ����״̬�Ļص�����
 *
 * \param[in] chn    ͨ����
 * \param[in] err    ���ߵĴ�������
 * \param[in] status ���ߵ�״̬
 *
 * \retval AW_TRUE  ��������Ϣ������rngbuf
 * \retval AW_FALSE ������Ϣ������rngbuf
 */
static aw_bool_t __can_err_sta_cb (
    int                 chn,
    aw_can_bus_err_t    err,
    aw_can_bus_status_t status)
{
    __cop_inst_t     *p_cop_inst;

    /* ���ҵ����õ�ʵ�� */
    p_cop_inst = __cop_inst_find_by_chn(chn);

    if (p_cop_inst != NULL) {
        p_cop_inst->status = status;
    }
    /* ��������ֵ����������Ϣ�Ƿ����rngbuf,AW_TRUE:������,AW_FALSE:���� */
    return AW_TRUE;
}

/**
 * \brief ��ȡ�����ʲ���
 *
 * \param[in] chn ͨ����
 * \param[in] val ������ֵ
 *
 * \return p_brt �����ʲ���
 */
static aw_can_baud_param_t *__cop_can_brt_get (int chn, int val)
{
    int i;

    aw_can_baud_param_t       *p_brt = NULL;
    aw_canfestival_brt_list_t *p_map = NULL;

    for (i = 0; i < __g_cop_inst_cnt; i++) {
        if (__gp_cop_brt_list[i].chn == chn) {
            p_map = &__gp_cop_brt_list[i];
            break;
        }
    }

    if (p_map == NULL) {
        goto exit;
    }

    for (i = 0; i < AW_CANFESTIVAL_BRT_LIST_CNT; i++) {
        if (p_map->brt[i].val == val) {
            p_brt = &p_map->brt[i].param;
            break;
        }
    }

exit:
    return p_brt;
}

/**
 * \brief ͨ��ͨ���Ų���ʵ��
 *
 * \param[in] chn ͨ����
 *
 * \return CANfestival ���
 */
static __cop_inst_t *__cop_inst_find_by_chn (int chn)
{
    int                i;
    __cop_inst_t      *p_inst = NULL;
    aw_spinlock_isr_t  lock = AW_SPINLOCK_ISR_UNLOCK;

    aw_spinlock_isr_take(&lock);
    for (i = 0; i < __g_cop_inst_cnt; i++) {
        if (__gp_cop_inst[i].chn == chn) {
            p_inst = &__gp_cop_inst[i];
            break;
        }
    }
    aw_spinlock_isr_give(&lock);

    return p_inst;
}

/**
 * \brief CAN��ʼ��
 *
 * \param[in] chn ͨ����
 * \param[in] brt ������
 *
 * \return AW_CAN_ERR_NONE������������������ο��������
 */
static aw_can_err_t __cop_can_startup (int chn, int brt)
{
    aw_can_err_t ret;

    aw_can_baud_param_t *p_brt = __cop_can_brt_get(chn, brt);

    if (p_brt == NULL) {
        __COP_MSG(("no baud param\n"));
        ret = -AW_CAN_ERR_INVALID_PARAMETER;
        goto exit;
    }

    /** \brief ��ʼ��CAN */
    ret = aw_can_init(chn,
                      AW_CAN_WORK_MODE_NORMAL,
                      p_brt,
                      &__g_can_app_callback_fun);

    __COP_MSG(("controller init...\n"));

    if (ret != AW_CAN_ERR_NONE) {
        goto exit;
    }

    /** \brief ����CAN */
    ret = aw_can_start(chn);
    __COP_MSG(("controller start!\n"));

exit:
    return ret;
}

#ifndef NOT_USE_DYNAMIC_LOADING

/*UnLoads the dll*/
UNS8 UnLoadCanDriver (LIB_HANDLE handle)
{
    return 0;
}

/*Loads the dll and get funcs ptr*/
LIB_HANDLE LoadCanDriver (char* driver_name)
{
    return NULL;
}

#endif

UNS8 canSend (CAN_PORT p_port, Message *p_message)
{
    int               chn;
    uint32_t          send_done_cnt;
    aw_can_std_msg_t  msg;
    aw_can_err_t      ret = 0;
    __cop_inst_t     *p_cop_inst = NULL;

    EnterMutex();
    p_cop_inst = p_port;
    if (p_cop_inst == NULL || p_message == NULL || !p_cop_inst->used ) {
        ret = -1;
    }
    chn = p_cop_inst->chn;
    LeaveMutex();

    if (ret < 0) {
        goto exit;
    }

    msg.can_msg.id = p_message->cob_id.w;
    msg.can_msg.length = p_message->len;
    msg.can_msg.flags = p_message->rtr ? AW_CAN_MSG_FLAG_REMOTE : 0;
    memcpy(msg.msgbuff, p_message->data, sizeof(msg.msgbuff));

    ret = aw_can_std_msgs_send(
        chn, &msg, 1, &send_done_cnt, __COP_CAN_SEND_MSG_TIMEOUT);

    if (ret == AW_CAN_ERR_NONE && send_done_cnt == 1) {
        return 0;
    }

exit:
    return -1;
}

static void __cop_can_recv (void *p_arg)
{
    int                  chn;
    uint32_t             recv_done_cnt;
    Message              message;
    aw_can_err_t         ret;
    aw_can_std_msg_t     msg;
    aw_can_bus_status_t  bus_status;
    aw_spinlock_isr_t    lock = AW_SPINLOCK_ISR_UNLOCK;
    __cop_inst_t        *p_cop_inst = p_arg;

    if (p_cop_inst == NULL) {
        AW_FOREVER {
            aw_mdelay(1000);
        }
    }

    chn = p_cop_inst->chn;

    AW_FOREVER {

        ret = aw_can_std_msgs_rcv(
            chn, &msg, 1, &recv_done_cnt, __COP_CAN_RECV_MSG_TIMEOUT);

        EnterMutex();
        if (p_cop_inst->used  || p_cop_inst->p_data != NULL) {

            if ((ret == AW_CAN_ERR_NONE) && (recv_done_cnt > 0)) {
                message.cob_id.w = msg.can_msg.id;
                message.len = msg.can_msg.length;
                message.rtr =
                    (msg.can_msg.flags & AW_CAN_MSG_FLAG_REMOTE) ? 1 : 0;

                memcpy(message.data, msg.msgbuff, sizeof(message.data));
                canDispatch(p_cop_inst->p_data, &message);
            }
        }

        aw_spinlock_isr_take(&lock);
        bus_status = p_cop_inst->status;
        p_cop_inst->status = AW_CAN_BUS_STATUS_OK;
        aw_spinlock_isr_give(&lock);

        if (bus_status == AW_CAN_BUS_STATUS_OFF) {

            __COP_MSG(("busoff!\n"));

            /* ֹͣCAN */
            aw_can_stop(chn);
            __COP_MSG(("controller stop!\n"));

            /* ����CAN */
            aw_can_start(chn);
            __COP_MSG(("controller start!\n"));
        }

        LeaveMutex();
    }
}

CAN_PORT canOpen (s_BOARD *p_board, CO_Data *p_data)
{
    aw_can_err_t  ret;
    int           chn, brt;
    CAN_PORT      p_fd = NULL;
    __cop_inst_t *p_cop_inst = NULL;

    if (p_board == NULL || p_data == NULL) {
        return NULL;
    }

    chn = atoi(p_board->busname);
    brt = atoi(p_board->baudrate);

    /* ͨ��ͨ���Ų���ʵ�� */
    p_cop_inst = __cop_inst_find_by_chn(chn);

    if (p_cop_inst == NULL) {
        return NULL;
    }

    EnterMutex();

    if (p_cop_inst->used ) {
        goto exit;
    }

    ret = __cop_can_startup(chn, brt);

    if (ret != AW_CAN_ERR_NONE) {
        goto exit;
    }

    p_cop_inst->used = AW_TRUE;
    p_cop_inst->p_data = p_data;
    p_data->canHandle = p_cop_inst;
    p_fd = p_cop_inst;
    AW_TASK_RESUME(p_cop_inst->recv_task);
    aw_can_start(chn);

exit:
    LeaveMutex();

    return p_fd;
}

int canClose (CO_Data *p_data)
{
    int           ret = 0;
    __cop_inst_t *p_cop_inst = NULL;

    EnterMutex();

    if (p_data == NULL) {
        ret = -1;
        goto exit;
    }

    if (p_data->canHandle == NULL) {
        ret = -1;
        goto exit;
    }

    aw_can_stop(p_cop_inst->chn);
    p_cop_inst = p_data->canHandle;
    p_cop_inst->used = AW_FALSE;
    p_cop_inst->p_data = NULL;
    p_cop_inst->status = 0;
    p_data->canHandle = NULL;
    AW_TASK_SUSPEND(p_cop_inst->recv_task);

exit:
    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival ��ʼ��
 *
 * \param[in] inst_cnt       ʵ��Canfestival�ĸ���
 * \param[in] recv_task_prio ������������ȼ�
 * \param[in] p_hwtimer      ��Ҫ�ṩһ����ʱ�����
 * \param[in] p_chn_brt_list ʵ���õ���CAN busͨ����Ӧ�Ĳ����ʱ�
 *
 * \return AW_OK�����ɹ�����������ԭ��ο�������
 */
aw_err_t aw_canfestival_init (
    int                              inst_cnt,
    int                              recv_task_prio,
    const aw_hwtimer_handle_t        p_hwtimer,
    const aw_canfestival_brt_list_t *p_chn_brt_list)
{
    int i, inst_size, brp_list_size;

    __cop_inst_t              *p_cop_inst = NULL;
    aw_canfestival_brt_list_t *p_brt_list = NULL;

    if (p_hwtimer == NULL || p_chn_brt_list == NULL) {
        return -AW_EINVAL;
    }

    inst_size = inst_cnt * sizeof(__cop_inst_t);
    brp_list_size = inst_cnt * sizeof(aw_canfestival_brt_list_t);
    p_cop_inst = aw_mem_align(inst_size + brp_list_size, sizeof(uint32_t));
    p_brt_list = (aw_canfestival_brt_list_t *)(p_cop_inst + inst_cnt);

    if (p_cop_inst == NULL || p_brt_list == NULL) {
        return -AW_ENOMEM;
    }

    /* ��ȫ�����ݽ��г�ʼ��  */
    memset(p_cop_inst, 0x00, inst_size + brp_list_size);
    memcpy(p_brt_list, p_chn_brt_list, brp_list_size);

    for (i = 0; i < inst_cnt; i++) {

        p_cop_inst[i].chn = p_brt_list[i].chn;

        AW_TASK_INIT(p_cop_inst[i].recv_task,
                     "cop_recv",
                     recv_task_prio,
                     __COP_RECV_TASK_STACK_SIZE,
                     __cop_can_recv,
                     &p_cop_inst[i]);

        AW_TASK_STARTUP(p_cop_inst[i].recv_task);

        AW_TASK_SUSPEND(p_cop_inst[i].recv_task);
    }

    aw_canfestival_timer_register(p_hwtimer);

    __gp_cop_inst = p_cop_inst;
    __gp_cop_brt_list = p_brt_list;
    __g_cop_inst_cnt = inst_cnt;

    return AW_OK;
}

/* end of file */
