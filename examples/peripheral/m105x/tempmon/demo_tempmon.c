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
 * \brief mcu�¶ȶ�ȡ����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��AW_DEV_IMX1050_TEMPMON
 *
 * - ʵ������
 *   1. ���ڴ�ӡmcu�¶���Ϣ��
 *
 * \par Դ����
 * \snippet demo_tempmon.c src_tempmon
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_tempmon mcu�¶ȶ�ȡ
 * \copydoc demo_tempmon.c
 */

/** [src_tempmon] */
#include "aworks.h"         /* this file must be included first */
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_temp.h"

/**
 * \brief tempmon demo entry
 */
void demo_tempmon_entry (void)
{
    float temp;

    AW_FOREVER {

        /* get current cpu temperature */
        temp = aw_temp_read(0) / 1000.0;
        AW_INFOF(("temp is %f centigrade\n", temp));

        aw_mdelay(200);
    }
}

/** [src_tempmon] */

/* end of file */
