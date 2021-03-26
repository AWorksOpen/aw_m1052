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
 * \note  本例程进行单发、单收的速度测试。
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      AW_COM_CONSOLE、
 *      相应平台的CAN、UART设备。
 *      若需要测试其他CAN设备，可修改例程中的CAN通道并使能aw_prj_params.h头文件中对应的CAN设备。
 *   2. 连接CAN的相关引脚到CAN收发器 RX=>RX TX=>TX，
 *      收发器再接到USBCAN-II设备上 H=>H L=>L, CAN波特率1000K。
 *   3. 打开串口调试终端，波特率115200-8-N-1。
 *
 * - 实验现象：
 *   1. __CAN_TEST_MODE = __CAN_TEST_MSG_RECV
 *      则打开ZCANPRO上位机连接启动设备后，设置不断的发送报文，
 *      达到100000帧后，进行统计接收速度。
 *   2. __CAN_TEST_MODE = __CAN_TEST_MSG_SEND
 *      则打开ZCANPRO上位机连接启动设备后，会不断的收取报文，
 *      发送达到100000帧，进行统计发送速度。
 *   3. 串口打印相关调试信息。
 *
 * \par 源代码
 * \snippet demo_can_speed.c src_can_speed
 *
 * \internal
 * \par Modification History
 * - 1.00 18-09-18  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_can_speed 高速CAN控制器(测速)
 * \copydoc demo_can_speed.c
 */

/** [src_can_speed] */

#include "aworks.h"
#include "string.h"
#include "aw_can.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_spinlock.h"
#include "aw_system.h"
#include "aw_prj_params.h"
#include "aw_demo_config.h"

#define __CAN_CHN                  DE_CAN_CH    /* CAN通道 */
#define __CAN_RECV_MSG_TIMEOUT     10           /* CAN报文接收超时时间 */
#define __CAN_SEND_MSG_TIMEOUT     300          /* CAN报文发送超时时间 */
#define __CAN_MSG_CNT              128          /* CAN消息个数 */

#define __CAN_TASK_STACK_SIZE      4096         /* CAN报文接收任务堆栈大小 */
#define __CAN_TASK_PRIO            12           /* CAN报文接收任务优先级 */

#define __CAN_TEST_MSG_CNT         100000       /* CAN共收发多少帧才进行统计 */

#define __CAN_TEST_MSG_RECV        0            /* CAN单收速度测试 */
#define __CAN_TEST_MSG_SEND        1            /* CAN单发速度测试 */
#define __CAN_TEST_MODE            __CAN_TEST_MSG_SEND /* CAN测试模式选择 */

#undef AW_INFOF
#define AW_INFOF(x) aw_kprintf("CAN%d : ", __CAN_CHN); aw_kprintf x

/* CAN总线状态 */
static volatile aw_can_bus_status_t __g_can_bus_status = AW_CAN_BUS_STATUS_OK;

static aw_can_baud_param_t __g_can_btr = AW_CFG_CAN_BTR_1000K;

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

/**
 * \brief CAN任务入口
 *
 * \param[in] p_arg 任务参数
 *
 * \return 无
 */
#if __CAN_TEST_MODE == __CAN_TEST_MSG_SEND
aw_local void* __task_handle (void *p_arg)
{
    uint32_t            i;
    aw_tick_t           t1 = 0, t2 = 0;
    aw_can_err_t        ret;
    aw_spinlock_isr_t   lock;
    aw_can_bus_status_t bus_status;         /* CAN bus总线状态 */
    uint32_t            total_send_cnt = 0; /* 总共发送的个数 */
    uint32_t            send_done_cnt  = 0; /* 发送完成个数 */
    aw_can_std_msg_t    msg[__CAN_MSG_CNT]; /* CAN信息缓冲区 */

    /* 初始化CAN*/
    ret = __can_startup();

    if (ret != AW_CAN_ERR_NONE) {
        AW_INFOF(("controller startup failed:%d!\n", ret));
        goto exit;
    }

    aw_spinlock_isr_init(&lock, 0);

    /* 配置发送的参数 */
    memset(msg, 0x00, sizeof(msg));
    for (i = 0; i < __CAN_MSG_CNT; i++) {
        msg[i].can_msg.length = 8;
        msg[i].can_msg.flags  = AW_CAN_MSG_FLAG_SEND_NORMAL;
    }

    AW_FOREVER {

        /* 记录发送起始时间 */
        if (t1 == 0) {
            t1 = aw_sys_tick_get();
        }

        /* 发送标准CAN报文 */
        ret = aw_can_std_msgs_send(__CAN_CHN,
                                   msg,
                                   __CAN_MSG_CNT,
                                   &send_done_cnt,
                                   __CAN_SEND_MSG_TIMEOUT);

        total_send_cnt += send_done_cnt;

        if (total_send_cnt >= __CAN_TEST_MSG_CNT) {

            /* 记录发送全部消息的时间 */
            t2 = aw_sys_tick_get() - t1;

            AW_INFOF(("send total cnt:%d tick:%d, speed:%d frame/s\n",
                      total_send_cnt,
                      t2,
                      (uint32_t)((uint64_t)total_send_cnt * 1000 / t2)));

            t1 = 0;
            total_send_cnt = 0;
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
    }

exit:
    AW_FOREVER {
        aw_mdelay(1000);
    }

    return 0;
}
#else
aw_local void* __task_handle (void *p_arg)
{
    aw_can_err_t        ret;
    aw_spinlock_isr_t   lock;
    aw_tick_t           t1 = 0, t2 = 0;
    aw_can_bus_status_t bus_status;         /* CAN bus总线状态 */
    uint32_t            recv_fifo_size = 0; /* CAN FIFO大小 */
    uint32_t            recv_cnt       = 0; /* 一次接收的个数 */
    uint32_t            recv_done_cnt  = 0; /* 接收完成个数 */
    uint32_t            total_recv_cnt = 0; /* 共接收的个数 */
    aw_can_std_msg_t    msg[__CAN_MSG_CNT]; /* CAN信息缓冲区 */

    ret = __can_startup();

    if (ret != AW_CAN_ERR_NONE) {
        AW_INFOF(("controller startup failed:%d!\n", ret));
        goto exit;
    }

    aw_spinlock_isr_init(&lock, 0);

    /* 获取接收FIFO大小 */
    recv_fifo_size = aw_can_rcv_fifo_size(__CAN_CHN);

    AW_INFOF(("receive fifo size %d messages.\n", recv_fifo_size));

    recv_cnt = min(recv_fifo_size, __CAN_MSG_CNT);

    AW_INFOF(("receive cnt %d messages.\n", recv_cnt));

    AW_FOREVER {

        /* 接收标准CAN报文 */
        ret = aw_can_std_msgs_rcv(__CAN_CHN,
                                  msg,
                                  recv_cnt,
                                  &recv_done_cnt,
                                  __CAN_RECV_MSG_TIMEOUT);

        if ((ret == AW_CAN_ERR_NONE) && (recv_done_cnt > 0)) {
            if (t1 == 0) {
                t1 = aw_sys_tick_get();
            }

            total_recv_cnt += recv_done_cnt;

            if (total_recv_cnt >= __CAN_TEST_MSG_CNT) {

                t2 = aw_sys_tick_get() - t1;

                AW_INFOF(("recv total cnt:%d tick:%d, speed:%d frame/s\n",
                          total_recv_cnt,
                          t2,
                          (uint32_t)((uint64_t)total_recv_cnt * 1000 / t2)));

                t1 = 0;
                total_recv_cnt = 0;
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
    }

exit:
    AW_FOREVER {
        aw_mdelay(1000);
    }
    
    return 0;
}
#endif

/**
 * \brief 建立CAN任务
 *
 * \param[in] 无
 *
 * \return 无
 */
void demo_can_speed_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("can_task",
                         __CAN_TASK_PRIO,
                         __CAN_TASK_STACK_SIZE,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("can speed task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);
}

/** [src_can_speed] */

/* end of file */
