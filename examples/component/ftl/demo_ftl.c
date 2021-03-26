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
 * \brief Aworks Flashת��������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - NANDFLASH�����
 *      - AW_COM_FTL
 *      - AW_COM_CONSOLE
 *   2. ����ʵ��������޸�MTD�豸����������__MTD_NAME�����֡�
 *   3. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������:
 *   1. ����FTL�豸��д�����ڴ�ӡ������Ϣ��
 *
 * \par Դ����
 * \snippet demo_ftl.c src_ftl
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-05  sdq, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_ftl FTL��д
 * \copydoc demo_ftl.c
 */

/** [src_ftl] */

#include "aworks.h"
#include "aw_ftl.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_delay.h"
#include "aw_mem.h"

/* mtd��������nandflash��Ӳ������(awbl_hwconf_xxx_nandflash.h)����� */
#define __MTD_NAME  "/dev/mtd0"

/* ��Ҫ���Ե��߼���������� */
#define __TEST_SECTOR_NUM   150

/**
 * \brief �������
 * \param[in] p_arg : �������
 * \return ��
 */
void demo_ftl_entry (void)
{
    struct aw_ftl_dev *cur;
    uint32_t           arg = 0;
    char               name[100];
    uint8_t           *buf = NULL;
    aw_err_t           ret;
    uint32_t           sector_size;     /* �豸������С */

    /* ��һ��mtd�豸������ftl�豸������ģʽΪ�Զ���ʽ�� */
    ret = aw_ftl_dev_create ("/dev/ftl", __MTD_NAME);
    if (ret != AW_OK) {
        AW_INFOF(("ftl dev create failed\r\n"));
        return;
    }

    /* ��ȡ�ոմ�����ftl�豸 */
    cur = aw_ftl_dev_get("/dev/ftl");

    /* ����ϵͳ��Ϣ�ĸ���ʱ��Ϊ60s */
    aw_ftl_dev_ioctl(cur, AW_FTL_SYS_INFO_UPDATE_TIME, (void *)60000);

    /* ��ȡ�豸������ */
    if (!aw_ftl_dev_ioctl(cur, AW_FTL_GET_DEV_NAME, (void *)name)) {
        AW_INFOF(("Device name : %s\r\n", name));
    }

    /* ��ȡ�豸��������С */
    if (!aw_ftl_dev_ioctl(cur, AW_FTL_GET_SECTOR_SIZE, (void *)&sector_size)) {
        AW_INFOF(("Sector size : %d\r\n", sector_size));
    }

    /* ��ȡ�豸���������� */
    if (!aw_ftl_dev_ioctl(cur, AW_FTL_GET_SECTOR_COUNT, (void *)&arg)) {
        AW_INFOF(("Sector counts : %d\r\n", arg));
    }

    /* �����豸���߼��������ֵΪ100%ʱ�������� */
    aw_ftl_dev_ioctl(cur, AW_FTL_LOGIC_THRESHOLD, (void *)100);

    /* �����豸����־�������ֵΪ87%ʱ�������� */
    aw_ftl_dev_ioctl(cur, AW_FTL_LOG_THRESHOLD, (void *)87);

    buf = aw_mem_alloc(sector_size);
    if (buf == NULL) {
        AW_INFOF(("mem alloc failed\r\n"));
        return;
    }

    memset((char *)buf, 'h', sector_size);

    /* дbuf�е����ݵ�����
     * ע�⣬��д����ʱbuf�ռ��СҪ���ڻ����ftl�豸��������С��
     * ftl�豸��������С����ͨ��ioctl���
      */
    if (!aw_ftl_sector_write(cur, __TEST_SECTOR_NUM, buf)) {
        AW_INFOF(("Sector write success\r\n"));
    }

    memset((char *)buf, 0, sector_size);

    /* �����������ݵ�buf�� */
    for(int i = 0 ; i < 20 ;i++) {
        ret = aw_ftl_sector_read(cur, __TEST_SECTOR_NUM, buf);
        if (ret == AW_OK) {
            AW_INFOF(("Sector read success\r\n"));
        }
    }

    /* ��ӡbuf�е��ַ��� */
    AW_INFOF(("Sector %d data: %s\r\n", __TEST_SECTOR_NUM, buf));

    /* ��ʹ����ftl���ͷ�ftl�豸 */
    if (!aw_ftl_dev_put(cur)) {
        AW_INFOF(("Put ftl device\r\n"));
    }

    /* ����ftl�豸 */
    if (!aw_ftl_dev_destroy("/dev/ftl")) {
        AW_INFOF(("Destory ftl device\r\n"));
    }
}

/** [src_ftl] */
 
/* end of file */
