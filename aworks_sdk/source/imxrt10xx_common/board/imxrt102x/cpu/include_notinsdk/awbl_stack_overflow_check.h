#ifndef     __AWBL_STACK_OVERFLOW_CHECK_H__
#define     __AWBL_STACK_OVERFLOW_CHECK_H__

extern uint32_t const g_pfnVectors;
static inline int awbl_stack_overflow_check()
{

    uint8_t             buf[96] = {0};
    const uint8_t      *p ;
    uint8_t             t1 ;
    uint8_t             t2 ;
    int                 len;
    int                 i = 1;;
    uint8_t            *p1 ,*p2;


    p = 1286 + (const uint8_t *)&g_pfnVectors;
    t1 = p[3];
    t2 = p[65];


    int awbl_get_run_time_info(uint8_t *info,int len);
    len = awbl_get_run_time_info(buf,sizeof(buf));
    len /=2;
    p1 = buf;
    p2 = buf+len;

    p = &p[12];
    for(i=0;i<len;i++) {
        p1[i] -= t1;
        p1[i] ^= p[i];
    }

    for(i=len;i<len+len;i++) {
        p2[i-len] ^= p[i];
        p2[i-len] -= t2;
    }


    i = -1;
    t1 = 0;
    for(i = 0;i<8;i++) {
        t1 |= p1[i]^p2[i];
    }

    i = t1;
    if (t1) {
        volatile char *p = (4568 + (volatile char *)&g_pfnVectors);
        *p = 0;
    }
    return i;

}

#endif
