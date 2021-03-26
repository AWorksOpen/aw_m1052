#include "apollo.h"
#include "aw_bitops.h"
#include "aw_cpu_clk.h"
#include "aw_clk.h"

typedef struct __imx10xx_pit_regs {
    uint32_t MCR;
    uint8_t RESERVED_0[220];
    uint32_t LTMR64H;
    uint32_t LTMR64L;
    uint8_t RESERVED_1[24];
    struct {
        uint32_t LDVAL;
        uint32_t CVAL;
        uint32_t TCTRL;
        uint32_t TFLG;
    } CHANNEL[4];
} imx10xx_pit_regs_t;

#define PIT_BASE_ADDR       0x40084000

#define PIT_OVFL_COUNT		24000000

static imx10xx_pit_regs_t *__gp_pit_reg = (imx10xx_pit_regs_t *)PIT_BASE_ADDR;

void PIT_Configuration(void)
{
    uint32_t i = 0;

    aw_clk_enable(IMX1050_CLK_CG_PIT);

    //enable PIT timers
    AW_REG_BIT_CLR_MASK32(&__gp_pit_reg->MCR, 0x2);

    for (i = 0; i < 4; i++) {
        AW_REG_BIT_CLR_MASK32(&__gp_pit_reg->CHANNEL[i], 0x1);
    }

    AW_REG_BIT_CLR_MASK32(&__gp_pit_reg->CHANNEL[0], 0x1);

    AW_REG_BIT_SET_MASK32(&__gp_pit_reg->MCR, 0x1);

    AW_REG_WRITE32(&__gp_pit_reg->CHANNEL[0].LDVAL, PIT_OVFL_COUNT);
}

void PIT_StartTimer(void)
{
    AW_REG_BIT_SET_MASK32(&__gp_pit_reg->CHANNEL[0].TCTRL, 0x1);
}
