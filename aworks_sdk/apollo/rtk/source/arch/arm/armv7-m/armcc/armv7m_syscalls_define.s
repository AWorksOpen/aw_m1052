
                PRESERVE8
                THUMB

#include "rtk_config.h"

; �ɱ��ⲿ���õĺ�������
    MACRO
$label  __FUNC_DECL
$label  PROC
    EXPORT  $label
    MEND

; ����ʹ�õĺ�������
    MACRO
$label  __LOCAL_FUNC_DECL
$label  PROC
    MEND

; ��������
    MACRO
        __FUNC_END
    ENDP
    MEND

                AREA    |.text|, CODE, READONLY

;����ʵ�� void armv7m_exec_syscalls();
armv7m_exec_syscalls __FUNC_DECL

#if CONFIG_RTK_USERMODE  
    IMPORT  rtk_get_syscalls_function_from_number

    push    {lr}

    ; ��ʱr0Ϊϵͳ���ñ��,��ϵͳ���ñ�Ż�ȡ�������ں˸�����
    bl      rtk_get_syscalls_function_from_number
    pop     {lr}
    ; ���rtk_get_syscalls_function_from_number����NULL���򷵻�-AW_ENOSYS
    cmp     r0, #0
    it      ne
    bne     syscall_number_correct
    ;  �������
    pop     {r0-r3}
    ldr     r0, =-71
    bx      lr
syscall_number_correct
    ;  ��ʵ���ں˺�����ַѹջ
    push    {r0};

    ;  �˿�sp��ʼ�Ķ�ջֵӦ����pc,r0,r1,r2,r3
    ;  ������Щ�Ĵ���֮��,sp�պö�Ӧ��Ӧ�õ���syscallʱ�Ĳ�����ջ
    ;  ��ʱ�ں�sys�������û�syscall�����Ĳ�������ȫһ����
    ;  ������ε�����Щ�Ĵ���ȴ�ǳ����Σ�
    ;  ��Ϊ��ջ˳�������r0,r1,r2,r3,pc��������pop {r0-r3,pc},һ��ָ���

    ;  �������һ�ε���������ȵ���pc����r0-r3��ֵ��δ������
    ;  pc���Ѿ���ת��sys_������c�������ˣ���ʱsys_ϵ��c���������Ĳ������û�����Ĳ�����һ��
    ;  �����ldr��ȡ��r0-r3����ʱ����ȵ���pc����sp��ֵΪ�ı����ת��c������������һ��
    ;       ������޸�sp��ֵ����ʹ��ldr����ȡpc������������޸�sp��ֵ֮��ͻȻ�����жϣ�
    ;           ����Ϊpc�ı���λ���Ѿ���spջ��֮���ˣ��жϵ��Զ�ѹջ��ʹ��pc����λ�õ�ֵ���޸�
    ;           ʹ��ldr��ȡ��pc�Ͳ���ȷ
    ;               ��Ȼ��������޸�sp֮ǰ�Ƚ����жϣ����ڻ�ȡpc��ֵ֮ǰ���������жϣ�����
    ;                   ��������
    ;               ���ǻ�ȡpc��ֵ֮�󣬾��Ѿ���ת��c�������ˣ���ʱ�ͱ�����c�����������ж�
    ;                   �������鷳��

    ;  ����������һ��ָ��ͬʱ����pc���޸�sp��ֵ����ȷ��λ���Ǳ���ģ�
    ;  ��ʱ������ͨ������ջ�еļĴ���˳��ʹ�ÿ���ʹ��pop {r0-r3,pc}��������в���

    ;  ����r0-r3
    ldr         r1, [sp, #4]
    str         r1, [sp, #0]

    ldr         r1, [sp, #8]
    str         r1, [sp, #4]

    ldr         r1, [sp, #12]
    str         r1, [sp, #8]

    ldr         r1, [sp, #16]
    str         r1, [sp, #12]
    ;  ��ѹ��pc
    str         r0, [sp, #16]

    ;  �ָ�����ת��sysϵ��c����
    pop {r0-r3, pc}

#else 
    b .
#endif
    __FUNC_END

    END
