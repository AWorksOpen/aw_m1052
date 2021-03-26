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
 * \brief ������ʾ���̣��ж�ģʽ��
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨�Ĵ��ں꣨�磺AW_DEV_xxx_LPUARTx��
 *      - AW_COM_CONSOLE
 *   2. ʹ�ñ�����ʱ��������shell �� (AW_COM_SHELL_SERIAL)�������̵�
 *      TEST_SERIAL_NUM ��ʹ�ò���Ϊ COM0 (��Ϊshellʹ��COM0�����ȡ��������)
 *   3. �������е�RX��TXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ���ڽ����ж϶�дģʽ��������������λ���������ݣ����ڽ����յ������ݷ���
 *      ����λ����
 *
 * - ��ע��
 *   1. COMx�봮�ڵĶ�Ӧ��ϵ��aw_prj_params.h�ļ��в鿴��
 *
 * \par Դ����
 * \snippet demo_serial_int.c src_serial_int
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-24  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_serial_int ����(�ж�ģʽ)
 * \copydoc demo_serial_int.c
 */

/** [src_serial_int] */
#include "aworks.h"

#include "aw_delay.h"
#include "aw_serial.h"
#include "aw_ioctl.h"

#define  TEST_SERIAL_NUM   COM1

/**
 * \brief ���� demo
 * \return ��
 */
aw_local void* __task_handle (void *parg)
{
    char                      buf[32];
    int                       len = 0;
    struct  aw_serial_dcb     dcb;
    aw_err_t                  ret;
    struct aw_serial_timeout  timeout;

    /* ��ȡdcb��Ĭ������ */
    ret = aw_serial_dcb_get(TEST_SERIAL_NUM, &dcb);
    if (AW_OK != ret) {
        return 0;
    }

    /* ���ô��ڲ�����Ϊ115200��8������λ��1λֹͣλ������żУ�� */
    dcb.baud_rate = 115200;
    dcb.byte_size = 8;
    dcb.stop_bits = AW_SERIAL_ONESTOPBIT;
    dcb.f_parity = AW_FALSE;
    ret = aw_serial_dcb_set(TEST_SERIAL_NUM, &dcb);
    if (AW_OK != ret) {
        return 0;
    }

    /* ���ô��ڳ�ʱ */
    ret = aw_serial_timeout_get(TEST_SERIAL_NUM, &timeout);
    if (AW_OK != ret) {
        return 0;
    }

    timeout.rd_timeout = 1000;          /* ���ܳ�ʱΪ1s */
    timeout.rd_interval_timeout = 50;   /* ��䳬ʱΪ50ms */
    ret = aw_serial_timeout_set(TEST_SERIAL_NUM, &timeout);
    if (AW_OK != ret) {
        return 0;
    }


    /* �յ�ʲô���ݾͷ���ʲô���� */
    AW_FOREVER {

        /* ��ȡ���� */
        len = aw_serial_read(TEST_SERIAL_NUM, buf, sizeof(buf));
        if (len > 0) {
            aw_serial_write(TEST_SERIAL_NUM, buf, len);
        }
    }

    return 0;
}

/******************************************************************************/
void demo_serial_int_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Serial int demo",
                         12,
                         1024,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Serial int demo task create failed\r\n");
        return;
    }

    if (aw_task_startup(tsk) != AW_OK) {
        aw_task_delete(tsk);
    }
}

/** [src_serial_int] */

/* end of file */
