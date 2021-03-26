/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief I/O system shell commands.
 *
 * \internal
 * \par modification history:
 * - 1.01 16-01-07 deo, modify pwd cd ll ls mkdir ..., use aw_getcwd.
 * - 1.00 14-08-25 orz, first implementation.
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "apollo.h"
#include "aw_shell.h"

#include "aw_common.h"
#include "aw_time.h"
#include "aw_mem.h"
//#include "rtk_task.h"
#include "fs/aw_mount.h"
#include "io/sys/aw_stat.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_dirent.h"
#include "string.h"


aw_local aw_const char pwd_help_info[] = {
    "Command:pwd\n\n"
    "Description:Show current working path\n\n"
    "Examples:\n"
    "    1.pwd\n"
};



aw_local char *__size_disp (uint32_t size)
{
    char        *unit;
    static char  str[32];
    float        dsize;

#define __GB (1024 * 1024 * 1024)
#define __MB (1024 * 1024)
#define __KB (1024)

    if (size > __GB) {
        dsize = (float)size / __GB;
        unit = "GBytes";
    } else if (size > __MB) {
        dsize = (float)size / __MB;
        unit = "MBytes";
    } else if (size > __KB) {
        dsize = (float)size / __KB;
        unit = "KBytes";
    } else {
        unit = "Bytes";
    }

    snprintf(str, sizeof(str), "%.02f %s", dsize, unit);
    return str;
}


/******************************************************************************/
/*
 * progress bar class
 */
typedef struct __progress_bar {
    uint8_t     title_width;
    uint8_t     bar_width;
    uint32_t    val_max;
    uint32_t    val;
    char       *fbuf;
    char       *buf;
    uint16_t    bufsize;

#define PR_BAR_MODE_INC    0
#define PR_BAR_MODE_DEC    1

    struct aw_shell_user *sh;

    void (*destory) (struct __progress_bar *this);
    void (*reset) (struct __progress_bar *this, const char *title, uint32_t val_max);
    void (*update) (struct __progress_bar *this, uint32_t value, uint8_t mode);
} progress_bar_t;


aw_local void __pr_bar_update (struct __progress_bar *this, uint32_t value, uint8_t mode)
{
    uint32_t i, val;
    char perc[8];

    if (value > this->val_max) {
        value = this->val_max;
    }

    if (mode == PR_BAR_MODE_DEC) {
        value = this->val_max - value;
    }

    val = value * this->bar_width / this->val_max;

    if (val == this->val) {
        return;
    }
    this->val = val;

    i = this->title_width;
    this->fbuf[i++] = '[';
    val = val + i;
    for (; i < val; i++) {
        this->fbuf[i] = '#';
    }

    for (; i < this->bar_width + this->title_width + 1; i++) {
        this->fbuf[i] = '-';
    }
    this->fbuf[i++] = ']';
    this->fbuf[i++] = '\r';
    this->fbuf[i] = '\0';

    if (value == this->val_max) {
        strcpy(perc, " Done ");
    } else {
        sprintf(perc, " %d ", value * 100 / this->val_max);
    }
    memcpy(&this->fbuf[this->title_width + this->bar_width / 2 - 2], perc, strlen(perc));

    if (memcmp(this->buf, this->fbuf, this->bufsize) == 0) {
        return;
    }

    memcpy(this->buf, this->fbuf, this->bufsize);
    aw_shell_printf(this->sh, this->buf);
}

aw_local void __pr_bar_destory (struct __progress_bar *this)
{
    aw_mem_free(this->fbuf);
    aw_mem_free(this);
}


aw_local void __pr_bar_reset (struct __progress_bar *this, const char *title, uint32_t val_max)
{
    uint32_t        tmp;

    this->val_max = val_max;
    this->val     = 0xFFFFFFFF;

    tmp = min(strlen(title), this->title_width - 1);
    memcpy(this->fbuf, title, tmp);
    memset(&this->fbuf[tmp], ' ', this->title_width - tmp);
}


progress_bar_t *progress_bar_new (struct aw_shell_user *sh,
        const char *title, uint8_t twidth, uint8_t bwidth, uint32_t val_max)
{
    progress_bar_t *this;
    uint32_t        tmp;

    this = (progress_bar_t *)aw_mem_alloc(sizeof(*this));
    if (this == NULL) {
        return NULL;
    }
    memset(this, 0, sizeof(*this));

    this->sh          = sh;
    this->title_width = twidth;
    this->bar_width   = bwidth;
    this->val_max     = val_max;
    this->bufsize     = twidth + bwidth + 4;
    this->val         = 0xFFFFFFFF;
    this->fbuf        = (char *)aw_mem_alloc(this->bufsize << 1);
    if (this->fbuf == NULL) {
        goto __failed;
    }
    this->buf = this->fbuf + this->bufsize;

    tmp = min(strlen(title), twidth - 1);
    memcpy(this->fbuf, title, tmp);
    memset(&this->fbuf[tmp], ' ', twidth - tmp);

    this->update  = __pr_bar_update;
    this->destory = __pr_bar_destory;
    this->reset   = __pr_bar_reset;

    return this;
__failed:
    if (this) {
        if (this->fbuf) {
            aw_mem_free(this->fbuf);
        }
        aw_mem_free(this);
    }
    return NULL;
}





/******************************************************************************/
static int __pwd (int argc, char *argv[], struct aw_shell_user *sh)
{
    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,pwd_help_info);
        return AW_OK;
    }

    if(argc != 0){
        return AW_ERROR;
    }

    char  path[AW_IO_PATH_LEN_MAX];

    if (aw_getcwd(path, AW_IO_PATH_LEN_MAX) == NULL) {
        return -AW_ENOENT;
    }

    (void)aw_shell_puts(sh, path);
    return aw_shell_putc(sh, '\n');
}


aw_local aw_const char cd_help_info[] = {
    "Command:cd <path>\n\n"
    "Description:Change directory\n\n"
    "Examples:\n"
    "    1.cd u\n"
    "    2.cd ../\n"
    "    3.cd sd/test2\n"
    "    4.cd ../../\n"
};

static int __cd (int argc, char *argv[], struct aw_shell_user *sh)
{
    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,cd_help_info);
        return AW_OK;
    }

    int err;

    if (1 == argc) {
        err = aw_chdir(argv[0]);
        if (err == AW_OK) {
            __pwd(0, NULL, sh);
        }
        return err;
    } else {
        return -AW_EINVAL;
    }
}

/******************************************************************************/
static int __ls_dir_line (int                   detail,
                          time_t                timenow,
                          char                 *path,
                          const char           *filename,
                          struct aw_shell_user *sh)
{
    char           timebuf[80];
    struct aw_stat sbuf;
    struct tm      tm;
    time_t         ft;
    mode_t         mode;
    int            err, index;

    if (!detail) {
        if (filename) {
            aw_shell_puts(sh, filename);
        }
        (void)aw_shell_putc(sh, '\n');
        return 0;
    }

    if (filename != NULL) {
        index = strlen(path);
        path[index] = '/';
        strcpy(&path[index + 1], filename);
    } else {
        index = strlen(path);
        while (path[index] != '/') {
            index--;
        }
        index++;
        filename = path + index;
    }

    if ((err = aw_stat(path, &sbuf)) != 0) {
        goto __exit;
    }
    ft = sbuf.st_mtim.tv_sec;
    gmtime_r(&ft, &tm);
    if (difftime(timenow, ft) > (365 * 24 * 60 * 60 / 2 /* 6 months */)) {
        strftime(timebuf, sizeof(timebuf), "%b %d  %Y", &tm);
    } else {
        strftime(timebuf, sizeof(timebuf), "%b %d  %H:%M", &tm);
    }
    mode = sbuf.st_mode;
    (void)aw_shell_printf(sh, "%c%c%c%c%c%c%c%c%c%c %2d user apollo %9u %s %s\n",
                      (S_ISLNK(mode) ? ('l') : (S_ISDIR(mode) ? ('d') : ('-'))),
                      (mode & S_IRUSR) ? ('r') : ('-'),
                      (mode & S_IWUSR) ? ('w') : ('-'),
                      (mode & S_IXUSR) ? ('x') : ('-'),
                      (mode & S_IRGRP) ? ('r') : ('-'),
                      (mode & S_IWGRP) ? ('w') : ('-'),
                      (mode & S_IXGRP) ? ('x') : ('-'),
                      (mode & S_IROTH) ? ('r') : ('-'),
                      (mode & S_IWOTH) ? ('w') : ('-'),
                      (mode & S_IXOTH) ? ('x') : ('-'),
                      sbuf.st_nlink,
#if 0
                      (int)sbuf.st_uid,
                      (int)sbuf.st_gid,
#endif
                      (int)sbuf.st_size,
                      timebuf,
                      filename);

__exit:
    path[index] = '\0';
    return err;
}

aw_local aw_const char ls_help_info[] = {
    "Command:ls [-l] [path]\n\n"
    "Description:List files and directories\n\n"
    "Examples:\n"
    "    1.List files and folders in the current direct:\n"
    "      ls\n"
    "    2.list details of files and folders in the current direct:/\n"
    "      ls -l\n"
    "    3.list details of files and folders in the \"/u\" direct:\n"
    "      ls -l /u\n"
};

static int __ls (int argc, char *argv[], struct aw_shell_user *sh)
{
    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,ls_help_info);
        return AW_OK;
    }

    struct aw_stat     sbuf;
    struct aw_dirent  *pdirent;
    struct aw_dir     *pdir     = NULL;
    const char        *filename = "";
    char               path[AW_IO_PATH_LEN_MAX];
    time_t             timenow;
    int                err, detail = 0;

    if (1 == argc) {
        /* one argument, check if is "-l" or a file name */
        if (strcmp("-l", argv[0]) == 0) {
            detail = 1;
        } else {
            filename = argv[0];
        }
    } else if (2 == argc) {
        /* two arguments, must be "-l file/name" */
        if (strcmp("-l", argv[0]) != 0) {
            return -AW_EINVAL;
        }
        detail   = 1;
        filename = argv[1];
    }

    aw_io_full_path(path, filename);

    if ((err = aw_stat(path, &sbuf)) < 0) {
        goto __exit;
    } else if (S_ISDIR(sbuf.st_mode)) {
        if ((pdir = aw_opendir(path)) == NULL) {
            goto __exit;
        }
    }

    timenow = aw_time(NULL);
    if (NULL == pdir) {
        err = __ls_dir_line(detail, timenow, path, NULL, sh);
    } else {
        do {
            if ((pdirent = aw_readdir(pdir)) == NULL) {
                break;
            }
            err = __ls_dir_line(detail, timenow, path, pdirent->d_name, sh);
        } while (0 == err);
        aw_closedir(pdir);
    }

__exit:
    return err;
}

aw_local aw_const char ll_help_info[] = {
    "Command:ll [path]\n\n"
    "Description:List details of files and directories\n\n"
    "Examples:\n"
        "    1.list details of files and folders in the current direct:\n"
        "      ll \n"
        "    2.list details of files and folders in the \"/u\" direct:\n"
        "      ll /u\n"
};

static int __ll (int argc, char *argv[], struct aw_shell_user *sh)
{
    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,ll_help_info);
        return AW_OK;
    }

    char *argv_local[2];

    argv_local[0] = "-l";
    if (argc > 0) {
        argc = 2;
        argv_local[1] = argv[0];
    } else {
        argc = 1;
    }

    return __ls(argc, argv_local, sh);
}
aw_local aw_const char stat_help_info[] = {
    "Command:stat <path>\n\n"
    "Description:View a file status\n\n"
    "Examples:\n"
    "    1.stat /u/test1.txt \n"
    "    2.stat /sd/test2.txt \n"
    "    3.stat /nf/test3.txt \n"
    "    3.stat /sf/test4.txt \n"
};
static int __stat (int argc, char *argv[], struct aw_shell_user *sh)
{
    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,stat_help_info);
        return AW_OK;
    }

    if (argc < 1) {
        return -AW_EINVAL;
    }

    struct aw_stat sbuf;

    if (aw_stat(argv[0], &sbuf) < 0) {
        return aw_shell_puts(sh, "stat error\n");
    }
    return aw_shell_puts(sh, "stat OK\n");
}


aw_local aw_const char mkdir_help_info[] = {
    "Command:mkdir <path>\n\n"
    "Description:Create a directory\n\n"
    "Examples:\n"
    "    1.mkdir /u/test1 \n"
    "    2.mkdir /sd/test2 \n"
    "    3.mkdir /nf/test3 \n"
    "    4.mkdir /sf/test4 \n"
};

static int __mkdir (int argc, char *argv[], struct aw_shell_user *sh)
{
    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,mkdir_help_info);
        return AW_OK;
    }

    if (argc < 1) {
        return -AW_EINVAL;
    }

    return aw_mkdir(argv[0], S_IRWXU | S_IRWXG | S_IRWXO);
}

aw_local aw_const char rmdir_help_info[] = {
    "Command:rmdir <path>\n\n"
    "Description:Delete a directory\n\n"
    "Examples:\n"
    "    1.rmdir /u/test1 \n"
    "    2.rmdir /sd/test2 \n"
    "    3.rmdir /nf/test3 \n"
    "    4.rmdir /sf/test4 \n"
};

static int __rmdir (int argc, char *argv[], struct aw_shell_user *sh)
{
    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,rmdir_help_info);
        return AW_OK;
    }

    if (argc < 1) {
        return -AW_EINVAL;
    }

    return aw_rmdir(argv[0]);
}

aw_local aw_const char touch_help_info[] = {
    "Command:touch <path>\n\n"
    "Description:Create a file\n\n"
    "Examples:\n"
    "    1.touch /u/test1.txt \n"
    "    2.touch /sd/test2.txt \n"
    "    3.touch /nf/test3.txt \n"
    "    4.touch /sf/test4.txt \n"
};

static int __touch (int argc, char *argv[], struct aw_shell_user *sh)
{
    int   fd;

    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,touch_help_info);
        return AW_OK;
    }

    if (argc < 1) {
        return -AW_EINVAL;
    }

    if ((fd = aw_create(argv[0], S_IRWXU | S_IRWXG | S_IRWXO)) < 0) {
        return fd;
    }

    return aw_close(fd);
}

aw_local aw_const char rm_help_info[] = {
    "Command:rm <path>\n\n"
    "Description:Delete a file\n\n"
    "Examples:\n"
        "    1.rm /u/test1.txt \n"
        "    2.rm /sd/test2.txt \n"
        "    3.rm /nf/test3.txt \n"
        "    4.rm /sf/test4.txt \n"
};

static int __rm (int argc, char *argv[], struct aw_shell_user *sh)
{
    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,rm_help_info);
        return AW_OK;
    }

    if (argc < 1) {
        return -AW_EINVAL;
    }

    return aw_unlink(argv[0]);
}

aw_local aw_const char mv_help_info[] = {
    "Command:mv <old> <new>\n\n"
    "Description:Rename a file\n\n"
    "Examples:\n"
    "    1.mv /u/test1.txt /u/aworks1.txt\n"
    "    2.mv /sd/test2.txt /sd/aworks2.txt\n"
    "    3.mv /nf/test3.txt /nf/aworks3.txt\n"
    "    4.mv /sf/test4.txt /sf/aworks4.txt\n"
};

static int __mv (int argc, char *argv[], struct aw_shell_user *sh)
{
    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,mv_help_info);
        return AW_OK;
    }

    if (argc < 2) {
        return -AW_EINVAL;
    }

    return aw_rename(argv[0], argv[1]);
}

aw_local aw_const char mkfs_help_info[] = {
    "Command:mkfs <device> <FS type> <flag> <blk_size>\n\n"
    "Description:Format a device(u disk/sd card/flash) with\n"
    "            FS(vfat/txfs/lffs/yaffs) type specified\n"
    "            <flag> default as:0\n"
    "            <blk_size> default as:4096\n"
    "Examples:\n"
    "    1.Format u-disk:\n"
    "      mkfs \"/dev/h0-d1-0\" vfat \n"
    "      mkfs \"/dev/h0-d1-0\" txfs \n"
    "    2.Format sd-card:\n"
    "      mkfs \"/dev/sd0\" vfat \n"
    "      mkfs \"/dev/sd0\" txfs \n"
    "    3.Format nand-flash:\n"
    "      mkfs \"/dev/mtd0\" yaffs \n"
    "      mkfs \"/dev/mtd0\" lffs \n"
    "    4.Format spi-flash:\n"
    "      mkfs \"/sflash0\" yaffs \n"
    "      mkfs \"/sflash0\" lffs \n"
};

static int __mkfs (int argc, char *argv[], struct aw_shell_user *sh)
{
    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,mkfs_help_info);
        return AW_OK;
    }
    
    struct aw_fs_format_arg fmt = {"awdisk", 0, 0};
    int                     err;

    if (argc < 2) {
        return -AW_EINVAL;
    } if (argc > 2) {
        fmt.flags = *argv[2] - '0';
    }  if (argc > 3) {
        fmt.unit_size = atoi(argv[3]);
    }

    if (fmt.unit_size == 0) {
        fmt.unit_size = 4 * 1024;
    }

    aw_shell_printf(sh, "make %s on %s (flags=%x) ", argv[1], argv[0], fmt.flags);
    if ((err = aw_make_fs(argv[0], argv[1], &fmt)) != 0) {
        aw_shell_printf(sh, "failed: %d\n", err);
    } else {
        aw_shell_puts(sh, "OK\n");
    }
    return err;
}

aw_local aw_const char mount_help_info[] = {
    "Command:mount <mount point> <device name> <FS type> \n\n"
    "Description:Mount a device(u disk/sd card/flash) at a mount point\n"
    "            with FS(vfat/txfs/lffs/yaffs) type specified.\n\n"
    "Examples:\n"
    "    1.mount u-disk: \n"
    "      mount /u \"/dev/h0-d1-0\" vfat \n"
    "      mount /u \"/dev/h0-d1-0\" txfs \n"
    "    2.mount sd-card: \n"
    "      mount /sd \"/dev/sd0\" vfat \n"
    "      mount /sd \"/dev/sd0\" txfs \n"
    "    3.mount nand-flash: \n"
    "      mount /nf \"/dev/mtd0\" yaffs \n"
    "      mount /nf \"/dev/mtd0\" lffs \n"
    "    4.mount spi-flash: \n"
    "      mount /sf \"/sflash0\" yaffs \n"
    "      mount /sf \"/sflash0\" lffs \n"
};

static int __mount (int argc, char *argv[], struct aw_shell_user *sh)
{
    int err;

    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,mount_help_info);
        return AW_OK;
    }
    if (argc != 3) {
        return -AW_EINVAL;
    }
    aw_shell_printf(sh, "mount \"%s\" at \"%s\" as \"%s\"...\n",
                 argv[1], argv[0], argv[2]);
    if ((err = aw_mount(argv[0], argv[1], argv[2], 0)) != 0) {
        aw_shell_printf(sh, "mount failed: %d\n", err);
    } else {
        aw_shell_puts(sh, "mount OK\n");
    }
    return err;
}

aw_local aw_const char umount_help_info[] = {
    "Command:umount <path/device> \n\n"
    "Description:Unmount a mount point or device\n\n"
    "Examples:\n"
    "    1.umount u-disk:"
    "      umount \"/dev/h0-d1-0\" \n"
    "    2.umount sd-card:"
    "      umount \"/dev/sd0\"    \n"
    "    3.umount nand-flash:"
    "      umount \"/dev/mtd0\"     \n"
    "    4.umount spi-flash:"
    "      umount \"/sflash0\"     \n"
};

static int __umount (int argc, char *argv[], struct aw_shell_user *sh)
{
    int err;

    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,umount_help_info);
        return AW_OK;
    }
    if (argc != 1) {
        return -AW_EINVAL;
    }
    aw_shell_printf(sh, "umount \"%s\" ", argv[0]);
    if ((err = aw_umount(argv[0], 0)) != 0) {
        aw_shell_printf(sh, "failed: %d\n", err);
    } else {
        aw_shell_puts(sh, "OK\n");
    }
    return err;
}

static int __cp (int argc, char *argv[], struct aw_shell_user *p_user)
{
#define  __BUF_SIZE_MIN    2048
    uint32_t         bsize = __BUF_SIZE_MIN, len;
    aw_err_t         ret;
    struct aw_stat   sta;
    uint8_t         *p_buf = NULL;
    int              fd_src, fd_dst;
    progress_bar_t  *pbar = NULL;


    if ((argc != 2) && (argc != 3)) {
        return  -AW_EINVAL;
    }

    if (argc == 3) {
        bsize = max(atoi(argv[0]), __BUF_SIZE_MIN);
    }

    p_buf = (uint8_t *)aw_mem_alloc(bsize);
    if (NULL == p_buf) {
        aw_shell_printf(p_user, "No memory for copy buffer.\n");
        return  -AW_ENOMEM;
    }
    pbar = progress_bar_new(p_user, "", 8, 32, 0);

    fd_src = aw_open(argv[0], O_RDONLY, 0777);
    if (fd_src < 0) {
        aw_shell_printf(p_user, "Open file(%s) failed.\n", argv[0]);
        ret = -AW_ENOENT;
        goto __exit;
    }

    fd_dst = aw_open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd_src < 0) {
        aw_shell_printf(p_user, "Open file(%s) failed.\n", argv[1]);
        ret = -AW_ENOENT;
        goto __exit;
    }

    ret = aw_fstat(fd_src, &sta);
    if (ret != AW_OK) {
        goto __exit;
    }

    aw_shell_printf(p_user, "File size %s.\n", __size_disp(sta.st_size));

    if (pbar) {
        pbar->reset(pbar, "Copy", sta.st_size);
    }
    while (sta.st_size) {
        len = min(sta.st_size, bsize);

        ret = aw_read(fd_src, p_buf, len);
        if (ret != len) {
            aw_shell_printf(p_user, "Read failed.\n");
            ret = -AW_EIO;
            goto __exit;
        }

        ret = aw_write(fd_dst, p_buf, len);
        if (ret != len) {
            aw_shell_printf(p_user, "Write failed.\n");
            ret = -AW_EIO;
            goto __exit;
        }

        sta.st_size -= len;

        if (pbar) {
            pbar->update(pbar, sta.st_size, PR_BAR_MODE_DEC);
        }
    }
    aw_shell_printf(p_user, "\nCopy done.\n");

__exit:
    if (p_buf) {
        aw_mem_free(p_buf);
    }
    if (fd_src >= 0) {
        aw_close(fd_src);
    }
    if (fd_dst >= 0) {
        aw_close(fd_dst);
    }
    if (pbar) {
        pbar->destory(pbar);
    }
    return ret;
}

static int __file_rw_speed_test (int                     argc,
                                 char                   *argv[],
                                 struct aw_shell_user   *p_user)
{
#define __DEFAULT_SIZE  (1024 * 512) /* 512 KB */
#define __RW_CHAR       'A'

    char       *rw_buf;
    int         rw_size, speed;
    int         fd, i, len, err;
    aw_tick_t   ticks;
    aw_err_t    ret = AW_OK;

    (void)p_user;

    if (argc < 1) {
        return -AW_EINVAL;
    } else if (argc > 1) {
        if ((rw_size = atoi(argv[1])) < 1024) {
            rw_size = 1024;
        }
    } else {
        rw_size = __DEFAULT_SIZE;
    }
    aw_shell_printf(p_user, "\"%s\": %dB(%uKB) read and write speed test\n",
              argv[0], rw_size, rw_size / 1024);

    if ((rw_buf = aw_mem_alloc(rw_size)) == NULL) {
        aw_shell_printf(p_user, "alloc RW buffer failed\n");
        return  -AW_ENOMEM;
    }

    /*
     * open (create if not exist) and truncate the file size to zero
     *
     */
    if ((fd = aw_open(argv[0], O_RDWR | O_CREAT | O_TRUNC, 0777)) < 0) {
        aw_shell_printf(p_user, "Open to write failed\n");
        ret = -AW_ENOENT;
        goto __exit;
    }

    (void)memset(rw_buf, __RW_CHAR, rw_size);

    /*
     *    write  test...
     */
    ticks = aw_sys_tick_get();
    if ((len = aw_write(fd, rw_buf, rw_size)) != rw_size) {
        aw_shell_printf(p_user, "Write failed\n");
        aw_close(fd);
        ret = -AW_EIO;
        goto __exit;
    }

    if ((err = aw_close(fd)) != AW_OK) {
        aw_shell_printf(p_user, "Close write failed\n");
        ret = err;
        goto __exit;
    }

    ticks = aw_sys_tick_get() - ticks;
    speed = rw_size / aw_ticks_to_ms(ticks) * 1000;
    aw_shell_printf(p_user,
                    "Write file speed: tick %d, %d B/s (%u KB/s)\n",
                    ticks, speed, speed / 1024);


    /*
     *    read  test...
     */
    (void)memset(rw_buf, 0, rw_size);

    if ((fd = aw_open(argv[0], O_RDONLY, 0777)) < 0) {
        aw_shell_printf(p_user, "Open to read failed\n");
        ret = -AW_ENOENT;
        goto __exit;
    }
    ticks = aw_sys_tick_get();
    if ((len = aw_read(fd, rw_buf, rw_size)) != rw_size) {
        aw_shell_printf(p_user, "Read failed\n");
        aw_close(fd);
        ret = -AW_EIO;
        goto __exit;
    }

    if ((err = aw_close(fd)) != AW_OK) {
        aw_shell_printf(p_user, "Close read failed\n");
        ret = err;
        goto __exit;
    }

    ticks = aw_sys_tick_get() - ticks;
    speed = rw_size / aw_ticks_to_ms(ticks) * 1000;
    aw_shell_printf(p_user,
                    "Read file speed: tick %d, %d B/s (%u KB/s)\n",
                    ticks, speed, speed / 1024);

    /* verify */
    for (i = 0; i < rw_size; i++) {
        if (rw_buf[i] != __RW_CHAR) {
            aw_shell_printf(p_user, "Verify failed at %i\n", i);
            goto  __exit;
        }
    }

    aw_shell_printf(p_user, "Verify data ok.\n");

__exit:
    aw_mem_free(rw_buf);
    return  ret;
}

/******************************************************************************/
static const struct aw_shell_cmd __g_ios_shell_cmd_tbl[] = {
    {__mkfs,    "mkfs",  "Format a device"},
    {__mount,   "mount", "Mount a device at a mount point with FS type specified\n"},
    {__umount,  "umount","Umount a mount point or device"},
    {__mkdir,   "mkdir", "Create a directory"},
    {__rmdir,   "rmdir", "Delete a directory"},
    {__touch,   "touch", "Create a file"},
    {__stat,    "stat",  "Show a file state"},
    {__pwd,     "pwd",   "Show current working directory"},
    {__rm,      "rm",    "Delete a file"},
    {__mv,      "mv",    "Rename a file"},
    {__cd,      "cd",    "Change working directory"},
    {__ls,      "ls",    "List files and directories"},
    {__ll,      "ll",    "List details of files and directories"},
    {__cp,      "cp",    "<file_src> <file_dest> - copy file"},
    {__file_rw_speed_test,  "fspeed",  "<path> [size]  - file RW speed test"},
};

/******************************************************************************/
void aw_io_shell_init (void)
{
    static struct aw_shell_cmd_list cl;

    (void)AW_SHELL_REGISTER_CMDS(&cl, __g_ios_shell_cmd_tbl);
}

/* end of file */
