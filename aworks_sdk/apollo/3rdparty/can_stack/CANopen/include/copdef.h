/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  default configuration of CANopen

  -------------------------------------------------------------------------

                $RCSfile: copdef.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.26 $  $Date: 2007/01/08 13:51:10 $
                
                $State: Exp $
                
                Build Environment:
                KEIL uVision 2

  -------------------------------------------------------------------------

  Revision History:

****************************************************************************/

#ifndef _COPDEF_H_
#define _COPDEF_H_


//===========================================================================
// defines for functionality time control (for operation systems)
//===========================================================================

#define FUNCTION_NO_CONTROL     0   // no time control
#define FUNCTION_CALL_TIME      1   // process function returnes next relative time to call it
#define FUNCTION_BREAK_TIME     2   // process function breaks after specified time

#define FUNCTION_CALLTIME_INFINITE  -1


//===========================================================================
// defines for CRC calculation modes
//===========================================================================

#define SDO_NO_CRC              0   // no CRC calculation
#define SDO_CRC_TABLE           1   // CRC table (faster than polynom but needs more memory)
#define SDO_CRC_POLYNOM         2   // CRC polynom


//===========================================================================
// Default configuration:
// If there are no configurations in COPCFG.H these default configurations
// will be active.
//
// DO NOT CHANGE THIS SETTINGS! TO CONFIGURE CANopen STACK USE ALWAYS FILE
// COPCFG.H IN YOUR PROJECT DIRECTORY!
//===========================================================================

#ifndef COP_USE_OPERATION_SYSTEM
    #if (defined (WIN32) || defined (LINUX) || defined (PXROS))
        #define COP_USE_OPERATION_SYSTEM     TRUE
        #define COP_USE_DELETEINST_FUNC      TRUE
    #else
        #define COP_USE_OPERATION_SYSTEM    FALSE
        //#define COP_USE_OPERATION_SYSTEM    TRUE
    #endif
#endif

#ifndef COP_USE_DELETEINST_FUNC
    #define COP_USE_DELETEINST_FUNC         CDRV_USE_DELETEINST_FUNC
#endif


//---------------------------------------------------------------------------
// CDRV specific defines
//---------------------------------------------------------------------------

// These default configurations are located in CDRV.H because CAN driver can
// be sold as stand alone product.


//---------------------------------------------------------------------------
// COB specific defines
//---------------------------------------------------------------------------

#ifndef COB_MORE_THAN_128_ENTRIES
    #define COB_MORE_THAN_128_ENTRIES       FALSE
#endif

#ifndef COB_MORE_THAN_256_ENTRIES
    #define COB_MORE_THAN_256_ENTRIES       FALSE
#endif

#ifndef COB_SEARCHALGO
    #define COB_SEARCHALGO                  COB_SEARCHALGO_LIST
#endif

// Function CobClearRtr() only will be used from PDO module if REMOTE-PDO's
// are used. Therefore this definition depends on definition of PDO_USE_REMOTE_PDOS.
#ifndef COB_USE_RTR_CONSUMER
    #define COB_USE_RTR_CONSUMER            PDO_USE_REMOTE_PDOS
#endif

// Function CobGetParam() is never used (perhabs only in SO-874).
#ifndef COB_USE_ADDITIONAL_API
    #define COB_USE_ADDITIONAL_API          TRUE
#endif

#ifndef COB_IMPLEMENT_SET_PARAM
    #define COB_IMPLEMENT_SET_PARAM         TRUE
#endif


//---------------------------------------------------------------------------
// OBD specific defines
//---------------------------------------------------------------------------

#ifndef OBD_SUPPORTED_OBJ_SIZE
    #define OBD_SUPPORTED_OBJ_SIZE          OBD_OBJ_SIZE_SMALL
#endif

#ifndef OBD_CHECK_OBJECT_RANGE
    #define OBD_CHECK_OBJECT_RANGE          FALSE
#endif

#ifndef OBD_USE_DYNAMIC_OD
    #define OBD_USE_DYNAMIC_OD              FALSE
#endif

#ifndef OBD_CHECK_FLOAT_VALID
    #define OBD_CHECK_FLOAT_VALID           FALSE
#endif

// should be TRUE for default because OpenPCS (it should work without changes)
// but for code size optimizations we want be able to deactivate the code
#ifndef OBD_USE_STRING_DOMAIN_IN_RAM
    #define OBD_USE_STRING_DOMAIN_IN_RAM    TRUE
#endif

#ifndef OBD_USE_STORE_RESTORE
    #define OBD_USE_STORE_RESTORE           CCM_USE_STORE_RESTORE
#endif

#ifndef OBD_INCLUDE_A000_TO_DEVICE_PART
    #define OBD_INCLUDE_A000_TO_DEVICE_PART FALSE
#endif

#ifndef OBD_USE_VARIABLE_SUBINDEX_TAB
    #define OBD_USE_VARIABLE_SUBINDEX_TAB   FALSE
#endif

#ifndef OBD_IMPLEMENT_PDO_FCT
    #define OBD_IMPLEMENT_PDO_FCT           TRUE
#endif

#ifndef OBD_IMPLEMENT_ARRAY_FCT
    #define OBD_IMPLEMENT_ARRAY_FCT         TRUE
#endif

#ifndef OBD_IMPLEMENT_READ_WRITE
    #define OBD_IMPLEMENT_READ_WRITE        TRUE
#endif

#ifndef OBD_IMPLEMENT_DEFINE_VAR
    #define OBD_IMPLEMENT_DEFINE_VAR        TRUE
#endif

#ifndef OBD_IMPLEMENT_INIT_MOD_TAB
    #define OBD_IMPLEMENT_INIT_MOD_TAB      TRUE
#endif


//---------------------------------------------------------------------------
// LSS slave specific defines
//---------------------------------------------------------------------------

#ifndef LSSS_PROCESS_TIME_CONTROL
    #define LSSS_PROCESS_TIME_CONTROL       FUNCTION_NO_CONTROL
#endif


//---------------------------------------------------------------------------
// LSS master specific defines
//---------------------------------------------------------------------------

#ifndef LSSM_PROCESS_TIME_CONTROL
    #define LSSM_PROCESS_TIME_CONTROL       FUNCTION_NO_CONTROL
#endif

#ifndef LSSM_PROCESS_DELAY_TIME
    #define LSSM_PROCESS_DELAY_TIME         50  // = 5ms
#endif

#ifndef LSSM_CONFIRM_TIMEOUT
    #define LSSM_CONFIRM_TIMEOUT            500 // = 50ms
#endif


//---------------------------------------------------------------------------
// NMT common specific defines
//---------------------------------------------------------------------------

#ifndef NMT_IMP_NMTMHBC_CTRL
    // if NMTM module or HBC module is included then automaticaly NMTMHBC control
    // will be inmplemented
    #if ((CCM_MODULE_INTEGRATION & (CCM_MODULE_NMTM | CCM_MODULE_HBC)) != 0)
        #define NMT_IMP_NMTMHBC_CTRL        TRUE
    #else
        #define NMT_IMP_NMTMHBC_CTRL        FALSE
    #endif
#endif

#ifndef NMT_IMP_NMTSHBP_CTRL
    // if NMTS module or HBP module is included then automaticaly NMTSHBP control
    // will be inmplemented
    #if ((CCM_MODULE_INTEGRATION & (CCM_MODULE_NMTS | CCM_MODULE_HBP)) != 0)
        #define NMT_IMP_NMTSHBP_CTRL        TRUE
    #else
        #define NMT_IMP_NMTSHBP_CTRL        FALSE
    #endif
#endif


//---------------------------------------------------------------------------
// NMT slave specific defines
//---------------------------------------------------------------------------

#ifndef NMTS_PROCESS_TIME_CONTROL
    #define NMTS_PROCESS_TIME_CONTROL       FUNCTION_NO_CONTROL
#endif

#ifndef NMTS_USE_LIFEGUARDING
    #define NMTS_USE_LIFEGUARDING           TRUE
#endif

#ifndef NMTS_USE_NODEGUARDING
    #define NMTS_USE_NODEGUARDING           TRUE
#endif


//---------------------------------------------------------------------------
// NMT master specific defines
//---------------------------------------------------------------------------

#ifndef NMTM_PROCESS_TIME_CONTROL
    #define NMTM_PROCESS_TIME_CONTROL       FUNCTION_NO_CONTROL
#endif


//---------------------------------------------------------------------------
// HBP specific defines
//---------------------------------------------------------------------------

#ifndef HBP_PROCESS_TIME_CONTROL
    #define HBP_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL
#endif


//---------------------------------------------------------------------------
// HBC specific defines
//---------------------------------------------------------------------------

#ifndef HBC_PROCESS_TIME_CONTROL
    #define HBC_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL
#endif

// Functions HbcGetNextFreeConsumer() and HbcGetHeartbeatTime() only will
// be used in SO-1063. Therefore default value is set to FALSE.
#ifndef HBC_USE_ADDITIONAL_API
    #define HBC_USE_ADDITIONAL_API          FALSE
#endif


//---------------------------------------------------------------------------
// EMCP specific defines
//---------------------------------------------------------------------------

#ifndef EMCP_USE_PREDEF_ERROR_FIELD
    #define EMCP_USE_PREDEF_ERROR_FIELD     TRUE
#endif

#ifndef EMCP_USE_EVENT_CALLBACK
    #define EMCP_USE_EVENT_CALLBACK         FALSE
#endif

#ifndef EMCP_ENABLE_ERROR_WRITE
    #define EMCP_ENABLE_ERROR_WRITE         FALSE
#endif


//---------------------------------------------------------------------------
// PDO specific defines
//---------------------------------------------------------------------------

#ifndef PDO_PROCESS_TIME_CONTROL
    #define PDO_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL
#endif

#ifndef PDO_GRANULARITY
    #define PDO_GRANULARITY                 8
#endif

#ifndef PDO_USE_EVENT_TIMER
    #define PDO_USE_EVENT_TIMER             FALSE
#endif

#ifndef PDO_USE_SYNC_PDOS
    #define PDO_USE_SYNC_PDOS               FALSE
#endif

#ifndef PDO_USE_REMOTE_PDOS
    #define PDO_USE_REMOTE_PDOS             FALSE
#endif

#ifndef PDO_USE_SYNC_PRODUCER
    #define PDO_USE_SYNC_PRODUCER           FALSE
#endif

#ifndef PDO_USE_ERROR_CALLBACK
    #define PDO_USE_ERROR_CALLBACK          FALSE
#endif

#ifndef PDO_USE_STATIC_MAPPING
    #define PDO_USE_STATIC_MAPPING          FALSE
#endif

#ifndef PDO_USE_DUMMY_MAPPING
    #define PDO_USE_DUMMY_MAPPING           TRUE
#endif

#ifndef PDO_DISABLE_FORCE_PDO
    #define PDO_DISABLE_FORCE_PDO           FALSE
#endif

#ifndef PDO_USE_BIT_MAPPING
    #define PDO_USE_BIT_MAPPING             FALSE
#endif

#ifndef PDO_USE_MPDO_DAM_PRODUCER
    #define PDO_USE_MPDO_DAM_PRODUCER       FALSE
#endif

#ifndef PDO_USE_MPDO_DAM_CONSUMER
    #define PDO_USE_MPDO_DAM_CONSUMER       FALSE
#endif

#ifndef PDO_USE_MPDO_SAM_PRODUCER
    #define PDO_USE_MPDO_SAM_PRODUCER       FALSE
#endif

#ifndef PDO_USE_MPDO_SAM_CONSUMER
    #define PDO_USE_MPDO_SAM_CONSUMER       FALSE
#endif

#ifndef PDO_USE_MUX_PDOS
    #if ( (PDO_USE_MPDO_DAM_PRODUCER != FALSE) || (PDO_USE_MPDO_DAM_CONSUMER != FALSE) || \
          (PDO_USE_MPDO_SAM_PRODUCER != FALSE) || (PDO_USE_MPDO_SAM_CONSUMER != FALSE)    )
        #define PDO_USE_MUX_PDOS            TRUE
    #else
        #define PDO_USE_MUX_PDOS            FALSE
    #endif
#endif

// Function PdoRegisterPdoTab() only will be used in SO-874 (dynamic OD)
// Therefore it depends on defininition of OBD_USE_DYNAMIC_OD.
#ifndef PDO_USE_ADDITIONAL_API
    #define PDO_USE_ADDITIONAL_API          OBD_USE_DYNAMIC_OD
#endif

#ifndef CCM_USE_PDO_CHECK_VAR
    #define CCM_USE_PDO_CHECK_VAR           TRUE
#endif


//---------------------------------------------------------------------------
#if ((PDO_GRANULARITY != 8) && (PDO_USE_BIT_MAPPING != FALSE))
    #define PDO_MAX_MAPPENTRIES     64
#else
    #define PDO_MAX_MAPPENTRIES     8
#endif


//---------------------------------------------------------------------------
// SDO Server specific defines
//---------------------------------------------------------------------------

#ifndef SDOS_PROCESS_TIME_CONTROL
    #define SDOS_PROCESS_TIME_CONTROL       FUNCTION_NO_CONTROL
#endif

#ifndef SDO_BLOCKTRANSFER
    #define SDO_BLOCKTRANSFER               FALSE
#endif

#ifndef SDO_BLOCKSIZE_DOWNLOAD
    #define SDO_BLOCKSIZE_DOWNLOAD          6
#endif

#ifndef SDO_BLOCKSIZE_UPLOAD
    #define SDO_BLOCKSIZE_UPLOAD            6
#endif

#ifndef SDO_SEGMENTTRANSFER
    #define SDO_SEGMENTTRANSFER             TRUE
#endif

#ifndef SDO_CALCULATE_CRC
    #define SDO_CALCULATE_CRC               SDO_NO_CRC
#endif

#ifndef SDOS_DEFAULT_TIMEOUT
    #define SDOS_DEFAULT_TIMEOUT            5000
#endif

// Function SdosGetFreeSdosServer() only is used in SO-874. Therefore
// defualt configuration is set to FALSE. If you have to use this function,
// then set it to TRUE in CopCfg.h!
#ifndef SDOS_USE_ADDITIONAL_API
    #define SDOS_USE_ADDITIONAL_API         FALSE
#endif

#ifndef SDOS_MULTI_SERVER_SUPPORT
    #define SDOS_MULTI_SERVER_SUPPORT       TRUE
#endif


//---------------------------------------------------------------------------
// SDO Client specific defines
//---------------------------------------------------------------------------

#ifndef SDOC_PROCESS_TIME_CONTROL
    #define SDOC_PROCESS_TIME_CONTROL       FUNCTION_NO_CONTROL
#endif

#ifndef SDOC_DEFAULT_TIMEOUT
    #define SDOC_DEFAULT_TIMEOUT            5000
#endif

// The following SDO Client function only will be used in SO-874 and SO-1063:
// SdocGetFreeSdoClient(), SdocGetNextFreeClient(), SdocGetClientNumber(),
// SdocConvertClientIndex(), SdocGetTransmittedBytes()
#ifndef SDOC_USE_ADDITIONAL_API
    #define SDOC_USE_ADDITIONAL_API         FALSE
#endif


//---------------------------------------------------------------------------
// SRDO specific defines
//---------------------------------------------------------------------------

#ifndef SRDO_GRANULARITY
    #define SRDO_GRANULARITY                 16
#endif

#ifndef SRDO_USE_STATIC_MAPPING
    #define SRDO_USE_STATIC_MAPPING         TRUE
#endif

#ifndef SRDO_USE_DUMMY_MAPPING
    #define SRDO_USE_DUMMY_MAPPING          FALSE
#endif

//---------------------------------------------------------------------------
#if (SRDO_GRANULARITY == 8)
    #define SRDO_MAX_MAPPENTRIES     16
#elif (SRDO_GRANULARITY == 16)
    #define SRDO_MAX_MAPPENTRIES     8
#elif (SRDO_GRANULARITY == 32)
    #define SRDO_MAX_MAPPENTRIES     4
#elif (SRDO_GRANULARITY == 64)
    #define SRDO_MAX_MAPPENTRIES     2
#else
    #error 'ERROR: invalid SRDO granularity!'
#endif

//---------------------------------------------------------------------------
// SDOM specific defines
//---------------------------------------------------------------------------
#ifndef SDOM_COB_ID_LIST_COUNT
    #define SDOM_COB_ID_LIST_COUNT 254
#endif

#ifndef SDOM_CONNECT_LIST_COUNT
    #define SDOM_CONNECT_LIST_COUNT 254
#endif

//---------------------------------------------------------------------------
// SRD specific defines
//---------------------------------------------------------------------------
#ifndef SRD_DEFAULT_TIMEOUT
    #define SRD_DEFAULT_TIMEOUT 50000
#endif

#ifndef SRD_DEFAULT_RETRYCOUNT
    #define SRD_DEFAULT_RETRYCOUNT 6
#endif

//---------------------------------------------------------------------------
// default defug level
#ifndef DEF_DEBUG_LVL
    #define DEF_DEBUG_LVL           0xE0800000
#endif
// means (COP_DBGLVL_ALWAYS | COP_DBGLVL_ERROR | COP_DBGLVL_ASSERT | COP_DBGLVL_CCM)
// see cop.h for possible values



#endif // _COPDEF_H_

// Please keep an empty line at the end of this file.

