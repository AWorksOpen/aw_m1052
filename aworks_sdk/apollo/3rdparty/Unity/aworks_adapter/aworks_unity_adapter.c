#include "apollo.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
#include "aw_vdebug.h"

void unity_output_char_aworks(int c)
{
    char        t[2];

    t[0] = (char )c;
    t[1] = 0;

    aw_kprintf(t);
}

void UNITY_OUTPUT_FLUSH(void)
{
    
}