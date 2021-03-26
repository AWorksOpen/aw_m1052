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
 * \brief AWorks平台 C++ run time eabi(ARM) lib.
 *        See http://www.codesourcery.com/public/cxx-abi/abi.html.
 *
 * \attention: 析构处理已实现，但目前不做析构处理.
 *
 * \internal
 * \par modification history
 * - 1.00 2016-03-10  cod, first implementation
 * \endinternal
 */
#include "apollo.h"
#include "string.h"
/******************************************************************************
  C 环境函数
*******************************************************************************/
extern "C" {

/* CXA 函数声明 */
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
  C++ 对象分为三类: 全局对象, 静态局部对象, 动态对象
  
  全局对象:类似于 C 程序中的全局变量, 一般在 main 程序执行前, 需要初始化全局变量.
                   同样, C++ 程序要求在运行第一个 C++ 程序前, 必须构造所有的全局对象,
                   编译器会把所有 C++ 全局对象构造函数以函数指针表的方式放在以下节区:
        .ctors .ctors.* .preinit_array .init_array 中. AWorks平台在执行用户程序之前
                   会自动调用__cppRtDoCtors() 函数来执行所有的全局对象构造函数(详情请参见相关代码).
                   同样, 在应用程序退出时需要析构所有全局对象, 这里有两种可能性:
  
  第一: 编译器会把所有 C++ 全局对象析构函数以函数指针表的方式放在以下节区:
      .dtors .dtors.* .fini_array 中. 所有的全局对象析构函数.
  第二: 编译器会自动在构造函数后添加一部分代码, 功能是: 执行完构造函数, 如果对象存在析构函数, 将析构函数
               通过 __aeabi_atexit 压入 atexit()栈区, 所以AWorks平台应在停止时将调用
      __cxa_finalize()函数, 执行所有对象的析构函数.
  
  静态局部对象:(函数请参考 cppSupLib.cpp 中事例)是在第一次调用容器函数时被构造, 构造完毕后会将改对象
                             的析构函数通过 __aeabi_atexit 压入 atexit() 栈区, AWorks平台应在停止时将调用
            __cxa_finalize() 函数, 执行所有对象的析构函数.
  
  动态对象:是通过执行 new 和 delete 操作来隐形的执行构造和析构函数的, 操作系统并没有特殊的处理.
*******************************************************************************/

/**
 * \brief eabi 设置 __cxa_finalize 时需要运行的回调方法.
 *
 * \param[in] object     : 参数
 * \param[in] destructor : 函数指针
 * \param[in] dso_handle : 句柄
 *
 * \retval 0  : 成功
 * \retval -1 : 失败
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
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memcpy8 (void *dest, const void *src, size_t n)
{
    memcpy(dest, src, n);
}

/**
 * \brief memcpy4
 *
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memcpy4 (void *dest, const void *src, size_t n) 
{
    memcpy(dest, src, n);
}

/**
 * \brief memcpy
 *
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memcpy (void *dest, const void *src, size_t n) 
{
    memcpy(dest, src, n);
}

/**
 * \brief __aeabi_memmove8
 *
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memmove8 (void *dest, const void *src, size_t n) 
{
    memcpy(dest, src, n);
}

/**
 * \brief memmove4
 *
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memmove4 (void *dest, const void *src, size_t n) 
{
    memcpy(dest, src, n);
}

/**
 * \brief memmove
 *
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memmove (void *dest, const void *src, size_t n) 
{
    memcpy(dest, src, n);
}

/**
 * \brief memset8
 *
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memset8 (void *dest, size_t n, int c)
{
    memset(dest, c, n);
}

/**
 * \brief memset4
 *
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memset4 (void *dest, size_t n, int c)
{
    memset(dest, c, n);
}

/**
 * \brief memset
 *
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memset (void *dest, size_t n, int c)
{
    memset(dest, c, n);
}


/**
 * \brief memclr8
 *
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memclr8 (void *dest, size_t n)
{
    __aeabi_memset8(dest, n, 0);
}

/**
 * \brief memclr4
 *
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memclr4(void *dest, size_t n)
{
    __aeabi_memset4(dest, n, 0);
}

/**
 * \brief memclr
 *
 * \param[in] dest : 目标
 * \param[in] src  : 源
 * \param[in] n    : 个数
 *
 */
void __aeabi_memclr(void *dest, size_t n)
{
    __aeabi_memset(dest, n, 0);
}
}
/* end of file */
