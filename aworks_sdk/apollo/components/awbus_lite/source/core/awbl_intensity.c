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
 * \brief AWBus intensity interface implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.01 14-11-01  ops, redefine the intensity device by using new specification.
 * - 1.00 14-07-03  ops, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_assert.h"
#include "aw_intensity.h"

aw_err_t aw_it_level_set(aw_it_dev_t* p_dev, int level)
{

    if (NULL == p_dev) {
        return -AW_ENXIO;
    }

    return p_dev->pfn_it_level_set(p_dev, level);
}

aw_err_t aw_it_level_get(aw_it_dev_t* p_dev)
{

    if (NULL == p_dev) {
        return -AW_ENXIO;
    }

    return p_dev->pfn_it_level_get(p_dev);
}

/* end of file*/

