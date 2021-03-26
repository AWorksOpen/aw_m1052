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
 * \brief gpio spi driver
 *
 * AWBus spi bus controller simulated by GPIO
 *
 * \internal
 * \par modification history:
 * - 1.10 14-11-20  deo, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "aw_gpio.h"
#include "aw_list.h"
#include "awbl_plb.h"
#include "awbl_spibus.h"
#include "aw_spi.h"
#include "aw_errno.h"
#include "aw_delay.h"

#include "driver/busctlr/awbl_gpio_spi.h"


/*******************************************************************************
  macro operate
*******************************************************************************/
#define __GPIO_SPI_DEV_DECL(p_this, p_dev) \
          struct awbl_gpio_spi_dev *p_this = \
         (struct awbl_gpio_spi_dev *)(p_dev)


#define __GPIO_SPI_INFO_DECL(p_info, p_dev) \
    struct awbl_gpio_spi_devinfo *p_info = \
        (struct awbl_gpio_spi_devinfo *)AWBL_DEVINFO_GET(p_dev)


/******************************************************************************/
aw_local uint8_t __gpio_spi_rw_byte (struct awbl_gpio_spi_dev *p_dev,
                                     uint8_t                   byte)
{
    __GPIO_SPI_INFO_DECL(p_info, p_dev);
    uint8_t         i;
    uint8_t         rbyte = 0;
    uint8_t         wmask, rmask;

    if (p_dev->mode & AW_SPI_LSB_FIRST) {
        wmask = 0x01;
        rmask = 0x80;
    } else {
        wmask = 0x80;
        rmask = 0x01;
    }

    /* 根据时钟相位输入第一个周期 */
    p_dev->io_set(p_info->mosi, ((byte & wmask) != 0));
    aw_udelay(p_dev->dly);
    if (p_dev->mode & AW_SPI_CPHA) {
        p_dev->sck_state = !p_dev->sck_state;
        p_dev->io_set(p_info->sck, p_dev->sck_state);
    }
    aw_udelay(p_dev->dly);

    if ( p_dev->mode & AW_SPI_3WIRE ) {
        aw_gpio_pin_cfg(p_info->mosi, AW_GPIO_INPUT  | AW_GPIO_PULL_UP);
        rbyte |= (aw_gpio_get(p_info->mosi)) ? rmask : 0x00;
        aw_gpio_pin_cfg(p_info->mosi, AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);
    } else {
        rbyte |= (aw_gpio_get(p_info->miso)) ? rmask : 0x00;
    }

    p_dev->sck_state = !p_dev->sck_state;
    p_dev->io_set(p_info->sck, p_dev->sck_state);
    aw_udelay(p_dev->dly);

    /* 剩余七个周期 */
    for (i=0; i<7; i++) {
        if (p_dev->mode & AW_SPI_LSB_FIRST) {
            byte >>= 1;
        } else {
            byte <<= 1;
        }
        p_dev->io_set(p_info->mosi, ((byte & wmask) != 0));
        p_dev->sck_state = !p_dev->sck_state;
        aw_udelay(p_dev->dly);
        p_dev->io_set(p_info->sck, p_dev->sck_state);
        aw_udelay(p_dev->dly);

        if (p_dev->mode & AW_SPI_LSB_FIRST) {
            rbyte >>= 1;
        } else {
            rbyte <<= 1;
        }

        if ( p_dev->mode & AW_SPI_3WIRE ) {
            aw_gpio_pin_cfg(p_info->mosi, AW_GPIO_INPUT  | AW_GPIO_PULL_UP);
            rbyte |= (aw_gpio_get(p_info->mosi)) ? rmask : 0x00;
            aw_gpio_pin_cfg(p_info->mosi, AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);
        } else {
            rbyte |= (aw_gpio_get(p_info->miso)) ? rmask : 0x00;
        }

        p_dev->sck_state = !p_dev->sck_state;
        p_dev->io_set(p_info->sck, p_dev->sck_state);
        aw_udelay(p_dev->dly);
    }

    /* 根据时钟极性使时钟线处于空闲状态 */
    p_dev->sck_state = ((p_dev->mode & AW_SPI_CPOL) != 0);
    p_dev->io_set(p_info->sck, p_dev->sck_state);
    aw_udelay(p_dev->dly);
    return rbyte;
}


/***********************************************************************************/
aw_local aw_err_t __gpio_spi_config (struct awbl_spi_master *p_master,
                                     struct awbl_spi_config *config)
{
    __GPIO_SPI_DEV_DECL(p_dev, p_master);
    __GPIO_SPI_INFO_DECL(p_info, p_dev);

    p_dev->mode  = config->mode;
    p_dev->bpw   = config->bpw;
    p_dev->speed = config->speed_hz;

    p_dev->sck_state = ((p_dev->mode & AW_SPI_CPOL) != 0);
    p_dev->io_set(p_info->sck, p_dev->sck_state);

    p_dev->dly = 500000 / p_dev->speed;
    if (p_dev->dly < 2) {
        p_dev->dly = 2;
    }
    return AW_OK;
}

/***********************************************************************************/
aw_local aw_err_t __gpio_spi_setup (struct awbl_spi_master *p_master,
                                    struct aw_spi_device   *p_dev)
{

    if ((p_dev->bits_per_word < 4) ||
        (p_dev->bits_per_word > 16)) {
        return -AW_ENOTSUP;
    }

    if (p_dev->mode & AW_SPI_AUTO_CS) {
        return -AW_ENOTSUP;
    }

    return AW_OK;
}


aw_local aw_err_t __gpio_spi_write_and_read(struct awbl_spi_master   *p_master,
                                            const void               *tx_buf,
                                            void                     *rx_buf,
                                            uint32_t                  nbytes)
{
    __GPIO_SPI_DEV_DECL(p_dev, p_master);
    __GPIO_SPI_INFO_DECL(p_info, p_dev);

    uint8_t *p_rx = (uint8_t *)rx_buf, *p_tx = (uint8_t *)tx_buf;
    uint8_t tmp_tx = p_info->dummp_byte;

    if (tx_buf && rx_buf) {
        while (nbytes--) {
            *p_rx++ = __gpio_spi_rw_byte(p_dev, *p_tx++);
        }
    } else if (tx_buf && !rx_buf) {
        while (nbytes--) {
            __gpio_spi_rw_byte(p_dev, *p_tx++);
        }
    } else if (!tx_buf && rx_buf) {
        while (nbytes--) {
            *p_rx++ = __gpio_spi_rw_byte(p_dev, tmp_tx);
        }
    } else {
        /*not to do*/
    }

    return AW_OK;
}

aw_local aw_err_t __gpio_spi_default_config(struct awbl_spi_config *p_cfg)
{
    p_cfg->bpw      = 8;
    p_cfg->mode     = AW_SPI_MODE_0;
    p_cfg->speed_hz = 10000;

    return AW_OK;
}


/******************************************************************************/
aw_local aw_const struct awbl_spi_master_devinfo2 __gp_gpio_spi_devinfo2 = {
        AWBL_FEATURE_SPI_CPOL_L       |
        AWBL_FEATURE_SPI_CPOL_H       |
        AWBL_FEATURE_SPI_CPHA_U       |
        AWBL_FEATURE_SPI_CPHA_D       |
        AWBL_FEATURE_SPI_LSB_FIRST    |
        AWBL_FEATURE_SPI_3WIRE ,
    0,
    __gpio_spi_setup,
    __gpio_spi_write_and_read,
    NULL,
    __gpio_spi_config,
    __gpio_spi_default_config
};

/******************************************************************************/
void __gpio_spi_hardware_init (awbl_dev_t *p_awdev)
{
    __GPIO_SPI_INFO_DECL(p_info, p_awdev);
    __GPIO_SPI_DEV_DECL(p_dev, p_awdev);

    if (p_info->hal_io_set != NULL) {
        p_dev->io_set = p_info->hal_io_set;
    } else {
        p_dev->io_set = aw_gpio_set;
    }

    if ( p_info->sck != -1) {
        aw_gpio_pin_cfg(p_info->sck,  AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);
        p_dev->io_set(p_info->sck,  1);
    }

    if ( p_info->miso != -1) {
        aw_gpio_pin_cfg(p_info->miso, AW_GPIO_INPUT  | AW_GPIO_PULL_UP);
        p_dev->io_set(p_info->miso, 1);
    }

    if ( p_info->mosi != -1) {
        aw_gpio_pin_cfg(p_info->mosi, AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);
        p_dev->io_set(p_info->mosi, 1);
    }

}

/******************************************************************************/
aw_local void __gpio_spi_inst_init1(awbl_dev_t *p_awdev)
{
    return;
}

/******************************************************************************/
aw_local void __gpio_spi_inst_init2(awbl_dev_t *p_awdev)
{
    __GPIO_SPI_INFO_DECL(p_info, p_awdev);
    __GPIO_SPI_DEV_DECL(p_dev, p_awdev);

    if (p_info->pfunc_plfm_init != NULL) {
        p_info->pfunc_plfm_init();
    }


    p_dev->super.p_devinfo2 = &__gp_gpio_spi_devinfo2;

    __gpio_spi_hardware_init(p_awdev);

    p_dev->dly = 100;
    awbl_spibus_create(&p_dev->super);

    return;
}

/******************************************************************************/
aw_local void __gpio_spi_inst_connect(awbl_dev_t *p_awdev)
{
    return;
}

/******************************************************************************/
aw_local aw_const struct awbl_drvfuncs __gp_gpio_spi_drvfuncs = {
        __gpio_spi_inst_init1,
        __gpio_spi_inst_init2,
        __gpio_spi_inst_connect
};

/******************************************************************************/
aw_local aw_const awbl_plb_drvinfo_t __gp_gpio_spi_drv_registration = {
    {
        AWBL_VER_1,
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,
        AWBL_GPIO_SPI_NAME,
        &__gp_gpio_spi_drvfuncs,
        NULL,
        NULL
    }
};


/******************************************************************************/
void awbl_gpio_spi_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__gp_gpio_spi_drv_registration);
}

/* end of file */


