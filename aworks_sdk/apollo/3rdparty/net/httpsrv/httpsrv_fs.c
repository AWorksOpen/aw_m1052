/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
* Comments:
*
*   This file contains the functions that are used to initialize FS
*   It also contains the FS driver functions.
*/
#include "httpsrv_fs.h"
#include "httpsrv_port.h"


static HTTPSRV_FS_FILE __g_httpsrv_fs_file;
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : HTTPSRV_FS_init
* Returned Value   : HTTPSRV_FS error code.
* Comments         : Initialize the Trivial File System.
*
*END*---------------------------------------------------------------------*/

void HTTPSRV_FS_init(HTTPSRV_FS_DRV_FUNCS *funcs)
{
    __g_httpsrv_fs_file.FUNCS = funcs;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : HTTPSRV_FS_open
* Returned Value   : HTTPSRV_FS error code.
* Comments         : Opens HTTPSRV_FS driver and initializes given file descriptor.
*
*END*----------------------------------------------------------------------*/

int HTTPSRV_FS_open(
    /* [IN] the remaining portion of the name of the device */
    char *open_name_ptr)
{

    if (__g_httpsrv_fs_file.FUNCS->pfn_fs_open)
    {
        return __g_httpsrv_fs_file.FUNCS->pfn_fs_open((const char *)open_name_ptr,
                                                      0x0000,
                                                      0777);
    }
    return HTTPSRV_FS_ERROR;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : HTTPSRV_FS_close
* Returned Value   : HTTPSRV_FS error code.
* Comments         : Closes given file descriptor.
*
*END*----------------------------------------------------------------------*/

int HTTPSRV_FS_close(
    /* [IN/OUT] the file handle for the device being closed */
    int fildes)
{
    if (__g_httpsrv_fs_file.FUNCS->pfn_fs_close)
    {
        return __g_httpsrv_fs_file.FUNCS->pfn_fs_close(fildes);
    }
    return HTTPSRV_FS_ERROR;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : HTTPSRV_FS_read
* Returned Value   : Number of characters read.
* Comments         : Reads data from given file.
*
*END*----------------------------------------------------------------------*/

int32_t HTTPSRV_FS_read(
    /* [IN/OUT] the stream to perform the operation on */
    int fildes,

    /* [IN] the data location to write to */
    char *data_ptr,

    /* [IN] the number of bytes to read */
    int32_t num)
{
    if (__g_httpsrv_fs_file.FUNCS->pfn_fs_read)
    {
        return __g_httpsrv_fs_file.FUNCS->pfn_fs_read(fildes, data_ptr, num);
    }
    return HTTPSRV_FS_ERROR;
}

int32_t HTTPSRV_FS_fseek(int fildes, int32_t offset, uint32_t mode)
{
    if (__g_httpsrv_fs_file.FUNCS->pfn_fs_fseek)
    {
        return __g_httpsrv_fs_file.FUNCS->pfn_fs_fseek(fildes, offset, mode);
    }
    return HTTPSRV_FS_ERROR;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : HTTPSRV_FS_ioctl
* Returned Value   : HTTPSRV_FS error code.
* Comments         : Performs specified operation related to given file.
*
*END*----------------------------------------------------------------------*/

int32_t HTTPSRV_FS_ioctl(
    /* [IN] the stream to perform the operation on */
    int fildes,

    /* [IN] the ioctl command */
    uint32_t cmd,

    /* [IN/OUT] the ioctl parameters */
    void *param_ptr)
{
    if (__g_httpsrv_fs_file.FUNCS->pfn_fs_ioctl)
    {
        return __g_httpsrv_fs_file.FUNCS->pfn_fs_ioctl(fildes, cmd, param_ptr);
    }
    return HTTPSRV_FS_ERROR;
}

size_t HTTPSRV_FS_size(
    /* [IN] the stream to perform the operation on */
    int fildes)
{
    if (__g_httpsrv_fs_file.FUNCS->pfn_fs_size)
    {
        return __g_httpsrv_fs_file.FUNCS->pfn_fs_size(fildes);
    }
    return 0;
}

int HTTPSRV_FS_is_gzip(
    /* [IN] the stream to perform the operation on */
    int fildes)
{
    if (__g_httpsrv_fs_file.FUNCS->pfn_is_gzip)
    {
        return __g_httpsrv_fs_file.FUNCS->pfn_is_gzip(fildes);
    }
    return -1;
}
