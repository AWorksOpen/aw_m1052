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
 * \brief AWBus 通用并行接口
 *
 * \internal
 * \par modification history:
 * - 1.00 15-10-15  dcf, first implementation
 * \endinternal
 */

#include <stdarg.h>
#include <stdint.h>
#include "awbl_upp.h"

#include "apollo.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_assert.h"

#include "awbus_lite.h"
#include "awbl_plb.h"

#include "aw_errno.h"
#include "aw_delay.h"

#include "aw_vdebug.h"


void awbl_upp_init(struct awbl_upp *_this )
{
    _this->features               = 0;
    _this->ioctl                  = 0;
    _this->xfer_complete_callback = 0;
    _this->align_size             = 0;
    _this->is_opened              = 0;
    _this->max_xfer_size          = (uint32_t)-1;
    _this->msgid                  = 0;
}

struct awbl_upp *awbl_upp_open( const char *device_name, int channel )
{
    awbl_dev_t *dev = awbl_dev_find_by_name( device_name, channel );
    struct awbl_upp *thiz = 0;

    if ( dev )
    {
        thiz = AW_CONTAINER_OF( dev, struct awbl_upp, super );
        if ( thiz->is_opened )
        {
            return thiz;
        }
        if ( AW_OK != awbl_upp_ioctl( thiz, AWBL_UPP_CMD_OPEN ) )
        {
            return 0;
        }
        thiz->is_opened = 1;
    }
    return thiz;
}

void awbl_upp_close( struct awbl_upp *thiz )
{
    if (thiz && thiz->is_opened)
    {
        awbl_upp_ioctl( thiz, AWBL_UPP_CMD_CLOSE );
        thiz->is_opened = 0;
    }
}

void awbl_upp_complete( struct awbl_upp *_this, void *buffer )
{
    if ( !_this )
    {
        return ;
    }
    aw_msgq_send( _this->msgid, (void*)&buffer, sizeof(buffer), 0, 0 );
    if ( _this->xfer_complete_callback )
    {
        (*_this->xfer_complete_callback )( _this->callback_arg );
    }
}

void awbl_upp_set_callback(struct awbl_upp *_this,
        void (*xfer_complete_callback)( void * ),
        void *arg )
{
    if ( !_this )
    {
        return ;
    }
    _this->callback_arg = arg;
    _this->xfer_complete_callback = xfer_complete_callback;
}

aw_err_t awbl_upp_ioctl(struct awbl_upp *_this, int cmd, ... )
{
    va_list vs;
    aw_err_t ret = AW_ERROR;

    va_start( vs, cmd );
    if (_this && _this->ioctl )
    {
        ret = AW_OK;
        switch (cmd)
        {
        case AWBL_UPP_CMD_SET_MSGQID:
            _this->msgid = va_arg( vs, aw_msgq_id_t );
            break;
        default:
            ret = (*_this->ioctl)( _this, cmd, vs );
            break;
        }
    }
    va_end( vs );
    return ret;
}

void awbl_upp_destroy(struct awbl_upp *_this)
{
    (void)_this;
}

aw_err_t awbl_upp_xfer_async(
        struct awbl_upp *_this,
        char            *window_address,
        uint32_t         line_size,
        uint32_t         line_offset,
        uint32_t         line_count)
{
    if (_this && _this->ioctl )
    {
        return awbl_upp_ioctl( _this, AWBL_UPP_CMD_XFER_START,
                window_address, line_size, line_offset, line_count );
    }
    return AW_ERROR;
}

aw_err_t awbl_upp_wait_async( struct awbl_upp *_this, 
                              void           **buffer, 
                              int              timeout )
{
    return aw_msgq_receive( _this->msgid, 
                     (void*)buffer, 
                    sizeof(*buffer), 
                            timeout );
}

/* end of file */
