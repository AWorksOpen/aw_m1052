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
 * @file
 * @brief telnet defines.
 *
 * @internal
 * @par History
 * - 150430, rnk, First implementation.
 * @endinternal
 */

#ifndef __AW_TELNET_H
#define __AW_TELNET_H

#define TELCMD_SE       0xF0
#define TELCMD_NOP      0xF1
#define TELCMD_DM       0xF2
#define TELCMD_BRK      0xF3
#define TELCMD_IP       0xF4
#define TELCMD_AO       0xF5
#define TELCMD_AYT      0xF6
#define TELCMD_EC       0xF7
#define TELCMD_EL       0xF8
#define TELCMD_GA       0xF9
#define TELCMD_SB       0xFA
#define TELCMD_WILL     0xFB
#define TELCMD_WONT     0xFC
#define TELCMD_DO       0xFD
#define TELCMD_DONT     0xFE
#define TELCMD_IAC      0xFF

/** \brief telnet mandatory control codes */
#define TELCC_NULL      '\0'
#define TELCC_LF        '\n'
#define TELCC_CR        '\r'

#define TELOPT_BINARY   0        /**< \brief Use 8 bit binary transmission */
#define TELOPT_ECHO     1        /**< \brief Echo data received */
#define TELOPT_SGA      3        /**< \brief Suppress Go-ahead signal */

#define TELNET_MAX_OPTS 4

#endif /* __AW_TELNET_H */

/* end of file */
