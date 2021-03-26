#ifndef INCLUDED_WWD_BUS_PROTOCOL_H
#define INCLUDED_WWD_BUS_PROTOCOL_H

#ifndef AXIO_AWORKS
#if defined(AW_DRV_AWBL_CYW43362_WIFI_SDIO) || \
    defined(AW_DRV_AWBL_CYW43455_WIFI_SDIO) || \
    defined(AW_DRV_AWBL_CYW43438_WIFI_SDIO)
#include "SDIO/wwd_bus_protocol.h"
#elif defined(AW_DRV_AWBL_CYW43362_WIFI_SPI)
#include "SPI/wwd_bus_protocol.h"
#else
#error "Specify wwd bus protocol pls."
#endif
#else
#include "aw_common_drivers_autoconf.h"
#if defined(CONFIG_DRV_CYW43362_SDIO) || \
    defined(CONFIG_DRV_CYW43455_SDIO) || \
    defined(CONFIG_DRV_CYW43438_SDIO)
#include "SDIO/wwd_bus_protocol.h"
#elif defined(CONFIG_DRV_CYW43362_SPI)
#include "SPI/wwd_bus_protocol.h"
#else
#error "Specify wwd bus protocol pls."
#endif
    
#endif

#endif
