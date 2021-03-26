#include "clk_provider.h"
#include "imx1020_clk.h"
#include "aw_bitops.h"

struct clk * imx_clk_register(
        struct clk *p_clk,
        aw_clk_id_t clk_id,
        const char *name,
        aw_clk_id_t parent_id,
        aw_const struct clk_ops *p_ops)
{
    struct clk_init_data        init = {0};
    struct clk                 *hw;

    hw = __clk_find_by_id(parent_id);
    init.name = name;
    init.clk_id = clk_id;
    init.parent_name = hw->name;
    init.flags = 0;
    init.ops = p_ops;

    hw = clk_register_hw(p_clk,&init);
    return hw;
}


struct clk * imx_mux_clk_register(
        struct clk *p_clk,
        aw_clk_id_t clk_id,
        const char *name,
        aw_const struct clk_ops *p_ops)
{
    struct clk_init_data        init = {0};
    struct clk                 *hw;

    init.name = name;
    init.clk_id = clk_id;
    init.parent_name = NULL;
    init.flags = 0;
    init.ops = p_ops;

    hw = clk_register_hw(p_clk,&init);
    return hw;
}
