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
 * \brief AWorks平台 C++ run time 全局对象构建与析构操作库.
 *
 * \internal
 * \par modification history
 * - 1.00 2016-03-10  cod, first implementation
 * \endinternal
 */
#include "apollo.h"

extern "C" {
/*******************************************************************************
  说明:
  
  编译器(gcc)编译 C++ 工程时, 如果存在全局对象, 那么全局对象的构建函数指针会放在可执行 elf 文件的
 .ctors节区(section), 析构函数会放在可执行 elf 文件的 .dtors 节区,
   一般标准 gcc 库会引出四个符号:
  __CTOR_LIST__
  __CTOR_END__
  __DTOR_LIST__
  __DTOR_END__
   
  其中 __CTOR_LIST__ 表示所有的全局对象构造函数指针数组的首地址, 起始指针为 0xFFFFFFFF,之后的每一项
  为一个构造函数的入口, 直到 __CTOR_END__ 为止, __CTOR_END__ 指向的函数指针为 0x00000000
  
  其中 __DTOR_LIST__ 表示所有的全局对象析构函数指针数组的首地址, 起始指针为 0xFFFFFFFF,之后的每一项
  为一个析构函数的入口, 直到 __DTOR_END__ 为止, __DTOR_END__ 指向的函数指针为 0x00000000
  
  一下代码就实现了这个 4 个符号类似的定义. 这样系统就可以在运行用户程序之前, 初始化 C++ 环境, 运行全局
  对象的构造函数, 在系统 reboot 时, 运行系统的析构函数.
  
  如果要让这些符号处于对应 .ctors 和 .dtors 节区指定的位置, 则需要在连接文件加入一下代码:
  
  .ctors :
  {
      KEEP (*cppRtBegin*.o(.ctors))
      KEEP (*(.preinit_array))
      KEEP (*(.init_array))
      KEEP (*(SORT(.ctors.*)))
      KEEP (*(.ctors))
      KEEP (*cppRtEnd*.o(.ctors))
  }
  
  .dtors :
  {
      KEEP (*cppRtBegin*.o(.dtors))
      KEEP (*(.fini_array))
      KEEP (*(SORT(.dtors.*)))
      KEEP (*(.dtors))
      KEEP (*cppRtEnd*.o(.dtors))
  }
  
  以上链接脚本, 将需要的符号定义到了 .ctors .dtors 节区对应的位置 (分别定义到了这两个节区的首尾)
  (其中 .init_array 和 .fini_array 分别是构建和析构具有静态存储时限的对象)
  
  注意:
  
  由于操作系统是在调用用户之前, 就运行了全局对象构造函数, 此时并没有进入多任务环境, 所以对象的构造函数一定
  要足够的简单, 一般仅是用来初始化对象的属性和一些基本数据结构, 更多的操作可以在类中加入专门的初始化方法
  来实现.
*******************************************************************************/
typedef void (*__aw_pfunc_t) (...);

/** \brief C++ 全局对象构建与析构函数表 (为了和一些编译器不产生冲突, 这里使用AWorks 自带的符号 */
extern __aw_pfunc_t __AW_CTOR_LIST__;
extern __aw_pfunc_t __AW_DTOR_LIST__;
extern __aw_pfunc_t __AW_CTOR_END__;
extern __aw_pfunc_t __AW_DTOR_END__;
/******************************************************************************/
/**
 * \brief C++ 运行全局对象构造函数
 */
void  __cppRtDoCtors (void)
{
#ifdef __GNUC__
    volatile __aw_pfunc_t      *pp_func;
    __aw_pfunc_t                p_func;

    pp_func = &__AW_CTOR_LIST__;
    while(pp_func < &__AW_CTOR_END__) {
        p_func = *pp_func;
        if(p_func != NULL && p_func != (__aw_pfunc_t)-1 ) {
            p_func();
        }
        pp_func ++;
    }
    
#endif   /*  __GNUC__                    */
}
/******************************************************************************/
/**
 * \brief  C++ 运行全局对象析构函数
 */
void  __cppRtDoDtors (void)
{
#ifdef __GNUC__
    volatile __aw_pfunc_t      *pp_func;
    __aw_pfunc_t                p_func;

    pp_func = &__AW_DTOR_LIST__;
    while(pp_func < &__AW_DTOR_END__) {
        p_func = *pp_func;
        if(p_func != NULL && p_func != (__aw_pfunc_t)-1 ) {
            p_func();
        }
        pp_func ++;
    }

#endif   /*  __GNUC__                    */
}
}
/* end of file */
