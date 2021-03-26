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
 * \brief ʹ��I2C1(�첽���䷽ʽ)����
 *
 * - ��������:
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - ��Ӧƽ̨��I2C�꣨�磺AW_DEV_xxx_LPI2Cx��
 *      - ��Ӧƽ̨�Ĵ��ں�
 *   2. ���Ӵ��ڣ��򿪴��ڵ�����������ò�����115200-8-N-1��
 *
 * - ʵ������:
 *   1.���ڴ�ӡ��д������Ϣ��
 *
 * \par Դ����
 * \snippet demo_i2c_rtc_async.c src_i2c_rtc_async
 *
 * \internal
 * \par History
 * - 1.00 17-09-01  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_i2c_rtc_async I2C(�첽����)
 * \copydoc demo_i2c_rtc_async.c
 */

/** [src_i2c_rtc_async] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_i2c.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_sem.h"
#include "aw_demo_config.h"

#define I2C_SLAVE_ADDR        0X51           /* �ӻ���ַ(�û��ɲ鿴����I2C�ӻ��������ļ����ɸ��������ļ��޸Ĵӻ���ַ) */
#define I2C_MASTER_BUSID      DE_I2C_ID      /* �ӻ�ʹ��I2C1(�û��ɲ鿴����I2C�ӻ��������ļ������������ļ��޸�I2C����) */

#define SLAVE_REG_SEC         0x02           /* �������ӵ�ַ���Ӵ˵�ַ��ʼд������(�û��ɸ��ݾ����I2C�ӻ��豸�޸�) */

AW_SEMB_DECL_STATIC(sem_async);


static void __rtc_callback (void *p_arg)
{
    AW_SEMB_GIVE(sem_async);          /* �ͷ��ź��� */
}

/**
 * \brief I2C�첽д����
 */
static aw_bool_t __aw_i2c_async_write (aw_i2c_device_t *i2c_slave,
                                       uint8_t         *data,
                                       size_t           size)
{
    aw_err_t   ret = AW_OK;

    aw_i2c_message_t  msg;
    aw_i2c_transfer_t trans[2];
    uint8_t           subaddr = SLAVE_REG_SEC;

    aw_i2c_mktrans(&trans[0],
                   I2C_SLAVE_ADDR,                  /* Ŀ��������ַ */
                   AW_I2C_M_7BIT | AW_I2C_M_WR,     /* 7-bit������ַ, д���� */
                   &subaddr,                        /* �ӵ�ַ */
                   1);                              /* �ӵ�ַ���ȣ�1byte */

    aw_i2c_mktrans(&trans[1],
                   I2C_SLAVE_ADDR,                  /* Ŀ��������ַ (��������) */
                   AW_I2C_M_WR | AW_I2C_M_NOSTART,  /* д����, ��������ʼ�źż��ӻ���ַ */
                   &data[0],                        /* ���ݻ����� */
                   size);                           /* ���ݸ��� */

    aw_i2c_mkmsg(&msg,
                 &trans[0],
                 2,
                 __rtc_callback,
                 (void *)0);

    /* �������Ϣ���첽 */
    ret = aw_i2c_async(I2C_MASTER_BUSID, &msg);
    if (ret != AW_OK) {
        return ret;
    }

    /* �ȴ���Ϣ������� */
    ret = aw_semb_take(&sem_async, AW_SEM_WAIT_FOREVER);
    if (ret != AW_OK) {
        return ret;
    }

    return AW_TRUE;
}

/**
 * \brief I2C�첽������
 */
static aw_bool_t __aw_i2c_async_read (aw_i2c_device_t  *i2c_slave,
                                      uint8_t          *data,
                                      size_t            size)
{
    aw_err_t    ret = AW_OK;

    aw_i2c_message_t  msg;
    aw_i2c_transfer_t trans[2];
    uint8_t           subaddr = SLAVE_REG_SEC;

    aw_i2c_mktrans(&trans[0],
                   I2C_SLAVE_ADDR,                  /* Ŀ��������ַ */
                   AW_I2C_M_7BIT | AW_I2C_M_WR,     /* 7-bit������ַ, д���� */
                   &subaddr,                        /* �ӵ�ַ */
                   1);                              /* �ӵ�ַ���ȣ�1byte */

    aw_i2c_mktrans(&trans[1],
                   I2C_SLAVE_ADDR,                  /* Ŀ��������ַ (��������) */
                   AW_I2C_M_RD | AW_I2C_M_NOSTART,  /* ������, ��������ʼ�źż��ӻ���ַ */
                   &data[0],                        /* ���ݻ����� */
                   size);                           /* ���ݸ��� */

    aw_i2c_mkmsg(&msg,
                 &trans[0],
                 2,
                 __rtc_callback,
                 (void *)0);

    /* �������Ϣ���첽 */
    ret = aw_i2c_async(I2C_MASTER_BUSID, &msg);
    if (ret != AW_OK) {
        return AW_FALSE;
    }

    /* �ȴ���Ϣ������� */
    ret = aw_semb_take(&sem_async, AW_SEM_WAIT_FOREVER);
    if (ret != AW_OK) {
        return AW_FALSE;
    }

    return AW_TRUE;
}

/**
 * \brief I2C �첽��д demo
 * \return ��
 */
void demo_i2c_rtc_async_entry (void)
{
    aw_i2c_device_t     i2c_slave;
    uint8_t             wr_data[10];
    uint8_t             rd_data[10];
    int                 i = 0;
    int                 ret;

    AW_SEMB_INIT(sem_async, 0, AW_SEM_Q_PRIORITY);

    aw_kprintf("I2C_ASYNC_Test:\r\n");

    aw_i2c_mkdev(&i2c_slave,
                 I2C_MASTER_BUSID,
                 I2C_SLAVE_ADDR,
                 AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);

    /* ��������� */
    memset(wr_data, 1, 10);
    memset(rd_data, 1, 10);

    /* ������� */
    for (i = 0; i < 10; i++) {
        wr_data[i] = i;
    }

    /* �첽д���� */
    ret = __aw_i2c_async_write(&i2c_slave, &wr_data[0], 10);
    if (ret == AW_FALSE) {
        aw_kprintf("aw_i2c_write fail:%d\r\n", ret);
        return ;
    } else {
        aw_kprintf("aw_i2c_write OK!\r\n");
    }

    /* �첽������ */
    ret = __aw_i2c_async_read(&i2c_slave, &rd_data[0], 10);
    if (ret == AW_FALSE) {
        aw_kprintf("aw_i2c_read fail:%d\r\n", ret);
        return ;
    } else {
        aw_kprintf("aw_i2c_read OK!\r\n");
    }

    /* ����У�� */
    ret = strcmp((const char * )wr_data, (const char *)rd_data);
    if (ret != 0) {
        aw_kprintf("verify fail:%d\r\n", ret);
    } else {
        aw_kprintf("verify OK!\r\n");
    }

    return ;
}

/** [src_i2c_rtc_async] */

/* end of file */
