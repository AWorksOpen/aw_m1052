/**
 * \file libmtp.h
 * Interface to the Media Transfer Protocol library.
 *
 * Copyright (C) 2005-2013 Linus Walleij <triad@df.lth.se>
 * Copyright (C) 2005-2008 Richard A. Low <richard@wentnet.com>
 * Copyright (C) 2007 Ted Bullock <tbullock@canada.com>
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
 * <code>
 * #include <libmtp.h>
 * </code>
 */
#ifndef LIBMTP_H_INCLUSION_GUARD
#define LIBMTP_H_INCLUSION_GUARD

#define LIBMTP_VERSION 1.1.17
#define LIBMTP_VERSION_STRING "1.1.17"
#include <stdio.h>

typedef char                 s8;
typedef unsigned char        u8;
typedef short                s16;
typedef unsigned short       u16;
typedef int                  s32;
typedef unsigned int         u32;
typedef long long            s64;
typedef unsigned long long   u64;
#ifndef time_t
#define time_t long
#endif


/* 这里定义的调试标志是MTP库接口用的外部标志
 * 请保持这个列表与libmtp.c同步*/
#define LIBMTP_DEBUG_NONE       0x00
#define LIBMTP_DEBUG_PTP        0x01
#define LIBMTP_DEBUG_PLST       0x02
#define LIBMTP_DEBUG_USB        0x04
#define LIBMTP_DEBUG_DATA       0x08
#define LIBMTP_DEBUG_ALL        0xFF


/**
 * 这里定义的文件类型是MTP库接口使用的外部类型。内部用作PTP(图片传输协议)定义的枚举类型有所不同
 */
typedef enum {
  LIBMTP_FILETYPE_FOLDER,
  LIBMTP_FILETYPE_WAV,
  LIBMTP_FILETYPE_MP3,
  LIBMTP_FILETYPE_WMA,
  LIBMTP_FILETYPE_OGG,
  LIBMTP_FILETYPE_AUDIBLE,
  LIBMTP_FILETYPE_MP4,
  LIBMTP_FILETYPE_UNDEF_AUDIO,
  LIBMTP_FILETYPE_WMV,
  LIBMTP_FILETYPE_AVI,
  LIBMTP_FILETYPE_MPEG,
  LIBMTP_FILETYPE_ASF,
  LIBMTP_FILETYPE_QT,
  LIBMTP_FILETYPE_UNDEF_VIDEO,
  LIBMTP_FILETYPE_JPEG,
  LIBMTP_FILETYPE_JFIF,
  LIBMTP_FILETYPE_TIFF,
  LIBMTP_FILETYPE_BMP,
  LIBMTP_FILETYPE_GIF,
  LIBMTP_FILETYPE_PICT,
  LIBMTP_FILETYPE_PNG,
  LIBMTP_FILETYPE_VCALENDAR1,
  LIBMTP_FILETYPE_VCALENDAR2,
  LIBMTP_FILETYPE_VCARD2,
  LIBMTP_FILETYPE_VCARD3,
  LIBMTP_FILETYPE_WINDOWSIMAGEFORMAT,
  LIBMTP_FILETYPE_WINEXEC,
  LIBMTP_FILETYPE_TEXT,
  LIBMTP_FILETYPE_HTML,
  LIBMTP_FILETYPE_FIRMWARE,
  LIBMTP_FILETYPE_AAC,
  LIBMTP_FILETYPE_MEDIACARD,
  LIBMTP_FILETYPE_FLAC,
  LIBMTP_FILETYPE_MP2,
  LIBMTP_FILETYPE_M4A,
  LIBMTP_FILETYPE_DOC,
  LIBMTP_FILETYPE_XML,
  LIBMTP_FILETYPE_XLS,
  LIBMTP_FILETYPE_PPT,
  LIBMTP_FILETYPE_MHT,
  LIBMTP_FILETYPE_JP2,
  LIBMTP_FILETYPE_JPX,
  LIBMTP_FILETYPE_ALBUM,
  LIBMTP_FILETYPE_PLAYLIST,
  LIBMTP_FILETYPE_UNKNOWN
} LIBMTP_filetype_t;


/**
 * 音频文件类型
 */
#define LIBMTP_FILETYPE_IS_AUDIO(a)\
(a == LIBMTP_FILETYPE_WAV ||\
 a == LIBMTP_FILETYPE_MP3 ||\
 a == LIBMTP_FILETYPE_MP2 ||\
 a == LIBMTP_FILETYPE_WMA ||\
 a == LIBMTP_FILETYPE_OGG ||\
 a == LIBMTP_FILETYPE_FLAC ||\
 a == LIBMTP_FILETYPE_AAC ||\
 a == LIBMTP_FILETYPE_M4A ||\
 a == LIBMTP_FILETYPE_AUDIBLE ||\
 a == LIBMTP_FILETYPE_UNDEF_AUDIO)

/**
 * 视频文件类型
 */
#define LIBMTP_FILETYPE_IS_VIDEO(a)\
(a == LIBMTP_FILETYPE_WMV ||\
 a == LIBMTP_FILETYPE_AVI ||\
 a == LIBMTP_FILETYPE_MPEG ||\
 a == LIBMTP_FILETYPE_UNDEF_VIDEO)

/**
 *  \def LIBMTP_FILETYPE_IS_AUDIOVIDEO
 *  Audio and&slash;or video filetype test.
 */
#define LIBMTP_FILETYPE_IS_AUDIOVIDEO(a)\
(a == LIBMTP_FILETYPE_MP4 ||\
 a == LIBMTP_FILETYPE_ASF ||\
 a == LIBMTP_FILETYPE_QT)

/**
 * 曲目文件类型
 */
#define LIBMTP_FILETYPE_IS_TRACK(a)\
(LIBMTP_FILETYPE_IS_AUDIO(a) ||\
 LIBMTP_FILETYPE_IS_VIDEO(a) ||\
 LIBMTP_FILETYPE_IS_AUDIOVIDEO(a))

/**
 * 这里定义的特性是MTP库接口使用的内部类型。
 */
typedef enum {
  LIBMTP_PROPERTY_StorageID,
  LIBMTP_PROPERTY_ObjectFormat,
  LIBMTP_PROPERTY_ProtectionStatus,
  LIBMTP_PROPERTY_ObjectSize,
  LIBMTP_PROPERTY_AssociationType,
  LIBMTP_PROPERTY_AssociationDesc,
  LIBMTP_PROPERTY_ObjectFileName,
  LIBMTP_PROPERTY_DateCreated,
  LIBMTP_PROPERTY_DateModified,
  LIBMTP_PROPERTY_Keywords,
  LIBMTP_PROPERTY_ParentObject,
  LIBMTP_PROPERTY_AllowedFolderContents,
  LIBMTP_PROPERTY_Hidden,
  LIBMTP_PROPERTY_SystemObject,
  LIBMTP_PROPERTY_PersistantUniqueObjectIdentifier,
  LIBMTP_PROPERTY_SyncID,
  LIBMTP_PROPERTY_PropertyBag,
  LIBMTP_PROPERTY_Name,
  LIBMTP_PROPERTY_CreatedBy,
  LIBMTP_PROPERTY_Artist,
  LIBMTP_PROPERTY_DateAuthored,
  LIBMTP_PROPERTY_Description,
  LIBMTP_PROPERTY_URLReference,
  LIBMTP_PROPERTY_LanguageLocale,
  LIBMTP_PROPERTY_CopyrightInformation,
  LIBMTP_PROPERTY_Source,
  LIBMTP_PROPERTY_OriginLocation,
  LIBMTP_PROPERTY_DateAdded,
  LIBMTP_PROPERTY_NonConsumable,
  LIBMTP_PROPERTY_CorruptOrUnplayable,
  LIBMTP_PROPERTY_ProducerSerialNumber,
  LIBMTP_PROPERTY_RepresentativeSampleFormat,
  LIBMTP_PROPERTY_RepresentativeSampleSize,
  LIBMTP_PROPERTY_RepresentativeSampleHeight,
  LIBMTP_PROPERTY_RepresentativeSampleWidth,
  LIBMTP_PROPERTY_RepresentativeSampleDuration,
  LIBMTP_PROPERTY_RepresentativeSampleData,
  LIBMTP_PROPERTY_Width,
  LIBMTP_PROPERTY_Height,
  LIBMTP_PROPERTY_Duration,
  LIBMTP_PROPERTY_Rating,
  LIBMTP_PROPERTY_Track,
  LIBMTP_PROPERTY_Genre,
  LIBMTP_PROPERTY_Credits,
  LIBMTP_PROPERTY_Lyrics,
  LIBMTP_PROPERTY_SubscriptionContentID,
  LIBMTP_PROPERTY_ProducedBy,
  LIBMTP_PROPERTY_UseCount,
  LIBMTP_PROPERTY_SkipCount,
  LIBMTP_PROPERTY_LastAccessed,
  LIBMTP_PROPERTY_ParentalRating,
  LIBMTP_PROPERTY_MetaGenre,
  LIBMTP_PROPERTY_Composer,
  LIBMTP_PROPERTY_EffectiveRating,
  LIBMTP_PROPERTY_Subtitle,
  LIBMTP_PROPERTY_OriginalReleaseDate,
  LIBMTP_PROPERTY_AlbumName,
  LIBMTP_PROPERTY_AlbumArtist,
  LIBMTP_PROPERTY_Mood,
  LIBMTP_PROPERTY_DRMStatus,
  LIBMTP_PROPERTY_SubDescription,
  LIBMTP_PROPERTY_IsCropped,
  LIBMTP_PROPERTY_IsColorCorrected,
  LIBMTP_PROPERTY_ImageBitDepth,
  LIBMTP_PROPERTY_Fnumber,
  LIBMTP_PROPERTY_ExposureTime,
  LIBMTP_PROPERTY_ExposureIndex,
  LIBMTP_PROPERTY_DisplayName,
  LIBMTP_PROPERTY_BodyText,
  LIBMTP_PROPERTY_Subject,
  LIBMTP_PROPERTY_Priority,
  LIBMTP_PROPERTY_GivenName,
  LIBMTP_PROPERTY_MiddleNames,
  LIBMTP_PROPERTY_FamilyName,
  LIBMTP_PROPERTY_Prefix,
  LIBMTP_PROPERTY_Suffix,
  LIBMTP_PROPERTY_PhoneticGivenName,
  LIBMTP_PROPERTY_PhoneticFamilyName,
  LIBMTP_PROPERTY_EmailPrimary,
  LIBMTP_PROPERTY_EmailPersonal1,
  LIBMTP_PROPERTY_EmailPersonal2,
  LIBMTP_PROPERTY_EmailBusiness1,
  LIBMTP_PROPERTY_EmailBusiness2,
  LIBMTP_PROPERTY_EmailOthers,
  LIBMTP_PROPERTY_PhoneNumberPrimary,
  LIBMTP_PROPERTY_PhoneNumberPersonal,
  LIBMTP_PROPERTY_PhoneNumberPersonal2,
  LIBMTP_PROPERTY_PhoneNumberBusiness,
  LIBMTP_PROPERTY_PhoneNumberBusiness2,
  LIBMTP_PROPERTY_PhoneNumberMobile,
  LIBMTP_PROPERTY_PhoneNumberMobile2,
  LIBMTP_PROPERTY_FaxNumberPrimary,
  LIBMTP_PROPERTY_FaxNumberPersonal,
  LIBMTP_PROPERTY_FaxNumberBusiness,
  LIBMTP_PROPERTY_PagerNumber,
  LIBMTP_PROPERTY_PhoneNumberOthers,
  LIBMTP_PROPERTY_PrimaryWebAddress,
  LIBMTP_PROPERTY_PersonalWebAddress,
  LIBMTP_PROPERTY_BusinessWebAddress,
  LIBMTP_PROPERTY_InstantMessengerAddress,
  LIBMTP_PROPERTY_InstantMessengerAddress2,
  LIBMTP_PROPERTY_InstantMessengerAddress3,
  LIBMTP_PROPERTY_PostalAddressPersonalFull,
  LIBMTP_PROPERTY_PostalAddressPersonalFullLine1,
  LIBMTP_PROPERTY_PostalAddressPersonalFullLine2,
  LIBMTP_PROPERTY_PostalAddressPersonalFullCity,
  LIBMTP_PROPERTY_PostalAddressPersonalFullRegion,
  LIBMTP_PROPERTY_PostalAddressPersonalFullPostalCode,
  LIBMTP_PROPERTY_PostalAddressPersonalFullCountry,
  LIBMTP_PROPERTY_PostalAddressBusinessFull,
  LIBMTP_PROPERTY_PostalAddressBusinessLine1,
  LIBMTP_PROPERTY_PostalAddressBusinessLine2,
  LIBMTP_PROPERTY_PostalAddressBusinessCity,
  LIBMTP_PROPERTY_PostalAddressBusinessRegion,
  LIBMTP_PROPERTY_PostalAddressBusinessPostalCode,
  LIBMTP_PROPERTY_PostalAddressBusinessCountry,
  LIBMTP_PROPERTY_PostalAddressOtherFull,
  LIBMTP_PROPERTY_PostalAddressOtherLine1,
  LIBMTP_PROPERTY_PostalAddressOtherLine2,
  LIBMTP_PROPERTY_PostalAddressOtherCity,
  LIBMTP_PROPERTY_PostalAddressOtherRegion,
  LIBMTP_PROPERTY_PostalAddressOtherPostalCode,
  LIBMTP_PROPERTY_PostalAddressOtherCountry,
  LIBMTP_PROPERTY_OrganizationName,
  LIBMTP_PROPERTY_PhoneticOrganizationName,
  LIBMTP_PROPERTY_Role,
  LIBMTP_PROPERTY_Birthdate,
  LIBMTP_PROPERTY_MessageTo,
  LIBMTP_PROPERTY_MessageCC,
  LIBMTP_PROPERTY_MessageBCC,
  LIBMTP_PROPERTY_MessageRead,
  LIBMTP_PROPERTY_MessageReceivedTime,
  LIBMTP_PROPERTY_MessageSender,
  LIBMTP_PROPERTY_ActivityBeginTime,
  LIBMTP_PROPERTY_ActivityEndTime,
  LIBMTP_PROPERTY_ActivityLocation,
  LIBMTP_PROPERTY_ActivityRequiredAttendees,
  LIBMTP_PROPERTY_ActivityOptionalAttendees,
  LIBMTP_PROPERTY_ActivityResources,
  LIBMTP_PROPERTY_ActivityAccepted,
  LIBMTP_PROPERTY_Owner,
  LIBMTP_PROPERTY_Editor,
  LIBMTP_PROPERTY_Webmaster,
  LIBMTP_PROPERTY_URLSource,
  LIBMTP_PROPERTY_URLDestination,
  LIBMTP_PROPERTY_TimeBookmark,
  LIBMTP_PROPERTY_ObjectBookmark,
  LIBMTP_PROPERTY_ByteBookmark,
  LIBMTP_PROPERTY_LastBuildDate,
  LIBMTP_PROPERTY_TimetoLive,
  LIBMTP_PROPERTY_MediaGUID,
  LIBMTP_PROPERTY_TotalBitRate,
  LIBMTP_PROPERTY_BitRateType,
  LIBMTP_PROPERTY_SampleRate,
  LIBMTP_PROPERTY_NumberOfChannels,
  LIBMTP_PROPERTY_AudioBitDepth,
  LIBMTP_PROPERTY_ScanDepth,
  LIBMTP_PROPERTY_AudioWAVECodec,
  LIBMTP_PROPERTY_AudioBitRate,
  LIBMTP_PROPERTY_VideoFourCCCodec,
  LIBMTP_PROPERTY_VideoBitRate,
  LIBMTP_PROPERTY_FramesPerThousandSeconds,
  LIBMTP_PROPERTY_KeyFrameDistance,
  LIBMTP_PROPERTY_BufferSize,
  LIBMTP_PROPERTY_EncodingQuality,
  LIBMTP_PROPERTY_EncodingProfile,
  LIBMTP_PROPERTY_BuyFlag,
  LIBMTP_PROPERTY_UNKNOWN
} LIBMTP_property_t;

/**
 * 这些是错误代码。你也可以获取错误代表的字符串
 */
typedef enum {
  LIBMTP_ERROR_NONE,
  LIBMTP_ERROR_GENERAL,
  LIBMTP_ERROR_PTP_LAYER,
  LIBMTP_ERROR_USB_LAYER,
  LIBMTP_ERROR_MEMORY_ALLOCATION,
  LIBMTP_ERROR_NO_DEVICE_ATTACHED,
  LIBMTP_ERROR_STORAGE_FULL,
  LIBMTP_ERROR_CONNECTING,
  LIBMTP_ERROR_CANCELLED
} LIBMTP_error_number_t;

typedef struct LIBMTP_device_entry_struct LIBMTP_device_entry_t;         /**< @see LIBMTP_device_entry_struct */
typedef struct LIBMTP_raw_device_struct LIBMTP_raw_device_t;             /**< @see LIBMTP_raw_device_struct */
typedef struct LIBMTP_error_struct LIBMTP_error_t;                       /**< @see LIBMTP_error_struct */
typedef struct LIBMTP_device_extension_struct LIBMTP_device_extension_t; /** < @see LIBMTP_device_extension_struct */
typedef struct LIBMTP_mtpdevice_struct LIBMTP_mtpdevice_t;               /**< @see LIBMTP_mtpdevice_struct */
typedef struct LIBMTP_file_struct LIBMTP_file_t;                         /**< @see LIBMTP_file_struct */
typedef struct LIBMTP_track_struct LIBMTP_track_t;                       /**< @see LIBMTP_track_struct */
typedef struct LIBMTP_playlist_struct LIBMTP_playlist_t;                 /**< @see LIBMTP_playlist_struct */
typedef struct LIBMTP_album_struct LIBMTP_album_t;                       /**< @see LIBMTP_album_struct */
typedef struct LIBMTP_folder_struct LIBMTP_folder_t;                     /**< @see LIBMTP_folder_t */
typedef struct LIBMTP_filesampledata_struct LIBMTP_filesampledata_t;     /**< @see LIBMTP_filesample_t */
typedef struct LIBMTP_devicestorage_struct LIBMTP_devicestorage_t;       /**< @see LIBMTP_devicestorage_t */

/**
 * 回调类型定义。请注意，进度百分比比率很容易通过<code>sent</code>除以<code>total</code>
 * @param sent   目前发送的字节数
 * @param total  要发送的字节总数
 * @param data   用户定义的可解引用的指针
 * @return  如果返回的不是0，则当前传输将被打断或取消
 */
typedef int (* LIBMTP_progressfunc_t) (u64 const sent, u64 const total,
                        void const * const data);


/**
 * MTP设备入口的数据结构体
 */
struct LIBMTP_device_entry_struct {
  char *vendor;             /**< 设备供应商 */
  u16 vendor_id;            /**< 设备供应商ID */
  char *product;            /**< 设备产品名 */
  u16 product_id;           /**< 设备产品ID */
  u32 device_flags;         /**< Bugs, device specifics etc */
};

/**
 * 连接到总线的原始MTP设备的数据结构体
 */
struct LIBMTP_raw_device_struct {
  LIBMTP_device_entry_t device_entry;   /**< 原始设备的设备入口 */
  u32 bus_location;                     /**< 总线位置，如果设备有效 */
  u8 devnum;                            /**< 总线上的设备数，如果设备有效 */
};

/**
 * 存储错误信息的数据结构体
 */
struct LIBMTP_error_struct {
    LIBMTP_error_number_t errornumber;    /* 错误号*/
    char *error_text;                     /* 错误信息文本*/
    LIBMTP_error_t *next;                 /* 指向下一个错误信息结构体的指针*/
};

/**
 * MTP 设备扩展结构体
 */
struct LIBMTP_device_extension_struct {
  /**
   * 扩展的名字，例如 "foo.com"
   */
  char *name;
  /**
   * 扩展部分的主修订号
   */
  int major;
  /**
   * 扩展部分的副修改号
   */
  int minor;
  /**
   * 下一个扩展的指针，如果这是最后一个则为NULL
   */
  LIBMTP_device_extension_t *next;
};


/**
 * 主要MTP设备对象结构体
 */
struct LIBMTP_mtpdevice_struct {
  /**
   * 对象位大小，一般是32或64
   */
  u8 object_bitsize;
  /**
   * 这个设备的参数，在内部使用前必须转换成\c (PTPParams*)
   */
  void *params;
  /**
   * 这个设备的参数，在内部使用前必须转换成\c (PTP_USB*)
   */
  void *usbinfo;
  /**
   * 这个设备的存储信息
   */
  LIBMTP_devicestorage_t *storage;
  /**
   * 错误栈。这个应该使用错误获取和清除函数处理，并不是通过取消引用此链表
   */
  LIBMTP_error_t *errorstack;
  /** 这个设备的最大电池电量*/
  u8 maximum_battery_level;
  /** 默认音乐文件夹 */
  u32 default_music_folder;
  /** 默认播放列表文件夹 */
  u32 default_playlist_folder;
  /** 默认图片文件夹 */
  u32 default_picture_folder;
  /** 默认视频文件夹 */
  u32 default_video_folder;
  /** 默认组织者文件夹 */
  u32 default_organizer_folder;
  /** Default ZENcast folder (only Creative devices...) */
  u32 default_zencast_folder;
  /** 默认专辑文件夹 */
  u32 default_album_folder;
  /** 默认文本文件夹 */
  u32 default_text_folder;
  /** Per device iconv() converters, only used internally */
  void *cd;
  /** 扩展列表 */
  LIBMTP_device_extension_t *extensions;
  /** 这个设备信息是否已经缓存*/
  int cached;
  /** 这个链表的下一个设备指针，如果这是最后一个设备，则为NULL */
  LIBMTP_mtpdevice_t *next;
};

/**
 * MTP 文件结构体
 */
struct LIBMTP_file_struct {
  u32 item_id;                  /**< 项ID*/
  u32 parent_id;                /**< 父目录ID */
  u32 storage_id;               /**< 当前存储设备的ID */
  char *filename;               /**< 文件名 */
  u64 filesize;                 /**< 文件大小(字节) */
  time_t modificationdate;      /**< 最后修改的日期 */
  LIBMTP_filetype_t filetype;   /**< 文件类型 */
  LIBMTP_file_t *next;          /**< 指向下一个文件，如果没有则为NULL */
};

/**
 * MTP 曲目结构体
 */
struct LIBMTP_track_struct {
  u32 item_id;                /**< 项 ID */
  u32 parent_id;              /**< 父目录ID */
  u32 storage_id;             /**< 存储设备ID */
  char *title;                /**< 曲目标题 */
  char *artist;               /**< 艺术家名字 */
  char *composer;             /**< 作曲家名字 */
  char *genre;                /**< 曲目的流派 */
  char *album;                /**< 曲目的专辑名*/
  char *date;                 /**< 原始日期(字符串记录) */
  char *filename;             /**< 曲目的原始文件名 */
  u16 tracknumber;            /**< 曲目号(按顺序记录) */
  u32 duration;               /**< 持续时间(毫秒) */
  u32 samplerate;             /**< Sample rate of original file, min 0x1f80 max 0xbb80 */
  u16 nochannels;             /**< Number of channels in this recording 0 = unknown, 1 or 2 */
  u32 wavecodec;              /**< FourCC wave codec name */
  u32 bitrate;                /**< (Average) bitrate for this file min=1 max=0x16e360 */
  u16 bitratetype;            /**< 0 = unused, 1 = constant, 2 = VBR, 3 = free */
  u16 rating;                 /**< User rating 0-100 (0x00-0x64) */
  u32 usecount;               /**< 使用/播放的次数 */
  u64 filesize;               /**< 曲目文件的大小(字节) */
  time_t modificationdate;    /**< 曲目的最新修改日期 */
  LIBMTP_filetype_t filetype; /**< 当前曲目文件类型 */
  LIBMTP_track_t *next;       /**< 指向下一个曲目结构体，没有则为NULL*/
};


/**
 * MTP 播放列表结构体
 */
struct LIBMTP_playlist_struct {
    u32 playlist_id;          /**< 播放列表 ID */
    u32 parent_id;            /**< 父目录ID */
    u32 storage_id;           /**< 存储设备ID */
    char *name;               /**< 播放列表名字 */
    u32 *tracks;              /**< 播放列表的曲目 */
    u32 no_tracks;            /**< 播放列表的曲目的数量 */
    LIBMTP_playlist_t *next;  /**< 下一个播放列表指针，没有则为NULL */
};

/**
 * MTP 专辑结构体
 */
struct LIBMTP_album_struct {
    u32 album_id;         /**< 播放列表 ID */
    u32 parent_id;        /**< 父结构体ID */
    u32 storage_id;       /**< 相关的存储设备ID */
    char *name;           /**< 专辑名字 */
    char *artist;         /**< 专辑作家名字 */
    char *composer;       /**< 录音作曲家名字 */
    char *genre;          /**< 专辑类型 */
    u32 *tracks;          /**< 专辑的曲目 */
    u32 no_tracks;        /**< 专辑曲目的数量 */
    LIBMTP_album_t *next; /**< 下一个专辑结构体，没有则为NULL */
};

/**
 * MTP 文件夹结构体
 */
struct LIBMTP_folder_struct {
    u32 folder_id;             /**< 文件夹 ID */
    u32 parent_id;             /**< 父文件夹ID */
    u32 storage_id;            /**< 这个文件夹的存储设备的ID */
    char *name;                /**< 文件夹的名字 */
    LIBMTP_folder_t *sibling;  /**< 同一目录等级的下一个文件夹，没有则为NULL*/
    LIBMTP_folder_t *child;    /**< 子文件夹，如果没有则为空 */
};

/**
 * LIBMTP对象代表性样本数据结构体
 */
struct LIBMTP_filesampledata_struct {
    u32 width;                    /**< 样本宽(如果是图片) */
    u32 height;                   /**< 样本高(如果是图片) */
    u32 duration;                 /**< 持续时间(毫秒，如果是音频) */
    LIBMTP_filetype_t filetype;   /**< 样本的文件类型 */
    u64 size;                     /**< 样本数据大小(字节) */
    char *data;                   /**< 样本数据 */
};


/**
 * LIBMTP 设备存储结构体
 */
struct LIBMTP_devicestorage_struct {
  u32 id;                         /**< 这个存储设备的独特的ID */
  u16 StorageType;                /**< 存储类型 */
  u16 FilesystemType;             /**< 文件系统类型 */
  u16 AccessCapability;           /**< 可访问容量 */
  u64 MaxCapacity;                /**< 最大容量*/
  u64 FreeSpaceInBytes;           /**< 空闲空间(字节) */
  u64 FreeSpaceInObjects;         /**< 空闲空闲(对象) */
  char *StorageDescription;       /**< 这个存储设备的一个描述简介 */
  char *VolumeIdentifier;         /**< 卷标识 */
  LIBMTP_devicestorage_t *next;   /**< 下一个存数设备, 跟着这条链表直到NULL*/
  LIBMTP_devicestorage_t *prev;   /**< 前一个存储设备 */
};

/* Make functions available for C++ */
#ifdef __cplusplus
extern "C" {
#endif

extern int LIBMTP_debug;

/**
 * @defgroup libmtp内部 API.
 * @{
 */
void LIBMTP_Set_Debug(int);
void LIBMTP_Init(void);
int LIBMTP_Get_Supported_Devices_List(LIBMTP_device_entry_t * * const, int * const);
/**
 * @}
 * @defgroup 基本设备管理 API.
 * @{
 */
LIBMTP_error_number_t LIBMTP_Detect_Raw_Devices(LIBMTP_raw_device_t **, int *);
LIBMTP_mtpdevice_t *LIBMTP_Open_Raw_Device(LIBMTP_raw_device_t *);
LIBMTP_mtpdevice_t *LIBMTP_Open_Raw_Device_Uncached(LIBMTP_raw_device_t *);
/* Begin old, legacy interface */
LIBMTP_mtpdevice_t *LIBMTP_Get_First_Device(void);
u32 LIBMTP_Number_Devices_In_List(LIBMTP_mtpdevice_t *);
/* End old, legacy interface */
void LIBMTP_Release_Device(LIBMTP_mtpdevice_t *);
void LIBMTP_Dump_Device_Info(LIBMTP_mtpdevice_t*);
int LIBMTP_Reset_Device(LIBMTP_mtpdevice_t*);
char *LIBMTP_Get_Friendlyname(LIBMTP_mtpdevice_t*);
char *LIBMTP_Get_Syncpartner(LIBMTP_mtpdevice_t*);
int LIBMTP_Get_Batterylevel(LIBMTP_mtpdevice_t *,
                u8 * const,
                u8 * const);
int LIBMTP_Get_Secure_Time(LIBMTP_mtpdevice_t *, char ** const);
int LIBMTP_Get_Device_Certificate(LIBMTP_mtpdevice_t *, char ** const);
int LIBMTP_Get_Supported_Filetypes(LIBMTP_mtpdevice_t *, u16 ** const, u16 * const);
void LIBMTP_Clear_Errorstack(LIBMTP_mtpdevice_t *);
void LIBMTP_Dump_Errorstack(LIBMTP_mtpdevice_t *);

/* 存储信息排列方式*/
#define LIBMTP_STORAGE_SORTBY_NOTSORTED 0    /* 不排列*/
#define LIBMTP_STORAGE_SORTBY_FREESPACE 1    /* 按空闲空间大小排列(由大到小)*/
#define LIBMTP_STORAGE_SORTBY_MAXSPACE  2    /* 按最大空间大小排列(由大到小)*/

int LIBMTP_Get_Storage(LIBMTP_mtpdevice_t *device, int const sortby);
int LIBMTP_Format_Storage(LIBMTP_mtpdevice_t *, LIBMTP_devicestorage_t *);
/**
 * 获取/设置任意属性。这些不会更新缓存，只应用于不存储在结构体中的属性
 */
char const * LIBMTP_Get_Property_Description(LIBMTP_property_t inproperty);
/**
 * @}
 * @defgroup 文件管理 API.
 * @{
 */
LIBMTP_file_t *LIBMTP_new_file_t(void);
void LIBMTP_destroy_file_t(LIBMTP_file_t*);
char const * LIBMTP_Get_Filetype_Description(LIBMTP_filetype_t);
LIBMTP_file_t *LIBMTP_Get_Filelisting(LIBMTP_mtpdevice_t *);
LIBMTP_file_t *LIBMTP_Get_Filelisting_With_Callback(LIBMTP_mtpdevice_t *, LIBMTP_progressfunc_t const, void const * const);

#define LIBMTP_FILES_AND_FOLDERS_ROOT 0xffffffff

LIBMTP_file_t * LIBMTP_Get_Files_And_Folders(LIBMTP_mtpdevice_t *, u32 const, u32 const);
LIBMTP_file_t *LIBMTP_Get_Filemetadata(LIBMTP_mtpdevice_t *, u32 const);
int LIBMTP_Get_File_To_File(LIBMTP_mtpdevice_t*, u32, char const * const,
                            LIBMTP_progressfunc_t const, void const * const);
int LIBMTP_Get_File_To_File_Descriptor(LIBMTP_mtpdevice_t*,
                                       u32 const,
                                       int const,
                                       LIBMTP_progressfunc_t const,
                                       void const * const);
int LIBMTP_Send_File_From_File(LIBMTP_mtpdevice_t *,
                               char const * const,
                               LIBMTP_file_t * const,
                               LIBMTP_progressfunc_t const,
                               void const * const);
int LIBMTP_Send_File_From_File_Descriptor(LIBMTP_mtpdevice_t *,
                                          int const,
                                          LIBMTP_file_t * const,
                                          LIBMTP_progressfunc_t const,
                                          void const * const);
int LIBMTP_Set_File_Name(LIBMTP_mtpdevice_t *,
                         LIBMTP_file_t *,
                         const char *);
LIBMTP_filesampledata_t *LIBMTP_new_filesampledata_t(void);
void LIBMTP_destroy_filesampledata_t(LIBMTP_filesampledata_t *);
int LIBMTP_Send_Representative_Sample(LIBMTP_mtpdevice_t *, u32 const,
                                      LIBMTP_filesampledata_t *);
int LIBMTP_Get_Representative_Sample(LIBMTP_mtpdevice_t *, u32 const,
                                     LIBMTP_filesampledata_t *);
/**
 * @}
 * @defgroup tracks 曲目管理API.
 * @{
 */
LIBMTP_track_t *LIBMTP_new_track_t(void);
void LIBMTP_destroy_track_t(LIBMTP_track_t *track);
LIBMTP_track_t *LIBMTP_Get_Trackmetadata(LIBMTP_mtpdevice_t*, u32 const);
int LIBMTP_Get_Track_To_File_Descriptor(LIBMTP_mtpdevice_t*, u32 const, int const,
                                        LIBMTP_progressfunc_t const, void const * const);
/**
 * @}
 * @defgroup 目录管理 API.
 * @{
 */
LIBMTP_folder_t *LIBMTP_new_folder_t(void);
void LIBMTP_destroy_folder_t(LIBMTP_folder_t *);
LIBMTP_folder_t *LIBMTP_Get_Folder_List(LIBMTP_mtpdevice_t *);
LIBMTP_folder_t *LIBMTP_Get_Folder_List_For_Storage(LIBMTP_mtpdevice_t *, u32 const);
u32 LIBMTP_Create_Folder(LIBMTP_mtpdevice_t*, char *, u32, u32);
int LIBMTP_Set_Folder_Name(LIBMTP_mtpdevice_t *, LIBMTP_folder_t *, const char *);
/**
 * @}
 * @defgroup 音频/视频播放列表管理 API.
 * @{
 */
LIBMTP_playlist_t *LIBMTP_new_playlist_t(void);
void LIBMTP_destroy_playlist_t(LIBMTP_playlist_t *);
LIBMTP_playlist_t *LIBMTP_Get_Playlist(LIBMTP_mtpdevice_t *, u32 const);
/**
 * @}
 * @defgroup 音频/视频专辑管理API.
 * @{
 */
LIBMTP_album_t *LIBMTP_new_album_t(void);
void LIBMTP_destroy_album_t(LIBMTP_album_t *album);
LIBMTP_album_t *LIBMTP_Get_Album(LIBMTP_mtpdevice_t *, u32 const);
int LIBMTP_Create_New_Album(LIBMTP_mtpdevice_t *, LIBMTP_album_t * const);
/**
 * @}
 * @defgroup 对象管理API.
 * @{
 */
int LIBMTP_Delete_Object(LIBMTP_mtpdevice_t *, u32);
int LIBMTP_Move_Object(LIBMTP_mtpdevice_t *, u32, u32, u32);
int LIBMTP_Copy_Object(LIBMTP_mtpdevice_t *, u32, u32, u32);
int LIBMTP_Set_Object_Filename(LIBMTP_mtpdevice_t *, u32 , char *);
/* End of C++ exports */
#ifdef __cplusplus
}
#endif
#endif
