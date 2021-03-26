#include "apollo.h"
#include "aw_sem.h"
#include "aw_vdebug.h"

extern uint32_t __g_adc_val0;
extern uint32_t __g_adc_val1;

AW_SEMB_IMPORT(adc_val0_sem);
AW_SEMB_IMPORT(adc_val1_sem);

extern void ADC_Configuration(void);
extern void XBARA_Configuration(void);
extern void PIT_Configuration(void);
extern void ADC_ETC_Configuration(void);
extern void PIT_StartTimer(void);

/**
 * \brief ��������ͨ��PIT��ʱ��������¼�������ADC_ETC������ʱʱ�䵽��ʱ������һ��ADC������
 *          �Ѳ���������ͨ�����ڴ�ӡ������
 *
 *          ʹ�ñ����̿���ֱ�Ӱ��ļ��������е�.c�ļ�����user_code�ļ����б���ʹ�ã�
 *          ��main���������pit_trig_adc_etc_demo()���ɣ�����ı䴥��ʱ�䣬
 *          ������aw_rt105x_pit.c���޸�PIT_OVFL_COUNT����꣬Ĭ����1����һ�δ����¼���
 *          ������ͨ���ֱ�ѡ��ADC1��ͨ��0��ͨ��15����Ӧ�������ϴ���8��TX��RX
 */


void pit_trig_adc_etc_demo (void)
{
    ADC_Configuration();
    XBARA_Configuration();
    PIT_Configuration();
    ADC_ETC_Configuration();

    PIT_StartTimer();

    while (1) {
        if (AW_SEMB_TAKE(adc_val0_sem, AW_WAIT_FOREVER) == AW_OK) {
            aw_kprintf("adc val 0 is %d\n", __g_adc_val0);
        }

        if (AW_SEMB_TAKE(adc_val1_sem, AW_WAIT_FOREVER) == AW_OK) {
            aw_kprintf("adc val 1 is %d\n", __g_adc_val1);
        }
    }
}
