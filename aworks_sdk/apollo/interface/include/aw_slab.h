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
 * \brief slab内存分配器。
 *
 * 使用本服务需要包含头文件 aw_slab.h
 *
 *  使用本服务，可以减少因为频繁申请，释放小内存而造成的内部碎片，
 *  但会降低内存使用率(有效荷载)。
 *  为了省去用户熟悉配置接口aw_slab_catenate的参数，提供接口
 *  aw_slab_easy_catenate()，只需要提供要管理的内存地址和大小即可，
 *  而且可以多次调用，达到管理多块内存的效果。
 *
 * \par 初始化slab分配器
 * \code
 *
 * static struct aw_slab foo;
 *
 * static void init_aw_slab(void)
 * {
 *     aw_slab_init(&foo);
 * } 
 * \endcode 
 *  
 * \par 往slab分配器添加内存
 * \code
 *
 * #define TRUNK_SIZE 1048576
 *  
 * static void add_trunk(void)
 * {
 *      void *p_trunk;
 *
 *      p_trunk = aw_mem_alloc(TRUNK_SIZE);
 *
 *      aw_slab_easy_catenate(&foo, p_trunk, TRUNK_SIZE); 
 * }
 * \endcode
 * 
 * \par 分配和释放内存
 * \code
 * int test_aw_slab(void)
 * {
 *     void *object;
 *
 *     object = aw_slab_malloc(&foo, 1024);
 *
 *     if(object) {
 *          aw_slab_free(&foo, object);    
 *     }  
 * }
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.01 17-03-09  imp, add aw_slab_memalign()
 * - 1.00 17-03-03  imp, first implementation
 * \endinternal
 */

#ifndef __AW_SLAB_H
#define __AW_SLAB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "aw_types.h"
#include "aw_errno.h"
#include "aw_list.h"

/**
 * \addtogroup grp_aw_if_slab
 * \copydoc aw_slab.h
 * @{
 */

/** \brief 开启钩子。 */
#define AW_SLAB_USING_HOOK  

/** \brief 记录内存分配情况。 */
#define AW_SLAB_MEM_STATS   

/** \brief zone个数的最大值，最大是72。 */
#define AW_SLAB_NZONES_MAX      72   


/**
 * \brief aw_slab结构体
 */
struct aw_slab {

#ifdef AW_SLAB_MEM_STATS

    /** \brief 当前分配出去的内存. */
    size_t used_mem;

    /** \brief 最大分配出去的内存. */
    size_t max_mem;   
#endif

#ifdef AW_SLAB_USING_HOOK

    /** 
     * \brief 分配内存时使用的钩子函数。 
     *
     * 需要定义宏 AW_SLAB_USING_HOOK 。
     *
     * \param[in] ptr   分配出来的内存地址
     * \param[in] size  分配的大小
     */
    void (*pfn_malloc_hook)(void *ptr, size_t size); 

    /** 
     * \brief 回收内存时使用的钩子函数。   
     *
     * 需要定义宏 AW_SLAB_USING_HOOK 。
     *
     * \param[in] ptr  回收的内存地址
     */
    void (*pfn_free_hook)(void *ptr);                
#endif

    /** \brief 堆内存入口. */
    struct aw_list_head heap;   
};

/**
 * \brief 初始化slab分配器。
 *
 * \param[in,out] p_slab   slab分配器
 */
extern
void aw_slab_init (struct aw_slab *p_slab);


/**
 * \brief 这个函数根据传入的参数配置该内存的属性。
 *
 * \param[in,out] p_slab       slab分配器
 * \param[in] begin_addr       要给aw_slab管理的内存的起始地址
 * \param[in] end_addr         要给aw_slab管理的内存的结束地址
 * \param[in] page_bits        页面大小，由1左移page_bits获得
 * \param[in] thresh           空闲zone的个数阈值
 * \param[in] zone_min_sz      aw_slab的管理对象zone，所占内存的最小值
 * \param[in] zone_max_sz      aw_slab的管理对象zone，所占内存的最大值
 * \param[in] zone_max_limit   由aw_slab所能管理的内存的最大值
 *                             若申请的大小比最大值还大，则不通过aw_slab管理
 *
 * \retval  AW_OK       成功
 * \retval -AW_EINVAL   输入的参数无法将内存链接到aw_slab
 */
extern
aw_err_t  aw_slab_heap_catenate (struct aw_slab *p_slab,
                                 void           *begin_addr, 
                                 void           *end_addr,
                                 uint8_t         page_bits,
                                 uint8_t         thresh,
                                 uint32_t        zone_min_sz,
                                 uint32_t        zone_max_sz,
                                 uint32_t        zone_max_limit);

/**
 * \brief 这个函数根据 \e mem_size 选择合适的参数配置该内存的分配时。
 *
 * \param[in,out] p_slab    slab分配器
 * \param[in] p_mem        内存空间的起始地址
 * \param[in] mem_size     内存空间的大小
 *
 * \retval  AW_OK          成功
 * \retval -AW_EINVAL      参数无效
 */
extern
aw_err_t  aw_slab_easy_catenate(struct aw_slab *p_slab, 
                                char           *p_mem, 
                                uint32_t        mem_size);


/**
 * \brief 这个函数会分配出满足 \e size 字节的内存给申请者。
 *
 * \param[in] p_slab     slab分配器
 * \param[in] size       申请的内存大小
 *
 * \return 若成功分配，则返回该内存地址；否则，返回NULL。
 */
extern
void *aw_slab_malloc(struct aw_slab *p_slab, size_t size);


/**
 * \brief 这个函数会重新分配出满足 \e size 字节大小的内存。
 *
 * \param[in] p_slab     slab分配器
 * \param[in] ptr        需要重新分配的内存空间
 * \param[in] size       重新分配的大小
 *
 * \retval   NULL        内存不足或者输入参数size为0
 * \retval   内存地址    分配成功
 *
 * \note 当返回不是NULL时，不应该再使用 \e ptr 存放的地址。
 */
extern
void *aw_slab_realloc(struct aw_slab *p_slab, void *ptr, size_t size);


/**
 * \brief 这个函数会分配连续的 \e count 个，大小为 \e size 字节的内存给申请者。
 *
 * \param[in] p_slab     slab分配器
 * \param[in] count      申请的个数
 * \param[in] size       申请的大小
 *
 * \retval   NULL        内存不足或者参数无效
 * \retval   内存地址    分配成功
 */
extern
void *aw_slab_calloc(struct aw_slab *p_slab, size_t count, size_t size);

/**
 * \brief 这个函数将 \e ptr 指向的内存返回给 \e p_slab 管理的堆里。
 *
 * \param[in] p_slab     slab分配器
 * \param[in] ptr        回收的内存
 *
 */
extern
void  aw_slab_free(struct aw_slab *p_slab, void *ptr);


/**
 * \brief 这个函数会寻找对齐 \e alignment 字节的内存，然后分配给申请者。
 *
 * 参数 \e alignment 需满足是2的幂的要求，当不满足或者内存不足时，
 * \e pp_memptr 会返回NULL。
 *       
 * \param[in]     p_slab       slab分配器
 * \param[in,out] pp_memptr    存放返回地址
 * \param[in]     alignment    对齐的字节
 * \param[in]     size         申请的大小
 *
 * \retval  AW_OK              操作成功
 * \retval -AW_ENOMEM          内存不足
 */
extern 
aw_err_t aw_slab_memalign(struct aw_slab *p_slab, 
                          void          **pp_memptr, 
                          size_t          alignment, 
                          size_t          size);


#ifdef AW_SLAB_MEM_STATS

/**
 * \brief 这个函数会返回内存使用情况。
 *
 * 需要定义宏 AW_SLAB_MEM_STATS 。
 *
 * \param[in]     p_slab        slab分配器
 * \param[in,out] p_total       总共拥有的内存
 * \param[in,out] p_used        当前使用量
 * \param[in,out] p_max_used    内存分配的峰值
 *
 * \retval  AW_OK       操作成功
 * \retval -AW_ENOMEM   内存不足
 */
extern
aw_err_t  aw_slab_memory_info (struct aw_slab *p_slab, 
                               uint32_t       *p_total, 
                               uint32_t       *p_used, 
                               uint32_t       *p_max_used);
#endif


#ifdef AW_SLAB_USING_HOOK

/**
 * \brief 这个函数会设置一个钩子函数，当动态内存分配成功时调用。
 *
 * 需要定义宏 AW_SLAB_USING_HOOK 。
 *
 * \param[in] p_slab    slab分配器
 * \param[in] pfn_hook  钩子函数
 *
 */
extern
void aw_slab_malloc_sethook(struct aw_slab *p_slab,
                            void          (*pfn_hook)(void *ptr, size_t size));

/**
 * \brief 这个函数会设置一个钩子函数，当内存回收时调用。
 *
 * 需要定义宏 AW_SLAB_USING_HOOK 。
 *
 * \param[in] p_slab    slab分配器
 * \param[in] pfn_hook  钩子函数
 *
 */
extern
void aw_slab_free_sethook(struct aw_slab *p_slab,
                          void          (*pfn_hook)(void *ptr));

#endif

/** @}  grp_aw_if_slab */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SLAB_H */

/* end of file */
