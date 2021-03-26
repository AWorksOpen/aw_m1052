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
 * \brief relay driver.
 *
 * \internal
 * \par modification history
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */


/**
 * \addtogroup aw_relay
 * @{
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "driver/relay/awbl_relay.h"
#include "aw_relay.h"
#include "aw_common.h"

/******************************************************************************/

/** @brief turn on relay */
void aw_relay_on (unsigned int id)
{
    awbl_relay_set(id, AW_TRUE);
}

/******************************************************************************/

/** @brief turn off relay */
void aw_relay_off (unsigned int id)
{
    awbl_relay_set(id, AW_FALSE);
}

/** @} */

/* end of file */
