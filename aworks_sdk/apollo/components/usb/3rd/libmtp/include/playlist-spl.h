#ifndef __MTP__PLAYLIST_SPL__H
#define __MTP__PLAYLIST_SPL__H

int is_spl_playlist(PTPObjectInfo *oi);

void spl_to_playlist_t(LIBMTP_mtpdevice_t* device, PTPObjectInfo *oi,
                       const u32 id, LIBMTP_playlist_t * const pl);
int playlist_t_to_spl(LIBMTP_mtpdevice_t *device,
                      LIBMTP_playlist_t * const metadata);
int update_spl_playlist(LIBMTP_mtpdevice_t *device,
                        LIBMTP_playlist_t * const newlist);

#endif
