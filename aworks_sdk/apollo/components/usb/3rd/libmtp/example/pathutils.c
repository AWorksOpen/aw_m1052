#include "common.h"
#include "pathutils.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "mtp_adapter.h"

/**
 * Ѱ�����ֶ�Ӧ��Ŀ¼ID
 *
 * @param folder Ŀ¼����
 * @param path   ҪѰ�ҵ�·������
 * @param parent ��Ŀ¼·��
 *
 * @return �ɹ������ҵ���Ŀ¼ID
 **/
static u32 lookup_folder_id (LIBMTP_folder_t * folder, char * path, char * parent)
{
    char * current;
    u32 ret = (u32) -1;

    /* ��Ŀ¼*/
    if (strcmp(path,"/")==0)
        return 0;
    /* ���Ŀ¼����*/
    if (folder == NULL) {
        return ret;
    }

    current = mtp_malloc (strlen(parent) + strlen(folder->name) + 2);
    sprintf(current,"%s/%s",parent,folder->name);
    /* ���Դ�Сд���Ƚ��ַ���*/
    if (strcasecmp (path, current) == 0) {
        mtp_free (current);
        return folder->folder_id;
    }
    /* ����Ŀ¼*/
    if (strncasecmp (path, current, strlen (current)) == 0) {
        /* Ѱ����Ŀ¼*/
        ret = lookup_folder_id (folder->child, path, current);
    }
    free (current);
    if (ret != (u32) (-1)) {
        return ret;
    }
    /* �����Ƚ�ͬ��Ŀ¼*/
    ret = lookup_folder_id (folder->sibling, path, parent);
    return ret;
}

/**
 * ����һ���ַ���ȥ�ҵ�һ����ID
 *
 * @param path    ҪѰ�ҵ�·��
 * @param files   �ļ�����
 * @param folders Ŀ¼����
 *
 * @return �ɹ�������ID
 **/
int parse_path (char * path, LIBMTP_file_t * files, LIBMTP_folder_t * folders)
{
    char *rest;
    u32 item_id;

    /* ���·���ǲ���һ����ID*/
    if (*path != '/') {
        /* ��ȡpath��������޷��ų�����*/
        item_id = strtoul(path, &rest, 0);
        /* Ӧ�ü�顰rest��������...*/
        /* �������һ�����֣�������һ���ļ���*/
        if (item_id == 0) {
            LIBMTP_file_t * file = files;

            /* Ѱ�Ҷ�Ӧ������ */
            while (file != NULL) {
                /* ���Դ�Сд���Ƚ��ַ���*/
                if (strcasecmp (file->filename, path) == 0) {
                    return file->item_id;
                }
                file = file->next;
            }
        }
        return item_id;
    }
    /* ���·���ǲ���һ����Ŀ¼·��*/
    item_id = lookup_folder_id(folders, path, "");
    if (item_id == (u32) -1) {
        /* �����ַ���*/
        char * dirc = mtp_strdup(path);
        char * basec = mtp_strdup(path);
        /* ��ȡ��Ŀ¼��·��*/
        char * parent = mtp_dirname(dirc);
        /* ��ȡ�ļ�����*/
        char * filename = mtp_basename(basec);
        /* ��ȡ��Ŀ¼ID*/
        u32 parent_id = lookup_folder_id(folders, parent,"");
        LIBMTP_file_t * file;

        file = files;
        while (file != NULL) {
            if (file->parent_id == parent_id) {
                /* �Ա��ļ�����*/
                if (strcasecmp (file->filename, filename) == 0) {
                    mtp_free(dirc);
                    mtp_free(basec);
                    return file->item_id;
                }
            }
            file = file->next;
        }
        mtp_free(dirc);
        mtp_free(basec);
    } else {
        return item_id;
    }

    return -1;
}

/**
 * ��ӡ���̽�����
 *
 * @param sent  �ѷ��͵�����
 * @param total ����������
 * @param data  �û�����
 *
 * @return �ɹ�����0��
 */
int progress (const u64 sent, const u64 total, void const * const data)
{
  int percent = (sent * 100) / total;
#ifdef __WIN32__
  printf("Progress: %I64u of %I64u (%d%%)\r", sent, total, percent);
#else
  fflush(stdout);
  mtp_printf("Progress: %llu of %llu (%d%%)\r\r", (int)sent, (int)total, percent);
#endif
  fflush(stdout);
  return 0;
}

/**
 * �����ļ���ȷ���ļ�����
 *
 * @param filename �ļ���
 *
 * @return �ɹ������ļ�����
 */
LIBMTP_filetype_t find_filetype (char * filename)
{
    char *ptype;
    LIBMTP_filetype_t filetype;

#ifdef __WIN32__
    ptype = strrchr(filename, '.');
#else
    /* �����ļ��������֡�.���ַ���λ��*/
    ptype = mtp_rindex(filename,'.');
#endif
    /* ��˵�����ļ���û�С�.�������*/
    if (!ptype) {
        ptype = "";
    } else {
        ++ptype;
    }

    /* ȷ���ļ����� */
    if (!strcasecmp (ptype, "wav")) {
        filetype = LIBMTP_FILETYPE_WAV;
    } else if (!strcasecmp (ptype, "mp3")) {
        filetype = LIBMTP_FILETYPE_MP3;
    } else if (!strcasecmp (ptype, "wma")) {
        filetype = LIBMTP_FILETYPE_WMA;
    } else if (!strcasecmp (ptype, "ogg")) {
        filetype = LIBMTP_FILETYPE_OGG;
    } else if (!strcasecmp (ptype, "mp4")) {
        filetype = LIBMTP_FILETYPE_MP4;
    } else if (!strcasecmp (ptype, "wmv")) {
        filetype = LIBMTP_FILETYPE_WMV;
    } else if (!strcasecmp (ptype, "avi")) {
        filetype = LIBMTP_FILETYPE_AVI;
    } else if (!strcasecmp (ptype, "mpeg") || !strcasecmp (ptype, "mpg")) {
        filetype = LIBMTP_FILETYPE_MPEG;
    } else if (!strcasecmp (ptype, "asf")) {
        filetype = LIBMTP_FILETYPE_ASF;
    } else if (!strcasecmp (ptype, "qt") || !strcasecmp (ptype, "mov")) {
        filetype = LIBMTP_FILETYPE_QT;
    } else if (!strcasecmp (ptype, "wma")) {
        filetype = LIBMTP_FILETYPE_WMA;
    } else if (!strcasecmp (ptype, "jpg") || !strcasecmp (ptype, "jpeg")) {
        filetype = LIBMTP_FILETYPE_JPEG;
    } else if (!strcasecmp (ptype, "jfif")) {
        filetype = LIBMTP_FILETYPE_JFIF;
    } else if (!strcasecmp (ptype, "tif") || !strcasecmp (ptype, "tiff")) {
        filetype = LIBMTP_FILETYPE_TIFF;
    } else if (!strcasecmp (ptype, "bmp")) {
        filetype = LIBMTP_FILETYPE_BMP;
    } else if (!strcasecmp (ptype, "gif")) {
        filetype = LIBMTP_FILETYPE_GIF;
    } else if (!strcasecmp (ptype, "pic") || !strcasecmp (ptype, "pict")) {
        filetype = LIBMTP_FILETYPE_PICT;
    } else if (!strcasecmp (ptype, "png")) {
        filetype = LIBMTP_FILETYPE_PNG;
    } else if (!strcasecmp (ptype, "wmf")) {
        filetype = LIBMTP_FILETYPE_WINDOWSIMAGEFORMAT;
    } else if (!strcasecmp (ptype, "ics")) {
        filetype = LIBMTP_FILETYPE_VCALENDAR2;
    } else if (!strcasecmp (ptype, "exe") || !strcasecmp (ptype, "com") ||
            !strcasecmp (ptype, "bat") || !strcasecmp (ptype, "dll") ||
            !strcasecmp (ptype, "sys")) {
        filetype = LIBMTP_FILETYPE_WINEXEC;
    } else if (!strcasecmp (ptype, "aac")) {
        filetype = LIBMTP_FILETYPE_AAC;
    } else if (!strcasecmp (ptype, "mp2")) {
        filetype = LIBMTP_FILETYPE_MP2;
    } else if (!strcasecmp (ptype, "flac")) {
        filetype = LIBMTP_FILETYPE_FLAC;
    } else if (!strcasecmp (ptype, "m4a")) {
        filetype = LIBMTP_FILETYPE_M4A;
    } else if (!strcasecmp (ptype, "doc")) {
        filetype = LIBMTP_FILETYPE_DOC;
    } else if (!strcasecmp (ptype, "xml")) {
        filetype = LIBMTP_FILETYPE_XML;
    } else if (!strcasecmp (ptype, "xls")) {
        filetype = LIBMTP_FILETYPE_XLS;
    } else if (!strcasecmp (ptype, "ppt")) {
        filetype = LIBMTP_FILETYPE_PPT;
    } else if (!strcasecmp (ptype, "mht")) {
        filetype = LIBMTP_FILETYPE_MHT;
    } else if (!strcasecmp (ptype, "jp2")) {
        filetype = LIBMTP_FILETYPE_JP2;
    } else if (!strcasecmp (ptype, "jpx")) {
        filetype = LIBMTP_FILETYPE_JPX;
    } else if (!strcasecmp (ptype, "bin")) {
        filetype = LIBMTP_FILETYPE_FIRMWARE;
    } else if (!strcasecmp (ptype, "vcf")) {
         filetype = LIBMTP_FILETYPE_VCARD3;
    } else {
        /* ���Ϊδ֪�ļ����� */
        filetype = LIBMTP_FILETYPE_UNKNOWN;
    }
    mtp_printf("type: %s, %d\r\n", ptype, filetype);
    return filetype;
}

/**
 * ��ȡ����·�������һ���ļ�����Ŀ¼��
 *
 * @param in  �����·��
 *
 * @return �ɹ������ļ�����Ŀ¼���ַ���
 */
char *mtp_basename(char *in) {
    char *p;

    if (in == NULL)
        return NULL;
    p = in + strlen(in) - 1;
    while (*p != '\\' && *p != '/' && *p != ':'){
        if(p == in){
            return NULL;
        }
        p--;
    }
    return ++p;
}

/**
 * ��ȡ����·���ĸ�Ŀ¼·��
 *
 * @param in  �����·��
 *
 * @return �ɹ����ظ�Ŀ¼·��
 */
char *mtp_dirname(char *in) {
    char *p;

    if (in == NULL)
        return NULL;
    p = in + strlen(in) - 1;
    while (*p != '\\' && *p != '/' && *p != ':'){
        if(p == in){
            return NULL;
        }
        p--;
    }
    p++;
    *p = '\0';
    return in;
}

/**
 * �����ַ���
 *
 * @param s  ������ַ���
 *
 * @return �ɹ����ش����ַ����Ŀ���
 */
char *mtp_strdup(const char *s){
    /* �Ϸ��Լ��*/
    if(s == NULL){
        return NULL;
    }
    /* �����ڴ�ռ�*/
    char *temp = mtp_malloc(strlen(s) + 1);
    if(temp == NULL){
        return NULL;
    }
    /* ��0�ռ�*/
    memset(temp, 0, strlen(s) + 1);
    /* �����ַ���*/
    memcpy(temp, s, strlen(s));

    return temp;
}

/**
 * �����ַ�����ǰnum���ֽ�
 *
 * @param s   ������ַ���
 * @param num Ҫ�������ֽ���
 *
 * @return �ɹ����ش����ַ����Ŀ���
 */
char *mtp_strndup(const char *s, int num){
    /* �Ϸ��Լ��*/
    if((s == NULL) || (num <= 0)){
        return NULL;
    }
    if(num >= strlen(s)){
        /* �����ڴ�ռ�*/
        char *temp = mtp_malloc(strlen(s) + 1);
        if(temp == NULL){
            return NULL;
        }
        /* ��0�ռ�*/
        memset(temp, 0, strlen(s) + 1);
        /* �����ַ���*/
        memcpy(temp, s, strlen(s));

        return temp;
    }
    else{
        /* �����ڴ�ռ�*/
        char *temp = mtp_malloc(num + 1);
        if(temp == NULL){
            return NULL;
        }
        /* ��0�ռ�*/
        memset(temp, 0, num + 1);
        /* �����ַ���*/
        memcpy(temp, s, num);

        return temp;
    }
    return NULL;
}

/**
 * �����������ַ�c���ַ���
 *
 * @param s ������ַ���
 * @param c Ҫƥ����ַ�
 *
 * @return �ɹ������������ַ�c���ַ���
 */
char *mtp_rindex(char *s, char c){
    int len;
    char *p;

    /* �Ϸ��Լ��*/
    if(s == NULL){
        return NULL;
    }
    /* ����ַ�������*/
    len = strlen(s);
    if(len == 0){
        return NULL;
    }
    /* �ƶ����ַ���ĩβ*/
    p = s + strlen(s) - 1;
    /* �����ַ���*/
    while(*p != c){
        if(p != s){
            p--;
        }
        else{
            return NULL;
        }
    }

    return p;
}

