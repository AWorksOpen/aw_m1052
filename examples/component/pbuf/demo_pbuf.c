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
 * \brief PBUF组件例程（一个资源被多个线程使用）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口使能宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 线程1创建一个对象（缓冲区），使用完并释放，期间线程2也使用到了该对象（资源）；
 *   2. 该资源被多个用户使用，在多个用户都使用完的情况下被释放。该对象在任务1中创建，在
 *      任务2中被销毁，可由串口打印出消息。
 *
 * \par 源代码
 * \snippet demo_pbuf.c src_pbuf
 *
 * \internal
 * \par Modification history
 * - 1.00 16-05-13  bob, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_pbuf PBUF演示
 * \copydoc demo_pbuf.c
 */

/** [src_pbuf] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_pbuf.h"
#include "aw_mem.h"

/******************************************************************************/
#define  BUF_NUM     (5) /**< \brief 缓冲区长度 */

/**
 * \brief 对象或数据包
 */
typedef struct object {

    /** \brief object_t子类，用于管理object_t对象内存 */
    struct aw_pbuf_custom a_pbuf;

} object_t;

aw_local object_t *gp_object = NULL;

/******************************************************************************/
/**
 * \brief 销毁对象内存（析构回调，用户不能使用）
 */
aw_local void __obj_destroy (struct aw_pbuf *p, void *p_arg)
{
    aw_mem_free((void *)p->p_payload);
    aw_mem_free((void *)gp_object);
    gp_object = NULL;
    aw_kprintf("destroy object!\r\n");
}

/**
 * \brief 引用对象
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
 * \brief 释放对象
 */
aw_local void __obj_free (object_t *p_a)
{
    if (NULL == p_a) {
        return ;
    }
    aw_pbuf_free(&p_a->a_pbuf.pbuf);
}

/**
 * \brief 写数据到对象中
 */
aw_local aw_err_t __obj_write (object_t *p_obj, const char *p_dat, uint8_t len)
{
    if (NULL == p_obj) {
        return AW_EINVAL;
    }

    return aw_pbuf_data_write(&p_obj->a_pbuf.pbuf, (const void *)p_dat, len, 0);
}

/**
 * \brief 取对象数据
 */
aw_local aw_err_t __obj_read (object_t *p_obj, char *p_dat, uint8_t len)
{
    if (NULL == p_obj) {
        return AW_EINVAL;
    }

    return aw_pbuf_data_read(&p_obj->a_pbuf.pbuf, p_dat, len, 0);
}


/**
 * \brief 实例化一个对象
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
 * \brief 任务1，创建一个对象并使用，使用完之后释放
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
    aw_mdelay(1000);   /* 对象使用中 */

    aw_kprintf("Had used object_a in task1, will release it\r\n");
    __obj_free(gp_object);

    aw_kprintf("task1 release object_a!\r\n");

    AW_FOREVER {
        aw_mdelay(1000);
    }

    return 0;
}

/**
 * \brief 任务2，引用任务1创建的对象，使用完后释放
 */
aw_local void* __task_func2 (void *parg)
{
    uint8_t i;
    uint8_t buf_dat[BUF_NUM] = {0};

    __obj_get(gp_object);

    aw_kprintf("task2 is using object_a...\r\n");
    __obj_read(gp_object, (char *)buf_dat, BUF_NUM);
    aw_mdelay(3000);   /* 对象使用中 */

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
    /* 定义任务实体，分配栈空间大小为1024 */
    AW_TASK_DECL_STATIC(task_pbuf1, 1024);

    /* 定义任务实体，分配栈空间大小为1024 */
    AW_TASK_DECL_STATIC(task_pbuf2, 1024);

    AW_TASK_INIT(task_pbuf1,            /* 定义的任务实体 */
                 "task_pbuf1",          /* 任务名 */
                 2,                     /* 任务优先级 */
                 1024,                  /* 任务栈大小 */
                 __task_func1,          /* 任务的入口函数 */
                 (void *)0);            /* 传递给任务的参数 */

    AW_TASK_INIT(task_pbuf2,            /* 定义的任务实体 */
                 "task_pbuf2",          /* 任务名 */
                 3,                     /* 任务优先级 */
                 1024,                  /* 任务栈大小 */
                 __task_func2,          /* 任务的入口函数 */
                 (void *)0);            /* 传递给任务的参数 */

    aw_kprintf("\npbuf demo testing...\n");

    aw_pbuf_init();

    AW_TASK_STARTUP(task_pbuf1);        /* 启动任务 */
    AW_TASK_STARTUP(task_pbuf2);        /* 启动任务 */
}

/** [src_pbuf] */

/* end of file */
