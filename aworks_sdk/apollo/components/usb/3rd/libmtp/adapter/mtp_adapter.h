#ifndef __MTP_ADAPTER_H__
#define __MTP_ADAPTER_H__
#include "stdio.h"
#include "apollo.h"
#include "aw_vdebug.h"
#include "libmtp.h"
#include "host/awbl_usbh.h"
#include "fs/aw_blk_dev.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"

#define AWORKS

typedef int MTP_FILE;

#define mtp_printf aw_kprintf


#define mtp_usb_device  awbl_usbh_device

#define LIBMTP_INFO(format, args...) \
    do { \
        if (LIBMTP_debug != 0) \
            aw_kprintf("LIBMTP: " format,  ##args); \
        else \
            aw_kprintf(format, ##args); \
  } while (0)

#define LIBMTP_ERROR(format, args...) \
    do { \
        if (LIBMTP_debug != 0) \
            aw_kprintf("LIBMTP: " format,  ##args); \
        else \
            aw_kprintf(format, ##args); \
  } while (0)

#define LIBMTP_USB_DEBUG_OFF(format, args...) \
    do { \
    } while (0)

#define LIBMTP_USB_DEBUG(format, args...) \
     do { \
         if ((LIBMTP_debug & LIBMTP_DEBUG_USB) != 0) \
         aw_kprintf("LIBMTP: " format, ##args); \
     } while (0)

#define LIBMTP_USB_DATA(buffer, length, base) \
     do { \
     } while (0)


/************************º¯ÊýÉùÃ÷**************************/
void *mtp_malloc(unsigned int size);
void mtp_free(void *ptr);
void *mtp_realloc(void *ptr, size_t newsize);
void *mtp_calloc(size_t nelem, size_t size);
char *mtp_get_home_path(void);
void mtp_usleep(uint32_t us);
MTP_FILE mtp_fopen(const char *filename, const char *mode);
char *mtp_fgets(char *str, int n, MTP_FILE stream);
int mtp_fclose(MTP_FILE fp);
int mtp_open (const char *path, int oflag, mode_t mode);
int mtp_close (int index);
ssize_t mtp_read (int index, void *p_buf, size_t nbyte);
ssize_t mtp_write (int index, void *p_buf, size_t nbyte);
int mtp_unlink (const char *path);
int mtp_lseek (int fildes, int offset, int whence);
#endif
