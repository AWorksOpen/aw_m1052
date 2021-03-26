/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (C) 2004-2009    SEGGER Microcontroller Systeme GmbH        *
*                                                                    *
*      Internet: www.segger.com    Support:  support@segger.com      *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : RTOSPlugin.c
Purpose     : Extracts information about tasks from RTOS.

Additional information:
  Eclipse based debuggers show information about threads.

---------------------------END-OF-HEADER------------------------------
*/

#include "RTOSPlugin.h"
#include "JLINKARM_Const.h"
#include <stdio.h>

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/
typedef U32 uint32_t;
typedef U16 uint16_t;
typedef U8 uint8_t;

#ifdef WIN32
  #define EXPORT __declspec(dllexport)
#else
  #define EXPORT __attribute__((visibility("default")))
#endif

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/


#define EMBOS_PLUGIN_VERSION             100

#define RTK_CUR_THREAD_ID_NO_OS             (0x00000001)


/*********************************************************************
*
*       Types, local
*
**********************************************************************
*/
#define     JLINK_REG_LIST_NO_VFP_REG_COUNT             17
#define     JLINK_REG_LIST_VFP_REG_COUNT                50
#define     JLINK_REG_LIST_SP_INDEX                     13

typedef struct {
    uint32_t        threadid;                                   // thread ID
    uint32_t        use_vfp;
    char            sThreadName[64];                            // thread name
    uint32_t        prio;
    uint32_t        regs[JLINK_REG_LIST_VFP_REG_COUNT];      // 每个线程的寄存器上下文
} THREAD_DETAIL;


typedef struct {
    signed short   offset;
    unsigned short bits;
} rtk_regs_stack_offset;


/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static const GDB_API *_pAPI;


static struct {
    uint32_t        rtk_stack_use_flag;
    uint32_t        rtk_task_sp_offst;
    uint32_t        rtk_task_list_node_offset;
    uint32_t        rtk_task_name_offset;
    uint32_t        rtk_task_prio_offset;
    uint32_t        vfp_d_reg_count;
    uint32_t        current_thread_id;
    uint32_t        ThreadCount;
    THREAD_DETAIL  *pThreadDetails;
} _embOS;



static const int _armv7_base_regs_offset_in_stack[] = {

        1 * 4,      // R0
        2 * 4,      // R1
        3 * 4,      // R2
        4 * 4,      // R3
        5 * 4,      // R4
        6 * 4,      // R5
        7 * 4,      // R6
        8 * 4,      // R7
        9 * 4,      // R8
        10 * 4,     // R9
        11 * 4,     // R10
        12 * 4,     // R11
        13 * 4,     // R12
        -1,         // SP
        14 * 4,     // LR
        15 * 4,     // PC
        0 * 4,      // CPSR
};

static const int _armv7_fp_regs_offset_in_stack[] = {
        32 * 4,     // FPSCR
        0* 4,      // S0
        1* 4,      // S1
        2* 4,      // S2
        3* 4,      // S3
        4* 4,      // S4
        5* 4,      // S5
        6* 4,      // S6
        7* 4,      // S7
        8* 4,      // S8
        9* 4,      // S9
        10* 4,      // S10
        11* 4,      // S11
        12 * 4,      // S12
        13 * 4,      // S13
        14 * 4,      // S14
        15* 4,      // S15
        16 * 4,      // S16
        17 * 4,      // S17
        18 * 4,      // S18
        19 * 4,      // S19
        20 * 4,      // S20
        21 * 4,      // S21
        22 * 4,      // S22
        23 * 4,      // S23
        24 * 4,      // S24
        25 * 4,      // S25
        26* 4,      // S26
        27* 4,      // S27
        28* 4,      // S28
        29* 4,      // S29
        30 * 4,      // S30
        31 * 4,      // S31
};


static RTOS_SYMBOLS RTOS_Symbols[] = {
  { "g_system_tasks_head",          0, 0 },
  { "rtk_task_current",             0, 0 },
  { "rtk_armv7m_jlink_rtos_info",   0, 0 },
  { NULL, 0, 0 }
};

enum RTOS_Symbol_Values {
    RTK_SYM_TASK_HEAD = 0,
    RTK_SYM_CUR_TASK = 1,
    RTK_SYM_RTOS_INFO = 2,
};





/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _AllocThreadlist(int count)
*
*  Function description
*    Allocates a thread list for count entries.
*/
static int _AllocThreadlist(void)
{
    int             count;

    count = _embOS.ThreadCount;
    _embOS.pThreadDetails = (THREAD_DETAIL*)_pAPI->pfAlloc(
            count * sizeof(THREAD_DETAIL));
    if (NULL == _embOS.pThreadDetails) {
        return -1;
    }

    memset(_embOS.pThreadDetails, 0, count * sizeof(THREAD_DETAIL));
    return 0;
}



/*********************************************************************
*
*       _FreeThreadlist()
*
*  Function description
*    Frees the thread list
*/
static void _FreeThreadlist(void)
{
    if (_embOS.pThreadDetails) {
        _pAPI->pfFree(_embOS.pThreadDetails);
        _embOS.pThreadDetails = NULL;
        _embOS.ThreadCount = 0;
        _embOS.current_thread_id = 0;
    }
}

/*********************************************************************
*
*       _ReadStack(U32 threadid)
*
*  Function description
*    Reads the task stack of the task with the ID threadid into _StackMem.
*/
static int _ReadThreadStack(
        uint32_t task_list_node_addr,
        THREAD_DETAIL *p_thread_tail)
{
    uint32_t                    rtk_task_addr;
    uint32_t                    rtk_task_sp_addr;
    uint32_t                    rtk_task_sp;
    uint32_t                    rtk_task_name_addr;
    uint32_t                    rtk_task_name;
    int                         api_ret;
    uint32_t                    temp_val;
    uint32_t                    temp_addr;
    uint32_t                    task_use_fp = 0;        //默认任务不使用硬件浮点
    const int                  *p_regs_offset_in_stack;
    int                         regs_count;
    int                         i;
    uint8_t                     temp_val8;

    memset(p_thread_tail,0,sizeof(*p_thread_tail));

    // 由task_list_node结构地址确定rtk_task结构的地址
    rtk_task_addr = task_list_node_addr - _embOS.rtk_task_list_node_offset;
    // 更新线程ID
    p_thread_tail->threadid = rtk_task_addr;
    
    // 更新线程优先级
    // 由rtk_task结构地址确定rtk_task成员name的地址
    temp_addr = rtk_task_addr + _embOS.rtk_task_prio_offset;
    api_ret = _pAPI->pfReadU32(temp_addr, &temp_val);
    if (api_ret != 0) {
        _pAPI->pfErrorOutf("Error reading rtk prio.\n");
        return api_ret;
    }
    p_thread_tail->prio = temp_val;

    // 更新线程名称
    // 由rtk_task结构地址确定rtk_task成员name的地址
    rtk_task_name_addr = rtk_task_addr + _embOS.rtk_task_name_offset;
    api_ret = _pAPI->pfReadU32(rtk_task_name_addr, &rtk_task_name);
    if (api_ret != 0) {
        _pAPI->pfErrorOutf("Error reading rtk task's name.\n");
        return api_ret;
    }

    if (0 == rtk_task_name) {
        // 无name
        snprintf(
            p_thread_tail->sThreadName,
            sizeof(p_thread_tail->sThreadName),
            "no name");
    }
    else {
        for (i = 0; i < (sizeof(p_thread_tail->sThreadName) - 1);i++) {
            api_ret = _pAPI->pfReadU8(rtk_task_name + i, &temp_val8);
            if (api_ret != 0) {
                _pAPI->pfErrorOutf("Error reading rtk task's name.\n");
                return api_ret;
            }

            p_thread_tail->sThreadName[i] = temp_val8;
            if (0 == temp_val8) {
                break;
            }

        }
        p_thread_tail->sThreadName[i] = 0;
    }

    // 由rtk_task结构地址确定rtk_task成员sp的地址
    rtk_task_sp_addr = rtk_task_addr + _embOS.rtk_task_sp_offst;

    // 更新线程寄存器
    // 当前线程特殊处理
    if (rtk_task_addr == _embOS.current_thread_id) {
        return 0;
    }

    // 读取rtk_task成员SP的值
    api_ret = _pAPI->pfReadU32(rtk_task_sp_addr, &rtk_task_sp);
    if (api_ret != 0) {
        _pAPI->pfErrorOutf("Error reading rtk task's sp.\n");
        return api_ret;
    }


    // 如果RTK的栈中使用了flag(在支持浮点或者SVC系统调用模式下)
    if (_embOS.rtk_stack_use_flag) {
        api_ret = _pAPI->pfReadU32(rtk_task_sp, &temp_val);
        if (api_ret != 0) {
            _pAPI->pfErrorOutf("Error reading rtk task's context flags.\n");
            return api_ret;
        }

        if (temp_val ){
            task_use_fp = 1;
        }
        rtk_task_sp +=4;
    }
    else {
        // 栈中没有flag，则表明不支持硬件浮点，也不支持SVC系统调用
    }

    // 读取所有的VFP寄存器
    if (task_use_fp) {
        
        if (_embOS.vfp_d_reg_count >16) {
            rtk_task_sp += 8 *(_embOS.vfp_d_reg_count - 16);
        }
        p_regs_offset_in_stack =_armv7_fp_regs_offset_in_stack;
        regs_count = sizeof(_armv7_fp_regs_offset_in_stack)/
                        sizeof(_armv7_fp_regs_offset_in_stack[0]);
                        
        // 从栈中读取所有可以读取的寄存器
        for (i = 0;i < regs_count; i ++) {
            if (p_regs_offset_in_stack[i] >= 0) {
                temp_addr = rtk_task_sp + p_regs_offset_in_stack[i];
                api_ret = _pAPI->pfReadU32(temp_addr, &temp_val);
                if (api_ret != 0) {
                    _pAPI->pfErrorOutf(
                            "Error reading rtk task's context @0x%08x.\n",
                            temp_addr);
                    return api_ret;
                }
                p_thread_tail->regs[i + JLINK_REG_LIST_NO_VFP_REG_COUNT] = temp_val;
            }
            else {
                // 栈中没有的内容返回寄存器的值返回0
            }
        }
        rtk_task_sp += regs_count * 4;
    }
    // 读取通用寄存器
    p_regs_offset_in_stack =_armv7_base_regs_offset_in_stack;
    regs_count = sizeof(_armv7_base_regs_offset_in_stack)/
                    sizeof(_armv7_base_regs_offset_in_stack[0]);
                    
    // 从栈中读取所有可以读取的寄存器
    for (i = 0;i < regs_count; i ++) {
        if (p_regs_offset_in_stack[i] >= 0) {
            temp_addr = rtk_task_sp + p_regs_offset_in_stack[i];
            api_ret = _pAPI->pfReadU32(temp_addr, &temp_val);
            if (api_ret != 0) {
                _pAPI->pfErrorOutf(
                        "Error reading rtk task's context @0x%08x.\n",
                        temp_addr);
                return api_ret;
            }
            p_thread_tail->regs[i] = temp_val;
        }
        else {
            // 栈中没有的内容返回寄存器的值返回0
        }
    }

    // 处理SP
    rtk_task_sp += regs_count * 4;
    p_thread_tail->regs[JLINK_REG_LIST_SP_INDEX] = rtk_task_sp;



    // 至此，寄存器更新完毕
    p_thread_tail->use_vfp = task_use_fp;

    // 更新完毕，返回成功
    return 0;
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

EXPORT int RTOS_Init(const GDB_API *pAPI, U32 core)
{
    _pAPI = pAPI;
    memset(&_embOS, 0, sizeof(_embOS));
    if (   (core == JLINK_CORE_CORTEX_A7)
            || (core == JLINK_CORE_CORTEX_A8)
            || (core == JLINK_CORE_CORTEX_A9)
         ) {
        return 1;
    }
    return 0;
}

EXPORT U32 RTOS_GetVersion()
{
    return EMBOS_PLUGIN_VERSION;
}

EXPORT RTOS_SYMBOLS* RTOS_GetSymbols()
{
    return RTOS_Symbols;
}

EXPORT U32 RTOS_GetNumThreads()
{
    return _embOS.ThreadCount;
}

EXPORT U32 RTOS_GetCurrentThreadId()
{
    return _embOS.current_thread_id;
}

EXPORT U32 RTOS_GetThreadId(U32 index)
{
    if (0 == _embOS.current_thread_id) {
        return 0;
    }

    return _embOS.pThreadDetails[index].threadid;
}

EXPORT int RTOS_GetThreadDisplay(char *pDisplay, U32 threadid)
{
    int                 find;
    int                 i;
    THREAD_DETAIL      *p_thread_detail;
    unsigned char      *p_src_buf;
    char               *p_hex_buf;

    if (0 == _embOS.ThreadCount) {
        return -1;
    }

    // 查找指定的线程
    find = 0;
    for (i = 0;i<_embOS.ThreadCount;i ++) {
        if (threadid == _embOS.pThreadDetails[i].threadid) {
            p_thread_detail = &_embOS.pThreadDetails[i];
            find = 1;
            break;
        }
    }
    if (!find ) {
        return 0;
    }

    snprintf(pDisplay,64,"%s:[%u]",p_thread_detail->sThreadName,p_thread_detail->prio);
    return strlen(pDisplay);
}

EXPORT int RTOS_GetThreadReg(
        char *pHexRegVal,
        uint32_t reg_jlink_index,
        uint32_t threadid)
{
    int                 reg_index;
    int                 find;
    int                 i;
    THREAD_DETAIL      *p_thread_detail;
    unsigned char      *p_src_buf;
    char               *p_hex_buf;

    if (threadid == 0 || threadid == _embOS.current_thread_id) {
        return -1; // Current thread or current execution returns CPU registers
    }

    // 从jlink_index转换到reg_index
    reg_index = reg_jlink_index;
    // 查找指定的线程
    find = 0;
    for (i = 0;i<_embOS.ThreadCount;i ++) {
        if (threadid == _embOS.pThreadDetails[i].threadid) {
            p_thread_detail = &_embOS.pThreadDetails[i];
            find = 1;
            break;
        }
    }
    
    if (!find) {
        return -1;
    }

    p_hex_buf = pHexRegVal;
    p_src_buf = (unsigned char *)&p_thread_detail->regs[reg_index];
    for (i = 0;i<4;i++) {
        p_hex_buf += snprintf(p_hex_buf, 3, "%02x", p_src_buf[i]);
    }

    return 0;
}

EXPORT int RTOS_GetThreadRegList(char *pHexRegList, U32 threadid)
{
    int                 reg_jlink_index;
    int                 find;
    int                 i;
    THREAD_DETAIL      *p_thread_detail;
    unsigned char      *p_src_buf;
    char               *p_hex_buf;

    if (threadid == 0 || threadid == _embOS.current_thread_id) {
        return -1; // Current thread or current execution returns CPU registers
    }

    // 查找指定的线程
    find = 0;
    for (i = 0;i<_embOS.ThreadCount;i ++) {
        if (threadid == _embOS.pThreadDetails[i].threadid) {
            p_thread_detail = &_embOS.pThreadDetails[i];
            find = -1;
            break;
        }
    }

    if (!find) {
        return -1;
    }


    p_hex_buf = pHexRegList;
    for (reg_jlink_index = 0;
            reg_jlink_index <= 49;
            reg_jlink_index ++) {
        p_src_buf = (unsigned char *)&p_thread_detail->regs[reg_jlink_index];
        for (i = 0;i<4;i++) {
            p_hex_buf += snprintf(p_hex_buf, 3, "%02x", p_src_buf[i]);
        }
    }
    
    for (i = 0;i < 27;i++) {
        p_hex_buf += snprintf(p_hex_buf, 9, "%s","00000000");
    }

    return 0;
}

EXPORT int RTOS_SetThreadReg(char* pHexRegVal, U32 RegIndex, U32 threadid)
{
    (void)pHexRegVal;
    (void)RegIndex;

    if (threadid == 0 || threadid == _embOS.current_thread_id) {
        return -1; // Current thread or current execution return CPU registers
    }
    //
    // Currently not supported
    //
    return 0;
}

EXPORT int RTOS_SetThreadRegList(char *pHexRegList, U32 threadid)
{
    (void)pHexRegList;

    if (threadid == 0 || threadid == _embOS.current_thread_id) {
        return -1; // Current thread or current execution return CPU registers
    }
    //
    // Currently not supported
    //
    return 0;
}

static int _rtk_refesh_global_info(void)
{
    int                 api_ret;
    uint32_t            rtos_info_addr;
    uint32_t            temp_val;

    rtos_info_addr = RTOS_Symbols[RTK_SYM_RTOS_INFO].address;

    api_ret = _pAPI->pfReadU32(
            rtos_info_addr,&temp_val);
    if (0 != api_ret) {
        return api_ret;
    }
    _embOS.rtk_task_list_node_offset = temp_val;

    api_ret = _pAPI->pfReadU32(
            rtos_info_addr + 4,&temp_val);
    if (0 != api_ret) {
        return api_ret;
    }

    _embOS.rtk_task_sp_offst = temp_val;

    api_ret = _pAPI->pfReadU32(
            rtos_info_addr + 8,&temp_val);
    if (0 != api_ret) {
        return api_ret;
    }
    _embOS.rtk_stack_use_flag = temp_val;

    api_ret = _pAPI->pfReadU32(
            rtos_info_addr + 12,&temp_val);
    if (0 != api_ret) {
        return api_ret;
    }
    _embOS.rtk_task_name_offset = temp_val;
    
    api_ret = _pAPI->pfReadU32(
            rtos_info_addr + 16,&temp_val);
    if (0 != api_ret) {
        return api_ret;
    }
    _embOS.rtk_task_prio_offset = temp_val;
    
    api_ret = _pAPI->pfReadU32(
            rtos_info_addr + 20,&temp_val);
    if (0 != api_ret) {
        return api_ret;
    }
    _embOS.vfp_d_reg_count = temp_val;


    return  0;
}

static int _rtk_get_threads_count(uint32_t *p_count)
{
    int                 api_ret;
    uint32_t            rtk_system_task_head_addr;
    uint32_t            next;
    uint32_t            cur_task;
    int                 count = 0;

    rtk_system_task_head_addr = RTOS_Symbols[RTK_SYM_TASK_HEAD].address;

    api_ret = _pAPI->pfReadU32(
            RTOS_Symbols[RTK_SYM_CUR_TASK].address,&cur_task);
    if (0 != api_ret) {
        return api_ret;
    }


    // 任务还没启动
    if (0 == cur_task) {
        *p_count = 0;
        return 0;
    }

    api_ret = _pAPI->pfReadU32(
            RTOS_Symbols[RTK_SYM_TASK_HEAD].address,&next);
    if (0 != api_ret) {
        return api_ret;
    }

    if (0 == next || next == rtk_system_task_head_addr) {
        *p_count = 0;
        return 0;
    }

    while(next != rtk_system_task_head_addr) {
        count ++;
        api_ret = _pAPI->pfReadU32(
            next,&next);
        if (0 != api_ret) {
            return api_ret;
        }
    }

    *p_count = count;
    return  0;
}

static int _rtk_get_all_thread_info(void)
{
    int                 api_ret;
    uint32_t            rtk_system_task_head_addr;
    uint32_t            next;
    uint32_t            cur_task;
    int                 count = 0;

    rtk_system_task_head_addr = RTOS_Symbols[RTK_SYM_TASK_HEAD].address;

    api_ret = _pAPI->pfReadU32(
            RTOS_Symbols[RTK_SYM_CUR_TASK].address,&cur_task);
    if (0 != api_ret) {
        return api_ret;
    }


    // 任务还没启动
    if (0 == cur_task) {
        return 0;
    }

    _embOS.current_thread_id = cur_task;

    api_ret = _pAPI->pfReadU32(
            RTOS_Symbols[RTK_SYM_TASK_HEAD].address,&next);
    if (0 != api_ret) {
        return api_ret;
    }

    if (0 == next || next == rtk_system_task_head_addr) {
        return 0;
    }

    while(next != rtk_system_task_head_addr) {

        api_ret = _ReadThreadStack(next,&_embOS.pThreadDetails[count]);
        if (0 != api_ret) {
            return api_ret;
        }
        count ++;
        api_ret = _pAPI->pfReadU32(
            next,&next);
        if (0 != api_ret) {
            return api_ret;
        }
    }

    return  0;
}

EXPORT int RTOS_UpdateThreads()
{
    U32             retval;
    U32             tcb;
    U32             TasksFound;
    U32             ThreadListSize;
    uint32_t        task_count;
    int             api_ret;

    _FreeThreadlist();

    api_ret = _rtk_refesh_global_info();
    if (0 != api_ret) {
        return api_ret;
    }

    api_ret = _rtk_get_threads_count(&task_count);
    if (0 != api_ret) {
        return api_ret;
    }

    _embOS.ThreadCount = task_count;
    if (0 == _embOS.ThreadCount) {
        _embOS.ThreadCount = 1;
    }
    api_ret = _AllocThreadlist();
    if (0 != api_ret) {
        return api_ret;
    }

    if (0 == task_count) {
        // 操作系统还没启动
        _embOS.pThreadDetails->threadid = RTK_CUR_THREAD_ID_NO_OS;
        _embOS.pThreadDetails->use_vfp = 0;
        strncpy(_embOS.pThreadDetails->sThreadName,"startup",64);
        _embOS.pThreadDetails->prio = 0;
        _embOS.current_thread_id = RTK_CUR_THREAD_ID_NO_OS;
    }
    else {
        api_ret = _rtk_get_all_thread_info();
        if (0 != api_ret) {
            return api_ret;
        }
    }

    return 0;

}
