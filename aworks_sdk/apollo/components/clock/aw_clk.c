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

#include <apollo.h>
#include "clk_provider.h"
#include <string.h>



aw_clk_rate_t aw_clk_rate_get(aw_clk_id_t clk_id)
{
    struct clk         *hw;
    hw = __clk_find_by_id(clk_id);
    return clk_get_rate(hw);
}


aw_err_t aw_clk_parent_set(aw_clk_id_t clk_id, aw_clk_id_t parent_id)
{
    struct clk         *hw,*parent;
    aw_err_t            ret = -AW_EINVAL;

    hw = __clk_find_by_id(clk_id);
    parent = __clk_find_by_id(parent_id);

    if (NULL == hw || NULL == parent) {
        goto cleanup;
    }

    ret = clk_set_parent(hw,parent);
cleanup:
    return ret;
}

aw_err_t aw_clk_enable(aw_clk_id_t clk_id)
{
    struct clk         *hw;

    hw = __clk_find_by_id(clk_id);
    if (NULL == hw) {
        return -AW_EINVAL;
    }
    return clk_enable(hw);
}

aw_clk_id_t aw_clk_parent_get(aw_clk_id_t clk_id)
{
    struct clk         *hw;

    hw = __clk_find_by_id(clk_id);
    if (NULL == hw) {
        return -AW_EINVAL;
    }
    hw = hw->parent;
    return __clk_get_id(hw);
}

aw_err_t aw_clk_disable(aw_clk_id_t clk_id)
{
    struct clk         *hw;

    hw = __clk_find_by_id(clk_id);
    if (NULL == hw) {
        return -AW_EINVAL;
    }
    return clk_disable(hw);
}

aw_err_t aw_clk_rate_set(aw_clk_id_t clk_id, aw_clk_rate_t rate)
{
    struct clk         *hw;

    hw = __clk_find_by_id(clk_id);
    if (NULL == hw) {
        return -AW_EINVAL;
    }
    return clk_set_rate(hw,rate);
}

#if CONFIG_CLK_ENABLE_SAVE_CONTEXT
aw_err_t aw_clk_save_context(uint8_t *p_buf,int size)
{
    return clk_save_context(p_buf,size);
}

aw_err_t aw_clk_restore_context(uint8_t *p_buf,int size)
{
    return clk_restore_context(p_buf,size);
}

#endif

/* end of file */
