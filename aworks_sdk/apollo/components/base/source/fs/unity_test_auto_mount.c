/*
 * gw_unity_test.c
 *
 *  Created on: 2019年06月28日
 *      Author: anu
 */

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_shell.h"
#include "aw_assert.h"
#include "aw_system.h"

#include "string.h"
#include "stdio.h"

#include "unity.h"
#include "unity_fixture.h"
#include <setjmp.h>
#include <stdio.h>

#include "aw_auto_mount.h"

#include "io/sys/aw_stat.h"
#include "io/aw_fcntl.h"
#include "fs/aw_mount.h"
#include "fs/aw_blk_dev.h"

#include "mtd/aw_mtd.h"

/*****************************************************************************/
/* 宏定义 */
#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {\
         __log(color, PFX, __LINE__, fmt, ##__VA_ARGS__);\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%4d] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)


#define PFX  "UNITY_TEST_NET"

#define __TIMEOUT_MSEC(tick_tmp, timeout) \
    for (tick_tmp = aw_sys_tick_get() + aw_ms_to_ticks(timeout); \
            aw_sys_tick_get() < tick_tmp; )

/* Caculate the MS interval time of current tick and given tick. */
#define __INTERVAL_MS_GET(tick)  aw_ticks_to_ms(aw_sys_tick_get() - (tick))

//#define UNITY_TEST_SKIP_SUPPORT

#ifdef  UNITY_TEST_SKIP_SUPPORT
#define unity_test_is_skip()    AW_FALSE
#define unity_test_set_skip()
#define unity_test_clr_skip()
#else
extern void unity_test_clr_skip (void);
extern aw_bool_t unity_test_is_skip (void);
#endif

/*****************************************************************************/
/* 变量定义 */
/*****************************************************************************/
/* 申明 */
int aw_mount_foreach (int (*pfn)(struct aw_mount_info *info, void *arg),
                      void *arg);
aw_err_t aw_auto_mount_tbl_add (aw_auto_mount_cfg_t *p_arr);
/*****************************************************************************/
static void __log (void         *p_color,
                   const char   *func_name,
                   int           line,
                   const char   *fmt, ...)
{
    char        log_buf[512 + 1];
    uint16_t    len;
    va_list     ap;

    va_start( ap, fmt );
    len = aw_vsnprintf((char *)log_buf, sizeof(log_buf), fmt, ap);
    va_end( ap );
    if (len >= sizeof(log_buf)) {
        aw_assert(len < sizeof(log_buf) - 1);
    }

#ifdef __VIH_DEBUG
    extern int aw_shell_printf_with_color (aw_shell_user_t *p_user,
                                           const char *color,
                                           const char *func_name,
                                           const char *fmt, ...);
    aw_shell_printf_with_color(AW_DBG_SHELL_IO, p_color, func_name, log_buf);
#else
    AW_INFOF((log_buf));
#endif
}


/******************************************************************************/

/******************************************************************************/
TEST_GROUP(auto_mount);

TEST_SETUP(auto_mount)
{
}

TEST_TEAR_DOWN(auto_mount)
{
    unity_test_clr_skip();
}
/******************************************************************************/
TEST(auto_mount, __check_macro)
{
#include "aw_prj_params.h"

#ifndef AW_COM_MTD_DEV
    aw_assert(AW_FALSE);
#endif

#ifndef AW_COM_MOUNT
    aw_assert(AW_FALSE);
#endif

#ifndef AW_COM_FS_LFFS
    aw_assert(AW_FALSE);
#endif

#ifndef AW_COM_FS_FATFS
    aw_assert(AW_FALSE);
#endif

#ifndef AW_COM_SDCARD
    aw_assert(AW_FALSE);
#endif
}

/******************************************************************************/
typedef struct {
    char *path;
    int   result;
} mount_check_arg_t;


/******************************************************************************/
static    uint8_t              wdata[4096];
static    uint8_t              rdata[4096];

static void __mtd_rw_test (char   *p_name)
{
    aw_err_t             ret;
    struct aw_mtd_info  *p_mtd;
    int                  i;
    size_t               real;
    struct aw_mtd_erase_info mtd_erase;

    p_mtd = aw_mtd_dev_find(p_name);
    aw_assert(p_mtd);
    aw_assert(sizeof(wdata) > p_mtd->write_size);

    if (p_mtd->type == AW_MTD_TYPE_NAND_FLASH) {
        mtd_erase.mtd      = p_mtd;
        mtd_erase.addr     = 0;
        mtd_erase.len      = p_mtd->erase_size;
        mtd_erase.callback = NULL;
        ret = aw_mtd_erase(p_mtd, &mtd_erase);
        aw_assert(ret == AW_OK);
    } else if (p_mtd->type == AW_MTD_TYPE_NOR_FLASH) {
        mtd_erase.mtd      = p_mtd;
        mtd_erase.addr     = 0;
        mtd_erase.len      = p_mtd->erase_size;
        mtd_erase.callback = NULL;
        ret = aw_mtd_erase(p_mtd, &mtd_erase);
        aw_assert(ret == AW_OK);
    } else {
        aw_assert(0);
    }

    /* Set the data for write. */
    memset(wdata, 0, sizeof(wdata));
    for (i = 0; i < p_mtd->write_size; i++) {
        wdata[i] = (uint8_t)i;
    }

    for (i = 0; i < 3; i++) {
        ret = aw_mtd_write(p_mtd,
                           i * p_mtd->write_size,
                           p_mtd->write_size,
                           &real,
                           wdata);
        aw_assert(AW_OK == ret);
    }

    for (i = 0; i < 3; i++) {
        ret = aw_mtd_read(p_mtd,
                          i * p_mtd->write_size,
                          p_mtd->write_size,
                          &real,
                          rdata);
        aw_assert(AW_OK == ret);

        ret = memcmp(rdata, wdata, p_mtd->write_size);
        aw_assert(0 == ret);
    }
}


static int __mount_check_hepler (struct aw_mount_info *info, void *arg)
{
    mount_check_arg_t *p_arg = arg;

    if (strcmp(info->dev_name, p_arg->path) == 0 ||
            strcmp(info->mnt_point, p_arg->path) == 0 ) {
        p_arg->result = 1;
        return 1;
    }
    return 0;
}


static void __test_auto_mount (aw_auto_mount_cfg_t *p_cfg_arr,
                               char *p_devname,
                               char *fs)
{
    aw_err_t ret;
    mount_check_arg_t    arg;
    memset(&arg, 0, sizeof(arg));

    /* 检查  sflash0 应为未挂载 */
    arg.path = p_devname;
    arg.result    = 0;
    aw_mount_foreach(__mount_check_hepler, &arg);
    aw_assert(arg.result == 0);

    ret = aw_auto_mount_tbl_add(p_cfg_arr);
    aw_assert(ret == AW_OK);

    arg.path = p_devname;
    arg.result    = 0;
    aw_mount_foreach(__mount_check_hepler, &arg);
    aw_assert(arg.result == 1);
}

/* 测试内容:测试自动挂载  /sflash0，挂载为 lffs
 * 测试结果:
 */
TEST(auto_mount, __sflash0)
{
    static aw_auto_mount_cfg_t __g_cfg[] = {
        { "/nf0", "/dev/mtd0",  "lffs", 0, 1},
        { "/nf1", "/dev/mtd1",  "lffs", 0, 1},
        { "/sf",  "/sflash0",   "lffs", 0, 1},
        { "/sd",  "/dev/sd0",   "vfat", 0, 1},
        { NULL, NULL, NULL, 0, 0}
    };

    /* 测试 mtd 读写  */
    __mtd_rw_test("/sflash0");
    __test_auto_mount(__g_cfg, "/sflash0", "lffs");
    aw_auto_mount_tbl_clear();
}

/* 测试内容:测试自动挂载  /sflash0 挂载为 yaffs
 * 测试结果:
 */
TEST(auto_mount, __sflash0_with_yaffs)
{
    static aw_auto_mount_cfg_t __g_cfg[] = {
        { "/nf0", "/dev/mtd0",  "lffs", 0, 1},
        { "/nf1", "/dev/mtd1",  "lffs", 0, 1},
        { "/sf",  "/sflash0",   "yaffs", 0, 1},
        { "/sd",  "/dev/sd0",   "vfat", 0, 1},
        { NULL, NULL, NULL, 0, 0}
    };

    aw_umount("/sflash0", 0);
    aw_mdelay(2000);

    /* 测试 mtd 读写  */
    __mtd_rw_test("/sflash0");
    __test_auto_mount(__g_cfg, "/sflash0", "yaffs");
    aw_auto_mount_tbl_clear();
}

/* 测试内容:测试自动挂载  "/dev/sd0"，sd卡是系统 启动前插好的
 * 测试结果:
 */
TEST(auto_mount, __sd0)
{
    static aw_auto_mount_cfg_t __g_cfg[] = {
        { "/nf0", "/dev/mtd0",  "lffs", 0, 1},
        { "/nf1", "/dev/mtd1",  "lffs", 0, 1},
        { "/sf",  "/sflash0",   "lffs", 0, 1},
        { "/sd",  "/dev/sd0",   "vfat", 0, 1},
        { "/u",  "/dev/h0-d1-0","vfat", 0, 1},
        { NULL, NULL, NULL, 0, 0}
    };

    __test_auto_mount(__g_cfg, "/dev/sd0", "vfat");
    aw_auto_mount_tbl_clear();
}

/* 测试内容:测试自动挂载 u 盘，u 盘是系统 启动前插好的
 * 测试结果:
 */
TEST(auto_mount, __udisk)
{
    static aw_auto_mount_cfg_t __g_cfg[] = {
        { "/nf0", "/dev/mtd0",      "lffs", 0, 1},
        { "/nf1", "/dev/mtd1",      "lffs", 0, 1},
        { "/sf",  "/sflash0",       "lffs", 0, 1},
        { "/sd",  "/dev/sd0",       "vfat", 0, 1},
        { "/u",   "/dev/h0-d1-0",   "vfat", 0, 1},
        { NULL, NULL, NULL, 0, 0}
    };

    __test_auto_mount(__g_cfg, "/dev/h0-d1-0", "vfat");
    aw_auto_mount_tbl_clear();
}

/* 测试内容:测试自动挂载u盘，系统启动时不插入 u盘，等待测试程序提示插入时才插入
 * 测试结果:
 */
TEST(auto_mount, __udisk_hotplug)
{
    static aw_auto_mount_cfg_t __g_cfg[] = {
        { "/nf0", "/dev/mtd0",      "lffs", 0, 1},
        { "/nf1", "/dev/mtd1",      "lffs", 0, 1},
        { "/sf",  "/sflash0",       "lffs", 0, 1},
        { "/sd",  "/dev/sd0",       "vfat", 0, 1},
        { "/u",   "/dev/h0-d1-0",   "vfat", 0, 1},
        { NULL, NULL, NULL, 0, 0}
    };

    struct aw_blk_dev   *p_blk;
    char                *p_name = "/dev/h0-d1-0";
    char                *p_mnt  = "/u";
    mount_check_arg_t    arg;
    aw_err_t             ret;

    /* Wait for Udisk to be plug out. */
    while (1) {
        p_blk = aw_blk_dev_get(p_name);
        if (!p_blk) {
            break;
        }
        __LOG_PFX(1, "Please remove Udisk device...");
        aw_mdelay(1000);
    }

    /* Waif to be unmount */
    while (1) {
        arg.path   = p_mnt;
        arg.result = 0;
        aw_mount_foreach(__mount_check_hepler, &arg);
        if (arg.result == 0) {
            break;
        }
        if (unity_test_is_skip()) {
            return;
        }
        __LOG_PFX(1, "Wait for %s to be unmounted ...", arg.path);
        aw_mdelay(1000);
    }

    /* Add a mount table. */
    ret = aw_auto_mount_tbl_add(__g_cfg);
    aw_assert(ret == AW_OK);

    p_blk = aw_blk_dev_get(p_name);
    aw_assert(!p_blk);

    while (1) {
        p_blk = aw_blk_dev_get(p_name);
        if (p_blk) {
            break;
        }
        __LOG_PFX(1, "Please insert Udisk device...");
        aw_mdelay(1000);
        if (unity_test_is_skip()) {
            return;
        }
    }

    while (1) {
        p_blk = aw_blk_dev_get(p_name);
        if (p_blk) {
            break;
        }
        __LOG_PFX(1, "Wait for device %s to mount VFAT automatically ...");
        aw_mdelay(1000);
        if (unity_test_is_skip()) {
            return;
        }
    }

    __LOG_PFX(1, "Device %s mount VFAT successfully.", p_name);
    aw_auto_mount_tbl_clear();
}

/* 测试内容: 测试 lffs 设备自动格式化，先擦除 mtd 设备
 * 测试结果:
 */
TEST(auto_mount, __auto_format_lffs_with_sflash0)
{
    static aw_auto_mount_cfg_t __g_cfg[] = {
        { "/nf0", "/dev/mtd0",  "lffs", 0, 1},
        { "/nf1", "/dev/mtd1",  "lffs", 0, 1},
        { "/sf",  "/sflash0",   "lffs", 0, 1},
        { "/sd",  "/dev/sd0",   "vfat", 0, 1},
        { NULL, NULL, NULL, 0, 0}
    };
    char                    *p_devname = "/sflash0";
    char                    *fs = "lffs";
    struct aw_fs_format_arg  fmt = {"awdisk", 0, 1};    /* Hard format. */

    aw_umount("/sflash0", 0);
    aw_mdelay(2000);

    /* 测试 mtd 读写  */
    __mtd_rw_test("/sflash0");

    /* 格式化磁盘, 确保没有 lffs 文件系统标志文件 */
    aw_make_fs(p_devname, fs, &fmt);

    __test_auto_mount(__g_cfg, p_devname, fs);
    aw_auto_mount_tbl_clear();
}

/* 测试内容:
 * 测试结果:
 */
TEST(auto_mount, __test)
{

}
/******************************************************************************/

TEST_GROUP_RUNNER(auto_mount)
{
    RUN_TEST_CASE(auto_mount, __check_macro);

    RUN_TEST_CASE(auto_mount, __sflash0);
//    RUN_TEST_CASE(auto_mount, __sflash0_with_yaffs);

//    RUN_TEST_CASE(auto_mount, __sd0);

    RUN_TEST_CASE(auto_mount, __udisk);
    RUN_TEST_CASE(auto_mount, __udisk_hotplug);

    RUN_TEST_CASE(auto_mount, __auto_format_lffs_with_sflash0);

    RUN_TEST_CASE(auto_mount, __test);
}
