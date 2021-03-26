/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief awtkʾ������
 *
 * - ��������:
 *   1. ������ʾ��
 *   2. ����Ĭ������4.3����ʾ�����������7����ʾ��������aw_prj_params.h�У�
 *      - ��7����ʾ��
 *      - �ر�4.3����ʾ��
 *
 * - ʵ������:
 *   1. ��һ����¼������Ҫ���д���У�飬����У��ͨ�������gui����
 *   2. �����ٴ���¼������Ҫ����У�飬ֱ�ӽ���gui����
 *
 */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_delay.h"
#include "aw_fb.h"

#define LED 0

extern void app_awtk_demo (void);

int aw_main()
{
    aw_kprintf("\r\nApplication Start. \r\n");

    app_awtk_demo();

    while (1) {
        aw_led_toggle(LED);
        aw_mdelay(500);
    }

    return 0;
}


/* end of file*/
