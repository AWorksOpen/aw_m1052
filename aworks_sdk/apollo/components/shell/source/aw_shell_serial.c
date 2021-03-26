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
 * \brief serial shell impl
 *
 * \internal
 * \par History
 * - 1.01 16-01-07, deo, add user create.
 * - 1.00 15-04-29, rnk, First implementation.
 * @endinternal
 */
#include "apollo.h"

#include "aw_ioctl.h"
#include "aw_serial.h"
#include "aw_task.h"

#include "aw_shell_serial.h"
#include "aw_serial.h"
#include <stdio.h>
#include <string.h>

/******************************************************************************/
static int io_puts(struct aw_shell_user *sh, const char *s)
{
    struct aw_serial_shell_dev *dev = (struct aw_serial_shell_dev *) sh->user_arg;
    int                         len;

    return aw_serial_write(dev->com, s, strlen(s));
}

static int io_putchar(struct aw_shell_user *sh, int ch)
{
    struct aw_serial_shell_dev *dev = (struct aw_serial_shell_dev *) sh->user_arg;
    char                        buf = ch;
    int                         len = 0;

    while (1) {
        len = aw_serial_write(dev->com, &buf, 1);
        if (len == 1) {
            return ch;
        }
    }
    return -AW_ENODATA;
}

static int io_getchar(struct aw_shell_user *sh)
{
    struct aw_serial_shell_dev *dev = (struct aw_serial_shell_dev *) sh->user_arg;
    char                        ch  = 0;
    int                         len = 0;

    while (1) {
        len = aw_serial_read(dev->com, &ch, 1);
        if (len == 1) {
            return ch;
        }
    }

    return -AW_ENODATA;
}

static int io_try_getchar(struct aw_shell_user *sh)
{
    struct aw_serial_shell_dev *dev = (struct aw_serial_shell_dev *) sh->user_arg;
    char                        ch  = 0;
    int                         len = 0;
    aw_err_t                    err;

    struct aw_serial_timeout    timeout_old,timeout_new;

    len = aw_serial_read(dev->com, &ch, 1);
    if (0 == len) {
        return -AW_ENODATA;
    } else if (len < 0) {
        return len;
    }
    return ch;
}

/******************************************************************************/
int aw_serial_shell_run(int arg)
{
    struct aw_serial_shell_dev *dev = (struct aw_serial_shell_dev *) arg;

    return aw_shell_server(&dev->sc, &dev->sh);
}

aw_err_t aw_serial_shell_init(struct aw_serial_shell_dev *dev,
                              int                         com,
                              const char *                user,
                              struct aw_shell_opt *       p_shell_opt)
{
    struct aw_serial_timeout timeout_cfg = {0};
    aw_err_t     err = AW_OK;
    dev->sh.io_puts  = io_puts;
    dev->sh.io_putc  = io_putchar;
    dev->sh.io_getc  = io_getchar;
    dev->sh.io_try_getc = io_try_getchar;
    dev->sh.user_arg = dev;
    dev->com         = com;

    err = aw_serial_timeout_get(com, &timeout_cfg);

    if (err) {
        return err;
    }

    /* 设置读超时  */
    timeout_cfg.rd_timeout          = aw_ms_to_ticks(50);

    /* 设置码间超时为100ms */
    timeout_cfg.rd_interval_timeout = 100;
    err = aw_serial_timeout_set(com, &timeout_cfg);

    if (err) {
        return err;
    }

    /* 默认shell */
    (void) aw_default_shell_set(&dev->sh);
    aw_shell_init(&dev->sc, &dev->sh, p_shell_opt);

    return aw_task_startup(dev->tsk);
}

/* end of file */
