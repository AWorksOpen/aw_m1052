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
 * \brief block device
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-25  deo, first implementation
 * \endinternal
 */

#ifndef __AW_BLK_DEV_H
#define __AW_BLK_DEV_H


#ifdef __cplusplus
extern "C" {
#endif

#include "aw_types.h"
#include "aw_sem.h"
#include "aw_list.h"
#include "aw_refcnt.h"

#define AW_BD_NAME_MAX    32



/******************************************************************************/

/**
 * \name block device ioctl commands
 * @{
 */
/** \brief flush block device*/
#define AW_BD_SYNC            0xbd000001
/** \brief eject and raise soft insert event */
#define AW_BD_SOFT_EJECT      0xbd000002
/** \brief eject and raise insert event */
#define AW_BD_HARD_EJECT      0xbd000003
/** \brief retrieve name of base block device */
#define AW_BD_GET_BASE_NAME   0xbd000004
/** \brief request block device test its validity */
#define AW_BD_TEST            0xbd000005
/** \brief block device stack is initialized */
#define AW_BD_STACK_COMPLETE  0xbd000006
/** \brief obtain block device information */
#define AW_BD_GET_ATTRIBUTE   0xbd000007
/** \brief obtain block size */
#define AW_BD_GET_BSIZE       0xbd000008
/** \brief obtain block counts */
#define AW_BD_GET_BLKS        0xbd000009

/** @} */

#define AW_BD_ARRT_READ_ONLY  0x01
#define AW_BD_ARRT_REMOVABLE  0x02


#define AW_BD_ARQ_READ      0x01  /**< \brief read blocks */
#define AW_BD_ARQ_WRITE     0x00  /**< \brief write to blocks */

#define AW_BD_EVT_INJECT   0x01
#define AW_BD_EVT_EJECT    0x02


/** \brief access request packet */
struct aw_blk_arp {
    struct aw_blk_dev   *p_dev;      /**< \brief the owner of this block_io */
    uint_t               blk_no;     /**< \brief the block number of this block_io */
    uint_t               nbytes;     /**< \brief total number of bytes in this segment */
    void                *p_data;     /**< \brief data pointer */
    uint_t               resid;      /**< \brief residual bytes for this segment */
    int                  error;      /**< \brief error code for this segment */
    uint_t               flags;      /**< \brief flags. BLOCK_IO_READ / BLOCK_IO_WRITE */
    void                *priv;       /**< \brief caller supplied data */

    /** \brief completion function for this segment */
    void               (*pfn_done) (struct aw_blk_arp *p_arp);
    struct aw_blk_arp   *p_next;     /**< \brief the next segment or NULL */
};




/** \brief block device */
struct aw_blk_dev {
    /**< \brief device name */
    char                    name[AW_BD_NAME_MAX];
    AW_MUTEX_DECL(          lock);      /**< \brief device lock*/
    struct aw_blk_dev      *p_next;     /**< \brief next device */
    struct aw_blk_cache    *p_cache;    /**< \brief cache support */
    struct aw_blk_dev      *p_base;     /**< \brief next device */
    struct aw_refcnt        ref;        /**< \brief reference count */
    uint8_t                 status;
#define AW_BD_STA_REMOVED   0x01
#define AW_BD_STA_EJECTED   0x02
#define AW_BD_STA_RESERVED  0x04

#define AW_BD_INJECTED(p_bd) \
    (((p_bd)->status & AW_BD_STA_EJECTED) == 0)
#define AW_BD_EJECTED(p_bd) \
    (((p_bd)->status & AW_BD_STA_EJECTED) == AW_BD_STA_EJECTED)
#define AW_BD_REMOVED(p_bd) \
    (((p_bd)->status & AW_BD_STA_REMOVED) == AW_BD_STA_REMOVED)
#define AW_BD_VALID(p_bd) ((p_bd)->status == 0)

    /**< \brief send control command to device */
    int (*pfn_ioctl) (struct aw_blk_dev *p_dev,
                  int                cmd,
                  void              *p_arg);

    /**< \brief access request */
    int (*pfn_areq) (struct aw_blk_dev *p_dev,
                 struct aw_blk_arp *p_arp);

    /**< \brief destroy device call back from  */
    void (*pfn_release) (struct aw_blk_dev *p_dev);
};




/** \brief block cache */
struct aw_blk_cache {
    struct aw_blk_dev        *p_bd;
    struct aw_list_head      *hash;
    struct aw_list_head       lru;
    uint_t                    n_hashs;
    uint_t                    bsize;
    uint_t                    nblks;
    uint_t                    n_nodes;    /**< \brief number of nodes */
    uint_t                    b_blks;
    uint8_t                  *p_bbuf;     /**< \brief bust buffer */
    struct aw_list_head       sync;
    AW_MUTEX_DECL(            lock);
};


typedef void (*aw_blk_dev_evt_fun_t)(const char  *name,
                                     int          event,
                                     void        *p_arg);




/** \brief initialize the block device lib */
aw_err_t aw_blk_dev_lib_init (int blk_evt_cnt);

/** \brief create a block device */
aw_err_t aw_blk_dev_create (struct aw_blk_dev  *p_dev,
                            const char         *name);

/** \brief destroy a block device */
aw_err_t aw_blk_dev_destroy (struct aw_blk_dev *p_dev);

/** \brief inject a block device into system */
aw_err_t aw_blk_dev_inject (struct aw_blk_dev *p_dev);

/** \brief eject a block device */
aw_err_t aw_blk_dev_eject (struct aw_blk_dev *p_dev);

/** \brief get a block device by name */
struct aw_blk_dev *aw_blk_dev_get (const char *name);

/** \brief put a block device by name */
aw_err_t aw_blk_dev_put (struct aw_blk_dev *p_dev);

/** \brief lock block device */
aw_err_t aw_blk_dev_lock (struct aw_blk_dev *p_dev);

/** \brief unlock block device */
aw_err_t aw_blk_dev_unlock (struct aw_blk_dev *p_dev);

/** \brief block device ioctl routine */
aw_err_t aw_blk_dev_ioctl (struct aw_blk_dev *p_dev,
                           int                cmd,
                           void              *p_arg);

/** \brief block device access request */
aw_err_t aw_blk_dev_areq (struct aw_blk_dev *p_dev,
                          struct aw_blk_arp *p_arp);

/** \brief retrieve the block device name */
aw_err_t aw_blk_dev_name (struct aw_blk_dev *p_dev,
                          const char        *name);

/** \brief wait for block device insert */
aw_err_t aw_blk_dev_wait_install (const char *p_name, int timeout);

/** \brief register a block device event */
aw_err_t aw_blk_dev_event_register (aw_blk_dev_evt_fun_t  pfn,
                                    void                 *p_arg,
                                    int                  *p_idx);

/** \brief unregister a block device event */
aw_err_t aw_blk_dev_event_unregister (int idx);


/** \brief create cache */
aw_err_t aw_blk_dev_cache_create (const char          *name,
                                  struct aw_blk_cache *p_cache,
                                  void                *p_buf,
                                  uint_t               size,
                                  uint_t               b_blks);

/** \brief destroy cache */
aw_err_t aw_blk_dev_cache_destroy (struct aw_blk_cache *p_cache);

#ifdef __cplusplus
}
#endif

#endif /* __AW_BLK_DEV_H */
