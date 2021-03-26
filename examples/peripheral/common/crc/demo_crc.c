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
 * \brief CRCУ�����̣����CRC��
 *
 * - ��������:
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ�ܣ�
 *      - ��Ӧƽ̨�Ĵ��ں�
 *      - AW_COM_CONSOLE
 *   2. ʹ�ô����߷ֱ�Ϳ�����ĵ��Դ���DURX��DUTX�������򿪴��ڵ������֣�
 *      ���ò�����Ϊ115200��1��ֹͣλ����У�飬�����ء�
 *
 * - ʵ������
 *   1. У������Ϊ�ַ�'1' ~ '9'���ֱ�ʹ��ƽ̨�ӿڡ����CRC��
 *      ����CRC-16��CCITT��׼������У�����ݣ� �Ա�У��������ӡ�ԱȽ����
 *
 * \par Դ����
 * \snippet demo_crc.c src_crc
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-24  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_crc CRCУ��
 * \copydoc demo_crc.c
 */

/** [src_crc] */
#include "aworks.h"
#include "aw_crc.h"
#include "aw_vdebug.h"

/******************************************************************************/

/**
 * \brief ���CRC����
 * \param[in] p_frame  ָ�����ݻ�������ָ��
 * \param[in] len      ���ݸ���
 * \param[in] crc      CRC��ֵ
 * \return crc16 ֵ
 */
aw_local uint16_t __software_crc16 (uint8_t *p_frame, uint16_t len, uint16_t crc)
{
    int i;

    for (; len > 0; len--) {
        crc = crc ^ (*p_frame++ << 8);
        for (i = 0; i < 8; i++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }

        crc &= 0xFFFF;
    }

    return crc;
}

/******************************************************************************/

/**
 * \brief crcУ������
 * \return ��
 */
void demo_crc_entry (void)
{
    aw_err_t  ret;
    uint8_t   ndata[9]   = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    uint16_t  crc16_sft  = 0; /* ��������CRCֵ */
    uint32_t  crc16_plat = 0; /* ƽ̨�ӿڼ����CRCֵ */

    aw_kprintf("\ncrc demo testing...\n");

    /* ������һ��ģ�ͣ���ģ�;��ǳ��õ�CRC-16ģ�� */
    AW_CRC_DECL(crc16,          /* crcģ�� crc16 */
                16,             /* crc��� 16 */
                0x1021,         /* ���ɶ���ʽ */
                0xFFFF,         /* ��ʼֵ�� CCITT��׼Ҫ���ֵ����Ϊ0 */
                AW_FALSE,       /* �����ֽ�bit������ */
                AW_FALSE,       /* ���CRC��bit������ */
                0x0000);        /* ������ֵ */


    ret =  AW_CRC_INIT(crc16,             /* crcģ�ͣ���AW_CRC_DECL()����� */
                       crctable16_1021,   /* crc��ϵͳ�ģ�����Ҫ���� */
                       AW_CRC_FLAG_AUTO); /* �Զ�ģʽ���Զ�ѡ��Ӳ�������ʵ�� */
    if (ret != AW_OK) {
        aw_kprintf("crc error\n");
    } else {
        AW_CRC_CAL(crc16,     /* crcģ�ͣ���AW_CRC_DECL()����� */
                   &ndata[0], /* ������CRC��������� */
                   5);        /* �˴μ�������� */

        AW_CRC_CAL(crc16,     /* crcģ�ͣ���AW_CRC_DECL()����� */
                   &ndata[5], /* ������CRC��������� */
                   4);        /* �˴μ�������� */

        /* �м���Լ�������AW_CRC_CAL()����
         * ...
         */

        /* ��������ȡ�����ս�� */
        crc16_plat = AW_CRC_FINAL(crc16);  /* crcģ�ͣ���AW_CRC_DECL()����� */

        aw_kprintf("platform crc16's value is = 0x%x\n", crc16_plat);

        /* �������������㣬����Ҫ���µ���AW_CRC_INIT()��ʼ�����ټ��㡣 */
    }

    /* ʹ�����CRC����õ�����ֵ */
    crc16_sft = __software_crc16(ndata, 9, 0xFFFF);

    aw_kprintf("software crc16's value is = 0x%x\n", crc16_sft);

    if (crc16_plat == crc16_sft) {
        aw_kprintf("\n---Equal---\n");
    } else {
        aw_kprintf("\n---No Equal---\n");
    }
}

/** [src_crc] */

/* end of file */
