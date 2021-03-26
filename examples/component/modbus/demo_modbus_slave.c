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
 * \brief AWorksƽ̨---Modbus��վ��ʾ����
 *
 * - �������裺
 *   1. Modbus��վʹ�ô���ģʽʱ��RTU/ASCIIģʽ������������ʹ�ô���3
 *      - AW_COM_CONSOLE
 *      - ���Դ���
 *      - ����3
 *   2. ������3(TXD��RXD)ͨ��USBת�����豸��PC������
 *   3. ��Modubs��վ��λ���������Modbus Poll��,���ô��ڲ�����115200�Ĳ����ʡ�żУ�顢1��ֹͣλ����
 *      ������վ���������󼴿ɡ�
 *
 * - ʵ������
 *   1. Modbus��վ��λ��,���涨��ַ���涨�Ĵ���/��Ȧ����������������ܹ���д��Ȧ��Ĵ���������
 *
 * - ��ע��
 *   1. ��ʹ��Modbus����ģʽ����ʱ����ʹ�ô��ڽ���������ʹ��ƽ̨�ϴ��ڽӿڷ���
 *
 * \par Դ����
 * \snippet demo_modbus_slave.c src_modbus_slave
 *
 * \internal
 * \par Modification history
 * - 1.00 16-03-26  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_modbus_slave Modbus��վ
 * \copydoc demo_modbus_slave.c
 */

/** [src_modbus_slave] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_serial.h"
#include "aw_demo_config.h"
#include "modbus/aw_mb_slave.h"  /* modbus��վ����  */
#include "modbus/aw_mb_utils.h"  /* modbus��վ���� */

/**
 * - ������ע���˶�/д��Ȧ����/д���ּĴ�����������Ĵ���������չ��д�Ĵ����������
 * - ��Ȧ��ʼ��ַΪ1000����Ȧ��Ϊ80����
 * - ���ּĴ�����ʼ��ַΪ1000�� �Ĵ�������Ϊ20����
 * - ����Ĵ�����ʼ��ַΪ1000�� �Ĵ�������Ϊ20����
 * - ��վ��λ�������������ڹ涨�ļĴ���/��Ȧ��ַ��Χ�����涨�ļĴ���/��Ȧ������Χ����������������
 */

#define MB_MASK_WRITE_REG_FUNCODE   22          /**< \brief д�Ĵ������빦���� */

#define REG_HOLD_ADDR_START  1000               /**< \brief ���ּĴ�����ʼ��ַ */
#define REG_HOLD_NUM         20                 /**< \brief ���ּĴ������� */

#define REG_INPUT_ADDR_START 1000               /**< \brief ����Ĵ�����ʼ��ַ */
#define REG_INPUT_NUM        20                 /**< \brief ����Ĵ������� */

#define REG_COIL_ADDR_START  1000               /**< \brief ��Ȧ��ʼ��ַ */
#define REG_COIL_NUM         10                 /**< \brief ��Ȧ�Ĵ������� */

aw_local uint16_t g_holding_reg[REG_HOLD_NUM];    /**< \brief ���ּĴ��� */
aw_local uint8_t  g_coil_reg[REG_COIL_NUM];       /**< \brief ��Ȧ���� 8*REG_COIL_NUM */
aw_local uint16_t g_input_reg[REG_INPUT_NUM] = {  /**< \brief ����Ĵ��� */
    0,   1,  2,  3,  4,  5,  6,  7,  8,  9,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
};

/**
 * \brief �ڴ���ȡ16λ�޷�����(���ģʽ)
 */
aw_local aw_inline uint16_t to_u16 (const uint8_t *buf)
{
    return ((uint16_t)buf[0] << 8) + buf[1];
}

/**
 * \brief ����Ȧ
 */
aw_local aw_mb_exception_t my_fun_rd_coil (aw_mb_slave_t  slave,
                                           uint8_t       *p_buf,
                                           uint16_t       addr,
                                           uint16_t       num)
{
    if ((addr < REG_COIL_ADDR_START) || num > (REG_COIL_NUM << 3)) {
        return AW_MB_EXP_ILLEGAL_DATA_ADDRESS;
    }

    /* ����Ȧֵ��buf�� */
    aw_mb_bits_cp_to_buf(p_buf, g_coil_reg, addr - REG_COIL_ADDR_START, num);

    return AW_MB_EXP_NONE;
}

/**
 * \brief д��Ȧ
 */
aw_local aw_mb_exception_t my_fun_wr_coil (aw_mb_slave_t  slave,
                                           uint8_t       *p_buf,
                                           uint16_t       addr,
                                           uint16_t       num)
{

    if ((addr < REG_COIL_ADDR_START) || num > (REG_COIL_NUM << 3)) {
        return AW_MB_EXP_ILLEGAL_DATA_ADDRESS;
    }

    /* ��buf������ֵд��ָ����Ȧ */
    aw_mb_bits_set_from_buf(g_coil_reg, p_buf, addr-REG_COIL_ADDR_START, num);

    return AW_MB_EXP_NONE;
}

/**
 * \brief �����ּĴ���
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

    /* ��ȡ���ּĴ������ݵ�buf�� */
    aw_mb_regcpy(p_buf, &g_holding_reg[index], num);

    return AW_MB_EXP_NONE;
}

/**
 * \brief д���ּĴ���
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

    /* ��buf����д�����ּĴ������� */
    aw_mb_regcpy(&g_holding_reg[index], p_buf, num);

    return AW_MB_EXP_NONE;
}

/**
 * \brief ������Ĵ���
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

    /* ��ȡ����Ĵ������ݵ�buf�� */
    aw_mb_regcpy(p_buf, &g_input_reg[index], num);

    return AW_MB_EXP_NONE;
}

/**
 * \brief ��չд�Ĵ�������������
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
 * \brief Modbus ��վ�������
 * \param[in] p_arg : �������
 * \return ��
 */
void demo_modbus_slave_entry (void)
{
    uint8_t       again;
    aw_mb_err_t   err;
    aw_mb_slave_t slave;

    /* ��ʼ����������Ϊ���ӻ���ַΪ1�����ڲ�����115200����У�� */
    struct aw_mb_serial_param param = {1, AW_MB_PAR_EVEN, DE_MODBUS_UART_ID, 115200 };

    /* Modbus�ӻ�RTUģʽ��ʼ�� */
    slave = aw_mb_slave_init(AW_MB_RTU, &param, &err);
    if (err != AW_MB_ERR_NOERR) {
        return ;
    }

    /* ����Ȧ�ص� */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_COILS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_READ,
                                  my_fun_rd_coil);

    /** ����ɢ���ص� */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_DISCRETEINPUTS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_READ,
                                  my_fun_rd_coil);

    /* д��Ȧ�ص� */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_COILS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_WRITE,
                                  my_fun_wr_coil);

    /* �����ּĴ����ص� */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_HOLDREGISTERS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_READ,
                                  my_fun_rd_holdreg);

    /* д���ּĴ����ص� */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_HOLDREGISTERS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_WRITE,
                                  my_fun_wr_holdreg);

    /* ������Ĵ����ص� */
    aw_mb_slave_register_callback(slave,
                                  AW_MB_FUNC_INPUTREGISTERS_CALLBACK,
                                  AW_MB_FUNC_CALLBACK_READ,
                                  my_fun_rd_inputreg);

    /* ע����չ�����봦���� */
    aw_mb_slave_register_handler(slave,
                                 MB_MASK_WRITE_REG_FUNCODE,
                                 my_mask_write);

    /* ���ôӻ���ַ */
    aw_mb_slave_set_addr(slave, 0x01);

    /* ��ʼ���� */
    err = aw_mb_slave_start(slave);
    if (err != AW_MB_ERR_NOERR) {
        return ;
    }

    AW_INFOF(("\nModbus Slave start.\n"));

    again = 0;
    AW_FOREVER {
        err = aw_mb_slave_poll(slave);    /* �ӻ���ѯ���� */
        if (err != AW_MB_ERR_NOERR ) {
            again++;
        }

        if (again > 20) {
            break;
        }
        aw_mdelay(5);
    }

    AW_INFOF(("\nModbus Slave exit.\n"));
    aw_mb_slave_stop(slave);    /* ֹͣ�ӻ� */
    aw_mb_slave_close(slave);   /* �رմӻ����˳� */

    for (;;) {
        aw_mdelay(1000);
    }
}

/** [src_modbus_slave] */

/* end of file */
