/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief LoRaNet例程（集中器）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_DEV_SX127X (具体模块使用的SPI、GPIO引脚，请查看awbus_hwconf_sx127x.h)
 *      - AW_COM_CONSOLE
 *      - AW_COM_SHELL_SERIAL
 *      - AW_DEV_GPIO_LED
 *      - AW_DEV_EXTEND_PCF85063
 *      - 调试串口使能宏
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 准备两个设备，一个设备下载该例程，另一个下载demo_lrnet_router.c例程或者demo_lrnet_terminal.c例程；
 *
 * - 实验现象：
 *   1. 该demo会注册相关shell命令，如：
 *      - cdetect   即集中器探测周围的路由或终端
 *      - cping     即集中器 ping 路由或终端
 *      - cgfetch   即集中器对路由表中的所有终端进行分组采集（需要先执行 cdetect 命令，填充路由表信息）
 *      用户可以通过执行shell命令来测试LoRaNet协议栈 center 相关接口
 *   2. 执行 "cdetect" shell命令，串口将打印出集中器探测到的 "路由" 或者 "终端" 设备信息；
 *   3. 从第 2 步中获得终端或者路由的地址，执行 "cping 0x0101" shell命令，串口将打印出集中器和地址为  "0x0101" 的  "终端" 的连接状态；
 *      "0x0101" 地址可改变，用户可通过 "cdetect" 查看"路由" 或者 "终端"地址。
 *   5. 执行  "cgfetch" shell命令，串口将打印出集中器和路由表中的 "终端" 设备进行收发数据信息；
 *      执行  "cgfetch" shell命前， 需要先执行 cdetect 命令，填充路由表信息。
 *
 * - 备注：
 *   1.终端可以直接挂载在集中器下；也可以挂载在路由下，通过路由(交换数据)和集中器连接。关系图如下:
 *                      集中器—— —— ——>路由—— —— ——>终端
 *                        |
 *                        |
 *                       \|/
 *                        .
 *                       终端
 *
 * \par 源代码
 * \snippet demo_lrnet_center.c src_lrnet_center
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lrnet_center LoRaNet集中器
 * \copydoc demo_lrnet_center.c
 */

/** [src_lrnet_center] */

#include <stdio.h>
#include "aworks.h"
#include "aw_shell.h"
#include "am_lrnet.h"
#include "am_lora_utilities.h"
#include "common/demo_lrnet_common.h"


/*
 * 输出打印路由表
 */
static void __print_route_table (void)
{
    uint8_t  r_buf[2];
    uint16_t t_buf[2];
    int      r_origin, t_origin, r_num, t_num, i, j, terminal_cnt, router_cnt;

    am_lora_kprintf("\r\n"
                    "\r\n--------------------------------------"
                    "\r\n  Route Table Print"
                    "\r\n--------------------------------------");

    router_cnt   = 0;
    terminal_cnt = 0;
    r_origin     = 0;

    /* 遍历路由表信息 */
    r_num = am_lrnet_center_routers_get(r_buf, sizeof(r_buf), &r_origin);
    uint8_t r_addr = r_buf[0];
    /*打印集中器地址，集中器地址固定存放在路由表的首位*/
    am_lora_kprintf("\r\n\r\nCenter addr: 0x%02X\r\n", (int)r_addr);

    /* 遍集中器下的终端 */
    t_origin = 0;
    while (0 != (t_num = am_lrnet_center_route_sub_dev_get(
                            r_addr, t_buf, sizeof(t_buf), &t_origin))) {
        for(j = 0; j < t_num; j++) {
            uint16_t t_addr = t_buf[j];

            if (AM_LRNET_ADDR_IS_TERMINAL(t_addr)) {
                am_lora_kprintf("\r\nTerminal addr: 0x%04X\r\n", (int) t_addr);
                terminal_cnt += 1;
            }
        }
    }
    /*打印集中器下挂载的终端*/
    if(terminal_cnt > 0){
        am_lora_kprintf("\r\n\r\nCenter 1, Terminal %d\r\n--------------------", (int) terminal_cnt);
    }

    /*遍历挂载在集中器下的路由*/
    if (r_num > 1) {
        terminal_cnt = 0;
        for (i = 1; i < r_num; i++) {
            uint8_t r_addr = r_buf[i];
            /*打印路由地址*/
            am_lora_kprintf("\r\n\r\nRouter addr: 0x%02X\r\n", (int) r_addr);

            /* 遍历挂载在路由下的终端  */
            t_origin = 0;
            while (0 != (t_num = am_lrnet_center_route_sub_dev_get(
                                    r_addr, t_buf, sizeof(t_buf), &t_origin))) {
                for(j = 0; j < t_num; j++) {
                    uint16_t t_addr = t_buf[j];

                    if (AM_LRNET_ADDR_IS_TERMINAL(t_addr)) {
                        am_lora_kprintf("\r\nTerminal 0x%04X", (int) t_addr);
                        terminal_cnt += 1;
                    }
                }
            }
            router_cnt++;
        }
    }
    if(router_cnt > 0){
        am_lora_kprintf("\r\n\r\nCenter 1, Router %d, Terminal %d",
                         (int) router_cnt, (int) terminal_cnt);
    }

    return ;
}

static void __gfetch_rx_cb (void *p_arg, am_lrnet_data_t *p_rx_data)
{
    if (0 != p_rx_data->data_size) {
        am_lora_kprintf("\r\nGFetch 0x%04X Rx: %s",
                        p_rx_data->dev_addr, p_rx_data->p_data);
    } else {
        am_lora_kprintf("\r\nGFetch 0x%04X Rx: Invalid",
                         p_rx_data->dev_addr);
    }
}

static void __gfetch_finish_cb (void *p_arg)
{
    am_lora_kprintf("\r\nGFetch Completed.");
}

static int __fetch_request_cb (am_lrnet_fetch_request_t *p_request)
{
    am_lora_kprintf("\r\nFetch Request: %s", p_request->p_data);
    return 0;   /* do not reply */
}

static void __upstream_rx_cb (am_lrnet_data_t *p_data)
{
    am_lora_kprintf("\r\nUpstream Rx: '%s'", p_data->p_data);
}

static void __exception_rx_cb (am_lrnet_data_t *p_data)
{
    am_lora_kprintf("\r\nException Rx: '%s'", p_data->p_data);
}

/*
 * 执行探测组网
 * 参数sub_router_num    :  预期的路由数量
 * 参数sub_terminal_num  :  预期的终端数量
 */
static void __detect_test (int sub_router_num, int sub_terminal_num)
{
    uint32_t  sec1, sec2;

    am_lora_kprintf("\r\nDetect Startup.");

    sec1 = am_lora_utilities_rtc_sec_get();
    if (AM_LORA_RET_OK == am_lrnet_center_detect(sub_router_num, sub_terminal_num)) {
        sec2 = am_lora_utilities_rtc_sec_get();
        am_lora_kprintf("\r\nNetlink Completed! %ds", sec2-sec1);
        __print_route_table();
    }
}

/*
 * 分组采集测试
 * 对路由表中的所有终端进行分组采集
 */
static void __gfetch_test (void)
{
    am_lrnet_center_gfetch_cfg_t cfg;
    int                          ret;

    /* 分组采集配置 */
    cfg.p_send_data     = (uint8_t *) CMD_GET_TEMPERATURE;
    cfg.send_data_size  = sizeof(CMD_GET_TEMPERATURE);
    cfg.fetch_data_size = _GFETCH_ACK_DATA_SIZE;

    /* 启动分组采集 */
    ret = am_lrnet_center_gfetch_asyn(&cfg,              /* 分组采集配置 */
                                      __gfetch_rx_cb,     /* 收到数据回调 */
                                      __gfetch_finish_cb, /* 采集完毕回调 */
                                      (void*) 0);        /* 参数p_arg */
    if (AM_LORA_RET_OK == ret) {
        am_lora_kprintf("\r\nGFetch Startup. Sent: %s",
                        (char*) cfg.p_send_data);
    } else {
        am_lora_kprintf("\r\nGFetch Startup Failed, error code=%d", (int) ret);
    }
}

/*
 * 分组采集（指定） 测试
 * 对指定路由下的指定的若干台终端执行分组采集
 */
static void __gfetch_specific_test (void)
{
    const uint16_t addr_list[] = { 0x0100, 0x0101, 0x0102, 0x0103 };

    am_lrnet_center_gfetch_cfg_t cfg;
    int                          ret;

    /* 分组采集配置 */
    cfg.p_send_data     = (uint8_t *) CMD_GET_TEMPERATURE;
    cfg.send_data_size  = sizeof(CMD_GET_TEMPERATURE);
    cfg.fetch_data_size = _GFETCH_ACK_DATA_SIZE;

    ret= am_lrnet_center_gfetch_specific_asyn(
                 &cfg,
                 AM_LRNET_ADDR_CENTER,
                 addr_list,
                 AM_LORA_NELEMENTS(addr_list),
                 __gfetch_rx_cb,
                 __gfetch_finish_cb,
                 (void*)0);
    if (ret != AM_LORA_RET_OK) {
        am_lora_kprintf("\r\nGFetch_Specific Failed, error code=%d", ret);
    }
}

/*
 * 执行授时广播
 */
static void __timebc_test (void)
{
    int ret = 0;

    am_lora_kprintf("\r\nTimeBc Test Start");

    ret = am_lrnet_center_time_broadcast();
    if (ret == AM_LORA_RET_OK) {
        am_lora_kprintf("\r\nTimeBc Test Success");
    } else {
        am_lora_kprintf("\r\nTimeBc Test Failed");
    }
}

/*
 * 分时上报广播测试
 * 设备群在每日00:10:00执行分时上报，从终端0x0100开始根据地址递增依次上报
 * 例如终端0x0100-0x01FF共256
 * 请注意定期执行授时广播以保持RTC时间同步
 */
static void __upstreambc_test (void)
{
    am_lrnet_upstream_cfg_t cfg = {
        3,              /* 上传时间分片（单位：秒） */
        { 0 }           /* 每日上传时间设置 */
    };

    am_lora_kprintf("\r\nUpstreamBc Test Start");

    AM_LRNET_UPSTREAM_TIME_BITMAP_SET(cfg.time_bitmap, 0, 10);
    (void) am_lrnet_center_upstream_broadcast(&cfg);

    am_lora_kprintf("\r\nUpstreamBc Test Finish");
}

/*
 * 路由多跳测试：
 * 构建路由表：center <- 0x01 <- 0x02 <- 0x03 <- 0x0100
 * 若对0x0100执行fetch操作，成功获取返回数据，则测试成功
 */
static void __multihop_test (void)
{
    uint8_t         rx_buf[20];
    am_lrnet_data_t tx_data, rx_data;

    am_lrnet_center_route_add(0x01, 0x00);
    am_lrnet_center_route_add(0x02, 0x01);
    am_lrnet_center_route_add(0x03, 0x02);
    am_lrnet_center_route_add(0x0100, 0x03);

    tx_data.p_data    = (uint8_t *) CMD_GET_TEMPERATURE;
    tx_data.data_size = sizeof(CMD_GET_TEMPERATURE);
    tx_data.dev_addr  = 0x0100;
    if (AM_LORA_RET_OK == am_lrnet_center_fetch(&tx_data, rx_buf, sizeof(rx_buf), &rx_data)) {
        am_lora_kprintf("\r\n%s", (char *) rx_data.p_data);
    } else {
        am_lora_kprintf("\r\nFetch Failed");
    }
}

#ifdef AW_ARM926EJ_S
extern volatile uint32_t  g_max_tick_use;
extern volatile uint32_t  g_sm_max_tick_use;
static int  __get_max_use_tick(int                   argc,
                               char                 *argv[],
                               struct aw_shell_user *p_shell_user)
{
    aw_kprintf("sm max: %u, exe max: %u\n", g_sm_max_tick_use, g_max_tick_use);

    return  0;
}
#endif

static int __center_timebc_test_cmd (int                   argc,
                                     char                 *argv[],
                                     struct aw_shell_user *p_shell_user)
{
    __timebc_test();

    return  AW_OK;
}

static int __center_upstreambc_test_cmd (int                   argc,
                                         char                 *argv[],
                                         struct aw_shell_user *p_shell_user)
{
    __upstreambc_test();

    return  AW_OK;
}

static int __center_detect_cmd (int                   argc,
                                char                 *argv[],
                                struct aw_shell_user *p_shell_user)
{
    __detect_test(10, 100);

    return  AW_OK;
}

static int __center_gfetch_cmd (int                   argc,
                                char                 *argv[],
                                struct aw_shell_user *p_shell_user)
{
    __gfetch_test();

    return  AW_OK;
}

static int __center_ping_cmd (int                   argc,
                              char                 *argv[],
                              struct aw_shell_user *p_shell_user)
{
    int       used_ms = 0;
    uint16_t  addr    = 0x0101;

    am_lora_kprintf("\r\nPing Test Start\r\n");

    if (argc == 1) {
        addr = lrnet_simple_strtoul(argv[0], NULL, 16);
        am_lora_kprintf("addr: 0x%04x\n", addr);

        used_ms = am_lrnet_center_ping(addr, 20);
        if (used_ms >= 0) {
            am_lora_kprintf("\r\nPing %d(20byte) Test Success, %dms", addr, used_ms);
        } else {
            am_lora_kprintf("\r\nPing %d(20byte) Test Failed", addr);
        }

        return  AW_OK;
    }else{
        am_lora_kprintf("must designate addr\n");
        return AW_ERROR;
    }
}

static const aw_shell_cmd_item_t __g_center_cmds[] = {
    {__center_detect_cmd, "cdetect",  "center detect"},
    {__center_gfetch_cmd, "cgfetch",  "center gfetch"},
    {__center_ping_cmd, "cping",  "<addr> center ping"},
    {__center_upstreambc_test_cmd, "cupsbc",  "center broadcast the time-sharing report"},
    {__center_timebc_test_cmd, "ctimebc",  "center broadcast timing"},
#ifdef AW_ARM926EJ_S
    {__get_max_use_tick, "get_tick", "get loranet use max tick"},
#endif
};

static void __center_cmd_init(void)
{
    static aw_shell_cmd_list_t cmd_list;

    AW_SHELL_REGISTER_CMDS(&cmd_list, __g_center_cmds);
}

/**
 * \brief LoRaNet center demo
 *
 * \return 无
 */
void  demo_lrnet_center_entry (void)
{
    int  ret;

    const am_lrnet_center_cfg_t cfg = {
            &g_lrnet_network_cfg,                   /* 网络配置 */
            {
                AM_LRNET_ADDR_CENTER,               /* 设备地址 */
                2,                                  /* 最大重发次数 */
                AM_LRNET_MODE_NORMAL                /* 工作模式 */
            },
            {
                {lrnet_common_rf_state_changed_cb }, /* RF状态改变回调 */
                __fetch_request_cb,                  /* 获取请求 */
                __upstream_rx_cb,                    /* 收到定时上报数据回调 */
                __exception_rx_cb                    /* 收到异常上报回调 */
            }
    };

    /* 调试信息输出 */
    lrnet_debug_startup_info("Center", cfg.p_net_cfg, &cfg.role_cfg);

    /* 初始化LoRaNet集中器设备 */
    ret = am_lrnet_center_init(&cfg);
    if (ret != 0) {
        am_lora_kprintf("\r\nInitialize Failed. Error Code %d", ret);
        while(1);
    }

    /* 初始化 shell 命令 */
    __center_cmd_init();
}

/** [src_lrnet_center] */

/* end of file */
