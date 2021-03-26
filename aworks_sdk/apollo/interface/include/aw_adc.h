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
 * \brief ͨ��ADC�ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
 * \code
 * #include aw_adc.h
 * \endcode
 *
 * \par ����1(��ͬ����ȡ)
 * \code
 * #include "aw_adc.h"
 *
 * #define N_SAMPLES    10        //�����������Ϊ10��
 * #define  CHANNEL      2        //ת��ͨ��Ϊͨ��2
 *
 * int main()
 * {
 *     uint16_t  adc_val[N_SAMPLES]; // �ٶ�����λ��Ϊ12λ������������Ϊuint16_t
 *     aw_err_t  ret;
 *     int       i;
 *
 *     //ͬ����ȡ��ת�������󷵻�
 *     ret = aw_adc_sync_read(CHANNEL, adc_val, N_SAMPLES, AW_FALSE);
 *
 *     if (ret != AW_OK) {
 *         aw_kprintf("ת������\r\n");
 *     } else {
 *         aw_kprintf("ת�����\r\n");
 *         //��ӡ����ֵ
 *         for (i = 0; i < N_SAMPLES; i++) {
 *         aw_kprintf("%d ", p_val[i]);
 *         }
 *     }
 *
 *     //��ӡ����ֵ
 *     for (i = 0; i < N_SAMPLES; i++) {
 *         aw_kprintf("%d ", p_val[i]);
 *     }
 * }
 * \endcode
 *
 * \par ����2(�ͻ��˶�ȡ)
 * \code
 * #include "aw_adc.h"
 *
 * void adc_callback (void *p_arg, int state)
 * {
 *      int num = (int)p_arg;
 *
 *      if (state != AW_OK ) {
 *          //��������ת��ʧ�ܣ��������л��Զ�ȡ���������ٴ�ת��������������
 *          //���������жϼ��ص�������ʱ��Ӧ�þ����̣ܶ��ٴ˿���ʹ���ź�����
 *          //��Ϣ���䴦��ת��ʧ�ܵĹ��������������
 *      } else {
 *          //����ת���ɹ�����������������
 *          //���������жϼ��ص�������ʱ��Ӧ�þ����̣ܶ��ٴ˿���ʹ���ź�����
 *          //��Ϣ���䴦�����ݵĹ��������������
 *      }
 * }
 *
 * int main()
 * {
 *     void                 adc_val1[100];
 *     uint16_t             adc_val2[100]
 *     aw_adc_buf_desc_t    desc[2];
 *     struct aw_adc_client client;
 *
 *     //����������������0���ص���������Ϊ0,0�Ż�����ת�����
 *     aw_adc_mkbufdesc(&desc[0],hs_val1,cnt,adc_callback,(void *)0);
 *
 *     //����������������1���ص���������Ϊ1,1�Ż�����ת�����
 *     aw_adc_mkbufdesc(&desc[1],hs_val2,cnt,adc_callback,(void *)1);
 *
 *     aw_adc_client_init(&client, IMX28_HSADC_LRADCPIN0, AW_FALSE);
 *
 *     //�����ͻ��ˣ�2��������ѭ������������ݣ�0������ʾ����ѭ�����
 *     //�����������أ���Ӧ����������������������Ӧ�Ļص�����
 *     aw_adc_client_start(&client, // �������Ŀͻ���
 *                          desc,   // ����������
 *                          2,      // �������ĸ���
 *                          5);     // ��������ѭ�����5�Σ�0���ʾ����ѭ�����
 *
 *     while(1) {
 *     }
 * }
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 2.01 15-06-10 tee, add and modify some interface
 * - 2.00 14-09-25 ehf, modify some interface and implemention to support
 *                 read mulitiple samples once
 * - 1.00 12-11-01 zyr, first implementation
 * \endinternal
 */

#ifndef __AW_ADC_H
#define __AW_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_adc
 * \copydoc aw_adc.h
 * @{
 */

#include "apollo.h"
#include "aw_psp_adc.h"
#include "aw_list.h"

/**
 * \brief ADCͨ�����Ͷ���
 */
typedef aw_psp_adc_channel_t  aw_adc_channel_t;

/**
 * \brief ADCֵ���Ͷ���
 */
typedef aw_psp_adc_val_t      aw_adc_val_t;

/**
 * \brief   ת����ɻص���������
 *
 * \param[in]  p_cookie    �û���ʼ���ͻ��˴���Ļص���������
 * \param[in]  state       ת����ɵ�״̬��
 *                             AW_OK        : ת���ɹ�
 *                             AW_ERROR     : ת������
 *
 * \note ����ת������ǰӦ��ȷ��ת��������״̬ΪAW_OK�������ΪAW_OK�������ת��
 *       ���н��Զ�ȡ����������ת����ͨ������£�ת��������������ڻ�������buf��
 *       С��ʹ��buf�л���Ҫ��Ƶ�������ݴ���ʱ���Գ������ݾ�����ˡ��������󻺳�
 *       ����buf��С������buf�л���ʱ������
 */
typedef void (*pfn_adc_complete_t) (void *p_cookie, aw_err_t state);

/**
 * \brief ADC����������
 */
typedef struct aw_adc_buf_desc {

    /**
     * \brief ���ڴ��ADCת����������ݻ�����
     *
     * ����������ʵ��ADCת��λ����أ���Ҫȷ���ֳ�Ϊ2���ݴα�8-bit��
     * ������ADCת��λ������ͨ��aw_adc_bits_get()��ȡ���ͻ��������͵Ĺ�ϵ���£�
     *
     *      ADC bits    |    buffer type
     *    ------------- | -------------------
     *       1  ~  8    |  uint8_t/int8_t
     *       9  ~  16   |  uint16_t/int16_t
     *      17  ~  31   |  uint32_t/int32_t
     */
    void               *p_buf;

    /** \brief ���ݻ������ĳ��� */
    uint32_t            length;

    /** \brief �û����������������Ļص����� */
    pfn_adc_complete_t  pfn_complete;

    /** \brief �ص������Ĳ��� */
    void               *p_arg;
} aw_adc_buf_desc_t;


/**
 * \brief ADC�ͻ��˽ṹ(�벻Ҫֱ��ʹ�øýṹ��ĳ�Ա����)
 *
 * �ýṹ���ھ�̬�����ڴ棬�û���������������ֱ��ʹ�����Ա����
 */
typedef struct aw_adc_client {

    struct aw_list_head      node;        /**< \brief �ͻ��˷�������ڵ�     */
    struct awbl_adc_service *p_serv;      /**< \brief ��ͻ��˹���ADC����    */

    aw_adc_channel_t         channel;     /**< \brief ��ÿͻ��˹�����ͨ���� */
    aw_adc_buf_desc_t       *p_desc;      /**< \brief ����������             */
    uint32_t                 desc_num;    /**< \brief ����������             */
    uint32_t                 count;       /**< \brief ����ת����ɵĴ���     */
    int                      stat;        /**< \brief �ͻ���״̬             */
    aw_bool_t                urgent;      /**< \brief ������                 */
    uint8_t                  data_bits;   /**< \brief ���������ݵ�λ��       */

} aw_adc_client_t;

/**
 * \brief ��ȡһ��ADCͨ����λ��
 *
 * \param[in] ch  ADCͨ����
 *
 * \return    >0      ��ADCͨ������Чλ��
 * \retval -AW_ENXIO  ADCͨ���Ų�����
 */
int aw_adc_bits_get(aw_adc_channel_t ch);

/**
 * \brief ��ȡһ��ADͨ���Ĳο���ѹ����λ��mV��
 *
 * \param[in] ch  ADCͨ����
 *
 * \return   >0       ��ADCͨ���ο���ѹ(mV)
 * \retval -AW_ENXIO  ADCͨ���Ų�����
 */
int aw_adc_vref_get(aw_adc_channel_t ch);

/**
 * \brief ��ȡADC��ǰ�Ĳ�����(Samples Per Sencond,ÿ���Ӳ����ĸ���)
 *
 * \param[in]  ch      ADCͨ����
 * \param[out] p_rate  ���ڶ�ȡ��ǰ�Ĳ�����
 *
 * \retval   AW_OK     ��ȡ�ɹ�
 * \retval -AW_ENXIO   ADCͨ���Ų�����
 */
aw_err_t aw_adc_rate_get(aw_adc_channel_t ch, uint32_t *p_rate);

/**
 * \brief ����ADC�Ĳ�����(Samples Per Sencond,ÿ���Ӳ����ĸ���)
 *
 * \param[in]  ch     ADCͨ����
 * \param[in]  rate   ���õĲ�����
 *
 * \retval   AW_OK    ���óɹ�
 * \retval -AW_ENXIO  ADCͨ���Ų�����
 *
 * \note ʵ�ʵĲ����ʿ�����˴����õĲ����ʴ���һ�����
 */
aw_err_t aw_adc_rate_set(aw_adc_channel_t ch, uint32_t rate);

/**
 * \brief ��һ��ͨ���Ĳ���ֵֵת��Ϊ��ѹֵ����λ��mV��
 *
 * \param[in]  ch     ADCͨ����
 * \param[in]  cnt    ADC����ֵ�ĸ���
 * \param[out] p_val  ���ADC����ֵ
 * \param[out] p_mv   ���ת������ĵ�ѹֵ(mV)
 *
 * \return   >0   ��ADCͨ������Чλ��
 * \retval -ENXIO ADCͨ���Ų�����
 */
aw_err_t aw_adc_val_to_mv(aw_adc_channel_t  ch,
                          void             *p_val,
                          uint32_t          cnt,
                          uint32_t         *p_mv);


/**
 * \brief ����ADC���ݻ���������������ز���
 *
 * \param[in]  p_desc        ADC���ݻ�����������ָ��
 * \param[in]  length        ���ݻ���������
 * \param[in]  pfn_complete  �����������Ļص�����
 * \param[in]  p_arg         �ص���������
 * \param[out] p_buf         ���ڴ��ADCת������Ļ�����,ע���������ͣ����ݿ��
 *                           Ϊ2���ݴα�8-bit,ȡ����ADCλ������Сֵ
 *
 * \return ��
 */
aw_local aw_inline void aw_adc_mkbufdesc (aw_adc_buf_desc_t     *p_desc,
                                          void                  *p_buf,
                                          uint32_t               length,
                                          pfn_adc_complete_t     pfn_complete,
                                          void                  *p_arg)
{
    p_desc->p_buf        = p_buf;
    p_desc->length       = length;
    p_desc->pfn_complete = pfn_complete;
    p_desc->p_arg        = p_arg;
}

/**
 * \brief ͬ����ȡһ��ADCͨ����������ֵ
 *
 * ʹ�øýӿڿ��Է���Ļ�ȡһ��ADͨ����һ����������ֵ,
 * ��������ӵĿ��ƿ�ʹ��ADC�ͻ���
 *
 * \param[in]  ch      ADͨ����
 * \param[out] p_val   ���ڴ�Ų���ֵ�������ַ������������ADC�����ݿ�����
 * \param[in]  samples ��Ҫ�����ĸ���
 * \param[in]  urgent  ����ת�����񣬽�������urgentΪFALSE��ת������ת��
 *
 * \return >=0       ��ADͨ����������ֵ
 * \retval -AW_ENXIO ADͨ���Ų�����
 *
 *
 * \note ����p_valֵ������
 *
 * ����������ʵ��ADCת��λ����أ���Ҫȷ���ֳ�Ϊ2���ݴα�8-bit��
 * ������ADCת��λ������ͨ��aw_adc_bits_get()��ȡ���ͻ��������͵Ĺ�ϵ���£�
 *
 *      ADC bits    |    buffer type
 *    ------------- | -------------------
 *       1  ~  8    |  uint8_t/int8_t
 *       9  ~  16   |  uint16_t/int16_t
 *      17  ~  31   |  uint32_t/int32_t
 */
aw_err_t aw_adc_sync_read(aw_adc_channel_t  ch,
                          void             *p_val,
                          uint32_t          samples,
                          aw_bool_t         urgent);

/**
 * \brief ��ʼ��ADC�ͻ���
 *
 * \param[in] p_client ���û��ṩ��ADC�ͻ����ڴ�ռ�
 * \param[in] ch       ��ADC�ͻ��˶�Ӧ��ת��ͨ��
 * \param[in] urgent   �ͻ����Ƿ������ת���������ͻ�����������ͨ�ͻ�����ת����
 *
 * \retval  AW_OK     ��ʼ���ɹ�
 * \retval -AW_EINVAL �������󣬼������ĺϷ���
 * \retval -AW_ENXIO  ͨ��Ϊ��Чͨ��
 *
 * \note ����ת�������ԣ�����ת���ͻ�����������ͨת���ͻ��ˣ������ǰ������ͨ��
 *       ���ˣ�����ڵ�ǰ��ͨ�ͻ��˵�һ�����н�����ǿ�ƴ����ͨ�ͻ��˵�ת����
 *       �л��������ͻ��ˡ�
 */
aw_err_t aw_adc_client_init(aw_adc_client_t    *p_client,
                            aw_adc_channel_t    ch,
                            aw_bool_t           urgent);

/**
 * \brief ����ADC�ͻ��˿�ʼת��
 *
 * �����ͻ���ת��ǰ��ȷ��ADC�ͻ����ѳ�ʼ������ʼ��һ�κ�������޸Ŀͻ��˶�Ӧ��
 * ͨ���ź�ת�������ԣ��������������Բ��ٳ�ʼ����
 *
 * \param[in] p_client  ��ʹ�õ�ADC�ͻ���
 * \param[in] p_desc    ����������,�����ǵ��������������������ĵ�ַ����������
 *                      ������������ĵ�ַ��
 * \param[in] desc_num  ָ�������������ĸ���
 * \param[in] count     ���е�ת��������һ��ת�����б�ʾdesc_numָ�������ĵĻ�
 *                      ��������������������ָ��Ϊ0�����������ϵ�ת������һ
 *                      �����н������Զ�����������һ������
 *
 * \retval  AW_OK       �����ɹ�
 * \retval -AW_EINVAL   ��������
 * \retval -AW_EALREADY �ͻ����Ѿ���ʼ���������¿�ʼ������ȡ���ͻ���ת��
 * \retval -AW_EPERM    �ͻ���δ��ʼ��
 *
 * \note  ���ڻ������������������������������˴��ADCת������Ļ������������ǵ�����
 *        ��������ָ���˶���������������ǰһ����������������Զ��������л���
 *        ��һ����������ָ������ת������Ϊ���ʱ����������ת��(0)������������������
 *        Ϊ2�����Ϊ����������Ϊ1�������ز������󣨵����������޷�ѭ����䣩��
 */
aw_err_t aw_adc_client_start(aw_adc_client_t    *p_client,
                             aw_adc_buf_desc_t  *p_desc,
                             int                 desc_num,
                             uint32_t            count);

/**
 * \brief ȡ��ADC�ͻ��˵�ת������
 *
 * �ýӿڻ�Ѷ�Ӧ�ͻ�������������б���ɾ��
 *
 * \param[in] p_client Ҫȡ����ADC�ͻ���
 *
 * \retval  AW_OK       �����ɹ�
 * \retval -AW_EINVAL   ��������
 * \retval -AW_EPERM    �ͻ���δ��ת�������У�ȡ������������
 */
aw_err_t aw_adc_client_cancel(aw_adc_client_t *p_client);


/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* __AW_ADC_H */

/* end of file */
