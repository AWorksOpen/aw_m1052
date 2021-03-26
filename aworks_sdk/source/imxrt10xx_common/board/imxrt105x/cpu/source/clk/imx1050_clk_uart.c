#include "clk_provider.h"
#include "imx1050_clk.h"
#include "simple_mux_clk.h"
#include "simple_div_clk.h"
#include "imx1050_ccm_regs.h"
#include "imx1050_reg_base.h"

aw_const static struct simple_mux_desc uart_clk_sel_desc = {
        {
                IMX1050_CLK_PLL3_DIV_6,
                IMX1050_CLK_OSC_24M
        },
        2,
        (void *)&CCM->CSCDR1,
        6,
        1
};

SIPMLE_MUX_CLK_DEFS(uart_clk_sel,uart_clk_sel_desc);

struct clk * imx1050_clk_register_uart_clk_sel(
        struct clk *p_uart_clk_sel,
        aw_clk_id_t clk_id,
        const char *name)
{
    return imx_mux_clk_register(
            p_uart_clk_sel,
            clk_id,
            name,
            &uart_clk_sel_ops);
}

aw_const static struct simple_div_clk_desc uart_podf_desc = {
        &CCM->CSCDR1,
        0,
        6,
        1,
        64,
        1,  //设置值要减一
};

SIPMLE_DIV_CLK_DEFS(uart_podf,uart_podf_desc)

struct clk * imx1050_clk_register_uart_podf(
        struct clk *p_uart_podf,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id)
{
    return imx_clk_register(
            p_uart_podf,
            clk_id,
            name,
            parent_id,
            &uart_podf_ops);
}

