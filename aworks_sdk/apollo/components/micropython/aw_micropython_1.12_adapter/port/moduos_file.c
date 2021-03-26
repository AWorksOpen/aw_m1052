/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 SummerGift <zhangyuan@rt-thread.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <string.h>
#include "moduos_file.h"
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/mperrno.h"
#include "unistd.h"
#include "aw_vdebug.h"
#include "time.h"
#include "io/aw_io_path.h"
#include "io/aw_dirent.h"
#include "fs/aw_mount.h"
#include "io/aw_unistd.h"
#include "py/objlist.h"
#include "py/obj.h"
#include "machine_sdcard.h"

#if MICROPY_PY_UOS_FILE

mp_obj_t mp_posix_mount(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    int res = 0;
    const char *dev ;
    const char *fs_type;

    //先判断pos_args[0]是不是块设备
    //再判断pos_args[0]是不是VFS文件系统对象
    mp_obj_base_t *base = (mp_obj_base_t *)pos_args[0];
    const char *mnt = mp_obj_str_get_str(pos_args[1]);


    if(base->type == &machine_sdcard_type){
        //sd卡
//        sdcard_card_obj_t *sd_obj = (sdcard_card_obj_t *)pos_args[0];
//        fs_type = sd_obj->fs_type;
//        dev = sd_obj->p_sdcard->p_sd_mem->p_card->name;
    }else if(dev ){
        //spi flash或者nandflash设备
    }

    res = aw_mount(mnt, dev, fs_type, 0);

    if (res < 0 ) {
        mp_raise_OSError(res);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_posix_mount_obj, 2, mp_posix_mount);

mp_obj_t mp_posix_mkfs(mp_obj_t arg1, mp_obj_t arg2) {
    int res = 0;
    const char *dev = mp_obj_str_get_str(arg1);
    const char *fs = mp_obj_str_get_str(arg2);
    struct aw_fs_format_arg fmt = {"awdisk", 0, 0};

    if ((res = aw_make_fs(dev,fs,&fmt)) != 0 ) {
        mp_raise_OSError(res);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_posix_mkfs_obj, mp_posix_mkfs);

mp_obj_t mp_posix_umount(mp_obj_t mnt_in) {
    int res = 0;
    const char *path = mp_obj_str_get_str(mnt_in);
    res = aw_umount(path, 0);

    if(res < 0){
        mp_raise_OSError(res);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_posix_umount_obj, mp_posix_umount);

mp_obj_t mp_posix_chdir(mp_obj_t path_in) {
    const char *changepath = mp_obj_str_get_str(path_in);
    int res = aw_chdir(changepath);

    if (res < 0) {
        mp_raise_OSError(res);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_posix_chdir_obj, mp_posix_chdir);

mp_obj_t mp_posix_getcwd(void) {

    char  path[AW_IO_PATH_LEN_MAX];
    char *res = aw_getcwd(path,AW_IO_PATH_LEN_MAX);
    if (res == NULL) {
        mp_raise_TypeError(res);
        return mp_const_none;
    }

    return mp_obj_new_str(path, strlen(path));
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_posix_getcwd_obj, mp_posix_getcwd);

int ls_dir_line (int detail,time_t timenow,char *path,const char *filename)
{
    char           timebuf[80];
    struct aw_stat sbuf;
    struct tm      tm;
    time_t         ft;
    mode_t         mode;
    int            err, index;

    if (!detail) {
        printf("%s\n",filename);
        return 0;
    }

    index = strlen(path);
    path[index] = '/';
    strcpy(&path[index + 1], filename);

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
    (void)printf("%c%c%c%c%c%c%c%c%c%c %2d user apollo %9u %s %s\n",
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

mp_obj_t mp_posix_listdir(size_t n_args, const mp_obj_t *args) {

    int err;
    struct aw_dirent  *pdirent;
    struct aw_dir     *pdir     = NULL;
    const char        *filename = "";
    char               path[AW_IO_PATH_LEN_MAX];

    if (1 == n_args) {
        filename = mp_obj_str_get_str(args[0]);
    }

    aw_io_full_path(path, filename);

    mp_import_stat_t path_type = mp_posix_import_stat(path);
    switch(path_type){
        case MP_IMPORT_STAT_NO_EXIST:
            mp_raise_msg_varg(&mp_type_NameError,"System can't find the path:%s",path);
            break;
        case MP_IMPORT_STAT_DIR:
            break;
        case MP_IMPORT_STAT_FILE:
            mp_raise_msg_varg(&mp_type_TypeError,"%s not a direct",path);
    }
    if((pdir = aw_opendir(path)) == NULL){
        mp_raise_msg_varg(&mp_type_Exception,"open %s fail",path);
    }

    //create a direct list
    mp_obj_t dir_list = mp_obj_new_list(0,NULL);

    while (1){
        if ((pdirent = aw_readdir(pdir)) == NULL) {
            break;
        }
        mp_obj_list_append(dir_list,mp_obj_new_str(pdirent->d_name,strlen(pdirent->d_name)));
    }

    if((err = aw_closedir(pdir))){
        mp_raise_OSError(err);
    }

    return dir_list;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_posix_listdir_obj, 0, 1, mp_posix_listdir);

typedef struct _mp_ilistdir_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    bool is_str;
    struct aw_dir dir;
    char path[];
} mp_ilistdir_it_t;

mp_obj_t mp_ilistdir_it_iternext(mp_obj_t self_in) {
    mp_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);
    struct aw_dirent  *pdirent;
    struct aw_stat buf;
    char path[24];

    for (;;) {
        if ((pdirent = aw_readdir(&self->dir)) == NULL) {
            break;
        }

        // make 4-tuple with info about this entry
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(4, NULL));
        if (self->is_str) {
            t->items[0] = mp_obj_new_str(pdirent->d_name, strlen(pdirent->d_name));
        } else {
            t->items[0] = mp_obj_new_bytes((const byte *)pdirent->d_name, strlen(pdirent->d_name));
        }
        memset(path,0,24);
        strcat(path,self->path);
        strcat(path,pdirent->d_name);
        aw_stat(path,&buf);
        if (S_ISDIR(buf.st_mode)) {
            // dir
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFDIR);
        } else {
            // file
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFREG);
        }

        t->items[2] = MP_OBJ_NEW_SMALL_INT(pdirent->d_ino); // no inode number
        t->items[3] = mp_obj_new_int_from_uint(buf.st_size);

        return MP_OBJ_FROM_PTR(t);
    }

    // ignore error because we may be closing a second time
    aw_closedir(&self->dir);

    return MP_OBJ_STOP_ITERATION;
}

mp_obj_t mp_posix_ilistdir(size_t n_args, const mp_obj_t *args) {
    bool is_str_type = true;
    struct aw_dir     *pdir     = NULL;
    const char *filename = "";
    char path[AW_IO_PATH_LEN_MAX];

    if (n_args == 1) {
        if (mp_obj_get_type(args[0]) == &mp_type_bytes) {
            is_str_type = false;
        }
        filename = mp_obj_str_get_str(args[0]);
    }

    assert(!aw_io_full_path(path, filename));

    mp_import_stat_t path_type = mp_posix_import_stat(path);
    switch(path_type){
        case MP_IMPORT_STAT_NO_EXIST:
            mp_raise_msg_varg(&mp_type_NameError,"System can't find the path:%s",path);
            break;
        case MP_IMPORT_STAT_DIR:
            break;
        case MP_IMPORT_STAT_FILE:
            mp_raise_msg_varg(&mp_type_TypeError,"%s not a direct",path);
    }

    // Create a new iterator object to list the dir
    mp_ilistdir_it_t *iter = m_new_obj(mp_ilistdir_it_t);
    iter->base.type = &mp_type_polymorph_iter;
    iter->iternext = mp_ilistdir_it_iternext;
    iter->is_str = is_str_type;

    if((pdir = aw_opendir(path)) == NULL){
        mp_raise_msg_varg(&mp_type_Exception,"open %s fail",path);
    }

    iter->dir = *pdir;
    strcpy(iter->path,path);

    return MP_OBJ_FROM_PTR(iter);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_posix_ilistdir_obj, 0, 1, mp_posix_ilistdir);

mp_obj_t mp_posix_mkdir(mp_obj_t path_in) {
    const char *createpath = mp_obj_str_get_str(path_in);
    int res = aw_mkdir(createpath, S_IRWXU | S_IRWXG | S_IRWXO);

    if (res < 0) {
        mp_raise_OSError(res);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_posix_mkdir_obj, mp_posix_mkdir);

mp_obj_t mp_posix_remove(uint n_args, const mp_obj_t *arg) {
    int index;
    int res = 0;
    if (n_args == 0) {
        printf("Usage: rm FILE...\n");
        printf("Remove (unlink) the FILE(s).\n");
        return mp_const_none;
    }
    for (index = 0; index < n_args; index++) {
        res = aw_unlink(mp_obj_str_get_str(arg[index]));

        if (res < 0){
            mp_raise_OSError(res);
        }
    }
    // TODO  recursive deletion
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR(mp_posix_remove_obj, 0, mp_posix_remove);

mp_obj_t mp_posix_rename(mp_obj_t old_path_in, mp_obj_t new_path_in) {
    const char *old_path = mp_obj_str_get_str(old_path_in);
    const char *new_path = mp_obj_str_get_str(new_path_in);
    int res = aw_rename(old_path, new_path);

    if (res < 0) {
        mp_raise_OSError(res);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_posix_rename_obj, mp_posix_rename);

mp_obj_t mp_posix_rmdir(uint n_args, const mp_obj_t *arg) {
    int index;
    int res = 0;
    if (n_args == 0) {
        printf("Usage: rm FILE...\n");
        printf("Remove (unlink) the FILE(s).\n");
        return mp_const_none;
    }
    for (index = 0; index < n_args; index++) {
        res = aw_rmdir(mp_obj_str_get_str(arg[index]));
        if (res < 0){
            mp_raise_OSError(res);
        }
    }
    // TODO  recursive deletion
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR(mp_posix_rmdir_obj, 0, mp_posix_rmdir);

STATIC const qstr os_stat_info_fields[10] = {
        MP_QSTR_st_mode,
        MP_QSTR_st_ino,
        MP_QSTR_st_dev,
        MP_QSTR_st_nlink,
        MP_QSTR_st_uid,
        MP_QSTR_st_gid,
        MP_QSTR_st_size,
        MP_QSTR_st_atime,
        MP_QSTR_st_mtime,
        MP_QSTR_st_ctime,
};
mp_obj_t mp_posix_stat(mp_obj_t path_in) {

    struct aw_stat sbuf;
    const char *createpath = mp_obj_str_get_str(path_in);
    mp_obj_t items[10];
    int res = aw_stat(createpath,&sbuf);
    if (res < 0) {
        mp_raise_OSError(res);
    }else{
        items[0] = MP_OBJ_NEW_SMALL_INT(sbuf.st_mode);
        items[1] = MP_OBJ_NEW_SMALL_INT(sbuf.st_ino);
        items[2] = MP_OBJ_NEW_SMALL_INT(sbuf.st_dev);
        items[3] = MP_OBJ_NEW_SMALL_INT(sbuf.st_nlink);
        items[4] = MP_OBJ_NEW_SMALL_INT(sbuf.st_uid);
        items[5] = MP_OBJ_NEW_SMALL_INT(sbuf.st_gid);
        items[6] = MP_OBJ_NEW_SMALL_INT(sbuf.st_size);
        items[7] = MP_OBJ_NEW_SMALL_INT(sbuf.st_atim.tv_sec);
        items[8] = MP_OBJ_NEW_SMALL_INT(sbuf.st_mtim.tv_sec);
        items[9] = MP_OBJ_NEW_SMALL_INT(sbuf.st_ctim.tv_sec);
    }

    return mp_obj_new_attrtuple(os_stat_info_fields,10,items);
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_posix_stat_obj, mp_posix_stat);


mp_import_stat_t mp_posix_import_stat(const char *path) {

    struct aw_stat stat_buf;
    char full_path[AW_IO_PATH_LEN_MAX];
    assert(!aw_io_full_path(full_path, path));

    if (aw_stat(full_path, &stat_buf) == 0) {
        if (S_ISDIR(stat_buf.st_mode)) {
            return MP_IMPORT_STAT_DIR;
        } else {
            return MP_IMPORT_STAT_FILE;
        }
    } else {
        return MP_IMPORT_STAT_NO_EXIST;
    }
}
#endif //MICROPY_UOS_FILE
