#ifndef         __NAND_COMMON_H__
#define         __NAND_COMMON_H__

/* standard command */
#define __NANDFLASH_CMD_READ0             0
#define __NANDFLASH_CMD_READ1             1
#define __NANDFLASH_CMD_RNDOUT            5
#define __NANDFLASH_CMD_PAGEPROG          0x10
#define __NANDFLASH_CMD_READOOB           0x50
#define __NANDFLASH_CMD_ERASE1            0x60
#define __NANDFLASH_CMD_STATUS            0x70
#define __NANDFLASH_CMD_SEQIN             0x80
#define __NANDFLASH_CMD_RNDIN             0x85
#define __NANDFLASH_CMD_READID            0x90
#define __NANDFLASH_CMD_ERASE2            0xd0
#define __NANDFLASH_CMD_PARAM             0xec
#define __NANDFLASH_CMD_GET_FEATURES      0xee
#define __NANDFLASH_CMD_SET_FEATURES      0xef
#define __NANDFLASH_CMD_RESET             0xff
#define __NANDFLASH_CMD_LOCK              0x2a
#define __NANDFLASH_CMD_UNLOCK1           0x23
#define __NANDFLASH_CMD_UNLOCK2           0x24
/* expand command for large page */
#define __NANDFLASH_CMD_READSTART         0x30
#define __NANDFLASH_CMD_RNDOUTSTART       0xE0
#define __NANDFLASH_CMD_CACHEDPROG        0x15

/* \brief nand flash comman whitch is followed by address */
static const uint8_t __g_nand_flash_cmd_addr[] = {
        __NANDFLASH_CMD_SEQIN,
        __NANDFLASH_CMD_ERASE1,
        __NANDFLASH_CMD_READ0,
        __NANDFLASH_CMD_READ1,
        __NANDFLASH_CMD_READOOB,
        __NANDFLASH_CMD_READID
};

#endif
