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
 * \brief �������̣�˫����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ��ӡ��һ������Ĳ�֣���������֮��ϲ��Ĺ�����Ϣ
 *
 * \par Դ����
 * \snippet demo_list_two.c src_list_two
 *
 * \internal
 * \par Modification history
 * - 1.00 14-08-13  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_list_two ��������
 * \copydoc demo_list_two.c
 */

/** [src_list_two] */
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
 * \brief ��������������ں���
 */
void demo_list_two_entry (void)
{
    _person_t               person_head;        /* ����һ��˫������ͷ�ڵ� */
    _person_t              *temp  = NULL;       /* ������ʱ��Ϣ������ʹ�� */
    struct aw_list_head    *pPos  = NULL;
    struct aw_list_head    *pNext = NULL;
    _person_t               NewNodeCut ;        /* ����ָ�ʱʹ�� */

    uint_t i = 0;
    AW_INIT_LIST_HEAD(&person_head.list);       /* ��ʼ��һ��ͷ�ڵ� */
    AW_INIT_LIST_HEAD(&NewNodeCut.list);

    aw_kprintf("��������������£� \n");

    /*
     * ��Ԫ�ذ�����������
     */
    for (i = 0; i < DataLength; i++) {

        __g_person[i].num       = __g_ui_num[i];
        __g_person[i].PassWord  = __g_ui_pass_word[i];

        /* ���ڵ㵹��������� */
//        aw_list_add(&(__g_person[i].list), &(person_head.list));

        /* ���ڵ������������ */
        aw_list_add_tail(&(__g_person[i].list ), &(person_head.list));
    }

    aw_kprintf("���������Ϣ���£� \n");

    /* forѭ�����б������������ */
    aw_list_for_each(pPos, &person_head.list) {
        /*
         * pPos����ÿ���ṹ�������ָ��
         * ����pPosָ���ƫ�����õ�ָ��ṹ��Ԫ�ص�ָ��temp
         */
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
    }

    aw_kprintf("���������Ϣ���£� \n");

    /* forѭ�����б������������ */
    aw_list_for_each_prev(pPos, &person_head.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
    }

    /*
     * ��������֮��Ĳ���
     * ��ԭ������ָ�Ϊ����������ͷ�ֱ�Ϊperson_head �� NewNodeCut.list
     */
    aw_list_for_each(pPos, &person_head.list) {
    temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 4) {

            /* ��ԭ��������Ϊ�������� */
            aw_list_cut_position(&NewNodeCut.list, &person_head.list, pPos);
            break;
        }
    }

    aw_kprintf("ԭ�����������Ϣ���£� \n");
    aw_list_for_each(pPos,&person_head.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d  PassWord= %d \n", temp->num, temp->PassWord);
    }
    aw_kprintf("��ֵõ��������������Ϣ���£� \n");
    aw_list_for_each(pPos,&NewNodeCut.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d  PassWord= %d \n", temp->num, temp->PassWord);
    }

    /*
     * �ڵ���ƣ��������е�һ���ڵ�ɾ������ӵ�һ���µ�����
     */
    aw_list_for_each(pPos, &person_head.list) {
    temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 5) {                                   /* pPos����Ҫɾ���Ľڵ� */
            aw_list_for_each(pNext,&NewNodeCut.list) {
                temp = aw_list_entry(pNext, _person_t, list);   /* pNext����Ҫ����Ľڵ� */
                if (temp->num == 1) {
                    aw_list_move(pPos, pNext);                  /* ��pPos�ڵ����pNext�ڵ�֮�� */
//                    aw_list_move_tail(pPos, n);                 /* ��pPos�ڵ����pNext�ڵ�֮ǰ */
                    break;
                }
            }
        break;
        }
    }

    aw_kprintf("�ڵ��ƶ���ԭ�����������Ϣ���£� \n");
    aw_list_for_each(pPos, &person_head.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d  PassWord= %d \n", temp->num, temp->PassWord);
    }
    aw_kprintf("�ڵ��ƶ����ֵõ��������������Ϣ���£� \n");
    aw_list_for_each(pPos,&NewNodeCut.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d  PassWord= %d \n", temp->num, temp->PassWord);
    }

    /*
     * ����ϲ�������������������ϲ�Ϊһ���µ�����
     */
    aw_list_for_each(pPos, &person_head.list) {
    temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 8) {

            /* ����������ϲ�Ϊһ������ */
            aw_list_splice_init(&NewNodeCut.list, pPos);
            break;
        }
    }

    aw_kprintf("��������ϲ����µ����������Ϣ���£� \n");
    aw_list_for_each(pPos, &person_head.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d  PassWord= %d \n", temp->num, temp->PassWord);
    }
}

/** [src_list_two] */

/* end of file */
