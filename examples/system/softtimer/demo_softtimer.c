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
 * \brief �����ʱ������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_GPIO_LED
 *      - AW_COM_CONSOLE
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ���������ط���LED�Ƶ�ʹ��
 *
 * - ʵ������
 *   1. LED_RUN��1sΪ������˸��
 *
 * - ��ע��
 *   1. ��ʱ���Ļص�һ�����жϷ�����ִ�У�����Ӧ�þ�����ִ�к�ʱ�̵Ĺ��������Ҳ�����
 *      �ص���ִ�������ӿڣ���ȴ��ź�����aw_mdelay��
 *
 * \par Դ����
 * \snippet demo_softtimer.c src_softtimer
 * 
 * \internal
 * \par Modification history
 * - 1.00 16-03-28  cod, first implementation.
 * \endinternal
 */
 
 /**
 * \addtogroup demo_if_softtimer �����ʱ��
 * \copydoc demo_softtimer.c
 */

/** [src_softtimer] */
#include "aworks.h"
#include "aw_led.h"
#include "aw_timer.h"
#include "aw_demo_config.h"

#define LED_RUN DM_LED

/**
 * \brief �����ʱ���ж���ں���
 */
aw_local void __timer_isr (void *p_arg)
{
    aw_led_toggle(LED_RUN);                     /* LED_RUN״̬��ת */

    aw_timer_start(p_arg, aw_ms_to_ticks(500)); /*  �ٴζ�ʱ500ms */
}

/**
 * \brief �����ʱ��������ں���
 */
void demo_softtimer_entry (void)
{
    static aw_timer_t timer;

    /* ��ʼ��һ�������ʱ�� */
    aw_timer_init(&timer, __timer_isr, (void *)&timer);

    /* ��ʱ500ms�� ���������ʱ�� */
    aw_timer_start(&timer, aw_ms_to_ticks(500));
}

/** [src_softtimer] */

/* end of file */
