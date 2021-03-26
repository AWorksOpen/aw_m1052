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
 * \brief Ethernet driver Interface for LWIP.
 *
 * \internal
 * \par History
 * - 14-12-15, afm, First implementation.
 * \endinternal
 */

/* Visible debug configurations */

/******************************************************************************/
#include "apollo.h"
#include "awbus_lite.h"
#include "aw_assert.h"
#include "aw_task.h"
#include "aw_msgq.h"
#include "aw_errno.h"
#include "aw_serial.h"
#include "aw_delay.h"

#include "aw_vdebug.h"

#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/sio.h"

#include "ppp/sio_mgn.h"

#include <string.h>

/**
 * Reads from the serial device.
 *
 * \param fd serial device handle
 * \param data pointer to data buffer for receiving
 * \param len maximum length (in bytes) of data to receive
 * \return number of bytes actually received - may be 0 if aborted by 
 *         sio_read_abort
 *
 * \note This function will block until data can be received. The blocking
 * can be cancelled by calling sio_read_abort().
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
    struct sio_mgn *p_sio = NULL;
    uint32_t err = 0;
    int per_len = 0, cur_len = 0;

    p_sio = sio_list_iterator((int)fd);

    while(1) {

        err = sys_arch_sem_wait(&p_sio->serial_abort_sem, 10);

        if (err != SYS_ARCH_TIMEOUT) {

            return 0;
        }

        if (p_sio->serial_cmd_state == AW_TRUE) {

            continue;
        }

        cur_len = p_sio->pfn_sio_read((void *)fd, data, len);
        if (cur_len > 5) {
            p_sio->flow_statistic_download += cur_len - 5;    /*  统计流量    */
        }

        return cur_len;
    }
    return 0;
}

/**
 * Writes to the serial device.
 *
 * \param fd serial device handle
 * \param data pointer to data to send
 * \param len length (in bytes) of data to send
 * \return number of bytes actually sent
 *
 * \note This function will block until all data can be sent.
 */
u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len)
{
    ssize_t res;

    struct sio_mgn *p_sio = NULL;
    p_sio = sio_list_iterator((int)fd);

    if (p_sio->serial_cmd_state == AW_TRUE) {
        return 0;
    }

    res = p_sio->pfn_sio_write((void *)fd, data, len);
    if (res > 5) {
        p_sio->flow_statistic_upload += len-5;        /*  统计流量    */
        return res;
    } else {
        return 0;
    }
}

/**
 * Aborts a blocking sio_read() call.
 *
 * \param fd serial device handle
 */
void sio_read_abort(sio_fd_t fd)
{
    struct sio_mgn *p_sio = NULL;

    p_sio = sio_list_iterator((int)fd);

    sys_sem_signal(&p_sio->serial_abort_sem);
}

/* end of file */
