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
 * \brief PPP SIO管理
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 15-01-12  ops, first implementation.
 * \endinternal
 */

#include "ppp/sio_mgn.h"
#include "lwip/arch.h"

aw_local struct aw_list_head __g_sio_list_head;

void sio_list_init(void)
{
    aw_list_head_init(&__g_sio_list_head);
}

void sio_list_add(struct sio_mgn *p_sio)
{
    aw_list_add_tail(&p_sio->elm, &__g_sio_list_head);
}

void sio_list_del(struct sio_mgn *p_sio)
{
    aw_list_del(&p_sio->elm);
}

struct sio_mgn* sio_list_iterator(int serial_fd)
{
    struct sio_mgn *p_cur = NULL;
    struct aw_list_head *p_pos = NULL;

    if (aw_list_empty(&__g_sio_list_head)) {

        AW_INFOF(("The sio list is empty. \n"));
        return NULL;
    }

    aw_list_for_each(p_pos, &__g_sio_list_head) {

        p_cur = aw_list_entry(p_pos, struct sio_mgn, elm);
#if 0 
        if (p_cur->serial_fd == serial_fd) {

            return p_cur;
        }
#endif 
        if ((int)p_cur->fd == serial_fd) {

            return p_cur;
        }
    }

    return NULL;
}

/* end of file */
