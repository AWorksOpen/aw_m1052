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


/* ���ﶨ��ĵ��Ա�־��MTP��ӿ��õ��ⲿ��־
 * �뱣������б���libmtp.cͬ��*/
#define LIBMTP_DEBUG_NONE       0x00
#define LIBMTP_DEBUG_PTP        0x01
#define LIBMTP_DEBUG_PLST       0x02
#define LIBMTP_DEBUG_USB        0x04
#define LIBMTP_DEBUG_DATA       0x08
#define LIBMTP_DEBUG_ALL        0xFF


/**
 * ���ﶨ����ļ�������MTP��ӿ�ʹ�õ��ⲿ���͡��ڲ�����PTP(ͼƬ����Э��)�����ö������������ͬ
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
 * ��Ƶ�ļ�����
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
 * ��Ƶ�ļ�����
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
 * ��Ŀ�ļ�����
 */
#define LIBMTP_FILETYPE_IS_TRACK(a)\
(LIBMTP_FILETYPE_IS_AUDIO(a) ||\
 LIBMTP_FILETYPE_IS_VIDEO(a) ||\
 LIBMTP_FILETYPE_IS_AUDIOVIDEO(a))

/**
 * ���ﶨ���������MTP��ӿ�ʹ�õ��ڲ����͡�
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
 * ��Щ�Ǵ�����롣��Ҳ���Ի�ȡ���������ַ���
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
 * �ص����Ͷ��塣��ע�⣬���Ȱٷֱȱ��ʺ�����ͨ��<code>sent</code>����<code>total</code>
 * @param sent   Ŀǰ���͵��ֽ���
 * @param total  Ҫ���͵��ֽ�����
 * @param data   �û�����Ŀɽ����õ�ָ��
 * @return  ������صĲ���0����ǰ���佫����ϻ�ȡ��
 */
typedef int (* LIBMTP_progressfunc_t) (u64 const sent, u64 const total,
                        void const * const data);


/**
 * MTP�豸��ڵ����ݽṹ��
 */
struct LIBMTP_device_entry_struct {
  char *vendor;             /**< �豸��Ӧ�� */
  u16 vendor_id;            /**< �豸��Ӧ��ID */
  char *product;            /**< �豸��Ʒ�� */
  u16 product_id;           /**< �豸��ƷID */
  u32 device_flags;         /**< Bugs, device specifics etc */
};

/**
 * ���ӵ����ߵ�ԭʼMTP�豸�����ݽṹ��
 */
struct LIBMTP_raw_device_struct {
  LIBMTP_device_entry_t device_entry;   /**< ԭʼ�豸���豸��� */
  u32 bus_location;                     /**< ����λ�ã�����豸��Ч */
  u8 devnum;                            /**< �����ϵ��豸��������豸��Ч */
};

/**
 * �洢������Ϣ�����ݽṹ��
 */
struct LIBMTP_error_struct {
    LIBMTP_error_number_t errornumber;    /* �����*/
    char *error_text;                     /* ������Ϣ�ı�*/
    LIBMTP_error_t *next;                 /* ָ����һ��������Ϣ�ṹ���ָ��*/
};

/**
 * MTP �豸��չ�ṹ��
 */
struct LIBMTP_device_extension_struct {
  /**
   * ��չ�����֣����� "foo.com"
   */
  char *name;
  /**
   * ��չ���ֵ����޶���
   */
  int major;
  /**
   * ��չ���ֵĸ��޸ĺ�
   */
  int minor;
  /**
   * ��һ����չ��ָ�룬����������һ����ΪNULL
   */
  LIBMTP_device_extension_t *next;
};


/**
 * ��ҪMTP�豸����ṹ��
 */
struct LIBMTP_mtpdevice_struct {
  /**
   * ����λ��С��һ����32��64
   */
  u8 object_bitsize;
  /**
   * ����豸�Ĳ��������ڲ�ʹ��ǰ����ת����\c (PTPParams*)
   */
  void *params;
  /**
   * ����豸�Ĳ��������ڲ�ʹ��ǰ����ת����\c (PTP_USB*)
   */
  void *usbinfo;
  /**
   * ����豸�Ĵ洢��Ϣ
   */
  LIBMTP_devicestorage_t *storage;
  /**
   * ����ջ�����Ӧ��ʹ�ô����ȡ�������������������ͨ��ȡ�����ô�����
   */
  LIBMTP_error_t *errorstack;
  /** ����豸������ص���*/
  u8 maximum_battery_level;
  /** Ĭ�������ļ��� */
  u32 default_music_folder;
  /** Ĭ�ϲ����б��ļ��� */
  u32 default_playlist_folder;
  /** Ĭ��ͼƬ�ļ��� */
  u32 default_picture_folder;
  /** Ĭ����Ƶ�ļ��� */
  u32 default_video_folder;
  /** Ĭ����֯���ļ��� */
  u32 default_organizer_folder;
  /** Default ZENcast folder (only Creative devices...) */
  u32 default_zencast_folder;
  /** Ĭ��ר���ļ��� */
  u32 default_album_folder;
  /** Ĭ���ı��ļ��� */
  u32 default_text_folder;
  /** Per device iconv() converters, only used internally */
  void *cd;
  /** ��չ�б� */
  LIBMTP_device_extension_t *extensions;
  /** ����豸��Ϣ�Ƿ��Ѿ�����*/
  int cached;
  /** ����������һ���豸ָ�룬����������һ���豸����ΪNULL */
  LIBMTP_mtpdevice_t *next;
};

/**
 * MTP �ļ��ṹ��
 */
struct LIBMTP_file_struct {
  u32 item_id;                  /**< ��ID*/
  u32 parent_id;                /**< ��Ŀ¼ID */
  u32 storage_id;               /**< ��ǰ�洢�豸��ID */
  char *filename;               /**< �ļ��� */
  u64 filesize;                 /**< �ļ���С(�ֽ�) */
  time_t modificationdate;      /**< ����޸ĵ����� */
  LIBMTP_filetype_t filetype;   /**< �ļ����� */
  LIBMTP_file_t *next;          /**< ָ����һ���ļ������û����ΪNULL */
};

/**
 * MTP ��Ŀ�ṹ��
 */
struct LIBMTP_track_struct {
  u32 item_id;                /**< �� ID */
  u32 parent_id;              /**< ��Ŀ¼ID */
  u32 storage_id;             /**< �洢�豸ID */
  char *title;                /**< ��Ŀ���� */
  char *artist;               /**< ���������� */
  char *composer;             /**< ���������� */
  char *genre;                /**< ��Ŀ������ */
  char *album;                /**< ��Ŀ��ר����*/
  char *date;                 /**< ԭʼ����(�ַ�����¼) */
  char *filename;             /**< ��Ŀ��ԭʼ�ļ��� */
  u16 tracknumber;            /**< ��Ŀ��(��˳���¼) */
  u32 duration;               /**< ����ʱ��(����) */
  u32 samplerate;             /**< Sample rate of original file, min 0x1f80 max 0xbb80 */
  u16 nochannels;             /**< Number of channels in this recording 0 = unknown, 1 or 2 */
  u32 wavecodec;              /**< FourCC wave codec name */
  u32 bitrate;                /**< (Average) bitrate for this file min=1 max=0x16e360 */
  u16 bitratetype;            /**< 0 = unused, 1 = constant, 2 = VBR, 3 = free */
  u16 rating;                 /**< User rating 0-100 (0x00-0x64) */
  u32 usecount;               /**< ʹ��/���ŵĴ��� */
  u64 filesize;               /**< ��Ŀ�ļ��Ĵ�С(�ֽ�) */
  time_t modificationdate;    /**< ��Ŀ�������޸����� */
  LIBMTP_filetype_t filetype; /**< ��ǰ��Ŀ�ļ����� */
  LIBMTP_track_t *next;       /**< ָ����һ����Ŀ�ṹ�壬û����ΪNULL*/
};


/**
 * MTP �����б�ṹ��
 */
struct LIBMTP_playlist_struct {
    u32 playlist_id;          /**< �����б� ID */
    u32 parent_id;            /**< ��Ŀ¼ID */
    u32 storage_id;           /**< �洢�豸ID */
    char *name;               /**< �����б����� */
    u32 *tracks;              /**< �����б����Ŀ */
    u32 no_tracks;            /**< �����б����Ŀ������ */
    LIBMTP_playlist_t *next;  /**< ��һ�������б�ָ�룬û����ΪNULL */
};

/**
 * MTP ר���ṹ��
 */
struct LIBMTP_album_struct {
    u32 album_id;         /**< �����б� ID */
    u32 parent_id;        /**< ���ṹ��ID */
    u32 storage_id;       /**< ��صĴ洢�豸ID */
    char *name;           /**< ר������ */
    char *artist;         /**< ר���������� */
    char *composer;       /**< ¼������������ */
    char *genre;          /**< ר������ */
    u32 *tracks;          /**< ר������Ŀ */
    u32 no_tracks;        /**< ר����Ŀ������ */
    LIBMTP_album_t *next; /**< ��һ��ר���ṹ�壬û����ΪNULL */
};

/**
 * MTP �ļ��нṹ��
 */
struct LIBMTP_folder_struct {
    u32 folder_id;             /**< �ļ��� ID */
    u32 parent_id;             /**< ���ļ���ID */
    u32 storage_id;            /**< ����ļ��еĴ洢�豸��ID */
    char *name;                /**< �ļ��е����� */
    LIBMTP_folder_t *sibling;  /**< ͬһĿ¼�ȼ�����һ���ļ��У�û����ΪNULL*/
    LIBMTP_folder_t *child;    /**< ���ļ��У����û����Ϊ�� */
};

/**
 * LIBMTP����������������ݽṹ��
 */
struct LIBMTP_filesampledata_struct {
    u32 width;                    /**< ������(�����ͼƬ) */
    u32 height;                   /**< ������(�����ͼƬ) */
    u32 duration;                 /**< ����ʱ��(���룬�������Ƶ) */
    LIBMTP_filetype_t filetype;   /**< �������ļ����� */
    u64 size;                     /**< �������ݴ�С(�ֽ�) */
    char *data;                   /**< �������� */
};


/**
 * LIBMTP �豸�洢�ṹ��
 */
struct LIBMTP_devicestorage_struct {
  u32 id;                         /**< ����洢�豸�Ķ��ص�ID */
  u16 StorageType;                /**< �洢���� */
  u16 FilesystemType;             /**< �ļ�ϵͳ���� */
  u16 AccessCapability;           /**< �ɷ������� */
  u64 MaxCapacity;                /**< �������*/
  u64 FreeSpaceInBytes;           /**< ���пռ�(�ֽ�) */
  u64 FreeSpaceInObjects;         /**< ���п���(����) */
  char *StorageDescription;       /**< ����洢�豸��һ��������� */
  char *VolumeIdentifier;         /**< ���ʶ */
  LIBMTP_devicestorage_t *next;   /**< ��һ�������豸, ������������ֱ��NULL*/
  LIBMTP_devicestorage_t *prev;   /**< ǰһ���洢�豸 */
};

/* Make functions available for C++ */
#ifdef __cplusplus
extern "C" {
#endif

extern int LIBMTP_debug;

/**
 * @defgroup libmtp�ڲ� API.
 * @{
 */
void LIBMTP_Set_Debug(int);
void LIBMTP_Init(void);
int LIBMTP_Get_Supported_Devices_List(LIBMTP_device_entry_t * * const, int * const);
/**
 * @}
 * @defgroup �����豸���� API.
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

/* �洢��Ϣ���з�ʽ*/
#define LIBMTP_STORAGE_SORTBY_NOTSORTED 0    /* ������*/
#define LIBMTP_STORAGE_SORTBY_FREESPACE 1    /* �����пռ��С����(�ɴ�С)*/
#define LIBMTP_STORAGE_SORTBY_MAXSPACE  2    /* �����ռ��С����(�ɴ�С)*/

int LIBMTP_Get_Storage(LIBMTP_mtpdevice_t *device, int const sortby);
int LIBMTP_Format_Storage(LIBMTP_mtpdevice_t *, LIBMTP_devicestorage_t *);
/**
 * ��ȡ/�����������ԡ���Щ������»��棬ֻӦ���ڲ��洢�ڽṹ���е�����
 */
char const * LIBMTP_Get_Property_Description(LIBMTP_property_t inproperty);
/**
 * @}
 * @defgroup �ļ����� API.
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
 * @defgroup tracks ��Ŀ����API.
 * @{
 */
LIBMTP_track_t *LIBMTP_new_track_t(void);
void LIBMTP_destroy_track_t(LIBMTP_track_t *track);
LIBMTP_track_t *LIBMTP_Get_Trackmetadata(LIBMTP_mtpdevice_t*, u32 const);
int LIBMTP_Get_Track_To_File_Descriptor(LIBMTP_mtpdevice_t*, u32 const, int const,
                                        LIBMTP_progressfunc_t const, void const * const);
/**
 * @}
 * @defgroup Ŀ¼���� API.
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
 * @defgroup ��Ƶ/��Ƶ�����б���� API.
 * @{
 */
LIBMTP_playlist_t *LIBMTP_new_playlist_t(void);
void LIBMTP_destroy_playlist_t(LIBMTP_playlist_t *);
LIBMTP_playlist_t *LIBMTP_Get_Playlist(LIBMTP_mtpdevice_t *, u32 const);
/**
 * @}
 * @defgroup ��Ƶ/��Ƶר������API.
 * @{
 */
LIBMTP_album_t *LIBMTP_new_album_t(void);
void LIBMTP_destroy_album_t(LIBMTP_album_t *album);
LIBMTP_album_t *LIBMTP_Get_Album(LIBMTP_mtpdevice_t *, u32 const);
int LIBMTP_Create_New_Album(LIBMTP_mtpdevice_t *, LIBMTP_album_t * const);
/**
 * @}
 * @defgroup �������API.
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
