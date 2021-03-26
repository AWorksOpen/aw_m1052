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
 * \brief NandBus source file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-12-18  smc, first implementation
 * \endinternal
 */

/*******************************************************************************
    include
*******************************************************************************/
#include "aworks.h"
#include "aw_delay.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_clk.h"
#include "driver/busctlr/awbl_imx1050_semc.h"
#include "driver/nand/awbl_imx1050_nand.h"
#include "string.h"
#include "driver/busctlr/nand_common.h"
#include "awbus_lite.h"
#include "imx1050_pin.h"
#include "aw_gpio.h"

/*******************************************************************************
                      register
*******************************************************************************/
/** SEMC - Register Layout Typedef */
typedef struct __imx1050_semc_regs{
    volatile uint32_t MCR;              /**< Module Control Register, offset: 0x0 */
    volatile uint32_t IOCR;             /**< IO Mux Control Register, offset: 0x4 */
    volatile uint32_t BMCR0;            /**< Master Bus (AXI) Control Register 0, offset: 0x8 */
    volatile uint32_t BMCR1;            /**< Master Bus (AXI) Control Register 1, offset: 0xC */
    volatile uint32_t BR[9];            /**< Base Register 0 (For SDRAM CS0 device)..Base Register 8 (For NAND device), array offset: 0x10, array step: 0x4 */
             uint8_t  RESERVED_0[4];
    volatile uint32_t INTEN;            /**< Interrupt Enable Register, offset: 0x38 */
    volatile uint32_t INTR;             /**< Interrupt Enable Register, offset: 0x3C */
    volatile uint32_t SDRAMCR0;         /**< SDRAM control register 0, offset: 0x40 */
    volatile uint32_t SDRAMCR1;         /**< SDRAM control register 1, offset: 0x44 */
    volatile uint32_t SDRAMCR2;         /**< SDRAM control register 2, offset: 0x48 */
    volatile uint32_t SDRAMCR3;         /**< SDRAM control register 3, offset: 0x4C */
    volatile uint32_t NANDCR0;          /**< NAND control register 0, offset: 0x50 */
    volatile uint32_t NANDCR1;          /**< NAND control register 1, offset: 0x54 */
    volatile uint32_t NANDCR2;          /**< NAND control register 2, offset: 0x58 */
    volatile uint32_t NANDCR3;          /**< NAND control register 3, offset: 0x5C */
    volatile uint32_t NORCR0;           /**< NOR control register 0, offset: 0x60 */
    volatile uint32_t NORCR1;           /**< NOR control register 1, offset: 0x64 */
    volatile uint32_t NORCR2;           /**< NOR control register 2, offset: 0x68 */
             uint32_t NORCR3;               /**< NOR control register 3, offset: 0x6C */
    volatile uint32_t SRAMCR0;          /**< SRAM control register 0, offset: 0x70 */
    volatile uint32_t SRAMCR1;          /**< SRAM control register 1, offset: 0x74 */
    volatile uint32_t SRAMCR2;          /**< SRAM control register 2, offset: 0x78 */
             uint32_t SRAMCR3;              /**< SRAM control register 3, offset: 0x7C */
    volatile uint32_t DBICR0;           /**< DBI-B control register 0, offset: 0x80 */
    volatile uint32_t DBICR1;           /**< DBI-B control register 1, offset: 0x84 */
             uint8_t  RESERVED_1[8];
    volatile uint32_t IPCR0;            /**< IP Command control register 0, offset: 0x90 */
    volatile uint32_t IPCR1;            /**< IP Command control register 1, offset: 0x94 */
    volatile uint32_t IPCR2;            /**< IP Command control register 2, offset: 0x98 */
    volatile uint32_t IPCMD;            /**< IP Command register, offset: 0x9C */
    volatile uint32_t IPTXDAT;          /**< TX DATA register (for IP Command), offset: 0xA0 */
             uint8_t  RESERVED_2[12];
    volatile const  uint32_t IPRXDAT;   /**< RX DATA register (for IP Command), offset: 0xB0 */
             uint8_t  RESERVED_3[12];
    volatile const  uint32_t STS0;      /**< Status register 0, offset: 0xC0 */
             uint32_t STS1;                 /**< Status register 1, offset: 0xC4 */
    volatile const  uint32_t STS2;                 /**< Status register 2, offset: 0xC8 */
             uint32_t STS3;                 /**< Status register 3, offset: 0xCC */
             uint32_t STS4;                 /**< Status register 4, offset: 0xD0 */
             uint32_t STS5;                 /**< Status register 5, offset: 0xD4 */
             uint32_t STS6;                 /**< Status register 6, offset: 0xD8 */
             uint32_t STS7;                 /**< Status register 7, offset: 0xDC */
             uint32_t STS8;                 /**< Status register 8, offset: 0xE0 */
             uint32_t STS9;                 /**< Status register 9, offset: 0xE4 */
             uint32_t STS10;                /**< Status register 10, offset: 0xE8 */
             uint32_t STS11;                /**< Status register 11, offset: 0xEC */
    volatile const  uint32_t STS12;     /**< Status register 12, offset: 0xF0 */
             uint32_t STS13;                /**< Status register 13, offset: 0xF4 */
             uint32_t STS14;                /**< Status register 14, offset: 0xF8 */
             uint32_t STS15;                /**< Status register 15, offset: 0xFC */
} __imx1050_semc_regs_t;

/*******************************************************************************
  local defines
*******************************************************************************/

/*! @name IPCR2 - IP Command control register 2 */
#define __SEMC_IPCR2_BM0_MASK                 (0x1U)
#define __SEMC_IPCR2_BM0_SHIFT                (0U)
#define __SEMC_IPCR2_BM0(x)                   (((uint32_t)(((uint32_t)(x)) << \
                        __SEMC_IPCR2_BM0_SHIFT)) & __SEMC_IPCR2_BM0_MASK)
#define __SEMC_IPCR2_BM1_MASK                 (0x2U)
#define __SEMC_IPCR2_BM1_SHIFT                (1U)
#define __SEMC_IPCR2_BM1(x)                   (((uint32_t)(((uint32_t)(x)) << \
                        __SEMC_IPCR2_BM1_SHIFT)) & __SEMC_IPCR2_BM1_MASK)
#define __SEMC_IPCR2_BM2_MASK                 (0x4U)
#define __SEMC_IPCR2_BM2_SHIFT                (2U)
#define __SEMC_IPCR2_BM2(x)                   (((uint32_t)(((uint32_t)(x)) << \
                       __SEMC_IPCR2_BM2_SHIFT)) & __SEMC_IPCR2_BM2_MASK)
#define __SEMC_IPCR2_BM3_MASK                 (0x8U)
#define __SEMC_IPCR2_BM3_SHIFT                (3U)
#define __SEMC_IPCR2_BM3(x)                   (((uint32_t)(((uint32_t)(x)) << \
                       __SEMC_IPCR2_BM3_SHIFT)) & __SEMC_IPCR2_BM3_MASK)

/*! @nam__e IPCR1 - IP Command control register 1 */
#define __SEMC_IPCR1_DATSZ_MASK                    (0x7U)
#define __SEMC_IPCR1_DATSZ_SHIFT                   (0U)
#define __SEMC_IPCR1_DATSZ(x)                      (((uint32_t)(((uint32_t)(x)) << __SEMC_IPCR1_DATSZ_SHIFT)) & __SEMC_IPCR1_DATSZ_MASK)


#define CONFIG_GPMI_MAX_DMA_DESC_COUNT      16
#define CONFIG_GPMI_DMA_BUF_SIZE            5120
#define CONGIG_GPMI_CMD_BUF_SIZE            512
#define BUG_HERE                            while(1){}


#define __NAND_RALOW   0    /*!< Adv active low. */
#define __NAND_RAHIGH  1    /*!< Adv active low. */

/*! @brief Define macros for SEMC driver. */
#define __SEMC_IPCOMMANDDATASIZEBYTEMAX (4U)
#define __SEMC_IPCOMMANDMAGICKEY (0xA55A)
#define __SEMC_IOCR_PINMUXBITWIDTH (0x3U)
#define __SEMC_IOCR_NAND_CE (4U)
#define __SEMC_IOCR_NOR_CE (5U)
#define __SEMC_IOCR_NOR_CE_A8 (2U)
#define __SEMC_IOCR_PSRAM_CE (6U)
#define __SEMC_IOCR_PSRAM_CE_A8 (3U)
#define __SEMC_IOCR_DBI_CSX (7U)
#define __SEMC_IOCR_DBI_CSX_A8 (4U)
#define __SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE (24U)
#define __SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHMAX (28U)
#define __SEMC_BMCR0_TYPICAL_WQOS (5U)
#define __SEMC_BMCR0_TYPICAL_WAGE (8U)
#define __SEMC_BMCR0_TYPICAL_WSH (0x40U)
#define __SEMC_BMCR0_TYPICAL_WRWS (0x10U)
#define __SEMC_BMCR1_TYPICAL_WQOS (5U)
#define __SEMC_BMCR1_TYPICAL_WAGE (8U)
#define __SEMC_BMCR1_TYPICAL_WPH (0x60U)
#define __SEMC_BMCR1_TYPICAL_WBR (0x40U)
#define __SEMC_BMCR1_TYPICAL_WRWS (0x24U)
#define __SEMC_STARTADDRESS (0x80000000U)
#define __SEMC_ENDADDRESS (0xDFFFFFFFU)
#define __SEMC_BR_MEMSIZE_MIN (4)
#define __SEMC_BR_MEMSIZE_OFFSET (2)
#define __SEMC_BR_MEMSIZE_MAX (4 * 1024 * 1024)
#define __SEMC_SDRAM_MODESETCAL_OFFSET (4)
#define __SEMC_BR_REG_NUM (9)
#define __SEMC_BYTE_NUMBIT (4)

#define __SEMC_IPCMD_CMD_MASK                      (0xFFFFU)
#define __SEMC_IPCMD_CMD_SHIFT                     (0U)
#define __SEMC_IPCMD_CMD(x)                        (((uint32_t)(((uint32_t)(x)) << SEMC_IPCMD_CMD_SHIFT)) & SEMC_IPCMD_CMD_MASK)
#define __SEMC_IPCMD_KEY_MASK                      (0xFFFF0000U)
#define __SEMC_IPCMD_KEY_SHIFT                     (16U)
#define __SEMC_IPCMD_KEY(x)                        (((uint32_t)(((uint32_t)(x)) << SEMC_IPCMD_KEY_SHIFT)) & SEMC_IPCMD_KEY_MASK)

#define __SEMC_NANDCR0_PS_MASK                     (0x1U)

#define __SEMC_INTR_IPCMDDONE_MASK                 (0x1U)
#define __SEMC_INTR_IPCMDERR_MASK                  (0x2U)

/*
 * NAND 地址命令一起发
 */
#define __NAND_IS_CMDADDR_FLAG         0xFF
#define __NAND_NO_CMDADDR_FLAG         0x00
#define __NADN_CMDADDR_BUF_LEN         3
#define __NAND_CMDADDR_CMDBIT          0
#define __NAND_CMDADDR_FLAGBIT         1
#define __NAND_CMDADDR_RIDBIT          2


/*
 * NAND WP
 */
#define __SEMC_NAND_nWP_PIN         GPIO5_1
#define __SEMC_NAND_nWP_OPEN()      aw_gpio_set(__SEMC_NAND_nWP_PIN, 0)
#define __SEMC_NAND_nWP_CLOSE()     aw_gpio_set(__SEMC_NAND_nWP_PIN, 1)

/*
 * NAND 基地址
 */
#define __NAND_START_ADDR         0xA0000000

static volatile uint8_t __nand_cmdaddr_buf[__NADN_CMDADDR_BUF_LEN] = {0};

/* write register */
#define __SEMC_REG_WRITE(reg_addr, data) \
        AW_REG_WRITE32(reg_addr, (data))

/* read register */
#define __SEMC_REG_READ(reg_addr) \
        AW_REG_READ32(reg_addr)

/* bit is get */
#define __SEMC_REG_BIT_ISSET(reg_addr, bit) \
        AW_REG_BIT_ISSET32(reg_addr, bit)

/* set bit */
#define __SEMC_REG_BIT_SET(reg_addr, bit) \
        AW_REG_BIT_SET32(reg_addr, bit)

/* clear bit */
#define __SEMC_REG_BIT_CLR(reg_addr, bit) \
        AW_REG_BIT_CLR32(reg_addr, bit)

/* get register bits */
#define __SEMC_REG_BITS_GET(reg_addr, start, len) \
        AW_REG_BITS_GET32(reg_addr, start, len)

/* set register bits */
#define __SEMC_REG_BITS_SET(reg_addr, start, len, value) \
        AW_REG_BITS_SET32(reg_addr, start, len, value)

/*******************************************************************************
    macro operation
*******************************************************************************/
#define __IMX1050_SEMC_DEVINFO_DECL(p_info, p_awdev) \
        struct awbl_imx1050_semc_devinfo *p_info = \
            (struct awbl_imx1050_semc_devinfo *)AWBL_DEVINFO_GET(p_awdev)

#define __IMX1050_SEMC_DEV_DECL(p_dev, p_awdev) \
        struct awbl_imx1050_semc *p_dev = AW_CONTAINER_OF(p_awdev, struct awbl_imx1050_semc, nandbus)

/*******************************************************************************
    enums
*******************************************************************************/
/* \brief BCH : Status Block Completion Codes */
enum BCH_COMPLETION_STATUS {
    BCH_COMPLETION_STATUS_GOOD = 0x00,
    BCH_COMPLETION_STATUS_ERASED = 0xFF,
    BCH_COMPLETION_STATUS_UNCORRECTABLE = 0xFE,
};

/**
 * enum dma_ctrl_flags - DMA flags to augment operation preparation,
 *  control completion, and communicate status.
 * @DMA_PREP_INTERRUPT - trigger an interrupt (callback) upon completion of
 *  this transaction
 * @DMA_CTRL_ACK - if clear, the descriptor cannot be reused until the client
 *  acknowledges receipt, i.e. has has a chance to establish any dependency
 *  chains
 * @DMA_PREP_PQ_DISABLE_P - prevent generation of P while generating Q
 * @DMA_PREP_PQ_DISABLE_Q - prevent generation of Q while generating P
 * @DMA_PREP_CONTINUE - indicate to a driver that it is reusing buffers as
 *  sources that were the result of a previous operation, in the case of a PQ
 *  operation it continues the calculation with new sources
 * @DMA_PREP_FENCE - tell the driver that subsequent operations depend
 *  on the result of this operation
 */
enum dma_ctrl_flags {
    DMA_PREP_INTERRUPT = (1 << 0),
    DMA_CTRL_ACK = (1 << 1),
    DMA_PREP_PQ_DISABLE_P = (1 << 2),
    DMA_PREP_PQ_DISABLE_Q = (1 << 3),
    DMA_PREP_CONTINUE = (1 << 4),
    DMA_PREP_FENCE = (1 << 5),
};


typedef enum __semc_ipcmd_nand_cmdmode {
    kSEMC_NANDCM_AXICmdAddrRead = 0x0U, /*!< For AXI read. */
    kSEMC_NANDCM_AXICmdAddrWrite,       /*!< For AXI write.  */
    kSEMC_NANDCM_Command,               /*!< command. */
    kSEMC_NANDCM_CommandHold,           /*!< Command hold. */
    kSEMC_NANDCM_CommandAddress,        /*!< Command address. */
    kSEMC_NANDCM_CommandAddressHold,    /*!< Command address hold.  */
    kSEMC_NANDCM_CommandAddressRead,    /*!< Command address read.  */
    kSEMC_NANDCM_CommandAddressWrite,   /*!< Command address write.  */
    kSEMC_NANDCM_CommandRead,           /*!< Command read.  */
    kSEMC_NANDCM_CommandWrite,          /*!< Command write.  */
    kSEMC_NANDCM_Read,                  /*!< Read.  */
    kSEMC_NANDCM_Write                  /*!< Write.  */
} __semc_ipcmd_nand_cmdmode_t;

/*! @brief SEMC IP command for NAND: address mode. */
typedef enum __semc_ipcmd_nand_addrmode {
    kSEMC_NANDAM_ColumnRow = 0x0U, /*!< Address mode: column and row address(5Byte-CA0/CA1/RA0/RA1/RA2). */
    kSEMC_NANDAM_ColumnCA0,        /*!< Address mode: column address only(1 Byte-CA0).  */
    kSEMC_NANDAM_ColumnCA0CA1,     /*!< Address mode: column address only(2 Byte-CA0/CA1). */
    kSEMC_NANDAM_RawRA0,           /*!< Address mode: row address only(1 Byte-RA0). */
    kSEMC_NANDAM_RawRA0RA1,        /*!< Address mode: row address only(2 Byte-RA0/RA1). */
    kSEMC_NANDAM_RawRA0RA1RA2      /*!< Address mode: row address only(3 Byte-RA0).  */
} __semc_ipcmd_nand_addrmode_t;



/**
 * enum dma_transfer_direction - dma transfer mode and direction indicator
 * @DMA_MEM_TO_MEM: Async/Memcpy mode
 * @DMA_MEM_TO_DEV: Slave mode & From Memory to Device
 * @DMA_DEV_TO_MEM: Slave mode & From Device to Memory
 * @DMA_DEV_TO_DEV: Slave mode & From Device to Device
 */
enum dma_transfer_direction {
    DMA_MEM_TO_MEM,
    DMA_MEM_TO_DEV,
    DMA_DEV_TO_MEM,
    DMA_DEV_TO_DEV,
    DMA_TRANS_NONE,
};

/*
 * Constants for hardware specific CLE/ALE/NCE function
 *
 * These are bits which can be or'ed to set/clear multiple
 * bits in one go.
 */
enum semc_cmd_ctrl_flag {
    NAND_NCE = 0x01,            /* Select the chip by setting nCE to low */
    NAND_CLE = 0x02,            /* Select the command latch by setting CLE to high */
    NAND_ALE = 0x04,            /* Select the address latch by setting ALE to high */
    NAND_CTRL_CLE = (NAND_NCE | NAND_CLE),
    NAND_CTRL_ALE = (NAND_NCE | NAND_ALE),
    NAND_CMD_NONE = -1,
};

/*******************************************************************************
    struct
*******************************************************************************/
struct scatterlist {
    unsigned int    length;
    uint32_t        dma_address;
};

/*******************************************************************************
    function
*******************************************************************************/

/*
 * SEMC reset
 */
aw_local void imx1050_semc_reset (struct awbl_nandbus *p_bus);

/*
 * config command
 */
aw_local aw_err_t __semc_config_ipcommand(__imx1050_semc_regs_t *p_reg, uint8_t size_bytes);

aw_local aw_err_t __semc_send_ipcommand (__imx1050_semc_regs_t *base, uint32_t address, uint16_t command, uint32_t write, uint32_t *read);

aw_local aw_err_t __semc_ipcommand_done (__imx1050_semc_regs_t *p_reg);

aw_local aw_err_t __semc_nand_write (__imx1050_semc_regs_t *base, uint32_t address, uint8_t *data, uint32_t size_bytes);

aw_local aw_err_t __semc_nand_read (__imx1050_semc_regs_t *base, uint32_t address, uint8_t *data, uint32_t size_bytes);

static uint16_t __semc_build_ipcommand (uint8_t userCommand,
                                        __semc_ipcmd_nand_addrmode_t addrMode,
                                        __semc_ipcmd_nand_cmdmode_t cmdMode)
{
    return (uint16_t)((uint16_t)userCommand << 8) | (uint16_t)(addrMode << 4) | ((uint8_t)cmdMode & 0x0Fu);
}

aw_local aw_err_t __imx1050_semc_set_timing (struct awbl_imx1050_semc    *p_dev,
                                             struct awbl_nand_timing     *p_tim)
{
    __IMX1050_SEMC_DEVINFO_DECL(p_info, &(p_dev->nandbus.awbus.super));
    __imx1050_semc_regs_t  *p_semc_regs = p_dev->p_semc_regs;

    uint8_t  tces, tceh, twel, tweh, trel, treh, ta , ceitv;
    uint8_t  twh, trhw, tadl, trr, twb;
    uint32_t clk_rate;

    /* configure timing parameter。 */
    clk_rate = aw_clk_rate_get(p_info->clk_io);

    /*
     * nandcr1
     * tces   [0,3]        10
     * tceh   [4,7]        10
     * twel   [8,11]       12
     * tweh   [12,15]      10
     * trel   [16,19]      15
     * treh   [20,23]      10
     * ta     [24,27]      25
     * ceitv  [28,31]      10
     */
    tces  = (((uint64_t)clk_rate * (10)) / 1000000000) -
            ((((uint64_t)clk_rate * (10)) % 1000000000)? 0:1);
    tceh  = (((uint64_t)clk_rate * (10)) / 1000000000) -
            ((((uint64_t)clk_rate * (10)) % 1000000000)? 0:1);
    twel  = (((uint64_t)clk_rate * (12)) / 1000000000) -
            ((((uint64_t)clk_rate * (12)) % 1000000000)? 0:1);
    tweh  = (((uint64_t)clk_rate * (10)) / 1000000000) -
            ((((uint64_t)clk_rate * (10)) % 1000000000)? 0:1);
    trel  = (((uint64_t)clk_rate * (15)) / 1000000000) -
            ((((uint64_t)clk_rate * (15)) % 1000000000)? 0:1);
    treh  = (((uint64_t)clk_rate * (10)) / 1000000000) -
            ((((uint64_t)clk_rate * (10)) % 1000000000)? 0:1);
    ta    = (((uint64_t)clk_rate * (25)) / 1000000000) -
            ((((uint64_t)clk_rate * (25)) % 1000000000)? 0:1);
    ceitv = (((uint64_t)clk_rate * (10)) / 1000000000) -
            ((((uint64_t)clk_rate * (10)) % 1000000000)? 0:1);

    __SEMC_REG_WRITE(&p_semc_regs->NANDCR1,
            ((tces<<0)|(tceh<<4)|(twel<<8)|(tweh<<12)|(trel<<16)|(treh<<20)|(ta<<24)|(ceitv<<28)));

    /*
     * nandcr1
     * twhr   [0,5]         10
     * trhw   [6,11]        20
     * tadl   [12,17]       25
     * trr    [18,23]       15
     * twb    [24,29]       15
     */
    twh  = (((uint64_t)clk_rate * (10)) / 1000000000) -
           ((((uint64_t)clk_rate * (10)) % 1000000000)? 0:1);
    trhw = (((uint64_t)clk_rate * (20)) / 1000000000) -
            ((((uint64_t)clk_rate * (20)) % 1000000000)? 0:1);
    tadl = (((uint64_t)clk_rate * (25)) / 1000000000) -
            ((((uint64_t)clk_rate * (25)) % 1000000000)? 0:1);
    trr  = (((uint64_t)clk_rate * (15)) / 1000000000) -
            ((((uint64_t)clk_rate * (15)) % 1000000000)? 0:1);
    twb  = (((uint64_t)clk_rate * (15)) / 1000000000) -
            ((((uint64_t)clk_rate * (15)) % 1000000000)? 0:1);

    __SEMC_REG_WRITE(&p_semc_regs->NANDCR2,
            ((twh<<0)|(trhw<6)|(tadl<<12)|(trr<<18)|(twb<<24)));

    return AW_OK;
}


/******************************************************************************/

aw_local aw_err_t __imx1050_semc_hw_init (struct awbl_imx1050_semc *p_dev)
{
    imx1050_semc_reset(&(p_dev->nandbus));

    return AW_OK;
}

aw_local void imx1050_semc_reset (struct awbl_nandbus *p_bus)
{
    __IMX1050_SEMC_DEV_DECL(p_dev, &(p_bus->awbus.super));
    struct __imx1050_semc_regs *p_semc_regs = p_dev->p_semc_regs;
    uint32_t tmpreg;

    /* Adv active low. */
    if ((p_semc_regs->MCR & 4) == 1) {
        __SEMC_REG_BIT_CLR(&p_semc_regs->MCR, 2);
    }

    /*
     * nandcr0
     * Port Size bit                [0]        0:8, 1:16
     * Burst Length                 [4,6]      0:1
     * EDO module                   [7]        0:dis
     * Column address bit number    [8,10]     100b:12
     */
    __SEMC_REG_WRITE(&p_semc_regs->NANDCR0, ((4<<8)|(1<<5)|(1<<7)));

    /*
     * nandcr1
     * tces   [0,3]        10
     * tceh   [4,7]        10
     * twel   [8,11]       12
     * tweh   [12,15]      10
     * trel   [16,19]      15
     * treh   [20,23]      10
     * ta     [24,27]      25
     * ceitv  [28,31]      10
     */
    __SEMC_REG_WRITE(&p_semc_regs->NANDCR1,
            ((1<<0)|(0x1<<4)|(2<<8)|(1<<12)|(2<<16)|(1<<20)|(4<<24)|(1<<28)));
    /*
     * nandcr1
     * twhr   [0,5]         10
     * trhw   [6,11]        10
     * tadl   [12,17]       25
     * trr    [18,23]       15
     * twb    [24,29]       15
     */
    __SEMC_REG_WRITE(&p_semc_regs->NANDCR2,
            ((3<<0)|(3<<6)|(3<<12)|(2<<18)|(2<<24)));

    /* Address option, */
    __SEMC_REG_WRITE(&p_semc_regs->NANDCR3, 0);

    __SEMC_REG_WRITE(&p_semc_regs->BR[8],  (0x23| (0xA0000000 & 0xFFFFF000)));
    p_semc_regs->BR[4] = 0x23| (0xA0000000 & 0xFFFFF000);

    /* Set NAND CS select csx0 */
    tmpreg  = __SEMC_REG_READ(&p_semc_regs->IOCR);
    tmpreg &= ~(7ul<<3);
    tmpreg |= (4<<3);
    __SEMC_REG_WRITE(&p_semc_regs->IOCR, tmpreg);

    /* Enable SEMC module */
    if ((p_semc_regs->MCR & 2) == 1) {
        __SEMC_REG_BIT_CLR(&p_semc_regs->MCR, 1);
    }

}

aw_local void __semc_idle_satus(__imx1050_semc_regs_t *p_reg)
{
    uint32_t tmpr;
    do  {
        tmpr = __SEMC_REG_READ(&p_reg->STS0);
    }while (!(tmpr & (1ul<<0)));
}

aw_local aw_err_t __semc_nand_axi_read (__imx1050_semc_regs_t *p_reg, uint32_t address, uint8_t *data, uint32_t size_bytes)
{

    uint32_t addr = address;
    uint16_t ip_cmd;
    uint32_t *tbuf;
    uint32_t tmp_data;
    uint8_t *p_tmp;

    if (address % 4) {
        addr -= (address % 4);
        tmp_data = *((volatile uint32_t *)addr);
        addr += 4;

        for (int i = 0; i < (4 - address % 4); ++i) {
            *data++ = tmp_data >> 8*(address % 4 + i) & 0xFF;
        }

        size_bytes -= (4 - (address%4));

    }
    tbuf = (uint32_t *)data;

    while (size_bytes >= 4) {
        *tbuf++ = *((volatile uint32_t *)addr);
        addr += 4;
        size_bytes -= 4;
    }

    if (size_bytes) {
        p_tmp =  (uint8_t *)(tbuf);
        p_tmp +=  (size_bytes-1);

        tmp_data = *((volatile uint32_t *)addr);
        while (size_bytes) {
            *p_tmp = (tmp_data >> (8 * (size_bytes - 1))) & 0xFFU;
            p_tmp--;
            size_bytes--;
        }
    }

    return AW_OK;
}

aw_local aw_err_t __semc_nand_axi_write (__imx1050_semc_regs_t *p_reg, uint32_t address, uint8_t *data, uint32_t size_bytes)
{

    uint32_t  addr = address;
    uint16_t  ip_cmd;
    uint32_t *tbuf = (uint32_t *)data;
    uint32_t  tmp_data = 0;
    uint32_t  tmp_size = size_bytes;

    /* write data */

    if (address % 4) {
        tmp_data = *((uint32_t *)data);
        tmp_data <<= (8 * (address % 4));
        tmp_data |= (0xFFFFFFFF >> (8 * (4 - (address % 4))));
        addr -= (address % 4);

        *((volatile uint32_t *)(addr)) = tmp_data;
        addr += 4;
        tmp_size -= (4 - (address % 4));
        tbuf = (uint32_t *)(data + 4 - (address % 4));
    }


    while (tmp_size >= __SEMC_IPCOMMANDDATASIZEBYTEMAX) {

        *((volatile uint32_t *)addr) = *tbuf++;
        addr += 4;
        tmp_size -= 4;
    }

    if (tmp_size) {

        tmp_size %= 4;
        tmp_data = *tbuf;

        tmp_data |= (uint32_t)(0xFFFFFFFF << (8*tmp_size));
        *((volatile uint32_t *)addr) = tmp_data;
    }

    return AW_OK;
}

aw_local aw_err_t imx1050_semc_opt (struct awbl_nandbus            *p_bus,
                                    struct awbl_nandbus_opt        *p_opt)
{
    __IMX1050_SEMC_DEV_DECL(p_dev, &(p_bus->awbus.super));
    __imx1050_semc_regs_t *p_semc_regs = p_dev->p_semc_regs;

    uint8_t    *ptr = (uint8_t *)p_opt->ptr;
    int         i;
    int         flag;
    int         ret = -AW_EINVAL;
    uint16_t ip_cmd;
    static uint32_t address = __NAND_START_ADDR;

    if (p_opt->type != NAND_ADDR) {
        __nand_cmdaddr_buf[__NAND_CMDADDR_FLAGBIT] = __NAND_NO_CMDADDR_FLAG;
    }

    /* operation */
    switch (p_opt->type) {

    case NAND_CMD:
        flag = 1;
        if (ptr[p_opt->len-1] == __NANDFLASH_CMD_READID) {
            __nand_cmdaddr_buf[__NAND_CMDADDR_RIDBIT] = __NANDFLASH_CMD_READID;
        } else {
            __nand_cmdaddr_buf[__NAND_CMDADDR_RIDBIT] = 0xFF;
        }

        /* close write protect */
        if ((ptr[p_opt->len-1] == __NANDFLASH_CMD_ERASE1) || \
            (ptr[p_opt->len-1] == __NANDFLASH_CMD_RNDIN) || \
            (ptr[p_opt->len-1] == __NANDFLASH_CMD_SEQIN)) {
            __SEMC_NAND_nWP_CLOSE();
        }

        for (i = 0; i < sizeof(__g_nand_flash_cmd_addr)/sizeof(__g_nand_flash_cmd_addr[0]);
                    i++) {
            if (ptr[p_opt->len-1] == __g_nand_flash_cmd_addr[i] ) {
                __nand_cmdaddr_buf[__NAND_CMDADDR_CMDBIT] = ptr[p_opt->len-1];
                __nand_cmdaddr_buf[__NAND_CMDADDR_FLAGBIT] = __NAND_IS_CMDADDR_FLAG;
                flag = 0;
                break;
            }
        }

        if (flag) {
            ip_cmd = __semc_build_ipcommand(ptr[p_opt->len-1] , kSEMC_NANDAM_ColumnRow, kSEMC_NANDCM_Command);
            __semc_idle_satus(p_semc_regs);
            __semc_send_ipcommand(p_semc_regs, __NAND_START_ADDR, ip_cmd, 0, NULL);
        }

        /* open write protect */
        if (ptr[p_opt->len-1] == __NANDFLASH_CMD_STATUS) {
            __SEMC_NAND_nWP_OPEN();
        }

        break;
    case NAND_ADDR:
        if (__nand_cmdaddr_buf[__NAND_CMDADDR_FLAGBIT] == __NAND_NO_CMDADDR_FLAG) {
            goto cleanup;
        }

        if (p_opt->len == 2) {
            address =      __NAND_START_ADDR     +
                        (((ptr[0]) + (ptr[1]<<8)) << 12);
            ip_cmd = __semc_build_ipcommand(__nand_cmdaddr_buf[__NAND_CMDADDR_CMDBIT] , kSEMC_NANDAM_RawRA0RA1, kSEMC_NANDCM_CommandAddress);
        }

        if (p_opt->len == 3) {
            address =      __NAND_START_ADDR     +
                        (((ptr[0]) + (ptr[1]<<8) + (ptr[2]<<16) ) << 12);
            ip_cmd = __semc_build_ipcommand(__nand_cmdaddr_buf[__NAND_CMDADDR_CMDBIT] , kSEMC_NANDAM_RawRA0RA1RA2, kSEMC_NANDCM_CommandAddress);
        }

        if (p_opt->len == 4) {
            address =      __NAND_START_ADDR     +
                          (ptr[0]                +
                         ((ptr[1] & 0xF)  << 8)  +
                        (((ptr[2]) + (ptr[3]<<8)) << 12));
        }

        if (p_opt->len == 5) {
            address =      __NAND_START_ADDR     +
                          (ptr[0]                +
                         ((ptr[1] & 0xF)  << 8)  +
                        (((ptr[2]) + (ptr[3]<<8) + (ptr[4]<<16) )<<12));
        }

        if ((p_opt->len == 2) || (p_opt->len == 3)) {

            __semc_idle_satus(p_semc_regs);

            ret = __semc_send_ipcommand(p_semc_regs, address, ip_cmd, 0, NULL);
            if (ret != AW_OK) {
                goto cleanup;
            }
            break;
        }

        if (__nand_cmdaddr_buf[__NAND_CMDADDR_CMDBIT] == __NANDFLASH_CMD_READID) {
            ip_cmd = __semc_build_ipcommand(__nand_cmdaddr_buf[__NAND_CMDADDR_CMDBIT] , kSEMC_NANDAM_RawRA0, kSEMC_NANDCM_CommandAddress);
            __semc_idle_satus(p_semc_regs);
            ret = __semc_send_ipcommand(p_semc_regs, address, ip_cmd, 0, NULL);
            if (ret != AW_OK) {
                goto cleanup;
            }
        } else {
            ip_cmd = __semc_build_ipcommand(__nand_cmdaddr_buf[__NAND_CMDADDR_CMDBIT] , kSEMC_NANDAM_ColumnRow, kSEMC_NANDCM_CommandAddress);
            __semc_idle_satus(p_semc_regs);

            ret = __semc_send_ipcommand(p_semc_regs, address, ip_cmd, 0, NULL);
            if (ret != AW_OK) {
                goto cleanup;
            }
        }
        break;
    case NAND_RD_DATA:
        if ((__nand_cmdaddr_buf[__NAND_CMDADDR_RIDBIT] != __NANDFLASH_CMD_READID) && (p_opt->len > 10)) {
            __semc_nand_axi_read(p_semc_regs, address, ptr, p_opt->len);
            address += p_opt->len;
        } else {
            __semc_nand_read(p_semc_regs, __NAND_START_ADDR, ptr, p_opt->len);
        }

        break;
    case NAND_WR_DATA:
//        __SEMC_NAND_nWP_CLOSE();
        __semc_nand_write(p_semc_regs, __NAND_START_ADDR, ptr, p_opt->len);
        __semc_nand_axi_write(p_semc_regs, address, ptr, p_opt->len);
        address += p_opt->len;
//        __SEMC_NAND_nWP_OPEN();
        break;
    default:
        ret = -AW_EINVAL;
        break;
    }
    ret = 0;
cleanup:
    return ret;
}

aw_local void imx1050_semc_select (struct awbl_nandbus         *p_bus,
                                   void                        *p_info)
{
    __IMX1050_SEMC_DEV_DECL(p_dev, &(p_bus->awbus.super));
    __imx1050_semc_regs_t *p_semc_regs = p_dev->p_semc_regs;
    uint32_t tmpreg;

    if (p_dev->p_nand_info == p_info) {
        return;
    }

    p_dev->p_nand_info = (awbl_nand_info_t *)p_info;

    /* Set NAND CS select csx0 */
    tmpreg  = __SEMC_REG_READ(&p_semc_regs->IOCR);
    if ((tmpreg & (~(7ul<<3))) != (4<<3)){
        tmpreg &= ~(7ul<<3);
        tmpreg |= (4<<3);
        __SEMC_REG_WRITE(&p_semc_regs->IOCR, tmpreg);
    }

    /* Address option, 0x08000000=128M */
    if (p_dev->p_nand_info->attr.chip_size > 0x08000000) {
        __SEMC_REG_WRITE(&p_semc_regs->NANDCR3, 0);
    } else {
        __SEMC_REG_WRITE(&p_semc_regs->NANDCR3, 2);
    }

    /* 设置时序  */
    __imx1050_semc_set_timing(p_dev,
            &(((struct awbl_imx1050_nand_info *)(p_dev->p_nand_info->p_platform_info))->tim));
}

aw_local aw_bool_t imx1050_semc_ready (struct awbl_nandbus *p_bus,
                                    void                *p_chip)
{
    volatile uint32_t tmp = 0;

    __IMX1050_SEMC_DEV_DECL(p_dev, &(p_bus->awbus.super));
    struct __imx1050_semc_regs *p_semc_regs = p_dev->p_semc_regs;

    tmp = __SEMC_REG_READ(&p_semc_regs->STS0);

    return (tmp & (0x1u<<1)) ? AW_TRUE : AW_FALSE;
}

aw_local void __imx1050_semc_isr (void *p_arg)
{

}

static aw_err_t __semc_config_ipcommand (__imx1050_semc_regs_t *p_reg, uint8_t size_bytes)
{
    volatile uint8_t size = size_bytes;

    p_reg->IPCR1 = __SEMC_IPCR1_DATSZ(size);

    if (size >= 4) {
        __SEMC_REG_WRITE(&p_reg->IPCR2, 0);
    }

    if (size < 4) {
        p_reg->IPCR2 |= __SEMC_IPCR2_BM3_MASK;
    }

    if (size < 3) {
        p_reg->IPCR2 |= __SEMC_IPCR2_BM2_MASK;
    }

    if (size < 2) {
        p_reg->IPCR2 |= __SEMC_IPCR2_BM1_MASK;
    }

    return AW_OK;
}

aw_local aw_err_t __semc_send_ipcommand (__imx1050_semc_regs_t *base, uint32_t address, uint16_t command, uint32_t write, uint32_t *read)
{
    uint32_t cmdMode;
    aw_bool_t readCmd = AW_FALSE;
    aw_bool_t writeCmd = AW_FALSE;
    aw_err_t result;

    /* Clear status bit */
    base->INTR |= __SEMC_INTR_IPCMDDONE_MASK;
    /* Set address. */
    base->IPCR0 = address;

    /* Check command mode. */
    cmdMode = command & 0xFU;

    readCmd =  (cmdMode == kSEMC_NANDCM_AXICmdAddrRead)  || (cmdMode == kSEMC_NANDCM_CommandAddressRead)  ||
               (cmdMode == kSEMC_NANDCM_CommandRead)     || (cmdMode == kSEMC_NANDCM_Read);
    writeCmd = (cmdMode == kSEMC_NANDCM_AXICmdAddrWrite) || (cmdMode == kSEMC_NANDCM_CommandAddressWrite) ||
               (cmdMode == kSEMC_NANDCM_CommandWrite)    || (cmdMode == kSEMC_NANDCM_Write);

    if (writeCmd) {
        /* Set data. */
        __SEMC_REG_WRITE(&base->IPTXDAT, write);
    }

    /* Set command code. */
    __SEMC_REG_WRITE(&base->IPCMD, (command | (0xa55a<<16)));

    /* Wait for command done. */
    result = __semc_ipcommand_done(base);
    if (result != AW_OK) {
        return result;
    }

    if (readCmd) {
        /* Get the read data */
        *read = base->IPRXDAT;
    }

    return AW_OK;
}

aw_local aw_err_t __semc_nand_write (__imx1050_semc_regs_t *base, uint32_t address, uint8_t *data, uint32_t size_bytes)
{
    aw_assert(data);

    aw_err_t result = AW_OK;
    uint16_t ipCmd;
    uint8_t dataSize = base->NANDCR0 & __SEMC_NANDCR0_PS_MASK;
    uint32_t tempData = 0;

    /* Write command built */
    ipCmd = __semc_build_ipcommand(0, kSEMC_NANDAM_ColumnRow, kSEMC_NANDCM_Write);
    while (size_bytes >= __SEMC_IPCOMMANDDATASIZEBYTEMAX) {
        /* Configure IP command data size. */
        __semc_config_ipcommand(base, __SEMC_IPCOMMANDDATASIZEBYTEMAX);
        __semc_idle_satus(base);
        result = __semc_send_ipcommand(base, address, ipCmd, *(uint32_t *)data, NULL);
        if (result != AW_OK) {
            break;
        }

        size_bytes -= __SEMC_IPCOMMANDDATASIZEBYTEMAX;
        data += __SEMC_IPCOMMANDDATASIZEBYTEMAX;
    }

    if ((result == AW_OK) && size_bytes) {

        __semc_config_ipcommand(base, size_bytes);

        while (size_bytes) {
            tempData |= ((uint32_t)*(data + size_bytes - 1) << ((size_bytes - 1) * __SEMC_BYTE_NUMBIT));
            size_bytes--;
        }

        __semc_idle_satus(base);
        result = __semc_send_ipcommand(base, address, ipCmd, tempData, NULL);
    }
    __semc_config_ipcommand(base, dataSize);

    return result;
}

aw_local aw_err_t __semc_nand_read (__imx1050_semc_regs_t *base, uint32_t address, uint8_t *data, uint32_t size_bytes)
{

    aw_err_t result = AW_OK;
    uint8_t dataSize = base->NANDCR0 & __SEMC_NANDCR0_PS_MASK;
    uint16_t ipCmd;
    uint32_t tempData = 0;

    /* Configure IP command data size. */
    __semc_config_ipcommand(base, __SEMC_IPCOMMANDDATASIZEBYTEMAX);
    /* Read command built */
    ipCmd = __semc_build_ipcommand(0, kSEMC_NANDAM_ColumnRow, kSEMC_NANDCM_Read);

    while (size_bytes >= __SEMC_IPCOMMANDDATASIZEBYTEMAX) {
        __semc_idle_satus(base);
        result = __semc_send_ipcommand(base, address, ipCmd, 0, (uint32_t *)data);
        if (result != AW_OK) {
            break;
        }

        data += __SEMC_IPCOMMANDDATASIZEBYTEMAX;
        size_bytes -= __SEMC_IPCOMMANDDATASIZEBYTEMAX;
    }

    while (size_bytes) {
        __semc_config_ipcommand(base, size_bytes);
        __semc_idle_satus(base);
        result = __semc_send_ipcommand(base, address, ipCmd, 0, &tempData);
        *(data) = (tempData >> (2*__SEMC_BYTE_NUMBIT * (size_bytes - 1))) & 0xFFU;
        data += 1;
        size_bytes -= 1;
    }

    __semc_config_ipcommand(base, dataSize);
    return result;
}



aw_local void imx1050_semc_inst_init2 (struct awbl_dev *p_awdev)
{
    __IMX1050_SEMC_DEVINFO_DECL(p_info, p_awdev);
    struct awbl_nandbus       *p_bus;
    struct awbl_imx1050_semc   *p_dev =
                 AW_CONTAINER_OF(p_awdev, struct awbl_imx1050_semc, nandbus);

    /* platform initialization */
    if (p_info->pfn_plfm_init != NULL) {
        p_info->pfn_plfm_init();
    }

    p_dev->p_semc_regs = (__imx1050_semc_regs_t *)p_info->semc_regbase;

    /* initialize hardware */
    __imx1050_semc_hw_init(p_dev);

    p_bus = &(p_dev->nandbus);

    p_bus->serv.pfn_opt    = imx1050_semc_opt;
    p_bus->serv.pfn_ready  = imx1050_semc_ready;
    p_bus->serv.pfn_reset  = imx1050_semc_reset;
    p_bus->serv.pfn_select = imx1050_semc_select;

    /**
     * 如果ecc_support不为AWBL_NAND_ECC_NONE，
     * 则需要填写NandFlash控制器中ecc_ctrl的相关内容
     */
    awbl_nandbus_create(p_bus, p_info->nandbus_index);

    aw_int_connect(p_info->inum, __imx1050_semc_isr, p_dev);
    aw_int_enable(p_info->inum);

    return;
}
/******************************************************************************/
aw_local aw_const struct awbl_drvfuncs __g_imx1050_semc_drvfuncs = {
        NULL,
        imx1050_semc_inst_init2,
        NULL
};

aw_local aw_const struct awbl_drvinfo __g_imx1050_semc_drv_registration = {
    AWBL_VER_1,                           /* awb_ver */
    AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,  /* bus_id */
    AWBL_IMX1050_SEMC_NAME,                 /* p_drvname */
    &__g_imx1050_semc_drvfuncs,             /* p_busfuncs */
    NULL,                                 /* p_methods */
    NULL                                  /* pfunc_drv_probe */
};


void awbl_imx1050_semc_drv_register (void)
{
    aw_err_t err;
    err = awbl_drv_register(&__g_imx1050_semc_drv_registration);
    aw_assert (err == 0);
    return;
}

aw_local aw_err_t __semc_ipcommand_done (__imx1050_semc_regs_t *p_reg)
{
    /* Poll status bit till command is done*/
    while (!(p_reg->INTR & __SEMC_INTR_IPCMDDONE_MASK))
    {};

    /* Clear status bit */
    p_reg->INTR |= __SEMC_INTR_IPCMDDONE_MASK;

    /* Check error status */
    if (p_reg->INTR & __SEMC_INTR_IPCMDERR_MASK) {
        p_reg->INTR |= __SEMC_INTR_IPCMDERR_MASK;
        return -AW_ERROR;
    }

    return 0;
}

/* end of file */


/* 带文件系统测试结果 */
// 62MB
//test_fs_read_write write speed 0.030 MB/s
//test_fs_read_write read speed 0.221 MB/s

//test_fs_read_write write speed 0.084 MB/s
//test_fs_read_write read speed 0.579 MB/s


//8MB

//test_fs_read_write write speed 0.040 MB/s
//test_fs_read_write read speed 0.222 MB/s

//test_fs_read_write write speed 0.106 MB/s
//test_fs_read_write read speed 0.582 MB/s


//6ul
//test_fs_read_write write speed 0.040 MB/s
//test_fs_read_write read speed 0.276 MB/s

//test_fs_read_write write speed 0.127 MB/s
//test_fs_read_write read speed 0.875 MB/s


//8MB
//test_fs_read_write write speed 0.051 MB/s
//test_fs_read_write read speed 0.273 MB/s

//test_fs_read_write write speed 0.158 MB/s
//test_fs_read_write read speed 0.868 MB/s
