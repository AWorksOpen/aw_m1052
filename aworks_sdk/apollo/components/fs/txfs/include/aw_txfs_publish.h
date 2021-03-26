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
 * \brief Transaction File System interfaces
 *
 * \internal
 * \par modification history:
 * - 1.00 17-07-06  mkr, first implementation
 * \endinternal
 */

#ifndef __AW_TXFS_H
#define __AW_TXFS_H

#include "aw_txfs_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief define the index of TM field about FSB/FIB */
#define AW_TXFS_TM_BLK_FSB  0 /**< \brief Index of TM field about FSB */
#define AW_TXFS_TM_BLK_FIB  1 /**< \brief Index of TM field about FIB */

/**
 * \brief Identifiers and version numbers
 * @{
 */
/** \brief TXFS id string */
#define AW_TXFS_ID_STRING       "WR_TXFS"

/** \brief TXFS major version 1 */
#define AW_TXFS_MAJOR_VERSION1  0x01

/** \brief TXFS major version */
#define AW_TXFS_MAJOR_VERSION   AW_TXFS_MAJOR_VERSION1

/** \brief TXFS minor version 1 */
#define AW_TXFS_MINOR_VERSION1  0x01

/** \brief TXFS minor version 2 */
#define AW_TXFS_MINOR_VERSION2  0x02

/** \brief TXFS minor version */
#define AW_TXFS_MINOR_VERSION   AW_TXFS_MINOR_VERSION2

/** \brief TXFS inode version 1 */
#define AW_TXFS_INODE_VERSION1  0x00

/** \brief TXFS inode version 2 */
#define AW_TXFS_INODE_VERSION2  0x01

/** \brief TXFS inode version */
#define AW_TXFS_INODE_VERSION   AW_TXFS_INODE_VERSION2
/** @} */

/** \brief defines a character that is not allowed in file/directory name */
#define AW_TXFS_BAD_CHAR    (':')


/**
 * \brief Limits for the logical block sizes
 * @{
 */
/** \brief The minium supported logical block size in bytes */
#define AW_TXFS_MIN_LOGICAL_BLK_SIZE     512
/** \brief The minium supported logical block size in bytes as a power of 2 */
#define AW_TXFS_MIN_LOGICAL_BLK_SIZE_2   9
/** \brief The maximum supported logical block size in bytes */
#define AW_TXFS_MAX_LOGICAL_BLK_SIZE     32768
/** \brief The maximum supported logical block size in bytes as a power of 2 */
#define AW_TXFS_MAX_LOGICAL_BLK_SIZE_2   15
/* @} */

/**
 * \brief Limits for the physical block sizes
 * @{
 */

/** \brief The minium supported logical block size in bytes */
#define AW_TXFS_MIN_PHYSICAL_BLK_SIZE    512
/** \brief The minium supported logical block size in bytes as a power of 2 */
#define AW_TXFS_MIN_PHYSICAL_BLK_SIZE_2  9
/** \brief The maximum supported logical block size in bytes */
#define AW_TXFS_MAX_PHYSICAL_BLK_SIZE    AW_TXFS_MAX_LOGICAL_BLK_SIZE
/** \brief The maximum supported logical block size in bytes as a power of 2 */
#define AW_TXFS_MAX_PHYSICAL_BLK_SIZE_2  AW_TXFS_MAX_LOGICAL_BLK_SIZE_2
/* @} */

/** \brief minimum # of buffers for TXFS devices */
#define AW_TXFS_DEV_MIN_BUFFERS          16

/**
 * \brief error codes
 * @{
 */
/** \brief TXFS 起始错误号 */
#define AW_TXFSLIB_ERR                       (195)

/** \brief 磁盘满 */
#define AW_TXFSLIB_ERR_DISK_FULL             (AW_TXFSLIB_ERR | 1)
/** \brief inode 已使用完，空闲iNode个数为0 */
#define AW_TXFSLIB_ERR_OUT_OF_INODES         (AW_TXFSLIB_ERR | 2)
/** \brief  */
#define AW_TXFSLIB_ERR_INVALID_TMR           (AW_TXFSLIB_ERR | 3)
/** \brief 磁盘(卷)格式错误 */
#define AW_TXFSLIB_ERR_UNKNOWN_VOLUME_FORMAT (AW_TXFSLIB_ERR | 4)
/** \brief 未找到磁盘块 */
#define AW_TXFSLIB_ERR_BLOCK_NOT_FOUND       (AW_TXFSLIB_ERR | 5)
/** \brief inode journal full */
#define AW_TXFSLIB_ERR_INODE_JOURNAL_FULL    (AW_TXFSLIB_ERR | 6)
/** \brief 事务传输起始保护失败 */
#define AW_TXFSLIB_ERR_PROTECT_BEGIN_FAIL    (AW_TXFSLIB_ERR | 7)
/* @} */

/**
 * \brief flags used by hrfsChkDsk
 * @{
 */
/** \brief no check */
#define AW_TXFS_CHKDSK_FLAG_NONE                 0x00

/** \brief check and upgrade */
#define AW_TXFS_CHKDSK_FLAG_UPGRADE              0x01

/** \brief rewind inode journal */
#define AW_TXFS_CHKDSK_FLAG_REWIND_INODE_JOURNAL 0x02
/** @} */


/** \brief typedef for the fs */
typedef struct aw_txfs_dev      *aw_txfs_dev_id;
typedef struct hrfs_volume_desc *aw_txfs_volume_desc_id;



extern aw_err_t (*hrfsFmtRtn)(char    *path, 
                              uint64_t diskSize, 
                              uint32_t blockSize,
                              uint32_t files, 
                              uint32_t majorVer, 
                              uint32_t minorVer,
                              uint32_t options);
/**
 *
 * \brief format the TXFS file system using advanced options
 * \note This routine formats a disk or partition referenced by the path to
 *       the media.
 *
 * \param[in] p_dev  the physical block device object pointer
 *
 * \param[in] disk_size  size of disk in bytes, This argument is used to
 *                       specify how many bytes of the media the TXFS file
 *                       system should occupy. It can be used to prevent TXFS
 *                       for using the end portion of the media. In general
 *                       this value should be 0 to specify that the entire
 *                       media is to be used.
 *
 * \param[in] blk_size  size of block in bytes, This parameter is used to
 *                      specify what block size, in byte, TXFS should use.
 *                      This block size must be a power of 2, greater than
 *                      the physical sector size, and be within 512 to 8196
 *                      bytes inclusively. In general this value should be
 *                      specified as 0 so the formatter can determine the most
 *                      efficient block size to use for the media size.
 *
 * \param[in] num_inodes  number of Inodes, This parameter is used
 *                        to specify the absolute maximum number of files and
 *                        directories the file system can ever have. Note this
 *                        does not include the root directory which the
 *                        formatter creates automatically. Specifying a value
 *                        of 0 will tell the TXFS formatter to allow for the
 *                        maximum number of files/directories the file system
 *                        can have based on the amount of data blocks.
 *                        I.e. One inode per data block.
 *
 * \param[in] major_ver   file system version to format, The <majorVersion>
 *                        and <minorVersion> parameters are used to specify
 *                        which particular version of the file system layout
 *                        should be used when formatting the disk.  A value
 *                        of zero for both the major and minor version is
 *                        use to indicate that the latest version of the file
 *                        system should be used.
 *
 * \param[in] minor_ver  file system version to format, The <majorVersion>
 *                       and <minorVersion> parameters are used to specify
 *                       which particular version of the file system layout
 *                       should be used when formatting the disk.  A value
 *                       of zero for both the major and minor version is
 *                       use to indicate that the latest version of the file
 *                       system should be used.
 *
 * \param[in] options  misc options, The <options> parameter is used to specify
 *                     additional formatting options.It is currently unused
 *
 * \return AW_OK on success or AW_ERROR on failure.
 */
aw_err_t aw_txfs_adv_format (void     *p_dev,
                             uint64_t  disk_size,
                             uint32_t  blk_size,
                             uint32_t  num_inodes,
                             uint32_t  major_ver,
                             uint32_t  minor_ver,
                             uint32_t  options);

/**
 * \brief format the TXFS file system via a path
 *
 * \detail This routine formats a disk or partition referenced by the path to
 *         the media.
 *
 * \param[in] p_dev  the connected disk device
 *
 * \param[in] disk_size  size of disk in bytes.
 *                       The <diskSize> argument is used to specifiy how many
 *                       bytes of the media the TXFS file system should occupy.
 *                       It can be used to prevent TXFS for using the end
 *                       portion of the media. In general this value should be
 *                       0 to specify that the entire media is to be used.
 *
 * \param[in] blk_size  size of block in bytes.
 *                      The <blkSize> parameter is used to specify what block
 *                      size, in bytes, TXFS should use. This block size must be
 *                      a power of 2, greater than the physical sector size, and
 *                      be within 512 to 8196 bytes inclusively. In general this
 *                      value should be specified as 0 so the formatter can
 *                      determine the most efficient block size to use for the
 *                      media size.
 *
 * \param[in] num_inodes  number of Inodes.
 *                        The <numInodes> parameter is used to specify the
 *                        absolute maximum number of files and directories the
 *                        file system can ever have. Note this does not include
 *                        the root directory which the formatter creates
 *                        automatically. Specifying a value of 0 will tell the
 *                        TXFS formatter to allow for the maximum number of
 *                        files/directories the file system can have based on 
 *                        the amount of data blocks. I.e. One inode per data 
 *                        block.
 *
 * \return AW_OK on success or AW_ERROR on failure.
 */
aw_err_t aw_txfs_format (void     *p_dev,
                         uint64_t  disk_size,
                         uint32_t  blk_size,
                         uint32_t  num_inodes);

/**
 * \brief Initialize the TXFS lib.
 *        Setting the max buffers and files in TXFS.
 *
 * \param[in] max_bufs  supporting buffers in FS
 * \param[in] max_files supporting files in FS
 *
 * \return  AW_OK on success or AW_ERROR otherwise
 */
aw_err_t aw_txfs_wr_lib_init (int max_bufs, int max_files);

/**
 * \brief initialize the TXFS device library routines
 *
 * \param[in] max_bufs   the max buffer numbers using in fs
 * \param[in] max_files  the max files supported in fs
 *
 * \return  AW_OK on success, AW_ERROR otherwise
 */
aw_err_t aw_txfs_dev_lib_init (int max_bufs, int max_files);

/**
 * \brief check the TXFS file system
 *
 * \param[in] p_dev       the physical block device object pointer
 * \param[in] verb_level  verbosity level
 * \param[in] flags       additional control information
 *
 * The <verb_level> argument is used to specify how much information is 
 * outputted to the console. A value of one indicates maximum verbosity. A value
 * of zero indicates minimum verbosity.
 *
 * The <flags> parameter is used to specify additional control information to 
 * the consistency checker.  If the AW_TXFS_CHKDSK_FLAG_UPGRADE bit is set, the 
 * checker will attempt to upgrade the file system to the newest version. All 
 * other flags are ignored if this bit is set. If the
 * AW_TXFS_CHKDSK_FLAG_REWIND_INODE_JOURNAL bit is set, the checker will attempt 
 * to rewind the inode journal. That is, if the inode journal is not empty and 
 * is marked as being out of sync with the other disk structures, inodes 
 * contained in the inode journal are copied back into the inode table on disk. 
 * This has the effect of restoring the inodes to the previous transaction.
 *
 * \return  AW_OK if media contains no errors
 *          AW_ERROR if one of more problems are detected.
 */
aw_err_t aw_txfs_chk_dsk (void *p_dev, int verb_level, int flags);

/**
 * \brief upgrade the TXFS file system to the latest version
 *
 * This routine is the TXFS consistency checker. It checks to see if the
 * file system referenced by the path is stable and consistent.
 *
 * WARNING. This function can only run on an inactive volume. Any currently
 *          opened files will closed as this routine will eject the current
 *          file system. The volume will also be unaccessible while the
 *          consistency checker executes.
 *
 * \param[in] p_dev  the physical block device object pointer
 *
 * \return  AW_OK if media was upgraded without errors.
 *          AW_ERROR if one of more problems are detected.
 */
aw_err_t aw_txfs_upgrade (void *p_dev);

/**
 * \brief create an TXFS device
 *
 * This routine creates an TXFS device. create and initial mount point
 *
 * \param[in] pDevName  Name of the TXFS device (mount point).
 * \param[in] p_dev     physical block device object pointer
 * \param[in] numBufs   # of [struct buf] to allocate.
 * \param[in] maxFiles  Maximum # of simultaneously open files
 *
 * \return  aw_txfs_dev_id if created and installed in Core I/O, NULL if not.
 */
aw_txfs_dev_id aw_txfs_dev_create (char *p_dev_name, void *p_dev,
                                   int   num_bufs,   int   max_files);

/**
* \brief create an TXFS device
*
* This routine is called by the file system monitor to instantiate TXFS.
*
* \param[in] p_dev         XBD for the device on which to mount.
* \param[in] p_dev_name    mount point string name.
* \param[in out] p_dev_id  the result TXFS device ID
*
* \return AW_OK if created and installed in Core I/O, AW_ERROR if not.
*/
aw_err_t aw_txfs_dev_create2 (void           *p_dev,
                              char           *p_dev_name,
                              aw_txfs_dev_id *p_dev_id);

/**
 * \brief explicitly delete an TXFS device
 *
 * \param[in] txfs_dev_id  TXFS device to delete
 *
 * This routine deletes an TXFS device. This can only be called from
 * a routine that has the device ID and is sure the device is no longer
 * in use. In normal operation, an eject event takes care of deletion.
 *
 * \return  AW_OK on success, AW_ERROR otherwise
 */
aw_err_t aw_txfs_dev_delete (aw_txfs_dev_id txfs_dev_id);

/**
 * \brief Probe the disk for TXFS
 *
 * \param[in] p_dev  physical block device object pointer
 *
 * This function is called from the file system monitor to test the disk
 * to see if it is an TXFS formatted disk.
 *
 * \return  AW_OK if the disk is formatted as TXFS, an errno if not.
 */
int aw_txfs_disk_probe (void *p_dev);

/**
 * \brief setting file system attribute
 *
 * \param[in] read_only  AW_TRUE:configure fs readonly,
 *                       AW_FALSE:configure fs read and write
 */
aw_err_t aw_txfs_attibute_set (aw_bool_t read_only);


#ifdef __cplusplus
}
#endif

#endif /* __AW_TXFS_H */

/* end of file */
