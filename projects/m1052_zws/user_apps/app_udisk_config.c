/*
 * sys_config.c
 *
 *  Created on: 2019年8月10日
 *      Author: wengyedong
 */


#include "aworks.h"
#include "aw_nvram.h"
#include "aw_crc.h"
#include "aw_delay.h"
#include "aw_mem.h"
#include "aw_ini_lib.h"
#include "aw_buzzer.h"
#include "fs/aw_blk_dev.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"
#include "app_config.h"
#include "string.h"
#include "aw_task.h"
#include "aw_vdebug.h"

#define __MOUT_POINT  "/uda"
#define __CONFIG_FILE "/zws-config.ini"
#define __DISK  "/dev/h0-d1-0"


aw_local void __printf_zws_devinfo (struct app_sys_config* p_cfg)
{
//    aw_kprintf("\r\n");
    aw_kprintf("ssid:%s\r\n", p_cfg->net->ssid);
    aw_kprintf("pswd:%s\r\n", p_cfg->net->pswd);
//    aw_kprintf("\r\n");
    aw_kprintf("type:%s\r\n", p_cfg->zws_devinfo->type);
    aw_kprintf("id  :%s\r\n", p_cfg->zws_devinfo->id);
    aw_kprintf("key :%s\r\n", p_cfg->zws_devinfo->key);
}


aw_local void __blk_evt_cb (const char *name, int event, void *p_arg)
{
    if (strcmp(name, __DISK)) {
        return;
    }

    if (event == AW_BD_EVT_INJECT) {
        aw_semb_give((aw_semb_id_t)p_arg);
    }
}


void *app_udisk_config (void *p_arg)
{
    aw_err_t        ret = AW_OK;
    int             fd = -1;
    char           *p_buf = NULL, *ptr;
    struct aw_stat  fsta;
    int             size = 0;
    AW_SEMB_DECL(   semb);
    aw_semb_id_t    semb_id = NULL;
    int             blk_idx = -1;

    struct app_sys_config *p_cfg = (struct app_sys_config *)p_arg;


    AW_CRC_DECL(crc32,
                32,
                0x04c11db7,
                0xFFFFFFFF,
                AW_FALSE,
                AW_TRUE,
                0xFFFFFFFF);

    /* 初始化CRC计算单元 */
    ret =  AW_CRC_INIT(crc32,
                       crctable32_04c11db7,
                       AW_CRC_FLAG_AUTO);
    if (ret != AW_OK) {
        goto __failed;
    }

    /* 进入配置模式 */
    semb_id = AW_SEMB_INIT(semb, 0, AW_SEM_Q_PRIORITY);
    if (semb_id == NULL) {
        ret = -AW_ENOMEM;
        goto __failed;
    }

    /* 注册一个U盘等待事件 */
    ret = aw_blk_dev_event_register(__blk_evt_cb, semb_id, &blk_idx);
    if (ret != AW_OK) {
        goto __failed;
    }

    while (1) {
        if (ret != AW_OK) {
            aw_buzzer_beep(1000);
        }
//        aw_kprintf("\r\nPlease insert an U_Disk device......\r\n");
        /* 等待U盘接入 */
        aw_semb_take(semb_id, AW_WAIT_FOREVER);

        p_cfg->net->connect_status = WIFI_DISCONNECT;   /* 设置wifi为断开状态，停止上报数据。 */

        if (fd >= 0) {
            aw_close(fd);
            fd = -1;
        }
        aw_umount(__MOUT_POINT, 0);

        /* 挂载U盘 */
        ret = aw_mount(__MOUT_POINT, __DISK, "vfat", 0);
        if (ret != AW_OK) {
            continue;
        }

        /* 打开配置文件 */
        fd = aw_open(__MOUT_POINT __CONFIG_FILE, O_RDONLY, 0777);
        if (fd < 0) {
            ret = fd;
            continue;
        }

        ret = aw_fstat(fd, &fsta);
        if (ret != AW_OK) {
            continue;
        }
        if (fsta.st_size == 0) {
            ret = -AW_ENODATA;
            continue;
        }

        /* 分配内存 */
        if ((p_buf != NULL) && fsta.st_size > size) {
            aw_mem_free(p_buf);
            p_buf = NULL;
        }

        if (p_buf == NULL) {
            p_buf = (char *)aw_mem_alloc(fsta.st_size + 4);
            if (p_buf == NULL) {
                ret = -AW_ENOMEM;
                continue;
            }
        }
        size = fsta.st_size;

        /* 读取配置文件数据 */
        ret = aw_read(fd, p_buf, size);
        if ((ret < 0) || ret == 0) {
            ret = -AW_ENODATA;
            continue;
        }

        p_buf[ret] = '\0';
        ret++;

        /* 解析INI文件字段 */
        /*[zws] type*/
        ptr = aw_ini_read_file_key(p_buf, ret, "zws", "type");
        if (ptr == NULL) {
            ret = -AW_ENOENT;
            continue;
        }
        strncpy(p_cfg->zws_devinfo->type, ptr, sizeof(p_cfg->zws_devinfo->type));

        /*[zws] id*/
        ptr = aw_ini_read_file_key(p_buf, ret, "zws", "id");
        if (ptr == NULL) {
            ret = -AW_ENOENT;
            continue;
        }
        strncpy(p_cfg->zws_devinfo->id, ptr, sizeof(p_cfg->zws_devinfo->id));

        /*[zws] key*/
        ptr = aw_ini_read_file_key(p_buf, ret, "zws", "key");
        if (ptr == NULL) {
            ret = -AW_ENOENT;
            continue;
        }
        strncpy(p_cfg->zws_devinfo->key, ptr, sizeof(p_cfg->zws_devinfo->key));

        /*[wifi] ssid*/
        ptr = aw_ini_read_file_key(p_buf, ret, "wifi", "ssid");
        if (ptr == NULL) {
            ret = -AW_ENOENT;
            continue;
        }
        strncpy(p_cfg->net->ssid, ptr, sizeof(p_cfg->net->ssid));

        /*[wifi] password*/
        ptr = aw_ini_read_file_key(p_buf, ret, "wifi", "password");
        if (ptr == NULL) {
            ret = -AW_ENOENT;
            continue;
        }
        strncpy(p_cfg->net->pswd, ptr, sizeof(p_cfg->net->pswd));

        /* 计算wifi配置信息CRC并保存到flash中 */
        AW_CRC_CAL(crc32,
                   (uint8_t*)p_cfg->net,
                   sizeof(p_cfg->net->ssid) + sizeof(p_cfg->net->pswd));
        p_cfg->net->crc = AW_CRC_FINAL(crc32);

        ret = aw_nvram_set("zws-config",
                            0,
                            (char *)p_cfg->net,
                            NVRAM_NETINFO_ADDR,
                            sizeof(p_cfg->net->ssid) +
                            sizeof(p_cfg->net->pswd) +
                            sizeof(p_cfg->net->crc));
        if (ret != AW_OK) {
            aw_kprintf("nvram set wifi config error!\r\n");
            goto __failed;
        }

        /* 计算mqtt云设备配置信息CRC并保存到flash中 */
        AW_CRC_CAL(crc32,
                   (uint8_t*)p_cfg->zws_devinfo,
                   sizeof(*p_cfg->zws_devinfo) - sizeof(p_cfg->zws_devinfo->crc));
        p_cfg->zws_devinfo->crc = AW_CRC_FINAL(crc32);

        ret = aw_nvram_set("zws-config",
                            0,
                            (char *)p_cfg->zws_devinfo,
                            NVRAM_DEVINFO_ADDR,
                            sizeof(*p_cfg->zws_devinfo));
        if (ret != AW_OK) {
            aw_kprintf("nvram set zws config error!\r\n");
            goto __failed;
        }

        aw_kprintf("\r\nNew ZWS Config Info Is :\r\n");
        __printf_zws_devinfo (p_cfg);
        aw_buzzer_beep(100);
        aw_mdelay(100);
        aw_buzzer_beep(100);

        aw_kprintf("\r\nConfig Information Saved OK. Please Reset The System To Restart.\r\n");
        while (1) {
            aw_mdelay(100);
        }
        return AW_OK;
    }

__failed:
    if (p_buf) {
        aw_mem_free(p_buf);
    }
    if (fd) {
        aw_close(fd);
    }
    if (semb_id) {
        AW_SEMB_TERMINATE(semb);
    }
    if (blk_idx >= 0) {
        aw_blk_dev_event_unregister(blk_idx);
    }
    aw_umount(__MOUT_POINT, 0);

    return NULL;
}


aw_err_t app_udisk_workgroup (struct app_sys_config *p_cfg)
{
    aw_err_t    ret = AW_ERROR;
    aw_task_id_t    u_tsk = NULL;
    u_tsk = aw_task_create("udisk_task",
                            10,
                            4096,
                            app_udisk_config,
                            (void *)p_cfg);
    if (u_tsk != NULL) {
        ret = aw_task_startup(u_tsk);
        if (ret != AW_OK) {
            aw_kprintf("\r\nU-Disk workgroup startup failed.\r\n");
            return AW_ERROR;
        }
        return AW_OK;
    }
    return AW_ERROR;
}



aw_err_t app_get_cfginfo(struct app_sys_config *p_cfg)
{
    aw_err_t        ret = AW_OK;
    uint32_t        crc;

    /* 声明一个CRC计算单元 */
    AW_CRC_DECL(crc32,
                32,
                0x04c11db7,
                0xFFFFFFFF,
                AW_FALSE,
                AW_TRUE,
                0xFFFFFFFF);

    /* 初始化CRC计算单元 */
    ret =  AW_CRC_INIT(crc32,
                       crctable32_04c11db7,
                       AW_CRC_FLAG_AUTO);
    if (ret != AW_OK) {
        aw_kprintf("CRC_INIT failed!\r\n");
        goto __failed;
    }

    /* 读取系统保存的wifi配置 */
    ret = aw_nvram_get("zws-config",                /* nvram卷名 */
                        0,                          /* 单元号 */
                        (char *)p_cfg->net,         /* 接收数据缓存 */
                        NVRAM_NETINFO_ADDR,         /* 卷内偏移地址 */
                        sizeof(p_cfg->net->ssid) +  /* size */
                        sizeof(p_cfg->net->pswd) +
                        sizeof(p_cfg->net->crc));
    if (ret != AW_OK) {
        aw_kprintf("nvram get net config error\r\n");
        p_cfg->cfg_sta = INVALID;
        goto __failed;
    }

    /* 计算CRC */
    AW_CRC_CAL(crc32,
               (uint8_t*)p_cfg->net,
               sizeof(p_cfg->net->ssid) + sizeof(p_cfg->net->pswd));
    crc = AW_CRC_FINAL(crc32);
    if (crc != p_cfg->net->crc) {
        p_cfg->cfg_sta = INVALID;
        goto __invalid;
    }

    /* 读取系统保存的云设备信息 */
    ret = aw_nvram_get("zws-config",                    /* nvram卷名 */
                        0,                              /* 单元号 */
                        (char *)p_cfg->zws_devinfo,     /* 接收数据缓存 */
                        NVRAM_DEVINFO_ADDR,             /* 卷内偏移地址 */
                        sizeof(*p_cfg->zws_devinfo));   /* size */

    if (ret != AW_OK) {
        aw_kprintf("nvram get zws_device config error\r\n");
        p_cfg->cfg_sta = INVALID;
        goto __failed;
    }

    /* 计算CRC */
    AW_CRC_CAL(crc32,
               (uint8_t*)p_cfg->zws_devinfo,
               sizeof(*p_cfg->zws_devinfo) -        /* size */
               sizeof(p_cfg->zws_devinfo->crc));
    crc = AW_CRC_FINAL(crc32);
    if (crc != p_cfg->zws_devinfo->crc) {
        p_cfg->cfg_sta = INVALID;
        goto __invalid;
    }

    aw_kprintf("\r\nCurrent ZWS Config Info Is :\r\n");
    __printf_zws_devinfo (p_cfg);

    p_cfg->cfg_sta = VALID;
    return AW_OK;

__invalid:
    aw_kprintf("\r\nZWS config info saved in flash was invalid.\r\n"
                   "Plese use U-Disk or shell to config!\r\n");

__failed:

    return AW_ERROR;
}





