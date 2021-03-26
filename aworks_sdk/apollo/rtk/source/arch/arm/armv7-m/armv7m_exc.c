/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https:/*www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ARMv7-M exception library
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-08  zen, first implemetation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_exc.h"
#include "aw_bitops.h"
#include "aw_task.h"
#include "rtk.h"
#include "rtk_int.h"
#include "rtk_task_internal.h"
#include "arch/arm/armv7-m/armv7m_core.h"

static const char * get_cur_task_name()
{
    struct rtk_task         *p_task;
    p_task = rtk_task_self();

    if (NULL != p_task) {
        return p_task->name;
    }
    else {
        return "no task";
    }
}

/*******************************************************************************
  defines
*******************************************************************************/

#define __EXCNUM_IS_VALID(excnum)  (((excnum) >= 0) && ((excnum) <= EXCNUM_MAX))

/*******************************************************************************
  globals
*******************************************************************************/

aw_local aw_pfuncvoid_t __gpfunc_exc_nmi_handler;       /* NMI handler */
aw_local aw_pfuncvoid_t __gpfunc_exc_systick_handler;   /* SysTick handler */
aw_local aw_pfuncvoid_t __gpfunc_exc_eint_handler;      /* EINT handler */

/*******************************************************************************
  implementation
*******************************************************************************/


/******************************************************************************/
aw_err_t aw_exc_int_connect(int excnum, aw_pfuncvoid_t pfunc_esr)
{
    if (!__EXCNUM_IS_VALID(excnum)) {
        return -AW_ENXIO;      /* not a valid exception number */
    }

    if (pfunc_esr == NULL) {
        return -AW_EFAULT;     /* not a valid address */
    }

    switch (excnum) {

    case EXCNUM_NMI:
        __gpfunc_exc_nmi_handler = pfunc_esr;
        break;

    case EXCNUM_SYSTICK:
        __gpfunc_exc_systick_handler = pfunc_esr;
        break;

    case EXCNUM_EINT:
        __gpfunc_exc_eint_handler = pfunc_esr;
        break;

    default:
        return -AW_ENOTSUP;    /* not supported exception */
    }

    return AW_OK;
}

/******************************************************************************/
void armv7m_exc_nmi_handler (void)
{
    if (__gpfunc_exc_nmi_handler != NULL) {
        __gpfunc_exc_nmi_handler(NULL);
        return;
    }

    while (1);
}

typedef struct {
    uint32_t        R0;         /* 异常发生时R0 */
    uint32_t        R1;         /* 异常发生时R1 */
    uint32_t        R2;         /* 异常发生时R2 */
    uint32_t        R3;         /* 异常发生时R3 */
    uint32_t        R4;         /* 异常发生时R4 */
    uint32_t        R5;         /* 异常发生时R5 */
    uint32_t        R6;         /* 异常发生时R6 */
    uint32_t        R7;         /* 异常发生时R7 */
    uint32_t        R8;         /* 异常发生时R8 */
    uint32_t        R9;         /* 异常发生时R9 */
    uint32_t        R10;        /* 异常发生时R10 */
    uint32_t        R11;        /* 异常发生时R11 */
    uint32_t        R12;        /* 异常发生时R12 */
    uint32_t        SP;         /* 异常发生时SP */
    uint32_t        LR;         /* 异常发生时LR */
    uint32_t        PC;         /* 异常发生时的PC值 */
    uint32_t        xPSR;
    uint32_t        MSP;
    uint32_t        PSP;
    uint32_t        LR_FATAL;   /* 异常返回值 */
}fault_error_context_t;

fault_error_context_t __g_fault_err_context;

#define kprintf aw_kprintf

typedef void (*pfn_show_fault_info_t)(void);

struct armv7m_fault
{
    const char *            fault_name;
    pfn_show_fault_info_t   pfn_show_info;
};

static void show_memmanage_fault_info(void)
{
    uint32_t        mfsr;

    mfsr = AW_REG_BITS_GET32(&ARMV7M_SCB->CFSR,0,8);
    kprintf("MemManage fault's status is:");

    if(mfsr & (1 << 0) ) {
        kprintf("prefetch abort;");
    }

    if(mfsr & (1 << 1) ) {
        kprintf("data abort;");
    }

    if(mfsr & (1 << 3) ) {
        kprintf("pop error;");
    }

    if(mfsr & (1 << 4) ) {
        kprintf("push error;");
    }

    kprintf("\n");

    if(mfsr & (1 << 7) ) {
        kprintf("fault when access 0x%08x\n",ARMV7M_SCB->MMFAR);
    }

}

static void show_bus_fault_info(void)
{
    uint32_t        bfsr;

    bfsr = AW_REG_BITS_GET32(&ARMV7M_SCB->CFSR,8,8);

    kprintf("bus fault's status is:");
    if(bfsr & (1 << 0) ) {
        kprintf("prefetch abort;");
    }

    if(bfsr & (1 << 1) ) {
        kprintf("precise data abort;");
    }
    if(bfsr & (1 << 2) ) {
        kprintf("imprecise data abort;");
    }


    if(bfsr & (1 << 3) ) {
        kprintf("pop error;");
    }

    if(bfsr & (1 << 4) ) {
        kprintf("push error;");
    }

    kprintf("\n");

    if(bfsr & (1 << 7) ) {
        kprintf("fault when access 0x%08x\n",ARMV7M_SCB->BFAR);
    }

}

static void show_usage_fault_info(void)
{
    uint32_t        ufsr;

    ufsr = AW_REG_BITS_GET32(&ARMV7M_SCB->CFSR,16,16);
    kprintf("usage fault's status is:");

    if(ufsr & (1 << 0) ) {
        kprintf("unkown instruction;");
    }

    if(ufsr & (1 << 1) ) {
        kprintf("try to arm state;");
    }
    if(ufsr & (1 << 2) ) {
        kprintf("invalid EXC_RETURN;");
    }


    if(ufsr & (1 << 3) ) {
        kprintf("No coprocesser;");
    }

    if(ufsr & (1 << 8) ) {
        kprintf("unaligned;");
    }
    if(ufsr & (1 << 9) ) {
        kprintf("div by zero;");
    }

    kprintf("\n");

}

static void show_hard_fault_info(void)
{
    uint32_t            hfsr;

    hfsr = readl(&ARMV7M_SCB->HFSR);
    kprintf("hard fault's status is:");

    if(hfsr & (1 << 1) ) {
        kprintf("prefetch vector;");
    }

    if(hfsr & (1 << 30) ) {
        kprintf("other fault upgrade;");
    }

    if(hfsr & (1 << 31) ) {
        kprintf("debug event;");
    }

    kprintf("\n");

    if(hfsr & (1 << 30) ) {
        uint32_t        cfsr;
        cfsr = ARMV7M_SCB->CFSR;
        if ( (cfsr >>0) & 0xFF ) {
            show_memmanage_fault_info();
        }
        if ( (cfsr >>8) & 0xFF ) {
            show_bus_fault_info();
        }
        if ( (cfsr >>16) & 0xFFF ) {
            show_usage_fault_info();
        }

    }
}

static const struct armv7m_fault __g_fault[] = {
        {"unknown",NULL},
        {"unknown",NULL},
        {"unknown",NULL},
        {"hard fault",show_hard_fault_info},
        {"MemManage fault",show_memmanage_fault_info},
        {"bus fault",show_bus_fault_info},
        {"usage fault",show_usage_fault_info},
        {"unknown",NULL},
        {"unknown",NULL},
        {"unknown",NULL},
        {"unknown",NULL},
        {"unknown",NULL},
        {"unknown",NULL},
        {"unknown",NULL},
};

static void armv7m_show_fault_context(fault_error_context_t *p)
{
    kprintf("The fault context:\n");
    kprintf("R0 = 0x%08x\n",p->R0);
    kprintf("R1 = 0x%08x\n",p->R1);
    kprintf("R2 = 0x%08x\n",p->R2);
    kprintf("R3 = 0x%08x\n",p->R3);
    kprintf("R4 = 0x%08x\n",p->R4);
    kprintf("R5 = 0x%08x\n",p->R5);
    kprintf("R6 = 0x%08x\n",p->R6);
    kprintf("R7 = 0x%08x\n",p->R7);
    kprintf("R8 = 0x%08x\n",p->R8);
    kprintf("R9 = 0x%08x\n",p->R9);
    kprintf("R10 = 0x%08x\n",p->R10);
    kprintf("R11 = 0x%08x\n",p->R11);
    kprintf("R12 = 0x%08x\n",p->R12);
    kprintf("SP = 0x%08x\n",p->SP);
    kprintf("LR = 0x%08x\n",p->LR);
    kprintf("PC = 0x%08x\n",p->PC);

    kprintf("xPSR = 0x%08x\n",p->xPSR);
    kprintf("MSP = 0x%08x\n",p->MSP);
    kprintf("PSP = 0x%08x\n",p->PSP);
    kprintf("LR_FATAL = 0x%08x\n",p->LR_FATAL);
}

void armv7m_show_fault_error(void)
{
    volatile int                not_exit = 1;
    fault_error_context_t      *p_fatal_context = &__g_fault_err_context;
    int                         val;
    const struct armv7m_fault   *p_fault_info = NULL;

    val = readl(&ARMV7M_SCB->ICSR);
    val &= 0x1FF;
    if (val < sizeof(__g_fault)/sizeof(__g_fault[0] ) ) {
        p_fault_info = &__g_fault[val];
    }
    if (NULL != p_fault_info ) {
        kprintf(
                "\n%s@0x%08x @%s\n",
                p_fault_info->fault_name,
                p_fatal_context->PC,
                get_cur_task_name());
        if (NULL != p_fault_info->pfn_show_info) {
            p_fault_info->pfn_show_info();
        }
    }

    armv7m_show_fault_context(p_fatal_context);
    while(not_exit);
}



/******************************************************************************/
void armv7m_exc_systick_handler (void)
{
    rtk_tick_down_counter_announce();
}

/******************************************************************************/

void armv7m_exc_eint_handler (void)
{
    if (__gpfunc_exc_eint_handler != NULL) {

        RTK_ENTER_INT_CONTEXT();
        __gpfunc_exc_eint_handler(NULL);
        RTK_EXIT_INT_CONTEXT();

        return;
    }

    while (1);
}

void armv7m_exc_nvic_int_handler (void)
{
    RTK_ENTER_INT_CONTEXT();
    armv7m_nvic_gen_int_handler();
    RTK_EXIT_INT_CONTEXT();
}

void rtk_cpu_enter_int_context(void)
{

}

void rtk_cpu_exit_int_context(void)
{

}
/* end of file */
