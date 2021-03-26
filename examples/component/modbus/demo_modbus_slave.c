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
 * \brief AWorks平台---Modbus从站演示例程
 *
 * - 操作步骤：
 *   1. Modbus主站使用串行模式时（RTU/ASCII模式），本例程是使用串口3
 *      - AW_COM_CONSOLE
 *      - 调试串口
 *      - 串口3
 *   2. 将串口3(TXD、RXD)通过USB转串口设备和PC相连；
 *   3. 打开Modubs主站上位机软件（如Modbus Poll）,设置串口参数（115200的波特率、偶校验、1个停止位），
 *      发送主站功能码请求即可。
 *
 * - 实验现象：
 *   1. Modbus主站上位机,按规定地址、规定寄存器/线圈个数，发送请求后能够读写线圈或寄存器正常；
 *
 * - 备注：
 *   1. 当使用Modbus串行模式服务时，所使用串口将不再允许使用平台上串口接口服务。
 *
 * \par 源代码
 * \snippet demo_modbus_slave.c src_modbus_slave
 *
 * \internal
 * \par Modification history
 * - 1.00 16-03-26  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_modbus_slave Modbus从站
 * \copydoc demo_modbus_slave.c
 */

/** [src_modbus_slave] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_serial.h"
#include "aw_demo_config.h"
#include "modbus/aw_mb_slave.h"  /* modbus从站服务  */
#include "modbus/aw_mb_utils.h"  /* modbus从站工具 */

/**
 * - 本例程注册了读/写线圈、读/写保持寄存器，读输入寄存器服务及扩展的写寄存器掩码服务
 * - 线圈起始地址为1000，线圈数为80个。
 * - 保持寄存器起始地址为1000， 寄存器个数为20个。
 * - 输入寄存器起始地址为1000， 寄存器个数为20个。
 * - 主站上位机发送请求需在规定的寄存器/线圈地址范围，及规定的寄存器/线圈个数范围，才能正常操作。
 */

#define MB_MASK_WRITE_REG_FUNCODE   22          /**< \brief 写寄存器掩码功能码 */

#define REG_HOLD_ADDR_START  1000               /**< \brief 保持寄存器起始地址 */
#define REG_HOLD_NUM         20                 /**< \brief 保持寄存器个数 */

#define REG_INPUT_ADDR_START 1000               /**< \brief 输入寄存器起始地址 */
#define REG_INPUT_NUM        20                 /**< \brief 输入寄存器个数 */

#define REG_COIL_ADDR_START  1000               /**< \brief 线圈起始地址 */
#define REG_COIL_NUM         10                 /**< \brief 线圈寄存器个数 */

aw_local uint16_t g_holding_reg[REG_HOLD_NUM];    /**< \brief 保持寄存器 */
aw_local uint8_t  g_coil_reg[REG_COIL_NUM];       /**< \brief 线圈个数 8*REG_COIL_NUM */
aw_local uint16_t g_input_reg[REG_INPUT_NUM] = {  /**< \brief 输入寄存器 */
    0,   1,  2,  3,  4,  5,  6,  7,  8,  9,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
};

/**
 * \brief 内存提取16位无符号数(大端模式)
 */
aw_local aw_inline uint16_t to_u16 (const uint8_t *buf)
{
    return ((uint16_t)buf[0] << 8) + buf[1];
}

/**
 * \brief 读线圈
 */
aw_local aw_mb_exception_t my_fun_rd_coil (aw_mb_slave_t  slave,
                                           uint8_t       *p_buf,
                                           uint16_t       addr,
                                           uint16_t       num)
{
    if ((addr < REG_COIL_ADDR_START) || num > (REG_COIL_NUM << 3)) {
        return AW_MB_EXP_ILLEGAL_DATA_ADDRESS;
    }

    /* 读线圈值到buf中 */
    aw_mb_bits_cp_to_buf(p_buf, g_coil_reg, addr - REG_COIL_ADDR_START, num);

    return AW_MB_EXP_NONE;
}

/**
 * \brief 写线圈
 */
aw_local aw_mb_exception_t my_fun_wr_coil (aw_mb_slave_t  slave,
                                           uint8_t       *p_buf,
                                           uint16_t       addr,
                                           uint16_t       num)
{

    if ((addr < REG_COIL_ADDR_START) || num > (REG_COIL_NUM << 3)) {
        return AW_MB_EXP_ILLEGAL_DATA_ADDRESS;
    }

    /* 将buf缓冲区值写到指定线圈 */
    aw_mb_bits_set_from_buf(g_coil_reg, p_buf, addr-REG_COIL_ADDR_START, num);

    return AW_MB_EXP_NONE;
}

/**
 * \brief 读保持寄存器
 */
aw_local aw_mb_exception_t my_fun_rd_holdreg (aw_mb_slave_t  slave,
                                              uint8_t       *p_buf,
                                              uint16_t       addr,
                                              uint16_t       num)
{
    uint8_t index;

    if ((addr < REG_HOLD_ADDR_START) ||
        (addr+num) > (REG_HOLD_ADDR_START+REG_HOLD_NUM)) {
        return AW_MB_EXP_ILLEGAL_DATA_ADDRESS;
    }

    index = (uint8_t)(addr - REG_HOLD_ADDR_START);

    /* 读取保持寄存器数据到buf中 */
    aw_mb_regcpy(p_buf, &g_holding_reg[index], num);

    return AW_MB_EXP_NONE;
}

/**
 * \brief 写保持寄存器
 */
aw_local aw_mb_exception_t my_fun_wr_holdreg (aw_mb_slave_t  slave,
                                              uint8_t       *p_buf,
                                              uint16_t       addr,
                                              uint16_t       num)
{
    uint8_t index;

    if ((addr < REG_HOLD_ADDR_START) ||
        (addr+num) > (REG_HOLD_ADDR_START+REG_HOLD_NUM)) {
        return AW_MB_EXP_ILLEGAL_DATA_ADDRESS;
    }

    index = (uint8_t)(addr - REG_HOLD_ADDR_START);

    /* 将buf数据写到保持寄存器数据 */
    aw_mb_regcpy(&g_holding_reg[index], p_buf, num);

    return AW_MB_EXP_NONE;
}

/**
 * \brief 读输入寄存器
 */
aw_local aw_mb_exception_t my_fun_rd_inputreg (aw_mb_slave_t  slave,
                                               uint8_t       *p_buf,
                                               uint16_t       addr,
                                               uint16_t       num)
{
    uint8_t index;

    if ((addr < REG_INPUT_ADDR_START) ||
        (addr + num) > (REG_INPUT_ADDR_START+REG_INPUT_NUM)) {
        return AW_MB_EXP_ILLEGAL_DATA_ADDRESS;
    }

    index = (uint8_t)(addr - REG_INPUT_ADDR_START);

    /* 读取输入寄存器数据到buf中 */
    aw_mb_regcpy(p_buf, &g_input_reg[index], num);

    return AW_MB_EXP_NONE;
}

/**
 * \brief 扩展写寄存器功能码掩码
 */
aw_local aw_mb_exception_t my_mask_write (aw_mb_slave_t  slave,
                                          uint8_t       *p_pdu,
                                          uint16_t      *p_len)
{
    uint16_t addr, and, or;

    if (*p_len != 7) {
        return AW_MB_EXP_ILLEGAL_DATA_VALUE;
    }

    addr = to_u16(p_pdu + 1);
    and  = to_u16(p_pdu + 3);
    or   = to_u16(p_pdu + 5);

    if ((addr < REG_HOLD_ADDR_START) || (addr > REG_HOLD_ADDR_START+REG_HOLD_NUM) ) {
        return AW_MB_EXP_ILLEGAL_DATA_ADDRESS;
    }

    g_holding_reg[addr - REG_HOLD_ADDR_START] &= and;
    g_holding_reg[addr - REG_HOLD_ADDR_START] |= or;

    return AW_MB_EXP_NONE;
}

/******************************************************************************/
/**
 * \brief Modbus 从站任务入口
 * \param[in] p_arg : 任务参数
 * \return 无
 */
void demo_modbus_slave_entry (void)
{
    uint8_t       again;
    aw_mb_err_t   err;
    aw_mb_slave_t slave;

    /* 初始化参数配置为：从机地址为1，串口波特率115200，无校验 */
    struct aw_mb_serial_param param = {1, AW_MB_PAR_EVEN, DE_MODBUS_UART_ID, 115200 };

    /* Modbus从机RTU模式初始化 */
    slave = aw_mb_slave_init(AW_MB_RTU, &param, &err);
    if (err != AW_MB_ERR_NOERR) {
        return ;
    }

    /* 读线圈回调 */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_COILS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_READ,
                                  my_fun_rd_coil);

    /** 读离散量回调 */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_DISCRETEINPUTS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_READ,
                                  my_fun_rd_coil);

    /* 写线圈回调 */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_COILS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_WRITE,
                                  my_fun_wr_coil);

    /* 读保持寄存器回调 */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_HOLDREGISTERS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_READ,
                                  my_fun_rd_holdreg);

    /* 写保持寄存器回调 */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_HOLDREGISTERS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_WRITE,
                                  my_fun_wr_holdreg);

    /* 读输入寄存器回调 */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_INPUTREGISTERS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_READ,
                                  my_fun_rd_inputreg);

    /* 注册扩展功能码处理函数 */
    aw_mb_slave_register_handler(slave,
                                 MB_MASK_WRITE_REG_FUNCODE,
                                 my_mask_write);

    /* 设置从机地址 */
    aw_mb_slave_set_addr(slave, 0x01);

    /* 开始运行 */
    err = aw_mb_slave_start(slave);
    if (err != AW_MB_ERR_NOERR) {
        return ;
    }

    AW_INFOF(("\nModbus Slave start.\n"));

    again = 0;
    AW_FOREVER {
        err = aw_mb_slave_poll(slave);    /* 从机轮询操作 */
        if (err != AW_MB_ERR_NOERR ) {
            again++;
        }

        if (again > 20) {
            break;
        }
        aw_mdelay(5);
    }

    AW_INFOF(("\nModbus Slave exit.\n"));
    aw_mb_slave_stop(slave);    /* 停止从机 */
    aw_mb_slave_close(slave);   /* 关闭从机，退出 */

    for (;;) {
        aw_mdelay(1000);
    }
}

/** [src_modbus_slave] */

/* end of file */
