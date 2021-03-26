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
 * \brief sys_defer �������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. �����ȴ�ӡ��job done, job data: 0xabcdef00���ٴ�ӡ
 *      ��job done, job data: 0x12345678��
 *
 * - ��ע��
 *   1. ���û���һЩ������Ҫ���첽���ã��������ύ��һЩ�������߳�ȥִ�У���ô defer �ͺ�������
 *      ֻ�趨�� defer ����������Ȼ�󽫹����ڵ� commit ���� defer �м���
 *   2. ��Ȼ��AWorks ����ᴴ��һЩϵͳ defer ȥ�������� nor_defer �� isr_defer��
 *      ISR defer�� һ�����ڴ����������
 *      Normal defer�� һ�����ڴ���ǽ�����
 *
 * \par Դ����
 * \snippet demo_sys_defer.c src_sys_defer
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_sys_defer ϵͳ�ӳ���ҵ�������
 * \copydoc demo_sys_defer.c
 */

/** [src_sys_defer] */
#include "aworks.h"
#include "aw_sys_defer.h"
#include "aw_vdebug.h"
#include "aw_sem.h"

aw_local struct aw_defer_djob   __g_djob_nor;
aw_local struct aw_defer_djob   __g_djob_isr;

AW_SEMC_DECL_STATIC(__g_sem);

aw_local void __job_call (void *p_arg)
{
    AW_INFOF(("job done, job data: 0x%08x\n", (uint32_t)p_arg));
    AW_SEMC_GIVE(__g_sem);
}

/**
 * \brief sys_defer����������
 * \return ��
 */
void demo_sys_defer_entry (void)
{
    aw_err_t  ret = AW_OK;

    /* ��ʼ���ź��� */
    AW_SEMC_INIT(__g_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    /* ��ʼ�������ڵ� */
    aw_defer_job_init(&__g_djob_nor, __job_call, (void *)0x12345678);
    aw_defer_job_init(&__g_djob_isr, __job_call, (void *)0xabcdef00);

    /* ��һ�������ڵ������ϵͳ��ͨ�ӳ���ҵ�д��� */
    ret = aw_sys_defer_commit(AW_SYS_DEFER_TYPE_NOR, &__g_djob_nor);
    if (ret != AW_OK) {
        AW_ERRF(("aw_sys_defer_commit err: %d\n", ret));
    }

    /* �жϵ�ǰ�����ڵ��Ƿ��Ѿ�ʹ�� */
    if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, &__g_djob_nor)) {
        AW_INFOF(("nor job is used\n"));
    }

    /* ��һ�������ڵ������ϵͳ�����ӳ���ҵ�д��� */
    ret = aw_sys_defer_commit(AW_SYS_DEFER_TYPE_ISR, &__g_djob_isr);
    if (ret != AW_OK) {
        AW_ERRF(("aw_sys_defer_commit err: %d\n", ret));
    }

    /* �жϵ�ǰ�����ڵ��Ƿ��Ѿ�ʹ�� */
    if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_ISR, &__g_djob_isr)) {
        AW_INFOF(("isr job is used\n"));
    }

    /* �ȴ���ǰ��������� */
    AW_SEMC_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);
    AW_SEMC_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);
}

/** [src_sys_defer] */

/* end of file */
