#include "common.h"
#include "pathutils.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "mtp_adapter.h"

/**
 * 寻找名字对应的目录ID
 *
 * @param folder 目录链表
 * @param path   要寻找的路径名字
 * @param parent 父目录路径
 *
 * @return 成功返回找到的目录ID
 **/
static u32 lookup_folder_id (LIBMTP_folder_t * folder, char * path, char * parent)
{
    char * current;
    u32 ret = (u32) -1;

    /* 根目录*/
    if (strcmp(path,"/")==0)
        return 0;
    /* 检查目录链表*/
    if (folder == NULL) {
        return ret;
    }

    current = mtp_malloc (strlen(parent) + strlen(folder->name) + 2);
    sprintf(current,"%s/%s",parent,folder->name);
    /* 忽略大小写，比较字符串*/
    if (strcasecmp (path, current) == 0) {
        mtp_free (current);
        return folder->folder_id;
    }
    /* 是子目录*/
    if (strncasecmp (path, current, strlen (current)) == 0) {
        /* 寻找子目录*/
        ret = lookup_folder_id (folder->child, path, current);
    }
    free (current);
    if (ret != (u32) (-1)) {
        return ret;
    }
    /* 继续比较同级目录*/
    ret = lookup_folder_id (folder->sibling, path, parent);
    return ret;
}

/**
 * 分析一个字符串去找到一个项ID
 *
 * @param path    要寻找的路径
 * @param files   文件链表
 * @param folders 目录链表
 *
 * @return 成功返回项ID
 **/
int parse_path (char * path, LIBMTP_file_t * files, LIBMTP_folder_t * folders)
{
    char *rest;
    u32 item_id;

    /* 检查路径是不是一个项ID*/
    if (*path != '/') {
        /* 获取path名字里的无符号长整数*/
        item_id = strtoul(path, &rest, 0);
        /* 应该检查“rest”的内容...*/
        /* 如果不是一个数字，假设是一个文件名*/
        if (item_id == 0) {
            LIBMTP_file_t * file = files;

            /* 寻找对应的名字 */
            while (file != NULL) {
                /* 忽略大小写，比较字符串*/
                if (strcasecmp (file->filename, path) == 0) {
                    return file->item_id;
                }
                file = file->next;
            }
        }
        return item_id;
    }
    /* 检查路径是不是一个含目录路径*/
    item_id = lookup_folder_id(folders, path, "");
    if (item_id == (u32) -1) {
        /* 拷贝字符串*/
        char * dirc = mtp_strdup(path);
        char * basec = mtp_strdup(path);
        /* 获取父目录的路径*/
        char * parent = mtp_dirname(dirc);
        /* 获取文件名字*/
        char * filename = mtp_basename(basec);
        /* 获取父目录ID*/
        u32 parent_id = lookup_folder_id(folders, parent,"");
        LIBMTP_file_t * file;

        file = files;
        while (file != NULL) {
            if (file->parent_id == parent_id) {
                /* 对比文件名字*/
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
 * 打印进程进度条
 *
 * @param sent  已发送的数量
 * @param total 完整的数量
 * @param data  用户数据
 *
 * @return 成功返回0。
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
 * 根据文件名确定文件类型
 *
 * @param filename 文件名
 *
 * @return 成功返回文件类型
 */
LIBMTP_filetype_t find_filetype (char * filename)
{
    char *ptype;
    LIBMTP_filetype_t filetype;

#ifdef __WIN32__
    ptype = strrchr(filename, '.');
#else
    /* 返回文件名最后出现‘.’字符的位置*/
    ptype = mtp_rindex(filename,'.');
#endif
    /* 这说明有文件名没有“.”的情况*/
    if (!ptype) {
        ptype = "";
    } else {
        ++ptype;
    }

    /* 确定文件类型 */
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
        /* 标记为未知文件类型 */
        filetype = LIBMTP_FILETYPE_UNKNOWN;
    }
    mtp_printf("type: %s, %d\r\n", ptype, filetype);
    return filetype;
}

/**
 * 获取整个路径的最后一个文件名或目录名
 *
 * @param in  输入的路径
 *
 * @return 成功返回文件名或目录名字符串
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
 * 获取整个路径的父目录路径
 *
 * @param in  输入的路径
 *
 * @return 成功返回父目录路径
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
 * 拷贝字符串
 *
 * @param s  传入的字符串
 *
 * @return 成功返回传入字符串的拷贝
 */
char *mtp_strdup(const char *s){
    /* 合法性检查*/
    if(s == NULL){
        return NULL;
    }
    /* 申请内存空间*/
    char *temp = mtp_malloc(strlen(s) + 1);
    if(temp == NULL){
        return NULL;
    }
    /* 清0空间*/
    memset(temp, 0, strlen(s) + 1);
    /* 拷贝字符串*/
    memcpy(temp, s, strlen(s));

    return temp;
}

/**
 * 拷贝字符串的前num个字节
 *
 * @param s   传入的字符串
 * @param num 要拷贝的字节数
 *
 * @return 成功返回传入字符串的拷贝
 */
char *mtp_strndup(const char *s, int num){
    /* 合法性检查*/
    if((s == NULL) || (num <= 0)){
        return NULL;
    }
    if(num >= strlen(s)){
        /* 申请内存空间*/
        char *temp = mtp_malloc(strlen(s) + 1);
        if(temp == NULL){
            return NULL;
        }
        /* 清0空间*/
        memset(temp, 0, strlen(s) + 1);
        /* 拷贝字符串*/
        memcpy(temp, s, strlen(s));

        return temp;
    }
    else{
        /* 申请内存空间*/
        char *temp = mtp_malloc(num + 1);
        if(temp == NULL){
            return NULL;
        }
        /* 清0空间*/
        memset(temp, 0, num + 1);
        /* 拷贝字符串*/
        memcpy(temp, s, num);

        return temp;
    }
    return NULL;
}

/**
 * 返回最后出现字符c的字符串
 *
 * @param s 传入的字符串
 * @param c 要匹配的字符
 *
 * @return 成功返回最后出现字符c的字符串
 */
char *mtp_rindex(char *s, char c){
    int len;
    char *p;

    /* 合法性检查*/
    if(s == NULL){
        return NULL;
    }
    /* 检查字符串长度*/
    len = strlen(s);
    if(len == 0){
        return NULL;
    }
    /* 移动到字符串末尾*/
    p = s + strlen(s) - 1;
    /* 遍历字符串*/
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

