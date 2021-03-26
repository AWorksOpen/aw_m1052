#include "aworks.h"
#include "aw_assert.h"
#include "aw_delay.h"
#include "aw_task.h"
#include "aw_shell.h"
#include "aw_led.h"
#include "string.h"
#include "app_config.h"
#include "aw_nvram.h"
#include "aw_crc.h"
#include "aw_mem.h"
#include "aw_vdebug.h"
#include "aw_buzzer.h"
#include "aw_list.h"


extern struct app __g_app;

aw_local void __printf_zws_devinfo (aw_shell_user_t *p_user,
                                    struct app_sys_config* p_cfg)
{
    aw_shell_color_set (p_user, AW_SHELL_FCOLOR_PINK);
    aw_shell_printf(p_user,"ssid:%s\r\n", p_cfg->net->ssid);
    aw_shell_printf(p_user,"pswd:%s\r\n", p_cfg->net->pswd);
    aw_shell_printf(p_user,"\r\n");
    aw_shell_printf(p_user,"type:%s\r\n", p_cfg->zws_devinfo->type);
    aw_shell_printf(p_user,"id  :%s\r\n", p_cfg->zws_devinfo->id);
    aw_shell_printf(p_user,"key :%s\r\n", p_cfg->zws_devinfo->key);
    aw_shell_color_set (p_user, AW_SHELL_FCOLOR_WHITE);
}

aw_local void __printf_zws_help_info (void)
{
    aw_kprintf("Input zws -t  <type>        Set zws device type\r\n"
               "          -i  <id>          Set zws device id\r\n"
               "          -k  <key>         Set zws device key\r\n"
               "          -s  <ssid>        Set wifi ssid\r\n"
               "          -p  <pswd>        Set wifi pswd\r\n"
               "          -d                Set zws device info done\r\n"
               "          -g                Get zws device info\r\n");
}


/**
 * \brief shell配置zws应用函数
 */
int __set_zws_devinfo (int                   argc,
                       char                 *argv[],
                       struct aw_shell_user *sh)
{
    char *ptr = NULL;
    aw_err_t    ret = 0;
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
        return AW_ERROR;
    }

    __g_app.p_cfg->net->connect_status = WIFI_DISCONNECT;   /* 设置wifi为断开状态，停止上报数据。 */

    while (1) {
        /* 如果参数形式是“-type [arg1]”的形式 */
        if (aw_shell_args_get(sh, "-t", 1) == AW_OK) {
            __g_app.p_cfg->cfg_sta = CHANGED;
            memset(__g_app.p_cfg->zws_devinfo->type, 0x00, sizeof(__g_app.p_cfg->zws_devinfo->type));
            ptr = aw_shell_str_arg(sh, 1);
            strncpy(__g_app.p_cfg->zws_devinfo->type, ptr, sizeof(__g_app.p_cfg->zws_devinfo->type));

            aw_shell_color_set (sh, AW_SHELL_FCOLOR_PINK);
            aw_shell_printf(sh,"type:%s ", __g_app.p_cfg->zws_devinfo->type);
            aw_shell_color_set (sh, AW_SHELL_FCOLOR_WHITE);
            aw_shell_printf(sh,"Set OK.\r\n");

        /* 如果参数形式是“-id [arg1] ”的形式 */
        } else if (aw_shell_args_get(sh, "-i", 1) == AW_OK) {
            __g_app.p_cfg->cfg_sta = CHANGED;
            memset(__g_app.p_cfg->zws_devinfo->id, 0x00, sizeof(__g_app.p_cfg->zws_devinfo->id));
            ptr = aw_shell_str_arg(sh, 1);
            strncpy(__g_app.p_cfg->zws_devinfo->id, ptr, sizeof(__g_app.p_cfg->zws_devinfo->id));

            aw_shell_color_set (sh, AW_SHELL_FCOLOR_PINK);
            aw_shell_printf(sh,"id  :%s ", __g_app.p_cfg->zws_devinfo->id);
            aw_shell_color_set (sh, AW_SHELL_FCOLOR_WHITE);
            aw_shell_printf(sh,"Set OK.\r\n");

        /* 如果参数形式是“-key [arg1]”的形式 */
        } else if (aw_shell_args_get(sh, "-k", 1) == AW_OK) {
            __g_app.p_cfg->cfg_sta = CHANGED;
            memset(__g_app.p_cfg->zws_devinfo->key, 0x00, sizeof(__g_app.p_cfg->zws_devinfo->key));
            ptr = aw_shell_str_arg(sh, 1);
            strncpy(__g_app.p_cfg->zws_devinfo->key, ptr, sizeof(__g_app.p_cfg->zws_devinfo->key));

            aw_shell_color_set (sh, AW_SHELL_FCOLOR_PINK);
            aw_shell_printf(sh,"key :%s ", __g_app.p_cfg->zws_devinfo->key);
            aw_shell_color_set (sh, AW_SHELL_FCOLOR_WHITE);
            aw_shell_printf(sh,"Set OK.\r\n");

        /* 如果参数形式是“-ssid [arg1]”的形式 */
        } else if (aw_shell_args_get(sh, "-s", 1) == AW_OK) {
            __g_app.p_cfg->cfg_sta = CHANGED;
            memset(__g_app.p_cfg->net->ssid, 0x00, sizeof(__g_app.p_cfg->net->ssid));
            ptr = aw_shell_str_arg(sh, 1);
            strncpy(__g_app.p_cfg->net->ssid, ptr, sizeof(__g_app.p_cfg->net->ssid));

            aw_shell_color_set (sh, AW_SHELL_FCOLOR_PINK);
            aw_shell_printf(sh,"ssid:%s ", __g_app.p_cfg->net->ssid);
            aw_shell_color_set (sh, AW_SHELL_FCOLOR_WHITE);
            aw_shell_printf(sh,"Set OK.\r\n");

        /* 如果参数形式是“-pwsd [arg1]”的形式 */
        } else if (aw_shell_args_get(sh, "-p", 1) == AW_OK) {
            __g_app.p_cfg->cfg_sta = CHANGED;
            memset(__g_app.p_cfg->net->pswd, 0x00, sizeof(__g_app.p_cfg->net->pswd));
            ptr = aw_shell_str_arg(sh, 1);
            strncpy(__g_app.p_cfg->net->pswd, ptr, sizeof(__g_app.p_cfg->net->pswd));

            aw_shell_color_set (sh, AW_SHELL_FCOLOR_PINK);
            aw_shell_printf(sh,"pswd:%s ", __g_app.p_cfg->net->pswd);
            aw_shell_color_set (sh, AW_SHELL_FCOLOR_WHITE);
            aw_shell_printf(sh,"Set OK.\r\n");

        /* 如果参数形式是-get 的形式 */
        } else if (aw_shell_args_get(sh, "-g", 0) == AW_OK) {
            __g_app.p_cfg->cfg_sta = CHANGED;
            __printf_zws_devinfo(sh, __g_app.p_cfg);

        /* 如果参数形式是--help 的形式 */
        } else if (aw_shell_args_get(sh, "--help", 0) == AW_OK) {
            __printf_zws_help_info();

        /* 如果参数形式是“-sta [arg1]”的形式 */
        } else if (aw_shell_args_get(sh, "-r", 0) == AW_OK) {
            __g_app.p_cfg->net->connect_status = WIFI_CONNECTED;   /* 设置wifi为连接状态，启动上报数据。 */

        } else if (aw_shell_args_get(sh, "-d", 0) == AW_OK) {
            if (__g_app.p_cfg->cfg_sta == CHANGED) {        /* 配置信息有修改，重新计算CRC值并保存到flash */

                /* 计算wifi配置信息CRC并保存到flash中 */
                AW_CRC_CAL(crc32,
                           (uint8_t*)__g_app.p_cfg->net,
                           sizeof(__g_app.p_cfg->net->ssid) + sizeof(__g_app.p_cfg->net->pswd));
                __g_app.p_cfg->net->crc = AW_CRC_FINAL(crc32);

                ret = aw_nvram_set("zws-config",
                                    0,
                                    (char *)__g_app.p_cfg->net,
                                    NVRAM_NETINFO_ADDR,
                                    sizeof(__g_app.p_cfg->net->ssid) +
                                    sizeof(__g_app.p_cfg->net->pswd) +
                                    sizeof(__g_app.p_cfg->net->crc));
                if (ret != AW_OK) {
                    aw_kprintf("nvram set wifi config error!\r\n");
                    return AW_ERROR;
                }

                /* 计算mqtt云设备配置信息CRC并保存到flash中 */
                AW_CRC_CAL(crc32,
                           (uint8_t*)__g_app.p_cfg->zws_devinfo,
                           sizeof(*__g_app.p_cfg->zws_devinfo) - sizeof(__g_app.p_cfg->zws_devinfo->crc));
                __g_app.p_cfg->zws_devinfo->crc = AW_CRC_FINAL(crc32);

                ret = aw_nvram_set("zws-config",
                                    0,
                                    (char *)__g_app.p_cfg->zws_devinfo,
                                    NVRAM_DEVINFO_ADDR,
                                    sizeof(*__g_app.p_cfg->zws_devinfo));
                if (ret != AW_OK) {
                    aw_kprintf("nvram set zws config error!\r\n");
                    return AW_ERROR;
                }

                aw_kprintf("\r\nCurrent ZWS Config Info Is :\r\n");
                __printf_zws_devinfo(sh, __g_app.p_cfg);
                aw_buzzer_beep(100);
                aw_mdelay(100);
                aw_buzzer_beep(100);

                aw_shell_color_set (sh, AW_SHELL_FCOLOR_GREEN);
                aw_shell_printf(sh,"Config Information Saved OK. Please Reset The System To Restart.");
                aw_shell_color_set (sh, AW_SHELL_FCOLOR_WHITE);

                __g_app.p_cfg->cfg_sta = DONE;
                break;

            } else if (__g_app.p_cfg->cfg_sta == VALID) {   /* 配置信息无修改且有效 ，更改配置状态为完成 */
                __g_app.p_cfg->cfg_sta = DONE;
                break;
            } else {
                aw_kprintf("config status error,line:%d\r\n", __LINE__);
            }

        /* 直接退出 */
        } else {
            break;
        }
    }

    return AW_OK;
}

/**
 * \brief shell 命令
 */
static const aw_shell_cmd_item_t __g_my_aworks_cmd[] = {
    {__set_zws_devinfo,    "zws",     "AWorks ZWS Application"}
};


/**
 * @\brief 通过shell配置zws设备
 */
aw_err_t app_shell_config (void)
{
    static aw_shell_cmd_list_t aworks_cmd_list;     /* 定义shell命令集 */

    /* 添加自定义shell命令  */
    AW_SHELL_REGISTER_CMDS(&aworks_cmd_list, __g_my_aworks_cmd);

    return AW_OK;
}

/** [src_shell] */

/* end of file */
