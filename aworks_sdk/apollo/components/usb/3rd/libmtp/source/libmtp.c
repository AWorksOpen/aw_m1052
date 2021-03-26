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


/* ȫ�ֵ��Եȼ�
 * ����ʹ��һ����־ϵͳ������������־
 *
 * LIBMTP_DEBUG��������Ϊʹ��MTP�Ⲣ����LIBMTP_Init���õ��Ա�־����ֵ������ʮ����
 * ��ʾ(�����ԡ�0����ͷ�������԰˽��Ʊ�ʾ)����16����(�����ԡ�0x����ʼ)��
 *
 * ֵ��-1��ʹ�����еĵ��Ա�־
 *
 * �����е�һЩʵ�ó�����������־��-d����ʹ��LIBMTP_DEBUG_PTP��LIBMTP_DEBUG_DATA����
 * ����LIBMTP_DEBUG=9һ��
 *
 * ��־(ͨ�����ʮ�����ƽ��кϲ�)��
 *  0x00 [0000 0000] : LIBMTP_DEBUG_NONE  : �޵��� (Ĭ��)
 *  0x01 [0000 0001] : LIBMTP_DEBUG_PTP   : PTP ����
 *  0x02 [0000 0010] : LIBMTP_DEBUG_PLST  : Playlist ����
 *  0x04 [0000 0100] : LIBMTP_DEBUG_USB   : USB ����
 *  0x08 [0000 1000] : LIBMTP_DEBUG_DATA  : USB data ����
 *
 * �뱣������б���libmtp.hͬ��
 */
int LIBMTP_debug = LIBMTP_DEBUG_NONE;

/* ����MTP���ڲ�MTP�ļ����ͺ�libgphoto2/PTP�ȼ۶���֮���ӳ�䡣������Ҫ�������
 * libmtp.h�豸Ҳ��������ptp.h���ܰ�װ�����ǲ���Ҫ����*/
typedef struct filemap_struct {
    char *description;        /**< �ļ����͵��ı����� */
    LIBMTP_filetype_t id;     /**< MTP���ڲ��ļ����� */
    u16 ptp_id;               /**< PTP�ļ�����ID */
    struct filemap_struct *next;
} filemap_t;

/*
 * ����MTP���ڲ�MTP�������ͺ�libgphoto2/PTP�ȼ۶���֮���ӳ�䡣������Ҫ�������
 * libmtp.h�豸Ҳ��������ptp.h���ܰ�װ�����ǲ���Ҫ����*/
typedef struct propertymap_struct {
    char *description;        /**< �����Ե��ı����� */
    LIBMTP_property_t id;     /**< MTP���ڲ��������� */
    u16 ptp_id;               /**< PTP����ID */
    struct propertymap_struct *next;
} propertymap_t;

/* ȫ�ֱ���*/
/* �洢ȫ���ļ�����ӳ���*/
static filemap_t *g_filemap = NULL;
/* �洢ȫ������ӳ��� */
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
 * ����ļ�����׺���ǲ��ǡ�.ogg��
 *
 * @param name �ļ����ַ���
 *
 * @return ����0��ʾ��׺�����ǡ�.ogg��������1��ʾ��
 **/
static int has_ogg_extension(char *name) {
    char *ptype;

    if (name == NULL)
        return 0;
    /* ��ȡ�ļ���׺��*/
    ptype = strrchr(name,'.');
    if (ptype == NULL)
        return 0;
    /* ����׺���ǲ��ǡ�.ogg��*/
    if (!strcasecmp (ptype, ".ogg"))
        return 1;
    return 0;
}

/**
 * ����ļ�����׺���ǲ��ǡ�.flac��
 *
 * @param name �ļ����ַ���
 *
 * @return ����0��ʾ��׺�����ǡ�.flac��������1��ʾ��
 **/
static int has_flac_extension(char *name) {
    char *ptype;

    if (name == NULL)
        return 0;
    /* ��ȡ�ļ���׺��*/
    ptype = strrchr(name,'.');
    if (ptype == NULL)
        return 0;
    /* ����׺���ǲ��ǡ�.flac��*/
    if (!strcasecmp (ptype, ".flac"))
        return 1;
    return 0;
}


/**
 * ����һ���µ��ļ�ӳ�����
 * @return ����һ���µķ���õ��ļ�ӳ�����
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
 * ע��һ�� MTP(ý�崫��Э��)��PTP(ͼƬ����Э��)�ļ������������ݼ���
 *
 * @param description �ļ����͵��ı�����
 * @param id          MTP���ڲ��ļ�����ID
 * @param ptp_id      PTP�ļ�����ID
 * @return  �ɹ�����0����������ֵ�����
*/
static int register_filetype(char const * const description, LIBMTP_filetype_t const id,
                 u16 const ptp_id){
    filemap_t *new = NULL, *current;

    /* ֮ǰ��MTP���ļ����ͱ�ע�᣿*/
    current = g_filemap;
    while (current != NULL) {
        if(current->id == id) {
            break;
        }
        current = current->next;
    }

    /* ����һ�����*/
    if(current == NULL) {
        new = new_filemap_entry();
        if(new == NULL) {
            return 1;
        }

        new->id = id;
        if(description != NULL) {
            /* �����ַ���*/
            new->description = mtp_strdup(description);
        }
        new->ptp_id = ptp_id;
        /* �����ڵ�������*/
        if(g_filemap == NULL) {
            g_filemap = new;
        } else {
            current = g_filemap;
            while (current->next != NULL ) current=current->next;
            current->next = new;
        }
        /* ���´��ڵ����*/
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

/* ��ʼ���ļ�ӳ��*/
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
 * ����ӳ�䵽MTP�ӿ����͵�PTP�ڲ��ӿ�����
 *
 * @param intype MTP��ӿ�����
 *
 * @return �ɹ�����PTP(libgphoto2)�ӿ�����
 **/
static u16 map_libmtp_type_to_ptp_type(LIBMTP_filetype_t intype)
{
    filemap_t *current;
    /* ��ȡ�ļ�ӳ���*/
    current = g_filemap;
    /* Ѱ�Ҷ�Ӧ�ĸ�ʽ*/
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
 * ����ӳ�䵽PTP�ӿ����͵�MTP�ڲ��ӿ�����
 *
 * @param intype PTP (libgphoto2)�ӿ�����
 *
 * @return �ɹ�����MTP��ӿ�����
 **/
static LIBMTP_filetype_t map_ptp_type_to_libmtp_type(u16 intype)
{
    filemap_t *current;
    /* ��ȡ�ļ�ӳ���*/
    current = g_filemap;
    /* Ѱ�Ҷ�Ӧ�ĸ�ʽ*/
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
 * ����һ���µ�����ӳ�����
 * @return ����һ���µķ���õ�����ӳ�����
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
 * ע��һ�� MTP(ý�崫��Э��)��PTP(ͼƬ����Э��)�����������ݼ���
 *
 * @param description ���Ե��ı�����
 * @param id          MTP���ڲ�����ID
 * @param ptp_id      PTP����ID
 * @return  �ɹ�����0����������ֵ�����
*/
static int register_property(char const * const description, LIBMTP_property_t const id,
                 u16 const ptp_id)
{
    propertymap_t *new = NULL, *current;
    /* ֮ǰ��MTP�����Ա�ע�᣿*/
    current = g_propertymap;
    while (current != NULL) {
        if(current->id == id) {
            break;
        }
        current = current->next;
    }
    /* ����һ�����*/
    if(current == NULL) {
        new = new_propertymap_entry();
        if(new == NULL) {
            return 1;
        }

        new->id = id;
        if(description != NULL) {
            /* �����ַ���*/
            new->description = mtp_strdup(description);
        }
        new->ptp_id = ptp_id;

        /* �����ڵ�������*/
        if(g_propertymap == NULL) {
            g_propertymap = new;
        } else {
            current = g_propertymap;
            while (current->next != NULL ) current=current->next;
            current->next = new;
        }
        /* ���´��ڵ����*/
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


/* ��ʼ������ӳ��*/
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
 * ����ӳ�䵽PTP�ӿ����Ե�MTP�ڲ��ӿ�����
 *
 * @param inproperty PTP (libgphoto2)�ӿ�����
 *
 * @return �ɹ�����MTP��ӿ�����
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
 * ���õ��Եȼ�
 *
 * Ĭ�ϣ����Եȼ�����Ϊ��0��(����)��
 */
void LIBMTP_Set_Debug(int level)
{
  if (LIBMTP_debug || level)
    LIBMTP_ERROR("LIBMTP_Set_Debug: Setting debugging level to %d (0x%02x) "
                 "(%s)\n", level, level, level ? "on" : "off");

  LIBMTP_debug = level;
}

/**
 * ��ʼ��MTP��
 * */
void LIBMTP_Init(void){
    /* ���õ��Եȼ�������ȫ������*/
    LIBMTP_Set_Debug(-1);
    /* ��ʼ���ļ�ӳ��*/
    init_filemap();
    /* ��ʼ������ӳ��*/
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
 * ����libmtp�ļ����͵��ı�����
 *
 * @param intype libmtp�ڲ��ļ�����
 *
 * @return �ɹ������ļ����͵������ı�
 **/
char const * LIBMTP_Get_Filetype_Description(LIBMTP_filetype_t intype)
{
    filemap_t *current;

    current = g_filemap;

    /* ƥ���ļ�����*/
    while (current != NULL) {
        if(current->id == intype) {
            return current->description;
        }
        current = current->next;
    }

    return "Unknown filetype";
}

/**
 * ����libmtp���Ե��ı�����
 *
 * @param inproperty libmtp�ڲ�����
 *
 * @return �ɹ��������Ե������ı�
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
 * �����������һ���´���ISO 8601 �����ܸ߾��ȵĵ�ǰ��ʱ���
 * ������ԣ�������������ʱ��
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
 * ����һ��������ַ���
 *
 * @param device       MTP�豸
 * @param object_id    ����ID
 * @param attribute_id PTP����ID
 *
 * @return �ɹ�������Ч���ַ�����ʧ�ܷ���NULL
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
    /* Ѱ�Ҷ�Ӧ������*/
    prop = ptp_find_object_prop_in_cache(params, object_id, attribute_id);
    if (prop) {
        if (prop->propval.str != NULL)
            return mtp_strdup(prop->propval.str);
        else
            return NULL;
    }
    /* ��ȡ����ֵ*/
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
 * �Ӷ��������м���һ���޷���64λ����
 *
 * @param device        MTP�豸
 * @param object_id     ����ID
 * @param attribute_id  PTP����ID
 * @param value_default ʧ�ܷ���Ĭ��ֵ
 *
 * @return ����ֵ
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
    /* Ѱ�Ҷ�Ӧ������*/
    prop = ptp_find_object_prop_in_cache(params, object_id, attribute_id);
    if (prop)
        return prop->propval.u64;
    /* ��ȡ����ֵ*/
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
 * �Ӷ��������м���һ���޷���32λ����
 *
 * @param device        MTP�豸
 * @param object_id     ����ID
 * @param attribute_id  PTP����ID
 * @param value_default ʧ�ܷ���Ĭ��ֵ
 *
 * @return ����ֵ
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
    /* Ѱ�Ҷ�Ӧ������*/
    prop = ptp_find_object_prop_in_cache(params, object_id, attribute_id);
    if (prop)
        return prop->propval.u32;
    /* ��ȡ����ֵ*/
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
 * �Ӷ��������м���һ���޷���16λ����
 *
 * @param device        MTP�豸
 * @param object_id     ����ID
 * @param attribute_id  PTP����ID
 * @param value_default ʧ�ܷ���Ĭ��ֵ
 *
 * @return ����ֵ
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

    /* Ѱ�Ҷ�Ӧ������*/
    prop = ptp_find_object_prop_in_cache(params, object_id, attribute_id);
    if (prop)
        return prop->propval.u16;
    /* ��ȡ����ֵ*/
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
 * ���ö������Ե��ַ���
 *
 * @param device       MTP�豸
 * @param object_id    ����ID
 * @param attribute_id Ҫ���õ�����ID
 * @param string       Ҫ���õ��ַ���
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
 */
static int set_object_string(LIBMTP_mtpdevice_t *device, u32 const object_id,
                             u16 const attribute_id, char const * const string)
{
    PTPPropertyValue propval;
    PTPParams *params;
    u16 ret;
    /* �Ϸ��Լ��*/
    if (!device || !string)
        return -1;

    params = (PTPParams *) device->params;
    /* ����Ƿ�֧�����ö�������ֵ*/
    if (!ptp_operation_issupported(params,PTP_OC_MTP_SetObjectPropValue)) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "set_object_string(): could not set object string: "
                "PTP_OC_MTP_SetObjectPropValue not supported.");
        return -1;
    }
    propval.str = (char *) string;
    /* ���ö�������ֵ*/
    ret = ptp_mtp_setobjectpropvalue(params, object_id, attribute_id, &propval, PTP_DTC_STR);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "set_object_string(): could not set object string.");
        return -1;
    }

    return 0;
}

/**
 * ����һ��������޷���32λ��������
 *
 * @param device       ���MTP�豸
 * @param object_id    ����ID
 * @param attribute_id Ҫ���õ�����ID
 * @param value        Ҫ���õ�32λ�޷�������
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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
    /* ����Ƿ�֧�����ö�������ֵ*/
    if (!ptp_operation_issupported(params,PTP_OC_MTP_SetObjectPropValue)) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "set_object_u32(): could not set unsigned 32bit integer property: "
                    "PTP_OC_MTP_SetObjectPropValue not supported.");
        return -1;
    }
    /* ���ö�������ֵ*/
    propval.u32 = value;
    ret = ptp_mtp_setobjectpropvalue(params, object_id, attribute_id, &propval, PTP_DTC_UINT32);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "set_object_u32(): could not set unsigned 32bit integer property.");
        return -1;
    }

    return 0;
}

/**
 * ��ȡMTP�豸����ĵ�һ��MTP�豸
 *
 * @return �ɹ�����MTP�豸�ṹ��
 */
LIBMTP_mtpdevice_t *LIBMTP_Get_First_Device(void)
{
    LIBMTP_mtpdevice_t *first_device = NULL;
    LIBMTP_raw_device_t *devices;
    int numdevs;
    LIBMTP_error_number_t ret;

    /* ��⵱ǰԭʼ�豸������MTP�豸����*/
    ret = LIBMTP_Detect_Raw_Devices(&devices, &numdevs);
    if (ret != LIBMTP_ERROR_NONE) {
        return NULL;
    }
    /* �Ҳ����豸*/
    if (devices == NULL || numdevs == 0) {
        mtp_free(devices);
        return NULL;
    }
    /* ��ȡMTP�豸����ĵ�һ���豸*/
    first_device = LIBMTP_Open_Raw_Device(&devices[0]);
    /* �ͷ�����*/
    mtp_free(devices);
    return first_device;
}


/**
 * ��д���Ժ���
 * �������ǿ��Թص����Դ�ӡ
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
 * ��д������
 * �������ǿ��Խ����д���Ȳ������ǵĴ����ջ��
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
 * ������չ������
 *
 * @param mtpdevice MTP�豸�ṹ��
 * @param desc ��չ������
 */
static void parse_extension_descriptor(LIBMTP_mtpdevice_t *mtpdevice,
                                       char *desc){
    int start = 0;
    int end = 0;

    /* ��Canon A70Ϊ�� */
    if (!desc)
        return;

    /* �������÷ֺŷָ� */
    while (end < strlen(desc)) {
        /* ������ʼ�Ŀո� */
        while ((end < strlen(desc)) && (desc[start] == ' ' )) {
            start++;
            end++;
        }
        /* �����չ������ */
        while ((end < strlen(desc)) && (desc[end] != ';'))
            end++;
        if (end < strlen(desc)) {
            /* ��ȡ��չ����������*/
            char *element = mtp_strndup(desc + start, end - start);
            if (element) {
                int i = 0;
                // printf("  Element: \"%s\"\n", element);

                /* ����һ����չ������ */
                while ((i < strlen(element)) && (element[i] != ':'))
                    i++;
                if (i < strlen(element)) {
                    char *name = mtp_strndup(element, i);
                    int major = 0, minor = 0;

                    /* ��չ�汾������ MAJOR.MINOR, ������ֻ��һ��0������ҲҪ���� */
                    if ((2 == sscanf(element + i + 1,"%d.%d",&major,&minor)) ||
                                (1 == sscanf(element + i + 1,"%d",&major))) {
                        LIBMTP_device_extension_t *extension;

                        extension = mtp_malloc(sizeof(LIBMTP_device_extension_t));
                        extension->name = name;
                        extension->major = major;
                        extension->minor = minor;
                        extension->next = NULL;
                        /* ����MTP�豸��չ����������*/
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
 * ���������ԭʼ�豸�д�һ���豸���������½ӿ��з����豸����ѡ��ʽ�����½ӿ��У�������ĳ��
 * �豸�Ϲ���ʱ������豸���Խ���һЩ����
 *
 * @param rawdevice �򿪡���ʵ���豸��ԭʼ�豸
 * @return ����һ���򿪵��豸
 */
LIBMTP_mtpdevice_t *LIBMTP_Open_Raw_Device_Uncached(LIBMTP_raw_device_t *rawdevice)
{
    LIBMTP_mtpdevice_t *mtp_device;
    u8 bs = 0;
    PTPParams *current_params;
    PTP_USB *ptp_usb;
    LIBMTP_error_number_t err;
    int i;

    /* Ϊ�豸����һ����̬�Ŀռ�*/
    mtp_device = (LIBMTP_mtpdevice_t *) mtp_malloc(sizeof(LIBMTP_mtpdevice_t));
    /* ����Ƿ���һ���ڴ������� */
    if(mtp_device == NULL) {
        /* ������һ���ڴ�������. ���Ǵ����������豸Ȼ�����*/

        /* TODO: ����������Ӧ�ø���׳Щ */
        LIBMTP_ERROR("LIBMTP PANIC: connect_usb_devices encountered a memory "
            "allocation error with device %d on bus %d, trying to continue",
            rawdevice->devnum, rawdevice->bus_location);

        return NULL;
    }
    memset(mtp_device, 0, sizeof(LIBMTP_mtpdevice_t));
    /* Ĭ�ϲ�����*/
    mtp_device->cached = 0;
    /* ���� PTP params */
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
    /* ֮���⽫��һ��ָ��PTP_USB��ָ�� */
    current_params->data = NULL;
    /* ���� upp ���ص��Ժʹ����� */
    current_params->debug_func = LIBMTP_ptp_debug;
    current_params->error_func = LIBMTP_ptp_error;
    /* С���ֽ��� */
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
    /* ���� usbinfo, ͬʱ��һ���Ự */
    err = configure_usb_device(rawdevice, current_params,
                   &mtp_device->usbinfo);
    if (err != LIBMTP_ERROR_NONE) {
        mtp_free(current_params);
        mtp_free(mtp_device);
        return NULL;
    }
    ptp_usb = (PTP_USB*) mtp_device->usbinfo;
    /* ���ò���ָ�� */
    ptp_usb->params = current_params;
    /* �����豸��Ϣ��֮��ʹ��*/
    if (ptp_getdeviceinfo(current_params, &current_params->deviceinfo) != PTP_RC_OK) {
        LIBMTP_ERROR("LIBMTP PANIC: Unable to read device information on device "
                     "%d on bus %d, trying to continue",
                     rawdevice->devnum, rawdevice->bus_location);

        /* �ͷ��ڴ� */
        mtp_free(mtp_device->usbinfo);
        mtp_free(mtp_device->params);
        current_params = NULL;
        mtp_free(mtp_device);
        return NULL;
    }

    /* ����Ƿ���һ��PTP�豸���Ƿ񱻱��ΪMTP�豸*/
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
    /* �����豸��Ϣ�����չ������*/
    parse_extension_descriptor(mtp_device,
                               current_params->deviceinfo.VendorExtensionDesc);

    /* Android�кܶ�bug�����Android����ǿ�Ʒ�����Щ�����־����ͬ��SONY NWZ Walkmen���豸*/
    {
        LIBMTP_device_extension_t *tmpext = mtp_device->extensions;
        int is_microsoft_com_wpdna = 0;
        int is_android = 0;
        int is_sony_net_wmfu = 0;
        int is_sonyericsson_com_se = 0;

        /* ѭ����չ�����ñ�־ */
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

        /* ����ض���Э��ջ */
        if (is_microsoft_com_wpdna && is_sonyericsson_com_se && !is_android) {
          /*
           * ����ͨ���ṩproviding WPDNA��SonyEricsson extension��NO Android extension��
           * ���AricentЭ��ջ
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
    /* ���OGG����FLAC�ļ����ͱ����Ϊ"unknown"������Ƿ�̼���������ʵ��֧������*/
    if (FLAG_OGG_IS_UNKNOWN(ptp_usb)) {
        for (i = 0;i < current_params->deviceinfo.ImageFormats_len;i++) {
            if (current_params->deviceinfo.ImageFormats[i] == PTP_OFC_MTP_OGG) {
                /* ������δ֪��ȡ����� */
                ptp_usb->rawdevice.device_entry.device_flags &= ~DEVICE_FLAG_OGG_IS_UNKNOWN;
                break;
            }
        }
    }
    if (FLAG_FLAC_IS_UNKNOWN(ptp_usb)) {
        for (i = 0;i < current_params->deviceinfo.ImageFormats_len;i++) {
            if (current_params->deviceinfo.ImageFormats[i] == PTP_OFC_MTP_FLAC) {
                /* ������δ֪��ȡ����� */
                ptp_usb->rawdevice.device_entry.device_flags &= ~DEVICE_FLAG_FLAC_IS_UNKNOWN;
                break;
            }
        }
    }
    /* ȷ���Ƿ�����С֧��32��64λ�� */
    if (ptp_operation_issupported(current_params,PTP_OC_MTP_GetObjectPropsSupported)) {
        for (i = 0;i < current_params->deviceinfo.ImageFormats_len;i++) {
            PTPObjectPropDesc opd;
            /* ��ȡ��������������*/
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
                    /* ����������С*/
                    LIBMTP_ERROR("LIBMTP PANIC: "
                                "awkward object size data type: %04x\n", opd.DataType);
                    bs = 0;
                    break;
                }
            }
        }
    }
    if (bs == 0) {
        /* û�м�⵽�����λ��С��������32λ*/
        bs = 32;
    }

    mtp_device->object_bitsize = bs;

    /* ����豸��û�д���*/
    mtp_device->errorstack = NULL;

    /* Ĭ�����ĵ���������������ǻ������ */
    mtp_device->maximum_battery_level = 100;

    /* ����豸�Ƿ�֧�ֶ����������� */
    if(!FLAG_BROKEN_BATTERY_LEVEL(ptp_usb) &&
            ptp_property_issupported( current_params, PTP_DPC_BatteryLevel)) {
      PTPDevicePropDesc dpd;

      /* ���Զ�������*/
      if(ptp_getdevicepropdesc(current_params, PTP_DPC_BatteryLevel,
                   &dpd) != PTP_RC_OK) {
          add_error_to_errorstack(mtp_device, LIBMTP_ERROR_CONNECTING,
                    "Unable to read Maximum Battery Level for this "
                    "device even though the device supposedly "
                    "supports this functionality");
      }

      /* TODO: is this appropriate? */
      /* �����������0����ʹ��Ĭ��ֵ*/
      if (dpd.FORM.Range.MaximumValue.u8 != 0) {
        mtp_device->maximum_battery_level = dpd.FORM.Range.MaximumValue.u8;
      }

      ptp_free_devicepropdesc(&dpd);
    }
    /* ��������Ĭ���ļ���Ϊ 0xffffffffU (��Ŀ¼) */
    mtp_device->default_music_folder = 0xffffffffU;
    mtp_device->default_playlist_folder = 0xffffffffU;
    mtp_device->default_picture_folder = 0xffffffffU;
    mtp_device->default_video_folder = 0xffffffffU;
    mtp_device->default_organizer_folder = 0xffffffffU;
    mtp_device->default_zencast_folder = 0xffffffffU;
    mtp_device->default_album_folder = 0xffffffffU;
    mtp_device->default_text_folder = 0xffffffffU;

    /* ���ó�ʼ�洢��Ϣ */
    mtp_device->storage = NULL;
    /* ��ȡMTP�豸�洢��Ϣ*/
    if (LIBMTP_Get_Storage(mtp_device, LIBMTP_STORAGE_SORTBY_NOTSORTED) == -1) {
        add_error_to_errorstack(mtp_device,
                  LIBMTP_ERROR_GENERAL,
                  "Get Storage information failed.");
        mtp_device->storage = NULL;
    }

    return mtp_device;
}


/**
 * MTP��ԭʼ�豸
 * @param rawdevice MTPԭʼ�豸ָ��
 * @return �ɹ�����MTP�豸ָ��
 */
LIBMTP_mtpdevice_t *LIBMTP_Open_Raw_Device(LIBMTP_raw_device_t *rawdevice)
{
    /* ��ԭʼ�豸*/
    LIBMTP_mtpdevice_t *mtp_device = LIBMTP_Open_Raw_Device_Uncached(rawdevice);

    /* ����Ƿ���MTPZ�豸 */
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

    /* �����豸Ϊ�ѻ���*/
    mtp_device->cached = 1;

    /* Ȼ���ȡ��������Զ�λĬ���ļ��У��������ĸ������ǻ����豸�е����о�����Ӷ��ӿ��Ժ�Ĳ���*/
    flush_handles(mtp_device);
    return mtp_device;
}

/**
 * ��ȡ�豸�б��пɵõ��豸������
 *
 * @param device_list MTP�豸����
 *
 * @return �ɹ������豸�������豸������
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
 * ��Ӵ��󵽴���ջ
 *
 * @param device     MTP�豸
 * @param ptp_error  ������
 * @param error_text ���������ı�
 **/
static void add_error_to_errorstack(LIBMTP_mtpdevice_t *device,
                    LIBMTP_error_number_t errornumber,
                    char const * const error_text){
    LIBMTP_error_t *newerror;

    if (device == NULL) {
        LIBMTP_ERROR("LIBMTP PANIC: Trying to add error to a NULL device!\n");
        return;
    }
    /* ����һ��������Ϣ�ṹ���ڴ�*/
    newerror = (LIBMTP_error_t *) mtp_malloc(sizeof(LIBMTP_error_t));
    newerror->errornumber = errornumber;
    newerror->error_text = mtp_strdup(error_text);
    newerror->next = NULL;
    /* ���뵽MTP�豸�Ĵ���ջ������*/
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
 * ���һ��PTP��Ĵ��󵽴���ջ��
 *
 * @param device     MTP�豸
 * @param ptp_error  ������
 * @param error_text ���������ı�
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
 * �رպ��ͷ�һ���ѷ����MTP�豸
 *
 * @param device Ҫ�ͷŵ�MTP�豸
 **/
void LIBMTP_Release_Device(LIBMTP_mtpdevice_t *device)
{
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;

    /* �ر�MTP�豸*/
    close_device(ptp_usb, params);
    /* �������ջ*/
    LIBMTP_Clear_Errorstack(device);
#if defined(HAVE_ICONV) && defined(HAVE_LANGINFO_H)
    iconv_close(params->cd_locale_to_ucs2);
    iconv_close(params->cd_ucs2_to_locale);
#endif
    mtp_free(ptp_usb);
    /* �ͷ�PTP����*/
    ptp_free_params(params);
    mtp_free(params);
    /* �ͷŴ洢����*/
    free_storage_list(device);
    /* �ͷ���չ����*/
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
 * ���MTP�豸����ջ
 *
 * @param device MTP�豸
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
 * ����MTP�豸����ջ
 *
 * @param device MTP�豸
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
 * �������ͨ������Ŀ¼������ȡ���о�������ݣ��˼�����ͨ����ȡ���������Ԫ���ݣ�0xffffffff��ζ��
 * �����ж��������Ԫ���ݡ�����Ծ������MTP�豸(Ҳ������)����Ч�������ٶȷǳ��졣����ڻ�ȡԪ����ʱ�����ˣ���
 * ������ջ��
 *
 * @param device MTP�豸
 * @return �ɹ�����0��
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
     * ��������ʹ�豸�����豸��ÿ���ļ����������ڵ�����Ӧ�з�����
     *
     * һЩ�ٶȽ������豸�Լ����зǳ�����ļ�ϵͳ���豸��������Ҫ�ȱ�׼����ֵ������ʱ��
     * ���ܷ�����Ӧ��
     *
     * ��ʱ���ó�ʱ������ʹ����㷺���豸
     */
    /* ��ȡUSB��ʱ�¼�*/
    get_usb_device_timeout(ptp_usb, &oldtimeout);
    /* ��������USB��ʱʱ��*/
    set_usb_device_timeout(ptp_usb, 60000);
    /* ��ȡ��������*/
    ret = ptp_mtp_getobjectproplist(params, 0xffffffff, &props, &nrofprops);
    /* ��������USB��ʱʱ��*/
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

    /* ����ͨ������ObjectHandle���������������ÿ�����ı�ʱ�����ǵõ�һ���µĶ��󣬵���ͬʱ
     * ��ֻ��ͬһ������Ĳ�ͬ����*/
    prop = props;
    /* Ѱ�Ҳ�ͬ�Ķ���ĸ���*/
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
                /* ���ñ�־Ϊ�Ѽ���*/
                params->objects[i].flags |= PTPOBJECT_OBJECTINFO_LOADED;
                /* ��������ļ����Ƿ�Ϊ��*/
                if (!params->objects[i].oi.Filename) {
                    /* I have one such file on my Creative (Marcus) */
                    params->objects[i].oi.Filename = mtp_strdup("<null>");
                }
            }
            i++;
            /* ��ȡ������*/
            lasthandle = prop->ObjectHandle;
            params->objects[i].oid = prop->ObjectHandle;
        }
        /* ���ö�������*/
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

            /* �������е�������MTP���Ե�ÿһ�����������������*/
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
    /* ������һ������ */
    if (i >= 0) {
        params->objects[i].flags |= PTPOBJECT_OBJECTINFO_LOADED;
        params->nrofobjects = i + 1;
    } else {
        params->nrofobjects = 0;
    }
    mtp_free (props);
    /* ����PTP���� */
    ptp_objects_sort (params);
    return 0;
}

/**
 * �ݹ�Ļ�ȡ����
 * ����������豸�ϵ�����Ŀ¼�еݹ飬�Ӹ�Ŀ¼��ʼ�����ƶ����ռ�Ԫ���ݡ���ĳЩ�豸��ʹ�������ã���Щ�豸
 * ֻ����ĳ��Ŀ¼�����ݣ������ǻ�ȡ���ж����Ԫ���ݡ�
 *
 * @param device    MTP�豸
 * @param params    PTP����
 * @param storageid ��ȡ����Ĵ洢��ID
 * @param parent    ��ȡ����ĸ�����
 *
 * @return �ɹ�����PTP_RC_OK
 **/
u16 get_handles_recursively(LIBMTP_mtpdevice_t *device,
                    PTPParams *params, u32 storageid, u32 parent){
    PTPObjectHandles currentHandles;
    int i = 0;

    /* ��ȡ������*/
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

    /* ���ڽ����ҵ����κ���Ŀ¼*/
    for (i = 0; i < currentHandles.n; i++) {
        PTPObject *ob;
        /* ���ض�����Ϣ*/
        ret = ptp_object_want(params,currentHandles.Handler[i],
                PTPOBJECT_OBJECTINFO_LOADED, &ob);
        if (ret == PTP_RC_OK) {
            /* �Ǹ�Ŀ¼*/
            if (ob->oi.ObjectFormat == PTP_OFC_Association)
                /* ��������Ŀ¼�µĶ���*/
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
 * ÿ���豸�д洢����ı�ʱ���˺�������ˢ���ڲ�����������ڲ���ӻ��Ƴ�����Ĳ�����һ�㲻����
 * ���������
 *
 * @param device ʹ�õ�MTP�豸
 **/
static void flush_handles(LIBMTP_mtpdevice_t *device)
{
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    int ret;
    u32 i;
    /* ����豸�Ƿ��Ѿ�������*/
    if (!device->cached) {
        return;
    }
    /* PTP�����Ѿ����ڶ���*/
    if (params->objects != NULL) {
        /* �ͷŶ���*/
        for (i = 0;i < params->nrofobjects;i++)
            ptp_free_object (&params->objects[i]);
        mtp_free(params->objects);
        params->objects = NULL;
        params->nrofobjects = 0;
    }
    /* ֧�֡���ȡ�������ԡ�����*/
    if (ptp_operation_issupported(params,PTP_OC_MTP_GetObjPropList)
            && !FLAG_BROKEN_MTPGETOBJPROPLIST(ptp_usb)
            && !FLAG_BROKEN_MTPGETOBJPROPLIST_ALL(ptp_usb)) {
        /* ʹ�����ķ�ʽ���Ȼ����෵��ֵ*/
        ret = get_all_metadata_fast(device);
    }

    /* ���֮ǰ�Ĳ���ʧ�ܻ򷵻�û�ж�����ʹ�������ķ���*/
    if (params->nrofobjects == 0) {
        /* ʹ��ͨ�õ������ȡ���о��*/
        if (device->storage == NULL) {
            /* ��ȡ���д洢�ĸ�Ŀ¼�µĶ���*/
            get_handles_recursively(device, params,
                    PTP_GOH_ALL_STORAGE,
                    PTP_GOH_ROOT_PARENT);
        } else {
            /* ���λ�ȡÿ���洢�ľ��*/
            LIBMTP_devicestorage_t *storage = device->storage;
            while(storage != NULL) {
                /* ��ȡ��Ҫ�洢�ĸ�Ŀ¼�µĶ���*/
                get_handles_recursively(device, params,
                        storage->id,
                        PTP_GOH_ROOT_PARENT);
                storage = storage->next;
            }
        }
    }

    /* ����������޸��κο��ļ���������룬Ȼ������Ҫ�Ĵ洢�豸�ĸ��ļ�Ŀ¼�¶�λһЩĬ�ϵ��ļ���*/
    for(i = 0; i < params->nrofobjects; i++) {
        PTPObject *ob, *xob;

        ob = &params->objects[i];
        /* ִ�м��ض�����Ϣ����*/
        ret = ptp_object_want(params,params->objects[i].oid,
                PTPOBJECT_OBJECTINFO_LOADED, &xob);
        if (ret != PTP_RC_OK) {
            LIBMTP_ERROR("broken! %x not found\n", params->objects[i].oid);
        }
        if (ob->oi.Filename == NULL)
            ob->oi.Filename = mtp_strdup("<null>");
        if (ob->oi.Keywords == NULL)
            ob->oi.Keywords = mtp_strdup("<null>");

        /* ���Է�Ŀ¼�Ķ��� */
        if(ob->oi.ObjectFormat != PTP_OFC_Association)
            continue;
        /* ֻ�ڸ��ļ�Ŀ¼�� */
        if (ob->oi.ParentObject == 0xffffffffU) {
            LIBMTP_ERROR("object %x has parent 0xffffffff (-1) continuing anyway\n",
                        ob->oid);
        } else if (ob->oi.ParentObject != 0x00000000U)
            continue;
        /* ֻʹ����Ҫ�Ĵ洢�豸 */
        if (device->storage != NULL && ob->oi.StorageID != device->storage->id)
            continue;

        /* ����Ĭ���ļ��� */
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
 * �ͷ��豸�Ĵ洢��Ϣ����
 *
 * @param device Ҫ�ͷŴ洢��Ϣ�����MTP�豸ָ��
 */
static void free_storage_list(LIBMTP_mtpdevice_t *device)
{
    LIBMTP_devicestorage_t *storage;
    LIBMTP_devicestorage_t *tmp;

    /* ��ȡ�洢��Ϣ����*/
    storage = device->storage;
    while(storage != NULL) {
        /* �ͷŴ洢��Ϣ����*/
        if (storage->StorageDescription != NULL) {
            mtp_free(storage->StorageDescription);
        }
        /* �ͷž�ID*/
        if (storage->VolumeIdentifier != NULL) {
            mtp_free(storage->VolumeIdentifier);
        }
        /* ��ȡ��һ���洢��Ϣ*/
        tmp = storage;
        storage = storage->next;
        /* �ͷŴ�����Ϣ*/
        mtp_free(tmp);
    }
  device->storage = NULL;

  return;
}

/*
 * ����sortby��ʽ�����豸�洢��Ϣ
 *
 * @param device MTP�豸
 * @param sortby ���з�ʽ(�ɴ�С)
 *
 * @return �ɹ�����0
 **/
static int sort_storage_by(LIBMTP_mtpdevice_t *device, int const sortby)
{
    LIBMTP_devicestorage_t *oldhead, *ptr1, *ptr2, *newlist;

    /* �Ϸ��Լ��*/
    if (device->storage == NULL)
        return -1;
    /* ��������Ϊ�������ࡱ*/
    if (sortby == LIBMTP_STORAGE_SORTBY_NOTSORTED)
        return 0;

    oldhead = ptr1 = ptr2 = device->storage;

    newlist = NULL;

    while(oldhead != NULL) {
        ptr1 = ptr2 = oldhead;
        /* �ҵ������пռ䡱�����ռ䡱���Ĵ洢��Ϣ�ṹ��*/
        while(ptr1 != NULL) {
            /* ���տ��пռ����*/
            if (sortby == LIBMTP_STORAGE_SORTBY_FREESPACE && ptr1->FreeSpaceInBytes > ptr2->FreeSpaceInBytes)
                ptr2 = ptr1;
            /* �������ռ����*/
            if (sortby == LIBMTP_STORAGE_SORTBY_MAXSPACE && ptr1->FreeSpaceInBytes > ptr2->FreeSpaceInBytes)
                ptr2 = ptr1;

            ptr1 = ptr1->next;
        }

        /* ���ҵ��Ĵ洢��Ϣ�ڵ��ǰһ���ڵ��nextָ��ָ���һ���ڵ�*/
        if(ptr2->prev != NULL) {
            ptr1 = ptr2->prev;
            ptr1->next = ptr2->next;
        } else {
            oldhead = ptr2->next;
            if(oldhead != NULL)
                oldhead->prev = NULL;
        }

        /* ���ҵ��Ĵ洢��Ϣ�ڵ�ĺ�һ���ڵ��prevָ��ָ��ǰһ���ڵ�*/
        ptr1 = ptr2->next;
        if(ptr1 != NULL) {
            ptr1->prev = ptr2->prev;
        } else {
            ptr1 = ptr2->prev;
            if(ptr1 != NULL)
                ptr1->next = NULL;
        }
        /* �嵽�µ�������ȥ*/
        if(newlist == NULL) {
            newlist = ptr2;
            newlist->prev = NULL;
        } else {
            ptr2->prev = newlist;
            newlist->next = ptr2;
            newlist = newlist->next;
        }
    }
    /* ���µ��������¸�ֵ��MTP�豸*/
    if (newlist != NULL) {
        newlist->next = NULL;
        while(newlist->prev != NULL)
            newlist = newlist->prev;
        device->storage = newlist;
    }

    return 0;
}

/**
 * ����������豸�洢������Ѱ�ҵ�һ����д�Ĵ洢�豸
 *
 * @param device  MTP�豸
 * @param fitsize �ļ���С
 *
 * @return �ɹ����ش洢�豸��ID
 **/
static u32 get_writeable_storageid(LIBMTP_mtpdevice_t *device,
                                   u64 fitsize){
    LIBMTP_devicestorage_t *storage;
    /* Ӧ����0xffffffffu��*/
    u32 store = 0x00000000;
    int subcall_ret;

    /* ���Ƿ��д洢�豸�����ʺ�����ļ���С*/
    storage = device->storage;
    if (storage == NULL) {
        /* ��ʱ��洢�豸��ⲻ��*/
        store = 0x00000000U;
    } else {
        while(storage != NULL) {
            /* ��Щ�洢�豸����ʹ��*/
            if (storage->StorageType == PTP_ST_FixedROM ||
                    storage->StorageType == PTP_ST_RemovableROM) {
                storage = storage->next;
                continue;
            }
            /* �洢�豸ID�ĵ�16λ0x0000��Ӧ���ǿ�д��*/
            if ((storage->id & 0x0000FFFFU) == 0x00000000U) {
                storage = storage->next;
                continue;
            }
            /* ���洢�豸�ķ���Ȩ��*/
            if (storage->AccessCapability == PTP_AC_ReadOnly ||
                    storage->AccessCapability == PTP_AC_ReadOnly_with_Object_Deletion) {
                storage = storage->next;
                continue;
            }
            /* ����Ƿ��ʺ�����ļ��Ĵ�С*/
            subcall_ret = check_if_file_fits(device, storage, fitsize);
            if (subcall_ret != 0) {
                storage = storage->next;
            } else {
                /* �ҵ�һ����д���ʺ�����ļ���С�Ĵ洢�豸*/
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
 * ����ͨ����Ŀ¼ID(�����)ȥ��ȡ��Ŀ¼�Ĵ洢�豸ID
 *
 * @param device    MTP�豸
 * @param fitsize   �ļ���С
 * @param parent_id ��Ŀ¼ID
 *
 * @return �ɹ����ش洢�豸ID
 **/
static int get_suggested_storage_id(LIBMTP_mtpdevice_t *device,
                                    u64 fitsize, u32 parent_id){
    PTPParams *params = (PTPParams *) device->params;
    PTPObject *ob;
    u16 ret;

    /* ͨ����Ŀ¼IDѰ�Ҹ�Ŀ¼�Ķ���*/
    ret = ptp_object_want(params, parent_id, PTPOBJECT_MTPPROPLIST_LOADED, &ob);
    if ((ret != PTP_RC_OK) || (ob->oi.StorageID == 0)) {
        add_ptp_error_to_errorstack(device, ret, "get_suggested_storage_id(): "
                "could not get storage id from parent id.");
        return get_writeable_storageid(device, fitsize);
    } else {
        /* ���ش洢�豸ID */
        return ob->oi.StorageID;
    }
}

/**
 * ���豸�洢������һ��ȷ���Ĵ洢�豸�л�ȡʣ��ռ�
 *
 * @param device    MTP�豸
 * @param storage   MTP�豸�еĴ洢�豸
 * @param freespace Ҫ���ص�ʣ��洢�ռ�
 *
 * @return �ɹ�����0
 **/
static int get_storage_freespace(LIBMTP_mtpdevice_t *device,
                                 LIBMTP_devicestorage_t *storage,
                                 u64 *freespace){
    PTPParams *params = (PTPParams *) device->params;

    /* ����ѯ�ʴ洢�豸��Ϣ����Ϊ��һЩ�豸�ͺ���ȷ��Ҫ�������ڲ�ѯ�洢�豸��Ϣʱˢ����������*/
    if (ptp_operation_issupported(params,PTP_OC_GetStorageInfo)) {
        PTPStorageInfo storageInfo;
        u16 ret;
        /* ��ȡ�洢�豸��Ϣ*/
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
    /* ���ʣ��ռ��С�Ƿ�Ϸ�*/
    if(storage->FreeSpaceInBytes == (uint64_t) -1)
        return -1;
    *freespace = storage->FreeSpaceInBytes;
    return 0;
}

/**
 * �˺������ڴ�ӡ��PTPЭ���ṩ�Ĵ����ı���Ϣ���Լ�����һЩMTP�ض���Ϣ(�������)��
 *
 * @param device ���ϱ���Ϣ��MTP�豸
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
    /* ��ӡһЩ��ϸ��Ϣ */
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
                    /* ��ȡ����ϸһ�������*/
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
 * ��λ�豸
 *
 * @param device Ҫ��λ��MTP�豸
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
 **/
int LIBMTP_Reset_Device(LIBMTP_mtpdevice_t *device)
{
    PTPParams *params = (PTPParams *) device->params;
    u16 ret;

    /* ��֧�ָ�λ�豸*/
    if (!ptp_operation_issupported(params,PTP_OC_ResetDevice)) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL,
                                "LIBMTP_Reset_Device(): "
                                "device does not support resetting.");
        return -1;
    }
    /* PTP��λ�豸*/
    ret = ptp_resetdevice(params);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "Error resetting.");
        return -1;
    }
    return 0;
}

/**
 * �⽫����MTP�ġ��Ѻ����ơ��豸��ͨ����ֻ�������ߵ����֣����������ڡ�John Doe's Digital Audio Player����
 * �������Ӧ�ñ�����MTP�豸��֧�֡�
 *
 * @param device MTP�豸
 *
 * @return �ɹ�����һ���·���Ĵ����Ѻ����Ƶ�UTF-8��ʽ���ַ���
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
    /* ��ȡ�豸����ֵ*/
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
 * �⽫����MTP�豸��ͬ����顣�������Ӧ�ñ�����MTP�豸֧�֡�
 *
 * @param device Ҫ������MTP�豸
 *
 * @return һ���·���Ĵ���ͬ������UTF-8�ַ����������ʹ�ú���뱻�������ͷ�
 **/
char *LIBMTP_Get_Syncpartner(LIBMTP_mtpdevice_t *device)
{
    PTPPropertyValue propval;
    char *retstring = NULL;
    PTPParams *params = (PTPParams *) device->params;
    u16 ret;
    /* ����Ƿ�֧��*/
    if (!ptp_property_issupported(params, PTP_DPC_MTP_SynchronizationPartner)) {
        return NULL;
    }
    /* ��ȡ�豸����ֵ*/
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
 * ����豸�Ƿ���Դ洢һ���ļ�
 *
 * @param device   MTP�豸
 * @param storage  Ҫ���Ĵ洢�豸
 * @param filesize �ļ���С
 *
 * @return
 **/
static int check_if_file_fits(LIBMTP_mtpdevice_t *device,
                              LIBMTP_devicestorage_t *storage,
                              u64 const filesize) {
    PTPParams *params = (PTPParams *) device->params;
    u64 freebytes;
    int ret;

    /* ����Ƿ���Ի�ȡ�洢�豸��Ϣ*/
    if (!ptp_operation_issupported(params,PTP_OC_GetStorageInfo)) {
        return 0;
    }
    /* ��ȡ�洢�豸ʣ��ռ�*/
    ret = get_storage_freespace(device, storage, &freebytes);
    if (ret != 0) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL,
                "check_if_file_fits(): error checking free storage.");
        return -1;
    } else {
        /* ���ʣ��ռ��Ƿ������ļ���С*/
        if (filesize > freebytes) {
            return -1;
        }
    }
    return 0;
}

/**
 * ������������豸�ĵ�ǰ����
 *
 * @param Ҫ������MTP�豸
 * @param Ҫ���ص�������
 * @param Ҫ���صĵ�ǰ����
 *
 * @return �ɹ�����0������ֵ��ʾʧ�ܣ�ʧ�ܵ�һ��ԭ�����豸��֧�ֵ�������
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
    /* ��ȡ���豸����ֵ*/
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
 * ��ʽ���洢�豸
 *
 * @param device  MTP�豸
 * @param storage Ҫ��ʽ���Ĵ洢�豸��ID
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
 */
int LIBMTP_Format_Storage(LIBMTP_mtpdevice_t *device,
                          LIBMTP_devicestorage_t *storage)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    /* ����Ƿ�֧�ָ�ʽ���洢�豸*/
    if (!ptp_operation_issupported(params,PTP_OC_FormatStore)) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL,
                "LIBMTP_Format_Storage(): "
                "device does not support formatting storage.");
        return -1;
    }
    /* ��ʽ���洢�豸*/
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
 * ���豸����ȡUnicode���Եĺ��������Ǽ���PTP�淶��������Unicode�豸���Եı�׼������
 *
 * @param device      Ҫ��ȡ���Ե��豸
 * @param unicstring  Ҫ���ص�����
 * @param property    Ҫ����������
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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

    /* Unicode�ַ�����16λ�޷�����������*/
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

    /* ��ȡʵ�ʵ�����*/
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
 * ���������XML�ļ��ַ�������ʽ�����豸�İ�ȫʱ��
 *
 * @param device  Ҫ��ȡ��ȫʱ���MTP�豸
 * @param sectime Ҫ���صİ�ȫʱ���ַ�����ʧ�ܷ��ؿ�
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ��
 **/
//int LIBMTP_Get_Secure_Time(LIBMTP_mtpdevice_t *device, char ** const sectime)
//{
//    return get_device_unicode_property(device, sectime, PTP_DPC_MTP_SecureTime);
//}

/**
 * ����������豸(��Կ)֤����ΪXML�ĵ��ַ������豸����
 *
 * @param device  Ҫ��ȡ�豸֤���MTP�豸
 * @param devcert Ҫ���ص��豸֤���ַ�����ʧ�ܷ��ؿ�
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ��
 **/
//int LIBMTP_Get_Device_Certificate(LIBMTP_mtpdevice_t *device, char ** const devcert)
//{
//    return get_device_unicode_property(device, devcert, PTP_DPC_MTP_DeviceCertificate);
//}


/**
 * �������������֧�ֵ��ļ����ͣ����豸����֧�ֵ��ļ����͡������豸���Բ��ŵ���Ƶ�ļ��ȵȡ�����б�
 * ���򻯵�����libmtp���Դ�����ļ����ͣ����������г�libmtp�����ڲ�������ļ����ͣ��粥���б��
 * �ļ��С�
 *
 * @param device    Ҫ������MTP�豸
 * @param filetypes Ҫ���ص�֧�ֵ��ļ����͵�ָ��
 * @param length    Ҫ���ص�����ĳ���
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
 **/
int LIBMTP_Get_Supported_Filetypes(LIBMTP_mtpdevice_t *device, u16 ** const filetypes,
                                   u16 * const length){
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    u16 *localtypes;
    u16 localtypelen;
    u32 i;

    /* �����δ֪���ͣ�������Ҫ���������ڴ�*/
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
 * ��ȡ�����MTP�豸�Ĵ洢��Ϣ
 *
 * @param device ��Ҫ��ȡ�洢��Ϣ��MTP�豸ָ��
 * @param sortby ��libmtp.h�����LIBMTP_STORAGE_SORTBY_. 0 �� LIBMTP_STORAGE_SORTBY_NOTSORTED
 *               ����
 *
 * @return �ɹ�����0������1ָ��ֻ�ܻ�ȡ�洢ID���ܻ�ȡ�洢���ԣ�����-1ָ��ʧ��
 */
int LIBMTP_Get_Storage(LIBMTP_mtpdevice_t *device, int const sortby)
{
    u32 i = 0;
    PTPStorageInfo storageInfo;
    PTPParams *params = (PTPParams *) device->params;
    PTPStorageIDs storageIDs;
    LIBMTP_devicestorage_t *storage = NULL;
    LIBMTP_devicestorage_t *storageprev = NULL;

    /* MTP�豸���ڴ�����Ϣ���ͷŵ�*/
    if (device->storage != NULL)
        free_storage_list(device);

    //if (!ptp_operation_issupported(params,PTP_OC_GetStorageIDs))
    //    return -1;
    /* ��ȡ�洢ID*/
    if (ptp_getstorageids (params, &storageIDs) != PTP_RC_OK)
        return -1;
    /* ���洢��Ϣ������*/
    if (storageIDs.n < 1)
        return -1;
    /* ����Ƿ�֧�֡���ȡ�洢��Ϣ������*/
    if (!ptp_operation_issupported(params,PTP_OC_GetStorageInfo)) {
        for (i = 0; i < storageIDs.n; i++) {
            /* �����ڴ�*/
            storage = (LIBMTP_devicestorage_t *)mtp_malloc(sizeof(LIBMTP_devicestorage_t));
            storage->prev = storageprev;
            if (storageprev != NULL)
                storageprev->next = storage;
            if (device->storage == NULL)
                device->storage = storage;
            /* ��֧�ֻ�ȡ�洢��Ϣ����ʼ���洢��Ϣ*/
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
            /* ��ȡ�洢��Ϣ*/
            ret = ptp_getstorageinfo(params, storageIDs.Storage[i], &storageInfo);
            if (ret != PTP_RC_OK) {
                /* ��Ӵ��󵽴���ջ*/
                add_ptp_error_to_errorstack(device, ret, "LIBMTP_Get_Storage(): "
                                  "Could not get storage info.");
                if (device->storage != NULL) {
                    /* �ͷŴ洢��Ϣ����*/
                    free_storage_list(device);
                }
                return -1;
            }
            /* ����MTP�豸�洢��Ϣ�ṹ���ڴ�*/
            storage = (LIBMTP_devicestorage_t *)mtp_malloc(sizeof(LIBMTP_devicestorage_t));
            storage->prev = storageprev;
            if (storageprev != NULL)
                storageprev->next = storage;
            if (device->storage == NULL)
                device->storage = storage;
            /* ����MTP�豸�洢��Ϣ�ṹ��*/
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
        /* ���д洢��Ϣ*/
        sort_storage_by(device, sortby);
        mtp_free(storageIDs.Storage);
        return 0;
    }
}

/**
 * ����һ�����ļ�Ԫ���ݽṹ��
 *
 * @return �ɹ������·�����ļ��ṹ��
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
 * ����һ���ļ�
 *
 * @param file Ҫ���ٵ��ļ�Ԫ����
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
 * �Ѷ���ת��Ϊ�ļ����ݽṹ��
 *
 * @param device MTP�豸
 * @param ob     ����ṹ��
 *
 * @return �ɹ�����MTP�ļ��ṹ��
 */
static LIBMTP_file_t *obj2file(LIBMTP_mtpdevice_t *device, PTPObject *ob)
{
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    LIBMTP_file_t *file;
    int i;

    /* ����һ���µ��ļ��ڴ�*/
    file = LIBMTP_new_file_t();

    file->parent_id = ob->oi.ParentObject;
    file->storage_id = ob->oi.StorageID;
    /* ��ȡ�ļ���*/
    if (ob->oi.Filename != NULL) {
        file->filename = mtp_strdup(ob->oi.Filename);
    }

    /* �����ļ�����*/
    file->filetype = map_ptp_type_to_libmtp_type(ob->oi.ObjectFormat);

    /* һ��������豸���ݣ�һЩ�ļ������Ϊ��δ֪�����ͣ���ʵ��OGG��FLAC�ļ�������ļ���׺��*/
    /* δ֪�ļ�����*/
    if (file->filetype == LIBMTP_FILETYPE_UNKNOWN) {
        /* ����ļ������ǲ��ǡ�.ogg��*/
        if ((FLAG_IRIVER_OGG_ALZHEIMER(ptp_usb) ||
                FLAG_OGG_IS_UNKNOWN(ptp_usb)) &&
                has_ogg_extension(file->filename)) {
            file->filetype = LIBMTP_FILETYPE_OGG;
        }
        /* ����ļ������ǲ��ǡ�.flac��*/
        if (FLAG_FLAC_IS_UNKNOWN(ptp_usb) && has_flac_extension(file->filename)) {
            file->filetype = LIBMTP_FILETYPE_FLAC;
        }
    }

    /* �����ļ�����޸�����*/
    file->modificationdate = ob->oi.ModificationDate;

    /* ����ֻ��32λ�ļ���С��֮������ʹ��PTP_OPC_ObjectSize����*/
    file->filesize = ob->oi.ObjectCompressedSize;

    /* �ļ�ID������׷������ļ�*/
    file->item_id = ob->oid;

    /* ��������л���Ԫ���ݣ�ʹ����*/
    if (ob->mtpprops) {
        MTPProperties *prop = ob->mtpprops;

        for (i = 0; i < ob->nrofmtpprops; i++, prop++) {
            /* ѡ������С*/
            if (prop->property == PTP_OPC_ObjectSize) {
                /* ��������Ѿ������ã���64λ����ֵ��PTP32λֵ���ã�������д��*/
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

        /* ���Ȳ鿴����Ϊ�˶����ʽ������Щ����*/
        ret = ptp_mtp_getobjectpropssupported(params, map_libmtp_type_to_ptp_type(file->filetype), &propcnt, &props);
        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "obj2file: call to ptp_mtp_getobjectpropssupported() failed.");
            // Silently fall through.
        } else {
            /* ��������֧�ֵ�����*/
            for (i = 0; i < propcnt; i++) {
                switch (props[i]) {
                case PTP_OPC_ObjectSize:
                    if (device->object_bitsize == 64) {
                        /* ��ȡ�ļ���С*/
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
 * �����豸��һ�������ļ���Ԫ����
 *
 * @param device ��ص�MTP�豸
 * @param fileid Ҫ��ȡ�Ķ����ID
 *
 * @return �ɹ�����Ԫ���ݵ����
 **/
LIBMTP_file_t *LIBMTP_Get_Filemetadata(LIBMTP_mtpdevice_t *device, u32 const fileid)
{
    PTPParams *params = (PTPParams *) device->params;
    u16 ret;
    PTPObject *ob;

    /* ��ȡ�豸���о����ֻ���ѻ����˵��豸*/
    if (device->cached && params->nrofobjects == 0) {
        flush_handles(device);
    }
    /* ����IDΪfileid�Ķ���*/
    ret = ptp_object_want(params, fileid, PTPOBJECT_OBJECTINFO_LOADED|PTPOBJECT_MTPPROPLIST_LOADED, &ob);
    if (ret != PTP_RC_OK)
        return NULL;

    /* ת�����ļ��ṹ��*/
    return obj2file(device, ob);
}

/**
 * ����������ڼ���ĳ���豸��ĳ���洢��ĳ�������ļ��е�����
 *
 * @param device  ��ص�MTP�豸
 * @param storage �洢�豸��ID
 * @param parent  ����Ŀ¼��ID
 *
 * @return �ɹ�����һ������Ŀ¼���ļ�������
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
        /* �������ֻ����δ������豸ʹ��*/
        LIBMTP_ERROR("tried to use %s on a cached device!\n", __func__);
        return NULL;
    }
    /* ���洢�豸ID*/
    if (storage == 0)
        storageid = PTP_GOH_ALL_STORAGE;
    else
        storageid = storage;
    /* ��ȡparentĿ¼�µ����ж�����*/
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

        /* ��ȡһ���ļ���Ԫ���ݣ����ʧ�ܣ�������һ��*/
        file = LIBMTP_Get_Filemetadata(device, currentHandles.Handler[i]);
        if (file == NULL)
            continue;

        /* ����һ������֮�󷵻��׵�ַ*/
        if (curfile == NULL) {
            curfile = file;
            retfiles = file;
        } else {
            curfile->next = file;
            curfile = file;
        }
    }

    mtp_free(currentHandles.Handler);

    /* ���������е�һ���ļ��ĵ�ַ*/
    return retfiles;
}

/**
 * ����һ���µ���ĿԪ���ݽṹ��
 *
 * @return �ɹ������·������ĿԪ���ݽṹ��
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
 * ����һ����ĿԪ���ݽṹ��
 *
 * @param track Ҫ���ٵ���ĿԪ���ݽṹ��
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
 * �������������ӳ��Ϳ�����ʹ�õ���ĿԪ������
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
 * ͨ����������ĿID������Ŀ��Ԫ����
 *
 * @param device       ���MTP�豸
 * @param trackid      ��ĿID
 * @param objectformat �����ʽ
 * @param track        Ҫ���ص���������Ŀ�ṹ��
 */
static void get_track_metadata(LIBMTP_mtpdevice_t *device, u16 objectformat,
                               LIBMTP_track_t *track)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    u32 i;
    MTPProperties *prop;
    PTPObject *ob;

    /* ���������Ԫ���ݣ���ʹ����*/
    ret = ptp_object_want(params, track->item_id, PTPOBJECT_MTPPROPLIST_LOADED, &ob);
    if (ob->mtpprops) {
        prop = ob->mtpprops;
        for (i = 0;i < ob->nrofmtpprops;i++,prop++)
            pick_property_to_track_metadata(device, prop, track);
    } else {
        u16 *props = NULL;
        u32 propcnt = 0;

        /* ������������ʽ֧����Щ����*/
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
                /* ��Щ������ô��Ҫ*/
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
 * ����һ���豸��Ŀ��Ԫ����
 *
 * @param device  ��ص�MTP�豸
 * @param trackid Ҫ��������Ŀ��ID
 *
 * @@return �ɹ����� ��Ŀ�ṹ�壬ʧ�ܷ���NULL
 */
LIBMTP_track_t *LIBMTP_Get_Trackmetadata(LIBMTP_mtpdevice_t *device, u32 const trackid)
{
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    PTPObject *ob;
    LIBMTP_track_t *track;
    LIBMTP_filetype_t mtptype;
    u16 ret;

    /* ��ȡ���о��*/
    if (params->nrofobjects == 0)
        flush_handles(device);

    ret = ptp_object_want (params, trackid, PTPOBJECT_OBJECTINFO_LOADED, &ob);
    if (ret != PTP_RC_OK)
        return NULL;

    mtptype = map_ptp_type_to_libmtp_type(ob->oi.ObjectFormat);

    /* �������ǲ�֪����ô����Ķ���*/
    if (!LIBMTP_FILETYPE_IS_TRACK(mtptype) &&
            /* ���м��δ������ļ�����Ϊ��������OGG��FLAC�ļ�*/
            (ob->oi.ObjectFormat != PTP_OFC_Undefined ||
                    (!FLAG_IRIVER_OGG_ALZHEIMER(ptp_usb) &&
                            !FLAG_OGG_IS_UNKNOWN(ptp_usb) &&
                            !FLAG_FLAC_IS_UNKNOWN(ptp_usb)))) {
        //printf("Not a music track (name: %s format: %d), skipping...\n", oi->Filename, oi->ObjectFormat);
        return NULL;
  }

    /* ����һ���µ���Ŀ�ṹ��*/
    track = LIBMTP_new_track_t();

    /* �����ܶ�λ�������Ŀ��ΨһID*/
    track->item_id = ob->oid;
    track->parent_id = ob->oi.ParentObject;
    track->storage_id = ob->oi.StorageID;
    track->modificationdate = ob->oi.ModificationDate;

    track->filetype = mtptype;

    /* ԭʼ���ض��ļ�����*/
    track->filesize = ob->oi.ObjectCompressedSize;
    if (ob->oi.Filename != NULL) {
        track->filename = mtp_strdup(ob->oi.Filename);
    }

    /* һ���ر�ļ��ݣ��豸����ǵ���Щ�ļ�������Ϊ��δ֪��������ʵ��OGG��FLAC�ļ���
     * ���Ǽ���ļ����ĺ�׺���ǲ��ǡ�.ogg����.flac��*/
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
            /* ����һ��OGG/FLAC�ļ������Զ�����*/
            LIBMTP_destroy_track_t(track);
            return NULL;
        }
    }
    /* ��ȡ��ĿԪ����*/
    get_track_metadata(device, ob->oi.ObjectFormat, track);
    return track;
}

/**
 * �⽫���豸���ļ���Ϣ��ȡ�����ļ���������ʶ���ļ���
 * ������ܿ���Ǳ�ڵ�������ʽ�����豸�ϵ��ļ��Թ����Ż�㲥�����磬ͨ�����ļ����ص���
 * ������(���磬�׽���)�ϡ�
 *
 * @param device   ��ص�MTP�豸
 * @param id       Ҫ�������ļ���ID
 * @param fd       Ҫд��ȥ���ļ�������
 * @param callback �ص�����
 * @param data     �ص������Ĳ���
 *
 * @return ����ɹ�����0��ʧ�ܷ�������ֵ
 **/
int LIBMTP_Get_File_To_File_Descriptor(LIBMTP_mtpdevice_t *device,
                                       u32 const id,
                                       int const fd,
                                       LIBMTP_progressfunc_t const callback,
                                       void const * const data){
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;

    /* ��ȡ�ļ�Ԫ����*/
    LIBMTP_file_t *mtpfile = LIBMTP_Get_Filemetadata(device, id);
    if (mtpfile == NULL) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Get_File_To_File_Descriptor(): Could not get object info.");
        return -1;
    }
    if (mtpfile->filetype == LIBMTP_FILETYPE_FOLDER) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Get_File_To_File_Descriptor(): Bad object format.");
        return -1;
    }

    /* �ص�*/
    ptp_usb->callback_active = 1;
    ptp_usb->current_transfer_total = mtpfile->filesize + PTP_USB_BULK_HDR_LEN + sizeof(u32); /* ���󳤶�*/
    ptp_usb->current_transfer_complete = 0;
    ptp_usb->current_transfer_callback = callback;
    ptp_usb->current_transfer_callback_data = data;

    /* ����Ҫmtpfile��*/
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
 * �⽫���豸����Ŀ��Ϣ��ȡ�����ļ���������ʶ���ļ���
 *
 * @param device   ��ص�MTP�豸
 * @param id       Ҫ��������Ŀ��ID
 * @param fd       Ҫд��ȥ���ļ�������
 * @param callback �ص�����
 * @param data     �ص������Ĳ���
 *
 * @return ����ɹ�����0��ʧ�ܷ�������ֵ
 **/
int LIBMTP_Get_Track_To_File_Descriptor(LIBMTP_mtpdevice_t *device,
                                        u32 const id,
                                        int const fd,
                                        LIBMTP_progressfunc_t const callback,
                                        void const * const data){
    return LIBMTP_Get_File_To_File_Descriptor(device, id, fd, callback, data);
}

/**
 * �ѱ��ص�һ���ļ����͵�MTP�豸
 *
 * @param device   ���MTP�豸
 * @param path     �����ļ���·��
 * @param filedata MTP�豸���ļ��ṹ��
 * @param callback �ص�����
 * @param data     �ص���������
 *
 * @reutrn �ɹ�����0��ʧ�ܷ�������ֵ
 */
int LIBMTP_Send_File_From_File(LIBMTP_mtpdevice_t *device,
                               char const * const path, LIBMTP_file_t * const filedata,
                               LIBMTP_progressfunc_t const callback,
                               void const * const data)
{
    int fd;
    int ret;

    /* �Ϸ��Լ��*/
    if (path == NULL) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Send_File_From_File(): Bad arguments, path was NULL.");
        return -1;
    }

    /* ���ļ�*/
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
    /* �����ļ�������*/
    ret = LIBMTP_Send_File_From_File_Descriptor(device, fd, filedata, callback, data);

    /* �ر��ļ�*/
#ifdef USE_WINDOWS_IO_H
    _close(fd);
#else
    mtp_close(fd);
#endif

    return ret;
}

/**
 * �ӱ����ļ��������з����ļ���MTP�豸
 *
 * @param device   ���MTP�豸
 * @param fd       �����ļ�������
 * @param filedata MTP�ļ��ṹ��
 * @param callback �ص�����
 * @param data     �ص���������
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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

    /* �����ļ�������Ϣ*/
    if (send_file_object_info(device, filedata))
    {
        /* ����Ҫ���������Ϣ��Ϊsend_file_object_info�Ѿ�����*/
        return -1;
    }

    /* �ص�*/
    ptp_usb->callback_active = 1;
    /* �ڷ�������ô�����ݺ󣬻ص���ֹͣ�Լ���һ������ͷ��������һ���������ݽ׶Ρ�����û�в���*/
    ptp_usb->current_transfer_total = filedata->filesize + PTP_USB_BULK_HDR_LEN * 2;
    ptp_usb->current_transfer_complete = 0;
    ptp_usb->current_transfer_callback = callback;
    ptp_usb->current_transfer_callback_data = data;

    /* ���ǿ�����Ҫ���ӳ�ʱʱ�䣬�ļ����ܴܺ���Ĭ�ϳ�ʱʱ�������δ�������ʱ��*/
    get_usb_device_timeout(ptp_usb, &oldtimeout);
    timeout = oldtimeout + (ptp_usb->current_transfer_total / guess_usb_speed(ptp_usb)) * 1000;
    set_usb_device_timeout(ptp_usb, timeout);
    /* ���Ͷ�Ӧ�ļ��������Ķ���*/
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
    /* �Ѷ�����ӵ�������*/
    add_object_to_cache(device, filedata->item_id);

    /* �ӻ����л�ȡ�豸�����parent_id��������ӵ�����Ĳ��������豸�в����������
     * ���Ǵӻ����л�ȡ�µ�parent_id*/
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
 * �����ļ�������Ϣ
 *
 * @param device   ��ص�MTP�豸
 * @param filedata Ҫ���ļ�һ��д����ļ�Ԫ���ݼ�
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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
    /* �Ϸ��Լ�飺��һд�豸������0�����ļ������0�����ļ���һЩ�豸�ϵ��³������⣬���������bug��־*/
    if (filedata->filesize == 0) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "send_file_object_info(): "
                "File of zero size.");
        return -1;
    }
#endif
    /* ���洢�豸�Ƿ�����ļ���С����*/
    if (filedata->storage_id != 0) {
        store = filedata->storage_id;
    } else {
        store = get_suggested_storage_id(device, filedata->filesize, localph);
    }

    /* ����Ƿ�ʹ����Ҫ�Ĵ洢�豸*/
    storage = device->storage;
    if (storage != NULL && store != storage->id) {
        use_primary_storage = 0;
    }

    /*
     * û�и����ض����ļ��У������ܵĲ���Ĭ�ϵ��ļ��С�����ʹ�õ��ǹ̶��б����Ĭ���ļ��С�
     * ��Ȼ����Щ�豸��Ҫ�����ļ�����ĳЩ�ļ����в����ҵ�/��ʾ���ǣ����������һ�ַ�����ʵ����һ�㡣
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
    /* ����Ƿ�֧�ַ��Ͷ��������б�*/
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

        /* Ĭ�ϵĸ�Ŀ¼ID*/
        if (localph == 0)
            localph = 0xFFFFFFFFU; // Set to -1

        /* �µĶ���ID��������Ϊ0x00000000U*/
        filedata->item_id = 0x00000000U;
        /* ��ȡ�ļ���ʽ֧�ֵ�����*/
        ret = ptp_mtp_getobjectpropssupported(params, of, &propcnt, &properties);

        for (i = 0;i < propcnt;i++) {
            PTPObjectPropDesc opd;
            /* ��ȡ������������*/
            ret = ptp_mtp_getobjectpropdesc(params, properties[i], of, &opd);
            if (ret != PTP_RC_OK) {
                add_ptp_error_to_errorstack(device, ret, "send_file_object_info(): "
                        "could not get property description.");
            } else if (opd.GetSet) {
                /* ���µ��ļ����֧�ֵ�����*/
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
                    /* ���֧�����ǵ�ǰ��ʱ��*/
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
        /* ����PTP���������б�*/
        ret = ptp_mtp_sendobjectproplist(params, &store, &localph, &filedata->item_id,
                        of, filedata->filesize, props, nrofprops);

        /* �ͷ������б�1*/
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
            /* ���Ǵ��ļ���MTP��׼�е�һ������*/
            new_file.ObjectCompressedSize = (u32) 0xFFFFFFFF;
        } else {
            new_file.ObjectCompressedSize = (u32) filedata->filesize;
        }
        new_file.ObjectFormat = of;
        new_file.StorageID = store;
        new_file.ParentObject = localph;
        new_file.ModificationDate = time(NULL);

        /* ����������Ϣ*/
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

    /* ���ø�Ŀ¼ID*/
    filedata->parent_id = localph;

    return 0;
}
/**
 * �������ɾ��MTP�豸���һ���ļ�����Ŀ�������б��ļ��л��κ���������
 *
 * ���ɾ��һ���ļ��У����޷���֤�豸������ɾ�����ļ����е������ļ�������ϣ�����ǲ��ᱻɾ������������
 * ��������Ϊ�Ǵ��ڶ���ID�Ķ����б��С���ȫ�ķ�ʽ�ǵݹ��ɾ���ļ����а����������ļ�(��Ŀ¼)��Ȼ��ɾ
 * ���ļ��б���
 *
 * @param device    ��ص�MTP�豸
 * @param object_id Ҫɾ���Ķ����ID
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
 */
int LIBMTP_Delete_Object(LIBMTP_mtpdevice_t *device, u32 object_id)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    /* PTPɾ������*/
    ret = ptp_deleteobject(params, object_id, 0);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Delete_Object(): could not delete object.");
        return -1;
    }

    return 0;
}

/**
 * ��һ�������ƶ�����һ���ط�
 * �������������һ����ʱ�䣬�ر����ڲ�ͬ�Ĵ洢�豸���ƶ�
 *
 * @param device     ��ص�MTP�豸
 * @param object_id  Ҫ�ƶ��Ķ���ID
 * @param storage_id Ŀ��λ�õĴ洢�豸ID
 * @param parent_id  Ŀ��λ�õĸ�Ŀ¼ID
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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
 * ��һ�������Ƶ���һ���ط�
 * �������������һ����ʱ�䣬�ر����ڲ�ͬ�Ĵ洢�豸���ƶ�
 *
 * @param device     ��ص�MTP�豸
 * @param object_id  Ҫ���ƵĶ���ID
 * @param storage_id Ŀ��λ�õĴ洢�豸ID
 * @param parent_id  Ŀ��λ�õĸ�Ŀ¼ID
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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
 * ����һ�������ļ�������
 *
 * @param device      ��ص�MTP�豸
 * @param object_id   Ҫ�����ֵĶ����ID
 * @param ptp_type    �����PTP�ļ�����
 * @param newname_ptr �µ��ļ���
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
 */
static int set_object_filename(LIBMTP_mtpdevice_t *device,
                               u32 object_id, u16 ptp_type,
                               const char **newname_ptr){
    PTPParams             *params = (PTPParams *) device->params;
    PTP_USB               *ptp_usb = (PTP_USB*) device->usbinfo;
    PTPObjectPropDesc     opd;
    u16              ret;
    char                  *newname;

    /* ��������Ƿ�����޸��ļ��ļ���*/
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
    /* �����µ��ļ���*/
    newname = mtp_strdup(*newname_ptr);

    if (FLAG_ONLY_7BIT_FILENAMES(ptp_usb)) {
        strip_7bit_from_utf8(newname);
    }

    if (ptp_operation_issupported(params, PTP_OC_MTP_SetObjPropList) &&
            !FLAG_BROKEN_SET_OBJECT_PROPLIST(ptp_usb)) {
        MTPProperties *props = NULL;
        MTPProperties *prop = NULL;
        int nrofprops = 0;
        /* ��ȡһ���µĶ�������*/
        prop = ptp_get_new_object_prop_entry(&props, &nrofprops);
        prop->ObjectHandle = object_id;
        prop->property = PTP_OPC_ObjectFileName;
        prop->datatype = PTP_DTC_STR;
        prop->propval.str = newname;
        /* �����µĶ�������*/
        ret = ptp_mtp_setobjectproplist(params, props, nrofprops);
        /* ɾ�������б�*/
        ptp_destroy_object_prop_list(props, nrofprops);

        if (ret != PTP_RC_OK) {
            add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "set_object_filename(): "
                    " could not set object property list.");
            ptp_free_objectpropdesc(&opd);
            return -1;
        }
    } else if (ptp_operation_issupported(params, PTP_OC_MTP_SetObjectPropValue)) {
        /* ���ö����ַ���*/
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
    /* �ͷŶ�����������*/
    ptp_free_objectpropdesc(&opd);

    /* �����µĶ�������*/
    update_metadata_cache(device, object_id);

    mtp_free(newname);

    return 0;
}


/**
 * ��������������¶�һ���ļ���������
 *
 * @param device  ��ص�MTP�豸
 * @param file    Ҫ���������ļ�Ԫ���ݽṹ��
 * @param newname �µ�����
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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
 * ��������������¶�һ���ļ��н�������
 *
 * @param device  ��ص�MTP�豸
 * @param folder  Ҫ���������ļ���Ԫ���ݽṹ��
 * @param newname �µ�����
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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
 * ���ö����ļ���
 *
 * @param device     ��ص�MTP�豸
 * @param object_id  ����id
 * @param newname    �µ�����
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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
 * ��ȡMTP�豸���һ���ļ������ݵ����ص��ļ���
 *
 * @param device   ��ص�MTP�豸
 * @param id       Ҫ��ȡ���ݵ��ļ�ID
 * @param path     �����ļ���·��
 * @param callback �ص�����
 * @param data     �ص���������
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
 */
int LIBMTP_Get_File_To_File(LIBMTP_mtpdevice_t *device, u32 const id,
                            char const * const path, LIBMTP_progressfunc_t const callback,
                            void const * const data){
    int fd = -1;
    int ret;

    /* �Ϸ��Լ��*/
    if (path == NULL) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Get_File_To_File(): Bad arguments, path was NULL.");
        return -1;
    }

    /* ���ļ�*/
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
      /* �⽫���豸���ļ���Ϣ��ȡ�����ļ���������ʶ���ļ�*/
      ret = LIBMTP_Get_File_To_File_Descriptor(device, id, fd, callback, data);
      /* �ر��ļ�*/
      mtp_close(fd);

      /* ɾ�������ļ�*/
      if (ret == -1) {
          mtp_unlink(path);
      }

      return ret;
}


/**
 * ��ȡ�ļ�����
 *
 * @param device Ҫ��ȡ�ļ������MTP�豸
 *
 * @return �ɹ�����MTP�ļ�����
 */
LIBMTP_file_t *LIBMTP_Get_Filelisting(LIBMTP_mtpdevice_t *device)
{
    LIBMTP_INFO("WARNING: LIBMTP_Get_Filelisting() is deprecated.\n");
    LIBMTP_INFO("WARNING: please update your code to use LIBMTP_Get_Filelisting_With_Callback()\n");
    return LIBMTP_Get_Filelisting_With_Callback(device, NULL, NULL);
}

/**
 *  ��ȡ�ļ�����
 *
 * @param device   Ҫ��ȡ�ļ������MTP�豸
 * @param callback �û�����Ļص�����
 * @param data     ���ݸ��ص������Ĳ���
 *
 * @return �ɹ�����LIBMTP_file_t���ݽṹ����ļ�����
 **/
LIBMTP_file_t *LIBMTP_Get_Filelisting_With_Callback(LIBMTP_mtpdevice_t *device,
                                                    LIBMTP_progressfunc_t const callback,
                                                    void const * const data){
    u32 i = 0;
    LIBMTP_file_t *retfiles = NULL;
    LIBMTP_file_t *curfile = NULL;
    PTPParams *params = (PTPParams *) device->params;

    /* ��ȡ�豸���о��*/
    if (params->nrofobjects == 0) {
        flush_handles(device);
    }
    /* �������ж���*/
    for (i = 0; i < params->nrofobjects; i++) {
        LIBMTP_file_t *file;
        PTPObject *ob;
        /* ���ûص�����*/
        if (callback != NULL)
            callback(i, params->nrofobjects, data);

        ob = &params->objects[i];
        /* Ŀ¼*/
        if (ob->oi.ObjectFormat == PTP_OFC_Association) {
            /* MTP���ļ���ʹ�ô˶����ʽ������ζ����Щ�ļ����������ļ����б���*/
            continue;
        }

        /* �Ѷ���ת��ΪMTP�ļ��ṹ��*/
        file = obj2file(device, ob);
        if (file == NULL) {
            continue;
        }

        /* ��ӵ�֮��᷵�ص�������*/
        if (retfiles == NULL) {
            retfiles = file;
            curfile = file;
        } else {
            curfile->next = file;
            curfile = file;
        }

        // Call listing callback
        // double progressPercent = (double)i*(double)100.0 / (double)params->handles.n;

    } /* �������ѭ��*/
    return retfiles;
}

/**
 * ����һ���µ��ļ��нṹ���Լ������ڴ�
 *
 * @return �ɹ����ط�����ļ��нṹ��
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
 * �ݹ�ɾ���ļ��нṹ����ڴ棬��Ӧ�����ڶ����ļ��У���������Ŀ¼��
 *
 * @param folder Ҫ���ٵ��ļ���
 */
void LIBMTP_destroy_folder_t(LIBMTP_folder_t *folder)
{
    /* �Ϸ��Լ��*/
    if(folder == NULL) {
        return;
    }

    /* ������Ŀ¼���ӵײ���ʼ����*/
    if(folder->child != NULL) {
        LIBMTP_destroy_folder_t(folder->child);
    }
    /* ����ͬ��Ŀ¼*/
    if(folder->sibling != NULL) {
        LIBMTP_destroy_folder_t(folder->sibling);
    }
    /* �ͷ��ļ��������ڴ�*/
    if(folder->name != NULL) {
        mtp_free(folder->name);
    }

    mtp_free(folder);
}

/**
 * Function used to recursively get subfolders from params.
 * �ݹ��ȡĿ¼����Ŀ¼
 *
 * @param list   Ŀ¼����
 * @param parent ��Ŀ¼ID
 *
 * @return
 */
static LIBMTP_folder_t *get_subfolders_for_folder(LIBMTP_folder_t *list, uint32_t parent)
{
    LIBMTP_folder_t *retfolders = NULL;
    LIBMTP_folder_t *children, *iter, *curr;

    /* ��ȡͬ��Ŀ¼*/
    iter = list->sibling;
    while(iter != list) {
        /* ���Ŀ¼�ĸ�Ŀ¼ID�Ƿ����*/
        if (iter->parent_id != parent) {
            iter = iter->sibling;
            continue;
        }

        /* ����֪�� iter�ǡ�parent������Ŀ¼��������ǿ����ڵ�ǰ��ȫ�ر���'iter'����Ϊ�����ǵݹ�ʱ��
         * û�������˻��'list'����ȡ��*/
        /* ��ȡ��Ŀ¼*/
        children = get_subfolders_for_folder(list, iter->folder_id);

        curr = iter;
        iter = iter->sibling;

        /* ���������Ƴ���ǰĿ¼*/
        curr->child->sibling = curr->sibling;
        curr->sibling->child = curr->child;
        /* ����Ŀ¼���ӵ���ǰĿ¼*/
        curr->child = children;
        /* �ѵ�ǰĿ¼�ŵ�ͬ��Ŀ¼����*/
        curr->sibling = retfolders;
        retfolders = curr;
    }
    /* ���ص�ǰĿ¼�ṹ��*/
    return retfolders;
}

/**
 * �ڵ�ǰ��MTP�豸�Ϸ���һ�����пɵõ��ļ��е�����
 *
 * @param device  Ҫ��ȡ�ļ��е�MTP�豸
 * @param storage Ҫ��ȡ�ļ�����Ĵ洢�豸��ID
 *
 * @return �ɹ�����һ���ļ��е�����
 */
LIBMTP_folder_t *LIBMTP_Get_Folder_List_For_Storage(LIBMTP_mtpdevice_t *device,
                            u32 const storage){
    PTPParams *params = (PTPParams *) device->params;
    LIBMTP_folder_t head, *rv;
    int i;

    /* �����û�л�ȡ���еľ������ˢ�»�ȡ�豸�ľ��*/
    if (params->nrofobjects == 0) {
        flush_handles(device);
    }

    /* �⽫�����ļ��е���ʱ�������෴��˳�����У���ʹ���ļ��нṹ�����е��ļ���ָ�롣
     * Ȼ�����ǿ���ͨ���鿴�����ʱ�����������ļ��еĲ�νṹ�����������Ǵ������ʱ������ɾ��
     * �ļ��С�����Ȼ�������ڹ����ļ��в�νṹʱ����ִ�еĲ����������⣬������ʱ�����˳����
     * �෴�ģ����Ե�������ͬ������ǰ��ʱ�������˳��������ھ�������е�˳������ͬ��*/
    head.sibling = &head;
    head.child = &head;
    for (i = 0; i < params->nrofobjects; i++) {
        LIBMTP_folder_t *folder;
        PTPObject *ob;

        ob = &params->objects[i];
        /* Ѱ���ļ���*/
        if (ob->oi.ObjectFormat != PTP_OFC_Association) {
            continue;
        }
        /* ������Ǳ������еĴ洢�豸��������Ĵ洢�豸�ǲ��Ǻ�������Ҫʹ�õĴ洢�豸��һ����*/
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

        /* ����һ���ļ���*/
        folder = LIBMTP_new_folder_t();
        if (folder == NULL) {
            /* �����ڴ�ʧ��*/
            return NULL;
        }
        /* �ļ���ID(����ID)*/
        folder->folder_id = ob->oid;
        /* �ļ��и�ID(����ĸ�����)*/
        folder->parent_id = ob->oi.ParentObject;
        /* ��صĴ洢�豸��ID*/
        folder->storage_id = ob->oi.StorageID;
        /* �ļ�������*/
        folder->name = (ob->oi.Filename) ? (char *)mtp_strdup(ob->oi.Filename) : NULL;

        /* ��Ŀ¼��ָ��head��siblingָ����һ��folder*/
        folder->sibling = head.sibling;
        folder->child = &head;
        head.sibling->child = folder;
        head.sibling = folder;
    }

    /* ���ǴӸ����ĸ��ļ��п�ʼ���ݹ�Ļ�ȡ����*/
    rv = get_subfolders_for_folder(&head, 0x00000000U);

    /* ��һЩ�豸�����ڡ����ļ��С�0xffffffff����һЩ�ļ���������0x00000000û�з����κ��ļ��У�
     * ����Ҹ��ļ���0xffffffffU����Ŀ¼*/
    if (rv == NULL) {
        rv = get_subfolders_for_folder(&head, 0xffffffffU);
        if (rv != NULL)
            LIBMTP_ERROR("Device have files in \"root folder\" 0xffffffffU - "
                         "this is a firmware bug (but continuing)\n");
    }

    /* ��ʱ������ҪΪ�յģ��Է���һ������κ�ʣ��Ĺ¶�����*/
    while(head.sibling != &head) {
        LIBMTP_folder_t *curr = head.sibling;

        LIBMTP_INFO("Orphan folder with ID: 0x%08x name: \"%s\" encountered.\n",
                     curr->folder_id, curr->name);
        curr->sibling->child = curr->child;
        curr->child->sibling = curr->sibling;
        curr->child = NULL;
        curr->sibling = NULL;
        /* �����ļ���*/
        LIBMTP_destroy_folder_t(curr);
    }
    return rv;
}

/**
 * ���ص�ǰMTP�豸�����пɵ�Ŀ¼����
 *
 * @param device Ҫ��ȡĿ¼�����MTP�豸
 *
 * @return �ɹ����ػ�ȡ����Ŀ¼����
 **/
LIBMTP_folder_t *LIBMTP_Get_Folder_List(LIBMTP_mtpdevice_t *device)
{
    return LIBMTP_Get_Folder_List_For_Storage(device, PTP_GOH_ALL_STORAGE);
}

/**
 * �ڵ�ǰ��MTP�豸�ϴ���һ���µ��ļ���
 * PTP���ļ���������"association"���豸�ڲ�û�С��ļ��С��ĸ��������
 * �����ļ���ƽ���б���Щ�ļ���"associations"�������ļ����ļ��п��Խ���
 * ��Ϊ"����"��
 *
 * @param device      MTP�豸
 * @param name        Ҫ�������ļ��е�����
 * @param parent_id   ��Ŀ¼ID
 * @param storage_id  ��ǰ�洢�豸ID
 *
 * @return �ɹ������´������ļ��е�ID��ʧ�ܷ���0
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

    /* û��ָ���洢�豸ID*/
    if (storage_id == 0) {
        /* ����²�һ���ļ�����Ҫ512�ֽ�*/
        /* ��ȡһ�����õĴ洢�豸*/
        store = get_suggested_storage_id(device, 512, parent_id);
    } else {
        store = storage_id;
    }
    parenthandle = parent_id;

    memset(&new_folder, 0, sizeof(new_folder));
    new_folder.Filename = name;
    /* ȥ���ļ�����>0x7F���ַ�*/
    if (FLAG_ONLY_7BIT_FILENAMES(ptp_usb)) {
        strip_7bit_from_utf8(new_folder.Filename);
    }
    new_folder.ObjectCompressedSize = 0;
    new_folder.ObjectFormat = PTP_OFC_Association;
    new_folder.ProtectionStatus = PTP_PS_NoProtection;
    new_folder.AssociationType = PTP_AT_GenericFolder;
    new_folder.ParentObject = parent_id;
    new_folder.StorageID = store;

    /* ������������*/
    if (!(params->device_flags & DEVICE_FLAG_BROKEN_SEND_OBJECT_PROPLIST) &&
            ptp_operation_issupported(params, PTP_OC_MTP_SendObjectPropList)) {
        MTPProperties *props = (MTPProperties*)mtp_calloc(2, sizeof(MTPProperties));

        props[0].property = PTP_OPC_ObjectFileName;
        props[0].datatype = PTP_DTC_STR;
        props[0].propval.str = name;

        props[1].property = PTP_OPC_Name;
        props[1].datatype = PTP_DTC_STR;
        props[1].propval.str = name;
        /* �����µĶ�������*/
        ret = ptp_mtp_sendobjectproplist(params, &store, &parenthandle, &new_id, PTP_OFC_Association,
                                         0, props, 1);
        mtp_free(props);
    } else {
        /* ���Ͷ�����Ϣ*/
        ret = ptp_sendobjectinfo(params, &store, &parenthandle, &new_id, &new_folder);
    }

    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Create_Folder: Could not send object info.");
        if (ret == PTP_RC_AccessDenied) {
            add_ptp_error_to_errorstack(device, ret, "ACCESS DENIED.");
        }
        return 0;
    }
    /* ��Ҫ�����µ��ļ��ж�����Ϣ����Ϊ����̬�����ü����ַ���*/

    /* ��Ӷ���*/
    add_object_to_cache(device, new_id);

    return new_id;
}

/**
 * ����һ���µĲ����б�Ԫ����
 *
 * @return �ɹ�����һ���·����Ԫ���ݽṹ��
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
 * ����һ�������б�Ԫ���ݽṹ��
 *
 * @param playlist Ҫ���ٵĲ����б�Ԫ����
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
 * ������������豸һ�������Ĳ����б�
 *
 * @param device Ҫ��ȡ�����б��MTP�豸
 * @param plid   Ҫ�����Ĳ����б��ID
 *
 * @return �ɹ�����һ����Ч�Ĳ����б�Ԫ����
 */
LIBMTP_playlist_t *LIBMTP_Get_Playlist(LIBMTP_mtpdevice_t *device, u32 const plid)
{
    PTP_USB *ptp_usb = (PTP_USB*) device->usbinfo;
    const int REQ_SPL = FLAG_PLAYLIST_SPL(ptp_usb);
    PTPParams *params = (PTPParams *) device->params;
    PTPObject *ob;
    LIBMTP_playlist_t *pl;
    u16 ret;

    /* ��ȡ���о��*/
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
//        /* ����һ���µĲ����б�����*/
//        pl = LIBMTP_new_playlist_t();
//        spl_to_playlist_t(device, &ob->oi, ob->oid, pl);
//        return pl;
//    }

    /* ����һ�������б�����*/
    else if ( ob->oi.ObjectFormat != PTP_OFC_MTP_AbstractAudioVideoPlaylist ) {
        return NULL;
    }

    /* ����һ���µĲ����б�����*/
    pl = LIBMTP_new_playlist_t();

    pl->name = get_string_from_object(device, ob->oid, PTP_OPC_Name);
    if (pl->name == NULL) {
        pl->name = mtp_strdup(ob->oi.Filename);
    }
    pl->playlist_id = ob->oid;
    pl->parent_id = ob->oi.ParentObject;
    pl->storage_id = ob->oi.StorageID;

    /* ��ȡ��������б����Ŀ�б�*/
    ret = ptp_mtp_getobjectreferences(params, pl->playlist_id, &pl->tracks, &pl->no_tracks);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Get_Playlist(): Could not get object references.");
        pl->tracks = NULL;
        pl->no_tracks = 0;
    }

    return pl;
}

/**
 * ����һ�������б��ר�����µ�ժҪ�б�
 *
 * @param device       Ҫ�����µ�ժҪ�б��MTP�豸
 * @param name         �µ�ժҪ�б������
 * @param genre        �µ�ժҪ�б�����ͻ�ΪNULL
 * @param parenthandle ���ļ��е�ID��0�������ļ���
 * @param objectformat Ҫ�������µ�ժҪ�б������
 * @param suffix       ���ڴ˲������������⡰�ļ����ġ�.foo��(4�ֽ�)��׺
 * @param newid        �¶����ID
 * @param tracks       ����������Ŀ����
 * @param no_tracks    ��Ŀ������
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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

    /* ����µ�ժҪ�б�����*/
    if (!name) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): list name was NULL, using default name \"Unknown\"");
        return -1;
    }

    if (storageid == 0) {
        /* �ȼ���һ��ժҪ�б�Ĵ�СҪ512�ֽ�*/
        store = get_suggested_storage_id(device, 512, localph);
    } else {
        store = storageid;
    }

    /* ����Ƿ�֧�ֶ����ʽ*/
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
    /* ����µ�����*/
    fname[0] = '\0';
    if (strlen(name) > strlen(suffix)) {
        char const * const suff = &name[strlen(name) - strlen(suffix)];
        if (!strcmp(suff, suffix)) {
            strncpy(fname, name, sizeof(fname));
        }
    }
    /* �����ֺ�������µĺ�׺��*/
    if (fname[0] == '\0') {
        strncpy(fname, name, sizeof(fname)-strlen(suffix)-1);
        strcat(fname, suffix);
        fname[sizeof(fname)-1] = '\0';
    }
    /* ���ö�����������*/
    if (ptp_operation_issupported(params, PTP_OC_MTP_SendObjectPropList) &&
            !FLAG_BROKEN_SEND_OBJECT_PROPLIST(ptp_usb)) {
        MTPProperties *props = NULL;
        MTPProperties *prop = NULL;
        int nrofprops = 0;

        *newid = 0x00000000U;
        /* ��ȡ֧�ֵĶ�������*/
        ret = ptp_mtp_getobjectpropssupported(params, objectformat, &propcnt, &properties);

        for (i  =0;i < propcnt;i++) {
            PTPObjectPropDesc opd;
            /* ��ȡ������������*/
            ret = ptp_mtp_getobjectpropdesc(params, properties[i], objectformat, &opd);
            if (ret != PTP_RC_OK) {
                add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "create_new_abstract_list(): "
                        "could not get property description.");
            } else if (opd.GetSet) {
                /* �����µĶ�������*/
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
                    /* ���֧�֣����ǵ�ǰ��ʱ��*/
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
        /* �����µ���������*/
        ret = ptp_mtp_sendobjectproplist(params, &store, &localph, newid,
                                         objectformat, 0, props, nrofprops);

        /* �ͷ���������*/
        ptp_destroy_object_prop_list(props, nrofprops);

        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "create_new_abstract_list(): Could not send object property list.");
            if (ret == PTP_RC_AccessDenied) {
                add_ptp_error_to_errorstack(device, ret, "ACCESS DENIED.");
            }
            return -1;
        }

        /* ���Ϳհ׶���*/
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
        /* ��ĳ������ϣ��������1*/
        new_object.ObjectCompressedSize = 0;
        new_object.ObjectFormat = objectformat;

        /* ����һ���µĶ�����Ϣ*/
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
        /* һ������������*/
        for (i = 0;i < propcnt;i++) {
            PTPObjectPropDesc opd;
            /* ��ȡ��������*/
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
        /* ����Ŀ��Ϊ����������ӵ�������*/
        ret = ptp_mtp_setobjectreferences (params, *newid, (u32 *) tracks, no_tracks);
        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "create_new_abstract_list(): could not add tracks as object references.");
            return -1;
        }
    }
    /* �Ѷ�����ӵ�������*/
    add_object_to_cache(device, *newid);

    return 0;
}

/**
 * ��������һ���µ�ר��Ԫ���ݽṹ��
 *
 * @return �ɹ������´�����ר��Ԫ���ݽṹ��
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
 * ɾ��ר���ṹ����ڴ�
 *
 * @param album Ҫ���ٵ�ר���ṹ��
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
 * �������������ӳ�䲢���Ƶ����Ԫ����(�������)
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
 * ����һ��ר����Ԫ����
 *
 * @param device  MTP�豸
 * @param alb     Ҫ���ص�ר����Ԫ����
 */
static void get_album_metadata(LIBMTP_mtpdevice_t *device,
                               LIBMTP_album_t *alb)
{
    u16 ret;
    PTPParams *params = (PTPParams *) device->params;
    u32 i;
    MTPProperties *prop;
    PTPObject *ob;

    /* ��������л������Ԫ���ݼ���ʹ����*/
    ret = ptp_object_want(params, alb->album_id, PTPOBJECT_MTPPROPLIST_LOADED, &ob);
    if (ob->mtpprops) {
        prop = ob->mtpprops;
        for (i = 0;i < ob->nrofmtpprops; i++ ,prop++)
            pick_property_to_album_metadata(device, prop, alb);
    } else {
        u16 *props = NULL;
        u32 propcnt = 0;

        /* �������ר����ʲô����*/
        ret = ptp_mtp_getobjectpropssupported(params, PTP_OFC_MTP_AbstractAudioAlbum, &propcnt, &props);
        if (ret != PTP_RC_OK) {
            add_ptp_error_to_errorstack(device, ret, "get_album_metadata(): call to ptp_mtp_getobjectpropssupported() failed.");
            return;
        } else {
            /* ��ȡר����Ϣ*/
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
 * �����豸��һ��������ר��
 *
 * @param device Ҫ��ȡר����MTP�豸
 * @param albid  Ҫ��ȡר����ID
 *
 * @return �ɹ�����һ����Ч��ר��Ԫ���ݣ�ʧ�ܷ���NULL
 */
LIBMTP_album_t *LIBMTP_Get_Album(LIBMTP_mtpdevice_t *device, u32 const albid)
{
    PTPParams *params = (PTPParams *) device->params;
    u16 ret;
    PTPObject *ob;
    LIBMTP_album_t *alb;

    /* ��ȡ���о��*/
    if (params->nrofobjects == 0)
        flush_handles(device);

    ret = ptp_object_want(params, albid, PTPOBJECT_OBJECTINFO_LOADED, &ob);
    if (ret != PTP_RC_OK)
        return NULL;

    /* ����ǲ���һ��ר��*/
    if (ob->oi.ObjectFormat != PTP_OFC_MTP_AbstractAudioAlbum)
        return NULL;

    /* ����һ���µ�ר������*/
    alb = LIBMTP_new_album_t();
    alb->album_id = ob->oid;
    alb->parent_id = ob->oi.ParentObject;
    alb->storage_id = ob->oi.StorageID;

    /* ��ȡ֧�ֵ�Ԫ����*/
    get_album_metadata(device, alb);

    /* ��ȡ���ר������Ŀ����*/
    ret = ptp_mtp_getobjectreferences(params, alb->album_id, &alb->tracks, &alb->no_tracks);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Get_Album: Could not get object references.");
        alb->tracks = NULL;
        alb->no_tracks = 0;
    }

    return alb;
}

/**
 * �����ṩ��ר��Ԫ���ݴ���һ���µ�ר��
 *
 * @param device   ��ص�MTP�豸
 * @param metadata Ҫ��������ר��Ԫ����
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
 */
int LIBMTP_Create_New_Album(LIBMTP_mtpdevice_t *device, LIBMTP_album_t * const metadata)
{
    u32 localph = metadata->parent_id;

    /* ���û����ʹ��Ĭ�ϵ��ļ���*/
    if (localph == 0) {
        if (device->default_album_folder != 0)
            localph = device->default_album_folder;
        else
            localph = device->default_music_folder;
    }
    metadata->parent_id = localph;

    /* �����µ�ժҪ�б�*/
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
 * �����������һ������Ĵ������������ݡ�����Ӧ���ǲ������ɽ��ܵĸ�ʽ(iRiver��Creative���ƺ���JPEG)��
 * �Ͳ��ܹ���(Creative�����20KB)��
 *
 * @param device     ��ص�MTP�豸
 * @param id         Ҫ���õĶ���ID
 * @param sampledata Ҫ���͵���������
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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

    /* ��ȡҪΪ�䷢�ʹ��������ݵĶ�����ļ���ʽ*/
    ret = ptp_object_want (params, id, PTPOBJECT_OBJECTINFO_LOADED, &ob);
    if (ret != PTP_RC_OK) {
      add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Send_Representative_Sample(): could not get object info.");
      return -1;
    }

    /* ��������ܷ��͵Ĵ������������ݵĶ����ʽ*/
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

    /* ������������*/
    propval.a.count = sampledata->size;
    propval.a.v = mtp_malloc(sizeof(PTPPropertyValue) * sampledata->size);
    for (i = 0; i < sampledata->size; i++) {
        propval.a.v[i].u8 = sampledata->data[i];
    }
    /* ��ȡ��������ֵ*/
    ret = ptp_mtp_setobjectpropvalue(params,id,PTP_OPC_RepresentativeSampleData,
                                     &propval,PTP_DTC_AUINT8);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Send_Representative_Sample(): could not send sample data.");
        mtp_free(propval.a.v);
        return -1;
    }
    mtp_free(propval.a.v);

    /* ���������ͼƬ�����ÿ�͸ߣ������ľ����ó���ʱ������ݴ�С*/
    switch(sampledata->filetype) {
    case LIBMTP_FILETYPE_JPEG:
    case LIBMTP_FILETYPE_JFIF:
    case LIBMTP_FILETYPE_TIFF:
    case LIBMTP_FILETYPE_BMP:
    case LIBMTP_FILETYPE_GIF:
    case LIBMTP_FILETYPE_PICT:
    case LIBMTP_FILETYPE_PNG:
        if (!FLAG_BROKEN_SET_SAMPLE_DIMENSIONS(ptp_usb)) {
            /* ͼ�����ÿ�͸�*/
            set_object_u32(device, id, PTP_OPC_RepresentativeSampleHeight, sampledata->height);
            set_object_u32(device, id, PTP_OPC_RepresentativeSampleWidth, sampledata->width);
        }
        break;
    default:
        /* ����ͼ�����ó���ʱ������ݴ�С*/
        set_object_u32(device, id, PTP_OPC_RepresentativeSampleDuration, sampledata->duration);
        set_object_u32(device, id, PTP_OPC_RepresentativeSampleSize, sampledata->size);
        break;
    }

    return 0;
}

/**
 * ��ȡ����Ĵ������������ݣ�����豸֧�֣�����ʹ��ר���Ĵ�����������������
 *
 * @param device  MTP�豸
 * @param id      Ҫ��ȡ���ݵĶ����ID
 * @param pointer Ҫ��ȡ���ݵ�LIBMTP_filesampledata_t�ṹ��
 *
 * @return �ɹ�����0��ʧ�ܷ�������ֵ
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

    /* ��ȡҪΪ�䷢�ʹ��������ݵĶ�����ļ���ʽ*/
    ret = ptp_object_want (params, id, PTPOBJECT_OBJECTINFO_LOADED, &ob);
    if (ret != PTP_RC_OK) {
        add_error_to_errorstack(device, LIBMTP_ERROR_GENERAL, "LIBMTP_Get_Representative_Sample(): could not get object info.");
        return -1;
    }

    /* ��������Ƿ���Դ洢��������ʽ�Ĵ���������������*/
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

    /* ��ȡ����*/
    ret = ptp_mtp_getobjectpropvalue(params, id, PTP_OPC_RepresentativeSampleData,
                   &propval, PTP_DTC_AUINT8);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "LIBMTP_Get_Representative_Sample(): could not get sample data.");
        return -1;
    }

    /* ��������*/
    sampledata->size = propval.a.count;
    sampledata->data = mtp_malloc(sizeof(PTPPropertyValue) * propval.a.count);
    for (i = 0; i < propval.a.count; i++) {
        sampledata->data[i] = propval.a.v[i].u8;
    }
    mtp_free(propval.a.v);

    /* ��ȡ��������*/
    sampledata->width = get_u32_from_object(device, id, PTP_OPC_RepresentativeSampleWidth, 0);
    sampledata->height = get_u32_from_object(device, id, PTP_OPC_RepresentativeSampleHeight, 0);
    sampledata->duration = get_u32_from_object(device, id, PTP_OPC_RepresentativeSampleDuration, 0);
    sampledata->filetype = map_ptp_type_to_libmtp_type(
            get_u16_from_object(device, id, PTP_OPC_RepresentativeSampleFormat, LIBMTP_FILETYPE_UNKNOWN));

    return 0;
}

/**
 * ��������һ���µ���������Ԫ���ݽṹ��
 *
 * @return �ɹ������´������������ݽṹ���ָ��
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
 * �����ļ���������
 *
 * @param sample Ҫ���ٵ��ļ�����Ԫ����
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
 * ���һ�����󵽻���
 *
 * @param device    Ҫ��ӻ����MTP�豸
 * @param object_id Ҫ��ӵ�����Ķ���ID
 **/
static void add_object_to_cache(LIBMTP_mtpdevice_t *device, u32 object_id)
{
    PTPParams *params = (PTPParams *)device->params;
    u16 ret;

    /* ��Ӷ���*/
    ret = ptp_add_object_to_cache(params, object_id);
    if (ret != PTP_RC_OK) {
        add_ptp_error_to_errorstack(device, ret, "add_object_to_cache(): couldn't add object to cache");
    }
}

/**
 * �����޸ĺ���»���
 *
 * @param device    Ҫ���»����MTP�豸
 * @param object_id Ҫ���»���Ķ���ID
 */
static void update_metadata_cache(LIBMTP_mtpdevice_t *device, u32 object_id)
{
    PTPParams *params = (PTPParams *)device->params;
    /* �ӻ�����ɾ��ԭ���Ķ���*/
    ptp_remove_object_from_cache(params, object_id);
    /* ����Ӷ���*/
    add_object_to_cache(device, object_id);
}
