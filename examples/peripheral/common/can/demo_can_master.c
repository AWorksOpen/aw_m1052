/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 高速CAN控制器演示例程
 * \note  本例程作为控制端，被控制端可利用 demo_can_slave.c。
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      AW_DEV_GPIO_KEY
 *      AW_DEV_PWM_BUZZER
 *      AW_COM_INPUT_EV_KEY、
 *      AW_COM_CONSOLE、
 *      相应平台的CAN、UART设备。
 *      若需要测试其他CAN设备，可修改例程中的CAN通道并使能aw_prj_params.h头文件中对应的CAN设备。
 *   2. 连接CAN的相关引脚到CAN收发器 RX=>RX TX=>TX，
 *      收发器再接到需要通讯的设备或通道CAN的收发器上 H=>H L=>L。
 *   3. 打开串口调试终端，波特率115200-8-N-1。
 *
 * - 实验现象：
 *   1. 按下按键KEY_1，本通道会发送控制LED报文，DATA每bit代表一个灯状态，打开所有灯
 *      (标准帧 ID=0X01 DATA=0XFF 0XFF 0XFF 0XFF 0XFF 0XFF 0XFF 0XFF)。
 *   2. 松开按键KEY_1，本通道会发送控制LED报文，DATA每bit代表一个灯状态，关闭所有灯
 *      (标准帧 ID=0X01 DATA=0X00 0X00 0X00 0X00 0X00 0X00 0X00 0X00)。
 *   3. 收到LED控制应答报文，蜂鸣器会响起
 *   4. 串口打印相关调试信息。
 *
 * \par 源代码
 * \snippet demo_can_master.c src_can_master
 *
 * \internal
 * \par Modification History
 * - 1.00 18-09-18  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_can_master 高速CAN控制器(主机)
 * \copydoc demo_can_master.c
 */

/** [src_can_master] */

#include "aworks.h"
#include "string.h"
#include "aw_can.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_buzzer.h"
#include "aw_vdebug.h"
#include "aw_input.h"
#include "aw_rngbuf.h"
#include "aw_spinlock.h"
#include "aw_prj_params.h"
#include "aw_demo_config.h"

#define __CAN_CHN                  DE_CAN_CH    /* CAN通道 */
#define __CAN_RECV_MSG_TIMEOUT     10           /* CAN报文接收超时时间 */
#define __CAN_SEND_MSG_TIMEOUT     300          /* CAN报文发送超时时间 */
#define __CAN_MSG_CNT              10           /* CAN消息个数 */

#define __CAN_RECV_TASK_STACK_SIZE 2048         /* CAN报文接收任务堆栈大小 */
#define __CAN_SEND_TASK_STACK_SIZE 2048         /* CAN报文发送任务堆栈大小 */
#define __CAN_RECV_TASK_PRIO       12           /* CAN报文接收任务优先级 */
#define __CAN_SEND_TASK_PRIO       13           /* CAN报文发送任务优先级 */

#define __CAN_MSG_ID_LED           0X01         /* 消息ID：LED控制功能码 */
#define __CAN_MSG_ID_LED_ACK       0X81         /* 消息ID：LED控制应答功能码 */

#undef AW_INFOF
#define AW_INFOF(x) aw_kprintf("CAN%d : ", __CAN_CHN); aw_kprintf x

/* CAN总线状态 */
static volatile aw_can_bus_status_t __g_can_bus_status = AW_CAN_BUS_STATUS_OK;

static aw_can_baud_param_t __g_can_btr = AW_CFG_CAN_BTR_500K;

/******************************************************************************/
static uint32_t __can_timestamp_get_cb (uint32_t timestamp);
static aw_bool_t __can_err_sta_cb (
    int chn, aw_can_bus_err_t err, aw_can_bus_status_t status);

static aw_can_app_callbacks_t __g_can_app_callback_fun = {
    __can_timestamp_get_cb,
    NULL,
    NULL,
    __can_err_sta_cb,
    NULL
};

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
 * \param[in] chn 通道号
 * \param[in] err CAN 总线的错误类型
 * \param[in] status CAN 总线的状态
 *
 * \retval AW_TRUE  不保存消息到接收rngbuf
 * \retval AW_FALSE 保存消息到接收rngbuf
 */
static aw_bool_t __can_err_sta_cb (
    int chn, aw_can_bus_err_t err, aw_can_bus_status_t status)
{
    aw_can_err_cnt_t  can_err_reg;

    aw_can_err_cnt_get(chn, &can_err_reg);
    if ((can_err_reg.tx_err_cnt > 127) || (can_err_reg.rx_err_cnt > 127)) {
        aw_can_err_cnt_clr(chn);
    }

    __g_can_bus_status = status;

    return AW_TRUE; /* 函数返回值决定错误消息是否存入rngbuf,AW_TRUE:不保存,AW_FALSE:保存 */
}

/**
 * \brief CAN初始化
 *
 * \param[in] 无
 *
 * \return AW_CAN_ERR_NONE正常启动，其他错误请参考错误代码
 */
static aw_can_err_t __can_startup (void)
{
    aw_can_err_t ret;

    /** \brief 初始化CAN */
    ret = aw_can_init(__CAN_CHN,                   /** \brief CAN通道号 */
                      AW_CAN_WORK_MODE_NORMAL,     /** \brief 控制器工作模式 */
                      &__g_can_btr,                /** \brief 当前波特率 */
                      &__g_can_app_callback_fun);  /** \brief 用户回调函数 */

    AW_INFOF(("controller init...\n"));

    if (ret != AW_CAN_ERR_NONE) {
        goto exit;
    }

    /** \brief 启动CAN */
    ret = aw_can_start(__CAN_CHN);
    AW_INFOF(("controller start!\n"));

exit:
    return ret;
}

/******************************************************************************/
/**
 * \brief 按键回调函数
 * \param[in] p_input_data  按键/指针输入事件数据
 * \param[in] p_usr_data    用户自定义数据
 */
aw_local void __key_proc_cb (aw_input_event_t *p_input_data, void *p_usr_data)
{
    struct aw_input_key_data *p_key_data =
                             (struct aw_input_key_data *)p_input_data;

    aw_can_std_msg_t msg;

    if (AW_INPUT_EV_KEY != p_input_data->ev_type || p_usr_data == NULL) {
        return;
    }

    if (p_key_data->key_code == KEY_1) {
        if (p_key_data->key_state) {
            AW_INFOF(("KEY1 is down\n"));

            /* 设置报文：控制所有LED灯打开 */
            memset(&msg, 0x00, sizeof(msg));
            msg.can_msg.id = __CAN_MSG_ID_LED;
            memset(msg.msgbuff, 0xFF, sizeof(msg.msgbuff));
            msg.can_msg.length = sizeof(msg.msgbuff);

            aw_rngbuf_put(p_usr_data, (char *)&msg, sizeof(msg));
        } else {
            AW_INFOF(("KEY1 is up\n"));

            /* 设置报文：控制所有LED灯关闭 */
            memset(&msg, 0x00, sizeof(msg));
            msg.can_msg.id = __CAN_MSG_ID_LED;
            msg.can_msg.length = sizeof(msg.msgbuff);

            aw_rngbuf_put(p_usr_data, (char *)&msg, sizeof(msg));
        }
    }
}

/**
 * \brief CAN发送任务入口
 *
 * \param[in] p_arg 任务参数
 *
 * \return 无
 */
static void* __can_send_task (void *p_arg)
{
    aw_can_err_t        ret;
    aw_spinlock_isr_t   lock;
    aw_can_bus_status_t bus_status;         /* CAN bus总线状态 */
    uint32_t            send_cnt      = 0;  /* 发送个数 */
    uint32_t            send_done_cnt = 0;  /* 发送完成个数 */
    aw_can_std_msg_t    msg[__CAN_MSG_CNT]; /* CAN信息缓冲区 */

    /* 初始化一个环形缓冲(__CAN_MSG_CNT个aw_can_std_msg_t大小的空间) */
    AW_RNGBUF_DEFINE(rngbuf, aw_can_std_msg_t, __CAN_MSG_CNT);

    struct aw_input_handler key_hdlr;

    /* 注册一个按键输入句柄到输入子系统 */
    /* 当按键按下，则会构建一个CAN标准帧放进环形缓冲 */
    aw_input_handler_register(&key_hdlr, __key_proc_cb, &rngbuf);

    aw_spinlock_isr_init(&lock, 0);

    AW_FOREVER {

        /* 获取环形缓冲已存在的CAN标准帧数量 */
        send_cnt = aw_rngbuf_nbytes(&rngbuf) / sizeof(aw_can_std_msg_t);
        if (send_cnt > 0) {

            /* 获取环形缓冲中的CAN标准帧 */
            aw_rngbuf_get(&rngbuf,
                          (char *)msg,
                          send_cnt * sizeof(aw_can_std_msg_t));

            /* 发送标准CAN报文 */
            ret = aw_can_std_msgs_send(__CAN_CHN,
                                       msg,
                                       send_cnt,
                                       &send_done_cnt,
                                       __CAN_SEND_MSG_TIMEOUT);

            if (ret == AW_CAN_ERR_NONE && send_done_cnt == send_cnt) {
                AW_INFOF(("send done! sent:%d\n", send_done_cnt));
            } else {
                AW_INFOF(("send error! sent:%d\n", send_done_cnt));
            }
        }

        aw_spinlock_isr_take(&lock);
        bus_status = __g_can_bus_status;
        __g_can_bus_status = AW_CAN_BUS_STATUS_OK;
        aw_spinlock_isr_give(&lock);

        if (bus_status == AW_CAN_BUS_STATUS_OFF) {

            /* 停止CAN */
            aw_can_stop(__CAN_CHN);
            AW_INFOF(("controller stop!\n"));

            /* 启动CAN */
            aw_can_start(__CAN_CHN);
            AW_INFOF(("controller start!\n"));
        }

        aw_mdelay(100);
    }

    return 0;
}

/**
 * \brief CAN接收任务入口
 *
 * \param[in] p_arg 任务参数
 *
 * \return 无
 */
static void* __can_recv_task (void *p_arg)
{
    aw_can_err_t     ret;
    uint32_t         recv_done_cnt = 0;  /* 接收完成个数 */
    aw_can_std_msg_t msg[__CAN_MSG_CNT]; /* CAN信息缓冲区 */

    AW_FOREVER {

        /* 接收标准CAN报文 */
        ret = aw_can_std_msgs_rcv(__CAN_CHN,
                                  msg,
                                  __CAN_MSG_CNT,
                                  &recv_done_cnt,
                                  __CAN_RECV_MSG_TIMEOUT);

        while ((ret == AW_CAN_ERR_NONE) && (recv_done_cnt--)) {

            AW_INFOF(("recv cnt:%d\n", recv_done_cnt + 1));
            if (msg[recv_done_cnt].can_msg.id == __CAN_MSG_ID_LED_ACK) {
                aw_buzzer_beep(100); /* 蜂鸣器鸣叫100毫秒 */
            }
        }
    }

    return 0;
}

/**
 * \brief 启动CAN、并建立CAN任务
 *
 * \param[in] 无
 *
 * \return 无
 */
void demo_can_master_entry (void)
{
    aw_can_err_t ret;

    aw_task_id_t    send_task;
    aw_task_id_t    recv_task;

    ret = __can_startup();

    if (ret != AW_CAN_ERR_NONE) {
        AW_INFOF(("controller startup failed:%d!\n", ret));
        return;
    }

    send_task = aw_task_create("can_send_task",
                               __CAN_SEND_TASK_PRIO,
                               __CAN_SEND_TASK_STACK_SIZE,
                               __can_send_task,
                               (void *)NULL);
    if (send_task == NULL) {
        aw_kprintf("can send task create failed\r\n");
        return;
    }

    recv_task = aw_task_create("can_recv_task",
                               __CAN_RECV_TASK_PRIO,
                               __CAN_RECV_TASK_STACK_SIZE,
                               __can_recv_task,
                               (void *)NULL);
    if (recv_task == NULL) {
        aw_kprintf("can recv task create failed\r\n");
        return;
    }

    aw_task_startup(send_task);
    aw_task_startup(recv_task);
}

/** [src_can_master] */

/* end of file */
