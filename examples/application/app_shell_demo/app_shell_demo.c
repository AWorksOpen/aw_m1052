/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief shell系统
 *
 * - 操作步骤：
 *   1. 在命令行输入".test"指令并回车进入".test"命令集，
 *      按下键盘上的 Tab 键，查看".test"下的所有指令
 *      - "auto"指令可以自动执行以下指令：
 *              "led 0 on"
 *              "led 0 off"
 *              "buzzer on"
 *              "buzzer off"
 *              "sflash -w 0 AWorks\\ test."
 *              "sflash -r 0 12"
 *              "rtc"
 *              "lcd -c"
 *              "lcd -h"
 *              "lcd -v"
 *        在"auto"自动执行操作下，需打开以上指令对应的外设，具体可查看app_xxx.c中信息
 *     - 具体单个指令使用方法，可查看对应app_xxx.c例程中注释信息
 *
 * \par 源代码
 * \snippet app_shell_demo.c app_shell_demo
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

#include "aworks.h"                     /* 此头文件必须被首先包含 */
#include "aw_shell.h"
#include "aw_delay.h"
#include "app_config.h"

//#ifdef APP_LCD
//#include "GUI.h"
//#endif

extern int app_version (int argc, char **argv, struct aw_shell_user *p_user);
extern int app_led (int argc, char **argv, struct aw_shell_user *p_user);
extern int app_buzzer (int argc, char **argv, struct aw_shell_user *p_user);
extern int app_sflash (int argc, char **argv, struct aw_shell_user *p_user);
extern int app_rtc (int argc, char **argv, struct aw_shell_user *p_user);
extern int app_cat (int argc, char **argv, struct aw_shell_user *p_user);
extern int app_echo (int argc, char **argv, struct aw_shell_user *p_user);
extern int app_net (int argc, char **argv, struct aw_shell_user *p_user);
extern int app_lcd (int argc, char **argv, struct aw_shell_user *p_user);
extern int app_system_restart (int argc, char **argv, struct aw_shell_user *p_user);
int app_auto (int argc, char **argv, struct aw_shell_user *p_user);

#ifdef APP_FTPD
extern int app_ftpd (int argc, char **argv, struct aw_shell_user *p_user);
#endif

#ifdef APP_MIFARE
extern int app_mifare (int argc, char **argv, struct aw_shell_user *p_user);
#endif

#ifdef APP_WIFI
extern int app_wifi (int argc, char **argv, struct aw_shell_user *p_user);
#endif


/**
 * 子命令集初始化函数
 */
void entry_init(struct aw_shell_user *user)
{
//#ifdef APP_LCD
//    GUI_Init();
//#endif

#ifdef APP_WIFI
    void test_wifi_init (void);
    test_wifi_init();
#endif

}


/******************************************************************************/
aw_local struct aw_shell_cmdset __g_app_cmdset = {
    .p_enter_name   = ".test",         /* 命令集进入名字 */
    .p_exit_name    = ".exit",         /* 命令集退出  */
    .p_man          = "test demo",
    .pfn_always_run = NULL,
    .pfn_entry_init = entry_init,
};




/******************************************************************************/
aw_local const struct aw_shell_cmd __g_app_cmdset_cmds[] = {
    {app_version, "version",  "get the SDK packet version \r\n"},
    {app_led,     "led",      "control LED\r\n"
                              "[id][state]                 -id    0/1   \r\n"
                              "                            -state on/off\r\n"
                              "e.g. led 0 on                            \r\n"},

    {app_buzzer,  "buzzer",   "control buzzer\r\n"
                              "[state]                     -state on/off\r\n"
                              "e.g. buzzer on                           \r\n"},

    {app_sflash,  "sflash",   "SPI Flash read/write test\r\n"
                              "[-w offset data]          -write data to flash \r\n"
                              "[-r offset length]        -read data from flash\r\n"
                              "e.g. sflash -w 0 AWorks                        \r\n"
                              "e.g. sflash -r 0 6                             \r\n"},


    {app_rtc,     "rtc",      "get/set RTC value.\r\n"
                              "[]                             -get RTC        \r\n"
                              "[-d date]                      -set date to RTC\r\n"
                              "[-t time]                      -set time to RTC\r\n"
                              "e.g. rtc -d 2018-05-26 -t 17:00:30             \r\n"},
    {app_echo,    "echo",     "disk write test.\r\n"
                              "[echo][data] > [file]      -write data to disk file \r\n"
                              "[echo][data] > [file]      -append data to disk file \r\n"
                              "e.g. echo \"AWorks \" > test.txt                  \r\n"
                              "e.g. echo \"How are you?\" >> test.txt             \r\n"},
    {app_cat,     "cat",      "disk read test.\r\n"
                              "[cat][file]      -read data to disk file \r\n"
                              "e.g. cat test.txt                  \r\n"},
    {app_net,     "ipconfig", "net IP config.\r\n"
                              "[]                        -show net config                       \r\n"
                              "[-select id]              -select net device                     \r\n"
                              "[-config ip mask gateway] -change net config                     \r\n"
                              "[-dhcpc state]     -turn on/off DHCP client                  \r\n"
                              "e.g. ipconfig -select 0                                          \r\n"
                              "e.g. ipconfig -config 192.168.12.236 255.255.255.0 192.168.12.254\r\n"
                              "e.g. ipconfig -dhcpc on                                           \r\n"},

   {app_lcd,      "lcd",      "test LCD and touch.\r\n"
                              "[-c] Set the color of the screen.\r\n"
                              "[-b 0~100]  Set the brightness of the screen.\r\n"
                              "[-j]        Screen calibration.              \r\n"
                              "e.g. lcd -c                                  \r\n"
                              "e.g. lcd -b 0                                \r\n"
                              "e.g. lcd -j                                  \r\n"},

#ifdef APP_FTPD
   {app_ftpd,     "ftpd",     "test ftpd server.\r\n"
                              "<path> <port> [usrname] [password].\r\n"
                              "e.g. ftpd / 21                               \r\n"
                              "e.g. ftpd / 21 AWorks 1234                   \r\n"},
#endif

#ifdef APP_MIFARE
    {app_mifare,  "mifare",   "Mifare card test(Ctrl+C exit).\r\n"},
#endif

#ifdef APP_WIFI
    {app_wifi,     "wifi",    "WiFi test\r\n"
                              "[scan] -scan wifi channels                      \r\n"
                              "[scan] [passive] -scan all wifi channels        \r\n"
                              "[join] <ssid> <key> [ip] [netmask gateway] [dns]\r\n"
                              "       -join an AP. DHCP assumed if no IP address\r\n"
                              "       provided.                                \r\n"
                              "[getapinfo] -get specify AP config.             \r\n"
                              "[leave] -leave an AP.                           \r\n"
                              "[startap] <ssid> <open|wpa2|wpa2_aes> <key>     \r\n"
                              "          <channel> [ip netmask gateway]        \r\n"
                              "          -bring up local SoftAP.               \r\n"
                              "[stopap] -shutdown local soft AP.               \r\n"
                              "[maxassoc] -get the maximum number of associations\r\n"
                              "           supported by all interfaces (STA and \r\n"
                              "           Soft AP).If the STA interface is     \r\n"
                              "           associated then the Soft AP can support\r\n"
                              "           (max_assoc - 1) associated clients.  \r\n"
                              "[getstalist] -Get list of associated clients.   \r\n"
                              "[getlinkstatus] -Get link status of the Wi-Fi   \r\n"
                              "                interface.                      \r\n"
                              "[settxpwr] <dbm> -Set the tx power.             \r\n"
                              "[gettxpwr] -Get the tx power.                   \r\n"
                              "[setcc] -Set the country code.                  \r\n"
                              "[getcc] -Get the country code.                  \r\n"
                              "[set11n] [1|0] Enable or disable 11n support.   \r\n"
                              "[powersave] [1|0] Enables powersave mode.       \r\n"
                              "[rate] STA Interface or AP Interface.           \r\n"
                              "       Get the current data rate for the provided\r\n"
                              "       interface (STA and Soft AP).             \r\n"
                              "[getch] <0|1> STA Interface or AP Interface.    \r\n"
                              "        Get the current channel on the WLAN radio\r\n"
                              "        (STA and Soft AP).                      \r\n"
                              "        On most WLAN devices this will get the  \r\n"
                              "        channel for both AP *AND* STA.          \r\n"
                              "[setch] [ch] <0|1> STA Interface or AP Interface.\r\n"
                              "        Set the current channel on the WLAN radio\r\n"
                              "        (STA and Soft AP).                      \r\n"
                              "        On most WLAN devices this will set the  \r\n"
                              "        channel for both AP *AND* STA.\r\n"
                              "[setmm] [1|0] Enable or disable monitor mode.   \r\n"
                              "[fbt] <0|1> STA Interface or AP Interface.      \r\n"
                              "      Get Fast BSS Transition over distribution \r\n"
                              "      system.                                   \r\n"
                              "[fbtcap] <0|1> STA Interface or AP Interface.   \r\n"
                              "         Get the value of WLFBT.                \r\n"
                              "[nics]  Show the nics message.                   \r\n"
                              "[drvlog] <start|stop> Enable or disable driver log.\r\n"
    },
#endif
    {app_auto,     "auto",    "auto test.\r\n"},
#ifdef APP_SYS_RST
    {app_system_restart, "sys_rst",  "system restart(by watchdog)\r\n"},
#endif
};




/******************************************************************************/
int app_auto (int argc, char **argv, struct aw_shell_user *p_user)
{
    /* 通过shell子系统调用shell命令子集的命令 */

    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "led 0 on");
    aw_mdelay(500);
    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "led 0 off");

    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "buzzer on");
    aw_mdelay(500);
    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "buzzer off");

    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "sflash -w 0 AWorks\\ test.");
    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "sflash -r 0 12");

    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "rtc");

    aw_shell_system("mount /sd0 /dev/sd0 vfat");
    aw_shell_system("cd sd0");

    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "echo \"How are you\" > test.txt");
    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "cat test.txt");

    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "lcd -c");
    aw_mdelay(500);
    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "lcd -h");
    aw_mdelay(500);
    aw_shell_system_ex(AW_DBG_SHELL_IO, ".test", "lcd -v");

    return AW_OK;
}




/******************************************************************************/
void app_shell_demo (void)
{
    /* 注册一个命令子集 */
    AW_SHELL_REGISTER_CMDSET(&__g_app_cmdset, __g_app_cmdset_cmds);
}
