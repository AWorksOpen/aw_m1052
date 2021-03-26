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
 * \brief kernel output functions
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-23  orz, first implementation
 * \endinternal
 */

#include "aw_common.h"
#include "aw_koutput.h"
#include "aw_sem.h"

/**
 * \addtogroup aw_koutput
 * @{
 */

/*******************************************************************************
    kernel output functions
*******************************************************************************/
void  *gp_kout_file                            = NULL;
int  (*gpfunc_kputc)(const char  c, void *fil) = NULL;
int  (*gpfunc_kputs)(const char *s, void *fil) = NULL;
AW_MUTEX_IMPORT(_g_kprintf_mutex);


/**
 * \brief set kernel output functions
 *
 * \param fil       The handle for f_putc and f_puts functions
 * \param f_putc    Pointer to function to output a char
 * \param f_puts    Pointer to function to output a string
 *
 * \note There is no locks when setting the output functions, and there
 *       is no locks in aw_kprintf() too, so use this functions and aw_kprintf
 *       carefully.
 */
void aw_koutput_set (void  *fil,
                     int  (*f_putc) (const char  c, void *fil),
                     int  (*f_puts) (const char *s, void *fil))
{
    if (NULL != gp_kout_file ||
            NULL != gpfunc_kputc ||
            NULL != gpfunc_kputs ) {
        return;
    }
    gp_kout_file = fil;
    gpfunc_kputc = f_putc;
    gpfunc_kputs = f_puts;
    AW_MUTEX_INIT(_g_kprintf_mutex,AW_SEM_Q_PRIORITY);
}

/* @} */

/* end of file */

