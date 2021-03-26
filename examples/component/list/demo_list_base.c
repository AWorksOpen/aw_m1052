/*******************************************************************************
*                                 AWorks
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
 * \brief �������̣���ɾ�Ĳ飩
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ��ӡ��������������Ĺ��̣����ӽڵ㡢ɾ���ڵ㡢���ҽڵ���޸Ľڵ�
 *
 * \par Դ����
 * \snippet demo_list_base.c src_list_base
 *
 * \internal
 * \par Modification history
 * - 1.00 14-08-13  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_list_base �����������
 * \copydoc demo_list_base.c
 */

/** [src_list_base] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_list.h"

/**
 * \brief �ṹ�����ͺ����ݶ���
 */
typedef uint_t _element_type;

struct _person {
    _element_type       num;
    _element_type       PassWord;
    struct aw_list_head list;       /* �̳�ͨ������ṹ */
};
typedef struct _person _person_t;

#define DataLength 8

/**
 * \brief ����Ϊ�������Ͷ���
 */
static int          __g_ui_num[DataLength]       = {1,2,3,4,5,6,7,8};
static int          __g_ui_pass_word[DataLength] = {2,3,2,3,4,5,4,2};
static _person_t    __g_person[DataLength];
static _person_t    NewNodeAdd = {9,6};         /* ������ӽڵ�ʹ�� */
static _person_t    NewNodeRep = {10,5};        /* �����滻�ڵ�ʹ�� */

static _person_t           *temp  = NULL;       /* ������ʱ��Ϣ������ʹ�� */
static struct aw_list_head *pPos  = NULL;
static struct aw_list_head *pNext = NULL;

/**
 * \brief ��Ԫ�ذ�����������
 * person_head �����ͷ�ڵ�
 */
static void __list_add_node (_person_t *p_person_head)
{
    uint_t i = 0;

    for (i = 0; i< DataLength; i++) {
        __g_person[i].num       = __g_ui_num[i];
        __g_person[i].PassWord  = __g_ui_pass_word[i];
        AW_INIT_LIST_HEAD(&__g_person[i].list);

        /* ���ڵ������������ */
        aw_list_add_tail(&(__g_person[i].list ), &(p_person_head->list));
    }
}

static void __list_print_node (_person_t *p_person_head)
{
    _person_t           *temp = NULL;   /* ������ʱ��Ϣ������ʹ�� */
    struct aw_list_head *pPos = NULL;

    aw_kprintf("���������Ϣ���£� \n");

    /* forѭ�����б������������ */
    aw_list_for_each(pPos, &p_person_head->list) {
        /*
         * pPos����ÿ���ṹ�������ָ��
         * ����pPosָ���ƫ�����õ�ָ��ṹ��Ԫ�ص�ָ��temp
         */
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
    }
}

/**
 * \brief �����ض�Ԫ�ؽڵ㲢���˽ڵ�ɾ��
 * person_head �����ͷ�ڵ�
 */
static void __list_delete_node (_person_t *p_person_head)
{
    /* pNextΪ���ָ��,����ɾ����������� */
    aw_list_for_each_safe(pPos, pNext, &p_person_head->list) {
        temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 3) {
            /* ��Ԫ��ɾ������ɾ����Ľڵ��ʼ��Ϊͷ�ڵ� */
            aw_list_del_init(pPos);
            break;
        }
    }

    /* ��ɾ���Ľڵ���Ϣ */
    aw_kprintf("ɾ���ڵ����Ϣ���£� \n");
    aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
}

/**
 * \brief �����ض�Ԫ�ؽڵ㣬����һ���½ڵ����˽ڵ�֮��
 * person_head �����ͷ�ڵ�
 */
static void __list_search_add_node (_person_t *p_person_head)
{
    aw_list_for_each(pPos, &p_person_head->list) {
        temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 5) {
            /* ���½ڵ�������� */
            aw_list_add(&NewNodeAdd.list, pPos);
            break;
        }
    }

    /* �¼���Ľڵ���Ϣ */
    aw_kprintf("�¼���Ľڵ����Ϣ���£� \n");
    aw_kprintf(" num= %d PassWord= %d \n", NewNodeAdd.num, NewNodeAdd.PassWord);
}

/**
* \brief �����ض�Ԫ�ؽڵ㣬����һ���½ڵ�����˽ڵ�
* person_head �����ͷ�ڵ�
*/
static void __list_search_replace_node (_person_t *p_person_head)
{
    aw_list_for_each(pPos, &p_person_head->list) {
        temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 2) {
            /* ���½ڵ����ԭ�нڵ㲢�������� */
            aw_list_replace_init(pPos,&NewNodeRep.list);
            break;
        }
    }

    aw_kprintf("����Ľڵ����Ϣ���£� \n");    /* �¼���Ľڵ���Ϣ */
    aw_kprintf(" num= %d PassWord= %d \n", NewNodeRep.num, NewNodeRep.PassWord);
    aw_kprintf("������Ľڵ����Ϣ���£� \n");  /* ������Ľڵ���Ϣ */
    aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
}

/******************************************************************************/
/**
 * \brief ����Ļ�������(��ɾ�Ĳ�)��ں���
 */
void demo_list_base_entry (void)
{
    _person_t person_head;                      /* ����һ��˫������ͷ�ڵ� */
    AW_INIT_LIST_HEAD(&person_head.list);       /* ��ʼ��һ��ͷ�ڵ� */

    aw_kprintf("��������������£� \n");

    aw_kprintf("\n\n�������ڵ���Ϣ�� \n");
    __list_add_node(&person_head);              /* ��Ԫ�ذ����������� */
    __list_print_node(&person_head);            /* ��ӡ����Ľڵ���Ϣ */

    aw_kprintf("\n\nɾ������ڵ���Ϣ�� \n");
    __list_delete_node(&person_head);           /* �����ض�Ԫ�ؽڵ㲢���˽ڵ�ɾ�� */
    __list_print_node(&person_head);            /* ��ӡ����Ľڵ���Ϣ */

    aw_kprintf("\n\n�����ض�Ԫ�ؽڵ㣬����һ���½ڵ����˽ڵ�֮�� \n");
    __list_search_add_node(&person_head);       /* �����ض�Ԫ�ؽڵ㣬����һ���½ڵ����˽ڵ�֮�� */
    __list_print_node(&person_head);            /* ��ӡ����Ľڵ���Ϣ */

    aw_kprintf("\n\n�����ض�Ԫ�ؽڵ㣬����һ���½ڵ�����˽ڵ㣺 \n");
    __list_search_replace_node(&person_head);   /* �����ض�Ԫ�ؽڵ㣬����һ���½ڵ�����˽ڵ� */
    __list_print_node(&person_head);            /* ��ӡ����Ľڵ���Ϣ */

}

/** [src_list_base] */

/* end of file */
