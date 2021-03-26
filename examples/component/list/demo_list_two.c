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
 * \brief 链表例程（双链表）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口对应的宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 打印出一个链表的拆分，两个链表之间合并的过程信息
 *
 * \par 源代码
 * \snippet demo_list_two.c src_list_two
 *
 * \internal
 * \par Modification history
 * - 1.00 14-08-13  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_list_two 链表间操作
 * \copydoc demo_list_two.c
 */

/** [src_list_two] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_list.h"

/**
 * \brief 结构体类型和数据定义
 */
typedef uint_t  _element_type;

struct _person {
    _element_type       num;
    _element_type       PassWord;
    struct aw_list_head list;       /* 继承通用链表结构 */
};
typedef struct _person _person_t;

#define DataLength 8

/**
 * \brief 以下为数据类型定义
 */
static int          __g_ui_num[DataLength]       = {1,2,3,4,5,6,7,8};
static int          __g_ui_pass_word[DataLength] = {2,3,2,3,4,5,4,2};
static _person_t    __g_person[DataLength];

/**
 * \brief 操作两个链表入口函数
 */
void demo_list_two_entry (void)
{
    _person_t               person_head;        /* 定义一个双向链表头节点 */
    _person_t              *temp  = NULL;       /* 储存暂时信息，遍历使用 */
    struct aw_list_head    *pPos  = NULL;
    struct aw_list_head    *pNext = NULL;
    _person_t               NewNodeCut ;        /* 链表分割时使用 */

    uint_t i = 0;
    AW_INIT_LIST_HEAD(&person_head.list);       /* 初始化一个头节点 */
    AW_INIT_LIST_HEAD(&NewNodeCut.list);

    aw_kprintf("链表基本服务如下： \n");

    /*
     * 将元素挨个插入链表
     */
    for (i = 0; i < DataLength; i++) {

        __g_person[i].num       = __g_ui_num[i];
        __g_person[i].PassWord  = __g_ui_pass_word[i];

        /* 将节点倒序插入链表 */
//        aw_list_add(&(__g_person[i].list), &(person_head.list));

        /* 将节点正序插入链表 */
        aw_list_add_tail(&(__g_person[i].list ), &(person_head.list));
    }

    aw_kprintf("正序输出信息如下： \n");

    /* for循环进行遍历并正序输出 */
    aw_list_for_each(pPos, &person_head.list) {
        /*
         * pPos保存每个结构体的链表指针
         * 根据pPos指针和偏移量得到指向结构体元素的指针temp
         */
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
    }

    aw_kprintf("倒序输出信息如下： \n");

    /* for循环进行遍历并倒序输出 */
    aw_list_for_each_prev(pPos, &person_head.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
    }

    /*
     * 两个链表之间的操作
     * 将原来链表分割为两个链表，表头分别为person_head 和 NewNodeCut.list
     */
    aw_list_for_each(pPos, &person_head.list) {
    temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 4) {

            /* 将原来链表拆分为两个链表 */
            aw_list_cut_position(&NewNodeCut.list, &person_head.list, pPos);
            break;
        }
    }

    aw_kprintf("原来链表输出信息如下： \n");
    aw_list_for_each(pPos,&person_head.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d  PassWord= %d \n", temp->num, temp->PassWord);
    }
    aw_kprintf("拆分得到的新链表输出信息如下： \n");
    aw_list_for_each(pPos,&NewNodeCut.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d  PassWord= %d \n", temp->num, temp->PassWord);
    }

    /*
     * 节点搬移，将链表中的一个节点删除后添加到一个新的链表
     */
    aw_list_for_each(pPos, &person_head.list) {
    temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 5) {                                   /* pPos保存要删除的节点 */
            aw_list_for_each(pNext,&NewNodeCut.list) {
                temp = aw_list_entry(pNext, _person_t, list);   /* pNext保存要插入的节点 */
                if (temp->num == 1) {
                    aw_list_move(pPos, pNext);                  /* 将pPos节点插入pNext节点之后 */
//                    aw_list_move_tail(pPos, n);                 /* 将pPos节点插入pNext节点之前 */
                    break;
                }
            }
        break;
        }
    }

    aw_kprintf("节点移动后原来链表输出信息如下： \n");
    aw_list_for_each(pPos, &person_head.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d  PassWord= %d \n", temp->num, temp->PassWord);
    }
    aw_kprintf("节点移动后拆分得到的新链表输出信息如下： \n");
    aw_list_for_each(pPos,&NewNodeCut.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d  PassWord= %d \n", temp->num, temp->PassWord);
    }

    /*
     * 链表合并，将单独的两个链表合并为一个新的链表
     */
    aw_list_for_each(pPos, &person_head.list) {
    temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 8) {

            /* 将两个链表合并为一个链表 */
            aw_list_splice_init(&NewNodeCut.list, pPos);
            break;
        }
    }

    aw_kprintf("两个链表合并后新的链表输出信息如下： \n");
    aw_list_for_each(pPos, &person_head.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d  PassWord= %d \n", temp->num, temp->PassWord);
    }
}

/** [src_list_two] */

/* end of file */
