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
 * \file  rtk.h
 * \brief rtk is light weigh and powerful real time scheduler since 2012.
 *
 * \internal
 * \par modification history:
 * - 0.9 12-08-01 sni first version.
 * \endinternal
 */

#ifndef __RTK_MEM_H
#define __RTK_MEM_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "aw_types.h"

void rtk_mem_init(void);

void *rtk_sys_mem_alloc(size_t size);
void *rtk_sys_mem_align(size_t size,size_t align);
void rtk_sys_mem_free(void *ptr);

void *rtk_usr_mem_alloc(size_t size);
void *rtk_usr_mem_align(size_t size,size_t align);
void rtk_usr_mem_free(void *ptr);

size_t rtk_sys_mem_get_free_size(void);
size_t rtk_usr_mem_get_free_size(void);

#ifdef __cplusplus
}
#endif  /*  __cplusplus  */


#endif

