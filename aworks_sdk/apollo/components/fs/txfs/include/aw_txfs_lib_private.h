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
 * \brief Transactional File System private interfaces
 *
 * \internal
 * \par modification history:
 * - 1.00 17-07-06  mkr, first implementation
 * \endinternal
 */

#ifndef __AW_TXFS_LIB_PRIVATE_H
#define __AW_TXFS_LIB_PRIVATE_H

#include "aw_txfs_types.h"
#include "aw_txfs_port.h"
#include "aw_txfs_publish.h"
#include "aw_txfs_config.h"
#include "aw_txfs_tree.h"
#include "aw_txfs_vnode.h"
#include "aw_txfs_dll.h"
#include "aw_time.h"
#include "fs/aw_blk_dev.h"
#include "io/sys/aw_statvfs.h"
#include "aw_refcnt.h"
#include "aw_defer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AW_TXFS_MUTEX_CREATE_OPT (AW_SEM_Q_PRIORITY | AW_SEM_INVERSION_SAFE)

/**
 * \brief pack align macro
 */
#ifndef _WRS_PACK_ALIGN
#define _WRS_PACK_ALIGN(x) __attribute__((packed, aligned(x)))
#endif /* _WRS_PACK_ALIGN */

/** \brief FS inode start numbers */
#define AW_TXFS_INODE_STRT_NUM        (2u)

/** \brief FS root directory inode number */
#define AW_TXFS_INODE_ROOT_DIR        AW_TXFS_INODE_STRT_NUM

/** \brief max value of an unsigned int */
#define AW_TXFS_MAX_INODES            (0xffffffffu)

/** \brief Inode states */
#define AW_TXFS_INODE_STATE_FREE      (0xFF) /**< \brief ����״̬ */
#define AW_TXFS_INODE_STATE_ALLOC     (0xFE) /**< \brief ����״̬ */
#define AW_TXFS_INODE_STATE_TO_BE_DEL (0xFC) /**< \brief ��ɾ��״̬ */

/** \brief The number of inodes in one journal */
#define AW_TXFS_INODE_JOURNAL_ENTRIES (7)

/** \brief invalid block number */
#define AW_TXFS_INVALID_BLK_NUM       ((uint32_t)(-1))

/** \brief invalid inode number */
#define AW_TXFS_INVALID_INODE_NUM     ((uint32_t)(-1))

/** \brief TXFS volume magic number */
#define AW_TXFS_VOLUME_MAGIC          (0x00a87cb7)

/** \brief Initial 32 bit CRC value */
#define AW_TXFS_INTIAL_CRC32          (0xffffffffu)

/** \brief Good final 32 bit CRC value */
#define AW_TXFS_GOOD_CRC32            (0xdebb20e3)

/** \brief Macro to get inode from aw_txfs_vnode */
#define AW_TXFS_VTOI(ip,vnp)               \
    ip = (AW_TXFS_VTODATA(aw_txfs_inode_t, vnp));  \
    ip->inode_number = vnp->v_inode;

/******************************************************************************/
/**
 * \note Byte swap macros for converting in memory representations to disk
 *       representations for various types.
 *
 *       the byte-order convert just like the convert between network and host.
 */

#if _BYTE_ORDER==_BIG_ENDIAN

#define aw_txfs_ntohl(x)    (x)
#define aw_txfs_ntohs(x)    (x)
#define aw_txfs_htonl(x)    (x)
#define aw_txfs_htons(x)    (x)
#endif  /* _BYTE_ORDER==_BIG_ENDIAN */

#if _BYTE_ORDER==_LITTLE_ENDIAN

/** \brief convert big endian to little endian - for long type */
#define aw_txfs_ntohl(x)           \
    ((((x) & 0x000000ff) << 24)  | \
     (((x) & 0x0000ff00) <<  8)  | \
     (((x) & 0x00ff0000) >>  8)  | \
     (((x) & 0xff000000) >> 24))

/** \brief convert little endian to big endian - for long type */
#define aw_txfs_htonl(x)           \
    ((((x) & 0x000000ff) << 24)  | \
     (((x) & 0x0000ff00) <<  8)  | \
     (((x) & 0x00ff0000) >>  8)  | \
     (((x) & 0xff000000) >> 24))

/** \brief convert big endian to little endian - for short type */
#define aw_txfs_ntohs(x)     \
    ((((x) & 0x00ff) << 8) | \
     (((x) & 0xff00) >> 8))

/** \brief convert little endian to big endian - for short type */
#define aw_txfs_htons(x)     \
    ((((x) & 0x00ff) << 8) | \
     (((x) & 0xff00) >> 8))
#endif  /* _BYTE_ORDER==_LITTLE_ENDIAN */

/** \brief Converting for shorts and longs type */
#define aw_txfs_dtohs  aw_txfs_ntohs /**< \brief big to little - short */
#define aw_txfs_dtohl  aw_txfs_ntohl /**< \brief big to little - long */
#define aw_txfs_htods  aw_txfs_htons /**< \brief little to big - short */
#define aw_txfs_htodl  aw_txfs_htonl /**< \brief little to big - long */

/** \brief For long long or 64 bit quantities we need our own macros */
#if (_BYTE_ORDER == _BIG_ENDIAN)
/**
 * \brief Nothing to do for big endian targets since TXFS on-disk
 *        format is big endian
 */
#define aw_txfs_dtohll(x) (x)
#define aw_txfs_htodll(x) (x)

#else /* Handle little endian case */
/**
 * \brief swap byte-order for long long type number.
 *
 * \param[in] num  long long type number.
 *
 * \return long long type number had swapped.
 */
uint64_t aw_txfs_htodll_le(uint64_t num);

#define aw_txfs_htodll(x)    aw_txfs_htodll_le(x)
#define aw_txfs_dtohll(x)    aw_txfs_htodll(x)
#endif /* _BYTE_ORDER==BIG_ENDIAN */

/******************************************************************************/

/** \brief calculate the number of inodes in per block */
#define AW_TXFS_INODES_PER_BLK(bsize) ((bsize) >> 6)

/** \brief round up the number of inodes based on inodes per block */
#define AW_TXFS_NUM_INODES_UP(iperblk, ni)  \
    ((iperblk) + (((ni) - 1) & (-(iperblk))))

/**
 * \brief convert bytes to blocks based on a block size
 *        specified as a power of 2
 */
//#define BYTES_TO_BLKS(bytes, blk_size2) ((bytes) >> (blk_size2))

/** \brief convert time value to milliseconds */
#define AW_TXFS_TIMESPEC_TO_TXFSTIME(ts) \
    ((aw_txfs_txfstime_t)(ts).tv_sec * 1000 + ((ts).tv_nsec + 999999) / 1000000)

/** \brief convert milliseconds to seconds */
#define AW_TXFS_TXFSTIME_TO_SEC(htime)  (htime / 1000)

/** \brief convert milliseconds to nsec */
#define AW_TXFS_TXFSTIME_TO_NSEC(htime) ((htime % 1000) * 1000000)

/** \brief ABSOLUTE HANDLE, every handle is a transaction. */
#define AW_TXFS_STANDALONE      (0) /** wait until all transaction complete. */
#define AW_TXFS_AUTOMATIC       (1)

/** \brief getting high buffer in aw_txfs_vnode */
#define AW_TXFS_VBTOHB(vb)      ((struct aw_txfs_hbuf *)(vb->b_priv))

#define AW_TXFS_SYNC            (1) /**< \brief synchronous commit block  */
#define AW_TXFS_ASYNC           (0) /**< \brief asynchronous commit block  */

/**
 * \brief �����ļ�ϵͳ�ڴ棨buffers��ʹ���ʣ��ṩʹ���ʿ�������ļ�ϵͳ�Ķ�д�ٶ�
 *
 * ������ʹ�������ò�ǡ��ʱ����ֻ������ľ����ļ�ϵͳ������ѭ��״̬��
 * �ļ�ϵͳ�ڴ�ʹ����Ĭ������Ϊ40%������������Ϊn��������(�����������ļ�
 * ��д�������)Ϊ��
 */
#define AW_TXHRFS_COMMIT_POINT  (4) /**< \brief 40% cache usage*/

/** \brief TXFS commit(write data to disk) period */
#define AW_TXFS_COMMIT_PERIOD   (5) /**< \brief 5 seconds */

/** \brief print exception information */
#define AW_TXFS_EXCEPTION(x)                                         \
    do {                                                             \
        if (x) {                                                     \
            print_info(__FILE__, "%d", __LINE__, "TXFS EXCEPTION\n"); \
        }                                                            \
    } while(0)

/** \brief TXFS transaction state */
enum aw_txfs_trans_state {
    AW_TXFS_NO_COMMIT,     /**< \brief δ�ύ */
    AW_TXFS_NEED_COMMIT,   /**< \brief ��Ҫ�ύ */
    AW_TXFS_START_COMMIT,  /**< \brief ��ʼ�ύ */
    AW_TXFS_FINISH_COMMIT, /**< \brief ����ύ */
    AW_TXFS_QUIT_COMMIT    /**< \brief �˳��ύ */
};

/** \brief TXFS volume state */
typedef enum aw_txfs_vol_state {
    AW_TXFS_VOLUME_DEAD,
    AW_TXFS_VOLUME_CREATED,
    AW_TXFS_VOLUME_MOUNTED,
    AW_TXFS_VOLUME_DYING
} aw_txfs_vol_state_t;

/** \brief The type used to represent a file's size. 64 bits, unsigned */
typedef int64_t  aw_txfs_fsize_t;

/** \brief The type used to represent the generation number of an inode */
typedef uint32_t aw_txfs_fgen_t;

/** \brief Types used to represent the file owners' UID and GID values */
typedef uint32_t aw_txfs_fid_t;

/** \brief Type used to represent the number of hard links to a file */
typedef uint16_t aw_txfs_link_count_t;

/** \brief Type used to represent the mode bits of a file */
typedef uint16_t aw_txfs_fmode_t;

/** \brief Time in TXFS is a 64 bit signed quantity of ms since 1970 */
typedef int64_t  aw_txfs_txfstime_t;

typedef int (*txfs_func)();

/** \brief The on-disk representation of an TXFS V1.1 inode */
typedef struct aw_txfs_disk_inode_v1r1
{
    /** \brief The last time the file attributes (inode) were changed. */
    aw_txfs_txfstime_t ctime;

    /** \brief The last time the file data was changed */
    aw_txfs_txfstime_t mtime;

    /** \brief The size of the file */
    aw_txfs_fsize_t size;

    /** \brief # of times inode has been allocated */
    aw_txfs_fgen_t generation;

    /** \brief UID of value of the owner of the file */
    aw_txfs_fid_t uid;

    /** \brief GID of value of the owner of the file */
    aw_txfs_fid_t gid;

    /** \brief The number of blocks associated with inode */
    uint32_t n_blks;

    /** \brief Direct & Indirect block references */
    uint32_t indirect[4];

    /** \brief Number of hard links to file */
    aw_txfs_link_count_t link_cnt;

    /** \brief Mode bits for file */
    aw_txfs_fmode_t mode;

    /** \brief State:  free, allocated, to be deleted */
    uint8_t state;

    /** \brief Pad out size to 64 bytes */
    char reserved[2];

    /** \brief version format of inode */
    uint8_t version;
} _WRS_PACK_ALIGN(1) aw_txfs_disk_inode_v1r1_t;

/** \brief The on-disk representation of an TXFS V1.2 inode */
typedef struct aw_txfs_disk_inode_v1r2
{
    /** \brief The size of the file */
    aw_txfs_fsize_t size;

    /** \brief # of times inode has been allocated */
    aw_txfs_fgen_t  generation;

    /** \brief UID of value of the owner of the file */
    aw_txfs_fid_t   uid;

    /** \brief GID of value of the owner of the file */
    aw_txfs_fid_t   gid;

    /** \brief The number of blocks associated with inode */
    uint32_t        n_blks;

    /** \brief Direct & Indirect block references */
    uint32_t        indirect[4];

    /** \brief Number of hard links to file */
    aw_txfs_link_count_t link_cnt;

    /** \brief Mode bits for file */
    aw_txfs_fmode_t mode;

    /** \brief The last time the file attributes (inode) were changed. */
    uint8_t ctime[6];

    /** \brief The last time the file data was changed */
    uint8_t mtime[6];

    /** \brief The last time the file data was accessed */
    uint8_t atime[6];

    /** \brief State: free, allocated, to be deleted */
    uint8_t state;

    /** \brief version format of inode */
    uint8_t version;
} _WRS_PACK_ALIGN(1) aw_txfs_disk_inode_v1r2_t;

/** \brief Holder for on-disk representation inode */
typedef struct aw_txfs_disk_inode {
    char    space[64];   /**< \brief Pad out size to 64 bytes */
} aw_txfs_disk_inode_t;

/** \brief The in-memory representation of an TXFS inode */
typedef struct aw_txfs_incore_inode {
    /** \brief The last time the file attributes (inode) were changed. */
    aw_txfs_txfstime_t  ctime;

    /** \brief The last time the file data was changed */
    aw_txfs_txfstime_t  mtime;

    /** \brief The last time the file data was accessed */
    aw_txfs_txfstime_t  atime;

    /** \brief The size of the file */
    aw_txfs_fsize_t     size;

    /** \brief # of times inode has been allocated */
    aw_txfs_fgen_t      generation;

    /** \brief UID of value of the owner of the file */
    aw_txfs_fid_t       uid;

    /** \brief GID of value of the group of the file */
    aw_txfs_fid_t       gid;

    /* The number of blocks associated with inode */
    uint32_t            n_blks;

    /**
     * \brief indirect member record the file data in physical position
     *
     * - indirect[0]  point to the physical direct block number
     * - indirect[1]  1st level indirect block number
     * - indirect[2]  2nd level indirect block number
     * - indirect[3]  3rd level indirect block number
     *
     * Direct & Indirect block references
     */
    uint32_t             indirect[4];

    /** \brief Number of hard links to file */
    aw_txfs_link_count_t link_cnt;

    /** \brief Mode bits for file */
    aw_txfs_fmode_t      mode;

    /** \brief State: free, allocated, to be deleted */
    uint8_t              state;

    /** \brief version format of on-disk inode */
    uint8_t              version;
} aw_txfs_incore_inode_t;

/**
 * When we allocate block for a file, the last allocating information can be
 * used for guide which block to allocate. By doing this, we can speed up the
 * allocating speed and reduce the disk fragment.
 *
 * \FIXME More information can be added in this structure to optimize further.
 */
typedef struct aw_txfs_alloc_pos {
    uint32_t last_alloc_phyblk_num;
} aw_txfs_alloc_pos_t;

/** \brief inode structure in memory */
typedef struct aw_txfs_inode_struct {
    /** \brief Node of "to be del" list */
    aw_txfs_dl_node      del_node;

    /** \brief incore inode data */
    aw_txfs_incore_inode_t inode_data;

    /* 
    * �ļ���ص�i�����ţ�ͨ��i�����ſ����ҵ����ļ�������ص�i�ڵ���Ϣ����ŵ�λ
     * �ã���IT�����У�,������Ҫ��inode��Ϣ�ʹ�IT�����ж�ȡ��
     * ���浽��һ����Ա inode_data ������
     */
    uint32_t             inode_number;

    /** \brief directory slot(bytes) from lookup op */
    uint32_t             dir_slot;

    /** \brief inode number of subdirectory being deleted from */
    uint32_t             dir_inode;

    /** \brief saved dir_slot for final unlink */
    uint32_t             dir_del_slot;

    /** \brief guide which block to alloc */
    aw_txfs_alloc_pos_t  i_alloc_pos;
} aw_txfs_inode_t;

/**
 * \brief The representation of the TXFS inode journal,
 *        inode journal is 512 bytes
 * \note inode journal ��¼�ļ��޸�ǰ����Ϣ������ļ�����ͬ�������̹����г�����
 *       �����ļ�ϵͳ�������ϵ磩������ͨ���ü�¼�ָ���д��ǰ���ļ���Ϣ��
 */
typedef struct aw_txfs_inode_journal {
    /** \brief Transaction number. */
    uint64_t trans_num;

    /** \brief I-Node numbers. */
    uint32_t inode_num[AW_TXFS_INODE_JOURNAL_ENTRIES];

    /** \brief For alignment. */
    char     reserved[28];

    /** \brief Inode data */
    aw_txfs_disk_inode_t inode_dat[AW_TXFS_INODE_JOURNAL_ENTRIES];
} _WRS_PACK_ALIGN(1) aw_txfs_inode_journal_t;

/** \brief TXFS Transaction Master Record */
typedef struct aw_txfs_tmr
{
    /** \brief TXFS transaction number */
    uint64_t    trans_num;

    /** \brief TXFS timestamp */
    uint64_t    time_stamp;

    /** \brief to be delete inodes number */
    uint32_t    num_inodes_to_del;

    /** \brief free inodes number */
    uint32_t    num_free_inodes;

    /** \brief free data block number */
    uint32_t    num_free_dat_blks;

    /** \brief no used now */
    uint32_t    spare;
} _WRS_PACK_ALIGN(1) aw_txfs_tmr_t;

/** \brief TXFS incore super block structure */
typedef struct aw_txfs_incore_superblk {
    char     id_str[8];  /**< \brief Identification or eyecatcher string */

    int64_t  ctime;      /**< \brief time at which superblock was created. */

    uint8_t  major_vers; /**< \brief Major version number */
    uint8_t  minor_vers; /**< \brief Minor version number */

    uint16_t blk_size2;  /**< \brief Block size as a power of 2. */
    
    /** \brief Total # of blocks in fs (including offset). */
    uint32_t total_blks; 

    /** \brief Size of the reserved space at the start of the media. */
    uint32_t reserved_blks;

    /** \brief The number of iNodes this file system instantiation has. */
    uint32_t inode_count;

    /** \brief Block group size(=dsSize + 1 for 1st iter) */
    uint32_t bg_size;     
    
    /** \brief Data space size(=bgSize - 1 for 1st iter) */
    uint32_t ds_size;    
    
    /** \brief Number of block groups(1 for 1st iteration) */
    uint32_t nblk_groups; 

    uint32_t fsb_off[2];  /**< \brief 1st & 2nd free space bitmap offsets */
    uint32_t fib_off[2];  /**< \brief 1st & 2nd free inode bitmap offsets */

    uint32_t it_off;      /**< \brief inode table offset */
    uint32_t ij_off;      /**< \brief inode journal offset */

    uint32_t tm_off[2];   /**< \brief 1st & 2nd transaction map offsets */
    
    /** \brief 1st & 2nd transaction  master record offsets */
    uint32_t tmr_off[2];  

    uint32_t ds_off;      /**< \brief data space offset */
    uint32_t pad;         /**< \brief pad out structure */
    uint32_t crc;         /**< \brief superblock CRC */
} aw_txfs_incore_superblk_t;

/**
 * \brief The representation of the TXFS Super Block.
 *
 * The size of this structure must be a multiple of 8
 */
typedef struct aw_txfs_disk_superblk
{
    char        id_str[8];  /**< \brief Identification or eyecatcher string */
    int64_t     ctime;      /**< \brief time at which superblock was created. */
    uint8_t     major_vers; /**< \brief Major version number */
    uint8_t     minor_vers; /**< \brief Minor version number */
    uint16_t    blk_size2;  /**< \brief Block size as a power of 2. */
    
    /** \brief Total # of blocks in fs (including offset). */
    uint32_t    total_blks; 

    /** \brief Size of the reserved space at the start of the media. */
    uint32_t    reserved_blks;

    /** \brief The number of iNodes this file system instantiation has. */
    uint32_t    inode_count;

    /** \brief Block group size (=dsSize + 1 for 1st iter) */
    uint32_t    bg_size;     
    
    /** \brief Data space size (=bgSize - 1 for 1st iter) */
    uint32_t    ds_size;     
    
    /** \brief Number of block groups. (1 for 1st iteration) */
    uint32_t    nblk_groups; 

    uint32_t    fsb_off[2];  /**< \brief 1st & 2nd free space bitmap offsets */
    uint32_t    fib_off[2];  /**< \brief 1st & 2nd free inode bitmap offsets */

    uint32_t    it_off;      /**< \brief inode table offset */
    uint32_t    ij_off;      /**< \brief inode journal offset */

    uint32_t    tm_off[2];   /**< \brief 1st & 2nd transaction map offsets */
    
    /** \brief 1st & 2nd transaction  master record offsets */
    uint32_t    tmr_off[2];  

    uint32_t    ds_off;      /**< \brief data space offset */
    uint32_t    pad;         /**< \brief pad out structure */
    uint32_t    crc;         /**< \brief superblock CRC */
} _WRS_PACK_ALIGN(1) aw_txfs_disk_superblk_t;

/** \brief TXFS directory entry */
typedef struct aw_txfs_dirent
{
    /** \brief iNode corresponding to file */
    uint32_t       inode_num;

    /** \brief File Name */
    unsigned char  file_name[AW_TXFS_MAX_FILENAME_LEN + 1];
} _WRS_PACK_ALIGN(1) aw_txfs_dirent_t;

/** \brief TXFS transaction structure */
typedef struct aw_txfs_transaction {
    uint64_t      trans_no;

    AW_MUTEX_DECL(trans_mutex);
    aw_mutex_id_t trans_mutex_id;

    /* transaction state, \ref enum aw_txfs_trans_state */
    int          state;
    AW_SEMB_DECL(state_sem);

    atomic_t     ref;

    /*
     * We don't use the delay write code before, since it is in the 
     * aw_txfs_vnode layer,it is not suit for the new transaction mechanism of 
     * TXFS.
     * So we put the delay write code in TXFS layer to make the new
     * transaction mechanism be easy to implement.
     */

    /*
     * meta_blklist contains dirty buffers of meta data in current transaction:
     * 1) the buffers in meta_blklist can't be scheduled.
     * 2) the next transaction won't start until finish committing meta_blklist
     *    of current transaction.
     * 3) buffers are not in free list.
     * Ԫ�����б���¼����Ԫ���ݻ���飬Ԫ������ָ���ݵ����ݣ����ļ�ϵͳ���൱
     * ��FSB��FIB��IT��������Щ����
     */
    AW_TAILQ_HEAD(, aw_txfs_hbuf)meta_blklist;

    /*
     * data_blklist contains dirty buffers of file data in current transaction:
     * 1) it can be scheduled.
     * 2) starting next transaction is allowed when committing data_blklist.
     * 3) buffers are not in free list.
     * ���ݿ��б���¼�����ļ���Ŀ¼������
     */
    AW_TAILQ_HEAD(, aw_txfs_hbuf)data_blklist;

    int          datablk_no;      /**< \brief ��Ҫ������ļ�/Ŀ¼���ݿ���� */
    AW_SEMB_DECL(datablk_no_sem); /**< \brief ���ݴ�����ɵ��ź��� */

    /** \brief һ�δ����е����ݿ����� = Ԫ(metadata)���ݿ� + ���ݿ�(datablkNo)*/
    int          total_blk_no;

    /**
     * \brief For the current transaction, it can journal inode when the 
     * previous is committing. The journal for the new transaction is empty
     * and can't be committed to disk until the previous is totally committed. 
     * So we can Journal the inode of current transaction in the memory and 
     * commit it when committing the current transaction.
     */
    aw_txfs_inode_journal_t *p_inode_journal;
    /*
     * \brief ʣ��� inode journal �������� p_inode_journal ָ��Ŀռ仹���Դ�
     *        �ŵ� inode �ĸ���
     */
    int                      free_journal_cnt;

    int          alive_handle_no;      /**< \brief һ��������� handle ���� */
    AW_SEMB_DECL(alive_handle_no_sem); /**< \brief ���� handle ��������ź��� */

    /** \brief ���ͷŵ����ݿ�ţ�����δ���ύ��ɣ��ں������ */
    AW_TAILQ_HEAD(freeQueue, aw_txfs_blk_entry)pin_list;

    /** \brief ָ�� TM ������� FSB ��¼��һ������ */
    void         *valid_fsb_tm;  
    
    /** \brief ָ�� TM ������� FIB ��¼��һ������ */
    void         *valid_fib_tm;  

    int           total_handle_no; /**< \brief �����ܴ��� handle ���� */
} aw_txfs_transaction_t;


/** \brief �ļ�ϵͳ���ٻ����ṹ�� */
typedef struct aw_txfs_hbuf {
    AW_TAILQ_ENTRY(aw_txfs_hbuf)dirty_list; /**< \brief �ļ�ϵͳ���ٻ�������� */
    struct aw_txfs_buf         *owner;      /**< \brief ָ����ٻ�����ָ�� */
    aw_txfs_transaction_t      *trans;      /**< \brief ָ�������ָ�� */
} aw_txfs_hbuf_t;

/**
 * \brief TXFS volume descriptor structure
 *
 * Fields that describe the physical layout of the backing media,
 * try to go in the volume descriptor.
 */
typedef struct aw_txfs_volume_desc {
    /** \brief value: AW_TXFS_VOLUME_MAGIC */
    uint32_t                 magic;

    /** \brief Created/Mounted/Dying/Dead */
    aw_txfs_vol_state_t      state;

    /** \brief TXFS Super Block */
    aw_txfs_incore_superblk_t *p_superblk;

    /** \brief device associated with volume */
    void    *device;

    /** \brief # of sectors on backing media */
    sector_t n_sectors;
    /** \brief sector size of backing media */
    uint32_t sector_size;
    /** \brief The number of sectors in a block */
    uint32_t sectors_perblk;
    /** \brief The number of sectors in a block as a power of 2 */
    uint32_t sectors_perblk2;

    /** \brief block size in bytes */
    uint32_t blksize;

    /** \brief # of blocks */
    uint32_t blk_cnt;

    /**
     * \brief valid transaction set: 0 or 1,
     * updated when transaction had finished 
     */
    int      valid_trans_set;

    /**
     * \brief The transaction number of the last set transaction point,
     * initial according to the latest TMR field during in mount.
     */
    uint64_t valid_trans_no;

    /**
     * \brief The transaction number of the current transaction.
     * initial during in FS mounting(set to the last transaction number + 1).
     */
    uint64_t working_trans_no;

    /** \brief initial according to the latest TMR field during in mount. */
    uint32_t     num_inodes_to_del;

    /**
     * TXFS allow two transaction works together: one is committing to disk and
     * another is writing to cache. In this way, TXFS can make full use of CPU
     * and disk I/O. curTrans is the transaction which is writing to cache and
     * prevTrans is the transaction which is committing to the disk.
     */
    aw_txfs_transaction_t  *work_trans;
    aw_txfs_transaction_t  *commit_trans;
    aw_txfs_transaction_t   trans_buf[2];

    AW_MUTEX_DECL(vol_mutex);
    aw_mutex_id_t vol_mutex_id;
    int           vol_mutex_init;

    AW_SEMB_DECL(barrier_sem);    /** \brief һ������handle�е�ͬ���ź� */
    int          barrier_sem_init;
    
    /** \brief ����handle�źţ���1ʱ��ʾ�����ύ�������� */
    int          handle_barrier;  

#if (AW_TXFS_IS_DEFER_COMMIT )
    /** \brief defer working node */
    struct aw_defer_djob    defer_wnode;
#else
    /** \brief commit task members */
    long         commit_period;   /**< \brief commit period */
    aw_task_id_t commit_task;     /**< \brief commit task id */
    uint8_t      task_name[10];   /**< \brief commit task name */

    AW_SEMB_DECL(cmt_tsk_wsemb);  /**< \brief �ύ����ȴ��ź���  */
    aw_semb_id_t cmt_tsk_wsemb_id;
#endif

    AW_MUTEX_DECL(fs_mutex);      /**< \brief �ļ�ϵͳ���������� */
    aw_mutex_id_t fs_mutex_id;

  /** \brief һ�ú��������������⼴�����ͷŵ����ݿ�ţ�FSB��������δ���ͷ�  */
    RB_HEAD(blkTree, aw_txfs_blk_entry)pin_tree;

    /**
     * The "allocation rotors" make the block allocator scan from the
     * middle back to the middle, instead of start to end every time.
     * Each scan starts from wherever the last scan was successful.
     * This tends to make blocks (and inodes) get allocated sequentially.
     *
     * This is purely for optimization (and not yet turned on).
     */
    uint32_t alloc_rotor[2];  /**< \brief [0]: FSB, [1]: FIB */

    /**
     * These are mainly used for the "sector discard" interface,
     * but also help speed up allocation.
     *
     * The "bitmap point" number controls when the code switches from
     * "raw blocks" to "bitmap".  It can never be larger than the raw
     * block array, of course, but for some applications performance may
     * be better with a smaller number (and for debugging we want to
     * be able to set it to a tiny number).  We are using the rawBlocks
     * field if blks_freed_this_trans <= blksFreedBitmapPoint, otherwise we
     * are using the bitmap.
     *
     * The "magic number" 8192 here is based on the maximum file system and
     * block size ... probably should use a #define for this.
     */
    aw_bool_t   do_discard;
    uint32_t blks_freed_this_trans;   /**< \brief blocks freed since commit */
    uint32_t inodes_freed_this_trans; /**< \brief inodes freed since commit */

    /**
     * \brief free inodes number.
     * initial according to the latest TMR field during in mount.
     */
    uint32_t free_inodes;

    /**
     * \brief free data blocks number.
     * initial according to the latest TMR field during in mount.
     */
    uint32_t free_data_blks;

    aw_txfs_mount_t *p_mount;

    /** \brief no used now */
    voff_t           trie_offset[5];

    /** \brief List of "to be deleted" inodes */
    aw_txfs_dl_list  to_be_del_list;

    /** \brief the fellow three members using commit raw data(IJ and TMR) */
    void        *p_blk_data; /**< \brief a logical block data buffer */
    AW_SEMB_DECL(bio_sem);   /**< \brief for raw data block commit sync */
    aw_semb_id_t bio_semid;  /**< \brief binary semaphore id of bio_sem */
} aw_txfs_vol_desc_t;

/** \brief TXFS transaction handle info structure */
typedef struct aw_txfs_handle {
    aw_txfs_transaction_t *trans;     /**< \brief transaction */
    aw_txfs_vol_desc_t    *vol_desc;  /**< \brief volume desc */

    /**
     * \brief transaction handle policy,
     * AW_TXFS_STANDALONE or AW_TXFS_AUTOMATIC
     */
    int                    policy;

    /** \brief writing block number in one transaction handle */
    int                    wr_blk_no;
} aw_txfs_handle_t;

/** \brief TXFS device structure */
typedef struct aw_txfs_dev {
    /** \brief TXFS Volume Descriptor */
    aw_txfs_vol_desc_t txfs_vol_desc;

    /** \brief sector scratch-pad */
    uint8_t *p_sector_data;

    /** \brief Binary semaphore for BIO/XBD calls */
    AW_SEMB_DECL(bio_sem);

    /** \brief Binary semaphore id for BIO/XBD calls */
    aw_semb_id_t bio_sem_id;
} aw_txfs_dev_t;

/** \brief structure used by time functions */
typedef struct aw_txfs_tm {
    aw_tm_t   tm;    /**< \brief original [tm] structure.                   */
    int       msec;  /**< \brief milliseconds after the second   - [0..999] */
} aw_txfs_tm_t;

/** \brief block entry structure */
struct aw_txfs_blk_entry {
    RB_ENTRY(aw_txfs_blk_entry)       rb_node;
    AW_TAILQ_ENTRY(aw_txfs_blk_entry) q_node;
    uint32_t                          blk_no;
};

/******************************************************************************
 * external functions
 *****************************************************************************/
/**
 * \brief hrfs_converters interfaces
 * @{
 */

/**
 * \brief converts an in-core superblock to disk format
 *
 * \param[in out] p_dest  destination super block
 * \param[in] p_src       source super block
 *
 * \note This function takes a super block that is in in-core (native endian)
 *       format and converts it to the on-disk (big endian) format.
 */
void aw_txfs_superblk_core_dsk_cvt(aw_txfs_disk_superblk_t   *p_dest,
                                   aw_txfs_incore_superblk_t *p_src);

/**
 * \brief converts a v1.1 in-core inode to disk format
 *
 * \param[in] p_dest  destination inode(disk-inode v1.1)
 * \param[in] p_src   source inode(in-core inode)
 *
 * \note This function takes a V1.1 inode that is in in-core (native endian)
 *       format and converts it to the on-disk (big endian) format.
 */
void aw_txfs_inode_core_dsk_cvt_v1r1 (aw_txfs_disk_inode_v1r1_t *p_dest,
                                      aw_txfs_incore_inode_t    *p_src);

/**
 * \brief converts a v1.2 in-core inode to disk format
 *
 * \param[in] p_dest  destination inode(disk-inode v1.2)
 * \param[in] p_src   source inode(in-core inode)
 *
 * \note This function takes a V1.2 inode that is in in-core (native endian)
 *       format and converts it to the on-disk (big endian) format.
 */
void aw_txfs_inode_core_dsk_cvt_v1r2(aw_txfs_disk_inode_v1r2_t *p_dest,
                                     aw_txfs_incore_inode_t    *p_src);

/**
 * \brief converts a disk superblock to in-core format
 *
 * \param[in out] p_dest  destination super block(in-core)
 * \param[in]     p_src   source super block(disk)
 *
 * \note This function takes a super block that is in on-disk (big endian)
 *       format and converts it to the in-core format in the host's native
 *       endianess.
 */
void aw_txfs_super_blk_dsk_core_cvt(aw_txfs_incore_superblk_t *p_dest,
                                    aw_txfs_disk_superblk_t   *p_src);

/**
 * \brief converts a v1.1 disk inode to in-core format
 *
 * \param[in] p_dest  destination inode(in-core inode)
 * \param[in] p_src   source inode(disk-core v1.1)
 *
 * \note This function takes a V1.1 inode that is in on-disk (big endian)
 *       format and converts it to the in-core format in the host's native
 *       endianess.
 */
void aw_txfs_inode_dsk_core_cvt_v1r1(aw_txfs_incore_inode_t    *p_dest,
                                     aw_txfs_disk_inode_v1r1_t *p_src);

/**
 * \brief converts a v1.2 disk inode to in-core format
 *
 * \param[in] p_dest  destination inode(in-core inode)
 * \param[in] p_src   source inode(disk-core v1.2)
 *
 * \note This function takes a V1.2 inode that is in on-disk (big endian)
 *       format and converts it to the in-core format in the host's native
 *       endianess.
 */
void aw_txfs_inode_dsk_core_cvt_v1r2(aw_txfs_incore_inode_t    *p_dest,
                                     aw_txfs_disk_inode_v1r2_t *p_src);

/** @} */

/******************************************************************************/
/**
 * \brief hrfs_super_blk interfaces
 * @{
 */

/**
 * \brief Done function for XBD operations
 *
 * This routine assumes the bio_caller1 field in the passed in bio struct is
 * a binary semaphore. Specifically the one in the aw_txfs_dev_t struct. This
 * function is for performing synchronizing single bio operations such as
 * xbd_strategy.
 *
 * \param[in] p_bio  the pointer of block device I/O data
 *
 * \return N/A
 */
void aw_txfs_sync_xbd_done(struct aw_blk_arp *p_bio);

/**
 * \brief Read the super block
 *
 * This routine scans the media for the TXFS super block and reads its contents
 *
 * \param[in] devPtr  pointer to TXFS Device structure
 *
 * \return  If the function finds a suitable super block it returns AW_OK.
 *          If an TXFS super block cannot be found the function returns an errno.
 */
int aw_txfs_superblk_rd(aw_txfs_dev_t *p_dev);

#if (AW_TXFS_DEBUG_ENABLE == 1)
/**
 * \brief Print super block contents
 * \param[in] pSuperBlk  The pointer of incore super block
 * \return N/A
 */
void aw_txfs_print_superblk(aw_txfs_incore_superblk_t *p_superblk);
#endif

/** @} */

/******************************************************************************/
/**
 * \brief hrfs_time_lib interfaces
 * @{
 */

/**
 * \brief return # of milliseconds since midnight Jan 1, 1900
 *
 * This routine returns the number of milliseconds since midnight, January 1,
 * 1900.
 *
 * \returns  # of milliseconds since midnight, January 1, 1900
 */
aw_txfs_txfstime_t aw_txfs_time_get(void);

/**
 * \brief convert "broken-down" TXFS time to string
 *
 * \param[in]  p_txfs_tm  Buffer contain time to convert in TXFS format
 * \param[out] p_buf      Place to write ASCII time format data
 * \param[in]  buf_len    Size of the supplied ASCII time buffer
 *
 * This routine converts the "broken-down" TXFS time pointed to by <pHrfsTm>
 * into a string of the form:
 * .CS
 *       SUN SEP 16 01:03:52 1973\en\e0
 * .CE
 * The string is copied into <pBuffer>.  Note that the field pHrfsTm->msec is
 * not displayed.
 *
 * \return  the number of bytes copied to <pBuffer>.
 */
int aw_txfs_asc_time(aw_txfs_tm_t *p_txfs_tm, char *p_buf, size_t buf_len);

/**
 * \brief split time in msec into aw_txfs_tm_t format
 *
 * \param[in] milliSeconds  milliseconds to convert to aw_txfs_tm_t format
 * \param[in] pHrfsTm       Buffer to store aw_txfs_tm_t format time
 *
 * This routine splits the time specified in milliseconds into the "broken-down"
 * format of the aw_txfs_tm_t structure.  Should the equivalent number of seconds
 * exceed what can be represent by a signed integer, this routine will currently
 * return ERROR, and the split will not have occurred.
 *
 * \return  AW_OK success, or ERROR if the split did not occur
 */
aw_err_t aw_txfs_time_split (int64_t milliseconds, aw_txfs_tm_t *p_txfs_tm);

/**
 * \brief condense time in aw_txfs_tm_t to time in msec
 *
 * \param[in] pHrfsTm  Pointer to where aw_txfs_tm_t format time is stored
 *
 * This routine condenses the "broken-down" time pointed to by <pHrfsTm> into
 * the number of milliseconds since midnight Jan 1, 1900.
 *
 * \return  # of milliseconds since midnight Jan 1, 1900
 */
int64_t aw_txfs_time_condense (aw_txfs_tm_t *p_txfs_tm);

/* @} */

/******************************************************************************/
/**
 * \brief hrfs_trans_interfaces
 * @{
 */

/**
 * \brief red-black tree key compare function
 *
 * \param[in] p_entry_a  a block entry
 * \param[in] p_entry_b  another block entry
 *
 * \return 0 : a's blk_no == b's blk_no
 *         1 : a's blk_no >  b's blk_no
*         -1 : a's blk_no <  b's blk_no
 */
int aw_txfs_blk_key_compare (struct aw_txfs_blk_entry *p_entry_a,
                             struct aw_txfs_blk_entry *p_entry_b);
RB_PROTOTYPE(blkTree, aw_txfs_blk_entry, rb_node, aw_txfs_blk_key_compare);

/**
 * \brief take a mutex used in exchange
 *
 * \param[in] mutexId  mutex semaphore id
 *
 * This routine takes a mutex for use in an internal, native implementation
 * of basic condition variable functionality.
 *
 * \retval  AW_OK   �ɹ�
 * \retval  errno   ʧ��
 */
int aw_txfs_mutex_lock (aw_mutex_id_t mutex_id);

/**
 * \brief give a mutex used in exchange
 *
 * \param[in] mutexId  mutex semaphore id
 *
 * This routine gives a mutex for use in an internal, native implementation
 * of basic condition variable functionality.
 *
 * \return  AW_OK or ERROR
 */
int aw_txfs_mutex_unlock (aw_mutex_id_t mutex_id);

/**
 * \brief Updates the trie entries for the given inode.
 *
 * \param[in] p_handle    �ļ�ϵͳhandleֵ
 * \param[in] p_txfs_dev  ָ���ļ�ϵͳ�豸��ָ��
 * \param[in] p_vnode     ָ���ļ���ص�vnodeָ��
 * \param[in] lbn         ��������ļ���ص��ļ��߼����
 * \param[in] new_pbn     �µ��߼����
 * \param[in] p_trie_buf  ��¼����������ĸ��ٻ�������
 *
 * This routine walks back through the tries updating for a new physical
 * to logical block mapping
 *
 * �ú��������޸��ļ��߼��飬Ҫ�޸�һ���ļ����߼���ţ��͵ø������ļ��߼������
 * �ص��������������ݣ����newPbn�µ��߼���Ϊ��Ч���߼��飬���ж��Ƿ���Ҫ����
 * �صĴ��̿��ͷ�
 *
 * \return  AW_OK on success, errno otherwise
 */
int aw_txfs_update_trie_path (struct aw_txfs_handle *p_handle,
                              aw_txfs_dev_t         *p_txfs_dev,
                              struct aw_txfs_vnode  *p_vnode,
                              uint32_t               lbn,
                              uint32_t               new_pbn,
                              struct aw_txfs_hbuf   *p_trie_buf[3]);

/***
 * \brief commit raw data to disk
 *
 * \param[in] p_txfs_dev  pointer to aw_txfs_dev_t
 * \param[in] p_dat       data committed to disk
 * \param[in] size        data size in bytes
 * \param[in] blk_num     physical block number in disk
 * \param[in] flag        W/R flags,
 *                        - AW_BLOCK_IO_WRITE write
 *                        - AW_BLOCK_IO_READ  read
 *
 * \return N/A
 */
void aw_txfs_raw_commit_blk (aw_txfs_dev_t *p_txfs_dev,
                             void          *p_dat,
                             uint32_t       size,
                             uint32_t       blk_num,
                             int            flag);

/**
 * \brief wait all transactions of TXFS to commit
 *
 * \param[in] p_vol  pointer to HRFS_VOLUME_DESC
 *
 * When application issue a FIOFLUSH command, the application will wait until
 * both the work transaction and the commit transaction are committed. And if 
 * the handle is an ABSOLUTE handle, it also will wait the other transaction to
 * commit
 *
 * \return  N/A
 */
void aw_txfs_wait_trans_commit (aw_txfs_vol_desc_t *p_vol);

/**
 * \brief protect begin for a transaction handle
 */
aw_txfs_handle_t *aw_txfs_protect_begin (aw_txfs_vol_desc_t *p_txfs_vol,
                                         int                 policy);

/**
 * \brief protect end for a transaction handle
 */
void aw_txfs_protect_end (struct aw_txfs_handle *p_handle, int result);

/**
 * \brief write block data completely callback,
 *
 * \param[in] p_bio  pointer to aw_block_io
 *
 * \return N/A
 */
void aw_txfs_write_bio_done (struct aw_blk_arp *p_bio);

/**
 * \brief commit the block, that write data
 *
 * \param[in] sync  AW_TXFS_SYNC :synchronous commit,
 *                  HRFS_ASYNC:asynchronous commit.
 *
 * Before aw_txfs_commit_blk, buf with AW_TXFS_B_BUSY, b_avail, b_mutex set
 * it likes buf_startwrite
 *
 * \return  N/A
 */
void aw_txfs_commit_blk (aw_txfs_vol_desc_t    *p_vol,
                         aw_txfs_transaction_t *p_trans,
                         aw_txfs_hbuf_t        *p_hbuf,
                         int                    sync);

/**
 * \brief re-busy a buffer by clear the AW_TXFS_B_INTRANS flag in buffer
 *
 * \param[in] p_hbuf  pointer to aw_txfs_hbuf
 *
 * \return N/A
 */
void aw_txfs_rebusy_blk (struct aw_txfs_hbuf *p_hbuf);

/**
 * \brief the task used to commit TXFS transaction
 *
 * aw_txfs_commit_task will be waked up to commit the current transaction in 
 * three conditions:
 * 1) Timeout
 * 2) Waked up by aw_txfs_protect_end
 *    (according to memory usage or some error happen)
 * 3) waked up by aw_txfs_wait_trans_commit
 *
 * \return N/A
 */
void aw_txfs_commit_task (void *p_arg1);


/**
 * \brief txfs defer working node function
 *
 * \param[in] p_arg  parameter of defer commit task
 *
 * \return N/A
 */
void txfs_defer_commit_task (void *p_arg);



/**
 * \brief release a buffer
 * \param[in] p_hbuf  pointer to fs high speech cache.
 */
void aw_txfs_relse_blk (struct aw_txfs_hbuf *p_hbuf);

/**
 * \brief getting a buffer data according to aw_txfs_vnode and logical block 
 *        number
 *
 * �����߼������к�vnode��ȡһ��HRFS_BUF����
 *
 * \param[in] p_vnode  pointer to aw_txfs_vnode
 * \param[in] lbn      logical block numbers
 * \param[in] size     the size want to read
 * \param[in] pri      priority of the buffer
 *
 * \return TXFS high speech buffer cache if success, NULL if failed
 */
aw_txfs_hbuf_t *aw_txfs_get_blk (struct aw_txfs_vnode *p_vnode,
                                 aw_txfs_lblkno_t      lbn,
                                 int                   size,
                                 int                   pri);

/**
 * \brief reading a buffer data according to the aw_txfs_vnode and logical block 
 *        number.
 *
 * �����߼������к�vnode��ȡһ��hrfsBuf����
 *
 * \param[in] p_vnode      pointer to aw_txfs_vnode
 * \param[in] lblk_no      logical block number of the file
 * \param[in] size         size of the data in reading
 * \param[in out] pp_hbuf  pointer to the fs high speech buffer cache
 * \param[out] pri         priority of the buffer
 *
 * \return AW_OK if successful, ENOMEM if no memory
 */
int aw_txfs_read_blk (struct aw_txfs_vnode *p_vnode,
                      aw_txfs_lblkno_t      lblk_no,
                      int                   size,
                      struct aw_txfs_hbuf **pp_hbuf,
                      int                   pri);
/**
 * \brief convert physical block number to sector number
 *
 * \param[in] p_vol_desc  volume descriptor pointer
 * \param[in] p_bio       block device io request packet
 * \param[in] pbn         physical block number
 *
 * \return N/A
 */
void aw_txfs_update_bio_phy (aw_txfs_vol_desc_t *p_vol_desc,
                             struct aw_blk_arp  *p_bio,
                             uint32_t            pbn);

/**
 * \brief The core algorithm for data block COW
 *
 * \param[in] p_handle        pointer to struct hrfsHandle
 * \param[in] p_vnode         pointer to aw_txfs_vnode
 * \param[in] p_org_buf       pointer to source buffer
 * \param[in out] pp_new_buf  pointer to new buffer
 * \param[in] p_hint          if deal with file indirect block, hint is the
 *                            pointer to aw_txfs_vnode; if deal with file date
 *                            block, hint is equal to NULL.
 *
 * \return AW_OK if successful or errno.
 */
int aw_txfs_cow_blk (struct aw_txfs_handle  *p_handle,
                     struct aw_txfs_vnode   *p_vnode,
                     struct aw_txfs_hbuf    *p_org_buf,
                     struct aw_txfs_hbuf   **pp_new_buf,
                     void                   *p_hint);

/**
 * \brief write a block to disk
 *
 * just insert the aw_txfs_hbuf to transaction linked list
 *
 * \param[in] p_handle  pointer to struct aw_txfs_handle
 * \param[in] p_vnode   pointer to struct aw_txfs_vnode
 * \param[in] p_hbuf    pointer to struct aw_txfs_hbuf
 *
 * \return AW_OK
 */
int aw_txfs_write_blk (struct aw_txfs_handle *p_handle,
                       struct aw_txfs_vnode  *p_vnode,
                       struct aw_txfs_hbuf   *p_hbuf);

/**
 * \brief check if there is enough space to journal inode
 *
 * \param[in] p_handle      pointer to HRFS_HANDLE
 * \param[in] ivec          pointer to inode numbers
 * \param[in] req_free_cnt  request for free inode numbers
 *
 * ���handle->trans->inodeJournal�У��Ƿ����㹻��iNodeNumber���пռ�洢
 * iNodeNumber
 *
 * \return AW_OK if has enough space, else AW_TXFSLIB_ERR_INODE_JOURNAL_FULL if
 *         no enough space
 */
int aw_txfs_validate_inode_journal (aw_txfs_handle_t *p_handle,
                                    uint32_t          ivec[],
                                    int               req_free_cnt);

/**
 * \brief update inode info to handle->trans->inode_journal->inode_data
 *
 * \param[in] p_handle    the pointer to HRFS_HANDLE
 * \param[in] p_txfs_dev  the pointer to aw_txfs_dev_t
 * \param[in] p_inode     the pointer to p_aw_txfs_inode
 *
 * ��inode��Ϣ���¼�¼��handle->trans->inode_journal->iNodeData��
 *
 * \return AW_OK if success, AW_ERROR if failed
 */
int aw_txfs_update_inode_journal (aw_txfs_handle_t *p_handle,
                                  aw_txfs_dev_t    *p_txfs_dev,
                                  aw_txfs_inode_t  *p_inode);

/** @} */

/******************************************************************************/
/**
 * \brief hrfs_syncer_vnode_interfaces
 * @{
 */

/**
 * \brief reads an inode from disk
 *
 * \param[in] pSyncer   syncer aw_txfs_vnode pointer
 * \param[in] pVolDesc  fs volume pointer
 * \param[in] iNodePtr  memory inode pointer
 *
 * This function reads an inode from the inode table on disk. The inode is
 * converted to host format.
 *
 * \return  AW_OK on success or an errno on failure.
 */
int aw_txfs_inode_get_internal (struct aw_txfs_vnode       *pSyncer,
                                aw_txfs_vol_desc_t *pVolDesc,
                                aw_txfs_inode_t    *iNodePtr);

/**
 * \brief get an TXFS I-Node
 *
 * This function reads an inode from the inode table on disk. The inode is
 * converted to host format. It is a front end function.
 *
 * \return  AW_OK on success or an errno on failure.
 */
int aw_txfs_inode_get (struct aw_txfs_vnode *p_vnode);

/**
 * \brief mount the TXFS file system
 *
 * \param[in] pMount  pointer to mount point
 *
 * This function mounts TXFS. It does the actually instantiation of the TXFS
 * file system.
 *
 * ��������Ҫ�ǳ�ʼ�� �ļ�ϵͳ�豸���󣬼� aw_txfs_dev_t ���еĸ���Ա
 *
 * \return  AW_OK on success or an errno on failure
 */
int aw_txfs_mount (aw_txfs_mount_t *pMount);

/**
 * \brief retrieve statistical information.
 *
 * \param[in] p_mnt       pointer to mount point
 * \param[in] p_stat_vfs  pointer to VFS info structure
 *
 * This function fills out the VFS stat structure.
 *
 * \return  AW_OK always
 */
int aw_txfs_stat (aw_txfs_mount_t *p_mnt, struct aw_statvfs *p_stat_vfs);

/** @} */

/******************************************************************************/
/**
 * \brief hrfs_syncer_vnode_wr_interfaces
 * @{
 */

/**
 * \brief Write an inode to the inode table on disk
 *
 * Writes an inode out to the inode table on disk. The inode is converted to
 * on-disk format.
 *
 * \param[in] p_handle    affair trans handle
 * \param[in] p_syncer    syncer vnode
 * \param[in] p_txfs_dev  TXFS device
 * \param[in] p_inode     inode
 *
 * \return  AW_OK on success or an errno on failure
 */
int aw_txfs_inode_put (aw_txfs_handle_t     *p_handle,
                       struct aw_txfs_vnode *p_syncer,
                       aw_txfs_dev_t        *p_txfs_dev,
                       aw_txfs_inode_t      *p_inode);

/**
 * \brief Write an inode to the inode table on disk
 *
 * \param[in] syncer_vp  aw_txfs_vnode for syncer
 *
 * Writes an inode out to the inode table on disk. The inode is converted to
 * on-disk format. It is only used by hrfsJorunalRewind
 *
 * \return  AW_OK on success or an errno on failure
 */
int aw_txfs_inode_put_rewind (struct aw_txfs_vnode    *syncer_vp,
                              aw_txfs_dev_t   *pHrfsDev,
                              aw_txfs_inode_t *p_inode);

/**
 * \brief Get the specified bit value
 *
 * This function gets the value of the bit specified by bitPos to 1. Numbering
 * is the same as in hrfsSearchOneBitmap()
 *
 * \return  1 of the specified bit is 1 and 0 if bit is zero
 *
 * \example
 * pData[0x01,0x02], == 00000001 00000010
 * checking the bit15 value whether equal to 1, you should using follow
 * hrfsBitGet(pData, 15);
 * not
 * hrfsBitGet(pData, 9);
 *
 * 'bitPos' always counting from left to right(MSB->LSB)
 */
uint32_t aw_txfs_bit_get (unsigned char *p_dat, uint32_t bit_pos);

/**
 * \brief see if a block is pinned.
 *
 * \param[in] blk         data block number
 * \param[in] p_vol_desc  pointer to fs volume
 *
 * \return AW_TRUE if the block is pinned, else AW_FALSE.
 */
aw_bool_t aw_txfs_block_pinned (uint32_t blk, aw_txfs_vol_desc_t *p_vol_desc);

/**
* \brief allocate a block for an I-Node
*
* \param[in] p_inode  pointer to I-Node
* \param[in] p_va     attributes for file
*
* This routine allocates a block for an I-Node.  After a successful allocation,
* it will also initialize the I-Node.
*
* \return   AW_OK if a free inode is available, errno if an error occurs or no
*           free inodes are available.
* - AW_OK if no error
* - errno if fail
* - AW_TXFSLIB_ERR_OUT_OF_INODES no free inode number
*/
int aw_txfs_inode_alloc (aw_txfs_handle_t *p_handle,
                         aw_txfs_dev_t    *p_txfs_dev,
                         aw_txfs_inode_t  *p_inode,
                         aw_txfs_vattr_t  *p_va);

/**
 * \brief allocate a block for data
 *
 * \param[in] p_datablk_num  pointer to data block number
 *
 * This routine allocates a block for data.
 * ��ȡһ�����е����ݿ��data block number
 *
 * \return  AW_OK if a free data block is available, errno if an error occurs or
 *          no free data blocks are available.
 */
int aw_txfs_data_blk_alloc (aw_txfs_handle_t *p_handle,
                            aw_txfs_dev_t    *p_txfs_dev,
                            aw_txfs_inode_t  *p_inode,
                            uint32_t         *p_datablk_num);

/**
 * \brief free an I-Node
 *
 * \param[in] inode_num  I-Node Number
 *
 * This routine frees an I-Node.
 *
 * \return  AW_OK on success or an errno on failure
 */
int aw_txfs_inode_free (aw_txfs_handle_t *p_handle,
                        aw_txfs_dev_t    *p_txfs_dev,
                        uint32_t          inode_num);

/**
 * \brief free a data block
 *
 * \param[in] handle         handle pointer associate with transaction
 * \param[in] pHrfsDev       pointer to TXFS device
 * \param[in] dataBlkNumber  Data Block Number
 *
 * This routine frees a data block.
 *
 * \return  AW_OK on success or an errno on failure
 */
int aw_txfs_data_blk_free (aw_txfs_handle_t *handle,
                           aw_txfs_dev_t    *pHrfsDev,
                           uint32_t          dataBlkNumber);

/** @} */

/******************************************************************************/
/**
 * \brief hrfs_vfs_lib_interfaces
 * @{
 */

/**
 * \brief initializes the file system
 *
 * \param[in] p_mnt  pointer to mount point.
 * \param[in] flags   no used in now.
 *
 * \return  AW_OK on success or an errno on failure.
 */
int aw_txfs_vfs_start (aw_txfs_mount_t *p_mnt, int flags);

/**
 * \brief closes the file system to further operations
 *
 * \param[in] p_mnt  pointer to mount point.
 * \param[in] flags   no used in now.
 *
 * \return  AW_OK always
 */
int aw_txfs_vfs_unmount (aw_txfs_mount_t *p_mnt, int flags);

/**
 * \brief retrieves file system statistics
 *
 * \param[in] pMount    pointer to mount point
 * \param[in] pStatVfs  VFS info structure pointer
 *
 * \return  AW_OK on success or an errno on failure.
 */
int aw_txfs_vfs_stat_vfs (aw_txfs_mount_t *p_mnt, struct aw_statvfs *p_stat_vfs);

/**
 * \brief get a V-Node based upon an I-Node
 *
 * \param[in] pMount       pointer to mount point
 * \param[in] iNodeNumber  inode number
 * \param[out] ppVnode     V-node
 *
 * \return  AW_OK on success or an errno on failure.
 */
int aw_txfs_vfs_vget (aw_txfs_mount_t       *p_mnt,
                      ino_t                  inode_num,
                      struct aw_txfs_vnode **pp_vnode);

/**
* \brief get the root V-Node referenced by <pMount>
*
* \param[in] pMount        pointer to mount point
* \param[out] ppRootVnode  V-node
*
* \return  AW_OK on success or an errno on failure.
*/
int aw_txfs_vfs_root (aw_txfs_mount_t       *p_mnt,
                      struct aw_txfs_vnode **pp_root_vnode);

/**
 * \brief performs general intialization for all FS's of this type
 *
 * returns: N/A
 */
void aw_txfs_vfs_init (void);

/**
 * \brief reinitializes all file systems of this type
 *
 * \return  N/A
 */
void aw_txfs_vfs_reinit (void);

/**
 * \brief finalizes all file systems of this type
 *
 * \return  N/A
 */
void aw_txfs_vfs_done (void);

/** @} */

/******************************************************************************/
/**
 * \brief hrfs_vnode_operation_lib_interfaces
 * @{
 */

/**
 * \brief release the buffers
 *
 * \param[in] p_trie_buf  pointer to aw_txfs_hbuf
 * \param[in] start       start offset of buffers
 * \param[in] end         end offset of buffers
 */
void aw_txfs_free_bufs (struct aw_txfs_hbuf *p_trie_buf[], int start, int end);

/**
 * \brief Getting a physical block number according to file logical block number
 *
 * \param[in] p_syncer         aw_txfs_vnode for syncer
 * \param[in] lbn              logical block number
 * \param[in] p_inode          pointer to inode
 * \param[in] p_vol_desc       pointer to volume
 * \param[out] p_pbn           physical block number in disk
 * \param[in out] pp_trie_buf  indirect index number about logical number
 */
int aw_txfs_find_trie_path (struct aw_txfs_vnode  *p_syncer,
                            uint32_t               lbn,
                            aw_txfs_inode_t       *p_inode,
                            aw_txfs_vol_desc_t    *p_vol_desc,
                            uint32_t              *p_pbn,
                            struct aw_txfs_hbuf  **pp_trie_buf);

/**
 * \brief checking assigned inode number whether to being delete
 *        ���inodeNumber�Ƿ񽫱�ɾ�����ھ�Ĵ�ɾ��inode�����в���
 *        pVolDesc->to_be_del_list
 * \param[in] p_vol_desc  TXFS volume descriptor structure.
 * \param[in] inode_num   inode number
 *
 * \return AW_TRUE if inode number to being delete, else AW_FALSE.
 */
aw_bool_t aw_txfs_inode_to_be_del (aw_txfs_vol_desc_t *p_vol_desc,
                                uint32_t            inode_num);

/** \brief  */
int aw_txfs_vop_read_inactive (struct aw_txfs_vnode *p_vnode);

/** \brief aw_txfs_vnode print function  */
void aw_txfs_vop_print (struct aw_txfs_vnode *p_vnode);

/**
 * \brief read the directory item from a directory according to the offset
 *
 * \param[in] p_dvnode              aw_txfs_vnode associate with directory
 * \param[out] p_formatted_dir_ent  directory item what our find
 * \param[in] p_cred                credential
 * \param[out] p_eof_flag           a flag indicate whether out off the
 *                                  directory size
 * \param[in out] p_dir_offset      start offset of finding
 */
int aw_txfs_vop_read_dir (struct aw_txfs_vnode *p_dvnode,
                          struct aw_dirent     *p_formatted_dir_ent,
                          aw_txfs_ucred_t      *p_cred,
                          int                  *p_eof_flag,
                          int                  *p_dir_offset);

/**
 * \brief Get file attribute
 *
 * \param[in] p_vnode  vnode
 * \param[in] p_vattr  file vnode attribute
 * \param[in] p_cred   credential
 *
 * \return AW_OK
 */
int aw_txfs_vop_get_attr (struct aw_txfs_vnode *p_vnode,
                          aw_txfs_vattr_t      *p_vattr,
                          aw_txfs_ucred_t      *p_cred);

/**
 * \brief Find a file.
 *        According to aw_txfs_vnode of directory and the path/file name
 *
 * \param[in]  p_dvnode   aw_txfs_vnode of directory to find the file in
 * \param[out] pp_fvnode  aw_txfs_vnode for found file/directory
 * \param[in]  p_cn       file/path name info
 *
 * \return
 * - AW_OK if success
 * - ENAMETOOLONG name too long
 * - ENOENT
 * - errno
 */
int aw_txfs_vop_lookup (struct aw_txfs_vnode          *p_dvnode,
                        struct aw_txfs_vnode         **pp_fvnode,
                        struct aw_txfs_componentname  *p_cn);

/**
 * \brief read disk data to buffer according to the physical block number
 *
 * \param[in] p_syncer  syncer aw_txfs_vnode pointer
 * \param[in] p_buf     pointer to buffer
 * \param[in] pbn       physical block number
 */
void aw_txfs_vop_internal_read_strategy (struct aw_txfs_vnode *p_syncer,
                                         struct aw_txfs_buf   *p_buf,
                                         uint32_t              pbn);

/**
 * \brief Read the logical block for a dir or a file
 *
 * \param[in] p_vnode  aw_txfs_vnode of a file
 * \param[in] p_buf    buf associated with file
 *
 * We don't need aw_txfs_protect_begin() and aw_txfs_protect_end() here, since
 * the caller has already done this.
 */
void aw_txfs_vop_read_strategy (struct aw_txfs_vnode *p_vnode,
                                struct aw_txfs_buf   *p_buf);

/**
 * \brief get dirent corresponding to dirSlot in pDirVnode.
 *
 * \param[in] p_dir_vnode         directory aw_txfs_vnode pointer
 * \param[in out] pp_out_buf      pointer to aw_txfs_hbuf
 * \param[in out] pp_out_dir_ent  aw_txfs_dirent_t pointer what you want
 *
 * \return  AW_OK if no error encountered.
 */
int aw_txfs_dir_ent_from_slot (struct aw_txfs_vnode  *p_dir_vnode,
                               struct aw_txfs_hbuf  **pp_out_buf,
                               aw_txfs_dirent_t     **pp_out_dir_ent);

/**
 * \brief POSIX advisory file locking.
 *
 * All the real work is in lf_advlock; we just have to supply the
 * file size (per the in-core inode).
 */
int aw_txfs_vop_adv_lock (struct aw_txfs_vnode *p_vnode,
                          void                 *p_id,
                          int                   op,
                          struct aw_txfs_flock *p_fl,
                          int                   flags);

/**
 * \brief TXFS aw_txfs_vnode pathconf function
 *
 * \param[in] name     require name in pathconf
 * \param[out] p_ret_val  out value associate with name parameter
 *
 * \return AW_OK if success, EINVAL if failed
 */
int aw_txfs_vop_path_conf (struct aw_txfs_vnode *p_vnode,
                           int                   name,
                           long                 *p_ret_val);

/** @} */

/******************************************************************************/
/**
 * \brief hrfs_vnode_operation_write_lib_interfaces
 * @{
 */

/**
 * \brief aw_txfs_vnode activate - initial aw_txfs_vnode.
 *
 * \param[in] p_vnode  pointer to aw_txfs_vnode
 *
 * \return AW_OK
 */
int aw_txfs_vop_activate (struct aw_txfs_vnode *p_vnode);

/**
 * \brief Read from a file
 *
 * \param[in] p_vnode  file vnode
 * \param[in] p_uio    user transfer io info
 * \param[in] ioflag   no using now
 * \param[in] p_cred   credentials of caller
 *
 * \return errno
 */
int aw_txfs_file_vop_read (struct aw_txfs_vnode *p_vnode,
                           struct aw_txfs_uio   *p_uio,
                           int                   ioflag,
                           aw_txfs_ucred_t      *p_cred);

/**
* \brief Check access permissions
* \param[in] p_vnode  file vnode
* \param[in] mode     file mode(credential R W X) what our want to check
* \param[in] p_cred   credentials of caller
*
* \return errno, AW_OK if have the file mode, EACCES if have not credential
*         for the file
*/
int aw_txfs_vop_access (struct aw_txfs_vnode *p_vnode,
                        int                   mode,
                        aw_txfs_ucred_t      *p_cred);

/**
 * \brief internal truncation function
 *
 * This routine determines the range of blocks to be deleted from a file
 * based on the new length of the file. This function is a nop if the file
 * is getting bigger or the number of blocks stays the same.
 *
 * \param[in] p_handle    transfer transaction handle
 * \param[in] p_txfs_dev  TXFS device
 * \param[in] p_vnode     aw_txfs_vnode of the file
 * \param[in] p_cred      Credentials of caller
 * \param[in] new_length  new length for the file
 *
 * \return  AW_OK on success, errno otherwise
 */
int aw_txfs_internal_truncate (aw_txfs_handle_t     *p_handle,
                               aw_txfs_dev_t        *p_txfs_dev,
                               struct aw_txfs_vnode *p_vnode,
                               aw_txfs_ucred_t      *p_cred,
                               voff_t                new_length);

/**
 * \brief file/directory creation
 *        This routine is the front end for file creation.
 *
 * \param[in] p_dvnode       directory vnode that file was created under here
 * \param[in out] pp_fvnode  the created file vnode
 * \param[in] p_cn           name component pointer
 * \param[in] p_va           attributes for the new file
 *
 * returns: AW_OK on success, errno otherwise
 */
int aw_txfs_vop_create (struct aw_txfs_vnode          *p_dvnode,
                        struct aw_txfs_vnode         **pp_fvnode,
                        struct aw_txfs_componentname  *p_cn,
                        aw_txfs_vattr_t               *p_va);

/**
 * \brief make a directory
 *        This routine creates a directory.
 *
 * \param[in] p_dvnode  directory vnode that directory was created under here
 * \param[in] pp_vnode  directory vnode was created
 * \param[in] p_cn      name component pointer
 * \param[in] p_va      attributes for the new directory
 *
 * \return  AW_OK on success, errno otherwise
 */
int aw_txfs_vop_mkdir (struct aw_txfs_vnode          *p_dvnode,
                       struct aw_txfs_vnode         **pp_vnode,
                       struct aw_txfs_componentname  *p_cn,
                       aw_txfs_vattr_t               *p_va);

/**
 * \brief add a dirEnt for the I-Node specified by <pVnode> and <pName>
 *
 * \param[in] p_dir_vnode  aw_txfs_vnode for directory
 * \param[in] p_vnode      aw_txfs_vnode for a file
 * \param[in] p_name       file name
 *
 * This routine adds a directory entry for the I-Node specified by <pVnode>
 * and <pName> in the directory specified by <pDirVnode>.
 *
 * \note Do not make a link for a directory
 *
 * returns: AW_OK on success, errno otherwise
 */
int aw_txfs_vop_link (struct aw_txfs_vnode         *p_dir_vnode,
                      struct aw_txfs_vnode         *p_vnode,
                      struct aw_txfs_componentname *p_name);

/**
 * \brief delete a directory
 *
 * \param[in] p_dvnode  aw_txfs_vnode of directory to find the dir in
 * \param[in] p_vnode   aw_txfs_vnode of sub-directory to remove
 * \param[in] p_cn      name component pointer
 *
 * This routine deletes a directory.
 *
 * \return  AW_OK on success, errno otherwise
 */
int aw_txfs_vop_rmdir (struct aw_txfs_vnode         *p_dvnode,
                       struct aw_txfs_vnode         *p_vnode,
                       struct aw_txfs_componentname *p_cn);

/**
 * \brief rename a file or directory.
 *
 * \param[in] p_fdvnode  aw_txfs_vnode of directory of from-file
 * \param[in] p_fvnode   from-file
 * \param[in] p_fcn      from-name component pointer
 * \param[in] p_tdvnode  aw_txfs_vnode of directory of to-file
 * \param[in] p_tvnode   to-file
 * \param[in] p_tcn      to-name component pointer
 *
 * Note that fdvp and fvp are both unlocked, while tdvp and tvp are
 * both locked (except that tvp can be NULL).  We must vput tdvp and tvp,
 * and leave fdvp and fvp unlocked, on return.
 *
 * This is a horribly complicated routine.  I apologize for the
 * complexity, but it just seems inherently hard.
 *
 * ��Ϻ��� rename ���˺�����ʵ��ϸ�ڣ�
 * �����ע����  /usr/src  ->  /temp/dst Ϊ����
 * /usr  : ԴĿ¼��      /src : Դ�ļ�
 * /temp : Ŀ��Ŀ¼      /dst : Ŀ���ļ�
 *
 * \return  AW_OK on success, errno otherwise
 */
int aw_txfs_vop_rename (struct aw_txfs_vnode         *p_fdvnode,
                        struct aw_txfs_vnode         *p_fvnode,
                        struct aw_txfs_componentname *p_fcn,
                        struct aw_txfs_vnode         *p_tdvnode,
                        struct aw_txfs_vnode         *p_tvnode,
                        struct aw_txfs_componentname *p_tcn);

/**
 * \brief remove a directory entry specified by <pVnode> and <pName>
 *
 * \param[in] p_dir_vnode  pointer to aw_txfs_vnode of parent directory
 * \param[in] p_vnode      pointer to aw_txfs_vnode of directory or file
 * \param[in] p_name       directory or file name
 *
 * This routine removes I-Node directory entry specified by <pVnode> and <pName>
 * in the directory specified by <pDirVnode>.
 *
 * \return  AW_OK on success, errno otherwise
 */
int aw_txfs_vop_unlink (struct aw_txfs_vnode         *p_dir_vnode,
                        struct aw_txfs_vnode         *p_vnode,
                        struct aw_txfs_componentname *p_name);

/**
 * \brief truncate a file
 *
 * \param[in] p_vnode     pointer to aw_txfs_vnode
 * \param[in] new_length  new length of file
 * \param[in] flags       flags - no using
 * \param[in] p_ucred     Credentials of caller
 *
 * This routine is the front end to file truncation.
 *
 * \return  AW_OK on success, errno otherwise
 */
int aw_txfs_vop_truncate (struct aw_txfs_vnode *p_vnode,
                          voff_t                new_length,
                          int                   flags,
                          aw_txfs_ucred_t      *p_ucred);

/**
 * \brief write bytes to a file
 *
 * \param[in] p_vnode  pointer to aw_txfs_vnode
 * \param[in] p_uio    aw_txfs_uio pointer
 * \param[in] ioflag   flags
 * \param[in] p_cred   Credentials of caller
 *
 * \return  AW_OK on success, errno otherwise
 */
int aw_txfs_file_vop_write (struct aw_txfs_vnode *p_vnode,
                            struct aw_txfs_uio   *p_uio,
                            int                   ioflag,
                            aw_txfs_ucred_t      *p_cred);

/**
 * \brief mark a file (inode) as being inactive.
 *
 * \param[in] p_vnode  pointer to aw_txfs_vnode
 *
 * \return  AW_OK inode no in delete list and it state is no free,
 *         -AW_EBADF inode has inactive and free, or other error.
 */
int aw_txfs_vop_inactive (struct aw_txfs_vnode *p_vnode);

/**
 * \brief set the attributes of a file
 *        This routine sets the attributes of a file's inode.
 *
 * \param[in] p_vattr  Pointer to v-node
 * \param[in] p_vattr  attributes want to setting
 * \param[in] p_cred   Credentials of caller
 *
 * \return  AW_OK on success, errno otherwise
 */
int aw_txfs_vop_set_attr (struct aw_txfs_vnode *p_vnode,
                          aw_txfs_vattr_t      *p_vattr,
                          aw_txfs_ucred_t      *p_cred);

/**
 * \brief Close a file and flush the inode to disk
 *
 * \param[in] p_vnode  Pointer to v-node
 * \param[in] fflag    flags
 * \param[in] p_cred   Credentials of caller
 *
 * This operator closes the currently open file and calls hrfsInodePut()
 * to write out any pending modifications to the in-core inode.  The only
 * pending modification should be the access time stamp since we don't
 * want to flush out the inode on every access.
 *
 * \return  AW_OK on success, errno otherwise
 */
int aw_txfs_vop_close_with_update (struct aw_txfs_vnode *p_vnode,
                                   int                   fflag,
                                   aw_txfs_ucred_t      *p_cred);

/**
 * \brief ioctl commands function
 *
 * \param[in] p_vnode    pointer to aw_txfs_vnode
 * \param[in] command    commmand
 * \param[in out] p_dat  return value associate with command
 * \param[in] fflag      flags
 * \param[in] p_cred     Credentials
 *
 * \return  AW_OK on success, errno otherwise
 */
int aw_txfs_vop_ioctl (struct aw_txfs_vnode *p_vnode,
                       u_long                command,
                       void                 *p_dat,
                       int                   fflag,
                       aw_txfs_ucred_t      *p_cred);

/**
 * \brief Open a file - This is a nop for TXFS
 *
 * \param[in] p_vnode  file of vnode
 * \param[in] mode     file of mode
 * \param[in] p_cred   Credentials
 *
 * \return AW_OK
 */
int aw_txfs_vop_open (struct aw_txfs_vnode *p_vnode,
                      int                   mode,
                      aw_txfs_ucred_t      *p_cred);

/**
 * \brief Close a file - This is a nop for TXFS
 *
 * \param[in] p_vnode  file of vnode
 * \param[in] fflag    flags
 * \param[in] p_cred   Credentials
 *
 * \return AW_OK
 */
int aw_txfs_vop_close (struct aw_txfs_vnode *p_vnode,
                       int                   fflag,
                       aw_txfs_ucred_t      *p_cred);

/**
 * \brief sync operation
 *        Wait for transaction commit finishing
 *
 * \param[in] p_vnode  file of vnode
 * \param[in] p_cred   Credentials
 * \param[in] flags    flags
 *
 * \return AW_OK
 */
int aw_txfs_vop_fsync (struct aw_txfs_vnode *p_vnode,
                       aw_txfs_ucred_t      *p_cred,
                       int                   flags);

/**
 * \brief aw_txfs_vnode seek operation - his is a nop for TXFS
 *
 * \param[in] p_vnode  file of vnode
 * \param[in] oldoff   old offset value
 * \param[in] newoff   new offset value
 * \param[in] p_cred   Credentials
 *
 * \return AW_OK
 */
int aw_txfs_vop_seek (struct aw_txfs_vnode *p_vnode,
                      voff_t                oldoff,
                      voff_t                newoff,
                      aw_txfs_ucred_t      *p_cred);

/**
 * \brief aw_txfs_vnode operation abort - his is a nop for TXFS
 *
 * \param[in] p_vnode  file of vnode
 * \param[in] p_cn     file of component name
 *
 * \return AW_OK
 */
int aw_txfs_vop_abort (struct aw_txfs_vnode         *p_vnode,
                       struct aw_txfs_componentname *p_cn);

/** @} */
/******************************************************************************/
/**
 * \brief txfs_wr_lib interface
 * @{
 */
/**
 * \brief Initialize the TXFS lib.
 *        Setting the max buffers and files in TXFS.
 *
 * \param[in] max_bufs  supporting buffers in FS
 * \param[in] max_files supporting files in FS
 *
 * \return  AW_OK on success or AW_ERROR otherwise
 */
//aw_err_t aw_txfs_wr_lib_init (int max_bufs, int max_files);
/** @} */
/******************************************************************************/
/**
 * \brief extension variables
 * @{
 */

/** \brief ����defer���� */
extern struct aw_defer g_txfs_defer;

/** \brief ����ʱ����Ƿ�ʹ�� */
extern aw_bool_t aw_txfs_access_timestamp_en;

/** \brief ����дģʽʹ�ܱ�־ */
extern aw_bool_t aw_txfs_highspeed_wr_mode;

/** \brief �ļ�ϵͳ֧�ֵĻ������������ */
extern int aw_txfs_max_bufs;

/** \brief �ļ�ϵͳ��ǰ������������ */
extern int aw_txfs_cur_bufs;

/* global tables for txfs aw_txfs_vnode operations
 * ������Щ�ⲿ������ȫ�ֵģ�Ӧ�������ļ�ϵͳӦ�����ⲿ���岢��ʼ������ʼ����ֵ
 * �Ǻ����оٵ����������������-�ֱ��Ƕ�д(R/W)�Լ�ֻ��(R/O)������
 *
 * \example ��ʼ���ļ�ϵͳ�ɶ�д��ʾ����
 *
 * const struct aw_txfs_vfsops    *txfsVfsOps     = aw_txfs_vfs_ops_config_rw;
 * const struct aw_txfs_vnode_ops *txfsOps        = aw_txfs_ops_config_rw;
 * const struct aw_txfs_vnode_ops *txfsDirOps     = aw_txfs_dir_ops_config_rw;
 * const struct aw_txfs_vnode_ops *txfsFileOps    = aw_txfs_file_ops_config_rw;
 * const struct aw_txfs_vnode_ops *txfsSymLinkOps = aw_txfs_symlink_ops_config_rw;
 */
/** \brief VFS operations */
extern const struct aw_txfs_vfsops    *aw_txfs_vfs_ops;

/** \brief aw_txfs_vnode normal operations */
extern const struct aw_txfs_vnode_ops *aw_txfs_ops;

/** \brief aw_txfs_vnode directory operations */
extern const struct aw_txfs_vnode_ops *aw_txfs_dir_ops;

/** \brief aw_txfs_vnode file operations */
extern const struct aw_txfs_vnode_ops *aw_txfs_file_ops;

/** \brief aw_txfs_vnode symbolic link operations */
extern const struct aw_txfs_vnode_ops *aw_txfs_symlink_ops;

/** \brief R/W aw_txfs_vnode operator table definition */
extern const struct aw_txfs_vfsops    aw_txfs_vfs_ops_config_rw;
extern const struct aw_txfs_vnode_ops aw_txfs_ops_config_rw;
extern const struct aw_txfs_vnode_ops aw_txfs_dir_ops_config_rw;
extern const struct aw_txfs_vnode_ops aw_txfs_file_ops_config_rw;
extern const struct aw_txfs_vnode_ops aw_txfs_symlink_ops_config_rw;

/** \brief R/O aw_txfs_vnode operator table definition */
extern const struct aw_txfs_vfsops    aw_txfs_vfs_ops_config_ro;
extern const struct aw_txfs_vnode_ops aw_txfs_ops_config_ro;
extern const struct aw_txfs_vnode_ops aw_txfs_dir_ops_config_ro;
extern const struct aw_txfs_vnode_ops aw_txfs_file_ops_config_ro;
extern const struct aw_txfs_vnode_ops aw_txfs_symlink_ops_config_ro;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AW_TXFS_LIB_PRIVATE_H */

/* end of file */
