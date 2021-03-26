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
 * \brief 链表例程（增删改查）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口对应的宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 打印出链表基础操作的过程：增加节点、删除节点、查找节点和修改节点
 *
 * \par 源代码
 * \snippet demo_list_base.c src_list_base
 *
 * \internal
 * \par Modification history
 * - 1.00 14-08-13  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_list_base 链表基础操作
 * \copydoc demo_list_base.c
 */

/** [src_list_base] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_list.h"

/**
 * \brief 结构体类型和数据定义
 */
typedef uint_t _element_type;

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
static _person_t    NewNodeAdd = {9,6};         /* 链表添加节点使用 */
static _person_t    NewNodeRep = {10,5};        /* 链表替换节点使用 */

static _person_t           *temp  = NULL;       /* 储存暂时信息，遍历使用 */
static struct aw_list_head *pPos  = NULL;
static struct aw_list_head *pNext = NULL;

/**
 * \brief 将元素挨个插入链表
 * person_head 链表的头节点
 */
static void __list_add_node (_person_t *p_person_head)
{
    uint_t i = 0;

    for (i = 0; i< DataLength; i++) {
        __g_person[i].num       = __g_ui_num[i];
        __g_person[i].PassWord  = __g_ui_pass_word[i];
        AW_INIT_LIST_HEAD(&__g_person[i].list);

        /* 将节点正序插入链表 */
        aw_list_add_tail(&(__g_person[i].list ), &(p_person_head->list));
    }
}

static void __list_print_node (_person_t *p_person_head)
{
    _person_t           *temp = NULL;   /* 储存暂时信息，遍历使用 */
    struct aw_list_head *pPos = NULL;

    aw_kprintf("正序输出信息如下： \n");

    /* for循环进行遍历并正序输出 */
    aw_list_for_each(pPos, &p_person_head->list) {
        /*
         * pPos保存每个结构体的链表指针
         * 根据pPos指针和偏移量得到指向结构体元素的指针temp
         */
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
    }
}

/**
 * \brief 查找特定元素节点并将此节点删除
 * person_head 链表的头节点
 */
static void __list_delete_node (_person_t *p_person_head)
{
    /* pNext为标记指针,方便删除后继续遍历 */
    aw_list_for_each_safe(pPos, pNext, &p_person_head->list) {
        temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 3) {
            /* 将元素删除并将删除后的节点初始化为头节点 */
            aw_list_del_init(pPos);
            break;
        }
    }

    /* 被删除的节点信息 */
    aw_kprintf("删除节点的信息如下： \n");
    aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
}

/**
 * \brief 查找特定元素节点，并将一个新节点插入此节点之后
 * person_head 链表的头节点
 */
static void __list_search_add_node (_person_t *p_person_head)
{
    aw_list_for_each(pPos, &p_person_head->list) {
        temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 5) {
            /* 将新节点加入链表 */
            aw_list_add(&NewNodeAdd.list, pPos);
            break;
        }
    }

    /* 新加入的节点信息 */
    aw_kprintf("新加入的节点的信息如下： \n");
    aw_kprintf(" num= %d PassWord= %d \n", NewNodeAdd.num, NewNodeAdd.PassWord);
}

/**
* \brief 查找特定元素节点，并将一个新节点替代此节点
* person_head 链表的头节点
*/
static void __list_search_replace_node (_person_t *p_person_head)
{
    aw_list_for_each(pPos, &p_person_head->list) {
        temp = aw_list_entry(pPos, _person_t, list);
        if (temp->num == 2) {
            /* 将新节点替代原有节点并加入链表 */
            aw_list_replace_init(pPos,&NewNodeRep.list);
            break;
        }
    }

    aw_kprintf("替代的节点的信息如下： \n");    /* 新加入的节点信息 */
    aw_kprintf(" num= %d PassWord= %d \n", NewNodeRep.num, NewNodeRep.PassWord);
    aw_kprintf("被替代的节点的信息如下： \n");  /* 被替代的节点信息 */
    aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
}

/******************************************************************************/
/**
 * \brief 链表的基本操作(增删改查)入口函数
 */
void demo_list_base_entry (void)
{
    _person_t person_head;                      /* 定义一个双向链表头节点 */
    AW_INIT_LIST_HEAD(&person_head.list);       /* 初始化一个头节点 */

    aw_kprintf("链表基本服务如下： \n");

    aw_kprintf("\n\n添加链表节点信息： \n");
    __list_add_node(&person_head);              /* 将元素挨个插入链表 */
    __list_print_node(&person_head);            /* 打印链表的节点信息 */

    aw_kprintf("\n\n删除链表节点信息： \n");
    __list_delete_node(&person_head);           /* 查找特定元素节点并将此节点删除 */
    __list_print_node(&person_head);            /* 打印链表的节点信息 */

    aw_kprintf("\n\n查找特定元素节点，并将一个新节点插入此节点之后： \n");
    __list_search_add_node(&person_head);       /* 查找特定元素节点，并将一个新节点插入此节点之后 */
    __list_print_node(&person_head);            /* 打印链表的节点信息 */

    aw_kprintf("\n\n查找特定元素节点，并将一个新节点替代此节点： \n");
    __list_search_replace_node(&person_head);   /* 查找特定元素节点，并将一个新节点替代此节点 */
    __list_print_node(&person_head);            /* 打印链表的节点信息 */

}

/** [src_list_base] */

/* end of file */
