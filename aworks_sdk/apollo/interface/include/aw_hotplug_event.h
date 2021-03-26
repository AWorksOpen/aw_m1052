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
 * \brief �Ȳ������ӿ�
 *
 * ʹ�ñ�ģ����Ҫ����ͷ�ļ� aw_hotplug_event.h
 *
 * \par ��ʾ��
 * \code
 *
 * #include "aw_hotplug_event.h"
 *
 * aw_hotplug_event_t event;
 *
 * static int ref_callback (int stat, void *p_arg)
 * {
 *     if (AW_HOTPLUG_EVT_INSTALL == stat) {
 *         aw_kprintf("refcnt test:sdcard install!\n");
 *     } else if (AW_HOTPLUG_EVT_INSTALL == stat){
 *         aw_kprintf("refcnt test:sdcard remove!\n");
 *     } else {
 *         aw_kprintf("refcnt test:err\n");
 *     }
 *
 *     return AW_OK;
 * }
 *
 * void test_hotplug (void)
 * {
 *     int stat = 0;
 *
 *     aw_hotplug_event_register(&event,
 *                               "/dev/sd0",
 *                               AW_HOTPLUG_EVT_INSTALL |
 *                               AW_HOTPLUG_EVT_REMOVE,
 *                               ref_callback,
 *                               NULL);
 *
 *     if (AW_HOTPLUG_EVT_INSTALL != aw_hotplug_stat_get(&event, &stat)) {
 *         stat = AW_HOTPLUG_EVT_INSTALL;
 *         aw_hotplug_event_wait(&event, stat, 10000);
 *     }

 *     while (1) {
 *         aw_mdelay(100);
 *     }
 * }
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-08-08 bob, first implementation.
 * \endinternal
 */

#ifndef __AW_HOTPLUG_EVT_H
#define __AW_HOTPLUG_EVT_H

/**
 * \addtogroup grp_aw_if_hotplug_event
 * \copydoc aw_hotplug_event.h
 * @{
 */

#include "aw_types.h"
#include "aw_common.h"
#include "aw_list.h"
#include "aw_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief �豸�����¼���״̬�� */
#define AW_HOTPLUG_EVT_INSTALL    0x01

/** \brief �豸�Ƴ��¼���״̬�� */
#define AW_HOTPLUG_EVT_REMOVE     0x02

/** \brief �����¼���״̬�� */
#define AW_HOTPLUG_EVT_ALL        0xFFFFFFFF

/**
 * \brief �Ȳ���¼��ṹ��
 */
typedef struct aw_hotplug_event {

    /** \brief �¼��ڵ� */
    struct aw_list_head node;

    /** \brief �¼��������豸�� */
    const char         *p_devname;

    /** \brief �¼���־ */
    int                 event_flgs;

    /** \brief �¼��ص����� */
    int               (*pfn_callback)(int event_flag, void *p_arg);

    /** \brief �ص��������� */
    void               *p_arg;
} aw_hotplug_event_t;


/**
 * \brief �Ȳ���豸�ṹ��
 */
typedef struct aw_hotplug_dev {

    /** \brief �豸�ڵ� */
    struct aw_list_head     node;

    /** \brief �豸�� */
    const char             *p_devname;

    /** \brief �Ȳ���豸״̬ */
    int                     stat;
} aw_hotplug_dev_t;


/**
 * \brief ע���Ȳ���¼�
 *
 * \param[in]  p_event        : ָ���Ȳ���¼�
 * \param[in]  p_name         : �Ȳ���¼������������
 * \param[in]  event_flgs     : �¼���־
 * \param[in]  pfn_callback   : �¼��ص�����
 * \param[in]  p_arg          : �ص���������
 *
 * \retval AW_OK              : ��ʼ���ɹ�
 * \retval -AW_EINVAL         : ������Ч
 */
aw_err_t aw_hotplug_event_register (aw_hotplug_event_t *p_event,
                                    const char         *p_name,
                                    int                 event_flgs,
                                    int               (*pfn_callback)(int   flag,
                                                                      void *p_arg),
                                    void               *p_arg);

/**
 * \brief ȡ��ע���Ȳ���¼�
 *
 * \param[in]  p_event        : ָ���Ȳ���¼�
 *
 * \return ��
 */
void aw_hotplug_event_unregister (aw_hotplug_event_t *p_event);

/**
 * \brief �Ȳ���¼�����¼�����
 *
 * \param[in]      p_event        : ָ���Ȳ���¼�
 * \param[in,out]  p_event_flgs   : �����Ҫ�������¼�
 *
 * \retval AW_OK              : ��ʼ���ɹ�
 * \retval -AW_EINVAL         : ������Ч
 *
 * \note p_event_flgsָ�뷵������¼���������¼���
 */
aw_err_t aw_hotplug_event_add (aw_hotplug_event_t *p_event,
                               int                *p_event_flgs);

/**
 * \brief �Ȳ���¼�ɾ���¼�����
 *
 * \param[in]      p_event        : ָ���Ȳ���¼�
 * \param[in,out]  p_event_flgs   : ɾ����Ҫ�������¼�
 *
 * \retval AW_OK              : ��ʼ���ɹ�
 * \retval -AW_EINVAL         : ������Ч
 *
 * \note p_event_flgsָ�뷵��ɾ���¼���������¼���
 */
aw_err_t aw_hotplug_event_del (aw_hotplug_event_t *p_event,
                               int                *p_event_flgs);

/**
 * \brief �ȴ�ĳ���¼���״̬������
 *
 * \param[in]  p_event        : ָ���Ȳ���¼�
 * \param[in]  stat           : �¼���״̬��
 * \param[in]  time_out       : ��ʱ
 *
 * \retval AW_OK              : ��ʼ���ɹ�
 * \retval -AW_EINVAL         : ������Ч
 * \retval -AW_ENOMEM         : �ڴ�ռ䲻��
 * \retval -AW_ETIME          : ��ʱ
 */
aw_err_t aw_hotplug_event_wait (aw_hotplug_event_t *p_event,
                                int                 stat,
                                int                 time_out);

/**
 * \brief ��ȡ�Ȳ���豸��ǰ״̬
 *
 * \param[in]  p_event        : ָ���Ȳ���¼�
 * \param[out] p_stat         : �豸״̬
 *
 * \retval AW_OK              : ��ʼ���ɹ�
 * \retval -AW_EINVAL         : ������Ч
 */
aw_err_t aw_hotplug_stat_get (aw_hotplug_event_t* p_event,
                              int                *p_stat);

/**
 * \brief �Ȳ���豸ע��
 *
 * \param[in]  p_dev          : �Ȳ���豸
 * \param[in]  p_name         : �豸��
 * \param[in]  flags          : ֧�ֵ��¼�
 *
 * \retval AW_OK              : ��ʼ���ɹ�
 * \retval -AW_EINVAL         : ������Ч
 * \retval -AW_EEXIST         : �豸�Ѵ���
 */
aw_err_t aw_hotplug_dev_register (aw_hotplug_dev_t *p_dev,
                                  const char       *p_name,
                                  int               flags);

/**
 * \brief �Ȳ���豸ȡ��ע��
 *
 * \param[in]  p_dev          : �Ȳ���豸
 *
 * \retval AW_OK              : ��ʼ���ɹ�
 * \retval -AW_EINVAL         : ������Ч
 */
aw_err_t aw_hotplug_dev_unregister (aw_hotplug_dev_t *p_dev);

/**
 * \brief ���������¼����Ȳ�����
 *
 * \param[in]  p_dev          : �Ȳ���豸
 * \param[in]  event_flgs     : ���͵��¼�
 *
 * \retval AW_OK              : ��ʼ���ɹ�
 * \retval -AW_EINVAL         : ������Ч
 */
aw_err_t aw_hotplug_event_send (aw_hotplug_dev_t *p_dev,
                                int               event_flgs);

/**
 * \brief �Ȳ�������ʼ��
 *
 * \retval ��
 */
void aw_hotplug_init(void);

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_hotplug_event */

#endif /* } __AW_HOTPLUG_EVT_H */

