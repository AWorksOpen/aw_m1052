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
 * \brief ��ʱ������ʾ����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��AW_DEV_GPIO_LED��
 *
 * - ʵ������
 *   1. LED��2sΪ������˸��
 *
 * \par Դ����
 * \snippet demo_delay.c src_delay
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  sup, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_delay ��ʱ����
 * \copydoc demo_delay.c
 */

/** [src_delay] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_led.h"
#include "aw_demo_config.h"

#define  LED0      DM_LED

/**
 * \brief ��ʱ������ʾ�������
 * \return ��
 */
void demo_delay_entry (void)
{
    AW_FOREVER {
        aw_led_on(LED0);
        aw_mdelay(1000);    /* ��ʱ1s */

        aw_led_off(LED0);
        aw_mdelay(1000);    /* ��ʱ1s */
    }
}

/** [src_delay] */

/* end of file */
