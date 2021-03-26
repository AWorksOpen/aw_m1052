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

// armv7m �������б�־�Ķ���
#define ARMV7M_CONTEXT_FLAGS_FPU_ENABLE     (1 << 0)
#define ARMV7M_CONTEXT_FLAGS_PRIVILEGED     (1 << 1)

#define ARMV7M_CONTOL_SP_SEL_PSP            (1 << 1)
#define ARMV7M_CONTOL_SP_SEL_MSP            (0 << 1)

#define ARMV7M_CONTOL_SP_SEL_FPCA           (1 << 2)


#define EMBOS_PLUGIN_VERSION             100

#define RTK_CUR_THREAD_ID_NO_OS             (0x00000001)


/*********************************************************************
*
*       Types, local
*
**********************************************************************
*/
#define     ARMV7M_REG_SP_THREAD_DETAIL_INDEX           13
#define     ARMV7M_REG_MSP_THREAD_DETAIL_INDEX          17
#define     ARMV7M_REG_PSP_THREAD_DETAIL_INDEX          18
#define     ARMV7M_REG_CONTROL_THREAD_DETAIL_INDEX      22
#define     ARMV7M_REG_XPSR_THREAD_DETAIL_INDEX         16

#define     RTK_ARMV7M_CONTEXT_REG_COUNT_VFP            56
#define     RTK_ARMV7M_CONTEXT_REG_COUNT_NO_VFP         23

typedef struct {
    uint32_t        threadid;           // thread ID
    uint32_t        use_vfp;
    char            sThreadName[64];    // thread name
    uint32_t        regs[56];           // ÿ���̵߳ļĴ���������
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
    uint32_t        current_thread_id;
    uint32_t        ThreadCount;
    THREAD_DETAIL  *pThreadDetails;
} _embOS;



static const int _armv7m_base_regs_offset_in_stack[] = {

        8 * 4,      // R0
        9 * 4,      // R1
        10* 4,      // R2
        11* 4,      // R3
        0 * 4,      // R4
        1 * 4,      // R5
        2 * 4,      // R6
        3 * 4,      // R7
        4 * 4,      // R8
        5 * 4,      // R9
        6 * 4,      // R10
        7 * 4,      // R11
        12 * 4,     // R12
        -2,         // SP
        13 *4,      // LR
        14 *4,      // PC
        15 *4,      // XPSR
        -1,         // MSP
        -1,         // PSP
        -1,         // PRIMASK
        -1,         // BASEPRI
        -1,         // FAULTMASK
        -1,         // CONTROL
};

static const int _armv7m_vfp_regs_offset_in_stack[] = {
        24* 4,      // R0
        25* 4,      // R1
        26* 4,      // R2
        27* 4,      // R3
        16* 4,      // R4
        17* 4,      // R5
        18* 4,      // R6
        19* 4,      // R7
        20* 4,      // R8
        21* 4,      // R9
        22* 4,      // R10
        23* 4,      // R11
        28* 4,      // R12
        -2,         // SP
        29 *4,      // LR
        30 *4,      // PC
        31 *4,      // XPSR
        -1,         // MSP
        -1,         // PSP
        -1,         // PRIMASK
        -1,         // BASEPRI
        -1,         // FAULTMASK
        -1,         // CONTROL
        48 * 4,     // FPSCR
        32* 4,      // S0
        33* 4,      // S1
        34* 4,      // S2
        35* 4,      // S3
        36* 4,      // S4
        37* 4,      // S5
        38* 4,      // S6
        39* 4,      // S7
        40* 4,      // S8
        41* 4,      // S9
        42* 4,      // S10
        43* 4,      // S11
        44* 4,      // S12
        45* 4,      // S13
        46* 4,      // S14
        47* 4,      // S15
        0 * 4,      // S16
        1 * 4,      // S17
        2 * 4,      // S18
        3 * 4,      // S19
        4 * 4,      // S20
        5 * 4,      // S21
        6 * 4,      // S22
        7 * 4,      // S23
        8 * 4,      // S24
        9 * 4,      // S25
        10* 4,      // S26
        11* 4,      // S27
        12* 4,      // S28
        13* 4,      // S29
        14* 4,      // S30
        15* 4,      // S31
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
    uint32_t                    task_use_fp = 0;        //Ĭ������ʹ��Ӳ������
    const int                  *p_regs_offset_in_stack;
    int                         regs_count;
    int                         i;
    uint32_t                    regs_count_in_stack = 0;
    uint8_t                     temp_val8;

    memset(p_thread_tail,0,sizeof(*p_thread_tail));

    // ��task_list_node�ṹ��ַȷ��rtk_task�ṹ�ĵ�ַ
    rtk_task_addr = task_list_node_addr - _embOS.rtk_task_list_node_offset;
    // �����߳�ID
    p_thread_tail->threadid = rtk_task_addr;

    // �����߳�����
    // ��rtk_task�ṹ��ַȷ��rtk_task��Աname�ĵ�ַ
    rtk_task_name_addr = rtk_task_addr + _embOS.rtk_task_name_offset;
    api_ret = _pAPI->pfReadU32(rtk_task_name_addr, &rtk_task_name);
    if (api_ret != 0) {
        _pAPI->pfErrorOutf("Error reading rtk task's name.\n");
        return api_ret;
    }

    if (0 == rtk_task_name) {
        // ��name
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

    // ��rtk_task�ṹ��ַȷ��rtk_task��Աsp�ĵ�ַ
    rtk_task_sp_addr = rtk_task_addr + _embOS.rtk_task_sp_offst;

    // �����̼߳Ĵ���
    // ��ǰ�߳����⴦��
    if (rtk_task_addr == _embOS.current_thread_id) {
        return 0;
    }

    // ��ȡrtk_task��ԱSP��ֵ
    api_ret = _pAPI->pfReadU32(rtk_task_sp_addr, &rtk_task_sp);
    if (api_ret != 0) {
        _pAPI->pfErrorOutf("Error reading rtk task's sp.\n");
        return api_ret;
    }


    // ���RTK��ջ��ʹ����flag(��֧�ָ������SVCϵͳ����ģʽ��)
    if (_embOS.rtk_stack_use_flag) {
        api_ret = _pAPI->pfReadU32(rtk_task_sp, &temp_val);
        if (api_ret != 0) {
            _pAPI->pfErrorOutf("Error reading rtk task's context flags.\n");
            return api_ret;
        }

        if (temp_val & ARMV7M_CONTEXT_FLAGS_FPU_ENABLE){
            task_use_fp = 1;
        }
        rtk_task_sp +=4;
    }
    else {
        // ջ��û��flag���������֧��Ӳ�����㣬Ҳ��֧��SVCϵͳ����
    }


    if (task_use_fp) {
        p_regs_offset_in_stack =_armv7m_vfp_regs_offset_in_stack;
        regs_count = sizeof(_armv7m_vfp_regs_offset_in_stack)/
                        sizeof(_armv7m_vfp_regs_offset_in_stack[0]);
        regs_count_in_stack = 50;
    }
    else {
        p_regs_offset_in_stack =_armv7m_base_regs_offset_in_stack;
        regs_count = sizeof(_armv7m_base_regs_offset_in_stack)/
                        sizeof(_armv7m_base_regs_offset_in_stack[0]);
        regs_count_in_stack = 16;
    }

    // ��ջ�ж�ȡ���п��Զ�ȡ�ļĴ���
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
            // ջ��û�е����ݷ��ؼĴ�����ֵ����0
        }
    }

    // ����PSP
    temp_addr = p_thread_tail->regs[ARMV7M_REG_XPSR_THREAD_DETAIL_INDEX];
    temp_val = rtk_task_sp + regs_count_in_stack * 4;
    if (temp_addr & (1 << 9)) {
        if (temp_val % 8 != 0) {
            temp_val +=4;
        }
    }
    p_thread_tail->regs[ARMV7M_REG_PSP_THREAD_DETAIL_INDEX] = temp_val;
    // ����������˵��ʼ��ʹ��PSP
    p_thread_tail->regs[ARMV7M_REG_SP_THREAD_DETAIL_INDEX] = temp_val;

    // MSP�޷���ȡ
    // PRIMASK��BASEPRIӦ��Ϊ0,�����޷����������л�����ô������Ӧ���ǵ�ǰ����
    // FAULTMASKҲӦ��Ϊ0
    // CONTROL�Ĵ���,ȡ���������Ƿ�ʹ���˸���
    temp_val = ARMV7M_CONTOL_SP_SEL_PSP;
    if (task_use_fp) {
        temp_val |= ARMV7M_CONTOL_SP_SEL_FPCA;
    }
    p_thread_tail->regs[ARMV7M_REG_CONTROL_THREAD_DETAIL_INDEX] = temp_val;


    // ���ˣ��Ĵ����������
    p_thread_tail->use_vfp = task_use_fp;

    // ������ϣ����سɹ�
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
    if (   (core == JLINK_CORE_CORTEX_M3)
            || (core == JLINK_CORE_CORTEX_M4)
            || (core == JLINK_CORE_CORTEX_M7)
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

    // ����ָ�����߳�
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

    snprintf(pDisplay,64,"%s",p_thread_detail->sThreadName);
    return strlen(p_thread_detail->sThreadName);
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

    // ��jlink_indexת����reg_index
    reg_index = reg_jlink_index;
    // ����ָ�����߳�
    find = 0;
    for (i = 0;i<_embOS.ThreadCount;i ++) {
        if (threadid == _embOS.pThreadDetails[i].threadid) {
            p_thread_detail = &_embOS.pThreadDetails[i];
            find = -1;
            break;
        }
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
    int                 reg_index;
    int                 reg_jlink_index;
    int                 find;
    int                 i;
    THREAD_DETAIL      *p_thread_detail;
    unsigned char      *p_src_buf;
    char               *p_hex_buf;

    if (threadid == 0 || threadid == _embOS.current_thread_id) {
        return -1; // Current thread or current execution returns CPU registers
    }

    // ����ָ�����߳�
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
            reg_jlink_index <= 16;
            reg_jlink_index ++) {
        reg_index = reg_jlink_index;
        if (reg_index < 0) {
            p_hex_buf += snprintf(p_hex_buf, 9, "%s","00000000");
        }
        else {
            p_src_buf = (unsigned char *)&p_thread_detail->regs[reg_index];
            for (i = 0;i<4;i++) {
                p_hex_buf += snprintf(p_hex_buf, 3, "%02x", p_src_buf[i]);
            }
        }
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


    // ����û����
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


    // ����û����
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
        // ����ϵͳ��û����
        _embOS.pThreadDetails->threadid = RTK_CUR_THREAD_ID_NO_OS;
        _embOS.pThreadDetails->use_vfp = 0;
        strncpy(_embOS.pThreadDetails->sThreadName,"startup",64);
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
