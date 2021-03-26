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
 * \brief TXFS uio interface.
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-23  mkr, first implementation.
 * \endinternal
 */

#ifndef __AW_TXFS_UIO_H
#define __AW_TXFS_UIO_H

#include "aw_txfs_types.h"

/******************************************************************************/
#define AW_TXFS_UIO_READ      (0) /**< \brief Read operation */
#define AW_TXFS_UIO_WRITE     (1) /**< \brief Write operation */
/******************************************************************************/
/** \brief iovec structure */
struct aw_txfs_iovec {
    
    /** \brief start address, point to data buffer based address */
    char   *iov_base;  
    size_t  iov_len;   /**< \brief size of the io operate */
    size_t  iov_index; /**< \brief index of io operate */
};

/** \brief uio struct */
struct aw_txfs_uio {
    /** \brief pointer to struct aw_txfs_iovec structure */
    struct aw_txfs_iovec *uio_iov;

    uint8_t   uio_iovcnt; /**< \brief the numbers of uio_iov */
    uint64_t  uio_offset; /**< \brief offset of the current pos */
    size_t    uio_resid;  /**< \brief the remain byte numbers */
    
    /** \brief memory segment flags, user or system memory */
    uint32_t  uio_segflg; 
    uint32_t  uio_rw;     /**< \brief aw_txfs_uio op, write or read */
    void     *uio_procp;
};
/******************************************************************************/
/**
 * \brief copy num data between p_uio and p_data
 *
 * \param[in] p_data  destination data buffer
 * \param[in] num     number data in bytes
 * \param[in] p_uio   pointer to aw_txfs_uio
 *
 * \return copied data number
 */
size_t aw_txfs_uiomove (char *p_data, size_t num, struct aw_txfs_uio *p_uio);

#endif /* __AW_TXFS_UIO_H */

/* end of file */
