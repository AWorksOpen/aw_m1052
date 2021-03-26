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
 * \brief NTC 热敏电阻驱动程序.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */

#include "driver/thermistor/awbl_temp.h"
#include "aw_compiler.h"

/******************************************************************************/

/** \brief 服务链表头*/
struct awbl_temp_service *__gp_temp_serv_head = NULL;

/** \brief 添加服务 */
aw_err_t awbl_temp_serv_add (struct awbl_temp_service *p_temp_serv)
{
    struct awbl_temp_service *p_tmp_serv = __gp_temp_serv_head; 
    
    /* 在链表头插入 */
    if (p_temp_serv != NULL) {
        if (NULL == __gp_temp_serv_head) {
            __gp_temp_serv_head = p_temp_serv;
        } else {
            while (p_tmp_serv != NULL) {
                if (p_tmp_serv->p_next == NULL) {
                    p_tmp_serv->p_next = p_temp_serv;
                    break;
                }
                p_tmp_serv = p_tmp_serv->p_next;
            }
        }
        p_temp_serv->p_next = NULL;
        return AW_OK;
    }
    return -AW_EINVAL;
}

/******************************************************************************/
aw_local struct awbl_temp_service * __temp_id_get_serv (unsigned int id)
{
    struct awbl_temp_service *p_serv_cur = __gp_temp_serv_head;
    while ((p_serv_cur != NULL)) {
        if (id == p_serv_cur->id) {
            return p_serv_cur;
        }
        p_serv_cur = p_serv_cur->p_next;
    }
    return NULL;
}

/******************************************************************************/
int aw_temp_read (unsigned int id)
{
    struct awbl_temp_service *p_temp_serv = NULL;

    p_temp_serv = __temp_id_get_serv(id);
    if (NULL == p_temp_serv) {
        return -AW_ENXIO;
    }
    
    return p_temp_serv->p_servfuncs->pfunc_read(p_temp_serv);
}

/* end of file */
