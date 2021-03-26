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
 * \brief Calculate ���ܲ���
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ�� ��Ӧƽ̨UART�ꡣ
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *
 * - ��ע��
 *   1.countԽ�󣬼�����Խ��ȷ
 *
 * \par Դ����
 * \snippet demo_cal_pi.c src_cal_pi
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-09  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_cal_pi �������ܲ���
 * \copydoc demo_cal_pi.c
 */

/** [src_cal_pi] */

#include "aworks.h"
#include "aw_system.h"
#include "aw_vdebug.h"

static double __cal_pi (int cal_count)
{
    register double t, r, N, c;
    register int    i;

    r = 0;
    c = 4;
    N = 1;

    //Բ���ʼ��㷽�� �� = 4(1-1/3+1/5-1/7+....1/n)
    for (i = 0; i < cal_count; i++) {
        N = 2 * i + 1;
        t = c / (double)N;
        if (i % 2) {
            r -= t;
        } else {
            r += t;
        }
    }
    return r;
}

/**
 * \brief �������
 * \return ��
 */
void demo_cal_pi_entry (void)
{
    int                 count = 30000000;
    double              pi;
    volatile aw_tick_t  t1, t2;

    t1 = aw_sys_tick_get();
    pi = __cal_pi(count);
    t2 = aw_sys_tick_get();

    aw_kprintf("loop %d times, pi = %.9f, time = %d ms\n",
               count, pi, aw_ticks_to_ms(t2 - t1));
    return;
}

/** [src_cal_pi] */

/* end of file */
