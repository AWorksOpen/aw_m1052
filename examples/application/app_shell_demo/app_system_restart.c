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
 * \brief ͨ��shell��������������
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - WDT�豸��(���Ź�)
 *   2. ���������ص�flash���棬�ε�������WDT�ϵ�����ñ�����������󣬰���3������
 *   3. ��shell�������".test"��Ŀ¼������"sys_rst"��
 *
 * \par Դ����
 * \snippet app_system_restart.c app_system_restart
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "aw_wdt.h"
#include "aw_shell.h"
#include "aw_delay.h"
#include "app_config.h"
#include "aw_vdebug.h"

#ifdef APP_SYS_RST


int app_system_restart  (int argc, char **argv, struct aw_shell_user *p_user)
{
    struct awbl_wdt wdt_dev;
    aw_err_t  ret = AW_OK;

    /* ���ÿ��Ź�ʱ�� */
    ret = aw_wdt_add(&wdt_dev, 1);
    if (ret != AW_OK) {
        aw_kprintf("*ERR: WDT add error %d\n", ret);
        return -AW_EINVAL;
    }


    /* LED0 �����˸ */
    while (1) {
        aw_mdelay(100);
    }

    return AW_OK;
}


#endif
