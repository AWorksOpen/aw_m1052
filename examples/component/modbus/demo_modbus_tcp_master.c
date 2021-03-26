/*******************************************************************************
*                                  AWorks
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
 * \brief AWorks平台---Modbus主站演示例程（使用TCP连接）
 *
 * - 操作步骤：
 *   1. 使用TCP模式时，PC和开发板应该处于同一个网段；设置Modbus Slave上位机应该为TCP模式，
 *      需要在以太网配置文件中修改某些配置，修改项如下：
 *      1) 若与开发板连接的PC没有DHCP服务器则将__get_dhcp_en() 函数的返回值设为AW_FALSE表示禁能DHCP；
 *         并正确设置IP地址（和开发板应该处于同一网段）；
 *      2) 若与开发板连接的PC有DHCP服务器则将__get_dhcp_en() 函数的返回值设为AW_TRUE表示使能DHCP，
 *         并且无需关心设置的默认IP地址。
 *   2. 需在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - 调试串口
 *      - 网卡
 *   3. 打开Modubs从站上位机软件（如Modbus Slave）,TCP连接方式。设置从站允许访问的寄存器/线圈地
 *      址范围及个数，启动从站即可。
 *   4. 将板子中的TXD、RXD通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. Modbus从站上位机寄存器、线圈值按主站请求变化。
 *   2. 控制台打印调试信息。
 *
 * - 备注：
 *   1. 当使用Modbus串行模式服务时，所使用串口将不再允许使用平台上串口接口服务；

 *   3. 打开aw_prj_params.h文件中将要使用的网卡打开。

 *
 * \par 源代码
 * \snippet demo_modbus_tcp_master.c src_modbus_tcp_master
 *
 * \internal
 * \par Modification history
 * - 1.00 16-04-05  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_modbus_tcp_master Modbus主站(TCP连接)
 * \copydoc demo_modbus_tcp_master.c
 */

/** [src_modbus_tcp_master] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_serial.h"
#include "modbus/aw_mb_master.h"  /* modbus主站服务 */

/******************************************************************************/
/** \brief 用户自定义读写参数类型 */
struct wr_mask_param {
    uint16_t addr;     /**< \brief 寄存器地址 */
    uint16_t and_mask; /**< \brief 与操作值 */
    uint16_t or_mask;  /**< \brief 或操作值 */
} my_mask_param;

/** \brief 发送PDU数据处理接口 */
aw_local aw_mb_err_t __mb_snd_mask_handler (void     *p_params,
                                            uint8_t  *p_pdudata,
                                            uint32_t *p_pdudata_len)
{
    struct wr_mask_param *p_mask_param = (struct wr_mask_param *)p_params;

    p_pdudata[0] = (uint8_t)(p_mask_param->addr >> 8);
    p_pdudata[1] = (uint8_t)(p_mask_param->addr);

    p_pdudata[2] = (uint8_t)(p_mask_param->and_mask >> 8);
    p_pdudata[3] = (uint8_t)(p_mask_param->and_mask);

    p_pdudata[4] = (uint8_t)(p_mask_param->or_mask >> 8);
    p_pdudata[5] = (uint8_t)(p_mask_param->or_mask);

    *p_pdudata_len = 6;

    return AW_MB_ERR_NOERR;
}

/******************************************************************************/
#define MB_MASK_WRITE_REG_FUNCODE  22  /**< \brief 写寄存器掩码功能码 */
#define SLAVE_ADDR                 1   /**< \brief 目标从站地址 */

void demo_modbus_tcp_master_entry (void)
{
    uint8_t                 *p_buf = NULL;
    uint16_t                 rd_buf[20];
    uint16_t                 wr_buf[20];
    aw_mb_master_t           master;
    aw_mb_err_t              err     = AW_MB_ERR_NOERR;
    int                      i       = 0;
    aw_mb_master_rw_param_t  rw_param;
    uint8_t                  re_con_times = 3;  /* 最大重连次数 */

    /* TCP模式类型参数 */
    /* 目标IP地址：192.168.12.240  端口： 502 */
    /* 注：IP地址视实际情况而定，端口必须是502 */
    const aw_mb_tcp_client_params_t tcp_params = {"192.168.12.240", 502};

    master = aw_mb_master_create();
    if (master == NULL) {
        AW_ERRF(("Modbus Master Create Failed\r\n"));
        return;
    }

    /* 主站配置为TCP模式，配置目标IP地址、端口 */
    err = aw_mb_master_config(master, AW_MB_TCP, &tcp_params);

    if (err != AW_MB_ERR_NOERR) {
        AW_ERRF(("Modbus Master Config Failed, err: %d!\r\n", err));
        aw_mb_master_delete(master);  /* 配置失败，删除主站 */
        return ;
    }

    /* 注册写保持寄存器掩码功能码处理函数 */
    aw_mb_master_funcode_register(master,
                                  MB_MASK_WRITE_REG_FUNCODE,
                                  __mb_snd_mask_handler,
                                  NULL);

    while (re_con_times--) {
        /* 启动主站，若为TCP模式，在此建立连接 */
        err = aw_mb_master_start(master);
        if (err == AW_MB_ERR_NOERR) {
            break;
        }
    }

    if (err != AW_MB_ERR_NOERR) {
        AW_ERRF(("Modbus Master Start Failed, err: %d\r\n", err));
        return;
    } else {
        AW_INFOF(("Modbus Master Start.\r\n"));
    }

    AW_FOREVER {

        /* 读线圈---从地址1000开始，读取20个线圈值，放入rd_buf缓冲区中 */
        p_buf = (uint8_t *)rd_buf;
        err   = aw_mb_coils_read(master, SLAVE_ADDR, 1000, 20, p_buf);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("coils read:\n"));
            for (i = 0; i < 20; i++) {
                AW_INFOF(("rd_buf[%d] = %x\n", 1000 + i, p_buf[i]));
            }
        }
        memset(p_buf, 0, 20);

        /* 读离散量---从地址1000开始，读取20个离散量，放入rd_buf缓冲区中 */
        err = aw_mb_discrete_inputs_read(master, SLAVE_ADDR, 1000, 20, p_buf);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("discrete input read:\n"));
            for (i = 0; i < 20; i++) {
                AW_INFOF(("rd_buf[%d] = %x\n", 1000 + i, p_buf[i]));
            }
        }
        memset(p_buf, 0, 20);

        /* 读保持寄存器---从地址1000开始，读取20个保持寄存器，读取数据放入rd_buf缓冲区 */
        err = aw_mb_holding_reg_read(master, SLAVE_ADDR, 1000, 20, rd_buf);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("holding reg read:\n"));
            for (i = 0; i < 20; i++) {
                AW_INFOF(("rd_buf[%d] = %x\n", 1000 + i, rd_buf[i]));
            }
        }
        memset(rd_buf, 0, sizeof(rd_buf));

        /* 读输入寄存器---从地址1000开始，读取20个保持寄存器，读取数据放入rd_buf缓冲区 */
        err = aw_mb_input_reg_read(master, SLAVE_ADDR, 1000, 20, rd_buf);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("input reg read:\n"));
            for (i = 0; i < 20; i++) {
                AW_INFOF(("rd_buf[%d] = %x\n", 1000 + i, rd_buf[i]));
            }
        }
        memset(rd_buf, 0, sizeof(rd_buf));

        /* 写单个线圈---对地址为1000的线圈写ON */
        err = aw_mb_single_coil_write(master, SLAVE_ADDR, 1000, 0xFF00);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("\nsingle coil write success!\n"));
        }

        /* 写单个保持寄存器---对地址为1000的寄存器写入0x1234 */
        err = aw_mb_single_reg_write(master, SLAVE_ADDR, 1000, 0x1234);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("\nsingle reg write success!\n"));
        }

        /* 写多个线圈---从线圈地址1000开始的16个线圈写ON */
        wr_buf[0] = 0xFFFF;
        err = aw_mb_multiple_coils_write(master, SLAVE_ADDR,
                                         1000, 16, (uint8_t *)&wr_buf[0]);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("\nmultiple coils write success!\n"));
        }


        /* 写多个寄存器---从地址为1000开始，写入20个寄存器，20个寄存器值均为0xABCD */
        for (i = 0; i < 20; i++) {
            wr_buf[i] = 0xABCD;
        }
        err = aw_mb_multiple_regs_write(master, SLAVE_ADDR, 1000, 20, wr_buf);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("\nmultiple regs write success!\n"));
        }

        /* 读写多个寄存器
         * 从地址1000开始，读取20个寄存器，放入缓冲区rd_buf中，同时
         * 将缓冲区wr_buf中的20个值写入从地址1020开始的20个线圈中
         */
        rw_param.rd_addr  = 1000;
        rw_param.rd_num   = 10;
        rw_param.p_rd_buf = &rd_buf[0];
        rw_param.wr_addr  = 1001;
        rw_param.wr_num   = 10;
        rw_param.p_wr_buf = &wr_buf[0];
        err = aw_mb_multiple_regs_read_write(master, SLAVE_ADDR, &rw_param);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("\nmultiple regs read write success!\n"));
            for (i = 0; i < 10; i++) {
                AW_INFOF(("rd_buf[%d] = %x\n", i, rd_buf[i]));
            }
        }

        /* 写掩码功能---用户自定义 ---广播所有从站
         * 将地址1000的寄存器，与0x01,或0x0,即每次操作后地址1000的寄存器值为1
         */
        my_mask_param.addr     = 1000;
        my_mask_param.and_mask = 0x01;
        my_mask_param.or_mask  = 0;
        err = aw_mb_master_request(master, AW_MB_ADDRESS_BROADCAST,
                                   MB_MASK_WRITE_REG_FUNCODE, &my_mask_param);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("Write reg mask success!\n"));
        }

        aw_mdelay(1000);
    }
}

/** [src_modbus_tcp_master] */

/* end of file */
