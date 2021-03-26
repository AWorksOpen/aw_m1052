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
 * \brief AWorksƽ̨ C++ run time eabi(ARM) lib.
 *        See http://www.codesourcery.com/public/cxx-abi/abi.html.
 *
 * \attention: ����������ʵ�֣���Ŀǰ������������.
 *
 * \internal
 * \par modification history
 * - 1.00 2016-03-10  cod, first implementation
 * \endinternal
 */
#include "apollo.h"
#include "string.h"
/******************************************************************************
  C ��������
*******************************************************************************/
extern "C" {

/* CXA �������� */
extern int  __cxa_atexit(void (*f)(void *), void *p, void *d);

/**
 * Temporary hack: this variable should not be part of the C library
 * itself, but placed in the .bss section of each executable or
 * shared library instead.
 *
 * We keep it here temporarily until the build system has been modified properly
 * to use crtbegin_so.S and crtend_so.S when generating shared libraries.
 * It must be a 'weak' symbol to avoid conflicts with the definitions
 * that have been moved to crtbegin_static.S and crtbegin_dynamic.S
 *
 * For the record, it is used for static C++ object construction and destruction
 * See http://www.codesourcery.com/public/cxx-abi/abi.html#dso-dtor
 */
void *__attribute__((weak)) __dso_handle = &__dso_handle;

/*******************************************************************************
  C++ �����Ϊ����: ȫ�ֶ���, ��̬�ֲ�����, ��̬����
  
  ȫ�ֶ���:������ C �����е�ȫ�ֱ���, һ���� main ����ִ��ǰ, ��Ҫ��ʼ��ȫ�ֱ���.
                   ͬ��, C++ ����Ҫ�������е�һ�� C++ ����ǰ, ���빹�����е�ȫ�ֶ���,
                   ������������� C++ ȫ�ֶ����캯���Ժ���ָ���ķ�ʽ�������½���:
        .ctors .ctors.* .preinit_array .init_array ��. AWorksƽ̨��ִ���û�����֮ǰ
                   ���Զ�����__cppRtDoCtors() ������ִ�����е�ȫ�ֶ����캯��(������μ���ش���).
                   ͬ��, ��Ӧ�ó����˳�ʱ��Ҫ��������ȫ�ֶ���, ���������ֿ�����:
  
  ��һ: ������������� C++ ȫ�ֶ������������Ժ���ָ���ķ�ʽ�������½���:
      .dtors .dtors.* .fini_array ��. ���е�ȫ�ֶ�����������.
  �ڶ�: ���������Զ��ڹ��캯�������һ���ִ���, ������: ִ���깹�캯��, ������������������, ����������
               ͨ�� __aeabi_atexit ѹ�� atexit()ջ��, ����AWorksƽ̨Ӧ��ֹͣʱ������
      __cxa_finalize()����, ִ�����ж������������.
  
  ��̬�ֲ�����:(������ο� cppSupLib.cpp ������)���ڵ�һ�ε�����������ʱ������, ������Ϻ�Ὣ�Ķ���
                             ����������ͨ�� __aeabi_atexit ѹ�� atexit() ջ��, AWorksƽ̨Ӧ��ֹͣʱ������
            __cxa_finalize() ����, ִ�����ж������������.
  
  ��̬����:��ͨ��ִ�� new �� delete ���������ε�ִ�й��������������, ����ϵͳ��û������Ĵ���.
*******************************************************************************/

/**
 * \brief eabi ���� __cxa_finalize ʱ��Ҫ���еĻص�����.
 *
 * \param[in] object     : ����
 * \param[in] destructor : ����ָ��
 * \param[in] dso_handle : ���
 *
 * \retval 0  : �ɹ�
 * \retval -1 : ʧ��
 */
int __attribute__((weak)) __aeabi_atexit (void *object,
                                          void (*destructor) (void *),
                                          void *dso_handle)
{
    return  __cxa_atexit(destructor, object, dso_handle);
}

/**
 * \brief memcpy8
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memcpy8 (void *dest, const void *src, size_t n)
{
    memcpy(dest, src, n);
}

/**
 * \brief memcpy4
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memcpy4 (void *dest, const void *src, size_t n) 
{
    memcpy(dest, src, n);
}

/**
 * \brief memcpy
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memcpy (void *dest, const void *src, size_t n) 
{
    memcpy(dest, src, n);
}

/**
 * \brief __aeabi_memmove8
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memmove8 (void *dest, const void *src, size_t n) 
{
    memcpy(dest, src, n);
}

/**
 * \brief memmove4
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memmove4 (void *dest, const void *src, size_t n) 
{
    memcpy(dest, src, n);
}

/**
 * \brief memmove
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memmove (void *dest, const void *src, size_t n) 
{
    memcpy(dest, src, n);
}

/**
 * \brief memset8
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memset8 (void *dest, size_t n, int c)
{
    memset(dest, c, n);
}

/**
 * \brief memset4
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memset4 (void *dest, size_t n, int c)
{
    memset(dest, c, n);
}

/**
 * \brief memset
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memset (void *dest, size_t n, int c)
{
    memset(dest, c, n);
}


/**
 * \brief memclr8
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memclr8 (void *dest, size_t n)
{
    __aeabi_memset8(dest, n, 0);
}

/**
 * \brief memclr4
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memclr4(void *dest, size_t n)
{
    __aeabi_memset4(dest, n, 0);
}

/**
 * \brief memclr
 *
 * \param[in] dest : Ŀ��
 * \param[in] src  : Դ
 * \param[in] n    : ����
 *
 */
void __aeabi_memclr(void *dest, size_t n)
{
    __aeabi_memset(dest, n, 0);
}
}
/* end of file */
