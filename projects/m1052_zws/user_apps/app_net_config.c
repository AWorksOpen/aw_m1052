/*
 * app_mynet.c
 *
 *  Created on: 2019年10月11日
 *      Author: zengqingyuhang
 */


#include "aworks.h"
#include "aw_wifi.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
//#include "net/aw_netif.h"
#include "aw_netif.h"
#include "stdio.h"
#include "aw_task.h"
#include "app_config.h"



aw_local aw_err_t __wifi_init (struct net_info * p_net)
{
    aw_err_t        ret;
    aw_wifi_id_t    wfid;
    aw_wifi_ssid_t  ssid;
    aw_wifi_key_t   pkey;
    uint8_t         timeout = 0;

    wfid = aw_wifi_dev_open(p_net->wifi_name);
    if (AW_WIFI_ID_INVALID == wfid) {
        aw_kprintf("No Wi-FI device found.\r\n");
        return -AW_ENODEV;
    }

    ssid.length = snprintf((char *)ssid.value, sizeof(ssid.value), "%s" , p_net->ssid);
    pkey.length = snprintf((char *)pkey.value, sizeof(pkey.value), "%s" ,p_net->pswd);

    p_net->connect_status = WIFI_DISCONNECT;
    timeout = 20;
    do {
        aw_wifi_leave(wfid);

        ret = aw_wifi_join(wfid,
                           NULL,
                           &ssid,
                           AW_WIFI_SECURITY_WPA2_AES_PSK,
                           &pkey);
        if (AW_OK != ret) {
            aw_kprintf("Join %s failed %d.", p_net->ssid, ret);
            aw_mdelay(200);
            timeout--;
        }
    } while (ret != AW_OK && timeout > 0);
    if (timeout == 0) {
        aw_kprintf("Join %s timeout!", p_net->ssid);
        goto __failed;
    }
    p_net->connect_status = WIFI_CONNECTED;

__failed:
    aw_wifi_dev_close(wfid);
    return ret;
}

aw_netif_ev_hdl_t  __net_link_event_handler (aw_netif_t *p_netif,
                                            aw_netif_event_t event,
                                            void *p_arg)
{
    struct net_info *p_net = (struct net_info *)p_arg;

    if (event == AW_NETIF_EVENT_LINK_DOWN) {
        p_net->connect_status = WIFI_DISCONNECT;
    } else if (event == AW_NETIF_EVENT_LINK_UP) {
        p_net->connect_status = WIFI_CONNECTED;
    }
    return NULL;
}

aw_local aw_err_t __netif_init (const char *dev,
                                int         auto_ip,
                                const char *ip,
                                const char *mask,
                                const char *gw,
                                struct net_info *p_net)
{
    aw_netif_t       *p_netif;
    aw_err_t          ret;
    struct in_addr    addr;
    aw_wifi_mac_t     mac;

    p_netif = aw_netif_dev_open(dev);
    if (p_netif == NULL) {
        aw_kprintf("No net device found.\r\n");
        return -AW_ENODEV;
    }

    ret = aw_netif_event_cb_register (p_netif,
                                      __net_link_event_handler,
                                      p_net);

    if (auto_ip) {
        aw_bool_t bound;
        int    timeout = 20 * 1000;

        aw_netif_dhcpc_start(p_netif);
        while (AW_FALSE == bound && timeout) {
            aw_mdelay(100);
            timeout -= 100;
            ret = aw_netif_dhcpc_is_bound(p_netif, &bound);
            if (ret != AW_OK) {
                aw_kprintf("Check DHCPC failed %d.\r\n", ret);
                goto __exit;
            }
        }
        if (AW_FALSE == bound) {
            aw_kprintf("DHCP Timed out!\r\n");
            ret = -AW_ETIME;
            goto __exit;
        }
    } else {
        aw_netif_dhcpc_stop(p_netif);

        inet_aton(ip, &addr);
        ret = aw_netif_ipv4_ip_set(p_netif, &addr);
        if(ret != AW_OK){
            aw_kprintf("Set IP failed %d.\r\n", ret);
            goto __exit;
        }

        inet_aton(mask, &addr);
        ret = aw_netif_ipv4_netmask_set(p_netif, &addr);
        if(ret != AW_OK){
            aw_kprintf("Set mask failed %d.\r\n", ret);
            goto __exit;
        }

        inet_aton(gw, &addr);
        aw_netif_ipv4_gw_set(p_netif, &addr);
        if(ret != AW_OK){
            aw_kprintf("Set gate way failed %d.\r\n",ret);
            goto __exit;
        }
    }
    /* 设置当前网卡为默认网卡 */
    ret = aw_netif_set_default (p_netif);
    if (ret != AW_OK) {
        aw_kprintf("Set default network-card Error! Line:%d\r\n", __LINE__ );
    }

    aw_netif_ipv4_ip_get(p_netif, &addr);
    aw_kprintf("IP: %s\r\n", inet_ntoa(addr));

    aw_netif_ipv4_netmask_get(p_netif, &addr);
    aw_kprintf("Mask: %s\r\n", inet_ntoa(addr));

    aw_netif_ipv4_gw_get(p_netif, &addr);
    aw_kprintf("GateaWay: %s\r\n", inet_ntoa(addr));

    aw_netif_hwaddr_get(p_netif, mac.octet, 6);
    aw_kprintf("MAC: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                mac.octet[0], mac.octet[1], mac.octet[2],
                mac.octet[3], mac.octet[4], mac.octet[5]);

__exit:
    aw_netif_dev_close(p_netif);
    return ret;
}


aw_err_t app_net_init (struct app *p_this, const char *wifi_device)
{
    aw_err_t        ret;

    p_this->p_cfg->net->wifi_name = wifi_device;
    /* 初始化Wi-Fi */
    ret = __wifi_init(p_this->p_cfg->net);
    if (ret != AW_OK) {
        return ret;
    }

    /* 初始化网络功能 */
    ret = __netif_init("st", 1, NULL, NULL, NULL, p_this->p_cfg->net);
    if (ret != AW_OK) {
        return ret;
    }

    return AW_OK;
}

