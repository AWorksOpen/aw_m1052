/** @file
 *  NVRAM variables which define BCM43362 Parameters for the
 *  USI module used on the BCM943362WCD4_3 board
 *
 */

#ifndef __CYW43362_WIFI_NVRAM_AP6181_H
#define __CYW43362_WIFI_NVRAM_AP6181_H

#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Character array of NVRAM image
 */

static const uint8_t cyw43362_wifi_nvram_ap6181[] =
    //#AP6181_NVRAM_V1.1_01152013
    //#adjuest PA parameter for g/n mode
    "manfid=0x2d0""\x00"
    "prodid=0x492""\x00"
    "vendid=0x14e4""\x00"
    "devid=0x4343""\x00"
    "boardtype=0x0598""\x00"

    //# Board Revision is P307, same nvram file can be used for P304, P305, P306 and P307 as the tssi pa params used are same
    //#Please force the automatic RX PER data to the respective board directory if not using P307 board, for e.g. for P305 boards force the data into the following directory /projects/SPI_BCM43362/a1_labdata/boardtests/results/sdg_rev0305
    "boardrev=0x1307""\x00"
    "boardnum=777""\x00"
    "xtalfreq=26000""\x00"
    "boardflags=0xa00""\x00"
    "sromrev=3""\x00"
    "wl0id=0x431b""\x00"
    "macaddr=00:04:9f:00:01:10""\x00"
    "aa2g=1""\x00"
    "ag0=2""\x00"
    "maxp2ga0=74""\x00"
    "cck2gpo=0x2222""\x00"
    "ofdm2gpo=0x66666666""\x00"
    "mcs2gpo0=0x7777""\x00"
    "mcs2gpo1=0x7777""\x00"
    "pa0maxpwr=56""\x00"

    //#P207 PA params
    //#pa0b0=5447
    //#pa0b1=-658
    //#pa0b2=-175<div></div>
    //
    //#Same PA params for P304,P305, P306, P307
    "pa0b0=5447""\x00"
    "pa0b1=-607""\x00"
    "pa0b2=-160""\x00"
    "pa0itssit=62""\x00"
    "pa1itssit=62""\x00"

    "cckPwrOffset=5""\x00"
    "ccode=0""\x00"
    "rssismf2g=0xa""\x00"
    "rssismc2g=0x3""\x00"
    "rssisav2g=0x7""\x00"
    "triso2g=0""\x00"
    "noise_cal_enable_2g=0""\x00"
    "noise_cal_po_2g=0""\x00"
    "swctrlmap_2g=0x04040404,0x02020202,0x02020202,0x010101,0x1ff""\x00"
    "temp_add=29767""\x00"
    "temp_mult=425""\x00"

    "\x00\x00";

#ifdef __cplusplus
} /* extern "C" */
#endif

#else

#error Wi-Fi NVRAM image included twice

#endif
