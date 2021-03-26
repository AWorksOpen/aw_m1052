/*
 * awbl_adc_key.h
 *
 *  Created on: 2020��9��28��
 *      Author: zengqingyuhang
 */

#ifndef _AWBL_ADC_KEY_H_
#define _CODE_AWBL_ADC_KEY_H_


#include "aw_common.h"
#include "awbus_lite.h"
#include "awbl_input.h"
#include "aw_types.h"

#define ADC_KEY_DRV_NAME            "adc_key_drv"


struct  adc_key_cfg {

    uint32_t        voltage_mv;                 /* ��������ȥʱ�ĵ�ѹֵ */
    int             key_code;                   /* �������� */
    char           *key_name;                   /* �������� */
    aw_bool_t       continuous;                 /* �Ƿ�֧������ 1��֧�֣�0����֧�� */
    aw_bool_t       pressed;                    /* ������ʼ״̬ */
    uint32_t        cnt;                        /* ������ʼ����ֵ�������ж�����ģʽ�������� */
};


struct awbl_adc_key_devinfo {

    uint32_t            adc_chan;               /* ad����ʹ�õ�ת��ͨ���� */
    struct adc_key_cfg *key_cfg_tab;            /* ��ѹֵ�밴���Ķ��ձ� */
    uint32_t            key_nums;               /* �������� */
    uint32_t            scan_period_ms;         /* ����ɨ������ */
    uint32_t            continue_cnt;           /* ��������ģʽ��ֵ */
    float               offset;                 /* ��ѹֵ���¸�����Χ����������Ϊ0.1��
                                                                                                            ������0.1��Χ�ĵ�ѹֵ������Ч�İ��������¼� */
    aw_bool_t           printf_org_val;         /* �Ƿ��ӡadת����ԭʼֵ */
};


struct awbl_adc_key_dev {

    struct awbl_dev     pdev;
    uint32_t            key_status;
};


void awbl_adc_key_drv_register (void);



#endif /* _AWBL_ADC_KEY_H_ */
