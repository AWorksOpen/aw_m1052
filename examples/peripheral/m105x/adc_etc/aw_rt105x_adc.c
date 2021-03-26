#include "apollo.h"
#include "aw_clk.h"
#include "aw_cpu_clk.h"

typedef struct __imx10xx_adc_regs {

    uint32_t        HC[8];
    const uint32_t  HS;
    const uint32_t  R[8];
    uint32_t        CFG;
    uint32_t        GC;
    uint32_t        GS;
    uint32_t        CV;
    uint32_t        OFS;
    uint32_t        CAL;

} imx10xx_adc_regs_t;

#define ADC_BASE_ADDR 0x400C4000

static imx10xx_adc_regs_t *__gp_adc_reg = (imx10xx_adc_regs_t *)ADC_BASE_ADDR;

int ADC_Calibration (void)
{
    int         ret  = 0;
    aw_bool_t   trig = AW_FALSE;

    if (0 != (AW_REG_READ32(&__gp_adc_reg->CFG) & 0x2000)) {
        trig = AW_TRUE;
        AW_REG_BIT_CLR_MASK32(&__gp_adc_reg->CFG, 0x2000);
    }

    AW_REG_WRITE32(&__gp_adc_reg->GS, 0x2);
    AW_REG_BIT_SET_MASK32(&__gp_adc_reg->GC, 0x80);

    while (0 != (AW_REG_READ32(&__gp_adc_reg->GC) & 0x80)) {
        if (0 != (AW_REG_READ32(&__gp_adc_reg->GS) & 0x2)) {
            ret = -AW_EINVAL;
            break;
        }
    }

    if (0 == (AW_REG_READ32(&__gp_adc_reg->HS) & (1))) {
        ret = -AW_EINVAL;
    }

    if (0 != (AW_REG_READ32(&__gp_adc_reg->GS) & 0x2)) {
        ret = -AW_EINVAL;
    }

    AW_REG_READ32(&__gp_adc_reg->R[0]);

    if (trig == AW_TRUE) {
        AW_REG_BIT_SET_MASK32(&__gp_adc_reg->CFG, 0x2000);
    }

    ret = AW_OK;

    return ret;
}

void ADC_Configuration (void)
{
    //adc clock enable
    aw_clk_enable(IMX1050_CLK_CG_ADC1);

    uint32_t reg = 0;

    reg = AW_REG_READ32(&__gp_adc_reg->CFG) & (0xC000 | 0x2000);

    //打开Async时钟，使用12-bit模式，选择使用硬件触发
    reg |= 0x200B;

    AW_REG_WRITE32(&__gp_adc_reg->CFG, reg);

    //Async时钟使能，使用单次转换模式
    reg = AW_REG_READ32(&__gp_adc_reg->GC) & ~(0x41);
    reg |= 1;

    AW_REG_WRITE32(&__gp_adc_reg->GC, reg);

    AW_REG_BIT_SET_MASK32(&__gp_adc_reg->CFG, 0x2000);

    //选择使用ADC_ETC
    AW_REG_WRITE32(&__gp_adc_reg->HC[0], 0x10);
    AW_REG_WRITE32(&__gp_adc_reg->HC[1], 0x10);

    int i;

    for (i = 0; i < 10; i++) {

        //这个是ADC的校准功能，使用前应该校准一下。
        if (AW_OK == ADC_Calibration()) {
            break;
        }
    }

}
