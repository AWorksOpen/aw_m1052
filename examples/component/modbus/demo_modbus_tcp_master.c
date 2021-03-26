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
 * \brief AWorksƽ̨---Modbus��վ��ʾ���̣�ʹ��TCP���ӣ�
 *
 * - �������裺
 *   1. ʹ��TCPģʽʱ��PC�Ϳ�����Ӧ�ô���ͬһ�����Σ�����Modbus Slave��λ��Ӧ��ΪTCPģʽ��
 *      ��Ҫ����̫�������ļ����޸�ĳЩ���ã��޸������£�
 *      1) ���뿪�������ӵ�PCû��DHCP��������__get_dhcp_en() �����ķ���ֵ��ΪAW_FALSE��ʾ����DHCP��
 *         ����ȷ����IP��ַ���Ϳ�����Ӧ�ô���ͬһ���Σ���
 *      2) ���뿪�������ӵ�PC��DHCP��������__get_dhcp_en() �����ķ���ֵ��ΪAW_TRUE��ʾʹ��DHCP��
 *         ��������������õ�Ĭ��IP��ַ��
 *   2. ����aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - ���Դ���
 *      - ����
 *   3. ��Modubs��վ��λ���������Modbus Slave��,TCP���ӷ�ʽ�����ô�վ������ʵļĴ���/��Ȧ��
 *      ַ��Χ��������������վ���ɡ�
 *   4. �������е�TXD��RXDͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. Modbus��վ��λ���Ĵ�������Ȧֵ����վ����仯��
 *   2. ����̨��ӡ������Ϣ��
 *
 * - ��ע��
 *   1. ��ʹ��Modbus����ģʽ����ʱ����ʹ�ô��ڽ���������ʹ��ƽ̨�ϴ��ڽӿڷ���

 *   3. ��aw_prj_params.h�ļ��н�Ҫʹ�õ������򿪡�

 *
 * \par Դ����
 * \snippet demo_modbus_tcp_master.c src_modbus_tcp_master
 *
 * \internal
 * \par Modification history
 * - 1.00 16-04-05  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_modbus_tcp_master Modbus��վ(TCP����)
 * \copydoc demo_modbus_tcp_master.c
 */

/** [src_modbus_tcp_master] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_serial.h"
#include "modbus/aw_mb_master.h"  /* modbus��վ���� */

/******************************************************************************/
/** \brief �û��Զ����д�������� */
struct wr_mask_param {
    uint16_t addr;     /**< \brief �Ĵ�����ַ */
    uint16_t and_mask; /**< \brief �����ֵ */
    uint16_t or_mask;  /**< \brief �����ֵ */
} my_mask_param;

/** \brief ����PDU���ݴ���ӿ� */
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
#define MB_MASK_WRITE_REG_FUNCODE  22  /**< \brief д�Ĵ������빦���� */
#define SLAVE_ADDR                 1   /**< \brief Ŀ���վ��ַ */

void demo_modbus_tcp_master_entry (void)
{
    uint8_t                 *p_buf = NULL;
    uint16_t                 rd_buf[20];
    uint16_t                 wr_buf[20];
    aw_mb_master_t           master;
    aw_mb_err_t              err     = AW_MB_ERR_NOERR;
    int                      i       = 0;
    aw_mb_master_rw_param_t  rw_param;
    uint8_t                  re_con_times = 3;  /* ����������� */

    /* TCPģʽ���Ͳ��� */
    /* Ŀ��IP��ַ��192.168.12.240  �˿ڣ� 502 */
    /* ע��IP��ַ��ʵ������������˿ڱ�����502 */
    const aw_mb_tcp_client_params_t tcp_params = {"192.168.12.240", 502};

    master = aw_mb_master_create();
    if (master == NULL) {
        AW_ERRF(("Modbus Master Create Failed\r\n"));
        return;
    }

    /* ��վ����ΪTCPģʽ������Ŀ��IP��ַ���˿� */
    err = aw_mb_master_config(master, AW_MB_TCP, &tcp_params);

    if (err != AW_MB_ERR_NOERR) {
        AW_ERRF(("Modbus Master Config Failed, err: %d!\r\n", err));
        aw_mb_master_delete(master);  /* ����ʧ�ܣ�ɾ����վ */
        return ;
    }

    /* ע��д���ּĴ������빦���봦���� */
    aw_mb_master_funcode_register(master,
                                  MB_MASK_WRITE_REG_FUNCODE,
                                  __mb_snd_mask_handler,
                                  NULL);

    while (re_con_times--) {
        /* ������վ����ΪTCPģʽ���ڴ˽������� */
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

        /* ����Ȧ---�ӵ�ַ1000��ʼ����ȡ20����Ȧֵ������rd_buf�������� */
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

        /* ����ɢ��---�ӵ�ַ1000��ʼ����ȡ20����ɢ��������rd_buf�������� */
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

        /* �����ּĴ���---�ӵ�ַ1000��ʼ����ȡ20�����ּĴ�������ȡ���ݷ���rd_buf������ */
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

        /* ������Ĵ���---�ӵ�ַ1000��ʼ����ȡ20�����ּĴ�������ȡ���ݷ���rd_buf������ */
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

        /* д������Ȧ---�Ե�ַΪ1000����ȦдON */
        err = aw_mb_single_coil_write(master, SLAVE_ADDR, 1000, 0xFF00);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("\nsingle coil write success!\n"));
        }

        /* д�������ּĴ���---�Ե�ַΪ1000�ļĴ���д��0x1234 */
        err = aw_mb_single_reg_write(master, SLAVE_ADDR, 1000, 0x1234);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("\nsingle reg write success!\n"));
        }

        /* д�����Ȧ---����Ȧ��ַ1000��ʼ��16����ȦдON */
        wr_buf[0] = 0xFFFF;
        err = aw_mb_multiple_coils_write(master, SLAVE_ADDR,
                                         1000, 16, (uint8_t *)&wr_buf[0]);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("\nmultiple coils write success!\n"));
        }


        /* д����Ĵ���---�ӵ�ַΪ1000��ʼ��д��20���Ĵ�����20���Ĵ���ֵ��Ϊ0xABCD */
        for (i = 0; i < 20; i++) {
            wr_buf[i] = 0xABCD;
        }
        err = aw_mb_multiple_regs_write(master, SLAVE_ADDR, 1000, 20, wr_buf);
        if (err != AW_MB_ERR_NOERR) {
            AW_INFOF(("Modbus Master Request Failed, err: %d.\r\n", err));
        } else {
            AW_INFOF(("\nmultiple regs write success!\n"));
        }

        /* ��д����Ĵ���
         * �ӵ�ַ1000��ʼ����ȡ20���Ĵ��������뻺����rd_buf�У�ͬʱ
         * ��������wr_buf�е�20��ֵд��ӵ�ַ1020��ʼ��20����Ȧ��
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

        /* д���빦��---�û��Զ��� ---�㲥���д�վ
         * ����ַ1000�ļĴ�������0x01,��0x0,��ÿ�β������ַ1000�ļĴ���ֵΪ1
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
