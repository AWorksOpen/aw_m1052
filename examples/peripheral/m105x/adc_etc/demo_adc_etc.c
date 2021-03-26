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
 * \brief 本例程是通过PIT定时器的溢出事件来触发ADC_ETC，当定时时间到达时，触发一次ADC采样，
 *          把采样的数据通过串口打印出来。
 *
 *          使用本例程可以直接把文件夹下所有的.c文件拉到user_code文件夹中编译使用，
 *          在main函数中添加pit_trig_adc_etc_demo()即可，若想改变触发时间，
 *          可以在aw_rt105x_pit.c中修改PIT_OVFL_COUNT这个宏，默认是1秒钟一次触发事件。
 *          触发的通道分别选择ADC1的通道0和通道15，对应开发板上串口8的TX和RX
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
