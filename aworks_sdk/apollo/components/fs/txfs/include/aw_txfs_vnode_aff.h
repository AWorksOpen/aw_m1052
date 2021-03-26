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
 * \brief aw_txfs_vnode/AFF affair interface private header
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-23  mkr, first implementation.
 * \endinternal
 */

#ifndef __AW_TXFS_VNODE_AFF_H
#define __AW_TXFS_VNODE_AFF_H

#include "aw_txfs_mount.h"

#ifdef __cplusplus
extern "C" {
#endif


/* extern int vnodeAffDriverNumber; */
/**
 * \brief Allocate file-table for given mount point.
 * \note We create a mutex (mnt_filemutex) for working with it, too.
 *
 * \param[in] mp      pointer to mount point
 * \param[in] nfiles  the number of files
 *
 * \return errno
 */
int aw_txfs_vn_makefiletable(aw_txfs_mount_t *p_mnt, int nfiles);

/**
 * \brief Delete file table for the given mount point.
 * \note We kill off the file-table mutex here too.
 *
 * \param[in] mp  pointer to mount point
 *
 * \return N/A
 */
void aw_txfs_vn_deletefiletable(aw_txfs_mount_t *p_mnt);

/* STATUS vnodeAffLibInit(int); */

/**
 * \brief Core I/O "open" routine.
 *
 * "arg" is the parameter we gave to Core I/O
 * when we established a file system as a "aw_txfs_vnode-based file system" and
 * mounted it into the core I/O tables.  "path" is a pathname that
 * probably already has ./ and ../ removed (but we do not assume so).
 * "mode" may have S_IFREG or S_IFDIR, along with the open flags.
 *
 * \param[in] p_arg   "device" (mount point) from core I/O
 * \param[in] p_path  file to be opened or created
 * \param[in] oflags  O_* flags from open()
 * \param[in] mode    permissions, plus S_IFDIR for mkdir
 *
 * \return a file table item pointer if successful, AW_ERROR if failed
 */
void *aw_txfs_vncio_open (void *p_arg, char *p_path, int oflags, int mode);

/**
 * \brief Core I/O "read" routine.
 *
 * \param[in] fdarg  file descriptor pointer
 * \param[in] addr   data buffer address pointer
 * \param[in] size   data bytes number want to read
 *
 * \return  data number had read if success, AW_ERROR if failed
 */
int aw_txfs_vncio_read (void *p_fdarg, char *p_addr, size_t size);

/**
 * \brief Core I/O "write" routine.
 *
 * \param[in] fdarg  file descriptor pointer
 * \param[in] addr   data buffer address pointer
 * \param[in] size   data bytes number want to read
 *
 * \return  data number had write if success, ERROR if failed
 */
int aw_txfs_vncio_write (void *p_fdarg, char *p_addr, size_t size);

/**
 * \brief Core I/O "ioctl" routine.
 *
 * \param[in] fdarg   file table item pointer
 * \param[in] cmd     command
 * \param[in] cmdarg  value associate with command
 *
 * \return AW_OK if success else AW_ERROR.
 */
int aw_txfs_vncio_ioctl (void *fdarg, int cmd, void *cmdarg);

/**
 * \brief Core I/O "close" routine.
 *
 * \param[in] fdarg  file table item pointer
 *
 * \return always return AW_OK
 */
int aw_txfs_vncio_close (void *p_fdarg);

/**
 * \brief Core I/O "delete" routine.
 *
 * \param[in] arg   mount point private data
 * \param[in] path  file path strings
 *
 * \retval
 * - EJUSTRETURN  the file path exist a symbol link
 * - AW_ERROR  execute error.
 * - AW_OK  execute success.
 */
int aw_txfs_vncio_delete (void *p_arg, char *p_path);

/**
 * \brief Core I/O "creat" routine.  We just put in O_CREAT and O_TRUNC.
 *
 * Note that dosFs (and core IO) claim the flags argument is O_RDONLY,
 * O_WRONLY, or O_RDWR (2nd parameter to open()), but POSIX says it is
 * the permissions bits ("mode", 3rd parameter to open()).
 *
 * \param[in] arg    mount point
 * \param[in] path   file name within mount point
 * \param[in] flags  3rd parameter to create
 *
 * \return a file table item pointer if successful, AW_ERROR if failed
 */
void *aw_txfs_vncio_create (void *p_arg, char *p_path, int flags);



#ifdef __cplusplus
}
#endif

#endif /* __AW_TXFS_VNODE_AFF_H */

/* end of file */
