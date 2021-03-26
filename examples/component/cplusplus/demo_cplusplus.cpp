/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief C++����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ����AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ���ڴ�ӡ�����Գɹ���Ϣ���������ʧ��,���ӡ��ʧ����Ϣ��
 *
 * - ��ע��
 *   keil��armcc��������֧�ָ�����
 *
 * \par Դ����
 * \snippet demo_cplusplus.cpp src_cplusplus
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-14  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_cplusplus C++����
 * \copydoc demo_cplusplus.cpp
 */

/** [src_cplusplus] */

#include "aworks.h"
#include "aw_vdebug.h"
#include <vector>
#include <typeinfo>
#include <iostream>

using namespace std;

/** \brief �����ж϶���Ĺ��캯���Ƿ��Ѿ�ִ�� */
aw_local int    __g_constructor = 0;

/** \brief ����ctest1�࣬��������ȫ�ֶ���Ĺ��캯������������ */
class ctest1
{
public:
    ctest1() {
        __g_constructor = 1;
        AW_INFOF(("Ctest1 constructor\n"));
    };
    ~ctest1() {
        AW_INFOF(("Ctest1 destructor\n"));
    };

};

/** \brief ����ȫ�ֶ���*/
ctest1 __g_test1_global_obj;

/** \brief
 * ����ȫ�ֶ����캯��
 */
aw_local void __test_global_object (void)
{
    if (__g_constructor == 1) {
        aw_kprintf("test global object constructor successful!\n");
    } else {
        aw_kprintf("test global object constructor error!\n");
    }
    return ;
}


/** \brief ��������� ctest2�ã����Ծֲ���̬�����캯������������ */
class ctest2
{
public:
    ctest2() {
        AW_INFOF(("Ctest2 constructor\n"));
        __g_constructor = 2;
    };
    ~ctest2() {
        AW_INFOF(("Ctest2 destructor\n"));
    };

};

/** \brief
 * ���Ծֲ���̬�����캯��
 */
aw_local void __test_local_static_object (void)
{
    /*����һ���ֲ���̬����*/
    static ctest2 test2_obj;

    if ( __g_constructor == 2 ) {
        aw_kprintf("test local static object constructor successful!\n");
    } else {
        aw_kprintf("test local static object constructor error!\n");
    }
    return ;
}

template <typename T>
void swap_t_func(T *a, T *b)
{
    T   c;
     c = *a;
    *a = *b;
    *b =  c;

}

/** \brief ����ģ�� */
aw_local void __test_template (void)
{
    int         i_src1, i_src2, i_dst;
    double      d_src1, d_src2, d_dst1, d_dst2;

    i_src1 = i_src2 = 5689;
    i_dst = 256478;

    swap_t_func(&i_src1,&i_dst);

    /*�ж� swap �Ƿ�ɹ�*/
    if (i_src2 != i_dst || i_src2 == i_src1) {
        aw_kprintf("test template failed\r\n");
        return ;
    }

    d_src1 = d_src2 = 3.141592654;
    d_dst1 = d_dst2 = 2.618569423;

    swap_t_func(&d_src1, &d_dst1);

    if (d_dst1 != d_src2 || d_src1 != d_dst2) {
        aw_kprintf("test template failed\r\n");
        return ;
    }

    aw_kprintf("test template successful!\r\n");
    return ;
}

/** \brief ����new */
aw_local void __test_new (void)
{
    int *p = NULL;

    p = new int[256];

    if (p == NULL) {
        aw_kprintf("test new fail!\r\n");
    } else {
        aw_kprintf("test new successful!\r\n");
        delete p;
    }

    return ;
}

#define TEST_VECT_COUNT         4096

/** \brief ����vector */
aw_local void  __test_vector (void)
{
    int i = 0;

    /*���� int �͵�����*/
    vector<int> *p_vect = new vector<int>;

    /*���� int �͵�����*/
    vector<int>::const_iterator    iter;

    /*�ж�new�Ƿ�ɹ�*/
    if (NULL == p_vect) {
        aw_kprintf("new vector fail");
        return ;
    }

    /*ѹ������*/
    for (i = 1; i<=TEST_VECT_COUNT; i++ ) {
        p_vect->push_back(i);
    }

    /*������vector�е�����*/
    i = 1;
    iter =  p_vect->begin();
    while (iter != p_vect->end()) {
        if ( *iter != i) {
            aw_kprintf("test vector fail\r\n");
            return ;
        }
        iter ++;
        i ++;
    }

    aw_kprintf("test vector seccessful!\r\n");
    delete p_vect;

    return ;
}


static int division (int x, int y)
{
    if (y == 0) {
        throw (int)-AW_EDOM;
    }
    return x / y;
}

/** \brief �����쳣 */
aw_local void __test_exception (void)
{
    int r = 0;
    int t;

    try
    {
        t = division(5,3);
        t = division(5,0);
    }
    catch (int e)
    {
       r = e;
    }

    if (t != 1 || r != -AW_EDOM) {
        aw_kprintf("test_ exception failed!\r\n");
    } else {
        aw_kprintf("test_ exception successful!\r\n");
    }
    return ;
}

class c_test_rtti_base
{
public:
    virtual ~c_test_rtti_base() {};
    virtual void test() {};
};

class c_test_rtti:public c_test_rtti_base
{
    virtual void test()
    {
        aw_kprintf("test\r\n");
    };
};

/** \brief ����ʱ����ʶ�� */
aw_local void  __test_rtti (void)
{
    int                             r = 0;
    c_test_rtti_base                c1;
    c_test_rtti                     c2;
    c_test_rtti_base                *p;

    p = &c1;
    r = 0;
    if (typeid(*p) == typeid(c_test_rtti_base)) {
        r = 1;
    }

    if (r != 1) {
        aw_kprintf("test rtti fail!\r\n");
        return ;
    }

    p = &c2;
    r = 0;
    if (typeid(*p) == typeid(c_test_rtti)) {
        r = 1;
    }

    if (r != 1) {
        aw_kprintf("test rtti fail!\r\n");
        return ;
    }

    aw_kprintf("test rtti successful!\r\n");
    return ;
}

extern "C" void demo_cplusplus_entry (void)
{
    /* ���Ա�׼����� */
    cout<<"start c++ demo."<<endl;

    /* ����ȫ�ֶ��������������� */
    AW_INFOF(("\n\ntest C++ global object constructor and destructor:\n"));
    __test_global_object();

    /* ���Ծֲ���̬���������������� */
    AW_INFOF(("\n\ntest C++ local static object constructor and destructor:\n"));
    __test_local_static_object();

    /* new delete ���� */
    AW_INFOF(("\n\ntest C++ new delete:\n"));
    __test_new();

    /* vector ���� */
    AW_INFOF(("\n\ntest C++ vector:\n"));
    __test_vector();

    /* ģ����� */
    AW_INFOF(("\n\ntest C++ template swap:\n"));
    __test_template();

    /* �����쳣 */
    AW_INFOF(("\n\ntest C++ exceptions(try/catch) 5/3 or 5/0:\n"));
    __test_exception();

    /*��������ʱ����ʶ��*/
    AW_INFOF(("\n\ntest C++ rtti:\n"));
    __test_rtti();

    AW_INFOF(("\nc++ test ending\n"));

    return ;
}

