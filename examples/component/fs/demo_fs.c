/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWorks 文件系统例程
 *
 * - 本例程展示了 AWorks 文件系统中大多数接口操作，以使用FAT文件系统操作SD卡为例。
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *      以及调试串口对应的宏和SD卡的宏
 *   2. 板子插上SD卡
 *   3. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象： 打印出创建文件、写文件、关闭文件、打开文件、读文件、文件重命名等操作信息。
 *
 *
 * \par 源代码
 * \snippet demo_fs.c src_fs
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_fs 文件系统
 * \copydoc demo_fs.c
 */

/** [src_fs] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_time.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_dirent.h"
#include "io/sys/aw_stat.h"
#include "aw_delay.h"
#include "fs/aw_blk_dev.h"
#include <string.h>


/*
 * 使用文件系统前注意：
 *
 * 1. 如果存储器之前没有被格式化，需要进行格式化（格式化一次就够了）
 *
 * 2. 使用文件系统接口(读写文件或文件夹等)前必须进行aw_mount，即挂载
 *
 * 3. 物理存储器：
 *    1) 以 SPI FLASH 为例，其mtd设备名字在aw_hwconf_spi_flash0.h文件中已指定，为"/flexspi_flash0",
 *       文件系统挂载时就需要这个名字
 *    2) 以 SD 卡为例，其块设备名字为"/dev/sd0"
 *    3) 以 NAND FLASH 为例，其mtd设备名字列表在awbl_hwconf_xxx_nandflash.h中指定（具体名字视情况而定）
 *    并且要在aw_prj_param.h中使能相应的物理存储器
 *
 * 4. 文件同时打开个数设置：在aw_prj_params.h文件中，配置宏：AW_CFG_FATFS_FILES 和 AW_CFG_IO_OPEN_FILES_MAX
 *       并且AW_CFG_IO_OPEN_FILES_MAX >= AW_CFG_FATFS_FILES
 *
 * 5. 文件系统接口类似于POSIX标准，接口对照表如下：
 *
 * 打开/创建   aw_open
 *
 * 关闭     aw_close
 *
 * 写文件   aw_write
 *
 * 读文件   aw_read
 *
 * 重命名   aw_rename
 *
 * 改变文件指针   aw_lseek
 *
 * 获取文件属性   aw_stat
 *
 * 目录创建   aw_mkdir
 *
 * 删除目录   aw_rmdir
 *
 * 搜索文件相关   aw_opendir, aw_readdir
 *
 * 删除文件   aw_unlink
 *
 * 文件截断   aw_truncate
 *
 */


/* 在FAT中，当设置不支持长文件名时，所有的文件名都是大写（包括扩展名），
 * 并且文件名必须符合8.3原则，即8个文件名字符，3个扩展名字符
 *
 * 目前已有文件系统 "vfat", "lffs", "txfs", 本例程使用vfat操作SD卡
 */
#define  TEST_FS_NAME           "vfat"
#define  TEST_STORER_NAME       "/dev/sd0"
#define  TEST_MOUNT_POINT       "/sda"
#define  TEST_FILE_NAME         "aworks.txt"
#define  TEST_FILE_NEW_NAME     "zlg.txt"
#define  TEST_DIR_NAME          "dir"


/**
 * \brief 格式化磁盘
 *
 * \return -1  错误
 *          0  成功
 */
aw_local int __fs_format (void)
{
    aw_err_t ret;

    /* 卷名为"awdisk"， 卷大小为4k  */
    struct aw_fs_format_arg fmt = {"awdisk", 1024 * 4, 0};

    /* 制作文件系统 ，将存储器名为 TEST_STORER_NAME 制作为 TEST_FS_NAME 类型的文件系统  */
    ret = aw_make_fs(TEST_STORER_NAME, TEST_FS_NAME, &fmt);
    if (ret != AW_OK) {
        aw_kprintf("make %s by %s failed %d\n", TEST_STORER_NAME, TEST_FS_NAME, ret);
        return -1;
    }
    aw_kprintf("make %s by %s OK\n", TEST_STORER_NAME, TEST_FS_NAME);
    return 0;
}

 /**
  * \brief 挂载磁盘
  *
  * \return -1  错误
  *          0  成功
  */
aw_local int __fs_mount (void)
{
    aw_err_t ret;

    /* 文件系统挂载 */
    ret = aw_mount(TEST_MOUNT_POINT, TEST_STORER_NAME, TEST_FS_NAME, 0);
    if (ret != AW_OK) {
        aw_kprintf("mount %s to %s failed %d!\n", TEST_STORER_NAME, TEST_MOUNT_POINT, ret);
        return -1;
    }
    aw_kprintf("mount OK.\n");
    return 0;
}

/**
 * \brief 写数据
 *
 * \return -1  错误
 *          0  成功
 */
aw_local int __file_write (void)
{
    int        fd;
    const char str[] = "this is aworks file test.";
    char       path[255];
    int        len, ret;

    /* 生成文件完整路径 */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NAME);

     /* 创建新文件 */
    fd = aw_open(path, O_RDWR | O_CREAT, 0777);
    if (fd < 0) {
        aw_kprintf("create file failed %d\n", fd);
        return -1;
    }
    aw_kprintf("create file %s ok.\n", TEST_FILE_NAME);

    len = strlen(str);

    /* 写入数据 */
    ret = aw_write(fd, str, len);
    if (ret != len) {
        aw_kprintf("write file %s failed.\n", TEST_FILE_NAME);
        ret = -1;
    } else {
        aw_kprintf("write file %s ok.\n", TEST_FILE_NAME);
        ret = 0;
    }

    /* 关闭文件，保存数据 */
    aw_close(fd);

    return ret;
}

 /**
  * \brief 读数据
  *
  * \return -1  错误
  *          0  成功
  */
 aw_local int __file_read (void)
 {
     int        fd;
     char       buf[32];
     char       path[255];
     int        ret;

     /* 生成文件完整路径 */
     sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NAME);

      /* 打开文件 */
     fd = aw_open(path, O_RDWR, 0777);
     if (fd < 0) {
         aw_kprintf("open file failed %d\n", fd);
         return -1;
     }
     aw_kprintf("open file %s ok.\n", TEST_FILE_NAME);

     /* 读取数据 */
     ret = aw_read(fd, buf, sizeof(buf));
     if (ret < 0) {
         aw_kprintf("read file %s failed.\n", TEST_FILE_NAME);
         ret = -1;
     } else {
         buf[ret] = '\0';
         aw_kprintf("read file %s ok.\n", TEST_FILE_NAME);
         aw_kprintf("read data: %s\n", buf);
         ret = 0;
     }

     /* 关闭文件，保存数据 */
     aw_close(fd);

     return ret;
 }

 /**
  * \brief 重命名
  *
  * \return -1  错误
  *          0  成功
  */
 aw_local int __file_rename (void)
 {
     char       old[255], new[255];
     int        ret;

     /* 生成文件完整路径 */
     sprintf(old, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NAME);
     sprintf(new, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NEW_NAME);

     ret = aw_rename(old, new);
     if (ret != AW_OK) {
         aw_kprintf("%s rename to %s failed.\n", TEST_FILE_NAME, TEST_FILE_NEW_NAME);
         return -1;
     }

     aw_kprintf("%s rename to %s ok.\n", TEST_FILE_NAME, TEST_FILE_NEW_NAME);
     return 0;
 }

/**
* \brief 获取文件属性
*
* \return -1  错误
*          0  成功
*/
aw_local int __file_stat (void)
{
    struct aw_stat  sta;
    char            buf[255];
    int             ret;
    struct aw_tm    tm;

    /* 生成文件完整路径 */
    sprintf(buf, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NAME);

    ret = aw_stat(buf, &sta);
    if (ret != AW_OK) {
        aw_kprintf("get %s stat failed.\n", TEST_FILE_NAME);
        return -1;
    }

    /* 获取文件创建时间 */
    aw_time_to_tm(&sta.st_mtim.tv_sec, &tm);
    if (difftime(aw_time(NULL), sta.st_mtim.tv_sec) > (365 * 24 * 60 * 60 / 2 /* 6 months */)) {
        strftime(buf, sizeof(buf), "%b %d  %Y", (struct tm*)&tm);
    } else {
        strftime(buf, sizeof(buf), "%b %d  %H:%M", (struct tm*)&tm);
    }

    /* 输出文件信息 */
    aw_kprintf("file %s stat:\n", TEST_FILE_NAME);

    aw_kprintf("%c%c%c%c%c%c%c%c%c%c %2d user AWorks %9u %s %s\n",
               (S_ISLNK(sta.st_mode) ? ('l') : (S_ISDIR(sta.st_mode) ? ('d') : ('-'))),
               (sta.st_mode & S_IRUSR) ? ('r') : ('-'),
               (sta.st_mode & S_IWUSR) ? ('w') : ('-'),
               (sta.st_mode & S_IXUSR) ? ('x') : ('-'),
               (sta.st_mode & S_IRGRP) ? ('r') : ('-'),
               (sta.st_mode & S_IWGRP) ? ('w') : ('-'),
               (sta.st_mode & S_IXGRP) ? ('x') : ('-'),
               (sta.st_mode & S_IROTH) ? ('r') : ('-'),
               (sta.st_mode & S_IWOTH) ? ('w') : ('-'),
               (sta.st_mode & S_IXOTH) ? ('x') : ('-'),
               sta.st_nlink,
               (int)sta.st_size,
               buf,
               TEST_FILE_NAME);

    return 0;
}

/**
* \brief 文件指针索引
*
* \return -1  错误
*          0  成功
*/
aw_local int __file_seek (void)
{
    int        fd = -1;
    char       buf[32];
    char       path[255];
    off_t      offset;
    int        ret;

    /* 生成文件完整路径 */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NAME);

     /* 打开文件 */
    fd = aw_open(path, O_RDWR, 0777);
    if (fd < 0) {
        goto __failed;
    }
    aw_kprintf("open file %s ok.\n", TEST_FILE_NAME);

    /* 设置文件指针到偏移位置5 */
    offset = aw_lseek(fd, 5, SEEK_SET);
    if (offset < 0) {
        goto __failed;
    }
    aw_kprintf("file data seek ok.\n", TEST_FILE_NAME);

    /* 读取数据 */
    ret = aw_read(fd, buf, sizeof(buf));
    if (ret < 0) {
        goto __failed;
    } else {
        buf[ret] = '\0';
        aw_kprintf("read data: %s\n", buf);
        ret = 0;
    }

    /* 关闭文件，保存数据 */
    aw_close(fd);
    return 0;

__failed:
    if (fd >= 0) {
        aw_close(fd);
    }
    aw_kprintf("file data seek failed.\n");
    return -1;
}

/**
* \brief 文件删除
*
* \return -1  错误
*          0  成功
*/
aw_local int __file_delete (void)
{
    char       path[255];
    int        ret;

    /* 生成文件完整路径 */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NEW_NAME);

    ret = aw_unlink(path);
    if (ret != AW_OK) {
        aw_kprintf("file delete failed.\n");
        return -1;
    }

    aw_kprintf("file delete ok.\n");
    return 0;
}

/**
* \brief 新建文件夹
*
* \return -1  错误
*          0  成功
*/
aw_local int __dir_create (void)
{
    char       path[255];
    int        ret;

    /* 生成文件完整路径 */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_DIR_NAME);

    ret = aw_mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
    if (ret != AW_OK) {
        aw_kprintf("directory create failed.\n");
        return -1;
    }

    aw_kprintf("directory create ok.\n");
    return 0;
}

/**
* \brief 删除文件夹
*
* \return -1  错误
*          0  成功
*/
aw_local int __dir_delete (void)
{
    char       path[255];
    int        ret;

    /* 生成文件完整路径 */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_DIR_NAME);

    ret = aw_rmdir(path);
    if (ret != AW_OK) {
        aw_kprintf("directory delete failed.\n");
        return -1;
    }

    aw_kprintf("directory delete ok.\n");
    return 0;
}

/**
* \brief 查询文件夹
*
* \return -1  错误
*          0  成功
*/
aw_local int __dir_list (void)
{
    char                path[255];
    struct aw_dir      *p_dir;
    struct aw_dirent   *p_entry;

    /* 生成文件完整路径 */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_DIR_NAME);

    /* 打开目录*/
    p_dir = aw_opendir(path);
    if (p_dir == NULL) {
        aw_kprintf("open directory %s failed.\n", TEST_DIR_NAME);
        return -1;
    }

    /* 列出所有文件 */
    while((p_entry = aw_readdir(p_dir)) != NULL) {
        aw_kprintf("%s\n", p_entry->d_name);
    }

    /* 关闭目录 */
    aw_closedir(p_dir);

    return 0;
}

/**
 * \brief 文件系统 demo入口
 *
 * \param[in]  format   格式化选项（非0则进行格式化操作）
 *
 * \return 无
 */
void  demo_fs_entry (aw_bool_t format)
{
    aw_kprintf("\n\n\n**********************************************\n");
    aw_kprintf(" test file system function:\n");

    /* 等待系统识别到存储设备 */
    if (aw_blk_dev_wait_install(TEST_STORER_NAME, 3000)) {
        aw_kprintf("%s is not exist.\r\n", TEST_STORER_NAME);
        return;
    }

    /* 格式化存储器，谨慎操作 */
    if (format) {
        if (__fs_format() == -1) {
            return;
        }
    }

    /* 挂载磁盘，类似PC上将U盘变成一个G盘、I盘等一样 */
    if (__fs_mount() == -1) {
        return;
    }

    /* 写入数据 */
    if (__file_write() == -1) {
        goto __exit;
    }

    /* 读取数据 */
    if (__file_read() == -1) {
        goto __exit;
    }

    /* 索引文件数据 */
    if (__file_seek() == -1) {
        goto __exit;
    }

    /* 获取文件属性 */
    if (__file_stat() == -1) {
        goto __exit;
    }

    /* 重命名 */
    if (__file_rename() == -1) {
        goto __exit;
    }

    /* 删除文件 */
    if (__file_delete() == -1) {
        goto __exit;
    }


    /* 新建文件夹 */
    if (__dir_create() == -1) {
        goto __exit;
    }

    /* 查询文件夹 */
    if (__dir_list() == -1) {
        goto __exit;
    }

    /* 删除文件夹 */
    if (__dir_delete() == -1) {
        goto __exit;
    }

__exit:
    aw_umount(TEST_MOUNT_POINT, 0);
}

/** [src_fs] */

/* end of file */
