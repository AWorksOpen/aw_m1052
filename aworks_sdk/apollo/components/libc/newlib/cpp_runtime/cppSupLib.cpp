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
 * \brief ����ϵͳƽ̨ C++ run time support lib.
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
  ��������
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
  gurad �����
*******************************************************************************/
#define __AW_CPP_GUARD_GET(p_igv)        (*((char *)(p_igv)))
#define __AW_CPP_GUARD_SET(p_igv, x)     (*((char *)(p_igv))  = x)

/*  Ӧ���� construction ���    */
#define __AW_CPP_GUARD_MAKE_RELE(p_igv)  (*((char *)(p_igv)) |= 0x40)
#define __AW_CPP_GUARD_IS_RELE(p_igv)    (*((char *)(p_igv)) &  0x40)
/******************************************************************************
  __cxa_guard_acquire
  __cxa_guard_release
  __cxa_guard_abort
  
  ��������Ҫ������Ǿ�̬�ֲ����������������, 
  �������³���:
  
  void  foo ()
  {
      static student   tom;
      
      ...
  }
  
  ���� foo() ����, ���������ɵĴ�����Ҫ���ȼ����� tom �Ƿ񱻽���, ���û�й���, ����Ҫ���� tom.
  ����Ϊ�˷�ֹ�������ͬʱ���� foo() ����, ����������Ҫ __cxa_guard_acquire ��
 __cxa_guard_release �����ʵ�ֿ�����. ���������ɵ�α��������:
  
  ...
  if (__cxa_guard_acquire(&tom_stat) == 1) {
      ...
      construct tom
      ...
      __cxa_guard_release(&tom_stat);
  }
  
  ���� tom_stat Ϊ������Ϊ tom ��������״̬��¼"����".
*******************************************************************************/

/**
 * \brief ��� p_igv ��صĶ����Ƿ���Ա�����,
 *
 * \param[in] p_igv          �����챣������
 *
 * \retval 0 : ���ɱ�����
 * \retval 1 : ���Ա�����
 */
int  __cxa_guard_acquire (int volatile  *p_igv)
{
    int is_can_con;

    __AW_CPP_RT_LOCK();
    
    if (__AW_CPP_GUARD_IS_RELE(p_igv)) {     /*  �Ѿ��� construction ���    */
        
        is_can_con = 0;                      /*  ���� construction           */
    
    } else {
        
        __AW_CPP_GUARD_SET(p_igv, 1);   /*  ����Ϊ���� construction     */
        is_can_con = 1;                 /*  ���� construction           */
    }
    
    __AW_CPP_RT_UNLOCK();
    
    return  (is_can_con);
}

/**
 * \brief p_igv ��صĶ��󱻹�����Ϻ���ô˺���. ������󲻿��ٱ�����,
 *
 * \param[in] �������챣������
 */
void  __cxa_guard_release (int volatile *p_igv)
{
    __AW_CPP_RT_LOCK();
    
    __AW_CPP_GUARD_SET(p_igv, 1);     /*  ���� construction           */
    __AW_CPP_GUARD_MAKE_RELE(p_igv);  /*  ����Ϊ release ��־         */

    __AW_CPP_RT_UNLOCK();
}

/**
 * \brief p_igv ��صĶ�����������ô˺���, �˶����ֿɱ�����
 *
 * \param[in] p_igv : �����챣������
 *
 */
void  __cxa_guard_abort (int volatile *p_igv)
{
    __AW_CPP_RT_LOCK();
    
    __AW_CPP_GUARD_SET(p_igv, 0);  /*  ���� construction           */                                                              /*  LW_CFG_THREAD_EXT_EN > 0    */

    __AW_CPP_RT_UNLOCK();
}

/**
 * \brief �����������е����鷽��ʱ, ϵͳ���ô˺���.
 *
 * \param[in] p_igv : �����챣������
 *
 */
void __cxa_pure_virtual ()
{
    while (1);
}

/**
 * \brief ���� __cxa_finalize ʱ��Ҫ���еĻص�����.
 *
 * \param[in] f : ����ָ��
 * \param[in] p : ����
 * \param[in] d : ���
 *
 * \retval  0 : �ɹ�
 * \retval -1 : ʧ��
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
** ��������: __cxa_finalize
** ��������: ���ñ� __cxa_atexit ��װ�ķ���, (һ��Ϊ��������)
** �䡡��  : d         ��� (NULL ��ʾ��������)
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/


/**
 * \brief ���ñ� __cxa_atexit ��װ�ķ���, (һ��Ϊ��������)
 *
 * \param[in] d : ���
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
 * \brief module ʹ�ô˺���ִ�� module �ڲ��� __cxa_atexit ��װ�ķ���, (һ��Ϊ��������)
 *
 * \param[in] pvStart : ��ʼ�ڴ�
 * \param[in] stSize  : �ڴ��С
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
             *  ��ִ��ģ���ڴ�ռ��ڲ��ĺ����ڵ�
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
 * \brief C++����ʱ��ʼ��
 */
void aw_cplusplus_runtime_init (void)
{
    aw_mutex_id_t mutex_id;

    AW_INIT_LIST_HEAD(&__g_linecpp_func_list);

    mutex_id = AW_MUTEX_INIT(__g_cpp_rt_lock,
                             AW_SEM_Q_PRIORITY | AW_SEM_INVERSION_SAFE);

    __cppRtDoCtors(); /*  ����ȫ�ֶ����캯��        */
}

/**
 * \brief C++ ����ʱж��
 */
void aw_cplusplus_runtime_uninit (void)
{
    __cxa_finalize(NULL);

    __cppRtDoDtors();        /*  ����ȫ�ֶ�����������        */
    
    AW_MUTEX_TERMINATE(__g_cpp_rt_lock);
}
}
/* end of file */
