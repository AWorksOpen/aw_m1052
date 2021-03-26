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
 * \brief ÍøÂç°ü¹ÜÀí
 *
 * \internal
 * \par Modification History
 * - 1.00 18-04-14  phd, first implementation
 * \endinternal
 */

#ifndef __AW_NET_BUF_H
#define __AW_NET_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_xx
 * \copydoc aw_net_buf.h
 * @{
 */

typedef void * (*aw_net_buf_cpy_t) (void *dest, const void *src, size_t len);

/**
 * Return actual payload size
 *
 * @param  p_buf Memory chain
 * @return     Size in bytes
 */
size_t aw_net_buf_slice_len (aw_net_buf_t *p_buf);

/**
 * Return total length of the memory chain
 *
 * @param  p_buf Memory chain
 * @return       Chain length
 */
size_t aw_net_buf_chain_len (aw_net_buf_t *p_buf);

void *aw_net_buf_slice_peek (aw_net_buf_t *p_buf);

void *aw_net_buf_slice_pop (aw_net_buf_t **p_buf);

size_t aw_net_buf_copy_partial (aw_net_buf_t *p_buf,
                                void         *p_mem,
                                size_t        len,
                                size_t        offset);

size_t aw_net_buf_copy_partial_custom (aw_net_buf_t     *p_buf,
                                       aw_net_buf_cpy_t  pfn_cpy,
                                       void             *p_mem,
                                       size_t            len,
                                       size_t            offset);

/** @} grp_aw_if_xx */

#ifdef __cplusplus
}
#endif

#endif

/* end of file */
