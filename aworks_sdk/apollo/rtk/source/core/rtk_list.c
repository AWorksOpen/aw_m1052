/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file  rtk_list.c
 * \brief 实现rtk中的双向链表
 *
 * \internal
 * \par modification history:
 * - 1.00.00 18-07-19 sni, first version
 * \endinternal
 */
#include "rtk.h"
#include "rtk_mem.h"
#include "rtk_arch.h"
#include "rtk_task_internal.h"
#include "rtk_priority_q_internal.h"
#include "aw_errno.h"
#include "aw_common.h"
#include <string.h>

void rtk_list_node_init(rtk_list_node_t *list_node)
{
    list_node->next = list_node;
    list_node->prev = list_node;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static void __list_add(
        rtk_list_node_t *elm,
        rtk_list_node_t *prev,
        rtk_list_node_t *next)
{
    next->prev = elm;
    elm->next = next;
    elm->prev = prev;
    prev->next = elm;
}

void rtk_list_add(rtk_list_node_t *elm, rtk_list_node_t *head)
{
    __list_add(elm, head, head->next);
}

void rtk_list_add_tail(rtk_list_node_t *elm, rtk_list_node_t *head)
{
    __list_add(elm, head->prev, head);
}

int rtk_list_empty(const rtk_list_node_t *head)
{
    return head->next == head;
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static void __list_del(rtk_list_node_t * prev, rtk_list_node_t * next)
{
    next->prev = prev;
    prev->next = next;
}

void rtk_list_del_init(rtk_list_node_t *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = entry->prev = entry;
}

void rtk_list_del(rtk_list_node_t *entry)
{
    rtk_list_del_init(entry);
}

int rtk_list_is_last(const rtk_list_node_t *list,const rtk_list_node_t *head)
{
    return list->next == head;
}

