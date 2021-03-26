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
 * \brief ������ϵͳͨ�ñ�׼�ӿ�
 *
 *    ͨ������ӿڰ�����ꡢ���̡�ҡ�ˡ������豸��
 *
 *    �����豸ͨ���ᷢ�����������¼�, ����, ��귢����������¼��������¼�, ����
 *    ���������¼�,
 *    �������������������¼�. ��������ϵͳ���������¼����ĸ�������豸����, ����
 *    ����Ͷ���¼�.
 *
 *    ������ϵͳ��ʹ������ѭ"˭����˭����"��ԭ��, �û�ע��ص�������ϵͳ, �ڻص�
 *    ������, ������ǿ��ת��Ϊ�����ĵ��������ͼ���.
 *
 * \par ʹ��˵��
 * \code
 *
 * (1) ʹ���ϱ��¼���׼�ӿ�ǰ, ����ȷ��������������ϵͳ(aw_prj_params.h)
 *
 * ......
 *
 * #define AW_COM_AWBL_INPUT_EV             // Input Service�¼�����
 * #define AW_COM_AWBL_INPUT_EV_KEY         // Input Service�����¼�
 * #define AW_COM_AWBL_INPUT_EV_ABS         // Input Service�����¼�
 *
 * ......
 *
 * (2) ��ʼ���¼���, �Լ��� AWorksƽ̨ע��������ϵͳ(aw_prj_config.c)
 * static void aw_prj_task_level_init( void )
 * {
 * ......
 *
 *   aw_event_lib_init();           // event lib initialization
 *
 * #ifdef AW_COM_AWBL_INPUT_EV
 *   awbl_input_ev_mgr_register();
 * #endif
 *
 * ......
 * }
 *
 * (3) �����⴦ʹ��������ϵͳ��׼�ӿ�ǰ�����Ȱ���aw_input.hͷ�ļ�
 *
 * \endcode
 *
 * \par ��ʾ��
 * \code
 *
 * #include "aw_input.h"
 *
 * (1) �������⴦�����ϱ��ӿ�, �ϱ����ݣ�ͨ���ɰ��������ϱ���
 *
 * aw_input_report_key(AW_INPUT_EV_KEY, KEY_1, 1);
 *
 * (2) ע��ص������������ϱ����ݣ��û��������ڴ��������¼���
 *
 * void key_proc0_cb (aw_input_event_t *p_input_data, void *p_usr_data)
 * {
 *     if (p_input_data->ev_type != AW_INPUT_EV_KEY) {
 *         return;
 *     }
 *
 *     aw_input_key_data_t *p_key_data = (aw_input_key_data_t *)p_input_data;
 *
 *     switch (key_data->key_code) {
 *
 *     case KEY_1 :
 *         if (p_key_data->key_state) {
 *             AW_INFOF(("KEY1 is down : %d.\n", p_key_data->key_state));
 *         } else {
 *             AW_INFOF(("KEY1 is up : %d.\n", p_key_data->key_state));
 *         }
 *         break;
 *         //...
 *     }
 * }
 *
 * void my_func (void)
 * {
 *     aw_local struct aw_input_handler input_hdlr;
 *
 *     aw_input_handler_register(&input_hdlr, key_proc0_cb, NULL);
 * }
 *
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.01 15-01-07  ops, encapsulate aw_input_handler and redefine how to 
 *                       register/unregister.
 * - 1.00 14-07-18  ops, first implementation.
 * \endinternal
 */

#ifndef __AW_INPUT_H
#define __AW_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_input
 * \copydoc aw_input.h
 * @{
 */

#include "aw_input_code.h"
#include "aw_types.h"
#include "aw_event.h"

/**
 * \name �����¼�����
 * @{
 */

#define AW_INPUT_EV_KEY    0             /**< \brief �����¼� */
#define AW_INPUT_EV_ABS    1             /**< \brief �����¼� */
#define AW_INPUT_EV_REL    2             /**< \brief ����¼� */

/** @} */

/**
 * \brief ���궨��
 */
typedef struct aw_input_pos {

    int                    x;            /**< \brief x������ֵ */
    int                    y;            /**< \brief y������ֵ */
    int                    z;            /**< \brief z������ֵ */
} aw_input_pos_t;

/**
 * \brief �����¼�
 */
typedef struct aw_input_event {
    /**
     * \brief �¼�������
     * ��ǰ֧�ֵ��¼������У�
     * - AW_INPUT_EV_KEY
     * - AW_INPUT_EV_ABS
     * - AW_INPUT_EV_REL
     */
    int                    ev_type;
} aw_input_event_t;

/** \brief �������� */
typedef struct aw_input_key_data {

    aw_input_event_t       input_ev;     /**< \brief  �¼����� */
    int                    key_code;     /**< \brief  ���������� */
    
     /**
      * \brief  ����״̬ 
      *
      * 0   : �����ͷ�
      * ��0 : �������¡���ֵ�����������أ���һ��Ϊ1�����ڲ��ֿ��Ը��ܰ���ѹ��
      *       �İ���������ͨ����ֵ���ְ����İ��³̶ȣ�ֵԽ�󣬰��µĳ̶�Խ��
      *       ������ͨ�Ľ���������״̬�İ���������ͨ����ֵ�Ƿ�Ϊ�����жϰ����Ƿ�
      *       ���¼��ɡ�
      */
    int                    key_state;
} aw_input_key_data_t;

/** \brief ָ�������豸���� */
typedef struct aw_input_ptr_data {

    aw_input_event_t       input_ev;     /**< \brief  �¼����� */
    int                    ptr_code;     /**< \brief  ָ�������豸������ */
    aw_input_pos_t         pos;          /**< \brief  ����ֵ */
} aw_input_ptr_data_t;

/** \brief ͳһ�������豸�������� */
typedef union aw_input_data {

    aw_input_key_data_t    key_data;     /**< \brief  ���� */
    aw_input_ptr_data_t    ptr_data;     /**< \brief  ָ��(���) */
} aw_input_data_t;

/**
 * \brief ������ϵͳ���ݽ��ջص���������.
 *
 * \param[in] p_input_data : �������ݡ�ʵ����������Ӧ���� p_input_data->input_ev
 *                           �ж����磺����Ϊ AW_INPUT_EV_KEY�����Ӧ��ʵ������
 *                           ����Ϊ��aw_input_key_data_t���ڻص������пɽ�
 *                           p_input_data ǿתΪ aw_input_key_data_t* ����ʹ��
 * \param[in] p_usr_data   : �û�����
 *
 * \return ��
 */
typedef void (*aw_input_cb_t) (aw_input_event_t *p_input_data, void *p_usr_data);

/**
 * \brief �����¼�������
 */
typedef struct aw_input_handler {

    struct event_handler  ev_handler;  /**< \brief �¼���� */
    aw_input_cb_t         pfn_cb;      /**< \brief ������ϵͳ���ݽ��ջص����� */
    void                 *p_usr_data;  /**< \brief �û��Զ������� */
} aw_input_handler_t;

/*******************************************************************************
      ������ϵͳ �������ϱ��ӿڣ�
*******************************************************************************/

/**
 * \brief �ϱ�һ�������¼���������ϵͳ
 *
 * \param[in] ev_type     �¼�������
 * \param[in] code        ������
 * \param[in] key_state   ����״̬: 0, �����ͷ�; ��0, ��������
 *
 * \retval   AW_OK      �ϱ��ɹ�
 * \retval  -AW_ENODEV  δע����¼�
 */
int aw_input_report_key (int ev_type, int code, int key_state);

/**
 * \brief �ϱ�һ�������¼���������ϵͳ
 *
 * \param[in] ev_type     �¼�������
 * \param[in] code        ������  (ABS_COORD : ��������, REL_COORD : �������)
 * \param[in] coord_x     X����
 * \param[in] coord_y     Y����
 * \param[in] coord_z     Z����
 *
 * \retval   AW_OK      �ϱ��ɹ�
 * \retval  -AW_ENODEV  δע����¼�
 */
int aw_input_report_coord (int   ev_type,
                           int   code,
                           int   coord_x,
                           int   coord_y,
                           int   coord_z);

/*******************************************************************************
      ������ϵͳ��ע��/ע���ӿڣ�
*******************************************************************************/

/**
 * \brief ע��һ�������¼���������������ϵͳ
 *
 * \param[in] p_input_handler  : �����¼�������ʵ��
 * \param[in] pfn_cb           : �¼�����ص�����.
 * \param[in] p_usr_data       : �û��Զ�������, ����, ��ΪNULL.
 *
 * \retval  AW_OK      �ɹ�ע��
 * \retval -AW_EINVAL  ��p_input_handler��p_cb_funcΪNULLʱ, ע��ʧ��.
 */
aw_err_t aw_input_handler_register (aw_input_handler_t  *p_input_handler,
                                    aw_input_cb_t        pfn_cb,
                                    void                *p_usr_data);

/**
 * \brief ��������ϵͳ��ע��ָ���������¼�������
 *
 * \param[in] p_input_handler  :  ��ע���������¼�������
 *
 * \retval  AW_OK      �ɹ�ע��.
 * \retval -AW_EINVAL  ��p_input_handlerΪNULLʱ, ע��ʧ��.
 */
aw_err_t aw_input_handler_unregister (aw_input_handler_t *p_input_handler);


/** @} grp_aw_if_input */

#ifdef __cplusplus
}
#endif

#endif /* __AW_INPUT_H */

/* end of file */
