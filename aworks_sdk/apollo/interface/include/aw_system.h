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
 * \brief ϵͳ��ط���
 *
 * ֻҪ������ apollo.h �Ϳ���ʹ�ñ�����
 *
 * \par ��ʾ��:�������ִ��ʱ��
 * \code
 *  #include "apollo.h"
 *
 *  int main(){
 *      unsigned int tick;
 *
 *      tick = aw_sys_tick_get();
 *      //  do something
 *      tick = aw_sys_tick_get() - tick;
 *      aw_kprintf("tick used=%d\n , time=%d ms",
 *                  tick,
 *                  aw_ticks_to_ms(tick));
 *      return 0;
 *  }
 * \endcode
 *
 * //�������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.00 13-02-27  zen, first implementation
 * \endinternal
 */

#ifndef __AW_SYSTEM_H
#define __AW_SYSTEM_H

#include "aw_types.h"
#include "aw_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_system
 * \copydoc aw_system.h
 * @{
 */

/**
 * \brief ȡ��ϵͳʱ�ӽ���Ƶ��
 *
 * \return ϵͳʱ�ӽ���Ƶ��, ����Ϊ unsigned long
 */
unsigned long aw_sys_clkrate_get (void);

/**
 * \brief ȡ��ϵͳ��ǰ�Ľ��ļ���ֵ,
 *
 * ϵͳ�����󣬽��ļ���ֵ��0��ʼ��ÿ��1��tick����1 ��ϵͳ���ĵ�Ƶ�ʿɵ���
 * aw_sys_clkrate_get() ��ȡ��
 *
 * \return ϵͳ��ǰ�Ľ��ļ���ֵ, ����Ϊ unsigned int
 *
 * \par ʾ��:�������ִ��ʱ��
 * \code
 *  #include "apollo.h"
 *
 *  int main(){
 *      unsigned int tick;
 *
 *      tick = aw_sys_tick_get();
 *      //  do something
 *      tick = aw_sys_tick_get() - tick;
 *      aw_kprintf("tick used=%d\n , time=%d ms",
 *                  tick,
 *                  aw_ticks_to_ms(tick));
 *      return 0;
 *  }
 * \endcode
 *
 * // �������ݴ���ӡ�����
 */
aw_tick_t aw_sys_tick_get (void);

/**
 * \brief  ��������ʱ��tick�Ĳ�ֵ
 *
 * \param  __t0 t0ʱ�Ľ��ĸ���
 * \param  __t1 t1ʱ�Ľ��ĸ���
 *
 * \return ϵͳ��ǰʱ���ֵ(ʱ�����ʱ���ļ���ֵ)
 *
 * \note ����Ҫ���Ƿ�ת������: �����ı�ʾ�������Զ����ⷴת���⣬��0xfffffff����
 *       ʾ�޷��ŵ����ֵ(32bit)��Ҳ��ʾ�з�������-1.
 */
static aw_inline aw_tick_t aw_sys_tick_diff(aw_tick_t __t0, aw_tick_t __t1)
{
    return __t1 - __t0;
}

/**
 * \brief ת������ʱ��Ϊϵͳʱ�ӽ��ĸ���
 * \param ms ����ʱ��
 * \return ����ʱ���Ӧ�Ľ��ĸ���
 * \note ����������һ�����ĵĽ�����һ�����ġ���һ������Ϊ5���룬��1~5���붼��
 *       ת��Ϊ1�����ġ�
 * \par ʾ��
 * \code
 * #include "apollo.h"
 *
 * aw_task_delay(aw_ms_to_ticks(500));    // ��ʱ500ms
 * \endcode
 */
aw_tick_t aw_ms_to_ticks (unsigned int ms);

/**
 * \brief   ת��ϵͳʱ�ӽ��ĸ���Ϊ����ʱ��
 * \param   ticks ���ĸ���
 * \return  ϵͳʱ�ӽ�������Ӧ�ĺ���ʱ��
 *
 * \par ʾ��:�������ִ��ʱ��
 * \code
 *  #include "apollo.h"
 *
 *  int main(){
 *      unsigned int tick;
 *
 *      tick = aw_sys_tick_get();
 *      //  do something
 *      tick = aw_sys_tick_get() - tick;
 *      aw_kprintf("tick used=%d\n , time=%d ms",
 *                  tick,
 *                  aw_ticks_to_ms(tick));
 *      return 0;
 *  }
 * \endcode
 */
unsigned int aw_ticks_to_ms (aw_tick_t ticks);

 
/**
 * \brief ��ȡ��ǰ��������ID
 *
 * \return ��������ID
 */
uint32_t aw_core_id_self(void);

/**
 * \brief ��ȡ���������ص�ַ��Ӧ��ȫ�ֵ�ַ (�����ַ, DMA��������������ʹ�ô˵�
 *        ַ)
 *
 * \return ȫ�ֵ�ַ
 */
void *aw_global_addr_get(void *p_addr);

/** @} grp_aw_if_system */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif    /* __AW_SYSTEM_H    */

/* end of file */
