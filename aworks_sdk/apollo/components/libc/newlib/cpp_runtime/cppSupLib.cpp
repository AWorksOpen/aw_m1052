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
 * \brief 操作系统平台 C++ run time support lib.
 *
 * \internal
 * \par modification history
 * - 1.00 2016-03-10  cod, first implementation
 * \endinternal
 */
#include "apollo.h"
#include "aw_list.h"
#include "aw_mem.h"
#include "aw_sem.h"

extern "C" {
/*******************************************************************************
  函数声明
*******************************************************************************/
extern void  __cppRtDoCtors(void);
extern void  __cppRtDoDtors(void);
extern void  __cppRtDummy(void);
/*******************************************************************************
  C++ func list
*******************************************************************************/
typedef struct {
    struct aw_list_head   line_manage;
    aw_pfuncvoid_t        func;
    void                 *p_arg;
    void                 *p_handle;
} aw_cpp_func_list_t;

aw_local struct aw_list_head  __g_linecpp_func_list;

AW_MUTEX_DECL_STATIC(__g_cpp_rt_lock);

#define __AW_CPP_RT_LOCK()   AW_MUTEX_LOCK(__g_cpp_rt_lock, AW_SEM_WAIT_FOREVER)
#define __AW_CPP_RT_UNLOCK() AW_MUTEX_UNLOCK(__g_cpp_rt_lock)
/******************************************************************************
  gurad 宏操作
*******************************************************************************/
#define __AW_CPP_GUARD_GET(p_igv)        (*((char *)(p_igv)))
#define __AW_CPP_GUARD_SET(p_igv, x)     (*((char *)(p_igv))  = x)

/*  应经被 construction 完毕    */
#define __AW_CPP_GUARD_MAKE_RELE(p_igv)  (*((char *)(p_igv)) |= 0x40)
#define __AW_CPP_GUARD_IS_RELE(p_igv)    (*((char *)(p_igv)) &  0x40)
/******************************************************************************
  __cxa_guard_acquire
  __cxa_guard_release
  __cxa_guard_abort
  
  函数组主要解决的是静态局部对象构造可重入问题, 
  例如以下程序:
  
  void  foo ()
  {
      static student   tom;
      
      ...
  }
  
  调用 foo() 函数, 编译器生成的代码需要首先检查对象 tom 是否被建立, 如果没有构造, 则需要构造 tom.
  但是为了防止多个任务同时调用 foo() 函数, 所以这里需要 __cxa_guard_acquire 和
 __cxa_guard_release 配合来实现可重入. 编译器生成的伪代码如下:
  
  ...
  if (__cxa_guard_acquire(&tom_stat) == 1) {
      ...
      construct tom
      ...
      __cxa_guard_release(&tom_stat);
  }
  
  其中 tom_stat 为编译器为 tom 对象分配的状态记录"变量".
*******************************************************************************/

/**
 * \brief 获得 p_igv 相关的对象是否可以被构造,
 *
 * \param[in] p_igv          对象构造保护变量
 *
 * \retval 0 : 不可被构造
 * \retval 1 : 可以被构造
 */
int  __cxa_guard_acquire (int volatile  *p_igv)
{
    int is_can_con;

    __AW_CPP_RT_LOCK();
    
    if (__AW_CPP_GUARD_IS_RELE(p_igv)) {     /*  已经被 construction 完毕    */
        
        is_can_con = 0;                      /*  不可 construction           */
    
    } else {
        
        __AW_CPP_GUARD_SET(p_igv, 1);   /*  设置为正在 construction     */
        is_can_con = 1;                 /*  可以 construction           */
    }
    
    __AW_CPP_RT_UNLOCK();
    
    return  (is_can_con);
}

/**
 * \brief p_igv 相关的对象被构造完毕后调用此函数. 这个对象不可再被构造,
 *
 * \param[in] ：对象构造保护变量
 */
void  __cxa_guard_release (int volatile *p_igv)
{
    __AW_CPP_RT_LOCK();
    
    __AW_CPP_GUARD_SET(p_igv, 1);     /*  不可 construction           */
    __AW_CPP_GUARD_MAKE_RELE(p_igv);  /*  设置为 release 标志         */

    __AW_CPP_RT_UNLOCK();
}

/**
 * \brief p_igv 相关的对象被析构后调用此函数, 此对象又可被构造
 *
 * \param[in] p_igv : 对象构造保护变量
 *
 */
void  __cxa_guard_abort (int volatile *p_igv)
{
    __AW_CPP_RT_LOCK();
    
    __AW_CPP_GUARD_SET(p_igv, 0);  /*  可以 construction           */                                                              /*  LW_CFG_THREAD_EXT_EN > 0    */

    __AW_CPP_RT_UNLOCK();
}

/**
 * \brief 在析构函数中调用虚方法时, 系统调用此函数.
 *
 * \param[in] p_igv : 对象构造保护变量
 *
 */
void __cxa_pure_virtual ()
{
    while (1);
}

/**
 * \brief 设置 __cxa_finalize 时需要运行的回调方法.
 *
 * \param[in] f : 函数指针
 * \param[in] p : 参数
 * \param[in] d : 句柄
 *
 * \retval  0 : 成功
 * \retval -1 : 失败
 */
int  __cxa_atexit (void (*f)(void *), void *p, void *d)
{
    aw_cpp_func_list_t *p_cppfl = \
            (aw_cpp_func_list_t *)aw_mem_alloc(sizeof(aw_cpp_func_list_t));
    
    if (p_cppfl == NULL) {
#ifdef printk
        printk(KERN_ERR "C++ run time error - __cxa_atexit system low memory\n");
#endif  /*  printk                      */
        return  (AW_ERROR);
    }
    
    AW_INIT_LIST_HEAD(&p_cppfl->line_manage);

    p_cppfl->func     = (aw_pfuncvoid_t)f;
    p_cppfl->p_arg    = p;
    p_cppfl->p_handle = d;
    
    __AW_CPP_RT_LOCK();
    aw_list_add(&p_cppfl->line_manage, &__g_linecpp_func_list);
    __AW_CPP_RT_UNLOCK();
    
    return  (AW_OK);
}
/******************************************************************************
** 函数名称: __cxa_finalize
** 功能描述: 调用被 __cxa_atexit 安装的方法, (一般为析构函数)
** 输　入  : d         句柄 (NULL 表示运行所有)
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/


/**
 * \brief 调用被 __cxa_atexit 安装的方法, (一般为析构函数)
 *
 * \param[in] d : 句柄
 */
void __cxa_finalize (void  *d)
{
    struct aw_list_head  *p_linetemp, *p_tmp;
    aw_cpp_func_list_t   *p_cppfl = NULL;
    
    if (d) {
        __AW_CPP_RT_LOCK();
        aw_list_for_each_safe(p_linetemp, p_tmp, &__g_linecpp_func_list) {
            p_cppfl = aw_list_entry(p_linetemp, aw_cpp_func_list_t, line_manage);
            if (p_cppfl->p_handle == d) {
                aw_list_del_init(&p_cppfl->line_manage);
                __AW_CPP_RT_UNLOCK();
                if (p_cppfl->func) {
                    p_cppfl->func(p_cppfl->p_arg);
                }

                aw_mem_free(p_cppfl);
                __AW_CPP_RT_LOCK();
            }
        }
        __AW_CPP_RT_UNLOCK();
    
    } else {
        __AW_CPP_RT_LOCK();

        aw_list_for_each_safe(p_linetemp, p_tmp, &__g_linecpp_func_list) {
            p_cppfl = aw_list_entry(p_linetemp, aw_cpp_func_list_t, line_manage);
            aw_list_del_init(&p_cppfl->line_manage);
            __AW_CPP_RT_UNLOCK();
            if (p_cppfl->func) {
                p_cppfl->func(p_cppfl->p_arg);
            }

            aw_mem_free(p_cppfl);
            __AW_CPP_RT_LOCK();
        }

        __AW_CPP_RT_UNLOCK();
    }
}

/**
 * \brief module 使用此函数执行 module 内部被 __cxa_atexit 安装的方法, (一般为析构函数)
 *
 * \param[in] pvStart : 起始内存
 * \param[in] stSize  : 内存大小
 *
 */
void __cxa_module_finalize (void  *pvStart, size_t  stSize)
{
    struct aw_list_head  *p_linetemp, *p_tmp;
    aw_cpp_func_list_t   *p_cppfl = NULL;
    
    __AW_CPP_RT_LOCK();

    aw_list_for_each_safe(p_linetemp, p_tmp, &__g_linecpp_func_list) {
        p_cppfl = aw_list_entry(p_linetemp, aw_cpp_func_list_t, line_manage);
        if (((uint8_t *)p_cppfl->func >= (uint8_t *)pvStart) &&
            ((uint8_t *)p_cppfl->func <  ((uint8_t *)pvStart + stSize))) {
            /*
             *  仅执行模块内存空间内部的函数节点
             */
            aw_list_del_init(&p_cppfl->line_manage);
            __AW_CPP_RT_UNLOCK();
            if (p_cppfl->func) {
                p_cppfl->func(p_cppfl->p_arg);
            }
            aw_mem_free(p_cppfl);
            __AW_CPP_RT_LOCK();
        }
    }

    __AW_CPP_RT_UNLOCK();
}

/**
 * \brief C++运行时初始化
 */
void aw_cplusplus_runtime_init (void)
{
    aw_mutex_id_t mutex_id;

    AW_INIT_LIST_HEAD(&__g_linecpp_func_list);

    mutex_id = AW_MUTEX_INIT(__g_cpp_rt_lock,
                             AW_SEM_Q_PRIORITY | AW_SEM_INVERSION_SAFE);

    __cppRtDoCtors(); /*  运行全局对象构造函数        */
}

/**
 * \brief C++ 运行时卸载
 */
void aw_cplusplus_runtime_uninit (void)
{
    __cxa_finalize(NULL);

    __cppRtDoDtors();        /*  运行全局对象析构函数        */
    
    AW_MUTEX_TERMINATE(__g_cpp_rt_lock);
}
}
/* end of file */
