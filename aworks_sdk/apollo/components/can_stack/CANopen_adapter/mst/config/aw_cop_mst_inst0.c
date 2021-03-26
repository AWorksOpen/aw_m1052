/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus CANopen Master type implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.02 15-12-02  cae, first implementation
 * \endinternal
 */

// -------------------------------------------------------------------------
// process variables for application
BYTE  MEM   abDigitalIn8BitInst0_g[16];
WORD  MEM   awDigitalIn16BitInst0_g[8];

BYTE  MEM   abDigitalOut8BitInst0_g[16];
WORD  MEM   awDigitalOut16BitInst0_g[8];

WORD  MEM   awAnalogIn16BitInst0_g [4];
DWORD MEM   adwAnalogIn32BitInst0_g[4];
DWORD MEM   adwAnalogUpLimitInst0_g[4];
DWORD MEM   adwAnalogLoLimitInst0_g[4];

WORD  MEM   awAnalogOut16BitInst0_g [4];
DWORD MEM   adwAnalogOut32BitInst0_g[4];

// -------------------------------------------------------------------------
// table with all process variables
CONST tVarParam ROM aVarTabInst0_g[] =
{
    // ---------------------------------------------------------------------
    // digital input 8 bit
    {kVarValidAll, 0x6000, 0x01, sizeof(BYTE), &abDigitalIn8BitInst0_g[0], NULL, NULL},
    {kVarValidAll, 0x6000, 0x02, sizeof(BYTE), &abDigitalIn8BitInst0_g[1], NULL, NULL},
    {kVarValidAll, 0x6000, 0x03, sizeof(BYTE), &abDigitalIn8BitInst0_g[2], NULL, NULL},
    {kVarValidAll, 0x6000, 0x04, sizeof(BYTE), &abDigitalIn8BitInst0_g[3], NULL, NULL},
    {kVarValidAll, 0x6000, 0x05, sizeof(BYTE), &abDigitalIn8BitInst0_g[4], NULL, NULL},
    {kVarValidAll, 0x6000, 0x06, sizeof(BYTE), &abDigitalIn8BitInst0_g[5], NULL, NULL},
    {kVarValidAll, 0x6000, 0x07, sizeof(BYTE), &abDigitalIn8BitInst0_g[6], NULL, NULL},
    {kVarValidAll, 0x6000, 0x08, sizeof(BYTE), &abDigitalIn8BitInst0_g[7], NULL, NULL},
    {kVarValidAll, 0x6000, 0x09, sizeof(BYTE), &abDigitalIn8BitInst0_g[8], NULL, NULL},
    {kVarValidAll, 0x6000, 0x0A, sizeof(BYTE), &abDigitalIn8BitInst0_g[9], NULL, NULL},
    {kVarValidAll, 0x6000, 0x0B, sizeof(BYTE), &abDigitalIn8BitInst0_g[10], NULL, NULL},
    {kVarValidAll, 0x6000, 0x0C, sizeof(BYTE), &abDigitalIn8BitInst0_g[11], NULL, NULL},
    {kVarValidAll, 0x6000, 0x0D, sizeof(BYTE), &abDigitalIn8BitInst0_g[12], NULL, NULL},
    {kVarValidAll, 0x6000, 0x0E, sizeof(BYTE), &abDigitalIn8BitInst0_g[13], NULL, NULL},
    {kVarValidAll, 0x6000, 0x0F, sizeof(BYTE), &abDigitalIn8BitInst0_g[14], NULL, NULL},
    {kVarValidAll, 0x6000, 0x10, sizeof(BYTE), &abDigitalIn8BitInst0_g[15], NULL, NULL},

#ifndef COP_KIT

    {kVarValidAll, 0x6100, 0x01, sizeof(WORD), &awDigitalIn16BitInst0_g[0], NULL, NULL},
    {kVarValidAll, 0x6100, 0x02, sizeof(WORD), &awDigitalIn16BitInst0_g[1], NULL, NULL},
    {kVarValidAll, 0x6100, 0x03, sizeof(WORD), &awDigitalIn16BitInst0_g[2], NULL, NULL},
    {kVarValidAll, 0x6100, 0x04, sizeof(WORD), &awDigitalIn16BitInst0_g[3], NULL, NULL},
    {kVarValidAll, 0x6100, 0x05, sizeof(WORD), &awDigitalIn16BitInst0_g[4], NULL, NULL},
    {kVarValidAll, 0x6100, 0x06, sizeof(WORD), &awDigitalIn16BitInst0_g[5], NULL, NULL},
    {kVarValidAll, 0x6100, 0x07, sizeof(WORD), &awDigitalIn16BitInst0_g[6], NULL, NULL},
    {kVarValidAll, 0x6100, 0x08, sizeof(WORD), &awDigitalIn16BitInst0_g[7], NULL, NULL},

#endif // COP_KIT

    // 8 bit digital output
    // ---------------------------------------------------------------------
    // digital output 8 bit
    {kVarValidAll, 0x6200, 0x01, sizeof(BYTE), &abDigitalOut8BitInst0_g[0], NULL, NULL},
    {kVarValidAll, 0x6200, 0x02, sizeof(BYTE), &abDigitalOut8BitInst0_g[1], NULL, NULL},
    {kVarValidAll, 0x6200, 0x03, sizeof(BYTE), &abDigitalOut8BitInst0_g[2], NULL, NULL},
    {kVarValidAll, 0x6200, 0x04, sizeof(BYTE), &abDigitalOut8BitInst0_g[3], NULL, NULL},
    {kVarValidAll, 0x6200, 0x05, sizeof(BYTE), &abDigitalOut8BitInst0_g[4], NULL, NULL},
    {kVarValidAll, 0x6200, 0x06, sizeof(BYTE), &abDigitalOut8BitInst0_g[5], NULL, NULL},
    {kVarValidAll, 0x6200, 0x07, sizeof(BYTE), &abDigitalOut8BitInst0_g[6], NULL, NULL},
    {kVarValidAll, 0x6200, 0x08, sizeof(BYTE), &abDigitalOut8BitInst0_g[7], NULL, NULL},
    {kVarValidAll, 0x6200, 0x09, sizeof(BYTE), &abDigitalOut8BitInst0_g[8], NULL, NULL},
    {kVarValidAll, 0x6200, 0x0A, sizeof(BYTE), &abDigitalOut8BitInst0_g[9], NULL, NULL},
    {kVarValidAll, 0x6200, 0x0B, sizeof(BYTE), &abDigitalOut8BitInst0_g[10], NULL, NULL},
    {kVarValidAll, 0x6200, 0x0C, sizeof(BYTE), &abDigitalOut8BitInst0_g[11], NULL, NULL},
    {kVarValidAll, 0x6200, 0x0D, sizeof(BYTE), &abDigitalOut8BitInst0_g[12], NULL, NULL},
    {kVarValidAll, 0x6200, 0x0E, sizeof(BYTE), &abDigitalOut8BitInst0_g[13], NULL, NULL},
    {kVarValidAll, 0x6200, 0x0F, sizeof(BYTE), &abDigitalOut8BitInst0_g[14], NULL, NULL},
    {kVarValidAll, 0x6200, 0x10, sizeof(BYTE), &abDigitalOut8BitInst0_g[15], NULL, NULL},

#ifndef COP_KIT

    // ---------------------------------------------------------------------
    // digital output 16 bit
    {kVarValidAll, 0x6300, 0x01, sizeof(WORD), &awDigitalOut16BitInst0_g[0], NULL, NULL},
    {kVarValidAll, 0x6300, 0x02, sizeof(WORD), &awDigitalOut16BitInst0_g[1], NULL, NULL},
    {kVarValidAll, 0x6300, 0x03, sizeof(WORD), &awDigitalOut16BitInst0_g[2], NULL, NULL},
    {kVarValidAll, 0x6300, 0x04, sizeof(WORD), &awDigitalOut16BitInst0_g[3], NULL, NULL},
    {kVarValidAll, 0x6300, 0x05, sizeof(WORD), &awDigitalOut16BitInst0_g[4], NULL, NULL},
    {kVarValidAll, 0x6300, 0x06, sizeof(WORD), &awDigitalOut16BitInst0_g[5], NULL, NULL},
    {kVarValidAll, 0x6300, 0x07, sizeof(WORD), &awDigitalOut16BitInst0_g[6], NULL, NULL},
    {kVarValidAll, 0x6300, 0x08, sizeof(WORD), &awDigitalOut16BitInst0_g[7], NULL, NULL},

    // ---------------------------------------------------------------------
    // analog input 16 bit
    {kVarValidAll, 0x6401, 0x01, sizeof(WORD), &awAnalogIn16BitInst0_g[0], NULL, NULL},
    {kVarValidAll, 0x6401, 0x02, sizeof(WORD), &awAnalogIn16BitInst0_g[1], NULL, NULL},
    {kVarValidAll, 0x6401, 0x03, sizeof(WORD), &awAnalogIn16BitInst0_g[2], NULL, NULL},
    {kVarValidAll, 0x6401, 0x04, sizeof(WORD), &awAnalogIn16BitInst0_g[3], NULL, NULL},

    // ---------------------------------------------------------------------
    // analog input 32 bit
    {kVarValidAll, 0x6402, 0x01, sizeof(DWORD), &adwAnalogIn32BitInst0_g[0], NULL, NULL},
    {kVarValidAll, 0x6402, 0x02, sizeof(DWORD), &adwAnalogIn32BitInst0_g[1], NULL, NULL},
    {kVarValidAll, 0x6402, 0x03, sizeof(DWORD), &adwAnalogIn32BitInst0_g[2], NULL, NULL},
    {kVarValidAll, 0x6402, 0x04, sizeof(DWORD), &adwAnalogIn32BitInst0_g[3], NULL, NULL},

    // ---------------------------------------------------------------------
    // analog output 16 bit
    {kVarValidAll, 0x6411, 0x01, sizeof(WORD), &awAnalogOut16BitInst0_g[0], NULL, NULL},
    {kVarValidAll, 0x6411, 0x02, sizeof(WORD), &awAnalogOut16BitInst0_g[1], NULL, NULL},
    {kVarValidAll, 0x6411, 0x03, sizeof(WORD), &awAnalogOut16BitInst0_g[2], NULL, NULL},
    {kVarValidAll, 0x6411, 0x04, sizeof(WORD), &awAnalogOut16BitInst0_g[3], NULL, NULL},

    // ---------------------------------------------------------------------
    // analog output 32 bit
    {kVarValidAll, 0x6412, 0x01, sizeof(DWORD), &adwAnalogOut32BitInst0_g[0], NULL, NULL},
    {kVarValidAll, 0x6412, 0x02, sizeof(DWORD), &adwAnalogOut32BitInst0_g[1], NULL, NULL},
    {kVarValidAll, 0x6412, 0x03, sizeof(DWORD), &adwAnalogOut32BitInst0_g[2], NULL, NULL},
    {kVarValidAll, 0x6412, 0x04, sizeof(DWORD), &adwAnalogOut32BitInst0_g[3], NULL, NULL},

    // ---------------------------------------------------------------------
    // analog input upper limit
    {kVarValidAll, 0x6424, 0x01, sizeof(DWORD), &adwAnalogUpLimitInst0_g[0], NULL, NULL},
    {kVarValidAll, 0x6424, 0x02, sizeof(DWORD), &adwAnalogUpLimitInst0_g[1], NULL, NULL},
    {kVarValidAll, 0x6424, 0x03, sizeof(DWORD), &adwAnalogUpLimitInst0_g[2], NULL, NULL},
    {kVarValidAll, 0x6424, 0x04, sizeof(DWORD), &adwAnalogUpLimitInst0_g[3], NULL, NULL},

    // ---------------------------------------------------------------------
    // analog input lower limit
    {kVarValidAll, 0x6425, 0x01, sizeof(DWORD), &adwAnalogLoLimitInst0_g[0], NULL, NULL},
    {kVarValidAll, 0x6425, 0x02, sizeof(DWORD), &adwAnalogLoLimitInst0_g[1], NULL, NULL},
    {kVarValidAll, 0x6425, 0x03, sizeof(DWORD), &adwAnalogLoLimitInst0_g[2], NULL, NULL},
    {kVarValidAll, 0x6425, 0x04, sizeof(DWORD), &adwAnalogLoLimitInst0_g[3], NULL, NULL}

#endif // COP_KIT
};

// -------------------------------------------------------------------------
// table with all PDOs
tPdoParam aPdoTabInst0_g[] =
{
   /*
     *  -------------------------------------------- RxPDOs --------------------
     *  receive with COB-ID from TxPDO 1 of node 1
     */
    {kPdoValidAll, 0x1400, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1401, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1402, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1403, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1404, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1405, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1406, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1409, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x140a, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x140b, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x140c, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x140d, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x140e, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x140f, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1410, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1411, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1412, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1413, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1414, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1415, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1416, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1417, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1418, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1419, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x141a, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x141b, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x141c, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x141d, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x141e, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x141f, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1420, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1421, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1422, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1423, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1424, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1425, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1426, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1427, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1428, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1429, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x142a, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x142b, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x142c, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x142d, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x142e, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x142f, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1430, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1431, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1432, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1433, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1434, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1435, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1436, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1437, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1438, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1439, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x143a, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x143b, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x143c, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x143d, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x143e, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x143f, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1440, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1441, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1442, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1443, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1444, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1445, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1446, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1447, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1448, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1449, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x144a, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x144b, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x144c, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x144d, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x144e, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x144f, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1450, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1451, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1452, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1453, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1454, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1455, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1456, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1457, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1458, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1459, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x145a, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x145b, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x145c, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x145d, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x145e, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x145f, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1460, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1461, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1462, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1463, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1464, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1465, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1466, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1467, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1468, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1469, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x146a, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x146b, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x146c, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x146d, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x146e, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x146f, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1470, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1471, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1472, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1473, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1474, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1475, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1476, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1477, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1478, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x1479, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x147a, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x147b, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x147c, kPdoTypeRecv,0,
        (0x80000180 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x147d, kPdoTypeRecv,0,
        (0x80000280 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x147e, kPdoTypeRecv,0,
        (0x80000380 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    {kPdoValidAll, 0x147f, kPdoTypeRecv,0,
        (0x80000480 + 0), 255,   0, 0,
        8, {0x62000108, 0x62000208, 0x62000308, 0x62000408, 0x62000508, 0x62000608, 0x62000708, 0x62000808},
        AppCbRxPdo},

    /*
     *  -------------------------------------------- TxPDOs ---------------------------------------------
     *  COB-ID like Pre Defined Connection Set
     */
    {kPdoValidAll, 0x1800, kPdoTypeSend,0,
        (0xC0000200 + 0), 255,   1, 0,
        8, {0x60000108, 0x60000208, 0x60000308, 0x60000408, 0x60000508, 0x60000608, 0x60000708, 0x60000808},
        AppCbTxPdo},

    {kPdoValidAll, 0x1801, kPdoTypeSend,0,
        (0xC0000300 + 0), 255,   1, 0,
        8, {0x60000108, 0x60000208, 0x60000308, 0x60000408, 0x60000508, 0x60000608, 0x60000708, 0x60000808},
        AppCbTxPdo},

    {kPdoValidAll, 0x1802, kPdoTypeSend,0,
        (0xC0000400 + 0), 255, 50000, 0,
        8, {0x60000108, 0x60000208, 0x60000308, 0x60000408, 0x60000508, 0x60000608, 0x60000708, 0x60000808},
        AppCbTxPdo},

    {kPdoValidAll, 0x1803, kPdoTypeSend,0,
        (0xC0000500 + 0), 255, 50000, 0,
        8, {0x60000108, 0x60000208, 0x60000308, 0x60000408, 0x60000508, 0x60000608, 0x60000708, 0x60000808},
        AppCbTxPdo}
};

/******************************************************************************
            init table for all NMT slave nodes
 ******************************************************************************/
BYTE ROM aSlaveTabInst0_g[AW_COP_MAX_SLAVES] = {0};

/*******************************************************************************
                     init table for all SDO server 
 ******************************************************************************/
tSdocParam ROM aSdoClientTabInst0_g[AW_COP_MAX_SLAVES]= {{0}};

/******************************************************************************
 *  init table with all Heartbeat Producers
 ******************************************************************************/
tHbcProdParam aHeartbeatProducerParamInst0_g[AW_COP_MAX_SLAVES] = {{0}};

/******************************************************************************
 *  default parameters for all CANopen nodes controlled by life guarding
 ******************************************************************************/
tNmtmSlaveParam DefaultLifeguardParameterInst0_g = {1000, 3};

/******************************************************************************
 *  cop data
 ******************************************************************************/
aw_cop_data_t MEM g_cop_data_inst0 = {
    aSlaveTabInst0_g,
    sizeof(aSlaveTabInst0_g),

    aSdoClientTabInst0_g,
    sizeof(aSdoClientTabInst0_g),

    aPdoTabInst0_g,
    sizeof(aPdoTabInst0_g),

    aHeartbeatProducerParamInst0_g,
    sizeof(aHeartbeatProducerParamInst0_g),

    &DefaultLifeguardParameterInst0_g,

    (tVarParam *)aVarTabInst0_g,
    sizeof(aVarTabInst0_g),

    abDigitalIn8BitInst0_g,
    sizeof(abDigitalIn8BitInst0_g),
    awDigitalIn16BitInst0_g,
    sizeof(awDigitalIn16BitInst0_g),

    abDigitalOut8BitInst0_g,
    sizeof(abDigitalOut8BitInst0_g),
    awDigitalOut16BitInst0_g,
    sizeof(awDigitalOut16BitInst0_g),

    awAnalogIn16BitInst0_g,
    sizeof(awAnalogIn16BitInst0_g),
    adwAnalogIn32BitInst0_g,
    sizeof(adwAnalogIn32BitInst0_g),

    awAnalogOut16BitInst0_g,
    sizeof(awAnalogOut16BitInst0_g),
    adwAnalogOut32BitInst0_g,
    sizeof(adwAnalogOut32BitInst0_g),

    adwAnalogUpLimitInst0_g,
    sizeof(adwAnalogUpLimitInst0_g),
    adwAnalogLoLimitInst0_g,
    sizeof(adwAnalogLoLimitInst0_g),
};

