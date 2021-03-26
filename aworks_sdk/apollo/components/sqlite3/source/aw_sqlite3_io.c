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
 * \brief Sqlite3's vfs implementation.(AWorks platform)
 *
 * \internal
 * \par modification history:
 * - 1.00 16-01-04  cyl, first implementation.
 * \endinternal
 */


#include "apollo.h"
#include "aw_sqlite3_config.h"
#include "sqlite3.h"
#include "aw_vdebug.h"
#include "string.h"

#ifdef SQLITE_OS_AWORKS

/******************************************************************************/

#include "aw_task.h"
#include "aw_time.h"
#include "aw_assert.h"
#include "aw_errno.h"
#include "io/aw_unistd.h"
#include "io/aw_fcntl.h"
#include "io/sys/aw_stat.h"

#include "aw_sem.h"

/** \brief the max length of full path name */
#define AW_SQLITE3_MAX_PATH 256


/**
 * \name Allowed values for the aw_sqlite3_file.ctrl_flags bitmask.
 * @{
 */

/** \brief Persistent WAL mode */
#define AWFILE_PERSIST_WAL     0x04

/** \brief SQLITE_IOCAP_POWERSAFE_OVERWRITE */
#define AWFILE_PSOW            0x10

/**
 * @}
 */


/**
 * \brief The aw_sqlite3_file structure is subclass of sqlite3_file specific
 *        to the AWorks VFS implementations.
 */
typedef struct aw_sqlite3_file aw_sqlite3_file;
struct aw_sqlite3_file {

    /** \brief Always the first entry */
    const sqlite3_io_methods *p_method;

    /** \brief The VFS used to open this file */
    sqlite3_vfs        *p_vfs;

    /** \brief The file descriptor */
    int                 fd;

    /** \brief The type of lock held on this fd */
    uint8_t             lock_type;

    /** \brief Behavioral bits.  AWFILE_* flags */
    uint8_t             ctrl_flags;

    /** \brief The AWorks errno from last IO error */
    int                 last_errno;

    /** \brief full pathname of this file */
    const char         *path;

    /** \brief configured by FCNTL_CHUNK_SIZE */
    int                 chunk;
};


/** \brief forward declaration */
static int __sqlite3_tempname_get (int nbuf, char *pbuf);


/** \brief delay function(ms level) */
static void __sqlite3_delay (uint32_t nms)
{
    aw_task_delay(aw_ms_to_ticks(nms));
}

/**
 * \brief Close a file.
 *
 * It is reported that an attempt to close a handle might sometimes
 * fail.  This is a very unreasonable result, but Windows is notorious
 * for being unreasonable so I do not doubt that it might happen.  If
 * the close fails, we pause for 100 milliseconds and try again.  As
 * many as MAX_CLOSE_ATTEMPT attempts to close the handle are made before
 * giving up and returning an error.
 */
#define MAX_CLOSE_ATTEMPT 3
static int __sqlite3_io_close (sqlite3_file *id)
{
    int rc, cnt = 0;

    aw_sqlite3_file *pfile = (aw_sqlite3_file *)id;

    aw_assert(id!=0);
    aw_assert(pfile->fd != -1);

#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("CLOSE tid=%lu, pfile=%p, file=%d\n",
              aw_task_id_self(),
              pfile,
              pfile->fd));
#endif

    do {
        rc = aw_close(pfile->fd);
    } while (rc != 0 &&
             ++cnt < MAX_CLOSE_ATTEMPT &&
             (__sqlite3_delay(100), 1));

    if (rc) {
        pfile->fd = 0;
    }

#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("CLOSE tid=%lu, pfile=%p, file=%d, rc=%s\n",
              aw_task_id_self(),
              pfile,
              pfile->fd,
              rc ? "ok" : "failed"));
#endif

    return rc;
}


/**
 * \brief Read data from a file into a buffer.  Return SQLITE_OK if all
 *        bytes were read successfully and SQLITE_IOERR if anything goes
 *        wrong.
 */
static int __sqlite3_io_read (sqlite3_file  *id,
                              void          *pbuf,
                              int            nbytes,
                              sqlite3_int64  offset)
{
    aw_sqlite3_file *pfile = (aw_sqlite3_file *)id;

    aw_assert(id != 0);
    aw_assert(pfile->fd != -1);
    aw_assert(pbuf != 0);
    aw_assert(nbytes > 0);
    aw_assert(offset >= 0);

#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("READ tid=%lu, pfile=%p, file=%d, buffer=%p, amount=%d, "
              "offset=%lld, lock=%d\n",
              aw_task_id_self(),
              pfile,
              pfile->fd,
              pbuf,
              nbytes,
              offset,
              pfile->lock_type));
#endif

    if (-1 == aw_lseek(pfile->fd, offset, SEEK_SET)) {
        return SQLITE_IOERR_SEEK;
    }

    if (nbytes != aw_read(pfile->fd, pbuf, nbytes)) {

        AW_ERRNO_GET(pfile->last_errno);

#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("READ tid=%lu, pfile=%p, file=%d, "
                  "rc=SQLITE_IOERR_SHORT_READ\n",
                  aw_task_id_self(),
                  pfile,
                  pfile->fd));
#endif

        return SQLITE_IOERR_SHORT_READ;
    }

    return SQLITE_OK;
}


/**
 * \brief Write data from a buffer into a file.  Return SQLITE_OK on success
 *        or some other error code on failure.
 */
static int __sqlite3_io_write (sqlite3_file  *id,
                               const void    *pbuf,
                               int            nbytes,
                               sqlite3_int64  offset)
{
    aw_sqlite3_file *pfile = (aw_sqlite3_file *)id;

    aw_assert(id != 0);
    aw_assert(pfile->fd != -1);
    aw_assert(pbuf != 0);
    aw_assert(nbytes > 0);
    aw_assert(offset >= 0);

#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("WRITE tid=%lu, pfile=%p, file=%d, buffer=%p, amount=%d, "
             "offset=%lld, lock=%d\n",
             aw_task_id_self(),
             pfile,
             pfile->fd,
             pbuf,
             nbytes,
             offset,
             pfile->lock_type));
#endif

    if (-1 == aw_lseek(pfile->fd, offset, SEEK_SET)) {
        return SQLITE_IOERR_SEEK;
    }

    if (nbytes != aw_write(pfile->fd, pbuf, nbytes)) {

        AW_ERRNO_GET(pfile->last_errno);

#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("WRITE tid=%lu, pfile=%p, file=%d, rc=SQLITE_IOERR_WRITE\n",
                  aw_task_id_self(),
                  pfile,
                  pfile->fd));
#endif

        return SQLITE_IOERR_WRITE;

    }

    return SQLITE_OK;
}

/**
 * \brief Truncate an open file to a specified size
 */
static int __sqlite3_io_truncate (sqlite3_file  *id,
                                  sqlite3_int64  nbytes)
{
    aw_sqlite3_file *pfile = (aw_sqlite3_file *)id;

    aw_assert(id != 0);
    aw_assert(pfile->fd != -1);
    aw_assert(nbytes > 0);

#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("TRUNCATE tid=%lu, pfile=%p, file=%d, size=%lld, lock=%d\n",
              aw_task_id_self(),
              pfile,
              pfile->fd,
              nbytes,
              pfile->lock_type));
#endif

    /**
     * If the user has configured a chunk-size for this file, truncate the
     * file so that it consists of an integer number of chunks (i.e. the
     * actual file size after the operation may be larger than the requested
     * size).
     */
    if (pfile->chunk > 0) {
        nbytes = ((nbytes + pfile->chunk - 1) / pfile->chunk) * pfile->chunk;
    }

    if (-1 == aw_ftruncate(pfile->fd, nbytes)) {

        AW_ERRNO_GET(pfile->last_errno);

#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("TRUNCATE tid=%lu, pfile=%p, file=%d, "
                  "rc=SQLITE_IOERR_TRUNCATE\n",
                  aw_task_id_self(),
                  pfile,
                  pfile->fd));
#endif

        return SQLITE_IOERR_TRUNCATE;

    }

    return SQLITE_OK;
}


#ifdef SQLITE_TEST
/**
 * \brief Count the number of fullsyncs and normal syncs.  This is used to test
 *        that syncs and fullsyncs are occuring at the right times.
 */
SQLITE_API int sqlite3_sync_count     = 0;
SQLITE_API int sqlite3_fullsync_count = 0;
#endif

/**
 * \brief Make sure all writes to a particular file are committed to disk.
 */
static int __sqlite3_io_sync (sqlite3_file *id, int flags)
{
    aw_sqlite3_file *pfile = (aw_sqlite3_file *)id;

    aw_assert(id != 0);
    aw_assert(pfile->fd != -1);

    /* Check that one of SQLITE_SYNC_NORMAL or FULL was passed */
    aw_assert((flags & 0x0F) == SQLITE_SYNC_NORMAL ||
              (flags & 0x0F) == SQLITE_SYNC_FULL);

#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("SYNC tid=%lu, pfile=%p, file=%d, flags=%x, lock=%d\n",
             aw_task_id_self(),
             pfile,
             pfile->fd,
             flags,
             pfile->lock_type));
#endif

#ifndef SQLITE_TEST
    (void)flags;
#else
    if ((flags & 0x0F) == SQLITE_SYNC_FULL) {
        sqlite3_fullsync_count++;
    }
    sqlite3_sync_count++;
#endif

    /**
     * If we compiled with the SQLITE_NO_SYNC flag, then syncing is a no-op
     */
#ifdef SQLITE_NO_SYNC
    return SQLITE_OK;
#else
    /* aw_fsync() returns zero when successful, or zero when it fails. */
    if (aw_fsync(pfile->fd)) {

        AW_ERRNO_GET(pfile->last_errno);

#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("SYNC tid=%lu, pfile=%p, file=%d, rc=SQLITE_IOERR_FSYNC\n",
                 aw_task_id_self(),
                 pfile,
                 pfile->fd));
#endif

        return SQLITE_IOERR_FSYNC;

    } else {
        return SQLITE_OK;
    }
#endif
}

/**
 * \brief Determine the current size of a file in bytes
 */
static int __sqlite3_io_filesize (sqlite3_file  *id,
                                  sqlite3_int64 *psize)
{
    struct aw_stat   filestat;
    aw_sqlite3_file *pfile = (aw_sqlite3_file *)id;

    aw_assert(id != 0);
    aw_assert(pfile->fd != -1);
    aw_assert(psize != 0);

#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("SIZE file=%d, psize=%p\n",
              pfile->fd,
              psize));
#endif

    if (0 == aw_fstat(pfile->fd, &filestat)) {
        *psize = filestat.st_size;

#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("SIZE file=%d, psize=%p, *psize=%d, rc=SQLITE_OK\n",
                  pfile->fd,
                  psize,
                  *psize));
#endif

        return SQLITE_OK;
    } else {
        *psize = 0;
        return SQLITE_IOERR_FSTAT;
    }
}

/** \brief file lock, AWorks platform unsupport. */
static int __sqlite3_io_lock (sqlite3_file *id, int lock_type)
{
    return SQLITE_OK;
}

/** \brief file unlock, AWorks platform unsupport. */
static int __sqlite3_io_unlock (sqlite3_file *id, int lock_type)
{
    return SQLITE_OK;
}

/** \brief check reserved lock, AWorks platform unsupport. */
static int __sqlite3_io_reservedlock_check (sqlite3_file *id, int *p_resout)
{
    return SQLITE_OK;
}


/**
 * \brief
 * If *p_arg is inititially negative then this is a query.  Set *p_arg to
 * 1 or 0 depending on whether or not bit mask of pfile->ctrl_flags is set.
 *
 * If *p_arg is 0 or 1, then clear or set the mask bit of pfile->ctrl_flags.
 */
static void __sqlite3_modebit (aw_sqlite3_file *pfile,
                               unsigned char    mask,
                               int             *p_arg)
{
    if (*p_arg < 0) {
        *p_arg = (pfile->ctrl_flags & mask) != 0;
    } else if ((*p_arg) == 0) {
        pfile->ctrl_flags &= ~mask;
    } else {
        pfile->ctrl_flags |= mask;
    }
}

/**
 * \brief Control and query of the open file handle.
 */
static int __sqlite3_io_file_ctrl (sqlite3_file *id, int op, void *p_arg)
{
    int   rc    = 0;
    char *tfile = 0;

    aw_sqlite3_file *pfile = (aw_sqlite3_file *)id;

#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("FCNTL file=%d, op=%d, p_arg=%p\n",
             pfile->fd,
             op,
             p_arg));
#endif

    switch (op) {

    case SQLITE_FCNTL_LOCKSTATE:

        *(int *)p_arg = pfile->lock_type;
#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("FCNTL file=%d, rc=SQLITE_OK\n",
                  pfile->fd));
#endif
        return SQLITE_OK;

    case SQLITE_LAST_ERRNO:

        *(int *)p_arg = pfile->last_errno;
#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("FCNTL file=%d, rc=SQLITE_OK\n",
                  pfile->fd));
#endif
        return SQLITE_OK;

    case SQLITE_FCNTL_CHUNK_SIZE:

        pfile->chunk = *(int *)p_arg;
#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("FCNTL file=%d, rc=SQLITE_OK\n",
                  pfile->fd));
#endif
        return SQLITE_OK;

    case SQLITE_FCNTL_SIZE_HINT:

        if (pfile->chunk > 0) {
            sqlite3_int64 old_size;
            rc = __sqlite3_io_filesize(id, &old_size);
            if (SQLITE_OK == rc) {
                sqlite3_int64 new_size = *(sqlite3_int64*)p_arg;
                if (new_size > old_size) {
                    rc = __sqlite3_io_truncate(id, new_size);
                }
            }
#ifdef SQLITE_AWORKS_DEBUG
            AW_INFOF(("FCNTL file=%d\n",
                      pfile->fd));
#endif
            return rc;
        }

#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("FCNTL file=%d, rc=SQLITE_OK\n",
                  pfile->fd));
#endif
        return SQLITE_OK;

    case SQLITE_FCNTL_PERSIST_WAL:

        __sqlite3_modebit(pfile,
                          AWFILE_PERSIST_WAL,
                          (int*)p_arg);
#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("FCNTL file=%d, rc=SQLITE_OK\n",
                 pfile->fd));
#endif
        return SQLITE_OK;

    case SQLITE_FCNTL_POWERSAFE_OVERWRITE:

        __sqlite3_modebit(pfile,
                          AWFILE_PSOW,
                          (int*)p_arg);
#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("FCNTL file=%d, rc=SQLITE_OK\n",
                  pfile->fd));
#endif
        return SQLITE_OK;

    case SQLITE_FCNTL_VFSNAME:

        *(char**)p_arg = sqlite3_mprintf("%s", pfile->p_vfs->zName);
#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("FCNTL file=%d, rc=SQLITE_OK\n",
                  pfile->fd));
#endif
        return SQLITE_OK;

    case SQLITE_FCNTL_TEMPFILENAME:

        //todo: dangerous!!!! memory overflow maybe
        tfile = sqlite3_malloc(pfile->p_vfs->mxPathname);
        rc = __sqlite3_tempname_get(pfile->p_vfs->mxPathname, tfile);
        if (rc == SQLITE_OK) {
            *(char**)p_arg = tfile;
        }
#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("FCNTL file=%d\n",
                  pfile->fd));
#endif
        return rc;

    default:
#ifdef SQLITE_AWORKS_DEBUG
        AW_INFOF(("FCNTL file=%d,  rc=SQLITE_NOTFOUND\n",
                  pfile->fd));
#endif
        return SQLITE_NOTFOUND;
    }

}

/**
 * \brief Return the sector size in bytes of the underlying block device for
 *        the specified file.
 */
static int __sqlite3_io_sectorsize (sqlite3_file *id)
{
    (void)id;
    /* define SQLITE_DEFAULT_SECTOR_SIZE 4096 */
    return 4096;
}

/**
 * \brief Return a vector of device characteristics.
 */
static int __sqlite3_io_device_characteristics (sqlite3_file *id)
{
    aw_sqlite3_file *pfile = (aw_sqlite3_file*)id;

    return SQLITE_IOCAP_UNDELETABLE_WHEN_OPEN |
     ((pfile->ctrl_flags & AWFILE_PSOW) ? SQLITE_IOCAP_POWERSAFE_OVERWRITE : 0);
}



/**
 * \brief This vector defines all the methods that can operate on an
 *        sqlite3_file for AWorks.
 */
static const sqlite3_io_methods __io_methods = {
    1,
    __sqlite3_io_close,
    __sqlite3_io_read,
    __sqlite3_io_write,
    __sqlite3_io_truncate,
    __sqlite3_io_sync,
    __sqlite3_io_filesize,
    __sqlite3_io_lock,
    __sqlite3_io_unlock,
    __sqlite3_io_reservedlock_check,
    __sqlite3_io_file_ctrl,
    __sqlite3_io_sectorsize,
    __sqlite3_io_device_characteristics,
    0,
    0,
    0,
    0,
    0,
    0
};


/************************** VFS ***********************************************/


/*
 * Create a temporary file name in zBuf.  zBuf must be big enough to
 * hold at pVfs->mxPathname characters.
 */
static int __sqlite3_tempname_get (int nbuf, char *pbuf)
{
    static char chars[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    size_t i, j;
    int temppath;

    /*todo: notice!!!!*/
    char str_temppath[AW_SQLITE3_MAX_PATH + 2];

    memset(str_temppath, 0, AW_SQLITE3_MAX_PATH + 2);

    if (sqlite3_temp_directory) {
        sqlite3_snprintf(AW_SQLITE3_MAX_PATH - 30,
                         str_temppath,
                         "%s",
                         sqlite3_temp_directory);
    }

    /**
     * Check that the output buffer is large enough for the temporary file
     * name. If it is not, return SQLITE_ERROR.
     */
    temppath = strlen(str_temppath);

    /* define SQLITE_TEMP_FILE_PREFIX "etilqs_" */
    if ((temppath + strlen("etilqs_") + 18) >= nbuf) {
        return SQLITE_ERROR;
    }

    for (i = temppath; i > 0 && str_temppath[i-1]=='/'; i--) {}

    str_temppath[i] = 0;

    sqlite3_snprintf(nbuf - 18,
                     pbuf,
                     (temppath > 0) ? "%s/""etilqs_" : "etilqs_",
                     str_temppath);

    j = strlen(pbuf);

    sqlite3_randomness(15, &pbuf[j]);

    for (i=0; i<15; i++, j++) {
        pbuf[j] = (char)chars[((unsigned char)pbuf[j])%(sizeof(chars)-1)];
    }

    pbuf[j]   = 0;
    pbuf[j+1] = 0;

#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("TEMP FILENAME: %s\n", pbuf));
#endif

  return SQLITE_OK;
}

/** \brief get the file mode from the flags */
static int __createfile_mode_find (const char *p_filename,
                                   int         flags,
                                   mode_t     *p_mode)
{
    int rc = SQLITE_OK;

    struct aw_stat filestat;         /* Output of aw_stat() on database file */

    *p_mode = 0;

    if (flags & (SQLITE_OPEN_WAL | SQLITE_OPEN_MAIN_JOURNAL)) {

        char dbfile[AW_SQLITE3_MAX_PATH + 1]; /* Database file path */
        int  ndbfile;                      /* Number of valid bytes in dbfile */

        ndbfile = strlen(p_filename) - 1;
#ifdef SQLITE_ENABLE_8_3_NAMES
        while (ndbfile > 0 && sqlite3Isalnum(p_filename[ndbfile])) {
            ndbfile--;
        }
        if (ndbfile==0 || p_filename[ndbfile] != '-') return SQLITE_OK;
#else
        while (p_filename[ndbfile] != '-') {
            aw_assert(ndbfile > 0);
            aw_assert(p_filename[ndbfile] != '\n');
            ndbfile--;
        }
#endif

        memcpy(dbfile, p_filename, ndbfile);

        if (0 == aw_stat(dbfile, &filestat)) {
            *p_mode = filestat.st_mode & 0777;
        } else {
            rc = SQLITE_IOERR_FSTAT;
        }

    } else if (flags & SQLITE_OPEN_DELETEONCLOSE) {

        *p_mode = 0600;
    } else {

        *p_mode = 0700;

    }

    return rc;
}



/**
 * \brief Open the file p_filename.
 *
 * Previously, the SQLite OS layer used three functions in place of this
 * one:
 *
 *     sqlite3OsOpenReadWrite();
 *     sqlite3OsOpenReadOnly();
 *     sqlite3OsOpenExclusive();
 *
 * These calls correspond to the following combinations of flags:
 *
 *     ReadWrite() ->     (READWRITE | CREATE)
 *     ReadOnly()  ->     (READONLY)
 *     OpenExclusive() -> (READWRITE | CREATE | EXCLUSIVE)
 *
 * The old OpenExclusive() accepted a boolean argument - "delFlag". If
 * true, the file was configured to be automatically deleted when the
 * file handle closed. To achieve the same effect using this new
 * interface, add the DELETEONCLOSE flag to those specified above for
 * OpenExclusive().
 */
static int __sqlite3_vfs_open (sqlite3_vfs  *p_vfs,
                               const char   *p_filename,
                               sqlite3_file *id,
                               int           flags,
                               int          *p_outflags)
{
    aw_sqlite3_file *pfile = (aw_sqlite3_file *)id;

    mode_t open_mode;                  /* Mode to pass to aw_open() */

    int    open_flags = 0;             /* Flags to pass to aw_open() */
    int    etype = flags & 0xFFFFFF00; /* Type of file to open */
    int    rc = SQLITE_OK;             /* Function Return Code */

    int    is_exclusive   = (flags & SQLITE_OPEN_EXCLUSIVE);
    int    is_delete      = (flags & SQLITE_OPEN_DELETEONCLOSE);
    int    is_create      = (flags & SQLITE_OPEN_CREATE);
    int    is_readonly    = (flags & SQLITE_OPEN_READONLY);
    int    is_read_write  = (flags & SQLITE_OPEN_READWRITE);

    /* If creating a master or main-file journal, this function will open
     * a file-descriptor on the directory too. The first time unixSync()
     * is called the directory file descriptor will be fsync()ed and close()d.
     */
    int sync_dir= (is_create &&
                   (etype == SQLITE_OPEN_MASTER_JOURNAL ||
                    etype == SQLITE_OPEN_MAIN_JOURNAL   ||
                    etype == SQLITE_OPEN_WAL));

    /* If argument zPath is a NULL pointer, this function is required to open
     * a temporary file. Use this buffer to store the file name in.
     */
    char tmpname[AW_SQLITE3_MAX_PATH + 2];

    const char *__p_filename = p_filename;

    /* Check the following statements are true:
     *
     *   (a) Exactly one of the READWRITE and READONLY flags must be set, and
     *   (b) if CREATE is set, then READWRITE must also be set, and
     *   (c) if EXCLUSIVE is set, then CREATE must also be set.
     *   (d) if DELETEONCLOSE is set, then CREATE must also be set.
     */
    aw_assert((is_readonly == 0 || is_read_write == 0) &&
              (is_read_write    || is_readonly));

    aw_assert(is_create == 0    || is_read_write);
    aw_assert(is_exclusive == 0 || is_create);
    aw_assert(is_delete == 0    || is_create);

    /* The main DB, main journal, WAL file and master journal are never
    ** automatically deleted. Nor are they ever temporary files.  */
    aw_assert((!is_delete && __p_filename) ||
              etype != SQLITE_OPEN_MAIN_DB);

    aw_assert((!is_delete && __p_filename) ||
              etype != SQLITE_OPEN_MAIN_JOURNAL);

    aw_assert((!is_delete && __p_filename) ||
              etype != SQLITE_OPEN_MASTER_JOURNAL);

    aw_assert((!is_delete && __p_filename) ||
              etype != SQLITE_OPEN_WAL);

    /* Assert that the upper layer has set one of the "file-type" flags. */
    aw_assert(etype == SQLITE_OPEN_MAIN_DB        ||
              etype == SQLITE_OPEN_TEMP_DB        ||
              etype == SQLITE_OPEN_MAIN_JOURNAL   ||
              etype == SQLITE_OPEN_TEMP_JOURNAL   ||
              etype == SQLITE_OPEN_SUBJOURNAL     ||
              etype == SQLITE_OPEN_MASTER_JOURNAL ||
              etype == SQLITE_OPEN_TRANSIENT_DB   ||
              etype == SQLITE_OPEN_WAL);

    aw_assert(pfile != 0);

    memset(pfile, 0, sizeof(aw_sqlite3_file));

    /**
     * If the second argument to this function is NULL, generate a
     * temporary file name to use
     */
    if (!__p_filename) {
        aw_assert(is_delete && !sync_dir);

        rc = __sqlite3_tempname_get(AW_SQLITE3_MAX_PATH + 2, tmpname);
        if (rc != SQLITE_OK) {
            return rc;
        }

        __p_filename = tmpname;

        /* Generated temporary filenames are always double-zero terminated
        ** for use by sqlite3_uri_parameter(). */
        aw_assert(__p_filename[strlen(__p_filename)+1] == 0);
    }

    /**
     * \brief
     * Determine the value of the flags parameter passed to AWorks function
     * aw_open(). These must be calculated even if aw_open() is not called, as
     * they may be stored as part of the file handle and used by the
     * 'conch file' locking functions later on.
     */
    if (is_readonly) {
        open_flags |= O_RDONLY;
    }
    if (is_read_write) {
        open_flags |= O_RDWR;
    }
    if (is_create) {
        open_flags |= O_CREAT;
    }
    if (is_exclusive) {
        open_flags |= O_EXCL;
    }

    __createfile_mode_find(__p_filename, open_flags, &open_mode);

    pfile->fd = aw_open(__p_filename, open_flags, open_mode);
#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("OPENX   %-3d %s 0%x\n",
              pfile->fd,
              __p_filename,
              open_flags));
#endif

    if (p_outflags) {
        if (pfile->fd < 0) {
            AW_ERRNO_GET(pfile->last_errno);
            *p_outflags = 0;
        } else {
            *p_outflags = flags;
        }
    }

    pfile->p_method   = &__io_methods;
    pfile->last_errno = 0;
    pfile->p_vfs      = p_vfs;
    pfile->path       = p_filename;

    /* define SQLITE_POWERSAFE_OVERWRITE 1 */
    if (sqlite3_uri_boolean(p_filename, "psow", 1)) {
        pfile->ctrl_flags |= AWFILE_PSOW;
    }

    return rc;
}


/**
 * \brief Delete the named file.
 */
static int __sqlite3_vfs_delete (sqlite3_vfs *p_vfs,
                                 const char  *p_filename,
                                 int          sync_dir)
{
    int rc = SQLITE_OK;

    (void)(p_vfs);
    (void)(sync_dir);

    rc = aw_delete(p_filename);
    if (rc) {
        rc = SQLITE_IOERR_DELETE_NOENT;
    } else {
        rc = SQLITE_OK;
    }

#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("DELETE \"%s\" %s\n",
              p_filename,
              (rc ? "failed" : "ok" )));
#endif

    return rc;
}


/**
 * \brief Check the existence and status of a file.
 *        try using the [flags] to access [p_filename],
 *        if ok then *p_resout = 1, otherwise, *p_resout = 0.
 */
static int __sqlite3_vfs_access (sqlite3_vfs *p_vfs,
                                 const char  *p_filename,
                                 int          flags,
                                 int         *p_resout)
{
    struct aw_stat filestat;

    (void)p_vfs;

    if (aw_stat(p_filename, &filestat) == -1) {
        *p_resout = 0;
    } else {
        switch (flags) {
        case SQLITE_ACCESS_EXISTS:
            *p_resout = 1;
            break;
        case SQLITE_ACCESS_READWRITE:
            if ((filestat.st_mode & S_IRWXU) ||
                (filestat.st_mode & S_IRWXG) ||
                (filestat.st_mode & S_IRWXO)) {
                *p_resout = 1;
            } else {
                *p_resout = 0;
            }
            break;
        case SQLITE_ACCESS_READ:
            if ((filestat.st_mode & S_IRUSR) ||
                (filestat.st_mode & S_IRGRP) ||
                (filestat.st_mode & S_IROTH)) {
                *p_resout = 1;
            } else {
                *p_resout = 0;
            }
            break;

        default:
            aw_assert(!"Invalid flags argument");
            break;
        }
    }

#ifdef SQLITE_AWORKS_DEBUG
    AW_INFOF(("ACCESS name=%s, p_resout=%p, *p_resout=%d, rc=SQLITE_OK\n",
             p_filename,
             p_resout,
             *p_resout));
#endif

    return SQLITE_OK;
}


/**
 * \brief Turn a relative pathname into a full pathname.  Write the full
 *        pathname into p_full[]. p_full[] will be at least pVfs->mxPathname
 *        bytes in size.
 */
static int __sqlite3_vfs_fullpathname (sqlite3_vfs *p_vfs,
                                       const char  *p_relative,
                                       int          nfull,
                                       char        *p_full)
{
    (void)p_vfs;
    (void)nfull;

    //todo:AWorks platform's pathname is full pathname defaultly?
    memcpy(p_full, p_relative, nfull);

    return SQLITE_OK;
}

/**
 * \brief Write nbytes bytes of random data to the supplied buffer p_out.
 */
static int __sqlite3_vfs_randomness (sqlite3_vfs *p_vfs,
                                     int          nbytes,
                                     char        *p_out)
{
    (void)p_vfs;
    memset(p_out, 0, nbytes);
    return SQLITE_OK;
}

/**
 * \brief Sleep for a little while.  Return the amount of time slept.
 */
static int __sqlite3_vfs_sleep (sqlite3_vfs *p_vfs, int microsec)
{
    aw_task_delay(aw_ms_to_ticks(microsec / 1000));
    return microsec;
}


/**
 * \brief get currenttime
 */
static int __sqlite3_vfs_currenttime64 (sqlite3_vfs   *p_vfs,
                                        sqlite3_int64 *p_now)
{
    static const sqlite3_int64 epoch = 24405875 * (sqlite3_int64)8640000;

    aw_timespec_t now;
    int           rc = SQLITE_OK;

    aw_timespec_get(&now);

    *p_now = epoch + 1000 * (sqlite3_int64)now.tv_sec + now.tv_nsec / 1000000;
#ifdef SQLITE_TEST
    if (sqlite3_current_time) {
        *p_now = 1000 * (sqlite3_int64)sqlite3_current_time + epoch;
    }
#endif

    (void)p_vfs;

    return rc;
}

/**
 * \brief
 * Find the current time (in Universal Coordinated Time).  Write into
 * p_now the current time and date as a Julian Day number times 86_400_000.
 * In other words, write into *p_now the number of milliseconds since the Julian
 * epoch of noon in Greenwich on November 24, 4714 B.C according to the
 * proleptic Gregorian calendar.
 *
 * On success, return SQLITE_OK.  Return SQLITE_ERROR if the time and date
 * cannot be found.
 */
static int __sqlite3_vfs_currenttime (sqlite3_vfs *p_vfs, double *p_now)
{
    int rc;

    sqlite3_int64 i = 0;

    (void)p_vfs;

    rc = __sqlite3_vfs_currenttime64(0, &i);

    *p_now = i/86400000.0;

    return rc;
}


/**
 * \brief AWorks platform didnt support
 */
static int __sqlite3_vfs_lasterror_get (sqlite3_vfs  *p_vfs,
                                        int           nbytes,
                                        char         *pbuf)
{
    (void)p_vfs;
    (void)nbytes;
    (void)pbuf;

    return SQLITE_OK;
}

/**
 * \brief Initialize and deinitialize the operating system interface.
 */
int SQLITE_STDCALL sqlite3_os_init (void)
{
    static sqlite3_vfs __vfs = {
        1,                                 /* iVersion */
        sizeof(aw_sqlite3_file),           /* szOsFile */
        AW_SQLITE3_MAX_PATH,               /* mxPathname */
        0,                                 /* pNext */
        "aworks",                          /* zName */
        0,                                 /* pAppData */
        __sqlite3_vfs_open,                /* xOpen */
        __sqlite3_vfs_delete,              /* xDelete */
        __sqlite3_vfs_access,              /* xAccess */
        __sqlite3_vfs_fullpathname,        /* xFullPathname */
        0,                                 /* xDlOpen */
        0,                                 /* xDlError */
        0,                                 /* xDlSym */
        0,                                 /* xDlClose */
        __sqlite3_vfs_randomness,          /* xRandomness */
        __sqlite3_vfs_sleep,               /* xSleep */
        __sqlite3_vfs_currenttime,         /* xCurrentTime */
        __sqlite3_vfs_lasterror_get,       /* xGetLastError */
        __sqlite3_vfs_currenttime64,       /* xCurrentTimeInt64 */
        0,                                 /* xSetSystemCall */
        0,                                 /* xGetSystemCall */
        0                                  /* xNextSystemCall */
    };

    sqlite3_vfs_register(&__vfs, 1);
    return SQLITE_OK;
}

/**
 * \brief Shutdown the operating system interface.
 *
 * Some operating systems might need to do some cleanup in this routine,
 * to release dynamically allocated objects.  But not on AWorks.
 * This routine is a no-op for AWorks.
 */
int SQLITE_STDCALL sqlite3_os_end (void)
{
    return SQLITE_OK;
}

#endif /* SQLITE_OS_AWORKS */

