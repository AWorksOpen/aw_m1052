/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief imx10xx USB host controller driver source file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-12-20  deo, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "aworks.h"
#include "awbus_lite.h"
#include "aw_int.h"
#include "aw_assert.h"
#include "driver\usb\awbl_imx10xx_usbh.h"




#define __USBH_REG_CMD                     (0x140)
#define __USBH_REG_CMD_RS                  (1 << 0)        /* controller run/stop */
#define __USBH_REG_CMD_RST                 (1 << 1)        /* controller reset */
#define __USBH_REG_CMD_ITC_NO_THRESHOLD    (~(0xff << 16)) /* Interrupt Threshold */

#define __USBH_REG_STS                     (0x144)

#define __USBH_REG_PORTSC1                 (0x184)         /* port status and control */
#define __USBH_REG_PORTSC1_PTS_MASK        (3 << 30)       /* parallel xcvr select mask */
#define __USBH_REG_PORTSC1_PTS_UTMI        (0 << 30)       /* UTMI/UTMI+ */
#define __USBH_REG_PORTSC1_PTW             (1 << 28)       /* UTMI width */
#define __USBH_REG_PORTSC1_PSPD_BIT        (26)


#define __USBH_PHY_REG_CTRL                 (0x30)          /* USB PHY Control register offsets */
#define __USBH_PHY_REG_CTRL_SET             (0x34)
#define __USBH_PHY_REG_CTRL_CLR             (0x38)
#define __USBH_PHY_REG_CTRL_SFTRST          (1u<<31)
#define __USBH_PHY_REG_CTRL_CLKGATE         (1u<<30)
#define __USBH_PHY_REG_CTRL_ENUTMILEVEL2    (1u<<15)
#define __USBH_PHY_REG_CTRL_ENUTMILEVEL3    (1u<<14)
#define __USBH_PHY_REG_CTRL_HOSTDISCONIRQ   (1u<<3)
#define __USBH_PHY_REG_CTRL_ENHOSTDISCON    (1u<<1)

#define __USBH_PHY_REG_PWD                  (0x00)

/*******************************************************************************
  IMX28 USB register define
*******************************************************************************/
#define __IMX10xx_USB_CAPLENGTH               (0x100)




/*******************************************************************************
  macro operate
*******************************************************************************/
#define __AWDEV_TO_IMX(p_awdev) \
        AW_CONTAINER_OF(AW_CONTAINER_OF(AW_CONTAINER_OF(p_awdev, \
                                                        struct awbl_busctlr, \
                                                        super), \
                                        struct awbl_usbh, \
                                        super), \
                        struct awbl_imx10xx_usbh, \
                        hc)


#define __IMX_REG32_WR(data, addr)  writel(data, (volatile void __iomem *)(addr))
#define __IMX_REG32_RD(addr)        readl((volatile void __iomem *)(addr))





/******************************************************************************/
aw_local aw_inline void __mdelay(uint32_t us)
{
    while (us--) {
        volatile uint32_t i = 26 * 1000;
        while (i--);
    }
}




/******************************************************************************/
aw_local void __rh_change (struct awbl_usbh_ehci *p_ehci, uint16_t change)
{
    struct awbl_imx10xx_usbh *p_this;
    p_this = AW_CONTAINER_OF(p_ehci, struct awbl_imx10xx_usbh, ehci);
    awbl_usbh_host_rh_change(&p_this->hc, change);
}




/******************************************************************************/
aw_local aw_err_t __usbh_request (struct awbl_usbh     *p_hc,
                                  struct awbl_usbh_trp *p_trp)
{
    struct awbl_imx10xx_usbh *p_this;
    p_this = AW_CONTAINER_OF(p_hc, struct awbl_imx10xx_usbh, hc);
    return awbl_usbh_ehci_request(&p_this->ehci, p_trp);
}




/******************************************************************************/
aw_local aw_err_t __usbh_cancel (struct awbl_usbh     *p_hc,
                                 struct awbl_usbh_trp *p_trp)
{
    struct awbl_imx10xx_usbh *p_this;
    p_this = AW_CONTAINER_OF(p_hc, struct awbl_imx10xx_usbh, hc);
    return awbl_usbh_ehci_cancel(&p_this->ehci, p_trp);
}




/******************************************************************************/
aw_local aw_err_t __usbh_ep_en (struct awbl_usbh          *p_hc,
                                struct awbl_usbh_endpoint *p_ep)
{
    struct awbl_imx10xx_usbh *p_this;
    p_this = AW_CONTAINER_OF(p_hc, struct awbl_imx10xx_usbh, hc);
    return awbl_usbh_ehci_ep_enable(&p_this->ehci, p_ep);
}




/******************************************************************************/
aw_local aw_err_t __usbh_ep_dis (struct awbl_usbh          *p_hc,
                                 struct awbl_usbh_endpoint *p_ep)
{
    struct awbl_imx10xx_usbh *p_this;
    p_this = AW_CONTAINER_OF(p_hc, struct awbl_imx10xx_usbh, hc);
    return awbl_usbh_ehci_ep_disable(&p_this->ehci, p_ep);
}




/******************************************************************************/
aw_local aw_err_t __usbh_rh_check (struct awbl_usbh *p_hc,
                                   uint16_t         *p_change)
{
    struct awbl_imx10xx_usbh *p_this;
    p_this = AW_CONTAINER_OF(p_hc, struct awbl_imx10xx_usbh, hc);
    return awbl_usbh_ehci_rh_change_check(&p_this->ehci, p_change);
}




/******************************************************************************/
aw_local aw_err_t __usbh_rh_ctrl (struct awbl_usbh        *p_hc,
                                  uint8_t                  type,
                                  uint8_t                  req,
                                  uint16_t                 val,
                                  uint16_t                 idx,
                                  uint16_t                 len,
                                  void                    *p_data)
{
    struct awbl_imx10xx_usbh *p_this;
    p_this = AW_CONTAINER_OF(p_hc, struct awbl_imx10xx_usbh, hc);
    return awbl_usbh_ehci_rh_ctrl(&p_this->ehci, type, req, val, idx, len, p_data);
}




/******************************************************************************/
aw_const aw_local struct awbl_usbh_drv __g_usbh_drv = {
        __usbh_request,
        __usbh_cancel,
        __usbh_ep_en,
        __usbh_ep_dis,
        __usbh_rh_check,
        __usbh_rh_ctrl
};




/******************************************************************************/
aw_local void __phy_init (struct awbl_imx10xx_usbh      *p_hc,
                          struct awbl_imx10xx_usbh_info *p_info)
{
    uint32_t temp, timeout = 10000;

    /* stop and reset the host controller */
    temp =  __IMX_REG32_RD(p_info->usb_reg + __USBH_REG_CMD);
    temp &= ~__USBH_REG_CMD_RS;

    __IMX_REG32_WR(temp, p_info->usb_reg + __USBH_REG_CMD);
    while (__IMX_REG32_RD(p_info->usb_reg + __USBH_REG_CMD)
                          & __USBH_REG_CMD_RS);
    temp |= __USBH_REG_CMD_RST;
    __IMX_REG32_WR(temp, p_info->usb_reg + __USBH_REG_CMD);
    while (__IMX_REG32_RD(p_info->usb_reg + __USBH_REG_CMD)
                          & __USBH_REG_CMD_RST);
    __mdelay(50);

    /* reset PHY */
    __IMX_REG32_WR(__USBH_PHY_REG_CTRL_SFTRST,
                    p_info->phy_reg + __USBH_PHY_REG_CTRL_SET);
    do {
        temp = __IMX_REG32_RD(p_info->phy_reg + __USBH_PHY_REG_CTRL);
    } while (!(temp & __USBH_PHY_REG_CTRL_CLKGATE) && (timeout--));

    /* enable PHY clock */
    __IMX_REG32_WR( __USBH_PHY_REG_CTRL_SFTRST |
                    __USBH_PHY_REG_CTRL_CLKGATE,
                    p_info->phy_reg + __USBH_PHY_REG_CTRL_CLR);

    __mdelay(50);

    temp = __IMX_REG32_RD(p_info->usb_reg + __USBH_REG_PORTSC1);
    temp &= ~__USBH_REG_PORTSC1_PTS_MASK;
    temp |= (__USBH_REG_PORTSC1_PTS_UTMI | __USBH_REG_PORTSC1_PTW);
    __IMX_REG32_WR(temp, p_info->usb_reg + __USBH_REG_PORTSC1);

    /* start PHY */
    __IMX_REG32_WR(0, p_info->phy_reg + __USBH_PHY_REG_PWD);

    /* enable full/low speed device support */
    __IMX_REG32_WR( __USBH_PHY_REG_CTRL_ENUTMILEVEL2 |
                    __USBH_PHY_REG_CTRL_ENUTMILEVEL3,
                    p_info->phy_reg + __USBH_PHY_REG_CTRL_SET);
}




/******************************************************************************/
aw_local void __usbh_isr(void *p_arg)
{
    struct awbl_imx10xx_usbh      *p_this;
    struct awbl_imx10xx_usbh_info *p_info;
    volatile uint32_t              status;

    p_this = __AWDEV_TO_IMX(p_arg);
    p_info = (struct awbl_imx10xx_usbh_info *)AWBL_DEVINFO_GET(p_arg);


    status = __IMX_REG32_RD(p_info->usb_reg + __USBH_REG_STS);
    if (status & (1<<2)) { /* port change */
        uint32_t temp;

        /* PHY */
        temp = __IMX_REG32_RD(p_info->phy_reg + __USBH_PHY_REG_CTRL);
        if (temp & __USBH_PHY_REG_CTRL_HOSTDISCONIRQ) {
            __IMX_REG32_WR((__USBH_PHY_REG_CTRL_ENHOSTDISCON |
                            __USBH_PHY_REG_CTRL_HOSTDISCONIRQ),
                            p_info->phy_reg + __USBH_PHY_REG_CTRL_CLR);

        } else {
            __IMX_REG32_WR(__USBH_PHY_REG_CTRL_ENHOSTDISCON,
                           p_info->phy_reg + __USBH_PHY_REG_CTRL_SET);
        }
    }

    /* call EHCI IRQ handle function */
    awbl_usbh_ehci_irq(&p_this->ehci);
}




/******************************************************************************/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return ;
}




/******************************************************************************/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    struct awbl_imx10xx_usbh      *p_this;
    struct awbl_imx10xx_usbh_info *p_info;

    p_this = __AWDEV_TO_IMX(p_awdev);
    p_info = (struct awbl_imx10xx_usbh_info *)AWBL_DEVINFO_GET(p_awdev);

    if (p_info->pfu_plfm_init) {
        p_info->pfu_plfm_init();
    }

    if (p_info->pfn_pwr_ctrl) {
        p_info->pfn_pwr_ctrl(AW_FALSE);
    }

    __mdelay(50);
    __phy_init(p_this, p_info);

    if (p_info->pfn_pwr_ctrl) {
        p_info->pfn_pwr_ctrl(AW_TRUE);
    }

    return;
}




/******************************************************************************/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    struct awbl_imx10xx_usbh      *p_this;
    struct awbl_imx10xx_usbh_info *p_info;

    p_this = __AWDEV_TO_IMX(p_awdev);
    p_info = (struct awbl_imx10xx_usbh_info *)AWBL_DEVINFO_GET(p_awdev);

    awbl_usbh_ehci_init(&p_this->ehci,
                        p_info->usb_reg + __IMX10xx_USB_CAPLENGTH,
                        p_info->prio,
                        __rh_change,
                        __USBH_REG_PORTSC1_PSPD_BIT,
                        p_info->nqhs,
                        p_info->nqtds,
                        p_info->nitds,
                        p_info->nsitds);




    awbl_usbh_host_create(&p_this->hc, p_this->ehci.nports, &__g_usbh_drv);

    aw_int_connect(p_info->inum,
                   (aw_pfuncvoid_t)__usbh_isr,
                   (void *)p_awdev);
    aw_int_enable(p_info->inum);

    return ;
}




/******************************************************************************/
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
        __drv_init1,
        __drv_init2,
        __drv_connect
};




/******************************************************************************/
aw_local aw_const struct awbl_drvinfo __g_drv_registration = {
    AWBL_VER_1,                             /* awb_ver */
    AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* bus_id */
    AWBL_IMX10XX_USBH_DRV_NAME,             /* p_drvname */
    &__g_drvfuncs,                          /* p_busfuncs */
    NULL,                                   /* p_methods */
    NULL                                    /* pfunc_drv_probe */
};




/******************************************************************************/
void awbl_imx10xx_usbh_drv_register (void)
{
    aw_err_t ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_drv_registration);
    aw_assert(ret == AW_OK);
    return;
}
