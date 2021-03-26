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
#include "aw_rngbuf.h"
#include "aw_canfestival_cfg.h"

#define __COP_RECV_TASK_STACK_SIZE     2048

#define __COP_CAN_RECV_MSG_TIMEOUT     10    /**< \brief CAN报文接收超时时间 */
#define __COP_CAN_SEND_MSG_TIMEOUT     300   /**< \brief CAN报文发送超时时间 */

#define __COP_SDO_ID_INVALID           0X80000000L /**< \brief 无效的SDO ID */
#define __COP_PDO_ID_INVALID           0X80000000L /**< \brief 无效的PDO ID */

#define __COP_NODE_ID_MIN              0X01 /**< \brief 最小的节点ID号 */
#define __COP_NODE_ID_MAX              0X7F /**< \brief 最小的节点ID号 */

#define __COP_MSG(x) AW_INFOF(("CAN : ")); AW_INFOF(x)

/** \brief canopen 消息计数信号量 */
typedef struct __cop_msg_semc {
    AW_SEMC_DECL(semc);
} __cop_msg_semc_t;

/** \brief canopen 节点信息 */
typedef struct __cop_node_info {
    uint8_t  node_id;
    uint8_t  check_mode;
    uint16_t check_period;
    uint16_t retry_factor;
} __cop_node_info_t;

/** \brief canopen 实例 */
typedef struct __cop_inst {
    aw_bool_t     used; /**< \brief 该实例是否已经被使用 */
    int           chn;  /**< \brief 该实例对应的CAN通道 */
    
    /** \brief 协议栈接收任务 */
    AW_TASK_DECL(recv_task, __COP_RECV_TASK_STACK_SIZE);  
    
    CO_Data      *p_data;                       /**< \brief 协议栈数据 */

    aw_rngbuf_t       p_rpdo_rngbufs;           /**< \brief RPDO对应的rngbuf */
    __cop_msg_semc_t *p_rpdo_rnbufs_msg_semc;   /**< \brief rngbuf消息计数 */
    uint8_t          *p_rpdo_rngbufs_mem;       /**< \brief rngbuf对应的内存 */

    /** \brief RPDO更新回调函数 */
    void (*p_rpdo_update_callback_fun) (void     *p_arg,
                                        uint16_t  pdo_id,
                                        uint8_t  *p_data,
                                        uint8_t   len);   
    void *p_rpdo_update_callback_arg;            /**< \brief RPDO更新回调参数 */

    __cop_node_info_t *p_node_info;              /**< \brief 节点信息 */

    volatile aw_can_bus_status_t status;         /**< \brief CAN总线状态 */
} __cop_inst_t;

/******************************************************************************/
static uint32_t __can_timestamp_get_cb (uint32_t timestamp);
static aw_bool_t __can_err_sta_cb (
    int                 chn,
    aw_can_bus_err_t    err,
    aw_can_bus_status_t status);
static __cop_inst_t *__cop_inst_find_by_chn (int chn);
static void __cop_recv_callback (__cop_inst_t *p_cop_inst, Message *p_msg);

static aw_can_app_callbacks_t __g_can_app_callback_fun = {
    __can_timestamp_get_cb,
    NULL,
    NULL,
    __can_err_sta_cb,
    NULL
};

static aw_canfestival_brt_list_t  *__gp_cop_brt_list = NULL;  /* 波特率列表 */
static __cop_inst_t               *__gp_cop_inst = NULL;      /* 实例句柄 */
static CO_Data                   **__gp_cop_inst_data = NULL; /* 实例数据 */
static uint32_t                    __g_cop_inst_cnt = 0;      /* 实例个数 */
static uint32_t                    __g_cop_inst_init_cnt = 0; /* 实例初始化数 */

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

    msg.can_msg.id = p_message->cob_id;
    msg.can_msg.length = p_message->len;
    msg.can_msg.flags = p_message->rtr ? AW_CAN_MSG_FLAG_REMOTE : 0;
    memcpy(msg.msgbuff, p_message->data, sizeof(msg.msgbuff));

    ret = aw_can_std_msgs_send(
        chn, &msg, 1, &send_done_cnt, __COP_CAN_SEND_MSG_TIMEOUT);

    if (ret == AW_CAN_ERR_NONE && send_done_cnt == 1) {
        return 0;
    }

exit:
    return (UNS8)(-1);
}

/**
 * CAN change baudrate routine
 * @param port CAN port
 * @param baud baudrate
 * @return success or error
 */
UNS8 canChangeBaudRate (CAN_PORT p_port, char *p_baud)
{
    int  chn, brt;

    aw_can_err_t         ret;
    __cop_inst_t        *p_cop_inst = NULL;
    aw_can_baud_param_t *p_brt = NULL;

    if (p_port == NULL || p_baud == NULL) {
        return 1; /* NOT OK */
    }

    brt = atoi(p_baud);
    p_cop_inst = p_port;

    LeaveMutex();
    chn = p_cop_inst->chn;
    p_brt = __cop_can_brt_get(chn, brt);
    ret = aw_can_baudrate_set(chn, p_brt);
    EnterMutex();

    if (ret == AW_CAN_BUS_ERR_NONE) {
        return 0; /* OK */
    }

    return 1; /* NOT OK */
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
                message.cob_id = msg.can_msg.id;
                message.len = msg.can_msg.length;
                message.rtr =
                    (msg.can_msg.flags & AW_CAN_MSG_FLAG_REMOTE) ? 1 : 0;

                memcpy(message.data, msg.msgbuff, sizeof(message.data));
                canDispatch(p_cop_inst->p_data, &message);
                __cop_recv_callback(p_cop_inst, &message);
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
    p_cop_inst->status = 0;
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
 * \brief CANfestival 实例的句柄获取
 *
 * \param[in] inst_num 实例号
 *
 * \return 对应实例的句柄
 */
void *aw_canfestival_inst_get (int inst_num)
{
    void *p_inst = NULL;

    if (inst_num >= __g_cop_inst_cnt) {
        goto exit;
    }

    p_inst = &__gp_cop_inst[inst_num];

exit:
    return p_inst;
}

/**
 * \brief CANfestival 实例的数据获取
 *
 * \param[in] inst_num 实例号
 *
 * \return 对应实例的数据
 */
void *aw_canfestival_inst_data_get (int inst_num)
{
    void *p_inst_data = NULL;

    if (inst_num >= __g_cop_inst_cnt) {
        goto exit;
    }

    p_inst_data = __gp_cop_inst_data[inst_num];

exit:
    return p_inst_data;
}

/**
 * \brief RPDO更新回调函数注册
 *
 * \param[in] inst_num  实例号
 * \param[in] p_msg     要发送的消息
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_msg_send (int inst_num, aw_can_std_msg_t *p_msg)
{
    int               chn;
    uint32_t          send_done_cnt;
    aw_can_err_t      ret = AW_CAN_ERR_NONE;
    __cop_inst_t     *p_cop_inst = aw_canfestival_inst_get(inst_num);

    EnterMutex();
    if (p_cop_inst == NULL || p_msg == NULL || !p_cop_inst->used ) {
        ret = -AW_CAN_ERR_INVALID_PARAMETER;
    }
    chn = p_cop_inst->chn;
    LeaveMutex();

    if (ret != AW_CAN_ERR_NONE) {
        goto exit;
    }

    ret = aw_can_std_msgs_send(
        chn, p_msg, 1, &send_done_cnt, __COP_CAN_SEND_MSG_TIMEOUT);

exit:
    return ret;
}

/**
 * \brief RPDO更新的回调函数
 *
 * \param[in] p_cop_inst    实例
 * \param[in] p_index_table  索引表
 * \param[in] sub_index      子索引
 *
 * \return 无
 */
static void __cop_rpdo_update_callback (
    __cop_inst_t *p_cop_inst, Message *p_msg)
{
    if (p_cop_inst->p_rpdo_update_callback_fun != NULL) {
        p_cop_inst->p_rpdo_update_callback_fun(
            p_cop_inst->p_rpdo_update_callback_arg,
            p_msg->cob_id,
            p_msg->data,
            p_msg->len);
    }
}

/**
 * \brief RPDO更新回调函数注册
 *
 * \param[in] inst_num       实例号
 * \param[in] p_callback_fun 回调函数
 * \param[in] p_arg          用户参数
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_rpdo_update_callback_register (
    int    inst_num,
    void (*p_callback_fun) (
        void *p_arg, uint16_t pdo_id, uint8_t *p_data, uint8_t len),
    void  *p_arg)
{
    __cop_inst_t *p_cop_inst = NULL;

    p_cop_inst = aw_canfestival_inst_get(inst_num);
    if (p_cop_inst == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    p_cop_inst->p_rpdo_update_callback_fun = p_callback_fun;
    p_cop_inst->p_rpdo_update_callback_arg = p_arg;

    return AW_CAN_ERR_NONE;
}

/**
 * \brief 设置RPDO更新时的回调函数
 *
 * \param[in] p_cop_inst    实例
 * \param[in] p_msg         消息
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
static void __cop_recv_callback (__cop_inst_t *p_cop_inst, Message *p_msg)
{
    uint16_t  cob_id = p_msg->cob_id;
    CO_Data  *p_cop_data = p_cop_inst->p_data;
    uint16_t  offset_index, last_index;
    int       rpdo_cnt, i;

    switch(cob_id >> 7) {
    case PDO1tx:
    case PDO1rx:
    case PDO2tx:
    case PDO2rx:
    case PDO3tx:
    case PDO3rx:
    case PDO4tx:
    case PDO4rx:
        if (!p_cop_inst->p_data->CurrentCommunicationState.csPDO) {
            break;
        }

        if (p_msg->rtr == REQUEST) {
            break;
        }

        offset_index = p_cop_data->firstIndex->PDO_RCV;
        last_index = p_cop_data->lastIndex->PDO_RCV;
        rpdo_cnt = last_index - offset_index;

        if (offset_index == 0 || rpdo_cnt < 0) {
            break;
        }

        for (i = 0; i <= rpdo_cnt; i++) {

            /* subindex = 1 存放的是COB ID */
            if ((*(uint32_t *)
                p_cop_data->objdict[offset_index + i].pSubindex[1].pObject) ==
                cob_id) {
                aw_rngbuf_put(&(p_cop_inst->p_rpdo_rngbufs[i]),
                              (const char *)p_msg,
                              sizeof(*p_msg));

                AW_SEMC_GIVE(
                    p_cop_inst->p_rpdo_rnbufs_msg_semc[i].semc);

                __cop_rpdo_update_callback(p_cop_inst, p_msg);
                break;
            }
        }
        break;
    default:
        break;
    }
}

/**
 * \brief CANfestival 获取实例对应的节点的个数
 *
 * \param[in] p_cop_data 实例数据
 *
 * \return 个数，小于等于0则为无效
 */
static int8_t __inst_node_cnt (CO_Data *p_cop_data)
{
    int16_t sdo_cnt;

    sdo_cnt =
        p_cop_data->lastIndex->SDO_CLT - p_cop_data->firstIndex->SDO_CLT + 1;
    if (p_cop_data->firstIndex->SDO_CLT > 0 && sdo_cnt > 0) {
        return sdo_cnt;
    }

    return -1;
}

/**
 * \brief CANfestival 获取实例对应的节点的个数
 *
 * \param[in] inst_num 实例号
 *
 * \return 个数，小于等于0则为无效
 */
int8_t aw_canfestival_inst_node_cnt (int inst_num)
{
    CO_Data *p_cop_data = NULL;

    p_cop_data = aw_canfestival_inst_data_get(inst_num);
    if (p_cop_data == NULL) {
        return -1;
    }

    return __inst_node_cnt(p_cop_data);
}

/**
 * \brief CANfestival 获取PDO ID对应的PDO NUM
 *
 * \param[in]  inst_num   实例号
 * \param[in]  node_id    NODE ID 号
 * \param[out] p_node_num NODE NUM 号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_node_num_get (
    int inst_num, int node_id, int *p_node_num)
{
    int           i;
    aw_can_err_t  ret = -AW_CAN_ERR_NO_OP;
    CO_Data      *p_cop_data = NULL;
    indextable   *p_index_table = NULL;
    uint32_t      error_code;
    int8_t        node_cnt;
    uint16_t      sdo_index = 0x1280;
    uint8_t       node_id_subindex = 3;

    if (p_node_num == NULL) {
        return -AW_CAN_ERR_INVALID_PARAMETER;
    }

    p_cop_data = aw_canfestival_inst_data_get(inst_num);
    if (p_cop_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    EnterMutex();
    node_cnt = __inst_node_cnt(p_cop_data);
    for (i = 0; i < node_cnt; i++) {

        p_index_table = (indextable *)(*p_cop_data->scanIndexOD)(
                    p_cop_data, sdo_index, &error_code);

        if (p_index_table != NULL && (*(uint8_t *)(
            p_index_table->pSubindex[node_id_subindex].pObject)) == node_id) {
            *p_node_num = i;
            ret = AW_CAN_ERR_NONE;
            break;
        }

        sdo_index++;
    }
    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival 设置实例的某个节点的信息
 *
 * \param[in] inst_num     实例号
 * \param[in] node_num     节点号
 * \param[in] node_id      节点ID
 * \param[in] check_mode   检查模式
 * \param[in] check_period 检查周期
 * \param[in] retry_factor 检查因子
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_node_info_set (int      inst_num,
                                                uint8_t  node_num,
                                                uint8_t  node_id,
                                                uint8_t  check_mode,
                                                uint16_t check_period,
                                                uint16_t retry_factor)
{
    __cop_inst_t       *p_cop_inst  = NULL;
     CO_Data           *p_cop_data  = NULL;
     __cop_node_info_t *p_node_info = NULL;
     aw_can_err_t       ret         = AW_CAN_ERR_NONE;

    uint32_t cob_id_client_to_server_tsdo;
    uint32_t cob_id_server_to_client_rsdo;
    uint8_t  node_id_of_the_sdo_server;
    uint16_t sdo_index;
    uint32_t exp_size;
    int8_t   node_cnt;

    p_cop_inst = aw_canfestival_inst_get(inst_num);
    if (p_cop_inst == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    p_cop_data = aw_canfestival_inst_data_get(inst_num);
    if (p_cop_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    p_node_info = p_cop_inst->p_node_info;
    if (p_node_info == NULL) {
        return -AW_CAN_ERR_NO_OP;
    }

    EnterMutex();

    node_cnt = __inst_node_cnt(p_cop_data);
    if (node_cnt <= 0 || node_num >= node_cnt) {
        ret = -AW_CAN_ERR_INVALID_PARAMETER;
        goto exit;
    }

    sdo_index = 0x1280 + node_num;

    cob_id_client_to_server_tsdo = 0x600 + node_id;
    cob_id_server_to_client_rsdo = 0x580 + node_id;
    node_id_of_the_sdo_server = node_id;

    if (node_id == 0) {
        cob_id_client_to_server_tsdo |= __COP_SDO_ID_INVALID;
        cob_id_server_to_client_rsdo |= __COP_SDO_ID_INVALID;
    }

    exp_size = sizeof(cob_id_client_to_server_tsdo);
    if (writeLocalDict(p_cop_data,
                       sdo_index,
                       1,
                       &cob_id_client_to_server_tsdo,
                       &exp_size,
                       RW) != OD_SUCCESSFUL) {
        ret = -AW_CAN_ERR_NOT_INITIALIZED;
        goto exit;
    }

    exp_size = sizeof(cob_id_server_to_client_rsdo);
    if (writeLocalDict(p_cop_data,
                       sdo_index,
                       2,
                       &cob_id_server_to_client_rsdo,
                       &exp_size,
                       RW) != OD_SUCCESSFUL) {
        ret = -AW_CAN_ERR_NOT_INITIALIZED;
        goto exit;
    }

    exp_size = sizeof(node_id_of_the_sdo_server);
    if (writeLocalDict(p_cop_data,
                       sdo_index,
                       3,
                       &node_id_of_the_sdo_server,
                       &exp_size,
                       RW) != OD_SUCCESSFUL) {
        ret = -AW_CAN_ERR_NOT_INITIALIZED;
        goto exit;
    }

    p_node_info[node_num].node_id = node_id;
    p_node_info[node_num].check_mode = check_mode;
    p_node_info[node_num].check_period = check_period;
    p_node_info[node_num].retry_factor = retry_factor;

exit:
    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival 获取实例的某个节点的信息
 *
 * \param[in]  inst_num       实例号
 * \param[in]  node_num       节点号
 * \param[out] p_node_id      节点ID
 * \param[out] p_check_mode   检查模式
 * \param[out] p_check_period 检查周期
 * \param[out] p_retry_factor 检查因子
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_node_info_get (int       inst_num,
                                                uint8_t   node_num,
                                                uint8_t  *p_node_id,
                                                uint8_t  *p_check_mode,
                                                uint16_t *p_check_period,
                                                uint16_t *p_retry_factor)
{
    __cop_inst_t      *p_cop_inst  = NULL;
    CO_Data           *p_cop_data  = NULL;
    __cop_node_info_t *p_node_info = NULL;
    aw_can_err_t       ret = AW_CAN_ERR_NONE;
    int8_t             node_cnt;

    if (p_node_id      == NULL ||
        p_check_mode   == NULL ||
        p_check_period == NULL ||
        p_retry_factor == NULL) {
        return -AW_CAN_ERR_INVALID_PARAMETER;
    }

    p_cop_inst = aw_canfestival_inst_get(inst_num);
    if (p_cop_inst == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    p_cop_data = aw_canfestival_inst_data_get(inst_num);
    if (p_cop_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    p_node_info = p_cop_inst->p_node_info;
    if (p_node_info == NULL) {
        return -AW_CAN_ERR_NO_OP;
    }

    EnterMutex();

    node_cnt = __inst_node_cnt(p_cop_data);
    if (node_cnt <= 0 || node_num >= node_cnt) {
        ret = -AW_CAN_ERR_INVALID_PARAMETER;
        goto exit;
    }

    *p_node_id = p_node_info[node_num].node_id;
    *p_check_mode = p_node_info[node_num].check_mode;
    *p_check_period = p_node_info[node_num].check_period;
    *p_retry_factor = p_node_info[node_num].retry_factor;

exit:
    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival 实例添加节点信息
 *
 * \param[in] inst_num 实例号
 * \param[in] node_id      节点ID
 * \param[in] check_mode   检查模式
 * \param[in] check_period 检查周期
 * \param[in] retry_factor 检查因子
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_node_add (int      inst_num,
                                           uint8_t  node_id,
                                           uint8_t  check_mode,
                                           uint16_t check_period,
                                           uint16_t retry_factor)
{
    __cop_inst_t *p_cop_inst = NULL;
    CO_Data      *p_cop_data = NULL;
    int           node_num;
    aw_can_err_t  ret;

    p_cop_inst = aw_canfestival_inst_get(inst_num);
    if (p_cop_inst == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    p_cop_data = aw_canfestival_inst_data_get(inst_num);
    if (p_cop_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    EnterMutex();
    ret = aw_canfestival_node_num_get(inst_num, 0, &node_num);
    if (ret != AW_CAN_ERR_NONE) {
        goto exit;
    }

    ret = aw_canfestival_inst_node_info_set(inst_num,
                                            node_num,
                                            node_id,
                                            check_mode,
                                            check_period,
                                            retry_factor);

exit:
    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival 实例移除节点
 *
 * \param[in] inst_num 实例号
 * \param[in] node_id  节点ID
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_node_remove (int      inst_num,
                                              uint8_t  node_id)
{
    __cop_inst_t *p_cop_inst = NULL;
    CO_Data      *p_cop_data = NULL;
    int           node_num;
    aw_can_err_t  ret;

    p_cop_inst = aw_canfestival_inst_get(inst_num);
    if (p_cop_inst == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    p_cop_data = aw_canfestival_inst_data_get(inst_num);
    if (p_cop_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    EnterMutex();

    ret = aw_canfestival_node_num_get(inst_num, node_id, &node_num);
    if (ret != AW_CAN_ERR_NONE) {
        ret = -AW_CAN_ERR_NO_DEV;
        goto exit;
    }

    ret = aw_canfestival_inst_node_info_set(inst_num, node_num, 0, 0, 0, 0);

exit:
    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival 获取实例对应的PDO的个数
 *
 * \param[in] p_cop_data 实例数据
 * \param[in] is_rpdo    是否是RPDO
 *
 * \return 个数，小于等于0则为无效
 */
static int8_t __inst_pdo_cnt (CO_Data *p_cop_data, aw_bool_t is_rpdo)
{
    int16_t pdo_cnt;

    if (is_rpdo) {
        pdo_cnt = p_cop_data->lastIndex->PDO_RCV -
            p_cop_data->firstIndex->PDO_RCV + 1;
        if (p_cop_data->firstIndex->PDO_RCV > 0 && pdo_cnt > 0) {
            return pdo_cnt;
        }
    } else {
        pdo_cnt = p_cop_data->lastIndex->PDO_TRS -
            p_cop_data->firstIndex->PDO_TRS + 1;
         if (p_cop_data->firstIndex->PDO_TRS > 0 && pdo_cnt > 0) {
             return pdo_cnt;
         }
    }

    return -1;
}

/**
 * \brief CANfestival 获取实例对应的PDO的个数
 *
 * \param[in] inst_num 实例号
 * \param[in] is_rpdo  是否是RPDO
 *
 * \return 个数，小于等于0则为无效
 */
int8_t aw_canfestival_inst_pdo_cnt (int inst_num, aw_bool_t is_rpdo)
{
    CO_Data *p_cop_data = NULL;

    p_cop_data = aw_canfestival_inst_data_get(inst_num);
    if (p_cop_data == NULL) {
        return -1;
    }

    return __inst_pdo_cnt(p_cop_data, is_rpdo);
}

/**
 * \brief CANfestival 获取PDO ID对应的PDO NUM
 *
 * \param[in]  inst_num  实例号
 * \param[in]  pdo_id    PDO ID 号
 * \param[in]  is_rpdo   是否是RPDO
 * \param[out] p_pdo_num PDO NUM 号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_pdo_num_get (
    int inst_num, int pdo_id, aw_bool_t is_rpdo, int *p_pdo_num)
{
    aw_can_err_t  ret;
    CO_Data      *p_cop_data = NULL;
    uint16_t      pdo_index;
    uint16_t      cob_id_subindex = 1;
    uint16_t      offset_objdict;
    uint16_t      last_index;
    uint32_t      i;
    indextable   *p_index_table = NULL;
    uint32_t      error_code;

    if (p_pdo_num == NULL) {
        return -AW_CAN_ERR_INVALID_PARAMETER;
    }

    p_cop_data = aw_canfestival_inst_data_get(inst_num);

    if (p_cop_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    if (is_rpdo) {
        pdo_index = 0x1400;
        offset_objdict = p_cop_data->firstIndex->PDO_RCV;
        last_index = p_cop_data->lastIndex->PDO_RCV;
    } else {
        pdo_index = 0x1800;
        offset_objdict = p_cop_data->firstIndex->PDO_TRS;
        last_index = p_cop_data->lastIndex->PDO_TRS;
    }

    if (offset_objdict == 0 || last_index == 0) {
        return -AW_CAN_ERR_NO_OP;
    }

    *p_pdo_num = 0;
    ret = -AW_CAN_ERR_NO_OP;

    for (i = offset_objdict; i <= last_index; i++) {
        p_index_table = (indextable *)(*p_cop_data->scanIndexOD)(
            p_cop_data, pdo_index, &error_code);

        if (p_index_table != NULL &&
            (*(uint32_t *)(p_index_table->pSubindex[cob_id_subindex].pObject) ==
            pdo_id)) {
            *p_pdo_num = i - offset_objdict;
            ret = AW_CAN_ERR_NONE;
            break;
        }

        pdo_index++;
    }

    return ret;
}

/**
 * \brief CANfestival 失能所有RPDO或TPDO
 *
 * \param[in]  inst_num  实例号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_all_pdo_disable (int inst_num, aw_bool_t is_rpdo)
{
    CO_Data      *p_cop_data = NULL;
    uint16_t      pdo_index;
    uint16_t      cob_id_subindex = 1;
    uint16_t      offset_objdict;
    uint16_t      last_index;
    uint32_t      i;
    indextable   *p_index_table = NULL;
    uint32_t      error_code;

    p_cop_data = aw_canfestival_inst_data_get(inst_num);

    if (p_cop_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    if (is_rpdo) {
        pdo_index = 0x1400;
        offset_objdict = p_cop_data->firstIndex->PDO_RCV;
        last_index = p_cop_data->lastIndex->PDO_RCV;
    } else {
        pdo_index = 0x1800;
        offset_objdict = p_cop_data->firstIndex->PDO_TRS;
        last_index = p_cop_data->lastIndex->PDO_TRS;
    }

    if (offset_objdict == 0 || last_index == 0) {
        return -AW_CAN_ERR_NO_OP;
    }

    for (i = offset_objdict; i <= last_index; i++) {
        p_index_table = (indextable *)(*p_cop_data->scanIndexOD)(
            p_cop_data, pdo_index, &error_code);

        if (p_index_table != NULL) {
            *(uint32_t *)(p_index_table->pSubindex[cob_id_subindex].pObject) =
                __COP_PDO_ID_INVALID;
        }

        pdo_index++;
    }

    return AW_CAN_ERR_NONE;
}

/**
 * \brief CANfestival 实例添加PDO
 *
 * \param[in] inst_num 实例号
 * \param[in] pdo_id   PDO ID 号
 * \param[in] is_rpdo  是否是RPDO
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_pdo_add (
    int inst_num, int pdo_id, aw_bool_t is_rpdo)
{
    __cop_inst_t *p_cop_inst = NULL;
    CO_Data      *p_cop_data = NULL;
    aw_can_err_t  ret;
    int           pdo_num;
    uint16_t      pdo_index;
    uint32_t      exp_size;

    p_cop_inst = aw_canfestival_inst_get(inst_num);
    if (p_cop_inst == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    p_cop_data = aw_canfestival_inst_data_get(inst_num);
    if (p_cop_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    EnterMutex();
    ret = aw_canfestival_pdo_num_get(
        inst_num, __COP_PDO_ID_INVALID, is_rpdo, &pdo_num);
    if (ret != AW_CAN_ERR_NONE) {
        goto exit;
    }

    if (is_rpdo) {
        pdo_index = 0x1400 + pdo_num;
    } else {
        pdo_index = 0x1800 + pdo_num;
    }

    exp_size = sizeof(pdo_id);
    if (writeLocalDict(p_cop_data,
                       pdo_index,
                       0x01,
                       &pdo_id,
                       &exp_size,
                       RW) == OD_SUCCESSFUL) {
        ret = AW_CAN_ERR_NONE;
    } else {
        ret = -AW_CAN_ERR_NOT_INITIALIZED;
    }

exit:
    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival 实例移除PDO
 *
 * \param[in] inst_num 实例号
 * \param[in] pdo_id   PDO ID 号
 * \param[in] is_rpdo  是否是RPDO
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_pdo_remove (
    int inst_num, int pdo_id, aw_bool_t is_rpdo)
{
    __cop_inst_t *p_cop_inst = NULL;
    CO_Data      *p_cop_data = NULL;
    aw_can_err_t  ret;
    int           pdo_num;
    uint16_t      pdo_index;
    uint32_t      exp_size;

    p_cop_inst = aw_canfestival_inst_get(inst_num);
    if (p_cop_inst == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    p_cop_data = aw_canfestival_inst_data_get(inst_num);
    if (p_cop_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    EnterMutex();
    ret = aw_canfestival_pdo_num_get(inst_num, pdo_id, is_rpdo, &pdo_num);
    if (ret != AW_CAN_ERR_NONE) {
        goto exit;
    }

    if (is_rpdo) {
        pdo_index = 0x1400 + pdo_num;
    } else {
        pdo_index = 0x1800 + pdo_num;
    }

    pdo_id = __COP_PDO_ID_INVALID;
    exp_size = sizeof(pdo_id);
    if (writeLocalDict(p_cop_data,
                       pdo_index,
                       0x01,
                       &pdo_id,
                       &exp_size,
                       RW) == OD_SUCCESSFUL) {
        ret = AW_CAN_ERR_NONE;
    } else {
        ret = -AW_CAN_ERR_NOT_INITIALIZED;
    }

exit:
    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival 设置PDO数据
 *
 * \param[in]  inst_num  实例号
 * \param[in]  pdo_num   PDO NUM 号
 * \param[in]  is_rpdo   是否是RPDO
 * \param[in]  p_data    PDO 数据
 * \param[in]  length    PDO 数据长度
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_pdo_data_set (int        inst_num,
                                          int        pdo_num,
                                          aw_bool_t  is_rpdo,
                                          uint8_t   *p_data,
                                          uint8_t    length)
{
    CO_Data      *p_cop_data = NULL;
    indextable   *p_index_tab_map = NULL;
    uint8_t       map_cnt;
    uint32_t      i;

    p_cop_data = aw_canfestival_inst_data_get(inst_num);

    if (p_cop_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    if (is_rpdo) {
        p_index_tab_map = (indextable *)
            p_cop_data->objdict + p_cop_data->firstIndex->PDO_RCV_MAP + pdo_num;
    } else {
        p_index_tab_map = (indextable *)
            p_cop_data->objdict + p_cop_data->firstIndex->PDO_TRS_MAP + pdo_num;
    }

    map_cnt = *((uint8_t *)p_index_tab_map->pSubindex[0].pObject);

    for (i = 1; i <= map_cnt; i++) {
        uint32_t map_parameter =
            *((uint32_t *)p_index_tab_map->pSubindex[i].pObject);

        uint16_t index = (uint16_t)(map_parameter >> 16);
        uint8_t  subindex =
            (uint8_t)((map_parameter >> (uint8_t)8) & (uint32_t)0x000000FF);
        uint32_t size = (uint32_t) (map_parameter & (uint32_t)0x000000FF);

        uint32_t byte_size;
        uint32_t exp_size;
        uint32_t real_size;

        if (size ==0 || size > 64) {
            continue;
        }

        byte_size = 1 + ((size - 1) >> 3); /*1->8 => 1 ; 9->16 => 2, ... */
        exp_size = min(length, byte_size);
        real_size = exp_size;

        if (OD_SUCCESSFUL != writeLocalDict(
            p_cop_data, index, subindex, p_data, &real_size, RW)) {
            return -AW_CAN_ERR_ILLEGAL_DATA_LENGTH;
        }

        length -= exp_size;
        p_data += exp_size;
        if (length == 0) {
            break;
        }
    }

    sendOnePDOevent(p_cop_data, pdo_num);

    return AW_CAN_ERR_NONE;
}


/**
 * \brief CANfestival 获取PDO数据
 *
 * \param[in]  inst_num  实例号
 * \param[in]  pdo_num   PDO NUM 号
 * \param[in]  is_rpdo   是否是RPDO
 * \param[in]  p_data    PDO 数据
 * \param[in]  p_len     PDO 数据长度
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_pdo_data_get (int        inst_num,
                                          int        pdo_num,
                                          aw_bool_t  is_rpdo,
                                          uint8_t   *p_data,
                                          uint8_t   *p_len)
{
    CO_Data      *p_cop_data = NULL;
    indextable   *p_index_tab_map = NULL;
    uint8_t       map_cnt;
    uint32_t      i;

    p_cop_data = aw_canfestival_inst_data_get(inst_num);

    if (p_cop_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    if (is_rpdo) {
        p_index_tab_map = (indextable *)
            p_cop_data->objdict + p_cop_data->firstIndex->PDO_RCV_MAP + pdo_num;
    } else {
        p_index_tab_map = (indextable *)
            p_cop_data->objdict + p_cop_data->firstIndex->PDO_TRS_MAP + pdo_num;
    }

    *p_len = 0;
    map_cnt = *((uint8_t *)p_index_tab_map->pSubindex[0].pObject);

    for (i = 1; i <= map_cnt; i++) {
        uint8_t  data_type;
        uint32_t map_parameter =
            *((uint32_t *)p_index_tab_map->pSubindex[i].pObject);

        uint16_t index = (uint16_t)(map_parameter >> 16);
        uint8_t  subindex =
            (uint8_t)((map_parameter >> (uint8_t)8) & (uint32_t)0x000000FF);
        uint32_t size = (uint32_t) (map_parameter & (uint32_t)0x000000FF);

        uint32_t byte_size;

        if (size ==0 || size > 64) {
            continue;
        }

        byte_size = 1 + ((size - 1) >> 3); /*1->8 => 1 ; 9->16 => 2, ... */

        if (OD_SUCCESSFUL != readLocalDict(
            p_cop_data, index, subindex, p_data, &byte_size, &data_type, RW)) {
            return -AW_CAN_ERR_ILLEGAL_DATA_LENGTH;
        }

        *p_len += byte_size;
        p_data += byte_size;
    }

    return AW_CAN_ERR_NONE;
}

/**
 * \brief CANfestival 获取PDO数据
 *
 * \param[in]  inst_num  实例号
 * \param[in]  pdo_num   PDO NUM 号
 * \param[in]  p_data    PDO 数据
 * \param[in]  p_len     PDO 数据长度
 * \param[in]  wait_time 获取超时时间 ms
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_rpdo_rngbuf_data_get (int       inst_num,
                                                  int       pdo_num,
                                                  uint8_t  *p_data,
                                                  uint32_t *p_len,
                                                  uint32_t  wait_time)
{
    size_t        nbytes;
    Message       msg;
    aw_err_t      ret;
    __cop_inst_t *p_cop_inst = NULL;

    p_cop_inst = aw_canfestival_inst_get(inst_num);

    if (p_cop_inst == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    ret = AW_SEMC_TAKE(p_cop_inst->p_rpdo_rnbufs_msg_semc[pdo_num].semc,
                       aw_ms_to_ticks(wait_time));

    if (ret == AW_OK) {
        nbytes = aw_rngbuf_get(
            &(p_cop_inst->p_rpdo_rngbufs[pdo_num]), (char *)&msg, sizeof(msg));

        if (nbytes != sizeof(msg)) {
            return -AW_CAN_ERR_ABORT_TRANSMITTED;
        }

        memcpy(p_data, msg.data, msg.len);
        *p_len = msg.len;

        return AW_CAN_ERR_NONE;
    } else {
        return -AW_CAN_ERR_TRANSMITTED_TIMEOUT;
    }
}

/**
 * \brief CANfestival 实例启动
 *
 * \param[in] inst_num 实例号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_start (int inst_num)
{
    aw_can_err_t  ret;
    __cop_inst_t *p_cop_inst = NULL;

    p_cop_inst = aw_canfestival_inst_get(inst_num);

    if (p_cop_inst == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    EnterMutex();

    if (p_cop_inst->used ) {
        ret = -AW_CAN_ERR_CTRL_STARTED;
        goto exit;
    }

    p_cop_inst->used = AW_TRUE;
    p_cop_inst->status = 0;
    AW_TASK_RESUME(p_cop_inst->recv_task);
    ret = aw_can_start(p_cop_inst->chn);
    __COP_MSG(("controller start!\n"));

exit:
    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival 实例停止
 *
 * \param[in] inst_num 实例号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_stop (int inst_num)
{
    aw_can_err_t  ret;
    __cop_inst_t *p_cop_inst = NULL;

    p_cop_inst = aw_canfestival_inst_get(inst_num);

    if (p_cop_inst == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    EnterMutex();

    if ( !p_cop_inst->used ) {
        ret = -AW_CAN_ERR_CTRL_NOT_START;
        goto exit;
    }

    aw_can_stop(p_cop_inst->chn);
    p_cop_inst->used = AW_FALSE;
    p_cop_inst->status = 0;
    AW_TASK_SUSPEND(p_cop_inst->recv_task);
    __COP_MSG(("controller stop!\n"));

exit:
    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival 实例的初始化
 *
 * \param[in] inst_num 实例号
 * \param[in] brt      波特率 单位Kbps
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_init (int inst_num, int brt)
{
    aw_can_err_t         ret;
    __cop_inst_t        *p_cop_inst = NULL;
    CO_Data             *p_cop_inst_data = NULL;
    aw_can_baud_param_t *p_brt = NULL;

    p_cop_inst = aw_canfestival_inst_get(inst_num);
    p_cop_inst_data = aw_canfestival_inst_data_get(inst_num);

    if (p_cop_inst == NULL || p_cop_inst_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    EnterMutex();

    if (p_cop_inst->used ) {
        ret = -AW_CAN_ERR_CTRL_STARTED;
        goto exit;
    }

    p_brt = __cop_can_brt_get(p_cop_inst->chn, brt);
    if (p_brt == NULL) {
        __COP_MSG(("no baud param\n"));
        ret = -AW_CAN_ERR_INVALID_PARAMETER;
        goto exit;
    }

    /** \brief 初始化CAN */
    ret = aw_can_init(p_cop_inst->chn,
                      AW_CAN_WORK_MODE_NORMAL,
                      p_brt,
                      &__g_can_app_callback_fun);

    __COP_MSG(("controller init...\n"));
    if (ret != AW_CAN_ERR_NONE) {
        goto exit;
    }

    p_cop_inst->used = AW_FALSE;
    p_cop_inst->p_data = p_cop_inst_data;
    p_cop_inst_data->canHandle = p_cop_inst;

    if (__g_cop_inst_init_cnt == 0) {
        StartTimerLoop(NULL);
    }
    __g_cop_inst_init_cnt++;

exit:
    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival 实例的解初始化
 *
 * \param[in] inst_num 实例号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_deinit (int inst_num)
{
    aw_can_err_t         ret = AW_CAN_ERR_NONE;
    __cop_inst_t        *p_cop_inst = NULL;
    CO_Data             *p_cop_inst_data = NULL;

    p_cop_inst = aw_canfestival_inst_get(inst_num);
    p_cop_inst_data = aw_canfestival_inst_data_get(inst_num);

    if (p_cop_inst == NULL || p_cop_inst_data == NULL) {
        return -AW_CAN_ERR_NO_DEV;
    }

    EnterMutex();

    if (p_cop_inst->used ) {
        aw_canfestival_inst_stop(inst_num);
    }

    p_cop_inst->used = AW_FALSE;
    p_cop_inst->p_data = NULL;
    p_cop_inst_data->canHandle = NULL;

    __g_cop_inst_init_cnt--;
    if (__g_cop_inst_init_cnt == 0) {
        StopTimerLoop(NULL);
    }

    LeaveMutex();

    return ret;
}

/**
 * \brief CANfestival 初始化
 *
 * \param[in] inst_cnt       实例Canfestival的个数
 * \param[in] recv_task_prio 接收任务的优先级
 * \param[in] rpdo_fifo_cnt  每个RPDO拥有的FIFO数量
 * \param[in] p_hwtimer      需要提供一个定时器句柄
 * \param[in] p_inst_data    实例数据
 * \param[in] p_chn_brt_list 实例用到的CAN bus通道对应的波特率表
 *
 * \return AW_OK操作成功，其他错误原因参考错误码
 */
aw_err_t aw_canfestival_init (
    int                               inst_cnt,
    int                               recv_task_prio,
    int                               rpdo_fifo_cnt,
    const aw_hwtimer_handle_t         p_hwtimer,
    void                            **p_inst_data,
    const aw_canfestival_brt_list_t  *p_chn_brt_list)
{
    int i, j;
    int mem_size, inst_size, brp_list_size;
    int rpdo_cnt, all_rpdo_cnt;
    int per_rngbuf_mem_size, rpdo_rngbuf_size, rpdo_rngbuf_mem_size;
    int rpdo_rnbufs_msg_semc_size;
    int node_cnt, all_node_cnt;
    int node_info_size;

    __cop_inst_t              *p_cop_inst = NULL;
    CO_Data                   *p_cop_inst_data = NULL;
    aw_canfestival_brt_list_t *p_brt_list = NULL;
    aw_rngbuf_t                p_rpdo_rngbuf = NULL;
    __cop_msg_semc_t          *p_rpdo_rnbufs_msg_semc = NULL;
    uint8_t                   *p_rpdo_rngbuf_mem = NULL;
    __cop_node_info_t         *p_node_info = NULL;

    if (p_hwtimer == NULL || p_inst_data == NULL || p_chn_brt_list == NULL) {
        return -AW_EINVAL;
    }

    /* 统计多个实例总共的RPDO的个数 */
    all_rpdo_cnt = 0;
    for (i = 0; i < inst_cnt; i++) {
        p_cop_inst_data = p_inst_data[i];
        if (p_cop_inst_data == NULL) {
            return -AW_EINVAL;
        }
        rpdo_cnt = __inst_pdo_cnt(p_cop_inst_data, AW_TRUE);
        if (rpdo_cnt <= 0) {
            return -AW_EINVAL;
        }
        all_rpdo_cnt += rpdo_cnt;
    }

    /* 统计多个实例总共的Node的个数 */
    all_node_cnt = 0;
    for (i = 0; i < inst_cnt; i++) {
        p_cop_inst_data = p_inst_data[i];
        if (p_cop_inst_data == NULL) {
            return -AW_EINVAL;
        }
        node_cnt = __inst_node_cnt(p_cop_inst_data);
        if (node_cnt <= 0) {
            return -AW_EINVAL;
        }
        all_node_cnt += node_cnt;
    }

    /*
     * 内存的分布情况如下：
     * cop_inst[N]  brt_list[N]  rpdo_rngbuf[N]  rpdo_rnbufs_msg_semc[[N]] ->
     * rpdo_rngbuf_mem[N]  node_info[N]
     */
    inst_size = inst_cnt * sizeof(__cop_inst_t);

    brp_list_size = inst_cnt * sizeof(aw_canfestival_brt_list_t);

    rpdo_rngbuf_size = all_rpdo_cnt * sizeof(struct aw_rngbuf);
    rpdo_rnbufs_msg_semc_size = all_rpdo_cnt * sizeof(__cop_msg_semc_t);

    /* 一字节rngbuf要求，另外保留一字节用于对齐*/
    per_rngbuf_mem_size = rpdo_fifo_cnt * sizeof(Message) + 1 + 1;
    rpdo_rngbuf_mem_size = all_rpdo_cnt * per_rngbuf_mem_size;

    node_info_size = all_node_cnt * sizeof(__cop_node_info_t);

    mem_size = inst_size + brp_list_size + rpdo_rngbuf_size +
        rpdo_rnbufs_msg_semc_size + rpdo_rngbuf_mem_size + node_info_size;

    /* 根据内存排布，进行分配地址 */
    p_cop_inst = aw_mem_align(mem_size, sizeof(uint32_t));

    p_brt_list = 
        (aw_canfestival_brt_list_t *)((uint8_t *)p_cop_inst + inst_size);

    p_rpdo_rngbuf = (aw_rngbuf_t)((uint8_t *)p_brt_list + brp_list_size);

    p_rpdo_rnbufs_msg_semc = 
        (__cop_msg_semc_t *)((uint8_t *)p_rpdo_rngbuf + rpdo_rngbuf_size);
        
    p_rpdo_rngbuf_mem =
        (uint8_t *)p_rpdo_rnbufs_msg_semc + rpdo_rnbufs_msg_semc_size;

    p_node_info = (__cop_node_info_t *)
        ((uint8_t *)p_rpdo_rngbuf_mem + rpdo_rngbuf_mem_size);

    if (p_cop_inst == NULL) {
        return -AW_ENOMEM;
    }

    /* 对全局数据进行初始化  */
    memset(p_cop_inst, 0x00, mem_size);
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

        p_cop_inst[i].p_rpdo_rngbufs = p_rpdo_rngbuf;
        p_cop_inst[i].p_rpdo_rnbufs_msg_semc = p_rpdo_rnbufs_msg_semc;
        p_cop_inst[i].p_rpdo_rngbufs_mem = p_rpdo_rngbuf_mem;
        p_cop_inst[i].p_node_info = p_node_info;
        p_cop_inst_data = p_inst_data[i];

        node_cnt = __inst_node_cnt(p_cop_inst_data);
        rpdo_cnt = __inst_pdo_cnt(p_cop_inst_data, AW_TRUE);

        for (j = 0; j < rpdo_cnt; j++) {
            aw_rngbuf_init(
                &(p_cop_inst[i].p_rpdo_rngbufs[j]),
                (char *)
                &(p_cop_inst[i].p_rpdo_rngbufs_mem[j * per_rngbuf_mem_size]),
                per_rngbuf_mem_size - 1);

            AW_SEMC_INIT(p_cop_inst[i].p_rpdo_rnbufs_msg_semc[j].semc,
                         AW_SEM_EMPTY,
                         AW_SEM_Q_PRIORITY);
        }

        p_rpdo_rngbuf += rpdo_cnt;
        p_rpdo_rngbuf_mem += rpdo_cnt * per_rngbuf_mem_size;
        p_node_info += node_cnt * sizeof(__cop_node_info_t);
    }

    aw_canfestival_timer_register(p_hwtimer);

    __gp_cop_inst = p_cop_inst;
    __gp_cop_inst_data = (CO_Data **)p_inst_data;
    __gp_cop_brt_list = p_brt_list;
    __g_cop_inst_cnt = inst_cnt;
    __g_cop_inst_init_cnt = 0;

    return AW_OK;
}

/* end of file */
