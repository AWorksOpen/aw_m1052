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
 * \brief C++例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里打开AW_COM_CONSOLE和调试串口对应的宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口打印出测试成功信息，如果测试失败,则打印出失败信息。
 *
 * - 备注：
 *   keil下armcc编译器不支持该例程
 *
 * \par 源代码
 * \snippet demo_cplusplus.cpp src_cplusplus
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-14  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_cplusplus C++例程
 * \copydoc demo_cplusplus.cpp
 */

/** [src_cplusplus] */

#include "aworks.h"
#include "aw_vdebug.h"
#include <vector>
#include <typeinfo>
#include <iostream>

using namespace std;

/** \brief 用于判断对象的构造函数是否已经执行 */
aw_local int    __g_constructor = 0;

/** \brief 定义ctest1类，用来测试全局对象的构造函数和析构函数 */
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

/** \brief 定义全局对象*/
ctest1 __g_test1_global_obj;

/** \brief
 * 测试全局对象构造函数
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


/** \brief 定义测试类 ctest2用，测试局部静态对象构造函数和析构函数 */
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
 * 测试局部静态对象构造函数
 */
aw_local void __test_local_static_object (void)
{
    /*定义一个局部静态对象*/
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

/** \brief 测试模板 */
aw_local void __test_template (void)
{
    int         i_src1, i_src2, i_dst;
    double      d_src1, d_src2, d_dst1, d_dst2;

    i_src1 = i_src2 = 5689;
    i_dst = 256478;

    swap_t_func(&i_src1,&i_dst);

    /*判断 swap 是否成功*/
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

/** \brief 测试new */
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

/** \brief 测试vector */
aw_local void  __test_vector (void)
{
    int i = 0;

    /*定义 int 型的容器*/
    vector<int> *p_vect = new vector<int>;

    /*定义 int 型迭代器*/
    vector<int>::const_iterator    iter;

    /*判断new是否成功*/
    if (NULL == p_vect) {
        aw_kprintf("new vector fail");
        return ;
    }

    /*压入数据*/
    for (i = 1; i<=TEST_VECT_COUNT; i++ ) {
        p_vect->push_back(i);
    }

    /*测试在vector中的数据*/
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

/** \brief 测试异常 */
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

/** \brief 运行时类型识别 */
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
    /* 测试标准输出流 */
    cout<<"start c++ demo."<<endl;

    /* 测试全局对象构造与析构函数 */
    AW_INFOF(("\n\ntest C++ global object constructor and destructor:\n"));
    __test_global_object();

    /* 测试局部静态对象构造与析构函数 */
    AW_INFOF(("\n\ntest C++ local static object constructor and destructor:\n"));
    __test_local_static_object();

    /* new delete 测试 */
    AW_INFOF(("\n\ntest C++ new delete:\n"));
    __test_new();

    /* vector 测试 */
    AW_INFOF(("\n\ntest C++ vector:\n"));
    __test_vector();

    /* 模板测试 */
    AW_INFOF(("\n\ntest C++ template swap:\n"));
    __test_template();

    /* 测试异常 */
    AW_INFOF(("\n\ntest C++ exceptions(try/catch) 5/3 or 5/0:\n"));
    __test_exception();

    /*测试运行时类型识别*/
    AW_INFOF(("\n\ntest C++ rtti:\n"));
    __test_rtti();

    AW_INFOF(("\nc++ test ending\n"));

    return ;
}

