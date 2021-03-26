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
 * \brief ARMv7-M �쳣����
 *
 * ��ģ��ΪCortex-M3(ARMv7-M�ܹ�)���쳣ģ��ʵ�֡�
 *
 * \par ����
 *
 * - ʵ��AnyWhere��׼�쳣�ӿڣ��ο� \ref grp_aw_serv_exc
 * - ʵ���˶�NMI��HardFalt��SVCall��PendSV��SysTick��EINT���쳣����
 *      ������Щ�쳣�Ķ����������ο� \ref grp_aw_plfm_arm_exc_excnums
 * - ��ģ��û���ṩ��Reset�쳣�Ĺ���
 * - ��ģ�齫�����ⲿ�ж��쳣(IRQ,16��֮����쳣)����ΪEINT�쳣����Щ�쳣����
 *   �ж�ģ�������ο� \ref grp_aw_serv_int
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-04  zen, first implementation
 * \endinternal
 */

#ifndef __ARMV7M_EXC_H
#define __ARMV7M_EXC_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/**
 * \addtogroup grp_aw_plfm_arm_exc_excnums
 * @{
 */

/**
 * \name ARMv7-M �쳣�Ŷ���
 * @{
 */

/**
 * \brief Reset �쳣
 *
 * ���ϵ�(power up)��������(warm reset)ʱ��������λ(Reset)���쳣ģ�ͽ���λ����
 * һ��������ʽ���쳣������λ��Чʱ������ֹͣ����������ܷ�����һ��ָ���е��κ�
 * �㡣����λʧЧʱ�������������и�λ���(reset entry)�ṩ�ĵ�ַ�����¿�ʼִ�С�
 * ִ�н�����Ȩģʽ�����¿�ʼ��
 */
#define EXCNUM_RESET        1

/**
 * \brief NMI �쳣
 *
 * NMI �쳣�����������������������ǳ���λ֮���������ȼ��쳣���������õ�ʹ��
 * ����ӵ�й̶������ȼ�-2��NMI �����ԣ�
 *  - �������κ��쳣���λ���ֹ
 *  - ������λ��������κ��쳣��ռ
 *
 * \attention �ڲ���CPU�ϣ�δʵ�ֱ��쳣
 */
#define EXCNUM_NMI          2

/**
 * \brief HardFalt �쳣
 *
 * HardFault ������ͨ���쳣�����в������������µ��쳣��HardFaults ӵ�й̶���
 * ���ȼ� -1����ζ�����������κ����ȼ������õ��쳣���Ÿ��߸����ȼ���
 */
#define EXCNUM_HARDFALT     3

/**
 * \brief MemManage �쳣
 *
 * MemManage ���������ڴ汣����ش������������쳣������ָ��������ڴ洫�䣬MPU
 * ��̶��ڴ汣�����ƾ������쳣���ô������Ǳ�������ֹ������ִ��(Execute Never)
 * �ڴ�ε�ָ����ʡ�
 */
#define EXCNUM_MEMMANAGE    4

/**
 * \brief BusFault �쳣
 *
 * BusFault ����ָ��������ڴ洫�������µ��ڴ���ش����������ġ����쳣����������
 * �ڴ�ϵͳ����鵽��ĳ�������ϵĴ���
 */
#define EXCNUM_BUSFAULT     5

/**
 * \brief UsageFault �쳣
 *
 * UsageFault ������ָ��ִ����صĴ��������µ��쳣����Щ���������
 *  - һ��δ�����ָ��
 *  - һ�ηǷ��Ĳ��������
 *  - �쳣����ʱ�����Ĵ���
 *
 * ������������ܻᵼ�� UsageFault �쳣������ں˱�����Ϊ������Щ����Ļ���
 *  - ��һ��δ����ĵ�ַ�Ͻ����ֺͰ����ڴ����
 *  - �� 0 ��
 */
#define EXCNUM_USAGEFAULT   6

/**
 * \brief SvCall �쳣
 *
 * �����û�����(SVC) ���� SVC ָ������쳣����OS�����У�Ӧ�ó������ʹ��SVC
 * ָ��������OS�ں˺����Լ��豸������
 */
#define EXCNUM_SVCALL       11

/**
 * \brief PendSV �쳣
 *
 * PendSV �����ж�������ϵͳ������������OS�����У���û�������쳣��Ծʱ��ʹ��
 * PendSV �������������л���
 */
#define EXCNUM_PENDSV       14

/**
 * \brief SysTick �쳣
 *
 * SysTick �쳣����ϵͳ��ʱ������ֵΪ0ʱ���������쳣�����ͬ��Ҳ���Բ��� SysTick
 * �쳣����OS�����У�������ʹ�ø��쳣��Ϊϵͳ���ġ�

 */
#define EXCNUM_SYSTICK      15

/**
 * \brief EINT �쳣
 *
 * 16�����쳣����Ϊ EINT �ж� (�����IRQ�ж�)�����쳣����������������������
 * �жϵ�ָ��ִ�ж����첽�ġ���ϵͳ�У�����ʹ���ж����봦����ͨ�š�
 */
#define EXCNUM_EINT         16


/** @} */

/** @} grp_armv7m_exc_excnums */

#define EXCNUM_MAX          EXCNUM_EINT /**< �����쳣����ֵ */

/**
 * \addtogroup grp_aw_plfm_arm_exc_usrcfg
 * @{
 */

/**
 * \name �쳣������
 * @{
 *
 * �����쳣����������ڵ�ַ��Ҫ����д����Ӧ���쳣������
 * \attention �ϲ��û�������Ĵ˲���
 */

/**
 * \brief NMI �쳣������
 */
void armv7m_exc_nmi_handler(void);

/**
 * \brief HardFalt �쳣������
 */
void armv7m_exc_hardfault_handler(void);

/**
 * \brief MemManage �쳣������
 */
void armv7m_exc_memmanage_handler(void);

/**
 * \brief BusFault �쳣������
 */
void armv7m_exc_busfault_handler(void);

/**
 * \brief UsageFault �쳣������
 */
void armv7m_exc_usagefault_handler(void);

/**
 * \brief SVCall �쳣������
 */
void armv7m_exc_svcall_handler(void);

/**
 * \brief pendSV �쳣������
 */
void armv7m_exc_pendsv_handler(void);

/**
 * \brief SysTick �쳣������
 */
void armv7m_exc_systick_handler(void);

/**
 * \brief EINT �쳣������
 */
void armv7m_exc_eint_handler(void);

/** @} */

/** @} grp_armv7m_exc_usrcfg */

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __ARMV7M_EXC_H */

/* end of file */
