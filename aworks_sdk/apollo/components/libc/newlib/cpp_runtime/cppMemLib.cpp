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
 * \brief ����ϵͳƽ̨ C++ �ڴ�֧�� (���� new, delete �����).
 *         (Ϊ�ں�ģ���ṩ�򵥵� C++ ֧��, Ӧ�ó�����ʹ�� libstdc++ or libsupc++)
 *
 * \internal
 * \par modification history
 * - 1.00 2016-03-10  cod, first implementation
 * \endinternal
 */
#include "aw_mem.h"

/**
 * \brief ���� new ����.
 *
 * \param[in] st_size   : �ڴ��С
 *
 * \return :  ������ڴ�ָ��.
 */
void  *operator  new (size_t   st_size)
{
    return ((void *)aw_mem_alloc(st_size));
}

/**
 * \brief ���� new ����.
 *
 * \param[in] st_size   : �ڴ��С
 *
 * \return :  ������ڴ�ָ��.
 */
void  *operator  new[] (size_t   st_size)
{
    return ((void *)aw_mem_alloc(st_size));
}

/**
 * \brief ���� delete ����.
 *
 * \param[in] p_mem : ��Ҫ�ͷŵ��ڴ���ָ��
 *
 */
void  operator  delete (void  *p_mem)
{
    if (p_mem) {
        aw_mem_free(p_mem);
    }
}

/**
 * \brief ���� delete ����.
 *
 * \param[in] p_mem : ��Ҫ�ͷŵ��ڴ���ָ��
 *
 */
void  operator  delete[] (void  *p_mem)
{
    if (p_mem) {
        aw_mem_free(p_mem);
    }
}

/* end of file */
