#include "aworks.h"
#include "stdlib.h"
#include "string.h"

#include "aw_shell.h"
#include "aw_assert.h"

#include "lwip/ext/net_sts.h"
#include "lwip/ext/ipv4_nat.h"
#include "lwip/ip4.h"
#include "lwip/ip_addr.h"

static void __log (void         *p_color,
                   const char   *func_name,
                   int           line,
                   const char   *fmt, ...)
{
    char        log_buf[256];
    uint16_t    len;
    va_list     ap;

    va_start( ap, fmt );
    len = aw_vsnprintf((char *)log_buf, sizeof(log_buf), fmt, ap);
    va_end( ap );
    aw_assert(len < sizeof(log_buf) - 1);

#ifdef __VIH_DEBUG
    aw_shell_printf_with_color(AW_DBG_SHELL_IO, p_color, func_name, log_buf);
#else
    AW_INFOF((log_buf));
#endif
}

void net_sts_example (void)
{
    aw_net_sts_ctl_init_t    init;
    aw_net_sts_dev_init_t    dev_init;
    aw_net_sts_ctl_t        *p_ctl;
    aw_net_sts_dev_t        *p_dev;
    aw_err_t                 ret;

    memset(&init, 0, sizeof(init));

    init.pfn_log_out = __log;
    p_ctl = aw_net_sts_ctl_create(&init);

    {
        memset(&dev_init, 0, sizeof(dev_init));
        memcpy(dev_init.netif_name, "pp", 3);
        dev_init.chk_ms      = 5000;
        dev_init.pfn_log_out = __log;
        dev_init.prioty      = 10;
        dev_init.type        = AW_NET_STS_DEV_TYPE_4G;
        dev_init.u._4g.pfn_event_notify = NULL;
        p_dev = aw_net_sts_dev_create(&dev_init);

        aw_net_sts_add(p_ctl, p_dev);
        aw_mdelay(30000);

        /**
         * ÐÞ¸Ä 4g µÄ init ÉèÖÃ
         */
        if (1) {
            memset(&dev_init, 0, sizeof(dev_init));
            memcpy(dev_init.netif_name, "pp", 3);
            dev_init.chk_ms      = 10000;
            dev_init.pfn_log_out = __log;
            dev_init.prioty      = 0;
            dev_init.type        = AW_NET_STS_DEV_TYPE_4G;
            dev_init.u._4g.pfn_event_notify = NULL;
            ret = aw_net_sts_dev_reinit(p_ctl, p_dev, &dev_init);
            aw_assert(ret == AW_OK);
        }
    }
    {
        memset(&dev_init, 0, sizeof(dev_init));
        memcpy(dev_init.netif_name, "eth0", 4);
        dev_init.chk_ms      = 30000;
        dev_init.p_chk_inet_addr    = "121.33.243.38";
        dev_init.pfn_log_out = __log;
        dev_init.prioty      = 1;
        dev_init.type        = AW_NET_STS_DEV_TYPE_ETH;
        p_dev = aw_net_sts_dev_create(&dev_init);

        aw_net_sts_add(p_ctl, p_dev);
    }
    if (1) {
        memset(&dev_init, 0, sizeof(dev_init));
        memcpy(dev_init.netif_name, "eth1", 4);
        dev_init.chk_ms      = 30000;
        dev_init.p_chk_inet_addr    = "121.33.243.38";
        dev_init.pfn_log_out = __log;
        dev_init.prioty      = 2;
        dev_init.type        = AW_NET_STS_DEV_TYPE_ETH;
        p_dev = aw_net_sts_dev_create(&dev_init);

        aw_net_sts_add(p_ctl, p_dev);
    }

    if (1) {
        memset(&dev_init, 0, sizeof(dev_init));
        memcpy(dev_init.netif_name, "st", 3);
        dev_init.chk_ms      = 30000;
        dev_init.p_chk_inet_addr    = "121.33.243.38";
        dev_init.pfn_log_out = __log;
        dev_init.prioty      = 3;
        dev_init.type        = AW_NET_STS_DEV_TYPE_WIFI;
        p_dev = aw_net_sts_dev_create(&dev_init);

        aw_net_sts_add(p_ctl, p_dev);
    }
}

void ipv4_nat_example (void)
{
    aw_err_t       ret;
    ip_nat_entry_t nat_entry;
    ip4_addr_t     src_ip4_addr;
    ip4_addr_t     mask_ip4_addr;

    memset(&nat_entry, 0, sizeof(nat_entry));

    IP4_ADDR(&src_ip4_addr, 192, 168, 0, 0);
    IP4_ADDR(&mask_ip4_addr, 255, 255, 0, 0);
    ip_addr_copy_from_ip4(nat_entry.source_net, src_ip4_addr);
    ip_addr_copy_from_ip4(nat_entry.source_netmask, mask_ip4_addr);

    ret = ip_nat_add(&nat_entry);
    aw_assert(ret == AW_OK);

    IP4_ADDR(&src_ip4_addr, 172, 16, 0, 0);
    IP4_ADDR(&mask_ip4_addr, 255, 0xF0, 0, 0);
    ip_addr_copy_from_ip4(nat_entry.source_net, src_ip4_addr);
    ip_addr_copy_from_ip4(nat_entry.source_netmask, mask_ip4_addr);

    ret = ip_nat_add(&nat_entry);
    aw_assert(ret == AW_OK);

    IP4_ADDR(&src_ip4_addr, 10, 0, 0, 0);
    IP4_ADDR(&mask_ip4_addr, 255, 0, 0, 0);
    ip_addr_copy_from_ip4(nat_entry.source_net, src_ip4_addr);
    ip_addr_copy_from_ip4(nat_entry.source_netmask, mask_ip4_addr);

    ret = ip_nat_add(&nat_entry);
    aw_assert(ret == AW_OK);

}

