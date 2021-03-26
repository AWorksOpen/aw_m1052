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
 * \brief CanFestival CAN驱动适配
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

#define __COP_CAN_RECV_MSG_TIMEOUT     10    /**< \brief CAN报文接收超时时间 */
#define __COP_CAN_SEND_MSG_TIMEOUT     300   /**< \brief CAN报文发送超时时间 */

#define __COP_MSG(x) AW_INFOF(("CAN : ")); AW_INFOF(x)

/** \brief canopen 实例 */
typedef struct __cop_inst {
    aw_bool_t     used; /**< \brief 该实例是否已经被使用 */
    int           chn;  /**< \brief 该实例对应的CAN通道 */
    
    /** \brief 协议栈接收任务 */
    AW_TASK_DECL (recv_task, __COP_RECV_TASK_STACK_SIZE); 
    CO_Data      *p_data;                     /**< \brief 协议栈数据 */

    volatile aw_can_bus_status_t status;      /**< \brief CAN总线状态 */
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

static aw_canfestival_brt_list_t *__gp_cop_brt_list = NULL; /* 波特率列表 */
static __cop_inst_t              *__gp_cop_inst = NULL;     /* 实例句柄 */
static uint32_t                   __g_cop_inst_cnt = 0;     /* 实例个数 */

/**
 * \brief 时间戳获取的回调函数
 *
 * \param[in] timestamp 当前报文的时间戳
 *
 * \return 用户修改后的时间戳
 */
static uint32_t __can_timestamp_get_cb (uint32_t timestamp)
{
    return timestamp;
}

/**
 * \brief 错误状态的回调函数
 *
 * \param[in] chn    通道号
 * \param[in] err    总线的错误类型
 * \param[in] status 总线的状态
 *
 * \retval AW_TRUE  不保存消息到接收rngbuf
 * \retval AW_FALSE 保存消息到接收rngbuf
 */
static aw_bool_t __can_err_sta_cb (
    int                 chn,
    aw_can_bus_err_t    err,
    aw_can_bus_status_t status)
{
    __cop_inst_t     *p_cop_inst;

    /* 查找到可用的实例 */
    p_cop_inst = __cop_inst_find_by_chn(chn);

    if (p_cop_inst != NULL) {
        p_cop_inst->status = status;
    }
    /* 函数返回值决定错误消息是否存入rngbuf,AW_TRUE:不保存,AW_FALSE:保存 */
    return AW_TRUE;
}

/**
 * \brief 获取波特率参数
 *
 * \param[in] chn 通道号
 * \param[in] val 波特率值
 *
 * \return p_brt 波特率参数
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
 * \brief 通过通道号查找实例
 *
 * \param[in] chn 通道号
 *
 * \return CANfestival 句柄
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
 * \brief CAN初始化
 *
 * \param[in] chn 通道号
 * \param[in] brt 波特率
 *
 * \return AW_CAN_ERR_NONE正常启动其他错误请参考错误代码
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

    /** \brief 初始化CAN */
    ret = aw_can_init(chn,
                      AW_CAN_WORK_MODE_NORMAL,
                      p_brt,
                      &__g_can_app_callback_fun);

    __COP_MSG(("controller init...\n"));

    if (ret != AW_CAN_ERR_NONE) {
        goto exit;
    }

    /** \brief 启动CAN */
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

            /* 停止CAN */
            aw_can_stop(chn);
            __COP_MSG(("controller stop!\n"));

            /* 启动CAN */
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

    /* 通过通道号查找实例 */
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
 * \brief CANfestival 初始化
 *
 * \param[in] inst_cnt       实例Canfestival的个数
 * \param[in] recv_task_prio 接收任务的优先级
 * \param[in] p_hwtimer      需要提供一个定时器句柄
 * \param[in] p_chn_brt_list 实例用到的CAN bus通道对应的波特率表
 *
 * \return AW_OK操作成功，其他错误原因参考错误码
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

    /* 对全局数据进行初始化  */
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
