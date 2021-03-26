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
 * \file
 * \brief doubly linked list library header.
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-23 mkr, first implementation.
 * \endinternal
 */

#ifndef __AW_TXFS_DLL_H
#define __AW_TXFS_DLL_H

/** \brief Node of a linked list. */
typedef struct _txfs_dlnode {
    /** \brief Points at the next node in the list */
    struct _txfs_dlnode *next;

    /** \brief Points at the previous node in the list */
    struct _txfs_dlnode *previous;
} _txfs_dl_node;

/** \brief Header for a linked list. */
typedef struct {
    _txfs_dl_node *head;  /**< \brief header of list */
    _txfs_dl_node *tail;  /**< \brief tail of list */
} _txfs_dl_list;

typedef _txfs_dl_node aw_txfs_dl_node;
typedef _txfs_dl_list aw_txfs_dl_list;

/**
 * \brief initialize doubly linked list descriptor
 *
 * Initialize the specified list to an empty list.
 */
#define AW_TXFS_DLL_INIT(list)                 \
{                                              \
    ((aw_txfs_dl_list *)(list))->head = NULL;  \
    ((aw_txfs_dl_list *)(list))->tail = NULL;  \
}

/**
 * \brief find first node in list
 *
 * \description
 * Finds the first node in a doubly linked list.
 *
 * \return Pointer to the first node in a list, or
 *         NULL if the list is empty.
 */
#define AW_TXFS_DLL_FIRST(pList)          \
(                                         \
    (((aw_txfs_dl_list *)(pList))->head)  \
)


/**
 * \brief find next node in list
 *
 * Locates the node immediately after the node pointed to by the pNode.
 *
 * \return Pointer to the next node in the list, or
 *         NULL if there is no next node.
 */
#define AW_TXFS_DLL_NEXT(pNode)                  \
(                                        \
    (((aw_txfs_dl_node *)(pNode))->next) \
)

/**
 * \brief insert node in list after specified node
 *
 * This macro inserts the specified node in the specified list.
 * The new node is placed following the specified 'previous' node in the list.
 * If the specified previous node is NULL, the node is inserted at the head
 * of the list.
 */
#define AW_TXFS_DLL_INSERT(list, previousNode, node)                           \
{                                                                              \
    aw_txfs_dl_node *temp;                                                     \
    if ((previousNode) == NULL)                                                \
        {                                                                      \
        temp = ((aw_txfs_dl_list *)(list))->head;                              \
        ((aw_txfs_dl_list *)(list))->head = (aw_txfs_dl_node *)(node);         \
        }                                                                      \
    else                                                                       \
        {                                                                      \
        temp = ((aw_txfs_dl_node *)(previousNode))->next;                      \
        ((aw_txfs_dl_node *)(previousNode))->next = (aw_txfs_dl_node *)(node); \
        }                                                                      \
    if (temp == NULL)                                                          \
        ((aw_txfs_dl_list *)(list))->tail = (aw_txfs_dl_node *)(node);         \
    else                                                                       \
       temp->previous = (aw_txfs_dl_node *)(node);                             \
    ((aw_txfs_dl_node *)(node))->next = temp;                                  \
    ((aw_txfs_dl_node *)(node))->previous = (aw_txfs_dl_node *)(previousNode); \
}

/**
 * \brief add node to end of list
 *
 * This macro adds the specified node to the end of the specified list.
 */
#define DLL_ADD(list, node)                                             \
{                                                                       \
    aw_txfs_dl_node *listTail = (list)->tail;                           \
    AW_TXFS_DLL_INSERT((list), listTail, (node));                       \
}

/**
 * \brief remove specified node in list
 *
 * Remove the specified node in the doubly linked list.
 */
#define AW_TXFS_DLL_REMOVE(list, node)                               \
{                                                                       \
    if (((aw_txfs_dl_node *)(node))->previous == NULL)                        \
        ((aw_txfs_dl_list *)(list))->head = ((aw_txfs_dl_node *)(node))->next; \
    else                                                                  \
        ((aw_txfs_dl_node *)(node))->previous->next = \
            ((aw_txfs_dl_node *)(node))->next;     \
    if (((aw_txfs_dl_node *)(node))->next == NULL)                         \
        ((aw_txfs_dl_list *)(list))->tail = \
            ((aw_txfs_dl_node *)(node))->previous;           \
    else                                                                 \
        ((aw_txfs_dl_node *)(node))->next->previous = \
            ((aw_txfs_dl_node *)(node))->previous; \
}

#endif /* __AW_TXFS_DLL_H */

/* end of file */

