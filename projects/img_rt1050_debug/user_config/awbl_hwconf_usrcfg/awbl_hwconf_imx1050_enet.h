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

#ifndef __AWBL_HWCONF_IMX1050_ENET_H
#define __AWBL_HWCONF_IMX1050_ENET_H

#ifdef AW_DEV_IMX1050_ENET

#include "driver/net/awbl_imx10xx_enet.h"

#define ENET_PAD_CTRL (PUS_2_100K_Ohm_Pull_Up    | \
                       PKE_1_Pull_Keeper_Enabled | \
                       PUE_1_Pull                | \
                       SPEED_3_max_200MHz        | \
                       DSE_5_R0_5                | \
                       SRE_0_Fast_Slew_Rate)

#define MDIO_PAD_CTRL (PUS_2_100K_Ohm_Pull_Up    | \
                       PKE_1_Pull_Keeper_Enabled | \
                       PUE_1_Pull                | \
                       DSE_5_R0_5                | \
                       SRE_0_Fast_Slew_Rate      | \
                       Open_Drain_Enabled)

#define ENET_CLK_PAD_CTRL (DSE_6_R0_6 | SRE_0_Fast_Slew_Rate)


aw_local void enet_reset_phy (void)
{
    aw_udelay(1000);
    aw_gpio_set(GPIO1_9, 0);
    aw_udelay(1000);
    aw_gpio_set(GPIO1_9, 1);
    aw_udelay(1000);
}

/* 平台相关初始化 */
aw_local void __imx1050_enet_plfm_init (void)
{
    aw_clk_enable(IMX1050_CLK_CG_ENET);
    aw_clk_enable(IMX1050_CLK_ENET1_REF);
    aw_clk_rate_set(IMX1050_CLK_ENET1_REF, 50000000);

    int enet_gpios[] = {
         GPIO1_9,
         GPIO2_25,
         GPIO2_23,
         GPIO2_24,
         GPIO2_22,
         GPIO2_20,
         GPIO2_21,
         GPIO2_26,
         GPIO2_27,
         GPIO2_30,
         GPIO2_31,
     };

    if (aw_gpio_pin_request(AWBL_IMX10XX_ENET_NAME, enet_gpios, AW_NELEMENTS(enet_gpios)) == AW_OK) {
        /* 先为PHY芯片提供50Mhz时钟输入 */
        aw_gpio_pin_cfg(GPIO2_26, GPIO2_26_ENET_REF_CLK | IMX1050_PAD_CTL(ENET_CLK_PAD_CTRL));   /* ENET_TX_CLK (GPIO_B1_10) */
        aw_udelay(100);

        /* ENET_nRST */
        aw_gpio_pin_cfg(GPIO1_9,  GPIO1_9_GPIO | AW_GPIO_OUTPUT_INIT_HIGH_VAL);

        /**
         * Hard Reset PHY
         */
        enet_reset_phy();

        aw_gpio_pin_cfg(GPIO2_25, GPIO2_25_ENET_TX_EN | IMX1050_PAD_CTL(ENET_PAD_CTRL));      /* ENET_TX_EN (GPIO_B1_09) */
        aw_gpio_pin_cfg(GPIO2_23, GPIO2_23_ENET_TX_DATA00 | IMX1050_PAD_CTL(ENET_PAD_CTRL));  /* ENET_TXD0 (GPIO_B1_07) */
        aw_gpio_pin_cfg(GPIO2_24, GPIO2_24_ENET_TX_DATA01 | IMX1050_PAD_CTL(ENET_PAD_CTRL));  /* ENET_TXD1 (GPIO_B1_08) */
        aw_gpio_pin_cfg(GPIO2_22, GPIO2_22_ENET_RX_EN | IMX1050_PAD_CTL(ENET_PAD_CTRL));      /* ENET_RX_EN (GPIO_B1_06) */
        aw_gpio_pin_cfg(GPIO2_20, GPIO2_20_ENET_RX_DATA00 | IMX1050_PAD_CTL(ENET_PAD_CTRL));  /* ENET_RXD0 (GPIO_B1_04) */
        aw_gpio_pin_cfg(GPIO2_21, GPIO2_21_ENET_RX_DATA01 | IMX1050_PAD_CTL(ENET_PAD_CTRL));  /* ENET_RXD1 (GPIO_B1_05) */
        aw_gpio_pin_cfg(GPIO2_27, GPIO2_27_ENET_RX_ER | IMX1050_PAD_CTL(ENET_PAD_CTRL));      /* ENET_RX_ER (GPIO_B1_11) */

        aw_gpio_pin_cfg(GPIO2_30, GPIO2_30_ENET_MDC | IMX1050_PAD_CTL(ENET_PAD_CTRL));        /* ENET_MDC */
        aw_gpio_pin_cfg(GPIO2_31, GPIO2_31_ENET_MDIO | IMX1050_PAD_CTL(MDIO_PAD_CTRL));       /* ENET_MDIO */
    }
}

aw_local char *__get_ipaddr (void)
{
    return "192.168.1.10";
}

aw_local char *__get_netmsk (void)
{
    return "255.255.255.0";
}

aw_local char *__get_gateway (void)
{
    return "192.168.1.1";
}

/* 试图获取从OTP中获取以太网地址，如果失败则随机产生 */

/** \brief 判断以太网地址是否全为0 */
aw_local inline aw_bool_t is_zero_ether_addr (const uint8_t *addr)
{
    uint8_t val = 0;
    val = addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5];
    if (val) {
        return AW_FALSE;
    } else {
        return AW_TRUE;
    }
}

/** \brief 判断以太网地址是否为多播地址 */
aw_local inline aw_bool_t is_multicast_ether_addr (const uint8_t *addr)
{
    if ((addr[0] == 0xFF) &&
        (addr[1] == 0xFF) &&
        (addr[2] == 0xFF) &&
        (addr[3] == 0xFF) &&
        (addr[4] == 0xFF) &&
        (addr[5] == 0xFF)) {
        return AW_TRUE;
    }

    return AW_FALSE;
}

/** \brief 判断以太网地址是否合法 */
aw_local inline aw_bool_t is_valid_ether_addr (const uint8_t *addr)
{
    /* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
     * explicitly check for it here. */
    return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}

aw_local aw_err_t __get_macaddr (uint8_t *p_mac_buf, int buf_len)
{
    uint8_t  mac[6];
    aw_err_t ret;

    ret = aw_nvram_get("otp_zy_mac", 0, (char *)mac, 0, sizeof(mac));
    if ((ret != AW_OK) || (!is_valid_ether_addr(mac))) {
        AW_WARNF(("invalid mac address, generate one\r\n"));

        srand(aw_sys_tick_get());

        mac[0] = 0x00;
        mac[1] = 0x14;
        mac[2] = 0x97;
        mac[3] = rand();
        mac[4] = rand();
        mac[5] = rand();
    }

    memcpy(p_mac_buf, mac, sizeof(mac));
    return AW_OK;
}

aw_local aw_bool_t __get_dhcp_en (void)
{
    return AW_FALSE;
}

aw_local aw_const aw_netif_info_get_entry_t enet_info = {
    __get_ipaddr,
    __get_netmsk,
    __get_gateway,
    __get_macaddr,
    __get_dhcp_en,
};


/* 设备信息 */
aw_local aw_const struct awbl_imx10xx_enet_devinfo __g_imx1050_enet_devinfo = {
    "eth0",
    &enet_info,
    AW_TASK_SYS_PRIORITY(5),        /* 接收任务优先级    */

    IMX1050_ENET_BASE,              /* enet寄存器基地址 */
    INUM_ENET,                      /* 中断编号 */
    IMX1050_CLK_CG_ENET,
    64,
    64,
    IMX1050_MII_BUSID,
    PHY_INTERFACE_MODE_RMII,        /* PHY MODE */
    __imx1050_enet_plfm_init,
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_enet_dev __g_imx1050_enet_dev;

#define AWBL_HWCONF_IMX1050_ENET                      \
    {                                                 \
        AWBL_IMX10XX_ENET_NAME,                       \
        0,                                            \
        AWBL_BUSID_PLB,                               \
        0,                                            \
        (struct awbl_dev *) &__g_imx1050_enet_dev,    \
        &__g_imx1050_enet_devinfo                     \
    },

#else
#define AWBL_HWCONF_IMX1050_ENET

#endif  /* AW_DEV_IMX1050_ENET */

#endif  /* __AWBL_HWCONF_IMX1050_ENET_H */

/* end of file */
