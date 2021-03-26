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
 * \brief ��������(Լɪ��)
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ��������Ļ����������Լɪ��
 *
 * \par Դ����
 * \snippet demo_list_joseph.c src_list_joseph
 *
 * \internal
 * \par Modification history
 * - 1.00 14-08-13  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_list_joseph ����(Լɪ��)
 * \copydoc demo_list_joseph.c
 */

/** [src_list_joseph] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_list.h"

/**
 * \brief �ṹ�����ͺ����ݶ���
 */
typedef uint_t  _element_type;

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


/**
 * \brief ��������Ļ����������Լɪ��
 * ������������֪n���ˣ��Ա��1��2��3...n�ֱ��ʾ��Χ����һ��Բ����Χ��ÿһ���˶���һ��Ψһ�����룬
 *         ���Ϊ1���˽��Լ�������˵������Ϊ��������m��Ȼ��ʼ����������m���Ǹ��˳��У�
 *         �����Լ�������˵����ֵ��mȻ��ɱ����������һ�����ִ�1��ʼ����������m���Ǹ����ֳ���Ȼ��ɱ��
 *         �����Լ����뽫m���¸�ֵ�����˹����ظ���ȥ��ֱ��Բ����Χ����ȫ�����в���ɱ���������γ��е�˳��
 */
void demo_list_joseph_entry (void)
{
    _person_t person_head;

    _person_t *temp = NULL;
    struct aw_list_head *pPos    = NULL;    /* ������������б�ɱ���Ľڵ� */
    struct aw_list_head *pHead   = NULL;    /* ����ÿ�α��������е�ͷ�ڵ� */
    struct aw_list_head *pNext   = NULL;    /* ���汻ɱ���ڵ����һ���ڵ㣬��¼ʹ�� */

    uint_t i = 0;
    uint_t FlagNum = __g_ui_pass_word[0];   /* ��¼ÿһ���ڵ��������Ϣ */
    AW_INIT_LIST_HEAD(&person_head.list);   /* ��ʼ��һ��ͷ�ڵ� */

    aw_kprintf("����ΪԼɪ������ \n");

    /* ��Ԫ�ذ����������� */
    for(i = 0; i < DataLength; i++) {

        __g_person[i].num      = __g_ui_num[i];
        __g_person[i].PassWord = __g_ui_pass_word[i];

        /* ���ڵ������������ */
        aw_list_add_tail(&(__g_person[i].list), &(person_head.list));
    }

    aw_kprintf("���������Ϣ���£� \n");

    /* forѭ�����б������������ */
    aw_list_for_each(pPos, &person_head.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
    }

    pHead = &(person_head.list);    /* ����ÿ�α�����ͷ�ڵ� */
    i = 1;                          /* ����ÿ�α������ߵĲ��� */

    while (!aw_list_empty(&person_head.list)) {     /* �����нڵ㱻ɱ��������Ϊ�գ�ֹͣ���� */

        /*
         * forѭ��ģ��aw_list_for_each_safe(pPos, n, q)������ȥ���˽�ֹ����
         * ʵ������Ӧ�Ĳ������ҵ�Ҫɱ���Ľڵ�
         */
        for (pPos = pHead->next, pNext = pPos->next; ; pPos = pNext, pNext = pPos->next) {
            if ((i == FlagNum) && (pPos != &person_head.list)) {   /* ������Ӧ�Ĳ������ҵ���ɱ���Ľڵ� */
                break;
            } else if (pPos != &person_head.list) {     /* �������Ľڵ㲻����ͷ��㣨û�����ݣ� */
                i++;    /* û���ҵ�����������ͬʱ������¼ */
            }
        }

        FlagNum = (aw_list_entry(pPos, _person_t, list))->PassWord; /* ���ҵ���ɱ���ڵ��������Ϊ�µ�Ҫ�ߵĲ��� */
        aw_list_del_init(pPos);                                     /* ���ҵ��Ľڵ�ɱ��������������� */
        pHead = pNext->prev;                                        /* ���¹���ͷ�ڵ� */
        i = 1;                                                      /* �����������¸�ֵΪ1�������´α��� */

        aw_kprintf(" ��ɱ���Ľڵ���Ϊ %d \n", aw_list_entry(pPos, _person_t, list)->num);
    }
}

/** [src_list_joseph] */

/* end of file */
