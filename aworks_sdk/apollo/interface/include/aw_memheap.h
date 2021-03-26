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
 * \brief 堆管理器
 *
 * 使用本服务需要包含头文件 aw_memheap.h
 *
 * \par 初始化内存堆
 * \code
 * 
 * aw_memheap_t my_heap;  
 *
 * static void memheap_init(void)
 * {
 *      aw_memheap_init(&my_heap,
 *                      "my_heap",   // 内存堆的名字
 *                       MEM_ADDR,   // 可用的地址空间
 *                       MEM_SIZE);  // 地址空间大小
 * }
 * \endcode
 *
 * \par 申请、释放内存
 * \code
 *
 * static void memheap_init(void)
 * {
 *      void *ptr = aw_memheap_alloc(&my_heap, 32);
 *     
 *      aw_memheap_free(ptr);    
 * }
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-06-13  zen, first impementation
 * \endinternal
 */

#ifndef __AW_MEMHEAP_H
#define __AW_MEMHEAP_H

#include "aworks.h"
#include "aw_sem.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_memheap
 * \copydoc aw_memheap.h
 * @{
 */

/** \brief 内存堆的内存单元的定义 */
struct aw_memheap_item {

    /** \brief 内存单元的魔数，保护写越界 */
    uint32_t                magic;    

    /** \brief 堆内存的指针 */                  
    struct aw_memheap      *pool_ptr;                   

     /** \brief 下一个内存单元 */
    struct aw_memheap_item *next;        

    /** \brief 前一个内存单元 */              
    struct aw_memheap_item *prev;                       

    /** \brief 下一个空闲的内存单元 */
    struct aw_memheap_item *next_free;    

    /** \brief 前一个空闲的内存单元 */              
    struct aw_memheap_item *prev_free;     

};

/** \brief 内存堆的内存单元 */
typedef struct aw_memheap_item aw_memheap_item_t;

/**
 * \brief 内存堆的定义
 *
 * \note 用户不应该修改结构体的成员。
 */
struct aw_memheap {

    /** \brief 堆的名字 */
    const char             *name;    

    /** \brief 堆内存的起始地址 */           
    void                   *start_addr;         

    /** \brief 堆内存的大小 */
    uint32_t                pool_size;   

    /** \brief 可用大小 */       
    uint32_t                available_size;     

    /** \brief 分配的最大值 */
    uint32_t                max_used_size;      

    /** \brief 当前使用的单元链表 */
    struct aw_memheap_item *block_list;         

    /** \brief 空闲单元链表 */
    struct aw_memheap_item *free_list;         

    /** \brief 空闲块的链表头 */ 
    struct aw_memheap_item  free_header;        

    /** \brief 互斥锁 */
    AW_MUTEX_DECL(lock);     
};

/** \brief 内存堆 */
typedef struct aw_memheap aw_memheap_t;

/**
 * \brief 初始化内存堆
 *
 * \param[in,out]  memheap     堆内存使用的结构体
 * \param[in]      name        堆内存的名字
 * \param[in]      start_addr  堆内存的起始地址
 * \param[in]      size        堆内存的大小
 *
 * \retval AW_OK 初始化成功
 */
aw_err_t aw_memheap_init(struct aw_memheap *memheap,
                         const char        *name,
                         void              *start_addr,
                         uint32_t           size);


/**
 * \brief 从内存堆分配内存
 *
 * \param[in]  heap   目标内存堆
 * \param[in]  size   申请内存的大小
 *
 * \return 若成功，返回分配得到的内存地址；若失败，返回NULL
 */
void *aw_memheap_alloc(struct aw_memheap *heap, uint32_t size);


/**
 * \brief 检查内存的大小
 *
 * \param[in]  heap   目标内存堆
 * \param[in]  ptr    内存地址
 *
 * \return 返回 \e ptr 所指向的内存空间大小 
 */
size_t aw_memheap_memsize(struct aw_memheap *heap, void *ptr);


/**
 * \brief 改变已分配内存空间的大小
 *
 * \param[in]  heap      目标内存堆
 * \param[in]  ptr       内存地址
 * \param[in]  newsize   重新分配的大小
 *
 * \return 成功时，返回满足大小要求的内存空间地址；
 *         失败时，返回NULL。
 */
void *aw_memheap_realloc(struct aw_memheap *heap, void *ptr, size_t newsize);


/**
 * \brief 释放内存
 *
 * \param[in]  ptr 从内存堆分配得到的内存地址
 */
void aw_memheap_free(void *ptr);

/** @}  grp_aw_if_memheap */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif /* __AW_MEMHEAP_H    */

/* end of file */
