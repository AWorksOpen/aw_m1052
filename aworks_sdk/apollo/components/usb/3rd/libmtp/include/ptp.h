/* ptp.h
 *
 * Copyright (C) 2001 Mariusz Woloszyn <emsi@ipartners.pl>
 * Copyright (C) 2003-2017 Marcus Meissner <marcus@jet.franken.de>
 * Copyright (C) 2006-2008 Linus Walleij <triad@df.lth.se>
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

#ifndef __PTP_H__
#define __PTP_H__

#if defined(HAVE_ICONV) && defined(HAVE_LANGINFO_H)
#include <iconv.h>
#endif
#include <stdarg.h>
#include "gphoto2-endian.h"
#include "device-flags.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "libmtp.h"

/* PTP 数据层字节序*/
#define PTP_DL_BE           0xF0
#define PTP_DL_LE           0x0F

/* USB 接口类 */
#ifndef USB_CLASS_PTP
#define USB_CLASS_PTP           6
#endif

/* PTP 请求/响应/事件的通用PTP容器(独立于传输)*/
struct _PTPContainer {
    /* 事件代码*/
    u16 Code;
    /* 会话ID*/
    u32 SessionID;
    /* 事务ID*/
    u32 Transaction_ID;
    /* 参数   可以是任意小于或等于uint32_t的大小*/
    u32 Param1;
    u32 Param2;
    u32 Param3;
    /* 事件只能有三个参数 */
    u32 Param4;
    u32 Param5;
    /* 有意义的参数的数量 */
    u8  Nparam;
};
typedef struct _PTPContainer PTPContainer;

/* PTP批量容器最大长度*/
#define PTP_USB_BULK_SS_MAX_PACKET_LEN_WRITE    1024
#define PTP_USB_BULK_SS_MAX_PACKET_LEN_READ   1024
/* PTP批量容器头长度*/
#define PTP_USB_BULK_HDR_LEN        (2*sizeof(u32) + 2*sizeof(u16))
/* PTP批量写数据长度*/
#define PTP_USB_BULK_PAYLOAD_LEN_WRITE  (PTP_USB_BULK_SS_MAX_PACKET_LEN_WRITE - PTP_USB_BULK_HDR_LEN)
/* 数据长度 = PTP批量容器最大长度 - PTP批量容器头长度*/
#define PTP_USB_BULK_PAYLOAD_LEN_READ   (PTP_USB_BULK_SS_MAX_PACKET_LEN_READ - PTP_USB_BULK_HDR_LEN)
/* PTP批量容器头长度 + 5个参数的长度*/
#define PTP_USB_BULK_REQ_LEN    (PTP_USB_BULK_HDR_LEN + 5*sizeof(u32))

/* PTP批量容器*/
struct _PTPUSBBulkContainer {
    /* 请求长度*/
    u32 length;
    /* 容器类型*/
    u16 type;
    /* 事件代码*/
    u16 code;
    /* 事务ID*/
    u32 trans_id;
    /* 参数表*/
    union {
        struct {
            u32 param1;
            u32 param2;
            u32 param3;
            u32 param4;
            u32 param5;
        } params;
        /* 这个必须设置为PTP_USB_BULK_PAYLOAD_LEN_WRITE和PTP_USB_BULK_PAYLOAD_LEN_READ的最大值*/
        unsigned char data[PTP_USB_BULK_PAYLOAD_LEN_READ];
    } payload;
};
typedef struct _PTPUSBBulkContainer PTPUSBBulkContainer;

/* USB 容器类型 */
#define PTP_USB_CONTAINER_UNDEFINED     0x0000
#define PTP_USB_CONTAINER_COMMAND       0x0001
#define PTP_USB_CONTAINER_DATA          0x0002
#define PTP_USB_CONTAINER_RESPONSE      0x0003
#define PTP_USB_CONTAINER_EVENT         0x0004

/* 自定义ID*/
/* List is linked from here: http://www.imaging.org/site/IST/Standards/PTP_Standards.aspx */
#define PTP_VENDOR_EASTMAN_KODAK        0x00000001
#define PTP_VENDOR_SEIKO_EPSON          0x00000002
#define PTP_VENDOR_AGILENT          0x00000003
#define PTP_VENDOR_POLAROID         0x00000004
#define PTP_VENDOR_AGFA_GEVAERT         0x00000005
#define PTP_VENDOR_MICROSOFT            0x00000006
#define PTP_VENDOR_EQUINOX          0x00000007
#define PTP_VENDOR_VIEWQUEST            0x00000008
#define PTP_VENDOR_STMICROELECTRONICS       0x00000009
#define PTP_VENDOR_NIKON            0x0000000A
#define PTP_VENDOR_CANON            0x0000000B
#define PTP_VENDOR_FOTONATION           0x0000000C
#define PTP_VENDOR_PENTAX           0x0000000D
#define PTP_VENDOR_FUJI             0x0000000E
#define PTP_VENDOR_NDD_MEDICAL_TECHNOLOGIES 0x00000012
#define PTP_VENDOR_SAMSUNG          0x0000001a
#define PTP_VENDOR_PARROT           0x0000001b
#define PTP_VENDOR_PANASONIC            0x0000001c
/* not from standards papers, but from traces: */
#define PTP_VENDOR_SONY             0x00000011 /* 在A900中观察到 */

/* MTP使用的自定义扩展ID (偶尔, 通常使用6) */
#define PTP_VENDOR_MTP          0xffffffff

/* gphoto overrides */
#define PTP_VENDOR_GP_OLYMPUS          0x0000fffe
#define PTP_VENDOR_GP_OLYMPUS_OMD      0x0000fffd


/* 操作代码 */

/* PTP v1.0 操作代码 */
#define PTP_OC_Undefined                0x1000
#define PTP_OC_GetDeviceInfo            0x1001
#define PTP_OC_OpenSession              0x1002
#define PTP_OC_CloseSession             0x1003
#define PTP_OC_GetStorageIDs            0x1004
#define PTP_OC_GetStorageInfo           0x1005
#define PTP_OC_GetNumObjects            0x1006
#define PTP_OC_GetObjectHandles         0x1007
#define PTP_OC_GetObjectInfo            0x1008
#define PTP_OC_GetObject                0x1009
#define PTP_OC_GetThumb                 0x100A
#define PTP_OC_DeleteObject             0x100B
#define PTP_OC_SendObjectInfo           0x100C
#define PTP_OC_SendObject               0x100D
#define PTP_OC_InitiateCapture          0x100E
#define PTP_OC_FormatStore              0x100F
#define PTP_OC_ResetDevice              0x1010
#define PTP_OC_SelfTest                 0x1011
#define PTP_OC_SetObjectProtection      0x1012
#define PTP_OC_PowerDown                0x1013
#define PTP_OC_GetDevicePropDesc        0x1014
#define PTP_OC_GetDevicePropValue       0x1015
#define PTP_OC_SetDevicePropValue       0x1016
#define PTP_OC_ResetDevicePropValue     0x1017
#define PTP_OC_TerminateOpenCapture     0x1018
#define PTP_OC_MoveObject               0x1019
#define PTP_OC_CopyObject               0x101A
#define PTP_OC_GetPartialObject         0x101B
#define PTP_OC_InitiateOpenCapture      0x101C
/* PTP v1.1 操作代码 */
#define PTP_OC_StartEnumHandles     0x101D
#define PTP_OC_EnumHandles      0x101E
#define PTP_OC_StopEnumHandles      0x101F
#define PTP_OC_GetVendorExtensionMaps   0x1020
#define PTP_OC_GetVendorDeviceInfo  0x1021
#define PTP_OC_GetResizedImageObject    0x1022
#define PTP_OC_GetFilesystemManifest    0x1023
#define PTP_OC_GetStreamInfo        0x1024
#define PTP_OC_GetStream        0x1025

/* Canon 扩展操作代码 */
#define PTP_OC_CANON_GetPartialObjectInfo   0x9001
/* 9002 - sends 2 uint32, nothing back  */
#define PTP_OC_CANON_SetObjectArchive       0x9002
#define PTP_OC_CANON_KeepDeviceOn       0x9003
#define PTP_OC_CANON_LockDeviceUI       0x9004
#define PTP_OC_CANON_UnlockDeviceUI     0x9005
#define PTP_OC_CANON_GetObjectHandleByName  0x9006
/* no 9007 observed yet */
#define PTP_OC_CANON_InitiateReleaseControl 0x9008
#define PTP_OC_CANON_TerminateReleaseControl    0x9009
#define PTP_OC_CANON_TerminatePlaybackMode  0x900A
#define PTP_OC_CANON_ViewfinderOn       0x900B
#define PTP_OC_CANON_ViewfinderOff      0x900C
#define PTP_OC_CANON_DoAeAfAwb          0x900D
/* 900e - send nothing, gets 5 uint16t in 32bit entities back in 20byte datablob */
#define PTP_OC_CANON_GetCustomizeSpec       0x900E
#define PTP_OC_CANON_GetCustomizeItemInfo   0x900F
#define PTP_OC_CANON_GetCustomizeData       0x9010
#define PTP_OC_CANON_SetCustomizeData       0x9011
#define PTP_OC_CANON_GetCaptureStatus       0x9012
#define PTP_OC_CANON_CheckEvent         0x9013
#define PTP_OC_CANON_FocusLock          0x9014
#define PTP_OC_CANON_FocusUnlock        0x9015
#define PTP_OC_CANON_GetLocalReleaseParam   0x9016
#define PTP_OC_CANON_SetLocalReleaseParam   0x9017
#define PTP_OC_CANON_AskAboutPcEvf      0x9018
#define PTP_OC_CANON_SendPartialObject      0x9019
#define PTP_OC_CANON_InitiateCaptureInMemory    0x901A
#define PTP_OC_CANON_GetPartialObjectEx     0x901B
#define PTP_OC_CANON_SetObjectTime      0x901C
#define PTP_OC_CANON_GetViewfinderImage     0x901D
#define PTP_OC_CANON_GetObjectAttributes    0x901E
#define PTP_OC_CANON_ChangeUSBProtocol      0x901F
#define PTP_OC_CANON_GetChanges         0x9020
#define PTP_OC_CANON_GetObjectInfoEx        0x9021
#define PTP_OC_CANON_InitiateDirectTransfer 0x9022
#define PTP_OC_CANON_TerminateDirectTransfer    0x9023
#define PTP_OC_CANON_SendObjectInfoByPath   0x9024
#define PTP_OC_CANON_SendObjectByPath       0x9025
#define PTP_OC_CANON_InitiateDirectTansferEx    0x9026
#define PTP_OC_CANON_GetAncillaryObjectHandles  0x9027
#define PTP_OC_CANON_GetTreeInfo        0x9028
#define PTP_OC_CANON_GetTreeSize        0x9029
#define PTP_OC_CANON_NotifyProgress         0x902A
#define PTP_OC_CANON_NotifyCancelAccepted   0x902B
/* 902c: no parms, read 3 uint32 in data, no response parms */
#define PTP_OC_CANON_902C           0x902C
#define PTP_OC_CANON_GetDirectory       0x902D
#define PTP_OC_CANON_902E           0x902E
#define PTP_OC_CANON_902F           0x902F  /* used during camera init */

#define PTP_OC_CANON_SetPairingInfo     0x9030
#define PTP_OC_CANON_GetPairingInfo     0x9031
#define PTP_OC_CANON_DeletePairingInfo      0x9032
#define PTP_OC_CANON_GetMACAddress      0x9033 /* no args */
/*
0000  12 00 00 00 02 00 33 90-1a 00 00 00 2c 9e fc c8  ......3.....,...
0010  33 e3                  -                         3.
 */

/* 9034: 1 param, no parms returned */
#define PTP_OC_CANON_SetDisplayMonitor      0x9034
#define PTP_OC_CANON_PairingComplete        0x9035
#define PTP_OC_CANON_GetWirelessMAXChannel  0x9036

#define PTP_OC_CANON_GetWebServiceSpec      0x9068 /* no args */
/* data returned:
0000  1e 00 00 00 02 00 68 90-1a 00 00 00 00 01 08 00  ......h.........
0010  14 00 bc ce 00 00 78 00-78 00 00 14 00 00        ......x.x.....
*/

#define PTP_OC_CANON_GetWebServiceData      0x9069 /* no args */
#define PTP_OC_CANON_SetWebServiceData      0x906A
#define PTP_OC_CANON_DeleteWebServiceData   0x906B
#define PTP_OC_CANON_GetRootCertificateSpec 0x906C /* no args */
/*
0000  12 00 00 00 02 00 6c 90-1a 00 00 00 00 01 6c 30  ......l.......l0
0010  00 00                  -                         ..
*/
#define PTP_OC_CANON_GetRootCertificateData 0x906D /* no args */
/* empty data on test */
#define PTP_OC_CANON_SetRootCertificateData 0x906F
#define PTP_OC_CANON_GetGpsMobilelinkObjectInfo 0x9075 /* 2 args: utcstart, utcend */
#define PTP_OC_CANON_SendGpsTagInfo     0x9076 /* 1 arg: oid */
#define PTP_OC_CANON_GetTranscodeApproxSize 0x9077 /* 1 arg: oid? */
#define PTP_OC_CANON_RequestTranscodeStart  0x9078 /* 1 arg: oid? */
#define PTP_OC_CANON_RequestTranscodeCancel 0x9079 /* 1 arg: oid? */

/* 9101: no args, 8 byte data (01 00 00 00 00 00 00 00), no resp data. */
#define PTP_OC_CANON_EOS_GetStorageIDs      0x9101
/* 9102: 1 arg (0)
 * 0x28 bytes of data:
    00000000: 34 00 00 00 02 00 02 91 0a 00 00 00 04 00 03 00
    00000010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    00000020: 00 00 ff ff ff ff 03 43 00 46 00 00 00 03 41 00
    00000030: 3a 00 00 00
 * no resp args
 */
#define PTP_OC_CANON_EOS_GetStorageInfo     0x9102
#define PTP_OC_CANON_EOS_GetObjectInfo      0x9103
#define PTP_OC_CANON_EOS_GetObject      0x9104
#define PTP_OC_CANON_EOS_DeleteObject       0x9105
#define PTP_OC_CANON_EOS_FormatStore        0x9106
#define PTP_OC_CANON_EOS_GetPartialObject   0x9107
#define PTP_OC_CANON_EOS_GetDeviceInfoEx    0x9108

/* sample1:
 * 3 cmdargs: 1,0xffffffff,00 00 10 00;
 * data:
    00000000: 48 00 00 00 02 00 09 91 12 00 00 00 01 00 00 00
    00000010: 38 00 00 00 00 00 00 30 01 00 00 00 01 30 00 00
    00000020: 01 00 00 00 10 00 00 00 00 00 00 00 00 00 00 20
    00000030: 00 00 00 30 44 43 49 4d 00 00 00 00 00 00 00 00   DCIM
    00000040: 00 00 00 00 cc c3 01 46
 * 2 respargs: 0x0, 0x3c
 *
 * sample2:
 *
    00000000: 18 00 00 00 01 00 09 91 15 00 00 00 01 00 00 00
    00000010: 00 00 00 30 00 00 10 00

    00000000: 48 00 00 00 02 00 09 91 15 00 00 00 01 00 00 00
    00000010: 38 00 00 00 00 00 9c 33 01 00 00 00 01 30 00 00
    00000020: 01 00 00 00 10 00 00 00 00 00 00 00 00 00 00 30
    00000030: 00 00 9c 33 32 33 31 43 41 4e 4f 4e 00 00 00 00    231CANON
    00000040: 00 00 00 00 cc c3 01 46

 */
#define PTP_OC_CANON_EOS_GetObjectInfoEx    0x9109
#define PTP_OC_CANON_EOS_GetThumbEx     0x910A
#define PTP_OC_CANON_EOS_SendPartialObject  0x910B
#define PTP_OC_CANON_EOS_SetObjectAttributes    0x910C
#define PTP_OC_CANON_EOS_GetObjectTime      0x910D
#define PTP_OC_CANON_EOS_SetObjectTime      0x910E

/* 910f: no args, no data, 1 response arg (0). */
#define PTP_OC_CANON_EOS_RemoteRelease      0x910F
/* Marcus: looks more like "Set DeviceProperty" in the trace.
 *
 * no cmd args
 * data phase (0xc, 0xd11c, 0x1)
 * no resp args
 */
#define PTP_OC_CANON_EOS_SetDevicePropValueEx   0x9110
#define PTP_OC_CANON_EOS_GetRemoteMode      0x9113
/* 9114: 1 arg (0x1), no data, no resp data. */
#define PTP_OC_CANON_EOS_SetRemoteMode      0x9114
/* 9115: 1 arg (0x1), no data, no resp data. */
#define PTP_OC_CANON_EOS_SetEventMode       0x9115
/* 9116: no args, data phase, no resp data. */
#define PTP_OC_CANON_EOS_GetEvent       0x9116
#define PTP_OC_CANON_EOS_TransferComplete   0x9117
#define PTP_OC_CANON_EOS_CancelTransfer     0x9118
#define PTP_OC_CANON_EOS_ResetTransfer      0x9119

/* 911a: 3 args (0xfffffff7, 0x00001000, 0x00000001), no data, no resp data. */
/* 911a: 3 args (0x001dfc60, 0x00001000, 0x00000001), no data, no resp data. */
#define PTP_OC_CANON_EOS_PCHDDCapacity      0x911A

/* 911b: no cmd args, no data, no resp args */
#define PTP_OC_CANON_EOS_SetUILock      0x911B
/* 911c: no cmd args, no data, no resp args */
#define PTP_OC_CANON_EOS_ResetUILock        0x911C
#define PTP_OC_CANON_EOS_KeepDeviceOn       0x911D /* no arg */
#define PTP_OC_CANON_EOS_SetNullPacketMode  0x911E /* 1 param */
#define PTP_OC_CANON_EOS_UpdateFirmware     0x911F
#define PTP_OC_CANON_EOS_TransferCompleteDT 0x9120
#define PTP_OC_CANON_EOS_CancelTransferDT   0x9121
#define PTP_OC_CANON_EOS_SetWftProfile      0x9122
#define PTP_OC_CANON_EOS_GetWftProfile      0x9123 /* 2 args: setnum, configid */
#define PTP_OC_CANON_EOS_SetProfileToWft    0x9124
#define PTP_OC_CANON_EOS_BulbStart      0x9125
#define PTP_OC_CANON_EOS_BulbEnd        0x9126
#define PTP_OC_CANON_EOS_RequestDevicePropValue 0x9127

/* 0x9128 args (0x1/0x2, 0x0), no data, no resp args */
#define PTP_OC_CANON_EOS_RemoteReleaseOn    0x9128
/* 0x9129 args (0x1/0x2), no data, no resp args */
#define PTP_OC_CANON_EOS_RemoteReleaseOff   0x9129

#define PTP_OC_CANON_EOS_RegistBackgroundImage  0x912A
#define PTP_OC_CANON_EOS_ChangePhotoStudioMode  0x912B
#define PTP_OC_CANON_EOS_GetPartialObjectEx 0x912C
#define PTP_OC_CANON_EOS_ResetMirrorLockupState 0x9130 /* no args */
#define PTP_OC_CANON_EOS_PopupBuiltinFlash  0x9131
#define PTP_OC_CANON_EOS_EndGetPartialObjectEx  0x9132
#define PTP_OC_CANON_EOS_MovieSelectSWOn    0x9133 /* no args */
#define PTP_OC_CANON_EOS_MovieSelectSWOff   0x9134 /* no args */
#define PTP_OC_CANON_EOS_GetCTGInfo     0x9135
#define PTP_OC_CANON_EOS_GetLensAdjust      0x9136
#define PTP_OC_CANON_EOS_SetLensAdjust      0x9137
#define PTP_OC_CANON_EOS_ReadyToSendMusic   0x9138
/* 3 paramaeters, no data, OFC, size, unknown */
#define PTP_OC_CANON_EOS_CreateHandle       0x9139
#define PTP_OC_CANON_EOS_SendPartialObjectEx    0x913A
#define PTP_OC_CANON_EOS_EndSendPartialObjectEx 0x913B
#define PTP_OC_CANON_EOS_SetCTGInfo     0x913C
#define PTP_OC_CANON_EOS_SetRequestOLCInfoGroup 0x913D
#define PTP_OC_CANON_EOS_SetRequestRollingPitchingLevel 0x913E /* 1 arg: onoff? */
/* 3 args, 0x21201020, 0x110, 0x1000000 (potentially reverse order) */
#define PTP_OC_CANON_EOS_GetCameraSupport   0x913F
#define PTP_OC_CANON_EOS_SetRating      0x9140 /* 2 args, objectid, rating? */
#define PTP_OC_CANON_EOS_RequestInnerDevelopStart   0x9141 /* 2 args: 1 type, 1 object? */
#define PTP_OC_CANON_EOS_RequestInnerDevelopParamChange 0x9142
#define PTP_OC_CANON_EOS_RequestInnerDevelopEnd     0x9143
#define PTP_OC_CANON_EOS_GpsLoggingDataMode     0x9144 /* 1 arg */
#define PTP_OC_CANON_EOS_GetGpsLogCurrentHandle     0x9145
#define PTP_OC_CANON_EOS_SetImageRecoveryData       0x9146 /* sends data? */
#define PTP_OC_CANON_EOS_GetImageRecoveryList       0x9147
#define PTP_OC_CANON_EOS_FormatImageRecoveryData    0x9148
#define PTP_OC_CANON_EOS_GetPresetLensAdjustParam   0x9149 /* no arg */
#define PTP_OC_CANON_EOS_GetRawDispImage        0x914A /* ? 2 args ? */
#define PTP_OC_CANON_EOS_SaveImageRecoveryData      0x914B
#define PTP_OC_CANON_EOS_RequestBLE         0x914C /* 2? args? */
#define PTP_OC_CANON_EOS_DrivePowerZoom         0x914D /* 1 arg */

#define PTP_OC_CANON_EOS_GetIptcData        0x914F
#define PTP_OC_CANON_EOS_SetIptcData        0x9150 /* sends data? */
#define PTP_OC_CANON_EOS_InitiateViewfinder 0x9151  /* no arg */
#define PTP_OC_CANON_EOS_TerminateViewfinder    0x9152
/* EOS M2 wlan: 2 params, 0x00200000 0x01000000 */
#define PTP_OC_CANON_EOS_GetViewFinderData  0x9153
#define PTP_OC_CANON_EOS_DoAf           0x9154
#define PTP_OC_CANON_EOS_DriveLens      0x9155
#define PTP_OC_CANON_EOS_DepthOfFieldPreview    0x9156 /* 1 arg */
#define PTP_OC_CANON_EOS_ClickWB        0x9157 /* 2 args: x,y */
#define PTP_OC_CANON_EOS_Zoom           0x9158 /* 1 arg: zoom */
#define PTP_OC_CANON_EOS_ZoomPosition       0x9159 /* 2 args: x,y */
#define PTP_OC_CANON_EOS_SetLiveAfFrame     0x915A /* sends data? */
#define PTP_OC_CANON_EOS_TouchAfPosition    0x915B /* 3 args: type,x,y */
#define PTP_OC_CANON_EOS_SetLvPcFlavoreditMode  0x915C /* 1 arg */
#define PTP_OC_CANON_EOS_SetLvPcFlavoreditParam 0x915D /* 1 arg */
#define PTP_OC_CANON_EOS_AfCancel       0x9160
#define PTP_OC_CANON_EOS_SetImageRecoveryDataEx 0x916B
#define PTP_OC_CANON_EOS_GetImageRecoveryListEx 0x916C
#define PTP_OC_CANON_EOS_NotifyAutoTransferStatus   0x916E
#define PTP_OC_CANON_EOS_GetReducedObject   0x916F
#define PTP_OC_CANON_EOS_GetObjectInfo64    0x9170  /* 1 arg: oid */
#define PTP_OC_CANON_EOS_GetObject64        0x9171  /* 1 arg: oid */
#define PTP_OC_CANON_EOS_GetPartialObject64 0x9172  /* args: oid, offset, maxbyte */
#define PTP_OC_CANON_EOS_GetObjectInfoEx64  0x9173  /* 2 args: storageid, oid  ? */
#define PTP_OC_CANON_EOS_GetPartialObjectEX64   0x9174  /* args: oid, offset 64bit, maxbyte */
#define PTP_OC_CANON_EOS_CreateHandle64     0x9175
#define PTP_OC_CANON_EOS_NotifySaveComplete 0x9177
#define PTP_OC_CANON_EOS_NotifyEstimateNumberofImport       0x9182 /* 1 arg: importnumber */
#define PTP_OC_CANON_EOS_NotifyNumberofImported 0x9183 /* 1 arg: importnumber */
#define PTP_OC_CANON_EOS_NotifySizeOfPartialDataTransfer    0x9184 /* 4 args: filesizelow, filesizehigh, downloadsizelow, downloadsizehigh */
#define PTP_OC_CANON_EOS_NotifyFinish       0x9185 /* 1 arg: reason */
#define PTP_OC_CANON_EOS_GetObjectURL       0x91AB
#define PTP_OC_CANON_EOS_SetDefaultCameraSetting        0x91BE
#define PTP_OC_CANON_EOS_GetAEData      0x91BF
#define PTP_OC_CANON_EOS_NotifyNetworkError 0x91E8 /* 1 arg: errorcode */
#define PTP_OC_CANON_EOS_AdapterTransferProgress        0x91E9
#define PTP_OC_CANON_EOS_TransferComplete2  0x91F0
#define PTP_OC_CANON_EOS_CancelTransfer2    0x91F1
#define PTP_OC_CANON_EOS_FAPIMessageTX      0x91FE
#define PTP_OC_CANON_EOS_FAPIMessageRX      0x91FF

/* Nikon 扩展操作代码 */
#define PTP_OC_NIKON_GetProfileAllData  0x9006
#define PTP_OC_NIKON_SendProfileData    0x9007
#define PTP_OC_NIKON_DeleteProfile  0x9008
#define PTP_OC_NIKON_SetProfileData 0x9009
#define PTP_OC_NIKON_AdvancedTransfer   0x9010
#define PTP_OC_NIKON_GetFileInfoInBlock 0x9011
#define PTP_OC_NIKON_Capture        0x90C0  /* 1 param,   no data */
#define PTP_OC_NIKON_AfDrive        0x90C1  /* no params, no data */
#define PTP_OC_NIKON_SetControlMode 0x90C2  /* 1 param,  no data */
#define PTP_OC_NIKON_DelImageSDRAM  0x90C3  /* 1 param,  no data */
#define PTP_OC_NIKON_GetLargeThumb  0x90C4
#define PTP_OC_NIKON_CurveDownload  0x90C5  /* 1 param,   data in */
#define PTP_OC_NIKON_CurveUpload    0x90C6  /* 1 param,   data out */
#define PTP_OC_NIKON_CheckEvent     0x90C7  /* no params, data in */
#define PTP_OC_NIKON_DeviceReady    0x90C8  /* no params, no data */
#define PTP_OC_NIKON_SetPreWBData   0x90C9  /* 3 params,  data out */
#define PTP_OC_NIKON_GetVendorPropCodes 0x90CA  /* 0 params, data in */
#define PTP_OC_NIKON_AfCaptureSDRAM 0x90CB  /* no params, no data */
#define PTP_OC_NIKON_GetPictCtrlData    0x90CC  /* 2 params, data in */
#define PTP_OC_NIKON_SetPictCtrlData    0x90CD  /* 2 params, data out */
#define PTP_OC_NIKON_DelCstPicCtrl  0x90CE  /* 1 param, no data */
#define PTP_OC_NIKON_GetPicCtrlCapability   0x90CF  /* 1 param, data in */
/* Nikon Liveview stuff */
#define PTP_OC_NIKON_GetPreviewImg  0x9200
#define PTP_OC_NIKON_StartLiveView  0x9201  /* no params */
#define PTP_OC_NIKON_EndLiveView    0x9202  /* no params */
#define PTP_OC_NIKON_GetLiveViewImg 0x9203  /* no params, data in */
#define PTP_OC_NIKON_MfDrive        0x9204  /* 2 params */
#define PTP_OC_NIKON_ChangeAfArea   0x9205  /* 2 params */
#define PTP_OC_NIKON_AfDriveCancel  0x9206  /* no params */
/* 2 params:
 * 0xffffffff == No AF before,  0xfffffffe == AF before capture
 * sdram=1, card=0
 */
#define PTP_OC_NIKON_InitiateCaptureRecInMedia  0x9207  /* 1 params */

#define PTP_OC_NIKON_GetVendorStorageIDs    0x9209  /* no params, data in */

#define PTP_OC_NIKON_StartMovieRecInCard    0x920a  /* no params, no data */
#define PTP_OC_NIKON_EndMovieRec        0x920b  /* no params, no data */

#define PTP_OC_NIKON_TerminateCapture       0x920c  /* 2 params */

#define PTP_OC_NIKON_GetDevicePTPIPInfo 0x90E0

#define PTP_OC_NIKON_GetPartialObjectHiSpeed    0x9400  /* 3 params, data in */

/* From Nikon V1 Trace */
#define PTP_OC_NIKON_GetDevicePropEx        0x9504  /* gets device prop data */

/* Sony stuff */
/* 9201:
 *  3 params: 1,0,0 ; IN: data 8 bytes all 0
 * or:
 *  3 params: 2,0,0 ; IN: data 8 bytes all 0
 * or
 *  3 params: 3,0,0,: IN: data 8 butes all 0
 */
#define PTP_OC_SONY_SDIOConnect         0x9201
/* 9202: 1 param, 0xc8; IN data:
 * 16 bit: 0xc8
 * ptp array 32 bit: index, 16 bit values of propcodes  */
#define PTP_OC_SONY_GetSDIOGetExtDeviceInfo 0x9202

#define PTP_OC_SONY_GetDevicePropdesc       0x9203
#define PTP_OC_SONY_GetDevicePropertyValue  0x9204
/* 1 param, 16bit propcode, SEND DATA: propvalue */
#define PTP_OC_SONY_SetControlDeviceA       0x9205
#define PTP_OC_SONY_GetControlDeviceDesc    0x9206
/* 1 param, 16bit propcode, SEND DATA: propvalue */
#define PTP_OC_SONY_SetControlDeviceB       0x9207
/* get all device property data at once */
#define PTP_OC_SONY_GetAllDevicePropData    0x9209  /* gets a 4126 byte blob of device props ?*/
/* 微软/MTP扩展代码 */

#define PTP_OC_MTP_GetObjectPropsSupported  0x9801
#define PTP_OC_MTP_GetObjectPropDesc        0x9802
#define PTP_OC_MTP_GetObjectPropValue       0x9803
#define PTP_OC_MTP_SetObjectPropValue       0x9804
#define PTP_OC_MTP_GetObjPropList       0x9805
#define PTP_OC_MTP_SetObjPropList       0x9806
#define PTP_OC_MTP_GetInterdependendPropdesc    0x9807
#define PTP_OC_MTP_SendObjectPropList       0x9808
#define PTP_OC_MTP_GetObjectReferences      0x9810
#define PTP_OC_MTP_SetObjectReferences      0x9811
#define PTP_OC_MTP_UpdateDeviceFirmware     0x9812
#define PTP_OC_MTP_Skip             0x9820

/*
 * Windows媒体数字权限管理的便携式设备扩展代码(microsoft.com/WMDRMPD: 10.1)
 */
#define PTP_OC_MTP_WMDRMPD_GetSecureTimeChallenge   0x9101
#define PTP_OC_MTP_WMDRMPD_GetSecureTimeResponse    0x9102
#define PTP_OC_MTP_WMDRMPD_SetLicenseResponse   0x9103
#define PTP_OC_MTP_WMDRMPD_GetSyncList      0x9104
#define PTP_OC_MTP_WMDRMPD_SendMeterChallengeQuery  0x9105
#define PTP_OC_MTP_WMDRMPD_GetMeterChallenge    0x9106
#define PTP_OC_MTP_WMDRMPD_SetMeterResponse     0x9107
#define PTP_OC_MTP_WMDRMPD_CleanDataStore       0x9108
#define PTP_OC_MTP_WMDRMPD_GetLicenseState      0x9109
#define PTP_OC_MTP_WMDRMPD_SendWMDRMPDCommand   0x910A
#define PTP_OC_MTP_WMDRMPD_SendWMDRMPDRequest   0x910B
/*
 * Windows媒体数字权限管理的便携式设备扩展代码(microsoft.com/WMDRMPD: 10.1)
 * 以下是没有与“供应商定义的命令代码”相关联的公共文档标识符的操作
 */
#define PTP_OC_MTP_WMDRMPD_SendWMDRMPDAppRequest    0x9212
#define PTP_OC_MTP_WMDRMPD_GetWMDRMPDAppResponse    0x9213
#define PTP_OC_MTP_WMDRMPD_EnableTrustedFilesOperations 0x9214
#define PTP_OC_MTP_WMDRMPD_DisableTrustedFilesOperations 0x9215
#define PTP_OC_MTP_WMDRMPD_EndTrustedAppSession     0x9216
/* ^^^ guess ^^^ */

/*
 * Microsoft 高级音频/视频传输扩展(microsoft.com/AAVT: 1.0)
 */
#define PTP_OC_MTP_AAVT_OpenMediaSession        0x9170
#define PTP_OC_MTP_AAVT_CloseMediaSession       0x9171
#define PTP_OC_MTP_AAVT_GetNextDataBlock        0x9172
#define PTP_OC_MTP_AAVT_SetCurrentTimePosition      0x9173

/*
 * Windows媒体数字权限管理的网络设备代码 (microsoft.com/WMDRMND: 1.0) MTP/IP?
 */
#define PTP_OC_MTP_WMDRMND_SendRegistrationRequest  0x9180
#define PTP_OC_MTP_WMDRMND_GetRegistrationResponse  0x9181
#define PTP_OC_MTP_WMDRMND_GetProximityChallenge    0x9182
#define PTP_OC_MTP_WMDRMND_SendProximityResponse    0x9183
#define PTP_OC_MTP_WMDRMND_SendWMDRMNDLicenseRequest    0x9184
#define PTP_OC_MTP_WMDRMND_GetWMDRMNDLicenseResponse    0x9185

/*
 * Windows 媒体播放器可移植设备扩展代码(microsoft.com/WMPPD: 11.1)
 */
#define PTP_OC_MTP_WMPPD_ReportAddedDeletedItems    0x9201
#define PTP_OC_MTP_WMPPD_ReportAcquiredItems            0x9202
#define PTP_OC_MTP_WMPPD_PlaylistObjectPref     0x9203

/*
 * 未记录的Zune操作代码
 * 可能与WMPPD扩展集有关？
 */
#define PTP_OC_MTP_ZUNE_GETUNDEFINED001             0x9204

/* WiFi 配置MTP扩展代码 (microsoft.com/WPDWCN: 1.0) */
#define PTP_OC_MTP_WPDWCN_ProcessWFCObject      0x9122

/* Android Random I/O Extensions Codes */
#define PTP_OC_ANDROID_GetPartialObject64       0x95C1
#define PTP_OC_ANDROID_SendPartialObject        0x95C2
#define PTP_OC_ANDROID_TruncateObject           0x95C3
#define PTP_OC_ANDROID_BeginEditObject          0x95C4
#define PTP_OC_ANDROID_EndEditObject            0x95C5

#define PTP_OC_PARROT_GetSunshineValues     0x9201
#define PTP_OC_PARROT_GetTemperatureValues  0x9202
#define PTP_OC_PARROT_GetAngleValues        0x9203
#define PTP_OC_PARROT_GetGpsValues      0x9204
#define PTP_OC_PARROT_GetGyroscopeValues    0x9205
#define PTP_OC_PARROT_GetAccelerometerValues    0x9206
#define PTP_OC_PARROT_GetMagnetometerValues 0x9207
#define PTP_OC_PARROT_GetImuValues      0x9208
#define PTP_OC_PARROT_GetStatusMask     0x9209
#define PTP_OC_PARROT_EjectStorage      0x920A
#define PTP_OC_PARROT_StartMagnetoCalib     0x9210
#define PTP_OC_PARROT_StopMagnetoCalib      0x9211
#define PTP_OC_PARROT_MagnetoCalibStatus    0x9212
#define PTP_OC_PARROT_SendFirmwareUpdate    0x9213

#define PTP_OC_PANASONIC_9101           0x9101
#define PTP_OC_PANASONIC_OpenSession        0x9102  /* opensession (1 arg, seems to be storage id 0x00010001)*/
#define PTP_OC_PANASONIC_CloseSession       0x9103  /* closesession (no arg) */
#define PTP_OC_PANASONIC_9104           0x9104  /* get ext device id (1 arg?) */

/* 响应代码 */

/* PTP v1.0 响应代码*/
#define PTP_RC_Undefined                0x2000
#define PTP_RC_OK                       0x2001
#define PTP_RC_GeneralError             0x2002
#define PTP_RC_SessionNotOpen           0x2003
#define PTP_RC_InvalidTransactionID     0x2004
#define PTP_RC_OperationNotSupported    0x2005
#define PTP_RC_ParameterNotSupported    0x2006
#define PTP_RC_IncompleteTransfer       0x2007
#define PTP_RC_InvalidStorageId         0x2008
#define PTP_RC_InvalidObjectHandle      0x2009
#define PTP_RC_DevicePropNotSupported   0x200A
#define PTP_RC_InvalidObjectFormatCode  0x200B
#define PTP_RC_StoreFull                0x200C
#define PTP_RC_ObjectWriteProtected     0x200D
#define PTP_RC_StoreReadOnly            0x200E
#define PTP_RC_AccessDenied             0x200F
#define PTP_RC_NoThumbnailPresent       0x2010
#define PTP_RC_SelfTestFailed           0x2011
#define PTP_RC_PartialDeletion          0x2012
#define PTP_RC_StoreNotAvailable        0x2013
#define PTP_RC_SpecificationByFormatUnsupported         0x2014
#define PTP_RC_NoValidObjectInfo        0x2015
#define PTP_RC_InvalidCodeFormat        0x2016
#define PTP_RC_UnknownVendorCode        0x2017
#define PTP_RC_CaptureAlreadyTerminated 0x2018
#define PTP_RC_DeviceBusy               0x2019
#define PTP_RC_InvalidParentObject      0x201A
#define PTP_RC_InvalidDevicePropFormat  0x201B
#define PTP_RC_InvalidDevicePropValue   0x201C
#define PTP_RC_InvalidParameter         0x201D
#define PTP_RC_SessionAlreadyOpened     0x201E
#define PTP_RC_TransactionCanceled      0x201F
#define PTP_RC_SpecificationOfDestinationUnsupported            0x2020
/* PTP v1.1 响应代码 */
#define PTP_RC_InvalidEnumHandle    0x2021
#define PTP_RC_NoStreamEnabled      0x2022
#define PTP_RC_InvalidDataSet       0x2023

/* Eastman Kodak 扩展响应代码 */
#define PTP_RC_EK_FilenameRequired  0xA001
#define PTP_RC_EK_FilenameConflicts 0xA002
#define PTP_RC_EK_FilenameInvalid   0xA003

/* Nikon 特定响应代码 */
#define PTP_RC_NIKON_HardwareError      0xA001
#define PTP_RC_NIKON_OutOfFocus         0xA002
#define PTP_RC_NIKON_ChangeCameraModeFailed 0xA003
#define PTP_RC_NIKON_InvalidStatus      0xA004
#define PTP_RC_NIKON_SetPropertyNotSupported    0xA005
#define PTP_RC_NIKON_WbResetError       0xA006
#define PTP_RC_NIKON_DustReferenceError     0xA007
#define PTP_RC_NIKON_ShutterSpeedBulb       0xA008
#define PTP_RC_NIKON_MirrorUpSequence       0xA009
#define PTP_RC_NIKON_CameraModeNotAdjustFNumber 0xA00A
#define PTP_RC_NIKON_NotLiveView        0xA00B
#define PTP_RC_NIKON_MfDriveStepEnd     0xA00C
#define PTP_RC_NIKON_MfDriveStepInsufficiency   0xA00E
#define PTP_RC_NIKON_AdvancedTransferCancel 0xA022

/* Canon 特定响应代码 */
#define PTP_RC_CANON_UNKNOWN_COMMAND        0xA001
#define PTP_RC_CANON_OPERATION_REFUSED      0xA005
#define PTP_RC_CANON_LENS_COVER         0xA006
#define PTP_RC_CANON_BATTERY_LOW        0xA101
#define PTP_RC_CANON_NOT_READY          0xA102

#define PTP_RC_CANON_A009           0xA009

#define PTP_RC_CANON_EOS_UnknownCommand     0xA001
#define PTP_RC_CANON_EOS_OperationRefused   0xA005
#define PTP_RC_CANON_EOS_LensCoverClosed    0xA006
#define PTP_RC_CANON_EOS_LowBattery     0xA101
#define PTP_RC_CANON_EOS_ObjectNotReady     0xA102
#define PTP_RC_CANON_EOS_CannotMakeObject   0xA104
#define PTP_RC_CANON_EOS_MemoryStatusNotReady   0xA106


/* Microsoft/MTP 特定代码 */
#define PTP_RC_MTP_Undefined            0xA800
#define PTP_RC_MTP_Invalid_ObjectPropCode   0xA801
#define PTP_RC_MTP_Invalid_ObjectProp_Format    0xA802
#define PTP_RC_MTP_Invalid_ObjectProp_Value 0xA803
#define PTP_RC_MTP_Invalid_ObjectReference  0xA804
#define PTP_RC_MTP_Invalid_Dataset      0xA806
#define PTP_RC_MTP_Specification_By_Group_Unsupported       0xA807
#define PTP_RC_MTP_Specification_By_Depth_Unsupported       0xA808
#define PTP_RC_MTP_Object_Too_Large     0xA809
#define PTP_RC_MTP_ObjectProp_Not_Supported 0xA80A

/* Microsoft 高级音频 /视频传输响应代码
 * (microsoft.com/AAVTAdvanced Audio/Video Transfer) 1.0) */
#define PTP_RC_MTP_Invalid_Media_Session_ID 0xA170
#define PTP_RC_MTP_Media_Session_Limit_Reached  0xA171
#define PTP_RC_MTP_No_More_Data         0xA172

/* WiFi 网络配置 MTP扩展 错误代码 (microsoft.com/WPDWCN: 1.0) */
#define PTP_RC_MTP_Invalid_WFC_Syntax       0xA121
#define PTP_RC_MTP_WFC_Version_Not_Supported    0xA122

/* libptp2 扩展错误代码 */
#define PTP_ERROR_NODEVICE      0x02F9
#define PTP_ERROR_TIMEOUT       0x02FA
#define PTP_ERROR_CANCEL        0x02FB
#define PTP_ERROR_BADPARAM      0x02FC
#define PTP_ERROR_RESP_EXPECTED     0x02FD
#define PTP_ERROR_DATA_EXPECTED     0x02FE
#define PTP_ERROR_IO            0x02FF

/* PTP 事件代码 */

#define PTP_EC_Undefined        0x4000
#define PTP_EC_CancelTransaction    0x4001
#define PTP_EC_ObjectAdded      0x4002
#define PTP_EC_ObjectRemoved        0x4003
#define PTP_EC_StoreAdded       0x4004
#define PTP_EC_StoreRemoved     0x4005
#define PTP_EC_DevicePropChanged    0x4006
#define PTP_EC_ObjectInfoChanged    0x4007
#define PTP_EC_DeviceInfoChanged    0x4008
#define PTP_EC_RequestObjectTransfer    0x4009
#define PTP_EC_StoreFull        0x400A
#define PTP_EC_DeviceReset      0x400B
#define PTP_EC_StorageInfoChanged   0x400C
#define PTP_EC_CaptureComplete      0x400D
#define PTP_EC_UnreportedStatus     0x400E

/* Canon 扩展事件代码 */
#define PTP_EC_CANON_ExtendedErrorcode      0xC005  /* ? */
#define PTP_EC_CANON_ObjectInfoChanged      0xC008
#define PTP_EC_CANON_RequestObjectTransfer  0xC009
#define PTP_EC_CANON_ShutterButtonPressed0  0xC00B
#define PTP_EC_CANON_CameraModeChanged      0xC00C
#define PTP_EC_CANON_ShutterButtonPressed1  0xC00E

#define PTP_EC_CANON_StartDirectTransfer    0xC011
#define PTP_EC_CANON_StopDirectTransfer     0xC013

#define PTP_EC_CANON_TranscodeProgress      0xC01B /* EOS ? */

/* Canon EOS 事件*/
#define PTP_EC_CANON_EOS_RequestGetEvent        0xc101
#define PTP_EC_CANON_EOS_ObjectAddedEx          0xc181
#define PTP_EC_CANON_EOS_ObjectRemoved          0xc182
#define PTP_EC_CANON_EOS_RequestGetObjectInfoEx     0xc183
#define PTP_EC_CANON_EOS_StorageStatusChanged       0xc184
#define PTP_EC_CANON_EOS_StorageInfoChanged     0xc185
#define PTP_EC_CANON_EOS_RequestObjectTransfer      0xc186
#define PTP_EC_CANON_EOS_ObjectInfoChangedEx        0xc187
#define PTP_EC_CANON_EOS_ObjectContentChanged       0xc188
#define PTP_EC_CANON_EOS_PropValueChanged       0xc189
#define PTP_EC_CANON_EOS_AvailListChanged       0xc18a
#define PTP_EC_CANON_EOS_CameraStatusChanged        0xc18b
#define PTP_EC_CANON_EOS_WillSoonShutdown       0xc18d
#define PTP_EC_CANON_EOS_ShutdownTimerUpdated       0xc18e
#define PTP_EC_CANON_EOS_RequestCancelTransfer      0xc18f
#define PTP_EC_CANON_EOS_RequestObjectTransferDT    0xc190
#define PTP_EC_CANON_EOS_RequestCancelTransferDT    0xc191
#define PTP_EC_CANON_EOS_StoreAdded         0xc192
#define PTP_EC_CANON_EOS_StoreRemoved           0xc193
#define PTP_EC_CANON_EOS_BulbExposureTime       0xc194
#define PTP_EC_CANON_EOS_RecordingTime          0xc195
#define PTP_EC_CANON_EOS_InnerDevelopParam      0xc196
#define PTP_EC_CANON_EOS_RequestObjectTransferDevelop   0xc197
#define PTP_EC_CANON_EOS_GPSLogOutputProgress       0xc198
#define PTP_EC_CANON_EOS_GPSLogOutputComplete       0xc199
#define PTP_EC_CANON_EOS_TouchTrans         0xc19a
#define PTP_EC_CANON_EOS_RequestObjectTransferExInfo    0xc19b
#define PTP_EC_CANON_EOS_PowerZoomInfoChanged       0xc19d
#define PTP_EC_CANON_EOS_RequestPushMode        0xc19f
#define PTP_EC_CANON_EOS_RequestObjectTransferTS    0xc1a2
#define PTP_EC_CANON_EOS_AfResult           0xc1a3
#define PTP_EC_CANON_EOS_CTGInfoCheckComplete       0xc1a4
#define PTP_EC_CANON_EOS_OLCInfoChanged         0xc1a5
#define PTP_EC_CANON_EOS_ObjectAddedEx64        0xc1a7
#define PTP_EC_CANON_EOS_ObjectInfoChangedEx64      0xc1a8
#define PTP_EC_CANON_EOS_RequestObjectTransfer64    0xc1a9
#define PTP_EC_CANON_EOS_RequestObjectTransferDT64  0xc1aa
#define PTP_EC_CANON_EOS_RequestObjectTransferFTP64 0xc1ab
#define PTP_EC_CANON_EOS_RequestObjectTransferInfoEx64  0xc1ac
#define PTP_EC_CANON_EOS_RequestObjectTransferMA64  0xc1ad
#define PTP_EC_CANON_EOS_ImportError            0xc1af
#define PTP_EC_CANON_EOS_BlePairing         0xc1b0
#define PTP_EC_CANON_EOS_RequestAutoSendImages      0xc1b1
#define PTP_EC_CANON_EOS_RequestObjectTransferFTP   0xc1f1

/* Nikon 扩展事件代码 */

/* Nikon 扩展事件代码 */
#define PTP_EC_Nikon_ObjectAddedInSDRAM     0xC101
#define PTP_EC_Nikon_CaptureCompleteRecInSdram  0xC102
/* Gets 1 parameter, objectid pointing to DPOF object */
#define PTP_EC_Nikon_AdvancedTransfer       0xC103
#define PTP_EC_Nikon_PreviewImageAdded      0xC104

/* Olympus E series */
#define PTP_EC_Olympus_PropertyChanged      0xC102
#define PTP_EC_Olympus_CaptureComplete      0xC103

/* Sony */
#define PTP_EC_Sony_ObjectAdded         0xC201
#define PTP_EC_Sony_ObjectRemoved       0xC202
#define PTP_EC_Sony_PropertyChanged     0xC203

/* MTP 时间代码 */
#define PTP_EC_MTP_ObjectPropChanged        0xC801
#define PTP_EC_MTP_ObjectPropDescChanged    0xC802
#define PTP_EC_MTP_ObjectReferencesChanged  0xC803

#define PTP_EC_PARROT_Status            0xC201
#define PTP_EC_PARROT_MagnetoCalibrationStatus  0xC202

#define PTP_EC_PANASONIC_ObjectAdded        0xC108
#define PTP_EC_PANASONIC_ObjectAddedSDRAM   0xC109


/* GetObjectHandles 常数*/
#define PTP_GOH_ALL_STORAGE 0xffffffff
#define PTP_GOH_ALL_FORMATS 0x00000000
#define PTP_GOH_ALL_ASSOCS  0x00000000
#define PTP_GOH_ROOT_PARENT 0xffffffff

/* PTP 设备信息结构体 (由 GetDevInfo返回) */
struct _PTPDeviceInfo {
    u16 StandardVersion;
    u32 VendorExtensionID;
    u16 VendorExtensionVersion;
    char    *VendorExtensionDesc;
    u16 FunctionalMode;
    u32 OperationsSupported_len;
    u16 *OperationsSupported;
    u32 EventsSupported_len;
    u16 *EventsSupported;
    u32 DevicePropertiesSupported_len;
    u16 *DevicePropertiesSupported;
    u32 CaptureFormats_len;
    u16 *CaptureFormats;
    u32 ImageFormats_len;
    u16 *ImageFormats;
    char    *Manufacturer;
    char    *Model;
    char    *DeviceVersion;
    char    *SerialNumber;
};
typedef struct _PTPDeviceInfo PTPDeviceInfo;

/* PTP 存储ID结构体 (returned by GetStorageIDs) */
struct _PTPStorageIDs {
    u32 n;
    u32 *Storage;
};
typedef struct _PTPStorageIDs PTPStorageIDs;

/* PTP 存储信息结构体 (returned by GetStorageInfo) */
struct _PTPStorageInfo {
    u16 StorageType;
    u16 FilesystemType;
    u16 AccessCapability;
    u64 MaxCapability;
    u64 FreeSpaceInBytes;
    u32 FreeSpaceInImages;
    char    *StorageDescription;
    char    *VolumeLabel;
};
typedef struct _PTPStorageInfo PTPStorageInfo;

/* PTP对象句柄*/
struct _PTPObjectHandles {
    u32 n;           /* 此对象下的对象的数目*/
    u32 *Handler;    /* 促对象下的对象*/
};
typedef struct _PTPObjectHandles PTPObjectHandles;

/* PTP 对象信息结构体 (由 GetObjectInfo返回) */
struct _PTPObjectInfo {
    u32 StorageID;
    u16 ObjectFormat;
    u16 ProtectionStatus;
    /* 在常规的objectinfo中这是32位的，但我们在这里保留了一般的对象大小
     * 它是通过其他的methods到达的，所以使用64位*/
    u64 ObjectCompressedSize;
    u16 ThumbFormat;
    u32 ThumbCompressedSize;
    u32 ThumbPixWidth;
    u32 ThumbPixHeight;
    u32 ImagePixWidth;
    u32 ImagePixHeight;
    u32 ImageBitDepth;
    u32 ParentObject;
    u16 AssociationType;
    u32 AssociationDesc;
    u32 SequenceNumber;
    char    *Filename;
    time_t  CaptureDate;
    time_t  ModificationDate;
    char    *Keywords;
};
typedef struct _PTPObjectInfo PTPObjectInfo;

/* 最大MTP字符串长度，包括终止空字符 */
#define PTP_MAXSTRLEN               255

/* PTP(图片传输协议)对象格式代码*/

/* 辅助格式*/
#define PTP_OFC_Undefined           0x3000
#define PTP_OFC_Defined             0x3800
#define PTP_OFC_Association         0x3001     /* 目录*/
#define PTP_OFC_Script              0x3002
#define PTP_OFC_Executable          0x3003
#define PTP_OFC_Text                0x3004
#define PTP_OFC_HTML                0x3005
#define PTP_OFC_DPOF                0x3006
#define PTP_OFC_AIFF                0x3007
#define PTP_OFC_WAV             0x3008
#define PTP_OFC_MP3             0x3009
#define PTP_OFC_AVI             0x300A
#define PTP_OFC_MPEG                0x300B
#define PTP_OFC_ASF             0x300C
#define PTP_OFC_QT              0x300D /* guessing */
/* 图片格式*/
#define PTP_OFC_EXIF_JPEG           0x3801
#define PTP_OFC_TIFF_EP             0x3802
#define PTP_OFC_FlashPix            0x3803
#define PTP_OFC_BMP             0x3804
#define PTP_OFC_CIFF                0x3805
#define PTP_OFC_Undefined_0x3806        0x3806
#define PTP_OFC_GIF             0x3807
#define PTP_OFC_JFIF                0x3808
#define PTP_OFC_PCD             0x3809
#define PTP_OFC_PICT                0x380A
#define PTP_OFC_PNG             0x380B
#define PTP_OFC_Undefined_0x380C        0x380C
#define PTP_OFC_TIFF                0x380D
#define PTP_OFC_TIFF_IT             0x380E
#define PTP_OFC_JP2             0x380F
#define PTP_OFC_JPX             0x3810
/* ptp v1.1 has only DNG new */
#define PTP_OFC_DNG             0x3811
/* Eastman Kodak 扩展辅助类型 */
#define PTP_OFC_EK_M3U              0xb002
/* 佳能 扩展*/
#define PTP_OFC_CANON_CRW           0xb101
#define PTP_OFC_CANON_CRW3          0xb103
#define PTP_OFC_CANON_MOV           0xb104
#define PTP_OFC_CANON_MOV2          0xb105
#define PTP_OFC_CANON_CR3           0xb108
/* CHDK 特定原始模式 */
#define PTP_OFC_CANON_CHDK_CRW          0xb1ff
/* 索尼 */
#define PTP_OFC_SONY_RAW            0xb101
/* MTP 扩展 */
#define PTP_OFC_MTP_MediaCard           0xb211
#define PTP_OFC_MTP_MediaCardGroup      0xb212
#define PTP_OFC_MTP_Encounter           0xb213
#define PTP_OFC_MTP_EncounterBox        0xb214
#define PTP_OFC_MTP_M4A             0xb215
#define PTP_OFC_MTP_ZUNEUNDEFINED       0xb217 /* 未知文件类型 */
#define PTP_OFC_MTP_Firmware            0xb802
#define PTP_OFC_MTP_WindowsImageFormat      0xb881
#define PTP_OFC_MTP_UndefinedAudio      0xb900
#define PTP_OFC_MTP_WMA             0xb901
#define PTP_OFC_MTP_OGG             0xb902
#define PTP_OFC_MTP_AAC             0xb903
#define PTP_OFC_MTP_AudibleCodec        0xb904
#define PTP_OFC_MTP_FLAC            0xb906
#define PTP_OFC_MTP_SamsungPlaylist     0xb909
#define PTP_OFC_MTP_UndefinedVideo      0xb980
#define PTP_OFC_MTP_WMV             0xb981
#define PTP_OFC_MTP_MP4             0xb982
#define PTP_OFC_MTP_MP2             0xb983
#define PTP_OFC_MTP_3GP             0xb984
#define PTP_OFC_MTP_UndefinedCollection     0xba00
#define PTP_OFC_MTP_AbstractMultimediaAlbum 0xba01
#define PTP_OFC_MTP_AbstractImageAlbum      0xba02
#define PTP_OFC_MTP_AbstractAudioAlbum      0xba03
#define PTP_OFC_MTP_AbstractVideoAlbum      0xba04
#define PTP_OFC_MTP_AbstractAudioVideoPlaylist  0xba05
#define PTP_OFC_MTP_AbstractContactGroup    0xba06
#define PTP_OFC_MTP_AbstractMessageFolder   0xba07
#define PTP_OFC_MTP_AbstractChapteredProduction 0xba08
#define PTP_OFC_MTP_AbstractAudioPlaylist   0xba09
#define PTP_OFC_MTP_AbstractVideoPlaylist   0xba0a
#define PTP_OFC_MTP_AbstractMediacast       0xba0b
#define PTP_OFC_MTP_WPLPlaylist         0xba10
#define PTP_OFC_MTP_M3UPlaylist         0xba11
#define PTP_OFC_MTP_MPLPlaylist         0xba12
#define PTP_OFC_MTP_ASXPlaylist         0xba13
#define PTP_OFC_MTP_PLSPlaylist         0xba14
#define PTP_OFC_MTP_UndefinedDocument       0xba80
#define PTP_OFC_MTP_AbstractDocument        0xba81
#define PTP_OFC_MTP_XMLDocument         0xba82
#define PTP_OFC_MTP_MSWordDocument      0xba83
#define PTP_OFC_MTP_MHTCompiledHTMLDocument 0xba84
#define PTP_OFC_MTP_MSExcelSpreadsheetXLS   0xba85
#define PTP_OFC_MTP_MSPowerpointPresentationPPT 0xba86
#define PTP_OFC_MTP_UndefinedMessage        0xbb00
#define PTP_OFC_MTP_AbstractMessage     0xbb01
#define PTP_OFC_MTP_UndefinedContact        0xbb80
#define PTP_OFC_MTP_AbstractContact     0xbb81
#define PTP_OFC_MTP_vCard2          0xbb82
#define PTP_OFC_MTP_vCard3          0xbb83
#define PTP_OFC_MTP_UndefinedCalendarItem   0xbe00
#define PTP_OFC_MTP_AbstractCalendarItem    0xbe01
#define PTP_OFC_MTP_vCalendar1          0xbe02
#define PTP_OFC_MTP_vCalendar2          0xbe03
#define PTP_OFC_MTP_UndefinedWindowsExecutable  0xbe80
#define PTP_OFC_MTP_MediaCast           0xbe81
#define PTP_OFC_MTP_Section         0xbe82

/* PTP 关联类型 */
#define PTP_AT_Undefined            0x0000
#define PTP_AT_GenericFolder            0x0001
#define PTP_AT_Album                0x0002
#define PTP_AT_TimeSequence         0x0003
#define PTP_AT_HorizontalPanoramic      0x0004
#define PTP_AT_VerticalPanoramic        0x0005
#define PTP_AT_2DPanoramic          0x0006
#define PTP_AT_AncillaryData            0x0007

/* PTP 保护状态 */

#define PTP_PS_NoProtection         0x0000
#define PTP_PS_ReadOnly             0x0001
#define PTP_PS_MTP_ReadOnlyData         0x8002
#define PTP_PS_MTP_NonTransferableData      0x8003

/* PTP 存储类型 */

#define PTP_ST_Undefined            0x0000
#define PTP_ST_FixedROM             0x0001
#define PTP_ST_RemovableROM         0x0002
#define PTP_ST_FixedRAM             0x0003
#define PTP_ST_RemovableRAM         0x0004

/* PTP 文件系统类型值 */

#define PTP_FST_Undefined           0x0000
#define PTP_FST_GenericFlat         0x0001
#define PTP_FST_GenericHierarchical     0x0002
#define PTP_FST_DCF             0x0003

/* PTP 存储信息访问功能值 */

#define PTP_AC_ReadWrite            0x0000
#define PTP_AC_ReadOnly             0x0001
#define PTP_AC_ReadOnly_with_Object_Deletion    0x0002

/* PTP属性值*/
union _PTPPropertyValue {
    char        *str;   /* 通用字符串, 被分配 */
    u8         u8;
    s8         i8;
    u16        u16;
    s16        i16;
    u32        u32;
    s32        i32;
    u64        u64;
    s64        i64;
    /* XXXX: 128位的符号和无符号丢失 */
    struct array {
        u32    count;
        union _PTPPropertyValue *v; /* 被分配, 元素计数 */
    } a;
};
typedef union _PTPPropertyValue PTPPropertyValue;

/* MTP操作的元数据列表 */
struct _MTPProperties {
    u16        property;          /* 属性*/
    u16        datatype;          /* 数据类型*/
    u32        ObjectHandle;      /* 对象句柄*/
    PTPPropertyValue    propval;  /* 属性值*/
};
typedef struct _MTPProperties MTPProperties;

struct _PTPPropDescRangeForm {
    PTPPropertyValue    MinimumValue;
    PTPPropertyValue    MaximumValue;
    PTPPropertyValue    StepSize;
};
typedef struct _PTPPropDescRangeForm PTPPropDescRangeForm;

/* 描述数据集的设备属性 , 枚举形式 */
struct _PTPPropDescEnumForm {
    u16        NumberOfValues;
    PTPPropertyValue    *SupportedValue;    /* 被分配*/
};
typedef struct _PTPPropDescEnumForm PTPPropDescEnumForm;

/* 描述数据集的设备属性 (DevicePropDesc) */
struct _PTPDevicePropDesc {
    u16        DevicePropertyCode;
    u16        DataType;
    u8         GetSet;
    PTPPropertyValue    FactoryDefaultValue;
    PTPPropertyValue    CurrentValue;
    u8         FormFlag;
    union   {
        PTPPropDescEnumForm Enum;
        PTPPropDescRangeForm    Range;
    } FORM;
};
typedef struct _PTPDevicePropDesc PTPDevicePropDesc;

struct _PTPObjectPropDesc {
    u16        ObjectPropertyCode;
    u16        DataType;
    u8         GetSet;
    PTPPropertyValue    FactoryDefaultValue;
    u32        GroupCode;
    u8         FormFlag;
    union   {
        PTPPropDescEnumForm Enum;
        PTPPropDescRangeForm    Range;
    } FORM;
};
typedef struct _PTPObjectPropDesc PTPObjectPropDesc;

/* Canon filesystem's folder entry Dataset */

#define PTP_CANON_FilenameBufferLen 13
#define PTP_CANON_FolderEntryLen    28

struct _PTPCANONFolderEntry {
    u32    ObjectHandle;
    u16    ObjectFormatCode;
    u8     Flags;
    u32    ObjectSize;
    time_t      Time;
    char        Filename[PTP_CANON_FilenameBufferLen];

    u32    StorageID;
};
typedef struct _PTPCANONFolderEntry PTPCANONFolderEntry;



/* Nikon Wifi 配置文件 */
struct _PTPNIKONWifiProfile {
    /* 值在读写配置文件时都有效 */
    char      profile_name[17];
    u8   device_type;
    u8   icon_type;
    char      essid[33];

    /* 值只在读的时候有效。有些在写包里，但会被自动设置，例如id，display_order和creation_date*/
    u8   id;
    u8   valid;
    u8   display_order;
    char      creation_date[16];
    char      lastusage_date[16];

    /* 值只在写的时候有效 */
    u32  ip_address;
    u8   subnet_mask; /* 第一个0位位置，例如24代表255.255.255.0 */
    u32  gateway_address;
    u8   address_mode; /* 0 - 手动, 2-3 -  DHCP ad-hoc/管理*/
    u8   access_mode; /* 0 - 手, 1 - Adhoc */
    u8   wifi_channel; /* 1-11 */
    u8   authentification; /* 0 - 开放, 1 - 共享, 2 - WPA-PSK */
    u8   encryption; /* 0 - None, 1 - WEP 64bit, 2 - WEP 128bit (not supported: 3 - TKIP) */
    u8   key[64];
    u8   key_nr;
/*  char      guid[16]; */
};

typedef struct _PTPNIKONWifiProfile PTPNIKONWifiProfile;
enum _PTPCanon_changes_types {
    PTP_CANON_EOS_CHANGES_TYPE_UNKNOWN,
    PTP_CANON_EOS_CHANGES_TYPE_OBJECTINFO,
    PTP_CANON_EOS_CHANGES_TYPE_OBJECTTRANSFER,
    PTP_CANON_EOS_CHANGES_TYPE_PROPERTY,
    PTP_CANON_EOS_CHANGES_TYPE_CAMERASTATUS,
    PTP_CANON_EOS_CHANGES_TYPE_FOCUSINFO,
    PTP_CANON_EOS_CHANGES_TYPE_FOCUSMASK,
    PTP_CANON_EOS_CHANGES_TYPE_OBJECTREMOVED,
    PTP_CANON_EOS_CHANGES_TYPE_OBJECTINFO_CHANGE,
    PTP_CANON_EOS_CHANGES_TYPE_OBJECTCONTENT_CHANGE
};

struct _PTPCanon_New_Object {
    u32    oid;
    PTPObjectInfo   oi;
};

struct _PTPCanon_changes_entry {
    enum _PTPCanon_changes_types    type;
    union {
        struct _PTPCanon_New_Object object; /* TYPE_OBJECTINFO */
        char                *info;
        u16            propid;
        int             status;
    } u;
};
typedef struct _PTPCanon_changes_entry PTPCanon_changes_entry;

typedef struct _PTPCanon_Property {
    u32        size;
    u32        proptype;
    unsigned char       *data;

    /* 填写查询 */
    PTPDevicePropDesc   dpd;
} PTPCanon_Property;


/* 数据类型代码 */
#define PTP_DTC_UNDEF       0x0000
#define PTP_DTC_INT8        0x0001
#define PTP_DTC_UINT8       0x0002
#define PTP_DTC_INT16       0x0003
#define PTP_DTC_UINT16      0x0004
#define PTP_DTC_INT32       0x0005
#define PTP_DTC_UINT32      0x0006
#define PTP_DTC_INT64       0x0007
#define PTP_DTC_UINT64      0x0008
#define PTP_DTC_INT128      0x0009
#define PTP_DTC_UINT128     0x000A

#define PTP_DTC_ARRAY_MASK  0x4000

#define PTP_DTC_AINT8       (PTP_DTC_ARRAY_MASK | PTP_DTC_INT8)
#define PTP_DTC_AUINT8      (PTP_DTC_ARRAY_MASK | PTP_DTC_UINT8)
#define PTP_DTC_AINT16      (PTP_DTC_ARRAY_MASK | PTP_DTC_INT16)
#define PTP_DTC_AUINT16     (PTP_DTC_ARRAY_MASK | PTP_DTC_UINT16)
#define PTP_DTC_AINT32      (PTP_DTC_ARRAY_MASK | PTP_DTC_INT32)
#define PTP_DTC_AUINT32     (PTP_DTC_ARRAY_MASK | PTP_DTC_UINT32)
#define PTP_DTC_AINT64      (PTP_DTC_ARRAY_MASK | PTP_DTC_INT64)
#define PTP_DTC_AUINT64     (PTP_DTC_ARRAY_MASK | PTP_DTC_UINT64)
#define PTP_DTC_AINT128     (PTP_DTC_ARRAY_MASK | PTP_DTC_INT128)
#define PTP_DTC_AUINT128    (PTP_DTC_ARRAY_MASK | PTP_DTC_UINT128)

#define PTP_DTC_STR     0xFFFF

/* 设备属性代码 */

/* PTP v1.0 属性代码*/
#define PTP_DPC_Undefined       0x5000
#define PTP_DPC_BatteryLevel        0x5001
#define PTP_DPC_FunctionalMode      0x5002
#define PTP_DPC_ImageSize       0x5003
#define PTP_DPC_CompressionSetting  0x5004
#define PTP_DPC_WhiteBalance        0x5005
#define PTP_DPC_RGBGain         0x5006
#define PTP_DPC_FNumber         0x5007
#define PTP_DPC_FocalLength     0x5008
#define PTP_DPC_FocusDistance       0x5009
#define PTP_DPC_FocusMode       0x500A
#define PTP_DPC_ExposureMeteringMode    0x500B
#define PTP_DPC_FlashMode       0x500C
#define PTP_DPC_ExposureTime        0x500D
#define PTP_DPC_ExposureProgramMode 0x500E
#define PTP_DPC_ExposureIndex       0x500F
#define PTP_DPC_ExposureBiasCompensation    0x5010
#define PTP_DPC_DateTime        0x5011
#define PTP_DPC_CaptureDelay        0x5012
#define PTP_DPC_StillCaptureMode    0x5013
#define PTP_DPC_Contrast        0x5014
#define PTP_DPC_Sharpness       0x5015
#define PTP_DPC_DigitalZoom     0x5016
#define PTP_DPC_EffectMode      0x5017
#define PTP_DPC_BurstNumber     0x5018
#define PTP_DPC_BurstInterval       0x5019
#define PTP_DPC_TimelapseNumber     0x501A
#define PTP_DPC_TimelapseInterval   0x501B
#define PTP_DPC_FocusMeteringMode   0x501C
#define PTP_DPC_UploadURL       0x501D
#define PTP_DPC_Artist          0x501E
#define PTP_DPC_CopyrightInfo       0x501F
/* PTP v1.1 属性代码 */
#define PTP_DPC_SupportedStreams    0x5020
#define PTP_DPC_EnabledStreams      0x5021
#define PTP_DPC_VideoFormat     0x5022
#define PTP_DPC_VideoResolution     0x5023
#define PTP_DPC_VideoQuality        0x5024
#define PTP_DPC_VideoFrameRate      0x5025
#define PTP_DPC_VideoContrast       0x5026
#define PTP_DPC_VideoBrightness     0x5027
#define PTP_DPC_AudioFormat     0x5028
#define PTP_DPC_AudioBitrate        0x5029
#define PTP_DPC_AudioSamplingRate   0x502A
#define PTP_DPC_AudioBitPerSample   0x502B
#define PTP_DPC_AudioVolume     0x502C

/* Proprietary vendor extension device property mask */
#define PTP_DPC_EXTENSION_MASK      0xF000
#define PTP_DPC_EXTENSION       0xD000

/* Zune extension 设备属性代码 */
#define PTP_DPC_MTP_ZUNE_UNKNOWN1   0xD181
#define PTP_DPC_MTP_ZUNE_UNKNOWN2   0xD132
#define PTP_DPC_MTP_ZUNE_UNKNOWN3   0xD215
#define PTP_DPC_MTP_ZUNE_UNKNOWN4   0xD216

/* Eastman Kodak extension 设备属性代码 */
#define PTP_DPC_EK_ColorTemperature 0xD001
#define PTP_DPC_EK_DateTimeStampFormat  0xD002
#define PTP_DPC_EK_BeepMode     0xD003
#define PTP_DPC_EK_VideoOut     0xD004
#define PTP_DPC_EK_PowerSaving      0xD005
#define PTP_DPC_EK_UI_Language      0xD006

/* Canon 扩展设备属性代码 */
#define PTP_DPC_CANON_BeepMode      0xD001
#define PTP_DPC_CANON_BatteryKind   0xD002
#define PTP_DPC_CANON_BatteryStatus 0xD003
#define PTP_DPC_CANON_UILockType    0xD004
#define PTP_DPC_CANON_CameraMode    0xD005
#define PTP_DPC_CANON_ImageQuality  0xD006
#define PTP_DPC_CANON_FullViewFileFormat 0xD007
#define PTP_DPC_CANON_ImageSize     0xD008
#define PTP_DPC_CANON_SelfTime      0xD009
#define PTP_DPC_CANON_FlashMode     0xD00A
#define PTP_DPC_CANON_Beep      0xD00B
#define PTP_DPC_CANON_ShootingMode  0xD00C
#define PTP_DPC_CANON_ImageMode     0xD00D
#define PTP_DPC_CANON_DriveMode     0xD00E
#define PTP_DPC_CANON_EZoom     0xD00F
#define PTP_DPC_CANON_MeteringMode  0xD010
#define PTP_DPC_CANON_AFDistance    0xD011
#define PTP_DPC_CANON_FocusingPoint 0xD012
#define PTP_DPC_CANON_WhiteBalance  0xD013
#define PTP_DPC_CANON_SlowShutterSetting    0xD014
#define PTP_DPC_CANON_AFMode        0xD015
#define PTP_DPC_CANON_ImageStabilization    0xD016
#define PTP_DPC_CANON_Contrast      0xD017
#define PTP_DPC_CANON_ColorGain     0xD018
#define PTP_DPC_CANON_Sharpness     0xD019
#define PTP_DPC_CANON_Sensitivity   0xD01A
#define PTP_DPC_CANON_ParameterSet  0xD01B
#define PTP_DPC_CANON_ISOSpeed      0xD01C
#define PTP_DPC_CANON_Aperture      0xD01D
#define PTP_DPC_CANON_ShutterSpeed  0xD01E
#define PTP_DPC_CANON_ExpCompensation   0xD01F
#define PTP_DPC_CANON_FlashCompensation 0xD020
#define PTP_DPC_CANON_AEBExposureCompensation   0xD021
#define PTP_DPC_CANON_AvOpen        0xD023
#define PTP_DPC_CANON_AvMax     0xD024
#define PTP_DPC_CANON_FocalLength   0xD025
#define PTP_DPC_CANON_FocalLengthTele   0xD026
#define PTP_DPC_CANON_FocalLengthWide   0xD027
#define PTP_DPC_CANON_FocalLengthDenominator    0xD028
#define PTP_DPC_CANON_CaptureTransferMode   0xD029
#define CANON_TRANSFER_ENTIRE_IMAGE_TO_PC   0x0002
#define CANON_TRANSFER_SAVE_THUMBNAIL_TO_DEVICE 0x0004
#define CANON_TRANSFER_SAVE_IMAGE_TO_DEVICE 0x0008
/* we use those values: */
#define CANON_TRANSFER_MEMORY       (2|1)
#define CANON_TRANSFER_CARD     (8|4|1)

#define PTP_DPC_CANON_Zoom      0xD02A
#define PTP_DPC_CANON_NamePrefix    0xD02B
#define PTP_DPC_CANON_SizeQualityMode   0xD02C
#define PTP_DPC_CANON_SupportedThumbSize    0xD02D
#define PTP_DPC_CANON_SizeOfOutputDataFromCamera    0xD02E
#define PTP_DPC_CANON_SizeOfInputDataToCamera       0xD02F
#define PTP_DPC_CANON_RemoteAPIVersion  0xD030
#define PTP_DPC_CANON_FirmwareVersion   0xD031
#define PTP_DPC_CANON_CameraModel   0xD032
#define PTP_DPC_CANON_CameraOwner   0xD033
#define PTP_DPC_CANON_UnixTime      0xD034
#define PTP_DPC_CANON_CameraBodyID  0xD035
#define PTP_DPC_CANON_CameraOutput  0xD036
#define PTP_DPC_CANON_DispAv        0xD037
#define PTP_DPC_CANON_AvOpenApex    0xD038
#define PTP_DPC_CANON_DZoomMagnification    0xD039
#define PTP_DPC_CANON_MlSpotPos     0xD03A
#define PTP_DPC_CANON_DispAvMax     0xD03B
#define PTP_DPC_CANON_AvMaxApex     0xD03C
#define PTP_DPC_CANON_EZoomStartPosition        0xD03D
#define PTP_DPC_CANON_FocalLengthOfTele 0xD03E
#define PTP_DPC_CANON_EZoomSizeOfTele   0xD03F
#define PTP_DPC_CANON_PhotoEffect   0xD040
#define PTP_DPC_CANON_AssistLight   0xD041
#define PTP_DPC_CANON_FlashQuantityCount    0xD042
#define PTP_DPC_CANON_RotationAngle 0xD043
#define PTP_DPC_CANON_RotationScene 0xD044
#define PTP_DPC_CANON_EventEmulateMode  0xD045
#define PTP_DPC_CANON_DPOFVersion   0xD046
#define PTP_DPC_CANON_TypeOfSupportedSlideShow  0xD047
#define PTP_DPC_CANON_AverageFilesizes  0xD048
#define PTP_DPC_CANON_ModelID       0xD049

#define PTP_DPC_CANON_EOS_PowerZoomPosition 0xD055
#define PTP_DPC_CANON_EOS_StrobeSettingSimple   0xD056
#define PTP_DPC_CANON_EOS_ConnectTrigger    0xD058
#define PTP_DPC_CANON_EOS_ChangeCameraMode  0xD059

/* From EOS 400D trace. */
#define PTP_DPC_CANON_EOS_Aperture      0xD101
#define PTP_DPC_CANON_EOS_ShutterSpeed      0xD102
#define PTP_DPC_CANON_EOS_ISOSpeed      0xD103
#define PTP_DPC_CANON_EOS_ExpCompensation   0xD104
#define PTP_DPC_CANON_EOS_AutoExposureMode  0xD105
#define PTP_DPC_CANON_EOS_DriveMode     0xD106
#define PTP_DPC_CANON_EOS_MeteringMode      0xD107
#define PTP_DPC_CANON_EOS_FocusMode     0xD108
#define PTP_DPC_CANON_EOS_WhiteBalance      0xD109
#define PTP_DPC_CANON_EOS_ColorTemperature  0xD10A
#define PTP_DPC_CANON_EOS_WhiteBalanceAdjustA   0xD10B
#define PTP_DPC_CANON_EOS_WhiteBalanceAdjustB   0xD10C
#define PTP_DPC_CANON_EOS_WhiteBalanceXA    0xD10D
#define PTP_DPC_CANON_EOS_WhiteBalanceXB    0xD10E
#define PTP_DPC_CANON_EOS_ColorSpace        0xD10F
#define PTP_DPC_CANON_EOS_PictureStyle      0xD110
#define PTP_DPC_CANON_EOS_BatteryPower      0xD111
#define PTP_DPC_CANON_EOS_BatterySelect     0xD112
#define PTP_DPC_CANON_EOS_CameraTime        0xD113
#define PTP_DPC_CANON_EOS_AutoPowerOff      0xD114
#define PTP_DPC_CANON_EOS_Owner         0xD115
#define PTP_DPC_CANON_EOS_ModelID       0xD116
#define PTP_DPC_CANON_EOS_PTPExtensionVersion   0xD119
#define PTP_DPC_CANON_EOS_DPOFVersion       0xD11A
#define PTP_DPC_CANON_EOS_AvailableShots    0xD11B
#define PTP_CANON_EOS_CAPTUREDEST_HD        4
#define PTP_DPC_CANON_EOS_CaptureDestination    0xD11C
#define PTP_DPC_CANON_EOS_BracketMode       0xD11D
#define PTP_DPC_CANON_EOS_CurrentStorage    0xD11E
#define PTP_DPC_CANON_EOS_CurrentFolder     0xD11F
#define PTP_DPC_CANON_EOS_ImageFormat       0xD120  /* file setting */
#define PTP_DPC_CANON_EOS_ImageFormatCF     0xD121  /* file setting CF */
#define PTP_DPC_CANON_EOS_ImageFormatSD     0xD122  /* file setting SD */
#define PTP_DPC_CANON_EOS_ImageFormatExtHD  0xD123  /* file setting exthd */
#define PTP_DPC_CANON_EOS_RefocusState      0xD124
#define PTP_DPC_CANON_EOS_CameraNickname    0xD125
#define PTP_DPC_CANON_EOS_ConnectStatus     0xD127
#define PTP_DPC_CANON_EOS_LV_AF_EyeDetect   0xD12C
#define PTP_DPC_CANON_EOS_AutoTransMobile   0xD12D
#define PTP_DPC_CANON_EOS_URLSupportFormat  0xD12E
#define PTP_DPC_CANON_EOS_SpecialAcc        0xD12F
#define PTP_DPC_CANON_EOS_CompressionS      0xD130
#define PTP_DPC_CANON_EOS_CompressionM1     0xD131
#define PTP_DPC_CANON_EOS_CompressionM2     0xD132
#define PTP_DPC_CANON_EOS_CompressionL      0xD133
#define PTP_DPC_CANON_EOS_IntervalShootSetting  0xD134
#define PTP_DPC_CANON_EOS_IntervalShootState    0xD135
#define PTP_DPC_CANON_EOS_PushMode      0xD136
#define PTP_DPC_CANON_EOS_LvCFilterKind     0xD137
#define PTP_DPC_CANON_EOS_AEModeDial        0xD138
#define PTP_DPC_CANON_EOS_AEModeCustom      0xD139
#define PTP_DPC_CANON_EOS_MirrorUpSetting   0xD13A
#define PTP_DPC_CANON_EOS_HighlightTonePriority 0xD13B
#define PTP_DPC_CANON_EOS_AFSelectFocusArea 0xD13C
#define PTP_DPC_CANON_EOS_HDRSetting        0xD13D
#define PTP_DPC_CANON_EOS_TimeShootSetting  0xD13E
#define PTP_DPC_CANON_EOS_NFCApplicationInfo    0xD13F
#define PTP_DPC_CANON_EOS_PCWhiteBalance1   0xD140
#define PTP_DPC_CANON_EOS_PCWhiteBalance2   0xD141
#define PTP_DPC_CANON_EOS_PCWhiteBalance3   0xD142
#define PTP_DPC_CANON_EOS_PCWhiteBalance4   0xD143
#define PTP_DPC_CANON_EOS_PCWhiteBalance5   0xD144
#define PTP_DPC_CANON_EOS_MWhiteBalance     0xD145
#define PTP_DPC_CANON_EOS_MWhiteBalanceEx   0xD146
#define PTP_DPC_CANON_EOS_PowerZoomSpeed    0xD149
#define PTP_DPC_CANON_EOS_NetworkServerRegion   0xD14A
#define PTP_DPC_CANON_EOS_GPSLogCtrl        0xD14B
#define PTP_DPC_CANON_EOS_GPSLogListNum     0xD14C
#define PTP_DPC_CANON_EOS_UnknownPropD14D   0xD14D  /*found in Canon EOS 5D M3*/
#define PTP_DPC_CANON_EOS_PictureStyleStandard  0xD150
#define PTP_DPC_CANON_EOS_PictureStylePortrait  0xD151
#define PTP_DPC_CANON_EOS_PictureStyleLandscape 0xD152
#define PTP_DPC_CANON_EOS_PictureStyleNeutral   0xD153
#define PTP_DPC_CANON_EOS_PictureStyleFaithful  0xD154
#define PTP_DPC_CANON_EOS_PictureStyleBlackWhite    0xD155
#define PTP_DPC_CANON_EOS_PictureStyleAuto  0xD156
#define PTP_DPC_CANON_EOS_PictureStyleExStandard    0xD157
#define PTP_DPC_CANON_EOS_PictureStyleExPortrait    0xD158
#define PTP_DPC_CANON_EOS_PictureStyleExLandscape   0xD159
#define PTP_DPC_CANON_EOS_PictureStyleExNeutral     0xD15A
#define PTP_DPC_CANON_EOS_PictureStyleExFaithful    0xD15B
#define PTP_DPC_CANON_EOS_PictureStyleExBlackWhite  0xD15C
#define PTP_DPC_CANON_EOS_PictureStyleExAuto        0xD15D
#define PTP_DPC_CANON_EOS_PictureStyleExFineDetail  0xD15E
#define PTP_DPC_CANON_EOS_PictureStyleUserSet1  0xD160
#define PTP_DPC_CANON_EOS_PictureStyleUserSet2  0xD161
#define PTP_DPC_CANON_EOS_PictureStyleUserSet3  0xD162
#define PTP_DPC_CANON_EOS_PictureStyleExUserSet1    0xD163
#define PTP_DPC_CANON_EOS_PictureStyleExUserSet2    0xD164
#define PTP_DPC_CANON_EOS_PictureStyleExUserSet3    0xD165
#define PTP_DPC_CANON_EOS_ShutterReleaseCounter 0xD167
#define PTP_DPC_CANON_EOS_AvailableImageSize    0xD168
#define PTP_DPC_CANON_EOS_ErrorHistory      0xD169
#define PTP_DPC_CANON_EOS_LensExchangeHistory   0xD16A
#define PTP_DPC_CANON_EOS_StroboExchangeHistory 0xD16B
#define PTP_DPC_CANON_EOS_PictureStyleParam1    0xD170
#define PTP_DPC_CANON_EOS_PictureStyleParam2    0xD171
#define PTP_DPC_CANON_EOS_PictureStyleParam3    0xD172
#define PTP_DPC_CANON_EOS_MovieRecordVolumeLine 0xD174
#define PTP_DPC_CANON_EOS_NetworkCommunicationMode  0xD175
#define PTP_DPC_CANON_EOS_CanonLogGamma         0xD176
#define PTP_DPC_CANON_EOS_SmartphoneShowImageConfig 0xD177
#define PTP_DPC_CANON_EOS_HighISOSettingNoiseReduction  0xD178
#define PTP_DPC_CANON_EOS_MovieServoAF      0xD179
#define PTP_DPC_CANON_EOS_ContinuousAFValid 0xD17A
#define PTP_DPC_CANON_EOS_Attenuator        0xD17B
#define PTP_DPC_CANON_EOS_UTCTime       0xD17C
#define PTP_DPC_CANON_EOS_Timezone      0xD17D
#define PTP_DPC_CANON_EOS_Summertime        0xD17E
#define PTP_DPC_CANON_EOS_FlavorLUTParams   0xD17F
#define PTP_DPC_CANON_EOS_CustomFunc1       0xD180
#define PTP_DPC_CANON_EOS_CustomFunc2       0xD181
#define PTP_DPC_CANON_EOS_CustomFunc3       0xD182
#define PTP_DPC_CANON_EOS_CustomFunc4       0xD183
#define PTP_DPC_CANON_EOS_CustomFunc5       0xD184
#define PTP_DPC_CANON_EOS_CustomFunc6       0xD185
#define PTP_DPC_CANON_EOS_CustomFunc7       0xD186
#define PTP_DPC_CANON_EOS_CustomFunc8       0xD187
#define PTP_DPC_CANON_EOS_CustomFunc9       0xD188
#define PTP_DPC_CANON_EOS_CustomFunc10      0xD189
#define PTP_DPC_CANON_EOS_CustomFunc11      0xD18a
#define PTP_DPC_CANON_EOS_CustomFunc12      0xD18b
#define PTP_DPC_CANON_EOS_CustomFunc13      0xD18c
#define PTP_DPC_CANON_EOS_CustomFunc14      0xD18d
#define PTP_DPC_CANON_EOS_CustomFunc15      0xD18e
#define PTP_DPC_CANON_EOS_CustomFunc16      0xD18f
#define PTP_DPC_CANON_EOS_CustomFunc17      0xD190
#define PTP_DPC_CANON_EOS_CustomFunc18      0xD191
#define PTP_DPC_CANON_EOS_CustomFunc19      0xD192
#define PTP_DPC_CANON_EOS_CustomFunc19      0xD192
#define PTP_DPC_CANON_EOS_InnerDevelop      0xD193
#define PTP_DPC_CANON_EOS_MultiAspect       0xD194
#define PTP_DPC_CANON_EOS_MovieSoundRecord  0xD195
#define PTP_DPC_CANON_EOS_MovieRecordVolume 0xD196
#define PTP_DPC_CANON_EOS_WindCut       0xD197
#define PTP_DPC_CANON_EOS_ExtenderType      0xD198
#define PTP_DPC_CANON_EOS_OLCInfoVersion    0xD199
#define PTP_DPC_CANON_EOS_UnknownPropD19A   0xD19A /*found in Canon EOS 5D M3*/
#define PTP_DPC_CANON_EOS_UnknownPropD19C   0xD19C /*found in Canon EOS 5D M3*/
#define PTP_DPC_CANON_EOS_UnknownPropD19D   0xD19D /*found in Canon EOS 5D M3*/
#define PTP_DPC_CANON_EOS_GPSDeviceActive   0xD19F
#define PTP_DPC_CANON_EOS_CustomFuncEx      0xD1a0
#define PTP_DPC_CANON_EOS_MyMenu        0xD1a1
#define PTP_DPC_CANON_EOS_MyMenuList        0xD1a2
#define PTP_DPC_CANON_EOS_WftStatus     0xD1a3
#define PTP_DPC_CANON_EOS_WftInputTransmission  0xD1a4
#define PTP_DPC_CANON_EOS_HDDirectoryStructure  0xD1a5
#define PTP_DPC_CANON_EOS_BatteryInfo       0xD1a6
#define PTP_DPC_CANON_EOS_AdapterInfo       0xD1a7
#define PTP_DPC_CANON_EOS_LensStatus        0xD1a8
#define PTP_DPC_CANON_EOS_QuickReviewTime   0xD1a9
#define PTP_DPC_CANON_EOS_CardExtension     0xD1aa
#define PTP_DPC_CANON_EOS_TempStatus        0xD1ab
#define PTP_DPC_CANON_EOS_ShutterCounter    0xD1ac
#define PTP_DPC_CANON_EOS_SpecialOption     0xD1ad
#define PTP_DPC_CANON_EOS_PhotoStudioMode   0xD1ae
#define PTP_DPC_CANON_EOS_SerialNumber      0xD1af
#define PTP_DPC_CANON_EOS_EVFOutputDevice   0xD1b0
#define PTP_DPC_CANON_EOS_EVFMode       0xD1b1
#define PTP_DPC_CANON_EOS_DepthOfFieldPreview   0xD1b2
#define PTP_DPC_CANON_EOS_EVFSharpness      0xD1b3
#define PTP_DPC_CANON_EOS_EVFWBMode     0xD1b4
#define PTP_DPC_CANON_EOS_EVFClickWBCoeffs  0xD1b5
#define PTP_DPC_CANON_EOS_EVFColorTemp      0xD1b6
#define PTP_DPC_CANON_EOS_ExposureSimMode   0xD1b7
#define PTP_DPC_CANON_EOS_EVFRecordStatus   0xD1b8
#define PTP_DPC_CANON_EOS_LvAfSystem        0xD1ba
#define PTP_DPC_CANON_EOS_MovSize       0xD1bb
#define PTP_DPC_CANON_EOS_LvViewTypeSelect  0xD1bc
#define PTP_DPC_CANON_EOS_MirrorDownStatus  0xD1bd
#define PTP_DPC_CANON_EOS_MovieParam        0xD1be
#define PTP_DPC_CANON_EOS_MirrorLockupState 0xD1bf
#define PTP_DPC_CANON_EOS_FlashChargingState    0xD1C0
#define PTP_DPC_CANON_EOS_AloMode       0xD1C1
#define PTP_DPC_CANON_EOS_FixedMovie        0xD1C2
#define PTP_DPC_CANON_EOS_OneShotRawOn      0xD1C3
#define PTP_DPC_CANON_EOS_ErrorForDisplay   0xD1C4
#define PTP_DPC_CANON_EOS_AEModeMovie       0xD1C5
#define PTP_DPC_CANON_EOS_BuiltinStroboMode 0xD1C6
#define PTP_DPC_CANON_EOS_StroboDispState   0xD1C7
#define PTP_DPC_CANON_EOS_StroboETTL2Metering   0xD1C8
#define PTP_DPC_CANON_EOS_ContinousAFMode   0xD1C9
#define PTP_DPC_CANON_EOS_MovieParam2       0xD1CA
#define PTP_DPC_CANON_EOS_StroboSettingExpComposition       0xD1CB
#define PTP_DPC_CANON_EOS_MovieParam3       0xD1CC
#define PTP_DPC_CANON_EOS_MovieParam4       0xD1CD
#define PTP_DPC_CANON_EOS_LVMedicalRotate   0xD1CF
#define PTP_DPC_CANON_EOS_Artist        0xD1d0
#define PTP_DPC_CANON_EOS_Copyright     0xD1d1
#define PTP_DPC_CANON_EOS_BracketValue      0xD1d2
#define PTP_DPC_CANON_EOS_FocusInfoEx       0xD1d3
#define PTP_DPC_CANON_EOS_DepthOfField      0xD1d4
#define PTP_DPC_CANON_EOS_Brightness        0xD1d5
#define PTP_DPC_CANON_EOS_LensAdjustParams  0xD1d6
#define PTP_DPC_CANON_EOS_EFComp        0xD1d7
#define PTP_DPC_CANON_EOS_LensName      0xD1d8
#define PTP_DPC_CANON_EOS_AEB           0xD1d9
#define PTP_DPC_CANON_EOS_StroboSetting     0xD1da
#define PTP_DPC_CANON_EOS_StroboWirelessSetting 0xD1db
#define PTP_DPC_CANON_EOS_StroboFiring      0xD1dc
#define PTP_DPC_CANON_EOS_LensID        0xD1dd
#define PTP_DPC_CANON_EOS_LCDBrightness     0xD1de
#define PTP_DPC_CANON_EOS_CADarkBright      0xD1df

/* Nikon 扩展设备属性代码 */
#define PTP_DPC_NIKON_ShootingBank          0xD010
#define PTP_DPC_NIKON_ShootingBankNameA         0xD011
#define PTP_DPC_NIKON_ShootingBankNameB         0xD012
#define PTP_DPC_NIKON_ShootingBankNameC         0xD013
#define PTP_DPC_NIKON_ShootingBankNameD         0xD014
#define PTP_DPC_NIKON_ResetBank0            0xD015
#define PTP_DPC_NIKON_RawCompression            0xD016
#define PTP_DPC_NIKON_WhiteBalanceAutoBias      0xD017
#define PTP_DPC_NIKON_WhiteBalanceTungstenBias      0xD018
#define PTP_DPC_NIKON_WhiteBalanceFluorescentBias   0xD019
#define PTP_DPC_NIKON_WhiteBalanceDaylightBias      0xD01A
#define PTP_DPC_NIKON_WhiteBalanceFlashBias     0xD01B
#define PTP_DPC_NIKON_WhiteBalanceCloudyBias        0xD01C
#define PTP_DPC_NIKON_WhiteBalanceShadeBias     0xD01D
#define PTP_DPC_NIKON_WhiteBalanceColorTemperature  0xD01E
#define PTP_DPC_NIKON_WhiteBalancePresetNo      0xD01F
#define PTP_DPC_NIKON_WhiteBalancePresetName0       0xD020
#define PTP_DPC_NIKON_WhiteBalancePresetName1       0xD021
#define PTP_DPC_NIKON_WhiteBalancePresetName2       0xD022
#define PTP_DPC_NIKON_WhiteBalancePresetName3       0xD023
#define PTP_DPC_NIKON_WhiteBalancePresetName4       0xD024
#define PTP_DPC_NIKON_WhiteBalancePresetVal0        0xD025
#define PTP_DPC_NIKON_WhiteBalancePresetVal1        0xD026
#define PTP_DPC_NIKON_WhiteBalancePresetVal2        0xD027
#define PTP_DPC_NIKON_WhiteBalancePresetVal3        0xD028
#define PTP_DPC_NIKON_WhiteBalancePresetVal4        0xD029
#define PTP_DPC_NIKON_ImageSharpening           0xD02A
#define PTP_DPC_NIKON_ToneCompensation          0xD02B
#define PTP_DPC_NIKON_ColorModel            0xD02C
#define PTP_DPC_NIKON_HueAdjustment         0xD02D
#define PTP_DPC_NIKON_NonCPULensDataFocalLength     0xD02E  /* Set FMM Manual */
#define PTP_DPC_NIKON_NonCPULensDataMaximumAperture 0xD02F  /* Set F0 Manual */
#define PTP_DPC_NIKON_ShootingMode          0xD030
#define PTP_DPC_NIKON_JPEG_Compression_Policy       0xD031
#define PTP_DPC_NIKON_ColorSpace            0xD032
#define PTP_DPC_NIKON_AutoDXCrop            0xD033
#define PTP_DPC_NIKON_FlickerReduction          0xD034
#define PTP_DPC_NIKON_RemoteMode            0xD035
#define PTP_DPC_NIKON_VideoMode             0xD036
#define PTP_DPC_NIKON_EffectMode            0xD037
#define PTP_DPC_NIKON_1_Mode                0xD038
#define PTP_DPC_NIKON_CSMMenuBankSelect         0xD040
#define PTP_DPC_NIKON_MenuBankNameA         0xD041
#define PTP_DPC_NIKON_MenuBankNameB         0xD042
#define PTP_DPC_NIKON_MenuBankNameC         0xD043
#define PTP_DPC_NIKON_MenuBankNameD         0xD044
#define PTP_DPC_NIKON_ResetBank             0xD045
#define PTP_DPC_NIKON_A1AFCModePriority         0xD048
#define PTP_DPC_NIKON_A2AFSModePriority         0xD049
#define PTP_DPC_NIKON_A3GroupDynamicAF          0xD04A
#define PTP_DPC_NIKON_A4AFActivation            0xD04B
#define PTP_DPC_NIKON_FocusAreaIllumManualFocus     0xD04C
#define PTP_DPC_NIKON_FocusAreaIllumContinuous      0xD04D
#define PTP_DPC_NIKON_FocusAreaIllumWhenSelected    0xD04E
#define PTP_DPC_NIKON_FocusAreaWrap         0xD04F /* area sel */
#define PTP_DPC_NIKON_VerticalAFON          0xD050
#define PTP_DPC_NIKON_AFLockOn              0xD051
#define PTP_DPC_NIKON_FocusAreaZone         0xD052
#define PTP_DPC_NIKON_EnableCopyright           0xD053
#define PTP_DPC_NIKON_ISOAuto               0xD054
#define PTP_DPC_NIKON_EVISOStep             0xD055
#define PTP_DPC_NIKON_EVStep                0xD056 /* EV Step SS FN */
#define PTP_DPC_NIKON_EVStepExposureComp        0xD057
#define PTP_DPC_NIKON_ExposureCompensation      0xD058
#define PTP_DPC_NIKON_CenterWeightArea          0xD059
#define PTP_DPC_NIKON_ExposureBaseMatrix        0xD05A
#define PTP_DPC_NIKON_ExposureBaseCenter        0xD05B
#define PTP_DPC_NIKON_ExposureBaseSpot          0xD05C
#define PTP_DPC_NIKON_LiveViewAFArea            0xD05D /* FIXME: AfAtLiveview? */
#define PTP_DPC_NIKON_AELockMode            0xD05E
#define PTP_DPC_NIKON_AELAFLMode            0xD05F
#define PTP_DPC_NIKON_LiveViewAFFocus           0xD061
#define PTP_DPC_NIKON_MeterOff              0xD062
#define PTP_DPC_NIKON_SelfTimer             0xD063
#define PTP_DPC_NIKON_MonitorOff            0xD064
#define PTP_DPC_NIKON_ImgConfTime           0xD065
#define PTP_DPC_NIKON_AutoOffTimers         0xD066
#define PTP_DPC_NIKON_AngleLevel            0xD067
#define PTP_DPC_NIKON_D1ShootingSpeed           0xD068 /* continous speed low */
#define PTP_DPC_NIKON_D2MaximumShots            0xD069
#define PTP_DPC_NIKON_ExposureDelayMode         0xD06A
#define PTP_DPC_NIKON_LongExposureNoiseReduction    0xD06B
#define PTP_DPC_NIKON_FileNumberSequence        0xD06C
#define PTP_DPC_NIKON_ControlPanelFinderRearControl 0xD06D
#define PTP_DPC_NIKON_ControlPanelFinderViewfinder  0xD06E
#define PTP_DPC_NIKON_D7Illumination            0xD06F
#define PTP_DPC_NIKON_NrHighISO             0xD070
#define PTP_DPC_NIKON_SHSET_CH_GUID_DISP        0xD071
#define PTP_DPC_NIKON_ArtistName            0xD072
#define PTP_DPC_NIKON_CopyrightInfo         0xD073
#define PTP_DPC_NIKON_FlashSyncSpeed            0xD074
#define PTP_DPC_NIKON_FlashShutterSpeed         0xD075  /* SB Low Limit */
#define PTP_DPC_NIKON_E3AAFlashMode         0xD076
#define PTP_DPC_NIKON_E4ModelingFlash           0xD077
#define PTP_DPC_NIKON_BracketSet            0xD078  /* Bracket Type? */
#define PTP_DPC_NIKON_E6ManualModeBracketing        0xD079  /* Bracket Factor? */
#define PTP_DPC_NIKON_BracketOrder          0xD07A
#define PTP_DPC_NIKON_E8AutoBracketSelection        0xD07B  /* Bracket Method? */
#define PTP_DPC_NIKON_BracketingSet         0xD07C
#define PTP_DPC_NIKON_F1CenterButtonShootingMode    0xD080
#define PTP_DPC_NIKON_CenterButtonPlaybackMode      0xD081
#define PTP_DPC_NIKON_F2Multiselector           0xD082
#define PTP_DPC_NIKON_F3PhotoInfoPlayback       0xD083  /* MultiSelector Dir */
#define PTP_DPC_NIKON_F4AssignFuncButton        0xD084  /* CMD Dial Rotate */
#define PTP_DPC_NIKON_F5CustomizeCommDials      0xD085  /* CMD Dial Change */
#define PTP_DPC_NIKON_ReverseCommandDial        0xD086  /* CMD Dial FN Set */
#define PTP_DPC_NIKON_ApertureSetting           0xD087  /* CMD Dial Active */
#define PTP_DPC_NIKON_MenusAndPlayback          0xD088  /* CMD Dial Active */
#define PTP_DPC_NIKON_F6ButtonsAndDials         0xD089  /* Universal Mode? */
#define PTP_DPC_NIKON_NoCFCard              0xD08A  /* Enable Shutter? */
#define PTP_DPC_NIKON_CenterButtonZoomRatio     0xD08B
#define PTP_DPC_NIKON_FunctionButton2           0xD08C
#define PTP_DPC_NIKON_AFAreaPoint           0xD08D
#define PTP_DPC_NIKON_NormalAFOn            0xD08E
#define PTP_DPC_NIKON_CleanImageSensor          0xD08F
#define PTP_DPC_NIKON_ImageCommentString        0xD090
#define PTP_DPC_NIKON_ImageCommentEnable        0xD091
#define PTP_DPC_NIKON_ImageRotation         0xD092
#define PTP_DPC_NIKON_ManualSetLensNo           0xD093
#define PTP_DPC_NIKON_MovScreenSize         0xD0A0
#define PTP_DPC_NIKON_MovVoice              0xD0A1
#define PTP_DPC_NIKON_MovMicrophone         0xD0A2
#define PTP_DPC_NIKON_MovFileSlot           0xD0A3
#define PTP_DPC_NIKON_MovRecProhibitCondition       0xD0A4
#define PTP_DPC_NIKON_ManualMovieSetting        0xD0A6
#define PTP_DPC_NIKON_MovQuality            0xD0A7
#define PTP_DPC_NIKON_LiveViewScreenDisplaySetting  0xD0B2
#define PTP_DPC_NIKON_MonitorOffDelay           0xD0B3
#define PTP_DPC_NIKON_Bracketing            0xD0C0
#define PTP_DPC_NIKON_AutoExposureBracketStep       0xD0C1
#define PTP_DPC_NIKON_AutoExposureBracketProgram    0xD0C2
#define PTP_DPC_NIKON_AutoExposureBracketCount      0xD0C3
#define PTP_DPC_NIKON_WhiteBalanceBracketStep       0xD0C4
#define PTP_DPC_NIKON_WhiteBalanceBracketProgram    0xD0C5
#define PTP_DPC_NIKON_LensID                0xD0E0
#define PTP_DPC_NIKON_LensSort              0xD0E1
#define PTP_DPC_NIKON_LensType              0xD0E2
#define PTP_DPC_NIKON_FocalLengthMin            0xD0E3
#define PTP_DPC_NIKON_FocalLengthMax            0xD0E4
#define PTP_DPC_NIKON_MaxApAtMinFocalLength     0xD0E5
#define PTP_DPC_NIKON_MaxApAtMaxFocalLength     0xD0E6
#define PTP_DPC_NIKON_FinderISODisp         0xD0F0
#define PTP_DPC_NIKON_AutoOffPhoto          0xD0F2
#define PTP_DPC_NIKON_AutoOffMenu           0xD0F3
#define PTP_DPC_NIKON_AutoOffInfo           0xD0F4
#define PTP_DPC_NIKON_SelfTimerShootNum         0xD0F5
#define PTP_DPC_NIKON_VignetteCtrl          0xD0F7
#define PTP_DPC_NIKON_AutoDistortionControl     0xD0F8
#define PTP_DPC_NIKON_SceneMode             0xD0F9
#define PTP_DPC_NIKON_SceneMode2            0xD0FD
#define PTP_DPC_NIKON_SelfTimerInterval         0xD0FE
#define PTP_DPC_NIKON_ExposureTime          0xD100  /* Shutter Speed */
#define PTP_DPC_NIKON_ACPower               0xD101
#define PTP_DPC_NIKON_WarningStatus         0xD102
#define PTP_DPC_NIKON_MaximumShots          0xD103 /* remain shots (in RAM buffer?) */
#define PTP_DPC_NIKON_AFLockStatus          0xD104
#define PTP_DPC_NIKON_AELockStatus          0xD105
#define PTP_DPC_NIKON_FVLockStatus          0xD106
#define PTP_DPC_NIKON_AutofocusLCDTopMode2      0xD107
#define PTP_DPC_NIKON_AutofocusArea         0xD108
#define PTP_DPC_NIKON_FlexibleProgram           0xD109
#define PTP_DPC_NIKON_LightMeter            0xD10A  /* Exposure Status */
#define PTP_DPC_NIKON_RecordingMedia            0xD10B  /* Card or SDRAM */
#define PTP_DPC_NIKON_USBSpeed              0xD10C
#define PTP_DPC_NIKON_CCDNumber             0xD10D
#define PTP_DPC_NIKON_CameraOrientation         0xD10E
#define PTP_DPC_NIKON_GroupPtnType          0xD10F
#define PTP_DPC_NIKON_FNumberLock           0xD110
#define PTP_DPC_NIKON_ExposureApertureLock      0xD111  /* shutterspeed lock*/
#define PTP_DPC_NIKON_TVLockSetting         0xD112
#define PTP_DPC_NIKON_AVLockSetting         0xD113
#define PTP_DPC_NIKON_IllumSetting          0xD114
#define PTP_DPC_NIKON_FocusPointBright          0xD115
#define PTP_DPC_NIKON_ExternalFlashAttached     0xD120
#define PTP_DPC_NIKON_ExternalFlashStatus       0xD121
#define PTP_DPC_NIKON_ExternalFlashSort         0xD122
#define PTP_DPC_NIKON_ExternalFlashMode         0xD123
#define PTP_DPC_NIKON_ExternalFlashCompensation     0xD124
#define PTP_DPC_NIKON_NewExternalFlashMode      0xD125
#define PTP_DPC_NIKON_FlashExposureCompensation     0xD126
#define PTP_DPC_NIKON_HDRMode               0xD130
#define PTP_DPC_NIKON_HDRHighDynamic            0xD131
#define PTP_DPC_NIKON_HDRSmoothing          0xD132
#define PTP_DPC_NIKON_OptimizeImage         0xD140
#define PTP_DPC_NIKON_Saturation            0xD142
#define PTP_DPC_NIKON_BW_FillerEffect           0xD143
#define PTP_DPC_NIKON_BW_Sharpness          0xD144
#define PTP_DPC_NIKON_BW_Contrast           0xD145
#define PTP_DPC_NIKON_BW_Setting_Type           0xD146
#define PTP_DPC_NIKON_Slot2SaveMode         0xD148
#define PTP_DPC_NIKON_RawBitMode            0xD149
#define PTP_DPC_NIKON_ActiveDLighting           0xD14E /* was PTP_DPC_NIKON_ISOAutoTime */
#define PTP_DPC_NIKON_FlourescentType           0xD14F
#define PTP_DPC_NIKON_TuneColourTemperature     0xD150
#define PTP_DPC_NIKON_TunePreset0           0xD151
#define PTP_DPC_NIKON_TunePreset1           0xD152
#define PTP_DPC_NIKON_TunePreset2           0xD153
#define PTP_DPC_NIKON_TunePreset3           0xD154
#define PTP_DPC_NIKON_TunePreset4           0xD155
#define PTP_DPC_NIKON_WhiteBalanceNaturalLightAutoBias  0xD15E /* Only encountered in D850? */
#define PTP_DPC_NIKON_BeepOff               0xD160
#define PTP_DPC_NIKON_AutofocusMode         0xD161
#define PTP_DPC_NIKON_AFAssist              0xD163
#define PTP_DPC_NIKON_PADVPMode             0xD164  /* iso auto time */
#define PTP_DPC_NIKON_ImageReview           0xD165
#define PTP_DPC_NIKON_AFAreaIllumination        0xD166
#define PTP_DPC_NIKON_FlashMode             0xD167
#define PTP_DPC_NIKON_FlashCommanderMode        0xD168
#define PTP_DPC_NIKON_FlashSign             0xD169
#define PTP_DPC_NIKON_ISO_Auto              0xD16A
#define PTP_DPC_NIKON_RemoteTimeout         0xD16B
#define PTP_DPC_NIKON_GridDisplay           0xD16C
#define PTP_DPC_NIKON_FlashModeManualPower      0xD16D
#define PTP_DPC_NIKON_FlashModeCommanderPower       0xD16E
#define PTP_DPC_NIKON_AutoFP                0xD16F
#define PTP_DPC_NIKON_DateImprintSetting        0xD170
#define PTP_DPC_NIKON_DateCounterSelect         0xD171
#define PTP_DPC_NIKON_DateCountData         0xD172
#define PTP_DPC_NIKON_DateCountDisplaySetting       0xD173
#define PTP_DPC_NIKON_RangeFinderSetting        0xD174
#define PTP_DPC_NIKON_CSMMenu               0xD180
#define PTP_DPC_NIKON_WarningDisplay            0xD181
#define PTP_DPC_NIKON_BatteryCellKind           0xD182
#define PTP_DPC_NIKON_ISOAutoHiLimit            0xD183
#define PTP_DPC_NIKON_DynamicAFArea         0xD184
#define PTP_DPC_NIKON_ContinuousSpeedHigh       0xD186
#define PTP_DPC_NIKON_InfoDispSetting           0xD187
#define PTP_DPC_NIKON_PreviewButton         0xD189
#define PTP_DPC_NIKON_PreviewButton2            0xD18A
#define PTP_DPC_NIKON_AEAFLockButton2           0xD18B
#define PTP_DPC_NIKON_IndicatorDisp         0xD18D
#define PTP_DPC_NIKON_CellKindPriority          0xD18E
#define PTP_DPC_NIKON_BracketingFramesAndSteps      0xD190
#define PTP_DPC_NIKON_LiveViewMode          0xD1A0
#define PTP_DPC_NIKON_LiveViewDriveMode         0xD1A1
#define PTP_DPC_NIKON_LiveViewStatus            0xD1A2
#define PTP_DPC_NIKON_LiveViewImageZoomRatio        0xD1A3
#define PTP_DPC_NIKON_LiveViewProhibitCondition     0xD1A4
#define PTP_DPC_NIKON_MovieShutterSpeed         0xD1A8
#define PTP_DPC_NIKON_MovieFNumber          0xD1A9
#define PTP_DPC_NIKON_MovieISO              0xD1AA
#define PTP_DPC_NIKON_LiveViewMovieMode         0xD1AC /* ? */
#define PTP_DPC_NIKON_ExposureDisplayStatus     0xD1B0
#define PTP_DPC_NIKON_ExposureIndicateStatus        0xD1B1
#define PTP_DPC_NIKON_InfoDispErrStatus         0xD1B2
#define PTP_DPC_NIKON_ExposureIndicateLightup       0xD1B3
#define PTP_DPC_NIKON_FlashOpen             0xD1C0
#define PTP_DPC_NIKON_FlashCharged          0xD1C1
#define PTP_DPC_NIKON_FlashMRepeatValue         0xD1D0
#define PTP_DPC_NIKON_FlashMRepeatCount         0xD1D1
#define PTP_DPC_NIKON_FlashMRepeatInterval      0xD1D2
#define PTP_DPC_NIKON_FlashCommandChannel       0xD1D3
#define PTP_DPC_NIKON_FlashCommandSelfMode      0xD1D4
#define PTP_DPC_NIKON_FlashCommandSelfCompensation  0xD1D5
#define PTP_DPC_NIKON_FlashCommandSelfValue     0xD1D6
#define PTP_DPC_NIKON_FlashCommandAMode         0xD1D7
#define PTP_DPC_NIKON_FlashCommandACompensation     0xD1D8
#define PTP_DPC_NIKON_FlashCommandAValue        0xD1D9
#define PTP_DPC_NIKON_FlashCommandBMode         0xD1DA
#define PTP_DPC_NIKON_FlashCommandBCompensation     0xD1DB
#define PTP_DPC_NIKON_FlashCommandBValue        0xD1DC
#define PTP_DPC_NIKON_ApplicationMode           0xD1F0
#define PTP_DPC_NIKON_ActiveSlot            0xD1F2
#define PTP_DPC_NIKON_ActivePicCtrlItem         0xD200
#define PTP_DPC_NIKON_ChangePicCtrlItem         0xD201
#define PTP_DPC_NIKON_MovieNrHighISO            0xD236

/* this is irregular, as it should be -0x5000 or 0xD000 based */
#define PTP_DPC_NIKON_1_ISO             0xF002
#define PTP_DPC_NIKON_1_ImageCompression        0xF009
#define PTP_DPC_NIKON_1_ImageSize           0xF00A
#define PTP_DPC_NIKON_1_WhiteBalance            0xF00C
#define PTP_DPC_NIKON_1_LongExposureNoiseReduction  0xF00D
#define PTP_DPC_NIKON_1_HiISONoiseReduction     0xF00E
#define PTP_DPC_NIKON_1_ActiveDLighting         0xF00F
#define PTP_DPC_NIKON_1_MovQuality          0xF01C

/* Fuji specific */
#define PTP_DPC_FUJI_ColorTemperature           0xD017
#define PTP_DPC_FUJI_Quality                0xD018
#define PTP_DPC_FUJI_ReleaseMode            0xD201
#define PTP_DPC_FUJI_FocusAreas             0xD206
#define PTP_DPC_FUJI_AELock             0xD213
#define PTP_DPC_FUJI_Copyright              0xD215
#define PTP_DPC_FUJI_Aperture               0xD218
#define PTP_DPC_FUJI_ShutterSpeed           0xD219

/* Microsoft/MTP 特殊 */
#define PTP_DPC_MTP_SecureTime              0xD101
#define PTP_DPC_MTP_DeviceCertificate           0xD102
#define PTP_DPC_MTP_RevocationInfo          0xD103
#define PTP_DPC_MTP_SynchronizationPartner      0xD401
#define PTP_DPC_MTP_DeviceFriendlyName          0xD402
#define PTP_DPC_MTP_VolumeLevel             0xD403
#define PTP_DPC_MTP_DeviceIcon              0xD405
#define PTP_DPC_MTP_SessionInitiatorInfo        0xD406
#define PTP_DPC_MTP_PerceivedDeviceType         0xD407
#define PTP_DPC_MTP_PlaybackRate                        0xD410
#define PTP_DPC_MTP_PlaybackObject                      0xD411
#define PTP_DPC_MTP_PlaybackContainerIndex              0xD412
#define PTP_DPC_MTP_PlaybackPosition                    0xD413
#define PTP_DPC_MTP_PlaysForSureID                      0xD131

/* Sony A900 */
#define PTP_DPC_SONY_DPCCompensation            0xD200
#define PTP_DPC_SONY_DRangeOptimize         0xD201
#define PTP_DPC_SONY_ImageSize              0xD203
#define PTP_DPC_SONY_ShutterSpeed           0xD20D
#define PTP_DPC_SONY_ColorTemp              0xD20F
#define PTP_DPC_SONY_CCFilter               0xD210
#define PTP_DPC_SONY_AspectRatio            0xD211
#define PTP_DPC_SONY_FocusFound                 0xD213 /* seems to be signaled (1->2) when focus is achieved */
#define PTP_DPC_SONY_ObjectInMemory             0xD215 /* used to signal when to retrieve new object */
#define PTP_DPC_SONY_ExposeIndex            0xD216
#define PTP_DPC_SONY_BatteryLevel           0xD218
#define PTP_DPC_SONY_PictureEffect          0xD21B
#define PTP_DPC_SONY_ABFilter               0xD21C
#define PTP_DPC_SONY_ISO                0xD21E /* ? */
#define PTP_DPC_SONY_AutoFocus              0xD2C1 /* ? half-press */
#define PTP_DPC_SONY_Capture                0xD2C2 /* ? full-press */
/* also seen: D2C3 D2C4 */
/* semi control opcodes */
#define PTP_DPC_SONY_Movie              0xD2C8 /* ? */
#define PTP_DPC_SONY_StillImage             0xD2C7 /* ? */

/* https://github.com/Parrot-Developers/sequoia-ptpy */
#define PTP_DPC_PARROT_PhotoSensorEnableMask            0xD201
#define PTP_DPC_PARROT_PhotoSensorsKeepOn           0xD202
#define PTP_DPC_PARROT_MultispectralImageSize           0xD203
#define PTP_DPC_PARROT_MainBitDepth             0xD204
#define PTP_DPC_PARROT_MultispectralBitDepth            0xD205
#define PTP_DPC_PARROT_HeatingEnable                0xD206
#define PTP_DPC_PARROT_WifiStatus               0xD207
#define PTP_DPC_PARROT_WifiSSID                 0xD208
#define PTP_DPC_PARROT_WifiEncryptionType           0xD209
#define PTP_DPC_PARROT_WifiPassphrase               0xD20A
#define PTP_DPC_PARROT_WifiChannel              0xD20B
#define PTP_DPC_PARROT_Localization             0xD20C
#define PTP_DPC_PARROT_WifiMode                 0xD20D
#define PTP_DPC_PARROT_AntiFlickeringFrequency          0xD210
#define PTP_DPC_PARROT_DisplayOverlayMask           0xD211
#define PTP_DPC_PARROT_GPSInterval              0xD212
#define PTP_DPC_PARROT_MultisensorsExposureMeteringMode     0xD213
#define PTP_DPC_PARROT_MultisensorsExposureTime         0xD214
#define PTP_DPC_PARROT_MultisensorsExposureProgramMode      0xD215
#define PTP_DPC_PARROT_MultisensorsExposureIndex        0xD216
#define PTP_DPC_PARROT_MultisensorsIrradianceGain       0xD217
#define PTP_DPC_PARROT_MultisensorsIrradianceIntegrationTime    0xD218
#define PTP_DPC_PARROT_OverlapRate              0xD219


/* MTP特定的对象特性 */
#define PTP_OPC_StorageID               0xDC01
#define PTP_OPC_ObjectFormat                0xDC02
#define PTP_OPC_ProtectionStatus            0xDC03
#define PTP_OPC_ObjectSize              0xDC04
#define PTP_OPC_AssociationType             0xDC05
#define PTP_OPC_AssociationDesc             0xDC06
#define PTP_OPC_ObjectFileName              0xDC07
#define PTP_OPC_DateCreated             0xDC08
#define PTP_OPC_DateModified                0xDC09
#define PTP_OPC_Keywords                0xDC0A
#define PTP_OPC_ParentObject                0xDC0B
#define PTP_OPC_AllowedFolderContents           0xDC0C
#define PTP_OPC_Hidden                  0xDC0D
#define PTP_OPC_SystemObject                0xDC0E
#define PTP_OPC_PersistantUniqueObjectIdentifier    0xDC41
#define PTP_OPC_SyncID                  0xDC42
#define PTP_OPC_PropertyBag             0xDC43
#define PTP_OPC_Name                    0xDC44
#define PTP_OPC_CreatedBy               0xDC45
#define PTP_OPC_Artist                  0xDC46
#define PTP_OPC_DateAuthored                0xDC47
#define PTP_OPC_Description             0xDC48
#define PTP_OPC_URLReference                0xDC49
#define PTP_OPC_LanguageLocale              0xDC4A
#define PTP_OPC_CopyrightInformation            0xDC4B
#define PTP_OPC_Source                  0xDC4C
#define PTP_OPC_OriginLocation              0xDC4D
#define PTP_OPC_DateAdded               0xDC4E
#define PTP_OPC_NonConsumable               0xDC4F
#define PTP_OPC_CorruptOrUnplayable         0xDC50
#define PTP_OPC_ProducerSerialNumber            0xDC51
#define PTP_OPC_RepresentativeSampleFormat      0xDC81
#define PTP_OPC_RepresentativeSampleSize        0xDC82
#define PTP_OPC_RepresentativeSampleHeight      0xDC83
#define PTP_OPC_RepresentativeSampleWidth       0xDC84
#define PTP_OPC_RepresentativeSampleDuration        0xDC85
#define PTP_OPC_RepresentativeSampleData        0xDC86
#define PTP_OPC_Width                   0xDC87
#define PTP_OPC_Height                  0xDC88
#define PTP_OPC_Duration                0xDC89
#define PTP_OPC_Rating                  0xDC8A
#define PTP_OPC_Track                   0xDC8B
#define PTP_OPC_Genre                   0xDC8C
#define PTP_OPC_Credits                 0xDC8D
#define PTP_OPC_Lyrics                  0xDC8E
#define PTP_OPC_SubscriptionContentID           0xDC8F
#define PTP_OPC_ProducedBy              0xDC90
#define PTP_OPC_UseCount                0xDC91
#define PTP_OPC_SkipCount               0xDC92
#define PTP_OPC_LastAccessed                0xDC93
#define PTP_OPC_ParentalRating              0xDC94
#define PTP_OPC_MetaGenre               0xDC95
#define PTP_OPC_Composer                0xDC96
#define PTP_OPC_EffectiveRating             0xDC97
#define PTP_OPC_Subtitle                0xDC98
#define PTP_OPC_OriginalReleaseDate         0xDC99
#define PTP_OPC_AlbumName               0xDC9A
#define PTP_OPC_AlbumArtist             0xDC9B
#define PTP_OPC_Mood                    0xDC9C
#define PTP_OPC_DRMStatus               0xDC9D
#define PTP_OPC_SubDescription              0xDC9E
#define PTP_OPC_IsCropped               0xDCD1
#define PTP_OPC_IsColorCorrected            0xDCD2
#define PTP_OPC_ImageBitDepth               0xDCD3
#define PTP_OPC_Fnumber                 0xDCD4
#define PTP_OPC_ExposureTime                0xDCD5
#define PTP_OPC_ExposureIndex               0xDCD6
#define PTP_OPC_DisplayName             0xDCE0
#define PTP_OPC_BodyText                0xDCE1
#define PTP_OPC_Subject                 0xDCE2
#define PTP_OPC_Priority                0xDCE3
#define PTP_OPC_GivenName               0xDD00
#define PTP_OPC_MiddleNames             0xDD01
#define PTP_OPC_FamilyName              0xDD02
#define PTP_OPC_Prefix                  0xDD03
#define PTP_OPC_Suffix                  0xDD04
#define PTP_OPC_PhoneticGivenName           0xDD05
#define PTP_OPC_PhoneticFamilyName          0xDD06
#define PTP_OPC_EmailPrimary                0xDD07
#define PTP_OPC_EmailPersonal1              0xDD08
#define PTP_OPC_EmailPersonal2              0xDD09
#define PTP_OPC_EmailBusiness1              0xDD0A
#define PTP_OPC_EmailBusiness2              0xDD0B
#define PTP_OPC_EmailOthers             0xDD0C
#define PTP_OPC_PhoneNumberPrimary          0xDD0D
#define PTP_OPC_PhoneNumberPersonal         0xDD0E
#define PTP_OPC_PhoneNumberPersonal2            0xDD0F
#define PTP_OPC_PhoneNumberBusiness         0xDD10
#define PTP_OPC_PhoneNumberBusiness2            0xDD11
#define PTP_OPC_PhoneNumberMobile           0xDD12
#define PTP_OPC_PhoneNumberMobile2          0xDD13
#define PTP_OPC_FaxNumberPrimary            0xDD14
#define PTP_OPC_FaxNumberPersonal           0xDD15
#define PTP_OPC_FaxNumberBusiness           0xDD16
#define PTP_OPC_PagerNumber             0xDD17
#define PTP_OPC_PhoneNumberOthers           0xDD18
#define PTP_OPC_PrimaryWebAddress           0xDD19
#define PTP_OPC_PersonalWebAddress          0xDD1A
#define PTP_OPC_BusinessWebAddress          0xDD1B
#define PTP_OPC_InstantMessengerAddress         0xDD1C
#define PTP_OPC_InstantMessengerAddress2        0xDD1D
#define PTP_OPC_InstantMessengerAddress3        0xDD1E
#define PTP_OPC_PostalAddressPersonalFull       0xDD1F
#define PTP_OPC_PostalAddressPersonalFullLine1      0xDD20
#define PTP_OPC_PostalAddressPersonalFullLine2      0xDD21
#define PTP_OPC_PostalAddressPersonalFullCity       0xDD22
#define PTP_OPC_PostalAddressPersonalFullRegion     0xDD23
#define PTP_OPC_PostalAddressPersonalFullPostalCode 0xDD24
#define PTP_OPC_PostalAddressPersonalFullCountry    0xDD25
#define PTP_OPC_PostalAddressBusinessFull       0xDD26
#define PTP_OPC_PostalAddressBusinessLine1      0xDD27
#define PTP_OPC_PostalAddressBusinessLine2      0xDD28
#define PTP_OPC_PostalAddressBusinessCity       0xDD29
#define PTP_OPC_PostalAddressBusinessRegion     0xDD2A
#define PTP_OPC_PostalAddressBusinessPostalCode     0xDD2B
#define PTP_OPC_PostalAddressBusinessCountry        0xDD2C
#define PTP_OPC_PostalAddressOtherFull          0xDD2D
#define PTP_OPC_PostalAddressOtherLine1         0xDD2E
#define PTP_OPC_PostalAddressOtherLine2         0xDD2F
#define PTP_OPC_PostalAddressOtherCity          0xDD30
#define PTP_OPC_PostalAddressOtherRegion        0xDD31
#define PTP_OPC_PostalAddressOtherPostalCode        0xDD32
#define PTP_OPC_PostalAddressOtherCountry       0xDD33
#define PTP_OPC_OrganizationName            0xDD34
#define PTP_OPC_PhoneticOrganizationName        0xDD35
#define PTP_OPC_Role                    0xDD36
#define PTP_OPC_Birthdate               0xDD37
#define PTP_OPC_MessageTo               0xDD40
#define PTP_OPC_MessageCC               0xDD41
#define PTP_OPC_MessageBCC              0xDD42
#define PTP_OPC_MessageRead             0xDD43
#define PTP_OPC_MessageReceivedTime         0xDD44
#define PTP_OPC_MessageSender               0xDD45
#define PTP_OPC_ActivityBeginTime           0xDD50
#define PTP_OPC_ActivityEndTime             0xDD51
#define PTP_OPC_ActivityLocation            0xDD52
#define PTP_OPC_ActivityRequiredAttendees       0xDD54
#define PTP_OPC_ActivityOptionalAttendees       0xDD55
#define PTP_OPC_ActivityResources           0xDD56
#define PTP_OPC_ActivityAccepted            0xDD57
#define PTP_OPC_Owner                   0xDD5D
#define PTP_OPC_Editor                  0xDD5E
#define PTP_OPC_Webmaster               0xDD5F
#define PTP_OPC_URLSource               0xDD60
#define PTP_OPC_URLDestination              0xDD61
#define PTP_OPC_TimeBookmark                0xDD62
#define PTP_OPC_ObjectBookmark              0xDD63
#define PTP_OPC_ByteBookmark                0xDD64
#define PTP_OPC_LastBuildDate               0xDD70
#define PTP_OPC_TimetoLive              0xDD71
#define PTP_OPC_MediaGUID               0xDD72
#define PTP_OPC_TotalBitRate                0xDE91
#define PTP_OPC_BitRateType             0xDE92
#define PTP_OPC_SampleRate              0xDE93
#define PTP_OPC_NumberOfChannels            0xDE94
#define PTP_OPC_AudioBitDepth               0xDE95
#define PTP_OPC_ScanDepth               0xDE97
#define PTP_OPC_AudioWAVECodec              0xDE99
#define PTP_OPC_AudioBitRate                0xDE9A
#define PTP_OPC_VideoFourCCCodec            0xDE9B
#define PTP_OPC_VideoBitRate                0xDE9C
#define PTP_OPC_FramesPerThousandSeconds        0xDE9D
#define PTP_OPC_KeyFrameDistance            0xDE9E
#define PTP_OPC_BufferSize              0xDE9F
#define PTP_OPC_EncodingQuality             0xDEA0
#define PTP_OPC_EncodingProfile             0xDEA1
#define PTP_OPC_BuyFlag                 0xD901

/* WiFi网路配置 MTP扩展特性代码 */
#define PTP_OPC_WirelessConfigurationFile       0xB104

/* 设备特性类型标志(DPFF,Device Property Form Flag)*/

#define PTP_DPFF_None           0x00
#define PTP_DPFF_Range          0x01
#define PTP_DPFF_Enumeration        0x02

/* MTP使用的对象特性代码(前3个和DPFF代码一样)*/
#define PTP_OPFF_None           0x00
#define PTP_OPFF_Range          0x01
#define PTP_OPFF_Enumeration        0x02
#define PTP_OPFF_DateTime       0x03
#define PTP_OPFF_FixedLengthArray   0x04
#define PTP_OPFF_RegularExpression  0x05
#define PTP_OPFF_ByteArray      0x06
#define PTP_OPFF_LongString     0xFF

/* 设备特性获取设置类型(DPGS,Device Property GetSet type)*/
#define PTP_DPGS_Get            0x00
#define PTP_DPGS_GetSet         0x01

typedef struct _PTPParams PTPParams;

typedef u16 (* PTPDataGetFunc) (PTPParams* params, void*priv,
                    unsigned long wantlen, unsigned char *data, unsigned long *gotlen);

typedef u16 (* PTPDataPutFunc) (PTPParams* params, void*priv,
                    unsigned long sendlen, unsigned char *data);

typedef struct _PTPDataHandler {
    PTPDataGetFunc      getfunc;
    PTPDataPutFunc      putfunc;
    void            *priv;
} PTPDataHandler;

/*
 * 这些函数接收面向PTP的参数，相应的执行字节序的转换，并通过适当的数据层发送它们
 */
typedef u16 (* PTPIOSendReq)   (PTPParams* params, PTPContainer* req, int dataphase);
typedef u16 (* PTPIOSendData)  (PTPParams* params, PTPContainer* ptp,
                     u64 size, PTPDataHandler*getter);
typedef u16 (* PTPIOGetResp)   (PTPParams* params, PTPContainer* resp);
typedef u16 (* PTPIOGetData)   (PTPParams* params, PTPContainer* ptp,
                                     PTPDataHandler *putter);
typedef u16 (* PTPIOCancelReq) (PTPParams* params, u32 transaction_id);
typedef u16 (* PTPIODevStatReq) (PTPParams* params);

/* 调试函数 */
typedef void (* PTPErrorFunc) (void *data, const char *format, va_list args)
#if (__GNUC__ >= 3)
    __attribute__((__format__(printf,2,0)))
#endif
;
typedef void (* PTPDebugFunc) (void *data, const char *format, va_list args)
#if (__GNUC__ >= 3)
    __attribute__((__format__(printf,2,0)))
#endif
;

/* PTP对象结构体*/
struct _PTPObject {
    u32    oid;                      /* 对象ID*/
    unsigned int    flags;           /* 标志*/
#define PTPOBJECT_OBJECTINFO_LOADED   (1<<0)
#define PTPOBJECT_CANONFLAGS_LOADED   (1<<1)
#define PTPOBJECT_MTPPROPLIST_LOADED  (1<<2)
#define PTPOBJECT_DIRECTORY_LOADED    (1<<3)
#define PTPOBJECT_PARENTOBJECT_LOADED (1<<4)
#define PTPOBJECT_STORAGEID_LOADED    (1<<5)

    PTPObjectInfo   oi;              /* PTP对象信息*/
    u32    canon_flags;              /* “佳能”标志*/
    MTPProperties   *mtpprops;       /* MTP属性*/
    unsigned int    nrofmtpprops;    /* MTP属性数量*/
};
typedef struct _PTPObject PTPObject;

/* 设备特性缓存 */
struct _PTPDeviceProperty {
    time_t          timestamp;
    PTPDevicePropDesc   desc;
    PTPPropertyValue    value;
};
typedef struct _PTPDeviceProperty PTPDeviceProperty;

/* 传输数据阶段描述，发送请求/传输驱动的内部标志 */
#define PTP_DP_NODATA           0x0000  /* 无数据阶段 */
#define PTP_DP_SENDDATA         0x0001  /* 发送数据 */
#define PTP_DP_GETDATA          0x0002  /* 接收数据 */
#define PTP_DP_DATA_MASK        0x00ff  /* 数据阶段屏蔽位 */

struct _PTPParams {
    /* 设备标志 */
    u32    device_flags;

    /* 数据层字节序 */
    u8     byteorder;
    u16    maxpacketsize;

    /* PTP IO: 自定义IO函数 */
    PTPIOSendReq    sendreq_func;
    PTPIOSendData   senddata_func;
    PTPIOGetResp    getresp_func;
    PTPIOGetData    getdata_func;
    PTPIOGetResp    event_check;
    PTPIOGetResp    event_check_queue;
    PTPIOGetResp    event_wait;
    PTPIOCancelReq  cancelreq_func;
    PTPIODevStatReq devstatreq_func;

    /* 自定义的错误和调试函数 */
    PTPErrorFunc    error_func;
    PTPDebugFunc    debug_func;

    /* 传递给上述函数的数据 */
    void        *data;

    /* ptp 事务 ID */
    u32    transaction_id;
    /* ptp 段 ID */
    u32    session_id;

    /* 用于开放捕获 */
    u32    opencapture_transid;

    /* PTP IO: 如果我们有MTP类型分割头/数据传输 */
    int     split_header_data;
    int     ocs64; /* 64bit 对象大小 */

    /* PTP: 用于PTP驱动的内部结构体*/
    PTPObject   *objects;
    unsigned int    nrofobjects;

    PTPDeviceInfo   deviceinfo;

    /* PTP: 当前事件队列 */
    PTPContainer    *events;
    int     nrofevents;

    /* 已启动列图视表 */
    int         inliveview;

    /* PTP: 属性的缓存时间，默认值2 */
    int         cachetime;

    /* PTP: 存储缓存 */
    PTPStorageIDs       storageids;
    int         storagechanged;

    /* PTP: 设备特性缓存 */
    PTPDeviceProperty   *deviceproperties;
    unsigned int        nrofdeviceproperties;

    /* PTP: Canon 特定标志列表 */
    PTPCanon_Property   *canon_props;
    unsigned int        nrofcanon_props;
    int         canon_viewfinder_on;
    int         canon_event_mode;

    /* PTP: Canon EOS 事件队列 */
    PTPCanon_changes_entry  *backlogentries;
    unsigned int        nrofbacklogentries;
    int         eos_captureenabled;
    int         eos_camerastatus;

    /* PTP: Nikon 特定 */
    int         controlmode;
    int         event90c7works;
    int         deletesdramfails;

    /* PTP: Wifi 配置文件 */
    u8     wifi_profiles_version;
    u8     wifi_profiles_number;
    PTPNIKONWifiProfile *wifi_profiles;

    /* IO: PTP/IP 有关数据 */
    int     cmdfd, evtfd;
    u8     cameraguid[16];
    u32    eventpipeid;
    char        *cameraname;

    /* Olympus UMS wrapping 相关数据*/
    PTPDeviceInfo   outer_deviceinfo;
    char        *olympus_cmd;
    char        *olympus_reply;
    struct _PTPParams *outer_params;

#if defined(HAVE_ICONV) && defined(HAVE_LANGINFO_H)
    /* PTP: iconv converters */
    iconv_t cd_locale_to_ucs2;
    iconv_t cd_ucs2_to_locale;
#endif

    /* IO:有时候响应包也会在数据阶段发送。这现在只会发生在Samsung player*/
    u8     *response_packet;
    u16    response_packet_size;
};

u16 ptp_usb_sendreq    (PTPParams* params, PTPContainer* req, int dataphase);
u16 ptp_usb_senddata   (PTPParams* params, PTPContainer* ptp,
                 u64 size, PTPDataHandler *handler);
u16 ptp_usb_getresp    (PTPParams* params, PTPContainer* resp);
u16 ptp_usb_getdata    (PTPParams* params, PTPContainer* ptp,
                             PTPDataHandler *handler);
u16 ptp_usb_control_cancel_request (PTPParams *params, u32 transid);

/**
 * 检查PTP是否支持某操作
 *
 * @param params     PTP参数
 * @param operation  想要检查是否支持的操作
 *
 * @return 支持返回1，不支持返回0
 **/
static inline int
ptp_operation_issupported(PTPParams* params, u16 operation)
{
    unsigned int i=0;

    for (; i < params->deviceinfo.OperationsSupported_len; i++) {
        if (params->deviceinfo.OperationsSupported[i] == operation)
            return 1;
    }
    return 0;
}

u16 ptp_getdeviceinfo (PTPParams* params, PTPDeviceInfo* deviceinfo);
u16 ptp_generic_no_data (PTPParams* params, u16 opcode, unsigned int cnt, ...);
u16 ptp_opensession (PTPParams *params, u32 session);

/**
 * 关闭会话
 *
 * @param params PTP参数
 * @return 成功返回PTP_RC_OK
 **/
#define ptp_closesession(params) ptp_generic_no_data(params, PTP_OC_CloseSession, 0)

/**
 * PTP复位设备
 *
 * @param params PTP参数
 *
 * @return 成功返回PTP_RC_OK
 */
#define ptp_resetdevice(params) ptp_generic_no_data(params, PTP_OC_ResetDevice, 0)

u16 ptp_getstorageids (PTPParams* params, PTPStorageIDs* storageids);
u16 ptp_getstorageinfo (PTPParams* params, u32 storageid, PTPStorageInfo* storageinfo);

/**
 * PTP格式化存储设备
 *
 * @param params    PTP参数
 * @param storageid 要格式化的存储设备的ID
 *
 * @return 成功返回PTP_RC_OK
 */
#define ptp_formatstore(params,storageid) ptp_generic_no_data(params, PTP_OC_FormatStore, 1, storageid)
u16 ptp_getobjecthandles (PTPParams* params, u32 storage, u32 objectformatcode, u32 associationOH,
            PTPObjectHandles* objecthandles);
u16 ptp_getobject (PTPParams *params, u32 handle,
                unsigned char** object);
u16 ptp_getobject_tofd     (PTPParams* params, u32 handle, int fd);

u16 ptp_deleteobject   (PTPParams* params, u32 handle, u32 ofc);
u16 ptp_moveobject (PTPParams* params, u32 handle,
                         u32 storage, u32 parent);
u16 ptp_copyobject     (PTPParams* params, u32 handle,
                        u32 storage, u32 parent);

u16 ptp_sendobjectinfo (PTPParams* params, u32* store,
                        u32* parenthandle, u32* handle, PTPObjectInfo* objectinfo);
u16 ptp_sendobject (PTPParams* params, unsigned char* object, u64 size);
u16 ptp_sendobject_fromfd (PTPParams* params, int fd, u64 size);


u16 ptp_getdevicepropdesc (PTPParams* params, u16 propcode, PTPDevicePropDesc* devicepropertydesc);
u16 ptp_getdevicepropvalue (PTPParams* params, u16 propcode,
                            PTPPropertyValue* value, u16 datatype);
/* Microsoft MTP extensions */
u16 ptp_mtp_getobjectpropdesc (PTPParams* params, u16 opc, u16 ofc, PTPObjectPropDesc *opd);
u16 ptp_mtp_getobjectpropvalue (PTPParams* params, u32 oid, u16 opc,
                                PTPPropertyValue *value, u16 datatype);
u16 ptp_mtp_setobjectpropvalue (PTPParams* params, u32 oid, u16 opc,
                                PTPPropertyValue *value, u16 datatype);
u16 ptp_mtp_getobjectreferences (PTPParams* params, u32 handle, u32** ohArray, u32* arraylen);
u16 ptp_mtp_setobjectreferences (PTPParams* params, u32 handle, u32* ohArray, u32 arraylen);
u16 ptp_mtp_getobjectproplist (PTPParams* params, u32 handle, MTPProperties **props, int *nrofprops);
u16 ptp_mtp_sendobjectproplist (PTPParams* params, u32* store, u32* parenthandle, u32* handle,
                                u16 objecttype, u64 objectsize, MTPProperties *props, int nrofprops);
u16 ptp_mtp_setobjectproplist (PTPParams* params, MTPProperties *props, int nrofprops);

u16 ptp_mtp_getobjectpropssupported (PTPParams* params, u16 ofc, u32 *propnum, u16 **props);
/* Non PTP protocol functions */
int ptp_property_issupported(PTPParams* params, u16 property);

void ptp_free_params (PTPParams *params);
void ptp_free_objectpropdesc(PTPObjectPropDesc* opd);
void ptp_free_devicepropdesc(PTPDevicePropDesc* dpd);
void ptp_free_devicepropvalue(u16 dt, PTPPropertyValue* dpd);
void ptp_free_object (PTPObject *ob);

const char *ptp_strerror(u16 ret, u16 vendor);
void ptp_debug (PTPParams *params, const char *format, ...);

const char* ptp_get_property_description(PTPParams* params, u16 dpc);

const char* ptp_get_opcode_name(PTPParams* params, u16 opcode);
const char* ptp_get_event_code_name(PTPParams* params, u16 event_code);

int ptp_render_ofc(PTPParams* params, u16 ofc, int spaceleft, char *txt);
MTPProperties *ptp_get_new_object_prop_entry(MTPProperties **props, int *nrofprops);
void ptp_destroy_object_prop(MTPProperties *prop);
void ptp_destroy_object_prop_list(MTPProperties *props, int nrofprops);

MTPProperties *ptp_find_object_prop_in_cache(PTPParams *params, u32 const handle, u32 const attribute_id);
u16 ptp_remove_object_from_cache(PTPParams *params, u32 handle);
u16 ptp_add_object_to_cache(PTPParams *params, u32 handle);
u16 ptp_object_want (PTPParams *params, u32 handle, unsigned int want, PTPObject **retob);
void ptp_objects_sort (PTPParams *params);
u16 ptp_object_find (PTPParams *params, u32 handle, PTPObject **retob);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

