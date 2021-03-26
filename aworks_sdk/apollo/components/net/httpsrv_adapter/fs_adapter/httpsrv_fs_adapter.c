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
 * \brief HTTP server file system adapter
 *
 * \internal
 * \par Modification History
 * - 1.00 18-03-07  sdy, first implementation.
 * \endinternal
 */

#include "httpsrv_fs_adapter.h"
#include "httpsrv_port.h"
#include "httpsrv_fs.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_stropts.h"

int httpsrv_fs_adapter_open(const char *, int oflag, mode_t mode);
int httpsrv_fs_adapter_close(int);
ssize_t httpsrv_fs_adapter_read(int, void *, size_t);
size_t httpsrv_fs_adapter_size(int);
int httpsrv_fs_adapter_is_gzip(int);
int httpsrv_fs_adapter_ioctl(int, int, void *);
off_t httpsrv_fs_adapter_fseek(int fd, off_t offset, int mode);

static HTTPSRV_FS_DRV_FUNCS __g_httpsrv_fs_dev_funcs = {
    httpsrv_fs_adapter_open,
    httpsrv_fs_adapter_close,
    httpsrv_fs_adapter_read,
    httpsrv_fs_adapter_size,
    httpsrv_fs_adapter_is_gzip,
    httpsrv_fs_adapter_ioctl,
    httpsrv_fs_adapter_fseek
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : httpsrv_fs_adapter_init
* Returned Value   : HTTPSRV_FS_ADAPTER error code.
* Comments         : Initialize the Trivial File System.
*
*END*---------------------------------------------------------------------*/
/**
 * \brief	HTTP server file system adapter initialization
 *
 * \return	pointer to file system device
 *
 * \note none
 */
void *httpsrv_fs_adapter_init(void)
{
    return &__g_httpsrv_fs_dev_funcs;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : HTTPSRV_FS_ADAPTER_open
* Returned Value   : HTTPSRV_FS_ADAPTER error code.
* Comments         : Opens HTTPSRV_FS_ADAPTER driver and initializes given file 
*                    descriptor.
*
*END*----------------------------------------------------------------------*/

int httpsrv_fs_adapter_open(
    /* [IN] the remaining portion of the name of the device */
    const char *open_name_ptr,

    int oflag,

    mode_t mode)
{
    return aw_open(open_name_ptr, oflag, mode);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : httpsrv_fs_adapter_close
* Returned Value   : HTTPSRV_FS_ADAPTER error code.
* Comments         : Closes given file descriptor.
*
*END*----------------------------------------------------------------------*/

int httpsrv_fs_adapter_close(
    /* [IN/OUT] the file handle for the device being closed */
    int fd)
{
    return aw_close (fd);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : httpsrv_fs_adapter_read
* Returned Value   : Number of characters read.
* Comments         : Reads data from given file.
*
*END*----------------------------------------------------------------------*/

ssize_t httpsrv_fs_adapter_read(
    /* [IN/OUT] the stream to perform the operation on */
    int fd,

    /* [IN] the data location to write to */
    void *ptr,

    /* [IN] the number of bytes to read */
    size_t num)
{
    return aw_read  (fd, ptr, num);
}

off_t httpsrv_fs_adapter_fseek(int fd, off_t offset, int mode)
{
    return aw_lseek (fd, offset, mode);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : httpsrv_fs_adapter_ioctl
* Returned Value   : HTTPSRV_FS_ADAPTER error code.
* Comments         : Performs specified operation related to given file.
*
*END*----------------------------------------------------------------------*/

int httpsrv_fs_adapter_ioctl(
    /* [IN] the stream to perform the operation on */
    int fd,

    /* [IN] the ioctl command */
    int cmd,

    /* [IN/OUT] the ioctl parameters */
    void *param_ptr)
{
    return aw_ioctl (fd, cmd, param_ptr);
}


size_t httpsrv_fs_adapter_size(int fd)
{
    size_t size = 0;
    off_t start = 0;
    off_t end   = 0;

    start = aw_lseek (fd, 0, SEEK_SET);
    end = aw_lseek (fd, 0, SEEK_END);
    size = end - start;

    return size;
}

int httpsrv_fs_adapter_is_gzip(int fd)
{
    int zip_flag;
    if (aw_ioctl (fd, AW_FILEGZIP, (void *)&zip_flag) == AW_OK) {
        return zip_flag;
    }

    return 0;
}

/* end of file */
