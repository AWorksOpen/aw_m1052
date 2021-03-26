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
 * \brief pbuf implementation
 *
 * \internal
 * \par Modification history
 * - 1.00 16-03-07  bob, first implementation.
 * \endinternal
 */

#include "aw_pbuf.h"
#include "aw_assert.h"
#include "aw_spinlock.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_mem.h"


#define AW_PBUF_ASSERT  /**< \brief 开启断言 */

/**
 * \brief pbuf 内存长度对齐（__PBUF_ALIGN_SIZE(7, 4) = 8）
 */
#define __PBUF_ALIGN_SIZE(size, align_size) \
            (((size) + align_size - 1) & ~(align_size-1))

/**
 * \brief pbuf 内存地址对齐
 */
#define __PBUF_ALIGN_ADDR(addr, align_size)  \
            ((void *)(((uint32_t)(addr) + align_size - 1) & ~(uint32_t)(align_size-1)))

/**
 * \brief pbuf 选择两个数字中较小的
 */
#define __PBUF_MIN(x , y)  (((x) < (y)) ? (x) : (y))

/**
 * \brief pbuf 数据长度对齐（__PBUF_ALIGN_SIZE(7, 4) = 4）
 */
#define __PBUF_ALIGN_DATA_LEN(size, align_size) ((size) & ~(align_size-1))

/**
 * \brief pbuf组件对象管理结构体
 */
static struct aw_pbuf_pool_manage {

    struct aw_list_head    head;
    AW_MUTEX_DECL(         mutex);
} __g_manage, *__gp_manage = &__g_manage; /** \brief 管理对象与其指针 */


/*******************************************************************************
* Functions declaration
*******************************************************************************/

static void __pbuf_memcpy(void *p_dst, void *p_src, size_t len)
{
    memcpy(p_dst, p_src, len);
    return ;
}

/******************************************************************************/
static aw_err_t __pbuf_pool_find(struct aw_pbuf_pool *p_pbuf_pool)
{
    struct aw_pbuf_pool *p_tmp_pbuf_pool = NULL;

    aw_list_for_each_entry(p_tmp_pbuf_pool,
                          &(__gp_manage->head),
                           struct aw_pbuf_pool,
                           node) {
        if (p_tmp_pbuf_pool == p_pbuf_pool) {
            return AW_OK;
        }
    }

    return AW_ERROR;
}

/******************************************************************************/
static aw_err_t __pbuf_pool_insert(struct aw_pbuf_pool *p_pbuf_pool)
{
    AW_MUTEX_LOCK(__gp_manage->mutex, AW_SEM_WAIT_FOREVER);
    aw_list_add (&p_pbuf_pool->node , &(__gp_manage->head));
    AW_MUTEX_UNLOCK(__gp_manage->mutex);
    return AW_OK;
}

/******************************************************************************/
void aw_pbuf_init(void)
{
    AW_INFOF(("pbuf init! \r\n"));

    AW_MUTEX_INIT(__gp_manage->mutex, AW_SEM_Q_PRIORITY);
    aw_list_head_init(&__gp_manage->head);
}

/******************************************************************************/
aw_err_t aw_pbuf_pool_init (struct aw_pbuf_pool *p_pbuf_pool,
                            void                *p_ram_mem,
                            size_t               ram_mem_size,

                            void                *p_romref_mem,
                            size_t               romref_mem_size,

                            size_t               ram_size,
                            size_t               align_size)
{
    aw_pool_id_t tmp_pool_id;
#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_pbuf_pool);
#endif

    if (NULL == p_pbuf_pool && align_size <= 0) {

        return -AW_EINVAL;
    }

    if (AW_OK == __pbuf_pool_find(p_pbuf_pool)) {
        return -AW_EBUSY;
    }


    /* 初始化POOL类型PBUF内存池 */
    if (NULL != p_ram_mem && ram_mem_size > 0) {
        tmp_pool_id = aw_pool_init(&p_pbuf_pool->pool_pbuf_pool,
                                    p_ram_mem,
                                    ram_mem_size,

                                    ram_size               +
                                    sizeof(struct aw_pbuf));

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != tmp_pool_id);
#endif
    }

    /* 初始化REF/ROM类型PBUF内存池 */
    if (NULL != p_romref_mem && romref_mem_size > 0) {
        tmp_pool_id = aw_pool_init(&p_pbuf_pool->rom_ref_pbuf_pool,
                                    p_romref_mem,
                                    romref_mem_size,
                                    sizeof(struct aw_pbuf));

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != tmp_pool_id);
#endif

    }

    (void)tmp_pool_id;

    /* 记录POOL类型PBUF数据缓冲区大小 */
    p_pbuf_pool->pool_pbuf_item_size = ram_size;

    /* 记录PBUF内存对齐宽度 */
    p_pbuf_pool->pbuf_align_size  = align_size;

    __pbuf_pool_insert(p_pbuf_pool);

    return AW_OK;
}

/******************************************************************************/
void aw_pbuf_pool_uninit (struct aw_pbuf_pool *p_pbuf_pool)
{
    struct aw_pbuf_pool *p_tmp_pbuf_pool = NULL;

    aw_list_for_each_entry(p_tmp_pbuf_pool,
                          &(__gp_manage->head),
                           struct aw_pbuf_pool,
                           node) {
        if (p_tmp_pbuf_pool == p_pbuf_pool) {
            AW_MUTEX_LOCK(__gp_manage->mutex, AW_SEM_WAIT_FOREVER);
            __aw_list_del_entry(&p_pbuf_pool->node);
            AW_MUTEX_UNLOCK(__gp_manage->mutex);
        }
    }
}

/******************************************************************************/
struct aw_pbuf *aw_pbuf_alloc (struct aw_pbuf_pool *p_pbuf_pool,
                               aw_pbuf_type_t       type,
                               size_t               size,
                               void                *p_mem)
{
    struct aw_pbuf      *p_q = NULL, *p_p = NULL, *p_r = NULL;
    size_t               len;
#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_pbuf_pool);
#endif

    if (NULL == p_pbuf_pool) {
        AW_ERRNO_SET(-AW_EINVAL);
        return NULL;
    }

    switch (type) {

    /* RAM类型PBUF内存分配（PBUF头和数据缓冲区在内存堆里面分配） */
    case AW_PBUF_TYPE_RAM:

        p_q = (struct aw_pbuf *)aw_mem_align(__PBUF_ALIGN_SIZE(sizeof(struct aw_pbuf), 
                                             p_pbuf_pool->pbuf_align_size) +
                                             __PBUF_ALIGN_SIZE(size,p_pbuf_pool->pbuf_align_size),

                                             p_pbuf_pool->pbuf_align_size);

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_q);
#endif
        if (NULL == p_q) {
            AW_INFOF(("pbuf : heap is empty, pbuf alloc failed! \r\n"));
            AW_ERRNO_SET(-AW_ENOMEM);
            return NULL;
        }

        p_q->p_pbuf_pool = p_pbuf_pool;
        p_q->p_payload   = __PBUF_ALIGN_ADDR((void *)((char *)(p_q) + sizeof(struct aw_pbuf)),
                           p_pbuf_pool->pbuf_align_size);
        p_q->tot_len   = size;
        p_q->len       = size;
        p_q->type      = type;
        p_q->ref       = 1;
        p_q->p_next    = NULL;

        AW_MUTEX_INIT(p_q->mutex, AW_SEM_Q_PRIORITY);

        break;

    /* POOL类型PBUF内存分配（PBUF头和数据缓冲区在内存池里面分配） */
    case AW_PBUF_TYPE_POOL:

        p_q = (struct aw_pbuf *)aw_pool_item_get(&p_pbuf_pool->pool_pbuf_pool);

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_q);
#endif
        if (NULL == p_q) {
            AW_INFOF(("pbuf : pool type pbuf pool is empty! pbuf alloc failed!\r\n"));
            AW_ERRNO_SET(-AW_ENOMEM);
            return NULL;
        }

        p_q->p_next      = NULL;
        p_q->p_pbuf_pool = p_pbuf_pool;
        p_q->p_payload   = __PBUF_ALIGN_ADDR((void *)((char *)(p_q) + sizeof(struct aw_pbuf)),
                                               p_pbuf_pool->pbuf_align_size);

        p_q->tot_len     = size;



        len = __PBUF_ALIGN_DATA_LEN( p_pbuf_pool->pool_pbuf_item_size -

                                    ((uint8_t *)p_q->p_payload - (((uint8_t *)p_q) +
                                     sizeof(struct aw_pbuf))),


                                    p_pbuf_pool->pbuf_align_size);

        p_q->len = __PBUF_MIN(size, len);

        size -= p_q->len;

        p_q->type      = type;
        p_q->ref       = 1;
        p_q->p_next    = NULL;
        AW_MUTEX_INIT(p_q->mutex, AW_SEM_Q_PRIORITY);

        /* 如果分配数据多就分配多个PBUF链成一个PBUF链 */
        p_r = p_q;
        while (size > 0) {
            p_p = (struct aw_pbuf *)aw_pool_item_get(&p_pbuf_pool->pool_pbuf_pool);

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_p);
#endif
            if (NULL == p_p) {
                AW_INFOF(("pbuf : pool type pbuf pool is empty! pbuf alloc failed! \r\n"));
                AW_ERRNO_SET(-AW_ENOMEM);
                return NULL;
            }
            p_p->p_next = NULL;
            p_p->p_pbuf_pool = p_pbuf_pool;
            p_p->p_payload = __PBUF_ALIGN_ADDR((void *)((char *)(p_p) + sizeof(struct aw_pbuf)),
                                                 p_pbuf_pool->pbuf_align_size);

            p_p->tot_len   = size;

            len = __PBUF_ALIGN_DATA_LEN(p_pbuf_pool->pool_pbuf_item_size -

                                        ((uint8_t *)p_p->p_payload - (((uint8_t *)p_p) +
                                         sizeof(struct aw_pbuf))),

                                        p_pbuf_pool->pbuf_align_size);


            p_p->len = __PBUF_MIN(size, len);

            size -= p_p->len;

            p_p->type    = type;
            p_p->ref     = 1;
            AW_MUTEX_INIT(p_p->mutex, AW_SEM_Q_PRIORITY);

            p_r->p_next  = p_p;
            p_r          = p_p;
        }

        break;

    /* ROM/REF类型PBUF内存分配（PBUF头在内存池分配，数据缓冲区由用户分配） */
    case AW_PBUF_TYPE_ROM:
    case AW_PBUF_TYPE_REF:

        if (NULL == p_mem) {
            AW_ERRNO_SET(-AW_EINVAL);
            return NULL;
        }

        p_q = (struct aw_pbuf *)aw_pool_item_get(&p_pbuf_pool->rom_ref_pbuf_pool);

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_q);
#endif
        if (NULL == p_q) {
            AW_INFOF(("pbuf : rom/ref type pbuf pool is empty! pbuf alloc failed! \r\n"));
            AW_ERRNO_SET(-AW_ENOMEM);
            return NULL;
        }

        p_q->p_pbuf_pool  = p_pbuf_pool;

        p_q->p_payload = p_mem;
        p_q->tot_len   = size;
        p_q->len       = size;
        p_q->type      = type;
        p_q->ref       = 1;
        AW_MUTEX_INIT(p_q->mutex, AW_SEM_Q_PRIORITY);

        p_q->p_next    = NULL;
        break;

    default :

#ifdef AW_PBUF_ASSERT
    aw_assert(0);
#endif
    AW_ERRNO_SET(-AW_EPERM);
    AW_INFOF(("pbuf : error type pbuf! pbuf alloc failed! \r\n"));
        break;
    }

    return p_q;
}

/******************************************************************************/
struct aw_pbuf *aw_pbuf_alloced_custom (struct aw_pbuf_custom    *p_pbuf,
                                        void                     *p_mem,
                                        size_t                    size,

                                        aw_pbuf_custom_callback_t pfn_app_free,
                                        void                     *p_arg)
{
    if (NULL == p_pbuf         ||
        NULL == pfn_app_free) {

        AW_ERRNO_SET(-AW_EINVAL);
        return NULL;
    }

    p_pbuf->pfn_app_free      = pfn_app_free;
    p_pbuf->p_arg             = p_arg;

    p_pbuf->pbuf.p_next    = NULL;
    p_pbuf->pbuf.p_payload = p_mem;
    p_pbuf->pbuf.len       = size;
    p_pbuf->pbuf.tot_len   = size;
    p_pbuf->pbuf.flags     = AW_PBUF_FLAGS_CUSTOM;
    p_pbuf->pbuf.ref       = 1;
    AW_MUTEX_INIT(p_pbuf->pbuf.mutex, AW_SEM_Q_PRIORITY);

    return &p_pbuf->pbuf;
}

/******************************************************************************/
aw_err_t aw_pbuf_realloc (struct aw_pbuf *p_pbuf, size_t size)
{
    int16_t        len, gorw;
    struct aw_pbuf *p_tmp_pbuf;

#ifdef AW_PBUF_ASSERT
    aw_assert((NULL != p_pbuf) && (size < p_pbuf->tot_len));
#endif

    /* 如果size >= p_pbuf->tot_len，返回，接口支持缩减 */
    if (NULL == p_pbuf) {
        return -AW_EINVAL;
    }

    if (size >= p_pbuf->tot_len) {
        return -AW_ENOTSUP;
    }

    len        = size;
    p_tmp_pbuf = p_pbuf;
    gorw       = size - p_pbuf->tot_len;

    AW_MUTEX_LOCK(p_pbuf->mutex, AW_SEM_WAIT_FOREVER);

    while (len > p_tmp_pbuf->len) {
        len -= p_tmp_pbuf->len;
        p_tmp_pbuf->tot_len += gorw;
        p_tmp_pbuf = p_tmp_pbuf->p_next;
    }

    /* AW_PBUF_TYPE_RAM需要重新调整，其他类型不需要操作 */

    if (AW_PBUF_TYPE_RAM == p_tmp_pbuf->type) {
        p_tmp_pbuf = aw_mem_realloc(p_tmp_pbuf,

                                    (uint16_t)((uint8_t *)p_tmp_pbuf->p_payload -
                                               (uint8_t *)p_tmp_pbuf ) +
                                               len);
#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_tmp_pbuf->p_payload);
#endif

    }

    p_tmp_pbuf->len     = len;
    p_tmp_pbuf->tot_len = len;

    AW_MUTEX_UNLOCK(p_pbuf->mutex);

    /* 如果有多余的PBUF,从链中删除 */
    if (p_tmp_pbuf->p_next != NULL) {
         /* 释放掉多余的 */
        aw_pbuf_free(p_tmp_pbuf->p_next);
    }
    /* 链最后一个PBUF指向空 */
    p_tmp_pbuf->p_next = NULL;

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_pbuf_header (struct aw_pbuf *p_pbuf, ssize_t header_size)
{
    uint16_t grow;

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_pbuf);
#endif

    if (NULL == p_pbuf) {
        return -AW_EINVAL;
    }

    if (header_size < 0) {
        grow = -header_size;
    }

    if (p_pbuf->len + header_size < 0) {
        return AW_ERROR;
    }

    AW_MUTEX_LOCK(p_pbuf->mutex, AW_SEM_WAIT_FOREVER);

    if (p_pbuf->type == AW_PBUF_TYPE_POOL ||
        p_pbuf->type == AW_PBUF_TYPE_RAM) {

        if (((uint8_t *)p_pbuf->p_payload - header_size) <
            ((uint8_t *)p_pbuf +
            sizeof(struct aw_pbuf))) {
            AW_MUTEX_UNLOCK(p_pbuf->mutex);

            return AW_ERROR;
        } else {
            /* 如果调整之后PBUF缓冲区与PBUF头冲突 */
            p_pbuf->p_payload = ((uint8_t *)p_pbuf->p_payload - header_size);
        }

    } else  if (p_pbuf->type == AW_PBUF_TYPE_REF ||
                p_pbuf->type == AW_PBUF_TYPE_ROM) {

        /* 如果调整之后PBUF缓冲区与PBUF头不冲突 */
        if (header_size < 0 && grow <= p_pbuf->len) {
            p_pbuf->p_payload = ((uint8_t *)p_pbuf->p_payload - header_size);
        } else {
            AW_MUTEX_UNLOCK(p_pbuf->mutex);
            return AW_ERROR;
        }

    } else {
#ifdef AW_PBUF_ASSERT
    AW_INFOF(("pbuf : error type pbuf! pbuf header failed! \r\n"));
    aw_assert(0);
#endif
    }

    AW_MUTEX_UNLOCK(p_pbuf->mutex);

    /* 更新PBUF缓冲区长度数据 */
    p_pbuf->len     += header_size;
    p_pbuf->tot_len += header_size;

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_pbuf_ref(struct aw_pbuf *p_pbuf)
{

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_pbuf);
#endif

    if (NULL == p_pbuf) {
        return -AW_EINVAL;
    }


    AW_MUTEX_LOCK(p_pbuf->mutex, AW_SEM_WAIT_FOREVER);

    /* 增加PBUF的引用计数 */
    if (NULL != p_pbuf) {
        p_pbuf->ref++;
    }

    AW_MUTEX_UNLOCK(p_pbuf->mutex);

    return AW_OK;
}

/******************************************************************************/
size_t aw_pbuf_free (struct aw_pbuf *p_pbuf)
{
    size_t          len = 0;
    aw_err_t        ret;
    struct aw_pbuf *p_q = p_pbuf, *p_p = NULL;
    struct aw_pbuf_custom *p_pbuf_custom = NULL;
    AW_INT_CPU_LOCK_DECL(s_old_stat);

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_pbuf);
#endif

    if (NULL == p_pbuf) {
        return len;
    }

    AW_MUTEX_LOCK(p_pbuf->mutex, AW_SEM_WAIT_FOREVER);
    while(NULL != p_q) {

        /* 减首链PBUF的引用计数 */
        AW_INT_CPU_LOCK(s_old_stat);
        p_q->ref--;
        AW_INT_CPU_UNLOCK(s_old_stat);

        /* 如果PBUF的引用计数为0，表明这个PBUF已经没有用户在使用，释放 */
        if (p_q->ref == 0) {

            p_p = p_q->p_next;

            if (p_q->flags & AW_PBUF_FLAGS_CUSTOM) {
                p_pbuf_custom = (struct aw_pbuf_custom *)p_q;
                if (p_pbuf_custom->pfn_app_free) {
                    p_pbuf_custom->pfn_app_free(&p_pbuf_custom->pbuf,
                                                 p_pbuf_custom->p_arg);
                }
            } else if (p_q->type == AW_PBUF_TYPE_ROM ||
                       p_q->type == AW_PBUF_TYPE_REF) {

                ret = aw_pool_item_return(&p_q->p_pbuf_pool->rom_ref_pbuf_pool,
                                           p_q);
#ifdef AW_PBUF_ASSERT
    aw_assert(AW_OK == ret);
#endif

            /* POOL类型PBUF头和数据缓冲区都由本模块内存池释放 */
            } else if (p_q->type == AW_PBUF_TYPE_POOL) {

                ret = aw_pool_item_return(&p_q->p_pbuf_pool->pool_pbuf_pool,
                                           p_q);
#ifdef AW_PBUF_ASSERT
    aw_assert(AW_OK == ret);
#endif
            /* POOL类型PBUF头和数据缓冲区都由本模块内存堆释放 */
            } else if (p_q->type == AW_PBUF_TYPE_RAM) {
                aw_mem_free((void *)p_q);
            } else {
#ifdef AW_PBUF_ASSERT
    AW_INFOF(("pbuf : error type pbuf! pbuf free failed! \r\n"));
    aw_assert(0);
#endif
            }

            p_q = p_p;
            len++;

        } else {
            break;
        }
    }

    AW_MUTEX_UNLOCK(p_pbuf->mutex);

    return len;
}

/******************************************************************************/
size_t aw_pbuf_cnt_get (struct aw_pbuf *p_pbuf)
{
    size_t      len = 0;

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_pbuf);
#endif
    if (NULL == p_pbuf) {
        return len;
    }

    /* 统计PBUF链的个数 */
    while (p_pbuf) {
        len++;
        p_pbuf = p_pbuf->p_next;
    }

    return len;
}

/******************************************************************************/
aw_err_t aw_pbuf_pkt_expand (struct aw_pbuf *p_head, struct aw_pbuf *p_tail)
{
    struct aw_pbuf *p_tmp_pbuf = NULL;

#ifdef AW_PBUF_ASSERT
    aw_assert((NULL != p_head) && (NULL != p_tail));
#endif
    if ((NULL == p_head) ||
        (NULL == p_tail)) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_head->mutex, AW_SEM_WAIT_FOREVER);

    p_tmp_pbuf = p_head;

    /* 为避免链交叉，检查p_tail是否已经在p_head，如果是，返回错误 */
    while(NULL != p_tmp_pbuf) {
        if (p_tmp_pbuf == p_tail) {
            AW_MUTEX_UNLOCK(p_head->mutex);
            /* 如果该p_tail链已经在p_head里面，返回错误 */
            return -AW_ENOTSUP;
        }
        p_tmp_pbuf = p_tmp_pbuf->p_next;
    }

    p_tmp_pbuf = p_head;
    /* 合并链之前的准备工作，p_head链中PBUF的tot_len都要更新 */
    while(NULL != p_tmp_pbuf->p_next) {
        p_tmp_pbuf->tot_len += p_tail->tot_len;
        p_tmp_pbuf = p_tmp_pbuf->p_next;
    }

    p_tmp_pbuf->tot_len += p_tail->tot_len;

    /* 链接两条链成一个包 */
    p_tmp_pbuf->p_next = p_tail;

    AW_MUTEX_UNLOCK(p_head->mutex);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_pbuf_pkt_chain (struct aw_pbuf *p_head, struct aw_pbuf *p_tail)
{
    struct aw_pbuf *p_tmp_pbuf = NULL;

#ifdef AW_PBUF_ASSERT
    aw_assert((NULL != p_head) && (NULL != p_tail));
#endif
    if ((NULL == p_head) ||
        (NULL == p_tail)) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_head->mutex, AW_SEM_WAIT_FOREVER);

    p_tmp_pbuf = p_head;

    /* 为避免链交叉，检查p_tail是否已经在p_head，如果是，返回错误 */
    while(NULL != p_tmp_pbuf->p_next) {
        /* 如果该p_tail链已经在p_head里面，返回错误 */
        if (p_tmp_pbuf == p_tail) {
            AW_MUTEX_UNLOCK(p_head->mutex);
            return -AW_ENOTSUP;
        }
        p_tmp_pbuf = p_tmp_pbuf->p_next;
    }

    if (p_tmp_pbuf == p_tail) {
        /* 如果该p_tail链已经在p_head里面，返回错误 */
        return -AW_ENOTSUP;
    }

    p_tmp_pbuf->p_next = p_tail;

    AW_MUTEX_UNLOCK(p_head->mutex);

    return AW_OK;
}

/******************************************************************************/
//struct aw_pbuf* aw_pbuf_dechain (struct aw_pbuf *p_pbuf)
//{
//    struct aw_pbuf *p_tmp_pbuf = NULL;
//
//#ifdef AW_PBUF_ASSERT
//    aw_assert(NULL != p_pbuf);
//#endif
//
//    if (NULL == p_pbuf) {
//        return NULL;
//    }
//
//    p_tmp_pbuf = p_pbuf->p_next;
//
//    if (p_tmp_pbuf != NULL) {
//        AW_MUTEX_LOCK(p_pbuf->mutex, AW_SEM_WAIT_FOREVER);
//        p_tmp_pbuf->tot_len = p_pbuf->tot_len - p_pbuf->len;
//        p_pbuf->tot_len     = p_pbuf->len;
//        p_pbuf->p_next      = NULL;
//        AW_MUTEX_UNLOCK(p_pbuf->mutex);
//
//        /* 释放之后的所有PBUF */
//        aw_pbuf_free(p_tmp_pbuf);
//    }
//
//    return p_pbuf;
//}

/******************************************************************************/
aw_err_t aw_pbuf_copy (struct aw_pbuf *p_to, struct aw_pbuf *p_from)
{
    uint16_t  offset_to=0, offset_from=0, len;

#ifdef AW_PBUF_ASSERT
    aw_assert((NULL != p_to) && (NULL != p_from));
#endif

    if ((NULL == p_to) || (NULL == p_from)) {
        return -AW_EINVAL;
    }

    if (p_to->tot_len < p_from->tot_len) {
        return -AW_ENOMEM;
    }

    do {
        /* 确定单次拷贝的长度 */
        if ((p_to->len - offset_to) >= (p_from->len - offset_from)) {
            len = p_from->len - offset_from;
        } else {
            len = p_to->len - offset_to;
        }
        AW_MUTEX_LOCK(p_to->mutex, AW_SEM_WAIT_FOREVER);
        /* 拷贝数据 */
        __pbuf_memcpy((uint8_t*)p_to->p_payload + offset_to,
                      (uint8_t*)p_from->p_payload + offset_from,
                                len);
        offset_to += len;
        offset_from += len;

        AW_MUTEX_UNLOCK(p_to->mutex);

#ifdef AW_PBUF_ASSERT
    aw_assert(offset_to <= p_to->len);
    aw_assert(offset_from <= p_from->len);
#endif

        if (offset_from >= p_from->len) {

            offset_from = 0;
            p_from = p_from->p_next;
#ifdef AW_PBUF_ASSERT
    aw_assert((p_to != NULL) || (p_from == NULL));
#endif
        }
        if (offset_to == p_to->len) {

            offset_to = 0;
            p_to = p_to->p_next;

#ifdef AW_PBUF_ASSERT
    aw_assert((p_to != NULL) || (p_from == NULL));
#endif
        }

        /* 拷贝数据不能超过一个包 */
        if((p_from != NULL) && (p_from->len == p_from->tot_len)) {

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_from->p_next);
#endif
        }

        /* 拷贝数据不能超过一个包 */
        if((p_to != NULL) && (p_to->len == p_to->tot_len)) {

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_to->p_next);
#endif
        }
    } while (p_from);

    return AW_OK;
}

/******************************************************************************/
ssize_t aw_pbuf_data_read (struct aw_pbuf *p_pbuf,
                           void           *p_data,
                           size_t          len,
                           size_t          offset)
{
    struct aw_pbuf *p_tmp_buf = NULL;
    size_t          left = 0;
    size_t          buf_copy_len;
    size_t          copied_total = 0;

#ifdef AW_PBUF_ASSERT
    aw_assert((NULL != p_pbuf) && (NULL != p_data));
#endif

    if((NULL == p_pbuf) || (NULL == p_data)) {
        AW_ERRNO_SET(-AW_EINVAL);
        return AW_ERROR;
    }


    for(p_tmp_buf = p_pbuf; len != 0 && p_tmp_buf != NULL; p_tmp_buf = p_tmp_buf->p_next) {
        if ((offset != 0) && (offset >= p_tmp_buf->len)) {
            offset -= p_tmp_buf->len;
        } else {

            buf_copy_len = p_tmp_buf->len - offset;
            if (buf_copy_len > len) {
                buf_copy_len = len;
            }

             __pbuf_memcpy(&((char*)p_data)[left],
                           &((char*)p_tmp_buf->p_payload)[offset],
                                    buf_copy_len);

             copied_total += buf_copy_len;
             left += buf_copy_len;
             len -= buf_copy_len;
             offset = 0;
        }

        if (p_tmp_buf->len == p_tmp_buf->tot_len && len != 0) {
            AW_ERRNO_SET(-AW_ENOSPC);
            return copied_total;
        }
    }
    return copied_total;
}

/******************************************************************************/
ssize_t aw_pbuf_data_write (struct aw_pbuf *p_pbuf,
                            const void      *p_data,
                            size_t           len,
                            size_t           offset)
{
    struct   aw_pbuf *p_tmp_buf;
    size_t            buf_copy_len;
    size_t            total_copy_len = len;
    size_t            copied_total = 0;

#ifdef AW_PBUF_ASSERT
    aw_assert((NULL != p_pbuf) &&
              (NULL != p_data) &&
              (p_pbuf->tot_len >= len));
#endif

    if ((NULL == p_pbuf) || (NULL == p_data) || (p_pbuf->tot_len < len + offset)) {
        AW_ERRNO_SET(-AW_EINVAL);
        return AW_ERROR;
    }

    for(p_tmp_buf = p_pbuf; total_copy_len != 0; p_tmp_buf = p_tmp_buf->p_next) {
        if (offset != 0 && offset > p_tmp_buf->len) {
            offset -= p_tmp_buf->len;
        } else {
            buf_copy_len = p_tmp_buf->len - offset;

            if (buf_copy_len > total_copy_len) {
                buf_copy_len = total_copy_len;
            }

            AW_MUTEX_LOCK(p_tmp_buf->mutex, AW_SEM_WAIT_FOREVER);
            __pbuf_memcpy(&((char*)p_tmp_buf->p_payload)[offset],
                          &((char*)p_data)[copied_total],
                          buf_copy_len);
            AW_MUTEX_UNLOCK(p_tmp_buf->mutex);

            total_copy_len -= buf_copy_len;
            copied_total += buf_copy_len;
            offset = 0;
        }

        if (p_tmp_buf->len == p_tmp_buf->tot_len && copied_total != len) {
            AW_ERRNO_SET(-AW_ENOSPC);
            return copied_total;
        }
    }

#ifdef AW_PBUF_ASSERT
    aw_assert(total_copy_len == 0);
    aw_assert(copied_total == len);
#endif

    return copied_total;
}

/******************************************************************************/
struct aw_pbuf* aw_pbuf_type_change (struct aw_pbuf *p_pbuf, aw_pbuf_type_t type)
{
    struct aw_pbuf *p_tmp_pbuf;

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_pbuf);
#endif

    if (NULL == p_pbuf) {
        AW_ERRNO_SET(-AW_EINVAL);
        return NULL;
    }

    if (type == AW_PBUF_TYPE_POOL) {

        p_tmp_pbuf = aw_pbuf_alloc(p_pbuf->p_pbuf_pool,
                                   AW_PBUF_TYPE_POOL,
                                   p_pbuf->tot_len,
                                   NULL);
#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_tmp_pbuf);
#endif

    } else if (type == AW_PBUF_TYPE_RAM) {

        p_tmp_pbuf = aw_pbuf_alloc(p_pbuf->p_pbuf_pool,
                                   AW_PBUF_TYPE_RAM,
                                   p_pbuf->tot_len,
                                   NULL);
#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_tmp_pbuf);
#endif

    } else {

#ifdef AW_PBUF_ASSERT
    aw_assert(0);
#endif
        AW_ERRNO_SET(-AW_EPERM);
        return NULL;
    }

    aw_pbuf_copy (p_tmp_pbuf, p_pbuf);

    aw_pbuf_free(p_pbuf);

    return p_tmp_pbuf;
}

/******************************************************************************/
uint8_t aw_pbuf_char_get (struct aw_pbuf* p_pbuf, size_t offset)
{
    uint16_t        copy_from  = offset;
    struct aw_pbuf* p_tmp_pbuf = p_pbuf;

#ifdef AW_PBUF_ASSERT
    aw_assert(NULL != p_pbuf);
#endif

    if (NULL == p_pbuf) {
        return 0;
    }

    while ((p_tmp_pbuf != NULL) && (p_tmp_pbuf->len <= copy_from)) {
        copy_from -= p_tmp_pbuf->len;
        p_tmp_pbuf = p_tmp_pbuf->p_next;
    }

    if (p_tmp_pbuf != NULL && copy_from < p_tmp_pbuf->len) {
        return ((uint8_t *)p_tmp_pbuf->p_payload)[copy_from];
    }

    return 0;
}

/******************************************************************************/
ssize_t aw_pbuf_memcmp (struct aw_pbuf *p_pbuf,
                         size_t          offset,
                         const void     *p_str,
                         size_t          len)
{
    struct aw_pbuf* p_tmp_pbuf = p_pbuf;
    size_t          start      = offset;
    uint16_t        i;
    uint8_t         a, b;

#ifdef AW_PBUF_ASSERT
    aw_assert((NULL != p_pbuf) &&
              (NULL != p_str));
#endif

    if ((NULL == p_pbuf) || (NULL == p_str)) {
        AW_ERRNO_SET(-AW_EINVAL);
        return AW_ERROR;
    }

    while ((p_tmp_pbuf != NULL) && (p_tmp_pbuf->len <= start)) {
        start -= p_tmp_pbuf->len;
        p_tmp_pbuf = p_tmp_pbuf->p_next;
    }

    if ((p_tmp_pbuf != NULL) && (p_tmp_pbuf->len > start)) {

        for(i = 0; i < len; i++) {
            a = aw_pbuf_char_get(p_tmp_pbuf, start + i);
            b = ((uint8_t*)p_str)[i];
            if (a != b) {
                return i+1;
            }
        }
        return 0;
    }

    return AW_ERROR;
}

/******************************************************************************/
ssize_t aw_pbuf_memfind (struct aw_pbuf    *p_pbuf,
                          const void        *p_mem,
                          size_t             mem_len,
                          size_t             start_offset)
{
    size_t   i, plus;
    size_t   max = p_pbuf->tot_len - mem_len;

#ifdef AW_PBUF_ASSERT
    aw_assert((NULL != p_pbuf) &&
              (NULL != p_mem));
#endif

    if ((NULL == p_pbuf) || (NULL == p_mem)) {
        AW_ERRNO_SET(-AW_EINVAL);
        return AW_ERROR;
    }

    if (p_pbuf->tot_len >= mem_len + start_offset) {
        for(i = start_offset; i <= max; ) {
            plus = aw_pbuf_memcmp(p_pbuf, i, p_mem, mem_len);
            if (plus == 0) {
                return i;
            } else {
                i += plus;
            }
        }
    }
    return AW_ERROR;
}

/******************************************************************************/
ssize_t aw_pbuf_strstr (struct aw_pbuf *p_pbuf, const char *p_substr)
{
    size_t substr_len;

#ifdef AW_PBUF_ASSERT
    aw_assert((NULL != p_pbuf)   &&
              (0 != p_substr[0]) &&
              (AW_ERROR != p_pbuf->tot_len));
#endif

    if ((p_substr == NULL) ||
        (p_substr[0] == 0) ||
        (p_pbuf->tot_len == AW_ERROR)) {

        AW_ERRNO_SET(-AW_EINVAL);
        return AW_ERROR;
    }

    substr_len = strlen(p_substr);

    return aw_pbuf_memfind(p_pbuf, p_substr, (size_t)substr_len, 0);
}


/* end of file */
