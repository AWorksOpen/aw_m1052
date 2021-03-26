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

/**
 * \file
 * \brief serial I/O for kernel output
 *
 * \internal
 * \par modification history:
 * - 1.01 12-11-26  orz, fix aw_koutput_sio_init() with SIO_BAUD_SET,
 *                  add line feed
 * - 1.00 12-11-23  orz, first implementation
 * \endinternal
 */

#include "aw_koutput.h"
#include "awbl_sio.h"
#include "aw_serial.h"
#include "aw_int.h"

/** \brief out a char into serial device */
static int __serial_putc (const char  c, void *fil)
{
    if( AW_INT_CONTEXT() ) {
        return 1;
    }

    if (AW_FAULT_CONTEXT()) {
        /* line feed */
        if ('\n' == c) {
            const char temp = '\r';
            while( 1 != aw_serial_poll_write((uint32_t)fil, &temp, 1));
        }
        while( 1 != aw_serial_poll_write((uint32_t)fil, &c, 1));
    }
    else {
        /* line feed */
        if ('\n' == c) {
            const char temp = '\r';
            while( 1 != aw_serial_write((uint32_t)fil, &temp, 1));
        }
        while( 1 != aw_serial_write((uint32_t)fil, &c, 1));
    }




    return 1;
}

/** \brief out a string into serial device */
static int __serial_puts (const char *s, void *fil)
{
    const char *ss;
    char        c;

    for (ss = s; (c = *ss) != '\0'; ss++) {
        (void)__serial_putc(c, fil);
    }

    return (ss - s);
}

/**
 * \brief Initialize serial I/O as kernel output
 *
 * \param com  serial device number
 * \param baud serial device baud rate
 */
void aw_koutput_sio_init (int com, int baud)
{
    if (com >= 0) {
        aw_serial_ioctl(com, SIO_BAUD_SET, (void *)baud);

        aw_koutput_set((void *)com, __serial_putc, __serial_puts);
    }
}

/* end of file */

