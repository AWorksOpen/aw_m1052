#include "apollo.h"
#include "aw_bitops.h"
#include "aw_int.h"
#include "aw_sem.h"

typedef struct __imx10xx_adc_etc_regs {
    uint32_t CTRL;
    uint32_t DONE0_1_IRQ;
    uint32_t DONE2_ERR_IRQ;
    uint32_t DMA_CTRL;
  struct {
        uint32_t TRIGn_CTRL;
        uint32_t TRIGn_COUNTER;
        uint32_t TRIGn_CHAIN_1_0;
        uint32_t TRIGn_CHAIN_3_2;
        uint32_t TRIGn_CHAIN_5_4;
        uint32_t TRIGn_CHAIN_7_6;
        uint32_t TRIGn_RESULT_1_0;
        uint32_t TRIGn_RESULT_3_2;
        uint32_t TRIGn_RESULT_5_4;
        uint32_t TRIGn_RESULT_7_6;
    } TRIG[8];
} __imx10xx_adc_etc_regs_t;

#define ADC_ETC_BASE_ADDR       0x403B0000

static __imx10xx_adc_etc_regs_t *__gp_adc_etc_reg = (__imx10xx_adc_etc_regs_t *)ADC_ETC_BASE_ADDR;

volatile uint32_t __g_adc_val0;
volatile uint32_t __g_adc_val1;

AW_SEMB_DECL(adc_val0_sem);
AW_SEMB_DECL(adc_val1_sem);

void adc_etc_done0_isr (void)
{
    //����жϱ�־
    AW_REG_WRITE32(&__gp_adc_etc_reg->DONE0_1_IRQ, 1);

    //��ȡval0ֵ
    __g_adc_val0 = 0xFFF & AW_REG_READ32(&__gp_adc_etc_reg->TRIG[0].TRIGn_RESULT_1_0);

    AW_SEMB_GIVE(adc_val0_sem);
}

void adc_etc_done1_isr (void)
{
    //����жϱ�־
    AW_REG_WRITE32(&__gp_adc_etc_reg->DONE0_1_IRQ, 0x10000);

    //��ȡval1ֵ
    __g_adc_val1 = AW_REG_READ32(&__gp_adc_etc_reg->TRIG[0].TRIGn_RESULT_1_0) >> 16;

    AW_SEMB_GIVE(adc_val1_sem);
}

void ADC_ETC_Configuration (void)
{
    uint32_t tmp;

    //��ʼ���������ź������¼��������ͷţ���ӡADC�������ݵ�ʱ���ȡ
    AW_SEMB_INIT(adc_val0_sem, 0, AW_SEM_Q_PRIORITY);
    AW_SEMB_INIT(adc_val1_sem, 0, AW_SEM_Q_PRIORITY);

    //�������λ
    AW_REG_BIT_CLR_MASK32(&__gp_adc_etc_reg->CTRL, 0x80000000);

    //ʹ��ADC1��ʹ���ⲿ����XBAR 0
    AW_REG_BIT_SET_MASK32(&__gp_adc_etc_reg->CTRL, 0x40000001);

    //����������Ϊ2
    AW_REG_WRITE32(&__gp_adc_etc_reg->TRIG[0].TRIGn_CTRL, 0x100);

    //�����ĳ�ʼʱ�Ӻͼ��ʱ�Ӷ���ֵΪ0
    AW_REG_WRITE32(&__gp_adc_etc_reg->TRIG[0].TRIGn_COUNTER, 0);

    //���õ�0�����
    tmp = AW_REG_READ32(&__gp_adc_etc_reg->TRIG[0].TRIGn_CHAIN_1_0);
    tmp &= ~0x7FFF;

    //ADCͨ��15��ʹ�õ�ADC1����Ӧ��������UART8_TX��ͬʱʹ����b2bģʽ��������ж�0
    tmp |= 0x301F;
    AW_REG_WRITE32(&__gp_adc_etc_reg->TRIG[0].TRIGn_CHAIN_1_0, tmp);

    //���õ�1�����
    tmp = AW_REG_READ32(&__gp_adc_etc_reg->TRIG[0].TRIGn_CHAIN_1_0);
    tmp &= ~0x7FFF0000;

    //ADCͨ��0��ʹ�õ�ADC1����Ӧ��������UART8_RX��ͬʱҲʹ����b2bģʽ��������ж�1
    tmp |= (0x5020 << 16);
    AW_REG_WRITE32(&__gp_adc_etc_reg->TRIG[0].TRIGn_CHAIN_1_0, tmp);

    //��ADC_ETC�жϣ�imx1050_inum.h��û��дADC_ETC�ı�ţ�����ֱ��ʹ�����֡�
    //118��ADC_ETC��IRQ0
    aw_int_connect(118, (aw_pfuncvoid_t)adc_etc_done0_isr, NULL);
    aw_int_enable(118);

    //119��ADC_ETC��IRQ1
    aw_int_connect(119, (aw_pfuncvoid_t)adc_etc_done1_isr, NULL);
    aw_int_enable(119);
}

