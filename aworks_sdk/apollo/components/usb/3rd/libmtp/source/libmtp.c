/**
 * \file libmtp.c
 *
 * Copyright (C) 2005-2011 Linus Walleij <triad@df.lth.se>
 * Copyright (C) 2005-2008 Richard A. Low <richard@wentnet.com>
 * Copyright (C) 2007 Ted Bullock <tbullock@canada.com>
 * Copyright (C) 2007 Tero Saarni <tero.saarni@gmail.com>
 * Copyright (C) 2008 Florent Mertens <flomertens@gmail.com>
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * This file provides an interface "glue" to the underlying
 * PTP implementation from libgphoto2. It uses some local
 * code to convert from/to UTF-8 (stored in unicode.c/.h)
 * and some small utility functions, mainly for debugging
 * (stored in util.c/.h).
 *
 * The three PTP files (ptp.c, ptp.h and ptp-pack.c) are
 * plain copied from the libhphoto2 codebase.
 *
 * The files libusb-glue.c/.h are just what they say: an
 * interface to libusb for the actual, physical USB traffic.
 */
//#include "config.h"
#include "libmtp.h"
#include "unicode.h"
#include "ptp.h"
#include "libusb-glue.h"
#include "playlist-spl.h"
#include "device-flags.h"
#include "mtp_adapter.h"
#include "pathutils.h"

#include "mtpz.h"
int use_mtpz;


#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>


/* 全局调试等级
 * 我们使用一个标志系统来启动部分日志
 *
 * LIBMTP_DEBUG环境变量为使用MTP库并调用LIBMTP_Init设置调试标志。该值可以用十进制
 * 表示(不能以“0”开头，否则将以八进制表示)，或16进制(必须以“0x”起始)。
 *
 * 值“-1”使能所有的调试标志
 *
 * 例子中的一些实用程序采用命令标志“-d”来使能LIBMTP_DEBUG_PTP和LIBMTP_DEBUG_DATA，和
 * 设置LIBMTP_DEBUG=9一样
 *
 * 标志(通过添加十六进制进行合并)：
 *  0x00 [0000 0000] : LIBMTP_DEBUG_NONE  : 无调试 (默认)
 *  0x01 [0000 0001] : LIBMTP_DEBUG_PTP   : PTP 调试
 *  0x02 [0000 0010] : LIBMTP_DEBUG_PLST  : Playlist 调试
 *  0x04 [0000 0100] : LIBMTP_DEBUG_USB   : USB 调试
 *  0x08 [0000 1000] : LIBMTP_DEBUG_DATA  : USB data 调试
 *
 * 请保持这个列表与libmtp.h同步
 */
int LIBMTP_debug = LIBMTP_DEBUG_NONE;

/* 这是MTP库内部MTP文件类型和libgphoto2/PTP等价定义之间的映射。我们需要这个否则
 * libmtp.h设备也必须依赖ptp.h才能安装，我们不想要这样*/
typedef struct filemap_struct {
    char *description;        /**< 文件类型的文本描述 */
    LIBMTP_filetype_t id;     /**< MTP库内部文件类型 */
    u16 ptp_id;               /**< PTP文件类型ID */
    struct filemap_struct *next;
} filemap_t;

/*
 * 这是MTP库内部MTP特性类型和libgphoto2/PTP等价定义之间的映射。我们需要这个否则
 * libmtp.h设备也必须依赖ptp.h才能安装，我们不想要这样*/
typedef struct propertymap_struct {
    char *description;        /**< 文特性的文本描述 */
    LIBMTP_property_t id;     /**< MTP库内部特性类型 */
    u16 ptp_id;               /**< PTP特性ID */
    struct propertymap_struct *next;
} propertymap_t;

/* 全局变量*/
/* 存储全局文件类型映射表*/
static filemap_t *g_filemap = NULL;
/* 存储全局特性映射表 */
static propertymap_t *g_propertymap = NULL;


static void add_error_to_errorstack(LIBMTP_mtpdevice_t *device,
                                    LIBMTP_error_number_t errornumber,
                                    char const * const error_text);
static void add_ptp_error_to_errorstack(LIBMTP_mtpdevice_t *device,
                                        u16 ptp_error, char const * const error_text);
static void flush_handles(LIBMTP_mtpdevice_t *device);
static void free_storage_list(LIBMTP_mtpdevice_t *device);
static u32 get_writeable_storageid(LIBMTP_mtpdevice_t *device,
                                   u64 fitsize);
static int get_storage_freespace(LIBMTP_mtpdevice_t *device,
                                 LIBMTP_devicestorage_t *storage,
                                 u64 *freespace);
static int check_if_file_fits(LIBMTP_mtpdevice_t *device,
                              LIBMTP_devicestorage_t *storage,
                              u64 const filesize);
static u16 map_libmtp_type_to_ptp_type(LIBMTP_filetype_t intype);
static LIBMTP_filetype_t map_ptp_type_to_libmtp_type(u16 intype);
static LIBMTP_property_t map_ptp_property_to_libmtp_property(u16 intype);
#ifdef HAVE_ICONV
static int get_device_unicode_property(LIBMTP_mtpdevice_t *device,
                                       char **unicstring, u16 property);
#endif
static char *get_iso8601_stamp(void);
static char *get_string_from_object(LIBMTP_mtpdevice_t *device, u32 const object_id,
                                    u16 const attribute_id);
static u64 get_u64_from_object(LIBMTP_mtpdevice_t *device, u32 const object_id,
                               u16 const attribute_id, u64 const value_default);
static u32 get_u32_from_object(LIBMTP_mtpdevice_t *device, u32 const object_id,
                               u16 const attribute_id, u32 const value_default);
static u16 get_u16_from_object(LIBMTP_mtpdevice_t *device, u32 const object_id,
                               u16 const attribute_id, u16 const value_default);
static int set_object_string(LIBMTP_mtpdevice_t *device, u32 const object_id,
                             u16 const attribute_id, char const * const string);
static int set_object_u32(LIBMTP_mtpdevice_t *device, u32 const object_id,
                          u16 const attribute_id, u32 const value);
static int create_new_abstract_list(LIBMTP_mtpdevice_t *device,
                                    char const * const name,
                                    char const * const artist,
                                    char const * const composer,
                                    char const * const genre,
                                    u32 const parenthandle,
                                    u32 const storageid,
                                    u16 const objectformat,
                                    char const * const suffix,
                                    u32 * const newid,
                                    u32 const * const tracks,
                                    u32 const no_tracks);
static int send_file_object_info(LIBMTP_mtpdevice_t *device, LIBMTP_file_t *filedata);
static void add_object_to_cache(LIBMTP_mtpdevice_t *device, u32 object_id);
static void update_metadata_cache(LIBMTP_mtpdevice_t *device, u32 object_id);
static int set_object_filename(LIBMTP_mtpdevice_t *device,
                               u32 object_id,
                               u16 ptp_type,
                               const char **newname);

/**
 * 检查文件名后缀名是不是“.ogg”
 *
 * @param name 文件名字符串
 *
 * @return 返回0表示后缀名不是“.ogg”，返回1表示是
 **/
static int has_ogg_extension(char *name) {
    char *ptype;

    if (name == NULL)
        return 0;
    /* 获取文件后缀名*/
    ptype = strrchr(name,'.');
    if (ptype == NULL)
        return 0;
    /* 检查后缀名是不是“.ogg”*/
    if (!strcasecmp (ptype, ".ogg"))
        return 1;
    return 0;
}

/**
 * 检查文件名后缀名是不是“.flac”
 *
 * @param name 文件名字符串
 *
 * @return 返回0表示后缀名不是“.flac”，返回1表示是
 **/
static int has_flac_extension(char *name) {
    char *ptype;

    if (name == NULL)
        return 0;
    /* 获取文件后缀名*/
    ptype = strrchr(name,'.');
    if (ptype == NULL)
        return 0;
    /* 检查后缀名是不是“.flac”*/
    if (!strcasecmp (ptype, ".flac"))
        return 1;
    return 0;
}


/**
 * 创建一个新的文件映射入口
 * @return 返回一个新的分配好的文件映射入口
 */
static filemap_t *new_filemap_entry(void)
{
    filemap_t *filemap;

    filemap = (filemap_t *)mtp_malloc(sizeof(filemap_t));

    if( filemap != NULL ) {
        filemap->description = NULL;
        filemap->id = LIBMTP_FILETYPE_UNKNOWN;
        filemap->ptp_id = PTP_OFC_Undefined;
        filemap->next = NULL;
    }

    return filemap;
}

/**
 * 注册一个 MTP(媒体传输协议)或PTP(图片传输协议)文件类型用于数据检索
 *
 * @param description 文件类型的文本描述
 * @param id          MTP库内部文件类型ID
 * @param ptp_id      PTP文件类型ID
 * @return  成功返回0，返回其他值则错误
*/
static int register_filetype(char const * const description, LIBMTP_filetype_t const id,
                 u16 const ptp_id){
    filemap_t *new = NULL, *current;

    /* 之前有MTP库文件类型被注册？*/
    current = g_filemap;
    while (current != NULL) {
        if(current->id == id) {
            break;
        }
        current = current->next;
    }

    /* 创建一个入口*/
    if(current == NULL) {
        new = new_filemap_entry();
        if(new == NULL) {
            return 1;
        }

        new->id = id;
        if(description != NULL) {
            /* 拷贝字符串*/
            new->description = mtp_strdup(description);
        }
        new->ptp_id = ptp_id;
        /* 添加入口到链表里*/
        if(g_filemap == NULL) {
            g_filemap = new;
        } else {
            current = g_filemap;
            while (current->next != NULL ) current=current->next;
            current->next = new;
        }
        /* 更新存在的入口*/
    } else {
        if (current->description != NULL) {
            free(current->description);
        }
        current->description = NULL;
        if(description != NULL) {
            current->description = mtp_strdup(description);
        }
        current->ptp_id = ptp_id;
    }
    return 0;
}

/* 初始化文件映射*/
static void init_filemap(void)
{
    register_filetype("Folder", LIBMTP_FILETYPE_FOLDER, PTP_OFC_Association);
    register_filetype("MediaCard", LIBMTP_FILETYPE_MEDIACARD, PTP_OFC_MTP_MediaCard);
    register_filetype("RIFF WAVE file", LIBMTP_FILETYPE_WAV, PTP_OFC_WAV);
    register_filetype("ISO MPEG-1 Audio Layer 3", LIBMTP_FILETYPE_MP3, PTP_OFC_MP3);
    register_filetype("ISO MPEG-1 Audio Layer 2", LIBMTP_FILETYPE_MP2, PTP_OFC_MTP_MP2);
    register_filetype("Microsoft Windows Media Audio", LIBMTP_FILETYPE_WMA, PTP_OFC_MTP_WMA);
    register_filetype("Ogg container format", LIBMTP_FILETYPE_OGG, PTP_OFC_MTP_OGG);
    register_filetype("Free Lossless Audio Codec (FLAC)", LIBMTP_FILETYPE_FLAC, PTP_OFC_MTP_FLAC);
    register_filetype("Advanced Audio Coding (AAC)/MPEG-2 Part 7/MPEG-4 Part 3", LIBMTP_FILETYPE_AAC, PTP_OFC_MTP_AAC);
    register_filetype("MPEG-4 Part 14 Container Format (Audio Emphasis)", LIBMTP_FILETYPE_M4A, PTP_OFC_MTP_M4A);
    register_filetype("MPEG-4 Part 14 Container Format (Audio+Video Emphasis)", LIBMTP_FILETYPE_MP4, PTP_OFC_MTP_MP4);
    register_filetype("Audible.com Audio Codec", LIBMTP_FILETYPE_AUDIBLE, PTP_OFC_MTP_AudibleCodec);
    register_filetype("Undefined audio file", LIBMTP_FILETYPE_UNDEF_AUDIO, PTP_OFC_MTP_UndefinedAudio);
    register_filetype("Microsoft Windows Media Video", LIBMTP_FILETYPE_WMV, PTP_OFC_MTP_WMV);
    register_filetype("Audio Video Interleave", LIBMTP_FILETYPE_AVI, PTP_OFC_AVI);
    register_filetype("MPEG video stream", LIBMTP_FILETYPE_MPEG, PTP_OFC_MPEG);
    register_filetype("Microsoft Advanced Systems Format", LIBMTP_FILETYPE_ASF, PTP_OFC_ASF);
    register_filetype("Apple Quicktime container format", LIBMTP_FILETYPE_QT, PTP_OFC_QT);
    register_filetype("Undefined video file", LIBMTP_FILETYPE_UNDEF_VIDEO, PTP_OFC_MTP_UndefinedVideo);
    register_filetype("JPEG file", LIBMTP_FILETYPE_JPEG, PTP_OFC_EXIF_JPEG);
    register_filetype("JP2 file", LIBMTP_FILETYPE_JP2, PTP_OFC_JP2);
    register_filetype("JPX file", LIBMTP_FILETYPE_JPX, PTP_OFC_JPX);
    register_filetype("JFIF file", LIBMTP_FILETYPE_JFIF, PTP_OFC_JFIF);
    register_filetype("TIFF bitmap file", LIBMTP_FILETYPE_TIFF, PTP_OFC_TIFF);
    register_filetype("BMP bitmap file", LIBMTP_FILETYPE_BMP, PTP_OFC_BMP);
    register_filetype("GIF bitmap file", LIBMTP_FILETYPE_GIF, PTP_OFC_GIF);
    register_filetype("PICT bitmap file", LIBMTP_FILETYPE_PICT, PTP_OFC_PICT);
    register_filetype("Portable Network Graphics", LIBMTP_FILETYPE_PNG, PTP_OFC_PNG);
    register_filetype("Microsoft Windows Image Format", LIBMTP_FILETYPE_WINDOWSIMAGEFORMAT, PTP_OFC_MTP_WindowsImageFormat);
    register_filetype("VCalendar version 1", LIBMTP_FILETYPE_VCALENDAR1, PTP_OFC_MTP_vCalendar1);
    register_filetype("VCalendar version 2", LIBMTP_FILETYPE_VCALENDAR2, PTP_OFC_MTP_vCalendar2);
    register_filetype("VCard version 2", LIBMTP_FILETYPE_VCARD2, PTP_OFC_MTP_vCard2);
    register_filetype("VCard version 3", LIBMTP_FILETYPE_VCARD3, PTP_OFC_MTP_vCard3);
    register_filetype("Undefined Windows executable file", LIBMTP_FILETYPE_WINEXEC, PTP_OFC_MTP_UndefinedWindowsExecutable);
    register_filetype("Text file", LIBMTP_FILETYPE_TEXT, PTP_OFC_Text);
    register_filetype("HTML file", LIBMTP_FILETYPE_HTML, PTP_OFC_HTML);
    register_filetype("XML file", LIBMTP_FILETYPE_XML, PTP_OFC_MTP_XMLDocument);
    register_filetype("DOC file", LIBMTP_FILETYPE_DOC, PTP_OFC_MTP_MSWordDocument);
    register_filetype("XLS file", LIBMTP_FILETYPE_XLS, PTP_OFC_MTP_MSExcelSpreadsheetXLS);
    register_filetype("PPT file", LIBMTP_FILETYPE_PPT, PTP_OFC_MTP_MSPowerpointPresentationPPT);
    register_filetype("MHT file", LIBMTP_FILETYPE_MHT, PTP_OFC_MTP_MHTCompiledHTMLDocument);
    register_filetype("Firmware file", LIBMTP_FILETYPE_FIRMWARE, PTP_OFC_MTP_Firmware);
    register_filetype("Abstract Album file", LIBMTP_FILETYPE_ALBUM, PTP_OFC_MTP_AbstractAudioAlbum);
    register_filetype("Abstract Playlist file", LIBMTP_FILETYPE_PLAYLIST, PTP_OFC_MTP_AbstractAudioVideoPlaylist);
    register_filetype("Undefined filetype", LIBMTP_FILETYPE_UNKNOWN, PTP_OFC_Undefined);
}

/**
 * 返回映射到MTP接口类型的PTP内部接口类型
 *
 * @param intype MTP库接口类型
 *
 * @return 成功返回PTP(libgphoto2)接口类型
 **/
static u16 map_libmtp_type_to_ptp_type(LIBMTP_filetype_t intype)
{
    filemap_t *current;
    /* 获取文件映射表*/
    current = g_filemap;
    /* 寻找对应的格式*/
    while (current != NULL) {
        if(current->id == intype) {
            return current->ptp_id;
        }
        current = current->next;
    }
    // printf("map_libmtp_type_to_ptp_type: unknown filetype.\n");
    return PTP_OFC_Undefined;
}

/**
 * 返回映射到PTP接口类型的MTP内部接口类型
 *
 * @param intype PTP (libgphoto2)接口类型
 *
 * @return 成功返回MTP库接口类型
 **/
static LIBMTP_filetype_t map_ptp_type_to_libmtp_type(u16 intype)
{
    filemap_t *current;
    /* 获取文件映射表*/
    current = g_filemap;
    /* 寻找对应的格式*/
    while (current != NULL) {
        if(current->ptp_id == intype) {
            return current->id;
        }
        current = current->next;
    }
  // printf("map_ptp_type_to_libmtp_type: unknown filetype.\n");
    return LIBMTP_FILETYPE_UNKNOWN;
}


/**
 * 创建一个新的特性映射入口
 * @return 返回一个新的分配好的特性映射入口
 */
static propertymap_t *new_propertymap_entry()
{
    propertymap_t *propertymap;

    propertymap = (propertymap_t *)mtp_malloc(sizeof(propertymap_t));

    if( propertymap != NULL ) {
        propertymap->description = NULL;
        propertymap->id = LIBMTP_PROPERTY_UNKNOWN;
        propertymap->ptp_id = 0;
        propertymap->next = NULL;
    }

    return propertymap;
}


/**
 * 注册一个 MTP(媒体传输协议)或PTP(图片传输协议)特性用于数据检索
 *
 * @param description 特性的文本描述
 * @param id          MTP库内部特性ID
 * @param ptp_id      PTP特性ID
 * @return  成功返回0，返回其他值则错误
*/
static int register_property(char const * const description, LIBMTP_property_t const id,
                 u16 const ptp_id)
{
    propertymap_t *new = NULL, *current;
    /* 之前有MTP库特性被注册？*/
    current = g_propertymap;
    while (current != NULL) {
        if(current->id == id) {
            break;
        }
        current = current->next;
    }
    /* 创建一个入口*/
    if(current == NULL) {
        new = new_propertymap_entry();
        if(new == NULL) {
            return 1;
        }

        new->id = id;
        if(description != NULL) {
            /* 拷贝字符串*/
            new->description = mtp_strdup(description);
        }
        new->ptp_id = ptp_id;

        /* 添加入口到链表里*/
        if(g_propertymap == NULL) {
            g_propertymap = new;
        } else {
            current = g_propertymap;
            while (current->next != NULL ) current=current->next;
            current->next = new;
        }
        /* 更新存在的入口*/
    } else {
        if (current->description != NULL) {
            free(current->description);
        }
        current->description = NULL;
        if(description != NULL) {
            current->description = mtp_strdup(description);
        }
        current->ptp_id = ptp_id;
    }

    return 0;
}


/* 初始化特性映射*/
static void init_propertymap(void)
{
    register_property("Storage ID", LIBMTP_PROPERTY_StorageID, PTP_OPC_StorageID);
    register_property("Object Format", LIBMTP_PROPERTY_ObjectFormat, PTP_OPC_ObjectFormat);
    register_property("Protection Status", LIBMTP_PROPERTY_ProtectionStatus, PTP_OPC_ProtectionStatus);
    register_property("Object Size", LIBMTP_PROPERTY_ObjectSize, PTP_OPC_ObjectSize);
    register_property("Association Type", LIBMTP_PROPERTY_AssociationType, PTP_OPC_AssociationType);
    register_property("Association Desc", LIBMTP_PROPERTY_AssociationDesc, PTP_OPC_AssociationDesc);
    register_property("Object File Name", LIBMTP_PROPERTY_ObjectFileName, PTP_OPC_ObjectFileName);
    register_property("Date Created", LIBMTP_PROPERTY_DateCreated, PTP_OPC_DateCreated);
    register_property("Date Modified", LIBMTP_PROPERTY_DateModified, PTP_OPC_DateModified);
    register_property("Keywords", LIBMTP_PROPERTY_Keywords, PTP_OPC_Keywords);
    register_property("Parent Object", LIBMTP_PROPERTY_ParentObject, PTP_OPC_ParentObject);
    register_property("Allowed Folder Contents", LIBMTP_PROPERTY_AllowedFolderContents, PTP_OPC_AllowedFolderContents);
    register_property("Hidden", LIBMTP_PROPERTY_Hidden, PTP_OPC_Hidden);
    register_property("System Object", LIBMTP_PROPERTY_SystemObject, PTP_OPC_SystemObject);
    register_property("Persistant Unique Object Identifier", LIBMTP_PROPERTY_PersistantUniqueObjectIdentifier, PTP_OPC_PersistantUniqueObjectIdentifier);
    register_property("Sync ID", LIBMTP_PROPERTY_SyncID, PTP_OPC_SyncID);
    register_property("Property Bag", LIBMTP_PROPERTY_PropertyBag, PTP_OPC_PropertyBag);
    register_property("Name", LIBMTP_PROPERTY_Name, PTP_OPC_Name);
    register_property("Created By", LIBMTP_PROPERTY_CreatedBy, PTP_OPC_CreatedBy);
    register_property("Artist", LIBMTP_PROPERTY_Artist, PTP_OPC_Artist);
    register_property("Date Authored", LIBMTP_PROPERTY_DateAuthored, PTP_OPC_DateAuthored);
    register_property("Description", LIBMTP_PROPERTY_Description, PTP_OPC_Description);
    register_property("URL Reference", LIBMTP_PROPERTY_URLReference, PTP_OPC_URLReference);
    register_property("Language Locale", LIBMTP_PROPERTY_LanguageLocale, PTP_OPC_LanguageLocale);
    register_property("Copyright Information", LIBMTP_PROPERTY_CopyrightInformation, PTP_OPC_CopyrightInformation);
    register_property("Source", LIBMTP_PROPERTY_Source, PTP_OPC_Source);
    register_property("Origin Location", LIBMTP_PROPERTY_OriginLocation, PTP_OPC_OriginLocation);
    register_property("Date Added", LIBMTP_PROPERTY_DateAdded, PTP_OPC_DateAdded);
    register_property("Non Consumable", LIBMTP_PROPERTY_NonConsumable, PTP_OPC_NonConsumable);
    register_property("Corrupt Or Unplayable", LIBMTP_PROPERTY_CorruptOrUnplayable, PTP_OPC_CorruptOrUnplayable);
    register_property("Producer Serial Number", LIBMTP_PROPERTY_ProducerSerialNumber, PTP_OPC_ProducerSerialNumber);
    register_property("Representative Sample Format", LIBMTP_PROPERTY_RepresentativeSampleFormat, PTP_OPC_RepresentativeSampleFormat);
    register_property("Representative Sample Sise", LIBMTP_PROPERTY_RepresentativeSampleSize, PTP_OPC_RepresentativeSampleSize);
    register_property("Representative Sample Height", LIBMTP_PROPERTY_RepresentativeSampleHeight, PTP_OPC_RepresentativeSampleHeight);
    register_property("Representative Sample Width", LIBMTP_PROPERTY_RepresentativeSampleWidth, PTP_OPC_RepresentativeSampleWidth);
    register_property("Representative Sample Duration", LIBMTP_PROPERTY_RepresentativeSampleDuration, PTP_OPC_RepresentativeSampleDuration);
    register_property("Representative Sample Data", LIBMTP_PROPERTY_RepresentativeSampleData, PTP_OPC_RepresentativeSampleData);
    register_property("Width", LIBMTP_PROPERTY_Width, PTP_OPC_Width);
    register_property("Height", LIBMTP_PROPERTY_Height, PTP_OPC_Height);
    register_property("Duration", LIBMTP_PROPERTY_Duration, PTP_OPC_Duration);
    register_property("Rating", LIBMTP_PROPERTY_Rating, PTP_OPC_Rating);
    register_property("Track", LIBMTP_PROPERTY_Track, PTP_OPC_Track);
    register_property("Genre", LIBMTP_PROPERTY_Genre, PTP_OPC_Genre);
    register_property("Credits", LIBMTP_PROPERTY_Credits, PTP_OPC_Credits);
    register_property("Lyrics", LIBMTP_PROPERTY_Lyrics, PTP_OPC_Lyrics);
    register_property("Subscription Content ID", LIBMTP_PROPERTY_SubscriptionContentID, PTP_OPC_SubscriptionContentID);
    register_property("Produced By", LIBMTP_PROPERTY_ProducedBy, PTP_OPC_ProducedBy);
    register_property("Use Count", LIBMTP_PROPERTY_UseCount, PTP_OPC_UseCount);
    register_property("Skip Count", LIBMTP_PROPERTY_SkipCount, PTP_OPC_SkipCount);
    register_property("Last Accessed", LIBMTP_PROPERTY_LastAccessed, PTP_OPC_LastAccessed);
    register_property("Parental Rating", LIBMTP_PROPERTY_ParentalRating, PTP_OPC_ParentalRating);
    register_property("Meta Genre", LIBMTP_PROPERTY_MetaGenre, PTP_OPC_MetaGenre);
    register_property("Composer", LIBMTP_PROPERTY_Composer, PTP_OPC_Composer);
    register_property("Effective Rating", LIBMTP_PROPERTY_EffectiveRating, PTP_OPC_EffectiveRating);
    register_property("Subtitle", LIBMTP_PROPERTY_Subtitle, PTP_OPC_Subtitle);
    register_property("Original Release Date", LIBMTP_PROPERTY_OriginalReleaseDate, PTP_OPC_OriginalReleaseDate);
    register_property("Album Name", LIBMTP_PROPERTY_AlbumName, PTP_OPC_AlbumName);
    register_property("Album Artist", LIBMTP_PROPERTY_AlbumArtist, PTP_OPC_AlbumArtist);
    register_property("Mood", LIBMTP_PROPERTY_Mood, PTP_OPC_Mood);
    register_property("DRM Status", LIBMTP_PROPERTY_DRMStatus, PTP_OPC_DRMStatus);
    register_property("Sub Description", LIBMTP_PROPERTY_SubDescription, PTP_OPC_SubDescription);
    register_property("Is Cropped", LIBMTP_PROPERTY_IsCropped, PTP_OPC_IsCropped);
    register_property("Is Color Corrected", LIBMTP_PROPERTY_IsColorCorrected, PTP_OPC_IsColorCorrected);
    register_property("Image Bit Depth", LIBMTP_PROPERTY_ImageBitDepth, PTP_OPC_ImageBitDepth);
    register_property("f Number", LIBMTP_PROPERTY_Fnumber, PTP_OPC_Fnumber);
    register_property("Exposure Time", LIBMTP_PROPERTY_ExposureTime, PTP_OPC_ExposureTime);
    register_property("Exposure Index", LIBMTP_PROPERTY_ExposureIndex, PTP_OPC_ExposureIndex);
    register_property("Display Name", LIBMTP_PROPERTY_DisplayName, PTP_OPC_DisplayName);
    register_property("Body Text", LIBMTP_PROPERTY_BodyText, PTP_OPC_BodyText);
    register_property("Subject", LIBMTP_PROPERTY_Subject, PTP_OPC_Subject);
    register_property("Priority", LIBMTP_PROPERTY_Priority, PTP_OPC_Priority);
    register_property("Given Name", LIBMTP_PROPERTY_GivenName, PTP_OPC_GivenName);
    register_property("Middle Names", LIBMTP_PROPERTY_MiddleNames, PTP_OPC_MiddleNames);
    register_property("Family Name", LIBMTP_PROPERTY_FamilyName, PTP_OPC_FamilyName);
    register_property("Prefix", LIBMTP_PROPERTY_Prefix, PTP_OPC_Prefix);
    register_property("Suffix", LIBMTP_PROPERTY_Suffix, PTP_OPC_Suffix);
    register_property("Phonetic Given Name", LIBMTP_PROPERTY_PhoneticGivenName, PTP_OPC_PhoneticGivenName);
    register_property("Phonetic Family Name", LIBMTP_PROPERTY_PhoneticFamilyName, PTP_OPC_PhoneticFamilyName);
    register_property("Email: Primary", LIBMTP_PROPERTY_EmailPrimary, PTP_OPC_EmailPrimary);
    register_property("Email: Personal 1", LIBMTP_PROPERTY_EmailPersonal1, PTP_OPC_EmailPersonal1);
    register_property("Email: Personal 2", LIBMTP_PROPERTY_EmailPersonal2, PTP_OPC_EmailPersonal2);
    register_property("Email: Business 1", LIBMTP_PROPERTY_EmailBusiness1, PTP_OPC_EmailBusiness1);
    register_property("Email: Business 2", LIBMTP_PROPERTY_EmailBusiness2, PTP_OPC_EmailBusiness2);
    register_property("Email: Others", LIBMTP_PROPERTY_EmailOthers, PTP_OPC_EmailOthers);
    register_property("Phone Number: Primary", LIBMTP_PROPERTY_PhoneNumberPrimary, PTP_OPC_PhoneNumberPrimary);
    register_property("Phone Number: Personal", LIBMTP_PROPERTY_PhoneNumberPersonal, PTP_OPC_PhoneNumberPersonal);
    register_property("Phone Number: Personal 2", LIBMTP_PROPERTY_PhoneNumberPersonal2, PTP_OPC_PhoneNumberPersonal2);
    register_property("Phone Number: Business", LIBMTP_PROPERTY_PhoneNumberBusiness, PTP_OPC_PhoneNumberBusiness);
    register_property("Phone Number: Business 2", LIBMTP_PROPERTY_PhoneNumberBusiness2, PTP_OPC_PhoneNumberBusiness2);
    register_property("Phone Number: Mobile", LIBMTP_PROPERTY_PhoneNumberMobile, PTP_OPC_PhoneNumberMobile);
    register_property("Phone Number: Mobile 2", LIBMTP_PROPERTY_PhoneNumberMobile2, PTP_OPC_PhoneNumberMobile2);
    register_property("Fax Number: Primary", LIBMTP_PROPERTY_FaxNumberPrimary, PTP_OPC_FaxNumberPrimary);
    register_property("Fax Number: Personal", LIBMTP_PROPERTY_FaxNumberPersonal, PTP_OPC_FaxNumberPersonal);
    register_property("Fax Number: Business", LIBMTP_PROPERTY_FaxNumberBusiness, PTP_OPC_FaxNumberBusiness);
    register_property("Pager Number", LIBMTP_PROPERTY_PagerNumber, PTP_OPC_PagerNumber);
    register_property("Phone Number: Others", LIBMTP_PROPERTY_PhoneNumberOthers, PTP_OPC_PhoneNumberOthers);
    register_property("Primary Web Address", LIBMTP_PROPERTY_PrimaryWebAddress, PTP_OPC_PrimaryWebAddress);
    register_property("Personal Web Address", LIBMTP_PROPERTY_PersonalWebAddress, PTP_OPC_PersonalWebAddress);
    register_property("Business Web Address", LIBMTP_PROPERTY_BusinessWebAddress, PTP_OPC_BusinessWebAddress);
    register_property("Instant Messenger Address 1", LIBMTP_PROPERTY_InstantMessengerAddress, PTP_OPC_InstantMessengerAddress);
    register_property("Instant Messenger Address 2", LIBMTP_PROPERTY_InstantMessengerAddress2, PTP_OPC_InstantMessengerAddress2);
    register_property("Instant Messenger Address 3", LIBMTP_PROPERTY_InstantMessengerAddress3, PTP_OPC_InstantMessengerAddress3);
    register_property("Postal Address: Personal: Full", LIBMTP_PROPERTY_PostalAddressPersonalFull, PTP_OPC_PostalAddressPersonalFull);
    register_property("Postal Address: Personal: Line 1", LIBMTP_PROPERTY_PostalAddressPersonalFullLine1, PTP_OPC_PostalAddressPersonalFullLine1);
    register_property("Postal Address: Personal: Line 2", LIBMTP_PROPERTY_PostalAddressPersonalFullLine2, PTP_OPC_PostalAddressPersonalFullLine2);
    register_property("Postal Address: Personal: City", LIBMTP_PROPERTY_PostalAddressPersonalFullCity, PTP_OPC_PostalAddressPersonalFullCity);
    register_property("Postal Address: Personal: Region", LIBMTP_PROPERTY_PostalAddressPersonalFullRegion, PTP_OPC_PostalAddressPersonalFullRegion);
    register_property("Postal Address: Personal: Postal Code", LIBMTP_PROPERTY_PostalAddressPersonalFullPostalCode, PTP_OPC_PostalAddressPersonalFullPostalCode);
    register_property("Postal Address: Personal: Country", LIBMTP_PROPERTY_PostalAddressPersonalFullCountry, PTP_OPC_PostalAddressPersonalFullCountry);
    register_property("Postal Address: Business: Full", LIBMTP_PROPERTY_PostalAddressBusinessFull, PTP_OPC_PostalAddressBusinessFull);
    register_property("Postal Address: Business: Line 1", LIBMTP_PROPERTY_PostalAddressBusinessLine1, PTP_OPC_PostalAddressBusinessLine1);
    register_property("Postal Address: Business: Line 2", LIBMTP_PROPERTY_PostalAddressBusinessLine2, PTP_OPC_PostalAddressBusinessLine2);
    register_property("Postal Address: Business: City", LIBMTP_PROPERTY_PostalAddressBusinessCity, PTP_OPC_PostalAddressBusinessCity);
    register_property("Postal Address: Business: Region", LIBMTP_PROPERTY_PostalAddressBusinessRegion, PTP_OPC_PostalAddressBusinessRegion);
    register_property("Postal Address: Business: Postal Code", LIBMTP_PROPERTY_PostalAddressBusinessPostalCode, PTP_OPC_PostalAddressBusinessPostalCode);
    register_property("Postal Address: Business: Country", LIBMTP_PROPERTY_PostalAddressBusinessCountry, PTP_OPC_PostalAddressBusinessCountry);
    register_property("Postal Address: Other: Full", LIBMTP_PROPERTY_PostalAddressOtherFull, PTP_OPC_PostalAddressOtherFull);
    register_property("Postal Address: Other: Line 1", LIBMTP_PROPERTY_PostalAddressOtherLine1, PTP_OPC_PostalAddressOtherLine1);
    register_property("Postal Address: Other: Line 2", LIBMTP_PROPERTY_PostalAddressOtherLine2, PTP_OPC_PostalAddressOtherLine2);
    register_property("Postal Address: Other: City", LIBMTP_PROPERTY_PostalAddressOtherCity, PTP_OPC_PostalAddressOtherCity);
    register_property("Postal Address: Other: Region", LIBMTP_PROPERTY_PostalAddressOtherRegion, PTP_OPC_PostalAddressOtherRegion);
    register_property("Postal Address: Other: Postal Code", LIBMTP_PROPERTY_PostalAddressOtherPostalCode, PTP_OPC_PostalAddressOtherPostalCode);
    register_property("Postal Address: Other: Counrtry", LIBMTP_PROPERTY_PostalAddressOtherCountry, PTP_OPC_PostalAddressOtherCountry);
    register_property("Organization Name", LIBMTP_PROPERTY_OrganizationName, PTP_OPC_OrganizationName);
    register_property("Phonetic Organization Name", LIBMTP_PROPERTY_PhoneticOrganizationName, PTP_OPC_PhoneticOrganizationName);
    register_property("Role", LIBMTP_PROPERTY_Role, PTP_OPC_Role);
    register_property("Birthdate", LIBMTP_PROPERTY_Birthdate, PTP_OPC_Birthdate);
    register_property("Message To", LIBMTP_PROPERTY_MessageTo, PTP_OPC_MessageTo);
    register_property("Message CC", LIBMTP_PROPERTY_MessageCC, PTP_OPC_MessageCC);
    register_property("Message BCC", LIBMTP_PROPERTY_MessageBCC, PTP_OPC_MessageBCC);
    register_property("Message Read", LIBMTP_PROPERTY_MessageRead, PTP_OPC_MessageRead);
    register_property("Message Received Time", LIBMTP_PROPERTY_MessageReceivedTime, PTP_OPC_MessageReceivedTime);
    register_property("Message Sender", LIBMTP_PROPERTY_MessageSender, PTP_OPC_MessageSender);
    register_property("Activity Begin Time", LIBMTP_PROPERTY_ActivityBeginTime, PTP_OPC_ActivityBeginTime);
    register_property("Activity End Time", LIBMTP_PROPERTY_ActivityEndTime, PTP_OPC_ActivityEndTime);
    register_property("Activity Location", LIBMTP_PROPERTY_ActivityLocation, PTP_OPC_ActivityLocation);
    register_property("Activity Required Attendees", LIBMTP_PROPERTY_ActivityRequiredAttendees, PTP_OPC_ActivityRequiredAttendees);
    register_property("Optional Attendees", LIBMTP_PROPERTY_ActivityOptionalAttendees, PTP_OPC_ActivityOptionalAttendees);
    register_property("Activity Resources", LIBMTP_PROPERTY_ActivityResources, PTP_OPC_ActivityResources);
    register_property("Activity Accepted", LIBMTP_PROPERTY_ActivityAccepted, PTP_OPC_ActivityAccepted);
    register_property("Owner", LIBMTP_PROPERTY_Owner, PTP_OPC_Owner);
    register_property("Editor", LIBMTP_PROPERTY_Editor, PTP_OPC_Editor);
    register_property("Webmaster", LIBMTP_PROPERTY_Webmaster, PTP_OPC_Webmaster);
    register_property("URL Source", LIBMTP_PROPERTY_URLSource, PTP_OPC_URLSource);
    register_property("URL Destination", LIBMTP_PROPERTY_URLDestination, PTP_OPC_URLDestination);
    register_property("Time Bookmark", LIBMTP_PROPERTY_TimeBookmark, PTP_OPC_TimeBookmark);
    register_property("Object Bookmark", LIBMTP_PROPERTY_ObjectBookmark, PTP_OPC_ObjectBookmark);
    register_property("Byte Bookmark", LIBMTP_PROPERTY_ByteBookmark, PTP_OPC_ByteBookmark);
    register_property("Last Build Date", LIBMTP_PROPERTY_LastBuildDate, PTP_OPC_LastBuildDate);
    register_property("Time To Live", LIBMTP_PROPERTY_TimetoLive, PTP_OPC_TimetoLive);
    register_property("Media GUID", LIBMTP_PROPERTY_MediaGUID, PTP_OPC_MediaGUID);
    register_property("Total Bit Rate", LIBMTP_PROPERTY_TotalBitRate, PTP_OPC_TotalBitRate);
    register_property("Bit Rate Type", LIBMTP_PROPERTY_BitRateType, PTP_OPC_BitRateType);
    register_property("Sample Rate", LIBMTP_PROPERTY_SampleRate, PTP_OPC_SampleRate);
    register_property("Number Of Channels", LIBMTP_PROPERTY_NumberOfChannels, PTP_OPC_NumberOfChannels);
    register_property("Audio Bit Depth", LIBMTP_PROPERTY_AudioBitDepth, PTP_OPC_AudioBitDepth);
    register_property("Scan Depth", LIBMTP_PROPERTY_ScanDepth, PTP_OPC_ScanDepth);
    register_property("Audio WAVE Codec", LIBMTP_PROPERTY_AudioWAVECodec, PTP_OPC_AudioWAVECodec);
    register_property("Audio Bit Rate", LIBMTP_PROPERTY_AudioBitRate, PTP_OPC_AudioBitRate);
    register_property("Video Four CC Codec", LIBMTP_PROPERTY_VideoFourCCCodec, PTP_OPC_VideoFourCCCodec);
    register_property("Video Bit Rate", LIBMTP_PROPERTY_VideoBitRate, PTP_OPC_VideoBitRate);
    register_property("Frames Per Thousand Seconds", LIBMTP_PROPERTY_FramesPerThousandSeconds, PTP_OPC_FramesPerThousandSeconds);
    register_property("Key Frame Distance", LIBMTP_PROPERTY_KeyFrameDistance, PTP_OPC_KeyFrameDistance);
    register_property("Buffer Size", LIBMTP_PROPERTY_BufferSize, PTP_OPC_BufferSize);
    register_property("Encoding Quality", LIBMTP_PROPERTY_EncodingQuality, PTP_OPC_EncodingQuality);
    register_property("Encoding Profile", LIBMTP_PROPERTY_EncodingProfile, PTP_OPC_EncodingProfile);
    register_property("Buy flag", LIBMTP_PROPERTY_BuyFlag, PTP_OPC_BuyFlag);
    register_property("Unknown property", LIBMTP_PROPERTY_UNKNOWN, 0);
}

/**
 * 返回映射到PTP接口属性的MTP内部接口属性
 *
 * @param inproperty PTP (libgphoto2)接口属性
 *
 * @return 成功返回MTP库接口属性
 **/
static LIBMTP_property_t map_ptp_property_to_libmtp_property(u16 inproperty)
{
    propertymap_t *current;

    current = g_propertymap;

    while (current != NULL) {
        if(current->ptp_id == inproperty) {
            return current->id;
        }
        current = current->next;
    }
    // printf("map_ptp_type_to_libmtp_type: unknown filetype.\n");
    return LIBMTP_PROPERTY_UNKNOWN;
}


/**
 * 设置调试等级
 *
 * 默认，调试等级设置为‘0’(禁能)。
 */
void LIBMTP_Set_Debug(int level)
{
  if (LIBMTP_debug || level)
    LIBMTP_ERROR("LIBMTP_Set_Debug: Setting debugging level to %d (0x%02x) "
                 "(%s)\n", level, level, level ? "on" : "off");

  LIBMTP_debug = level;
}

/**
 * 初始化MTP库
 * */
void LIBMTP_Init(void){
    /* 设置调试等级，开启全部调试*/
    LIBMTP_Set_Debug(-1);
    /* 初始化文件映射*/
    init_filemap();
    /* 初始化特性映射*/
    init_propertymap();

#ifndef AWORKS
    if (mtpz_loaddata() == -1)
      use_mtpz = 0;
    else
      use_mtpz = 1;
#endif
    return;
}

/**
 * 返回libmtp文件类型的文本描述
 *
 * @param intype libmtp内部文件类型
 *
 * @return 成功返回文件类型的描述文本
 **/
char const * LIBMTP_Get_Filetype_Description(LIBMTP_filetype_t intype)
{
    filemap_t *current;

    current = g_filemap;

    /* 匹配文件类型*/
    while (current != NULL) {
        if(current->id == intype) {
            return current->description;
        }
        current = current->next;
    }

    return "Unknown filetype";
}

/**
 * 返回libmtp属性的文本描述
 *
 * @param inproperty libmtp内部属性
 *
 * @return 成功返回属性的描述文本
 **/
char const * LIBMTP_Get_Property_Description(LIBMTP_property_t inproperty)
{
    propertymap_t *current;

    current = g_propertymap;

    while (current != NULL) {
        if(current->id == inproperty) {
            return current->description;
        }
        current = current->next;
    }

    return "Unknown property";
}

/**
 * 这个函数返回一个新创建ISO 8601 尽可能高精度的当前的时间戳
 * 如果可以，它甚至增加了时区
 */
static char *get_iso8601_stamp(void)
{
    time_t curtime;
    struct tm *loctime;
    char tmp[64];

    curtime = time(NULL);
    loctime = localtime(&curtime);
    strftime (tmp, sizeof(tmp), "%Y%m%dT%H%M%S.0%z", loctime);
    return mtp_strdup(tmp);
}

/**
 * 检索一个对象的字符串
 *
 * @param device       MTP设备
 * @param object_id    对象ID
 * @param attribute_id PTP属性ID
 *
 * @return 成功返回有效的字符串，失败返回NULL
 */
static char *get_string_from_object(LIBMTP_mtpdevice_t *device, u32 const object_id,
                                    u16 const attribute_id)
{
    PTPPropertyValue propval;
    char *retstring = NULL;
    PTPParams *params;
    uint16_t ret;
    MTPProperties *prop;

    if (!device || !object_id)
        return NULL;

    params = (PTPParams *) device->params;
    /* 寻找对应的属性*/
    prop = ptp_find_object_prop_in_cache(params, object_id, attribute_id);
    if (prop) {
        if (prop->propval.str != NULL)
            return mtp_strdup(prop->propval.str);
        else
            return NULL;
    }
    /* 获取属性值*/
    ret = ptp_mtp_getobjectpropvalue(params, object_id, attribute_id, &propval, PTP_DTC_STR);
    if (ret == PTP_RC_OK) {
        if (propval.str != NULL) {
            retstring = (char *) mtp_strdup(propval.str);
            mtp_free(propval.str);
        }
    } else {
        add_ptp_error_to_errorstack(device, ret, "get_string_from_object(): could not get object string.");
    }

    return retstring;
}

/**
 * 从对象属性中检索一个无符号64位整数
 *
 * @param device        MTP设备
 * @param object_id     对象ID
 * @param attribute_id  PTP属性ID
 * @param value_default 失败返回默认值
 *
 * @return 返回值
 **/
static u64 get_u64_from_object(LIBMTP_mtpdevice_t *device, u32 const object_id,
                                    u16 const attribute_id, u64 const value_default)
{
    PTPPropertyValue propval;
    u64 retval = value_default;
    PTPParams *params;
    u16 ret;
    MTPProperties *prop;

    if (!device)
        return value_default;

    params = (PTPParams *) device->params;
    /* 寻找对应的属性*/
    prop = ptp_find_object_prop_in_cache(params, object_id, attribute_id);
    if (prop)
        return prop->propval.u64;
    /* 获取属性值*/
    ret = ptp_mtp_getobjectpropvalue(params, object_id,
                                     attribute_id,
                                     &propval,
                                     PTP_DTC_UINT64);
    if (ret == PTP_RC_OK) {
        retval = propval.u64;
    } else {
        add_ptp_error_to_errorstack(device, ret, "get_u64_from_object(): could not get unsigned 64bit integer from object.");
    }

    return retval;
}

/**
 * 从对象属性中检索一个无符号32位整数
 *
 * @param device        MTP设备
 * @param object_id     对象ID
 * @param attribute_id  PTP属性ID
 * @param value_default 失败返回默认值
 *
 * @return 返回值
 **/
static u32 get_u32_from_object(LIBMTP_mtpdevice_t *device,u32 const object_id,
                               u16 const attribute_id, u32 const value_default){
    PTPPropertyValue propval;
    u32 retval = value_default;
    PTPParams *params;
    u16 ret;
    MTPProperties *prop;

    if (!device)
        return value_default;

    params = (PTPParams *) device->params;
    /* 寻找对应的属性*/
    prop = ptp_find_object_prop_in_cache(params, object_id, attribute_id);
    if (prop)
        return prop->propval.u32;
    /* 获取属性值*/
    ret = ptp_mtp_getobjectpropvalue(params, object_id,
                                     attribute_id,
                                     &propval,
                                     PTP_DTC_UINT32);
    if (ret == PTP_RC_OK) {
        retval = propval.u32;
    } else {
        add_ptp_error_to_errorstack(device, ret, "get_u32_from_object(): could not get unsigned 32bit integer from object.");
    }
    return retval;
}

/**
 * 从对象属性中检索一个无符号16位整数
 *
 * @param device        MTP设备
 * @param object_id     对象ID
 * @param attribute_id  PTP属性ID
 * @param value_default 失败返回默认值
 *
 * @return 返回值
 **/
static u16 get_u16_from_object(LIBMTP_mtpdevice_t *device, u32 const object_id,
                               u16 const attribute_id, u16 const value_default)
{
    PTPPropertyValue propval;
    u16 retval = value_default;
    PTPParams *params;
    u16 ret;
    MTPProperties *prop;

    if (!device)
        return value_default;

    params = (PTPParams *) device->params;

    /* 寻找对应的属性*/
    prop = ptp_find_object_prop_in_cache(params, object_id, attribute_id);
    if (prop)
        return prop->propval.u16;
    /* 获取属性值*/
    ret = ptp_mtp_getobjectpropvalue(params, object_id,
                                     attribute_id,
                                     &propval,
                                     PTP_DTC_UINT16);
    if (ret == PTP_RC_OK) {
        retval = propval.u16;
    } else {
        add_ptp_error_to_errorstack(device, ret, "get_u16_from_object(): could not get unsigned 16bit integer from object.");
    }

    return retval;
}

/**
 * 设置对象属性的字符串
 *
 * @param device       MTP设备
 * @param object_id    对象ID
 * @param attribute_id 要设置的属性ID
 * @param string       要设置的字符串
 *
 * @return 成功返回0，失败返回其他值
 */
static int set_object_string(LIBMTP_mtpdevice_t *device, u32 const object_id,
                             u16 const attribute_id, char const * const string)
{
    PTPPropertyValue propval;
    PTPParams *params;
    u16 ret;
    /* 合法性检查*/
    if (!device || !string)
        return -1;

    params = (PTPParams *) device->params;
    /* 检查是否支持设置对象属性值*/
    if (!ptp_operation_issupported(params,PTP_OC_MTP_SetObjectPropValue)) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "set_object_string(): could not set object string: "
                "PTP_OC_MTP_SetObjectPropValue not supported.");
        return -1;
    }
    propval.str = (char *) string;
    /* 设置对象属性值*/
    ret = ptp_mtp_setobjectpropvalue(params, object_id, attribute_id, &propval, PTP_DTC_STR);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "set_object_string(): could not set object string.");
        return -1;
    }

    return 0;
}

/**
 * 设置一个对象的无符号32位整数属性
 *
 * @param device       相关MTP设备
 * @param object_id    对象ID
 * @param attribute_id 要设置的属性ID
 * @param value        要设置的32位无符号整数
 *
 * @return 成功返回0，失败返回其他值
 */
static int set_object_u32(LIBMTP_mtpdevice_t *device, u32 const object_id,
                          u16 const attribute_id, u32 const value)
{
    PTPPropertyValue propval;
    PTPParams *params;
    u16 ret;

    if (!device)
        return -1;

    params = (PTPParams *) device->params;
    /* 检查是否支持设置对象属性值*/
    if (!ptp_operation_issupported(params,PTP_OC_MTP_SetObjectPropValue)) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "set_object_u32(): could not set unsigned 32bit integer property: "
                    "PTP_OC_MTP_SetObjectPropValue not supported.");
        return -1;
    }
    /* 设置对象属性值*/
    propval.u32 = value;
    ret = ptp_mtp_setobjectpropvalue(params, object_id, attribute_id, &propval, PTP_DTC_UINT32);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "set_object_u32(): could not set unsigned 32bit integer property.");
        return -1;
    }

    return 0;
}

/**
 * 获取MTP设备链表的第一个MTP设备
 *
 * @return 成功返回MTP设备结构体
 */
LIBMTP_mtpdevice_t *LIBMTP_Get_First_Device(void)
{
    LIBMTP_mtpdevice_t *first_device = NULL;
    LIBMTP_raw_device_t *devices;
    int numdevs;
    LIBMTP_error_number_t ret;

    /* 检测当前原始设备并返回MTP设备链表*/
    ret = LIBMTP_Detect_Raw_Devices(&devices, &numdevs);
    if (ret != LIBMTP_ERROR_NONE) {
        return NULL;
    }
    /* 找不到设备*/
    if (devices == NULL || numdevs == 0) {
        mtp_free(devices);
        return NULL;
    }
    /* 获取MTP设备链表的第一个设备*/
    first_device = LIBMTP_Open_Raw_Device(&devices[0]);
    /* 释放链表*/
    mtp_free(devices);
    return first_device;
}


/**
 * 重写调试函数
 * 这样我们可以关掉调试打印
 */
static void
#ifdef __GNUC__
__attribute__((__format__(printf,2,0)))
#endif
LIBMTP_ptp_debug(void *data, const char *format, va_list args)
{
    if ((LIBMTP_debug & LIBMTP_DEBUG_PTP) != 0) {
        vfprintf (stderr, format, args);
        fprintf (stderr, "\n");
        fflush (stderr);
    }
}

/**
 * 重写错误函数
 * 这样我们可以将所有错误等捕获到我们的错误堆栈中
 */
static void
#ifdef __GNUC__
__attribute__((__format__(printf,2,0)))
#endif
LIBMTP_ptp_error(void *data, const char *format, va_list args)
{
    // if (data == NULL) {
    vfprintf (stderr, format, args);
    fflush (stderr);
    /*
    FIXME: find out how we shall get the device here.
    } else {
        PTP_USB *ptp_usb = data;
        LIBMTP_mtpdevice_t *device = ...;
        char buf[2048];

        vsnprintf (buf, sizeof (buf), format, args);
        add_error_to_errorstack(device,
                                LIBMTP_ERROR_PTP_LAYER,
                                buf);
    }
    */
}

/**
 * 分析扩展描述符
 *
 * @param mtpdevice MTP设备结构体
 * @param desc 扩展描述符
 */
static void parse_extension_descriptor(LIBMTP_mtpdevice_t *mtpdevice,
                                       char *desc){
    int start = 0;
    int end = 0;

    /* 在Canon A70为空 */
    if (!desc)
        return;

    /* 描述符用分号分隔 */
    while (end < strlen(desc)) {
        /* 跳过初始的空格 */
        while ((end < strlen(desc)) && (desc[start] == ' ' )) {
            start++;
            end++;
        }
        /* 检测扩展描述符 */
        while ((end < strlen(desc)) && (desc[end] != ';'))
            end++;
        if (end < strlen(desc)) {
            /* 获取扩展描述符内容*/
            char *element = mtp_strndup(desc + start, end - start);
            if (element) {
                int i = 0;
                // printf("  Element: \"%s\"\n", element);

                /* 分析一个扩展描述符 */
                while ((i < strlen(element)) && (element[i] != ':'))
                    i++;
                if (i < strlen(element)) {
                    char *name = mtp_strndup(element, i);
                    int major = 0, minor = 0;

                    /* 扩展版本必须是 MAJOR.MINOR, 但三星只有一个0，所以也要处理 */
                    if ((2 == sscanf(element + i + 1,"%d.%d",&major,&minor)) ||
                                (1 == sscanf(element + i + 1,"%d",&major))) {
                        LIBMTP_device_extension_t *extension;

                        extension = mtp_malloc(sizeof(LIBMTP_device_extension_t));
                        extension->name = name;
                        extension->major = major;
                        extension->minor = minor;
                        extension->next = NULL;
                        /* 插入MTP设备扩展描述符链表*/
                        if (mtpdevice->extensions == NULL) {
                            mtpdevice->extensions = extension;
                        } else {
                            LIBMTP_device_extension_t *tmp = mtpdevice->extensions;
                            while (tmp->next != NULL)
                                tmp = tmp->next;
                            tmp->next = extension;
                        }
                    } else {
                        LIBMTP_ERROR("LIBMTP ERROR: couldnt parse extension %s\n", element);
                    }
                }
                mtp_free(element);
            }
        }
        end++;
        start = end;
    }
}

/**
 * 这个函数从原始设备中打开一个设备。它是在新接口中访问设备的首选方式，在新接口中，当库在某个
 * 设备上工作时，多个设备可以进行一些访问
 *
 * @param rawdevice 打开“真实”设备的原始设备
 * @return 返回一个打开的设备
 */
LIBMTP_mtpdevice_t *LIBMTP_Open_Raw_Device_Uncached(LIBMTP_raw_device_t *rawdevice)
{
    LIBMTP_mtpdevice_t *mtp_device;
    u8 bs = 0;
    PTPParams *current_params;
    PTP_USB *ptp_usb;
    LIBMTP_error_number_t err;
    int i;

    /* 为设备分配一个动态的空间*/
    mtp_device = (LIBMTP_mtpdevice_t *) mtp_malloc(sizeof(LIBMTP_mtpdevice_t));
    /* 检查是否有一个内存分配错误 */
    if(mtp_device == NULL) {
        /* 这里有一个内存分配错误. 我们打算忽略这个设备然后继续*/

        /* TODO: 这个错误语句应该更健壮些 */
        LIBMTP_ERROR("LIBMTP PANIC: connect_usb_devices encountered a memory "
            "allocation error with device %d on bus %d, trying to continue",
            rawdevice->devnum, rawdevice->bus_location);

        return NULL;
    }
    memset(mtp_device, 0, sizeof(LIBMTP_mtpdevice_t));
    /* 默认不缓存*/
    mtp_device->cached = 0;
    /* 创建 PTP params */
    current_params = (PTPParams *) mtp_malloc(sizeof(PTPParams));
    if (current_params == NULL) {
        mtp_free(mtp_device);
        return NULL;
    }
    memset(current_params, 0, sizeof(PTPParams));
    current_params->device_flags = rawdevice->device_entry.device_flags;
    current_params->nrofobjects = 0;
    current_params->cachetime = 2;
    current_params->objects = NULL;
    current_params->response_packet_size = 0;
    current_params->response_packet = NULL;
    /* 之后这将是一个指向PTP_USB的指针 */
    current_params->data = NULL;
    /* 设置 upp 本地调试和错误函数 */
    current_params->debug_func = LIBMTP_ptp_debug;
    current_params->error_func = LIBMTP_ptp_error;
    /* 小端字节序 */
    current_params->byteorder = PTP_DL_LE;
#if defined(HAVE_ICONV) && defined(HAVE_LANGINFO_H)
    current_params->cd_locale_to_ucs2 = iconv_open("UTF-16LE", "UTF-8");
    current_params->cd_ucs2_to_locale = iconv_open("UTF-8", "UTF-16LE");

    if(current_params->cd_locale_to_ucs2 == (iconv_t) -1 ||
            current_params->cd_ucs2_to_locale == (iconv_t) -1) {
        LIBMTP_ERROR("LIBMTP PANIC: Cannot open iconv() converters to/from UCS-2!\n"
                        "Too old stdlibc, glibc and libiconv?\n");
        mtp_free(current_params);
        mtp_free(mtp_device);
        return NULL;
  }
#endif
    mtp_device->params = current_params;
    /* 创建 usbinfo, 同时打开一个会话 */
    err = configure_usb_device(rawdevice, current_params,
                   &mtp_device->usbinfo);
    if (err != LIBMTP_ERROR_NONE) {
        mtp_free(current_params);
        mtp_free(mtp_device);
        return NULL;
    }
    ptp_usb = (PTP_USB*) mtp_device->usbinfo;
    /* 设置参数指针 */
    ptp_usb->params = current_params;
    /* 缓存设备信息以之后使用*/
    if (ptp_getdeviceinfo(current_params, &current_params->deviceinfo) != PTP_RC_OK) {
        LIBMTP_ERROR("LIBMTP PANIC: Unable to read device information on device "
                     "%d on bus %d, trying to continue",
                     rawdevice->devnum, rawdevice->bus_location);

        /* 释放内存 */
        mtp_free(mtp_device->usbinfo);
        mtp_free(mtp_device->params);
        current_params = NULL;
        mtp_free(mtp_device);
        return NULL;
    }

    /* 检查是否是一个PTP设备，是否被标记为MTP设备*/
    if (current_params->deviceinfo.VendorExtensionID != 0x00000006) {
        LIBMTP_ERROR("LIBMTP WARNING: no MTP vendor extension on device "
                     "%d on bus %d", rawdevice->devnum, rawdevice->bus_location);
        LIBMTP_ERROR("LIBMTP WARNING: VendorExtensionID: %08x",
                     current_params->deviceinfo.VendorExtensionID);
        LIBMTP_ERROR("LIBMTP WARNING: VendorExtensionDesc: %s",
                     current_params->deviceinfo.VendorExtensionDesc);
        LIBMTP_ERROR("LIBMTP WARNING: this typically means the device is PTP "
                     "(i.e. a camera) but not an MTP device at all. "
                     "Trying to continue anyway.");
    }
    /* 分析设备信息里的扩展描述符*/
    parse_extension_descriptor(mtp_device,
                               current_params->deviceinfo.VendorExtensionDesc);

    /* Android有很多bug，如果Android遇到强制分配这些错误标志。相同在SONY NWZ Walkmen的设备*/
    {
        LIBMTP_device_extension_t *tmpext = mtp_device->extensions;
        int is_microsoft_com_wpdna = 0;
        int is_android = 0;
        int is_sony_net_wmfu = 0;
        int is_sonyericsson_com_se = 0;

        /* 循环扩展和设置标志 */
        while (tmpext != NULL) {
            if (!strcmp(tmpext->name, "microsoft.com/WPDNA"))
                is_microsoft_com_wpdna = 1;
            if (!strcmp(tmpext->name, "android.com"))
                is_android = 1;
            if (!strcmp(tmpext->name, "sony.net/WMFU"))
                is_sony_net_wmfu = 1;
            if (!strcmp(tmpext->name, "sonyericsson.com/SE"))
                is_sonyericsson_com_se = 1;
            tmpext = tmpext->next;
        }

        /* 检查特定的协议栈 */
        if (is_microsoft_com_wpdna && is_sonyericsson_com_se && !is_android) {
          /*
           * 可以通过提供providing WPDNA，SonyEricsson extension和NO Android extension来
           * 检测Aricent协议栈
           */
            ptp_usb->rawdevice.device_entry.device_flags |= DEVICE_FLAGS_ARICENT_BUGS;
            LIBMTP_INFO("Aricent MTP stack device detected, assigning default bug flags\n");
        }
        else if (is_android) {
            ptp_usb->rawdevice.device_entry.device_flags |= DEVICE_FLAGS_ANDROID_BUGS;
            LIBMTP_INFO("Android device detected, assigning default bug flags\n");
        }
        else if (is_sony_net_wmfu) {
            ptp_usb->rawdevice.device_entry.device_flags |= DEVICE_FLAGS_SONY_NWZ_BUGS;
            LIBMTP_INFO("SONY NWZ device detected, assigning default bug flags\n");
        }
    }
    /* 如果OGG或者FLAC文件类型被标记为"unknown"，检查是否固件被更新以实际支持它了*/
    if (FLAG_OGG_IS_UNKNOWN(ptp_usb)) {
        for (i = 0;i < current_params->deviceinfo.ImageFormats_len;i++) {
            if (current_params->deviceinfo.ImageFormats[i] == PTP_OFC_MTP_OGG) {
                /* 不再是未知，取消标记 */
                ptp_usb->rawdevice.device_entry.device_flags &= ~DEVICE_FLAG_OGG_IS_UNKNOWN;
                break;
            }
        }
    }
    if (FLAG_FLAC_IS_UNKNOWN(ptp_usb)) {
        for (i = 0;i < current_params->deviceinfo.ImageFormats_len;i++) {
            if (current_params->deviceinfo.ImageFormats[i] == PTP_OFC_MTP_FLAC) {
                /* 不再是未知，取消标记 */
                ptp_usb->rawdevice.device_entry.device_flags &= ~DEVICE_FLAG_FLAC_IS_UNKNOWN;
                break;
            }
        }
    }
    /* 确定是否对象大小支持32或64位宽 */
    if (ptp_operation_issupported(current_params,PTP_OC_MTP_GetObjectPropsSupported)) {
        for (i = 0;i < current_params->deviceinfo.ImageFormats_len;i++) {
            PTPObjectPropDesc opd;
            /* 获取对象属性描述符*/
            if (ptp_mtp_getobjectpropdesc(current_params,
                                            PTP_OPC_ObjectSize,
                                            current_params->deviceinfo.ImageFormats[i],
                                            &opd) != PTP_RC_OK) {
                LIBMTP_ERROR("LIBMTP PANIC: "
                       "could not inspect object property description 0x%04x!\n", current_params->deviceinfo.ImageFormats[i]);
            } else {
                if (opd.DataType == PTP_DTC_UINT32) {
                    if (bs == 0) {
                        bs = 32;
                    } else if (bs != 32) {
                        LIBMTP_ERROR("LIBMTP PANIC: "
                                    "different objects support different object sizes!\n");
                        bs = 0;
                        break;
                    }
                } else if (opd.DataType == PTP_DTC_UINT64) {
                    if (bs == 0) {
                        bs = 64;
                    } else if (bs != 64) {
                        LIBMTP_ERROR("LIBMTP PANIC: "
                                "different objects support different object sizes!\n");
                        bs = 0;
                        break;
                    }
                } else {
                    /* 忽略其他大小*/
                    LIBMTP_ERROR("LIBMTP PANIC: "
                                "awkward object size data type: %04x\n", opd.DataType);
                    bs = 0;
                    break;
                }
            }
        }
    }
    if (bs == 0) {
        /* 没有检测到对象的位大小，假设是32位*/
        bs = 32;
    }

    mtp_device->object_bitsize = bs;

    /* 这个设备还没有错误*/
    mtp_device->errorstack = NULL;

    /* 默认最大的电量，如果可以我们会调整它 */
    mtp_device->maximum_battery_level = 100;

    /* 检查设备是否支持读最大电量属性 */
    if(!FLAG_BROKEN_BATTERY_LEVEL(ptp_usb) &&
            ptp_property_issupported( current_params, PTP_DPC_BatteryLevel)) {
      PTPDevicePropDesc dpd;

      /* 尝试读最大电量*/
      if(ptp_getdevicepropdesc(current_params, PTP_DPC_BatteryLevel,
                   &dpd) != PTP_RC_OK) {
          add_error_to_errorstack(mtp_device, LIBMTP_ERROR_CONNECTING,
                    "Unable to read Maximum Battery Level for this "
                    "device even though the device supposedly "
                    "supports this functionality");
      }

      /* TODO: is this appropriate? */
      /* 如果最大电量是0，则使用默认值*/
      if (dpd.FORM.Range.MaximumValue.u8 != 0) {
        mtp_device->maximum_battery_level = dpd.FORM.Range.MaximumValue.u8;
      }

      ptp_free_devicepropdesc(&dpd);
    }
    /* 设置所有默认文件夹为 0xffffffffU (根目录) */
    mtp_device->default_music_folder = 0xffffffffU;
    mtp_device->default_playlist_folder = 0xffffffffU;
    mtp_device->default_picture_folder = 0xffffffffU;
    mtp_device->default_video_folder = 0xffffffffU;
    mtp_device->default_organizer_folder = 0xffffffffU;
    mtp_device->default_zencast_folder = 0xffffffffU;
    mtp_device->default_album_folder = 0xffffffffU;
    mtp_device->default_text_folder = 0xffffffffU;

    /* 设置初始存储信息 */
    mtp_device->storage = NULL;
    /* 获取MTP设备存储信息*/
    if (LIBMTP_Get_Storage(mtp_device, LIBMTP_STORAGE_SORTBY_NOTSORTED) == -1) {
        add_error_to_errorstack(mtp_device,
                  LIBMTP_ERROR_GENERAL,
                  "Get Storage information failed.");
        mtp_device->storage = NULL;
    }

    return mtp_device;
}


/**
 * MTP打开原始设备
 * @param rawdevice MTP原始设备指针
 * @return 成功返回MTP设备指针
 */
LIBMTP_mtpdevice_t *LIBMTP_Open_Raw_Device(LIBMTP_raw_device_t *rawdevice)
{
    /* 打开原始设备*/
    LIBMTP_mtpdevice_t *mtp_device = LIBMTP_Open_Raw_Device_Uncached(rawdevice);

    /* 检查是否是MTPZ设备 */
    if (use_mtpz) {
        LIBMTP_device_extension_t *tmpext = mtp_device->extensions;

        while (tmpext != NULL) {
            if (!strcmp(tmpext->name, "microsoft.com/MTPZ")) {
                LIBMTP_INFO("MTPZ device detected. Authenticating...\n");
                if (PTP_RC_OK == ptp_mtpz_handshake(mtp_device->params)) {
                    LIBMTP_INFO ("(MTPZ) Successfully authenticated with device.\n");
                } else {
                    LIBMTP_INFO ("(MTPZ) Failure - could not authenticate with device.\n");
                }
                break;
            }
            tmpext = tmpext->next;
        }
    }

    /* 设置设备为已缓存*/
    mtp_device->cached = 1;

    /* 然后获取句柄并尝试定位默认文件夹，这样做的副作用是缓存设备中的所有句柄，从而加快以后的操作*/
    flush_handles(mtp_device);
    return mtp_device;
}

/**
 * 获取设备列表中可得的设备的数量
 *
 * @param device_list MTP设备链表
 *
 * @return 成功返回设备链表中设备的数量
 */
u32 LIBMTP_Number_Devices_In_List(LIBMTP_mtpdevice_t *device_list)
{
    u32 numdevices = 0;
    LIBMTP_mtpdevice_t *iter;
    for(iter = device_list; iter != NULL; iter = iter->next)
        numdevices++;

    return numdevices;
}

/**
 * 添加错误到错误栈
 *
 * @param device     MTP设备
 * @param ptp_error  错误编号
 * @param error_text 错误描述文本
 **/
static void add_error_to_errorstack(LIBMTP_mtpdevice_t *device,
                    LIBMTP_error_number_t errornumber,
                    char const * const error_text){
    LIBMTP_error_t *newerror;

    if (device == NULL) {
        LIBMTP_ERROR("LIBMTP PANIC: Trying to add error to a NULL device!\n");
        return;
    }
    /* 申请一个错误信息结构体内存*/
    newerror = (LIBMTP_error_t *) mtp_malloc(sizeof(LIBMTP_error_t));
    newerror->errornumber = errornumber;
    newerror->error_text = mtp_strdup(error_text);
    newerror->next = NULL;
    /* 插入到MTP设备的错误栈链表中*/
    if (device->errorstack == NULL) {
        device->errorstack = newerror;
    } else {
        LIBMTP_error_t *tmp = device->errorstack;

        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = newerror;
    }
}

/**
 * 添加一个PTP层的错误到错误栈中
 *
 * @param device     MTP设备
 * @param ptp_error  错误编号
 * @param error_text 错误描述文本
 **/
static void add_ptp_error_to_errorstack(LIBMTP_mtpdevice_t *device,
                    u16 ptp_error, char const * const error_text){
    PTPParams *params = (PTPParams *) device->params;

    if (device == NULL) {
        LIBMTP_ERROR("LIBMTP PANIC: Trying to add PTP error to a NULL device!\n");
        return;
    } else {
        char outstr[256];
        snprintf(outstr, sizeof(outstr), "PTP Layer error %04x: %s", ptp_error, error_text);
        outstr[sizeof(outstr)-1] = '\0';
        add_error_to_errorstack(device, LIBMTP_ERROR_PTP_LAYER, outstr);

        snprintf(outstr, sizeof(outstr), "Error %04x: %s", ptp_error, ptp_strerror(ptp_error, params->deviceinfo.VendorExtensionID));
        outstr[sizeof(outstr)-1] = '\0';
        add_error_to_errorstack(device, LIBMTP_ERROR_PTP_LAYER, outstr);
    }
}

/**
 * 关闭和释放一个已分配的MTP设备
 *
 * @param device 要释放的MTP设备
 **/
void LIBMTP_Release_Device(LIBMTP_mtpdevice_t *device)
{
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;

    /* 关闭MTP设备*/
    close_device(ptp_usb, params);
    /* 清除错误栈*/
    LIBMTP_Clear_Errorstack(device);
#if defined(HAVE_ICONV) && defined(HAVE_LANGINFO_H)
    iconv_close(params->cd_locale_to_ucs2);
    iconv_close(params->cd_ucs2_to_locale);
#endif
    mtp_free(ptp_usb);
    /* 释放PTP参数*/
    ptp_free_params(params);
    mtp_free(params);
    /* 释放存储链表*/
    free_storage_list(device);
    /* 释放扩展链表*/
    if (device->extensions != NULL) {
        LIBMTP_device_extension_t *tmp = device->extensions;

        while (tmp != NULL) {
            LIBMTP_device_extension_t *next = tmp->next;

            if (tmp->name)
                mtp_free(tmp->name);
            mtp_free(tmp);
            tmp = next;
        }
    }
    mtp_free(device);
}

/**
 * 清除MTP设备错误栈
 *
 * @param device MTP设备
 **/
void LIBMTP_Clear_Errorstack(LIBMTP_mtpdevice_t *device)
{
    if (device == NULL) {
        LIBMTP_ERROR("LIBMTP PANIC: Trying to clear the error stack of a NULL device!\n");
    } else {
        LIBMTP_error_t *tmp = device->errorstack;

        while (tmp != NULL) {
            LIBMTP_error_t *tmp2;

            if (tmp->error_text != NULL) {
                mtp_free(tmp->error_text);
            }
            tmp2 = tmp;
            tmp = tmp->next;
            mtp_free(tmp2);
        }
        device->errorstack = NULL;
    }
}

/**
 * 弹出MTP设备错误栈
 *
 * @param device MTP设备
 **/
void LIBMTP_Dump_Errorstack(LIBMTP_mtpdevice_t *device)
{
    if (device == NULL) {
        LIBMTP_ERROR("LIBMTP PANIC: Trying to dump the error stack of a NULL device!\n");
    } else {
        LIBMTP_error_t *tmp = device->errorstack;

        while (tmp != NULL) {
            if (tmp->error_text != NULL) {
                LIBMTP_ERROR("Error %d: %s\n", tmp->errornumber, tmp->error_text);
            } else {
                LIBMTP_ERROR("Error %d: (unknown)\n", tmp->errornumber);
            }
            tmp = tmp->next;
        }
    }
}

/**
 * 这个命令通过快速目录检索获取所有句柄和内容，此检索可通过获取对象的所有元数据，0xffffffff意味着
 * “所有对象的所有元数据”。这对绝大多数MTP设备(也有例外)都有效，而且速度非常快。如果在获取元数据时出错了，请
 * 检查错误栈。
 *
 * @param device MTP设备
 * @return 成功返回0。
 */
static int get_all_metadata_fast(LIBMTP_mtpdevice_t *device)
{
    PTPParams      *params = (PTPParams *) device->params;
    int        cnt = 0;
    int            i, j, nrofprops;
    u32   lasthandle = 0xffffffff;
    MTPProperties  *props = NULL;
    MTPProperties  *prop;
    u16       ret;
    int            oldtimeout;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;

    /*
     * 以下请求使设备生成设备上每个文件的链表，并在单个响应中返回它
     *
     * 一些速度较慢的设备以及具有非常大的文件系统的设备很容易需要比标准超市值更长的时间
     * 才能返回响应。
     *
     * 临时设置超时以允许使用最广泛的设备
     */
    /* 获取USB超时事件*/
    get_usb_device_timeout(ptp_usb, &oldtimeout);
    /* 重新设置USB超时时间*/
    set_usb_device_timeout(ptp_usb, 60000);
    /* 获取对象属性*/
    ret = ptp_mtp_getobjectproplist(params, 0xffffffff, &props, &nrofprops);
    /* 重新设置USB超时时间*/
    set_usb_device_timeout(ptp_usb, oldtimeout);

    if (ret == PTP_RC_MTP_Specification_By_Group_Unsupported) {
        add_ptp_error_to_errorstack(device, ret, "get_all_metadata_fast(): "
                    "cannot retrieve all metadata for an object on this device.");
        return -1;
    }
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "get_all_metadata_fast(): "
                "could not get proplist of all objects.");
        return -1;
    }
    if (props == NULL && nrofprops != 0) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL,
                "get_all_metadata_fast(): "
                "call to ptp_mtp_getobjectproplist() returned "
                "inconsistent results.");
        return -1;
    }

    /* 我们通过计算ObjectHandle来计算对象数量，每当它改变时，我们得到一个新的对象，当相同时
     * 它只是同一个对象的不同属性*/
    prop = props;
    /* 寻找不同的对象的个数*/
    for (i = 0;i < nrofprops;i++) {
        if (lasthandle != prop->ObjectHandle) {
            cnt++;
            lasthandle = prop->ObjectHandle;
        }
        prop++;
    }
    lasthandle = 0xffffffff;
    params->objects = mtp_calloc (cnt, sizeof(PTPObject));
    prop = props;
    i = -1;
    for (j = 0;j < nrofprops;j++) {
        if (lasthandle != prop->ObjectHandle) {
            if (i >= 0) {
                /* 设置标志为已加载*/
                params->objects[i].flags |= PTPOBJECT_OBJECTINFO_LOADED;
                /* 检查对象的文件名是否为空*/
                if (!params->objects[i].oi.Filename) {
                    /* I have one such file on my Creative (Marcus) */
                    params->objects[i].oi.Filename = mtp_strdup("<null>");
                }
            }
            i++;
            /* 获取对象句柄*/
            lasthandle = prop->ObjectHandle;
            params->objects[i].oid = prop->ObjectHandle;
        }
        /* 设置对象属性*/
        switch (prop->property) {
        case PTP_OPC_ParentObject:
            params->objects[i].oi.ParentObject = prop->propval.u32;
            params->objects[i].flags |= PTPOBJECT_PARENTOBJECT_LOADED;
            break;
        case PTP_OPC_ObjectFormat:
            params->objects[i].oi.ObjectFormat = prop->propval.u16;
            break;
        case PTP_OPC_ObjectSize:
            // We loose precision here, up to 32 bits! However the commands that
            // retrieve metadata for files and tracks will make sure that the
            // PTP_OPC_ObjectSize is read in and duplicated again.
            if (device->object_bitsize == 64) {
                params->objects[i].oi.ObjectCompressedSize = (uint32_t) prop->propval.u64;
            } else {
                params->objects[i].oi.ObjectCompressedSize = prop->propval.u32;
            }
            break;
        case PTP_OPC_StorageID:
            params->objects[i].oi.StorageID = prop->propval.u32;
            params->objects[i].flags |= PTPOBJECT_STORAGEID_LOADED;
            break;
        case PTP_OPC_ObjectFileName:
            if (prop->propval.str != NULL)
                params->objects[i].oi.Filename = mtp_strdup(prop->propval.str);
            break;
        default: {
            MTPProperties *newprops;

            /* 拷贝所有的其他的MTP属性到每一个对象的属性链表中*/
            if (params->objects[i].nrofmtpprops) {
                newprops = mtp_realloc(params->objects[i].mtpprops,
                        (params->objects[i].nrofmtpprops + 1)*sizeof(MTPProperties));
            } else {
                newprops = mtp_calloc(1,sizeof(MTPProperties));
            }
            if (!newprops)
                return 0; /* FIXME: error handling? */
            params->objects[i].mtpprops = newprops;
            memcpy(&params->objects[i].mtpprops[params->objects[i].nrofmtpprops],
                    &props[j],sizeof(props[j]));
            params->objects[i].nrofmtpprops++;
            params->objects[i].flags |= PTPOBJECT_MTPPROPLIST_LOADED;
            break;
            }
        }
        prop++;
    }
    /* 标记最后一个对象 */
    if (i >= 0) {
        params->objects[i].flags |= PTPOBJECT_OBJECTINFO_LOADED;
        params->nrofobjects = i + 1;
    } else {
        params->nrofobjects = 0;
    }
    mtp_free (props);
    /* 排列PTP对象 */
    ptp_objects_sort (params);
    return 0;
}

/**
 * 递归的获取对象
 * 这个函数在设备上的所有目录中递归，从根目录开始，在移动中收集元数据。在某些设备上使用它更好，这些设备
 * 只返回某个目录的数据，而不是获取所有对象的元数据。
 *
 * @param device    MTP设备
 * @param params    PTP参数
 * @param storageid 获取对象的存储的ID
 * @param parent    获取对象的父对象
 *
 * @return 成功返回PTP_RC_OK
 **/
u16 get_handles_recursively(LIBMTP_mtpdevice_t *device,
                    PTPParams *params, u32 storageid, u32 parent){
    PTPObjectHandles currentHandles;
    int i = 0;

    /* 获取对象句柄*/
    u16 ret = ptp_getobjecthandles(params,
                                   storageid,
                                   PTP_GOH_ALL_FORMATS,
                                   parent,
                                   &currentHandles);

    if (ret != PTP_RC_OK) {
        char buf[80];
        sprintf(buf,"get_handles_recursively(): could not get object handles of %08x", parent);
        add_ptp_error_to_errorstack(device, ret, buf);
        return ret;
    }

    if (currentHandles.Handler == NULL || currentHandles.n == 0)
        return PTP_RC_OK;

    /* 现在进入找到的任何子目录*/
    for (i = 0; i < currentHandles.n; i++) {
        PTPObject *ob;
        /* 加载对象信息*/
        ret = ptp_object_want(params,currentHandles.Handler[i],
                PTPOBJECT_OBJECTINFO_LOADED, &ob);
        if (ret == PTP_RC_OK) {
            /* 是个目录*/
            if (ob->oi.ObjectFormat == PTP_OFC_Association)
                /* 继续遍历目录下的对象*/
                get_handles_recursively(device, params,
                        storageid, currentHandles.Handler[i]);
        } else {
            add_error_to_errorstack(device,
                    LIBMTP_ERROR_CONNECTING,
                    "Found a bad handle, trying to ignore it.");
            return ret;
        }
    }
    mtp_free(currentHandles.Handler);
    return PTP_RC_OK;
}

/**
 * 每当设备中存储的项被改变时，此函数用于刷新内部句柄链表。对于不添加或移除对象的操作，一般不调用
 * 这个函数。
 *
 * @param device 使用的MTP设备
 **/
static void flush_handles(LIBMTP_mtpdevice_t *device)
{
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    int ret;
    u32 i;
    /* 检查设备是否已经缓存了*/
    if (!device->cached) {
        return;
    }
    /* PTP参数已经存在对象*/
    if (params->objects != NULL) {
        /* 释放对象*/
        for (i = 0;i < params->nrofobjects;i++)
            ptp_free_object (&params->objects[i]);
        mtp_free(params->objects);
        params->objects = NULL;
        params->nrofobjects = 0;
    }
    /* 支持“获取对象属性”操作*/
    if (ptp_operation_issupported(params,PTP_OC_MTP_GetObjPropList)
            && !FLAG_BROKEN_MTPGETOBJPROPLIST(ptp_usb)
            && !FLAG_BROKEN_MTPGETOBJPROPLIST_ALL(ptp_usb)) {
        /* 使用最快的方式，先户两侧返回值*/
        ret = get_all_metadata_fast(device);
    }

    /* 如果之前的操作失败或返回没有对象，则使用正常的方法*/
    if (params->nrofobjects == 0) {
        /* 使用通用的命令获取所有句柄*/
        if (device->storage == NULL) {
            /* 获取所有存储的根目录下的对象*/
            get_handles_recursively(device, params,
                    PTP_GOH_ALL_STORAGE,
                    PTP_GOH_ROOT_PARENT);
        } else {
            /* 依次获取每个存储的句柄*/
            LIBMTP_devicestorage_t *storage = device->storage;
            while(storage != NULL) {
                /* 获取主要存储的根目录下的对象*/
                get_handles_recursively(device, params,
                        storage->id,
                        PTP_GOH_ROOT_PARENT);
                storage = storage->next;
            }
        }
    }

    /* 遍历句柄，修复任何空文件名或空密码，然后在主要的存储设备的根文件目录下定位一些默认的文件夹*/
    for(i = 0; i < params->nrofobjects; i++) {
        PTPObject *ob, *xob;

        ob = &params->objects[i];
        /* 执行加载对象信息操作*/
        ret = ptp_object_want(params,params->objects[i].oid,
                PTPOBJECT_OBJECTINFO_LOADED, &xob);
        if (ret != PTP_RC_OK) {
            LIBMTP_ERROR("broken! %x not found\n", params->objects[i].oid);
        }
        if (ob->oi.Filename == NULL)
            ob->oi.Filename = mtp_strdup("<null>");
        if (ob->oi.Keywords == NULL)
            ob->oi.Keywords = mtp_strdup("<null>");

        /* 忽略非目录的对象 */
        if(ob->oi.ObjectFormat != PTP_OFC_Association)
            continue;
        /* 只在根文件目录下 */
        if (ob->oi.ParentObject == 0xffffffffU) {
            LIBMTP_ERROR("object %x has parent 0xffffffff (-1) continuing anyway\n",
                        ob->oid);
        } else if (ob->oi.ParentObject != 0x00000000U)
            continue;
        /* 只使用主要的存储设备 */
        if (device->storage != NULL && ob->oi.StorageID != device->storage->id)
            continue;

        /* 设置默认文件夹 */
        if (!strcasecmp(ob->oi.Filename, "My Music") ||
                !strcasecmp(ob->oi.Filename, "My_Music") ||
                !strcasecmp(ob->oi.Filename, "Music")) {
            device->default_music_folder = ob->oid;
        }
        else if (!strcasecmp(ob->oi.Filename, "My Playlists") ||
                !strcasecmp(ob->oi.Filename, "My_Playlists") ||
                !strcasecmp(ob->oi.Filename, "Playlists")) {
            device->default_playlist_folder = ob->oid;
        }
        else if (!strcasecmp(ob->oi.Filename, "My Pictures") ||
                !strcasecmp(ob->oi.Filename, "My_Pictures") ||
                !strcasecmp(ob->oi.Filename, "Pictures")) {
            device->default_picture_folder = ob->oid;
        }
        else if (!strcasecmp(ob->oi.Filename, "My Video") ||
                !strcasecmp(ob->oi.Filename, "My_Video") ||
                !strcasecmp(ob->oi.Filename, "Video")) {
            device->default_video_folder = ob->oid;
        }
        else if (!strcasecmp(ob->oi.Filename, "My Organizer") ||
                !strcasecmp(ob->oi.Filename, "My_Organizer")) {
            device->default_organizer_folder = ob->oid;
        }
        else if (!strcasecmp(ob->oi.Filename, "ZENcast") ||
                !strcasecmp(ob->oi.Filename, "Datacasts")) {
            device->default_zencast_folder = ob->oid;
        }
        else if (!strcasecmp(ob->oi.Filename, "My Albums") ||
                !strcasecmp(ob->oi.Filename, "My_Albums") ||
                !strcasecmp(ob->oi.Filename, "Albums")) {
            device->default_album_folder = ob->oid;
        }
        else if (!strcasecmp(ob->oi.Filename, "Text") ||
                !strcasecmp(ob->oi.Filename, "Texts")) {
            device->default_text_folder = ob->oid;
        }
    }
}


/**
 * 释放设备的存储信息链表
 *
 * @param device 要释放存储信息链表的MTP设备指针
 */
static void free_storage_list(LIBMTP_mtpdevice_t *device)
{
    LIBMTP_devicestorage_t *storage;
    LIBMTP_devicestorage_t *tmp;

    /* 获取存储信息链表*/
    storage = device->storage;
    while(storage != NULL) {
        /* 释放存储信息描述*/
        if (storage->StorageDescription != NULL) {
            mtp_free(storage->StorageDescription);
        }
        /* 释放卷ID*/
        if (storage->VolumeIdentifier != NULL) {
            mtp_free(storage->VolumeIdentifier);
        }
        /* 获取下一个存储信息*/
        tmp = storage;
        storage = storage->next;
        /* 释放储存信息*/
        mtp_free(tmp);
    }
  device->storage = NULL;

  return;
}

/*
 * 按照sortby方式排列设备存储信息
 *
 * @param device MTP设备
 * @param sortby 排列方式(由大到小)
 *
 * @return 成功返回0
 **/
static int sort_storage_by(LIBMTP_mtpdevice_t *device, int const sortby)
{
    LIBMTP_devicestorage_t *oldhead, *ptr1, *ptr2, *newlist;

    /* 合法性检查*/
    if (device->storage == NULL)
        return -1;
    /* 分类类型为“不分类”*/
    if (sortby == LIBMTP_STORAGE_SORTBY_NOTSORTED)
        return 0;

    oldhead = ptr1 = ptr2 = device->storage;

    newlist = NULL;

    while(oldhead != NULL) {
        ptr1 = ptr2 = oldhead;
        /* 找到“空闲空间”或“最大空间”最大的存储信息结构体*/
        while(ptr1 != NULL) {
            /* 按照空闲空间分类*/
            if (sortby == LIBMTP_STORAGE_SORTBY_FREESPACE && ptr1->FreeSpaceInBytes > ptr2->FreeSpaceInBytes)
                ptr2 = ptr1;
            /* 按照最大空间分类*/
            if (sortby == LIBMTP_STORAGE_SORTBY_MAXSPACE && ptr1->FreeSpaceInBytes > ptr2->FreeSpaceInBytes)
                ptr2 = ptr1;

            ptr1 = ptr1->next;
        }

        /* 把找到的存储信息节点的前一个节点的next指针指向后一个节点*/
        if(ptr2->prev != NULL) {
            ptr1 = ptr2->prev;
            ptr1->next = ptr2->next;
        } else {
            oldhead = ptr2->next;
            if(oldhead != NULL)
                oldhead->prev = NULL;
        }

        /* 把找到的存储信息节点的后一个节点的prev指针指向前一个节点*/
        ptr1 = ptr2->next;
        if(ptr1 != NULL) {
            ptr1->prev = ptr2->prev;
        } else {
            ptr1 = ptr2->prev;
            if(ptr1 != NULL)
                ptr1->next = NULL;
        }
        /* 插到新的链表中去*/
        if(newlist == NULL) {
            newlist = ptr2;
            newlist->prev = NULL;
        } else {
            ptr2->prev = newlist;
            newlist->next = ptr2;
            newlist = newlist->next;
        }
    }
    /* 把新的链表重新赋值给MTP设备*/
    if (newlist != NULL) {
        newlist->next = NULL;
        while(newlist->prev != NULL)
            newlist = newlist->prev;
        device->storage = newlist;
    }

    return 0;
}

/**
 * 这个函数从设备存储链表中寻找第一个可写的存储设备
 *
 * @param device  MTP设备
 * @param fitsize 文件大小
 *
 * @return 成功返回存储设备的ID
 **/
static u32 get_writeable_storageid(LIBMTP_mtpdevice_t *device,
                                   u64 fitsize){
    LIBMTP_devicestorage_t *storage;
    /* 应该是0xffffffffu？*/
    u32 store = 0x00000000;
    int subcall_ret;

    /* 看是否有存储设备可以适合这个文件大小*/
    storage = device->storage;
    if (storage == NULL) {
        /* 有时候存储设备检测不到*/
        store = 0x00000000U;
    } else {
        while(storage != NULL) {
            /* 这些存储设备不能使用*/
            if (storage->StorageType == PTP_ST_FixedROM ||
                    storage->StorageType == PTP_ST_RemovableROM) {
                storage = storage->next;
                continue;
            }
            /* 存储设备ID的低16位0x0000不应该是可写的*/
            if ((storage->id & 0x0000FFFFU) == 0x00000000U) {
                storage = storage->next;
                continue;
            }
            /* 检查存储设备的访问权限*/
            if (storage->AccessCapability == PTP_AC_ReadOnly ||
                    storage->AccessCapability == PTP_AC_ReadOnly_with_Object_Deletion) {
                storage = storage->next;
                continue;
            }
            /* 检查是否适合这个文件的大小*/
            subcall_ret = check_if_file_fits(device, storage, fitsize);
            if (subcall_ret != 0) {
                storage = storage->next;
            } else {
                /* 找到一个可写且适合这个文件大小的存储设备*/
                break;
            }
        }
        if (storage == NULL) {
            add_error_to_errorstack(device, LIBMTP_ERROR_STORAGE_FULL,
                  "get_writeable_storageid(): "
                  "all device storage is full or corrupt.");
            return -1;
        }
        store = storage->id;
    }
    return store;
}

/**
 * 尝试通过父目录ID(如果有)去获取父目录的存储设备ID
 *
 * @param device    MTP设备
 * @param fitsize   文件大小
 * @param parent_id 父目录ID
 *
 * @return 成功返回存储设备ID
 **/
static int get_suggested_storage_id(LIBMTP_mtpdevice_t *device,
                                    u64 fitsize, u32 parent_id){
    PTPParams *params = (PTPParams *) device->params;
    PTPObject *ob;
    u16 ret;

    /* 通过父目录ID寻找父目录的对象*/
    ret = ptp_object_want(params, parent_id, PTPOBJECT_MTPPROPLIST_LOADED, &ob);
    if ((ret != PTP_RC_OK) || (ob->oi.StorageID == 0)) {
        add_ptp_error_to_errorstack(device, ret, "get_suggested_storage_id(): "
                "could not get storage id from parent id.");
        return get_writeable_storageid(device, fitsize);
    } else {
        /* 返回存储设备ID */
        return ob->oi.StorageID;
    }
}

/**
 * 从设备存储链表中一个确定的存储设备中获取剩余空间
 *
 * @param device    MTP设备
 * @param storage   MTP设备中的存储设备
 * @param freespace 要返回的剩余存储空间
 *
 * @return 成功返回0
 **/
static int get_storage_freespace(LIBMTP_mtpdevice_t *device,
                                 LIBMTP_devicestorage_t *storage,
                                 u64 *freespace){
    PTPParams *params = (PTPParams *) device->params;

    /* 总是询问存储设备信息，因为有一些设备型号明确需要，我们在查询存储设备信息时刷新所有数据*/
    if (ptp_operation_issupported(params,PTP_OC_GetStorageInfo)) {
        PTPStorageInfo storageInfo;
        u16 ret;
        /* 获取存储设备信息*/
        ret = ptp_getstorageinfo(params, storage->id, &storageInfo);
        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret,
                    "get_storage_freespace(): could not get storage info.");
            return -1;
        }
        if (storage->StorageDescription != NULL) {
            mtp_free(storage->StorageDescription);
        }
        if (storage->VolumeIdentifier != NULL) {
            mtp_free(storage->VolumeIdentifier);
        }
        storage->StorageType = storageInfo.StorageType;
        storage->FilesystemType = storageInfo.FilesystemType;
        storage->AccessCapability = storageInfo.AccessCapability;
        storage->MaxCapacity = storageInfo.MaxCapability;
        storage->FreeSpaceInBytes = storageInfo.FreeSpaceInBytes;
        storage->FreeSpaceInObjects = storageInfo.FreeSpaceInImages;
        storage->StorageDescription = storageInfo.StorageDescription;
        storage->VolumeIdentifier = storageInfo.VolumeLabel;
    }
    /* 检查剩余空间大小是否合法*/
    if(storage->FreeSpaceInBytes == (uint64_t) -1)
        return -1;
    *freespace = storage->FreeSpaceInBytes;
    return 0;
}

/**
 * 此函数用于打印从PTP协议提供的大量文本信息，以及额外一些MTP特定信息(如果适用)。
 *
 * @param device 想上报信息的MTP设备
 **/
void LIBMTP_Dump_Device_Info(LIBMTP_mtpdevice_t *device)
{
    int i;
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    LIBMTP_devicestorage_t *storage = device->storage;
    LIBMTP_device_extension_t *tmpext = device->extensions;

    mtp_printf("USB low-level info:\r\n");
    dump_usbinfo(ptp_usb);
    /* 打印一些详细信息 */
    mtp_printf("Device info:\r\n");
    mtp_printf("   Manufacturer: %s\r\n", params->deviceinfo.Manufacturer);
    mtp_printf("   Model: %s\r\n", params->deviceinfo.Model);
    mtp_printf("   Device version: %s\r\n", params->deviceinfo.DeviceVersion);
    mtp_printf("   Serial number: %s\r\n", params->deviceinfo.SerialNumber);
    mtp_printf("   Vendor extension ID: 0x%08x\r\n", params->deviceinfo.VendorExtensionID);
    mtp_printf("   Vendor extension description: %s\r\n", params->deviceinfo.VendorExtensionDesc);
    mtp_printf("   Detected object size: %d bits\r\n", device->object_bitsize);
    mtp_printf("   Extensions:\r\n");
    while (tmpext != NULL) {
        mtp_printf("        %s: %d.%d\r\n", tmpext->name, tmpext->major, tmpext->minor);
        tmpext = tmpext->next;
    }
    mtp_printf("Supported operations:\r\n");
    for (i = 0;i < params->deviceinfo.OperationsSupported_len;i++)
        mtp_printf("   %04x: %s\r\n", params->deviceinfo.OperationsSupported[i], ptp_get_opcode_name(params, params->deviceinfo.OperationsSupported[i]));
    mtp_printf("Events supported:\r\n");
    if (params->deviceinfo.EventsSupported_len == 0) {
        mtp_printf("   None.\r\n");
    } else {
        for (i = 0;i < params->deviceinfo.EventsSupported_len;i++) {
            mtp_printf("   0x%04x: %s\r\n", params->deviceinfo.EventsSupported[i], ptp_get_event_code_name(params, params->deviceinfo.EventsSupported[i]));
        }
    }
    mtp_printf("Device Properties Supported:\n");
    for (i = 0;i < params->deviceinfo.DevicePropertiesSupported_len;i++) {
        char const *propdesc = ptp_get_property_description(params,
            params->deviceinfo.DevicePropertiesSupported[i]);

        if (propdesc != NULL) {
            mtp_printf("   0x%04x: %s\r\n",
                    params->deviceinfo.DevicePropertiesSupported[i], propdesc);
        } else {
            u16 prop = params->deviceinfo.DevicePropertiesSupported[i];
            mtp_printf("   0x%04x: Unknown property\r\n", prop);
        }
    }

    if (ptp_operation_issupported(params,PTP_OC_MTP_GetObjectPropsSupported)) {
        mtp_printf("Playable File (Object) Types and Object Properties Supported:\r\n");
        for (i = 0;i < params->deviceinfo.ImageFormats_len;i++) {
            char txt[256];
            u16 ret;
            u16 *props = NULL;
            u32 propcnt = 0;
            int j;

            (void) ptp_render_ofc (params, params->deviceinfo.ImageFormats[i],
                    sizeof(txt), txt);
            mtp_printf("   %04x: %s\r\n", params->deviceinfo.ImageFormats[i], txt);

            ret = ptp_mtp_getobjectpropssupported (params,
                    params->deviceinfo.ImageFormats[i], &propcnt, &props);
            if (ret != PTP_RC_OK) {
                add_ptp_error_to_errorstack(device, ret, "LIBMTP_Dump_Device_Info(): "
                        "error on query for object properties.");
            } else {
                for (j = 0;j < propcnt;j++) {
                    PTPObjectPropDesc opd;
                    int k;

                    mtp_printf("      %04x: %s", props[j],
                            LIBMTP_Get_Property_Description(map_ptp_property_to_libmtp_property(props[j])));
                    /* 获取再详细一点的描述*/
                    ret = ptp_mtp_getobjectpropdesc(params, props[j],
                            params->deviceinfo.ImageFormats[i], &opd);
                    if (ret != PTP_RC_OK) {
                        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL,
                                "LIBMTP_Dump_Device_Info(): "
                                "could not get property description.");
                        break;
                    }

                    if (opd.DataType == PTP_DTC_STR) {
                        mtp_printf(" STRING data type");
                        switch (opd.FormFlag) {
                        case PTP_OPFF_DateTime:
                            mtp_printf(" DATETIME FORM");
                            break;
                        case PTP_OPFF_RegularExpression:
                            mtp_printf(" REGULAR EXPRESSION FORM");
                            break;
                        case PTP_OPFF_LongString:
                            mtp_printf(" LONG STRING FORM");
                            break;
                        default:
                            break;
                        }
                    } else {
                        if (opd.DataType & PTP_DTC_ARRAY_MASK) {
                            mtp_printf(" array of");
                        }

                        switch (opd.DataType & (~PTP_DTC_ARRAY_MASK)) {

                        case PTP_DTC_UNDEF:
                            mtp_printf(" UNDEFINED data type");
                            break;
                        case PTP_DTC_INT8:
                            mtp_printf(" INT8 data type");
                            switch (opd.FormFlag) {
                            case PTP_OPFF_Range:
                                mtp_printf(" range: MIN %d, MAX %d, STEP %d",
                                        opd.FORM.Range.MinimumValue.i8,
                                        opd.FORM.Range.MaximumValue.i8,
                                        opd.FORM.Range.StepSize.i8);
                                break;
                            case PTP_OPFF_Enumeration:
                                mtp_printf(" enumeration: ");
                                for(k  =0;k < opd.FORM.Enum.NumberOfValues;k++) {
                                    mtp_printf("%d, ", opd.FORM.Enum.SupportedValue[k].i8);
                                }
                                break;
                            case PTP_OPFF_ByteArray:
                                mtp_printf(" byte array: ");
                                break;
                            default:
                                mtp_printf(" ANY 8BIT VALUE form");
                                break;
                            }
                            break;

                            case PTP_DTC_UINT8:
                                mtp_printf(" UINT8 data type");
                                switch (opd.FormFlag) {
                                case PTP_OPFF_Range:
                                    mtp_printf(" range: MIN %d, MAX %d, STEP %d",
                                            opd.FORM.Range.MinimumValue.u8,
                                            opd.FORM.Range.MaximumValue.u8,
                                            opd.FORM.Range.StepSize.u8);
                                    break;
                                case PTP_OPFF_Enumeration:
                                    mtp_printf(" enumeration: ");
                                    for(k = 0;k < opd.FORM.Enum.NumberOfValues;k++) {
                                        mtp_printf("%d, ", opd.FORM.Enum.SupportedValue[k].u8);
                                    }
                                    break;
                                case PTP_OPFF_ByteArray:
                                    mtp_printf(" byte array: ");
                                    break;
                                default:
                                    mtp_printf(" ANY 8BIT VALUE form");
                                    break;
                                }
                                break;

                                case PTP_DTC_INT16:
                                    mtp_printf(" INT16 data type");
                                    switch (opd.FormFlag) {
                                    case PTP_OPFF_Range:
                                        mtp_printf(" range: MIN %d, MAX %d, STEP %d",
                                                opd.FORM.Range.MinimumValue.i16,
                                                opd.FORM.Range.MaximumValue.i16,
                                                opd.FORM.Range.StepSize.i16);
                                        break;
                                    case PTP_OPFF_Enumeration:
                                        mtp_printf(" enumeration: ");
                                        for(k = 0;k < opd.FORM.Enum.NumberOfValues;k++) {
                                            mtp_printf("%d, ", opd.FORM.Enum.SupportedValue[k].i16);
                                        }
                                        break;
                                    default:
                                        mtp_printf(" ANY 16BIT VALUE form");
                                        break;
                                    }
                                    break;

                                    case PTP_DTC_UINT16:
                                        mtp_printf(" UINT16 data type");
                                        switch (opd.FormFlag) {
                                        case PTP_OPFF_Range:
                                            mtp_printf(" range: MIN %d, MAX %d, STEP %d",
                                                    opd.FORM.Range.MinimumValue.u16,
                                                    opd.FORM.Range.MaximumValue.u16,
                                                    opd.FORM.Range.StepSize.u16);
                                            break;
                                        case PTP_OPFF_Enumeration:
                                            mtp_printf(" enumeration: ");
                                            for(k = 0;k < opd.FORM.Enum.NumberOfValues;k++) {
                                                mtp_printf("%d, ", opd.FORM.Enum.SupportedValue[k].u16);
                                            }
                                            break;
                                        default:
                                            mtp_printf(" ANY 16BIT VALUE form");
                                            break;
                                        }
                                        break;

                                        case PTP_DTC_INT32:
                                            mtp_printf(" INT32 data type");
                                            switch (opd.FormFlag) {
                                            case PTP_OPFF_Range:
                                                mtp_printf(" range: MIN %d, MAX %d, STEP %d",
                                                        opd.FORM.Range.MinimumValue.i32,
                                                        opd.FORM.Range.MaximumValue.i32,
                                                        opd.FORM.Range.StepSize.i32);
                                                break;
                                            case PTP_OPFF_Enumeration:
                                                mtp_printf(" enumeration: ");
                                                for(k = 0;k < opd.FORM.Enum.NumberOfValues;k++) {
                                                    mtp_printf("%d, ", opd.FORM.Enum.SupportedValue[k].i32);
                                                }
                                                break;
                                            default:
                                                mtp_printf(" ANY 32BIT VALUE form");
                                                break;
                                            }
                                            break;

                                            case PTP_DTC_UINT32:
                                                mtp_printf(" UINT32 data type");
                                                switch (opd.FormFlag) {
                                                case PTP_OPFF_Range:
                                                    mtp_printf(" range: MIN %u, MAX %u, STEP %u",
                                                            opd.FORM.Range.MinimumValue.u32,
                                                            opd.FORM.Range.MaximumValue.u32,
                                                            opd.FORM.Range.StepSize.u32);
                                                    break;
                                                case PTP_OPFF_Enumeration:
                                                    // Special pretty-print for FOURCC codes
                                                    if (params->deviceinfo.ImageFormats[i] == PTP_OPC_VideoFourCCCodec) {
                                                        mtp_printf(" enumeration of u32 casted FOURCC: ");
                                                        for (k = 0;k < opd.FORM.Enum.NumberOfValues;k++) {
                                                            if (opd.FORM.Enum.SupportedValue[k].u32 == 0) {
                                                                mtp_printf("ANY, ");
                                                            } else {
                                                                char fourcc[6];
                                                                fourcc[0] = (opd.FORM.Enum.SupportedValue[k].u32 >> 24) & 0xFFU;
                                                                fourcc[1] = (opd.FORM.Enum.SupportedValue[k].u32 >> 16) & 0xFFU;
                                                                fourcc[2] = (opd.FORM.Enum.SupportedValue[k].u32 >> 8) & 0xFFU;
                                                                fourcc[3] = opd.FORM.Enum.SupportedValue[k].u32 & 0xFFU;
                                                                fourcc[4] = '\n';
                                                                fourcc[5] = '\0';
                                                                mtp_printf("\"%s\", ", fourcc);
                                                            }
                                                        }
                                                    }else {
                                                        mtp_printf(" enumeration: ");
                                                        for(k = 0;k < opd.FORM.Enum.NumberOfValues;k++) {
                                                            mtp_printf("%u, ", opd.FORM.Enum.SupportedValue[k].u32);
                                                        }
                                                    }
                                                    break;
                                                default:
                                                    mtp_printf(" ANY 32BIT VALUE form");
                                                    break;
                                                }
                                                break;

                                                case PTP_DTC_INT64:
                                                    mtp_printf(" INT64 data type");
                                                    break;

                                                case PTP_DTC_UINT64:
                                                    mtp_printf(" UINT64 data type");
                                                    break;

                                                case PTP_DTC_INT128:
                                                    mtp_printf(" INT128 data type");
                                                    break;

                                                case PTP_DTC_UINT128:
                                                    mtp_printf(" UINT128 data type");
                                                    break;

                                                default:
                                                    mtp_printf(" UNKNOWN data type");
                                                    break;
                        }
                    }
                    if (opd.GetSet) {
                        mtp_printf(" GET/SET");
                    } else {
                        mtp_printf(" READ ONLY");
                    }
                    mtp_printf(" GROUP 0x%x", opd.GroupCode);

                    mtp_printf("\r\n");
                    ptp_free_objectpropdesc(&opd);
                }
                mtp_free(props);
            }
        }
    }

    if(storage != NULL &&
            ptp_operation_issupported(params,PTP_OC_GetStorageInfo)) {
        mtp_printf("Storage Devices:\r\n");
        while(storage != NULL) {
            mtp_printf("   StorageID: 0x%08x\r\n",storage->id);
            mtp_printf("      StorageType: 0x%04x ",storage->StorageType);
            switch (storage->StorageType) {
            case PTP_ST_Undefined:
                mtp_printf("(undefined)\r\n");
                break;
            case PTP_ST_FixedROM:
                mtp_printf("fixed ROM storage\r\n");
                break;
            case PTP_ST_RemovableROM:
                mtp_printf("removable ROM storage\r\n");
                break;
            case PTP_ST_FixedRAM:
                mtp_printf("fixed RAM storage\r\n");
                break;
            case PTP_ST_RemovableRAM:
                mtp_printf("removable RAM storage\r\n");
                break;
            default:
                mtp_printf("UNKNOWN storage\r\n");
                break;
            }
            mtp_printf("      FilesystemType: 0x%04x ",storage->FilesystemType);
            switch(storage->FilesystemType) {
            case PTP_FST_Undefined:
                mtp_printf("(undefined)\r\n");
                break;
            case PTP_FST_GenericFlat:
                mtp_printf("generic flat filesystem\r\n");
                break;
            case PTP_FST_GenericHierarchical:
                mtp_printf("generic hierarchical\r\n");
                break;
            case PTP_FST_DCF:
                mtp_printf("DCF\r\n");
                break;
            default:
                mtp_printf("UNKNONWN filesystem type\r\n");
                break;
            }
            mtp_printf("      AccessCapability: 0x%04x ",storage->AccessCapability);
            switch(storage->AccessCapability) {
            case PTP_AC_ReadWrite:
                mtp_printf("read/write\r\n");
                break;
            case PTP_AC_ReadOnly:
                mtp_printf("read only\r\n");
                break;
            case PTP_AC_ReadOnly_with_Object_Deletion:
                mtp_printf("read only + object deletion\r\n");
                break;
            default:
                mtp_printf("UNKNOWN access capability\r\n");
                break;
            }
            mtp_printf("      MaxCapacity: %llu\r\n",
                    (long long unsigned int) storage->MaxCapacity);
            mtp_printf("      FreeSpaceInBytes: %llu\r\n",
                    (long long unsigned int) storage->FreeSpaceInBytes);
            mtp_printf("      FreeSpaceInObjects: %llu\r\n",
                    (long long unsigned int) storage->FreeSpaceInObjects);
            mtp_printf("      StorageDescription: %s\r\n",storage->StorageDescription);
            mtp_printf("      VolumeIdentifier: %s\r\n",storage->VolumeIdentifier);
            storage = storage->next;
        }
    }

    mtp_printf("Special directories:\r\n");
    mtp_printf("   Default music folder: 0x%08x\r\n", device->default_music_folder);
    mtp_printf("   Default playlist folder: 0x%08x\r\n", device->default_playlist_folder);
    mtp_printf("   Default picture folder: 0x%08x\r\n", device->default_picture_folder);
    mtp_printf("   Default video folder: 0x%08x\r\n", device->default_video_folder);
    mtp_printf("   Default organizer folder: 0x%08x\r\n",device->default_organizer_folder);
    mtp_printf("   Default zencast folder: 0x%08x\r\n",device->default_zencast_folder);
    mtp_printf("   Default album folder: 0x%08x\r\n",device->default_album_folder);
    mtp_printf("   Default text folder: 0x%08x\r\n", device->default_text_folder);
}

/**
 * 复位设备
 *
 * @param device 要复位的MTP设备
 *
 * @return 成功返回0，失败返回其他值
 **/
int LIBMTP_Reset_Device(LIBMTP_mtpdevice_t *device)
{
    PTPParams *params = (PTPParams *) device->params;
    u16 ret;

    /* 不支持复位设备*/
    if (!ptp_operation_issupported(params,PTP_OC_ResetDevice)) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL,
                                "LIBMTP_Reset_Device(): "
                                "device does not support resetting.");
        return -1;
    }
    /* PTP复位设备*/
    ret = ptp_resetdevice(params);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "Error resetting.");
        return -1;
    }
    return 0;
}

/**
 * 这将检索MTP的“友好名称”设备。通常这只是所有者的名字，或者类似于“John Doe's Digital Audio Player”。
 * 这个属性应该被所有MTP设备所支持。
 *
 * @param device MTP设备
 *
 * @return 成功返回一个新分配的代表友好名称的UTF-8格式的字符串
 */
char *LIBMTP_Get_Friendlyname(LIBMTP_mtpdevice_t *device)
{
    PTPPropertyValue propval;
    char *retstring = NULL;
    PTPParams *params = (PTPParams *) device->params;
    u16 ret;

    if (!ptp_property_issupported(params, PTP_DPC_MTP_DeviceFriendlyName)) {
        return NULL;
    }
    /* 获取设备属性值*/
    ret = ptp_getdevicepropvalue(params,
                                 PTP_DPC_MTP_DeviceFriendlyName,
                                 &propval,
                                 PTP_DTC_STR);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "Error getting friendlyname.");
        return NULL;
    }
    if (propval.str != NULL) {
        retstring = mtp_strdup(propval.str);
        mtp_free(propval.str);
    }
    return retstring;
}

/**
 * 这将检索MTP设备的同步伙伴。这个属性应该被所有MTP设备支持。
 *
 * @param device 要检索的MTP设备
 *
 * @return 一个新分配的代表同步伙伴的UTF-8字符串，这个在使用后必须被调用者释放
 **/
char *LIBMTP_Get_Syncpartner(LIBMTP_mtpdevice_t *device)
{
    PTPPropertyValue propval;
    char *retstring = NULL;
    PTPParams *params = (PTPParams *) device->params;
    u16 ret;
    /* 检查是否支持*/
    if (!ptp_property_issupported(params, PTP_DPC_MTP_SynchronizationPartner)) {
        return NULL;
    }
    /* 获取设备属性值*/
    ret = ptp_getdevicepropvalue(params,
                                 PTP_DPC_MTP_SynchronizationPartner,
                                 &propval,
                                 PTP_DTC_STR);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "Error getting syncpartner.");
        return NULL;
    }
    if (propval.str != NULL) {
        retstring = mtp_strdup(propval.str);
        free(propval.str);
    }
    return retstring;
}

/**
 * 检查设备是否可以存储一个文件
 *
 * @param device   MTP设备
 * @param storage  要检查的存储设备
 * @param filesize 文件大小
 *
 * @return
 **/
static int check_if_file_fits(LIBMTP_mtpdevice_t *device,
                              LIBMTP_devicestorage_t *storage,
                              u64 const filesize) {
    PTPParams *params = (PTPParams *) device->params;
    u64 freebytes;
    int ret;

    /* 检查是否可以获取存储设备信息*/
    if (!ptp_operation_issupported(params,PTP_OC_GetStorageInfo)) {
        return 0;
    }
    /* 获取存储设备剩余空间*/
    ret = get_storage_freespace(device, storage, &freebytes);
    if (ret != 0) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL,
                "check_if_file_fits(): error checking free storage.");
        return -1;
    } else {
        /* 检查剩余空间是否满足文件大小*/
        if (filesize > freebytes) {
            return -1;
        }
    }
    return 0;
}

/**
 * 这个函数检索设备的当前电量
 *
 * @param 要检索的MTP设备
 * @param 要返回的最大电量
 * @param 要返回的当前电量
 *
 * @return 成功返回0，其他值表示失败，失败的一般原因是设备不支持电量属性
 **/
int LIBMTP_Get_Batterylevel(LIBMTP_mtpdevice_t *device,
                            u8 * const maximum_level,
                            u8 * const current_level){
    PTPPropertyValue propval;
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;

    *maximum_level = 0;
    *current_level = 0;

    if (FLAG_BROKEN_BATTERY_LEVEL(ptp_usb) ||
            !ptp_property_issupported(params, PTP_DPC_BatteryLevel)) {
        return -1;
    }
    /* 获取上设备属性值*/
    ret = ptp_getdevicepropvalue(params, PTP_DPC_BatteryLevel,
                                 &propval, PTP_DTC_UINT8);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret,
                    "LIBMTP_Get_Batterylevel(): "
                    "could not get device property value.");
        return -1;
    }

    *maximum_level = device->maximum_battery_level;
    *current_level = propval.u8;

    return 0;
}

/**
 * 格式化存储设备
 *
 * @param device  MTP设备
 * @param storage 要格式化的存储设备的ID
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Format_Storage(LIBMTP_mtpdevice_t *device,
                          LIBMTP_devicestorage_t *storage)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    /* 检查是否支持格式化存储设备*/
    if (!ptp_operation_issupported(params,PTP_OC_FormatStore)) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL,
                "LIBMTP_Format_Storage(): "
                "device does not support formatting storage.");
        return -1;
    }
    /* 格式化存储设备*/
    ret = ptp_formatstore(params, storage->id);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Format_Storage(): "
                "failed to format storage.");
        return -1;
    }
    return 0;
}

#ifdef HAVE_ICONV
/**
 * 从设备中提取Unicode属性的函数。这是检索PTP规范中描述的Unicode设备属性的标准方法。
 *
 * @param device      要获取属性的设备
 * @param unicstring  要返回的属性
 * @param property    要检索的属性
 *
 * @return 成功返回0，失败返回其他值
 **/
static int get_device_unicode_property(LIBMTP_mtpdevice_t *device,
                                       char **unicstring, u16 property){
    PTPPropertyValue propval;
    PTPParams *params = (PTPParams *) device->params;
    u16 *tmp;
    u16 ret;
    int i;

    if (!ptp_property_issupported(params, property)) {
      return -1;
    }

    /* Unicode字符串是16位无符号整数数组*/
    ret = ptp_getdevicepropvalue(params,
                                 property,
                                 &propval,
                                 PTP_DTC_AUINT16);
    if (ret != PTP_RC_OK) {
        // TODO: add a note on WHICH property that we failed to get.
        *unicstring = NULL;
        add_ptp_error_to_errorstack(device, ret,
                    "get_device_unicode_property(): "
                    "failed to get unicode property.");
        return -1;
    }

    /* 提取实际的数组*/
    // printf("Array of %d elements\n", propval.a.count);
    tmp = mtp_malloc((propval.a.count + 1) * sizeof(u16));
    for (i = 0; i < propval.a.count; i++) {
        tmp[i] = propval.a.v[i].u16;
        // printf("%04x ", tmp[i]);
    }
    tmp[propval.a.count] = 0x0000U;
    mtp_free(propval.a.v);

    *unicstring = utf16_to_utf8(device, tmp);

    mtp_free(tmp);

    return 0;
}

#endif

/**
 * 这个函数以XML文件字符串的形式返回设备的安全时间
 *
 * @param device  要获取安全时间的MTP设备
 * @param sectime 要返回的安全时间字符串，失败返回空
 *
 * @return 成功返回0，失败返回其他值。
 **/
//int LIBMTP_Get_Secure_Time(LIBMTP_mtpdevice_t *device, char ** const sectime)
//{
//    return get_device_unicode_property(device, sectime, PTP_DPC_MTP_SecureTime);
//}

/**
 * 这个函数将设备(公钥)证书作为XML文档字符串从设备返回
 *
 * @param device  要获取设备证书的MTP设备
 * @param devcert 要返回的设备证书字符串，失败返回空
 *
 * @return 成功返回0，失败返回其他值。
 **/
//int LIBMTP_Get_Device_Certificate(LIBMTP_mtpdevice_t *device, char ** const devcert)
//{
//    return get_device_unicode_property(device, devcert, PTP_DPC_MTP_DeviceCertificate);
//}


/**
 * 这个函数检索了支持的文件类型，即设备声称支持的文件类型。例如设备可以播放的音频文件等等。这个列表
 * 被简化到包含libmtp可以处理的文件类型，即它不会列出libmtp将在内部处理的文件类型，如播放列表和
 * 文件夹。
 *
 * @param device    要检索的MTP设备
 * @param filetypes 要返回的支持的文件类型的指针
 * @param length    要返回的链表的长度
 *
 * @return 成功返回0，失败返回其他值
 **/
int LIBMTP_Get_Supported_Filetypes(LIBMTP_mtpdevice_t *device, u16 ** const filetypes,
                                   u16 * const length){
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    u16 *localtypes;
    u16 localtypelen;
    u32 i;

    /* 如果是未知类型，这里需要申请更多的内存*/
    localtypes = (u16 *) mtp_malloc(params->deviceinfo.ImageFormats_len * sizeof(u16));
    localtypelen = 0;

    for (i = 0;i < params->deviceinfo.ImageFormats_len;i++) {
        u16 localtype = map_ptp_type_to_libmtp_type(params->deviceinfo.ImageFormats[i]);
        if (localtype != LIBMTP_FILETYPE_UNKNOWN) {
            localtypes[localtypelen] = localtype;
            localtypelen++;
        }
    }
    // The forgotten Ogg support on YP-10 and others...
    if (FLAG_OGG_IS_UNKNOWN(ptp_usb)) {
        uint16_t *tmp = (u16 *) mtp_realloc(localtypes, (params->deviceinfo.ImageFormats_len + 1) * sizeof(u16));
        if (tmp == NULL)
            return -ENOMEM;
        localtypes = tmp;
        localtypes[localtypelen] = LIBMTP_FILETYPE_OGG;
        localtypelen++;
    }
    // The forgotten FLAC support on Cowon iAudio S9 and others...
    if (FLAG_FLAC_IS_UNKNOWN(ptp_usb)) {
        uint16_t *tmp = (u16 *) mtp_realloc(localtypes, (params->deviceinfo.ImageFormats_len + 1) * sizeof(u16));
        if (tmp == NULL)
            return -ENOMEM;
        localtypes = tmp;
        localtypes[localtypelen] = LIBMTP_FILETYPE_FLAC;
        localtypelen++;
    }

    *filetypes = localtypes;
    *length = localtypelen;

    return 0;
}

/**
 * 获取或更新MTP设备的存储信息
 *
 * @param device 想要获取存储信息的MTP设备指针
 * @param sortby 用libmtp.h定义的LIBMTP_STORAGE_SORTBY_. 0 或 LIBMTP_STORAGE_SORTBY_NOTSORTED
 *               排序
 *
 * @return 成功返回0，返回1指明只能获取存储ID不能获取存储属性，返回-1指明失败
 */
int LIBMTP_Get_Storage(LIBMTP_mtpdevice_t *device, int const sortby)
{
    u32 i = 0;
    PTPStorageInfo storageInfo;
    PTPParams *params = (PTPParams *) device->params;
    PTPStorageIDs storageIDs;
    LIBMTP_devicestorage_t *storage = NULL;
    LIBMTP_devicestorage_t *storageprev = NULL;

    /* MTP设备存在存数信息，释放掉*/
    if (device->storage != NULL)
        free_storage_list(device);

    //if (!ptp_operation_issupported(params,PTP_OC_GetStorageIDs))
    //    return -1;
    /* 获取存储ID*/
    if (ptp_getstorageids (params, &storageIDs) != PTP_RC_OK)
        return -1;
    /* 检查存储信息的数量*/
    if (storageIDs.n < 1)
        return -1;
    /* 检查是否支持“获取存储信息”操作*/
    if (!ptp_operation_issupported(params,PTP_OC_GetStorageInfo)) {
        for (i = 0; i < storageIDs.n; i++) {
            /* 申请内存*/
            storage = (LIBMTP_devicestorage_t *)mtp_malloc(sizeof(LIBMTP_devicestorage_t));
            storage->prev = storageprev;
            if (storageprev != NULL)
                storageprev->next = storage;
            if (device->storage == NULL)
                device->storage = storage;
            /* 不支持获取存储信息，初始化存储信息*/
            storage->id = storageIDs.Storage[i];
            storage->StorageType = PTP_ST_Undefined;
            storage->FilesystemType = PTP_FST_Undefined;
            storage->AccessCapability = PTP_AC_ReadWrite;
            storage->MaxCapacity = (u64) -1;
            storage->FreeSpaceInBytes = (u64) -1;
            storage->FreeSpaceInObjects = (u64) -1;
            storage->StorageDescription = mtp_strdup("Unknown storage");
            storage->VolumeIdentifier = mtp_strdup("Unknown volume");
            storage->next = NULL;

            storageprev = storage;
        }
        mtp_free(storageIDs.Storage);
        return 1;
    } else {
        for (i = 0; i < storageIDs.n; i++) {
            u16 ret;
            /* 获取存储信息*/
            ret = ptp_getstorageinfo(params, storageIDs.Storage[i], &storageInfo);
            if (ret != PTP_RC_OK) {
                /* 添加错误到错误栈*/
                add_ptp_error_to_errorstack(device, ret, "LIBMTP_Get_Storage(): "
                                  "Could not get storage info.");
                if (device->storage != NULL) {
                    /* 释放存储信息链表*/
                    free_storage_list(device);
                }
                return -1;
            }
            /* 申请MTP设备存储信息结构体内存*/
            storage = (LIBMTP_devicestorage_t *)mtp_malloc(sizeof(LIBMTP_devicestorage_t));
            storage->prev = storageprev;
            if (storageprev != NULL)
                storageprev->next = storage;
            if (device->storage == NULL)
                device->storage = storage;
            /* 设置MTP设备存储信息结构体*/
            storage->id = storageIDs.Storage[i];
            storage->StorageType = storageInfo.StorageType;
            storage->FilesystemType = storageInfo.FilesystemType;
            storage->AccessCapability = storageInfo.AccessCapability;
            storage->MaxCapacity = storageInfo.MaxCapability;
            storage->FreeSpaceInBytes = storageInfo.FreeSpaceInBytes;
            storage->FreeSpaceInObjects = storageInfo.FreeSpaceInImages;
            storage->StorageDescription = storageInfo.StorageDescription;
            storage->VolumeIdentifier = storageInfo.VolumeLabel;
            storage->next = NULL;

            storageprev = storage;
        }

        if (storage != NULL)
            storage->next = NULL;
        /* 排列存储信息*/
        sort_storage_by(device, sortby);
        mtp_free(storageIDs.Storage);
        return 0;
    }
}

/**
 * 创建一个新文件元数据结构体
 *
 * @return 成功返回新分配的文件结构体
 **/
LIBMTP_file_t *LIBMTP_new_file_t(void)
{
    LIBMTP_file_t *new = (LIBMTP_file_t *) mtp_malloc(sizeof(LIBMTP_file_t));
    if (new == NULL) {
        return NULL;
    }
    new->filename = NULL;
    new->item_id = 0;
    new->parent_id = 0;
    new->storage_id = 0;
    new->filesize = 0;
    new->modificationdate = 0;
    new->filetype = LIBMTP_FILETYPE_UNKNOWN;
    new->next = NULL;
    return new;
}

/**
 * 销毁一个文件
 *
 * @param file 要销毁的文件元数据
 **/
void LIBMTP_destroy_file_t(LIBMTP_file_t *file)
{
    if (file == NULL) {
        return;
    }
    if (file->filename != NULL)
        mtp_free(file->filename);
    mtp_free(file);
    return;
}

/**
 * 把对象转换为文件数据结构体
 *
 * @param device MTP设备
 * @param ob     对象结构体
 *
 * @return 成功返回MTP文件结构体
 */
static LIBMTP_file_t *obj2file(LIBMTP_mtpdevice_t *device, PTPObject *ob)
{
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    LIBMTP_file_t *file;
    int i;

    /* 申请一个新的文件内存*/
    file = LIBMTP_new_file_t();

    file->parent_id = ob->oi.ParentObject;
    file->storage_id = ob->oi.StorageID;
    /* 获取文件名*/
    if (ob->oi.Filename != NULL) {
        file->filename = mtp_strdup(ob->oi.Filename);
    }

    /* 设置文件类型*/
    file->filetype = map_ptp_type_to_libmtp_type(ob->oi.ObjectFormat);

    /* 一个特殊的设备兼容，一些文件被标记为“未知”类型，其实是OGG或FLAC文件。检查文件后缀名*/
    /* 未知文件类型*/
    if (file->filetype == LIBMTP_FILETYPE_UNKNOWN) {
        /* 检查文件类型是不是“.ogg”*/
        if ((FLAG_IRIVER_OGG_ALZHEIMER(ptp_usb) ||
                FLAG_OGG_IS_UNKNOWN(ptp_usb)) &&
                has_ogg_extension(file->filename)) {
            file->filetype = LIBMTP_FILETYPE_OGG;
        }
        /* 检查文件类型是不是“.flac”*/
        if (FLAG_FLAC_IS_UNKNOWN(ptp_usb) && has_flac_extension(file->filename)) {
            file->filetype = LIBMTP_FILETYPE_FLAC;
        }
    }

    /* 设置文件最后修改日期*/
    file->modificationdate = ob->oi.ModificationDate;

    /* 这里只有32位文件大小，之后我们使用PTP_OPC_ObjectSize属性*/
    file->filesize = ob->oi.ObjectCompressedSize;

    /* 文件ID，由于追踪这个文件*/
    file->item_id = ob->oid;

    /* 如果我们有缓存元数据，使用它*/
    if (ob->mtpprops) {
        MTPProperties *prop = ob->mtpprops;

        for (i = 0; i < ob->nrofmtpprops; i++, prop++) {
            /* 选择对象大小*/
            if (prop->property == PTP_OPC_ObjectSize) {
                /* 这里可能已经被设置，但64位精度值比PTP32位值更好，所以重写它*/
                if (device->object_bitsize == 64) {
                    file->filesize = prop->propval.u64;
                } else {
                    file->filesize = prop->propval.u32;
                }
                break;
            }
        }
    } else if (ptp_operation_issupported(params, PTP_OC_MTP_GetObjectPropsSupported)) {
        u16 *props = NULL;
        u32 propcnt = 0;
        int ret;

        /* 首先查看可以为此对象格式检索哪些属性*/
        ret = ptp_mtp_getobjectpropssupported(params, map_libmtp_type_to_ptp_type(file->filetype), &propcnt, &props);
        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "obj2file: call to ptp_mtp_getobjectpropssupported() failed.");
            // Silently fall through.
        } else {
            /* 遍历所有支持的属性*/
            for (i = 0; i < propcnt; i++) {
                switch (props[i]) {
                case PTP_OPC_ObjectSize:
                    if (device->object_bitsize == 64) {
                        /* 获取文件大小*/
                        file->filesize = get_u64_from_object(device, file->item_id, PTP_OPC_ObjectSize, 0);
                    } else {
                        file->filesize = get_u32_from_object(device, file->item_id, PTP_OPC_ObjectSize, 0);
                    }
                    break;
                default:
                    break;
                }
            }
            mtp_free(props);
        }
    }

    return file;
}

/**
 * 检索设备的一个单独文件的元数据
 *
 * @param device 相关的MTP设备
 * @param fileid 要获取的对象的ID
 *
 * @return 成功返回元数据的入口
 **/
LIBMTP_file_t *LIBMTP_Get_Filemetadata(LIBMTP_mtpdevice_t *device, u32 const fileid)
{
    PTPParams *params = (PTPParams *) device->params;
    u16 ret;
    PTPObject *ob;

    /* 获取设备所有句柄，只在已缓存了的设备*/
    if (device->cached && params->nrofobjects == 0) {
        flush_handles(device);
    }
    /* 返回ID为fileid的对象*/
    ret = ptp_object_want(params, fileid, PTPOBJECT_OBJECTINFO_LOADED|PTPOBJECT_MTPPROPLIST_LOADED, &ob);
    if (ret != PTP_RC_OK)
        return NULL;

    /* 转换成文件结构体*/
    return obj2file(device, ob);
}

/**
 * 这个函数用于检索某个设备上某个存储上某个父级文件夹的内容
 *
 * @param device  相关的MTP设备
 * @param storage 存储设备的ID
 * @param parent  父级目录的ID
 *
 * @return 成功返回一条包含目录与文件的链表
 **/
LIBMTP_file_t * LIBMTP_Get_Files_And_Folders(LIBMTP_mtpdevice_t *device,
                                             u32 const storage,
                                             u32 const parent){
    PTPParams *params = (PTPParams *) device->params;
    LIBMTP_file_t *retfiles = NULL;
    LIBMTP_file_t *curfile = NULL;
    PTPObjectHandles currentHandles;
    u32 storageid;
    u16 ret;
    int i = 0;

    if (device->cached) {
        /* 这个函数只能由未缓存的设备使用*/
        LIBMTP_ERROR("tried to use %s on a cached device!\n", __func__);
        return NULL;
    }
    /* 检查存储设备ID*/
    if (storage == 0)
        storageid = PTP_GOH_ALL_STORAGE;
    else
        storageid = storage;
    /* 获取parent目录下的所有对象句柄*/
    ret = ptp_getobjecthandles(params,
                               storageid,
                               PTP_GOH_ALL_FORMATS,
                               parent,
                               &currentHandles);

    if (ret != PTP_RC_OK) {
        char buf[80];
        sprintf(buf,"LIBMTP_Get_Files_And_Folders(): could not get object handles of %08x.", parent);
        add_ptp_error_to_errorstack(device, ret, buf);
        return NULL;
    }

    if (currentHandles.Handler == NULL || currentHandles.n == 0)
        return NULL;

    for (i = 0; i < currentHandles.n; i++) {
        LIBMTP_file_t *file;

        /* 获取一个文件的元数据，如果失败，尝试下一个*/
        file = LIBMTP_Get_Filemetadata(device, currentHandles.Handler[i]);
        if (file == NULL)
            continue;

        /* 串成一条链表，之后返回首地址*/
        if (curfile == NULL) {
            curfile = file;
            retfiles = file;
        } else {
            curfile->next = file;
            curfile = file;
        }
    }

    mtp_free(currentHandles.Handler);

    /* 返回链表中第一个文件的地址*/
    return retfiles;
}

/**
 * 创建一个新的曲目元数据结构体
 *
 * @return 成功返回新分配的曲目元数据结构体
 */
LIBMTP_track_t *LIBMTP_new_track_t(void)
{
    LIBMTP_track_t *new = (LIBMTP_track_t *) mtp_malloc(sizeof(LIBMTP_track_t));
    if (new == NULL) {
        return NULL;
    }
    new->item_id = 0;
    new->parent_id = 0;
    new->storage_id = 0;
    new->title = NULL;
    new->artist = NULL;
    new->composer = NULL;
    new->album = NULL;
    new->genre = NULL;
    new->date = NULL;
    new->filename = NULL;
    new->duration = 0;
    new->tracknumber = 0;
    new->filesize = 0;
    new->filetype = LIBMTP_FILETYPE_UNKNOWN;
    new->samplerate = 0;
    new->nochannels = 0;
    new->wavecodec = 0;
    new->bitrate = 0;
    new->bitratetype = 0;
    new->rating = 0;
    new->usecount = 0;
    new->modificationdate = 0;
    new->next = NULL;
    return new;
}

/**
 * 销毁一个曲目元数据结构体
 *
 * @param track 要销毁的曲目元数据结构体
 */
void LIBMTP_destroy_track_t(LIBMTP_track_t *track)
{
    if (track == NULL) {
        return;
    }
    if (track->title != NULL)
        mtp_free(track->title);
    if (track->artist != NULL)
        mtp_free(track->artist);
    if (track->composer != NULL)
        mtp_free(track->composer);
    if (track->album != NULL)
        mtp_free(track->album);
    if (track->genre != NULL)
        mtp_free(track->genre);
    if (track->date != NULL)
        mtp_free(track->date);
    if (track->filename != NULL)
        mtp_free(track->filename);
    mtp_free(track);
    return;
}

/**
 * 这个函数将属性映射和拷贝到使用的曲目元数据上
 */
static void pick_property_to_track_metadata(LIBMTP_mtpdevice_t *device, MTPProperties *prop, LIBMTP_track_t *track)
{
    switch (prop->property) {
    case PTP_OPC_Name:
        if (prop->propval.str != NULL)
            track->title = mtp_strdup(prop->propval.str);
        else
            track->title = NULL;
        break;
    case PTP_OPC_Artist:
        if (prop->propval.str != NULL)
            track->artist = mtp_strdup(prop->propval.str);
        else
        track->artist = NULL;
        break;
    case PTP_OPC_Composer:
        if (prop->propval.str != NULL)
            track->composer = mtp_strdup(prop->propval.str);
        else
            track->composer = NULL;
        break;
    case PTP_OPC_Duration:
        track->duration = prop->propval.u32;
        break;
    case PTP_OPC_Track:
        track->tracknumber = prop->propval.u16;
        break;
    case PTP_OPC_Genre:
        if (prop->propval.str != NULL)
            track->genre = mtp_strdup(prop->propval.str);
        else
            track->genre = NULL;
        break;
    case PTP_OPC_AlbumName:
        if (prop->propval.str != NULL)
            track->album = mtp_strdup(prop->propval.str);
        else
            track->album = NULL;
        break;
    case PTP_OPC_OriginalReleaseDate:
        if (prop->propval.str != NULL)
            track->date = mtp_strdup(prop->propval.str);
        else
            track->date = NULL;
        break;
        // These are, well not so important.
    case PTP_OPC_SampleRate:
        track->samplerate = prop->propval.u32;
        break;
    case PTP_OPC_NumberOfChannels:
        track->nochannels = prop->propval.u16;
        break;
    case PTP_OPC_AudioWAVECodec:
        track->wavecodec = prop->propval.u32;
        break;
    case PTP_OPC_AudioBitRate:
        track->bitrate = prop->propval.u32;
        break;
    case PTP_OPC_BitRateType:
        track->bitratetype = prop->propval.u16;
        break;
    case PTP_OPC_Rating:
        track->rating = prop->propval.u16;
        break;
    case PTP_OPC_UseCount:
        track->usecount = prop->propval.u32;
        break;
    case PTP_OPC_ObjectSize:
        if (device->object_bitsize == 64) {
            track->filesize = prop->propval.u64;
        } else {
            track->filesize = prop->propval.u32;
        }
        break;
    default:
        break;
    }
}

/**
 * 通过给定的曲目ID检索曲目的元数据
 *
 * @param device       相关MTP设备
 * @param trackid      曲目ID
 * @param objectformat 对象格式
 * @param track        要返回的填充完的曲目结构体
 */
static void get_track_metadata(LIBMTP_mtpdevice_t *device, u16 objectformat,
                               LIBMTP_track_t *track)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    u32 i;
    MTPProperties *prop;
    PTPObject *ob;

    /* 如果缓存了元数据，则使用它*/
    ret = ptp_object_want(params, track->item_id, PTPOBJECT_MTPPROPLIST_LOADED, &ob);
    if (ob->mtpprops) {
        prop = ob->mtpprops;
        for (i = 0;i < ob->nrofmtpprops;i++,prop++)
            pick_property_to_track_metadata(device, prop, track);
    } else {
        u16 *props = NULL;
        u32 propcnt = 0;

        /* 检索这个对象格式支持哪些属性*/
        ret = ptp_mtp_getobjectpropssupported(params, map_libmtp_type_to_ptp_type(track->filetype), &propcnt, &props);
        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "get_track_metadata(): call to ptp_mtp_getobjectpropssupported() failed.");
            // Just bail out for now, nothing is ever set.
            return;
        } else {
            for (i = 0;i < propcnt;i++) {
                switch (props[i]) {
                case PTP_OPC_Name:
                    track->title = get_string_from_object(device, track->item_id, PTP_OPC_Name);
                    break;
                case PTP_OPC_Artist:
                    track->artist = get_string_from_object(device, track->item_id, PTP_OPC_Artist);
                    break;
                case PTP_OPC_Composer:
                    track->composer = get_string_from_object(device, track->item_id, PTP_OPC_Composer);
                    break;
                case PTP_OPC_Duration:
                    track->duration = get_u32_from_object(device, track->item_id, PTP_OPC_Duration, 0);
                    break;
                case PTP_OPC_Track:
                    track->tracknumber = get_u16_from_object(device, track->item_id, PTP_OPC_Track, 0);
                    break;
                case PTP_OPC_Genre:
                    track->genre = get_string_from_object(device, track->item_id, PTP_OPC_Genre);
                    break;
                case PTP_OPC_AlbumName:
                    track->album = get_string_from_object(device, track->item_id, PTP_OPC_AlbumName);
                    break;
                case PTP_OPC_OriginalReleaseDate:
                    track->date = get_string_from_object(device, track->item_id, PTP_OPC_OriginalReleaseDate);
                    break;
                /* 这些不是那么重要*/
                case PTP_OPC_SampleRate:
                    track->samplerate = get_u32_from_object(device, track->item_id, PTP_OPC_SampleRate, 0);
                    break;
                case PTP_OPC_NumberOfChannels:
                    track->nochannels = get_u16_from_object(device, track->item_id, PTP_OPC_NumberOfChannels, 0);
                    break;
                case PTP_OPC_AudioWAVECodec:
                    track->wavecodec = get_u32_from_object(device, track->item_id, PTP_OPC_AudioWAVECodec, 0);
                    break;
                case PTP_OPC_AudioBitRate:
                    track->bitrate = get_u32_from_object(device, track->item_id, PTP_OPC_AudioBitRate, 0);
                    break;
                case PTP_OPC_BitRateType:
                    track->bitratetype = get_u16_from_object(device, track->item_id, PTP_OPC_BitRateType, 0);
                    break;
                case PTP_OPC_Rating:
                    track->rating = get_u16_from_object(device, track->item_id, PTP_OPC_Rating, 0);
                    break;
                case PTP_OPC_UseCount:
                    track->usecount = get_u32_from_object(device, track->item_id, PTP_OPC_UseCount, 0);
                    break;
                case PTP_OPC_ObjectSize:
                    if (device->object_bitsize == 64) {
                        track->filesize = get_u64_from_object(device, track->item_id, PTP_OPC_ObjectSize, 0);
                    } else {
                        track->filesize = (u64) get_u32_from_object(device, track->item_id, PTP_OPC_ObjectSize, 0);
                    }
                    break;
                }
            }
            mtp_free(props);
        }
    }
}

/**
 * 检索一个设备曲目的元数据
 *
 * @param device  相关的MTP设备
 * @param trackid 要检索的曲目的ID
 *
 * @@return 成功返回 曲目结构体，失败返回NULL
 */
LIBMTP_track_t *LIBMTP_Get_Trackmetadata(LIBMTP_mtpdevice_t *device, u32 const trackid)
{
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    PTPObject *ob;
    LIBMTP_track_t *track;
    LIBMTP_filetype_t mtptype;
    u16 ret;

    /* 获取所有句柄*/
    if (params->nrofobjects == 0)
        flush_handles(device);

    ret = ptp_object_want (params, trackid, PTPOBJECT_OBJECTINFO_LOADED, &ob);
    if (ret != PTP_RC_OK)
        return NULL;

    mtptype = map_ptp_type_to_libmtp_type(ob->oi.ObjectFormat);

    /* 忽略我们不知道怎么处理的东西*/
    if (!LIBMTP_FILETYPE_IS_TRACK(mtptype) &&
            /* 这行检查未定义的文件，因为可能忘了OGG或FLAC文件*/
            (ob->oi.ObjectFormat != PTP_OFC_Undefined ||
                    (!FLAG_IRIVER_OGG_ALZHEIMER(ptp_usb) &&
                            !FLAG_OGG_IS_UNKNOWN(ptp_usb) &&
                            !FLAG_FLAC_IS_UNKNOWN(ptp_usb)))) {
        //printf("Not a music track (name: %s format: %d), skipping...\n", oi->Filename, oi->ObjectFormat);
        return NULL;
  }

    /* 分配一个新的曲目结构体*/
    track = LIBMTP_new_track_t();

    /* 这是能定位到这个曲目的唯一ID*/
    track->item_id = ob->oid;
    track->parent_id = ob->oi.ParentObject;
    track->storage_id = ob->oi.StorageID;
    track->modificationdate = ob->oi.ModificationDate;

    track->filetype = mtptype;

    /* 原始的特定文件属性*/
    track->filesize = ob->oi.ObjectCompressedSize;
    if (ob->oi.Filename != NULL) {
        track->filename = mtp_strdup(ob->oi.Filename);
    }

    /* 一个特别的兼容，设备不会记得有些文件被定义为“未知”类型其实是OGG或FLAC文件。
     * 我们检查文件名的后缀名是不是“.ogg”或“.flac”*/
    if (track->filetype == LIBMTP_FILETYPE_UNKNOWN &&
            track->filename != NULL) {
        if ((FLAG_IRIVER_OGG_ALZHEIMER(ptp_usb) ||
                FLAG_OGG_IS_UNKNOWN(ptp_usb)) &&
                has_ogg_extension(track->filename))
            track->filetype = LIBMTP_FILETYPE_OGG;
        else if (FLAG_FLAC_IS_UNKNOWN(ptp_usb) &&
                has_flac_extension(track->filename))
            track->filetype = LIBMTP_FILETYPE_FLAC;
        else {
            /* 这是一个OGG/FLAC文件，所以丢弃它*/
            LIBMTP_destroy_track_t(track);
            return NULL;
        }
    }
    /* 获取曲目元数据*/
    get_track_metadata(device, ob->oi.ObjectFormat, track);
    return track;
}

/**
 * 这将从设备的文件信息获取到由文件描述符标识的文件。
 * 这个功能可以潜在地用于流式传输设备上的文件以供播放或广播，例如，通过将文件下载到流
 * 接收器(例如，套接字)上。
 *
 * @param device   相关的MTP设备
 * @param id       要检索的文件的ID
 * @param fd       要写回去的文件描述符
 * @param callback 回调函数
 * @param data     回调函数的参数
 *
 * @return 传输成功返回0，失败返回其他值
 **/
int LIBMTP_Get_File_To_File_Descriptor(LIBMTP_mtpdevice_t *device,
                                       u32 const id,
                                       int const fd,
                                       LIBMTP_progressfunc_t const callback,
                                       void const * const data){
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;

    /* 获取文件元数据*/
    LIBMTP_file_t *mtpfile = LIBMTP_Get_Filemetadata(device, id);
    if (mtpfile == NULL) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Get_File_To_File_Descriptor(): Could not get object info.");
        return -1;
    }
    if (mtpfile->filetype == LIBMTP_FILETYPE_FOLDER) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Get_File_To_File_Descriptor(): Bad object format.");
        return -1;
    }

    /* 回调*/
    ptp_usb->callback_active = 1;
    ptp_usb->current_transfer_total = mtpfile->filesize + PTP_USB_BULK_HDR_LEN + sizeof(u32); /* 请求长度*/
    ptp_usb->current_transfer_complete = 0;
    ptp_usb->current_transfer_callback = callback;
    ptp_usb->current_transfer_callback_data = data;

    /* 不需要mtpfile了*/
    LIBMTP_destroy_file_t(mtpfile);

    ret = ptp_getobject_tofd(params, id, fd);

    ptp_usb->callback_active = 0;
    ptp_usb->current_transfer_callback = NULL;
    ptp_usb->current_transfer_callback_data = NULL;

    if (ret == PTP_ERROR_CANCEL) {
        add_error_to_errorstack(device, LIBMTP_ERROR_CANCELLED, "LIBMTP_Get_File_From_File_Descriptor(): Cancelled transfer.");
        return -1;
    }
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Get_File_To_File_Descriptor(): Could not get file from device.");
        return -1;
    }

    return 0;
}


/**
 * 这将从设备的曲目信息获取到由文件描述符标识的文件。
 *
 * @param device   相关的MTP设备
 * @param id       要检索的曲目的ID
 * @param fd       要写回去的文件描述符
 * @param callback 回调函数
 * @param data     回调函数的参数
 *
 * @return 传输成功返回0，失败返回其他值
 **/
int LIBMTP_Get_Track_To_File_Descriptor(LIBMTP_mtpdevice_t *device,
                                        u32 const id,
                                        int const fd,
                                        LIBMTP_progressfunc_t const callback,
                                        void const * const data){
    return LIBMTP_Get_File_To_File_Descriptor(device, id, fd, callback, data);
}

/**
 * 把本地的一个文件发送到MTP设备
 *
 * @param device   相关MTP设备
 * @param path     本地文件的路径
 * @param filedata MTP设备的文件结构体
 * @param callback 回调函数
 * @param data     回调函数参数
 *
 * @reutrn 成功返回0，失败返回其他值
 */
int LIBMTP_Send_File_From_File(LIBMTP_mtpdevice_t *device,
                               char const * const path, LIBMTP_file_t * const filedata,
                               LIBMTP_progressfunc_t const callback,
                               void const * const data)
{
    int fd;
    int ret;

    /* 合法性检查*/
    if (path == NULL) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Send_File_From_File(): Bad arguments, path was NULL.");
        return -1;
    }

    /* 打开文件*/
#ifdef __WIN32__
#ifdef USE_WINDOWS_IO_H
    if ( (fd = _open(path, O_RDONLY|O_BINARY)) == -1 ) {
#else
    if ( (fd = open(path, O_RDONLY|O_BINARY)) == -1 ) {
#endif
#elif defined(AWORKS)
    if ( (fd = mtp_open(path, O_RDONLY, 0777)) == -1) {
#else
    if ( (fd = open(path, O_RDONLY)) == -1) {
#endif
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Send_File_From_File(): Could not open source file.");
        return -1;
    }
    /* 发送文件描述符*/
    ret = LIBMTP_Send_File_From_File_Descriptor(device, fd, filedata, callback, data);

    /* 关闭文件*/
#ifdef USE_WINDOWS_IO_H
    _close(fd);
#else
    mtp_close(fd);
#endif

    return ret;
}

/**
 * 从本地文件描述符中发送文件到MTP设备
 *
 * @param device   相关MTP设备
 * @param fd       本地文件描述符
 * @param filedata MTP文件结构体
 * @param callback 回调函数
 * @param data     回调函数参数
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Send_File_From_File_Descriptor(LIBMTP_mtpdevice_t *device,
                                          int const fd, LIBMTP_file_t * const filedata,
                                          LIBMTP_progressfunc_t const callback,
                                          void const * const data){
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    LIBMTP_file_t *newfilemeta;
    int oldtimeout;
    int timeout;

    /* 发送文件对象信息*/
    if (send_file_object_info(device, filedata))
    {
        /* 不需要输出错误信息因为send_file_object_info已经做了*/
        return -1;
    }

    /* 回调*/
    ptp_usb->callback_active = 1;
    /* 在发送了这么多数据后，回调将停止自己。一个批量头用于请求，一个用于数据阶段。请求没有参数*/
    ptp_usb->current_transfer_total = filedata->filesize + PTP_USB_BULK_HDR_LEN * 2;
    ptp_usb->current_transfer_complete = 0;
    ptp_usb->current_transfer_callback = callback;
    ptp_usb->current_transfer_callback_data = data;

    /* 我们可能需要增加超时时间，文件可能很大。用默认超时时间加上这次传输计算的时间*/
    get_usb_device_timeout(ptp_usb, &oldtimeout);
    timeout = oldtimeout + (ptp_usb->current_transfer_total / guess_usb_speed(ptp_usb)) * 1000;
    set_usb_device_timeout(ptp_usb, timeout);
    /* 发送对应文件描述符的对象*/
    ret = ptp_sendobject_fromfd(params, fd, filedata->filesize);

    ptp_usb->callback_active = 0;
    ptp_usb->current_transfer_callback = NULL;
    ptp_usb->current_transfer_callback_data = NULL;
    set_usb_device_timeout(ptp_usb, oldtimeout);

    if (ret == PTP_ERROR_CANCEL) {
        add_error_to_errorstack(device, LIBMTP_ERROR_CANCELLED, "LIBMTP_Send_File_From_File_Descriptor(): Cancelled transfer.");
        return -1;
    }
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Send_File_From_File_Descriptor(): "
                "Could not send object.");
        return -1;
    }
    /* 把对象添加到缓存里*/
    add_object_to_cache(device, filedata->item_id);

    /* 从缓存中获取设备分配的parent_id。将其添加到缓存的操作将从设备中查找它，因此
     * 我们从缓存中获取新的parent_id*/
    newfilemeta = LIBMTP_Get_Filemetadata(device, filedata->item_id);
    if (newfilemeta != NULL) {
        filedata->parent_id = newfilemeta->parent_id;
        filedata->storage_id = newfilemeta->storage_id;
        LIBMTP_destroy_file_t(newfilemeta);
    } else {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL,
                "LIBMTP_Send_File_From_File_Descriptor(): "
                "Could not retrieve updated metadata.");
        return -1;
    }

    return 0;
}

/**
 * 发送文件对象信息
 *
 * @param device   相关的MTP设备
 * @param filedata 要与文件一起写入的文件元数据集
 *
 * @return 成功返回0，失败返回其他值
 */
static int send_file_object_info(LIBMTP_mtpdevice_t *device, LIBMTP_file_t *filedata)
{
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    u32 store;
    int use_primary_storage = 1;
    u16 of = map_libmtp_type_to_ptp_type(filedata->filetype);
    LIBMTP_devicestorage_t *storage;
    u32 localph = filedata->parent_id;
    u16 ret;
    int i;

#if 0
    /* 合法性检查：有一写设备不能有0长度文件，如果0长度文件在一些设备上导致出现问题，在这里添加bug标志*/
    if (filedata->filesize == 0) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "send_file_object_info(): "
                "File of zero size.");
        return -1;
    }
#endif
    /* 检查存储设备是否符合文件大小需求*/
    if (filedata->storage_id != 0) {
        store = filedata->storage_id;
    } else {
        store = get_suggested_storage_id(device, filedata->filesize, localph);
    }

    /* 检查是否使用主要的存储设备*/
    storage = device->storage;
    if (storage != NULL && store != storage->id) {
        use_primary_storage = 0;
    }

    /*
     * 没有给定特定的文件夹，尽可能的查找默认的文件夹。这里使用的是固定列表检索默认文件夹。
     * 显然，有些设备需要将其文件放在某些文件夹中才能找到/显示它们，因此我们有一种方法来实现这一点。
     */
    if (localph == 0 && use_primary_storage) {
        if (LIBMTP_FILETYPE_IS_AUDIO(filedata->filetype)) {
            localph = device->default_music_folder;
        } else if (LIBMTP_FILETYPE_IS_VIDEO(filedata->filetype)) {
            localph = device->default_video_folder;
        } else if (of == PTP_OFC_EXIF_JPEG ||
                 of == PTP_OFC_JP2 ||
                 of == PTP_OFC_JPX ||
                 of == PTP_OFC_JFIF ||
                 of == PTP_OFC_TIFF ||
                 of == PTP_OFC_TIFF_IT ||
                 of == PTP_OFC_BMP ||
                 of == PTP_OFC_GIF ||
                 of == PTP_OFC_PICT ||
                 of == PTP_OFC_PNG ||
                 of == PTP_OFC_MTP_WindowsImageFormat) {
            localph = device->default_picture_folder;
        } else if (of == PTP_OFC_MTP_vCalendar1 ||
                 of == PTP_OFC_MTP_vCalendar2 ||
                 of == PTP_OFC_MTP_UndefinedContact ||
                 of == PTP_OFC_MTP_vCard2 ||
                 of == PTP_OFC_MTP_vCard3 ||
                 of == PTP_OFC_MTP_UndefinedCalendarItem) {
             localph = device->default_organizer_folder;
        } else if (of == PTP_OFC_Text) {
            localph = device->default_text_folder;
        }
    }

    if (FLAG_OGG_IS_UNKNOWN(ptp_usb) && of == PTP_OFC_MTP_OGG) {
        of = PTP_OFC_Undefined;
    }
    if (FLAG_FLAC_IS_UNKNOWN(ptp_usb) && of == PTP_OFC_MTP_FLAC) {
        of = PTP_OFC_Undefined;
    }
    /* 检查是否支持发送对象属性列表*/
    if (ptp_operation_issupported(params, PTP_OC_MTP_SendObjectPropList) &&
            !FLAG_BROKEN_SEND_OBJECT_PROPLIST(ptp_usb)) {
        /*
         * MTP enhanched does it this way (from a sniff):
         * -> PTP_OC_MTP_SendObjectPropList (0x9808):
         *    20 00 00 00 01 00 08 98 1B 00 00 00 01 00 01 00
         *    FF FF FF FF 00 30 00 00 00 00 00 00 12 5E 00 00
         *    Length: 0x00000020
         *    Type:   0x0001 PTP_USB_CONTAINER_COMMAND
         *    Code:   0x9808
         *    Transaction ID: 0x0000001B
         *    Param1: 0x00010001 <- store
         *    Param2: 0xffffffff <- parent handle (-1 ?)
         *    Param3: 0x00003000 <- file type PTP_OFC_Undefined - we don't know about PDF files
         *    Param4: 0x00000000 <- file length MSB (-0x0c header len)
         *    Param5: 0x00005e12 <- file length LSB (-0x0c header len)
         *
         * -> PTP_OC_MTP_SendObjectPropList (0x9808):
         *    46 00 00 00 02 00 08 98 1B 00 00 00 03 00 00 00
         *    00 00 00 00 07 DC FF FF 0D 4B 00 53 00 30 00 36 - dc07 = file name
         *    00 30 00 33 00 30 00 36 00 2E 00 70 00 64 00 66
         *    00 00 00 00 00 00 00 03 DC 04 00 00 00 00 00 00 - dc03 = protection status
         *    00 4F DC 02 00 01                               - dc4f = non consumable
         *    Length: 0x00000046
         *    Type:   0x0002 PTP_USB_CONTAINER_DATA
         *    Code:   0x9808
         *    Transaction ID: 0x0000001B
         *    Metadata....
         *    0x00000003 <- Number of metadata items
         *    0x00000000 <- Object handle, set to 0x00000000 since it is unknown!
         *    0xdc07     <- metadata type: file name
         *    0xffff     <- metadata type: string
         *    0x0d       <- number of (uint16_t) characters
         *    4b 53 30 36 30 33 30 36 2e 50 64 66 00 "KS060306.pdf", null terminated
         *    0x00000000 <- Object handle, set to 0x00000000 since it is unknown!
         *    0xdc03     <- metadata type: protection status
         *    0x0004     <- metadata type: uint16_t
         *    0x0000     <- not protected
         *    0x00000000 <- Object handle, set to 0x00000000 since it is unknown!
         *    0xdc4f     <- non consumable
         *    0x0002     <- metadata type: uint8_t
         *    0x01       <- non-consumable (this device cannot display PDF)
         *
         * <- Read 0x18 bytes back
         *    18 00 00 00 03 00 01 20 1B 00 00 00 01 00 01 00
         *    00 00 00 00 01 40 00 00
         *    Length: 0x000000018
         *    Type:   0x0003 PTP_USB_CONTAINER_RESPONSE
         *    Code:   0x2001 PTP_OK
         *    Transaction ID: 0x0000001B
         *    Param1: 0x00010001 <- store
         *    Param2: 0x00000000 <- parent handle
         *    Param3: 0x00004001 <- new file/object ID
         *
         * -> PTP_OC_SendObject (0x100d)
         *    0C 00 00 00 01 00 0D 10 1C 00 00 00
         * -> ... all the bytes ...
         * <- Read 0x0c bytes back
         *    0C 00 00 00 03 00 01 20 1C 00 00 00
         *    ... Then update metadata one-by one, actually (instead of sending it first!) ...
         */
        MTPProperties *props = NULL;
        int nrofprops = 0;
        MTPProperties *prop = NULL;
        u16 *properties = NULL;
        u32 propcnt = 0;

        /* 默认的父目录ID*/
        if (localph == 0)
            localph = 0xFFFFFFFFU; // Set to -1

        /* 新的对象ID必须设置为0x00000000U*/
        filedata->item_id = 0x00000000U;
        /* 获取文件格式支持的属性*/
        ret = ptp_mtp_getobjectpropssupported(params, of, &propcnt, &properties);

        for (i = 0;i < propcnt;i++) {
            PTPObjectPropDesc opd;
            /* 获取对象属性描述*/
            ret = ptp_mtp_getobjectpropdesc(params, properties[i], of, &opd);
            if (ret != PTP_RC_OK) {
                add_ptp_error_to_errorstack(device, ret, "send_file_object_info(): "
                        "could not get property description.");
            } else if (opd.GetSet) {
                /* 给新的文件添加支持的属性*/
                switch (properties[i]) {
                case PTP_OPC_ObjectFileName:
                    prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                    prop->ObjectHandle = filedata->item_id;
                    prop->property = PTP_OPC_ObjectFileName;
                    prop->datatype = PTP_DTC_STR;
                    if (filedata->filename != NULL) {
                        prop->propval.str = mtp_strdup(filedata->filename);
                        if (FLAG_ONLY_7BIT_FILENAMES(ptp_usb)) {
                            strip_7bit_from_utf8(prop->propval.str);
                        }
                    }
                    break;
                case PTP_OPC_ProtectionStatus:
                    prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                    prop->ObjectHandle = filedata->item_id;
                    prop->property = PTP_OPC_ProtectionStatus;
                    prop->datatype = PTP_DTC_UINT16;
                    prop->propval.u16 = 0x0000U; /* Not protected */
                    break;
                case PTP_OPC_NonConsumable:
                    prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                    prop->ObjectHandle = filedata->item_id;
                    prop->property = PTP_OPC_NonConsumable;
                    prop->datatype = PTP_DTC_UINT8;
                    prop->propval.u8 = 0x00; /* It is supported, then it is consumable */
                    break;
                case PTP_OPC_Name:
                    prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                    prop->ObjectHandle = filedata->item_id;
                    prop->property = PTP_OPC_Name;
                    prop->datatype = PTP_DTC_STR;
                    if (filedata->filename != NULL)
                        prop->propval.str = mtp_strdup(filedata->filename);
                    break;
                case PTP_OPC_DateModified:
                    /* 如果支持则标记当前的时间*/
                    if (!FLAG_CANNOT_HANDLE_DATEMODIFIED(ptp_usb)) {
                        prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                        prop->ObjectHandle = filedata->item_id;
                        prop->property = PTP_OPC_DateModified;
                        prop->datatype = PTP_DTC_STR;
                        prop->propval.str = get_iso8601_stamp();
                        filedata->modificationdate = time(NULL);
                    }
                    break;
                }
            }
            ptp_free_objectpropdesc(&opd);
        }
        mtp_free(properties);
        /* 发送PTP对象属性列表*/
        ret = ptp_mtp_sendobjectproplist(params, &store, &localph, &filedata->item_id,
                        of, filedata->filesize, props, nrofprops);

        /* 释放属性列表1*/
        ptp_destroy_object_prop_list(props, nrofprops);

        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "send_file_object_info():"
                    "Could not send object property list.");
            if (ret == PTP_RC_AccessDenied) {
                add_ptp_error_to_errorstack(device, ret, "ACCESS DENIED.");
            }
            return -1;
        }
    } else if (ptp_operation_issupported(params,PTP_OC_SendObjectInfo)) {
        PTPObjectInfo new_file;

        memset(&new_file, 0, sizeof(PTPObjectInfo));

        new_file.Filename = filedata->filename;
        if (FLAG_ONLY_7BIT_FILENAMES(ptp_usb)) {
            strip_7bit_from_utf8(new_file.Filename);
        }
        if (filedata->filesize > 0xFFFFFFFFL) {
            /* 这是大文件的MTP标准中的一个错误*/
            new_file.ObjectCompressedSize = (u32) 0xFFFFFFFF;
        } else {
            new_file.ObjectCompressedSize = (u32) filedata->filesize;
        }
        new_file.ObjectFormat = of;
        new_file.StorageID = store;
        new_file.ParentObject = localph;
        new_file.ModificationDate = time(NULL);

        /* 创建对象信息*/
        ret = ptp_sendobjectinfo(params, &store, &localph, &filedata->item_id, &new_file);

        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "send_file_object_info(): "
                    "Could not send object info.");
            if (ret == PTP_RC_AccessDenied) {
                add_ptp_error_to_errorstack(device, ret, "ACCESS DENIED.");
            }
            return -1;
        }
        // NOTE: the char* pointers inside new_file are not copies so don't
        // try to destroy this objectinfo!
    }

    /* 设置父目录ID*/
    filedata->parent_id = localph;

    return 0;
}
/**
 * 这个函数删除MTP设备里的一个文件，曲目，播放列表，文件夹或任何其他对象
 *
 * 如果删除一个文件夹，则无法保证设备会真正删除该文件夹中的所有文件，而是希望它们不会被删除，并出现在
 * 父项设置为非存在对象ID的对象列表中。安全的方式是递归的删除文件夹中包含的所有文件(和目录)，然后删
 * 除文件夹本身。
 *
 * @param device    相关的MTP设备
 * @param object_id 要删除的对象的ID
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Delete_Object(LIBMTP_mtpdevice_t *device, u32 object_id)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    /* PTP删除对象*/
    ret = ptp_deleteobject(params, object_id, 0);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Delete_Object(): could not delete object.");
        return -1;
    }

    return 0;
}

/**
 * 把一个对象移动到另一个地方
 * 这个操作会消耗一定的时间，特别是在不同的存储设备间移动
 *
 * @param device     相关的MTP设备
 * @param object_id  要移动的对象ID
 * @param storage_id 目标位置的存储设备ID
 * @param parent_id  目标位置的父目录ID
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Move_Object(LIBMTP_mtpdevice_t *device,
                       u32 object_id,
                       u32 storage_id,
                       u32 parent_id)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;

    ret = ptp_moveobject(params, object_id, storage_id, parent_id);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Move_Object(): could not move object.");
        return -1;
    }

    return 0;
}

/**
 * 把一个对象复制到另一个地方
 * 这个操作会消耗一定的时间，特别是在不同的存储设备间移动
 *
 * @param device     相关的MTP设备
 * @param object_id  要复制的对象ID
 * @param storage_id 目标位置的存储设备ID
 * @param parent_id  目标位置的父目录ID
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Copy_Object(LIBMTP_mtpdevice_t *device,
                       u32 object_id,
                       u32 storage_id,
                       u32 parent_id)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;

    ret = ptp_copyobject(params, object_id, storage_id, parent_id);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Copy_Object(): could not copy object.");
        return -1;
    }

    return 0;
}

/**
 * 更新一个对象文件名属性
 *
 * @param device      相关的MTP设备
 * @param object_id   要改名字的对象的ID
 * @param ptp_type    对象的PTP文件类型
 * @param newname_ptr 新的文件名
 *
 * @return 成功返回0，失败返回其他值
 */
static int set_object_filename(LIBMTP_mtpdevice_t *device,
                               u32 object_id, u16 ptp_type,
                               const char **newname_ptr){
    PTPParams             *params = (PTPParams *) device->params;
    PTP_USB               *ptp_usb = (PTP_USB*) device->usbinfo;
    PTPObjectPropDesc     opd;
    u16              ret;
    char                  *newname;

    /* 检查我们是否可以修改文件文件名*/
    ret = ptp_mtp_getobjectpropdesc(params, PTP_OPC_ObjectFileName, ptp_type, &opd);
    if (ret != PTP_RC_OK) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "set_object_filename(): "
                "could not get property description.");
        return -1;
    }

    if (!opd.GetSet) {
        ptp_free_objectpropdesc(&opd);
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "set_object_filename(): "
            " property is not settable.");
        // TODO: we COULD actually upload/download the object here, if we feel
        //       like wasting time for the user.
        return -1;
    }
    /* 拷贝新的文件名*/
    newname = mtp_strdup(*newname_ptr);

    if (FLAG_ONLY_7BIT_FILENAMES(ptp_usb)) {
        strip_7bit_from_utf8(newname);
    }

    if (ptp_operation_issupported(params, PTP_OC_MTP_SetObjPropList) &&
            !FLAG_BROKEN_SET_OBJECT_PROPLIST(ptp_usb)) {
        MTPProperties *props = NULL;
        MTPProperties *prop = NULL;
        int nrofprops = 0;
        /* 获取一个新的对象属性*/
        prop = ptp_get_new_object_prop_entry(&props, &nrofprops);
        prop->ObjectHandle = object_id;
        prop->property = PTP_OPC_ObjectFileName;
        prop->datatype = PTP_DTC_STR;
        prop->propval.str = newname;
        /* 发送新的对象属性*/
        ret = ptp_mtp_setobjectproplist(params, props, nrofprops);
        /* 删除属性列表*/
        ptp_destroy_object_prop_list(props, nrofprops);

        if (ret != PTP_RC_OK) {
            add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "set_object_filename(): "
                    " could not set object property list.");
            ptp_free_objectpropdesc(&opd);
            return -1;
        }
    } else if (ptp_operation_issupported(params, PTP_OC_MTP_SetObjectPropValue)) {
        /* 设置对象字符串*/
        ret = set_object_string(device, object_id, PTP_OPC_ObjectFileName, newname);
        if (ret != 0) {
            add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "set_object_filename(): "
                    " could not set object filename.");
            ptp_free_objectpropdesc(&opd);
            return -1;
        }
    } else {
        mtp_free(newname);
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "set_object_filename(): "
                " your device doesn't seem to support any known way of setting metadata.");
        ptp_free_objectpropdesc(&opd);
        return -1;
    }
    /* 释放对象属性描述*/
    ptp_free_objectpropdesc(&opd);

    /* 更新新的对象属性*/
    update_metadata_cache(device, object_id);

    mtp_free(newname);

    return 0;
}


/**
 * 这个函数用于重新对一个文件进行命名
 *
 * @param device  相关的MTP设备
 * @param file    要重命名的文件元数据结构体
 * @param newname 新的名字
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Set_File_Name(LIBMTP_mtpdevice_t *device,
                   LIBMTP_file_t *file, const char *newname)
{
    int         ret;

    ret = set_object_filename(device, file->item_id,
                              map_libmtp_type_to_ptp_type(file->filetype),
                              &newname);

    if (ret != 0) {
        return ret;
    }

    mtp_free(file->filename);
    file->filename = mtp_strdup(newname);
    return ret;
}


/**
 * 这个函数用于重新对一个文件夹进行命名
 *
 * @param device  相关的MTP设备
 * @param folder  要重命名的文件夹元数据结构体
 * @param newname 新的名字
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Set_Folder_Name(LIBMTP_mtpdevice_t *device,
                           LIBMTP_folder_t *folder, const char* newname)
{
    int ret;

    ret = set_object_filename(device, folder->folder_id,
                              PTP_OFC_Association,
                              &newname);

    if (ret != 0) {
        return ret;
        }

    mtp_free(folder->name);
    folder->name = mtp_strdup(newname);
    return ret;
}

/**
 * 设置对象文件名
 *
 * @param device     相关的MTP设备
 * @param object_id  对象id
 * @param newname    新的名字
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Set_Object_Filename(LIBMTP_mtpdevice_t *device,
                               u32 object_id, char* newname)
{
    int             ret;
    LIBMTP_file_t   *file;

    file = LIBMTP_Get_Filemetadata(device, object_id);

    if (file == NULL) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Set_Object_Filename(): "
                "could not get file metadata for target object.");
        return -1;
    }

    ret = set_object_filename(device, object_id, map_libmtp_type_to_ptp_type(file->filetype), (const char **) &newname);

    if (file->filename != NULL)
        mtp_free(file->filename);
    mtp_free(file);

    return ret;
}

/**
 * 获取MTP设备里的一个文件的数据到本地的文件里
 *
 * @param device   相关的MTP设备
 * @param id       要获取数据的文件ID
 * @param path     本地文件的路径
 * @param callback 回调函数
 * @param data     回调函数参数
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Get_File_To_File(LIBMTP_mtpdevice_t *device, u32 const id,
                            char const * const path, LIBMTP_progressfunc_t const callback,
                            void const * const data){
    int fd = -1;
    int ret;

    /* 合法性检查*/
    if (path == NULL) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Get_File_To_File(): Bad arguments, path was NULL.");
        return -1;
    }

    /* 打开文件*/
#ifdef __WIN32__
#ifdef USE_WINDOWS_IO_H
    if ( (fd = _open(path, O_RDWR|O_CREAT|O_TRUNC|O_BINARY,_S_IREAD)) == -1 ) {
#else
    if ( (fd = open(path, O_RDWR|O_CREAT|O_TRUNC|O_BINARY,S_IRWXU)) == -1 ) {
#endif
#else
      if ((fd = mtp_open(path, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU|S_IRGRP)) == -1) {
#endif
          add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Get_File_To_File(): Could not create file.");
          return -1;
      }
      /* 这将从设备的文件信息获取到由文件描述符标识的文件*/
      ret = LIBMTP_Get_File_To_File_Descriptor(device, id, fd, callback, data);
      /* 关闭文件*/
      mtp_close(fd);

      /* 删除部分文件*/
      if (ret == -1) {
          mtp_unlink(path);
      }

      return ret;
}


/**
 * 获取文件链表
 *
 * @param device 要获取文件链表的MTP设备
 *
 * @return 成功返回MTP文件链表
 */
LIBMTP_file_t *LIBMTP_Get_Filelisting(LIBMTP_mtpdevice_t *device)
{
    LIBMTP_INFO("WARNING: LIBMTP_Get_Filelisting() is deprecated.\n");
    LIBMTP_INFO("WARNING: please update your code to use LIBMTP_Get_Filelisting_With_Callback()\n");
    return LIBMTP_Get_Filelisting_With_Callback(device, NULL, NULL);
}

/**
 *  获取文件链表
 *
 * @param device   要获取文件链表的MTP设备
 * @param callback 用户定义的回调函数
 * @param data     传递给回调函数的参数
 *
 * @return 成功返回LIBMTP_file_t数据结构体的文件链表
 **/
LIBMTP_file_t *LIBMTP_Get_Filelisting_With_Callback(LIBMTP_mtpdevice_t *device,
                                                    LIBMTP_progressfunc_t const callback,
                                                    void const * const data){
    u32 i = 0;
    LIBMTP_file_t *retfiles = NULL;
    LIBMTP_file_t *curfile = NULL;
    PTPParams *params = (PTPParams *) device->params;

    /* 获取设备所有句柄*/
    if (params->nrofobjects == 0) {
        flush_handles(device);
    }
    /* 遍历所有对象*/
    for (i = 0; i < params->nrofobjects; i++) {
        LIBMTP_file_t *file;
        PTPObject *ob;
        /* 调用回调函数*/
        if (callback != NULL)
            callback(i, params->nrofobjects, data);

        ob = &params->objects[i];
        /* 目录*/
        if (ob->oi.ObjectFormat == PTP_OFC_Association) {
            /* MTP对文件夹使用此对象格式，这意味着这些文件将出现在文件夹列表中*/
            continue;
        }

        /* 把对象转换为MTP文件结构体*/
        file = obj2file(device, ob);
        if (file == NULL) {
            continue;
        }

        /* 添加到之后会返回的链表中*/
        if (retfiles == NULL) {
            retfiles = file;
            curfile = file;
        } else {
            curfile->next = file;
            curfile = file;
        }

        // Call listing callback
        // double progressPercent = (double)i*(double)100.0 / (double)params->handles.n;

    } /* 处理计数循环*/
    return retfiles;
}

/**
 * 创建一个新的文件夹结构体以及分配内存
 *
 * @return 成功返回分配的文件夹结构体
 **/
LIBMTP_folder_t *LIBMTP_new_folder_t(void)
{
    LIBMTP_folder_t *new = (LIBMTP_folder_t *) mtp_malloc(sizeof(LIBMTP_folder_t));
    if (new == NULL) {
        return NULL;
    }
    new->folder_id = 0;
    new->parent_id = 0;
    new->storage_id = 0;
    new->name = NULL;
    new->sibling = NULL;
    new->child = NULL;
    return new;
}

/**
 * 递归删除文件夹结构体的内存，这应该用于顶层文件夹，销毁整个目录数
 *
 * @param folder 要销毁的文件夹
 */
void LIBMTP_destroy_folder_t(LIBMTP_folder_t *folder)
{
    /* 合法性检查*/
    if(folder == NULL) {
        return;
    }

    /* 销毁子目录，从底部开始销毁*/
    if(folder->child != NULL) {
        LIBMTP_destroy_folder_t(folder->child);
    }
    /* 销毁同级目录*/
    if(folder->sibling != NULL) {
        LIBMTP_destroy_folder_t(folder->sibling);
    }
    /* 释放文件夹名字内存*/
    if(folder->name != NULL) {
        mtp_free(folder->name);
    }

    mtp_free(folder);
}

/**
 * Function used to recursively get subfolders from params.
 * 递归获取目录的子目录
 *
 * @param list   目录链表
 * @param parent 父目录ID
 *
 * @return
 */
static LIBMTP_folder_t *get_subfolders_for_folder(LIBMTP_folder_t *list, uint32_t parent)
{
    LIBMTP_folder_t *retfolders = NULL;
    LIBMTP_folder_t *children, *iter, *curr;

    /* 获取同级目录*/
    iter = list->sibling;
    while(iter != list) {
        /* 检查目录的父目录ID是否相等*/
        if (iter->parent_id != parent) {
            iter = iter->sibling;
            continue;
        }

        /* 我们知道 iter是‘parent’的子目录，因此我们可以在当前安全地保持'iter'，因为当我们递归时，
         * 没有其他人会从'list'中窃取它*/
        /* 获取子目录*/
        children = get_subfolders_for_folder(list, iter->folder_id);

        curr = iter;
        iter = iter->sibling;

        /* 从链表中移除当前目录*/
        curr->child->sibling = curr->sibling;
        curr->sibling->child = curr->child;
        /* 把子目录链接到当前目录*/
        curr->child = children;
        /* 把当前目录放到同级目录链表*/
        curr->sibling = retfolders;
        retfolders = curr;
    }
    /* 返回当前目录结构体*/
    return retfolders;
}

/**
 * 在当前的MTP设备上返回一个所有可得的文件夹的链表
 *
 * @param device  要获取文件夹的MTP设备
 * @param storage 要获取文件链表的存储设备的ID
 *
 * @return 成功返回一个文件夹的链表
 */
LIBMTP_folder_t *LIBMTP_Get_Folder_List_For_Storage(LIBMTP_mtpdevice_t *device,
                            u32 const storage){
    PTPParams *params = (PTPParams *) device->params;
    LIBMTP_folder_t head, *rv;
    int i;

    /* 如果还没有获取所有的句柄，则刷新获取设备的句柄*/
    if (params->nrofobjects == 0) {
        flush_handles(device);
    }

    /* 这将创建文件夹的临时链表，用相反的顺序排列，并使用文件夹结构中已有的文件夹指针。
     * 然后，我们可以通过查看这个临时链表来构建文件夹的层次结构，并在运行是从这个临时链表中删除
     * 文件夹。这显然减少了在构建文件夹层次结构时必须执行的操作数。另外，由于临时链表的顺序是
     * 相反的，所以当我们在同级链表前面时，事情的顺序与最初在句柄链表中的顺序是相同的*/
    head.sibling = &head;
    head.child = &head;
    for (i = 0; i < params->nrofobjects; i++) {
        LIBMTP_folder_t *folder;
        PTPObject *ob;

        ob = &params->objects[i];
        /* 寻找文件夹*/
        if (ob->oi.ObjectFormat != PTP_OFC_Association) {
            continue;
        }
        /* 如果不是遍历所有的存储设备，检查对象的存储设备是不是和我们想要使用的存储设备是一样的*/
        if (storage != PTP_GOH_ALL_STORAGE && storage != ob->oi.StorageID) {
            continue;
        }

        /*
         * Do we know how to handle these? They are part
         * of the MTP 1.0 specification paragraph 3.6.4.
         * For AssociationDesc 0x00000001U ptp_mtp_getobjectreferences()
         * should be called on these to get the contained objects, but
         * we basically don't care. Hopefully parent_id is maintained for all
         * children, because we rely on that instead.
         */
        if (ob->oi.AssociationDesc != 0x00000000U) {
            LIBMTP_INFO("MTP extended association type 0x%08x encountered\n", ob->oi.AssociationDesc);
        }

        /* 创建一个文件夹*/
        folder = LIBMTP_new_folder_t();
        if (folder == NULL) {
            /* 申请内存失败*/
            return NULL;
        }
        /* 文件夹ID(对象ID)*/
        folder->folder_id = ob->oid;
        /* 文件夹父ID(对象的父对象)*/
        folder->parent_id = ob->oi.ParentObject;
        /* 相关的存储设备的ID*/
        folder->storage_id = ob->oi.StorageID;
        /* 文件夹名字*/
        folder->name = (ob->oi.Filename) ? (char *)mtp_strdup(ob->oi.Filename) : NULL;

        /* 子目录都指向head，sibling指向下一个folder*/
        folder->sibling = head.sibling;
        folder->child = &head;
        head.sibling->child = folder;
        head.sibling = folder;
    }

    /* 我们从给定的根文件夹开始，递归的获取它们*/
    rv = get_subfolders_for_folder(&head, 0x00000000U);

    /* 有一些设备可能在“根文件夹”0xffffffff里有一些文件，因此如果0x00000000没有返回任何文件夹，
     * 请查找根文件夹0xffffffffU的子目录*/
    if (rv == NULL) {
        rv = get_subfolders_for_folder(&head, 0xffffffffU);
        if (rv != NULL)
            LIBMTP_ERROR("Device have files in \"root folder\" 0xffffffffU - "
                         "this is a firmware bug (but continuing)\n");
    }

    /* 临时链表需要为空的，以防万一，清除任何剩余的孤儿对象*/
    while(head.sibling != &head) {
        LIBMTP_folder_t *curr = head.sibling;

        LIBMTP_INFO("Orphan folder with ID: 0x%08x name: \"%s\" encountered.\n",
                     curr->folder_id, curr->name);
        curr->sibling->child = curr->child;
        curr->child->sibling = curr->sibling;
        curr->child = NULL;
        curr->sibling = NULL;
        /* 销毁文件夹*/
        LIBMTP_destroy_folder_t(curr);
    }
    return rv;
}

/**
 * 返回当前MTP设备的所有可得目录链表
 *
 * @param device 要获取目录链表的MTP设备
 *
 * @return 成功返回获取到的目录链表
 **/
LIBMTP_folder_t *LIBMTP_Get_Folder_List(LIBMTP_mtpdevice_t *device)
{
    return LIBMTP_Get_Folder_List_For_Storage(device, PTP_GOH_ALL_STORAGE);
}

/**
 * 在当前的MTP设备上创建一个新的文件夹
 * PTP的文件夹名字是"association"。设备内部没有“文件夹”的概念，它包含
 * 所有文件的平面列表，有些文件是"associations"，其他文件和文件夹可以将其
 * 称为"父级"。
 *
 * @param device      MTP设备
 * @param name        要创建的文件夹的名字
 * @param parent_id   父目录ID
 * @param storage_id  当前存储设备ID
 *
 * @return 成功返回新创建的文件夹的ID，失败返回0
 **/
u32 LIBMTP_Create_Folder(LIBMTP_mtpdevice_t *device, char *name,
                         u32 parent_id, u32 storage_id)
{
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    u32 parenthandle = 0;
    u32 store;
    PTPObjectInfo new_folder;
    u16 ret;
    u32 new_id = 0;

    /* 没有指定存储设备ID*/
    if (storage_id == 0) {
        /* 这里猜测一个文件夹需要512字节*/
        /* 获取一个可用的存储设备*/
        store = get_suggested_storage_id(device, 512, parent_id);
    } else {
        store = storage_id;
    }
    parenthandle = parent_id;

    memset(&new_folder, 0, sizeof(new_folder));
    new_folder.Filename = name;
    /* 去除文件名中>0x7F的字符*/
    if (FLAG_ONLY_7BIT_FILENAMES(ptp_usb)) {
        strip_7bit_from_utf8(new_folder.Filename);
    }
    new_folder.ObjectCompressedSize = 0;
    new_folder.ObjectFormat = PTP_OFC_Association;
    new_folder.ProtectionStatus = PTP_PS_NoProtection;
    new_folder.AssociationType = PTP_AT_GenericFolder;
    new_folder.ParentObject = parent_id;
    new_folder.StorageID = store;

    /* 创建对象属性*/
    if (!(params->device_flags & DEVICE_FLAG_BROKEN_SEND_OBJECT_PROPLIST) &&
            ptp_operation_issupported(params, PTP_OC_MTP_SendObjectPropList)) {
        MTPProperties *props = (MTPProperties*)mtp_calloc(2, sizeof(MTPProperties));

        props[0].property = PTP_OPC_ObjectFileName;
        props[0].datatype = PTP_DTC_STR;
        props[0].propval.str = name;

        props[1].property = PTP_OPC_Name;
        props[1].datatype = PTP_DTC_STR;
        props[1].propval.str = name;
        /* 发送新的对象属性*/
        ret = ptp_mtp_sendobjectproplist(params, &store, &parenthandle, &new_id, PTP_OFC_Association,
                                         0, props, 1);
        mtp_free(props);
    } else {
        /* 发送对象信息*/
        ret = ptp_sendobjectinfo(params, &store, &parenthandle, &new_id, &new_folder);
    }

    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Create_Folder: Could not send object info.");
        if (ret == PTP_RC_AccessDenied) {
            add_ptp_error_to_errorstack(device, ret, "ACCESS DENIED.");
        }
        return 0;
    }
    /* 不要销毁新的文件夹对象信息，因为它静态地引用几个字符串*/

    /* 添加对象*/
    add_object_to_cache(device, new_id);

    return new_id;
}

/**
 * 创建一个新的播放列表元数据
 *
 * @return 成功返回一个新分配的元数据结构体
 */
LIBMTP_playlist_t *LIBMTP_new_playlist_t(void)
{
    LIBMTP_playlist_t *new = (LIBMTP_playlist_t *) mtp_malloc(sizeof(LIBMTP_playlist_t));
    if (new == NULL) {
        return NULL;
    }
    new->playlist_id = 0;
    new->parent_id = 0;
    new->storage_id = 0;
    new->name = NULL;
    new->tracks = NULL;
    new->no_tracks = 0;
    new->next = NULL;
    return new;
}

/**
 * 销毁一个播放列表元数据结构体
 *
 * @param playlist 要销毁的播放列表元数据
 */
void LIBMTP_destroy_playlist_t(LIBMTP_playlist_t *playlist)
{
    if (playlist == NULL) {
        return;
    }
    if (playlist->name != NULL)
        mtp_free(playlist->name);
    if (playlist->tracks != NULL)
        mtp_free(playlist->tracks);
    mtp_free(playlist);
    return;
}

/**
 * 这个函数检索设备一个独立的播放列表
 *
 * @param device 要获取播放列表的MTP设备
 * @param plid   要检索的播放列表的ID
 *
 * @return 成功返回一个有效的播放列表元数据
 */
LIBMTP_playlist_t *LIBMTP_Get_Playlist(LIBMTP_mtpdevice_t *device, u32 const plid)
{
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    const int REQ_SPL = FLAG_PLAYLIST_SPL(ptp_usb);
    PTPParams *params = (PTPParams *) device->params;
    PTPObject *ob;
    LIBMTP_playlist_t *pl;
    u16 ret;

    /* 获取所有句柄*/
    if (params->nrofobjects == 0) {
        flush_handles(device);
    }

    ret = ptp_object_want (params, plid, PTPOBJECT_OBJECTINFO_LOADED, &ob);
    if (ret != PTP_RC_OK)
        return NULL;

//todo
//    // For Samsung players we must look for the .spl extension explicitly since
//    // playlists are not stored as playlist objects.
//    if ( REQ_SPL && is_spl_playlist(&ob->oi) ) {
//        /* 分配一个新的播放列表类型*/
//        pl = LIBMTP_new_playlist_t();
//        spl_to_playlist_t(device, &ob->oi, ob->oid, pl);
//        return pl;
//    }

    /* 不是一个播放列表，忽略*/
    else if ( ob->oi.ObjectFormat != PTP_OFC_MTP_AbstractAudioVideoPlaylist ) {
        return NULL;
    }

    /* 分配一个新的播放列表类型*/
    pl = LIBMTP_new_playlist_t();

    pl->name = get_string_from_object(device, ob->oid, PTP_OPC_Name);
    if (pl->name == NULL) {
        pl->name = mtp_strdup(ob->oi.Filename);
    }
    pl->playlist_id = ob->oid;
    pl->parent_id = ob->oi.ParentObject;
    pl->storage_id = ob->oi.StorageID;

    /* 获取这个播放列表的曲目列表*/
    ret = ptp_mtp_getobjectreferences(params, pl->playlist_id, &pl->tracks, &pl->no_tracks);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Get_Playlist(): Could not get object references.");
        pl->tracks = NULL;
        pl->no_tracks = 0;
    }

    return pl;
}

/**
 * 创建一个播放列表或专辑的新的摘要列表
 *
 * @param device       要创建新的摘要列表的MTP设备
 * @param name         新的摘要列表的名字
 * @param genre        新的摘要列表的类型或为NULL
 * @param parenthandle 父文件夹的ID，0则代表根文件夹
 * @param objectformat 要创建的新的摘要列表的类型
 * @param suffix       用于此操作创建的虚拟“文件”的“.foo”(4字节)后缀
 * @param newid        新对象的ID
 * @param tracks       这个链表的曲目数组
 * @param no_tracks    曲目的数量
 *
 * @return 成功返回0，失败返回其他值
 */
static int create_new_abstract_list(LIBMTP_mtpdevice_t *device,
                                    char const * const name,
                                    char const * const artist,
                                    char const * const composer,
                                    char const * const genre,
                                    u32 const parenthandle,
                                    u32 const storageid,
                                    u16 const objectformat,
                                    char const * const suffix,
                                    u32 * const newid,
                                    u32 const * const tracks,
                                    u32 const no_tracks){
    int i;
    int supported = 0;
    u16 ret;
    u16 *properties = NULL;
    u32 propcnt = 0;
    u32 store;
    u32 localph = parenthandle;
    u8 nonconsumable = 0x00U; /* By default it is consumable */
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    char fname[256];
    //uint8_t data[2];

    /* 检查新的摘要列表名字*/
    if (!name) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): list name was NULL, using default name \"Unknown\"");
        return -1;
    }

    if (storageid == 0) {
        /* 先假设一个摘要列表的大小要512字节*/
        store = get_suggested_storage_id(device, 512, localph);
    } else {
        store = storageid;
    }

    /* 检查是否支持对象格式*/
    for (i = 0; i < params->deviceinfo.ImageFormats_len; i++ ) {
        if (params->deviceinfo.ImageFormats[i] == objectformat) {
            supported = 1;
            break;
        }
    }
    if (!supported) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): player does not support this abstract type");
        LIBMTP_ERROR("Unsupported abstract list type: %04x\n", objectformat);
        return -1;
    }
    /* 添加新的名字*/
    fname[0] = '\0';
    if (strlen(name) > strlen(suffix)) {
        char const * const suff = &name[strlen(name) - strlen(suffix)];
        if (!strcmp(suff, suffix)) {
            strncpy(fname, name, sizeof(fname));
        }
    }
    /* 在名字后面添加新的后缀名*/
    if (fname[0] == '\0') {
        strncpy(fname, name, sizeof(fname)-strlen(suffix)-1);
        strcat(fname, suffix);
        fname[sizeof(fname)-1] = '\0';
    }
    /* 设置对象属性链表*/
    if (ptp_operation_issupported(params, PTP_OC_MTP_SendObjectPropList) &&
            !FLAG_BROKEN_SEND_OBJECT_PROPLIST(ptp_usb)) {
        MTPProperties *props = NULL;
        MTPProperties *prop = NULL;
        int nrofprops = 0;

        *newid = 0x00000000U;
        /* 获取支持的对象属性*/
        ret = ptp_mtp_getobjectpropssupported(params, objectformat, &propcnt, &properties);

        for (i  =0;i < propcnt;i++) {
            PTPObjectPropDesc opd;
            /* 获取对象属性描述*/
            ret = ptp_mtp_getobjectpropdesc(params, properties[i], objectformat, &opd);
            if (ret != PTP_RC_OK) {
                add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): "
                        "could not get property description.");
            } else if (opd.GetSet) {
                /* 设置新的对象属性*/
                switch (properties[i]) {
                case PTP_OPC_ObjectFileName:
                    prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                    prop->ObjectHandle = *newid;
                    prop->property = PTP_OPC_ObjectFileName;
                    prop->datatype = PTP_DTC_STR;
                    prop->propval.str = mtp_strdup(fname);
                    if (FLAG_ONLY_7BIT_FILENAMES(ptp_usb)) {
                        strip_7bit_from_utf8(prop->propval.str);
                    }
                    break;
                case PTP_OPC_ProtectionStatus:
                    prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                    prop->ObjectHandle = *newid;
                    prop->property = PTP_OPC_ProtectionStatus;
                    prop->datatype = PTP_DTC_UINT16;
                    prop->propval.u16 = 0x0000U; /* Not protected */
                    break;
                case PTP_OPC_NonConsumable:
                    prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                    prop->ObjectHandle = *newid;
                    prop->property = PTP_OPC_NonConsumable;
                    prop->datatype = PTP_DTC_UINT8;
                    prop->propval.u8 = nonconsumable;
                    break;
                case PTP_OPC_Name:
                    if (name != NULL) {
                        prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                        prop->ObjectHandle = *newid;
                        prop->property = PTP_OPC_Name;
                        prop->datatype = PTP_DTC_STR;
                        prop->propval.str = mtp_strdup(name);
                    }
                    break;
                case PTP_OPC_AlbumArtist:
                    if (artist != NULL) {
                        prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                        prop->ObjectHandle = *newid;
                        prop->property = PTP_OPC_AlbumArtist;
                        prop->datatype = PTP_DTC_STR;
                        prop->propval.str = mtp_strdup(artist);
                    }
                    break;
                case PTP_OPC_Artist:
                    if (artist != NULL) {
                        prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                        prop->ObjectHandle = *newid;
                        prop->property = PTP_OPC_Artist;
                        prop->datatype = PTP_DTC_STR;
                        prop->propval.str = mtp_strdup(artist);
                    }
                    break;
                case PTP_OPC_Composer:
                    if (composer != NULL) {
                        prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                        prop->ObjectHandle = *newid;
                        prop->property = PTP_OPC_Composer;
                        prop->datatype = PTP_DTC_STR;
                        prop->propval.str = mtp_strdup(composer);
                    }
                    break;
                case PTP_OPC_Genre:
                    if (genre != NULL) {
                        prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                        prop->ObjectHandle = *newid;
                        prop->property = PTP_OPC_Genre;
                        prop->datatype = PTP_DTC_STR;
                        prop->propval.str = mtp_strdup(genre);
                    }
                    break;
                case PTP_OPC_DateModified:
                    /* 如果支持，则标记当前的时间*/
                    if (!FLAG_CANNOT_HANDLE_DATEMODIFIED(ptp_usb)) {
                        prop = ptp_get_new_object_prop_entry(&props,&nrofprops);
                        prop->ObjectHandle = *newid;
                        prop->property = PTP_OPC_DateModified;
                        prop->datatype = PTP_DTC_STR;
                        prop->propval.str = get_iso8601_stamp();
                    }
                    break;
                }
            }
            ptp_free_objectpropdesc(&opd);
        }
        mtp_free(properties);
        /* 发送新的属性链表*/
        ret = ptp_mtp_sendobjectproplist(params, &store, &localph, newid,
                                         objectformat, 0, props, nrofprops);

        /* 释放属性链表*/
        ptp_destroy_object_prop_list(props, nrofprops);

        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "create_new_abstract_list(): Could not send object property list.");
            if (ret == PTP_RC_AccessDenied) {
                add_ptp_error_to_errorstack(device, ret, "ACCESS DENIED.");
            }
            return -1;
        }

        /* 发送空白对象*/
        ret = ptp_sendobject(params, NULL, 0);
        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "create_new_abstract_list(): Could not send blank object data.");
            return -1;
        }

    } else if (ptp_operation_issupported(params,PTP_OC_SendObjectInfo)) {
        PTPObjectInfo new_object;

        new_object.Filename = fname;
        if (FLAG_ONLY_7BIT_FILENAMES(ptp_usb)) {
            strip_7bit_from_utf8(new_object.Filename);
        }
        /* 在某种情况上，这必须是1*/
        new_object.ObjectCompressedSize = 0;
        new_object.ObjectFormat = objectformat;

        /* 创建一个新的对象信息*/
        ret = ptp_sendobjectinfo(params, &store, &localph, newid, &new_object);
        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "create_new_abstract_list(): Could not send object info (the playlist itself).");
            if (ret == PTP_RC_AccessDenied) {
                add_ptp_error_to_errorstack(device, ret, "ACCESS DENIED.");
            }
            return -1;
        }
    // NOTE: don't destroy new_object objectinfo afterwards - the strings it contains are
    // not copies.

#if 0
    /*
     * At one time we had to send this one blank data byte.
     * If we didn't, the handle will not be created and thus there is
     * no playlist. Possibly this was masking some bug, so removing it
     * now.
     */
    data[0] = '\0';
    data[1] = '\0';
    ret = ptp_sendobject(params, data, 1);
    if (ret != PTP_RC_OK) {
      add_ptp_error_to_errorstack(device, ret, "create_new_abstract_list(): Could not send blank object data.");
      return -1;
    }
#endif

        ret = ptp_mtp_getobjectpropssupported(params, objectformat, &propcnt, &properties);
        /* 一个个设置属性*/
        for (i = 0;i < propcnt;i++) {
            PTPObjectPropDesc opd;
            /* 获取属性描述*/
            ret = ptp_mtp_getobjectpropdesc(params, properties[i], objectformat, &opd);
            if (ret != PTP_RC_OK) {
                add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): "
                                        "could not get property description.");
            } else if (opd.GetSet) {
                switch (properties[i]) {
                case PTP_OPC_Name:
                    if (name != NULL) {
                        ret = set_object_string(device, *newid, PTP_OPC_Name, name);
                        if (ret != 0) {
                            add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): could not set entity name.");
                            return -1;
                        }
                    }
                    break;
                case PTP_OPC_AlbumArtist:
                    if (artist != NULL) {
                        ret = set_object_string(device, *newid, PTP_OPC_AlbumArtist, artist);
                        if (ret != 0) {
                            add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): could not set entity album artist.");
                            return -1;
                        }
                    }
                    break;
                case PTP_OPC_Artist:
                    if (artist != NULL) {
                        ret = set_object_string(device, *newid, PTP_OPC_Artist, artist);
                        if (ret != 0) {
                            add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): could not set entity artist.");
                            return -1;
                        }
                    }
                    break;
                case PTP_OPC_Composer:
                    if (composer != NULL) {
                        ret = set_object_string(device, *newid, PTP_OPC_Composer, composer);
                        if (ret != 0) {
                            add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): could not set entity composer.");
                            return -1;
                        }
                    }
                    break;
                case PTP_OPC_Genre:
                    if (genre != NULL) {
                        ret = set_object_string(device, *newid, PTP_OPC_Genre, genre);
                        if (ret != 0) {
                            add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): could not set entity genre.");
                            return -1;
                        }
                    }
                    break;
                case PTP_OPC_DateModified:
                    if (!FLAG_CANNOT_HANDLE_DATEMODIFIED(ptp_usb)) {
                        ret = set_object_string(device, *newid, PTP_OPC_DateModified, get_iso8601_stamp());
                        if (ret != 0) {
                            add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): could not set date modified.");
                            return -1;
                        }
                    }
                    break;
                }
            }
            ptp_free_objectpropdesc(&opd);
        }
        mtp_free(properties);
    }

    if (no_tracks > 0) {
        /* 将曲目作为对象引用添加到链表中*/
        ret = ptp_mtp_setobjectreferences (params, *newid, (u32 *) tracks, no_tracks);
        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "create_new_abstract_list(): could not add tracks as object references.");
            return -1;
        }
    }
    /* 把对象添加到缓存里*/
    add_object_to_cache(device, *newid);

    return 0;
}

/**
 * 创建分配一个新的专辑元数据结构体
 *
 * @return 成功返回新创建的专辑元数据结构体
 */
LIBMTP_album_t *LIBMTP_new_album_t(void)
{
    LIBMTP_album_t *new = (LIBMTP_album_t *) mtp_malloc(sizeof(LIBMTP_album_t));
    if (new == NULL) {
        return NULL;
    }
    new->album_id = 0;
    new->parent_id = 0;
    new->storage_id = 0;
    new->name = NULL;
    new->artist = NULL;
    new->composer = NULL;
    new->genre = NULL;
    new->tracks = NULL;
    new->no_tracks = 0;
    new->next = NULL;
    return new;
}

/**
 * 删除专辑结构体的内存
 *
 * @param album 要销毁的专辑结构体
 */
void LIBMTP_destroy_album_t(LIBMTP_album_t *album)
{
    if (album == NULL) {
        return;
    }
    if (album->name != NULL)
        mtp_free(album->name);
    if (album->artist != NULL)
        mtp_free(album->artist);
    if (album->composer != NULL)
        mtp_free(album->composer);
    if (album->genre != NULL)
        mtp_free(album->genre);
    if (album->tracks != NULL)
        mtp_free(album->tracks);
    mtp_free(album);
    return;
}

/**
 * 这个函数将属性映射并复制到相册元数据(如果适用)
 */
static void pick_property_to_album_metadata(LIBMTP_mtpdevice_t *device,
                                            MTPProperties *prop, LIBMTP_album_t *alb)
{
    switch (prop->property) {
    case PTP_OPC_Name:
        if (prop->propval.str != NULL)
            alb->name = mtp_strdup(prop->propval.str);
        else
            alb->name = NULL;
        break;
    case PTP_OPC_AlbumArtist:
        if (prop->propval.str != NULL) {
            // This should take precedence over plain "Artist"
            if (alb->artist != NULL)
                mtp_free(alb->artist);
            alb->artist = mtp_strdup(prop->propval.str);
        } else
            alb->artist = NULL;
        break;
    case PTP_OPC_Artist:
        if (prop->propval.str != NULL) {
            // Only use of AlbumArtist is not set
            if (alb->artist == NULL)
                alb->artist = mtp_strdup(prop->propval.str);
        } else
            alb->artist = NULL;
        break;
    case PTP_OPC_Composer:
        if (prop->propval.str != NULL)
            alb->composer = mtp_strdup(prop->propval.str);
        else
            alb->composer = NULL;
        break;
    case PTP_OPC_Genre:
        if (prop->propval.str != NULL)
            alb->genre = mtp_strdup(prop->propval.str);
        else
            alb->genre = NULL;
        break;
    }
}

/**
 * 检索一个专辑的元数据
 *
 * @param device  MTP设备
 * @param alb     要返回的专辑的元数据
 */
static void get_album_metadata(LIBMTP_mtpdevice_t *device,
                               LIBMTP_album_t *alb)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    u32 i;
    MTPProperties *prop;
    PTPObject *ob;

    /* 如果我们有缓存大型元数据集，使用它*/
    ret = ptp_object_want(params, alb->album_id, PTPOBJECT_MTPPROPLIST_LOADED, &ob);
    if (ob->mtpprops) {
        prop = ob->mtpprops;
        for (i = 0;i < ob->nrofmtpprops; i++ ,prop++)
            pick_property_to_album_metadata(device, prop, alb);
    } else {
        u16 *props = NULL;
        u32 propcnt = 0;

        /* 检索这个专辑有什么属性*/
        ret = ptp_mtp_getobjectpropssupported(params, PTP_OFC_MTP_AbstractAudioAlbum, &propcnt, &props);
        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "get_album_metadata(): call to ptp_mtp_getobjectpropssupported() failed.");
            return;
        } else {
            /* 获取专辑信息*/
            for (i = 0;i < propcnt;i++) {
                switch (props[i]) {
                case PTP_OPC_Name:
                    alb->name = get_string_from_object(device, ob->oid, PTP_OPC_Name);
                    break;
                case PTP_OPC_AlbumArtist:
                    if (alb->artist != NULL)
                        mtp_free(alb->artist);
                    alb->artist = get_string_from_object(device, ob->oid, PTP_OPC_AlbumArtist);
                    break;
                case PTP_OPC_Artist:
                    if (alb->artist == NULL)
                        alb->artist = get_string_from_object(device, ob->oid, PTP_OPC_Artist);
                    break;
                case PTP_OPC_Composer:
                    alb->composer = get_string_from_object(device, ob->oid, PTP_OPC_Composer);
                    break;
                case PTP_OPC_Genre:
                    alb->genre = get_string_from_object(device, ob->oid, PTP_OPC_Genre);
                    break;
                default:
                    break;
                }
            }
            mtp_free(props);
        }
    }
}

/**
 * 检索设备的一个单独的专辑
 *
 * @param device 要获取专辑的MTP设备
 * @param albid  要获取专辑的ID
 *
 * @return 成功返回一个有效的专辑元数据，失败返回NULL
 */
LIBMTP_album_t *LIBMTP_Get_Album(LIBMTP_mtpdevice_t *device, u32 const albid)
{
    PTPParams *params = (PTPParams *) device->params;
    u16 ret;
    PTPObject *ob;
    LIBMTP_album_t *alb;

    /* 获取所有句柄*/
    if (params->nrofobjects == 0)
        flush_handles(device);

    ret = ptp_object_want(params, albid, PTPOBJECT_OBJECTINFO_LOADED, &ob);
    if (ret != PTP_RC_OK)
        return NULL;

    /* 检查是不是一个专辑*/
    if (ob->oi.ObjectFormat != PTP_OFC_MTP_AbstractAudioAlbum)
        return NULL;

    /* 分配一个新的专辑类型*/
    alb = LIBMTP_new_album_t();
    alb->album_id = ob->oid;
    alb->parent_id = ob->oi.ParentObject;
    alb->storage_id = ob->oi.StorageID;

    /* 获取支持的元数据*/
    get_album_metadata(device, alb);

    /* 获取这个专辑的曲目链表*/
    ret = ptp_mtp_getobjectreferences(params, alb->album_id, &alb->tracks, &alb->no_tracks);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Get_Album: Could not get object references.");
        alb->tracks = NULL;
        alb->no_tracks = 0;
    }

    return alb;
}

/**
 * 基于提供的专辑元数据创建一个新的专辑
 *
 * @param device   相关的MTP设备
 * @param metadata 要创建的新专辑元数据
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Create_New_Album(LIBMTP_mtpdevice_t *device, LIBMTP_album_t * const metadata)
{
    u32 localph = metadata->parent_id;

    /* 如果没有则使用默认的文件夹*/
    if (localph == 0) {
        if (device->default_album_folder != 0)
            localph = device->default_album_folder;
        else
            localph = device->default_music_folder;
    }
    metadata->parent_id = localph;

    /* 创建新的摘要列表*/
    return create_new_abstract_list(device,
                                    metadata->name,
                                    metadata->artist,
                                    metadata->composer,
                                    metadata->genre,
                                    localph,
                                    metadata->storage_id,
                                    PTP_OFC_MTP_AbstractAudioAlbum,
                                    ".alb",
                                    &metadata->album_id,
                                    metadata->tracks,
                                    metadata->no_tracks);
}

/**
 * 这个函数发送一个对象的代表性样本数据。数据应该是播放器可接受的格式(iRiver和Creative，似乎是JPEG)。
 * 和不能过大。(Creative最大是20KB)。
 *
 * @param device     相关的MTP设备
 * @param id         要设置的对象ID
 * @param sampledata 要发送的样本数据
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Send_Representative_Sample(LIBMTP_mtpdevice_t *device,
                                      u32 const id,
                                      LIBMTP_filesampledata_t *sampledata)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    PTPPropertyValue propval;
    PTPObject *ob;
    u32 i;
    u16 *props = NULL;
    u32 propcnt = 0;
    int supported = 0;

    /* 获取要为其发送代表性数据的对象的文件格式*/
    ret = ptp_object_want (params, id, PTPOBJECT_OBJECTINFO_LOADED, &ob);
    if (ret != PTP_RC_OK) {
      add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Send_Representative_Sample(): could not get object info.");
      return -1;
    }

    /* 检查我们能发送的代表性样本数据的对象格式*/
    ret = ptp_mtp_getobjectpropssupported(params, ob->oi.ObjectFormat, &propcnt, &props);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Send_Representative_Sample(): could not get object properties.");
        return -1;
    }

    for (i = 0; i < propcnt; i++) {
        if (props[i] == PTP_OPC_RepresentativeSampleData) {
            supported = 1;
            break;
        }
    }
    if (!supported) {
        mtp_free(props);
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Send_Representative_Sample(): object type doesn't support RepresentativeSampleData.");
        return -1;
    }
    mtp_free(props);

    /* 继续发送数据*/
    propval.a.count = sampledata->size;
    propval.a.v = mtp_malloc(sizeof(PTPPropertyValue) * sampledata->size);
    for (i = 0; i < sampledata->size; i++) {
        propval.a.v[i].u8 = sampledata->data[i];
    }
    /* 获取对象属性值*/
    ret = ptp_mtp_setobjectpropvalue(params,id,PTP_OPC_RepresentativeSampleData,
                                     &propval,PTP_DTC_AUINT8);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Send_Representative_Sample(): could not send sample data.");
        mtp_free(propval.a.v);
        return -1;
    }
    mtp_free(propval.a.v);

    /* 如果样本是图片，设置宽和高，其他的就设置持续时间和数据大小*/
    switch(sampledata->filetype) {
    case LIBMTP_FILETYPE_JPEG:
    case LIBMTP_FILETYPE_JFIF:
    case LIBMTP_FILETYPE_TIFF:
    case LIBMTP_FILETYPE_BMP:
    case LIBMTP_FILETYPE_GIF:
    case LIBMTP_FILETYPE_PICT:
    case LIBMTP_FILETYPE_PNG:
        if (!FLAG_BROKEN_SET_SAMPLE_DIMENSIONS(ptp_usb)) {
            /* 图像，设置宽和高*/
            set_object_u32(device, id, PTP_OPC_RepresentativeSampleHeight, sampledata->height);
            set_object_u32(device, id, PTP_OPC_RepresentativeSampleWidth, sampledata->width);
        }
        break;
    default:
        /* 不是图像，设置持续时间和数据大小*/
        set_object_u32(device, id, PTP_OPC_RepresentativeSampleDuration, sampledata->duration);
        set_object_u32(device, id, PTP_OPC_RepresentativeSampleSize, sampledata->size);
        break;
    }

    return 0;
}

/**
 * 获取对象的代表性样本数据，如果设备支持，这里使用专辑的代表性样本数据属性
 *
 * @param device  MTP设备
 * @param id      要获取数据的对象的ID
 * @param pointer 要获取数据的LIBMTP_filesampledata_t结构体
 *
 * @return 成功返回0，失败返回其他值
 */
int LIBMTP_Get_Representative_Sample(LIBMTP_mtpdevice_t *device,
                                     u32 const id,
                                     LIBMTP_filesampledata_t *sampledata)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    PTPPropertyValue propval;
    PTPObject *ob;
    u32 i;
    u16 *props = NULL;
    u32 propcnt = 0;
    int supported = 0;

    /* 获取要为其发送代表性数据的对象的文件格式*/
    ret = ptp_object_want (params, id, PTPOBJECT_OBJECTINFO_LOADED, &ob);
    if (ret != PTP_RC_OK) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Get_Representative_Sample(): could not get object info.");
        return -1;
    }

    /* 检查我们是否可以存储这个对象格式的代表性样本性数据*/
    ret = ptp_mtp_getobjectpropssupported(params, ob->oi.ObjectFormat, &propcnt, &props);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Get_Representative_Sample(): could not get object properties.");
        return -1;
    }

    for (i = 0; i < propcnt; i++) {
        if (props[i] == PTP_OPC_RepresentativeSampleData) {
            supported = 1;
            break;
        }
    }
    if (!supported) {
        mtp_free(props);
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Get_Representative_Sample(): object type doesn't support RepresentativeSampleData.");
        return -1;
    }
    mtp_free(props);

    /* 获取数据*/
    ret = ptp_mtp_getobjectpropvalue(params, id, PTP_OPC_RepresentativeSampleData,
                   &propval, PTP_DTC_AUINT8);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Get_Representative_Sample(): could not get sample data.");
        return -1;
    }

    /* 储存数据*/
    sampledata->size = propval.a.count;
    sampledata->data = mtp_malloc(sizeof(PTPPropertyValue) * propval.a.count);
    for (i = 0; i < propval.a.count; i++) {
        sampledata->data[i] = propval.a.v[i].u8;
    }
    mtp_free(propval.a.v);

    /* 获取其他属性*/
    sampledata->width = get_u32_from_object(device, id, PTP_OPC_RepresentativeSampleWidth, 0);
    sampledata->height = get_u32_from_object(device, id, PTP_OPC_RepresentativeSampleHeight, 0);
    sampledata->duration = get_u32_from_object(device, id, PTP_OPC_RepresentativeSampleDuration, 0);
    sampledata->filetype = map_ptp_type_to_libmtp_type(
            get_u16_from_object(device, id, PTP_OPC_RepresentativeSampleFormat, LIBMTP_FILETYPE_UNKNOWN));

    return 0;
}

/**
 * 创建分配一个新的样本数据元数据结构体
 *
 * @return 成功返回新创建的样本数据结构体的指针
 */
LIBMTP_filesampledata_t *LIBMTP_new_filesampledata_t(void)
{
    LIBMTP_filesampledata_t *new = (LIBMTP_filesampledata_t *) mtp_malloc(sizeof(LIBMTP_filesampledata_t));
    if (new == NULL) {
        return NULL;
    }
    new->height = 0;
    new->width = 0;
    new->data = NULL;
    new->duration = 0;
    new->size = 0;
    return new;
}

/**
 * 销毁文件样本数据
 *
 * @param sample 要销毁的文件样本元数据
 */
void LIBMTP_destroy_filesampledata_t(LIBMTP_filesampledata_t * sample)
{
    if (sample == NULL) {
        return;
    }
    if (sample->data != NULL) {
        mtp_free(sample->data);
    }
    mtp_free(sample);
}


/**
 * 添加一个对象到缓存
 *
 * @param device    要添加缓存的MTP设备
 * @param object_id 要添加到缓存的对象ID
 **/
static void add_object_to_cache(LIBMTP_mtpdevice_t *device, u32 object_id)
{
    PTPParams *params = (PTPParams *)device->params;
    u16 ret;

    /* 添加对象*/
    ret = ptp_add_object_to_cache(params, object_id);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "add_object_to_cache(): couldn't add object to cache");
    }
}

/**
 * 对象被修改后更新缓存
 *
 * @param device    要更新缓存的MTP设备
 * @param object_id 要更新缓存的对象ID
 */
static void update_metadata_cache(LIBMTP_mtpdevice_t *device, u32 object_id)
{
    PTPParams *params = (PTPParams *)device->params;
    /* 从缓存中删除原来的对象*/
    ptp_remove_object_from_cache(params, object_id);
    /* 再添加对象*/
    add_object_to_cache(device, object_id);
}
