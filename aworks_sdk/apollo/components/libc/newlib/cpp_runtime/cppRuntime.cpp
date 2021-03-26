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
 * \brief AWorksƽ̨ C++ run time ȫ�ֶ��󹹽�������������.
 *
 * \internal
 * \par modification history
 * - 1.00 2016-03-10  cod, first implementation
 * \endinternal
 */
#include "apollo.h"

extern "C" {
/*******************************************************************************
  ˵��:
  
  ������(gcc)���� C++ ����ʱ, �������ȫ�ֶ���, ��ôȫ�ֶ���Ĺ�������ָ�����ڿ�ִ�� elf �ļ���
 .ctors����(section), ������������ڿ�ִ�� elf �ļ��� .dtors ����,
   һ���׼ gcc ��������ĸ�����:
  __CTOR_LIST__
  __CTOR_END__
  __DTOR_LIST__
  __DTOR_END__
   
  ���� __CTOR_LIST__ ��ʾ���е�ȫ�ֶ����캯��ָ��������׵�ַ, ��ʼָ��Ϊ 0xFFFFFFFF,֮���ÿһ��
  Ϊһ�����캯�������, ֱ�� __CTOR_END__ Ϊֹ, __CTOR_END__ ָ��ĺ���ָ��Ϊ 0x00000000
  
  ���� __DTOR_LIST__ ��ʾ���е�ȫ�ֶ�����������ָ��������׵�ַ, ��ʼָ��Ϊ 0xFFFFFFFF,֮���ÿһ��
  Ϊһ���������������, ֱ�� __DTOR_END__ Ϊֹ, __DTOR_END__ ָ��ĺ���ָ��Ϊ 0x00000000
  
  һ�´����ʵ������� 4 ���������ƵĶ���. ����ϵͳ�Ϳ����������û�����֮ǰ, ��ʼ�� C++ ����, ����ȫ��
  ����Ĺ��캯��, ��ϵͳ reboot ʱ, ����ϵͳ����������.
  
  ���Ҫ����Щ���Ŵ��ڶ�Ӧ .ctors �� .dtors ����ָ����λ��, ����Ҫ�������ļ�����һ�´���:
  
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
  
  �������ӽű�, ����Ҫ�ķ��Ŷ��嵽�� .ctors .dtors ������Ӧ��λ�� (�ֱ��嵽����������������β)
  (���� .init_array �� .fini_array �ֱ��ǹ������������о�̬�洢ʱ�޵Ķ���)
  
  ע��:
  
  ���ڲ���ϵͳ���ڵ����û�֮ǰ, ��������ȫ�ֶ����캯��, ��ʱ��û�н�������񻷾�, ���Զ���Ĺ��캯��һ��
  Ҫ�㹻�ļ�, һ�����������ʼ����������Ժ�һЩ�������ݽṹ, ����Ĳ������������м���ר�ŵĳ�ʼ������
  ��ʵ��.
*******************************************************************************/
typedef void (*__aw_pfunc_t) (...);

/** \brief C++ ȫ�ֶ��󹹽������������� (Ϊ�˺�һЩ��������������ͻ, ����ʹ��AWorks �Դ��ķ��� */
extern __aw_pfunc_t __AW_CTOR_LIST__;
extern __aw_pfunc_t __AW_DTOR_LIST__;
extern __aw_pfunc_t __AW_CTOR_END__;
extern __aw_pfunc_t __AW_DTOR_END__;
/******************************************************************************/
/**
 * \brief C++ ����ȫ�ֶ����캯��
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
 * \brief  C++ ����ȫ�ֶ�����������
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
