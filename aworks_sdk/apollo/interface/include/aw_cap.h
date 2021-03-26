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
 * \brief CAP (capture)���벶���׼����
 *
 * ʹ�ñ�������Ҫ��������ͷ�ļ�:
 * \code
 * #include "aw_cap.h"
 * \endcode
 *
 * \par ʹ��ʾ��
 * \code
 * #include "aw_cap.h"
 *
 * void callback(uint32_t count)
 * {
 *    static unsigned int num = 0;
 *    static unsigned int count1;
 *    unsigned int period_ns;
 *
 *    if (num == 0) {
 *         count1 = count;        // ��һ�β���õ���ֵ 
 *         num = 1;
 *    } else {
 *         // �õ�����������֮���ʱ�䣬�����ε�����  
 *         aw_cap_count_to_time(0,count1,count,&period_ns); 
 *         aw_kprintf("��ò��ε�����Ϊ:%d ns\n",period_ns);
 *    }
 *    count1 = count;            //  ����ֵ              
 * }
 * void aw_main()
 * {
 *     aw_cap_config(0,AW_CAP_TRIGGER_RISE,callback);
 *     aw_cap_enable(0);
 * }
 *  
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 14-06-05  tee, first implementation
 * \endinternal
 */

#ifndef __AW_CAP_H
#define __AW_CAP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_capture
 * \copydoc aw_cap.h
 * @{
 */

/** \brief �������벶���������ش��� */
#define AW_CAP_TRIGGER_RISE          0x01
    
/** \brief �½��ش��� */
#define AW_CAP_TRIGGER_FALL          0x02

/** \brief ˫���ش��� */   
#define AW_CAP_TRIGGER_BOTH_EDGES    0x04


/**
 * \brief ����CAP�Ļص�����
 *
 * \param[in] cap_id ���벶��ͨ����
 * \param[in] pfunc_callback ����ص��������������¼�����ʱ�������������ô˺�����
 *            �ɴ˵õ�����ֵ.�ص�������һ������p_arg���û��������ڶ�������count��
 *            ����ֵ��
 * \param[in] p_arg �û����������ݸ��ص������ĵ�һ������
 * \param[in] flags ��������ı�����������
 *                   - AW_CAP_TRIGGER_RISE           �����ش�������   
 *                   - AW_CAP_TRIGGER_FALL           �½��ش�������
 *                   - AW_CAP_TRIGGER_BOTH_EDGES     ˫���ش�������
 *
 * \retval      AW_OK      �ɹ�
 * \retval     -AW_ENXIO   cap_id Ϊ�����ڵ�ͨ����
 * \retval     -AW_EINVAL  ��Ч����
 *
 */
aw_err_t aw_cap_config(int           cap_id, 
                       unsigned int  flags,
                       void (*pfunc_callback)(void *p_arg,unsigned int count),
                       void         *p_arg);

/**
 * \brief ʹ��CAP�豸��ʹ�ܺ󣬶�ʱ����ʼ��ʱ
 *
 * \param[in] cap_id ���벶��ͨ����
 *
 * \retval      AW_OK      �ɹ�
 * \retval     -AW_ENXIO   cap_id Ϊ�����ڵ�ͨ����
 * \retval     -AW_EINVAL  ��Ч����
 */
aw_err_t aw_cap_enable(int cap_id);

/**
 * \brief ��λCAP�豸����λ�󣬶�ʱ��countֵ��λΪ0
 *
 * \param[in] cap_id ���벶��ͨ����
 *
 * \retval      AW_OK      �ɹ�
 * \retval     -AW_ENXIO   cap_id Ϊ�����ڵ�ͨ����
 * \retval     -AW_EINVAL  ��Ч����
 */
aw_err_t aw_cap_reset(int cap_id);

/**
 * \brief ����CAP�豸
 *
 * \param[in] cap_id ���벶��ͨ����
 *
 * \retval      AW_OK      �ɹ�
 * \retval     -AW_ENXIO   cap_id Ϊ�����ڵ�ͨ����
 * \retval     -AW_EINVAL  ��Ч����
 */
aw_err_t aw_cap_disable(int cap_id);

/**
 * \brief �����β���Ĳ���ֵת��Ϊ��Ӧ��ʱ��ֵ
 *
 * \param[in]   cap_id      ���벶��ͨ����
 * \param[in]   count1      ��һ�β���õ���ֵ
 * \param[in]   count2      �ڶ��β���õ���ֵ
 * \param[out]  p_time_ns   ����õ���ʱ��ֵ(��λ:����)
 *
 * \retval AW_OK     �ɹ�
 */
aw_err_t aw_cap_count_to_time(int           cap_id, 
                              unsigned int  count1,
                              unsigned int  count2,
                              unsigned int *p_time_ns);

/**
 * @}
 */
                              
#ifdef __cplusplus
}
#endif

#endif

/* end of file */
