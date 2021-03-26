/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief driver for dp83848ep 10/100/1000 ethernet PHY chips
 *
 * \internal
 * \par modification history
 * - 1.00 15-04-20  axn, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "awbl_miibus.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "driver/phy/awbl_dp83848ep_phy.h"

/*******************************************************************************
  import
*******************************************************************************/
AWBL_METHOD_IMPORT(awbl_mii_phy_mode_set);
AWBL_METHOD_IMPORT(awbl_mii_phy_mode_get);
AWBL_METHOD_IMPORT(awbl_mii_phy_notice);

/*******************************************************************************
  defines
*******************************************************************************/
/** \brief declare phy device instance */
#define __GEN_PHY_DEV_DECL(p_this, p_dev) \
    struct awbl_dp83848ep_phy_dev *p_this = \
        (struct awbl_dp83848ep_phy_dev *)(p_dev)

/** \brief declare phy infomation */
#define __GEN_PHY_DEVINFO_DECL(p_this_dvif, p_dev) \
    struct awbl_dp83848ep_phy_devinfo *p_this_dvif = \
        (struct awbl_dp83848ep_phy_devinfo *)AWBL_DEVINFO_GET(p_dev)

/** \brief DP83848C PHY */

/** \brief Default PHY device address         */
#define DP83848C_DEF_ADR            0x01        

/** \brief PHY Identifier                     */
#define DP83848C_ID                 0x20005C90  


/** \brief DP83848C PHY Registers */

/** \brief Basic Mode Control Register        */
#define PHY_REG_BMCR                0x00        

/** \brief Basic Mode Status Register         */
#define PHY_REG_BMSR                0x01        

/** \brief PHY Identifier 1                   */
#define PHY_REG_IDR1                0x02        

/** \brief PHY Identifier 2                   */
#define PHY_REG_IDR2                0x03        

/** \brief Auto-Negotiation Advertisement     */
#define PHY_REG_ANAR                0x04        

/** \brief Auto-Neg. Link Partner Abitily     */
#define PHY_REG_ANLPAR              0x05        

/** \brief Auto-Neg. Expansion Register       */
#define PHY_REG_ANER                0x06        

/** \brief Auto-Neg. Next Page TX             */
#define PHY_REG_ANNPTR              0x07        

/** \brief DP83848C PHY Extended Registers */

/** \brief Status Register                    */
#define PHY_REG_STS                 0x10        

/** \brief MII Interrupt Control Register     */
#define PHY_REG_MICR                0x11        

/** \brief MII Interrupt Status Register      */
#define PHY_REG_MISR                0x12        

/** \brief False Carrier Sense Counter        */
#define PHY_REG_FCSCR               0x14        

/** \brief Receive Error Counter              */
#define PHY_REG_RECR                0x15        

/** \brief PCS Sublayer Config. and Status    */
#define PHY_REG_PCSR                0x16        

/** \brief RMII and Bypass Register           */
#define PHY_REG_RBR                 0x17        

/** \brief LED Direct Control Register        */
#define PHY_REG_LEDCR               0x18        

/** \brief PHY Control Register               */
#define PHY_REG_PHYCR               0x19        

/** \brief 10Base-T Status/Control Register   */
#define PHY_REG_10BTSCR             0x1A        

/** \brief CD Test Control and BIST Extens.   */
#define PHY_REG_CDCTRL1             0x1B        

/** \brief Energy Detect Control Register     */
#define PHY_REG_EDCR                0x1D        

/** \brief DP83848C PHY Control and Status bits  */

/** \brief Full Duplex 100Mbit                */
#define PHY_FULLD_100M              0x2100      

/** \brief Half Duplex 100Mbit                */
#define PHY_HALFD_100M              0x2000      

/** \brief Full Duplex 10Mbit                 */
#define PHY_FULLD_10M               0x0100      

/** \brief Half Duplex 10MBit                 */
#define PHY_HALFD_10M               0x0000      

/** \brief Select Auto Negotiation            */
#define PHY_AUTO_NEG                0x1000      


#define PHY_RESTART_AUTO_NEG       (1u << 9)

/** \brief AutoNegotiation Complete (BMSR reg)*/
#define PHY_AUTO_NEG_DONE           0x0020      

/** \brief Reset bit (BMCR reg)               */
#define PHY_BMCR_RESET              0x8000      

/** \brief Link Valid Status (REG_STS reg)    */
#define LINK_VALID_STS              0x0001      

/** \brief Full Duplex Status (REG_STS reg)   */
#define FULL_DUP_STS                0x0004      

/** \brief 10Mbps Status (REG_STS reg)        */
#define SPEED_10M_STS               0x0002      

#define PHY_DRV_SPDLED             (1 << 5)

/** \brief Value to force on LED_SPEED output */
#define PHY_SPDLED_OUT             (1 << 2)     


#define LINK_STS_CHECK_MASK         0x0007



#define __PHY_MONITOR_TASK_PRI      (6)


#define __PHY_MONITOR_TASK_DELAY_MS (1000)

aw_local void __gen_phy_init(struct awbl_phy_device *p_phy_dev);
aw_local aw_err_t __gen_phy_mode_set (struct awbl_phy_device *p_phy_dev,
                                      uint32_t                mode);

aw_local aw_err_t __gen_phy_mode_get (struct awbl_phy_device *p_phy_dev,
                                      uint32_t               *mode,
                                      uint32_t               *status)
{
    uint16_t mii_sts;
    uint16_t mii_ctrl;
    uint16_t mii_anar;
    uint16_t mii_lpar;
    uint16_t gmii_anar = 0;
    uint16_t gmii_lpar = 0;
    uint16_t anlpar;

    *mode   = AWBL_IFM_ETHER;
    *status = AWBL_IFM_AVALID;

    {
        uint16_t  id1;
        uint16_t  id2;
        __GEN_PHY_DEVINFO_DECL(p_this_dvif, p_phy_dev);

        /* Check if this is a DP83848C PHY. */
        awbl_miibus_read(p_phy_dev, AWBL_MII_PHY_ID1_REG, &id1);
        awbl_miibus_read(p_phy_dev, AWBL_MII_PHY_ID2_REG, &id2);

        if (((id1 << 16) | (id2 & 0xFFF0)) != DP83848C_ID) {
            /* PHY working error */
            if (p_this_dvif->pfunc_plfm_init) {
                p_this_dvif->pfunc_plfm_init();
            }
            __gen_phy_mode_set (p_phy_dev, p_this_dvif->phy_mode_set);
            *mode |= AWBL_IFM_NONE;

            return AW_OK;
        }
    }

    /* read MII status register once to unlatch link status bit */
    awbl_miibus_read(p_phy_dev, AWBL_MII_STAT_REG, &mii_sts);

    /* read again to know its current value */
    awbl_miibus_read(p_phy_dev, AWBL_MII_STAT_REG, &mii_sts);

    /* no link bit means no carrier. */
    if (!(mii_sts & AWBL_MII_SR_LINK_STATUS)) {
        *mode |= AWBL_IFM_NONE;
        return AW_OK;
    }

    *status |= AWBL_IFM_ACTIVE;

    /*
     * read the control, ability advertisement and link
     * partner advertisement registers.
     */
    awbl_miibus_read(p_phy_dev, AWBL_MII_CTRL_REG, &mii_ctrl);
    awbl_miibus_read(p_phy_dev, AWBL_MII_AN_ADS_REG, &mii_anar);
    awbl_miibus_read(p_phy_dev, AWBL_MII_AN_PRTN_REG, &mii_lpar);

    if (mii_sts & AWBL_MII_SR_EXT_STS) {
        awbl_miibus_read(p_phy_dev, AWBL_MII_MASSLA_CTRL_REG, &gmii_anar);
        awbl_miibus_read(p_phy_dev, AWBL_MII_MASSLA_STAT_REG, &gmii_lpar);
    }

    /*
     * If autoneg is on, figure out the link settings from the
     * advertisement and partner ability registers. If autoneg is
     * off, use the settings in the control register.
     */
    if (mii_ctrl & AWBL_MII_CR_AUTO_EN) {
        anlpar = mii_anar & mii_lpar;
        if ((gmii_anar & AWBL_MII_MASSLA_CTRL_1000T_FD) &&
            (gmii_lpar & AWBL_MII_MASSLA_STAT_LP1000T_FD)) {
            *mode |= AWBL_IFM_1000_T | AWBL_IFM_FDX;
        } else if ((gmii_anar & AWBL_MII_MASSLA_CTRL_1000T_HD) &&
                   (gmii_lpar & AWBL_MII_MASSLA_STAT_LP1000T_HD)) {
            *mode |= AWBL_IFM_1000_T | AWBL_IFM_HDX;
        }
        else if (anlpar & AWBL_MII_ANAR_100TX_FD) {
            *mode |= AWBL_IFM_100_TX | AWBL_IFM_FDX;
        } else if (anlpar & AWBL_MII_ANAR_100TX_HD) {
            *mode |= AWBL_IFM_100_TX | AWBL_IFM_HDX;
        } else if (anlpar & AWBL_MII_ANAR_10TX_FD) {
            *mode |= AWBL_IFM_10_T | AWBL_IFM_FDX;
        } else if (anlpar & AWBL_MII_ANAR_10TX_HD) {
            *mode |= AWBL_IFM_10_T | AWBL_IFM_HDX;
        } else {
            *mode |= AWBL_IFM_NONE;
        }
    } else {
        if (mii_ctrl & AWBL_MII_CR_FDX) {
            *mode |= AWBL_IFM_FDX;
        } else {
            *mode |= AWBL_IFM_HDX;
        }
        if ((mii_ctrl & (AWBL_MII_CR_100 | AWBL_MII_CR_1000)) ==
                      (AWBL_MII_CR_100 | AWBL_MII_CR_1000)) {
            *mode |= AWBL_IFM_1000_T;
        } else if (mii_ctrl & AWBL_MII_CR_100) {
            *mode |= AWBL_IFM_100_TX;
        } else {
            *mode |= AWBL_IFM_10_T;
        }
    }
    return AW_OK;
}

aw_local aw_err_t __gen_phy_mode_set (struct awbl_phy_device *p_phy_dev,
                                      uint32_t                mode)
{
    uint16_t mii_val;
    uint16_t mii_anar  = 0;
    uint16_t gmii_anar = 0;
    uint16_t mii_ctrl  = 0;
    uint16_t mii_sts;

    __gen_phy_init(p_phy_dev);

    awbl_miibus_read(p_phy_dev, AWBL_MII_STAT_REG, &mii_sts);

    switch(AWBL_IFM_SUBTYPE(mode)) {

    case AWBL_IFM_AUTO:
        /* Set autoneg advertisement to advertise all modes. */
        mii_anar = AWBL_MII_ANAR_10TX_HD  | AWBL_MII_ANAR_10TX_FD |
                   AWBL_MII_ANAR_100TX_HD | AWBL_MII_ANAR_100TX_FD;

        if (mii_sts & AWBL_MII_SR_EXT_STS) {
            gmii_anar = AWBL_MII_MASSLA_CTRL_1000T_FD | 
                        AWBL_MII_MASSLA_CTRL_1000T_HD;
        }
        mii_ctrl = AWBL_MII_CR_AUTO_EN | AWBL_MII_CR_RESTART;
        break;

    case AWBL_IFM_1000_T:
        if (!(mii_sts & AWBL_MII_SR_EXT_STS)) {
            return AW_OK;
        }
        gmii_anar = AWBL_MII_MASSLA_CTRL_1000T_FD | 
                    AWBL_MII_MASSLA_CTRL_1000T_HD;
        mii_anar  = 0;
        mii_ctrl  = AWBL_MII_CR_AUTO_EN | AWBL_MII_CR_RESTART;
        break;

    case AWBL_IFM_100_TX:
        mii_ctrl = AWBL_MII_CR_100 | AWBL_MII_CR_AUTO_EN | AWBL_MII_CR_RESTART;
        if ((mode & AWBL_IFM_GMASK) == AWBL_IFM_FDX) {
            mii_anar  = AWBL_MII_ANAR_100TX_FD;
            mii_ctrl |= AWBL_MII_CR_FDX;
        } else {
            mii_anar  = AWBL_MII_ANAR_100TX_HD;
        }
        gmii_anar = 0;
        break;

    case AWBL_IFM_10_T:
        mii_ctrl = AWBL_MII_CR_AUTO_EN | AWBL_MII_CR_RESTART;
        if ((mode & AWBL_IFM_GMASK) == AWBL_IFM_FDX) {
            mii_anar  = AWBL_MII_ANAR_10TX_FD;
            mii_ctrl |= AWBL_MII_CR_FDX;
        } else {
            mii_anar  = AWBL_MII_ANAR_10TX_HD;
        }
        gmii_anar = 0;
        break;

    default:
        return AW_ERROR;
        break;
    }

    __gen_phy_init(p_phy_dev);

    awbl_miibus_read(p_phy_dev, AWBL_MII_AN_ADS_REG, &mii_val);

    mii_val &= ~(AWBL_MII_ANAR_10TX_HD  | AWBL_MII_ANAR_10TX_FD|
                 AWBL_MII_ANAR_100TX_HD | AWBL_MII_ANAR_100TX_FD);
    mii_val |= mii_anar;

    awbl_miibus_write(p_phy_dev, AWBL_MII_AN_ADS_REG, mii_val);

    if (mii_sts & AWBL_MII_SR_EXT_STS) {

        awbl_miibus_read(p_phy_dev, AWBL_MII_MASSLA_CTRL_REG, &mii_val);
        mii_val &= ~(AWBL_MII_MASSLA_CTRL_1000T_HD | 
                     AWBL_MII_MASSLA_CTRL_1000T_FD);
        mii_val |= gmii_anar;
        awbl_miibus_write(p_phy_dev, AWBL_MII_MASSLA_CTRL_REG, mii_val);
    }

    awbl_miibus_read(p_phy_dev, AWBL_MII_CTRL_REG, &mii_val);

    mii_val &= ~(AWBL_MII_CR_FDX     | 
                 AWBL_MII_CR_100     | 
                 AWBL_MII_CR_AUTO_EN |
                 AWBL_MII_CR_RESTART);
    mii_val |= mii_ctrl;

    awbl_miibus_write(p_phy_dev, AWBL_MII_CTRL_REG, mii_val);

    return AW_OK;
}


aw_local aw_err_t __gen_phy_notice (struct awbl_phy_device *p_phy_dev,
                                    uint32_t mode,
                                    uint32_t status)
{

    if ((status & AWBL_IFM_ACTIVE)) {
        switch(AWBL_IFM_SUBTYPE(mode)) {

        case AWBL_IFM_100_TX:
            /* 
             * Drive value of SPDLED bit onto LED_SPEED output : 
             * output 0, LED ON
             */
            awbl_miibus_write(p_phy_dev, PHY_REG_LEDCR, PHY_DRV_SPDLED);
            break;
        case AWBL_IFM_10_T:
            /* 
             * Drive value of SPDLED bit onto LED_SPEED output : 
             * output 1, LED OFF
             */
            awbl_miibus_write(p_phy_dev, 
                              PHY_REG_LEDCR, 
                              PHY_DRV_SPDLED | PHY_SPDLED_OUT);
            break;
        default:
            break;
        }
    } else {
        /* 
         * Drive value of SPDLED bit onto LED_SPEED output : 
         * output 1, LED OFF
         */
        awbl_miibus_write(p_phy_dev,
                          PHY_REG_LEDCR, 
                          PHY_DRV_SPDLED | PHY_SPDLED_OUT);
    }

    return AW_OK;
}

aw_local void __gen_phy_init (struct awbl_phy_device *p_phy_dev)
{
    uint16_t miiSts;
    uint16_t miiCtl;
    uint16_t miiVal;
    int i;

    /* Get status register so we can look for extended capabilities. */
    awbl_miibus_read(p_phy_dev, AWBL_MII_STAT_REG, &miiSts);

    miiVal = AWBL_MII_CR_POWER_DOWN;
    awbl_miibus_write(p_phy_dev, AWBL_MII_CTRL_REG, miiVal);

    aw_udelay(100);

    miiVal = 0;
    awbl_miibus_write(p_phy_dev, AWBL_MII_CTRL_REG, miiVal);

    /* Set reset bit and then wait for it to clear. */
    miiVal = AWBL_MII_CR_RESET;
    awbl_miibus_write(p_phy_dev, AWBL_MII_CTRL_REG, miiVal);

    for (i = 0; i < 1000; i++) {
        awbl_miibus_read(p_phy_dev, AWBL_MII_CTRL_REG, &miiCtl);
        if (!(miiCtl & AWBL_MII_CR_RESET)) {
            break;
        }
        aw_mdelay(1);
    }

    /*
     * If the extended capabilities bit is set, this is a gigE
     * PHY, so make sure we advertise gigE modes.
     */

    if (miiSts & AWBL_MII_SR_EXT_STS) {
        /* Enable advertisement of gigE modes. */
        miiVal = AWBL_MII_MASSLA_CTRL_1000T_FD | AWBL_MII_MASSLA_CTRL_1000T_HD;
        awbl_miibus_write(p_phy_dev, AWBL_MII_CTRL_REG, miiVal);
    }

    /*
     * Some PHYs come out of reset with their isolate bit set. Make
     * sure we don't write that bit back when setting the control
     * register.
     */
    miiCtl = AWBL_MII_CR_AUTO_EN | AWBL_MII_CR_RESTART;
    awbl_miibus_write(p_phy_dev, AWBL_MII_CTRL_REG, miiCtl);

    /*
     * dp83848ep
     */
    awbl_miibus_read(p_phy_dev, PHY_REG_PHYCR, &miiCtl);
    miiCtl &= ~(1 << 5);
    
    /* set MODE 2 for LINK LED */
    awbl_miibus_write(p_phy_dev, PHY_REG_PHYCR, miiCtl); 
    
#if 0 
    /* 10T/10T FULL/100T/100T FULL is supported */
    awbl_miibus_write(p_master, p_phy_dev->phy_addr, PHY_REG_ANAR, 0x01E1);
    /* enable Auto-Negotiation  */
    awbl_miibus_write(p_master, p_phy_dev->phy_addr, PHY_REG_BMCR, PHY_AUTO_NEG);
    /* Restart Auto-Negotiation */
    awbl_miibus_write(p_master, 
                      p_phy_dev->phy_addr, 
                      PHY_REG_BMCR,
                      PHY_AUTO_NEG | PHY_RESTART_AUTO_NEG);
#endif

    /* Drive value of SPDLED bit onto LED_SPEED output : output 1, LED OFF*/
    awbl_miibus_write(p_phy_dev, PHY_REG_LEDCR, PHY_DRV_SPDLED | PHY_SPDLED_OUT);
}


aw_local void __phy_int_handle(void *p_dev)
{

    __GEN_PHY_DEV_DECL(p_this, p_dev);

    AW_SEMB_GIVE(p_this->phy_smb);
}


aw_local void __phy_print_status(struct awbl_phy_device *p_phy_dev)
{
    __GEN_PHY_DEVINFO_DECL(p_this_dvif, p_phy_dev);
    uint16_t                speed    = 0;

    AW_INFOF(("PHY %d:%d - Link is %s",
              AWBL_DEV_UNIT_GET(p_phy_dev),
              p_this_dvif->phy_addr,
             (p_phy_dev->phy_state & AWBL_IFM_ACTIVE) ? "Up" : "Down"));

    if (p_phy_dev->phy_state & AWBL_IFM_ACTIVE) {

        switch (AWBL_IFM_SUBTYPE(p_phy_dev->phy_mode)) {

        case AWBL_IFM_1000_T: speed = 1000; break;
        case AWBL_IFM_100_TX: speed = 100;  break;
        case AWBL_IFM_10_T:   speed = 10;   break;
        default : break;
        }

        AW_INFOF((" - %d/%s", speed,
                ((p_phy_dev->phy_mode & AWBL_IFM_GMASK) == AWBL_IFM_FDX) ?
                  "Full" : "Half"));
    }
    AW_INFOF(("\n"));
}


aw_local void __phy_monitor_task(void *p_dev)
{
    __GEN_PHY_DEV_DECL(p_this, p_dev);
    __GEN_PHY_DEVINFO_DECL(p_this_dvif, p_this);
    uint32_t phy_mode, phy_stat, tmp_phy_stat;
    uint16_t misr = 0;

    while (1) {

        /* 有中断引脚信息则使用中断处理方式,否则就使用查询方式  */
        if (p_this_dvif->p_phy_int_info) {
            AW_SEMB_TAKE(p_this->phy_smb, AW_WAIT_FOREVER);

            /* clear the int flag */
            awbl_miibus_read(&p_this->phy_dev, PHY_REG_MISR, &misr);
        }


        if (AW_OK == __gen_phy_mode_get(&p_this->phy_dev,
                                        &phy_mode,
                                        &phy_stat)) {
            uint8_t update = 0;

            /* link state change & last link is active & current link is active*/
            if ((misr & (1 << 13)) &&
                (p_this->phy_dev.phy_state & AWBL_IFM_ACTIVE) &&
                (phy_stat & AWBL_IFM_ACTIVE)) {
                update = 2;

            } else if (phy_stat != p_this->phy_dev.phy_state) {
                if ((!(phy_stat & AWBL_IFM_ACTIVE)) || /* Link is down */
                    ((phy_stat & AWBL_IFM_ACTIVE) &&
                    (AWBL_IFM_SUBTYPE(phy_mode) != AWBL_IFM_NONE))) {
                    /* Link is up */
                    update = 1;
                }
            } else if (phy_mode != p_this->phy_dev.phy_mode){
                /* Link mode change */
                  if (AWBL_IFM_SUBTYPE(phy_mode) != AWBL_IFM_NONE) {

                      update = 1;
                  }
            }

            while (update--) {

                if (update == 1) {
                    /* 断开连接  */
                    tmp_phy_stat = AWBL_IFM_AVALID;
                } else {
                    tmp_phy_stat = phy_stat;
                }

                if (AW_OK == awbl_miibus_update((struct awbl_phy_device *)p_this)) {

                    /* p_phy_cur->phy_mode和phy_state是随时都可能变化的  */
                    p_this->phy_dev.phy_mode  = phy_mode;
                    p_this->phy_dev.phy_state = tmp_phy_stat;

                    awbl_miibus_notice (&p_this->phy_dev, phy_mode, tmp_phy_stat);
                    __phy_print_status(&p_this->phy_dev);
                }
            }

            if (!p_this_dvif->p_phy_int_info) {
                aw_mdelay(__PHY_MONITOR_TASK_DELAY_MS);
            }
        }
    }
}


/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __gen_phy_inst_init1(awbl_dev_t *p_dev)
{

}


/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __gen_phy_inst_init2(awbl_dev_t *p_dev)
{
    __GEN_PHY_DEV_DECL(p_this, p_dev);
    __GEN_PHY_DEVINFO_DECL(p_this_dvif, p_dev);

    if (p_this_dvif->phy_addr > 31) {
        return;
    }

    p_this->phy_dev.phy_addr     = p_this_dvif->phy_addr;

    p_this->phy_dev.phy_mode     = AWBL_IFM_ETHER;
    p_this->phy_dev.phy_state    = AWBL_IFM_AVALID;
    p_this->phy_dev.p_ethif_name = p_this_dvif->p_ethif_name;

    if (NULL != p_this_dvif->pfunc_plfm_init) {
        p_this_dvif->pfunc_plfm_init();
    }
}


/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __gen_phy_inst_connect(awbl_dev_t *p_dev)
{
    __GEN_PHY_DEV_DECL(p_this, p_dev);
    __GEN_PHY_DEVINFO_DECL(p_this_dvif, p_dev);
    uint16_t micr = 0, misr = 0;

    /* MII_BUS_DEV */
    struct awbl_dev *p_phy_parent = awbl_dev_parent(p_dev);

    if (p_phy_parent == NULL) {
        return;
    }

    /* 先进行模式设置，因为其中有phy的复位  */
    awbl_miibus_mode_set((struct awbl_phy_device *)p_dev,
                         p_this_dvif->phy_mode_set);

    /* 对PHY的中断寄存器进行相应的配置  */
    /* MICR  0x11 中断控制寄存器  */
    awbl_miibus_read(&p_this->phy_dev, PHY_REG_MICR, &micr);

    /* 中断引脚作为中断输出  */
    AW_BIT_SET(micr, 0);

    /* 开起中断  */
    AW_BIT_SET(micr, 1);

    awbl_miibus_write(&p_this->phy_dev, PHY_REG_MICR, micr);

    awbl_miibus_read(&p_this->phy_dev, PHY_REG_MISR, &misr);

    /* 使能自动协商完成中断 */
    AW_BIT_SET(misr, 2);

    /* 使能连接改变中断  */
    AW_BIT_SET(misr, 5);

    AW_SEMB_INIT(p_this->phy_smb, AW_SEM_Q_FIFO, AW_SEM_EMPTY);

    awbl_miibus_write(&p_this->phy_dev, PHY_REG_MISR, misr & 0x07f);

    aw_gpio_trigger_connect  (p_this_dvif->p_phy_int_info->phy_int_pin,
                              __phy_int_handle,
                              p_dev);

    /* 链接到GPIO中断  */
    aw_gpio_trigger_cfg  ( p_this_dvif->p_phy_int_info->phy_int_pin,
                           AW_GPIO_TRIGGER_FALL);

    aw_gpio_trigger_on (p_this_dvif->p_phy_int_info->phy_int_pin);


    /* create mii bus monitor task */
    AW_TASK_INIT(p_this->phy_monitor_task,
                 "phy bus monitor",
                 __PHY_MONITOR_TASK_PRI,
                 __PHY_MONITOR_STACK_SIZE,
                 __phy_monitor_task,
                 (void*)p_dev);

    AW_TASK_STARTUP(p_this->phy_monitor_task);
}

aw_local aw_bool_t __gen_phy_drv_probe (awbl_dev_t *p_dev)
{
    /* TODO: maybe checks the ID register values of the PHY */

    return AW_TRUE;
}


aw_local aw_const struct awbl_dev_method __g_gen_phy_methods[] = {
    AWBL_METHOD(awbl_mii_phy_mode_set, __gen_phy_mode_set),
    AWBL_METHOD(awbl_mii_phy_mode_get, __gen_phy_mode_get),
    AWBL_METHOD(awbl_mii_phy_notice,   __gen_phy_notice),
    AWBL_METHOD_END
};


/** \brief driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_gen_phy_drvfuncs = {
    __gen_phy_inst_init1,
    __gen_phy_inst_init2,
    __gen_phy_inst_connect
};

/** \brief driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_gen_phy_drv_registration = {
    AWBL_VER_1,                             /**< \brief awb_ver */
    AWBL_BUSID_MII,                         /**< \brief bus_id */
    AWBL_DP83848EP_PHY_NAME,                /**< \brief p_drvname */
    &__g_gen_phy_drvfuncs,                  /**< \brief p_busfuncs */
    &__g_gen_phy_methods[0],                /**< \brief p_methods */
     __gen_phy_drv_probe                    /**< \brief pfunc_drv_probe */
};

void awbl_dp83848ep_phy_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_gen_phy_drv_registration);
}


/* end of file */

