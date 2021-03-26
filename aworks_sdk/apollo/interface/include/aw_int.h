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
 * \brief �жϱ�׼�ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
 * \code
 * #include aw_int.h
 * \endcode
 *
 * �������ṩ�˲���ϵͳ�жϵĽӿڡ��жϺ���ƽ̨���塣
 * ��������ɵ��ñ�ģ�������CPU�жϵĿ��أ�
 * �жϷ����������ӡ��Ͽ���ʹ�ܡ����ܵĲ�����
 *
 * \par ��ʾ��
 * \code
 *  #include "apollo.h"
 *  #include "aw_int.h"
 *
 *  void my_isr(void *p_arg)
 *  {
 *      int my_arg = (int)p_arg; //my_arg = 5
 *      //�жϴ�������
 *  }
 *  aw_int_connect(INUM_ADC, my_isr, (void *)5); //����ADC���жϷ�����
 *  aw_int_enable(INUM_ADC);    // ����ADC�ж�
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.00 12-08-27  zen, first implementation
 * \endinternal
 */

#ifndef __AW_INT_H
#define __AW_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_int
 * \copydoc aw_int.h
 * @{
 */
#include "aw_types.h"
#include "aw_psp_int.h"

/**
 * \brief ����CPU LOCK
 * ���궨��һ���������Ա��� CPU �����ж�״̬
 * \sa AW_INT_CPU_LOCK_DECL_STATIC() AW_INT_CPU_LOCK() AW_INT_CPU_UNLOCK()
 */
#define AW_INT_CPU_LOCK_DECL(flags) \
    AW_PSP_INT_CPU_LOCK_DECL(flags)

/**
 * \brief ����CPU LOCK (��̬)
 * ���궨��һ���������Ա��� CPU �����ж�״̬
 * \sa AW_INT_CPU_LOCK_DECL() AW_INT_CPU_LOCK() AW_INT_CPU_UNLOCK()
 */
#define AW_INT_CPU_LOCK_DECL_STATIC(flags) \
    AW_PSP_INT_CPU_LOCK_DECL_STATIC(flags)

/**
 * \brief �رձ���CPU���ж�
 *
 * ���������Թرյ��ô˺����������ISR����CPU���жϡ�
 * AW_INT_CPU_LOCK() �� AW_INT_CPU_UNLOCK() ֧��Ƕ�׵��ã�����
 * AW_INT_CPU_UNLOCK() ������������CPU���жϣ�ֻ�������� AW_INT_CPU_UNLOCK()
 * �Ż����������жϡ�
 *
 * \attention
 *  - CPU�жϱ��رյ�ʱ��Ӧ�����ܵض�
 *  - ������Ӧ�������� AW_INT_CPU_UNLOCK() ��Ե���
 *  - Ӧ������ʹ�ÿ���CPU�ж���Ϊ�����Ļ����ֶΣ��Ƽ�ʹ��
 *            \ref grp_aw_if_spinlock ����֮
 *  - �������޷��ر�CPU�ġ����������жϡ�
 *
 * \par ʾ��:�����÷�
 * \code
 * int key;
 *
 * AW_INT_CPU_LOCK_DECL(key)
 *
 * AW_INT_CPU_LOCK(key);
 * // ���ܱ���ϵ��ٽ����(ִ��Ӧ�����ܵض�)
 * AW_INT_CPU_UNLOCK(key);
 * \endcode
 *
 * \par ʾ��:Ƕ��
 * \code
 * AW_INT_CPU_LOCK_DECL(key0);
 * AW_INT_CPU_LOCK_DECL(key1);
 * AW_INT_CPU_LOCK_DECL(key2);
 *
 * AW_INT_CPU_LOCK(key0);   //CPU���жϱ������ر�
 * AW_INT_CPU_LOCK(key1);   //CPU���жϼ������ֹر�
 * AW_INT_CPU_LOCK(key2);   //CPU���жϼ������ֹر�
 *
 * AW_INT_CPU_UNLOCK(key2);    //CPU���жϼ������ֹر�
 * AW_INT_CPU_UNLOCK(key1);    //CPU���жϼ������ֹر�
 * AW_INT_CPU_UNLOCK(key0);    //CPU���жϱ���������
 * \endcode
 *
 * \see AW_INT_CPU_UNLOCK()
 */
#define AW_INT_CPU_LOCK(key)    AW_PSP_INT_CPU_LOCK(key)

/**
 * \brief �򿪱���CPU���ж�
 *
 * ���������Դ򿪵��ô˺����������ISR����CPU���жϡ�
 *
 * \attention ������Ӧ�������� AW_INT_CPU_LOCK() ��Ե���
 *
 * \see AW_INT_CPU_LOCK()
 */
#define AW_INT_CPU_UNLOCK(key)  AW_PSP_INT_CPU_UNLOCK(key)

/**
 * \brief �ж��Ƿ����жϵ���������ִ��
 *
 * \retval AW_FALSE ��ǰ�����ж�������ִ��
 * \retval ����ֵ   ��ǰ���ж���������ִ��
 */
#define AW_INT_CONTEXT()    AW_PSP_INT_CONTEXT()

/**
 * \brief �жϵ�ǰ�Ƿ�������CPU fault������
 *
 * ���͵�ARM fault����Ϊdata abort��prefetch abort
 * \retval AW_FALSE     ��ǰ����fault������ִ��
 * \retval ����ֵ       ��ǰ��fault������ִ��
 */
#define AW_FAULT_CONTEXT()    AW_PSP_FAULT_CONTEXT()

/**
 * \brief ����һ��C������һ��Ӳ���ж�
 *
 * ����������һ��ָ����C������һ��ָ�����ж��ϡ����жϺ�Ϊ \a inum ���жϷ���ʱ��
 * ��������Ȩģʽ�µ��ø�C����( \a pfunc_isr )�������ݲ��� \a p_arg ��
 *
 * \par ʵ�����
 * ƽ̨����֧�����Ӷ��C������ͬһ���ж��ϣ����жϷ���ʱ�����ᰴ�����ӵ��Ⱥ�˳��
 * ������ЩC������ƽ̨Ҳ����ֻ֧������һ��C�����������Ѿ������ӵ��жϽ��᷵��
 * �ռ䲻�����(-ENOSPC)�����������ε��� aw_int_disable()�� aw_int_disconnect()
 * ��ֹ���Ͽ���ǰ�����ӵ�C�������ſ��Գɹ������µ�C������
 *
 * \attention   ����ȷ���ж� \a inum �Ѿ�����ֹ(�ж� \a inum Ĭ���Ǳ���ֹ��)��
 *              ����, �ڱ�����֮ǰӦ���ȵ��� aw_int_disable() ��ֹ��Ӧ���жϡ�
 *
 * \param[in]   inum        �жϺţ���ƽ̨����
 * \param[in]   pfunc_isr   Ҫ���õ�C����(ISR, �жϷ�����)������Ϊ�κ���ͨ��
 *                          C���룬���ǣ������ܵ���ĳЩ�������ĺ�����
 *                          ����Ӧ����֤������ִ�о����ܵض�
 * \param[in]   p_arg       Ҫ���ݸ�C�����Ĳ���
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_EINVAL   ��������
 * \retval     -AW_ENOTSUP  ��֧��
 *
 * \par ʾ���������÷�
 * \code
 *  void my_isr(void *p_arg)
 *  {
 *      int my_arg = (int)p_arg; //my_arg = 5
 *      //�жϴ�������
 *  }
 *  aw_int_connect(INUM_ADC, my_isr, (void *)5); //����ADC���жϷ�����
 * \endcode
 *
 * \par ʾ���������жϷ�����(ISR)
 * ��ʵ��ֻ֧�֡�1���ж�����1��ISR��ʱ�������·�ʽ����ISR:
 * \code
 *  aw_int_disable(INUM_ADC);                        // ��ֹ�ж�
 *  aw_int_disconnect(INUM_ADC, my_isr, (void *)5);  // �Ͽ��ɵ�ISR
 *  aw_int_connect(INUM_ADC, my_isr_new, (void *)6); // �����µ�ISR
 *  aw_int_enable(INUM_ADC);                         // �����ж�
 * \endcode
 *
 * \see aw_int_disconnect()
 */
aw_err_t aw_int_connect(int             inum,
                        aw_pfuncvoid_t  pfunc_isr,
                        void           *p_arg);

/**
 * \brief �Ͽ�һ��C������һ��Ӳ���жϵ�����
 *
 * ��������һ���ж������жϿ�һ������Ϊ \a p_arg ��C������
 *
 * \attention
 *  - Ӧ����֤ \a pfunc_isr �� \a p_arg ����ǰ���ݸ� aw_int_connect() �Ĳ�����ͬ��
 *
 * \param[in]   inum        �жϺ�
 * \param[in]   pfunc_isr   Ҫ�Ͽ���C����
 * \param[in]   p_arg       Ҫ�Ͽ���C�����Ĳ���
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_EINVAL   ��������
 * \retval     -AW_ENOTSUP  ��֧��
 * \retval     -AW_ENOENT   δ�ҵ�����Ϊ \a p_arg �� \a pfunc_isr
 *
 * \see aw_int_connect()
 */
aw_err_t aw_int_disconnect(int             inum,
                           aw_pfuncvoid_t  pfunc_isr,
                           void           *p_arg);

/**
 * \brief ʹ��ָ�����ж�
 *
 * ������ʹ����\a inum ָ�����ж�
 *
 * \param[in]   inum        �жϺ�
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_EINVAL   ��������
 * \retval     -AW_ENOTSUP  ��֧��
 *
 * \see aw_int_disable()
 */
aw_err_t aw_int_enable(int inum);

/**
 * \brief ����ָ�����ж�
 *
 * ������������\a inum ָ�����ж�
 *
 * \param[in]   inum        �жϺ�
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_EINVAL   ��������,��������жϺ�
 * \retval     -AW_ENOTSUP  ��֧��
 *
 * \see aw_int_enable()
 */
aw_err_t aw_int_disable(int inum);

/**
 * \brief �������ӵ��ж�������ĺ�������
 */
typedef void (*pfn_aw_int_vector_func_t)(void);


/**
 * \brief ����һ��C�������ж�������
 *
 * Ϊ������жϴ���Ч�ʣ�aw_int_vector_connect�ṩ��һ��ֱ�����ӵ��ж�������
 * �ķ�����CPU�����жϺ󣬽�ֱ������Ȩģʽ��ִ��ָ����C����
 * \par ʵ�����
 * �˷�������һ�������е�ƽ̨��ʵ�֣���һ��ƽ̨��Ҳ��һ�����е��ж϶�ʵ��
 * ͨ������ֻ��cortex-m3֮���ƽ̨��ʵ�֣�CPU�����ж�ֱ��ִ�ж�Ӧ��ISR,�жϲ���
 * ��Ҫ�Լ�ά�������ر���ҪЧ�ʵ�ƽ̨��ʵ�֡�
 *
 * \attention   ����ȷ���ж� \a inum �Ѿ�����ֹ(�ж� \a inum Ĭ���Ǳ���ֹ��)��
 *              ����, �ڱ�����֮ǰӦ���ȵ��� aw_int_disable() ��ֹ��Ӧ���жϡ�
 *
 * \param[in]   inum            �жϺţ���ƽ̨����
 * \param[in]   pfunc_vector    Ҫ���õ�C����(ISR, �жϷ�����)������Ϊ�κ���ͨ��
 *                          C���룬���ǣ������ܵ���ĳЩ�������ĺ�����
 *                          ����Ӧ����֤������ִ�о����ܵض�
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_EINVAL   ��������
 * \retval     -AW_ENOTSUP  ��֧��
 *
 * \par ʾ���������÷�
 * \code
 *  void my_isr(void)
 *  {
 *      //�жϴ�������
 *  }
 *  aw_int_vector_connect(INUM_ADC, my_isr);        //����ADC���ж���������
 * \endcode
 *
 * \par ʾ���������ж���������(ISR)
 * ��ʵ��ֻ֧�֡�1���ж�����1��ISR��ʱ�������·�ʽ����ISR:
 * \code
 *  aw_int_disable(INUM_ADC);                       // ��ֹ�ж�
 *  aw_int_vector_disconnect(INUM_ADC, my_isr);     // �Ͽ��ɵ�ISR
 *  aw_int_vector_connect(INUM_ADC, my_isr_new);    // �����µ�ISR
 *  aw_int_enable(INUM_ADC);                        // �����ж�
 * \endcode
 */
aw_err_t aw_int_vector_connect(int                      inum,
                               pfn_aw_int_vector_func_t pfunc_vector);

/**
 * \brief �Ͽ�һ��C������һ��Ӳ���ж������������
 *
 * ���������ж��������ж�Ӧ�жϵĺ�������ΪϵͳĬ�Ϻ���
 *
 * \attention
 *  - Ӧ����֤ \a pfunc_isr ����ǰ���ݸ� aw_int_vector_connect() �Ĳ�����ͬ��
 *
 * \param[in]   inum        �жϺ�
 * \param[in]   pfunc_isr   Ҫ�Ͽ���C����
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_EINVAL   ��������
 * \retval     -AW_ENOTSUP  ��֧��
 * \see aw_int_vector_connect()
 */
aw_err_t aw_int_vector_disconnect(int                      inum,
                                  pfn_aw_int_vector_func_t pfunc_vector);



/**
 * \brief ��ȡһ���жϵ�ǰ�����ȼ�
 * \param[in]   inum        �жϺ�
 * \param[in]   p_cur_prio  ����������жϵ�ǰ�����ȼ�
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_EINVAL   ��������
 */
aw_err_t aw_int_priority_get(int inum,int *p_cur_prio);

/**
 * \brief ��ȡһ���жϵĿ��������ȼ���Χ
 *
 * \param[in]   inum        �жϺ�
 * \param[in]   p_min_prio  �����ֵ����С�����ȼ�
 * \param[in]   p_max_prio  �����ֵ���������ȼ�
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_EINVAL   ��������
 * \retval     -AW_ENOTSUP  ��֧��
 * \attention ����ƽ̨�Ĳ�ͬ����ֵС�����ȼ�����ʵ�ʸ��ߣ�Ҳ����ʵ�ʸ���
 */
aw_err_t aw_int_priority_range_get(int inum,int *p_min_prio,int *p_max_prio);

/**
 * \brief ����һ���ж����ȼ�
 * \param[in]   inum        �жϺ�
 * \param[in]   new_prio    �µ��ж����ȼ�
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_EINVAL   ��������
 * \retval     -AW_ENOTSUP  ��֧��
 */
aw_err_t aw_int_priority_set(int inum,int new_prio);

/**
 * \brief �ֹ�����һ���ж�
 *
 * ��ʱ����ȻӲ��û�в����жϣ��������ǿ�����Ҫ��ISRִ��
 * �ֹ������жϽ���һ����ֱ�ӵ�����ISR�úܶ��ѡ���ֹ������жϺ������ǰ
 * û�н�ֹ�жϣ������ȼ�����Ҫ�󣬶�Ӧ�жϵ�ISR������ִ��
 *
 * \param[in]   inum        �жϺ�
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_EINVAL   ��������
 * \retval     -AW_ENOTSUP  ��֧��
 */
aw_err_t aw_int_pend(int inum);

/** @}  grp_aw_if_int */

#ifdef __cplusplus
}
#endif  /* __cplusplus 	*/

#endif  /* __AW_INT_H */

/* end of file */
