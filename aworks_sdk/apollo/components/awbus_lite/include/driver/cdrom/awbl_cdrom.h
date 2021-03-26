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
 * \brief CDROM class
 *
 * \internal
 * \par modification history:
 * - 1.00 17-06-24  anu, first implementation
 * \endinternal
 */

#ifndef __AWBL_CDROM_H
#define __AWBL_CDROM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/* CDROM Command type Definitions 下面的命令表请参考ssf-8020i */
typedef uint8_t awbl_cdrom_cmd_type_t;

/** 
 * \brief The generic packet command opcodes for CD/DVD Logical Units,
 *        From Table 57 of the SFF8090 Ver. 3 (Mt. Fuji) draft standard. 
 */
#define AWBL_CDROM_CMD_BLANK                         0xA1
#define AWBL_CDROM_CMD_CLOSE_TRACK                   0x5B
#define AWBL_CDROM_CMD_FLUSH_CACHE                   0x35
#define AWBL_CDROM_CMD_FORMAT_UNIT                   0x04
#define AWBL_CDROM_CMD_GET_CONFIGURATION             0x46
#define AWBL_CDROM_CMD_GET_EVENT_STATUS_NOTIFICATION 0x4A
#define AWBL_CDROM_CMD_GET_PERFORMANCE               0xAC
#define AWBL_CDROM_CMD_INQUIRY                       0x12
#define AWBL_CDROM_CMD_LOAD_UNLOAD                   0xA6
#define AWBL_CDROM_CMD_MECHANISM_STATUS              0xBD
#define AWBL_CDROM_CMD_MODE_SELECT_10                0x55
#define AWBL_CDROM_CMD_MODE_SENSE_10                 0x5A
#define AWBL_CDROM_CMD_PAUSE_RESUME                  0x4B
#define AWBL_CDROM_CMD_PLAY_AUDIO_10                 0x45
#define AWBL_CDROM_CMD_PLAY_AUDIO_MSF                0x47
#define AWBL_CDROM_CMD_PLAY_AUDIO_TI                 0x48
#define AWBL_CDROM_CMD_PLAY_CD                       0xBC
#define AWBL_CDROM_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL  0x1E
#define AWBL_CDROM_CMD_READ_10                       0x28
#define AWBL_CDROM_CMD_READ_12                       0xA8
#define AWBL_CDROM_CMD_READ_BUFFER                   0x3C
#define AWBL_CDROM_CMD_READ_BUFFER_CAPACITY          0x5C
#define AWBL_CDROM_CMD_READ_CDVD_CAPACITY            0x25
#define AWBL_CDROM_CMD_READ_CD                       0xBE
#define AWBL_CDROM_CMD_READ_CD_MSF                   0xB9
#define AWBL_CDROM_CMD_READ_DISC_INFO                0x51
#define AWBL_CDROM_CMD_READ_DVD_STRUCTURE            0xAD
#define AWBL_CDROM_CMD_READ_FORMAT_CAPACITIES        0x23
#define AWBL_CDROM_CMD_READ_HEADER                   0x44
#define AWBL_CDROM_CMD_READ_TRACK_RZONE_INFO         0x52
#define AWBL_CDROM_CMD_READ_SUBCHANNEL               0x42
#define AWBL_CDROM_CMD_READ_TOC_PMA_ATIP             0x43
#define AWBL_CDROM_CMD_REPAIR_RZONE_TRACK            0x58
#define AWBL_CDROM_CMD_REPORT_KEY                    0xA4
#define AWBL_CDROM_CMD_REQUEST_SENSE                 0x03
#define AWBL_CDROM_CMD_RESERVE_RZONE_TRACK           0x53
#define AWBL_CDROM_CMD_SEND_CUE_SHEET                0x5D
#define AWBL_CDROM_CMD_SCAN                          0xBA
#define AWBL_CDROM_CMD_SEEK                          0x2B
#define AWBL_CDROM_CMD_SEND_DVD_STRUCTURE            0xBF
#define AWBL_CDROM_CMD_SEND_EVENT                    0xA2
#define AWBL_CDROM_CMD_SEND_KEY                      0xA3
#define AWBL_CDROM_CMD_SEND_OPC                      0x54
#define AWBL_CDROM_CMD_SET_READ_AHEAD                0xA7
#define AWBL_CDROM_CMD_SET_STREAMING                 0xB6
#define AWBL_CDROM_CMD_START_STOP_UNIT               0x1B
#define AWBL_CDROM_CMD_STOP_PLAY_SCAN                0x4E
#define AWBL_CDROM_CMD_TEST_UNIT_READY               0x00
#define AWBL_CDROM_CMD_VERIFY_10                     0x2F
#define AWBL_CDROM_CMD_WRITE_10                      0x2A
#define AWBL_CDROM_CMD_WRITE_12                      0xAA
#define AWBL_CDROM_CMD_WRITE_AND_VERIFY_10           0x2E
#define AWBL_CDROM_CMD_WRITE_BUFFER                  0x3B
/** 
 * \brief This is listed as optional in ATAPI 2.6, but is (curiously)
 *        missing from Mt. Fuji, Table 57.  It _is_ mentioned in Mt. Fuji
 *        Table 377 as an MMC command for SCSi devices though...  Most ATAPI
 *        drives support it. 
 */
#define AWBL_CDROM_CMD_SET_SPEED                     0xBB
/** 
 * \brief This seems to be a SCSI specific CD-ROM opcode
 *        to play data at track/index 
 */
#define AWBL_CDROM_CMD_PLAYAUDIO_TI                  0x48
/**
 * \brief From MS Media Status Notification Support Specification. For
 *        older drives only.
 */
#define AWBL_CDROM_CMD_GET_MEDIA_STATUS              0xDA


/**
 * \brief Mode page codes for mode sense/set 下面的命令表请参考ssf-8020i 
 */
typedef uint8_t awbl_cdrom_mode_page_code_t;

/** \brief  Mode page codes for mode sense/set */
#define AWBL_CDROM_MODE_VENDOR_PAGE          0x00
#define AWBL_CDROM_MODE_R_W_ERROR_PAGE       0x01
#define AWBL_CDROM_MODE_WRITE_PARMS_PAGE     0x05
#define AWBL_CDROM_MODE_WCACHING_PAGE        0x08
#define AWBL_CDROM_MODE_AUDIO_CTL_PAGE       0x0e
#define AWBL_CDROM_MODE_POWER_PAGE           0x1a
#define AWBL_CDROM_MODE_FAULT_FAIL_PAGE      0x1c
#define AWBL_CDROM_MODE_TO_PROTECT_PAGE      0x1d
#define AWBL_CDROM_MODE_CAPABILITIES_PAGE    0x2a
#define AWBL_CDROM_MODE_ALL_PAGES            0x3f
/** 
 * \brief Not in Mt. Fuji, but in ATAPI 2.6 -- deprecated now in favor
 *        of MODE_SENSE_POWER_PAGE 
 */
#define AWBL_CDROM_MODE_CDROM_PAGE           0x0d


/**
 * \brief A CD-ROM physical sector size is 2048, 2052, 2056, 2324, 2332, 2336,
 *        2340, or 2352 bytes long.
 *
 *         Sector types of the standard CD-ROM data formats:
 *
 * format   sector type               user data size (bytes)
 * -----------------------------------------------------------------------------
 *   1     (Red Book)    CD-DA          2352    (CD_FRAMESIZE_RAW)
 *   2     (Yellow Book) Mode1 Form1    2048    (CD_FRAMESIZE)
 *   3     (Yellow Book) Mode1 Form2    2336    (CD_FRAMESIZE_RAW0)
 *   4     (Green Book)  Mode2 Form1    2048    (CD_FRAMESIZE)
 *   5     (Green Book)  Mode2 Form2    2328    (2324+4 spare bytes)
 *
 *
 *       The layout of the standard CD-ROM data formats:
 * -----------------------------------------------------------------------------
 * - audio (red):                  | audio_sample_bytes |
 *                                 |        2352        |
 *
 * - data (yellow, mode1):         | sync - head - data - EDC - zero - ECC |
 *                                 |  12  -   4  - 2048 -  4  -   8  - 276 |
 *
 * - data (yellow, mode2):         | sync - head - data |
 *                                 |  12  -   4  - 2336 |
 *
 * - XA data (green, mode2 form1): | sync - head - sub - data - EDC - ECC |
 *                                 |  12  -   4  -  8  - 2048 -  4  - 276 |
 *
 * - XA data (green, mode2 form2): | sync - head - sub - data - Spare |
 *                                 |  12  -   4  -  8  - 2324 -  4    |
 *
 */

/** \brief Some generally useful CD-ROM information -- mostly based on the above */
/** \brief max. minutes per CD, not really a limit */
#define AWBL_CDROM_MINS              74 
/** \brief seconds per minute */
#define AWBL_CDROM_SECS              60 
/** \brief frames per second */
#define AWBL_CDROM_FRAMES            75 
/** \brief 12 sync bytes per raw data frame */
#define AWBL_CDROM_SYNC_SIZE         12 
/** \brief MSF numbering offset of first frame */
#define AWBL_CDROM_MSF_OFFSET       150 
/** \brief lowest-level "data bytes piece" */
#define AWBL_CDROM_CHUNK_SIZE        24 
/** \brief chunks per frame */
#define AWBL_CDROM_NUM_OF_CHUNKS     98 
/** \brief subchannel data "frame" size */
#define AWBL_CDROM_FRAMESIZE_SUB     96 
/** \brief header (address) bytes per raw data frame */
#define AWBL_CDROM_HEAD_SIZE          4 
/** \brief subheader bytes per raw XA data frame */
#define AWBL_CDROM_SUBHEAD_SIZE       8 
/** \brief bytes EDC per most raw data frame types */
#define AWBL_CDROM_EDC_SIZE           4 
/** \brief bytes zero per yellow book mode 1 frame */
#define AWBL_CDROM_ZERO_SIZE          8 
/** \brief bytes ECC per most raw data frame types */
#define AWBL_CDROM_ECC_SIZE         276 
/** \brief bytes per frame, "cooked" mode */
#define AWBL_CDROM_FRAMESIZE       2048 
/** \brief bytes per frame, "raw" mode */
#define AWBL_CDROM_FRAMESIZE_RAW   2352 
/** \brief The maximum possible returned bytes */
#define AWBL_CDROM_FRAMESIZE_RAWER 2646 
/** \brief  most drives don't deliver everything: */
#define AWBL_CDROM_FRAMESIZE_RAW1 \
    (AWBL_CDROM_FRAMESIZE_RAW - AWBL_CDROM_SYNC_SIZE)                        
#define AWBL_CDROM_FRAMESIZE_RAW0 \
    (AWBL_CDROM_FRAMESIZE_RAW - AWBL_CDROM_SYNC_SIZE - AWBL_CDROM_HEAD_SIZE) 

/** \brief "before data" part of raw XA frame */
#define AWBL_CDROM_XA_HEAD      (AWBL_CDROM_HEAD_SIZE + AWBL_CDROM_SUBHEAD_SIZE) 
/** \brief "after data" part of raw XA frame */
#define AWBL_CDROM_XA_TAIL      (AWBL_CDROM_EDC_SIZE  + AWBL_CDROM_ECC_SIZE)   
/** \brief sync bytes + header of XA frame */  
#define AWBL_CDROM_XA_SYNC_HEAD (AWBL_CDROM_SYNC_SIZE + AWBL_CDROM_XA_HEAD)      

/** \brief  Request Sense Standard Data and bit define */
typedef struct awbl_cdrom_sense_request {

    uint8_t error_code;
    uint8_t segment_number;
    uint8_t sense_key;
    uint8_t information[4];
    uint8_t add_sense_len;
    uint8_t command_info[4];
    uint8_t asc;
    uint8_t ascq;
    uint8_t fruc;
    uint8_t sks[3];
    uint8_t asb[46];

} awbl_cdrom_sense_request_t;

#define AWBL_CDROM_SENSE_REQUEST_ERROR_CODE_SHIFT   0
#define AWBL_CDROM_SENSE_REQUEST_VALID_SHIFT        7
#define AWBL_CDROM_SENSE_REQUEST_SENSE_KEY_SHIFT    0
#define AWBL_CDROM_SENSE_REQUEST_ILI_SHIFT          5

#define AWBL_CDROM_SENSE_REQUEST_ERROR_CODE_MASK    (0x7F)
#define AWBL_CDROM_SENSE_REQUEST_VALID_MASK         (0x80)
#define AWBL_CDROM_SENSE_REQUEST_SENSE_KEY_MASK     (0x0F)
#define AWBL_CDROM_SENSE_REQUEST_ILI_MASK           (0x20)

/** \brief  read/write Data Rates (kbytes) */
typedef uint16_t awbl_cdrom_data_rates_t;
#define AWBL_CDROM_DATA_RATES_X1      176  /* 176 KBytes/second */
#define AWBL_CDROM_DATA_RATES_X2      353  /* 353 KBytes/second */
#define AWBL_CDROM_DATA_RATES_X2_2    387  /* 387 KBytes/second */
#define AWBL_CDROM_DATA_RATES_X3      528  /* 528 KBytes/second */
#define AWBL_CDROM_DATA_RATES_X4      706  /* 706 KBytes/second */
#define AWBL_CDROM_DATA_RATES_X8      1400 /* 1.4 MBytes/second */
#define AWBL_CDROM_DATA_RATES_X16     2800 /* 2.8 MBytes/second */

/** \brief  CD-ROM Capabilities and Mechanical Status Page Format */
typedef struct awbl_cdrom_capabilities_and_status {

    uint8_t rsvd0[8];
    uint8_t page_code;
    uint8_t page_length;
    uint8_t capabilities[6];
    uint8_t max_speed[2];
    uint8_t num_volume_level[2];
    uint8_t buf_size[2];
    uint8_t current_speed[2];
    uint8_t rsvd1;
    uint8_t digital_output;
    uint8_t rsvd2[2];

} awbl_cdrom_capabilities_and_status_t;


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_CDROM_H */
