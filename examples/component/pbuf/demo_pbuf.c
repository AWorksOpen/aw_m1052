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
 * \brief PBUF������̣�һ����Դ������߳�ʹ�ã�
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ���ʹ�ܺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. �߳�1����һ�����󣨻���������ʹ���겢�ͷţ��ڼ��߳�2Ҳʹ�õ��˸ö�����Դ����
 *   2. ����Դ������û�ʹ�ã��ڶ���û���ʹ���������±��ͷš��ö���������1�д�������
 *      ����2�б����٣����ɴ��ڴ�ӡ����Ϣ��
 *
 * \par Դ����
 * \snippet demo_pbuf.c src_pbuf
 *
 * \internal
 * \par Modification history
 * - 1.00 16-05-13  bob, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_pbuf PBUF��ʾ
 * \copydoc demo_pbuf.c
 */

/** [src_pbuf] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_pbuf.h"
#include "aw_mem.h"

/******************************************************************************/
#define  BUF_NUM     (5) /**< \brief ���������� */

/**
 * \brief ��������ݰ�
 */
typedef struct object {

    /** \brief object_t���࣬���ڹ���object_t�����ڴ� */
    struct aw_pbuf_custom a_pbuf;

} object_t;

aw_local object_t *gp_object = NULL;

/******************************************************************************/
/**
 * \brief ���ٶ����ڴ棨�����ص����û�����ʹ�ã�
 */
aw_local void __obj_destroy (struct aw_pbuf *p, void *p_arg)
{
    aw_mem_free((void *)p->p_payload);
    aw_mem_free((void *)gp_object);
    gp_object = NULL;
    aw_kprintf("destroy object!\r\n");
}

/**
 * \brief ���ö���
 */
aw_local aw_err_t __obj_get (object_t *p_a)
{
    if (NULL == p_a) {
        return AW_ERROR;
    } else {
        aw_pbuf_ref(&p_a->a_pbuf.pbuf);
        return AW_OK;
    }
}

/**
 * \brief �ͷŶ���
 */
aw_local void __obj_free (object_t *p_a)
{
    if (NULL == p_a) {
        return ;
    }
    aw_pbuf_free(&p_a->a_pbuf.pbuf);
}

/**
 * \brief д���ݵ�������
 */
aw_local aw_err_t __obj_write (object_t *p_obj, const char *p_dat, uint8_t len)
{
    if (NULL == p_obj) {
        return AW_EINVAL;
    }

    return aw_pbuf_data_write(&p_obj->a_pbuf.pbuf, (const void *)p_dat, len, 0);
}

/**
 * \brief ȡ��������
 */
aw_local aw_err_t __obj_read (object_t *p_obj, char *p_dat, uint8_t len)
{
    if (NULL == p_obj) {
        return AW_EINVAL;
    }

    return aw_pbuf_data_read(&p_obj->a_pbuf.pbuf, p_dat, len, 0);
}


/**
 * \brief ʵ����һ������
 */
aw_local object_t *__object_a (size_t mem_size)
{
    object_t *p_obj = NULL;
    char *p_mem     = NULL;

    p_obj = (object_t *)aw_mem_alloc(sizeof(object_t));
    if (NULL == p_obj) {
        return NULL;
    }

    p_mem = (char *)aw_mem_alloc(mem_size);
    if (NULL == p_mem) {
        return NULL;
    }

    aw_pbuf_alloced_custom(&p_obj->a_pbuf,
                           p_mem,
                           sizeof(object_t),
                           __obj_destroy,
                           (void *)p_obj);

    return p_obj;
}

/******************************************************************************/
/**
 * \brief ����1������һ������ʹ�ã�ʹ����֮���ͷ�
 */
aw_local void* __task_func1 (void *parg)
{
    const char data[BUF_NUM] = {0x00, 0x01, 0x02, 0x03, 0x04};

    if (NULL == (gp_object = __object_a(sizeof(data)))) {
        aw_kprintf("object_a created by task1, failed!\r\n");
        return 0;
    } else {
        aw_kprintf("object_a created by task1, success!\r\n");
    }

    aw_kprintf("object_a was using in task1...\r\n");
    __obj_write(gp_object, data, BUF_NUM);
    aw_mdelay(1000);   /* ����ʹ���� */

    aw_kprintf("Had used object_a in task1, will release it\r\n");
    __obj_free(gp_object);

    aw_kprintf("task1 release object_a!\r\n");

    AW_FOREVER {
        aw_mdelay(1000);
    }

    return 0;
}

/**
 * \brief ����2����������1�����Ķ���ʹ������ͷ�
 */
aw_local void* __task_func2 (void *parg)
{
    uint8_t i;
    uint8_t buf_dat[BUF_NUM] = {0};

    __obj_get(gp_object);

    aw_kprintf("task2 is using object_a...\r\n");
    __obj_read(gp_object, (char *)buf_dat, BUF_NUM);
    aw_mdelay(3000);   /* ����ʹ���� */

    for (i = 0; i < BUF_NUM; i++) {
        aw_kprintf("%d", buf_dat[i]);
    }

    aw_kprintf("\ntask2 had used object_a, will release it!\r\n");
    __obj_free(gp_object);

    aw_kprintf("task2 had released object_a!\r\n");

    AW_FOREVER {
        aw_mdelay(1000);
    }

    return 0;
}

/******************************************************************************/
/**
 * \brief pbuf demo
 */
void demo_pbuf_entry (void)
{
    /* ��������ʵ�壬����ջ�ռ��СΪ1024 */
    AW_TASK_DECL_STATIC(task_pbuf1, 1024);

    /* ��������ʵ�壬����ջ�ռ��СΪ1024 */
    AW_TASK_DECL_STATIC(task_pbuf2, 1024);

    AW_TASK_INIT(task_pbuf1,            /* ���������ʵ�� */
                 "task_pbuf1",          /* ������ */
                 2,                     /* �������ȼ� */
                 1024,                  /* ����ջ��С */
                 __task_func1,          /* �������ں��� */
                 (void *)0);            /* ���ݸ�����Ĳ��� */

    AW_TASK_INIT(task_pbuf2,            /* ���������ʵ�� */
                 "task_pbuf2",          /* ������ */
                 3,                     /* �������ȼ� */
                 1024,                  /* ����ջ��С */
                 __task_func2,          /* �������ں��� */
                 (void *)0);            /* ���ݸ�����Ĳ��� */

    aw_kprintf("\npbuf demo testing...\n");

    aw_pbuf_init();

    AW_TASK_STARTUP(task_pbuf1);        /* �������� */
    AW_TASK_STARTUP(task_pbuf2);        /* �������� */
}

/** [src_pbuf] */

/* end of file */
