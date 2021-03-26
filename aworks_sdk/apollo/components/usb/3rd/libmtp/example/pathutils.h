#ifndef PATHUTILS_H_INCLUSION_GUARD
#define PATHUTILS_H_INCLUSION_GUARD

int parse_path (char *, LIBMTP_file_t *, LIBMTP_folder_t *);
LIBMTP_filetype_t find_filetype (char *);
int progress (const u64, const u64, void const * const);
char *mtp_basename(char *in);
char *mtp_dirname(char *in);
char *mtp_strdup(const char *s);
char *mtp_strndup(const char *s, int num);
char *mtp_rindex(char *s, char c);
#endif


