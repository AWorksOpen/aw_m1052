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
    //清除中断标志
    AW_REG_WRITE32(&__gp_adc_etc_reg->DONE0_1_IRQ, 1);

    //读取val0值
    __g_adc_val0 = 0xFFF & AW_REG_READ32(&__gp_adc_etc_reg->TRIG[0].TRIGn_RESULT_1_0);

    AW_SEMB_GIVE(adc_val0_sem);
}

void adc_etc_done1_isr (void)
{
    //清除中断标志
    AW_REG_WRITE32(&__gp_adc_etc_reg->DONE0_1_IRQ, 0x10000);

    //读取val1值
    __g_adc_val1 = AW_REG_READ32(&__gp_adc_etc_reg->TRIG[0].TRIGn_RESULT_1_0) >> 16;

    AW_SEMB_GIVE(adc_val1_sem);
}

void ADC_ETC_Configuration (void)
{
    uint32_t tmp;

    //初始化这两个信号量，事件触发后释放，打印ADC采样数据的时候获取
    AW_SEMB_INIT(adc_val0_sem, 0, AW_SEM_Q_PRIORITY);
    AW_SEMB_INIT(adc_val1_sem, 0, AW_SEM_Q_PRIORITY);

    //先软件复位
    AW_REG_BIT_CLR_MASK32(&__gp_adc_etc_reg->CTRL, 0x80000000);

    //使用ADC1，使能外部触发XBAR 0
    AW_REG_BIT_SET_MASK32(&__gp_adc_etc_reg->CTRL, 0x40000001);

    //触发链长度为2
    AW_REG_WRITE32(&__gp_adc_etc_reg->TRIG[0].TRIGn_CTRL, 0x100);

    //触发的初始时延和间隔时延都赋值为0
    AW_REG_WRITE32(&__gp_adc_etc_reg->TRIG[0].TRIGn_COUNTER, 0);

    //配置第0这个链
    tmp = AW_REG_READ32(&__gp_adc_etc_reg->TRIG[0].TRIGn_CHAIN_1_0);
    tmp &= ~0x7FFF;

    //ADC通道15，使用的ADC1，对应开发板上UART8_TX，同时使能了b2b模式，打开完成中断0
    tmp |= 0x301F;
    AW_REG_WRITE32(&__gp_adc_etc_reg->TRIG[0].TRIGn_CHAIN_1_0, tmp);

    //配置第1这个链
    tmp = AW_REG_READ32(&__gp_adc_etc_reg->TRIG[0].TRIGn_CHAIN_1_0);
    tmp &= ~0x7FFF0000;

    //ADC通道0，使用的ADC1，对应开发板上UART8_RX，同时也使能了b2b模式，打开完成中断1
    tmp |= (0x5020 << 16);
    AW_REG_WRITE32(&__gp_adc_etc_reg->TRIG[0].TRIGn_CHAIN_1_0, tmp);

    //打开ADC_ETC中断，imx1050_inum.h中没有写ADC_ETC的标号，这里直接使用数字。
    //118是ADC_ETC的IRQ0
    aw_int_connect(118, (aw_pfuncvoid_t)adc_etc_done0_isr, NULL);
    aw_int_enable(118);

    //119是ADC_ETC的IRQ1
    aw_int_connect(119, (aw_pfuncvoid_t)adc_etc_done1_isr, NULL);
    aw_int_enable(119);
}

