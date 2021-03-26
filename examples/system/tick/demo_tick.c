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
 * \brief ���ò���ϵͳ���ķ�����������ʱ������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE��
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   
 * - ʵ������
 *   1. ��ӡ�����ʱ�������Ϣ��
 
 * - ˵����
 *   ���������ʱ���ڲ���ϵͳ�Ľ����²����ģ����ʱ�侫��ֻ��Ϊms����
 *
 * \par Դ����
 * \snippet demo_tick.c src_tick
 * 
 * \internal
 * \par Modification history
 * - 1.00 16-03-26  cod, first implementation.
 * \endinternal
 */
 
/**
 * \addtogroup demo_if_tick ϵͳ���ķ���
 * \copydoc demo_tick.c
 */

/** [src_tick] */

/* ʹ�ý��ķ���ֻ��Ҫ���� "aworks.h"ͷ�ļ� */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"


/** 
 * \brief tick demo ��ں���
 *
 * \return ��
 */
void demo_tick_entry (void)
{
    unsigned int tick;
    unsigned int mtime;
    unsigned int clkrate;
 
    /*
     * ϵͳ�Ľ����ǲ���ϵͳ��"����".
     * ϵͳ�Ľ�����ʱ��Ĺ�ϵ������ϵͳ���ĵ�Ƶ�ʵõ���ϵͳ�Ľ���Ƶ���ڣ�
     * aw_prj_param.h�ļ��е�ϵͳ���������á�Ĭ������Ϊ��
     *        #define AW_CFG_TICKS_PER_SECOND          1000
     * ������Ϊ1000,��ϵͳ����Ƶ��Ϊ1000Hz��һ�����ľ�Ϊ 1 ms.
     * ϵͳ�ṩ�˽ӿں�����ȡ����Ƶ�ʡ�
     */

    clkrate = aw_sys_clkrate_get();  /* ��ȡϵͳ�Ľ���Ƶ�� */

    tick = aw_sys_tick_get();        /* ��ȡ��ǰϵͳ�Ľ��� */

    aw_mdelay(500);                  /* ���Է�����Ҫ����ʱ��Ĵ��� */

    tick = aw_sys_tick_get() - tick; /* ��ǰ���ļ�ȥ�ϴεĽ��� */

    mtime = aw_ticks_to_ms(tick);    /* ������ת��Ϊʱ�� */

    aw_kprintf("system tick: %4d, use ms: %4d", clkrate, mtime);
}

/** [src_tick] */

/* end of file */
