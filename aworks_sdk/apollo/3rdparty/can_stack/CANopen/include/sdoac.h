/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  definitions for SDO Abort codes

  -------------------------------------------------------------------------

                $RCSfile: SdoAc.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.5 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

  a.s.: first implementation

****************************************************************************/

#ifndef _SDOACDEF_H_
#define _SDOACDEF_H_


// =========================================================================
// SDO abort codes
// =========================================================================

#define    SDOAC_TOGGEL_BIT_ERROR                    0x05030000L
#define    SDOAC_TIME_OUT                            0x05040000L 
#define    SDOAC_UNKNOWN_COMMAND_SPECIFIER           0x05040001L 
#define    SDOAC_INVALID_BLOCK_SIZE                  0x05040002L 
#define    SDOAC_INVALID_SEQUENCE_NUMBER             0x05040003L 
#define    SDOAC_CRC_ERROR                           0x05040004L 
#define    SDOAC_OUT_OF_MEMORY                       0x05040005L 
#define    SDOAC_UNSUPPORTED_ACCESS                  0x06010000L 
#define    SDOAC_READ_TO_WRITE_ONLY_OBJ              0x06010001L 
#define    SDOAC_WRITE_TO_READ_ONLY_OBJ              0x06010002L 
#define    SDOAC_OBJECT_NOT_EXIST                    0x06020000L 
#define    SDOAC_OBJECT_NOT_MAPPABLE                 0x06040041L 
#define    SDOAC_PDO_LENGTH_EXCEEDED                 0x06040042L 
#define    SDOAC_GEN_PARAM_INCOMPATIBILITY           0x06040043L 
#define    SDOAC_GEN_INTERNAL_INCOMPATIBILITY        0x06040047L 
#define    SDOAC_ACCESS_FAILED_DUE_HW_ERROR          0x06060000L 
#define    SDOAC_DATA_TYPE_LENGTH_NOT_MATCH          0x06070010L 
#define    SDOAC_DATA_TYPE_LENGTH_TOO_HIGH           0x06070012L 
#define    SDOAC_DATA_TYPE_LENGTH_TOO_LOW            0x06070013L 
#define    SDOAC_SUB_INDEX_NOT_EXIST                 0x06090011L 
#define    SDOAC_VALUE_RANGE_EXCEEDED                0x06090030L 
#define    SDOAC_VALUE_RANGE_TOO_HIGH                0x06090031L 
#define    SDOAC_VALUE_RANGE_TOO_LOW                 0x06090032L 
#define    SDOAC_MAX_VALUE_LESS_MIN_VALUE            0x06090036L 
#define    SDOAC_RESOURCE_NOT_AVAILABLE              0x060A0023L
#define    SDOAC_GENERAL_ERROR                       0x08000000L 
#define    SDOAC_DATA_NOT_TRANSF_OR_STORED           0x08000020L 
#define    SDOAC_DATA_NOT_TRANSF_DUE_LOCAL_CONTROL   0x08000021L 
#define    SDOAC_DATA_NOT_TRANSF_DUE_DEVICE_STATE    0x08000022L 
#define    SDOAC_OBJECT_DICTIONARY_NOT_EXIST         0x08000023L


#endif

// Please keep an empty line at the end of this file.
