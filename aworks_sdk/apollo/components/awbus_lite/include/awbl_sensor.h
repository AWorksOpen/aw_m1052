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
 * \brief �������豸����
 *
 * \internal
 * \par modification history
 * - 1.00 18-05-15  tee, first implementation
 * \endinternal
 */
#ifndef __AWBL_SENSOR_H
#define __AWBL_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_sensor.h"
#include "aw_sem.h"

/**
 * \addtogroup grp_awbl_if_sensor
 * \copydoc awbl_sensor.h
 * @{
 */

/**
 * \brief ��������ֵ����Ϊ��Чֵ��������������ʹ�ã�
 *
 * ��ĳһͨ���������⣬���ݶ�ȡʧ�ܣ���Ӧʹ�øú�������Ӧ�����ֵ����Ϊ��Чֵ��
 *
 * \param[in] p_val : ָ��Ҫ����Ϊ��Чֵ������
 * \param[in] errno : ��׼�����룬
 *
 * \note ��������ʹ�øú꽫ĳһ������ֵ����Ϊ��Чֵʱ�������벻��Ϊ  -AW_ENODEV��
 * ��ͨ�������ڣ���ʧ��ԭ��������м��ʹ�á�Ҳ����˵������������Ӧ���ж�ͨ����
 * ����ڣ��������������֧�ֵļ���ͨ�����ɣ��������ͨ����Ӧ�����Ѿ����ڵġ�
 *
 * ��������Ϊ -AW_ENODEV�� ��ǿ��ת��Ϊ�����룺-AW_EIO��
 */
#define AWBL_SENSOR_VAL_SET_INVALID(p_val, errno)                      \
    do {                                                               \
        (p_val)->val  = ((errno == (-AW_ENODEV)) ? (-AW_EIO) : errno); \
        (p_val)->unit = AW_SENSOR_UNIT_INVALID;                        \
    } while(0)

/**
 * \brief ��������������״̬���� val �У�������������ʹ�ã�
 *
 * ��ʹ�ܻ���ܶ��ͨ��ʱ��������ֵ���ڴ洢��ǰͨ�������Ľ����
 *
 * \param[in] p_val  : ָ��Ҫ����Ϊ��Чֵ������
 * \param[in] result : ���β����Ľ�������͵ģ� AW_OK���ɹ��� -AW_EIO��I/O����
 *
 *
 * \note ��������ʹ�øú�ʱ�������������Ϊ  -AW_ENODEV����ͨ�������ڣ�
 * -AW_ENODEV �����м��ʹ�á�Ҳ����˵������������Ӧ���ж�ͨ���Ƿ�
 * ���ڣ��������������֧�ֵļ���ͨ�����ɣ��������ͨ����Ӧ�����Ѿ����ڵġ�
 *
 * ��������Ϊ -AW_ENODEV�� ��ǿ��ת��Ϊ�����룺-AW_EIO��
 */
#define AWBL_SENSOR_VAL_SET_RESULT(p_val, result)                        \
    do {                                                                 \
        (p_val)->val  = ((result == (-AW_ENODEV)) ? (-AW_EIO) : result); \
        (p_val)->unit = AW_SENSOR_UNIT_INVALID;                          \
    } while(0)


struct awbl_sensor_servfuncs {

    /**
     * \brief ��ָ��ͨ����ȡ��������
     *
     * \param[in]  p_cookie : �����Զ������
     * \param[in]  p_ids    ��������ͨ�� id �б� p_id[0]��Ȼ���ڸ��豸
     * \param[in]  num      ��ͨ����Ŀ
     * \param[out] p_buf    : �����������������������С�� chan_num һ��
     *
     * \retval  AW_OK    �ɹ���һ������ͨ�����ݶ�ȡ�ɹ�
     * \retval    <0     ���ֻ�����ͨ����ȡʧ��
     *
     * \attention ��ͨ����ȡʧ��ʱ����������Ӧʹ�� AWBL_SENSOR_VAL_SET_INVALID()
     * ���ͨ����Ӧ�Ĵ�����ֵ���Ϊ��Ч�����ɲ������ڸô������豸��ͨ�������ɷ�
     * �ʻ��������ͨ����Ӧ�� ������ֵ��
     *
     * \note p_ids �б���ܰ������ͨ�������м����øú���ʱ������������һ��
     * ͨ����p_ids[0]���Ǹô������ܹ��ɼ��ġ�����ͨ��ʱ�ô������ṩ�ģ������ִ�
     * ��������֧�ֶ��ͨ������ʱ������Ӧ��������������ͨ��������������ͨ����
     * Ҫ�ɼ����򴫸���Ӧ�òɼ�������p_buf����Ӧλ��������ݡ����磬����������
     * �ɼ��б�ĵ�0��ͨ���͵�3��ͨ������Ӧ���p_buf[0] ��  p_buf[3]���������
     * ��������ͨ�����������Է��ʻ�ı䡣
     */
    aw_err_t (*pfn_data_get)(void                     *p_cookie,
                             const int                *p_ids,
                             int                       num,
                             aw_sensor_val_t          *p_buf);
                             
    /**
     * \brief ʹ�ܴ�������ĳһͨ��
     * 
     * ʹ�ܺ󣬴�����ͨ���������������ʼ�ɼ��ⲿ�źš��������˴������Ĳ���Ƶ
     * ��#AW_SENSOR_ATTR_SAMPLING_RATE������ֵ��Ϊ0����ϵͳ���Զ���������ֵָ��
     * �Ĳ�������ֵ��Ƶ�ʻ�ȡ�������еĲ���ֵ�����û���������׼��������������
     * ÿ�����ݲ��������󣬶��������û��ص��������ر�أ�������Ƶ��Ϊ0��Ĭ�ϣ���
     * ���Զ�������ÿ�ε��� pfn_data_get()����ʱ�ŴӴ������л�ȡ�������ݡ�
     * 
     * \param[in] p_cookie : �����Զ������
     * \param[in] p_ids    ��������ͨ�� id �б� p_id[0]��Ȼ���ڸ��豸
     * \param[in] num      ��������ͨ����Ŀ
     * \param[in] p_result ��ÿ��ͨ��ʹ�ܵĽ������valֵ�У����׼�����뺬��һ��
     *
     * \retval  AW_OK  ����ͨ��ʹ�ܳɹ�
     * \retval   <0    ���ֻ�����ͨ��ʹ��ʧ�ܣ�ÿһ��ͨ��ʹ�ܵĽ������ val ֵ��
     *
     * \attention p_result ָ��Ļ������ڴ洢ÿ��ͨ��ʹ�ܵĽ������������Ӧʹ��
     * AWBL_SENSOR_VAL_SET_RESULT() ������ÿ��ͨ��ʹ�ܵĽ�����ر�أ���p_result
     * ΪNULL�������������ÿ��ͨ��ʹ�ܵĽ����
     *
     * \note ����ֵΪNULL�������ͨ������ʹ�ܣ��Ѿ�Ĭ�ϴ���ʹ��״̬��
     */
    aw_err_t (*pfn_enable) (void                  *p_cookie,
                            const int             *p_ids,
                            int                    num,
                            aw_sensor_val_t       *p_result);
                                 
    /**
     * \brief ���ܴ�����ͨ��
     * 
     * ���ܺ󣬴��������رգ���������ֹͣ�ɼ��ⲿ�źţ�ͬʱ��ϵͳҲ���ٴӴ�����
     * �л�ȡ����ֵ�����۲���Ƶ���Ƿ�Ϊ0����
     *
     * \param[in] p_cookie : �����Զ������
     * \param[in] p_ids    ��������ͨ�� id �б� p_id[0]��Ȼ���ڸ��豸
     * \param[in] num      ��������ͨ����Ŀ
     * \param[in] p_result ��ÿ��ͨ��ʹ�ܵĽ������valֵ�У����׼�����뺬��һ��
     *
     * \retval  AW_OK  ����ͨ��ʹ�ܳɹ�
     * \retval   <0    ��׼�����룬���ֻ�����ͨ��ʹ��ʧ��
     *
     * \attention p_result ָ��Ļ������ڴ洢ÿ��ͨ�����ܵĽ������������Ӧʹ��
     * AWBL_SENSOR_VAL_SET_RESULT() ������ÿ��ͨ�����ܵĽ�����ر�أ���p_result
     * ΪNULL�������������ÿ��ͨ�����ܵĽ����
     *
     * \note ����ֵΪNULL�������ͨ���޷����ܣ�ʼ�մ���ʹ��״̬��
     */
    aw_err_t (*pfn_disable) (void                  *p_cookie,
                             const int             *p_ids,
                             int                    num,
                             aw_sensor_val_t       *p_result);
                                 
    /**
     * \brief ���ô�����ͨ������
     *
     * \param[in] p_cookie : �����Զ������
     * \param[in] id       ��������ͨ���� ��ͨ����Ȼ���ڸ��豸
     * \param[in] attr     �����ԣ�AW_SENSOR_ATTR_*(eg:
     *                       #AW_SENSOR_ATTR_SAMPLING_RATE)
     * \param[in] p_val    ������ֵ
     *
     * \return AW_OK���ɹ�������ֵ��ʧ�ܣ������׼����Ŷ��塣
     * 
     * \note ֧�ֵ���������崫������أ����ִ��������ܲ�֧���κ����ԡ���ʱ����
     * ���ú�������ΪNULL��
     */
    aw_err_t (*pfn_attr_set)(void                    *p_cookie,
                             int                      id,
                             int                      attr,
                             const aw_sensor_val_t   *p_val);

    /**
     * \brief ��ȡ������ͨ������
     *
     * \param[in]  p_cookie : �����Զ������
     * \param[in]  id       ��������ͨ�� id�� ��ͨ����Ȼ���ڸ��豸
     * \param[in]  attr     : ���ԣ�AW_SENSOR_ATTR_*(eg:
     *                        #AW_SENSOR_ATTR_SAMPLING_RATE)
     * \param[out] p_val    : ���ڻ�ȡ����ֵ
     *
     * \return AW_OK���ɹ�������ֵ��ʧ�ܣ������׼����Ŷ��塣
     * 
     * \note ֧�ֵ���������崫������أ����ִ��������ܲ�֧���κ����ԡ���ʱ����
     * ���ú�������ΪNULL��
     */
    aw_err_t (*pfn_attr_get) (void                   *p_cookie,
                              int                     id,
                              int                     attr,
                              aw_sensor_val_t        *p_val);

    /**
     * \brief ���ô�����һ��ͨ����������һ�������ص�����
     *
     * \param[in] p_cookie : �����Զ������
     * \param[in] id       ��������ͨ�� id�� ��ͨ����Ȼ���ڸ��豸
     * \param[in] flags    : ������־��ָ������������
     * \param[in] pfn_cb   : ����ʱִ�еĻص�������ָ��
     * \param[in] p_arg    : �ص������û�����
     *
     * \return AW_OK���ɹ�������ֵ��ʧ�ܣ������׼����Ŷ��塣
     *
     * \note ֧�ֵĴ���ģʽ����崫������أ����ִ�������֧���κδ���ģʽ����ʱ��
     * �ɽ��ú�������ΪNULL��
     */
    aw_err_t  (*pfn_trigger_cfg) (void                     *p_cookie,
                                  int                       id,
                                  uint32_t                  flags,
                                  aw_sensor_trigger_cb_t    pfn_cb,
                                  void                     *p_arg);
 
    /**
     * \brief �򿪴���
     *
     * \param[in] p_cookie : �����Զ������
     * \param[in] id       ��������ͨ�� id�� ��ͨ����Ȼ���ڸ��豸
     
     * \return AW_OK���ɹ�������ֵ��ʧ�ܣ������׼����Ŷ��塣
     */
    aw_err_t (*pfn_trigger_on) (void *p_cookie, int id);

    /**
     * \brief �رմ���
     *
     * \param[in] p_cookie : �����Զ������
     * \param[in] id       ��������ͨ�� id�� ��ͨ����Ȼ���ڸ��豸
     *
     * \return AW_OK���ɹ�������ֵ��ʧ�ܣ������׼����Ŷ��塣
     */
    aw_err_t (*pfn_trigger_off) (void *p_cookie, int id);
};

/**
 * \brief ��������֧�ֵ�������Ϣ������һ��ͨ�������ͣ�
 *
 * ����Ϣͨ�����豸����ȷ��������������ɶ��壬�����û�����ָ����Ϣ��
 *
 * һ���������豸��ID��С�������У�ID���䷽���ǣ��û�����ָ��һ����ʼ��ţ�ʵ��
 * ID��Χ��Ϊ�� start_id ~ (start_id + total_num - 1)�����ڸ÷�Χ�ڵ�ID����˳��
 * �����������ж���ġ�
 *
 * ���磬ĳ������֧��8·��ѹ�ɼ���2·�����ɼ����������� 8·��ѹͨ����Ȼ����2·
 * ����ͨ�� ����������Ϣ���Զ������£�
 *
 * // ��Ϣ���Ͷ���
 * const awbl_sensor_type_info_t info[2] = {
 *     {AW_SENSOR_TYPE_VOLTAGE, 8},        // ������8·������ѹͨ��
 *     {AW_SENSOR_TYPE_CURRENT, 2},        // Ȼ����2·��������ͨ��
 * };
 *
 * ��ID���е�˳���ǣ������� 2·��ѹͨ����Ȼ����2·����ͨ�����������6·��ѹͨ����
 * ��������Ϣ���Զ������£�
 *
 * // ��Ϣ���Ͷ���
 * const awbl_sensor_type_info_t info[3] = {
 *     {AW_SENSOR_TYPE_VOLTAGE, 2},        // ������2·������ѹͨ��
 *     {AW_SENSOR_TYPE_CURRENT, 2},        // Ȼ����2·��������ͨ��
 *     {AW_SENSOR_TYPE_VOLTAGE, 6},        // �����6·������ѹͨ��
 * };
 *
 * \attention ID����˳����������������ˣ�������Ϣ����������ɶ��壬�����û�����
 */
typedef struct awbl_sensor_type_info {
    int        type;              /**< \brief ���� */
    int        num;               /**< \brief �������������ͨ��Ϊ������ */
} awbl_sensor_type_info_t;

/**
 * \brief ��������������Ϣ
 */
typedef struct awbl_sensor_servinfo {

    /** \brief ֧�ֵĴ���������Ŀ */
    size_t                                 total_num;

    /** 
     * \brief ��ͨ����Ӧ�������б�
     *
     * ��������������ͨ�� ID �б��и���ͨ�������ͣ������ڼ���������ͨ��������ͬ
     * ��������� num ��ֵ��ʾ����������ͬ���͵�ͨ����
     * 
     * �б���������� num ֮�ͼ�Ӧ���봫����֧�ֵ�ͨ����Ŀ��total_num��һ�£���
     * ��÷�����Ч�����ᱻϵͳʹ�á�
     */
    const awbl_sensor_type_info_t         *p_type_info_list;
    
    /** \brief �б��С */
    size_t                                 list_size;

} awbl_sensor_servinfo_t;
 
/** \brief ���������� */
typedef struct awbl_sensor_service {
 
    /** \brief ���������Ϣ����ͨ����Ŀ��ͨ�����͵� */
    aw_const awbl_sensor_servinfo_t    *p_servinfo;
    
    /**
     * \brief ���������д�����ͨ������ʼ��ţ���ֵͨ�����û�ָ����
     *
     * ʵ��ռ�õ�ͨ����Ϊ�� start_id ~ (start_id + total_num - 1)
     *
     * ���磬�������豸֧��8��ͨ��������ʼ���Ϊ�� 10������ռ�õ�ͨ�����Ϊ��
     * 10 ~ 17������ȷ��ͨ�� id Ψһ�� ��Χ�������໥���渲�ǡ�
     */
    int                                start_id;
  
    /** \brief �������� */
    const struct awbl_sensor_servfuncs *p_servfuncs;
    
    /** \brief ������������ */
    void                               *p_cookie;
    
    /** \brief ����ָ����һ�����������񣬱������������ʽ��֯����������豸 */
    struct awbl_sensor_service         *p_next;

} awbl_sensor_service_t;
 
/** 
 * \brief �����������ʼ�����ڶ��׶γ�ʼ��֮�����
 * \return ��
 */
void awbl_sensor_init(void);

/** @} grp_awbl_if_sensor */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_SENSOR_H */

/* end of file */
