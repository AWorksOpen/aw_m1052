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
 * \brief expand I/O system.
 *
 * \internal
 * \par modification history:
 * - 1.00 16-01-07 deo, first implementation.
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "eio/aw_eio.h"
#include "aw_list.h"
#include "aw_sem.h"
#include "aw_pool.h"
#include "string.h"

/*******************************************************************************
  local variables
*******************************************************************************/

/**
 * \brief EIO 设备互斥锁，用于保护 EIO 设备
 */
AW_MUTEX_DECL_STATIC(         __gp_eio_dev_mutex);

/**
 * \brief EIO 设备链链头
 */
static struct aw_list_head    __gp_eio_dev_list;

/**
 * \brief EIO 通道有效性检查
 */
#define __PIPE_INVALID(p) \
                ((NULL == (p)) || \
                 (NULL == (p)->p_obj) || \
                 (NULL == (p)->p_obj->p_dev))

/**
 * \brief EIO 对象有效性检查
 */
#define __OBJ_INVALID(p) \
                ((NULL == (p)) || \
                 (NULL == (p)->p_dev))

/**
 * \brief 选择两个数之间较小的数字
 */
#define __EIO_SIZE_MIN(x , y)  (((x) < (y)) ? (x) : (y))
/******************************************************************************/
struct aw_eio_dev *__eio_dev_find (const char *name)
{
    struct aw_eio_dev  *p_dev;
    AW_MUTEX_LOCK(__gp_eio_dev_mutex, AW_SEM_WAIT_FOREVER);

    aw_list_for_each_entry(p_dev,
                           &__gp_eio_dev_list,
                           struct aw_eio_dev,
                           node) {
        if (strcmp(p_dev->name, name) == 0) {
            AW_MUTEX_UNLOCK(__gp_eio_dev_mutex);
            return p_dev;
        }
    }
    AW_MUTEX_UNLOCK(__gp_eio_dev_mutex);

    return NULL;
}

/******************************************************************************/
aw_bool_t __eio_complete (void *arg)
{
    aw_semb_give((aw_semb_id_t)arg);
    return AW_TRUE;
}

/******************************************************************************/
void __eio_pkt_fill (struct aw_eio_pkt  *p_pkt,
                     void               *p_buf,
                     size_t              len,
                     int                 opt,
                     aw_bool_t          (*pfn_complete) (void *arg),
                     void               *p_arg,
                     uint8_t             flags)
{
    p_pkt->p_buf         = p_buf;
    p_pkt->len           = len;
    p_pkt->opt           = opt;
    p_pkt->pfn_complete  = pfn_complete;
    p_pkt->p_arg         = p_arg;
    p_pkt->status        = -AW_ENOTCONN;
    p_pkt->ret_len       = 0;
    p_pkt->p_priv        = NULL;
    p_pkt->flags         = flags;
    p_pkt->p_cur_buf     = NULL;
}

/******************************************************************************/
aw_err_t __eio_submit (struct aw_eio_pipe *p_pipe,
                       struct aw_eio_pkt  *p_pkt)
{
    struct aw_eio_dev *p_dev;

    if (__PIPE_INVALID(p_pipe) || (NULL == p_pkt)) {
        return -AW_EINVAL;
    }

    p_dev = p_pipe->p_obj->p_dev;

    if ((p_dev->p_funs == NULL) ||
        (p_dev->p_funs->pfn_start == NULL)) {
        return -AW_ENOTSUP;
    }

    aw_spinlock_isr_take(&p_pipe->lock);
    aw_list_add_tail(&p_pkt->node,
                     &p_pipe->work_list);
    p_pkt->status = -AW_EISCONN;
    aw_spinlock_isr_give(&p_pipe->lock);

    return p_dev->p_funs->pfn_start(p_dev, p_pipe);
}

/******************************************************************************/
void __eio_cancel (struct aw_eio_pipe *p_pipe,
                   struct aw_eio_pkt  *p_pkt)
{
    struct aw_eio_dev *p_dev;

    p_dev = p_pipe->p_obj->p_dev;

    aw_spinlock_isr_take(&p_pipe->lock);
    if (aw_list_empty(&p_pkt->node)) {
        p_dev->p_funs->pfn_cancel(p_dev, p_pipe, p_pkt);
    } else {
        aw_list_del_init(&p_pkt->node);
    }
    aw_spinlock_isr_give(&p_pipe->lock);
}

/******************************************************************************/
int __eio_transfer_sync (struct aw_eio_pipe *p_pipe,
                         int                 opt,
                         void               *p_buf,
                         size_t              len,
                         int                 timeout)
{
    struct aw_eio_pkt  pkt;
    AW_SEMB_DECL(      sem);
    aw_semb_id_t       sem_id;
    int                ret;

    memset(&sem,0,sizeof(sem));
    sem_id = AW_SEMB_INIT(sem, 0, AW_SEM_Q_PRIORITY);
    if (sem_id == NULL) {
        return -AW_ENOMEM;
    }

    __eio_pkt_fill(&pkt,
                    p_buf,
                    len,
                    opt,
                    __eio_complete,
                   (void*)sem_id,
                    0);

    ret = __eio_submit(p_pipe, &pkt);

    if (ret != AW_OK) {
        goto __exit;
    }

    ret = aw_semb_take(sem_id, timeout);

    if (ret == -AW_ETIME) {
        __eio_cancel(p_pipe, &pkt);
        ret = pkt.ret_len;
        goto __exit;
    }

    if (pkt.status != AW_OK) {
        ret = pkt.status;
    } else {
        ret = pkt.len;
    }

__exit:
    AW_SEMB_TERMINATE(sem);
    return ret;
}

/******************************************************************************/
void aw_eio_init (void)
{
    AW_MUTEX_INIT(__gp_eio_dev_mutex,
                  AW_SEM_Q_PRIORITY);

    aw_list_head_init(&__gp_eio_dev_list);
}

/******************************************************************************/
aw_err_t aw_eio_obj_init (struct aw_eio_obj  *p_obj,
                          const char         *name,
                          void               *p_mem,
                          size_t              size)
{
    if ((NULL == p_obj) ||
        (NULL == name) ||
        (EOS == *name) ||
        (NULL == p_mem) ||
        (size == 0)) {
        return -AW_EINVAL;
    }

    /* find the EIO device */
    p_obj->p_dev = __eio_dev_find(name);
    if (p_obj->p_dev == NULL) {
        return -AW_ENODEV;
    }

    /* initialize packet pool */
    if(aw_pool_init(&p_obj->pkt_pool,
                       p_mem,
                       size,
                       sizeof(struct aw_eio_pkt)) == NULL) {
        return -AW_ENOMEM;
    }

    p_obj->p_mem = p_mem;
    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_eio_obj_uninit (struct aw_eio_obj  *p_obj)
{
    return AW_OK;
}

/******************************************************************************/
struct aw_eio_pipe *aw_eio_pipe_open (struct aw_eio_obj *p_obj,
                                      uint32_t           pipe_id)
{
    struct aw_eio_dev  *p_dev;
    struct aw_eio_pipe *p_pipe;

    if (__OBJ_INVALID(p_obj)) {
        return NULL;
    }

    p_dev = p_obj->p_dev;

    aw_spinlock_isr_take(&p_dev->lock);
    aw_list_for_each_entry(p_pipe,
                           &p_dev->pipes,
                           struct aw_eio_pipe,
                           node) {
        if ((pipe_id == p_pipe->id) &&
            (p_pipe->p_obj == NULL)) {
            p_pipe->p_obj = p_obj;
            aw_spinlock_isr_give(&p_dev->lock);
            return p_pipe;
        }
    }
    aw_spinlock_isr_give(&p_dev->lock);
    return NULL;
}

/******************************************************************************/
void aw_eio_pipe_close (struct aw_eio_pipe *p_pipe)
{
    if (__PIPE_INVALID(p_pipe)) {
        return;
    }

    aw_spinlock_isr_take(&p_pipe->lock);
    p_pipe->p_obj = NULL;
    aw_spinlock_isr_give(&p_pipe->lock);
}

/******************************************************************************/
int aw_eio_read (struct aw_eio_pipe *p_pipe,
                 void               *p_buf,
                 size_t              len,
                 int                 timeout)
{
    if (__PIPE_INVALID(p_pipe) ||
        ((p_pipe->id & AW_EIO_IN) == 0) ||
        (p_buf == NULL) ||
        (len == 0)) {
        return -AW_EINVAL;
    }

    return __eio_transfer_sync(p_pipe,
                               AW_EIO_READ,
                               p_buf,
                               len,
                               timeout);
}


/******************************************************************************/
int aw_eio_write (struct aw_eio_pipe *p_pipe,
                  void               *p_buf,
                  size_t              len,
                  int                 timeout)
{
    if (__PIPE_INVALID(p_pipe) ||
        ((p_pipe->id & AW_EIO_OUT) == 0) ||
        (p_buf == NULL) ||
        (len == 0)) {
        return -AW_EINVAL;
    }

    return __eio_transfer_sync(p_pipe,
                               AW_EIO_WRITE,
                               p_buf,
                               len,
                               timeout);
}

/******************************************************************************/
int aw_eio_issue(struct aw_eio_pipe *p_pipe,
                 int                 opt,
                 void               *p_buf,
                 size_t              len,
                 aw_bool_t            (*complete) (void *arg),
                 void               *arg)
{
    struct aw_eio_pkt  *p_pkt;
    struct aw_eio_obj  *p_obj;

    if (__PIPE_INVALID(p_pipe) ||
        (opt > AW_EIO_WRITE)   ||
        (p_buf == NULL)        ||
        (len == 0)) {
        return -AW_EINVAL;
    }

    if (!(p_pipe->id & (opt ? AW_EIO_OUT : AW_EIO_IN))) {
        return -AW_EINVAL;
    }

    p_obj = p_pipe->p_obj;

    p_pkt = (struct aw_eio_pkt*)aw_pool_item_get(&p_obj->pkt_pool);
    if (p_pkt == NULL) {
        return -AW_ENOMEM;
    }

    __eio_pkt_fill(p_pkt,
                   p_buf,
                   len,
                   opt,
                   complete,
                   arg,
                   AW_EIO_FLAGS_POOL);

    return __eio_submit(p_pipe, p_pkt);
}

/******************************************************************************/
int aw_eio_reclaim(struct aw_eio_pipe  *p_pipe,
                   int                 *p_opt,
                   void               **pp_buf,
                   size_t              *p_ret_len,
                   int                 *p_status,
                   int                  timeout,
                   aw_bool_t            abort)
{
    struct aw_eio_pkt *p_pkt = NULL;
    struct aw_eio_obj *p_obj;
    int                ret = AW_OK;
    aw_bool_t          aborted = AW_FALSE;
    aw_bool_t          reclaim_flag = AW_FALSE;

    if (__PIPE_INVALID(p_pipe)) {
        return -AW_EINVAL;
    }

    p_obj = p_pipe->p_obj;

    do {
        aw_spinlock_isr_take(&p_pipe->lock);
        if (!aw_list_empty(&p_pipe->done_list)) {
            aw_list_for_each_entry(p_pkt,
                                  &p_pipe->done_list,
                                   struct aw_eio_pkt,
                                   node) {
                if (!(p_pkt->flags & AW_EIO_FLAGS_PBUF)) {
                    aw_list_del(&p_pkt->node);
                    reclaim_flag = AW_TRUE;
                    break;
                }
            }
            aw_spinlock_isr_give(&p_pipe->lock);
            if (aborted  && !reclaim_flag ) {
                return -AW_ETIME;
            }
        } else {
            aw_spinlock_isr_give(&p_pipe->lock);
            if (aborted ) {
                return -AW_ETIME;
            }
            ret = AW_SEMC_TAKE(p_pipe->done_sem, timeout);
            if (ret != AW_OK) {
                if ((abort ) && (p_obj->p_dev->p_funs->pfn_abort)) {
                    p_obj->p_dev->p_funs->pfn_abort(p_obj->p_dev, p_pipe);
                    aborted = AW_TRUE;
                }
            }
        }
    } while (p_pkt == NULL);

    if (p_opt) {
        *p_opt = p_pkt->opt;
    }

    if (pp_buf) {
        *pp_buf = p_pkt->p_buf;
    }

    if (p_ret_len) {
        *p_ret_len = p_pkt->ret_len;
    }

    if (p_status) {
        *p_status = p_pkt->status;
    }

    aw_pool_item_return(&p_obj->pkt_pool, p_pkt);

    return AW_OK;
}

/******************************************************************************/

int aw_eio_pbuf_issue (struct aw_eio_pipe *p_pipe,
                       int                 opt,
                       struct aw_pbuf     *p_pbuf,
                       size_t              len,
                       aw_bool_t          (*pfn_complete) (void *arg),
                       void               *p_arg)
{
    struct aw_eio_pkt  *p_pkt;
    struct aw_eio_obj  *p_obj;

    if (__PIPE_INVALID(p_pipe) ||
        (opt > AW_EIO_WRITE)   ||
        (p_pbuf == NULL)) {
        return -AW_EINVAL;
    }

    if (!(p_pipe->id & (opt ? AW_EIO_OUT : AW_EIO_IN))) {
        return -AW_EINVAL;
    }

    p_obj = p_pipe->p_obj;

    p_pkt = (struct aw_eio_pkt*)aw_pool_item_get(&p_obj->pkt_pool);
    if (p_pkt == NULL) {
        return -AW_ENOMEM;
    }

    __eio_pkt_fill(p_pkt,
                   p_pbuf,
                   len,
                   opt,
                   pfn_complete,
                   p_arg,
                   AW_EIO_FLAGS_POOL | AW_EIO_FLAGS_PBUF);

    aw_pbuf_ref(p_pbuf);

    return __eio_submit(p_pipe, p_pkt);
}

/******************************************************************************/

int aw_eio_pbuf_reclaim(struct aw_eio_pipe  *p_pipe,
                        int                 *p_opt,
                        struct aw_pbuf     **pp_pbuf,
                        size_t              *p_ret_len,
                        int                 *p_status,
                        int                  timeout,
                        aw_bool_t            abort)
{
    struct aw_eio_pkt *p_pkt = NULL;
    struct aw_eio_obj *p_obj;
    int                ret = AW_OK;
    aw_bool_t             aborted = AW_FALSE;
    aw_bool_t             reclaim_flag = AW_FALSE;

    if (__PIPE_INVALID(p_pipe)) {
        return -AW_EINVAL;
    }

    p_obj = p_pipe->p_obj;

    do {
        aw_spinlock_isr_take(&p_pipe->lock);

        if (!aw_list_empty(&p_pipe->done_list)) {

            aw_list_for_each_entry(p_pkt,
                                  &p_pipe->done_list,
                                   struct aw_eio_pkt,
                                   node) {
                if (p_pkt->flags & AW_EIO_FLAGS_PBUF) {
                    aw_list_del(&p_pkt->node);
                    reclaim_flag = AW_TRUE;
                    break;
                }
            }
            aw_spinlock_isr_give(&p_pipe->lock);
            if (aborted  && !reclaim_flag ) {
                return -AW_ETIME;
            }

        } else {
            aw_spinlock_isr_give(&p_pipe->lock);
            if (aborted ) {
                return -AW_ETIME;
            }
            ret = AW_SEMC_TAKE(p_pipe->done_sem, timeout);
            if (ret != AW_OK) {
                if ((abort ) && (p_obj->p_dev->p_funs->pfn_abort)) {
                    p_obj->p_dev->p_funs->pfn_abort(p_obj->p_dev, p_pipe);
                    aborted = AW_TRUE;
                }
            }
        }
    } while (p_pkt == NULL);

    if (p_opt) {
        *p_opt = p_pkt->opt;
    }

    if (pp_pbuf) {
        *pp_pbuf = p_pkt->p_buf;
    }

    if (p_ret_len) {
        *p_ret_len = p_pkt->ret_len;
    }

    if (p_status) {
        *p_status = p_pkt->status;
    }


    aw_pbuf_free(*pp_pbuf);

    aw_pool_item_return(&p_obj->pkt_pool, p_pkt);

    return AW_OK;
}


/******************************************************************************/
aw_err_t aw_eio_ctrl (struct aw_eio_obj *p_obj,
                      int                cmd,
                      void              *arg)
{
    if (NULL == p_obj) {
        return -AW_EINVAL;
    }

    if ((p_obj->p_dev->p_funs == NULL) ||
        (p_obj->p_dev->p_funs->pfn_control == NULL)) {
        return -AW_ENOTSUP;
    }

    return p_obj->p_dev->p_funs->pfn_control(p_obj->p_dev, cmd, arg);
}

/******************************************************************************/
aw_err_t aw_eio_abort (struct aw_eio_pipe *p_pipe)
{
    struct aw_eio_dev   *p_dev;
    struct aw_list_head  list;
    struct aw_eio_pkt   *p_pkt, *p_tmp;

    if (__PIPE_INVALID(p_pipe)) {
        return -AW_EINVAL;
    }

    p_dev = p_pipe->p_obj->p_dev;

    if ((p_dev->p_funs) ||
        (p_dev->p_funs->pfn_abort)) {
        return -AW_ENOTSUP;
    }

    aw_spinlock_isr_take(&p_pipe->lock);
    aw_list_head_init(&list);
    aw_list_splice_tail_init(&p_pipe->work_list,
                             &list);
    aw_spinlock_isr_give(&p_pipe->lock);

    p_dev->p_funs->pfn_abort(p_dev, p_pipe);

    aw_list_for_each_entry_safe(p_pkt,
                                p_tmp,
                                &list,
                                struct aw_eio_pkt,
                                node) {
        aw_list_del(&p_pkt->node);
        p_pkt->ret_len = 0;
        p_pkt->status  = -AW_ECANCELED;
        aw_eio_dev_pkt_done(p_pipe, p_pkt);
    }

    return AW_OK;
}





/******************************************************************************/
aw_err_t aw_eio_dev_create (struct aw_eio_dev  *p_dev,
                            const char         *name,
                            struct aw_eio_funs *p_funs)
{
    if ((NULL == p_dev) ||
        (NULL == name)  ||
        (EOS == *name)  ||
        (NULL == p_funs)) {
        return -AW_EINVAL;
    }

    /* check if device exist */
    if (__eio_dev_find(name) != NULL) {
        return -AW_EEXIST;
    }

    p_dev->p_funs = p_funs;
    p_dev->name   = name;
    p_dev->np     = 0;

    aw_list_head_init(&p_dev->pipes);

    aw_spinlock_isr_init(&p_dev->lock, 0);

    aw_list_add_tail(&p_dev->node, &__gp_eio_dev_list);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_eio_dev_delete (struct aw_eio_dev *p_dev)
{
    if (NULL == p_dev) {
        return -AW_EINVAL;
    }

    if ((p_dev->p_funs) &&
        (p_dev->p_funs->pfn_abort)) {
        struct aw_eio_pipe *p_pipe;

        aw_list_for_each_entry(p_pipe,
                               &p_dev->pipes,
                               struct aw_eio_pipe,
                               node) {
            p_dev->p_funs->pfn_abort(p_dev, p_pipe);
        }
    }

    AW_MUTEX_LOCK(__gp_eio_dev_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_del(&p_dev->node);
    AW_MUTEX_UNLOCK(__gp_eio_dev_mutex);

    return AW_OK;
}


/******************************************************************************/
void aw_eio_dev_pipe_add (struct aw_eio_dev  *p_dev,
                          struct aw_eio_pipe *p_pipe,
                          uint32_t            id,
                          void               *priv)
{
    p_pipe->id   = id;
    p_pipe->p_priv = priv;

    aw_list_head_init(&p_pipe->done_list);
    aw_list_head_init(&p_pipe->work_list);
    AW_SEMC_INIT(p_pipe->done_sem, 0, AW_SEM_Q_PRIORITY);
    aw_spinlock_isr_init(&p_pipe->lock, 0);

    aw_spinlock_isr_take(&p_dev->lock);
    aw_list_add_tail(&p_pipe->node, &p_dev->pipes);
    p_dev->np++;
    aw_spinlock_isr_give(&p_dev->lock);
}

/******************************************************************************/
void aw_eio_dev_pipe_del (struct aw_eio_dev  *p_dev,
                          struct aw_eio_pipe *p_pipe)
{
    aw_spinlock_isr_take(&p_dev->lock);
    aw_list_del(&p_pipe->node);
    p_dev->np--;
    aw_spinlock_isr_give(&p_dev->lock);
}

/******************************************************************************/
struct aw_eio_pkt *aw_eio_dev_pkt_get (struct aw_eio_pipe *p_pipe)
{
    struct aw_eio_pkt *p_pkt = NULL;

    aw_spinlock_isr_take(&p_pipe->lock);
    if (!aw_list_empty(&p_pipe->work_list)) {
        p_pkt = (struct aw_eio_pkt*)aw_list_first_entry(
                        &p_pipe->work_list,
                        struct aw_eio_pkt,
                        node);
        aw_list_del_init(&p_pkt->node);
    }
    aw_spinlock_isr_give(&p_pipe->lock);
    return p_pkt;
}

/******************************************************************************/
/**
 * \brief get pbuf from a EIO pkt
 */
aw_err_t aw_eio_pkt_buf_get (struct aw_eio_pkt *p_pkt,
                             void             **pp_buf,
                             size_t            *p_len)
{
    if (p_pkt->flags & AW_EIO_FLAGS_PBUF) {

        if (p_pkt->p_cur_buf == NULL) {
            p_pkt->p_cur_buf = p_pkt->p_buf;

            *pp_buf = ((struct aw_pbuf *)p_pkt->p_cur_buf)->p_payload;

            *p_len = __EIO_SIZE_MIN(((struct aw_pbuf *)p_pkt->p_cur_buf)->len,
                                      p_pkt->len);
            p_pkt->len -= (*p_len);

        } else if ((NULL != ((struct aw_pbuf *)p_pkt->p_cur_buf)->p_next) &&
                    (p_pkt->len > 0)) {
            p_pkt->p_cur_buf = ((struct aw_pbuf *)p_pkt->p_cur_buf)->p_next;

            *pp_buf = ((struct aw_pbuf *)p_pkt->p_cur_buf)->p_payload;

            *p_len = __EIO_SIZE_MIN(((struct aw_pbuf *)p_pkt->p_cur_buf)->len,
                                      p_pkt->len);
            p_pkt->len -= (*p_len);
        } else {
            return -AW_ENOMEM;
        }
    } else {
        if (p_pkt->p_cur_buf == NULL) {
            p_pkt->p_cur_buf = p_pkt->p_buf;

            *pp_buf = p_pkt->p_buf;
            *p_len  = p_pkt->len;
        } else {
             return -AW_ENOMEM;
        }
    }

    return AW_OK;
}


/******************************************************************************/
void aw_eio_dev_pkt_done (struct aw_eio_pipe *p_pipe,
                          struct aw_eio_pkt  *p_pkt)
{
    aw_bool_t reclaim = AW_TRUE;

    if (p_pkt->pfn_complete) {
        if (p_pkt->pfn_complete(p_pkt->p_arg) ) {
            reclaim = AW_FALSE;
        }
    }

    if (reclaim ) {
        aw_spinlock_isr_take(&p_pipe->lock);
        aw_list_add_tail(&p_pkt->node,
                         &p_pipe->done_list);
        aw_spinlock_isr_give(&p_pipe->lock);
        AW_SEMC_GIVE(p_pipe->done_sem);
    } else if (p_pkt->flags & AW_EIO_FLAGS_POOL) {
        aw_pool_item_return(&p_pipe->p_obj->pkt_pool, p_pkt);
    }
}




