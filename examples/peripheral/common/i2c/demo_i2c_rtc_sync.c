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
 * \brief ʹ��I2C1(ͬ�����䷽ʽ)����
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
 * \snippet demo_i2c_rtc_sync.c src_i2c_rtc_sync
 *
 * \internal
 * \par History
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_i2c_rtc_sync I2C(ͬ������)
 * \copydoc demo_i2c_rtc_sync.c
 */

/** [src_i2c_rtc_sync] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_i2c.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_demo_config.h"

#define I2C_SLAVE_ADDR        0x7EU           /* �ӻ���ַ(�û��ɲ鿴����I2C�ӻ��������ļ����ɸ��������ļ��޸Ĵӻ���ַ) */
#define I2C_MASTER_BUSID      DE_I2C_ID      /* �ӻ�ʹ��I2C1(�û��ɲ鿴����I2C�ӻ��������ļ������������ļ��޸�I2C����) */

#define SLAVE_REG_SEC         0x02           /* �������ӵ�ַ���Ӵ˵�ַ��ʼд������(�û��ɸ��ݾ����I2C�ӻ��豸�޸�) */

/**
 * \brief I2C ͬ����д demo
 * \return ��
 */
void demo_i2c_rtc_sync_entry (void)
{
    aw_i2c_device_t     i2c_slave;
    uint8_t             wr_data[10];
    uint8_t             rd_data[10];
    int                 i = 0;
    int                 ret;

    aw_kprintf("I2C_SYNC_Test:\r\n");

    aw_i2c_mkdev(&i2c_slave,
                 I2C_MASTER_BUSID,
                 I2C_SLAVE_ADDR,
                 AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);

    /* ��������� */
    memset(wr_data, 1, 10);
    memset(rd_data, 1, 10);

    for(i = 0; i < 10; i++) {
        wr_data[i] = i;
    }

    /* д���ݵ��ӻ��豸 */
    ret = aw_i2c_write(&i2c_slave, SLAVE_REG_SEC, &wr_data[0], 10);
    if (ret != AW_OK) {
        aw_kprintf("aw_i2c_write fail:%d\r\n", ret);
        return ;
    } else {
        aw_kprintf("aw_i2c_write OK!\r\n");
    }

    /* �Ӵӻ��豸�ж�ȡ���� */
    ret = aw_i2c_read(&i2c_slave, SLAVE_REG_SEC, &rd_data[0], 10);
    if (ret != AW_OK) {
        aw_kprintf("aw_i2c_read fail:%d\r\n", ret);
        return ;
    } else {
        aw_kprintf("aw_i2c_read OK!\r\n");
    }

    /* ����У�� */
    ret = strcmp((const char * )wr_data, (const char *)rd_data);
    if (ret != AW_OK) {
        aw_kprintf("verify fail:%d\r\n", ret);
    } else {
        aw_kprintf("verify OK!\r\n");
    }

    return ;
}

/** [src_i2c_rtc_sync] */

/* end of file */
