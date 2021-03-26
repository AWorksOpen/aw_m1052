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
 * \brief segger rtt shell impl
 *
 * \internal
 * \par modification history:
 * - 1.00 18-08-29, phd, First implementation
 * \endinternal
 */

#include "apollo.h"

#include "aw_task.h"

#include "aw_shell_segger_rtt.h"

#include "SEGGER_RTT.h"

#include <stdio.h>
#include <string.h>

static int io_puts(struct aw_shell_user *sh, const char *s)
{
    struct aw_shell_segger_rtt_dev *dev = (struct aw_shell_segger_rtt_dev *) sh->user_arg;
    return SEGGER_RTT_Write(dev->buff_id, s, strlen(s));
}

static int io_putchar(struct aw_shell_user *sh, int ch)
{
    struct aw_shell_segger_rtt_dev *dev = (struct aw_shell_segger_rtt_dev *) sh->user_arg;
    char                            buf = ch;
    return SEGGER_RTT_Write(dev->buff_id, &buf, 1);
}

static int io_getchar(struct aw_shell_user *sh)
{
    struct aw_shell_segger_rtt_dev *dev = (struct aw_shell_segger_rtt_dev *) sh->user_arg;
    uint8_t                         ch  = 0;
    unsigned                        len;

    len = SEGGER_RTT_Read(dev->buff_id, &ch, 1);
    if (0 == len) {
        aw_task_delay(10);
        return -AW_ENODATA;
    }

    return ch;
}

static int io_try_getchar(struct aw_shell_user *sh)
{
    struct aw_shell_segger_rtt_dev *dev = (struct aw_shell_segger_rtt_dev *) sh->user_arg;
    uint8_t                         ch  = 0;
    unsigned                        len;

    len = SEGGER_RTT_Read(dev->buff_id, &ch, 1);
    if (0 == len) {
        return -AW_ENODATA;
    }

    return ch;
}

int aw_shell_segger_rtt_run(int arg)
{
    struct aw_shell_segger_rtt_dev *dev = (struct aw_shell_segger_rtt_dev *) arg;

    return aw_shell_server(&dev->sc, &dev->sh);
}

aw_err_t aw_shell_segger_rtt_init(struct aw_shell_segger_rtt_dev *dev,
                                  unsigned                        buff_id,
                                  const char *                    user,
                                  struct aw_shell_opt *           p_shell_opt)
{

    memset(&dev->sh,0,sizeof(dev->sh));
    dev->sh.io_puts  = io_puts;
    dev->sh.io_putc  = io_putchar;
    dev->sh.io_getc  = io_getchar;
    dev->sh.io_try_getc = io_try_getchar;
    dev->sh.user_arg = dev;
    dev->buff_id     = buff_id;

    (void) aw_default_shell_set(&dev->sh);

    aw_shell_init(&dev->sc, &dev->sh, p_shell_opt);

    return aw_task_startup(dev->tid);
}
