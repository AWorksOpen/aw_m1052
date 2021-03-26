#include "apollo.h"                     /* 此头文件必须被首先包含 */
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