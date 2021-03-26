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
 * \brief defer�ӳ���ҵ�������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ���ڴ�ӡ��job done, job data: 0x12345678��
 *
 * - ��ע��
 *   1. ���û���һЩ������Ҫ���첽���ã��������ύ��һЩ�������߳�ȥִ�У���ô defer �ͺ�������
 *      ֻ�趨�� defer ����������Ȼ�󽫹����ڵ� commit ���� defer �м���
 *   2. ��Ȼ��AWorks ����ᴴ��һЩϵͳ defer ȥ�����ο� "aw_sys_defer.h"
 *
 * \par Դ����
 * \snippet demo_defer.c src_defer
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_defer defer�ӳ���ҵ�������
 * \copydoc demo_defer.c
 */

/** [src_defer] */
#include "aworks.h"
#include "aw_defer.h"
#include "aw_vdebug.h"
#include "aw_sem.h"


aw_local struct aw_defer        __g_defer;  /* ����defer���� */
aw_local struct aw_defer_djob   __g_djob;   /* ���幤���ڵ� */
AW_SEMB_DECL_STATIC(__g_sem);

aw_local void __job_call (void *p_arg)
{
    AW_INFOF(("\njob done, job data: 0x%08x\n", (uint32_t)p_arg));
    AW_SEMB_GIVE(__g_sem);
}

/**
 * \brief defer����������
 * \return ��
 */
void demo_defer_entry (void)
{
    aw_err_t  ret = AW_OK;

    /* ��ʼ��һ���ӳ���ҵ */
    aw_defer_init(&__g_defer);

    /* �����ӳ���ҵ������3�����ȼ�Ϊ0����ջ��СΪ4096���̣߳��������� */
    AW_DEFER_START(&__g_defer, 3, 0, 4096, "demo_defer");

    /* ��ʼ��һ�������ڵ� */
    aw_defer_job_init(&__g_djob, __job_call, (void *)0x12345678);

    /* ��ʼ���ź��� */
    AW_SEMB_INIT(__g_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    /* ��һ�������ڵ�������ӳ���ҵ���� */
    ret = aw_defer_commit(&__g_defer, &__g_djob);
    if (ret != AW_OK) {
        AW_ERRF(("aw_defer_commit err: %d\n", ret));
    }

    /* �ȴ���ǰ��������� */
    AW_SEMB_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);

    /* �жϵ�ǰ�����ڵ��Ƿ���� */
    if (aw_defer_job_is_usable(&__g_defer, &__g_djob)) {
        AW_INFOF(("curr job can usable\n"));
    }
}

/** [src_defer] */
 
/* end of file */
