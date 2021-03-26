/* ptp.c
 *
 * Copyright (C) 2001-2004 Mariusz Woloszyn <emsi@ipartners.pl>
 * Copyright (C) 2003-2018 Marcus Meissner <marcus@jet.franken.de>
 * Copyright (C) 2006-2008 Linus Walleij <triad@df.lth.se>
 * Copyright (C) 2007 Tero Saarni <tero.saarni@gmail.com>
 * Copyright (C) 2009 Axel Waggershauser <awagger@web.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "ptp.h"
#include "libmtp.h"
#include "pathutils.h"
#include "mtp_adapter.h"

#define CHECK_PTP_RC(RESULT) do { u16 r = (RESULT); if (r != PTP_RC_OK) return r; } while(0)

#define _(String) (String)
#define N_(String) (String)


/* 初始化一个PTP容器*/
static inline void ptp_init_container(PTPContainer* ptp, u16 code, int n_param, ...)
{
    /* 定义一个指向当前参数的指针*/
    va_list args;
    int i;

    memset(ptp, 0, sizeof(*ptp));
    /* 事件代码*/
    ptp->Code = code;
    /* 参数数量*/
    ptp->Nparam = n_param;
    /* 初始化参数指针，让它指向可变参数的第一个参数*/
    va_start(args, n_param);
    for (i = 0; i < n_param; ++i)
        /* 获取下一个参数，并把参数指针指向下一个参数*/
        (&ptp->Param1)[i] = va_arg(args, u32);
    /* 关掉指针*/
    va_end(args);
}

#define NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N
#define NARGS(...) NARGS_SEQ(-1, ##__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0)

/* 初始化一个PTP容器*/
#define PTP_CNT_INIT(PTP, CODE, ...) \
    ptp_init_container(&PTP, CODE, NARGS(__VA_ARGS__), ##__VA_ARGS__)

static u16 ptp_exit_recv_memory_handler (PTPDataHandler*,unsigned char**,unsigned long*);
static u16 ptp_init_recv_memory_handler(PTPDataHandler*);
static u16 ptp_init_send_memory_handler(PTPDataHandler*,unsigned char*,unsigned long len);
static u16 ptp_exit_send_memory_handler (PTPDataHandler *handler);

void ptp_debug (PTPParams *params, const char *format, ...)
{
    va_list args;

    va_start (args, format);
    if (params->debug_func!=NULL)
        params->debug_func (params->data, format, args);
    else
    {
        vfprintf (stderr, format, args);
        fprintf (stderr,"\n");
        fflush (stderr);
    }
    va_end (args);
}

void ptp_error (PTPParams *params, const char *format, ...)
{
    va_list args;

    va_start (args, format);
    if (params->error_func!=NULL)
        params->error_func (params->data, format, args);
    else
    {
        vfprintf (stderr, format, args);
        fprintf (stderr,"\n");
        fflush (stderr);
    }
    va_end (args);
}

#include "ptp-pack.c"

/**
 * 执行PTP事务。ptp是一个PTP容器，其中填写了适当的字段(即操作代码和参数)。这取决于调用的人。
 * 这些标志决定了事务是否是一个数据阶段以及它的方向(发送或接收)。
 * 如果事务正在发送数据，则sendlen应包含其字节长度，否则将忽略它。
 * 数据应该包含一个指向将要发送的数据的指针地址，或者如果根据数据阶段的方向(发送或接收)接收到数据，
 * 或者忽略数据(无数据阶段)，则用这样的指针地址填充。
 * 指针的内存应该由调用者保留，如果要检索数据，应该分配适当的内存量(调用者应该处理这个问题)。
 *
 * @param params PTP参数
 * @param ptp PTP事务容器
 * @param flags 标志
 * @param sendlen 要发送的数据长度
 * @param handler PTP数据句柄
 * @return 一些PTP_RC_*代码。成功后，返回PTPContainer* ptp包含PTP响应阶段容器，所有字段均已填写。
 **/
u16 ptp_transaction_new (PTPParams* params, PTPContainer* ptp,
                         u16 flags, u64 sendlen, PTPDataHandler *handler) {
    int         tries;
    u16    cmd;

    /* 检查参数合法性*/
    if ((params == NULL) || (ptp == NULL))
        return PTP_ERROR_BADPARAM;

    /* 获取事件代码*/
    cmd = ptp->Code;
    ptp->Transaction_ID = params->transaction_id++;
    ptp->SessionID = params->session_id;
    /* 发送请求 */
    CHECK_PTP_RC(params->sendreq_func (params, ptp, flags));
    /* 数据阶段？ */
    switch (flags & PTP_DP_DATA_MASK) {
    case PTP_DP_SENDDATA:
        {
            /* 发送数据*/
            u16 ret = params->senddata_func(params, ptp, sendlen, handler);
            if (ret == PTP_ERROR_CANCEL)
                CHECK_PTP_RC(params->cancelreq_func(params, params->transaction_id-1));
            CHECK_PTP_RC(ret);
        }
        break;
    case PTP_DP_GETDATA:
        {
            /* 获取数据*/
            u16 ret = params->getdata_func(params, ptp, handler);
            if (ret == PTP_ERROR_CANCEL)
                CHECK_PTP_RC(params->cancelreq_func(params, params->transaction_id-1));
            CHECK_PTP_RC(ret);
        }
        break;
    case PTP_DP_NODATA:
        break;
    default:
        return PTP_ERROR_BADPARAM;
    }
    tries = 3;
    while (tries--) {
        u16 ret;

        /* 获取响应 */
        ret = params->getresp_func(params, ptp);
        /* 返回的响应的容器类型和期望的不一样*/
        if (ret == PTP_ERROR_RESP_EXPECTED) {
            ptp_debug (params,"PTP: response expected but not got, retrying.");
            tries++;
            continue;
        }
        CHECK_PTP_RC(ret);
        /* (Leica设备兼容)*/
        if (ptp->Transaction_ID < params->transaction_id - 1) {
            /* Leica对关闭会话的结果使用事务ID 0 */
            if (cmd == PTP_OC_CloseSession)
                break;
            tries++;
            ptp_debug (params,
                "PTP: Sequence number mismatch %d vs expected %d, suspecting old reply.",
                ptp->Transaction_ID, params->transaction_id - 1
            );
            continue;
        }
        if (ptp->Transaction_ID != params->transaction_id - 1) {
            /* 尝试清除上一个会话可能遗留下来的东西 */
            if ((cmd == PTP_OC_OpenSession) && tries)
                continue;
            ptp_error (params,
                "PTP: Sequence number mismatch %d vs expected %d.",
                ptp->Transaction_ID, params->transaction_id - 1
            );
#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
            return PTP_ERROR_BADPARAM;
#endif
        }
        break;
    }
    return ptp->Code;
}

/* 私有内存句柄 */
typedef struct {
    unsigned char   *data;   /* 数据缓存*/
    unsigned long   size;    /* 数据大小*/
    unsigned long   curoff;  /* 偏移*/
} PTPMemHandlerPrivate;

/**
 * 把数据从私有内存句柄里拷贝出去
 *
 * @param params PTP参数结构体
 * @param private 私有内存句柄
 * @param wantlen 想要拷贝的长度
 * @param data 数据拷贝出去的缓存地址
 * @param gotlen 实际拷贝得到的数据
 * @return 成功返回PTP_RC_OK
 */
static u16 memory_getfunc(PTPParams* params, void* private,
           unsigned long wantlen, unsigned char *data,
           unsigned long *gotlen) {
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*)private;
    unsigned long tocopy = wantlen;

    if (priv->curoff + tocopy > priv->size)
        tocopy = priv->size - priv->curoff;
    memcpy (data, priv->data + priv->curoff, tocopy);
    priv->curoff += tocopy;
    *gotlen = tocopy;
    return PTP_RC_OK;
}

/**
 * 把数据拷贝到私有内存句柄里
 *
 * @param params PTP参数结构体
 * @param private 私有内存句柄
 * @param sendlen 要拷贝的长度
 * @param data 数据拷贝进来的缓存地址
 * @return 成功返回PTP_RC_OK
 */
static u16 memory_putfunc(PTPParams* params, void* private,
           unsigned long sendlen, unsigned char *data) {
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*)private;

    if (priv->curoff + sendlen > priv->size) {
        priv->data = mtp_realloc (priv->data, priv->curoff+sendlen);
        if (!priv->data)
            return PTP_RC_GeneralError;
        priv->size = priv->curoff + sendlen;
    }
    memcpy (priv->data + priv->curoff, data, sendlen);
    priv->curoff += sendlen;
    return PTP_RC_OK;
}

/**
 * 初始化接收私有内存句柄
 *
 * @param handler PTP数据句柄
 * @return 成功返回PTP_RC_OK
 */
static u16 ptp_init_recv_memory_handler(PTPDataHandler *handler){
    PTPMemHandlerPrivate* priv;
    priv = mtp_malloc (sizeof(PTPMemHandlerPrivate));
    if (!priv)
        return PTP_RC_GeneralError;
    handler->priv = priv;
    handler->getfunc = memory_getfunc;
    handler->putfunc = memory_putfunc;
    priv->data = NULL;
    priv->size = 0;
    priv->curoff = 0;
    return PTP_RC_OK;
}

/**
 * 初始化发送私有内存句柄
 *
 * @param handler PTP数据句柄
 * @return 成功返回PTP_RC_OK
 */
static u16 ptp_init_send_memory_handler(PTPDataHandler *handler,
    unsigned char *data, unsigned long len) {
    PTPMemHandlerPrivate* priv;
    priv = mtp_malloc (sizeof(PTPMemHandlerPrivate));
    if (!priv)
        return PTP_RC_GeneralError;
    handler->priv = priv;
    handler->getfunc = memory_getfunc;
    handler->putfunc = memory_putfunc;
    priv->data = data;
    priv->size = len;
    priv->curoff = 0;
    return PTP_RC_OK;
}

/**
 * 释放发送私有内存句柄
 *
 * @param handler PTP数据句柄
 * @return 成功返回PTP_RC_OK
 */
static u16 ptp_exit_send_memory_handler (PTPDataHandler *handler)
{
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*)handler->priv;
    mtp_free (priv);
    return PTP_RC_OK;
}

/**
 * 释放接收私有内存句柄
 *
 * @param handler PTP数据句柄
 * @return 成功返回PTP_RC_OK
 */
static u16 ptp_exit_recv_memory_handler (PTPDataHandler *handler,
    unsigned char **data, unsigned long *size) {
    PTPMemHandlerPrivate* priv = (PTPMemHandlerPrivate*)handler->priv;
    *data = priv->data;
    *size = priv->size;
    mtp_free (priv);
    return PTP_RC_OK;
}

/* 文件描述符数据获取/放入句柄*/
typedef struct {
    int fd;
} PTPFDHandlerPrivate;

/**
 * 文件获取数据函数
 *
 * @param params  PTP参数
 * @param private 私有数据(文件描述符)
 * @param wantlen 想要读到的数据长度
 * @param data    获取数据的缓存
 * @param gotlen  实际读到的数据长度
 *
 * @return 成功返回PTP_RC_OK
 */
static u16 fd_getfunc(PTPParams* params, void* private,
           unsigned long wantlen, unsigned char *data,
           unsigned long *gotlen) {
    PTPFDHandlerPrivate* priv = (PTPFDHandlerPrivate*)private;
    int     got;

    got = mtp_read (priv->fd, data, wantlen);
    if (got != -1)
        *gotlen = got;
    else
        return PTP_RC_GeneralError;
    return PTP_RC_OK;
}

/**
 * 文件写数据函数
 *
 * @param params  PTP参数
 * @param private 私有数据(文件描述符)
 * @param sendlen 想要写的数据长度
 * @param data    获取数据的缓存
 *
 * @return 成功返回PTP_RC_OK
 */
static u16 fd_putfunc(PTPParams* params, void* private,
           unsigned long sendlen, unsigned char *data) {
    ssize_t     written;
    PTPFDHandlerPrivate* priv = (PTPFDHandlerPrivate*)private;

    written = mtp_write (priv->fd, data, sendlen);
    if (written != sendlen)
        return PTP_ERROR_IO;
    return PTP_RC_OK;
}

/**
 * PTP初始化文件描述符句柄
 *
 * @param handler  PTP数据句柄
 * @param fd       文件描述符
 *
 * @return 成功返回PTP_RC_OK
 */
static u16 ptp_init_fd_handler(PTPDataHandler *handler, int fd)
{
    PTPFDHandlerPrivate* priv;
    priv = mtp_malloc (sizeof(PTPFDHandlerPrivate));
    if (!priv)
        return PTP_RC_GeneralError;
    handler->priv = priv;
    handler->getfunc = fd_getfunc;
    handler->putfunc = fd_putfunc;
    priv->fd = fd;
    return PTP_RC_OK;
}

/**
 * PTP释放文件描述符句柄
 *
 * @param handler  PTP数据句柄
 *
 * @return 成功返回PTP_RC_OK
 */
static u16 ptp_exit_fd_handler (PTPDataHandler *handler)
{
    PTPFDHandlerPrivate* priv = (PTPFDHandlerPrivate*)handler->priv;
    mtp_free (priv);
    return PTP_RC_OK;
}

/**
 * 执行一个PTP事务
 * @param params PTP参数
 * @param PTPContainer PTP容器
 * @param flags 事务标记
 * @param data 接收数据的缓存
 * @param recvlen 接收到的数据长度
 * @return 成功返回事务代码
 * */
u16 ptp_transaction (PTPParams* params, PTPContainer* ptp,
        u16 flags, u64 sendlen, unsigned char **data, unsigned int *recvlen) {
    PTPDataHandler  handler;
    u16    ret;

    /* 检查事务标志*/
    switch (flags & PTP_DP_DATA_MASK) {
    /* 发送数据*/
    case PTP_DP_SENDDATA:
        if (!data)
            return PTP_ERROR_BADPARAM;
        /* 初始化发送内存句柄*/
        CHECK_PTP_RC(ptp_init_send_memory_handler (&handler, *data, sendlen));
        break;
    /* 接收数据*/
    case PTP_DP_GETDATA:
        if (!data)
            return PTP_ERROR_BADPARAM;
        *data = NULL;
        if (recvlen)
            *recvlen = 0;
        /* 初始化接收内存句柄*/
        CHECK_PTP_RC(ptp_init_recv_memory_handler (&handler));
        break;
    default:break;
    }
    /* 执行一个新事务*/
    ret = ptp_transaction_new (params, ptp, flags, sendlen, &handler);
    switch (flags & PTP_DP_DATA_MASK) {
    case PTP_DP_SENDDATA:
        ptp_exit_send_memory_handler (&handler);
        break;
    case PTP_DP_GETDATA: {
        unsigned long len;
        /* 获取收到的数据*/
        ptp_exit_recv_memory_handler (&handler, data, &len);
        if (ret != PTP_RC_OK) {
            len = 0;
            mtp_free(*data);
            *data = NULL;
        }
        if (recvlen)
            *recvlen = len;
        break;
    }
    default:break;
    }
    return ret;
}

/**
 * 获取设备信息数据集并填充设备信息结构
 *
 * @param params PTP参数
 * @param deviceinfo PTP设备信息结构体
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_getdeviceinfo (PTPParams* params, PTPDeviceInfo* deviceinfo)
{
    PTPContainer    ptp;
    unsigned char   *data = NULL;
    unsigned int    size;
    int     ret;

    /* 初始化一个获取设备信息的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_GetDeviceInfo);
    /* 发送获取设备信息事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &size));
    /* 解析设备信息*/
    ret = ptp_unpack_DI(params, data, deviceinfo, size);
    mtp_free(data);
    if (ret)
        return PTP_RC_OK;
    else
        return PTP_ERROR_IO;
}


/**
 * 发出一个通用的没有任何数据传输的PTP命令
 *
 * @param params  PTP参数
 * @param code    事务代码
 * @param n_param 参数数量
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_generic_no_data (PTPParams* params, u16 code, unsigned int n_param, ...)
{
    PTPContainer    ptp;
    va_list     args;
    unsigned int    i;

    if( n_param > 5 )
        return PTP_ERROR_BADPARAM;

    memset(&ptp, 0, sizeof(ptp));
    ptp.Code = code;
    ptp.Nparam = n_param;
    /* 初始化参数指针，让它指向可变参数的第一个参数*/
    va_start(args, n_param);
    for(i = 0; i < n_param; ++i)
        /* 获取下一个参数，并把参数指针指向下一个参数*/
        (&ptp.Param1)[i] = va_arg(args, u32);
    /* 关闭指针*/
    va_end(args);
    /* 发送一个无数据的事务*/
    return ptp_transaction(params, &ptp, PTP_DP_NODATA, 0, NULL, NULL);
}


/**
 * 建立一个新的会话
 *
 * @param params PTP参数
 * @param session 会话编号
 *
 * @return 一些PTP_RC_*代码.
 **/
u16 ptp_opensession (PTPParams* params, u32 session){
    PTPContainer    ptp;
    u16    ret;

    ptp_debug(params,"PTP: Opening session\r\n");

    /* 打开会话请求时，操作数据集的会话ID字段应该始终设置为0*/
    params->session_id = 0x00000000;
    /* 事务ID也应该设置为0 */
    params->transaction_id = 0x0000000;
    /* 响应包缓冲区清0 */
    params->response_packet = NULL;
    params->response_packet_size = 0;
    /* 没有分割的头部 */
    params->split_header_data = 0;

    /* 初始化一个打开会话的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_OpenSession, session);
    /* 创建新的无数据阶段的事务*/
    ret = ptp_transaction_new(params, &ptp, PTP_DP_NODATA, 0, NULL);
    /* TODO: 检查错误 */
    /* 现在设置全局会话ID为当前会话号 */
    params->session_id = session;
    return ret;
}

/**
 * 释放设备属性值
 *
 * @param dt  要释放的设备属性值的数据类型
 * @param dpd 要释放的设备属性值
 **/
void ptp_free_devicepropvalue(u16 dt, PTPPropertyValue* dpd)
{
    switch (dt) {
    case PTP_DTC_INT8:  case PTP_DTC_UINT8:
    case PTP_DTC_UINT16:    case PTP_DTC_INT16:
    case PTP_DTC_UINT32:    case PTP_DTC_INT32:
    case PTP_DTC_UINT64:    case PTP_DTC_INT64:
    case PTP_DTC_UINT128:   case PTP_DTC_INT128:
        /* Nothing to free */
        break;
    case PTP_DTC_AINT8: case PTP_DTC_AUINT8:
    case PTP_DTC_AUINT16:   case PTP_DTC_AINT16:
    case PTP_DTC_AUINT32:   case PTP_DTC_AINT32:
    case PTP_DTC_AUINT64:   case PTP_DTC_AINT64:
    case PTP_DTC_AUINT128:  case PTP_DTC_AINT128:
        mtp_free(dpd->a.v);
        break;
    case PTP_DTC_STR:
        mtp_free(dpd->str);
        break;
    }
}

/**
 * 释放设备属性描述符
 *
 * @param dpd 要释放的设备属性描述符
 **/
void ptp_free_devicepropdesc(PTPDevicePropDesc* dpd)
{
    u16 i;

    /* 释放设备属性值*/
    ptp_free_devicepropvalue (dpd->DataType, &dpd->FactoryDefaultValue);
    ptp_free_devicepropvalue (dpd->DataType, &dpd->CurrentValue);
    switch (dpd->FormFlag) {
    case PTP_DPFF_Range:
        ptp_free_devicepropvalue (dpd->DataType, &dpd->FORM.Range.MinimumValue);
        ptp_free_devicepropvalue (dpd->DataType, &dpd->FORM.Range.MaximumValue);
        ptp_free_devicepropvalue (dpd->DataType, &dpd->FORM.Range.StepSize);
        break;
    case PTP_DPFF_Enumeration:
        if (dpd->FORM.Enum.SupportedValue) {
            for (i  =0;i < dpd->FORM.Enum.NumberOfValues;i++)
                ptp_free_devicepropvalue (dpd->DataType, dpd->FORM.Enum.SupportedValue+i);
            mtp_free (dpd->FORM.Enum.SupportedValue);
        }
    }
    dpd->DataType = PTP_DTC_UNDEF;
    dpd->FormFlag = PTP_DPFF_None;
}

void ptp_free_objectpropdesc(PTPObjectPropDesc* opd)
{
    uint16_t i;

    ptp_free_devicepropvalue (opd->DataType, &opd->FactoryDefaultValue);
    switch (opd->FormFlag) {
    case PTP_OPFF_None:
        break;
    case PTP_OPFF_Range:
        ptp_free_devicepropvalue (opd->DataType, &opd->FORM.Range.MinimumValue);
        ptp_free_devicepropvalue (opd->DataType, &opd->FORM.Range.MaximumValue);
        ptp_free_devicepropvalue (opd->DataType, &opd->FORM.Range.StepSize);
        break;
    case PTP_OPFF_Enumeration:
        if (opd->FORM.Enum.SupportedValue) {
            for (i=0;i<opd->FORM.Enum.NumberOfValues;i++)
                ptp_free_devicepropvalue (opd->DataType, opd->FORM.Enum.SupportedValue+i);
            mtp_free (opd->FORM.Enum.SupportedValue);
        }
        break;
    case PTP_OPFF_DateTime:
    case PTP_OPFF_FixedLengthArray:
    case PTP_OPFF_RegularExpression:
    case PTP_OPFF_ByteArray:
    case PTP_OPFF_LongString:
        /* Ignore these presently, we cannot unpack them, so there is nothing to be freed. */
        break;
    default:
        fprintf (stderr, "Unknown OPFF type %d\n", opd->FormFlag);
        break;
    }
}

/**
 * 释放PTP参数的所有数据
 *
 * @param params PTP参数
 **/
void ptp_free_params (PTPParams *params)
{
    unsigned int i;

    mtp_free (params->cameraname);
    mtp_free (params->wifi_profiles);
    /* 释放对象*/
    for (i = 0;i < params->nrofobjects;i++)
        ptp_free_object (&params->objects[i]);
    mtp_free (params->objects);
    mtp_free (params->storageids.Storage);
    mtp_free (params->events);
    /* 释放佳能设备属性描述*/
    for (i = 0;i < params->nrofcanon_props;i++) {
        mtp_free (params->canon_props[i].data);
        ptp_free_devicepropdesc (&params->canon_props[i].dpd);
    }
    mtp_free (params->canon_props);
    mtp_free (params->backlogentries);
    /* 释放设备属性描述*/
    for (i = 0;i < params->nrofdeviceproperties;i++)
        ptp_free_devicepropdesc (&params->deviceproperties[i].desc);
    mtp_free (params->deviceproperties);

    ptp_free_DI (&params->deviceinfo);
}


/**
 * 获取StorageIDs数组和填充storageids结构体
 *
 * @param params PTP参数
 * @param storageids StorageIDs数组
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_getstorageids (PTPParams* params, PTPStorageIDs* storageids)
{
    PTPContainer    ptp;
    unsigned char   *data = NULL;
    unsigned int    size;

    /* 初始化一个获取StorageIDs的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_GetStorageIDs);
    /* 发出获取数据是事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &size));
    /* 解包存储ID*/
    ptp_unpack_SIDs(params, data, storageids, size);
    mtp_free(data);
    return PTP_RC_OK;
}

/**
 * 获取objectinfo结构体
 *
 * @param params PTP参数
 * @param handle 对象句柄
 * @param objectinfo 返回的objectinfo指针
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_getobjectinfo (PTPParams* params, u32 handle, PTPObjectInfo* objectinfo)
{
    PTPContainer    ptp;
    unsigned char   *data = NULL;
    unsigned int    size;

    /* 初始化一个获取对象信息的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_GetObjectInfo, handle);
    /* 发出获取数据的事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &size));
    /* 解包对象信息*/
    ptp_unpack_OI(params, data, objectinfo, size);
    mtp_free(data);
    return PTP_RC_OK;
}

/**
 * PTP获取对象
 *
 * @param params  PTP参数
 * @param handle  要获取的对象ID
 * @param object  要返回的获取到的对象
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_getobject (PTPParams* params, u32 handle, unsigned char** object)
{
    PTPContainer ptp;
    /* 初始化一个获取对象的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_GetObject, handle);
    /* 发送一个获取数据的事务*/
    return ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, object, NULL);
}


/**
 * PTP获取对象到文件描述符
 *
 * @param params PTP参数
 * @param handle 对象ID
 * @param fd     要写的文件描述符
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_getobject_tofd (PTPParams* params, u32 handle, int fd)
{
    PTPContainer    ptp;
    PTPDataHandler  handler;
    u16    ret;

    /* 初始化一个获取对象的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_GetObject, handle);
    ptp_init_fd_handler (&handler, fd);
    /* 发出获取数据的事务*/
    ret = ptp_transaction_new(params, &ptp, PTP_DP_GETDATA, 0, &handler);
    ptp_exit_fd_handler (&handler);
    return ret;
}

/**
 * PTP发送对象
 *
 * @param params PTP参数
 * @param object 对象
 * @param size   对象大小
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_sendobject (PTPParams* params, unsigned char* object, u64 size)
{
    PTPContainer ptp;
    /* 初始化一个发送对象的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_SendObject);
    /* 发出发送数据的事务*/
    return ptp_transaction(params, &ptp, PTP_DP_SENDDATA, size, &object, NULL);
}

/**
 * PTP删除对象
 *
 * @param params PTP参数
 * @param handle 对象ID
 * @param ofc    对象格式代码(可选)
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_deleteobject (PTPParams* params, u32 handle, u32 ofc)
{
    PTPContainer ptp;
    /* 初始化一个删除对象的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_DeleteObject, handle, ofc);
    /* 发出无数据的事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_NODATA, 0, NULL, NULL));
    /* 如果对象被缓存和可以被移除，清除缓存*/
    ptp_remove_object_from_cache(params, handle);
    return PTP_RC_OK;
}

/**
 * PTP移动对象
 *
 * @param params  PTP参数
 * @param handle  对象ID
 * @param storage 目的存储设备ID
 * @param parent  目的父目录ID
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_moveobject (PTPParams* params, u32 handle, u32 storage, u32 parent)
{
    PTPContainer ptp;
    /* 初始化一个移动对象的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_MoveObject, handle, storage, parent);
    /* 发出无数据的事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_NODATA, 0, NULL, NULL));
    /* 如果对象被缓存和可以被移除，清除缓存*/
    ptp_remove_object_from_cache(params, handle);
    return PTP_RC_OK;
}

/**
 * PTP复制对象
 *
 * @param params  PTP参数
 * @param handle  对象ID
 * @param storage 目的存储设备ID
 * @param parent  目的父目录ID
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_copyobject (PTPParams* params, u32 handle, u32 storage, u32 parent)
{
    PTPContainer ptp;
    /* 初始化一个复制对象的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_CopyObject, handle, storage, parent);
    /* 发出无数据的事务*/
    return ptp_transaction(params, &ptp, PTP_DP_NODATA, 0, NULL, NULL);
}

/**
 * 获取存储设备的信息
 *
 * @param params      PTP参数
 * @param storageid   要获取信息的存储设备ID
 * @param storageinfo 要返回的获取到的存储设备信息
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_getstorageinfo (PTPParams* params, u32 storageid,
            PTPStorageInfo* storageinfo)
{
    PTPContainer    ptp;
    unsigned char   *data = NULL;
    unsigned int    size;

    /* 初始化一个获取存储设备信息的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_GetStorageInfo, storageid);
    /* 发出获取数据是事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &size));
    if (!data || !size)
        return PTP_RC_GeneralError;
    memset(storageinfo, 0, sizeof(*storageinfo));
    /* 解析存储设备信息*/
    if (!ptp_unpack_SI(params, data, storageinfo, size)) {
        mtp_free(data);
        return PTP_RC_GeneralError;
    }
    mtp_free(data);
    return PTP_RC_OK;
}

/**
 * 获取对象句柄
 *
 * @param params           PTP参数
 * @param storage          存储ID
 * @param objectformatcode 对象格式
 * @param associationOH    关联的对象句柄
 * @param objecthandles    返回的对象句柄
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_getobjecthandles (PTPParams* params, u32 storage,
            u32 objectformatcode, u32 associationOH,
            PTPObjectHandles* objecthandles){
    PTPContainer    ptp;
    u16    ret;
    unsigned char   *data = NULL;
    unsigned int    size;

    objecthandles->Handler = NULL;
    objecthandles->n = 0;

    /* 初始化一个获取对象句柄的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_GetObjectHandles, storage, objectformatcode, associationOH);
    /* 发出获取数据是事务*/
    ret = ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &size);
    if (ret == PTP_RC_OK) {
        /* 解包对象句柄*/
        ptp_unpack_OH(params, data, objecthandles, size);
    } else {
        if ((storage == 0xffffffff) && (objectformatcode == 0) &&
            (associationOH == 0)) {
            /* 当在所有的存储中查询所有对象句柄并且得到错误，则把它当作“0句柄”处理*/
            objecthandles->Handler = NULL;
            objecthandles->n = 0;
            ret = PTP_RC_OK;
        }
    }
    mtp_free(data);
    return ret;
}

/**
 * 发送对象信息
 *
 * @param params       PTP参数
 * @param store        相关存储设备的ID
 * @param parenthandle 父目录的ID
 * @param handle       对象ID
 * @param objectinfo   对象信息
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_sendobjectinfo (PTPParams* params, u32* store,
                        u32* parenthandle, u32* handle, PTPObjectInfo* objectinfo)
{
    PTPContainer    ptp;
    u16    ret;
    unsigned char   *data = NULL;
    u32    size;

    /* 初始化一个发送对象信息的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_SendObjectInfo, *store, *parenthandle);
    /* 打包对象信息*/
    size = ptp_pack_OI(params, objectinfo, &data);
    /* 启动发送数据事务*/
    ret = ptp_transaction(params, &ptp, PTP_DP_SENDDATA, size, &data, NULL);
    mtp_free(data);
    *store = ptp.Param1;
    *parenthandle = ptp.Param2;
    *handle = ptp.Param3;
    return ret;
}

/**
 * 从文件描述符中发送对象
 *
 * @param params PTP参数
 * @param fd     文件描述符
 * @param size   文件大小
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_sendobject_fromfd (PTPParams* params, int fd, u64 size)
{
    PTPContainer    ptp;
    PTPDataHandler  handler;
    uint16_t    ret;
    /* 初始化一个发送对象的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_SendObject);
    /* 初始化文件描述符句柄*/
    ptp_init_fd_handler (&handler, fd);
    /* 发起发送数据事务*/
    ret = ptp_transaction_new(params, &ptp, PTP_DP_SENDDATA, size, &handler);
    /* 释放句柄*/
    ptp_exit_fd_handler (&handler);
    return ret;
}

#define PROPCACHE_TIMEOUT 5 /* 秒 */
/*
 * 获取设备属性描述符
 *
 * @param params PTP参数
 * @param propcode
 * @param devicepropertydesc 设备属性描述符
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_getdevicepropdesc (PTPParams* params, u16 propcode,
            PTPDevicePropDesc* devicepropertydesc)
{
    PTPContainer    ptp;
    u16    ret = PTP_RC_OK;
    unsigned char   *data = NULL;
    unsigned int    size;
    /* 初始化一个获取设备属性描述符的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_GetDevicePropDesc, propcode);
    /* 发送获取数据的事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &size));

    if (!data) {
        ptp_debug (params, "no data received for getdevicepropdesc");
        return PTP_RC_InvalidDevicePropFormat;
    }

    if (params->device_flags & DEVICE_FLAG_OLYMPUS_XML_WRAPPED) {
#ifdef HAVE_LIBXML2
        xmlNodePtr  code;

        ret = ptp_olympus_parse_output_xml (params,(char*)data,size,&code);
        if (ret == PTP_RC_OK) {
            int x;

            if (    (xmlChildElementCount(code) == 1) &&
                    (!strcmp((char*)code->name,"c1014"))
                    ) {
                code = xmlFirstElementChild (code);

                if (    (sscanf((char*)code->name,"p%x", &x)) &&
                        (x == propcode)
                        ) {
                    ret = parse_9301_propdesc (params, xmlFirstElementChild (code), devicepropertydesc);
                    xmlFreeDoc(code->doc);
                }
            }
        } else {
            ptp_debug(params,"failed to parse output xml, ret %x?", ret);
        }
#endif
    } else {
        /* 解析设备属性描述符*/
        if (!ptp_unpack_DPD(params, data, devicepropertydesc, size)) {
            ptp_debug(params,"failed to unpack DPD of propcode 0x%04x, likely corrupted?", propcode);
            mtp_free (data);
            return PTP_RC_InvalidDevicePropFormat;
        }
    }
    mtp_free(data);
    return ret;
}

/*
 * 获取设备属性值
 *
 * @param params           PTP参数
 * @param propcode         属性代码
 * @param PTPPropertyValue 设备属性值
 * @param datatype         数据类型
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_getdevicepropvalue (PTPParams* params, u16 propcode,
                            PTPPropertyValue* value, u16 datatype){
    PTPContainer    ptp;
    unsigned char   *data = NULL;
    unsigned int    size, offset = 0;
    u16    ret;

    /* 初始化一个获取设备属性值的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_GetDevicePropValue, propcode);
    /* 发送获取数据的事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &size));
    /* 解包设备属性值*/
    ret = ptp_unpack_DPV(params, data, &offset, size, value, datatype) ? PTP_RC_OK : PTP_RC_GeneralError;
    if (ret != PTP_RC_OK)
        ptp_debug (params, "ptp_getdevicepropvalue: unpacking DPV failed");
    mtp_free(data);
    return ret;
}

/**
 * 佳能获取对象信息
 *
 * @param params  PTP对象
 * @param store   存储ID
 * @param p2      未知 (默认是0)
 * @param parent  父对象(如果是0xffffffff， 则父对象的最顶层文件夹)
 * @param handle  对象ID(如果是0， 则读取属于父对象的所有对象的记录，如果不为0，则读取这个对象的记录)
 * @param entries 佳能文件夹入口
 * @param entnum  文件夹入口数目
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_canon_getobjectinfo (PTPParams* params, u32 store, u32 p2,
                            u32 parent, u32 handle,
                            PTPCANONFolderEntry** entries, u32* entnum){
    PTPContainer    ptp;
    u16    ret;
    unsigned char   *data = NULL;
    unsigned int    i, size;

    *entnum = 0;
    *entries = NULL;
    /* 初始一个获取节能对象扩展信息的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_CANON_GetObjectInfoEx, store, p2, parent, handle);
    data = NULL;
    size = 0;
    /* 发送获取数据的事务*/
    ret = ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &size);
    if (ret != PTP_RC_OK)
        goto exit;
    if (!data)
        return ret;
    if (ptp.Param1 > size / PTP_CANON_FolderEntryLen) {
        ptp_debug (params, "param1 is %d, size is only %d", ptp.Param1, size);
        ret = PTP_RC_GeneralError;
        goto exit;
    }

    *entnum = ptp.Param1;
    *entries = mtp_calloc(*entnum, sizeof(PTPCANONFolderEntry));
    if (*entries == NULL) {
        ret = PTP_RC_GeneralError;
        goto exit;
    }
    for(i = 0; i < (*entnum); i++) {
        if (size < i * PTP_CANON_FolderEntryLen)
            break;
        /* 解包佳能文件夹入口*/
        ptp_unpack_Canon_FE(params,
                            data + i * PTP_CANON_FolderEntryLen,
                            &((*entries)[i]));
    }

exit:
    mtp_free (data);
    return ret;
}

/**
 * 获取对象可能支持的属性
 *
 * @param params  PTP参数
 * @param ofc     对象格式代码
 * @param propnum 支持的属性数量
 * @param props   要返回的属性数组
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_mtp_getobjectpropssupported (PTPParams* params, u16 ofc,
                                     u32 *propnum, u16 **props) {
    PTPContainer    ptp;
    unsigned char   *data = NULL;
    unsigned int    xsize = 0;

    /* 初始一个获取节能对象支持属性的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_MTP_GetObjectPropsSupported, ofc);
    /* 发送一个获取数据事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &xsize));
    if (!data)
        return PTP_RC_GeneralError;
    /* 解包16位数组*/
    *propnum = ptp_unpack_uint16_t_array (params, data, 0, xsize, props);
    mtp_free(data);
    return PTP_RC_OK;
}

/**
 * 这个命令获取对象属性描述
 *
 * @param params PTP参数结构体
 * @param opc 对象属性代码
 * @param ofc 对象格式代码
 * @param opd PTP对象属性描述符
 *
 * @return 成功返回PTP_RC_OK
 *
 **/
u16 ptp_mtp_getobjectpropdesc (PTPParams* params, u16 opc, u16 ofc, PTPObjectPropDesc *opd) {
    PTPContainer    ptp;
    unsigned char   *data = NULL;
    unsigned int    size;
    /* 初始化一个获取对象属性描述符的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_MTP_GetObjectPropDesc, opc, ofc);
    /* 发送事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &size));
    /* 解析对象属性描述符*/
    ptp_unpack_OPD (params, data, opd, size);
    mtp_free(data);
    return PTP_RC_OK;
}

/**
 * 这个命令获取对象属性值
 *
 * @param params   PTP参数结构体
 * @param oid      对象ID
 * @param opc      对象属性代码
 * @param value    返回的属性值结构体
 * @param datatype 属性值数据类型
 *
 * @return 成功返回PTP_RC_OK
 *
 **/
u16 ptp_mtp_getobjectpropvalue (PTPParams* params, u32 oid, u16 opc,
                                PTPPropertyValue *value, u16 datatype) {
    PTPContainer    ptp;
    u16    ret = PTP_RC_OK;
    unsigned char   *data = NULL;
    unsigned int    size, offset = 0;
    /* 初始化一个获取对象属性值的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_MTP_GetObjectPropValue, oid, opc);
    /* 发送获取数据事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &size));
    /* 解析对象属值*/
    if (!ptp_unpack_DPV(params, data, &offset, size, value, datatype)) {
        ptp_debug (params, "ptp_mtp_getobjectpropvalue: unpacking DPV failed");
        ret = PTP_RC_GeneralError;
    }
    mtp_free(data);
    return ret;
}

/**
 * 设置一个对象的属性值
 *
 * @param params   PTP参数
 * @param oid      对象ID
 * @param opc      对象属性代码
 * @param value    要设置的属性值
 * @param datatype 数据类型
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_mtp_setobjectpropvalue (PTPParams* params, u32 oid, u16 opc,
                                PTPPropertyValue *value, u16 datatype) {
    PTPContainer    ptp;
    u16    ret;
    unsigned char   *data = NULL;
    u32    size;
    /* 初始化一个设置对象属性值的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_MTP_SetObjectPropValue, oid, opc);
    /* 打包设备属性值*/
    size = ptp_pack_DPV(params, value, &data, datatype);
    /* 发送发现数据事务*/
    ret = ptp_transaction(params, &ptp, PTP_DP_SENDDATA, size, &data, NULL);
    mtp_free(data);
    return ret;
}

/**
 * PTP获取对象引用
 *
 * @param params   PTP参数
 * @param handle   对象ID
 * @param ohArray  要返回的32位的数组
 * @param arraylen 返回的数组长度
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_mtp_getobjectreferences (PTPParams* params, u32 handle, u32** ohArray, u32* arraylen)
{
    PTPContainer    ptp;
    unsigned char   *data = NULL;
    unsigned int    size;
    /* 初始化一个获取对象引用的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_MTP_GetObjectReferences, handle);
    /* 发送获取数据事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data , &size));
    /* Sandisk Sansa跳过数据阶段，但响应返回OK，这里会返回NULL，处理它 */
    if ((data == NULL) || (size == 0)) {
        *arraylen = 0;
        *ohArray = NULL;
    } else {
        /* 解包数组*/
        *arraylen = ptp_unpack_uint32_t_array(params, data , 0, size, ohArray);
    }
    mtp_free(data);
    return PTP_RC_OK;
}

/**
 * PTP设置对象引用
 *
 * @param params   PTP参数
 * @param handle   新的对象ID
 * @param ohArray  要设置的数组
 * @param arraylen 数组长度
 *
 * @return 成功返回PTP_RC_OK
 */
u16 ptp_mtp_setobjectreferences (PTPParams* params, u32 handle, u32* ohArray, u32 arraylen)
{
    PTPContainer    ptp;
    u16    ret;
    unsigned char   *data = NULL;
    u32    size;
    /* 初始化一个设置对象引用的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_MTP_SetObjectReferences, handle);
    /* 打包32位数组*/
    size = ptp_pack_uint32_t_array(params, ohArray, arraylen, &data);
    /* 发送发送数据事务*/
    ret = ptp_transaction(params, &ptp, PTP_DP_SENDDATA, size, &data, NULL);
    mtp_free(data);
    return ret;
}

/**
 * PTP获取对象属性通用函数
 *
 * @param params PTP参数结构体
 * @param handle
 * @param props     要返回的MTP属性结构体
 * @param nrofprops 要返回的MTP属性的数量
 * @return 支持返回PTP_RC_OK。
 **/
u16
ptp_mtp_getobjectproplist_generic (PTPParams* params, u32 handle, u32 formats, u32 properties, u32 propertygroups, u32 level, MTPProperties **props, int *nrofprops)
{
    PTPContainer    ptp;
    unsigned char   *data = NULL;
    unsigned int    size;

    /* 初始化一个获取对象属性的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_MTP_GetObjPropList, handle, formats, properties, propertygroups, level);
    /* 发送获取数据的事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_GETDATA, 0, &data, &size));
    /* 解包对象属性*/
    *nrofprops = ptp_unpack_OPL(params, data, props, size);
    mtp_free(data);
    return PTP_RC_OK;
}

u16
ptp_mtp_getobjectproplist_level (PTPParams* params, u32 handle, u32 level, MTPProperties **props, int *nrofprops)
{
    return ptp_mtp_getobjectproplist_generic (params, handle,
             0x00000000U,  /* 0x00000000U 应该是 “所有格式” */
             0xFFFFFFFFU,  /* 0xFFFFFFFFU 应该是 “所有属性” */
             0,
             level,
             props,
             nrofprops);
}

/**
 * PTP获取对象信息
 *
 * @param params    PTP参数结构体
 * @param handle    对象ID
 * @param props     要返回的MTP属性结构体
 * @param nrofprops 要返回的MTP属性的数量
 * @return 支持返回PTP_RC_OK。
 **/
u16 ptp_mtp_getobjectproplist (PTPParams* params, u32 handle, MTPProperties **props, int *nrofprops)
{
    return ptp_mtp_getobjectproplist_level(params, handle, 0xFFFFFFFFU, props, nrofprops);
}

/**
 * PTP获取单个对象信息
 *
 * @param params    PTP参数结构体
 * @param handle    对象ID
 * @param props     要返回的MTP属性结构体
 * @param nrofprops 要返回的MTP属性的数量
 * @return 支持返回PTP_RC_OK。
 **/
u16 ptp_mtp_getobjectproplist_single (PTPParams* params, u32 handle, MTPProperties **props, int *nrofprops)
{
    return ptp_mtp_getobjectproplist_level(params, handle, 0, props, nrofprops);
}

/**
 * PTP设置对象属性列表
 *
 * @param params    PTP参数结构体
 * @param props     要设置的MTP属性列表
 * @param nrofprops 要设置的MTP属性的数量
 *
 * @return 支持返回PTP_RC_OK。
 */
u16 ptp_mtp_setobjectproplist (PTPParams* params, MTPProperties *props, int nrofprops)
{
    PTPContainer    ptp;
    unsigned char   *data = NULL;
    u32    size;
    /* 初始化一个设备对象属性列表的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_MTP_SetObjPropList);
    /* 打包对象属性列表1*/
    size = ptp_pack_OPL(params,props,nrofprops,&data);
    /* 发起发送数据事务*/
    CHECK_PTP_RC(ptp_transaction(params, &ptp, PTP_DP_SENDDATA, size, &data, NULL));
    mtp_free(data);
    return PTP_RC_OK;
}

/**
 * PTP发送对象属性链表函数
 *
 * @param params       PTP参数
 * @param store        相关的存储设备的ID
 * @param parenthandle 父目录的ID
 * @param handle       新的对象的ID
 * @param objecttype   新的对象的类型
 * @param objectsize   新的对象的大小
 * @param props        新的对象的属性
 * @param nrofprops    新的对象的属性数量
 *
 * @return 成功返回PTP_RC_OK。
 */
u16 ptp_mtp_sendobjectproplist (PTPParams* params, u32* store, u32* parenthandle, u32* handle,
                                u16 objecttype, u64 objectsize, MTPProperties *props, int nrofprops){
    PTPContainer    ptp;
    u16    ret;
    unsigned char   *data = NULL;
    u32    size;

    /* 初始化一个发送对象属性链表的PTP容器*/
    PTP_CNT_INIT(ptp, PTP_OC_MTP_SendObjectPropList, *store, *parenthandle, (u32) objecttype,
                 (u32) (objectsize >> 32), (u32) (objectsize & 0xffffffffU));

    /* 设置新的对象的对象句柄为0*/
    size = ptp_pack_OPL(params, props, nrofprops, &data);
    /* 启动发送数据事务*/
    ret = ptp_transaction(params, &ptp, PTP_DP_SENDDATA, size, &data, NULL);
    mtp_free(data);
    *store = ptp.Param1;
    *parenthandle = ptp.Param2;
    *handle = ptp.Param3;

    return ret;
}

/**
 * 检查设备是否支持属性
 *
 * @param params PTP参数结构体
 * @param property 要检查是否支持的属性
 * @return 支持返回1。
 */
int ptp_property_issupported(PTPParams* params, u16 property)
{
    unsigned int i;

    for (i = 0;i < params->deviceinfo.DevicePropertiesSupported_len; i++)
        if (params->deviceinfo.DevicePropertiesSupported[i] == property)
            return 1;
    return 0;
}

/**
 * PTP释放对象信息
 *
 * @param oi PTP对象信息
 **/
void ptp_free_objectinfo (PTPObjectInfo *oi)
{
    if (!oi)
        return;
    /* 释放文件名*/
    mtp_free (oi->Filename);
    oi->Filename = NULL;
    /* 释放密码*/
    mtp_free (oi->Keywords);
    oi->Keywords = NULL;
}

/**
 * PTP释放对象
 *
 * @param ob PTP对象
 **/
void ptp_free_object (PTPObject *ob)
{
    unsigned int i;
    if (!ob) return;
    /* 释放对象信息*/
    ptp_free_objectinfo (&ob->oi);
    /* 销毁MTP属性*/
    for (i = 0;i < ob->nrofmtpprops;i++)
        ptp_destroy_object_prop(&ob->mtpprops[i]);
    ob->flags = 0;
}

/* PTP 错误描述 */
static struct {
    u16 rc;
    u16 vendor;
    const char *txt;
} ptp_errors[] = {
    {PTP_RC_Undefined,      0, "PTP Undefined Error"},
    {PTP_RC_OK,         0, "PTP OK!"},
    {PTP_RC_GeneralError,       0, "PTP General Error"},
    {PTP_RC_SessionNotOpen,     0, "PTP Session Not Open"},
    {PTP_RC_InvalidTransactionID,   0, "PTP Invalid Transaction ID"},
    {PTP_RC_OperationNotSupported,  0, "PTP Operation Not Supported"},
    {PTP_RC_ParameterNotSupported,  0, "PTP Parameter Not Supported"},
    {PTP_RC_IncompleteTransfer, 0, "PTP Incomplete Transfer"},
    {PTP_RC_InvalidStorageId,   0, "PTP Invalid Storage ID"},
    {PTP_RC_InvalidObjectHandle,    0, "PTP Invalid Object Handle"},
    {PTP_RC_DevicePropNotSupported, 0, "PTP Device Prop Not Supported"},
    {PTP_RC_InvalidObjectFormatCode,0, "PTP Invalid Object Format Code"},
    {PTP_RC_StoreFull,      0, "PTP Store Full"},
    {PTP_RC_ObjectWriteProtected,   0, "PTP Object Write Protected"},
    {PTP_RC_StoreReadOnly,      0, "PTP Store Read Only"},
    {PTP_RC_AccessDenied,       0, "PTP Access Denied"},
    {PTP_RC_NoThumbnailPresent, 0, "PTP No Thumbnail Present"},
    {PTP_RC_SelfTestFailed,     0, "PTP Self Test Failed"},
    {PTP_RC_PartialDeletion,    0, "PTP Partial Deletion"},
    {PTP_RC_StoreNotAvailable,  0, "PTP Store Not Available"},
    {PTP_RC_SpecificationByFormatUnsupported, 0, "PTP Specification By Format Unsupported"},
    {PTP_RC_NoValidObjectInfo,  0, "PTP No Valid Object Info"},
    {PTP_RC_InvalidCodeFormat,  0, "PTP Invalid Code Format"},
    {PTP_RC_UnknownVendorCode,  0, "PTP Unknown Vendor Code"},
    {PTP_RC_CaptureAlreadyTerminated, 0, "PTP Capture Already Terminated"},
    {PTP_RC_DeviceBusy,     0, "PTP Device Busy"},
    {PTP_RC_InvalidParentObject,    0, "PTP Invalid Parent Object"},
    {PTP_RC_InvalidDevicePropFormat,0, "PTP Invalid Device Prop Format"},
    {PTP_RC_InvalidDevicePropValue, 0, "PTP Invalid Device Prop Value"},
    {PTP_RC_InvalidParameter,   0, "PTP Invalid Parameter"},
    {PTP_RC_SessionAlreadyOpened,   0, "PTP Session Already Opened"},
    {PTP_RC_TransactionCanceled,    0, "PTP Transaction Canceled"},
    {PTP_RC_SpecificationOfDestinationUnsupported, 0, "PTP Specification Of Destination Unsupported"},

    {PTP_RC_EK_FilenameRequired,    PTP_VENDOR_EASTMAN_KODAK, "Filename Required"},
    {PTP_RC_EK_FilenameConflicts,   PTP_VENDOR_EASTMAN_KODAK, "Filename Conflicts"},
    {PTP_RC_EK_FilenameInvalid, PTP_VENDOR_EASTMAN_KODAK, "Filename Invalid"},

    {PTP_RC_NIKON_HardwareError,        PTP_VENDOR_NIKON, "Hardware Error"},
    {PTP_RC_NIKON_OutOfFocus,       PTP_VENDOR_NIKON, "Out of Focus"},
    {PTP_RC_NIKON_ChangeCameraModeFailed,   PTP_VENDOR_NIKON, "Change Camera Mode Failed"},
    {PTP_RC_NIKON_InvalidStatus,        PTP_VENDOR_NIKON, "Invalid Status"},
    {PTP_RC_NIKON_SetPropertyNotSupported,  PTP_VENDOR_NIKON, "Set Property Not Supported"},
    {PTP_RC_NIKON_WbResetError,     PTP_VENDOR_NIKON, "Whitebalance Reset Error"},
    {PTP_RC_NIKON_DustReferenceError,   PTP_VENDOR_NIKON, "Dust Reference Error"},
    {PTP_RC_NIKON_ShutterSpeedBulb,     PTP_VENDOR_NIKON, "Shutter Speed Bulb"},
    {PTP_RC_NIKON_MirrorUpSequence,     PTP_VENDOR_NIKON, "Mirror Up Sequence"},
    {PTP_RC_NIKON_CameraModeNotAdjustFNumber, PTP_VENDOR_NIKON, "Camera Mode Not Adjust FNumber"},
    {PTP_RC_NIKON_NotLiveView,      PTP_VENDOR_NIKON, "Not in Liveview"},
    {PTP_RC_NIKON_MfDriveStepEnd,       PTP_VENDOR_NIKON, "Mf Drive Step End"},
    {PTP_RC_NIKON_MfDriveStepInsufficiency, PTP_VENDOR_NIKON, "Mf Drive Step Insufficiency"},
    {PTP_RC_NIKON_AdvancedTransferCancel,   PTP_VENDOR_NIKON, "Advanced Transfer Cancel"},

    {PTP_RC_CANON_UNKNOWN_COMMAND,  PTP_VENDOR_CANON, "Unknown Command"},
    {PTP_RC_CANON_OPERATION_REFUSED,PTP_VENDOR_CANON, "Operation Refused"},
    {PTP_RC_CANON_LENS_COVER,   PTP_VENDOR_CANON, "Lens Cover Present"},
    {PTP_RC_CANON_BATTERY_LOW,  PTP_VENDOR_CANON, "Battery Low"},
    {PTP_RC_CANON_NOT_READY,    PTP_VENDOR_CANON, "Camera Not Ready"},

    {PTP_ERROR_NODEVICE,        0, "PTP No Device"},
    {PTP_ERROR_TIMEOUT,     0, "PTP Timeout"},
    {PTP_ERROR_CANCEL,      0, "PTP Cancel Request"},
    {PTP_ERROR_BADPARAM,        0, "PTP Invalid Parameter"},
    {PTP_ERROR_RESP_EXPECTED,   0, "PTP Response Expected"},
    {PTP_ERROR_DATA_EXPECTED,   0, "PTP Data Expected"},
    {PTP_ERROR_IO,          0, "PTP I/O Error"},
    {0, 0, NULL}
};

/**
 * 获取PTP错误的描述
 *
 * @param ret    错误编号
 * @param vendor 自定义ID
 * @return 成功返回错误描述字符串
 **/
const char *ptp_strerror(u16 ret, u16 vendor)
{
    int i;

    for (i = 0; ptp_errors[i].txt != NULL; i++)
        if ((ptp_errors[i].rc == ret) && ((ptp_errors[i].vendor == 0) || (ptp_errors[i].vendor == vendor)))
            return ptp_errors[i].txt;
    return NULL;
}

const char* ptp_get_property_description(PTPParams* params, u16 dpc)
{
    int i;
    /* 设备属性描述 */
    struct {
        u16 dpc;
        const char *txt;
    } ptp_device_properties[] = {
        {PTP_DPC_Undefined,     N_("Undefined PTP Property")},
        {PTP_DPC_BatteryLevel,      N_("Battery Level")},
        {PTP_DPC_FunctionalMode,    N_("Functional Mode")},
        {PTP_DPC_ImageSize,     N_("Image Size")},
        {PTP_DPC_CompressionSetting,    N_("Compression Setting")},
        {PTP_DPC_WhiteBalance,      N_("White Balance")},
        {PTP_DPC_RGBGain,       N_("RGB Gain")},
        {PTP_DPC_FNumber,       N_("F-Number")},
        {PTP_DPC_FocalLength,       N_("Focal Length")},
        {PTP_DPC_FocusDistance,     N_("Focus Distance")},
        {PTP_DPC_FocusMode,     N_("Focus Mode")},
        {PTP_DPC_ExposureMeteringMode,  N_("Exposure Metering Mode")},
        {PTP_DPC_FlashMode,     N_("Flash Mode")},
        {PTP_DPC_ExposureTime,      N_("Exposure Time")},
        {PTP_DPC_ExposureProgramMode,   N_("Exposure Program Mode")},
        {PTP_DPC_ExposureIndex,
                    N_("Exposure Index (film speed ISO)")},
        {PTP_DPC_ExposureBiasCompensation,
                    N_("Exposure Bias Compensation")},
        {PTP_DPC_DateTime,      N_("Date & Time")},
        {PTP_DPC_CaptureDelay,      N_("Pre-Capture Delay")},
        {PTP_DPC_StillCaptureMode,  N_("Still Capture Mode")},
        {PTP_DPC_Contrast,      N_("Contrast")},
        {PTP_DPC_Sharpness,     N_("Sharpness")},
        {PTP_DPC_DigitalZoom,       N_("Digital Zoom")},
        {PTP_DPC_EffectMode,        N_("Effect Mode")},
        {PTP_DPC_BurstNumber,       N_("Burst Number")},
        {PTP_DPC_BurstInterval,     N_("Burst Interval")},
        {PTP_DPC_TimelapseNumber,   N_("Timelapse Number")},
        {PTP_DPC_TimelapseInterval, N_("Timelapse Interval")},
        {PTP_DPC_FocusMeteringMode, N_("Focus Metering Mode")},
        {PTP_DPC_UploadURL,     N_("Upload URL")},
        {PTP_DPC_Artist,        N_("Artist")},
        {PTP_DPC_CopyrightInfo,     N_("Copyright Info")},
        {PTP_DPC_SupportedStreams,  N_("Supported Streams")},
        {PTP_DPC_EnabledStreams,    N_("Enabled Streams")},
        {PTP_DPC_VideoFormat,       N_("Video Format")},
        {PTP_DPC_VideoResolution,   N_("Video Resolution")},
        {PTP_DPC_VideoQuality,      N_("Video Quality")},
        {PTP_DPC_VideoFrameRate,    N_("Video Framerate")},
        {PTP_DPC_VideoContrast,     N_("Video Contrast")},
        {PTP_DPC_VideoBrightness,   N_("Video Brightness")},
        {PTP_DPC_AudioFormat,       N_("Audio Format")},
        {PTP_DPC_AudioBitrate,      N_("Audio Bitrate")},
        {PTP_DPC_AudioSamplingRate, N_("Audio Samplingrate")},
        {PTP_DPC_AudioBitPerSample, N_("Audio Bits per sample")},
        {PTP_DPC_AudioVolume,       N_("Audio Volume")},
        {0,NULL}
    };
    struct {
        u16 dpc;
        const char *txt;
    } ptp_device_properties_EK[] = {
        {PTP_DPC_EK_ColorTemperature,   N_("Color Temperature")},
        {PTP_DPC_EK_DateTimeStampFormat, N_("Date Time Stamp Format")},
        {PTP_DPC_EK_BeepMode,       N_("Beep Mode")},
        {PTP_DPC_EK_VideoOut,       N_("Video Out")},
        {PTP_DPC_EK_PowerSaving,    N_("Power Saving")},
        {PTP_DPC_EK_UI_Language,    N_("UI Language")},
        {0,NULL}
    };

    struct {
        u16 dpc;
        const char *txt;
    } ptp_device_properties_Canon[] = {
        {PTP_DPC_CANON_BeepMode,    N_("Beep Mode")},
        {PTP_DPC_CANON_BatteryKind, N_("Battery Type")},
        {PTP_DPC_CANON_BatteryStatus,   N_("Battery Mode")},
        {PTP_DPC_CANON_UILockType,  N_("UILockType")},
        {PTP_DPC_CANON_CameraMode,  N_("Camera Mode")},
        {PTP_DPC_CANON_ImageQuality,    N_("Image Quality")},
        {PTP_DPC_CANON_FullViewFileFormat,  N_("Full View File Format")},
        {PTP_DPC_CANON_ImageSize,   N_("Image Size")},
        {PTP_DPC_CANON_SelfTime,    N_("Self Time")},
        {PTP_DPC_CANON_FlashMode,   N_("Flash Mode")},
        {PTP_DPC_CANON_Beep,        N_("Beep")},
        {PTP_DPC_CANON_ShootingMode,    N_("Shooting Mode")},
        {PTP_DPC_CANON_ImageMode,   N_("Image Mode")},
        {PTP_DPC_CANON_DriveMode,   N_("Drive Mode")},
        {PTP_DPC_CANON_EZoom,       N_("Zoom")},
        {PTP_DPC_CANON_MeteringMode,    N_("Metering Mode")},
        {PTP_DPC_CANON_AFDistance,  N_("AF Distance")},
        {PTP_DPC_CANON_FocusingPoint,   N_("Focusing Point")},
        {PTP_DPC_CANON_WhiteBalance,    N_("White Balance")},
        {PTP_DPC_CANON_SlowShutterSetting,  N_("Slow Shutter Setting")},
        {PTP_DPC_CANON_AFMode,      N_("AF Mode")},
        {PTP_DPC_CANON_ImageStabilization,      N_("Image Stabilization")},
        {PTP_DPC_CANON_Contrast,    N_("Contrast")},
        {PTP_DPC_CANON_ColorGain,   N_("Color Gain")},
        {PTP_DPC_CANON_Sharpness,   N_("Sharpness")},
        {PTP_DPC_CANON_Sensitivity, N_("Sensitivity")},
        {PTP_DPC_CANON_ParameterSet,    N_("Parameter Set")},
        {PTP_DPC_CANON_ISOSpeed,    N_("ISO Speed")},
        {PTP_DPC_CANON_Aperture,    N_("Aperture")},
        {PTP_DPC_CANON_ShutterSpeed,    N_("Shutter Speed")},
        {PTP_DPC_CANON_ExpCompensation, N_("Exposure Compensation")},
        {PTP_DPC_CANON_FlashCompensation,   N_("Flash Compensation")},
        {PTP_DPC_CANON_AEBExposureCompensation, N_("AEB Exposure Compensation")},
        {PTP_DPC_CANON_AvOpen,      N_("Av Open")},
        {PTP_DPC_CANON_AvMax,       N_("Av Max")},
        {PTP_DPC_CANON_FocalLength, N_("Focal Length")},
        {PTP_DPC_CANON_FocalLengthTele, N_("Focal Length Tele")},
        {PTP_DPC_CANON_FocalLengthWide, N_("Focal Length Wide")},
        {PTP_DPC_CANON_FocalLengthDenominator,  N_("Focal Length Denominator")},
        {PTP_DPC_CANON_CaptureTransferMode, N_("Capture Transfer Mode")},
        {PTP_DPC_CANON_Zoom,        N_("Zoom")},
        {PTP_DPC_CANON_NamePrefix,  N_("Name Prefix")},
        {PTP_DPC_CANON_SizeQualityMode, N_("Size Quality Mode")},
        {PTP_DPC_CANON_SupportedThumbSize,  N_("Supported Thumb Size")},
        {PTP_DPC_CANON_SizeOfOutputDataFromCamera,  N_("Size of Output Data from Camera")},
        {PTP_DPC_CANON_SizeOfInputDataToCamera,     N_("Size of Input Data to Camera")},
        {PTP_DPC_CANON_RemoteAPIVersion,N_("Remote API Version")},
        {PTP_DPC_CANON_FirmwareVersion, N_("Firmware Version")},
        {PTP_DPC_CANON_CameraModel, N_("Camera Model")},
        {PTP_DPC_CANON_CameraOwner, N_("Camera Owner")},
        {PTP_DPC_CANON_UnixTime,    N_("UNIX Time")},
        {PTP_DPC_CANON_CameraBodyID,    N_("Camera Body ID")},
        {PTP_DPC_CANON_CameraOutput,    N_("Camera Output")},
        {PTP_DPC_CANON_DispAv,      N_("Disp Av")},
        {PTP_DPC_CANON_AvOpenApex,  N_("Av Open Apex")},
        {PTP_DPC_CANON_DZoomMagnification,  N_("Digital Zoom Magnification")},
        {PTP_DPC_CANON_MlSpotPos,   N_("Ml Spot Position")},
        {PTP_DPC_CANON_DispAvMax,   N_("Disp Av Max")},
        {PTP_DPC_CANON_AvMaxApex,   N_("Av Max Apex")},
        {PTP_DPC_CANON_EZoomStartPosition,  N_("EZoom Start Position")},
        {PTP_DPC_CANON_FocalLengthOfTele,   N_("Focal Length Tele")},
        {PTP_DPC_CANON_EZoomSizeOfTele, N_("EZoom Size of Tele")},
        {PTP_DPC_CANON_PhotoEffect, N_("Photo Effect")},
        {PTP_DPC_CANON_AssistLight, N_("Assist Light")},
        {PTP_DPC_CANON_FlashQuantityCount,  N_("Flash Quantity Count")},
        {PTP_DPC_CANON_RotationAngle,   N_("Rotation Angle")},
        {PTP_DPC_CANON_RotationScene,   N_("Rotation Scene")},
        {PTP_DPC_CANON_EventEmulateMode,N_("Event Emulate Mode")},
        {PTP_DPC_CANON_DPOFVersion, N_("DPOF Version")},
        {PTP_DPC_CANON_TypeOfSupportedSlideShow,    N_("Type of Slideshow")},
        {PTP_DPC_CANON_AverageFilesizes,N_("Average Filesizes")},
        {PTP_DPC_CANON_ModelID,     N_("Model ID")},
        {0,NULL}
    };

    struct {
        u16 dpc;
        const char *txt;
    } ptp_device_properties_Nikon[] = {
        {PTP_DPC_NIKON_ShootingBank,            /* 0xD010 */
         N_("Shooting Bank")},
        {PTP_DPC_NIKON_ShootingBankNameA,       /* 0xD011 */
         N_("Shooting Bank Name A")},
        {PTP_DPC_NIKON_ShootingBankNameB,       /* 0xD012 */
         N_("Shooting Bank Name B")},
        {PTP_DPC_NIKON_ShootingBankNameC,       /* 0xD013 */
         N_("Shooting Bank Name C")},
        {PTP_DPC_NIKON_ShootingBankNameD,       /* 0xD014 */
         N_("Shooting Bank Name D")},
        {PTP_DPC_NIKON_ResetBank0,          /* 0xD015 */
         N_("Reset Bank 0")},
        {PTP_DPC_NIKON_RawCompression,          /* 0xD016 */
         N_("Raw Compression")},
        {PTP_DPC_NIKON_WhiteBalanceAutoBias,        /* 0xD017 */
         N_("Auto White Balance Bias")},
        {PTP_DPC_NIKON_WhiteBalanceTungstenBias,    /* 0xD018 */
         N_("Tungsten White Balance Bias")},
        {PTP_DPC_NIKON_WhiteBalanceFluorescentBias, /* 0xD019 */
         N_("Fluorescent White Balance Bias")},
        {PTP_DPC_NIKON_WhiteBalanceDaylightBias,    /* 0xD01a */
         N_("Daylight White Balance Bias")},
        {PTP_DPC_NIKON_WhiteBalanceFlashBias,       /* 0xD01b */
         N_("Flash White Balance Bias")},
        {PTP_DPC_NIKON_WhiteBalanceCloudyBias,      /* 0xD01c */
         N_("Cloudy White Balance Bias")},
        {PTP_DPC_NIKON_WhiteBalanceShadeBias,       /* 0xD01d */
         N_("Shady White Balance Bias")},
        {PTP_DPC_NIKON_WhiteBalanceColorTemperature,    /* 0xD01e */
         N_("White Balance Colour Temperature")},
        {PTP_DPC_NIKON_WhiteBalancePresetNo,        /* 0xD01f */
         N_("White Balance Preset Number")},
        {PTP_DPC_NIKON_WhiteBalancePresetName0,     /* 0xD020 */
         N_("White Balance Preset Name 0")},
        {PTP_DPC_NIKON_WhiteBalancePresetName1,     /* 0xD021 */
         N_("White Balance Preset Name 1")},
        {PTP_DPC_NIKON_WhiteBalancePresetName2,     /* 0xD022 */
         N_("White Balance Preset Name 2")},
        {PTP_DPC_NIKON_WhiteBalancePresetName3,     /* 0xD023 */
         N_("White Balance Preset Name 3")},
        {PTP_DPC_NIKON_WhiteBalancePresetName4,     /* 0xD024 */
         N_("White Balance Preset Name 4")},
        {PTP_DPC_NIKON_WhiteBalancePresetVal0,      /* 0xD025 */
         N_("White Balance Preset Value 0")},
        {PTP_DPC_NIKON_WhiteBalancePresetVal1,      /* 0xD026 */
         N_("White Balance Preset Value 1")},
        {PTP_DPC_NIKON_WhiteBalancePresetVal2,      /* 0xD027 */
         N_("White Balance Preset Value 2")},
        {PTP_DPC_NIKON_WhiteBalancePresetVal3,      /* 0xD028 */
         N_("White Balance Preset Value 3")},
        {PTP_DPC_NIKON_WhiteBalancePresetVal4,      /* 0xD029 */
         N_("White Balance Preset Value 4")},
        {PTP_DPC_NIKON_ImageSharpening,         /* 0xD02a */
         N_("Sharpening")},
        {PTP_DPC_NIKON_ToneCompensation,        /* 0xD02b */
         N_("Tone Compensation")},
        {PTP_DPC_NIKON_ColorModel,          /* 0xD02c */
         N_("Color Model")},
        {PTP_DPC_NIKON_HueAdjustment,           /* 0xD02d */
         N_("Hue Adjustment")},
        {PTP_DPC_NIKON_NonCPULensDataFocalLength,   /* 0xD02e */
         N_("Lens Focal Length (Non CPU)")},
        {PTP_DPC_NIKON_NonCPULensDataMaximumAperture,   /* 0xD02f */
         N_("Lens Maximum Aperture (Non CPU)")},
        {PTP_DPC_NIKON_ShootingMode,            /* 0xD030 */
         N_("Shooting Mode")},
        {PTP_DPC_NIKON_JPEG_Compression_Policy,     /* 0xD031 */
         N_("JPEG Compression Policy")},
        {PTP_DPC_NIKON_ColorSpace,          /* 0xD032 */
         N_("Color Space")},
        {PTP_DPC_NIKON_AutoDXCrop,          /* 0xD033 */
         N_("Auto DX Crop")},
        {PTP_DPC_NIKON_FlickerReduction,        /* 0xD034 */
         N_("Flicker Reduction")},
        {PTP_DPC_NIKON_RemoteMode,          /* 0xD035 */
         N_("Remote Mode")},
        {PTP_DPC_NIKON_VideoMode,           /* 0xD036 */
         N_("Video Mode")},
        {PTP_DPC_NIKON_EffectMode,          /* 0xD037 */
         N_("Effect Mode")},
        {PTP_DPC_NIKON_CSMMenuBankSelect,       /* 0xD040 */
         "PTP_DPC_NIKON_CSMMenuBankSelect"},
        {PTP_DPC_NIKON_MenuBankNameA,           /* 0xD041 */
         N_("Menu Bank Name A")},
        {PTP_DPC_NIKON_MenuBankNameB,           /* 0xD042 */
         N_("Menu Bank Name B")},
        {PTP_DPC_NIKON_MenuBankNameC,           /* 0xD043 */
         N_("Menu Bank Name C")},
        {PTP_DPC_NIKON_MenuBankNameD,           /* 0xD044 */
         N_("Menu Bank Name D")},
        {PTP_DPC_NIKON_ResetBank,           /* 0xD045 */
         N_("Reset Menu Bank")},
        {PTP_DPC_NIKON_A1AFCModePriority,       /* 0xD048 */
         "PTP_DPC_NIKON_A1AFCModePriority"},
        {PTP_DPC_NIKON_A2AFSModePriority,       /* 0xD049 */
         "PTP_DPC_NIKON_A2AFSModePriority"},
        {PTP_DPC_NIKON_A3GroupDynamicAF,        /* 0xD04a */
         "PTP_DPC_NIKON_A3GroupDynamicAF"},
        {PTP_DPC_NIKON_A4AFActivation,          /* 0xD04b */
         "PTP_DPC_NIKON_A4AFActivation"},
        {PTP_DPC_NIKON_FocusAreaIllumManualFocus,   /* 0xD04c */
         "PTP_DPC_NIKON_FocusAreaIllumManualFocus"},
        {PTP_DPC_NIKON_FocusAreaIllumContinuous,    /* 0xD04d */
         "PTP_DPC_NIKON_FocusAreaIllumContinuous"},
        {PTP_DPC_NIKON_FocusAreaIllumWhenSelected,  /* 0xD04e */
         "PTP_DPC_NIKON_FocusAreaIllumWhenSelected"},
        {PTP_DPC_NIKON_FocusAreaWrap,           /* 0xD04f */
         N_("Focus Area Wrap")},
        {PTP_DPC_NIKON_VerticalAFON,            /* 0xD050 */
         N_("Vertical AF On")},
        {PTP_DPC_NIKON_AFLockOn,            /* 0xD051 */
         N_("AF Lock On")},
        {PTP_DPC_NIKON_FocusAreaZone,           /* 0xD052 */
         N_("Focus Area Zone")},
        {PTP_DPC_NIKON_EnableCopyright,         /* 0xD053 */
         N_("Enable Copyright")},
        {PTP_DPC_NIKON_ISOAuto,             /* 0xD054 */
         N_("Auto ISO")},
        {PTP_DPC_NIKON_EVISOStep,           /* 0xD055 */
         N_("Exposure ISO Step")},
        {PTP_DPC_NIKON_EVStep,              /* 0xD056 */
         N_("Exposure Step")},
        {PTP_DPC_NIKON_EVStepExposureComp,      /* 0xD057 */
         N_("Exposure Compensation (EV)")},
        {PTP_DPC_NIKON_ExposureCompensation,        /* 0xD058 */
         N_("Exposure Compensation")},
        {PTP_DPC_NIKON_CenterWeightArea,        /* 0xD059 */
         N_("Centre Weight Area")},
        {PTP_DPC_NIKON_ExposureBaseMatrix,      /* 0xD05A */
         N_("Exposure Base Matrix")},
        {PTP_DPC_NIKON_ExposureBaseCenter,      /* 0xD05B */
         N_("Exposure Base Center")},
        {PTP_DPC_NIKON_ExposureBaseSpot,        /* 0xD05C */
         N_("Exposure Base Spot")},
        {PTP_DPC_NIKON_LiveViewAFArea,          /* 0xD05D */
         N_("Live View AF Area")},
        {PTP_DPC_NIKON_AELockMode,          /* 0xD05E */
         N_("Exposure Lock")},
        {PTP_DPC_NIKON_AELAFLMode,          /* 0xD05F */
         N_("Focus Lock")},
        {PTP_DPC_NIKON_LiveViewAFFocus,         /* 0xD061 */
         N_("Live View AF Focus")},
        {PTP_DPC_NIKON_MeterOff,            /* 0xD062 */
         N_("Auto Meter Off Time")},
        {PTP_DPC_NIKON_SelfTimer,           /* 0xD063 */
         N_("Self Timer Delay")},
        {PTP_DPC_NIKON_MonitorOff,          /* 0xD064 */
         N_("LCD Off Time")},
        {PTP_DPC_NIKON_ImgConfTime,         /* 0xD065 */
         N_("Img Conf Time")},
        {PTP_DPC_NIKON_AutoOffTimers,           /* 0xD066 */
         N_("Auto Off Timers")},
        {PTP_DPC_NIKON_AngleLevel,          /* 0xD067 */
         N_("Angle Level")},
        {PTP_DPC_NIKON_D1ShootingSpeed,         /* 0xD068 */
         N_("Shooting Speed")},
        {PTP_DPC_NIKON_D2MaximumShots,          /* 0xD069 */
         N_("Maximum Shots")},
        {PTP_DPC_NIKON_ExposureDelayMode,       /* 0xD06A */
         N_("Exposure delay mode")},
        {PTP_DPC_NIKON_LongExposureNoiseReduction,  /* 0xD06B */
         N_("Long Exposure Noise Reduction")},
        {PTP_DPC_NIKON_FileNumberSequence,      /* 0xD06C */
         N_("File Number Sequencing")},
        {PTP_DPC_NIKON_ControlPanelFinderRearControl,   /* 0xD06D */
         "PTP_DPC_NIKON_ControlPanelFinderRearControl"},
        {PTP_DPC_NIKON_ControlPanelFinderViewfinder,    /* 0xD06E */
         "PTP_DPC_NIKON_ControlPanelFinderViewfinder"},
        {PTP_DPC_NIKON_D7Illumination,          /* 0xD06F */
         N_("LCD Illumination")},
        {PTP_DPC_NIKON_NrHighISO,           /* 0xD070 */
         N_("High ISO noise reduction")},
        {PTP_DPC_NIKON_SHSET_CH_GUID_DISP,      /* 0xD071 */
         N_("On screen tips")},
        {PTP_DPC_NIKON_ArtistName,          /* 0xD072 */
         N_("Artist Name")},
        {PTP_DPC_NIKON_CopyrightInfo,           /* 0xD073 */
         N_("Copyright Information")},
        {PTP_DPC_NIKON_FlashSyncSpeed,          /* 0xD074 */
         N_("Flash Sync. Speed")},
        {PTP_DPC_NIKON_FlashShutterSpeed,       /* 0xD075 */
         N_("Flash Shutter Speed")},
        {PTP_DPC_NIKON_E3AAFlashMode,           /* 0xD076 */
         N_("Flash Mode")},
        {PTP_DPC_NIKON_E4ModelingFlash,         /* 0xD077 */
         N_("Modeling Flash")},
        {PTP_DPC_NIKON_BracketSet,          /* 0xD078 */
         N_("Bracket Set")},
        {PTP_DPC_NIKON_E6ManualModeBracketing,      /* 0xD079 */
         N_("Manual Mode Bracketing")},
        {PTP_DPC_NIKON_BracketOrder,            /* 0xD07A */
         N_("Bracket Order")},
        {PTP_DPC_NIKON_E8AutoBracketSelection,      /* 0xD07B */
         N_("Auto Bracket Selection")},
        {PTP_DPC_NIKON_BracketingSet, N_("NIKON Auto Bracketing Set")}, /* 0xD07C */
        {PTP_DPC_NIKON_F1CenterButtonShootingMode,  /* 0xD080 */
         N_("Center Button Shooting Mode")},
        {PTP_DPC_NIKON_CenterButtonPlaybackMode,    /* 0xD081 */
         N_("Center Button Playback Mode")},
        {PTP_DPC_NIKON_F2Multiselector,         /* 0xD082 */
         N_("Multiselector")},
        {PTP_DPC_NIKON_F3PhotoInfoPlayback,     /* 0xD083 */
         N_("Photo Info. Playback")},
        {PTP_DPC_NIKON_F4AssignFuncButton,      /* 0xD084 */
         N_("Assign Func. Button")},
        {PTP_DPC_NIKON_F5CustomizeCommDials,        /* 0xD085 */
         N_("Customise Command Dials")},
        {PTP_DPC_NIKON_ReverseCommandDial,      /* 0xD086 */
         N_("Reverse Command Dial")},
        {PTP_DPC_NIKON_ApertureSetting,         /* 0xD087 */
         N_("Aperture Setting")},
        {PTP_DPC_NIKON_MenusAndPlayback,        /* 0xD088 */
         N_("Menus and Playback")},
        {PTP_DPC_NIKON_F6ButtonsAndDials,       /* 0xD089 */
         N_("Buttons and Dials")},
        {PTP_DPC_NIKON_NoCFCard,            /* 0xD08A */
         N_("No CF Card Release")},
        {PTP_DPC_NIKON_CenterButtonZoomRatio,       /* 0xD08B */
         N_("Center Button Zoom Ratio")},
        {PTP_DPC_NIKON_FunctionButton2,         /* 0xD08C */
         N_("Function Button 2")},
        {PTP_DPC_NIKON_AFAreaPoint,         /* 0xD08D */
         N_("AF Area Point")},
        {PTP_DPC_NIKON_NormalAFOn,          /* 0xD08E */
         N_("Normal AF On")},
        {PTP_DPC_NIKON_CleanImageSensor,        /* 0xD08F */
         N_("Clean Image Sensor")},
        {PTP_DPC_NIKON_ImageCommentString,      /* 0xD090 */
         N_("Image Comment String")},
        {PTP_DPC_NIKON_ImageCommentEnable,      /* 0xD091 */
         N_("Image Comment Enable")},
        {PTP_DPC_NIKON_ImageRotation,           /* 0xD092 */
         N_("Image Rotation")},
        {PTP_DPC_NIKON_ManualSetLensNo,         /* 0xD093 */
         N_("Manual Set Lens Number")},
        {PTP_DPC_NIKON_MovScreenSize,           /* 0xD0A0 */
         N_("Movie Screen Size")},
        {PTP_DPC_NIKON_MovVoice,            /* 0xD0A1 */
         N_("Movie Voice")},
        {PTP_DPC_NIKON_MovMicrophone,           /* 0xD0A2 */
         N_("Movie Microphone")},
        {PTP_DPC_NIKON_MovFileSlot,         /* 0xD0A3 */
         N_("Movie Card Slot")},
        {PTP_DPC_NIKON_ManualMovieSetting,      /* 0xD0A6 */
         N_("Manual Movie Setting")},
        {PTP_DPC_NIKON_MovQuality,          /* 0xD0A7 */
         N_("Movie Quality")},
        {PTP_DPC_NIKON_MonitorOffDelay,         /* 0xD0B3 */
         N_("Monitor Off Delay")},
        {PTP_DPC_NIKON_Bracketing,          /* 0xD0C0 */
         N_("Bracketing Enable")},
        {PTP_DPC_NIKON_AutoExposureBracketStep,     /* 0xD0C1 */
         N_("Exposure Bracketing Step")},
        {PTP_DPC_NIKON_AutoExposureBracketProgram,  /* 0xD0C2 */
         N_("Exposure Bracketing Program")},
        {PTP_DPC_NIKON_AutoExposureBracketCount,    /* 0xD0C3 */
         N_("Auto Exposure Bracket Count")},
        {PTP_DPC_NIKON_WhiteBalanceBracketStep, N_("White Balance Bracket Step")}, /* 0xD0C4 */
        {PTP_DPC_NIKON_WhiteBalanceBracketProgram, N_("White Balance Bracket Program")}, /* 0xD0C5 */
        {PTP_DPC_NIKON_LensID,              /* 0xD0E0 */
         N_("Lens ID")},
        {PTP_DPC_NIKON_LensSort,            /* 0xD0E1 */
         N_("Lens Sort")},
        {PTP_DPC_NIKON_LensType,            /* 0xD0E2 */
         N_("Lens Type")},
        {PTP_DPC_NIKON_FocalLengthMin,          /* 0xD0E3 */
         N_("Min. Focal Length")},
        {PTP_DPC_NIKON_FocalLengthMax,          /* 0xD0E4 */
         N_("Max. Focal Length")},
        {PTP_DPC_NIKON_MaxApAtMinFocalLength,       /* 0xD0E5 */
         N_("Max. Aperture at Min. Focal Length")},
        {PTP_DPC_NIKON_MaxApAtMaxFocalLength,       /* 0xD0E6 */
         N_("Max. Aperture at Max. Focal Length")},
        {PTP_DPC_NIKON_FinderISODisp,           /* 0xD0F0 */
         N_("Finder ISO Display")},
        {PTP_DPC_NIKON_AutoOffPhoto,            /* 0xD0F2 */
         N_("Auto Off Photo")},
        {PTP_DPC_NIKON_AutoOffMenu,         /* 0xD0F3 */
         N_("Auto Off Menu")},
        {PTP_DPC_NIKON_AutoOffInfo,         /* 0xD0F4 */
         N_("Auto Off Info")},
        {PTP_DPC_NIKON_SelfTimerShootNum,       /* 0xD0F5 */
         N_("Self Timer Shot Number")},
        {PTP_DPC_NIKON_VignetteCtrl,            /* 0xD0F7 */
         N_("Vignette Control")},
        {PTP_DPC_NIKON_AutoDistortionControl,       /* 0xD0F8 */
         N_("Auto Distortion Control")},
        {PTP_DPC_NIKON_SceneMode,           /* 0xD0F9 */
         N_("Scene Mode")},
        {PTP_DPC_NIKON_ExposureTime,            /* 0xD100 */
         N_("Nikon Exposure Time")},
        {PTP_DPC_NIKON_ACPower, N_("AC Power")},    /* 0xD101 */
        {PTP_DPC_NIKON_WarningStatus, N_("Warning Status")},/* 0xD102 */
        {PTP_DPC_NIKON_MaximumShots,            /* 0xD103 */
         N_("Maximum Shots")},
        {PTP_DPC_NIKON_AFLockStatus, N_("AF Locked")},/* 0xD104 */
        {PTP_DPC_NIKON_AELockStatus, N_("AE Locked")},/* 0xD105 */
        {PTP_DPC_NIKON_FVLockStatus, N_("FV Locked")},/* 0xD106 */
        {PTP_DPC_NIKON_AutofocusLCDTopMode2,        /* 0xD107 */
         N_("AF LCD Top Mode 2")},
        {PTP_DPC_NIKON_AutofocusArea,           /* 0xD108 */
         N_("Active AF Sensor")},
        {PTP_DPC_NIKON_FlexibleProgram,         /* 0xD109 */
         N_("Flexible Program")},
        {PTP_DPC_NIKON_LightMeter,          /* 0xD10A */
         N_("Exposure Meter")},
        {PTP_DPC_NIKON_RecordingMedia,          /* 0xD10B */
         N_("Recording Media")},
        {PTP_DPC_NIKON_USBSpeed,            /* 0xD10C */
         N_("USB Speed")},
        {PTP_DPC_NIKON_CCDNumber,           /* 0xD10D */
         N_("CCD Serial Number")},
        {PTP_DPC_NIKON_CameraOrientation,       /* 0xD10E */
         N_("Camera Orientation")},
        {PTP_DPC_NIKON_GroupPtnType,            /* 0xD10F */
         N_("Group PTN Type")},
        {PTP_DPC_NIKON_FNumberLock,         /* 0xD110 */
         N_("FNumber Lock")},
        {PTP_DPC_NIKON_ExposureApertureLock,        /* 0xD111 */
         N_("Exposure Aperture Lock")},
        {PTP_DPC_NIKON_TVLockSetting,           /* 0xD112 */
         N_("TV Lock Setting")},
        {PTP_DPC_NIKON_AVLockSetting,           /* 0xD113 */
         N_("AV Lock Setting")},
        {PTP_DPC_NIKON_IllumSetting,            /* 0xD114 */
         N_("Illum Setting")},
        {PTP_DPC_NIKON_FocusPointBright,        /* 0xD115 */
         N_("Focus Point Bright")},
        {PTP_DPC_NIKON_ExternalFlashAttached,       /* 0xD120 */
         N_("External Flash Attached")},
        {PTP_DPC_NIKON_ExternalFlashStatus,     /* 0xD121 */
         N_("External Flash Status")},
        {PTP_DPC_NIKON_ExternalFlashSort,       /* 0xD122 */
         N_("External Flash Sort")},
        {PTP_DPC_NIKON_ExternalFlashMode,       /* 0xD123 */
         N_("External Flash Mode")},
        {PTP_DPC_NIKON_ExternalFlashCompensation,   /* 0xD124 */
         N_("External Flash Compensation")},
        {PTP_DPC_NIKON_NewExternalFlashMode,        /* 0xD125 */
         N_("External Flash Mode")},
        {PTP_DPC_NIKON_FlashExposureCompensation,   /* 0xD126 */
         N_("Flash Exposure Compensation")},
        {PTP_DPC_NIKON_HDRMode,             /* 0xD130 */
         N_("HDR Mode")},
        {PTP_DPC_NIKON_HDRHighDynamic,          /* 0xD131 */
         N_("HDR High Dynamic")},
        {PTP_DPC_NIKON_HDRSmoothing,            /* 0xD132 */
         N_("HDR Smoothing")},
        {PTP_DPC_NIKON_OptimizeImage,           /* 0xD140 */
         N_("Optimize Image")},
        {PTP_DPC_NIKON_Saturation,          /* 0xD142 */
         N_("Saturation")},
        {PTP_DPC_NIKON_BW_FillerEffect,         /* 0xD143 */
         N_("BW Filler Effect")},
        {PTP_DPC_NIKON_BW_Sharpness,            /* 0xD144 */
         N_("BW Sharpness")},
        {PTP_DPC_NIKON_BW_Contrast,         /* 0xD145 */
         N_("BW Contrast")},
        {PTP_DPC_NIKON_BW_Setting_Type,         /* 0xD146 */
         N_("BW Setting Type")},
        {PTP_DPC_NIKON_Slot2SaveMode,           /* 0xD148 */
         N_("Slot 2 Save Mode")},
        {PTP_DPC_NIKON_RawBitMode,          /* 0xD149 */
         N_("Raw Bit Mode")},
        {PTP_DPC_NIKON_ActiveDLighting,         /* 0xD14E */
         N_("Active D-Lighting")},
        {PTP_DPC_NIKON_FlourescentType,         /* 0xD14F */
         N_("Flourescent Type")},
        {PTP_DPC_NIKON_TuneColourTemperature,       /* 0xD150 */
         N_("Tune Colour Temperature")},
        {PTP_DPC_NIKON_TunePreset0,         /* 0xD151 */
         N_("Tune Preset 0")},
        {PTP_DPC_NIKON_TunePreset1,         /* 0xD152 */
         N_("Tune Preset 1")},
        {PTP_DPC_NIKON_TunePreset2,         /* 0xD153 */
         N_("Tune Preset 2")},
        {PTP_DPC_NIKON_TunePreset3,         /* 0xD154 */
         N_("Tune Preset 3")},
        {PTP_DPC_NIKON_TunePreset4,         /* 0xD155 */
         N_("Tune Preset 4")},
        {PTP_DPC_NIKON_BeepOff,             /* 0xD160 */
         N_("AF Beep Mode")},
        {PTP_DPC_NIKON_AutofocusMode,           /* 0xD161 */
         N_("Autofocus Mode")},
        {PTP_DPC_NIKON_AFAssist,            /* 0xD163 */
         N_("AF Assist Lamp")},
        {PTP_DPC_NIKON_PADVPMode,           /* 0xD164 */
         N_("Auto ISO P/A/DVP Setting")},
        {PTP_DPC_NIKON_ImageReview,         /* 0xD165 */
         N_("Image Review")},
        {PTP_DPC_NIKON_AFAreaIllumination,      /* 0xD166 */
         N_("AF Area Illumination")},
        {PTP_DPC_NIKON_FlashMode,           /* 0xD167 */
         N_("Flash Mode")},
        {PTP_DPC_NIKON_FlashCommanderMode,      /* 0xD168 */
         N_("Flash Commander Mode")},
        {PTP_DPC_NIKON_FlashSign,           /* 0xD169 */
         N_("Flash Sign")},
        {PTP_DPC_NIKON_ISO_Auto,            /* 0xD16A */
         N_("ISO Auto")},
        {PTP_DPC_NIKON_RemoteTimeout,           /* 0xD16B */
         N_("Remote Timeout")},
        {PTP_DPC_NIKON_GridDisplay,         /* 0xD16C */
         N_("Viewfinder Grid Display")},
        {PTP_DPC_NIKON_FlashModeManualPower,        /* 0xD16D */
         N_("Flash Mode Manual Power")},
        {PTP_DPC_NIKON_FlashModeCommanderPower,     /* 0xD16E */
         N_("Flash Mode Commander Power")},
        {PTP_DPC_NIKON_AutoFP,              /* 0xD16F */
         N_("Auto FP")},
        {PTP_DPC_NIKON_CSMMenu,             /* 0xD180 */
         N_("CSM Menu")},
        {PTP_DPC_NIKON_WarningDisplay,          /* 0xD181 */
         N_("Warning Display")},
        {PTP_DPC_NIKON_BatteryCellKind,         /* 0xD182 */
         N_("Battery Cell Kind")},
        {PTP_DPC_NIKON_ISOAutoHiLimit,          /* 0xD183 */
         N_("ISO Auto High Limit")},
        {PTP_DPC_NIKON_DynamicAFArea,           /* 0xD184 */
         N_("Dynamic AF Area")},
        {PTP_DPC_NIKON_ContinuousSpeedHigh,     /* 0xD186 */
         N_("Continuous Speed High")},
        {PTP_DPC_NIKON_InfoDispSetting,         /* 0xD187 */
         N_("Info Disp Setting")},
        {PTP_DPC_NIKON_PreviewButton,           /* 0xD189 */
         N_("Preview Button")},
        {PTP_DPC_NIKON_PreviewButton2,          /* 0xD18A */
         N_("Preview Button 2")},
        {PTP_DPC_NIKON_AEAFLockButton2,         /* 0xD18B */
         N_("AEAF Lock Button 2")},
        {PTP_DPC_NIKON_IndicatorDisp,           /* 0xD18D */
         N_("Indicator Display")},
        {PTP_DPC_NIKON_CellKindPriority,        /* 0xD18E */
         N_("Cell Kind Priority")},
        {PTP_DPC_NIKON_BracketingFramesAndSteps,    /* 0xD190 */
         N_("Bracketing Frames and Steps")},
        {PTP_DPC_NIKON_LiveViewMode,            /* 0xD1A0 */
         N_("Live View Mode")},
        {PTP_DPC_NIKON_LiveViewDriveMode,       /* 0xD1A1 */
         N_("Live View Drive Mode")},
        {PTP_DPC_NIKON_LiveViewStatus,          /* 0xD1A2 */
         N_("Live View Status")},
        {PTP_DPC_NIKON_LiveViewImageZoomRatio,      /* 0xD1A3 */
         N_("Live View Image Zoom Ratio")},
        {PTP_DPC_NIKON_LiveViewProhibitCondition,   /* 0xD1A4 */
         N_("Live View Prohibit Condition")},
        {PTP_DPC_NIKON_ExposureDisplayStatus,       /* 0xD1B0 */
         N_("Exposure Display Status")},
        {PTP_DPC_NIKON_ExposureIndicateStatus,      /* 0xD1B1 */
         N_("Exposure Indicate Status")},
        {PTP_DPC_NIKON_InfoDispErrStatus,       /* 0xD1B2 */
         N_("Info Display Error Status")},
        {PTP_DPC_NIKON_ExposureIndicateLightup,     /* 0xD1B3 */
         N_("Exposure Indicate Lightup")},
        {PTP_DPC_NIKON_FlashOpen,           /* 0xD1C0 */
         N_("Flash Open")},
        {PTP_DPC_NIKON_FlashCharged,            /* 0xD1C1 */
         N_("Flash Charged")},
        {PTP_DPC_NIKON_FlashMRepeatValue,       /* 0xD1D0 */
         N_("Flash MRepeat Value")},
        {PTP_DPC_NIKON_FlashMRepeatCount,       /* 0xD1D1 */
         N_("Flash MRepeat Count")},
        {PTP_DPC_NIKON_FlashMRepeatInterval,        /* 0xD1D2 */
         N_("Flash MRepeat Interval")},
        {PTP_DPC_NIKON_FlashCommandChannel,     /* 0xD1D3 */
         N_("Flash Command Channel")},
        {PTP_DPC_NIKON_FlashCommandSelfMode,        /* 0xD1D4 */
         N_("Flash Command Self Mode")},
        {PTP_DPC_NIKON_FlashCommandSelfCompensation,    /* 0xD1D5 */
         N_("Flash Command Self Compensation")},
        {PTP_DPC_NIKON_FlashCommandSelfValue,       /* 0xD1D6 */
         N_("Flash Command Self Value")},
        {PTP_DPC_NIKON_FlashCommandAMode,       /* 0xD1D7 */
         N_("Flash Command A Mode")},
        {PTP_DPC_NIKON_FlashCommandACompensation,   /* 0xD1D8 */
         N_("Flash Command A Compensation")},
        {PTP_DPC_NIKON_FlashCommandAValue,      /* 0xD1D9 */
         N_("Flash Command A Value")},
        {PTP_DPC_NIKON_FlashCommandBMode,       /* 0xD1DA */
         N_("Flash Command B Mode")},
        {PTP_DPC_NIKON_FlashCommandBCompensation,   /* 0xD1DB */
         N_("Flash Command B Compensation")},
        {PTP_DPC_NIKON_FlashCommandBValue,      /* 0xD1DC */
         N_("Flash Command B Value")},
        {PTP_DPC_NIKON_ActivePicCtrlItem,       /* 0xD200 */
         N_("Active Pic Ctrl Item")},
        {PTP_DPC_NIKON_ChangePicCtrlItem,       /* 0xD201 */
         N_("Change Pic Ctrl Item")},
        /* nikon 1 stuff */
        {PTP_DPC_NIKON_1_ISO,               /* 0xf002 */
         N_("ISO")},
        {PTP_DPC_NIKON_1_ImageSize,         /* 0xf00a */
         N_("Image Size")},
        {PTP_DPC_NIKON_1_LongExposureNoiseReduction,    /* 0xF00D */
         N_("Long Exposure Noise Reduction")},
        {PTP_DPC_NIKON_1_MovQuality,                    /* 0xF01C */
         N_("Movie Quality")},
        {PTP_DPC_NIKON_1_HiISONoiseReduction,           /* 0xF00E */
         N_("High ISO Noise Reduction")},
        {PTP_DPC_NIKON_1_WhiteBalance,              /* 0xF00C */
         N_("White Balance")},
        {PTP_DPC_NIKON_1_ImageCompression,              /* 0xF009 */
         N_("Image Compression")},
        {PTP_DPC_NIKON_1_ActiveDLighting,               /* 0xF00F */
         N_("Active D-Lighting")},
        {0,NULL}
    };
    struct {
        u16 dpc;
        const char *txt;
    } ptp_device_properties_MTP[] = {
        {PTP_DPC_MTP_SecureTime,        N_("Secure Time")},     /* D101 */
        {PTP_DPC_MTP_DeviceCertificate, N_("Device Certificate")},  /* D102 */
        {PTP_DPC_MTP_RevocationInfo,    N_("Revocation Info")},     /* D103 */
        {PTP_DPC_MTP_SynchronizationPartner,                /* D401 */
         N_("Synchronization Partner")},
        {PTP_DPC_MTP_DeviceFriendlyName,                /* D402 */
         N_("Friendly Device Name")},
        {PTP_DPC_MTP_VolumeLevel,       N_("Volume Level")},        /* D403 */
        {PTP_DPC_MTP_DeviceIcon,        N_("Device Icon")},     /* D405 */
        {PTP_DPC_MTP_SessionInitiatorInfo,  N_("Session Initiator Info")},/* D406 */
        {PTP_DPC_MTP_PerceivedDeviceType,   N_("Perceived Device Type")},/* D407 */
        {PTP_DPC_MTP_PlaybackRate,      N_("Playback Rate")},       /* D410 */
        {PTP_DPC_MTP_PlaybackObject,    N_("Playback Object")},     /* D411 */
        {PTP_DPC_MTP_PlaybackContainerIndex,                /* D412 */
         N_("Playback Container Index")},
        {PTP_DPC_MTP_PlaybackPosition,  N_("Playback Position")},   /* D413 */
        {PTP_DPC_MTP_PlaysForSureID,    N_("PlaysForSure ID")},     /* D131 (?) */
        {0,NULL}
    };
    struct {
        u16 dpc;
        const char *txt;
    } ptp_device_properties_FUJI[] = {
        {PTP_DPC_FUJI_ColorTemperature, N_("Color Temperature")},   /* 0xD017 */
        {PTP_DPC_FUJI_Quality, N_("Quality")},              /* 0xD018 */
        {PTP_DPC_FUJI_Quality, N_("Release Mode")},         /* 0xD201 */
        {PTP_DPC_FUJI_Quality, N_("Focus Areas")},          /* 0xD206 */
        {PTP_DPC_FUJI_Quality, N_("AE Lock")},              /* 0xD213 */
        {PTP_DPC_FUJI_Quality, N_("Aperture")},             /* 0xD218 */
        {PTP_DPC_FUJI_Quality, N_("Shutter Speed")},            /* 0xD219 */
        {0,NULL}
    };

    struct {
        u16 dpc;
        const char *txt;
    } ptp_device_properties_SONY[] = {
        {PTP_DPC_SONY_DPCCompensation, ("DOC Compensation")},   /* 0xD200 */
        {PTP_DPC_SONY_DRangeOptimize, ("DRangeOptimize")},  /* 0xD201 */
        {PTP_DPC_SONY_ImageSize, N_("Image size")},     /* 0xD203 */
        {PTP_DPC_SONY_ShutterSpeed, N_("Shutter speed")},   /* 0xD20D */
        {PTP_DPC_SONY_ColorTemp, N_("Color temperature")},  /* 0xD20F */
        {PTP_DPC_SONY_CCFilter, ("CC Filter")},         /* 0xD210 */
        {PTP_DPC_SONY_AspectRatio, N_("Aspect Ratio")},     /* 0xD211 */
        {PTP_DPC_SONY_FocusFound, N_("Focus status")},      /* 0xD213 */
        {PTP_DPC_SONY_ObjectInMemory, N_("Objects in memory")}, /* 0xD215 */
        {PTP_DPC_SONY_ExposeIndex, N_("Expose Index")},     /* 0xD216 */
        {PTP_DPC_SONY_BatteryLevel, N_("Battery Level")},   /* 0xD218 */
        {PTP_DPC_SONY_PictureEffect, N_("Picture Effect")}, /* 0xD21B */
        {PTP_DPC_SONY_ABFilter, N_("AB Filter")},       /* 0xD21C */
        {PTP_DPC_SONY_ISO, N_("ISO")},              /* 0xD21E */
        {PTP_DPC_SONY_Movie, N_("Movie")},          /* 0xD2C8 */
        {PTP_DPC_SONY_StillImage, N_("Still Image")},       /* 0xD2C7 */
        {0,NULL}
    };

    struct {
        u16 dpc;
        const char *txt;
    } ptp_device_properties_PARROT[] = {
        {PTP_DPC_PARROT_PhotoSensorEnableMask,      "PhotoSensorEnableMask"}, /* 0xD201 */
        {PTP_DPC_PARROT_PhotoSensorsKeepOn,     "PhotoSensorsKeepOn"}, /* 0xD202 */
        {PTP_DPC_PARROT_MultispectralImageSize,     "MultispectralImageSize"}, /* 0xD203 */
        {PTP_DPC_PARROT_MainBitDepth,           "MainBitDepth"}, /* 0xD204 */
        {PTP_DPC_PARROT_MultispectralBitDepth,      "MultispectralBitDepth"}, /* 0xD205 */
        {PTP_DPC_PARROT_HeatingEnable,          "HeatingEnable"}, /* 0xD206 */
        {PTP_DPC_PARROT_WifiStatus,         "WifiStatus"}, /* 0xD207 */
        {PTP_DPC_PARROT_WifiSSID,           "WifiSSID"}, /* 0xD208 */
        {PTP_DPC_PARROT_WifiEncryptionType,     "WifiEncryptionType"}, /* 0xD209 */
        {PTP_DPC_PARROT_WifiPassphrase,         "WifiPassphrase"}, /* 0xD20A */
        {PTP_DPC_PARROT_WifiChannel,            "WifiChannel"}, /* 0xD20B */
        {PTP_DPC_PARROT_Localization,           "Localization"}, /* 0xD20C */
        {PTP_DPC_PARROT_WifiMode,           "WifiMode"}, /* 0xD20D */
        {PTP_DPC_PARROT_AntiFlickeringFrequency,    "AntiFlickeringFrequency"}, /* 0xD210 */
        {PTP_DPC_PARROT_DisplayOverlayMask,     "DisplayOverlayMask"}, /* 0xD211 */
        {PTP_DPC_PARROT_GPSInterval,            "GPSInterval"}, /* 0xD212 */
        {PTP_DPC_PARROT_MultisensorsExposureMeteringMode,"MultisensorsExposureMeteringMode"}, /* 0xD213 */
        {PTP_DPC_PARROT_MultisensorsExposureTime,   "MultisensorsExposureTime"}, /* 0xD214 */
        {PTP_DPC_PARROT_MultisensorsExposureProgramMode,"MultisensorsExposureProgramMode"}, /* 0xD215 */
        {PTP_DPC_PARROT_MultisensorsExposureIndex,  "MultisensorsExposureIndex"}, /* 0xD216 */
        {PTP_DPC_PARROT_MultisensorsIrradianceGain, "MultisensorsIrradianceGain"}, /* 0xD217 */
        {PTP_DPC_PARROT_MultisensorsIrradianceIntegrationTime,"MultisensorsIrradianceIntegrationTime"}, /* 0xD218 */
        {PTP_DPC_PARROT_OverlapRate,            "OverlapRate"}, /* 0xD219 */
        {0,NULL}
    };


    for (i = 0; ptp_device_properties[i].txt != NULL; i++)
        if (ptp_device_properties[i].dpc == dpc)
            return (ptp_device_properties[i].txt);

    if (params->deviceinfo.VendorExtensionID == PTP_VENDOR_MICROSOFT
        || params->deviceinfo.VendorExtensionID == PTP_VENDOR_MTP
        || params->deviceinfo.VendorExtensionID == PTP_VENDOR_PANASONIC)
        for (i = 0; ptp_device_properties_MTP[i].txt != NULL; i++)
            if (ptp_device_properties_MTP[i].dpc == dpc)
                return (ptp_device_properties_MTP[i].txt);

    if (params->deviceinfo.VendorExtensionID==PTP_VENDOR_EASTMAN_KODAK)
        for (i=0; ptp_device_properties_EK[i].txt!=NULL; i++)
            if (ptp_device_properties_EK[i].dpc==dpc)
                return (ptp_device_properties_EK[i].txt);

    if (params->deviceinfo.VendorExtensionID == PTP_VENDOR_CANON)
        for (i = 0; ptp_device_properties_Canon[i].txt != NULL; i++)
            if (ptp_device_properties_Canon[i].dpc == dpc)
                return (ptp_device_properties_Canon[i].txt);

    if (params->deviceinfo.VendorExtensionID == PTP_VENDOR_NIKON)
        for (i = 0; ptp_device_properties_Nikon[i].txt != NULL; i++)
            if (ptp_device_properties_Nikon[i].dpc == dpc)
                return (ptp_device_properties_Nikon[i].txt);

    if (params->deviceinfo.VendorExtensionID == PTP_VENDOR_FUJI)
        for (i = 0; ptp_device_properties_FUJI[i].txt != NULL; i++)
            if (ptp_device_properties_FUJI[i].dpc == dpc)
                return (ptp_device_properties_FUJI[i].txt);

    if (params->deviceinfo.VendorExtensionID == PTP_VENDOR_SONY)
        for (i = 0; ptp_device_properties_SONY[i].txt != NULL; i++)
            if (ptp_device_properties_SONY[i].dpc == dpc)
                return (ptp_device_properties_SONY[i].txt);
    if (params->deviceinfo.VendorExtensionID == PTP_VENDOR_PARROT)
        for (i = 0; ptp_device_properties_PARROT[i].txt != NULL; i++)
            if (ptp_device_properties_PARROT[i].dpc == dpc)
                return (ptp_device_properties_PARROT[i].txt);


    return NULL;
}

struct {
    uint16_t ofc;
    const char *format;
} ptp_ofc_trans[] = {
    {PTP_OFC_Undefined,"Undefined Type"},
    {PTP_OFC_Defined,"Defined Type"},
    {PTP_OFC_Association,"Association/Directory"},
    {PTP_OFC_Script,"Script"},
    {PTP_OFC_Executable,"Executable"},
    {PTP_OFC_Text,"Text"},
    {PTP_OFC_HTML,"HTML"},
    {PTP_OFC_DPOF,"DPOF"},
    {PTP_OFC_AIFF,"AIFF"},
    {PTP_OFC_WAV,"MS Wave"},
    {PTP_OFC_MP3,"MP3"},
    {PTP_OFC_AVI,"MS AVI"},
    {PTP_OFC_MPEG,"MPEG"},
    {PTP_OFC_ASF,"ASF"},
    {PTP_OFC_QT,"Apple Quicktime"},
    {PTP_OFC_EXIF_JPEG,"JPEG"},
    {PTP_OFC_TIFF_EP,"TIFF EP"},
    {PTP_OFC_FlashPix,"FlashPix"},
    {PTP_OFC_BMP,"BMP"},
    {PTP_OFC_CIFF,"CIFF"},
    {PTP_OFC_GIF,"GIF"},
    {PTP_OFC_JFIF,"JFIF"},
    {PTP_OFC_PCD,"PCD"},
    {PTP_OFC_PICT,"PICT"},
    {PTP_OFC_PNG,"PNG"},
    {PTP_OFC_TIFF,"TIFF"},
    {PTP_OFC_TIFF_IT,"TIFF_IT"},
    {PTP_OFC_JP2,"JP2"},
    {PTP_OFC_JPX,"JPX"},
    {PTP_OFC_DNG,"DNG"},
};

struct {
    u16 ofc;
    const char *format;
} ptp_ofc_mtp_trans[] = {
    {PTP_OFC_MTP_MediaCard,"Media Card"},
    {PTP_OFC_MTP_MediaCardGroup,"Media Card Group"},
    {PTP_OFC_MTP_Encounter,"Encounter"},
    {PTP_OFC_MTP_EncounterBox,"Encounter Box"},
    {PTP_OFC_MTP_M4A,"M4A"},
    {PTP_OFC_MTP_Firmware,"Firmware"},
    {PTP_OFC_MTP_WindowsImageFormat,"Windows Image Format"},
    {PTP_OFC_MTP_UndefinedAudio,"Undefined Audio"},
    {PTP_OFC_MTP_WMA,"WMA"},
    {PTP_OFC_MTP_OGG,"OGG"},
    {PTP_OFC_MTP_AAC,"AAC"},
    {PTP_OFC_MTP_AudibleCodec,"Audible.com Codec"},
    {PTP_OFC_MTP_FLAC,"FLAC"},
    {PTP_OFC_MTP_SamsungPlaylist,"Samsung Playlist"},
    {PTP_OFC_MTP_UndefinedVideo,"Undefined Video"},
    {PTP_OFC_MTP_WMV,"WMV"},
    {PTP_OFC_MTP_MP4,"MP4"},
    {PTP_OFC_MTP_MP2,"MP2"},
    {PTP_OFC_MTP_3GP,"3GP"},
    {PTP_OFC_MTP_UndefinedCollection,"Undefined Collection"},
    {PTP_OFC_MTP_AbstractMultimediaAlbum,"Abstract Multimedia Album"},
    {PTP_OFC_MTP_AbstractImageAlbum,"Abstract Image Album"},
    {PTP_OFC_MTP_AbstractAudioAlbum,"Abstract Audio Album"},
    {PTP_OFC_MTP_AbstractVideoAlbum,"Abstract Video Album"},
    {PTP_OFC_MTP_AbstractAudioVideoPlaylist,"Abstract Audio Video Playlist"},
    {PTP_OFC_MTP_AbstractContactGroup,"Abstract Contact Group"},
    {PTP_OFC_MTP_AbstractMessageFolder,"Abstract Message Folder"},
    {PTP_OFC_MTP_AbstractChapteredProduction,"Abstract Chaptered Production"},
    {PTP_OFC_MTP_AbstractAudioPlaylist,"Abstract Audio Playlist"},
    {PTP_OFC_MTP_AbstractVideoPlaylist,"Abstract Video Playlist"},
    {PTP_OFC_MTP_AbstractMediacast,"Abstract Mediacast"},
    {PTP_OFC_MTP_WPLPlaylist,"WPL Playlist"},
    {PTP_OFC_MTP_M3UPlaylist,"M3U Playlist"},
    {PTP_OFC_MTP_MPLPlaylist,"MPL Playlist"},
    {PTP_OFC_MTP_ASXPlaylist,"ASX Playlist"},
    {PTP_OFC_MTP_PLSPlaylist,"PLS Playlist"},
    {PTP_OFC_MTP_UndefinedDocument,"Undefined Document"},
    {PTP_OFC_MTP_AbstractDocument,"Abstract Document"},
    {PTP_OFC_MTP_XMLDocument,"XMLDocument"},
    {PTP_OFC_MTP_MSWordDocument,"Microsoft Word Document"},
    {PTP_OFC_MTP_MHTCompiledHTMLDocument,"MHT Compiled HTML Document"},
    {PTP_OFC_MTP_MSExcelSpreadsheetXLS,"Microsoft Excel Spreadsheet (.xls)"},
    {PTP_OFC_MTP_MSPowerpointPresentationPPT,"Microsoft Powerpoint (.ppt)"},
    {PTP_OFC_MTP_UndefinedMessage,"Undefined Message"},
    {PTP_OFC_MTP_AbstractMessage,"Abstract Message"},
    {PTP_OFC_MTP_UndefinedContact,"Undefined Contact"},
    {PTP_OFC_MTP_AbstractContact,"Abstract Contact"},
    {PTP_OFC_MTP_vCard2,"vCard2"},
    {PTP_OFC_MTP_vCard3,"vCard3"},
    {PTP_OFC_MTP_UndefinedCalendarItem,"Undefined Calendar Item"},
    {PTP_OFC_MTP_AbstractCalendarItem,"Abstract Calendar Item"},
    {PTP_OFC_MTP_vCalendar1,"vCalendar1"},
    {PTP_OFC_MTP_vCalendar2,"vCalendar2"},
    {PTP_OFC_MTP_UndefinedWindowsExecutable,"Undefined Windows Executable"},
    {PTP_OFC_MTP_MediaCast,"Media Cast"},
    {PTP_OFC_MTP_Section,"Section"},
};

/**
 * PTP提供对象格式代码
 */
int ptp_render_ofc(PTPParams* params, u16 ofc, int spaceleft, char *txt)
{
    unsigned int i;

    if (!(ofc & 0x8000)) {
        for (i = 0;i < sizeof(ptp_ofc_trans)/sizeof(ptp_ofc_trans[0]);i++)
            if (ofc == ptp_ofc_trans[i].ofc)
                return snprintf(txt, spaceleft, "%s", _(ptp_ofc_trans[i].format));
    } else {
        switch (params->deviceinfo.VendorExtensionID) {
        case PTP_VENDOR_EASTMAN_KODAK:
            switch (ofc) {
            case PTP_OFC_EK_M3U:
                return snprintf (txt, spaceleft,"M3U");
            default:
                break;
            }
            break;
        case PTP_VENDOR_CANON:
            switch (ofc) {
            case PTP_OFC_CANON_CRW:
                return snprintf (txt, spaceleft,"CRW");
            default:
                break;
            }
            break;
        case PTP_VENDOR_SONY:
            switch (ofc) {
            case PTP_OFC_SONY_RAW:
                return snprintf (txt, spaceleft,"ARW");
            default:
                break;
            }
            break;
        case PTP_VENDOR_MICROSOFT:
        case PTP_VENDOR_MTP:
            for (i=0;i<sizeof(ptp_ofc_mtp_trans)/sizeof(ptp_ofc_mtp_trans[0]);i++)
                if (ofc == ptp_ofc_mtp_trans[i].ofc)
                    return snprintf(txt, spaceleft, "%s", _(ptp_ofc_mtp_trans[i].format));
            break;
        default:break;
        }
    }
    return snprintf (txt, spaceleft,_("Unknown(%04x)"), ofc);
}

typedef struct {
    u16 opcode;
    const char *name;
} ptp_opcode_trans_t;

ptp_opcode_trans_t ptp_opcode_trans[] = {
    {PTP_OC_Undefined,"Undefined"},
    {PTP_OC_GetDeviceInfo,"Get device info"},
    {PTP_OC_OpenSession,"Open session"},
    {PTP_OC_CloseSession,"Close session"},
    {PTP_OC_GetStorageIDs,"Get storage IDs"},
    {PTP_OC_GetStorageInfo,"Get storage info"},
    {PTP_OC_GetNumObjects,"Get number of objects"},
    {PTP_OC_GetObjectHandles,"Get object handles"},
    {PTP_OC_GetObjectInfo,"Get object info"},
    {PTP_OC_GetObject,"Get object"},
    {PTP_OC_GetThumb,"Get thumbnail"},
    {PTP_OC_DeleteObject,"Delete object"},
    {PTP_OC_SendObjectInfo,"Send object info"},
    {PTP_OC_SendObject,"Send object"},
    {PTP_OC_InitiateCapture,"Initiate capture"},
    {PTP_OC_FormatStore,"Format storage"},
    {PTP_OC_ResetDevice,"Reset device"},
    {PTP_OC_SelfTest,"Self test device"},
    {PTP_OC_SetObjectProtection,"Set object protection"},
    {PTP_OC_PowerDown,"Power down device"},
    {PTP_OC_GetDevicePropDesc,"Get device property description"},
    {PTP_OC_GetDevicePropValue,"Get device property value"},
    {PTP_OC_SetDevicePropValue,"Set device property value"},
    {PTP_OC_ResetDevicePropValue,"Reset device property value"},
    {PTP_OC_TerminateOpenCapture,"Terminate open capture"},
    {PTP_OC_MoveObject,"Move object"},
    {PTP_OC_CopyObject,"Copy object"},
    {PTP_OC_GetPartialObject,"Get partial object"},
    {PTP_OC_InitiateOpenCapture,"Initiate open capture"},
    /* PTP v1.1 操作代码 */
    {PTP_OC_StartEnumHandles,"Start Enumerate Handles"},
    {PTP_OC_EnumHandles,"Enumerate Handles"},
    {PTP_OC_StopEnumHandles,"Stop Enumerate Handles"},
    {PTP_OC_GetVendorExtensionMaps,"Get Vendor Extension Maps"},
    {PTP_OC_GetVendorDeviceInfo,"Get Vendor Device Info"},
    {PTP_OC_GetResizedImageObject,"Get Resized Image Object"},
    {PTP_OC_GetFilesystemManifest,"Get Filesystem Manifest"},
    {PTP_OC_GetStreamInfo,"Get Stream Info"},
    {PTP_OC_GetStream,"Get Stream"},
};

ptp_opcode_trans_t ptp_opcode_mtp_trans[] = {
    {PTP_OC_MTP_GetObjectPropsSupported,"Get object properties supported"},
    {PTP_OC_MTP_GetObjectPropDesc,"Get object property description"},
    {PTP_OC_MTP_GetObjectPropValue,"Get object property value"},
    {PTP_OC_MTP_SetObjectPropValue,"Set object property value"},
    {PTP_OC_MTP_GetObjPropList,"Get object property list"},
    {PTP_OC_MTP_SetObjPropList,"Set object property list"},
    {PTP_OC_MTP_GetInterdependendPropdesc,"Get interdependent property description"},
    {PTP_OC_MTP_SendObjectPropList,"Send object property list"},
    {PTP_OC_MTP_GetObjectReferences,"Get object references"},
    {PTP_OC_MTP_SetObjectReferences,"Set object references"},
    {PTP_OC_MTP_UpdateDeviceFirmware,"Update device firmware"},
    {PTP_OC_MTP_Skip,"Skip to next position in playlist"},

    /* WMDRMPD Extensions */
    {PTP_OC_MTP_WMDRMPD_GetSecureTimeChallenge,"Get secure time challenge"},
    {PTP_OC_MTP_WMDRMPD_GetSecureTimeResponse,"Get secure time response"},
    {PTP_OC_MTP_WMDRMPD_SetLicenseResponse,"Set license response"},
    {PTP_OC_MTP_WMDRMPD_GetSyncList,"Get sync list"},
    {PTP_OC_MTP_WMDRMPD_SendMeterChallengeQuery,"Send meter challenge query"},
    {PTP_OC_MTP_WMDRMPD_GetMeterChallenge,"Get meter challenge"},
    {PTP_OC_MTP_WMDRMPD_SetMeterResponse,"Get meter response"},
    {PTP_OC_MTP_WMDRMPD_CleanDataStore,"Clean data store"},
    {PTP_OC_MTP_WMDRMPD_GetLicenseState,"Get license state"},
    {PTP_OC_MTP_WMDRMPD_SendWMDRMPDCommand,"Send WMDRM-PD Command"},
    {PTP_OC_MTP_WMDRMPD_SendWMDRMPDRequest,"Send WMDRM-PD Request"},

    /* WMPPD Extensions */
    {PTP_OC_MTP_WMPPD_ReportAddedDeletedItems,"Report Added/Deleted Items"},
    {PTP_OC_MTP_WMPPD_ReportAcquiredItems,"Report Acquired Items"},
    {PTP_OC_MTP_WMPPD_PlaylistObjectPref,"Get transferable playlist types"},

    /* WMDRMPD Extensions... these have no identifiers associated with them */
    {PTP_OC_MTP_WMDRMPD_SendWMDRMPDAppRequest,"Send WMDRM-PD Application Request"},
    {PTP_OC_MTP_WMDRMPD_GetWMDRMPDAppResponse,"Get WMDRM-PD Application Response"},
    {PTP_OC_MTP_WMDRMPD_EnableTrustedFilesOperations,"Enable trusted file operations"},
    {PTP_OC_MTP_WMDRMPD_DisableTrustedFilesOperations,"Disable trusted file operations"},
    {PTP_OC_MTP_WMDRMPD_EndTrustedAppSession,"End trusted application session"},

    /* AAVT Extensions */
    {PTP_OC_MTP_AAVT_OpenMediaSession,"Open Media Session"},
    {PTP_OC_MTP_AAVT_CloseMediaSession,"Close Media Session"},
    {PTP_OC_MTP_AAVT_GetNextDataBlock,"Get Next Data Block"},
    {PTP_OC_MTP_AAVT_SetCurrentTimePosition,"Set Current Time Position"},

    /* WMDRMND Extensions */
    {PTP_OC_MTP_WMDRMND_SendRegistrationRequest,"Send Registration Request"},
    {PTP_OC_MTP_WMDRMND_GetRegistrationResponse,"Get Registration Response"},
    {PTP_OC_MTP_WMDRMND_GetProximityChallenge,"Get Proximity Challenge"},
    {PTP_OC_MTP_WMDRMND_SendProximityResponse,"Send Proximity Response"},
    {PTP_OC_MTP_WMDRMND_SendWMDRMNDLicenseRequest,"Send WMDRM-ND License Request"},
    {PTP_OC_MTP_WMDRMND_GetWMDRMNDLicenseResponse,"Get WMDRM-ND License Response"},

    /* WiFi Provisioning MTP Extension Codes (microsoft.com/WPDWCN: 1.0) */
    {PTP_OC_MTP_WPDWCN_ProcessWFCObject,"Process WFC Object"},

    /* Android Direct I/O Extensions */
    {PTP_OC_ANDROID_GetPartialObject64,"Get Partial Object (64bit Offset)"},
    {PTP_OC_ANDROID_SendPartialObject,"Send Partial Object"},
    {PTP_OC_ANDROID_TruncateObject,"Truncate Object"},
    {PTP_OC_ANDROID_BeginEditObject,"Begin Edit Object"},
    {PTP_OC_ANDROID_EndEditObject,"End Edit Object"},
};

ptp_opcode_trans_t ptp_opcode_nikon_trans[] = {
    {PTP_OC_NIKON_GetProfileAllData,"PTP_OC_NIKON_GetProfileAllData"},
    {PTP_OC_NIKON_SendProfileData,"PTP_OC_NIKON_SendProfileData"},
    {PTP_OC_NIKON_DeleteProfile,"PTP_OC_NIKON_DeleteProfile"},
    {PTP_OC_NIKON_SetProfileData,"PTP_OC_NIKON_SetProfileData"},
    {PTP_OC_NIKON_AdvancedTransfer,"PTP_OC_NIKON_AdvancedTransfer"},
    {PTP_OC_NIKON_GetFileInfoInBlock,"PTP_OC_NIKON_GetFileInfoInBlock"},
    {PTP_OC_NIKON_Capture,"PTP_OC_NIKON_Capture"},
    {PTP_OC_NIKON_AfDrive,"PTP_OC_NIKON_AfDrive"},
    {PTP_OC_NIKON_SetControlMode,"PTP_OC_NIKON_SetControlMode"},
    {PTP_OC_NIKON_DelImageSDRAM,"PTP_OC_NIKON_DelImageSDRAM"},
    {PTP_OC_NIKON_GetLargeThumb,"PTP_OC_NIKON_GetLargeThumb"},
    {PTP_OC_NIKON_CurveDownload,"PTP_OC_NIKON_CurveDownload"},
    {PTP_OC_NIKON_CurveUpload,"PTP_OC_NIKON_CurveUpload"},
    {PTP_OC_NIKON_CheckEvent,"PTP_OC_NIKON_CheckEvent"},
    {PTP_OC_NIKON_DeviceReady,"PTP_OC_NIKON_DeviceReady"},
    {PTP_OC_NIKON_SetPreWBData,"PTP_OC_NIKON_SetPreWBData"},
    {PTP_OC_NIKON_GetVendorPropCodes,"PTP_OC_NIKON_GetVendorPropCodes"},
    {PTP_OC_NIKON_AfCaptureSDRAM,"PTP_OC_NIKON_AfCaptureSDRAM"},
    {PTP_OC_NIKON_GetPictCtrlData,"PTP_OC_NIKON_GetPictCtrlData"},
    {PTP_OC_NIKON_SetPictCtrlData,"PTP_OC_NIKON_SetPictCtrlData"},
    {PTP_OC_NIKON_DelCstPicCtrl,"PTP_OC_NIKON_DelCstPicCtrl"},
    {PTP_OC_NIKON_GetPicCtrlCapability,"PTP_OC_NIKON_GetPicCtrlCapability"},
    {PTP_OC_NIKON_GetPreviewImg,"PTP_OC_NIKON_GetPreviewImg"},
    {PTP_OC_NIKON_StartLiveView,"PTP_OC_NIKON_StartLiveView"},
    {PTP_OC_NIKON_EndLiveView,"PTP_OC_NIKON_EndLiveView"},
    {PTP_OC_NIKON_GetLiveViewImg,"PTP_OC_NIKON_GetLiveViewImg"},
    {PTP_OC_NIKON_MfDrive,"PTP_OC_NIKON_MfDrive"},
    {PTP_OC_NIKON_ChangeAfArea,"PTP_OC_NIKON_ChangeAfArea"},
    {PTP_OC_NIKON_AfDriveCancel,"PTP_OC_NIKON_AfDriveCancel"},
    {PTP_OC_NIKON_InitiateCaptureRecInMedia,"PTP_OC_NIKON_InitiateCaptureRecInMedia"},
    {PTP_OC_NIKON_GetVendorStorageIDs,"PTP_OC_NIKON_GetVendorStorageIDs"},
    {PTP_OC_NIKON_StartMovieRecInCard,"PTP_OC_NIKON_StartMovieRecInCard"},
    {PTP_OC_NIKON_EndMovieRec,"PTP_OC_NIKON_EndMovieRec"},
    {PTP_OC_NIKON_TerminateCapture,"PTP_OC_NIKON_TerminateCapture"},
    {PTP_OC_NIKON_GetDevicePTPIPInfo,"PTP_OC_NIKON_GetDevicePTPIPInfo"},
    {PTP_OC_NIKON_GetPartialObjectHiSpeed,"PTP_OC_NIKON_GetPartialObjectHiSpeed"},
    {PTP_OC_NIKON_GetDevicePropEx,"PTP_OC_NIKON_GetDevicePropEx"},
};

ptp_opcode_trans_t ptp_opcode_canon_trans[] = {
    {PTP_OC_CANON_GetPartialObjectInfo,"PTP_OC_CANON_GetPartialObjectInfo"},
    {PTP_OC_CANON_SetObjectArchive,"PTP_OC_CANON_SetObjectArchive"},
    {PTP_OC_CANON_KeepDeviceOn,"PTP_OC_CANON_KeepDeviceOn"},
    {PTP_OC_CANON_LockDeviceUI,"PTP_OC_CANON_LockDeviceUI"},
    {PTP_OC_CANON_UnlockDeviceUI,"PTP_OC_CANON_UnlockDeviceUI"},
    {PTP_OC_CANON_GetObjectHandleByName,"PTP_OC_CANON_GetObjectHandleByName"},
    {PTP_OC_CANON_InitiateReleaseControl,"PTP_OC_CANON_InitiateReleaseControl"},
    {PTP_OC_CANON_TerminateReleaseControl,"PTP_OC_CANON_TerminateReleaseControl"},
    {PTP_OC_CANON_TerminatePlaybackMode,"PTP_OC_CANON_TerminatePlaybackMode"},
    {PTP_OC_CANON_ViewfinderOn,"PTP_OC_CANON_ViewfinderOn"},
    {PTP_OC_CANON_ViewfinderOff,"PTP_OC_CANON_ViewfinderOff"},
    {PTP_OC_CANON_DoAeAfAwb,"PTP_OC_CANON_DoAeAfAwb"},
    {PTP_OC_CANON_GetCustomizeSpec,"PTP_OC_CANON_GetCustomizeSpec"},
    {PTP_OC_CANON_GetCustomizeItemInfo,"PTP_OC_CANON_GetCustomizeItemInfo"},
    {PTP_OC_CANON_GetCustomizeData,"PTP_OC_CANON_GetCustomizeData"},
    {PTP_OC_CANON_SetCustomizeData,"PTP_OC_CANON_SetCustomizeData"},
    {PTP_OC_CANON_GetCaptureStatus,"PTP_OC_CANON_GetCaptureStatus"},
    {PTP_OC_CANON_CheckEvent,"PTP_OC_CANON_CheckEvent"},
    {PTP_OC_CANON_FocusLock,"PTP_OC_CANON_FocusLock"},
    {PTP_OC_CANON_FocusUnlock,"PTP_OC_CANON_FocusUnlock"},
    {PTP_OC_CANON_GetLocalReleaseParam,"PTP_OC_CANON_GetLocalReleaseParam"},
    {PTP_OC_CANON_SetLocalReleaseParam,"PTP_OC_CANON_SetLocalReleaseParam"},
    {PTP_OC_CANON_AskAboutPcEvf,"PTP_OC_CANON_AskAboutPcEvf"},
    {PTP_OC_CANON_SendPartialObject,"PTP_OC_CANON_SendPartialObject"},
    {PTP_OC_CANON_InitiateCaptureInMemory,"PTP_OC_CANON_InitiateCaptureInMemory"},
    {PTP_OC_CANON_GetPartialObjectEx,"PTP_OC_CANON_GetPartialObjectEx"},
    {PTP_OC_CANON_SetObjectTime,"PTP_OC_CANON_SetObjectTime"},
    {PTP_OC_CANON_GetViewfinderImage,"PTP_OC_CANON_GetViewfinderImage"},
    {PTP_OC_CANON_GetObjectAttributes,"PTP_OC_CANON_GetObjectAttributes"},
    {PTP_OC_CANON_ChangeUSBProtocol,"PTP_OC_CANON_ChangeUSBProtocol"},
    {PTP_OC_CANON_GetChanges,"PTP_OC_CANON_GetChanges"},
    {PTP_OC_CANON_GetObjectInfoEx,"PTP_OC_CANON_GetObjectInfoEx"},
    {PTP_OC_CANON_InitiateDirectTransfer,"PTP_OC_CANON_InitiateDirectTransfer"},
    {PTP_OC_CANON_TerminateDirectTransfer ,"PTP_OC_CANON_TerminateDirectTransfer "},
    {PTP_OC_CANON_SendObjectInfoByPath ,"PTP_OC_CANON_SendObjectInfoByPath "},
    {PTP_OC_CANON_SendObjectByPath ,"PTP_OC_CANON_SendObjectByPath "},
    {PTP_OC_CANON_InitiateDirectTansferEx,"PTP_OC_CANON_InitiateDirectTansferEx"},
    {PTP_OC_CANON_GetAncillaryObjectHandles,"PTP_OC_CANON_GetAncillaryObjectHandles"},
    {PTP_OC_CANON_GetTreeInfo ,"PTP_OC_CANON_GetTreeInfo "},
    {PTP_OC_CANON_GetTreeSize ,"PTP_OC_CANON_GetTreeSize "},
    {PTP_OC_CANON_NotifyProgress ,"PTP_OC_CANON_NotifyProgress "},
    {PTP_OC_CANON_NotifyCancelAccepted,"PTP_OC_CANON_NotifyCancelAccepted"},
    {PTP_OC_CANON_902C,"PTP_OC_CANON_902C"},
    {PTP_OC_CANON_GetDirectory,"PTP_OC_CANON_GetDirectory"},
    {PTP_OC_CANON_SetPairingInfo,"PTP_OC_CANON_SetPairingInfo"},
    {PTP_OC_CANON_GetPairingInfo,"PTP_OC_CANON_GetPairingInfo"},
    {PTP_OC_CANON_DeletePairingInfo,"PTP_OC_CANON_DeletePairingInfo"},
    {PTP_OC_CANON_GetMACAddress,"PTP_OC_CANON_GetMACAddress"},
    {PTP_OC_CANON_SetDisplayMonitor,"PTP_OC_CANON_SetDisplayMonitor"},
    {PTP_OC_CANON_PairingComplete,"PTP_OC_CANON_PairingComplete"},
    {PTP_OC_CANON_GetWirelessMAXChannel,"PTP_OC_CANON_GetWirelessMAXChannel"},
    {PTP_OC_CANON_GetWebServiceSpec,"PTP_OC_CANON_GetWebServiceSpec"},
    {PTP_OC_CANON_GetWebServiceData,"PTP_OC_CANON_GetWebServiceData"},
    {PTP_OC_CANON_SetWebServiceData,"PTP_OC_CANON_SetWebServiceData"},
    {PTP_OC_CANON_GetRootCertificateSpec,"PTP_OC_CANON_GetRootCertificateSpec"},
    {PTP_OC_CANON_GetRootCertificateData,"PTP_OC_CANON_GetRootCertificateData"},
    {PTP_OC_CANON_SetRootCertificateData,"PTP_OC_CANON_SetRootCertificateData"},
    {PTP_OC_CANON_EOS_GetStorageIDs,"PTP_OC_CANON_EOS_GetStorageIDs"},
    {PTP_OC_CANON_EOS_GetStorageInfo,"PTP_OC_CANON_EOS_GetStorageInfo"},
    {PTP_OC_CANON_EOS_GetObjectInfo,"PTP_OC_CANON_EOS_GetObjectInfo"},
    {PTP_OC_CANON_EOS_GetObject,"PTP_OC_CANON_EOS_GetObject"},
    {PTP_OC_CANON_EOS_DeleteObject,"PTP_OC_CANON_EOS_DeleteObject"},
    {PTP_OC_CANON_EOS_FormatStore,"PTP_OC_CANON_EOS_FormatStore"},
    {PTP_OC_CANON_EOS_GetPartialObject,"PTP_OC_CANON_EOS_GetPartialObject"},
    {PTP_OC_CANON_EOS_GetDeviceInfoEx,"PTP_OC_CANON_EOS_GetDeviceInfoEx"},
    {PTP_OC_CANON_EOS_GetObjectInfoEx,"PTP_OC_CANON_EOS_GetObjectInfoEx"},
    {PTP_OC_CANON_EOS_GetThumbEx,"PTP_OC_CANON_EOS_GetThumbEx"},
    {PTP_OC_CANON_EOS_SendPartialObject,"PTP_OC_CANON_EOS_SendPartialObject"},
    {PTP_OC_CANON_EOS_SetObjectAttributes,"PTP_OC_CANON_EOS_SetObjectAttributes"},
    {PTP_OC_CANON_EOS_GetObjectTime,"PTP_OC_CANON_EOS_GetObjectTime"},
    {PTP_OC_CANON_EOS_SetObjectTime,"PTP_OC_CANON_EOS_SetObjectTime"},
    {PTP_OC_CANON_EOS_RemoteRelease,"PTP_OC_CANON_EOS_RemoteRelease"},
    {PTP_OC_CANON_EOS_SetDevicePropValueEx,"PTP_OC_CANON_EOS_SetDevicePropValueEx"},
    {PTP_OC_CANON_EOS_GetRemoteMode,"PTP_OC_CANON_EOS_GetRemoteMode"},
    {PTP_OC_CANON_EOS_SetRemoteMode,"PTP_OC_CANON_EOS_SetRemoteMode"},
    {PTP_OC_CANON_EOS_SetEventMode,"PTP_OC_CANON_EOS_SetEventMode"},
    {PTP_OC_CANON_EOS_GetEvent,"PTP_OC_CANON_EOS_GetEvent"},
    {PTP_OC_CANON_EOS_TransferComplete,"PTP_OC_CANON_EOS_TransferComplete"},
    {PTP_OC_CANON_EOS_CancelTransfer,"PTP_OC_CANON_EOS_CancelTransfer"},
    {PTP_OC_CANON_EOS_ResetTransfer,"PTP_OC_CANON_EOS_ResetTransfer"},
    {PTP_OC_CANON_EOS_PCHDDCapacity,"PTP_OC_CANON_EOS_PCHDDCapacity"},
    {PTP_OC_CANON_EOS_SetUILock,"PTP_OC_CANON_EOS_SetUILock"},
    {PTP_OC_CANON_EOS_ResetUILock,"PTP_OC_CANON_EOS_ResetUILock"},
    {PTP_OC_CANON_EOS_KeepDeviceOn,"PTP_OC_CANON_EOS_KeepDeviceOn"},
    {PTP_OC_CANON_EOS_SetNullPacketMode,"PTP_OC_CANON_EOS_SetNullPacketMode"},
    {PTP_OC_CANON_EOS_UpdateFirmware,"PTP_OC_CANON_EOS_UpdateFirmware"},
    {PTP_OC_CANON_EOS_TransferCompleteDT,"PTP_OC_CANON_EOS_TransferCompleteDT"},
    {PTP_OC_CANON_EOS_CancelTransferDT,"PTP_OC_CANON_EOS_CancelTransferDT"},
    {PTP_OC_CANON_EOS_SetWftProfile,"PTP_OC_CANON_EOS_SetWftProfile"},
    {PTP_OC_CANON_EOS_GetWftProfile,"PTP_OC_CANON_EOS_GetWftProfile"},
    {PTP_OC_CANON_EOS_SetProfileToWft,"PTP_OC_CANON_EOS_SetProfileToWft"},
    {PTP_OC_CANON_EOS_BulbStart,"PTP_OC_CANON_EOS_BulbStart"},
    {PTP_OC_CANON_EOS_BulbEnd,"PTP_OC_CANON_EOS_BulbEnd"},
    {PTP_OC_CANON_EOS_RequestDevicePropValue,"PTP_OC_CANON_EOS_RequestDevicePropValue"},
    {PTP_OC_CANON_EOS_RemoteReleaseOn,"PTP_OC_CANON_EOS_RemoteReleaseOn"},
    {PTP_OC_CANON_EOS_RemoteReleaseOff,"PTP_OC_CANON_EOS_RemoteReleaseOff"},
    {PTP_OC_CANON_EOS_RegistBackgroundImage,"PTP_OC_CANON_EOS_RegistBackgroundImage"},
    {PTP_OC_CANON_EOS_ChangePhotoStudioMode,"PTP_OC_CANON_EOS_ChangePhotoStudioMode"},
    {PTP_OC_CANON_EOS_GetPartialObjectEx,"PTP_OC_CANON_EOS_GetPartialObjectEx"},
    {PTP_OC_CANON_EOS_ResetMirrorLockupState,"PTP_OC_CANON_EOS_ResetMirrorLockupState"},
    {PTP_OC_CANON_EOS_PopupBuiltinFlash,"PTP_OC_CANON_EOS_PopupBuiltinFlash"},
    {PTP_OC_CANON_EOS_EndGetPartialObjectEx,"PTP_OC_CANON_EOS_EndGetPartialObjectEx"},
    {PTP_OC_CANON_EOS_MovieSelectSWOn,"PTP_OC_CANON_EOS_MovieSelectSWOn"},
    {PTP_OC_CANON_EOS_MovieSelectSWOff,"PTP_OC_CANON_EOS_MovieSelectSWOff"},
    {PTP_OC_CANON_EOS_GetCTGInfo,"PTP_OC_CANON_EOS_GetCTGInfo"},
    {PTP_OC_CANON_EOS_GetLensAdjust,"PTP_OC_CANON_EOS_GetLensAdjust"},
    {PTP_OC_CANON_EOS_SetLensAdjust,"PTP_OC_CANON_EOS_SetLensAdjust"},
    {PTP_OC_CANON_EOS_ReadyToSendMusic,"PTP_OC_CANON_EOS_ReadyToSendMusic"},
    {PTP_OC_CANON_EOS_CreateHandle,"PTP_OC_CANON_EOS_CreateHandle"},
    {PTP_OC_CANON_EOS_SendPartialObjectEx,"PTP_OC_CANON_EOS_SendPartialObjectEx"},
    {PTP_OC_CANON_EOS_EndSendPartialObjectEx,"PTP_OC_CANON_EOS_EndSendPartialObjectEx"},
    {PTP_OC_CANON_EOS_SetCTGInfo,"PTP_OC_CANON_EOS_SetCTGInfo"},
    {PTP_OC_CANON_EOS_SetRequestOLCInfoGroup,"PTP_OC_CANON_EOS_SetRequestOLCInfoGroup"},
    {PTP_OC_CANON_EOS_SetRequestRollingPitchingLevel,"PTP_OC_CANON_EOS_SetRequestRollingPitchingLevel"},
    {PTP_OC_CANON_EOS_GetCameraSupport,"PTP_OC_CANON_EOS_GetCameraSupport"},
    {PTP_OC_CANON_EOS_SetRating,"PTP_OC_CANON_EOS_SetRating"},
    {PTP_OC_CANON_EOS_RequestInnerDevelopStart,"PTP_OC_CANON_EOS_RequestInnerDevelopStart"},
    {PTP_OC_CANON_EOS_RequestInnerDevelopParamChange,"PTP_OC_CANON_EOS_RequestInnerDevelopParamChange"},
    {PTP_OC_CANON_EOS_RequestInnerDevelopEnd,"PTP_OC_CANON_EOS_RequestInnerDevelopEnd"},
    {PTP_OC_CANON_EOS_GpsLoggingDataMode,"PTP_OC_CANON_EOS_GpsLoggingDataMode"},
    {PTP_OC_CANON_EOS_GetGpsLogCurrentHandle,"PTP_OC_CANON_EOS_GetGpsLogCurrentHandle"},
    {PTP_OC_CANON_EOS_InitiateViewfinder,"PTP_OC_CANON_EOS_InitiateViewfinder"},
    {PTP_OC_CANON_EOS_TerminateViewfinder,"PTP_OC_CANON_EOS_TerminateViewfinder"},
    {PTP_OC_CANON_EOS_GetViewFinderData,"PTP_OC_CANON_EOS_GetViewFinderData"},
    {PTP_OC_CANON_EOS_DoAf,"PTP_OC_CANON_EOS_DoAf"},
    {PTP_OC_CANON_EOS_DriveLens,"PTP_OC_CANON_EOS_DriveLens"},
    {PTP_OC_CANON_EOS_DepthOfFieldPreview,"PTP_OC_CANON_EOS_DepthOfFieldPreview"},
    {PTP_OC_CANON_EOS_ClickWB,"PTP_OC_CANON_EOS_ClickWB"},
    {PTP_OC_CANON_EOS_Zoom,"PTP_OC_CANON_EOS_Zoom"},
    {PTP_OC_CANON_EOS_ZoomPosition,"PTP_OC_CANON_EOS_ZoomPosition"},
    {PTP_OC_CANON_EOS_SetLiveAfFrame,"PTP_OC_CANON_EOS_SetLiveAfFrame"},
    {PTP_OC_CANON_EOS_TouchAfPosition,"PTP_OC_CANON_EOS_TouchAfPosition"},
    {PTP_OC_CANON_EOS_SetLvPcFlavoreditMode,"PTP_OC_CANON_EOS_SetLvPcFlavoreditMode"},
    {PTP_OC_CANON_EOS_SetLvPcFlavoreditParam,"PTP_OC_CANON_EOS_SetLvPcFlavoreditParam"},
    {PTP_OC_CANON_EOS_AfCancel,"PTP_OC_CANON_EOS_AfCancel"},
    {PTP_OC_CANON_EOS_SetDefaultCameraSetting,"PTP_OC_CANON_EOS_SetDefaultCameraSetting"},
    {PTP_OC_CANON_EOS_GetAEData,"PTP_OC_CANON_EOS_GetAEData"},
    {PTP_OC_CANON_EOS_NotifyNetworkError,"PTP_OC_CANON_EOS_NotifyNetworkError"},
    {PTP_OC_CANON_EOS_AdapterTransferProgress,"PTP_OC_CANON_EOS_AdapterTransferProgress"},
    {PTP_OC_CANON_EOS_TransferComplete2,"PTP_OC_CANON_EOS_TransferComplete2"},
    {PTP_OC_CANON_EOS_CancelTransfer2,"PTP_OC_CANON_EOS_CancelTransfer2"},
    {PTP_OC_CANON_EOS_FAPIMessageTX,"PTP_OC_CANON_EOS_FAPIMessageTX"},
    {PTP_OC_CANON_EOS_FAPIMessageRX,"PTP_OC_CANON_EOS_FAPIMessageRX"},
    {PTP_OC_CANON_EOS_SetImageRecoveryData,"PTP_OC_CANON_EOS_SetImageRecoveryData"},
    {PTP_OC_CANON_EOS_GetImageRecoveryList,"PTP_OC_CANON_EOS_GetImageRecoveryList"},
    {PTP_OC_CANON_EOS_FormatImageRecoveryData,"PTP_OC_CANON_EOS_FormatImageRecoveryData"},
    {PTP_OC_CANON_EOS_GetPresetLensAdjustParam,"PTP_OC_CANON_EOS_GetPresetLensAdjustParam"},
    {PTP_OC_CANON_EOS_GetRawDispImage,"PTP_OC_CANON_EOS_GetRawDispImage"},
    {PTP_OC_CANON_EOS_SaveImageRecoveryData,"PTP_OC_CANON_EOS_SaveImageRecoveryData"},
    {PTP_OC_CANON_EOS_RequestBLE,"PTP_OC_CANON_EOS_RequestBLE"},
    {PTP_OC_CANON_EOS_DrivePowerZoom,"PTP_OC_CANON_EOS_DrivePowerZoom"},
    {PTP_OC_CANON_EOS_GetIptcData,"PTP_OC_CANON_EOS_GetIptcData"},
    {PTP_OC_CANON_EOS_SetIptcData,"PTP_OC_CANON_EOS_SetIptcData"},
    {PTP_OC_CANON_EOS_GetObjectInfo64,"PTP_OC_CANON_EOS_GetObjectInfo64"},
    {PTP_OC_CANON_EOS_GetObject64,"PTP_OC_CANON_EOS_GetObject64"},
    {PTP_OC_CANON_EOS_GetPartialObject64,"PTP_OC_CANON_EOS_GetPartialObject64"},
    {PTP_OC_CANON_EOS_GetObjectInfoEx64,"PTP_OC_CANON_EOS_GetObjectInfoEx64"},
    {PTP_OC_CANON_EOS_GetPartialObjectEX64,"PTP_OC_CANON_EOS_GetPartialObjectEX64"},
    {PTP_OC_CANON_EOS_CreateHandle64,"PTP_OC_CANON_EOS_CreateHandle64"},
    {PTP_OC_CANON_EOS_NotifyEstimateNumberofImport,"PTP_OC_CANON_EOS_NotifyEstimateNumberofImport"},
    {PTP_OC_CANON_EOS_NotifyNumberofImported,"PTP_OC_CANON_EOS_NotifyNumberofImported"},
    {PTP_OC_CANON_EOS_NotifySizeOfPartialDataTransfer,"PTP_OC_CANON_EOS_NotifySizeOfPartialDataTransfer"},
    {PTP_OC_CANON_EOS_NotifyFinish,"PTP_OC_CANON_EOS_NotifyFinish"},
    {PTP_OC_CANON_EOS_SetImageRecoveryDataEx,"PTP_OC_CANON_EOS_SetImageRecoveryDataEx"},
    {PTP_OC_CANON_EOS_GetImageRecoveryListEx,"PTP_OC_CANON_EOS_GetImageRecoveryListEx"},
    {PTP_OC_CANON_EOS_NotifyAutoTransferStatus,"PTP_OC_CANON_EOS_NotifyAutoTransferStatus"},
    {PTP_OC_CANON_EOS_GetReducedObject,"PTP_OC_CANON_EOS_GetReducedObject"},
    {PTP_OC_CANON_EOS_NotifySaveComplete,"PTP_OC_CANON_EOS_NotifySaveComplete"},
    {PTP_OC_CANON_EOS_GetObjectURL,"PTP_OC_CANON_EOS_GetObjectURL"},
};

ptp_opcode_trans_t ptp_opcode_sony_trans[] = {
    {PTP_OC_SONY_SDIOConnect,"PTP_OC_SONY_SDIOConnect"},
    {PTP_OC_SONY_GetSDIOGetExtDeviceInfo,"PTP_OC_SONY_GetSDIOGetExtDeviceInfo"},
    {PTP_OC_SONY_GetDevicePropdesc,"PTP_OC_SONY_GetDevicePropdesc"},
    {PTP_OC_SONY_GetDevicePropertyValue,"PTP_OC_SONY_GetDevicePropertyValue"},
    {PTP_OC_SONY_SetControlDeviceA,"PTP_OC_SONY_SetControlDeviceA"},
    {PTP_OC_SONY_GetControlDeviceDesc,"PTP_OC_SONY_GetControlDeviceDesc"},
    {PTP_OC_SONY_SetControlDeviceB,"PTP_OC_SONY_SetControlDeviceB"},
    {PTP_OC_SONY_GetAllDevicePropData,"PTP_OC_SONY_GetAllDevicePropData"},
};

ptp_opcode_trans_t ptp_opcode_parrot_trans[] = {
    {PTP_OC_PARROT_GetSunshineValues,"PTP_OC_PARROT_GetSunshineValues"},
    {PTP_OC_PARROT_GetTemperatureValues,"PTP_OC_PARROT_GetTemperatureValues"},
    {PTP_OC_PARROT_GetAngleValues,"PTP_OC_PARROT_GetAngleValues"},
    {PTP_OC_PARROT_GetGpsValues,"PTP_OC_PARROT_GetGpsValues"},
    {PTP_OC_PARROT_GetGyroscopeValues,"PTP_OC_PARROT_GetGyroscopeValues"},
    {PTP_OC_PARROT_GetAccelerometerValues,"PTP_OC_PARROT_GetAccelerometerValues"},
    {PTP_OC_PARROT_GetMagnetometerValues,"PTP_OC_PARROT_GetMagnetometerValues"},
    {PTP_OC_PARROT_GetImuValues,"PTP_OC_PARROT_GetImuValues"},
    {PTP_OC_PARROT_GetStatusMask,"PTP_OC_PARROT_GetStatusMask"},
    {PTP_OC_PARROT_EjectStorage,"PTP_OC_PARROT_EjectStorage"},
    {PTP_OC_PARROT_StartMagnetoCalib,"PTP_OC_PARROT_StartMagnetoCalib"},
    {PTP_OC_PARROT_StopMagnetoCalib,"PTP_OC_PARROT_StopMagnetoCalib"},
    {PTP_OC_PARROT_MagnetoCalibStatus,"PTP_OC_PARROT_MagnetoCalibStatus"},
    {PTP_OC_PARROT_SendFirmwareUpdate,"PTP_OC_PARROT_SendFirmwareUpdate"},
};

const char* ptp_get_opcode_name(PTPParams* params, u16 opcode)
{
#define RETURN_NAME_FROM_TABLE(TABLE, OPCODE) \
{ \
    unsigned int i; \
    for (i=0; i<sizeof(TABLE)/sizeof(TABLE[0]); i++) \
        if (OPCODE == TABLE[i].opcode) \
            return TABLE[i].name; \
    return "Unknown PTP_OC"; \
}

    if (!(opcode & 0x8000))
        RETURN_NAME_FROM_TABLE(ptp_opcode_trans, opcode);

    switch (params->deviceinfo.VendorExtensionID) {
    case PTP_VENDOR_MICROSOFT:
    case PTP_VENDOR_PANASONIC:
    case PTP_VENDOR_MTP:    RETURN_NAME_FROM_TABLE(ptp_opcode_mtp_trans, opcode);
    case PTP_VENDOR_NIKON:  RETURN_NAME_FROM_TABLE(ptp_opcode_nikon_trans, opcode);
    case PTP_VENDOR_CANON:  RETURN_NAME_FROM_TABLE(ptp_opcode_canon_trans, opcode);
    case PTP_VENDOR_SONY:   RETURN_NAME_FROM_TABLE(ptp_opcode_sony_trans, opcode);
    case PTP_VENDOR_PARROT: RETURN_NAME_FROM_TABLE(ptp_opcode_parrot_trans, opcode);
    default:
        break;
    }
#undef RETURN_NAME_FROM_TABLE

    return "Unknown VendorExtensionID";
}

struct {
    u16 code;
    const char *name;
} ptp_event_codes[] = {
    {PTP_EC_Undefined, "Undefined"},
    {PTP_EC_CancelTransaction, "CancelTransaction"},
    {PTP_EC_ObjectAdded, "ObjectAdded"},
    {PTP_EC_ObjectRemoved, "ObjectRemoved"},
    {PTP_EC_StoreAdded, "StoreAdded"},
    {PTP_EC_StoreRemoved, "StoreRemoved"},
    {PTP_EC_DevicePropChanged, "DevicePropChanged"},
    {PTP_EC_ObjectInfoChanged, "ObjectInfoChanged"},
    {PTP_EC_DeviceInfoChanged, "DeviceInfoChanged"},
    {PTP_EC_RequestObjectTransfer, "RequestObjectTransfer"},
    {PTP_EC_StoreFull, "StoreFull"},
    {PTP_EC_DeviceReset, "DeviceReset"},
    {PTP_EC_StorageInfoChanged, "StorageInfoChanged"},
    {PTP_EC_CaptureComplete, "CaptureComplete"},
    {PTP_EC_UnreportedStatus, "UnreportedStatus"},

    {PTP_EC_MTP_ObjectPropChanged, "ObjectPropChanged"},
    {PTP_EC_MTP_ObjectPropDescChanged, "ObjectPropDescChanged"},
    {PTP_EC_MTP_ObjectReferencesChanged, "ObjectReferencesChanged"},
};

/**
 * PTP获取事件代码名
 *
 * @param params     PTP参数
 * @param event_code 事件代码
 *
 * @return 成功返回找到的事件名字字符串
 */
const char* ptp_get_event_code_name(PTPParams* params, u16 event_code)
{
    unsigned int i;
    for (i = 0; i<sizeof(ptp_event_codes)/sizeof(ptp_event_codes[0]); i++)
        if (event_code == ptp_event_codes[i].code)
            return ptp_event_codes[i].name;
    return "Unknown Event";
}

/*
 * 分配默认初始化的一个新的对象属性
 *
 * @param props     属性链表
 * @param nrofprops 属性数量
 *
 * @return 成功返回新分配的属性
 */
MTPProperties * ptp_get_new_object_prop_entry(MTPProperties **props, int *nrofprops)
{
    MTPProperties *newprops;
    MTPProperties *prop;

    newprops = mtp_realloc(*props, sizeof(MTPProperties) * (*nrofprops + 1));
    if (newprops == NULL)
        return NULL;
    /* 获取最后新分配的属性*/
    prop = &newprops[*nrofprops];
    /* 初始化新的属性*/
    prop->property = PTP_OPC_StorageID; /* Should be "unknown" */
    prop->datatype = PTP_DTC_UNDEF;
    prop->ObjectHandle = 0x00000000U;
    prop->propval.str = NULL;

    (*props) = newprops;
    (*nrofprops)++;
    return prop;
}

/**
 * PTP销毁对象属性
 *
 * @param prop MTP属性
 **/
void ptp_destroy_object_prop(MTPProperties *prop)
{
    if (!prop)
        return;
    /* 属性数据类型是字符串，释放属性字符串*/
    if (prop->datatype == PTP_DTC_STR && prop->propval.str != NULL)
        mtp_free(prop->propval.str);
    /* 属性数据类型是其他类型*/
    else if ((prop->datatype == PTP_DTC_AINT8 || prop->datatype == PTP_DTC_AINT16 ||
            prop->datatype == PTP_DTC_AINT32 || prop->datatype == PTP_DTC_AINT64 || prop->datatype == PTP_DTC_AINT128 ||
            prop->datatype == PTP_DTC_AUINT8 || prop->datatype == PTP_DTC_AUINT16 ||
            prop->datatype == PTP_DTC_AUINT32 || prop->datatype == PTP_DTC_AUINT64 || prop->datatype ==  PTP_DTC_AUINT128)
            && prop->propval.a.v != NULL)
        mtp_free(prop->propval.a.v);
}

/**
 * 销毁属性链表
 *
 * @param props     要销毁的MTP属性链表
 * @param nrofprops MTP属性链表数量
 */
void ptp_destroy_object_prop_list(MTPProperties *props, int nrofprops)
{
    int i;
    MTPProperties *prop = props;

    for (i = 0;i < nrofprops;i++,prop++)
        ptp_destroy_object_prop(prop);
    mtp_free(props);
}


/**
 * 在缓存中寻找一个对象属性
 *
 * @param params       PTP参数
 * @param handle       对象ID
 * @param attribute_id 属性ID
 *
 * @return 成功返回MTP属性结构体
 **/
MTPProperties * ptp_find_object_prop_in_cache(PTPParams *params, u32 const handle, u32 const attribute_id)
{
    unsigned int    i;
    MTPProperties   *prop;
    PTPObject   *ob;
    uint16_t    ret;
    /* 寻找对应的对象*/
    ret = ptp_object_find (params, handle, &ob);
    if (ret != PTP_RC_OK)
        return NULL;
    prop = ob->mtpprops;
    /* 遍历所有属性，寻找对应的属性ID*/
    for (i = 0;i < ob->nrofmtpprops;i++) {
        if (attribute_id == prop->property)
            return prop;
        prop++;
    }
    return NULL;
}

/**
 * 移除对象
 *
 * @param params PTP参数
 * @param handle 对象ID
 *
 * @return 成功返回PTP_RC_OK
 ***/
u16 ptp_remove_object_from_cache(PTPParams *params, u32 handle)
{
    unsigned int i;
    PTPObject   *ob;

    /* 寻找对应的对象*/
    CHECK_PTP_RC(ptp_object_find (params, handle, &ob));
    i = ob-params->objects;
    /* 释放对象 */
    ptp_free_object (ob);
    /* 重新排列对象*/
    if (i < params->nrofobjects - 1)
        memmove (ob, ob + 1,(params->nrofobjects - 1 - i) * sizeof(PTPObject));
    params->nrofobjects--;
    /* 重新分配内存*/
    params->objects = mtp_realloc(params->objects, sizeof(PTPObject)*params->nrofobjects);
    return PTP_RC_OK;
}

/**
 * 排列函数
 *
 * @param a PTP对象
 * @param b PTP对象
 * @return 如果小于0，则a在b左边，如果等于0，则顺序不确定，如果大于0，则a在b的右边
 **/
static int _cmp_ob (const void *a, const void *b)
{
    PTPObject *oa = (PTPObject*)a;
    PTPObject *ob = (PTPObject*)b;

    /* 不要把oid想减然后返回...
     * unsigned int -> int 在0xfffc0000 vs 0x0004000 这样的情况下转换将溢出*/
    if (oa->oid > ob->oid) return 1;
    if (oa->oid < ob->oid) return -1;
    return 0;
}

/**
 * PTP排列对象
 *
 * @param params PTP参数
 **/
void ptp_objects_sort (PTPParams *params)
{
    qsort (params->objects, params->nrofobjects, sizeof(PTPObject), _cmp_ob);
}

/**
 * 寻找对应的对象
 *
 * @param params PTP参数
 * @param handle 对象ID
 * @param retob  返回的对象结构体
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_object_find (PTPParams *params, u32 handle, PTPObject **retob)
{
    PTPObject   tmpob;

    tmpob.oid = handle;
    /* 二分法搜索对象*/
    *retob = bsearch (&tmpob, params->objects, params->nrofobjects, sizeof(tmpob), _cmp_ob);
    if (!*retob)
        return PTP_RC_GeneralError;
    return PTP_RC_OK;
}

/**
 * 寻找对应的对象ID的对象，找不到就插入新的对象
 *
 * @param params PTP参数
 * @param handle 对象ID
 * @param retob  返回的对象
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_object_find_or_insert (PTPParams *params, u32 handle, PTPObject **retob)
{
    unsigned int    begin, end, cursor;
    unsigned int    insertat;
    PTPObject   *newobs;

    if (!handle)
        return PTP_RC_GeneralError;
    *retob = NULL;

    /* 不存在对象*/
    if (!params->nrofobjects) {
        params->objects = mtp_calloc(1, sizeof(PTPObject));
        params->nrofobjects = 1;
        params->objects[0].oid = handle;
        *retob = &params->objects[0];
        return PTP_RC_OK;
    }
    begin = 0;
    end = params->nrofobjects - 1;
    /*ptp_debug (params, "searching %08x, total=%d", handle, params->nrofobjects);*/
    /* 二分法寻找对应的对象*/
    while (1) {
        cursor = (end - begin)/2 + begin;
        /*ptp_debug (params, "ob %d: %08x [%d-%d]", cursor, params->objects[cursor].oid, begin, end);*/
        if (params->objects[cursor].oid == handle) {
            *retob = &params->objects[cursor];
            return PTP_RC_OK;
        }
        if (params->objects[cursor].oid < handle)
            begin = cursor;
        else
            end = cursor;
        if ((end - begin) <= 1)
            break;
    }
    /* 最后剩下两个数*/
    if (params->objects[begin].oid == handle) {
        *retob = &params->objects[begin];
        return PTP_RC_OK;
    }
    if (params->objects[end].oid == handle) {
        *retob = &params->objects[end];
        return PTP_RC_OK;
    }
    /* 没找到对象，插入新的对象*/
    if ((begin == 0) && (handle < params->objects[0].oid)) {
        insertat = begin;
    } else {
        if ((end == params->nrofobjects - 1) && (handle > params->objects[end].oid))
            insertat = end + 1;
        else
            insertat = begin + 1;
    }
    /*ptp_debug (params, "inserting oid %x at [%x,%x], begin=%d, end=%d, insertat=%d\n", handle, params->objects[begin].oid, params->objects[end].oid, begin, end, insertat);*/
    /* 重现分配内存*/
    newobs = mtp_realloc (params->objects, sizeof(PTPObject)*(params->nrofobjects + 1));
    if (!newobs)
        return PTP_RC_GeneralError;
    params->objects = newobs;
    /* 重新排序*/
    if (insertat < params->nrofobjects)
        memmove (&params->objects[insertat + 1], &params->objects[insertat],
                (params->nrofobjects - insertat) * sizeof(PTPObject));
    /* 初始化新的对象*/
    memset(&params->objects[insertat], 0, sizeof(PTPObject));
    params->objects[insertat].oid = handle;
    *retob = &params->objects[insertat];
    params->nrofobjects++;
    return PTP_RC_OK;
}

/**
 * 对指定的对象执行某操作
 *
 * @param params PTP参数
 * @param handle 对象ID
 * @param want   想要的操作的标志
 * @param retob  返回的对象
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_object_want (PTPParams *params, u32 handle, unsigned int want, PTPObject **retob)
{
    u16    ret;
    PTPObject   *ob;
    /*Camera        *camera = ((PTPData *)params->data)->camera;*/

    /* 设备对GetObjectInfo无响应, 强制设置GetPropList */
    if (params->device_flags & DEVICE_FLAG_PROPLIST_OVERRIDES_OI)
        want |= PTPOBJECT_MTPPROPLIST_LOADED;

    *retob = NULL;
    if (!handle) {
        //ptp_debug (params, "ptp_object_want: querying handle 0?\n");
        mtp_printf("ptp_object_want: querying handle 0?\r\n");
        return PTP_RC_GeneralError;
    }
    /* 寻找或插入对应的对象*/
    CHECK_PTP_RC(ptp_object_find_or_insert (params, handle, &ob));
    *retob = ob;
    /* 是否符合要求*/
    if ((ob->flags & want) == want)
        return PTP_RC_OK;

#define X (PTPOBJECT_OBJECTINFO_LOADED | PTPOBJECT_STORAGEID_LOADED | PTPOBJECT_PARENTOBJECT_LOADED)
    /* 找到的对象标志不符合要求*/
    if ((want & X) && ((ob->flags & X) != X)) {
        u32    saveparent = 0;

        /* 一个EOS的问题, getobjecthandles(root)返回的对象没有根目录标志. */
        if (ob->flags & PTPOBJECT_PARENTOBJECT_LOADED)
            saveparent = ob->oi.ParentObject;
        /* 获取对象信息*/
        ret = ptp_getobjectinfo (params, handle, &ob->oi);
        if (ret != PTP_RC_OK) {
            /* 从内部链表中删除 */
            ptp_remove_object_from_cache(params, handle);
            return ret;
        }
        /* 没有文件名*/
        if (!ob->oi.Filename)
            ob->oi.Filename = mtp_strdup("<none>");
        if (ob->flags & PTPOBJECT_PARENTOBJECT_LOADED) {
            if (ob->oi.ParentObject != saveparent)
                ptp_debug (params, "saved parent %08x is not the same as read via getobjectinfo %08x", ob->oi.ParentObject, saveparent);
            ob->oi.ParentObject = saveparent;
        }

        /* 第二个EOS问题, 0x20000000 has 0x20000000 as parent */
        if (ob->oi.ParentObject == handle)
            ob->oi.ParentObject = 0;

        /* Apple iOS X does that for the root folder. */
        if (ob->oi.ParentObject == ob->oi.StorageID) {
            ptp_debug (params, "parent %08x of %s has same id as storage id. rewriting to 0.", ob->oi.ParentObject, ob->oi.Filename);
            ob->oi.ParentObject = 0;
        }

        /* 读取佳能特特殊标志 */
        if ((params->deviceinfo.VendorExtensionID == PTP_VENDOR_CANON) &&
            ptp_operation_issupported(params,PTP_OC_CANON_GetObjectInfoEx)) {
            PTPCANONFolderEntry *ents = NULL;
            u32            numents = 0;
            /* 获取佳能对象信息*/
            ret = ptp_canon_getobjectinfo(params,
                                          ob->oi.StorageID, 0,
                                          ob->oi.ParentObject, handle,
                                          &ents, &numents);
            if ((ret == PTP_RC_OK) && (numents >= 1))
                ob->canon_flags = ents[0].Flags;
            mtp_free (ents);
        }

        ob->flags |= X;
    }
#undef X
    if ((want & PTPOBJECT_MTPPROPLIST_LOADED) &&
        (!(ob->flags & PTPOBJECT_MTPPROPLIST_LOADED))) {
        int     nrofprops = 0;
        MTPProperties   *props = NULL;

        if (params->device_flags & DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST) {
            want &= ~PTPOBJECT_MTPPROPLIST_LOADED;
            goto fallback;
        }
        /* Microsoft/MTP 有快速目录检索 */
        if (!ptp_operation_issupported(params,PTP_OC_MTP_GetObjPropList)) {
            want &= ~PTPOBJECT_MTPPROPLIST_LOADED;
            goto fallback;
        }

        ptp_debug (params, "ptp2/mtpfast: reading mtp proplist of %08x", handle);
        /* 只检索一个对象 */
        ret = ptp_mtp_getobjectproplist_single (params, handle, &props, &nrofprops);
        if (ret != PTP_RC_OK)
            goto fallback;
        ob->mtpprops = props;
        ob->nrofmtpprops = nrofprops;

        /* 用检索的属性的数据重写对象信息数据*/
        if (params->device_flags & DEVICE_FLAG_PROPLIST_OVERRIDES_OI) {
            unsigned int i;
            MTPProperties *prop = ob->mtpprops;

            for (i = 0;i < ob->nrofmtpprops; i++, prop++) {
                /* 以防我们得到所有子树对象*/
                if (prop->ObjectHandle != handle)
                    continue;

                switch (prop->property) {
                case PTP_OPC_StorageID:
                    ob->oi.StorageID = prop->propval.u32;
                    break;
                case PTP_OPC_ObjectFormat:
                    ob->oi.ObjectFormat = prop->propval.u16;
                    break;
                case PTP_OPC_ProtectionStatus:
                    ob->oi.ProtectionStatus = prop->propval.u16;
                    break;
                case PTP_OPC_ObjectSize:
                    if (prop->datatype == PTP_DTC_UINT64) {
                        ob->oi.ObjectCompressedSize = prop->propval.u64;
                    } else if (prop->datatype == PTP_DTC_UINT32) {
                        ob->oi.ObjectCompressedSize = prop->propval.u32;
                    }
                    break;
                case PTP_OPC_AssociationType:
                    ob->oi.AssociationType = prop->propval.u16;
                    break;
                case PTP_OPC_AssociationDesc:
                    ob->oi.AssociationDesc = prop->propval.u32;
                    break;
                case PTP_OPC_ObjectFileName:
                    if (prop->propval.str) {
                        free(ob->oi.Filename);
                        ob->oi.Filename = mtp_strdup(prop->propval.str);
                    }
                    break;
                case PTP_OPC_DateCreated:
                    ob->oi.CaptureDate = ptp_unpack_PTPTIME(prop->propval.str);
                    break;
                case PTP_OPC_DateModified:
                    ob->oi.ModificationDate = ptp_unpack_PTPTIME(prop->propval.str);
                    break;
                case PTP_OPC_Keywords:
                    if (prop->propval.str) {
                        free(ob->oi.Keywords);
                        ob->oi.Keywords = mtp_strdup(prop->propval.str);
                    }
                    break;
                case PTP_OPC_ParentObject:
                    ob->oi.ParentObject = prop->propval.u32;
                    break;
                }
            }
        }

#if 0
        MTPProperties   *xpl;
        int j;
        PTPObjectInfo   oinfo;

        memset (&oinfo,0,sizeof(oinfo));
        /* hmm, not necessary ... only if we would use it */
        for (j=0;j<nrofprops;j++) {
            xpl = &props[j];
            switch (xpl->property) {
            case PTP_OPC_ParentObject:
                if (xpl->datatype != PTP_DTC_UINT32) {
                    ptp_debug (params, "ptp2/mtpfast: parentobject has type 0x%x???", xpl->datatype);
                    break;
                }
                oinfo.ParentObject = xpl->propval.u32;
                ptp_debug (params, "ptp2/mtpfast: parent 0x%x", xpl->propval.u32);
                break;
            case PTP_OPC_ObjectFormat:
                if (xpl->datatype != PTP_DTC_UINT16) {
                    ptp_debug (params, "ptp2/mtpfast: objectformat has type 0x%x???", xpl->datatype);
                    break;
                }
                oinfo.ObjectFormat = xpl->propval.u16;
                ptp_debug (params, "ptp2/mtpfast: ofc 0x%x", xpl->propval.u16);
                break;
            case PTP_OPC_ObjectSize:
                switch (xpl->datatype) {
                case PTP_DTC_UINT32:
                    oinfo.ObjectCompressedSize = xpl->propval.u32;
                    break;
                case PTP_DTC_UINT64:
                    oinfo.ObjectCompressedSize = xpl->propval.u64;
                    break;
                default:
                    ptp_debug (params, "ptp2/mtpfast: objectsize has type 0x%x???", xpl->datatype);
                    break;
                }
                ptp_debug (params, "ptp2/mtpfast: objectsize %u", xpl->propval.u32);
                break;
            case PTP_OPC_StorageID:
                if (xpl->datatype != PTP_DTC_UINT32) {
                    ptp_debug (params, "ptp2/mtpfast: storageid has type 0x%x???", xpl->datatype);
                    break;
                }
                oinfo.StorageID = xpl->propval.u32;
                ptp_debug (params, "ptp2/mtpfast: storageid 0x%x", xpl->propval.u32);
                break;
            case PTP_OPC_ProtectionStatus:/*UINT16*/
                if (xpl->datatype != PTP_DTC_UINT16) {
                    ptp_debug (params, "ptp2/mtpfast: protectionstatus has type 0x%x???", xpl->datatype);
                    break;
                }
                oinfo.ProtectionStatus = xpl->propval.u16;
                ptp_debug (params, "ptp2/mtpfast: protection 0x%x", xpl->propval.u16);
                break;
            case PTP_OPC_ObjectFileName:
                if (xpl->datatype != PTP_DTC_STR) {
                    ptp_debug (params, "ptp2/mtpfast: filename has type 0x%x???", xpl->datatype);
                    break;
                }
                if (xpl->propval.str) {
                    ptp_debug (params, "ptp2/mtpfast: filename %s", xpl->propval.str);
                    oinfo.Filename = strdup(xpl->propval.str);
                } else {
                    oinfo.Filename = NULL;
                }
                break;
            case PTP_OPC_DateCreated:
                if (xpl->datatype != PTP_DTC_STR) {
                    ptp_debug (params, "ptp2/mtpfast: datecreated has type 0x%x???", xpl->datatype);
                    break;
                }
                ptp_debug (params, "ptp2/mtpfast: capturedate %s", xpl->propval.str);
                oinfo.CaptureDate = ptp_unpack_PTPTIME (xpl->propval.str);
                break;
            case PTP_OPC_DateModified:
                if (xpl->datatype != PTP_DTC_STR) {
                    ptp_debug (params, "ptp2/mtpfast: datemodified has type 0x%x???", xpl->datatype);
                    break;
                }
                ptp_debug (params, "ptp2/mtpfast: moddate %s", xpl->propval.str);
                oinfo.ModificationDate = ptp_unpack_PTPTIME (xpl->propval.str);
                break;
            default:
                if ((xpl->property & 0xfff0) == 0xdc00)
                    ptp_debug (params, "ptp2/mtpfast:case %x type %x unhandled", xpl->property, xpl->datatype);
                break;
            }
        }
        if (!oinfo.Filename)
            /* i have one such file on my Creative */
            oinfo.Filename = strdup("<null>");
#endif
        ob->flags |= PTPOBJECT_MTPPROPLIST_LOADED;
fallback:   ;
    }
    if ((ob->flags & want) == want)
        return PTP_RC_OK;
    ptp_debug (params, "ptp_object_want: oid 0x%08x, want flags %x, have only %x?", handle, want, ob->flags);
    return PTP_RC_GeneralError;
}

/**
 * PTP添加对象到缓存中
 *
 * @param params PTP参数
 * @param handle 对象ID
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 ptp_add_object_to_cache(PTPParams *params, u32 handle)
{
    PTPObject *ob;
    return ptp_object_want (params, handle, PTPOBJECT_OBJECTINFO_LOADED|PTPOBJECT_MTPPROPLIST_LOADED, &ob);
}


