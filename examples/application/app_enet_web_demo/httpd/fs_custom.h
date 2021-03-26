#ifndef __FS_CUSTOM_H__
#define __FS_CUSTOM_H__

#include "lwip/opt.h"
#include "lwip/def.h"
#include "httpd/fsdata.h"
#include <string.h>

#define LWIP_HTTPD_CUSTOM_FILES 1

#if LWIP_HTTPD_CUSTOM_FILES

#define CUSTOM_FILE_SIZE_INDEX      2048

#define CUSTOM_FILE_NAME_INDEX      "index.txt"

extern struct fsdata_file custom_file_index[];

#endif /* LWIP_HTTPD_CUSTOM_FILES */

#endif /* __FS_CUSTOM_H__ */
