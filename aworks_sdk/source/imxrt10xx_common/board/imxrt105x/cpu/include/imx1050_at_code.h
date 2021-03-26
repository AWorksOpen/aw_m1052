/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief �ṩ���������嵽ָ���ڴ����������еķ���
 
 * \par ʾ��
 * \code
 * #include ��imx1050_at_code.h��
 *
 * __IMX1050_RAM_USED__(int test_at_code(int *p_var));
 *
 * void test_at_code(int *p_var)
 * {
 *     *p_var = 0;
 *
 *     return AW_OK;
 * }
 *
 * int main()
 * {
 *    int var;
 *
 *    test_at_code(&var);    //��ת���ڲ�ram��ִ��
 * }
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 19-07-19  ral, first implementation
 * \endinternal
 */

#ifndef __IMX1050_AT_CODE_H
#define __IMX1050_AT_CODE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus*/

/**
 * \brief ����һ�����������䶨�嵽".ram.func"��
 *  ֪ͨ�������ú��������õģ���ֹ���������Ż�ʱ����ȥ��
 *
 * \param[in] pfunc ������������
 */
#define __IMX1050_RAM_USED__(pfunc) __attribute__((used, section(".ram.func"))) pfunc

/**
 * \brief ����һ�����������䶨�嵽".ram.func"��
 *  ֪ͨ�������ú���������������չ����ֹ����������չ��
 *
 * \param[in] pfunc ������������
 */
#define __IMX1050_RAM_NOINLINE__(pfunc)  __attribute__((noinline, section(".ram.func"))) pfunc

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX1050_AT_CODE_H */

/* end of file */
