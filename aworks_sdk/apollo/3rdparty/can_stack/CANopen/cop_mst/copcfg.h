/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5 / CAN driver

  Description:  configuration file

  -------------------------------------------------------------------------

                $RCSfile: CopCfg.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.6 $  $Date: 2006/04/27 13:02:27 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

****************************************************************************/

#ifndef _COPCFG_H_
#define _COPCFG_H_


// =========================================================================
// generic defines which for whole CANopen
// =========================================================================

// COP_MAX_INSTANCES specifies count of instances of all CANopen modules
// except CAN driver. If it is greater than 1 the first parameter of all
// functions is the instance number.
#define COP_MAX_INSTANCES               1
//#define COP_MAX_INSTANCES               2

// This defines the target hardware. Here is encoded wich CPU and wich external
// peripherals are connected. For possible values refere to target.h. If
// necessary value is not available CAN driver and/or CANopen stack has to
// be adapted and tested.
#define TARGET_HARDWARE                 TGTHW_PPC_PHI_LPC_2294

// If COP_USE_CDRV_FUNCTION_POINTER != FALSE the COB module calls CAN driver
// functions with function pointers. Member m_CdrvFct of tCobInitParam
// has to be filled out. This makes CANopen stack slower but it is very
// important for applications with more CAN drivers than one.
#define COP_USE_CDRV_FUNCTION_POINTER   FALSE
//#define COP_USE_CDRV_FUNCTION_POINTER   TRUE

// If zhis define is set to TRUE then type tTime will be defined as WORD
// instead of DWORD. This is important for applications with smal CODE
// and DATA memory. But you have to know, that all timer of CANopen only
// can run until 6.5536 seconds if tTime = WORD!
#define COP_USE_SMALL_TIME              FALSE
//#define COP_USE_SMALL_TIME              TRUE

// Sometimes CANopen should not be shuted down (application specific).
// Therefore the function CcmShutDownCANopen() will not be compiled,
// because reducing code memory.
#define COP_USE_DELETEINST_FUNC         TRUE
#define CDRV_USE_DELETEINST_FUNC        TRUE

// Default defug level:
// Only debug traces of these modules will be compiled which flags are set in define DEF_DEBUG_LVL.
#define DEF_DEBUG_LVL           0xE0800000L
//   COP_DBGLVL_CDRV        =   0x00000001L
//   COP_DBGLVL_COB         =   0x00000002L
//   COP_DBGLVL_OBD         =   0x00000004L
//   COP_DBGLVL_NMT         =   0x00000008L
//   COP_DBGLVL_NMTS        =   0x00000010L
//   COP_DBGLVL_NMTM        =   0x00000020L
//   COP_DBGLVL_HBP         =   0x00000040L
//   COP_DBGLVL_HBC         =   0x00000080L
//   COP_DBGLVL_EMCP        =   0x00000100L
//   COP_DBGLVL_EMCC        =   0x00000200L
//   COP_DBGLVL_PDO         =   0x00000400L
//   COP_DBGLVL_SDOS        =   0x00000800L
//   COP_DBGLVL_SDOC        =   0x00001000L
//   COP_DBGLVL_LSSS        =   0x00002000L
//   COP_DBGLVL_LSSM        =   0x00004000L
//   COP_DBGLVL_SRDO        =   0x00008000L
// * COP_DBGLVL_CCM         =   0x00800000L
//   COP_DBGLVL_CCM302      =   0x01000000L
// * COP_DBGLVL_ASSERT      =   0x20000000L
// * COP_DBGLVL_ERROR       =   0x40000000L
// * COP_DBGLVL_ALWAYS      =   0x80000000L

// =========================================================================
// COPMGR specific optional features
// =========================================================================
#define COPMGR_FEATURES                    (0x000001FFL)
//#define COPMGR_FEAT_INCL_SLAVEASSIGN  0x00000001L // SlaveAssignment (include object 0x1F81)
//#define COPMGR_FEAT_INCL_REQUESTNMT   0x00000002L // RequestNMT (include objects 0x1F82, 0x1F83)
//#define COPMGR_FEAT_COMP_DEVICETYPE   0x00000004L // compare DeviceType (include object 0x1F84)
//#define COPMGR_FEAT_COMP_PRODUCT      0x00000008L // compare VendorID, ProductCode, RevisionNumber (include objects 0x1F85, 0x1F86, 0x1F87)
//#define COPMGR_FEAT_COMP_SERIALNUMBER 0x00000010L // compare SerialNumber (include object 0x1F88)
//#define COPMGR_FEAT_INCL_BOOTTIME     0x00000020L // maximum BootTime (include object 0x1F89)
//#define COPMGR_FEAT_INCL_EXT_SLAVE    0x00000040L // extended slave (include object 0x1F80 to configure extended slave features like automatically enter operational without NMTM module)
//#define COPMGR_FEAT_INCL_ADD_INFO     0x00000080L // a pointer to additional info can be set for each node by the application
//#define COPMGR_FEAT_INCL_COB          0x00000100L // includes functions for (un-)defining, receiving and sending application specific COBs

// non-standard features
   //#define COPMGR_FEAT_CONF_HEARTBEAT 0x80000000L // configure heartbeat/lifeguarding automatically

// =========================================================================
// CCM specific configurations
// =========================================================================

// CCM_MODULE_INTEGRATION defines all modules which are included in
// CANopen application. Please add or delete modules for your application.
#define CCM_MODULE_INTEGRATION          (0x000019FAL) /* mst */
//   CCM_MODULE_LSSS           0x00001  =  LSS slave
//   CCM_MODULE_LSSM           0x00002  =  LSS master
//   CCM_MODULE_NMTS           0x00004  =  NMT slave
// * CCM_MODULE_NMTM           0x00008  =  NMT master
// * CCM_MODULE_HBC            0x00010  =  heartbeat consumer module
// * CCM_MODULE_HBP            0x00020  =  heartbeat producer module
// * CCM_MODULE_EMCC           0x00040  =  emergency consumer module
// * CCM_MODULE_SDOC           0x00080  =  SDOC module
// * CCM_MODULE_PDO            0x00100  =  PDO module
//   CCM_MODULE_SRDO           0x00400  =  SRDO module (safety relevant data objects)
//   CCM_MODULE_DR303_3        0x00800  =  CiA DR303-3 (Indicator Specification)
//   CCM_MODULE_COPMGR         0x01000  =  CANopen Manager according to CiA 302
//   CCM_MODULE_SDOM           0x02000  =  SDO Manager module
//   CCM_MODULE_SRD            0x04000  =  SRD Client module
//   CCM_MODULE_CFGMA          0x08000  =  Configuration Manager
//   CCM_MODULE_CFGMASYS       0x10000  =  Configuration Manager with extended
//   CCM_MODULE_FLYMA          0x20000  =  Flying Master
//   CCM_MODULE_SDOGWY         0x40000  =  SDO Gateway
                      
// # Modules which are marked with '*' are included.
//   SDO server and emergency producer always exists.
                              
// disables or enables support0 of STORE/RESTORE activated with object 0x1010 and 0x1011
#define CCM_USE_STORE_RESTORE           FALSE
//#define CCM_USE_STORE_RESTORE           TRUE

// =========================================================================
// driver specific configurations
// =========================================================================

// -------------------------------------------------------------------------
// CAN driver
// -------------------------------------------------------------------------

// CDRV_MAX_INSTANCES defines count of instances of CAN driver.
// It should never be greater than COP_MAX_INSTANCES. If COP_MAX_INSTANCES
// (yes COP_MAX_INSTANCES) is greater than 1 the first parameter of
// functions of CAN driver is the instance number tCdrvInstance.
#define CDRV_MAX_INSTANCES     COP_MAX_INSTANCES

// Error state of CAN controller can be read in two different ways.
// If CDRV_USE_ERROR_ISR is set to FALSE Funktion CdrvGetStatus() reads
// the error state of CAN controller and returns it in tCdrvStatus.
// If CDRV_USE_ERROR_ISR is set to TRUE CAN Interrupt Service Routine reads
// the error state of CAN controller put it in an global variable and
// function CdrvGetStatus() returns it in tCdrvStatus.
// NOTE: Using CDRV_USE_ERROR_ISR with TRUE may occur much interrupts if
//      no CAN bus is connectid.
//#define CDRV_USE_ERROR_ISR          FALSE
#define CDRV_USE_ERROR_ISR          TRUE
// Here you can define, which CAN-Controller should be used. It is only
// neccessary to define, if you use only one instance of the CAN-driver.
// otherwise the define is ignored. 0 for CAN1 and 1 for CAN2. (Fujitsu only !!!)
#define CDRV_2294_USED_CAN_CONTROLLER        (1)

// This define is only used for FUULCAN-controller !!!
// If CDRV_USE_BASIC_CAN != FALSE, then one BASIC-CAN-Channel
// for receive is used
//#define CDRV_USE_BASIC_CAN              FALSE  // FullCAN
#define CDRV_USE_BASIC_CAN              TRUE  // BASICCAN

// Here you can define which CAN specification is used (CiA specification).
// Should 29 bit CAN identifiers be supported then use CAN20B. Otherwise
// use CAN20A. With CAN20A the CAN identifier is typed as 16 bit WORD. So
// you can keep back some data memory for future use.
#define CDRV_CAN_SPEC               CAN20A
//#define CDRV_CAN_SPEC               CAN20B

// If CDRV_USE_HIGHBUFF != FALSE high priority buffer will be implemented.
// With high priority buffer can important CAN messages be received or
// sent faster than other CAN messages. Important CAN messages should
// have kMtbHighPrio in m_MsgType of tCdrvMsg or in m_CobType of tCobParam.
// An application with few code and/or data memory can set this define
// to zero to get some memory for other things.
#define CDRV_USE_HIGHBUFF       FALSE
//#define CDRV_USE_HIGHBUFF       TRUE

// If CDRV_USE_IDVALID != FALSE driver will check CAN-ID before copying
// CAN message in buffer. If CAN-ID is not known for this application
// CAN message will not be copied.
#define CDRV_USE_IDVALID        FALSE
//#define CDRV_USE_IDVALID        TRUE

// CDRV_IDINFO_ALGO specifies IdInfo algorithm. CAN driver filters
// CAN messages by CAN-ID. For that there are some diferent algorithm
// Some of them are data memory intensive but faster and some of them
// use less data memory but are slower. You can use:
//   CDRV_IDINFO_ALGO_BITFIELD:  bit field algorithm (very fast)
//   CDRV_IDINFO_ALGO_IDLISTEXT: ID list with extended info (29 bit CAN-IDs)
//#define CDRV_IDINFO_ALGO        CDRV_IDINFO_ALGO_FULLCAN
#define CDRV_IDINFO_ALGO        CDRV_IDINFO_ALGO_BITFIELD
//#define CDRV_IDINFO_ALGO        CDRV_IDINFO_ALGO_IDLISTEXT

// CDRV_TIMESTAMP specifies if a timestamp is set in the message-struct of
// received CAN-messages. The timestamp has the format DWORD and is scaled
// in ms. The timestamp is relative.
//#define CDRV_TIMESTAMP          FALSE
#define CDRV_TIMESTAMP          TRUE

// For IdInfo algorithm == CDRV_IDINFO_ALGO_IDLISTEXT
// this defines the max count of list entries. For other algorithm this define
// is disregarded.
#define CDRV_IDINFO_ENTRIES     10


// =========================================================================
// COB specific defines
// =========================================================================

// If COB_MAX_RX_COB_ENTRIES, COB_MAX_TX_COB_ENTRIES and COB_MAX_RTR_COB_ENTRIES
// (and/or your own defines) are greater than 128 entries then set this define to
// TRUE. So COB handle will be typed to 16 bit WORD. Otherwise it is
// typed to 8 bit BYTE - data memory can be kept back for future use.
#define COB_MORE_THAN_128_ENTRIES         TRUE
#define COB_MORE_THAN_256_ENTRIES         TRUE


// =========================================================================
// CANopen specific defines
// =========================================================================

// =========================================================================
// OBD specific defines
// =========================================================================

// This define specifies maximum size of an object in object dectionary.
// If no  object is greater than   255 bytes then use OBD_OBJ_SIZE_SMALL.
// If no  object is greater than 65536 bytes then use OBD_OBJ_SIZE_MIDDLE.
// If any object is greater than 65536 bytes then use OBD_OBJ_SIZE_BIG.
//#define OBD_SUPPORTED_OBJ_SIZE          OBD_OBJ_SIZE_SMALL
//#define OBD_SUPPORTED_OBJ_SIZE          OBD_OBJ_SIZE_MIDDLE
#define OBD_SUPPORTED_OBJ_SIZE          OBD_OBJ_SIZE_BIG

// switch this define to TRUE if CANopen should compare object range
// automaticly
#define OBD_CHECK_OBJECT_RANGE          FALSE
//#define OBD_CHECK_OBJECT_RANGE          TRUE

// set this define to TRUE if there are strings or domains in OD, which
// may be changed in object size and/or object data pointer by its object
// callback function (called event kObdEvWrStringDomain)
#define OBD_USE_STRING_DOMAIN_IN_RAM    FALSE
//#define OBD_USE_STRING_DOMAIN_IN_RAM    TRUE


// =========================================================================
// LSS slave specific defines
// =========================================================================

// set this define to TRUE if CCM module should call LsssProcess() only after
// the next time to which the next slave node has to be checked
#define LSSS_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL
//#define LSSS_PROCESS_TIME_CONTROL        FUNCTION_CALL_TIME


// =========================================================================
// LSS master specific defines
// =========================================================================

// set this define to TRUE if CCM module should call LsssProcess() only after
// the next time to which the next slave node has to be checked
#define LSSM_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL
//#define LSSM_PROCESS_TIME_CONTROL        FUNCTION_CALL_TIME


// =========================================================================
// NMT slave specific defines
// =========================================================================

// set this define to TRUE if CCM module should call NmtsProcess() only after
// the next time to which the next slave node has to be checked
//#define NMTS_PROCESS_TIME_CONTROL        FUNCTION_CALL_TIME
#define NMTS_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL

// set this define to TRUE if the CANopen stack supports life guarding
//#define NMTS_USE_LIFEGUARDING           FALSE
#define NMTS_USE_LIFEGUARDING           TRUE

// =========================================================================
// NMT master specific defines
// =========================================================================

// set this define to TRUE if CCM module should call NmtmProcess() only after
// the next time to which the next slave node has to be checked
//#define NMTM_PROCESS_TIME_CONTROL        FUNCTION_CALL_TIME
#define NMTM_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL


// =========================================================================
// HBC specific defines
// =========================================================================

// for time control in process function of HBC submodule
//#define HBC_PROCESS_TIME_CONTROL        FUNCTION_CALL_TIME
#define HBC_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL


// =========================================================================
// HBP specific defines
// =========================================================================

// set this define to TRUE if CCM module should call HbpProcess() only after
// the next time to which the next slave node has to be checked
//#define HBP_PROCESS_TIME_CONTROL        FUNCTION_CALL_TIME
#define HBP_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL


// =========================================================================
// EMCP specific defines
// =========================================================================

// set this define to TRUE if object 0x1003 is supported
//#define EMCP_USE_PREDEF_ERROR_FIELD      FALSE
#define EMCP_USE_PREDEF_ERROR_FIELD      TRUE


// =========================================================================
// PDO specific defines
// =========================================================================

// Set this define to FUNCTION_NO_CONTROL if CCM module poll PdoProcess().
// Set this define to FUNCTION_CALL_TIME if CCM module should call PdoProcess()
// only after the next time to which the next PDO has to be checked.
// Set this define to FUNCTION_BREAK_TIME if PdoProcess() should break
// process after specified relative time.
#define PDO_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL
//#define PDO_PROCESS_TIME_CONTROL        FUNCTION_CALL_TIME
//#define PDO_PROCESS_TIME_CONTROL        FUNCTION_BREAK_TIME

// If PDO_GRANULARITY == 8  means bytemapping, you could mapp up to 8 variables
// in one PDO
// If PDO_GRANULARITY == 64 means bitmapping, you could mapp up to 64 variables
// in one PDO
#define PDO_GRANULARITY                 8
// at this time it is not possible to define PDO_GRANULARITY != 8 !!!

// disables or enables support of PDO event timer
//#define PDO_USE_EVENT_TIMER             FALSE
#define PDO_USE_EVENT_TIMER             TRUE

// disables or enables support of synchron PDO's
//#define PDO_USE_SYNC_PDOS               FALSE
#define PDO_USE_SYNC_PDOS               TRUE

// disables or enables support of remote PDO's
//#define PDO_USE_REMOTE_PDOS             FALSE
#define PDO_USE_REMOTE_PDOS             TRUE

// disables or enables support of generate sync messages 
//#define PDO_USE_SYNC_PRODUCER           FALSE
#define PDO_USE_SYNC_PRODUCER           TRUE


// disable or enable error callback funtion
#define PDO_USE_ERROR_CALLBACK          FALSE
//#define PDO_USE_ERROR_CALLBACK          TRUE

#define CCM_USE_PDO_CHECK_VAR           FALSE
//#define CCM_USE_PDO_CHECK_VAR           TRUE

// =========================================================================
// SDO server specific defines
// =========================================================================
// Set this define to FUNCTION_NO_CONTROL if CCM module poll SdosProcess().
// Set this define to FUNCTION_CALL_TIME if CCM module should call SdosProcess()
// only after the next time to which the next server has to be checked.
// Set this define to FUNCTION_BREAK_TIME if SdosProcess() should break
// process after specified relative time.
#define SDOS_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL
//#define SDOS_PROCESS_TIME_CONTROL        FUNCTION_CALL_TIME
//#define SDOS_PROCESS_TIME_CONTROL        FUNCTION_BREAK_TIME


// If this define is != FALSE, SDOS module supports SDO block transfere
// (see DS301 V4).
//#define SDO_BLOCKTRANSFER               FALSE
#define SDO_BLOCKTRANSFER               TRUE

// Block size must be a value between 0 < block size < 128.
#define SDO_BLOCKSIZE_DOWNLOAD          127
#define SDO_BLOCKSIZE_UPLOAD            127

// If this define is != FALSE, SDOS module supports SDO segmented transfer
// (see DS301 V4).
//#define SDO_SEGMENTTRANSFER             FALSE
#define SDO_SEGMENTTRANSFER             TRUE

// define the support for SDO CRC calculation (block transfer)
// SDO_CALCULATE_CRC = 0:  no CRC calculation supported
// SDO_CALCULATE_CRC = 1:  CRC calculation by lockup table (fast methode)
// SDO_CALCULATE_CRC = 2:  CRC calculation by formula (low code size)
#define SDO_CALCULATE_CRC               0

// timeout time in 100us for SDO transfer in SDO server
#define SDOS_DEFAULT_TIMEOUT            5000L

// =========================================================================
// SDO client specific defines
// =========================================================================

// set this define to TRUE if CCM module should call SdocProcess() only after
// the next time to which the next slave node has to be checked
#define SDOC_PROCESS_TIME_CONTROL        FUNCTION_NO_CONTROL
//#define SDOC_PROCESS_TIME_CONTROL        FUNCTION_CALL_TIME

//#define SDOC_USE_ADDITIONAL_API          FALSE
#define SDOC_USE_ADDITIONAL_API          TRUE

#endif //_COPCFG_H_



