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
 * \brief ���ü�����ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ���ʹ�ܺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *      ���ڴ�ӡ������Ϣ��
 *
 * - ��ע��
 *     1.����ڶ��������ʹ��ͬһ���ڴ���Դ����ĳ������ʹ����ɺ󣬲�ȷ���Ƿ�������������ʹ�������Դ���ʲ���ֱ���ͷ���Դ��
 *     ������Դ����������ü������ͷ�ǰ�ж��Ƿ����������������Դ��������ֵΪ0ʱ����Դ��Ч�ٽ����ͷš�
 *     2.����һ��ʼ�����ü������������Ҳ�õ����������ÿ��������ʹ���깫����Դ�󣬶�����aw_refcnt_put()�ӿ��ͷ�
 *     ���ü�����ֻ�е����ü���Ϊ0ʱ�Ż���ûص��������������ͷŵ��ڴ���Դ��
 *
 * \par Դ����
 * \snippet demo_refcnt.c src_refcnt
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  sup, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_refcnt ���ü���
 * \copydoc demo_refcnt.c
 */

/** [src_refcnt] */
#include "aworks.h"
#include "aw_refcnt.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "aw_task.h"
#include <string.h>
#include "aw_delay.h"

#define MEM_SIZE   10   /* ��̬�ڴ��С */

struct ref_mem {
    struct aw_refcnt    ref;
    uint8_t            *p_mem;
};

AW_MUTEX_DECL_STATIC(__g_data_mutex);   /* ���廥���ź��������ڱ���ȫ�ֱ��� */

aw_local char           __g_str[24];    /* ����ַ��� */
aw_local struct ref_mem test_ref;       /* ����һ��ȫ�ֵ����ü������� */


/**
 * \brief ���ü����ͷŻص��ӿ�
 * \return ��
 */
static void __release_callback (struct aw_refcnt *p_ref)
{
    struct ref_mem  *p = AW_CONTAINER_OF(p_ref, struct ref_mem, ref);

    /* �ͷŶ�̬�ڴ� */
    aw_mem_free(p->p_mem);

    aw_kprintf("resource is released by %s\n", __g_str);
}

static void* __task1_func (void *parg)
{
    int i = 0;

    /* ��ʼ��һ�����ü�������,��ʼֵΪ1 */
    aw_refcnt_init(&test_ref.ref);

    test_ref.p_mem = aw_mem_alloc(MEM_SIZE);
    if (NULL == test_ref.p_mem) {
        AW_ERRF(("aw_mem_alloc err: %d\n", -AW_ENOMEM));
        return 0;
    }

    AW_MUTEX_LOCK(__g_data_mutex, AW_SEM_WAIT_FOREVER);

    for (i = 0; i < MEM_SIZE; i++) {
        test_ref.p_mem[i] = (char)i;
    }

    AW_MUTEX_UNLOCK(__g_data_mutex);

    /* ��ʱ���ȴ�����2���� */
    aw_mdelay(3000);

    memcpy(__g_str, "task1", 5);

    /* ���ü�����һ��������ֵ��Ч(0)������ûص����� */
    aw_refcnt_put(&test_ref.ref, __release_callback);

    /* ��ѯ���ü���״̬����ʱ״̬Ϊ��Ч */
    AW_INFOF(("__task1_func end, count is %s\n",
             aw_refcnt_valid(&test_ref.ref) ? "valid" : "invalid"));

    while (1) {
        aw_mdelay(500);
    }

    return 0;
}

static void* __task2_func (void *parg)
{
    int i = 0;

    /* ���ü�����һ */
    aw_refcnt_get(&test_ref.ref);

    AW_MUTEX_LOCK(__g_data_mutex, AW_SEM_WAIT_FOREVER);

    /* ʹ�ö�̬�ڴ� */
    for (i = 0; i < MEM_SIZE; i++) {
        aw_kprintf("test_ref.p_mem[%d] = %d\r\n", i, test_ref.p_mem[i]);
    }

    AW_MUTEX_UNLOCK(__g_data_mutex);

    memcpy(__g_str, "task2", 5);

    /* ���ü�����һ��������ֵ��Ч(0)������ûص����� */
    aw_refcnt_put(&test_ref.ref, __release_callback);

    /* ��ѯ���ü���״̬����ʱ״̬Ϊ��Ч */
    AW_INFOF(("__task2_func end, count is %s\n",
             aw_refcnt_valid(&test_ref.ref) ? "valid" : "invalid"));

    while (1) {
        aw_mdelay(500);
    }

    return 0;

}

/**
 * \brief ���ü�����ʾ�������
 * \return ��
 */
void demo_refcnt_entry (void)
{
    /* ��������ʵ�壬����ջ�ռ��СΪ1024 */
    AW_TASK_DECL_STATIC(task1, 1024);

    /* ��������ʵ�壬����ջ�ռ��СΪ1024 */
    AW_TASK_DECL_STATIC(task2, 1024);

    AW_TASK_INIT(task1,                 /* ���������ʵ�� */
                 "task1_refcnt",        /* ������ */
                 5,                     /* �������ȼ� */
                 1024,                  /* ����ջ��С */
                 __task1_func,          /* �������ں��� */
                 (void *)NULL);         /* ���ݸ�����Ĳ��� */

    AW_TASK_INIT(task2,                 /* ���������ʵ�� */
                 "task2_refcnt",        /* ������ */
                 8,                     /* �������ȼ� */
                 1024,                  /* ����ջ��С */
                 __task2_func,          /* �������ں��� */
                 (void *)NULL);         /* ���ݸ�����Ĳ��� */

    AW_TASK_STARTUP(task1);     /* ��������1 */

    AW_TASK_STARTUP(task2);     /* ��������2 */

    return ;
}

/** [src_refcnt] */

/* end of file */
