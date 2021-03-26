/*
 * The Clear BSD License
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "boot_flexspi_nand_cfg.h"

#define NANDFLASH

//todo

#ifdef NANDFLASH

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(__CC_ARM) || defined(__GNUC__)
__attribute__((section(".boot_hdr.conf")))
#elif defined(__ICCARM__)
#pragma location = ".boot_hdr.conf"
#endif

#else

#endif

struct flash_control_block nandflash_control_block_FCB = {
    /*CRC校验需要代码运行一遍计算出来*/
    .crcChecksum = 0xBA580642,      /** \brief 0x000:Checksum.The “crcChecksum” calculation starts from
                                                     fingerprint to the end of FCB, 1020 bytes in total*/
    .fingerprint = 0x4E464342,      /** \brief 0x004:0x4E46_4342,ASCII: “NFCB” */
    .version = 0x00000001,          /** \brief 0x008:0x0000_0001 */
    .DBBTSearchStartPage = 64,       /** \brief 0x00C:Start Page address for bad block table search area.
                                                     If DBBT Search Area is 0 in FCB, then ROM assumes
                                                     that there are no bad blocks on NAND device boot area. */
    .searchStride = 1,              /** \brief 0x010:Search stride for DBBT and FCB search.
                                                     Not used by ROM Max value is 8.*/
    .searchCount = 1,               /** \brief 0x012:Copies of DBBT and FCB. Not used by ROM, max value is 8.*/
    .firmwareCopies = 0x01,            /** \brief 0x014:Firmware copies Valid range 1-8. */
    .Reserved0 = {0},               /** \brief 0x018:Reserved for future use Must be set to 0. */
    .firmwareInfo_Start_Page = 128,   /** \brief 0x040:Start page of this firmware.
                                                     NOTE: The StartPage must be the first page of a NAND block.*/
    .firmwareInfo_Page_Count = 0x0800,/** \brief 0x044:Pages in this firmware. limited in 4 Mbits*/
    .Reserved1 = {0},               /** \brief 0x048:Remain entries must be set to 0 */
    .Reserved2 = {0},               /** \brief 0x080:Reserved Must be set to 0 */

    .nandflash_config = {                   /** \brief 0x100:Serial NAND configuration block over FlexSPI */

        .memCfg = {
            .Tag = 0x42464346,              /** \brief 0x100:0x42464346, ascii:” FCFB” */
            .Version = 0x56010000,          /** \brief 0x104:0x56010000 / 0x56010100 "V1.00/1.10"*/
            .Reserved0 = 0,                 /** \brief 0x108:Reserved */
            .readSampleClkSrc = 0,          /** \brief 0x10C:0 – internal loopback
                                                             1 – loopback from DQS pad
                                                             3 – Flash provided DQS */
            .dataHoldTime = 0x03,              /** \brief 0x10D:Serial Flash CS Hold Time Recommend default value is 0x03 */
            .dataSetupTime = 0x03,             /** \brief 0x10E:Serial Flash CS setup time Recommended default value is 0x03 */
            .columnAdressWidth = 12,        /** \brief 0x10F:3 – For HyperFlash
                                                             12/13 – For Serial NAND, see datasheet to find correct value
                                                             0 – Other devices */
            .deviceModeCfgEnable = 1,       /** \brief 0x110:Device Mode Configuration Enable feature
                                                             0 – Disabled
                                                             1 – Enabled */
            .Reserved1 = 0,                 /** \brief 0x111:Reserved */
            .waitTimeCfgCommands = 0,       /** \brief 0x113:Wait time for all configuration commands, unit 100us. */

            .deviceModeSeq = 1,       /** \brief 0x114:Sequence parameter for device mode configuration */
            .deviceModeArg = 2,             /** \brief 0x118:Device Mode argument, effective only when deviceModeCfgEnable = 1 */
            .configCmdEnable = 0,           /** \brief 0x11C:Config Command Enable feature
                                                             0 – Disabled
                                                             1 – Enabled */
            .Reserved2 = {0},               /** \brief 0x11D:Reserved */
            .configCmdSeqs = {0},           /** \brief 0x120:Sequences for Config Command, allow 4 separate configuration command sequences. */
            .cfgCmdArgs = {0},              /** \brief 0x130:Arguments for each separate configuration command sequence. */

            .controllerMiscOption = 0x00000021,  /** \brief 0x140:Bit0 – differential clock enable.
                                                             Bit1 – CK2 enable, must set to 0 in this silicon.
                                                             Bit2 – ParallelModeEnable, must set to 0 for this silicon.
                                                             Bit3 – wordAddressableEnable.
                                                             Bit4 – Safe Configuration Frequency enable set to 1 for
                                                                    the devices that support DDR Read instructions.
                                                             Bit5 – Pad Setting Override Enable
                                                             Bit6 – DDR Mode Enable,set to 1 for device supports DDR read command */
            .deviceType = 0x02,             /** \brief 0x144: 1 – Serial NOR 2 – Serial NAND */
            .sflashPadType = 0x04,          /** \brief 0x145: 1 – Single pad
                                                              2 – Dual pads
                                                              4 – Quad pads
                                                              8 – Octal pads */
            .serialClkFreq = 0x06,          /** \brief 0x146:Chip specific value, for this silicon
                                                              1 – 30 MHz
                                                              2 – 50 MHz
                                                              3 – 60 MHz
                                                              4 – 75 MHz
                                                              5 – 80 MHz
                                                              6 – 100 MHz
                                                              7 – 133 MHz
                                                              8 – 166 MHz
                                                              Other value: 30 MHz */
            .lutCustomSeqEnable = 0x00,     /** \brief 0x147: 0 – Use pre-defined LUT sequence index and number
                                                              1 - Use LUT sequence parameters provided in this block */
            .Reserved3 = {0},               /** \brief 0x148:Reserved */

            .sflashA1Size = 0x10000000,     /** \brief 0x150:For SPI NOR,need to fill with actual size.
                                                             For SPI NAND,need to fill with actual size * 2
                                                             128 * 1024 * 1024 * 2 = 0x10000000 */
            .sflashA2Size = 0,              /** \brief 0x154:The same as above */
            .sflashB1Size = 0,              /** \brief 0x158:The same as above */
            .sflashB2Size = 0,              /** \brief 0x15C:The same as above */

            .csPadSettingOverride = 0,      /** \brief 0x160:Set to 0 if it is not supported */
            .sclkPadSettingOverride = 0,    /** \brief 0x164:Set to 0 if it is not supported */
            .dataPadSettingOverride = 0,    /** \brief 0x168:Set to 0 if it is not supported */
            .dqsPadSettingOverride = 0,     /** \brief 0x16C:Set to 0 if it is not supported */

            .timeoutInMs = 10,               /** \brief 0x170:Maximum wait time during read busy status
                                                             0 – Disabled timeout checking feature Other value – Timeout
                                                             if the wait time exceeds this value. */
            .commandInterval = 0,           /** \brief 0x174:Unit: ns Currently, it is used for SPI NAND only at high frequency */
            .dataValidTime = 0x00000003,    /** \brief 0x178:Time from clock edge to data valid edge, unit ns. This field
                                                             is used when the FlexSPI Root clock is less than 100 MHz
                                                             and the read sample clock source is device
                                                             provided DQS signal without CK2 support.
                                                             [31:16] data valid time for DLLB in terms of 0.1 ns
                                                             [15:0] data valid time for DLLA in terms of 0.1 ns */
            .busyOffset = 0,                /** \brief 0x17C:busy bit offset, valid range : 0-31 */
            .busyBitPolarity = 0x00,           /** \brief 0x17E:0 – busy bit is 1 if device is busy
                                                             1 – busy bit is 0 if device is busy */
//            /* 单线--SDR */
//            .ReadFromCache_LUT_INDEX_0[0] = 0x0C100403,
//            .ReadFromCache_LUT_INDEX_0[1] = 0x24043008,
//            .ReadFromCache_LUT_INDEX_0[2] = 0,
//            .ReadFromCache_LUT_INDEX_0[3] = 0,
            /*四线--SDR*/
            .ReadFromCache_LUT_INDEX_0[0] = 0x0C10046B,
            .ReadFromCache_LUT_INDEX_0[1] = 0x26803208,
            .ReadFromCache_LUT_INDEX_0[2] = 0,
            .ReadFromCache_LUT_INDEX_0[3] = 0,

            .ReadStatus_LUT_INDEX_1[0] = 0x04C0040F,
            .ReadStatus_LUT_INDEX_1[1] = 0X00002401,
            .ReadStatus_LUT_INDEX_1[2] = 0,
            .ReadStatus_LUT_INDEX_1[3] = 0,

            .WriteEnable_LUT_INDEX_3[0] = 0X00000406,
            .WriteEnable_LUT_INDEX_3[1] = 0,
            .WriteEnable_LUT_INDEX_3[2] = 0,
            .WriteEnable_LUT_INDEX_3[3] = 0,

            .BlockErase_LUT_INDEX_5[0] = 0X081804D8,
            .BlockErase_LUT_INDEX_5[1] = 0,
            .BlockErase_LUT_INDEX_5[2] = 0,
            .BlockErase_LUT_INDEX_5[3] = 0,

            /* 单线--SDR */
//            .ProgramLoad_LUT_INDEX_9[0] = 0X0C100402,
//            .ProgramLoad_LUT_INDEX_9[1] = 0X00002001,
//            .ProgramLoad_LUT_INDEX_9[2] = 0,
//            .ProgramLoad_LUT_INDEX_9[3] = 0,
            /*四线--SDR*/
            .ProgramLoad_LUT_INDEX_9[0] = 0X0C100432,
            .ProgramLoad_LUT_INDEX_9[1] = 0X00002280,
            .ProgramLoad_LUT_INDEX_9[2] = 0,
            .ProgramLoad_LUT_INDEX_9[3] = 0,

            .ReadPage_LUT_INDEX_11[0] = 0X08180413,
            .ReadPage_LUT_INDEX_11[1] = 0,
            .ReadPage_LUT_INDEX_11[2] = 0,
            .ReadPage_LUT_INDEX_11[3] = 0,

            .ReadEccStatus_LUT_INDEX_13[0] = 0x04C0040F,
            .ReadEccStatus_LUT_INDEX_13[1] = 0X00002401,
            .ReadEccStatus_LUT_INDEX_13[2] = 0,
            .ReadEccStatus_LUT_INDEX_13[3] = 0,

            .ProgramExecute_LUT_INDEX_14[0] = 0X08180410,
            .ProgramExecute_LUT_INDEX_14[1] = 0,
            .ProgramExecute_LUT_INDEX_14[2] = 0,
            .ProgramExecute_LUT_INDEX_14[3] = 0,

            .Reserved_LUT_INDEX_2 = {0},
            .ReadFromCacheOdd_LUT_INDEX_4 = {0},
            .Reserved_LUT_INDEX_6 = {0},
            .Reserved_LUT_INDEX_7 = {0},
            .Reserved_LUT_INDEX_8 = {0},
            .ProgramLoadOdd_LUT_INDEX_10 = {0},
            .Reserved_LUT_INDEX_12 = {0},
            .Reserved_LUT_INDEX_15 = {0},

            .lutCustomSeq = {0},                /*command index for specific device set by user.
                                                  Note: if lutCustomSeqEnable is not be set, the field below is ignore*/
            .Reserved4 = {0},
        },

        .pageDataSize = 0x0800,     /** \brief 0x1C0:Page size in terms of bytes,usually, it is 2048 or 4096 */
        .pageTotalSize = 4096,    /** \brief 0x1C4:It equals to 2 ^ width of column adddress  */
        .pagesPerBlock = 0x40,      /** \brief 0x1C8:Pages in one block */
        .bypassReadStatus = 0,   /** \brief 0x1CC:0 – Read Status Register
                                                     1 – Bypass Read status register */
        .bypassEccRead = 0,         /** \brief 0x1CD:0 – Perform ECC read
                                                     1 – Bypass ECC read */
        .hasMultiPlanes = 0,        /** \brief 0x1CE:0 – Only 1 plane
                                                     1 – Has two planes */
        .skippOddBlocks = 0,        /** \brief 0x1CF:0 – Read Odd blocks
                                                     1 – Skip Odd blocks */
        .eccCheckCustomEnable = 0,  /** \brief 0x1D0:0 – Use the common ECC check command and ECC related masks
                                                     1 - Use ECC check related masks provided in this configuration block */
        .ipCmdSerialClkFreq = 0x06,    /** \brief 0x1D1:Chip specific value, not used by ROM
                                                     0 – No change, keep current serial clock unchanged
                                                     1 – 30 MHz
                                                     2 – 50 MHz
                                                     3 – 60 MHz
                                                     4 – 75 MHz
                                                     5 – 80 MHz
                                                     6 – 100 MHz
                                                     7 – 133 MHz
                                                     8 – 166 MHz */
        .readPageTimeUs = 0,   /** \brief 0x1D2:Wait time during page read,this field will take effect on if
                                                     the bypassReadStatus is set to 1.NOTE: Only applicable to ROM. */
        .eccStatusMask = 0,         /** \brief 0x1D4:ECC Status Mask */
        .eccFailureMask = 0,        /** \brief 0x1D8:ECC Check Failure mask */
        .blocksPerDevice = 0x0400,  /** \brief 0x1DC:Blocks in a Serial NAND */
        .Reserved0 = {0},
    },
    .Reserved3 = {0},
};

//todo


#ifdef NANDFLASH
struct discovered_bad_block_table discovered_bad_block_DBBT __attribute__((section(".boot_hdr.dbbt"))) = {
#else
struct discovered_bad_block_table discovered_bad_block_DBBT = {
#endif

    /*CRC校验需要代码运行一遍计算出来*/
    .crcChecksum = 0x50D7539B,      /** \brief 0x000:Checksum.The "crcChecksum" is calculated with the same
                                                     algorithm as the one in FCB, from Fingerprint to the
                                                     end of DBBT, 1052 bytes in total. */
    .Fingerprint = 0x44424254,      /** \brief 0x004:32-bit word with a value of 0x4442_4254, in asci “DBBT” */
    .Version = 0x00000001,          /** \brief 0x008:32-bit version number, this version of DBBT is 0x00000001 */
    .Reserver0 = 0,                 /** \brief 0x00C:Reserved */
    .badBlockNumber =0,             /** \brief 0x010:Number of bad blocks */
    .Reserved1 = {0},               /** \brief 0x014:Must be filled with 0x00s */
    .BadBlockEntries = {0},         /** \brief 0x020-0x41F:Bad Block entries, only the first “badBlockNumber”
                                                     entries are valid, the remaining entries must be filled with 0x00s */
};





















