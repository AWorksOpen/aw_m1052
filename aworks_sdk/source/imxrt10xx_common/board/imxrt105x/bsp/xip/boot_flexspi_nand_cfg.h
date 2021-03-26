#ifndef __BOOT_FLEXSPI_NOR_CFG_H
#define __BOOT_FLEXSPI_NOR_CFG_H


#include <stdint.h>
#include <stdbool.h>

//#pragma anon_unions

/*  flexspi的配置信息 */

/* 结构体按1字节对齐 */

#pragma pack(1)

struct  flexspi_memcfg_block {
    uint32_t Tag;                       /** \brief 0x000:0x42464346, ascii:” FCFB” */
    uint32_t Version;                   /** \brief 0x004:0x56010000 / 0x56010100 "V1.00/1.10"*/
    uint32_t Reserved0;                 /** \brief 0x008:Reserved */
    uint8_t  readSampleClkSrc;          /** \brief 0x00C:0 – internal loopback
                                                         1 – loopback from DQS pad
                                                         3 – Flash provided DQS */
    uint8_t  dataHoldTime;              /** \brief 0x00D:Serial Flash CS Hold Time Recommend default value is 0x03 */
    uint8_t  dataSetupTime;             /** \brief 0x00E:Serial Flash CS setup time Recommended default value is 0x03 */
    uint8_t  columnAdressWidth;         /** \brief 0x00F:3 – For HyperFlash
                                                         12/13 – For Serial NAND, see datasheet to find correct value
                                                         0 – Other devices */
    uint8_t  deviceModeCfgEnable;       /** \brief 0x010:Device Mode Configuration Enable feature
                                                         0 – Disabled
                                                         1 – Enabled */
    uint8_t  Reserved1;                 /** \brief 0x011:Reserved */
    uint16_t waitTimeCfgCommands;       /** \brief 0x013:Wait time for all configuration commands, unit 100us. */
    uint32_t deviceModeSeq;             /** \brief 0x014:Sequence parameter for device mode configuration */
    uint32_t deviceModeArg;             /** \brief 0x018:Device Mode argument, effective only when deviceModeCfgEnable = 1 */
    uint8_t  configCmdEnable;           /** \brief 0x01C:Config Command Enable feature
                                                         0 – Disabled
                                                         1 – Enabled */
    uint8_t  Reserved2[3];              /** \brief 0x01D:Reserved */
    uint32_t configCmdSeqs[4];          /** \brief 0x020:Sequences for Config Command, allow 4 separate configuration command sequences. */
    uint32_t cfgCmdArgs[4];             /** \brief 0x030:Arguments for each separate configuration command sequence. */
    uint32_t controllerMiscOption;      /** \brief 0x040:Bit0 – differential clock enable
                                                         Bit1 – CK2 enable, must set to 0 in this silicon
                                                         Bit2 – ParallelModeEnable, must set to 0 for this silicon
                                                         Bit3 – wordAddressableEnable
                                                         Bit4 – Safe Configuration Frequency
                                                                enable set to 1 for the devices that support DDR
                                                                Read instructions
                                                         Bit5 – Pad Setting Override Enable
                                                         Bit6 – DDR Mode Enable,
                                                                set to 1 for device supports DDR read command */
    uint8_t  deviceType;                /** \brief 0x044: 1 – Serial NOR 2 – Serial NAND */
    uint8_t  sflashPadType;             /** \brief 0x045: 1 – Single pad
                                                          2 – Dual pads
                                                          4 – Quad pads
                                                          8 – Octal pads */
    uint8_t  serialClkFreq;             /** \brief 0x046:Chip specific value, for this silicon
                                                          1 – 30 MHz
                                                          2 – 50 MHz
                                                          3 – 60 MHz
                                                          4 – 75 MHz
                                                          5 – 80 MHz
                                                          6 – 100 MHz
                                                          7 – 133 MHz
                                                          8 – 166 MHz
                                                          Other value: 30 MHz */
    uint8_t  lutCustomSeqEnable;        /** \brief 0x047: 0 – Use pre-defined LUT sequence index and number
                                                          1 - Use LUT sequence parameters provided in this block */
    uint8_t  Reserved3[8];              /** \brief 0x048:Reserved */
    uint32_t sflashA1Size;              /** \brief 0x050:For SPI NOR,
                                                         need to fill with actual size For SPI NAND,
                                                         need to fill with actual size * 2 */
    uint32_t sflashA2Size;              /** \brief 0x054:The same as above */
    uint32_t sflashB1Size;              /** \brief 0x058:The same as above */
    uint32_t sflashB2Size;              /** \brief 0x05C:The same as above */
    uint32_t csPadSettingOverride;      /** \brief 0x060:Set to 0 if it is not supported */
    uint32_t sclkPadSettingOverride;    /** \brief 0x064:Set to 0 if it is not supported */
    uint32_t dataPadSettingOverride;    /** \brief 0x068:Set to 0 if it is not supported */
    uint32_t dqsPadSettingOverride;     /** \brief 0x06C:Set to 0 if it is not supported */
    uint32_t timeoutInMs;               /** \brief 0x070:Maximum wait time during read busy status
                                                         0 – Disabled timeout checking feature Other value – Timeout
                                                         if the wait time exceeds this value. */
    uint32_t commandInterval;           /** \brief 0x074:Unit: ns Currently, it is used for SPI NAND only at high frequency */
    uint32_t dataValidTime;             /** \brief 0x078:Time from clock edge to data valid edge, unit ns. This field
                                                         is used when the FlexSPI Root clock is less than 100 MHz
                                                         and the read sample clock source is device
                                                         provided DQS signal without CK2 support.
                                                         [31:16] data valid time for DLLB in terms of 0.1 ns
                                                         [15:0] data valid time for DLLA in terms of 0.1 ns */
    uint16_t busyOffset;                /** \brief 0x07C:busy bit offset, valid range : 0-31 */
    uint16_t busyBitPolarity;           /** \brief 0x07E:0 – busy bit is 1 if device is busy
                                                         1 – busy bit is 0 if device is busy */

    /* Each LUT sequence consists of up to 8 instructions = 8 * uint16_t = 4 * uint32_t */
                                        /** \brief 0x080:Lookup table [15:10] : OPCODE
                                                                      [ 9: 8] : NUM_PADS
                                                                      [ 7: 0] : OPERAND */
    uint32_t ReadFromCache_LUT_INDEX_0[4];
    uint32_t ReadStatus_LUT_INDEX_1[4];
    uint32_t Reserved_LUT_INDEX_2[4];
    uint32_t WriteEnable_LUT_INDEX_3[4];
    uint32_t ReadFromCacheOdd_LUT_INDEX_4[4];
    uint32_t BlockErase_LUT_INDEX_5[4];
    uint32_t Reserved_LUT_INDEX_6[4];
    uint32_t Reserved_LUT_INDEX_7[4];
    uint32_t Reserved_LUT_INDEX_8[4];
    uint32_t ProgramLoad_LUT_INDEX_9[4];
    uint32_t ProgramLoadOdd_LUT_INDEX_10[4];
    uint32_t ReadPage_LUT_INDEX_11[4];
    uint32_t Reserved_LUT_INDEX_12[4];
    uint32_t ReadEccStatus_LUT_INDEX_13[4];
    uint32_t ProgramExecute_LUT_INDEX_14[4];
    uint32_t Reserved_LUT_INDEX_15[4];

    uint32_t lutCustomSeq[12];

    uint8_t  Reserved4[16];       	/** \brief 0x1B0:Reserved for future use */
} ;

/*  flexspi nand flash 的配置表信息 */
struct  serial_nand_config_block {
    struct  flexspi_memcfg_block memCfg;  /** \brief FlexSPI Configuration block */
    uint32_t pageDataSize;          /** \brief 0x1C0:Page size in terms of bytes,usually, it is 2048 or 4096 */
    uint32_t pageTotalSize; 		/** \brief 0x1C4:It equals to 2 ^ width of column adddress  */
	uint32_t pagesPerBlock;			/** \brief 0x1C8:Pages in one block */
	uint8_t  bypassReadStatus;		/** \brief 0x1CC:0 – Read Status Register
                                                     1 – Bypass Read status register */
	uint8_t  bypassEccRead;			/** \brief 0x1CD:0 – Perform ECC read
                                                     1 – Bypass ECC read */
	uint8_t  hasMultiPlanes;		/** \brief 0x1CE:0 – Only 1 plane
	                                                 1 – Has two planes */
	uint8_t  skippOddBlocks;		/** \brief 0x1CF:0 – Read Odd blocks
                                                     1 – Skip Odd blocks */
	uint8_t  eccCheckCustomEnable;	/** \brief 0x1D0:0 – Use the common ECC check command and ECC related masks
                                                     1 - Use ECC check related masks provided in this configuration block */
    uint8_t  ipCmdSerialClkFreq; 	/** \brief 0x1D1:Chip specific value, not used by ROM
                                                     0 – No change, keep current serial clock unchanged
                                                     1 – 30 MHz
                                                     2 – 50 MHz
                                                     3 – 60 MHz
                                                     4 – 75 MHz
                                                     5 – 80 MHz
                                                     6 – 100 MHz
                                                     7 – 133 MHz
                                                     8 – 166 MHz */
    uint16_t readPageTimeUs;		/** \brief 0x1D2:Wait time during page read,this field will take effect on if
                                                     the bypassReadStatus is set to 1.NOTE: Only applicable to ROM. */
	uint32_t eccStatusMask;			/** \brief 0x1D4:ECC Status Mask */
	uint32_t eccFailureMask;		/** \brief 0x1D8:ECC Check Failure mask */
	uint32_t blocksPerDevice;		/** \brief 0x1DC:Blocks in a Serial NAND */
	uint8_t  Reserved0[32];      	/** \brief 0x1CC:Reserved */
};


/**
 * Flash Control Block  FCB
 */
struct flash_control_block {
    uint32_t    crcChecksum;                /** \brief 0x000:Checksum.The “crcChecksum” calculation starts from
                                                             fingerprint to the end of FCB, 1020 bytes in total*/
    uint32_t    fingerprint;                /** \brief 0x004:0x4E46_4342,ASCII: “NFCB” */
    uint32_t    version;                    /** \brief 0x008:0x0000_0001 */
    uint32_t    DBBTSearchStartPage;        /** \brief 0x00C:Start Page address for bad block table search area.
                                                             If DBBT Search Area is 0 in FCB, then ROM assumes
                                                             that there are no bad blocks on NAND device boot area. */
    uint16_t    searchStride;               /** \brief 0x010:Search stride for DBBT and FCB search.
                                                             Not used by ROM Max value is 8.*/
    uint16_t    searchCount;                /** \brief 0x012:Copies of DBBT and FCB. Not used by ROM, max value is 8.*/
    uint32_t    firmwareCopies;             /** \brief 0x014:Firmware copies Valid range 1-8. */
    uint8_t     Reserved0[40];              /** \brief 0x018:Reserved for future use Must be set to 0. */
    /*firmwareInfo_Table,占用64个字节,This table consists of (up to 8 entries)*/
    uint32_t    firmwareInfo_Start_Page;    /** \brief 0x040:Start page of this firmware.
                                                             NOTE: The StartPage must be the first page of a NAND block.*/
    uint32_t    firmwareInfo_Page_Count;    /** \brief 0x044:Pages in this firmware */
    uint32_t    Reserved1[14];              /** \brief 0x048:Remain entries must be set to 0 */
    uint8_t     Reserved2[128];             /** \brief 0x080:Reserved Must be set to 0 */
    struct  serial_nand_config_block  nandflash_config;   /** \brief 0x100:Serial NAND configuration block over FlexSPI */
    uint8_t     Reserved3[256];             /** \brief 0x300:Must be set to 0 */

};


/**
 * Discovered Bad Blocks Table  DBBT
 */
struct discovered_bad_block_table {
    uint32_t    crcChecksum;                /** \brief 0x000:Checksum.The "crcChecksum" is calculated with the same
                                                             algorithm as the one in FCB, from Fingerprint to the
                                                             end of DBBT, 1052 bytes in total. */
    uint32_t    Fingerprint;                /** \brief 0x004:32-bit word with a value of 0x4442_4254, in asci “DBBT” */
    uint32_t    Version;                    /** \brief 0x008:32-bit version number, this version of DBBT is 0x00000001 */
    uint32_t    Reserver0;                  /** \brief 0x00C:Reserved */
    uint32_t    badBlockNumber;             /** \brief 0x010:Number of bad blocks */
    uint8_t     Reserved1[12];              /** \brief 0x014:Must be filled with 0x00s */
    /*Bad Block Entries, occupy 1024 bytes*/
    uint32_t    BadBlockEntries[256];       /** \brief 0x020-0x41F:Bad Block entries, only the first “badBlockNumber”
                                                                   entries are valid, the
                                                                   remaining entries must be
                                                                   filled with 0x00s */
};



#pragma pack()

#endif    /* __BOOT_FLEXSPI_NOR_CFG_H */
