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
 * \brief ��������ض���ͷ�ļ�
 *
 * \internal
 * \par modification history:
 * - 1.10 14-09-28	zen, move some defines to aw_psp_complier.h
 * - 1.01 14-08-12  orz, add define of restrict
 * - 1.00 13-01-15  orz, move defines from aw_common.h
 * \endinternal
 */

#ifndef __AW_COMPILER /* { */
#define __AW_COMPILER

#include "aw_psp_compiler.h"


#define aw_inline           aw_psp_inline         /** \brief inline ����  */
#define aw_static_inline    aw_psp_static_inline  /** \biref static inline ����*/

#define aw_section(x)       aw_psp_section(x)     /** \brief __section() ���� */

#define aw_inline           aw_psp_inline
#define aw_static_inline    aw_psp_static_inline

#define aw_local            aw_psp_local
#define aw_const            aw_psp_const
#define aw_import           aw_psp_import
#define aw_export           aw_psp_export

#include "aw_barrier.h"

static void aw_inline __write_once_8(uint8_t *p_addr,uint8_t data)
{
    *(volatile uint8_t *)p_addr = data;
}

static void aw_inline __write_once_16(uint16_t *p_addr,uint16_t data)
{
    *(volatile uint16_t *)p_addr = data;
}

static void aw_inline __write_once_32(uint32_t *p_addr,uint32_t data)
{
    *(volatile uint32_t *)p_addr = data;
}

static uint8_t aw_inline __read_once_8(uint8_t *p_addr)
{
    register uint8_t        data;

    data = *((volatile uint8_t *)p_addr);
    aw_smp_read_barrier_depends();
    return data;
}

static uint16_t aw_inline __read_once_16(uint16_t *p_addr)
{
    register uint16_t       data;

    data = *((volatile uint16_t *)p_addr);
    aw_smp_read_barrier_depends();
    return data;
}

static uint32_t aw_inline __read_once_32(uint32_t *p_addr)
{
    register uint32_t       data;

    data = *((volatile uint32_t *)p_addr);
    aw_smp_read_barrier_depends();
    return data;
}

/*
 * READ_ONCE��WRITE_ONCEϵ�нӿڣ�������ֹ�������Ż���������˵��������ֹ��������
 * ����д����ζ����Ͷ�д�����򡣵�����ֹ��д������ֻ���ڱ�������ʶ�����������
 * �������ʱ��һ���ñ�������ʶ������ķ�������C������Ƕ����READ_ONCE��WRITE_ONCE
 *
 * ��Щ�ӿڵ���Ҫ��;��:������������ISR����֮���ͨ�ţ���������ISR��������
 * ͬһ��CPU�ϣ�;��֤�����������ӷ�Χ���ʣ�˺�ѷ��ʻ������Ӽ��ٷ��ʴ���
 *
 * �ܶ���֮����Щ���ĳ�̶ֳ��Ͽ������volatile�ؼ��֣�����ʵ���ϱ��Ƽ���Ϊ������
 * ����volatileʹ�õķ�����ʹ����Щ�꣬�����ó���Ա������������ʶ���Լ��ı���
 * ��Ҫʲô�̶ȵı����������ж����������������ǲ���Ҫ��
 *
 * volatie��ʹ��Ҳ���ʹ�úܶ����Ա�ڱ����Ƿ���Ҫ������ʱ����ɾ��裬��Ϊvolatile
 * ���㹻�ˣ���ʵ�ϣ��൱�������²����㹻
 *
 * ��Щ�������ĳЩ����Ҫ������������£��������ô�������������Ĺ�������ʵ�ִ���
 * ���жϻ��̰߳�ȫ
 *
 * READ_ONCEϵ�к�������һ��aw_smp_read_barrier_depends�������ԭ��ɲμ�barrier
 * ��ص��ĵ�
 */

/** \brief �������Ƕȱ�֤8λд����˺�ѣ��ϲ�����չ���ظ���ɾ����������  */
#define WRITE_ONCE8(x, val)     __write_once_8((uint8_t *)&(x),(uint8_t)(val));
/** \brief �������Ƕȱ�֤16λд����˺�ѣ��ϲ�����չ���ظ���ɾ����������  */
#define WRITE_ONCE16(x, val)    __write_once_16((uint16_t *)&(x),(uint16_t)(val));
/** \brief �������Ƕȱ�֤32λд����˺�ѣ��ϲ�����չ���ظ���ɾ����������  */
#define WRITE_ONCE32(x, val)    __write_once_32((uint32_t *)&(x),(uint32_t)(val));

/** \brief �������Ƕȱ�֤8λ������˺�ѣ��ϲ�����չ���ظ���ɾ����������  */
#define READ_ONCE8(x)           __read_once_8((uint8_t *)&(x));
/** \brief �������Ƕȱ�֤16λ������˺�ѣ��ϲ�����չ���ظ���ɾ����������  */
#define READ_ONCE16(x)          __read_once_16((uint16_t *)&(x));
/** \brief �������Ƕȱ�֤16λ������˺�ѣ��ϲ�����չ���ظ���ɾ����������  */
#define READ_ONCE32(x)          __read_once_32((uint32_t *)&(x));

#endif /* } __AW_COMPILER */

/* end of file */
