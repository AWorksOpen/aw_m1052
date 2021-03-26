#include "apollo.h"
#include "aw_cpu_clk.h"
#include "aw_clk.h"

typedef struct __imx10xx_xbara_regs {
    uint16_t SEL0;
    uint16_t SEL1;
    uint16_t SEL2;
    uint16_t SEL3;
    uint16_t SEL4;
    uint16_t SEL5;
    uint16_t SEL6;
    uint16_t SEL7;
    uint16_t SEL8;
    uint16_t SEL9;
    uint16_t SEL10;
    uint16_t SEL11;
    uint16_t SEL12;
    uint16_t SEL13;
    uint16_t SEL14;
    uint16_t SEL15;
    uint16_t SEL16;
    uint16_t SEL17;
    uint16_t SEL18;
    uint16_t SEL19;
    uint16_t SEL20;
    uint16_t SEL21;
    uint16_t SEL22;
    uint16_t SEL23;
    uint16_t SEL24;
    uint16_t SEL25;
    uint16_t SEL26;
    uint16_t SEL27;
    uint16_t SEL28;
    uint16_t SEL29;
    uint16_t SEL30;
    uint16_t SEL31;
    uint16_t SEL32;
    uint16_t SEL33;
    uint16_t SEL34;
    uint16_t SEL35;
    uint16_t SEL36;
    uint16_t SEL37;
    uint16_t SEL38;
    uint16_t SEL39;
    uint16_t SEL40;
    uint16_t SEL41;
    uint16_t SEL42;
    uint16_t SEL43;
    uint16_t SEL44;
    uint16_t SEL45;
    uint16_t SEL46;
    uint16_t SEL47;
    uint16_t SEL48;
    uint16_t SEL49;
    uint16_t SEL50;
    uint16_t SEL51;
    uint16_t SEL52;
    uint16_t SEL53;
    uint16_t SEL54;
    uint16_t SEL55;
    uint16_t SEL56;
    uint16_t SEL57;
    uint16_t SEL58;
    uint16_t SEL59;
    uint16_t SEL60;
    uint16_t SEL61;
    uint16_t SEL62;
    uint16_t SEL63;
    uint16_t SEL64;
    uint16_t SEL65;
    uint16_t CTRL0;
    uint16_t CTRL1;
} imx10xx_xbara_regs_t;

#define XBARA_BASE_ADDR         0x403BC000

static imx10xx_xbara_regs_t *__gp_xbar_reg = (imx10xx_xbara_regs_t *)XBARA_BASE_ADDR;

void XBARA_Configuration (void)
{
    aw_clk_enable(IMX1050_CLK_CG_XBAR1);

    //PIT_TRIGGER0--->XBAR1_IN56, XBAR1_OUT103--->ADC_ETC_TRIG00
    //根据上面的对应关系，把SEL103对应位赋值为56就建立好了PIT和ADC_ETC的连接关系
    *(volatile uint16_t *)&__gp_xbar_reg->SEL51 = (56 << 8);
}
