//#include "config.h"
#include "mtp_adapter.h"

#include <stdio.h>
#include <stdlib.h> // mkstmp()
#include <unistd.h>
#include <errno.h>
#ifndef AWORKS
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif
#include <string.h>

#include "libmtp.h"
#include "libusb-glue.h"
#include "ptp.h"
#include "unicode.h"
#include "util.h"

#include "playlist-spl.h"

/**
 * Debug macro
 */
#define LIBMTP_PLST_DEBUG(format, args...) \
  do { \
    if ((LIBMTP_debug & LIBMTP_DEBUG_PLST) != 0) \
      fprintf(stdout, "LIBMTP %s[%d]: " format, __FUNCTION__, __LINE__, ##args); \
  } while (0)


// Internal singly linked list of strings
// used to hold .spl playlist in memory
typedef struct text_struct {
    char* text;                // String
    struct text_struct *next; // Link to next line, NULL if end of list
} text_t;
/**
 * 检查对象是不是一个.spl播放列表
 *
 * @param oi 对象信息
 *
 * @return 是三星.spl对象返回1，不是返回0
 */
int is_spl_playlist(PTPObjectInfo *oi)
{
    return ((oi->ObjectFormat == PTP_OFC_Undefined) ||
            (oi->ObjectFormat == PTP_OFC_MTP_SamsungPlaylist)) &&
            (strlen(oi->Filename) > 4) &&
            (strcmp((oi->Filename + strlen(oi->Filename) - 4), ".spl") == 0);
}

///**
// * Take an object ID, a .spl playlist on the MTP device,
// * and convert it to a playlist_t object.
// *
// * @param device mtp device pointer
// * @param oi object we are reading
// * @param id .spl playlist id on MTP device
// * @param pl the LIBMTP_playlist_t pointer to be filled with info from id
// */
//
//void spl_to_playlist_t(LIBMTP_mtpdevice_t* device, PTPObjectInfo *oi,
//                       const u32 id, LIBMTP_playlist_t * const pl)
//{
//    // Fill in playlist metadata
//    // Use the Filename as the playlist name, dropping the ".spl" extension
//    pl->name = mtp_malloc(sizeof(char)*(strlen(oi->Filename) -4 +1));
//    memcpy(pl->name, oi->Filename, strlen(oi->Filename) -4);
//    // Set terminating character
//    pl->name[strlen(oi->Filename) - 4] = 0;
//    pl->playlist_id = id;
//    pl->parent_id = oi->ParentObject;
//    pl->storage_id = oi->StorageID;
//    pl->tracks = NULL;
//    pl->no_tracks = 0;
//
//    LIBMTP_PLST_DEBUG("pl->name='%s'\n", pl->name);
//
//    // open a temporary file
//    char tmpname[] = "/tmp/mtp-spl2pl-XXXXXX";
//    int fd = mkstemp(tmpname);
//    if(fd < 0) {
//        LIBMTP_ERROR("failed to make temp file for %s.spl -> %s, errno=%s\n", pl->name, tmpname, strerror(errno));
//        return;
//    }
//    // make sure the file will be deleted afterwards
//    if(unlink(tmpname) < 0)
//        LIBMTP_ERROR("failed to delete temp file for %s.spl -> %s, errno=%s\n", pl->name, tmpname, strerror(errno));
//    int ret = LIBMTP_Get_File_To_File_Descriptor(device, pl->playlist_id, fd, NULL, NULL);
//    if( ret < 0 ) {
//        // FIXME     add_ptp_error_to_errorstack(device, ret, "LIBMTP_Get_Playlist: Could not get .spl playlist file.");
//        close(fd);
//        LIBMTP_INFO("FIXME closed\n");
//    }
//
//    text_t* p = read_into_spl_text_t(device, fd);
//    close(fd);
//
//    // FIXME cache these somewhere else so we don't keep calling this!
//    LIBMTP_folder_t *folders;
//    LIBMTP_file_t *files;
//    folders = LIBMTP_Get_Folder_List(device);
//    files = LIBMTP_Get_Filelisting_With_Callback(device, NULL, NULL);
//
//    /* 转换播放列表为曲目id*/
//    pl->no_tracks = trackno_spl_text_t(p);
//    LIBMTP_PLST_DEBUG("%u track%s found\n", pl->no_tracks, pl->no_tracks==1?"":"s");
//    pl->tracks = mtp_malloc(sizeof(u32)*(pl->no_tracks));
//    tracks_from_spl_text_t(p, pl->tracks, folders, files);
//
//    free_spl_text_t(p);
//
//    // debug: add a break since this is the top level function call
//    LIBMTP_PLST_DEBUG("------------\n\n");
//}
