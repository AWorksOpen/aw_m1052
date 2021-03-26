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

#include "apollo.h"
#include "clk_provider.h"
#include <string.h>
#include "aw_spinlock.h"

static struct clk           **__g_clk_base;
static int                   __g_clk_count;

static aw_spinlock_isr_t    __clk_spinlock;


#define IS_VALID_CLK_ID(id) ( 0 <= (id) && __g_clk_count > (id) )

static aw_err_t __clk_add_child(struct clk *parent,struct clk *child);

struct clk * __clk_find_by_name(const char *name)
{
    int                 i;
    struct clk         *hw = NULL;

    if (NULL == name) {
        return NULL;
    }

    for (i = 0;i < __g_clk_count; i++) {
        if ( NULL != __g_clk_base[i] ) {
            if (NULL != __g_clk_base[i]->name ) {
                if (0 == strcmp(name,__g_clk_base[i]->name)) {
                    hw = __g_clk_base[i];
                    break;
                }
            }
        }
    }

    return hw;
}

aw_clk_id_t __clk_get_id(struct clk *clk)
{
#ifdef CONFIG_CLK_NO_CACHE_ID
    int                 i;

    for (i = 0;i<__g_clk_count;i++) {
        if (clk == __g_clk_base[i]) {
            return (aw_clk_id_t)i;
        }
    }
    return -1;
#else 
    return clk->clk_id;
#endif

}

struct clk * __clk_find_by_id(aw_clk_id_t clk_id)
{
    if (IS_VALID_CLK_ID(clk_id)) {
        return __g_clk_base[clk_id];
    }
    return NULL;
}

/**
 * __clk_round_rate - round the given rate for a clk
 * @clk: round the rate of this clock
 * @rate: the rate which is to be rounded
 *
 * Caller must hold prepare_lock.  Useful for clk_ops such as .set_rate
 */
aw_clk_rate_t __clk_round_rate(struct clk *clk, aw_clk_rate_t rate)
{
    aw_clk_rate_t       parent_rate = 0;
    struct clk         *parent_hw;
    aw_clk_id_t         parent;


    parent_hw = clk->parent;
    parent_rate = __clk_get_rate(parent_hw);

    if (clk->ops->determine_rate) {
        return clk->ops->determine_rate(clk, rate, &parent_rate,
                        &parent);
    }
    else if (clk->ops->round_rate) {
        return clk->ops->round_rate(clk, rate, &parent_rate);
    }
    else if (clk->flags & CLK_SET_RATE_PARENT) {
        return __clk_round_rate(parent_hw, rate);
    }
    else {
        return __clk_get_rate(clk);
    }
}

struct clk * __clk_get_parent(struct clk *clk)
{
    return clk->parent;
}


#if defined CONFIG_CLK_NO_CACHE_RATE || defined CONFIG_CLK_NO_CACHE_CHILD

aw_clk_rate_t __clk_get_rate(struct clk *hw)
{
    aw_clk_rate_t       rate = 0;
    aw_clk_rate_t       parent_rate;
    struct clk         *parent;

    if (NULL == hw) {
        return 0;
    }
    
    parent = __clk_get_parent(hw);
    parent_rate = __clk_get_rate(parent);
    
    if (NULL != hw->ops->recalc_rate) {
         rate = hw->ops->recalc_rate(hw,parent_rate);
    }
    else {
        rate = parent_rate;
    }

    return rate;
}

#else
aw_clk_rate_t __clk_get_rate(struct clk *hw)
{
    aw_clk_rate_t       rate = 0;

    if (NULL == hw) {
        return 0;
    }

    if ( (hw->flags & CLK_GET_RATE_NOCACHE) &&
            NULL != hw->ops->recalc_rate) {
        aw_clk_rate_t       parent_rate;
        struct clk         *parent;

        parent = __clk_get_parent(hw);
        parent_rate = __clk_get_rate(parent);
        rate = hw->ops->recalc_rate(hw,parent_rate);
        hw->rate = rate;
    }
    else {
        rate = hw->rate;
    }

    return rate;
}

#endif

static aw_err_t __clk_enable(struct clk *hw)
{
    aw_err_t        err = 0;

    if (NULL != hw->parent) {
        err = __clk_enable(hw->parent);
    }

    if (0 == err) {
        hw->ref_count ++;
        if (1 == hw->ref_count) {
            if (NULL != hw->ops) {
                if (NULL != hw->ops->enable) {
                    hw->ops->enable(hw);
                }
            }
        }
    }
    return err;
}

static aw_err_t __clk_exec_disable(struct clk *hw)
{
    aw_err_t        err = 0;

    if (NULL != hw->ops) {
        if (NULL != hw->ops->disable) {
            err = hw->ops->disable(hw);
        }
    }
    return err;
}

static aw_err_t __clk_disable(struct clk *hw)
{
    aw_err_t        err = 0;

    if (NULL == hw) {
        return 0;
    }
    if (hw->ref_count <= 0) {
        err = -AW_EBADF;
        goto cleanup;
    }

    hw->ref_count --;
    if (0 == hw->ref_count) {
        err = __clk_exec_disable(hw);
        if (0 != err) {
            hw->ref_count ++;
            goto cleanup;
        }
    }

    if (NULL != hw->parent) {
        err = __clk_disable(hw->parent);
    }
cleanup:
    return err;
}


struct clk *clk_register_hw(struct clk *hw,struct clk_init_data *p_init)
{
    const struct clk_ops       *p_ops;
    struct clk                 *ret = NULL;
    struct clk                 *parent_clk;

    p_ops = p_init->ops;
    if (NULL != p_ops) {
        /* 如果设置父时钟被实现，则获取父时钟也必须被实现 */
        if ( NULL != p_ops->set_parent
                && NULL == p_ops->get_parent) {
            goto cleanup;
        }
    }

    /* 输入name不能是NULL */
    if (NULL == p_init->name) {
        goto cleanup;
    }
    /* 和以前的时钟名称不能重复 */
    parent_clk = __clk_find_by_name(p_init->name);
    if (NULL != parent_clk) {
        goto cleanup;
    }

    /* 此时钟ID是否已经注册 */
    if (!IS_VALID_CLK_ID(p_init->clk_id) ) {
        goto cleanup;
    }
    if ( NULL != __g_clk_base[p_init->clk_id]) {
        goto cleanup;
    }

    memset(hw,0,sizeof(*hw));
    hw->name = p_init->name;
    hw->ref_count = 0;
#ifndef CONFIG_CLK_NO_CACHE_CHILD
    AW_INIT_HLIST_HEAD(&hw->children);
    AW_INIT_HLIST_NODE(&hw->child_node);
#endif
    
    hw->ops = p_init->ops;
    
#ifndef CONFIG_CLK_NO_CACHE_ID
    hw->clk_id = p_init->clk_id;
#endif
    hw->flags = p_init->flags;
#if 0
    hw ->new_child = NULL;
    hw ->new_rate = 0;
#endif 

    if (NULL == p_init->parent_name) {
        hw ->parent = NULL;
    }
    else {
        parent_clk = __clk_find_by_name(p_init->parent_name);
        if (NULL == parent_clk) {
            goto cleanup;
        }
        hw->parent = parent_clk;
    }

#ifndef CONFIG_CLK_NO_CACHE_RATE
    hw ->rate = 0;
#endif

    ret = hw;
    if (NULL == p_ops) {
        goto cleanup;
    }

    if (NULL != p_ops->init) {
        p_ops->init(hw);
    }

    if (NULL != hw->parent ) {
        aw_err_t            err;

        err = __clk_add_child(hw->parent,hw);
        if (0 != err) {
            ret = NULL;
            goto cleanup;
        }
    }

#ifndef CONFIG_CLK_NO_CACHE_RATE
    if (p_ops ->recalc_rate ) {
        aw_clk_rate_t       parent_rate;

        parent_rate = __clk_get_rate(hw->parent);
        hw->rate = p_ops->recalc_rate(hw,parent_rate);
    }
    else if (hw->parent >= 0) {
        hw->rate = __clk_get_rate(hw->parent);
    }
    else {
        hw ->rate = 0;
    }
#endif

cleanup:
    if (NULL != ret) {
        __g_clk_base[p_init->clk_id] = ret;
    }

    return ret;
}

static aw_err_t __clk_add_child(struct clk *parent,struct clk *child)
{
    aw_err_t            err = -AW_EINVAL;
    struct clk             *p;
    struct aw_hlist_node   *p_node;
#ifndef CONFIG_CLK_NO_CACHE_CHILD
    /* 首先检查是否已经存在此child */
    if (NULL == parent|| NULL == child) {
        goto cleanup;
    }

    aw_hlist_for_each_entry(p,p_node,&parent->children,struct clk,child_node) {
        if (p == child) {
            err = -AW_EEXIST;
            goto cleanup;
        }
    }

    aw_hlist_add_head(&child->child_node,&parent->children);
 #endif
    err = 0;
cleanup:
    return err;
}

static void __clk_recalc_rate(struct clk *hw)
{
#ifndef CONFIG_CLK_NO_CACHE_RATE
    aw_clk_rate_t           rate = 0;
    aw_clk_rate_t           parent_rate;
    struct clk             *parent;
    struct clk             *child;
    struct aw_hlist_node   *p_node;

    parent = __clk_get_parent(hw);
    parent_rate = __clk_get_rate(parent);

    if (NULL != hw->ops && NULL != hw->ops->recalc_rate) {
        rate = hw->ops->recalc_rate(hw,parent_rate);
    }
    else {
        rate = parent_rate;
    }

    hw ->rate = rate;
#ifndef CONFIG_CLK_NO_CACHE_CHILD
    aw_hlist_for_each_entry(child,p_node,&hw->children,struct clk,child_node) {
        __clk_recalc_rate(child);
    }
#endif                  /* CONFIG_CLK_NO_CACHE_CHILD */

#endif
}

static void __clk_reparent(struct clk *clk, struct clk *new_parent)
{
#ifndef CONFIG_CLK_NO_CACHE_CHILD
    aw_hlist_del_init(&clk->child_node);
#endif

#ifndef CONFIG_CLK_NO_SET_RATE_RECURSIVE
    /* avoid duplicate POST_RATE_CHANGE notifications */
    if (new_parent->new_child == clk) {
        new_parent->new_child = NULL;
    }
#endif
#ifndef CONFIG_CLK_NO_CACHE_CHILD
    aw_hlist_add_head(&clk->child_node, &new_parent->children);
#endif
    clk->parent = new_parent;
}

static struct clk *__clk_set_parent_before(struct clk *clk, struct clk *parent)
{
    unsigned long           flags;
    struct clk             *old_parent = clk->parent;

    /*
     * 1. Migrate prepare state between parents and prevent race with
     * __clk_enable().
     *
     * If the clock is not prepared, then a race with
     * __clk_enable/disable() is impossible since we already have the
     * prepare lock (future calls to __clk_enable() need to be preceded by
     * a clk_prepare()).
     *
     * If the clock is prepared, migrate the prepared state to the new
     * parent and also protect against a race with __clk_enable() by
     * forcing the clock and the new parent on.  This ensures that all
     * future calls to __clk_enable() are practically NOPs with respect to
     * hardware and software states.
     *
     * See also: Comment for __clk_set_parent() below.
     *
     * 2. enable two parents clock for .set_parent() operation if finding
     * flag CLK_SET_PARENT_ON
     */
    if (clk->ref_count || (clk->flags & CLK_SET_PARENT_ON)) {
        __clk_enable(parent);
        if (clk->ref_count) {
            __clk_enable(clk);
        } else {
            __clk_enable(old_parent);
        }
    }

    /* update the clk tree topology */
#if 0
    flags = clk_enable_lock();
#endif 
    __clk_reparent(clk, parent);
#if 0
    clk_enable_unlock(flags);
#endif 

    return old_parent;
}

static void __clk_set_parent_after(struct clk *clk, struct clk *parent,
        struct clk *old_parent)
{
    /*
     * Finish the migration of prepare state and undo the changes done
     * for preventing a race with __clk_enable().
     */
    if (clk->ref_count || (clk->flags & CLK_SET_PARENT_ON )) {
        if (clk->ref_count) {
            int         ref_count;
            ref_count = clk->ref_count;
            while (ref_count) {
                __clk_disable(old_parent);
                __clk_enable(parent);
                ref_count --;
            }
            __clk_disable(clk);
        } else {
            __clk_disable(old_parent);
        }
        __clk_disable(parent);
    }
}

static aw_err_t __clk_set_parent_do(struct clk *clk, struct clk *parent)
{
    int                     ret = -AW_ENOSYS;
    struct clk             *old_parent;

    old_parent = __clk_set_parent_before(clk, parent);

    /* change clock input source */
    if (parent && clk->ops->set_parent) {
        ret = clk->ops->set_parent(clk,__clk_get_id(parent) );
    }

    if (ret) {
        __clk_reparent(clk, old_parent);
        if (clk->ref_count || (clk->flags & CLK_SET_PARENT_ON) ) {
            __clk_disable(parent);
            if (clk->ref_count) {
                __clk_disable(clk);
            } else {
                __clk_disable(old_parent);
            }
        }

        return ret;
    }

    __clk_set_parent_after(clk, parent, old_parent);

    return 0;
}

static aw_err_t __clk_set_parent(struct clk *clk, struct clk *parent)
{
    struct clk             *child;
    int                     ret = 0;
    int                     p_index = 0;
    unsigned long           p_rate = 0;
    struct aw_hlist_node   *p_node;

    
    if (clk->flags & CLK_CANNOT_CHANGE) {
        while(1);
    }

    if (NULL == clk || !clk->ops || NULL == parent) {
        return -AW_EINVAL;
    }


    /* verify ops for for multi-parent clks */
    if ( NULL == clk->ops || NULL == clk->ops->set_parent ) {
        return -AW_ENOSYS;
    }

    /* prevent racing with updates to the clock topology */

    if (clk->parent == parent)
        goto out;

    /* check that we are allowed to re-parent if the clock is in use */
    if ((clk->flags & CLK_SET_PARENT_GATE) && clk->ref_count) {
        ret = -AW_EBUSY;
        goto out;
    }
#ifndef CONFIG_CLK_NO_CACHE_CHILD
    /* check two consecutive basic mux clocks */
    /* 因为不能出现两个连续的basic mux时钟 */
    if (clk->flags & CLK_IS_BASIC_MUX) {
        aw_hlist_for_each_entry(child,p_node,&clk->children,struct clk,child_node) {
            if (child->flags & CLK_IS_BASIC_MUX) {
                ret = -AW_EBUSY;
                goto out;
            }
        }
    }
#endif
    /* propagate PRE_RATE_CHANGE notifications */
    /* abort if a driver objects */
    /* do the re-parent */
    ret = __clk_set_parent_do(clk, parent);

    /* propagate rate an accuracy recalculation accordingly */
    if (AW_OK == ret) {
        __clk_recalc_rate(clk);
    }

out:

#if 0
    clk_prepare_unlock();
#endif 
    return ret;
}

#ifndef CONFIG_CLK_NO_SET_RATE_RECURSIVE
static void __clk_calc_subtree(struct clk *clk, unsigned long new_rate,
                 struct clk *new_parent)
{
    struct clk             *child;
    struct aw_hlist_node   *p_node;

    clk->new_rate = new_rate;
    clk->new_parent = new_parent;

    /* include clk in new parent's PRE_RATE_CHANGE notifications */
    clk->new_child = NULL;
    if (new_parent && new_parent != clk->parent) {
        new_parent->new_child = clk;
    }

    aw_hlist_for_each_entry(child,p_node,&clk->children,struct clk,child_node) {
        if (child->ops->recalc_rate) {
            child->new_rate = child->ops->recalc_rate(child, new_rate);
        }
        else {
            child->new_rate = new_rate;
        }
        __clk_calc_subtree(child, child->new_rate, NULL);
    }
}

/*
 * calculate the new rates returning the topmost clock that has to be
 * changed.
 */
static struct clk *__clk_calc_new_rates(struct clk *clk, unsigned long rate)
{
    struct clk         *top = clk;
    struct clk         *old_parent, *parent;
    unsigned long       best_parent_rate = 0;
    unsigned long       new_rate;
    aw_clk_id_t         parent_id = -1;

    /* sanity */
    if (NULL == clk || NULL == clk->ops)
        return NULL;

    /* save parent rate, if it exists */
    parent = old_parent = clk->parent;
    if (parent) {
        best_parent_rate = __clk_get_rate(parent);
    }

    /* find the closest rate and parent clk/rate */
    if (clk->ops->determine_rate) {
        new_rate = clk->ops->determine_rate(clk, rate,
                            &best_parent_rate,
                            &parent_id);
        parent = __clk_find_by_id(parent_id);
    } else if (clk->ops->round_rate) {
        new_rate = clk->ops->round_rate(clk, rate,
                        &best_parent_rate);
    } else if (!parent || !(clk->flags & CLK_SET_RATE_PARENT)) {
        /* pass-through clock without adjustable parent */
        clk->new_rate = __clk_get_rate(clk);
        return NULL;
    } else {
        /* pass-through clock with adjustable parent */
        top = __clk_calc_new_rates(parent, rate);
        new_rate = parent->new_rate;
        goto out;
    }

    /* some clocks must be gated to change parent */
    if (parent != old_parent &&
        (clk->flags & CLK_SET_PARENT_GATE) && clk->ref_count) {
        return NULL;
    }

    /* try finding the new parent index */


    if ((clk->flags & CLK_SET_RATE_PARENT) && parent &&
        best_parent_rate != __clk_get_rate(parent) ) {
        top = __clk_calc_new_rates(parent, best_parent_rate);
    }

out:
    __clk_calc_subtree(clk, new_rate, parent);
    return top;
}

/*
 * walk down a subtree and set the new rates notifying the rate
 * change on the way
 */
static void __clk_change_rate(struct clk *clk)
{
    struct clk *child;
    struct aw_hlist_node *tmp;
    unsigned long old_rate;
    unsigned long best_parent_rate = 0;
    aw_bool_t skip_set_rate = AW_FALSE;
    struct clk *old_parent;
    struct clk *parent = NULL;
    struct aw_hlist_node   *p_node;

    old_rate = __clk_get_rate(clk);

    if (clk->new_parent) {
        parent = clk->new_parent;
        best_parent_rate = __clk_get_rate(clk->new_parent);
    } else if (clk->parent) {
        parent = clk->parent;
        best_parent_rate = __clk_get_rate(clk->parent);
    }

    if (clk->new_parent && clk->new_parent != clk->parent) {
        old_parent = __clk_set_parent_before(clk, clk->new_parent);

        if ( 0 /* clk->ops->set_rate_and_parent*/) {
#if 0
            skip_set_rate = true;
            clk->ops->set_rate_and_parent(clk->hw, clk->new_rate,
                    best_parent_rate,
                    clk->new_parent_index);
#endif 
        } else if (clk->ops->set_parent) {
            clk->ops->set_parent(clk, __clk_get_id(clk->new_parent));
        }

        __clk_set_parent_after(clk, clk->new_parent, old_parent);
    }

    if ( (clk->flags & CLK_SET_PARENT_ON) && parent) {
#if 0
        __clk_prepare(parent);
#endif 
        __clk_enable(parent);
    }

    if (!skip_set_rate && clk->ops->set_rate)
        clk->ops->set_rate(clk, clk->new_rate, best_parent_rate);
#ifndef CONFIG_CLK_NO_CACHE_RATE
    if (clk->ops->recalc_rate)
        clk->rate = clk->ops->recalc_rate(clk, best_parent_rate);
    else
        clk->rate = best_parent_rate;
#endif
    if ( (clk->flags & CLK_SET_PARENT_ON) && parent) {
    }
    /*
     * Use safe iteration, as change_rate can actually swap parents
     * for certain clock types.
     */
    aw_hlist_for_each_entry_safe(child, p_node,tmp, &clk->children, struct clk,child_node) {
        /* Skip children who will be reparented to another clock */
        if (child->new_parent && child->new_parent != clk)
            continue;
        __clk_change_rate(child);
    }

    /* handle the new child who might not be in clk->children yet */
    if (clk->new_child) {
        __clk_change_rate(clk->new_child);
    }
}

/**
 * __clk_set_rate - specify a new rate for clk
 * @clk: the clk whose rate is being changed
 * @rate: the new rate for clk
 *
 * In the simplest case __clk_set_rate will only adjust the rate of clk.
 *
 * Setting the CLK_SET_RATE_PARENT flag allows the rate change operation to
 * propagate up to clk's parent; whether or not this happens depends on the
 * outcome of clk's .round_rate implementation.  If *parent_rate is unchanged
 * after calling .round_rate then upstream parent propagation is ignored.  If
 * *parent_rate comes back with a new rate for clk's parent then we propagate
 * up to clk's parent and set its rate.  Upward propagation will continue
 * until either a clk does not support the CLK_SET_RATE_PARENT flag or
 * .round_rate stops requesting changes to clk's parent_rate.
 *
 * Rate changes are accomplished via tree traversal that also recalculates the
 * rates for the clocks and fires off POST_RATE_CHANGE notifiers.
 *
 * Returns 0 on success, -EERROR otherwise.
 */
static aw_err_t __clk_set_rate(struct clk *clk, aw_clk_rate_t rate)
{
    aw_err_t                ret = 0;
    struct clk             *top;
    
    
    /* bail early if nothing to do */
    if (rate == __clk_get_rate(clk)) {
        goto out;
    }
    
    if (clk->flags & CLK_CANNOT_CHANGE) {
        while(1);
    }

    if ((clk->flags & CLK_SET_RATE_GATE) && clk->ref_count) {
        ret = -AW_EBUSY;
        goto out;
    }

    top = __clk_calc_new_rates(clk, rate);
    if (!top) {
        ret = -AW_EINVAL;
        goto out;
    }
    /* change the rates */
    __clk_change_rate(top);

out:
    return ret;
}

#else

static aw_err_t __clk_set_rate(struct clk *clk, aw_clk_rate_t rate)
{
    aw_err_t                ret = -AW_ENOTSUP;
    struct clk             *top;
    unsigned long           best_parent_rate = 0;
    unsigned long           old_parent_rate;
    aw_clk_rate_t           new_rate = rate;

    
    /* bail early if nothing to do */
    if (rate == __clk_get_rate(clk)) {
        ret = 0;
        goto out;
    }
    
    if (clk->flags & CLK_CANNOT_CHANGE) {
        while(1);
    }

    if ((clk->flags & CLK_SET_RATE_GATE) && clk->ref_count) {
        ret = -AW_EBUSY;
        goto out;
    }

    best_parent_rate = __clk_get_rate(clk->parent);
    old_parent_rate = best_parent_rate;
    
    if (clk->ops->round_rate) {
        new_rate = clk->ops->round_rate(clk, rate,
                        &best_parent_rate);
    }

    if (best_parent_rate != old_parent_rate) {
        ret = -AW_ENOTSUP;
        goto out;
    }
    
    if (NULL == clk->ops->set_rate) {
        ret = -AW_ENOSYS;
        goto out;
    }

    ret = clk->ops->set_rate(clk,new_rate,best_parent_rate);
    if (0 != ret) {
        goto out;
    }
   
    __clk_recalc_rate(clk);
out:
    return ret;
}

#endif


void aw_clk_init(struct clk * clk_base[],int count)
{
    __g_clk_base = clk_base;
    __g_clk_count = count;
    aw_spinlock_isr_init(&__clk_spinlock,0);
}

aw_clk_rate_t clk_get_rate(struct clk *hw)
{
    aw_clk_rate_t       rate;

    aw_spinlock_isr_take(&__clk_spinlock);
    rate = __clk_get_rate(hw);
    aw_spinlock_isr_give(&__clk_spinlock);
    return rate;
}


aw_err_t clk_set_parent(struct clk *clk, struct clk *parent)
{
    aw_err_t        err;

    aw_spinlock_isr_take(&__clk_spinlock);
    err = __clk_set_parent(clk,parent);
    aw_spinlock_isr_give(&__clk_spinlock);
    return err;
}

aw_err_t clk_enable(struct clk *hw)
{
    aw_err_t        err;

    aw_spinlock_isr_take(&__clk_spinlock);
    err = __clk_enable(hw);
    aw_spinlock_isr_give(&__clk_spinlock);
    return err;
}

aw_err_t clk_disable(struct clk *hw)
{
    aw_err_t        err;

    aw_spinlock_isr_take(&__clk_spinlock);
    err = __clk_disable(hw);
    aw_spinlock_isr_give(&__clk_spinlock);
    return err;
}

aw_err_t clk_set_rate(struct clk *clk, aw_clk_rate_t rate)
{
    aw_err_t        err;

    aw_spinlock_isr_take(&__clk_spinlock);
    err = __clk_set_rate(clk,rate);
    aw_spinlock_isr_give(&__clk_spinlock);
    return err;
}

#ifdef CONFIG_CLK_ENABLE_SAVE_CONTEXT
static aw_err_t __clk_save_context(uint8_t *p_buf,int size)
{
    aw_err_t                err = 0;
    int                     i;
    struct clk             *hw;
    const struct clk_ops   *p_ops;
    int                     len = 0;

    for( i = 0;i< __g_clk_count;i++) {

        hw = __g_clk_base[i];
        if (NULL != hw) {
            p_ops = hw->ops;
            if (NULL != p_ops->save_context) {
                err = p_ops->save_context(hw,p_buf + len,size- len);
                if (err < 0) {
                    goto cleanup;
                }
                len += err;
            }
        }
    }
    
cleanup:
    return err;
}

aw_err_t clk_save_context(uint8_t *p_buf,int size)
{
    aw_err_t        err;
    
    if (NULL == p_buf || size < 0 ) {
        return -AW_EINVAL;
    }

    aw_spinlock_isr_take(&__clk_spinlock);
    err = __clk_save_context(p_buf,size);
    aw_spinlock_isr_give(&__clk_spinlock);
    return err;
}

aw_err_t __clk_restore_context(uint8_t *p_buf,int size)
{
    aw_err_t                    err = 0;
    int                         i;
    struct clk                 *hw;
    const struct clk_ops       *p_ops;
    int                         len = 0;

    for( i = 0;i< __g_clk_count;i++) {

        hw = __g_clk_base[i];
        if (NULL != hw) {
            p_ops = hw->ops;
            if (NULL != p_ops->restore_context) {
                err = p_ops->restore_context(hw,p_buf + len,size- len);
                if (err < 0) {
                    goto cleanup;
                }
                len += err;
            }
        }
    }

cleanup:
    return err;
}

aw_err_t clk_restore_context(uint8_t *p_buf,int size)
{
    aw_err_t        err;

    if (NULL == p_buf ) {
        return -AW_EINVAL;
    }

    aw_spinlock_isr_take(&__clk_spinlock);
    err = __clk_restore_context(p_buf,size);
    aw_spinlock_isr_give(&__clk_spinlock);

    return err;
}
#endif

/* end of file */
