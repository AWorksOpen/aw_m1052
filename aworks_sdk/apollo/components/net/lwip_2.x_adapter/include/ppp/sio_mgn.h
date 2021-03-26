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
 * \brief PPP SIO
 *
 * \internal
 * \par modification history:
 * - 1.00 15-01-12  ops, first implementation.
 * \endinternal
 */
#ifndef SIO_MGN_H_
#define SIO_MGN_H_

#include "aw_ioctl.h"
#include "aw_list.h"

#include "arch/sys_arch.h"

struct sio_mgn {
    /* int                  serial_fd; */
    void *fd;

    u32_t (*pfn_sio_write)(void *fd, u8_t *data, u32_t len);
    u32_t (*pfn_sio_read)(void *fd, u8_t *data, u32_t len);

    sys_sem_t serial_abort_sem;

    uint8_t serial_cmd_state;

    uint32_t flow_statistic_download; /** byte */
    uint32_t flow_statistic_upload;   /** byte */

    struct aw_list_head elm;
};

struct sio_mgn *sio_list_iterator(int serial_fd);

void sio_list_init(void);

void sio_list_add(struct sio_mgn *p_sio);

void sio_list_del(struct sio_mgn *p_sio);

#endif /* SIO_MGN_H_ */

/* end of file */

