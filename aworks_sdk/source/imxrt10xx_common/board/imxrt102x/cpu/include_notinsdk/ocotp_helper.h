#ifndef         __OCOTP_HELPER_H__
#define         __OCOTP_HELPER_H__

static void inline ocotp_reg_read(uint8_t *data,volatile uint32_t *base,int count)
{
    uint32_t           *p;
    int                 i;

    p = (uint32_t *)data;
    for(i = 0;i<count;i++) {
        *p = *base;
        p ++;
        base += 4;
    }
}


#endif
