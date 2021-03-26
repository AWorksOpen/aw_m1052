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
 * \brief �¼�����ӿ�
 *
 * \internal
 * \par modification history:
 * - 1.02 17-09-05  anu, refine the description
 * - 1.01 15-01-05  orz, move event configuration to aw_event_cfg.c.
 * - 1.00 14-06-05  orz, first implementation.
 * \endinternal
 */

#ifndef __AW_EVENT_H
#define __AW_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_event
 * \copydoc aw_event.h
 * @{
 */

#include "aw_types.h"
#include "aw_compiler.h"
#include "aw_msgq.h"

/** \brief �¼������־���� */
enum event_handler_flags {
    EVENT_HANDLER_FLAG_NONE       = 0, /**< \brief ��Ҫ�� */
    EVENT_HANDLER_FLAG_AUTO_UNREG = 1  /**< \brief �¼��������ע���¼� */
};

/** \brief �¼������־���� */
enum event_proc_flags {
    EVENT_PROC_FLAG_ASYNC    = 0, /**< \brief �첽�¼�����  */
    EVENT_PROC_FLAG_SYNC     = 1, /**< \brief ͬ���¼�����  */
    EVENT_PROC_FLAG_CAT_ONLY = 2  /**< \brief ֻ�����¼����� */
};

struct event_type;                /**< \brief �¼��������� */
struct event_category;            /**< \brief �¼��������� */

/**
 * \brief �¼������ص���������
 *
 * \param[in] evt_type �¼�����ָ��
 * \param[in] evt_data �����¼����������
 * \param[in] hdl_data ��ʼ���¼�ʱ������û�����
 *
 * \return ��
 */
typedef void event_function_t (struct event_type *evt_type,
                               void              *evt_data,
                               void              *hdl_data);

/** \brief �¼�������� */
struct event_handler {
    uint16_t              flags; /**< \brief �¼�����ı�־ */
    event_function_t     *func;  /**< \brief �����Ļص����� */
    void                 *data;  /**< \brief �û����� */
    struct event_handler *next;  /**< \brief ��һ���¼���� */
};

/**
 * \brief ��ʼ���¼����
 *
 * \param[in] handler  �¼����
 * \param[in] func     �¼������ص�����
 * \param[in] data     �¼����û�����
 * \param[in] flags    �¼���־
 *
 * \return ��
 */
aw_static_inline void aw_event_handler_init (struct event_handler *handler,
                                             event_function_t     *func,
                                             void                 *data,
                                             uint16_t              flags)
{
    handler->flags = flags;
    handler->func  = func;
    handler->data  = data;
    handler->next  = NULL;
}

/**
 * \brief ����һ���¼����
 *
 * \param[in] handler  �¼����ָ��
 *
 * \return ��
 */
aw_static_inline void aw_event_handler_destroy (struct event_handler *handler)
{
}

/**
 * \brief �¼����Ͷ���
 */
struct event_type {
    struct event_handler  *handler;  /**< \brief �¼����ָ�� */
    struct event_category *category; /**< \brief �������ĸ��¼����� */
    struct event_type     *next;     /**< \brief ��һ���¼�����ָ�� */
};

/**
 * \brief ��ʼ���¼�����
 *
 * \param[in] event �¼�����
 *
 * \return ��
 */
void aw_event_init (struct event_type *event);

/**
 * \brief �����¼�����
 *
 * \param[in] event �¼�����ָ��
 *
 * \return ��
 */
aw_static_inline void aw_event_destroy (struct event_type *event)
{
}

/**
 * \brief ���¼�������ע���¼����
 *
 * \param[in] event    �¼�����ָ��
 * \param[in] handler  �¼����ָ��
 *
 * \return ��
 */
void aw_event_handler_register (struct event_type     *event,
                                struct event_handler  *handler);

/**
 * \brief ���¼������н�ע���¼����
 *
 * \param[in] event    �¼�����ָ��
 * \param[in] handler  �¼����ָ��
 *
 * \return ��
 */
int aw_event_handler_unregister (struct event_type     *event,
                                 struct event_handler  *handler);


/**
 * \brief ����һ���¼�����
 *
 * \note ���øú��������ᵼ���¼������и��¼����͵�������ע�������ص�
 *       �������ж��е��ñ�����
 *
 * \param[in] event     Ҫ�������¼�����
 * \param[in] evt_data  Ҫ�����¼�������
 * \param[in] proc_type �¼������־ \sa enum event_proc_flags
 *
 * \return AW_OK�򴥷��ɹ�����������ֵ��ο��������
 */
int aw_event_raise (struct event_type     *event,
                    void                  *evt_data,
                    enum event_proc_flags  proc_type);

/**
 * \brief �¼�����Ķ���
 *
 * \note һ���¼�������԰���һ�������¼�����
 */
struct event_category {
    struct event_type    *event;    /**< \brief �¼�����ָ�� */
    struct event_handler *handler;  /**< \brief �¼����ָ�� */
};

/**
 * \brief ��ʼ���¼�����
 *
 * \param[in] category �¼�����ָ��
 *
 * \return ��
 */
void aw_event_category_init (struct event_category *category);

/**
 * \brief �����¼�����
 *
 * \param[in] category �¼�����ָ��
 *
 * \return ��
 */
aw_static_inline void aw_event_category_destroy (
        struct event_category *category)
{
}

/**
 * \brief ע��һ���¼�������¼�������
 *
 * \param[in] category �¼�����ָ��
 * \param[in] handler  �¼����ָ��
 *
 * \return ��
 */
void aw_event_category_handler_register (struct event_category *category,
                                         struct event_handler  *handler);

/**
 * \brief ��ע��һ���¼�������¼�������
 *
 * \param[in] category �¼�����ָ��
 * \param[in] handler  �¼����ָ��
 *
 * \return ��
 */
int aw_event_category_handler_unregister (struct event_category *category,
                                          struct event_handler  *handler);

/**
 * \brief ��ע��һ���¼����͵��¼�������
 *
 * \param[in] category �¼�����ָ��
 * \param[in] event    �¼�����ָ��
 *
 * \return ��
 */
void aw_event_category_event_register (struct event_category *category,
                                       struct event_type     *event);

/**
 * \brief �¼���Ϣ�Ķ���
 *
 * \note �¼���Ϣ�����첽�����¼����ڲ�ʹ��
 */
struct event_msg {
    struct event_type *event;     /**< \brief �¼�����ָ�� */
    void              *evt_data;  /**< \brief ������¼����� */
    int                cat_only;  /**< \brief �Ƿ�ֻ�ܹ����¼����ദ�� */
};

/**
 * \brief ��Ҫ���͵���Ϣ�����Ŷ�
 *
 * \param[in] msg  �¼���Ϣ
 *
 * \return AW_OK�ɹ���������У���������ֵ��ο��������
 */
aw_err_t aw_event_cfg_msg_send (struct event_msg *msg);

/**
 * \brief �¼��������
 *
 * \note ���������ڲ�ʹ��
 *
 * \param[in] msgq_id  ��Ϣ���е�ID��
 *
 * \return ��
 */
void aw_event_task (void *msgq_id);

/**
 * \brief �¼��ĳ�ʼ��������
 *
 * \note �ú����� aw_event_cfg.c ��ʵ�� ��aw_event_lib_init()�б�����, 
 *       ��Ҫ���е���
 *
 * \return ��
 */
void aw_event_cfg_init (void);

/**
 * \brief �¼���ĳ�ʼ��
 *
 * \note �ú����ڲ�ʹ��
 *
 * \return ��
 */
void aw_event_lib_init (void);

/** @} grp_aw_if_event */

#ifdef __cplusplus
}
#endif

#endif /* } __AW_EVENT_H */

/* end of file */
