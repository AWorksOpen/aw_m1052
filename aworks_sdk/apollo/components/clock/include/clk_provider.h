/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

#ifndef         __CLK_PROVIDER_H__
#define         __CLK_PROVIDER_H__

#include "aw_clk.h"
#include "aw_spinlock.h"
#include "aw_list.h"

#ifndef CONFIG_CLK_MAX_PARENTS_NUM
#define CONFIG_CLK_MAX_PARENTS_NUM 8
#endif


struct clk;

/*
 * flags used across common struct clk.  these flags should only affect the
 * top-level framework.  custom flags for dealing with hardware specifics
 * belong in struct clk_foo
 */
 
 /** \brief must be gated across rate change */
#define CLK_SET_RATE_GATE       AW_BIT(0) 

/** \brief must be gated across re-parent */
#define CLK_SET_PARENT_GATE     AW_BIT(1) 

/** \brief propagate rate change up one level */
#define CLK_SET_RATE_PARENT     AW_BIT(2) 

/** \brief do not gate even if unused */
#define CLK_IGNORE_UNUSED       AW_BIT(3) 

/** \brief root clk, has no parent */
#define CLK_IS_ROOT             AW_BIT(4) 

/** \brief Basic clk, can't do a to_clk_foo() */
#define CLK_IS_BASIC            AW_BIT(5) 

/** \brief do not use the cached clk rate */
#define CLK_GET_RATE_NOCACHE    AW_BIT(6) 

/** \brief don't re-parent on rate change */
#define CLK_SET_RATE_NO_REPARENT AW_BIT(7) 

/** \brief do not use the cached clk accuracy */
#define CLK_GET_ACCURACY_NOCACHE AW_BIT(8) 
/*
 * \brief Basic mux clk, can't switch parent while there is another basic mux 
 * clk being its child.  Otherwise, a glitch might be propagated to downstream
 * clocks through this child mux.
 */
#define CLK_IS_BASIC_MUX    AW_BIT(9)
/*
 * \brief parent clock must be on across any operation including
 * clock gate/ungate, rate change and re-parent
 */
#define CLK_SET_PARENT_ON   AW_BIT(10)

/*
 * \brief 时钟的频率和父时钟不能被改变，主要供内部使用，用于标记不能随便修改的时
 * 钟如DDR时钟及其所有父时钟，以阻止意外的修改
 */
#define CLK_CANNOT_CHANGE   AW_BIT(11)

typedef void (*pfn_clk_init_t)(struct clk *hw);
typedef aw_err_t (*pfn_clk_enable_t)(struct clk *hw);
typedef aw_err_t (*pfn_clk_disable_t)(struct clk *hw);
typedef aw_err_t (*pfn_clk_set_parent_t)(struct clk *hw,aw_clk_id_t parent);
typedef aw_err_t (*pfn_clk_get_parent_t)(struct clk *hw,aw_clk_id_t *parent);
typedef aw_err_t (*pfn_clk_set_rate_t)(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate);
typedef aw_clk_rate_t (*pfn_clk_recalc_rate_t)(
        struct clk *hw,
        aw_clk_rate_t parent_rate);
typedef aw_clk_rate_t (*pfn_clk_round_rate_t)(
                        struct clk *hw,
                        aw_clk_rate_t desired_rate,
                        aw_clk_rate_t *best_parent_rate);
typedef aw_clk_rate_t (*pfn_clk_determine_rate_t)(
                        struct clk *hw,
                         aw_clk_rate_t desired_rate,
                         aw_clk_rate_t *best_parent_rate,
                         aw_clk_id_t *best_parent_clk);
typedef int (*pfn_clk_save_context_t)(struct clk *hw,uint8_t *p_buf,int size);
typedef int (*pfn_clk_restore_context_t)(struct clk *hw,uint8_t *p_buf,int size);

struct clk_ops {
    pfn_clk_init_t              init;
    pfn_clk_enable_t            enable;
    pfn_clk_disable_t           disable;
    pfn_clk_set_parent_t        set_parent;
    pfn_clk_get_parent_t        get_parent;
    pfn_clk_set_rate_t          set_rate;
    pfn_clk_recalc_rate_t       recalc_rate;
    pfn_clk_round_rate_t        round_rate;
    pfn_clk_determine_rate_t    determine_rate;
#ifdef CONFIG_CLK_ENABLE_SAVE_CONTEXT
    pfn_clk_save_context_t      save_context;
    pfn_clk_restore_context_t   restore_context;
#endif
};

struct clk {
    const char             *name;
    int                     ref_count;
    const struct clk_ops   *ops;
#ifndef CONFIG_CLK_NO_CACHE_ID
    aw_clk_id_t             clk_id;
#endif
    struct clk             *parent;
#ifndef CONFIG_CLK_NO_CACHE_RATE
    aw_clk_rate_t           rate;
#endif
    unsigned long           flags;

#ifndef CONFIG_CLK_NO_CACHE_CHILD

    struct aw_hlist_head    children;
    struct aw_hlist_node    child_node;
#endif

#ifndef CONFIG_CLK_NO_SET_RATE_RECURSIVE
    struct clk             *new_child;
    struct clk             *new_parent;
    aw_clk_rate_t           new_rate;
#endif
};

static inline unsigned long __clk_get_flags(struct clk *clk)
{
    return clk->flags;
}

static inline void __clk_set_flags(struct clk *clk,unsigned long flags)
{
    clk->flags |= flags;
}

static inline void __clk_clr_flags(struct clk *clk,unsigned long flags)
{
    clk ->flags &= (~flags);
}

aw_clk_rate_t __clk_round_rate(struct clk *clk, aw_clk_rate_t rate);

aw_clk_id_t __clk_get_id(struct clk *clk);
struct clk * __clk_find_by_name(const char *name);
struct clk * __clk_find_by_id(aw_clk_id_t clk_id);
struct clk * __clk_get_parent(struct clk *clk);
aw_clk_rate_t __clk_get_rate(struct clk *hw);

aw_err_t clk_enable(struct clk *hw);
aw_err_t clk_disable(struct clk *hw);

aw_clk_rate_t clk_get_rate(struct clk *clk);
aw_err_t clk_set_rate(struct clk *clk, aw_clk_rate_t rate);
aw_err_t clk_set_parent(struct clk *clk, struct clk *parent);

aw_err_t clk_save_context(uint8_t *p_buf,int size);
aw_err_t clk_restore_context(uint8_t *p_buf,int size);

/**
 * \brief struct clk_init_data - holds init data that's common to all clocks and 
 * is shared between the clock provider and the common clock framework.
 *
 * @name: clock name
 * @ops: operations this clock supports
 * @parent_names: array of string names for all possible parents
 * @num_parents: number of possible parents
 * @flags: framework-level hints and quirks
 */
struct clk_init_data {
    const char                 *name;
    const struct clk_ops       *ops;
    aw_clk_id_t                 clk_id;
    const char                 *parent_name;;
    unsigned long               flags;
};

/*
 * DOC: Basic clock implementations common to many platforms
 *
 * Each basic clock hardware type is comprised of a structure describing the
 * clock hardware, implementations of the relevant callbacks in struct clk_ops,
 * unique flags for that hardware type, a registration function and an
 * alternative macro for static initialization
 */

/**
 * \brief struct clk_fixed_rate - fixed-rate clock
 * @hw:     handle between common and hardware-specific interfaces
 * @fixed_rate: constant frequency of clock
 */
struct clk_fixed_rate {
    struct clk              hw;
#ifdef CONFIG_CLK_NO_CACHE_RATE
    aw_clk_rate_t           fixed_rate;
#endif      /* CONFIG_CLK_NO_CACHE_RATE */
};

struct clk *clk_register_fixed_rate(
        struct clk_fixed_rate *p_clk_fixed_rate,
        aw_clk_id_t clk_id,
        const char *name,
        unsigned long flags,
        unsigned long fixed_rate);


/**
 * \brief struct clk_user_fixed_rate - 用户固定频率时钟，
 * 实际上和固定频率时钟差不多，区别在于用户固定频率时钟可以设置频率
 * 用户固定频率时钟主要用于实现并不能确定时钟频率的外部时钟源输入，用户
 * 可以调用设置此时钟频率的接口来表明外部输入时钟频率
 * @hw:     handle between common and hardware-specific interfaces
 */
struct clk_user_fixed_rate {
    struct clk              hw;
#ifdef CONFIG_CLK_NO_CACHE_RATE
    aw_clk_rate_t           rate;
#endif
};

struct clk *clk_register_user_fixed_rate(
        struct clk_user_fixed_rate *p_clk_user_fixed_rate,
        aw_clk_id_t clk_id,
        const char *name,
        unsigned long flags);

/**
 * \brief struct clk_gate - gating clock
 *
 * @hw:     handle between common and hardware-specific interfaces
 * @reg:    register controlling gate
 * @bit_idx:    single bit controlling gate
 * @flags:  hardware-specific flags
 * @lock:   register lock
 *
 * Clock which can gate its output.  Implements .enable & .disable
 *
 * Flags:
 * CLK_GATE_SET_TO_DISABLE - by default this clock sets the bit at bit_idx to
 *  enable the clock.  Setting this flag does the opposite: setting the bit
 *  disable the clock and clearing it enables the clock
 * CLK_GATE_HIWORD_MASK - The gate settings are only in lower 16-bit
 *   of this register, and mask of gate bits are in higher 16-bit of this
 *   register.  While setting the gate bits, higher 16-bit should also be
 *   updated to indicate changing gate bits.
 */
struct clk_gate {
    struct clk      hw;
    void           *reg;
    uint8_t         bit_idx;
    uint8_t         flags;
    aw_spinlock_isr_t *lock;
};

#define CLK_GATE_SET_TO_DISABLE     AW_BIT(0)
#define CLK_GATE_HIWORD_MASK        AW_BIT(1)

struct clk *clk_register_gate(
        struct clk_gate *p_clk_gate,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        unsigned long flags,
        void *reg,
        uint8_t bit_idx,
        uint8_t clk_gate_flags,
        aw_spinlock_isr_t *lock);

struct clk_div_table {
    unsigned int    val;
    unsigned int    div;
};

/**
 * \brief struct clk_divider - adjustable divider clock
 *
 * @hw:     handle between common and hardware-specific interfaces
 * @reg:    register containing the divider
 * @shift:  shift to the divider bit field
 * @width:  width of the divider bit field
 * @table:  array of value/divider pairs, last entry should have div = 0
 * @lock:   register lock
 *
 * Clock with an adjustable divider affecting its output frequency.  Implements
 * .recalc_rate, .set_rate and .round_rate
 *
 * Flags:
 * CLK_DIVIDER_ONE_BASED - by default the divisor is the value read from the
 *  register plus one.  If CLK_DIVIDER_ONE_BASED is set then the divider is
 *  the raw value read from the register, with the value of zero considered
 *  invalid, unless CLK_DIVIDER_ALLOW_ZERO is set.
 * CLK_DIVIDER_POWER_OF_TWO - clock divisor is 2 raised to the value read from
 *  the hardware register
 * CLK_DIVIDER_ALLOW_ZERO - Allow zero divisors.  For dividers which have
 *  CLK_DIVIDER_ONE_BASED set, it is possible to end up with a zero divisor.
 *  Some hardware implementations gracefully handle this case and allow a
 *  zero divisor by not modifying their input clock
 *  (divide by one / bypass).
 * CLK_DIVIDER_HIWORD_MASK - The divider settings are only in lower 16-bit
 *   of this register, and mask of divider bits are in higher 16-bit of this
 *   register.  While setting the divider bits, higher 16-bit should also be
 *   updated to indicate changing divider bits.
 */
struct clk_divider {
    struct clk                      hw;
    void                           *reg;
    uint8_t                         shift;
    uint8_t                         width;
    uint8_t                         flags;
    aw_spinlock_isr_t              *lock;
    const struct clk_div_table     *table;
};

#define CLK_DIVIDER_ONE_BASED       AW_BIT(0)
#define CLK_DIVIDER_POWER_OF_TWO    AW_BIT(1)
#define CLK_DIVIDER_ALLOW_ZERO      AW_BIT(2)
#define CLK_DIVIDER_HIWORD_MASK     AW_BIT(3)

extern const struct clk_ops clk_divider_ops;
struct clk *clk_register_divider(
        struct clk_divider *p_clk_div,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        unsigned long flags,
        void *reg,
        uint8_t shift,
        uint8_t width,
        uint8_t clk_divider_flags,
        aw_spinlock_isr_t *lock );

struct clk *clk_register_divider_table(
        struct clk_divider *p_clk_div,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        unsigned long flags,
        void *reg,
        uint8_t shift,
        uint8_t width,
        uint8_t clk_divider_flags,
        const struct clk_div_table *table,
        aw_spinlock_isr_t *lock);


void aw_clk_init(struct clk * clk_base[],int count);
struct clk *clk_register_hw(struct clk *hw,struct clk_init_data *p_init);

/**
 * \brief struct clk_mux - multiplexer clock
 *
 * @hw:     handle between common and hardware-specific interfaces
 * @reg:    register controlling multiplexer
 * @shift:  shift to multiplexer bit field
 * @width:  width of mutliplexer bit field
 * @flags:  hardware-specific flags
 * @lock:   register lock
 *
 * Clock with multiple selectable parents.  Implements .get_parent, .set_parent
 * and .recalc_rate
 *
 * Flags:
 * CLK_MUX_INDEX_ONE - register index starts at 1, not 0
 * CLK_MUX_INDEX_BIT - register index is a single bit (power of two)
 * CLK_MUX_HIWORD_MASK - The mux settings are only in lower 16-bit of this
 *   register, and mask of mux bits are in higher 16-bit of this register.
 *   While setting the mux bits, higher 16-bit should also be updated to
 *   indicate changing mux bits.
 */
struct clk_mux {
    struct clk      hw;
    struct clk     *parents[CONFIG_CLK_MAX_PARENTS_NUM];
    int             num_parents;
    void           *reg;
    uint32_t        mask;
    uint8_t         shift;
    uint8_t         flags;
    aw_spinlock_isr_t *lock;
};

#define CLK_MUX_INDEX_ONE       AW_BIT(0)
#define CLK_MUX_INDEX_BIT       AW_BIT(1)
#define CLK_MUX_HIWORD_MASK     AW_BIT(2)
#define CLK_MUX_READ_ONLY       AW_BIT(3)   /* mux setting cannot be changed */

extern const struct clk_ops clk_mux_ops;
struct clk *clk_register_mux(
        struct clk_mux *p_clk_mux,
        aw_clk_id_t clk_id,
        const char *name,
        const char **parent_names,
        uint8_t num_parents,
        unsigned long flags,
        void *reg,
        uint8_t shift,
        uint8_t width,
        uint8_t clk_mux_flags,
        aw_spinlock_isr_t *lock);
/**
 * \brief struct clk_fixed_factor - fixed multiplier and divider clock
 *
 * @hw:     handle between common and hardware-specific interfaces
 * @mult:   multiplier
 * @div:    divider
 *
 * Clock with a fixed multiplier and divider. The output frequency is the
 * parent clock rate divided by div and multiplied by mult.
 * Implements .recalc_rate, .set_rate and .round_rate
 */

struct clk_fixed_factor {
    struct clk      hw;
    unsigned int    mult;
    unsigned int    div;
};

struct clk *clk_register_fixed_factor(
        struct clk_fixed_factor *p_clk_fixed_factor,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        unsigned long flags,
        unsigned int mult, unsigned int div);


#endif

/* end of file */
