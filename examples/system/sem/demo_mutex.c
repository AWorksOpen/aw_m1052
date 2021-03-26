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
 * \brief AWorksƽ̨�л�����ʹ�����̣�Mutex��
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. �����ʹ�û����ź���������ע�͵� USE_MUTEX �ĺ궨�塣���ӡ��Ϣ�л���֡�calculate error
 *      !������ȫ�ֱ������ʳ�����Ԥ�����������������2�������ǽ���������29�����ּ���29�����ʼ������
 *      ֵ����29��ͬ��
 *   2. ʹ�û������󣬴�ӡ��Ϣ�в�����֡�calculate error!��������after add 29: gdata���ֵΪ
 *      before:gdata���ֵ��29������������Դ������ȷ��
 *
 * - ˵����
 *   �ź�������ȡ���ͷ���ͬһ�������У�������Զ�λ�ȡ����Ҳ�����Ӧ����ͷš�
 *        ��������һ�������л�ȡ������һ���������ͷš�
 *        �����ź�����Ҫ�����豸������Ļ�����ʡ�����ǰ���룬���ʺ��ͷ���Դ��
 *        һ�������гɶԳ��ֻ�ȡ��LOCK�����ͷ�(UNLOCK)��
 *
 *   ����1������2��������ȫ�ֱ���__g_data��
 *   Ϊ��ȷ�����ʵ���ȷ�ԣ��ͱ���ʹ�û����ź���������б�������֤ȫ�ֱ����ķ����ǻ���ġ�
 *   ������1ʹ��ʱ������2�Ͳ���ʹ�ã�����2ʹ��ʱ������1����ʹ�ã�ȷ���˱������ʵ���ȷ�ԡ�
 *
 * \par Դ����
 * \snippet demo_mutex.c src_mutex
 *
 * \internal
 * \par Modification history
 * - 1.01 17-09-06  anu, modify.
 * - 1.00 16-03-26  cod, first implementation.
 * \endinternal
 */

 /**
 * \addtogroup demo_if_mutex ������
 * \copydoc demo_mutex.c
 */

/** [src_mutex] */

#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_sem.h"


/* ʹ�û����ź�������ȫ�ֱ����ķ��� */
#define USE_MUTEX

static unsigned int __g_data = 0;
AW_MUTEX_DECL_STATIC(__g_data_mutex);        /* ���廥���ź��� */


/**
 * \brief ����2��ں���
 *
 * \param[in] p_arg : ע��ʱ�û�������Զ������
 *
 * \return ��
 */
static void task2_entry (void *p_arg)
{
    unsigned int temp;
    volatile unsigned int i, j;

    /* ����ͨ������ʽ����һ����ѭ��������ѭ�� */
    AW_FOREVER {

#ifdef USE_MUTEX
        AW_MUTEX_LOCK(__g_data_mutex, AW_SEM_WAIT_FOREVER);
#endif
        temp = __g_data;
        aw_kprintf("before:gdata = %d\r\n", __g_data);

        /* ���ڳ�ʱ��ռ�и��� */
        for (i = 0; i < 10000; i++) {
            for (j = 0; j < 502; j++) {
                ;
            }
        }

        __g_data += 29;

        /* �����ʹ�� MUTEX ���Է��֣���ӡ������������Ԥ�ڲ�����ʹ�ú�����ȫ����ϵ� */
        aw_kprintf("after add 29: gdata = %d\r\n\r\n", __g_data);

        if ((temp + 29) != __g_data) {
            aw_kprintf("calculate error!\r\n");
        }

#ifdef USE_MUTEX
        AW_MUTEX_UNLOCK(__g_data_mutex);
#endif
        aw_mdelay(2);
    }
}

/**
 * \brief ����1��ں���
 *
 * \param[in] p_arg : ע��ʱ�û�������Զ������
 *
 * \return    ��
 */
static void task1_entry(void *p_arg)
{
    unsigned int temp;

    AW_FOREVER {

#ifdef USE_MUTEX
        AW_MUTEX_LOCK(__g_data_mutex, AW_SEM_WAIT_FOREVER);
#endif

        temp = __g_data;
        __g_data -= 11;

        if ((temp - 11) != __g_data) {
            aw_kprintf("calculate error!\r\n");
        }

#ifdef USE_MUTEX
        AW_MUTEX_UNLOCK(__g_data_mutex);
#endif

        aw_mdelay(1);
    }
}

/******************************************************************************/
/**
 * \brief ������demo��ں���
 */
void demo_mutex_entry (void)
{
    AW_TASK_DECL_STATIC(task1, 2048);
    AW_TASK_DECL_STATIC(task2, 2048);

    AW_TASK_INIT(task1,       /* ����ʵ�� */
                 "task1name", /* �������� */
                 5,           /* �������ȼ� */
                 2048,        /* �����ջ��С */
                 task1_entry, /* ������ں��� */
                 (void*)1);   /* ������ڲ��� */

    AW_TASK_INIT(task2,       /* ����ʵ�� */
                 "task2name", /* �������� */
                 6,           /* �������ȼ� */
                 2048,        /* �����ջ��С */
                 task2_entry, /* ������ں��� */
                 (void*)2);   /* ������ڲ��� */

    /* ��ʼ��ʱ��ѡ�����ã�
     *  AW_SEM_Q_FIFO���Ⱥ�˳���Ŷӣ�
     *  AW_SEM_Q_PRIORITY (�����ȼ��Ŷ�)
     */
    AW_MUTEX_INIT(__g_data_mutex, AW_SEM_Q_FIFO);

    AW_TASK_STARTUP(task1);
    AW_TASK_STARTUP(task2);
}

/** [src_mutex] */

/* end of file */
