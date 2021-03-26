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
 * \brief 串口通讯驱动内部头文件
 *
 * \internal
 * \par modification history
 * - 1.00 12-10-29  orz, first implementation
 * -      18-10-15  hsg, add aw_serdev instead of aw_tydev
 * \endinternal
 */

#ifndef __AWBL_SERIAL_PRIVATE_H
#define __AWBL_SERIAL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbl_sio.h"
#include "aw_rngbuf.h"
#include "aw_sem.h"
#include "aw_list.h"

/**
 * \name serial driver configurations
 * @{
 */


/** @} */

/**
 * \addtogroup grp_aw_serial_if
 * @{
 */

/**
 * \brief awbl_serdev  串口设备结构描述
 */
struct awbl_serdev {
    struct aw_list_head nod;            /**< \brief 设备节点 */
    struct aw_rngbuf  rdbuf;            /**< \brief 读数据环形缓冲 */
    AW_MUTEX_DECL(wr_mutex_sem);        /**< \brief 写互斥信号量 */
    AW_MUTEX_DECL(rd_mutex_sem);        /**< \brief 读互斥信号量 */
    uint32_t    rd_timeout;             /**< \brief 读超时时间 */
    uint32_t    rd_interval_timeout;    /**< \brief 码间超时 */
    AW_SEMB_DECL(rdsync_sem);           /**< \brief 读同步信号量 */
};


/* data types */
struct awbl_serial {
    struct awbl_serdev          serdev;
    struct aw_sio_chan          sio_chan;
    struct aw_serial_dcb        dcb;    /**< \brief 串口设备配置 */
    uint8_t                     no;     /**< \brief 设备号 */
};


aw_err_t awbl_serial_create (struct awbl_serial      *p_ser,
                             char                    *p_recv_buf,
                             uint32_t                 recv_size,
                             int                      no,
                             struct aw_sio_chan      *p_sio_chan);

void awbl_serial_rx (struct awbl_serial *p_ser, const char inchar);
void awbl_serial_int_idle (struct awbl_serial *p_ser);
int awbl_serial_recv_is_empty(struct awbl_serial *p_ser);
uint32_t awbl_serial_recv_freebytes(struct awbl_serial *p_ser);

static aw_inline void awbl_serial_rx_puts (struct awbl_serial *p_ser, char *p_buf, size_t nbyte)
{
    aw_rngbuf_put(&p_ser->serdev.rdbuf, p_buf, nbyte);
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_SERIAL_PRIVATE_H */

/* end of file */
