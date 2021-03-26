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
 * \brief ���λ�������ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ���ʹ�ܺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������:
 *      ���ڴ�ӡ ringbuf ���������Ϣ��
 *
 * \par Դ����
 * \snippet demo_rngbuf.c src_rngbuf
 *
 * \internal
 * \par History
 * - 1.00 17-09-06  sup, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_rngbuf ���λ�����
 * \copydoc demo_rngbuf.c
 */

/** [src_rngbuf] */
#include "aworks.h"             /* ��ͷ�ļ����뱻���Ȱ��� */
#include "aw_vdebug.h"
#include "aw_rngbuf.h"


/**
 * \brief ���λ����������ʾ�������
 * \return ��
 */
void demo_rngbuf_entry (void)
{
    uint8_t     i = 0;
    char        dat;
    char        buf[] = "hello aworks";

    /* ���岢��ʼ�����λ����� */
    AW_RNGBUF_DEFINE(rb, char, 16);

    /* ��ȡ���λ������Ĵ�С */
    AW_INFOF(("\nRingbuf size is:%d\n", AW_RNGBUF_SPACE_SIZE(rb)));

    /* ÿ��������������һ���ֽ�ֱ������ */
    AW_FOREVER {
        if (!aw_rngbuf_isfull(&rb)) { /* ������δ�� */

            /* ���λ���������һ���ֽ����� */
            aw_rngbuf_putchar(&rb, 'a' + i);
            AW_INFOF(("Ringbuf put a char:%c\n", 'a' + i));
            i++;
        } else { /* ����������  */
            AW_INFOF(("Ringbuf is full\n"));
            break;
        }
    }

    AW_INFOF(("---------------------------------------------------\n"));

    /* ÿ�δӻ�����ȡ��һ���ֽ�ֱ��������Ϊ�� */
    AW_FOREVER {
        if (!aw_rngbuf_isempty(&rb)) {

            /* �ӻ��λ�������ȡһ���ֽ����� */
            aw_rngbuf_getchar(&rb, &dat);
            AW_INFOF(("Ringbuf get a char:%c\n", dat));
        } else { /* ������Ϊ�� */
            AW_INFOF(("Ringbuf is empty\n"));
            break;
        }
    }

    /* һ�δ�Ŷ���ֽڵ���������Ȼ���ѯ�������洢״̬ */
    AW_INFOF(("\nRingbuf put:%s\n", buf));
    aw_rngbuf_put(&rb, buf, sizeof(buf));
    AW_INFOF(("Ringbuf saved bytes:%d\n", aw_rngbuf_nbytes(&rb)));
    AW_INFOF(("Ringbuf freebytes:%d\n", aw_rngbuf_freebytes(&rb)));

    /* ��ջ��λ�����, Ȼ���ѯ�������Ƿ�Ϊ�� */
    AW_INFOF(("\nRingbuf flush\n"));
    aw_rngbuf_flush(&rb);
    AW_INFOF(("Ringbuf %s empty\n",
                aw_rngbuf_isempty(&rb) ? "is" : "is not"));

    aw_rngbuf_putchar(&rb, 'a');        /* д������'a'��ָ���ƶ�һ���ֽ� */
    aw_rngbuf_put_ahead(&rb, 'b', 0);   /* д������'b'�������ƶ�д��ָ�� */
    aw_rngbuf_put_ahead(&rb, 'c', 1);   /* д������'c'�������ƶ�д��ָ�� */
    aw_rngbuf_move_ahead(&rb, 2);       /* ��ǰ�ƶ�����ָ��2���ֽ� */

    aw_rngbuf_get(&rb, buf, 3);         /* ��ʱ���԰�"abc"�������� */
    AW_INFOF(("\nRingbuf get data:"));
    for (i = 0; i < 3; i++) {
        AW_INFOF(("%c ", buf[i]));
    }
    AW_INFOF(("\n"));
}

/** [src_rngbuf] */

/* end of file */
