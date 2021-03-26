/*******************************************************************************
*                                 AWORKS
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief 操作系统平台 C++ 内存支持 (重载 new, delete 运算符).
 *         (为内核模块提供简单的 C++ 支持, 应用程序则使用 libstdc++ or libsupc++)
 *
 * \internal
 * \par modification history
 * - 1.00 2016-03-10  cod, first implementation
 * \endinternal
 */
#include "aw_mem.h"

/**
 * \brief 重载 new 运算.
 *
 * \param[in] st_size   : 内存大小
 *
 * \return :  分配的内存指针.
 */
void  *operator  new (size_t   st_size)
{
    return ((void *)aw_mem_alloc(st_size));
}

/**
 * \brief 重载 new 运算.
 *
 * \param[in] st_size   : 内存大小
 *
 * \return :  分配的内存指针.
 */
void  *operator  new[] (size_t   st_size)
{
    return ((void *)aw_mem_alloc(st_size));
}

/**
 * \brief 重载 delete 运算.
 *
 * \param[in] p_mem : 需要释放的内存首指针
 *
 */
void  operator  delete (void  *p_mem)
{
    if (p_mem) {
        aw_mem_free(p_mem);
    }
}

/**
 * \brief 重载 delete 运算.
 *
 * \param[in] p_mem : 需要释放的内存首指针
 *
 */
void  operator  delete[] (void  *p_mem)
{
    if (p_mem) {
        aw_mem_free(p_mem);
    }
}

/* end of file */
