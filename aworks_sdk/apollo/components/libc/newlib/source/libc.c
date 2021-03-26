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
 * \brief 
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-26 orz, first implementation.
 * \endinternal
 */
 
/* #include <rtthread.h> */
#include "apollo.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include "libc.h"

#ifdef RT_USING_PTHREADS
#include <pthread.h>
#endif

void libc_system_init(const char* tty_name)
{
#ifdef RT_USING_DFS
    int fd;

#ifndef RT_USING_DFS_DEVFS
#error Please enable devfs by defining RT_USING_DFS_DEVFS in rtconfig.h
#endif

    /** \brief init console device */
    rt_console_init(tty_name);

    /** \brief open console as stdin/stdout/stderr */
    fd = open("/dev/console", O_RDONLY, 0);    /**< \brief for stdin */
    fd = open("/dev/console", O_WRONLY, 0);    /**< \brief for stdout */
    fd = open("/dev/console", O_WRONLY, 0);    /**< \brief for stderr */
#endif

    /* set PATH and HOME */
    putenv("PATH=/");
    putenv("HOME=/");

#ifdef RT_USING_PTHREADS
    pthread_system_init();
#endif
}
