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

#include "evkmimxrt1010_flexspi_nor_config.h"


//Pre-defined LUT index
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD                         0
#define NOR_CMD_LUT_SEQ_IDX_READ_STATUSREG                         1
#define NOR_CMD_LUT_SEQ_IDX_WRITE_ENABLE                           3 
#define NOR_CMD_LUT_SEQ_IDX_ERASE_SECTOR                           5
#define NOR_CMD_LUT_SEQ_IDX_ERASE_BLOCK                            8
#define NOR_CMD_LUT_SEQ_IDX_PAGE_PROGRAM_SINGLE                    9
#define NOR_CMD_LUT_SEQ_IDX_CHIP_ERASE                             11
#define NOR_CMD_LUT_SEQ_IDX_DUMMY                                  15

//Reserved
#define NOR_CMD_LUT_SEQ_IDX_WRITE_STATUSREG                        2
#define NOR_CMD_LUT_SEQ_IDX_READ_ID                                4
/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(__CC_ARM) || defined(__GNUC__)
__attribute__((section(".boot_hdr.conf")))
#elif defined(__ICCARM__)
#pragma location = ".boot_hdr.conf"
#endif

const flexspi_nor_config_t qspiflash_config = {
    .memConfig =
        {
            .tag = FLEXSPI_CFG_BLK_TAG,
            .version = FLEXSPI_CFG_BLK_VERSION,
            .readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackInternally,
            .csHoldTime = 3u,
            .csSetupTime = 3u,
            
            .waitTimeCfgCommands = 0x10,
            // Enable DDR mode, Wordaddassable, Safe configuration, Differential clock
            .deviceType = kFlexSpiDeviceType_SerialNOR,
            .sflashPadType = kSerialFlash_4Pads,
            .serialClkFreq = kFlexSpiSerialClk_133MHz,
            .sflashA1Size = 8u * 1024u * 1024u,
            
            .busyOffset = 0x000F,
            .busyBitPolarity = 0x01,
            .lutCustomSeqEnable = false,
            .lookupTable =
                {
                    // QSPI Read
                    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD] = 
                        FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x6B, RADDR_SDR, FLEXSPI_1PAD, 0x18),
                    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD + 1] = 
                        FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, 0x08, READ_SDR, FLEXSPI_4PAD, 0x4),

                    // Read Status
                    [4 * NOR_CMD_LUT_SEQ_IDX_READ_STATUSREG] = 
                        FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x05, READ_SDR, FLEXSPI_1PAD, 0x04),

                    // Write enable
                    [4 * NOR_CMD_LUT_SEQ_IDX_WRITE_ENABLE] = 
                        FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x06, STOP, FLEXSPI_1PAD, 0x00),

                    // Erase Sector
                    [4 * NOR_CMD_LUT_SEQ_IDX_ERASE_SECTOR] = 
                        FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xD7, RADDR_SDR, FLEXSPI_1PAD, 0x18),

                    // Erase Block 64KB
                    [4 * NOR_CMD_LUT_SEQ_IDX_ERASE_BLOCK] = 
                        FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x52, RADDR_SDR, FLEXSPI_1PAD, 0x18),

                    // Page Program
                    [4 * NOR_CMD_LUT_SEQ_IDX_PAGE_PROGRAM_SINGLE] = 
                        FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x02, RADDR_SDR, FLEXSPI_1PAD, 0x18),
                    [4 * NOR_CMD_LUT_SEQ_IDX_PAGE_PROGRAM_SINGLE + 1] = 
                        FLEXSPI_LUT_SEQ(WRITE_SDR, FLEXSPI_1PAD, 0x04, STOP, FLEXSPI_1PAD, 0x00),

                    // Chip Erase
                    [4 * NOR_CMD_LUT_SEQ_IDX_CHIP_ERASE] = 
                        FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xC7, STOP, FLEXSPI_1PAD, 0x00),

                    // Dummy
                    [4 * NOR_CMD_LUT_SEQ_IDX_DUMMY] = 0,

                    // write status reg
                    [4 * NOR_CMD_LUT_SEQ_IDX_WRITE_STATUSREG] = 
                        FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x01, WRITE_SDR, FLEXSPI_1PAD, 0x04),

                    // read id
                    [4 * NOR_CMD_LUT_SEQ_IDX_READ_ID] = 
                        FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xAB, DUMMY_SDR, FLEXSPI_1PAD, 0x18),
                    [4 * NOR_CMD_LUT_SEQ_IDX_READ_ID + 1] = 
                        FLEXSPI_LUT_SEQ(READ_SDR, FLEXSPI_1PAD, 0x04, STOP, FLEXSPI_1PAD, 0x00),
                },
        },
    .pageSize = 256u,
    .sectorSize = 4u * 1024u,
    .blockSize = 256u * 1024u,
    .isUniformBlockSize = false,
};

