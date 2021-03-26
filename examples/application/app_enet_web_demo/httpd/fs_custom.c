#include "fs.h"
#include "fsdata.h"

unsigned char data_custom_file_index[CUSTOM_FILE_SIZE_INDEX + sizeof CUSTOM_FILE_NAME_INDEX] = {
	CUSTOM_FILE_NAME_INDEX
};

struct fsdata_file custom_file_index[] = {
	{
		NULL,
		data_custom_file_index,
		data_custom_file_index + sizeof CUSTOM_FILE_NAME_INDEX,
		sizeof data_custom_file_index - sizeof CUSTOM_FILE_NAME_INDEX,
	}
};

#define CUSTOM_FS_ROOT        custom_file_index

const static uint16_t g_max_file_size[] = {
	CUSTOM_FILE_SIZE_INDEX,
};

int fs_open_custom (struct fs_file *file, const char *name)
{
    const struct fsdata_file *f;
    int i = 0;

    if ((file == NULL) || (name == NULL)) {
        return -1;
    }

    for (f = CUSTOM_FS_ROOT; f != NULL; f = f->next, i++) {
        if (!strcmp(name, (char *) f->name)) {
            file->data = (const char *) f->data;
            file->len = f->len;
            file->maxlen = g_max_file_size[i];
            file->index = f->len;
            file->pextension = NULL;
            file->http_header_included = f->http_header_included;
#if HTTPD_PRECALCULATED_CHECKSUM
            file->chksum_count = f->chksum_count;
            file->chksum = f->chksum;
#endif /* HTTPD_PRECALCULATED_CHECKSUM */
#if LWIP_HTTPD_FILE_STATE
            file->state = fs_state_init(file, name);
#endif /* #if LWIP_HTTPD_FILE_STATE */
            return 1;
        }
    }
    /* file not found */
    return 0;
}

void fs_close_custom (struct fs_file *file)
{
}

void* fs_write_custom (struct fs_file *file, const unsigned char* buffer, size_t count)
{
    int i = 0;
    const struct fsdata_file *f;

    for (f = CUSTOM_FS_ROOT; f != NULL; f = f->next, i++) {
        if ((const void*) f->data == file->data) { /* FIXME: such a dirty method */
            if (g_max_file_size[i] >= count) {
                struct fsdata_file *dirty = (struct fsdata_file *) f;
                dirty->len = count;
                return MEMCPY((void* )file->data, buffer, count);
            }
        }
    }
    return NULL;
}

void* fs_setlen_custom (struct fs_file *file, size_t length)
{
    int i = 0;
    const struct fsdata_file *f;

    for (f = CUSTOM_FS_ROOT; f != NULL; f = f->next, i++) {
        if ((const void*) f->data == file->data) { /* FIXME: such a dirty method */
            if (g_max_file_size[i] >= length) {
                struct fsdata_file *dirty = (struct fsdata_file *) f;
                dirty->len = length;
                file->len = length;
                LWIP_DEBUGF(HTTPD_FS_DEBUG | LWIP_DBG_TRACE, ("fs_setlen_custom: %d\n", length));
                return (void*) dirty;
            }
        }
    }
    return NULL;
}
