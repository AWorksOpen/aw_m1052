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
 * \brief 链表例程(约瑟夫环)
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口对应的宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 利用链表的基本服务完成约瑟夫环
 *
 * \par 源代码
 * \snippet demo_list_joseph.c src_list_joseph
 *
 * \internal
 * \par Modification history
 * - 1.00 14-08-13  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_list_joseph 链表(约瑟夫环)
 * \copydoc demo_list_joseph.c
 */

/** [src_list_joseph] */
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
 * \brief 利用链表的基本服务完成约瑟夫环
 * 函数描述：已知n个人（以编号1，2，3...n分别表示）围坐在一张圆桌周围，每一个人都有一个唯一的密码，
 *         编号为1的人将自己的密码说出后作为报数密码m，然后开始报数，数到m的那个人出列，
 *         并将自己的密码说出赋值给m然后被杀死；他的下一个人又从1开始报数，数到m的那个人又出列然后被杀死
 *         并以自己密码将m重新赋值，依此规律重复下去，直到圆桌周围的人全部出列并被杀死，求依次出列的顺序
 */
void demo_list_joseph_entry (void)
{
    _person_t person_head;

    _person_t *temp = NULL;
    struct aw_list_head *pPos    = NULL;    /* 储存遍历过程中被杀死的节点 */
    struct aw_list_head *pHead   = NULL;    /* 储存每次遍历过程中的头节点 */
    struct aw_list_head *pNext   = NULL;    /* 储存被杀死节点的下一个节点，记录使用 */

    uint_t i = 0;
    uint_t FlagNum = __g_ui_pass_word[0];   /* 记录每一个节点的密码信息 */
    AW_INIT_LIST_HEAD(&person_head.list);   /* 初始化一个头节点 */

    aw_kprintf("下面为约瑟夫环任务： \n");

    /* 将元素挨个插入链表 */
    for(i = 0; i < DataLength; i++) {

        __g_person[i].num      = __g_ui_num[i];
        __g_person[i].PassWord = __g_ui_pass_word[i];

        /* 将节点正序插入链表 */
        aw_list_add_tail(&(__g_person[i].list), &(person_head.list));
    }

    aw_kprintf("正序输出信息如下： \n");

    /* for循环进行遍历并正序输出 */
    aw_list_for_each(pPos, &person_head.list) {
        temp = aw_list_entry(pPos, _person_t, list);
        aw_kprintf(" num= %d PassWord= %d \n", temp->num, temp->PassWord);
    }

    pHead = &(person_head.list);    /* 保存每次遍历的头节点 */
    i = 1;                          /* 保存每次遍历所走的步数 */

    while (!aw_list_empty(&person_head.list)) {     /* 当所有节点被杀死后链表为空，停止遍历 */

        /*
         * for循环模仿aw_list_for_each_safe(pPos, n, q)，不过去掉了截止条件
         * 实现走相应的步数，找到要杀死的节点
         */
        for (pPos = pHead->next, pNext = pPos->next; ; pPos = pNext, pNext = pPos->next) {
            if ((i == FlagNum) && (pPos != &person_head.list)) {   /* 走了相应的步数后找到被杀死的节点 */
                break;
            } else if (pPos != &person_head.list) {     /* 遍历过的节点不包括头结点（没有数据） */
                i++;    /* 没有找到继续遍历，同时步数记录 */
            }
        }

        FlagNum = (aw_list_entry(pPos, _person_t, list))->PassWord; /* 将找到的杀死节点的密码作为新的要走的步数 */
        aw_list_del_init(pPos);                                     /* 将找到的节点杀死，重新组成链表 */
        pHead = pNext->prev;                                        /* 重新归置头节点 */
        i = 1;                                                      /* 步数积累重新赋值为1，进行下次遍历 */

        aw_kprintf(" 被杀死的节点编号为 %d \n", aw_list_entry(pPos, _person_t, list)->num);
    }
}

/** [src_list_joseph] */

/* end of file */
