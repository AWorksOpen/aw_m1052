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
 * \brief AWorks MD5 �㷨ʹ������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ���ʹ�ܺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ��������������õ�MD5ֵ��
 *   2. �����ַ���"abcdefghijklmnopqrstuvwxyz" ��md5ֵ��������󣬸�ֵӦ��Ϊ��
 *      c3fcd3d76192e4007dfb496cca67e13b��һ��128bit������
 *
 * \par Դ����
 * \snippet demo_md5.c src_md5
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-05  mkr, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_md5 MD5�㷨
 * \copydoc demo_md5.c
 */

/** [src_md5] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_md5.h"
#include <string.h>

/**
 * \brief md5 �㷨��ں���
 * \return ��
 */
void demo_md5_entry (void)
{
    char str[] = "abcdefghijklmnopqrstuvwxyz";
    uint8_t std_result[] = {
            0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00,
            0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b,
    };
    uint8_t cal_result[16] = {0};
    uint8_t i;

    aw_md5_t md5;

    aw_kprintf("\nMD5 testing...\r\n");

    /* ��ʼ��md5�㷨 */
    aw_md5_init(&md5);

    /* �����������ֵ */
    aw_md5_update(&md5, (unsigned char *)str, strlen(str));

    /* ������ */
    aw_md5_final(cal_result, &md5);

    if (0 == memcmp(std_result, cal_result, 16)) {
        aw_kprintf(
                "\nThe calculate is right!\nThe md5 value of \"%s\" is ��\r\n",
                str);
        for (i = 0; i < 16; i++) {
            aw_kprintf("%0.2x", cal_result[i]);
        }
    } else {
        aw_kprintf("\nThe calculate is error!\r\n");
    }
}

/** [src_md5] */

/* end of file */
