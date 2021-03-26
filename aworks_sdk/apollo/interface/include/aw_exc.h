/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief �쳣��׼�ӿ�
 *
 * ʹ�ñ�������Ҫ������ͷ�ļ� aw_exc.h
 *
 * ��ģ���ṩ�˲���CPU�쳣�Ľӿڡ�ͨ����ֻӦ����ϵͳ��ʼ����ʱ�������Щ�ӿ���
 * �����쳣�����ӵȲ������쳣����ƽ̨���� ��
 *
 * \par ��ʾ��
 * \code
 * #include "apollo.h"
 * #include "aw_exc.h"
 *
 *  void my_esr(void)
 *  {
 *      //�쳣��������
 *  }
 *  aw_exc_int_connect(EXCNUM_HARDFALT,
 *                     (aw_pfuncvoid_t)my_esr); //����HardFalt�쳣������
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-04  zen, first implementation
 * \endinternal
 */

#ifndef __AW_EXC_H
#define __AW_EXC_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_exc
 * \copydoc aw_exc.h
 * @{
 */

#include "aw_common.h"
#include "aw_psp_exc.h"

/**
 * \brief ����һ��C������һ��CPU�쳣
 *
 * ����������һ��ָ����C������һ��ָ�����쳣�ϡ����쳣��Ϊ \a excnum ���쳣����
 * ʱ����������Ȩģʽ�µ��ø�C��������û��C�������ӣ���CPU��ִ�н�ͣ����
 * һ�� \c while(1) ����ѭ���С�һ���쳣ֻ������һ��C������������ӵ�C����������
 * ǰһ�����ӵ�C������
 *
 * \attention   aw_exc_int_connect() ֻӦ����ϵͳ��ʼ����ʱ�򱻵���
 *
 * \param[in]   excnum      �쳣�ţ���ƽ̨����
 *
 * \param[in]   pfunc_esr   Ҫ���õ�C����(ESR, �쳣������)������Ϊ�κ���ͨ��
 *                          C���룬���ǣ������ܵ���ĳЩ�������ĺ���������Ӧ��
 *                          ��֤������ִ�о����ܵض�
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_ENXIO    \a excnum Ϊ�����ڵ��쳣��
 * \retval     -AW_EFAULT   \a pfunc_esr Ϊ NULL
 *
 * \par ʾ��
 * \code
 *  #include "apollo.h"
 *  #include "aw_exc.h"
 *
 *  void my_esr(void)
 *  {
 *      //�쳣��������
 *  }
 *  aw_exc_int_connect(EXCNUM_HARDFALT,
 *                     (aw_pfuncvoid_t)my_esr); //����HardFalt�쳣������
 * \endcode
 */
aw_err_t aw_exc_int_connect(int excnum, aw_pfuncvoid_t pfunc_esr);

/** @} grp_aw_if_exc */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif	/* __AW_EXC_H */

/* end of file */

