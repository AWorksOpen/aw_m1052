/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.com

  Project:      CANopen V5

  Description:  Header file for CANopen API

  -------------------------------------------------------------------------

                $RCSfile: Cop.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.153 $  $Date: 2007/01/08 13:53:32 $

                $State: Exp $

                Build Environment:
                    all

  -------------------------------------------------------------------------

  Revision History:
  2003/12/11 f.j.:  neue Struktur tObdVStringDef definiert, die bei der
                        Anlage von Strings im RAM benoetigt wird, um die
                        Werte fuer die Zeiger auf die RAM Daten richtig
                        vorinitialisieren zu koennen

  2004/10/06 f.j.:  include configuration manager

  2004/11/15 a.s.:  Function CcmStoreCheckArchivState() added

  2004/12/07 r.d.:  Defines kObdPart... replaced from obd.h to cop.h.
  2004/12/07 r.d.:  Defines kObdEv...   replaced from obd.h to cop.h.
  2005/05/19 m.b.:  SRDO module included
  2005/12/13 k.t.:  Add SDO Manager and SRD Client Modul
  2005/12/22 k.t.:  take not needed structures to ccmmain.h
  2005/12/22 k.t.:  Add kSrdEventCopError in tSrdEvent

****************************************************************************/

#ifndef _COP_H_
#define _COP_H_


// ============================================================================
// include files
// ============================================================================
#ifdef WIN32

    #ifdef UNDER_RTSS
        // RTX header
        #include <windows.h>
        #include <process.h>
        #include <rtapi.h>

    #elif __BORLANDC__
        // borland C header
        #include <windows.h>
        #include <process.h>

    #elif WINCE
        #include <windows.h>

    #else
        // MSVC needs to include windows.h at first
        // the following defines ar necceray for function prototypes for waitable timers
        #define _WIN32_WINDOWS 0x0401
        #define _WIN32_WINNT   0x0400
        #include <windows.h>
        #include <process.h>
    #endif

#endif

#include "global_cop.h"     // global definitions

#ifdef PXROS

    #if (DEV_SYSTEM == _DEV_HIGHTEC_X86_)
        #include "pxdef.h"
        #include "pxpe.h"
    #else
        // PXROS include for MPC5x5 only
        #include "pxdef.h"
        #include "pxtrap.h"
        #include "pxpe.h"
    #endif
#endif // PXROS

#ifdef _C166 // Tasking C166_ST10 compiler

    // tasking wishes to include this headers at first
    #include <stdio.h>
    #include <assert.h>

#endif

#if (DEV_SYSTEM == _DEV_WIN32_)
    #pragma pack(push,1)    // structure with 1 byte alignment
#elif (DEV_SYSTEM == _DEV_GNU_MPC5X5_)
    #pragma pack(4)     // structure with 4 Byte Alignment
#endif


// definitions for DLL export
#if ((DEV_SYSTEM == _DEV_WIN32_) || (DEV_SYSTEM == _DEV_WIN_CE_)) && defined (COP_LIB)

    #define COPDLLEXPORT    __declspec (dllexport)

#else

    #define COPDLLEXPORT

#endif


// These constants defines modules which can be included in CANopen application.
// Use this constants for define CCM_MODULE_INTEGRATION in file CopCfg.h.
#define CCM_MODULE_LSSS         0x00000001L // LSS slave module
#define CCM_MODULE_LSSM         0x00000002L // LSS master module
#define CCM_MODULE_NMTS         0x00000004L // NMT slave module (must exist parallel to master !!!)
#define CCM_MODULE_NMTM         0x00000008L // NMT master module
#define CCM_MODULE_HBC          0x00000010L // Heartbeat consumer module
#define CCM_MODULE_HBP          0x00000020L // Heartbeat producer module
#define CCM_MODULE_EMCC         0x00000040L // Emergency consumer module
#define CCM_MODULE_SDOC         0x00000080L // SDO client module
#define CCM_MODULE_PDO          0x00000100L // PDO module
#define CCM_MODULE_SRDO         0x00000400L // SRDO module (safety relevant data objects)
#define CCM_MODULE_DR303_3      0x00000800L // CiA DR303-3 (Indicator Specification)
#define CCM_MODULE_COPMGR       0x00001000L // CANopen Manager according to CiA 302
#define CCM_MODULE_SDOM         0x00002000L // SDO Manager module
#define CCM_MODULE_SRD          0x00004000L // SRD Client module
#define CCM_MODULE_CFGMA        0x00008000L // Configuration Manager
#define CCM_MODULE_CFGMASYS     0x00010000L // Configuration Manager with extended
                                            // SYSTEC concise format
#define CCM_MODULE_FLYMA        0x00020000L // Flying Master
#define CCM_MODULE_SDOGWY       0x00040000L // SDO Gateway


// 13.05.2004 r.d.: standard CANopen does not work with "../include/..."
//#include "../include/copcfg.h"     // CANopen configuration file (configuration from application)
#if defined(AW_COM_CANOPEN_MST)
#include "cop_mst/copcfg.h"     // CANopen configuration file (configuration from application)
#elif defined(AW_COM_CANOPEN_SLV)
#include "cop_slv/copcfg.h"     // CANopen configuration file (configuration from application)
#else
#error "must define AW_COM_CANOPEN_MST or AW_COM_CANOPEN_SLV at project"
#endif
//#include "copcfg.h"   // CANopen configuration file (configuration from application)
#include "copdef.h"     // CANopen configuration file (default configuration)
#include "instdef.h"    // defines macros for instance types and table
#include "debug.h"      // debug definitions

#include "errordef.h"   // CANopen error codes for API funtions
#include "sdoac.h"      // CANopen abort codes for SDO
// needed, because Paradigm C used own target.h for Beck-Chip 1x3
#if (DEV_SYSTEM == _DEV_PAR_BECK1X3_)
  //  #include "../../../../include/target.h"   // target specific functions and definitions
#else
   // #include "target.h"     // target specific functions and definitions
#endif

#include "cdrvinc.h"    // interface files for CAN driver selection and access

#ifdef PXROS

    #include "tgtpxtsk.h"

#endif // PXROS


// ============================================================================
// types and prototypes for several modules
// ============================================================================

#define INFINITE_TIME                   -1


// ============================================================================
// COB specific defines, typedefs
// ============================================================================

#define CANID_INVALID                   0x80000000L // sign an COBID to invalid
#define CAN_FRAME_FORMAT_MASK           0x20000000L // Bit 29 = 0: 11Bit ID, 1 = 29BitID
#define CANID_SYNC_PRODUCER             0x40000000L


// ----------------------------------------------------------------------------
// This structure defines function pointers to CAN driver functions.
// Interface of these functions from all CAN drivers are the same
// with equal configurations.
typedef struct
{                                                   // $$$ Sollten unbenutzte Einträge nicht wegfallen?
    tCdrvInit               m_fpInit;               // used
    tCdrvAddInstance        m_fpAddInstance;        // used
    tCdrvDeleteInstance     m_fpDeleteInstance;     // used
    tCdrvSetBaudrate        m_fpSetBaudrate;        // used
    tCdrvReset              m_fpReset;              // used
    tCdrvDefineCanId        m_fpDefineCanId;        // used
    tCdrvUndefineCanId      m_fpUndefineCanId;      // used
    tCdrvWriteMsg           m_fpWriteMsg;           // used
    tCdrvReadMsg            m_fpReadMsg;            // used
    tCdrvReleaseMsg         m_fpReleaseMsg;         // used
    tCdrvUpdateMsg          m_fpUpdateMsg;          // used
    tCdrvInterruptHandler   m_fpInterruptHandler;   // n.u.
    tCdrvGetStatus          m_fpGetStatus;          // used
    tCdrvResetStatus        m_fpResetStatus;        // used
    tCdrvGetDiagnostic      m_fpGetDiagnostic;      // n.u.
    tCdrvClearRtr           m_fpClearRtr;           // used

} tCobCdrvFct;


// -------------------------------------------------------------------------
// type for index of COB entry table
// -------------------------------------------------------------------------
// Note:
// If there are less than 255 communication objects (COB) tCobEntryIndex should
// be defined to BYTE! Typedef should be dependant from a define (to be defined).
#if (COB_MORE_THAN_128_ENTRIES == FALSE)
    typedef BYTE tCobEntryIndex;
#else
    typedef WORD tCobEntryIndex;
#endif


// ============================================================================
// OBD specific defines, typedefs
// ============================================================================

// part of OD (bit oriented)
typedef tFastByte tObdPart;

#define kObdPartNo          0x00    // nothing
#define kObdPartGen         0x01    // generic part      (0x1000 - 0x1FFF)
#define kObdPartMan         0x02    // manufacturer part (0x2000 - 0x5FFF)
#define kObdPartDev         0x04    // device part       (0x6000 - 0x9FFF)
#define kObdPartUsr         0x08    // dynamic part e.g. for ICE61131-3

// combinations
#define kObdPartApp         (                            kObdPartDev              )   // application part (0x6000 - 0x9FFF) // 21.07.2006 r.d.: cahnged because definition in DS-301 (object 0x1010)
#define kObdPartApp2        (              kObdPartMan | kObdPartDev | kObdPartUsr)   // manufacturer and device part (0x2000 - 0x9FFF) and user OD
#define kObdPartAll         (kObdPartGen | kObdPartMan | kObdPartDev | kObdPartUsr)   // whole OD


//-----------------------------------------------------------------------------------------------------------
// events of object callback function
typedef tFastByte tObdEvent;
//                                                                                                      m_pArg points to
//                                                                                                    ---------------------
#define kObdEvCheckExist            0x06    // checking if object does exist (reading and writing)      NULL
#define kObdEvPreRead               0x00    // previous reading an object                               source data in OD
#define kObdEvPostRead              0x01    // after reading an object                                  destination data from caller
#define kObdEvWrStringDomain        0x07    // event for changing string/domain data pointer or size    struct tObdVStringDomain in RAM
#define kObdEvInitWrite             0x04    // initializes writing an object (checking object size)     size of object in OD
#define kObdEvPreWrite              0x02    // previos writing an object                                source data from caller
#define kObdEvPostWrite             0x03    // after eriting an object                                  destination data in OD
#define kObdEvAbortSdo              0x05    // after an abort of an SDO transfer                        ???


// -------------------------------------------------------------------------
// type for count of bytes in an object of object dictionary (OD)
// This type depends on application requirements.
// -------------------------------------------------------------------------

#define OBD_OBJ_SIZE_SMALL          0x00
#define OBD_OBJ_SIZE_MIDDLE         0x01
#define OBD_OBJ_SIZE_BIG            0x02

#if (OBD_SUPPORTED_OBJ_SIZE == OBD_OBJ_SIZE_SMALL)

    typedef BYTE  tObdSize; // All objects are smaller than 256 bytes.

#elif (OBD_SUPPORTED_OBJ_SIZE == OBD_OBJ_SIZE_MIDDLE)

    typedef WORD  tObdSize;   // All objects are smaller than 65535 Bytes.

#elif (OBD_SUPPORTED_OBJ_SIZE == OBD_OBJ_SIZE_BIG)

    typedef DWORD tObdSize; // For all objects as objects size are used an DWORD.

#else

    #error 'ERROR: unsupported value for object size (OBD_SUPPORTED_OBJ_SIZE)!'

#endif

// -------------------------------------------------------------------------
// types for data types defined in DS301
// -------------------------------------------------------------------------

// types of objects in object dictionary
// DS-301 defines these types as WORD
typedef WORD tObdType;

// types which are always supported
#define kObdTypBool         0x0001  // supported since V5.27 with PDO_USE_BIT_MAPPING = TRUE

#define kObdTypInt8         0x0002
#define kObdTypInt16        0x0003
#define kObdTypInt32        0x0004
#define kObdTypUInt8        0x0005
#define kObdTypUInt16       0x0006
#define kObdTypUInt32       0x0007
#define kObdTypReal32       0x0008
#define kObdTypVString      0x0009
#define kObdTypDomain       0x000F

#define kObdTypInt24        0x0010  // \ supported since V5.27 (always)
#define kObdTypUInt24       0x0016  // /

#define kObdTypReal64       0x0011  // \ .
#define kObdTypInt40        0x0012  // |
#define kObdTypInt48        0x0013  // |
#define kObdTypInt56        0x0014  // |
#define kObdTypInt64        0x0015  // > supported since V5.27 with define USE_VAR64
#define kObdTypUInt40       0x0018  // |
#define kObdTypUInt48       0x0019  // |
#define kObdTypUInt56       0x001A  // |
#define kObdTypUInt64       0x001B  // |
#define kObdTypTimeOfDay    0x000C  // |
#define kObdTypTimeDiff     0x000D  // /

// types which are not supported
#define kObdTypOString      0x000A
#define kObdTypUString      0x000B

// other types are not supported in this version


// -------------------------------------------------------------------------
// types for data types defined in DS301
// -------------------------------------------------------------------------

typedef unsigned char       tObdBoolean;      // 0001
typedef signed char         tObdInteger8;     // 0002
typedef signed short int    tObdInteger16;    // 0003
typedef signed long         tObdInteger32;    // 0004
typedef unsigned char       tObdUnsigned8;    // 0005
typedef unsigned short int  tObdUnsigned16;   // 0006
typedef unsigned long       tObdUnsigned32;   // 0007
typedef float               tObdReal32;       // 0008


typedef struct
{
   tObdSize         m_Size;
   char GENERIC*    m_pString;

} tObdVString;                          // 000D

#if (OBD_USE_STRING_DOMAIN_IN_RAM != FALSE)
// r.d.: if there are warnings in objdict.c then set this define in CopCfg.h to TRUE

    //f.j. default string in CONST
    typedef struct
    {
       tObdSize         m_Size;
       char GENERIC*    m_pDefString;
       char GENERIC*    m_pString;

    } tObdVStringDef;                       // ???

#endif

//r.d. parameter struct for changing object size and/or pointer to data of Strings or Domains
typedef struct
{
   tObdSize         m_DownloadSize;     // download size from SDO or APP
   tObdSize         m_ObjSize;          // current object size from OD - should be changed from callback function
   void GENERIC*    m_pData;            // current object ptr  from OD - should be changed from callback function

} tObdVStringDomain;                          // 000D

/*
typedef struct
{
   tObdSize         m_Size;
   char GENERIC*    m_pString;

} tObdOString;                          // 000C

typedef struct
{
   tObdSize              m_Size;
   short int GENERIC*    m_pString;

} tObdUString;                          // 000B
*/

//typedef ...           tObdBitString;    // 000E  ???
typedef unsigned char   tObdDomain;       // 000F
typedef signed   long   tObdInteger24;    // 0010
typedef unsigned long   tObdUnsigned24;   // 0016


#ifdef USE_VAR64

typedef signed QWORD    tObdInteger40;    // 0012
typedef signed QWORD    tObdInteger48;    // 0013
typedef signed QWORD    tObdInteger56;    // 0014
typedef signed QWORD    tObdInteger64;    // 0015

typedef unsigned QWORD  tObdUnsigned40;   // 0018
typedef unsigned QWORD  tObdUnsigned48;   // 0019
typedef unsigned QWORD  tObdUnsigned56;   // 001A
typedef unsigned QWORD  tObdUnsigned64;   // 001B

typedef double          tObdReal64;       // 0011

#ifndef _TIME_OF_DAY_DEFINED_
    #error 'ERROR: Declaration of tTimeOfDay in Global.h is missing!'
#endif

typedef tTimeOfDay tObdTimeOfDay;         // 000C
typedef tTimeOfDay tObdTimeDifference;    // 000D

#endif // USE_VAR64


// -------------------------------------------------------------------------
// structur for a variable in OD
// -------------------------------------------------------------------------

//-------------------------------------------------------------------------

typedef tCopKernel (PUBLIC ROM* tVarCallback) (CCM_DECL_INSTANCE_HDL_
    void GENERIC* pParam_p);

typedef struct
{
    void MEM*           m_pData;
    tObdSize            m_Size;

    #if (PDO_USE_STATIC_MAPPING == FALSE)
        tVarCallback    m_fpCallback;
        void GENERIC*   m_pArg;
    #endif

} tObdVarEntry;


// -------------------------------------------------------------------------
typedef tFastByte tVarParamValid;

#define kVarValidSize           0x01
#define kVarValidData           0x02
#define kVarValidCallback       0x04
#define kVarValidArg            0x08

#define kVarValidAll            0x0F


// -------------------------------------------------------------------------
// structur for defining a variable
// -------------------------------------------------------------------------

typedef struct
{
    BYTE            m_bValidFlag;   // values from tVarParamValid
    WORD            m_wIndex;
    BYTE            m_bSubindex;
    tObdSize        m_Size;
    void MEM*       m_pData;
    tVarCallback    m_fpCallback;
    void GENERIC*   m_pArg;

} tVarParam;


// ============================================================================
// PDO specific defines, typedefs
// ============================================================================

typedef tCopKernel (PUBLIC ROM* tPdoCallback) (CCM_DECL_INSTANCE_HDL_
    WORD wPdoCommuIndex_p);

typedef tFastByte tPdoParamType;

#define kPdoTypeSend    0
#define kPdoTypeRecv    1


// -------------------------------------------------------------------------
typedef tFastByte tPdoParamValid;

#define kPdoValidCanId          0x01
#define kPdoValidTxType         0x02
#define kPdoValidInihibitTime   0x04
#define kPdoValidEventTime      0x08
#define kPdoValidNrOfEntries    0x10
#define kPdoValidMappObjects    0x20
#define kPdoValidCallback       0x40

#define kPdoValidCommunication  0x0F    // = (kPdoValidCanId | kPdoValidTxType | kPdoValidInihibitTime | kPdoValidEventTime)
#define kPdoValidMapping        0x30    // = (kPdoValidNrOfEntries | kPdoValidMappObjects)
#define kPdoValidAll            0x7F    // = (kPdoValidCommunication | kPdoValidMapping | kPdoValidCallback)


// -------------------------------------------------------------------------
typedef struct
{
    BYTE            m_bValidFlag;                   // values from tPdoParamValid

    WORD            m_wPdoCommuIndex;               // Index of communication parameter
    BYTE            m_bPdoType;                     // values from tPdoParamType
    BYTE            m_bNodeID;
    DWORD           m_dwCanId;                      // CAN identifier like DS-301 V4
    BYTE            m_bTxTyp;                       // CANopen transmission type
    WORD            m_wInhibitTime;
    WORD            m_wEventTime;

    BYTE            m_bNrOfEntries;                 // Number of mapped objects
    DWORD           m_adwMappParam[PDO_MAX_MAPPENTRIES]; // mapping entries

    tPdoCallback    m_fpCallback;                   // functionpointer to a userspecific Callbackfunction

} tPdoParam;

// -------------------------------------------------------------------------
typedef struct
{
    WORD            m_wPdoCommuIndex;               // Index of communication parameter

    DWORD           m_dwCanId;                      // CAN identifier like DS-301 V4
    BYTE            m_bTxTyp;                       // CANopen transmission type
    WORD            m_wInhibitTime;
    WORD            m_wEventTime;

    BYTE            m_bSize;                        // Number of mapped bytes
    void MEM*       m_pVarData;                     // pointer to PDO variables

    tPdoCallback    m_fpCallback;                   // functionpointer to a userspecific Callbackfunction

} tPdoStaticParam;

typedef struct
{
    BYTE            m_bMPdoType;    // 0 = SAM; 1 = DAM; others undefined
    WORD            m_wIndex;
    BYTE            m_bSubindex;
    void GENERIC*   m_pData;
    tObdType        m_ObjType;

} tMPdoParam;


// -------------------------------------------------------------------------
// type for index of PDO entry table
// -------------------------------------------------------------------------
// Note:
// If there are less than 255 process data objects (PDO) tPdoCount should
// be defined to BYTE! Typedef should be dependant from a define .

//typedef BYTE tPdoIndex;
//#define PDO_INVALID_INDEX  0xFF
typedef WORD tPdoIndex;
#define PDO_INVALID_INDEX  0x0FFFF
/*
#if ((PDO_MAX_RXPDO_IN_OBD > 254) || (PDO_MAX_TXPDO_IN_OBD > 254))

    typedef WORD tPdoIndex;
    #define PDO_INVALID_INDEX  0x0FFFF

#else

    typedef BYTE tPdoIndex;
    #define PDO_INVALID_INDEX  0x0FF

#endif // ((PDO_MAX_RXPDO_IN_OBD > 254) || (PDO_MAX_TXPDO_IN_OBD > 254))
*/


// ============================================================================
// SDO server specific defines, typedefs
// ============================================================================

// ----------------------------------------------------------------------------
// struct for define an server
typedef tFastByte tSdosParamValid;

// This flags must casted to BYTE befor set to m_bValidFlags!
#define kSdosParamUnvalidFlags              0x00
#define kSdosParamValidFlagRxCANID          0x01    // set parameter m_dwRxCANID valid
#define kSdosParamValidFlagTxCANID          0x02    // set parameter m_dwTxCANID valid
#define kSdosParamValidFlagClientNodeId     0x04    // set parameter m_bClientNodeID valid
#define kSdosParamValidFlagTimeOut          0x08    // set parameter m_dwTimeOut valid
#define kSdosParamValidFlagCheck            0x80    // only parameter checking

#define kSdosParamValidAll                  (   kSdosParamValidFlagRxCANID      | \
                                                kSdosParamValidFlagTxCANID      | \
                                                kSdosParamValidFlagClientNodeId | \
                                                kSdosParamValidFlagTimeOut          )


// ----------------------------------------------------------------------------
typedef struct
{
    BYTE       m_bValidFlags;    // bit mask to set parameter valid (from tSdosParamValid)
    BYTE       m_bClientNodeID;  // NodeID of Client
    WORD       m_wObjIndex;      // index of server in OD
    DWORD      m_dwRxCANID;      // CAN-ID for Rx-COB
    DWORD      m_dwTxCANID;      // CAN-ID for Tx-COB
    DWORD      m_dwTimeOut;      // SDO-Timeout

} tSdosParam;



// ============================================================================
// SDO client specific defines, typedefs
// ============================================================================

//-------------------------------------------------------------------------
// define sdo client states for API functions
//-------------------------------------------------------------------------

typedef tFastByte tSdocState;

#define kSdocTransferNotActive      0
#define kSdocTransferRunning        1
#define kSdocTransferTxAborted      2
#define kSdocTransferRxAborted      3
#define kSdocTransferFinished       4


//-------------------------------------------------------------------------
// enum with SDO transfer types for function SdocInitTransfer()
//-------------------------------------------------------------------------

typedef tFastByte tSdocType;

#define kSdocTypeSegDown            0
#define kSdocTypeSegUp              1
#define kSdocTypeBlockDown          2
#define kSdocTypeBlockUp            3


//-------------------------------------------------------------------------
// this enum is used for struct tSdocParam to fill the struct element
// m_bValidFlags with defined values
//-------------------------------------------------------------------------

typedef tFastByte tSdocParamValid;

// This flags must casted to BYTE befor set to m_bValidFlags!
#define kSdocUnvalidFlags           0x00
#define kSdocValidFlagRxCanId       0x01    // set parameter m_RxCanId valid
#define kSdocValidFlagTxCanId       0x02    // set parameter m_TxCanId valid
#define kSdocValidFlagServerNodeId  0x04    // set parameter m_bServerNodeID valid
#define kSdocValidFlagTimeOut       0x08    // set parameter m_TimeOut valid
#define kSdocValidFlagCheck         0x80    // only parameter checking

#define kSdocValidAllParams         (   kSdocValidFlagRxCanId      | \
                                        kSdocValidFlagTxCanId      | \
                                        kSdocValidFlagServerNodeId | \
                                        kSdocValidFlagTimeOut       )


//-------------------------------------------------------------------------
// struct to define a SDO client
//-------------------------------------------------------------------------

typedef struct
{
    BYTE    m_bValidFlags;
    tTime   m_TimeOut;

    WORD    m_wClientIndex;
    DWORD   m_dwRxCanId;
    DWORD   m_dwTxCanId;
    BYTE    m_bServerNodeId;

} tSdocParam;


//-------------------------------------------------------------------------
// struct with parameters for callback function CbFinished()
//-------------------------------------------------------------------------

typedef struct
{
    WORD            m_wClientIndex;
    tSdocState      m_SdocState;
    DWORD           m_dwAbortCode;

} tSdocCbFinishParam;



//-------------------------------------------------------------------------
// type of callback function which is called by sdo transfer end
// (as a result of transfer error or successful transfer)
//-------------------------------------------------------------------------

typedef void (PUBLIC ROM* tSdocCbFinished) ( CCM_DECL_INSTANCE_HDL_
                                             tSdocCbFinishParam MEM* pParamList_p );


//-------------------------------------------------------------------------
// struct with parameters for initialize SDO transfer (SdocInitTransfer())
//-------------------------------------------------------------------------

typedef struct
{
    WORD            m_wClientIndex;
    WORD            m_wObjIndex;
    BYTE            m_bObjSubIndex;
    void GENERIC*   m_pData;
    tObdSize        m_DataLen;
    tTime           m_TimeOut;
    tSdocType       m_SdocType;
    tSdocCbFinished m_pfnCbFinished;

} tSdocTransferParam;



// ============================================================================
// types and prototypes for NMT module
// ============================================================================


// ----------------------------------------------------------------------------
typedef tFastByte tNmtEvent;

// NMT Module Control
#define kNmtEvEnterInitialising         0x01
#define kNmtEvResetNode                 0x02
#define kNmtEvPreResetCommunication     0x03    // new substate for initialisation of CANopen specific parts
#define kNmtEvResetCommunication        0x04
#define kNmtEvPostResetCommunication    0x05    // new substate for bootup message

#define kNmtEvEnterPreOperational       0x10
#define kNmtEvEnterOperational          0x20
#define kNmtEvEnterStopped              0x30


// ----------------------------------------------------------------------------
// NMT error controll events
typedef tFastByte tNmtErrCtrlEvent;

#define kNmtErrCtrlEvIdle                   0x00

// life guarding slave
#define kNmtErrCtrlEvLgConnected            0x10
#define kNmtErrCtrlEvLgMsgLost              0x11
#define kNmtErrCtrlEvLgLostConnection       0x12

// generic events for master
#define kNmtErrCtrlEvBootupReceived         0x20

// life guarding master
#define kNmtErrCtrlEvLgNodeStateChanged     0x31
#define kNmtErrCtrlEvLgToggleError          0x32
#define kNmtErrCtrlEvLgSuspended            0x33
#define kNmtErrCtrlEvLgNoAnswer             0x34

// heartbeat consumer
#define kNmtErrCtrlEvHbcConnected           0x40
#define kNmtErrCtrlEvHbcConnectionLost      0x41
#define kNmtErrCtrlEvHbcNodeStateChanged    0x42

#define kNmtErrCtrlEvUnknown                0xFF



// ----------------------------------------------------------------------------
//typedef tFastByte tNmtCommand;
typedef WORD tNmtCommand;

#define kNmtCommInitialize              0x0000
#define kNmtCommInitBeforePost          0x0101
#define kNmtCommStartRemoteNode         0x0001
#define kNmtCommEnterOperational        0x0001
#define kNmtCommStopRemoteNode          0x0002
#define kNmtCommEnterStopped            0x0002
#define kNmtCommEnterPostPreOperational 0x0180
#define kNmtCommEnterPreOperational     0x0080
#define kNmtCommResetNode               0x0081
#define kNmtCommResetCommunication      0x0082


// ============================================================================
// types for NMT master module
// ============================================================================

typedef struct
{
    WORD        m_wLifeguardTime;       // base time is 1 ms
    BYTE        m_bLifeguardFactor;

} tNmtmSlaveParam;


// ============================================================================
// types for HBC module
// ============================================================================

typedef struct
{
    WORD  wHeartbeatTime_p;
    BYTE  bProducerNodeId_p;
    BYTE  bSubindex_p;

} tHbcProdParam;


// ============================================================================
// types for EMCP module
// ============================================================================

typedef struct
{
    WORD    m_wEmcyErrorCode;
    BYTE    m_bErrorRegister;
    BYTE    m_abManufactErrorCode[5];
    WORD    m_wAdditionalInfo;

} tEmcParam;


// ============================================================================
// types for windows threads
// ============================================================================

typedef tFastByte tThrdPriority;

#define kThrdPriorityNormal         0
#define kThrdPriorityHighest        1
#define kThrdPriorityTimeCritical   2

// ============================================================================
// specific defines and typedefs for Indicator Specification (DR303-3)
// ============================================================================

typedef tFastByte tDr303RunState;
typedef tFastByte tDr303ErrorState;

#define kDr303RunReset                          0
#define kDr303RunLssProcess                     1
#define kDr303RunDeviceStopped                  2
#define kDr303RunDevicePreOperational           3
#define kDr303RunDeviceOperational              4

#define kDr303NoError                           1
#define kDr303ErrorWarningLimit                 2
#define kDr303ErrorLssProcess                   3
#define kDr303ErrorControlEvent                 4
#define kDr303ErrorSyncTimeout                  5
#define kDr303ErrorBusoff                       6


// ============================================================================
// SRDO specific defines, typedefs
// ============================================================================

typedef BYTE tSrdoIndex;

typedef struct
{
    BYTE            m_bDirection;       // direction and validity of SRDO (0-invalid; 1-Tx; 2-Rx)
    WORD            m_wSct;             // refresh time SCT
//    BYTE            m_bSrvt;            // validation time SRVT
//    DWORD           m_dwCobId1;         // COB-ID normal data
//    DWORD           m_dwCobId2;         // COB-ID invers data
//    WORD            m_wCheckSum;        // CRC of SRDO

} tSrdoCommuParam;


typedef struct
{
    BYTE            m_bNoOfMappedObjects;                       // Number of mapped objects
    void MEM*       m_apMappedVariable[SRDO_MAX_MAPPENTRIES];   // array of pointers to the mapped variables

} tSrdoMappParam;

typedef tFastByte tProgMonEvent;

#define kSrdoPMEvSctChecked                     0x01
#define kSrdoPMEvSctNotCheckedItIsTx            0x02
#define kSrdoPMEvSctNotCheckedItIsInvalid       0x03
#define kSrdoPMEvSctNotCheckedNotOperational    0x04
#define kSrdoPMEvSrdoError                      0x05
#define kSrdoPMEvSrdoReceived                   0x06
#define kSrdoPMEvSrdoTransmitted                0x07


// ============================================================================
// LSS specific defines, typedefs
// ============================================================================

#define LSS_INVALID_NODEID              0xFF

#define kLssModeOperation               0
#define kLssModeConfiguration           1
#define kLssModeSelective               2

typedef tFastByte tLssMode;

typedef struct
{
    DWORD   m_dwVendorId;               // equal to object 0x1018 / 1
    DWORD   m_dwProductCode;            // equal to object 0x1018 / 2
    DWORD   m_dwRevisionNr;             // equal to object 0x1018 / 3
    DWORD   m_dwSerialNr;               // equal to object 0x1018 / 4

} tLssAddress;

// LSS slave -------------
#define kLsssEvEnterConfiguration       0x00  // switching to configuration mode
#define kLsssEvConfigureNodeId          0x01  // node ID will be configured
#define kLsssEvConfigureBitTiming       0x02
#define kLsssEvActivateBitTiming        0x03
#define kLsssEvSaveConfiguration        0x04
#define kLsssEvDeactivateBusContact     0x05
#define kLsssEvActivateBusContact       0x06
#define kLsssEvEnterOperation           0x10
#define kLsssEvPreResetNode             0x11

typedef tFastByte tLssEvent;

// callback parameters
typedef struct
{
    tLssEvent   m_bLssEvent;            // values from tLssEvent
    BYTE        m_bNodeId;
    BYTE        m_bBtrTableSelector;
    BYTE        m_bBtrIndex;

} tLssCbParam;

// LSS master -------------
#define kLssmEvModeSelective            0x01
#define kLssmEvDeactivateBusContact     0x10
#define kLssmEvActivateBitTiming        0x11
#define kLssmEvActivateBusContact       0x12
#define kLssmEvResult                   0x20
#define kLssmEvIdentifyAnySlave         0x30
#define kLssmEvInquireData              0x40
#define kLssmEvTimeout                  0xFF

typedef tFastByte tLssmEvent;

typedef struct
{
    DWORD   m_dwVendorId;
    DWORD   m_dwProductCode;
    DWORD   m_dwRevisionNrLow;
    DWORD   m_dwRevisionNrHigh;
    DWORD   m_dwSerialNrLow;
    DWORD   m_dwSerialNrHigh;

} tLssmIdentifyParam;

typedef struct
{
    BYTE m_bLssErrorCode;
    BYTE m_bSpecErrorCode;

} tLssmResult;

typedef struct
{
    BYTE m_bTableSelector;
    BYTE m_bTableIndex;

} tLssmBitTiming;

#define kLssmCmdInquireVendorId         0x00
#define kLssmCmdInquireProductCode      0x01
#define kLssmCmdInquireRevisionNr       0x02
#define kLssmCmdInquireSerialNr         0x03
#define kLssmCmdInquireNodeId           0x04

#define CCM_CONVERT_LSSCMD_TO_LSSFLAG(cmd)  (1<<(cmd))
#define CCM_CHECK_LSSFLAG(flg,cmd)          ((flg) & (1<<(cmd))) != 0

#define CCM_LSSFLAGS_SLAVE_ADDRESS      (CCM_CONVERT_LSSCMD_TO_LSSFLAG (kLssmCmdInquireVendorId)    | \
                                         CCM_CONVERT_LSSCMD_TO_LSSFLAG (kLssmCmdInquireProductCode) | \
                                         CCM_CONVERT_LSSCMD_TO_LSSFLAG (kLssmCmdInquireRevisionNr)  | \
                                         CCM_CONVERT_LSSCMD_TO_LSSFLAG (kLssmCmdInquireSerialNr)       )
#define CCM_LSSFLAGS_ALL                (CCM_LSSFLAGS_SLAVE_ADDRESS                                 | \
                                         CCM_CONVERT_LSSCMD_TO_LSSFLAG (kLssmCmdInquireNodeId)         )


// ============================================================================
// API types and prototypes (CCM module)
// ============================================================================

// ----------------------------------------------------------------------------
// defines
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// events for PxROS application
// ----------------------------------------------------------------------------

#ifdef PXROS

    // only for operating system PxROS:
    // These constants defines events for PxROS tasks which signal specific
    // states of CANopen. Use these constants with calling PxROS-function
    // PxAwaitEvents() in task which calls CcmInitCANOpen().
    #define PX_EVENT_VALID_NODEID   0x00000001L // valid NodeId was configured during LSS process
                                                // NMT state machine starts now
                                                // this is only available, when LSS slave is configured
    #define PX_EVENT_NMT_STARTED    0x00000002L // NMT state machine is started now
                                                // CANopen porcess is running now

    #define PX_EVENT_NMT_FAILED     0x00000004L // NMT state machine could not be started
                                                // CANopen porcess is not running

    // events from 0x00000004 to 0x00008000 are reserved by CANopen
    // events from 0x00010000 to 0x80000000 can be defined for application
    // #define PX_EVENT_CHANGE_VAR     0x00010000L // e.g. timer event to change a variable

#endif // PXROS

#ifdef WIN32

    // only for operating system Windows:
    // These constants defines events for Windows task which signal specific
    // states of CANopen. Use these constants with calling Windows-function
    // WaitforMultipleObjekts() in task which calls CcmInitCANOpen().
    #define WIN_EVENT_VALID_NODEID  0x00000001L // valid NodeId was configured during LSS process
                                                // NMT state machine starts now
                                                // this is only available, when LSS slave is configured
    #define WIN_EVENT_NMT_STARTED   0x00000002L // NMT state machine is started now
                                                // CANopen porcess is running now

    #define WIN_EVENT_NMT_FAILED    0x00000004L // NMT state machine could not be started
                                                // CANopen porcess is not running
    // events from 0x00000004 to 0x00008000 are reserved by CANopen
    // events from 0x00010000 to 0x80000000 can be defined for application
    // #define WIN_EVENT_CHANGE_VAR     0x00010000L // e.g. timer event to change a variable

#endif //WIN32


// ----------------------------------------------------------------------------
// events for Windows event callback function
// ----------------------------------------------------------------------------

#if defined (COP_WINDOWS_DLL)

    #define COP_WINEVENT_FIRST_BUS_CONTACT  0x0000
    #define COP_WINEVENT_LSS_VALIDID        0x0001
    #define COP_WINEVENT_NMT_STARTED        0x0002
    #define COP_WINEVENT_LIFEGUARD_MST      0x0100
    #define COP_WINEVENT_LIFEGUARD_SLV      0x0101
    #define COP_WINEVENT_HEARTBEAT          0x0102
    #define COP_WINEVENT_EMERGENCY_RECV     0x0103
    #define COP_WINEVENT_SYNC_RECV          0x0104
    #define COP_WINEVENT_ERROR              0xFFFF

    typedef tFastByte tCcmCopType;

    #define kCcmMaster      1
    #define kCcmSlave       2

#endif // defined (COP_WINDOWS_DLL)


// ----------------------------------------------------------------------------
// important types for callback functions
// ----------------------------------------------------------------------------

typedef tCopKernel (PUBLIC ROM* tObdInitRam) (void MEM* pInitParam_p);

typedef tCopKernel (PUBLIC ROM* tCcmNmtEventCallback) (CCM_DECL_INSTANCE_HDL_
    tNmtEvent NmtEvent_p);

typedef tCopKernel (PUBLIC ROM* tCcmWinEventCallback) (CCM_DECL_INSTANCE_HDL_
    DWORD dwEvent_p,
    DWORD dwParam1_p, DWORD dwParam2_p, DWORD dwParam3_p, DWORD dwParam4_p);


// ----------------------------------------------------------------------------
// structure to initialize CANopen with CCM module
// ----------------------------------------------------------------------------
typedef struct
{
    #if ((COP_USE_CDRV_FUNCTION_POINTER != FALSE) && !defined (COP_WINDOWS_DLL))

        tCobCdrvFct GENERIC* m_pCdrvFct;

    #endif

    // ------------------------------------------------
    // node and instance specific parameters
    BYTE                    m_bInitNodeId;          // node id
    tCdrvBaudIndex          m_BaudIndex;            // baudrate as index
//  CONST WORD ROM*         m_pBdiTable;            // address of baudrate table
    CONST void ROM*         m_pBdiTable;            // address of baudrate table
    WORD                    m_wSizeOfBdiTab;
    DWORD                   m_dwAmr;                // acceptance filter
    DWORD                   m_dwAcr;
    tCdrvHwParam            m_HwParam;              // settings like base address, interrupt, ...
    tCdrvEnableInt          m_fpCanInterruptEnable; // function pointer to enable/disable CAN interrupt
    tCcmNmtEventCallback    m_fpNmtEventCallback;
    tObdInitRam             m_fpObdInitRam;

    #if defined (COP_WINDOWS_DLL)

        tCcmCopType             m_CopType;
        tCcmWinEventCallback    m_fpWinEventCallback;

    #endif

    #ifdef PXROS

        PxTask_t            m_StdTaskId;
        PxUChar_t           m_CopTaskPrio;

    #endif // PXROS

    #if (defined (WIN32) && !defined (COP_WINDOWS_DLL))

//        tVxDType            m_VxDType;
//        BYTE                m_bThrdPriority;
        HANDLE              m_hValidNodeId;
        HANDLE              m_ahNmtEventHandle[2];
//        HANDLE              m_hTransmitCanMsg;

    #endif

    #ifdef LINUX_SYSTEM

        BYTE                m_bDevNumber;   // device number for selecting CAN controller
        char*               m_pszDevName;   // optional: device name (e.g. "/dev/can0")
        tCdrvLinFlags       m_LinDevFlags;  // Linux specific device flags

    #endif

} tCcmInitParam;


// ----------------------------------------------------------------------------
// important enums
// ----------------------------------------------------------------------------

// This enum is used to call CcmInitCANopen(). The parameter FirstCall_p decides if function should delete
// whole instance table or not. If COP_MAX_INSTANCES = 1 then this parameter will be ignored.
typedef tFastByte tCcmInitCall;

// standard constants
#define kCcmAdditionalInstance      0x00        // adds a new instance for all CANopen modules
#define kCcmFirstInstance           0x01        // initializes the first instance of CANopen

// reserved constants for special appliactions
#define kCcmAddCcmInstFirstCdrvInst 0x02        // adds a new instance for CANopen stack but the first
                                                // instance for CAN driver
#define kCcmAddCcmInstAddCdrvInst   0x03        // the same like kCcmAdditionalInstance


typedef tCopKernel (PUBLIC ROM* tCcmErrorCallback) (CCM_DECL_INSTANCE_HDL_
    void MEM* pArg_p, tCopKernel ErrorCode_p);


// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// macros to separate version number from function CopGetStackVersion()
#define COP_GET_MAJOR_VERSION(dw)        ((BYTE) dw)
#define COP_GET_MINOR_VERSION(dw)       (((WORD) dw) >> 8)
#define COP_GET_RELEASE_VERSION(dw)      (       dw  >> 16)


// ============================================================================
// typedef for SDO Manager Module
// ============================================================================
typedef struct
{
    BYTE    m_bSizeOf;      // for Future extention of the structur
                            // size -> version
    tTime   m_SdocTimeout;  // timeout for SDO transfer during SRD Client scan
                            // in 100 µs

} tSdomInit;


// ============================================================================
// typedef and Eventcallback- function prototype for SRD Client Module
// ============================================================================
// Events for signaling to application
typedef enum
{
    kSrdEventRegister           =   0x01,   // SRD Client registered by SDO Manager
    kSrdEventConnected          =   0x02,   // connect to Slave established
    kSrdEventReleased           =   0x03,   // connect to Slave released
    kSrdEventErrorRegister      =   0x04,   // Error occur during register
                                            // -> information from SDO Manager
    kSrdEventAllDefaultSdos     =   0x05,   // all default SDO Server received
    kSrdEventCopErrorRegister   =   0x06,   // Error in local Cop function
                                            // during register occur
    kSrdEventErrorConnect       =   0x07,   // Error occur during connect request
                                            // -> information from SDO Manager
    kSrdEventCopErrorConnect    =   0x08,   //  Error in local Cop function
                                            // during connect request occur
    kSrdEventErrorDefCon        =   0x09,   // Error occur during default connect
                                            // request
                                            // -> information from SDO Manager
    kSrdEventCopErrorDefCon     =   0x0A,   //  Error in local Cop function
                                            // during default connect request
                                            // occur
    kSrdEventCopErrorRelease    =   0x0B    //  Error in local Cop function
                                            // during release connection
                                            // occur
} tSrdEvent;

// typedef for callback- function for eventsignaling
typedef tCopKernel (PUBLIC ROM* tSrdEventCallback) (CCM_DECL_INSTANCE_HDL_
    tSrdEvent SrdEvent_p,   // events of the type tSrdEvent from SRD Client
    void MEM* pArg_p);  // additional parameter (tbd. e.g. Error Code)

// struct for initialisation of SRD Client
typedef struct
{
    BYTE    m_bSizeOf;      // for Future extention of the structur
                            // size -> version
    BYTE    m_bRetryCount;  // Retry-Count for connection to SDO Manager
    tTime   m_wSrdTimeOut;  // Timeout-Count for connection to
                            // SDO Manager in 100 µs
} tSrdInit;

// argument for callback function for event kSrdEventConnected and kSrdEventReleased
typedef struct
{
    WORD    m_wClientIndex;
    BYTE    m_bSlaveNodeId;

} tSrdEventOkArg;

// struct for argument of callback- function in errorcase
typedef struct
{
    union
    {
        tCopKernel  m_CopErrorCode; // general Cop stack errors, kCopSrdNoAnswerSdom or kCopSrdSdomNotUndefSdoc
        BYTE        m_SdomError;    // error codes according to CiA 302 part 4 Object 0x1F10
                                    // 00h = General error
                                    // 01h = No SDO channel free (SRD -> SDO manager)
                                    // 02h = No SDO channel free/available (in network)
                                    // 03h = No SDO channel free/available (SRD -> S)
                                    // 04h = S not available
    } m_ErrorCode;
    WORD    m_wClientIndex;
    BYTE    m_bSlaveNodeId;

} tSrdEventErrorArg;


// ============================================================================
// common debug macros
// ============================================================================
// for using macro DEBUG_TRACEx()
//
// Example:
//      DEBUG_TRACE1 (COP_DBGLVL_OBD, "Value is %d\n" , wObjectIndex);
//
// This message only will be printed if:
//      - NDEBUG is not defined     AND !!!
//      - flag 0x00000004L is set in DEF_DEBUG_LVL (can be defined in copcfg.h)
//
// default level is defined in copdef.h

// debug-level and TRACE-macros         // standard-level   // flags for DEF_DEBUG_LVL
#define COP_DBGLVL_CDRV                 DEBUG_LVL_01        // 0x00000001L
#define COP_DBGLVL_CDRV_TRACE0          DEBUG_LVL_01_TRACE0
#define COP_DBGLVL_CDRV_TRACE1          DEBUG_LVL_01_TRACE1
#define COP_DBGLVL_CDRV_TRACE2          DEBUG_LVL_01_TRACE2
#define COP_DBGLVL_CDRV_TRACE3          DEBUG_LVL_01_TRACE3
#define COP_DBGLVL_CDRV_TRACE4          DEBUG_LVL_01_TRACE4

#define COP_DBGLVL_COB                  DEBUG_LVL_02        // 0x00000002L
#define COP_DBGLVL_COB_TRACE0           DEBUG_LVL_02_TRACE0
#define COP_DBGLVL_COB_TRACE1           DEBUG_LVL_02_TRACE1
#define COP_DBGLVL_COB_TRACE2           DEBUG_LVL_02_TRACE2
#define COP_DBGLVL_COB_TRACE3           DEBUG_LVL_02_TRACE3
#define COP_DBGLVL_COB_TRACE4           DEBUG_LVL_02_TRACE4

#define COP_DBGLVL_OBD                  DEBUG_LVL_03        // 0x00000004L
#define COP_DBGLVL_OBD_TRACE0           DEBUG_LVL_03_TRACE0
#define COP_DBGLVL_OBD_TRACE1           DEBUG_LVL_03_TRACE1
#define COP_DBGLVL_OBD_TRACE2           DEBUG_LVL_03_TRACE2
#define COP_DBGLVL_OBD_TRACE3           DEBUG_LVL_03_TRACE3
#define COP_DBGLVL_OBD_TRACE4           DEBUG_LVL_03_TRACE4

#define COP_DBGLVL_NMT                  DEBUG_LVL_04        // 0x00000008L
#define COP_DBGLVL_NMT_TRACE0           DEBUG_LVL_04_TRACE0
#define COP_DBGLVL_NMT_TRACE1           DEBUG_LVL_04_TRACE1
#define COP_DBGLVL_NMT_TRACE2           DEBUG_LVL_04_TRACE2
#define COP_DBGLVL_NMT_TRACE3           DEBUG_LVL_04_TRACE3
#define COP_DBGLVL_NMT_TRACE4           DEBUG_LVL_04_TRACE4

#define COP_DBGLVL_NMTS                 DEBUG_LVL_05        // 0x00000010L
#define COP_DBGLVL_NMTS_TRACE0          DEBUG_LVL_05_TRACE0
#define COP_DBGLVL_NMTS_TRACE1          DEBUG_LVL_05_TRACE1
#define COP_DBGLVL_NMTS_TRACE2          DEBUG_LVL_05_TRACE2
#define COP_DBGLVL_NMTS_TRACE3          DEBUG_LVL_05_TRACE3
#define COP_DBGLVL_NMTS_TRACE4          DEBUG_LVL_05_TRACE4

#define COP_DBGLVL_NMTM                 DEBUG_LVL_06        // 0x00000020L
#define COP_DBGLVL_NMTM_TRACE0          DEBUG_LVL_06_TRACE0
#define COP_DBGLVL_NMTM_TRACE1          DEBUG_LVL_06_TRACE1
#define COP_DBGLVL_NMTM_TRACE2          DEBUG_LVL_06_TRACE2
#define COP_DBGLVL_NMTM_TRACE3          DEBUG_LVL_06_TRACE3
#define COP_DBGLVL_NMTM_TRACE4          DEBUG_LVL_06_TRACE4

#define COP_DBGLVL_HBP                  DEBUG_LVL_07        // 0x00000040L
#define COP_DBGLVL_HBP_TRACE0           DEBUG_LVL_07_TRACE0
#define COP_DBGLVL_HBP_TRACE1           DEBUG_LVL_07_TRACE1
#define COP_DBGLVL_HBP_TRACE2           DEBUG_LVL_07_TRACE2
#define COP_DBGLVL_HBP_TRACE3           DEBUG_LVL_07_TRACE3
#define COP_DBGLVL_HBP_TRACE4           DEBUG_LVL_07_TRACE4

#define COP_DBGLVL_HBC                  DEBUG_LVL_08        // 0x00000080L
#define COP_DBGLVL_HBC_TRACE0           DEBUG_LVL_08_TRACE0
#define COP_DBGLVL_HBC_TRACE1           DEBUG_LVL_08_TRACE1
#define COP_DBGLVL_HBC_TRACE2           DEBUG_LVL_08_TRACE2
#define COP_DBGLVL_HBC_TRACE3           DEBUG_LVL_08_TRACE3
#define COP_DBGLVL_HBC_TRACE4           DEBUG_LVL_08_TRACE4

#define COP_DBGLVL_EMCP                 DEBUG_LVL_09        // 0x00000100L
#define COP_DBGLVL_EMCP_TRACE0          DEBUG_LVL_09_TRACE0
#define COP_DBGLVL_EMCP_TRACE1          DEBUG_LVL_09_TRACE1
#define COP_DBGLVL_EMCP_TRACE2          DEBUG_LVL_09_TRACE2
#define COP_DBGLVL_EMCP_TRACE3          DEBUG_LVL_09_TRACE3
#define COP_DBGLVL_EMCP_TRACE4          DEBUG_LVL_09_TRACE4

#define COP_DBGLVL_EMCC                 DEBUG_LVL_10        // 0x00000200L
#define COP_DBGLVL_EMCC_TRACE0          DEBUG_LVL_10_TRACE0
#define COP_DBGLVL_EMCC_TRACE1          DEBUG_LVL_10_TRACE1
#define COP_DBGLVL_EMCC_TRACE2          DEBUG_LVL_10_TRACE2
#define COP_DBGLVL_EMCC_TRACE3          DEBUG_LVL_10_TRACE3
#define COP_DBGLVL_EMCC_TRACE4          DEBUG_LVL_10_TRACE4

#define COP_DBGLVL_PDO                  DEBUG_LVL_11        // 0x00000400L
#define COP_DBGLVL_PDO_TRACE0           DEBUG_LVL_11_TRACE0
#define COP_DBGLVL_PDO_TRACE1           DEBUG_LVL_11_TRACE1
#define COP_DBGLVL_PDO_TRACE2           DEBUG_LVL_11_TRACE2
#define COP_DBGLVL_PDO_TRACE3           DEBUG_LVL_11_TRACE3
#define COP_DBGLVL_PDO_TRACE4           DEBUG_LVL_11_TRACE4

#define COP_DBGLVL_SDOS                 DEBUG_LVL_12        // 0x00000800L
#define COP_DBGLVL_SDOS_TRACE0          DEBUG_LVL_12_TRACE0
#define COP_DBGLVL_SDOS_TRACE1          DEBUG_LVL_12_TRACE1
#define COP_DBGLVL_SDOS_TRACE2          DEBUG_LVL_12_TRACE2
#define COP_DBGLVL_SDOS_TRACE3          DEBUG_LVL_12_TRACE3
#define COP_DBGLVL_SDOS_TRACE4          DEBUG_LVL_12_TRACE4

#define COP_DBGLVL_SDOC                 DEBUG_LVL_13        // 0x00001000L
#define COP_DBGLVL_SDOC_TRACE0          DEBUG_LVL_13_TRACE0
#define COP_DBGLVL_SDOC_TRACE1          DEBUG_LVL_13_TRACE1
#define COP_DBGLVL_SDOC_TRACE2          DEBUG_LVL_13_TRACE2
#define COP_DBGLVL_SDOC_TRACE3          DEBUG_LVL_13_TRACE3
#define COP_DBGLVL_SDOC_TRACE4          DEBUG_LVL_13_TRACE4

#define COP_DBGLVL_LSSS                 DEBUG_LVL_14        // 0x00002000L
#define COP_DBGLVL_LSSS_TRACE0          DEBUG_LVL_14_TRACE0
#define COP_DBGLVL_LSSS_TRACE1          DEBUG_LVL_14_TRACE1
#define COP_DBGLVL_LSSS_TRACE2          DEBUG_LVL_14_TRACE2
#define COP_DBGLVL_LSSS_TRACE3          DEBUG_LVL_14_TRACE3
#define COP_DBGLVL_LSSS_TRACE4          DEBUG_LVL_14_TRACE4

#define COP_DBGLVL_LSSM                 DEBUG_LVL_15        // 0x00004000L
#define COP_DBGLVL_LSSM_TRACE0          DEBUG_LVL_15_TRACE0
#define COP_DBGLVL_LSSM_TRACE1          DEBUG_LVL_15_TRACE1
#define COP_DBGLVL_LSSM_TRACE2          DEBUG_LVL_15_TRACE2
#define COP_DBGLVL_LSSM_TRACE3          DEBUG_LVL_15_TRACE3
#define COP_DBGLVL_LSSM_TRACE4          DEBUG_LVL_15_TRACE4

#define COP_DBGLVL_SRDO                 DEBUG_LVL_16        // 0x00008000L
#define COP_DBGLVL_SRDO_TRACE0          DEBUG_LVL_16_TRACE0
#define COP_DBGLVL_SRDO_TRACE1          DEBUG_LVL_16_TRACE1
#define COP_DBGLVL_SRDO_TRACE2          DEBUG_LVL_16_TRACE2
#define COP_DBGLVL_SRDO_TRACE3          DEBUG_LVL_16_TRACE3
#define COP_DBGLVL_SRDO_TRACE4          DEBUG_LVL_16_TRACE4

//...

#define COP_DBGLVL_CCM                  DEBUG_LVL_24        // 0x00800000L
#define COP_DBGLVL_CCM_TRACE0           DEBUG_LVL_24_TRACE0
#define COP_DBGLVL_CCM_TRACE1           DEBUG_LVL_24_TRACE1
#define COP_DBGLVL_CCM_TRACE2           DEBUG_LVL_24_TRACE2
#define COP_DBGLVL_CCM_TRACE3           DEBUG_LVL_24_TRACE3
#define COP_DBGLVL_CCM_TRACE4           DEBUG_LVL_24_TRACE4

#define COP_DBGLVL_COPMGR               DEBUG_LVL_25        // 0x01000000L
#define COP_DBGLVL_COPMGR_TRACE0        DEBUG_LVL_25_TRACE0
#define COP_DBGLVL_COPMGR_TRACE1        DEBUG_LVL_25_TRACE1
#define COP_DBGLVL_COPMGR_TRACE2        DEBUG_LVL_25_TRACE2
#define COP_DBGLVL_COPMGR_TRACE3        DEBUG_LVL_25_TRACE3
#define COP_DBGLVL_COPMGR_TRACE4        DEBUG_LVL_25_TRACE4

#define COP_DBGLVL_SDOGWY               DEBUG_LVL_26        // 0x02000000L
#define COP_DBGLVL_SDOGWY_TRACE0        DEBUG_LVL_26_TRACE0
#define COP_DBGLVL_SDOGWY_TRACE1        DEBUG_LVL_26_TRACE1
#define COP_DBGLVL_SDOGWY_TRACE2        DEBUG_LVL_26_TRACE2
#define COP_DBGLVL_SDOGWY_TRACE3        DEBUG_LVL_26_TRACE3
#define COP_DBGLVL_SDOGWY_TRACE4        DEBUG_LVL_26_TRACE4

//...

#define COP_DBGLVL_KERNEL               DEBUG_LVL_KERNEL    // 0x10000000L
#define COP_DBGLVL_KERNEL_TRACE0        DEBUG_LVL_KERNEL_TRACE0
#define COP_DBGLVL_KERNEL_TRACE1        DEBUG_LVL_KERNEL_TRACE1
#define COP_DBGLVL_KERNEL_TRACE2        DEBUG_LVL_KERNEL_TRACE2
#define COP_DBGLVL_KERNEL_TRACE3        DEBUG_LVL_KERNEL_TRACE3
#define COP_DBGLVL_KERNEL_TRACE4        DEBUG_LVL_KERNEL_TRACE4

#define COP_DBGLVL_ASSERT               DEBUG_LVL_ASSERT    // 0x20000000L
#define COP_DBGLVL_ASSERT_TRACE0        DEBUG_LVL_ASSERT_TRACE0
#define COP_DBGLVL_ASSERT_TRACE1        DEBUG_LVL_ASSERT_TRACE1
#define COP_DBGLVL_ASSERT_TRACE2        DEBUG_LVL_ASSERT_TRACE2
#define COP_DBGLVL_ASSERT_TRACE3        DEBUG_LVL_ASSERT_TRACE3
#define COP_DBGLVL_ASSERT_TRACE4        DEBUG_LVL_ASSERT_TRACE4

#define COP_DBGLVL_ERROR                DEBUG_LVL_ERROR     // 0x40000000L
#define COP_DBGLVL_ERROR_TRACE0         DEBUG_LVL_ERROR_TRACE0
#define COP_DBGLVL_ERROR_TRACE1         DEBUG_LVL_ERROR_TRACE1
#define COP_DBGLVL_ERROR_TRACE2         DEBUG_LVL_ERROR_TRACE2
#define COP_DBGLVL_ERROR_TRACE3         DEBUG_LVL_ERROR_TRACE3
#define COP_DBGLVL_ERROR_TRACE4         DEBUG_LVL_ERROR_TRACE4

#define COP_DBGLVL_ALWAYS               DEBUG_LVL_ALWAYS    // 0x80000000L
#define COP_DBGLVL_ALWAYS_TRACE0        DEBUG_LVL_ALWAYS_TRACE0
#define COP_DBGLVL_ALWAYS_TRACE1        DEBUG_LVL_ALWAYS_TRACE1
#define COP_DBGLVL_ALWAYS_TRACE2        DEBUG_LVL_ALWAYS_TRACE2
#define COP_DBGLVL_ALWAYS_TRACE3        DEBUG_LVL_ALWAYS_TRACE3
#define COP_DBGLVL_ALWAYS_TRACE4        DEBUG_LVL_ALWAYS_TRACE4


// ============================================================================
// general functions
// ============================================================================

#ifdef __cplusplus
extern "C"
{
#endif

//---------------------------------------------------------------------------
//
// Function:    CopGetStackVersion()
//
// Description: function returns the version of the CANopen stack
//
// Parameters:  none
//
// Returns:     DWORD = version of CANopen stack with format:
//                  bit  0 to  7:   major Version in hexadecimal notation
//                  bit  8 to 15:   minor Version in hexadecimal notation
//                  bit 16 to 31:   release number
//
//---------------------------------------------------------------------------

COPDLLEXPORT DWORD PUBLIC CopGetStackVersion (void);


// ============================================================================
// functions within ccmmain.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    CcmInitCANopen()
//
// Description: deletes all instances befor first instance
//              and initializes further instances of CANopen
//
// Parameters:  CCM_DECL_PTR_INSTANCE_HDL_  = (pointer to variable for instance handle)
//              tCcmInitPraram              = pointer to initializing struct
//
// Returns:     tCopKernel                  = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmInitCANopen (CCM_DECL_PTR_INSTANCE_HDL_
    tCcmInitParam GENERIC* pInitParam_p, tCcmInitCall FirstCall_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmShutDownCANopen()
//
// Description: deletes an instance of CANopen
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmShutDownCANopen (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CcmConnectToNet()
//
// Description: switches from NMT state INITIALISATION to PRE-OPERATIONAL
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmConnectToNet (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CcmDefineVarTab()
//
// Description: defines variables for application from a table
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pVarTab_p               = start address of table
//              wNumOfVars_p            = number of variables to define
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmDefineVarTab (CCM_DECL_INSTANCE_HDL_
    tVarParam GENERIC* pVarTab_p, WORD wNumOfVars_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmProcessLssInitState()
//
// Description: runs LSS process for 'invalid NodeId'
//              if NodeId is valid or has been configurated then function returns
//              with kCopSuccessful instatt of kCopLsssInvalidNodeId
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmProcessLssInitState (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CcmProcess()
//
// Description: runs through all process functions of CANopen
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmProcess (CCM_DECL_INSTANCE_HDL);


//---------------------------------------------------------------------------
//
// Function:    CcmRegisterErrorCallback()
//
// Description: 
//
// Parameters:  fpErrorCallback_p
//
// Return:      void
//
// State:       not tested
//
//---------------------------------------------------------------------------

COPDLLEXPORT void PUBLIC CcmRegisterErrorCallback (CCM_DECL_INSTANCE_HDL_
    tCcmErrorCallback   fpErrorCallback_p);


// ============================================================================
// Only for Linux
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:     CcmWaitForValidNodeId
//
// Description:  The function return, when a valid NodeId is configured over CAN
//               and LSS-protocoll
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmWaitForValidNodeId (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:     CcmWaitForNmtStarted
//
// Description:  The function return, when the NMT Statemachine is started
//               The application should call this function after CcmInitCANopen()
//               and bevor starting the real application
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmWaitForNmtStarted (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:     CcmLockedCopyData
//
// Description:  The funtion copies data, when the CANopen-Threads are locked
//               It can be used for a saved copy of data that are used in
//               application and CANopen-Stack
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmLockedCopyData (CCM_DECL_INSTANCE_HDL_
    BYTE GENERIC * pDst_p, BYTE GENERIC * pSrc_p, DWORD dwLen_p);


// ----------------------------------------------------------------------------
//
// Function:     CcmLockCanopenThreads
//
// Description:  The funktion locks the CANopen-Threads.
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//
// Returns:     void
//
// ----------------------------------------------------------------------------

void  PUBLIC CcmLockCanopenThreads (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:     CcmUnlockCanopenThreads
//
// Description:  The funktion unlocks the CANopen-Threads.
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//
// Returns:     void
//
// ----------------------------------------------------------------------------

void PUBLIC  CcmUnlockCanopenThreads (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:     CcmSendThreadEvent
//
// Description:  The function sends events to the CANopen-Threads.
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSendThreadEvent (CCM_DECL_INSTANCE_HDL_
    DWORD dwEvent_p);


// ============================================================================
// functions within ccmdfpdo.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:     CcmDefinePdoTab()
//
// Description:  defines PODs for application from a table
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               pPdoTab_p              = start address of table
//               wNumOfPdos_p           = number of PDOs to define
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmDefinePdoTab (CCM_DECL_INSTANCE_HDL_
    tPdoParam GENERIC* pPdoTab_p, WORD wNumOfPdos_p);


// ============================================================================
// functions within ccmstpdo.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    CcmDefineStaticPdoTab()
//
// Description: defines static PODs for application from a table
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pPdoTab_p               = start address of table
//              wNumOfPdos_p            = number of PDOs to define
//
// Returns:     tCopKernel = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmDefineStaticPdoTab (CCM_DECL_INSTANCE_HDL_
    tPdoStaticParam GENERIC* pPdoTab_p, WORD wNumOfPdos_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmSignalStaticPdo()
//
// Description: signals a PDO that it has to be sent in next process
//
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              wCommuIndex_p           = communication index of Tx-PDO
//
//
// Returns:     tCopKernel = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSignalStaticPdo (CCM_DECL_INSTANCE_HDL_
    WORD wCommuIndex_p);


// ============================================================================
// functions within ccmsnpdo.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:     CcmSignalCheckVar()
//
// Description:  signals CcmProcess() to check all variables for PDO's
//               to send them
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSignalCheckVar (CCM_DECL_INSTANCE_HDL);


//---------------------------------------------------------------------------
//
// Function:    CcmSignalVar()
//
// Description: signals a concretely variable which is mapped in a PDO
//              for sending the PDO
//
// Parameters:  pVar_p      = pointer to the variable which should be signaled
//
// Return:      tCopKernel  = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSignalVar (CCM_DECL_INSTANCE_HDL_
    void MEM* pVar_p);


//---------------------------------------------------------------------------
//
// Function:    CcmSendPdo()
//
// Description: function sends a PDO without check of conditions (Tx-Type,
//              event timer, inhibit timer, ...)
//              has to be called in critical section
//
// Parameters:  wPdoCommuIndex_p        = communication index of the PDO
//
// Return:      tCopKernel  = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSendPdo (CCM_DECL_INSTANCE_HDL_
    WORD wCommuIndex_p);


// ============================================================================
// functions within ccmobj.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:     CcmReadObject()
//
// Description:  reads a value from an object in OD
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               wIndex_p               = index of the object to read
//               bSubindex_p            = subindex of the object to read
//               pData_p                = destination address for the data
//               pSize_p                = INPUT:  pointer to size of destiation buffer
//                                        OUTPUT: pointer to read bytes
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmReadObject (CCM_DECL_INSTANCE_HDL_
    WORD              wIndex_p,
    BYTE              bSubindex_p,
    void GENERIC*     pDstData_p,
    tObdSize GENERIC* pSize_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmWriteObject()
//
// Description: writes a new value to an object in OD
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              wIndex_p                = index of the object to write
//              bSubindex_p             = subindex of the object to write
//              pData_p                 = address of the data to write
//              Size_p                  = size of data to write
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmWriteObject (CCM_DECL_INSTANCE_HDL_
    WORD          wIndex_p,
    BYTE          bSubindex_p,
    void GENERIC* pSrcData_p,
    tObdSize      Size_p);


// ============================================================================
// functions within ccmnmtm.c ccmnmtme.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    CcmInitNmtm()
//
// Description: initialises the NMT module for this CCM module
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmInitNmtm (CCM_DECL_INSTANCE_HDL);

// ----------------------------------------------------------------------------
//
// Function:    CcmInitNmtmEx()
//
// Description: register an user application specific callback handler
//              for NMT Master events
//
// Parameters:  CCM_DECL_INSTANCE_HDL  =   (instance handle)
//
// Returns:     tCopKernel  = error code
//
// ----------------------------------------------------------------------------

// type definition for user application specific callback handler
typedef tCopKernel (PUBLIC ROM* tAppNmtmEventCallback) (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p,
    tNmtErrCtrlEvent NmtmEvent_p);

COPDLLEXPORT tCopKernel PUBLIC CcmInitNmtmEx (CCM_DECL_INSTANCE_HDL_
    tAppNmtmEventCallback pfnCallback_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmDefineSlaveTab()
//
// Description: defines slaves for bootup and/or node-/lifeguarding from a table
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pbSlaveTab_p            = start address of slave table including NodeId's
//              bNumOfSlaves_p          = number of entries in table
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmDefineSlaveTab (CCM_DECL_INSTANCE_HDL_
    BYTE GENERIC* pbSlaveTab_p, BYTE bNumOfSlaves_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmSendNmtCommand()
//
// Description: sends a NMT command to an other slave node
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              bNodeId_p               = NodeId of slave node or own NodeId
//                                        (if zero command will be sent to all nodes)
//              NmtCommand_p            = NMT command to send or execute
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSendNmtCommand (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p, tNmtCommand NmtCommand_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmTriggerNodeGuard()
//
// Description: Sends a RTR frame to the specified slave node to get the current
//              node state. function does not return the node state. Slave node
//              answers the RTR frame after any time. If the master receives the
//              answer, then callback function CcmCbNmtmEvent() will be called.
//              The slave node has to be added in master data base previously.
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              bNodeId_p               = NodeId of slave node
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmTriggerNodeGuard (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmConfigLgm()
//
// Description: This function configures and activates life guard master for
//              controling life guard slave via liefe guarding.
//
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              bNodeId_p               = NodeId of slave node
//              pSlaveParam_p           = pointer to life guard parameters
//
//
// Returns:     tCopKernel = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmConfigLgm (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p, tNmtmSlaveParam GENERIC* pSlaveParam_p);


// ============================================================================
// functions within ccmlgs.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    CcmInitLgs()
//
// Description: initializes lifeguard slave
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmInitLgs (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CcmConfigLgs()
//
// Description: function configures lifeguarding for this node
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              wLifeGuardTime_p        = lifeguard time (time base 1ms)
//              bLifeGuardFactor_p      = lifeguard factor
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmConfigLgs (CCM_DECL_INSTANCE_HDL_
    WORD wLifeGuardTime_p, BYTE bLifeGuardFactor_p);


// ============================================================================
// functions within ccmstore.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:     CcmInitStore()
//
// Description:  initializes functionality for STORE, RESTORE and LOAD
//               for OD values in other memory mediums
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmInitStore (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CcmStoreCheckArchivState()
//
// Description: Function gets the state of an archiv part.
//              If function returnes kCopCcmStoreUnvalidState then application
//              should set configure PDO configuration (e.g. Pre Defined
//              Connection Set or its own seiitings)
//
// Parameters:  CCM_DECL_INSTANCE_HDL_ = (instance handle)
//              bCurrentOdPart_p       = part of OD
//
// Returns:     kCopSuccessful           - archiv is valid
//              kCopCcmStoreUnvalidState - archiv is invalid
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmStoreCheckArchivState (CCM_DECL_INSTANCE_HDL_
    tObdPart bCurrentOdPart_p);


// ----------------------------------------------------------------------------
// Function:     CcmStoreRestoreDefault()
//
// Description:  deletes all stored OD parameters from non-volatile memory
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//
// Returns:      tCopKernel             = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmStoreRestoreDefault (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:     CcmCbStore()
//
// Description:  callback function, called by OBD or SDOS module, that notifies of
//               access to object 0x1010
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               pParam_p               = address to calling parameters
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------
#if (CCM_USE_STORE_RESTORE != FALSE)
    COPDLLEXPORT tCopKernel PUBLIC CcmCbStore (CCM_DECL_INSTANCE_HDL_
        void MEM* pParam_p);
#else
    // this prevents linker error when CCM_USE_STORE_RESTORE==FALSE but object 0x1010/0x1011 is defined
    #define CcmCbStore          NULL
#endif


// ----------------------------------------------------------------------------
//
// Function:     CcmCbRestore()
//
// Description:  callback function, called by OBD or SDOS module, that notifies of
//               access to object 0x1011
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               pParam_p               = address to calling parameters
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------
#if (CCM_USE_STORE_RESTORE != FALSE)
    COPDLLEXPORT tCopKernel PUBLIC CcmCbRestore (CCM_DECL_INSTANCE_HDL_
        void MEM* pParam_p);
#else
    // this prevents linker error when CCM_USE_STORE_RESTORE==FALSE but object 0x1010/0x1011 is defined
    #define CcmCbRestore        NULL
#endif


// ============================================================================
// functions within ccmhbc.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:     CcmInitHbc()
//
// Description:  function sets the adress of callbackfunction CcmCbHbcEvent()
//               to the CANopen stack
//
// Parameters:   CCM_DECL_INSTANCE_HDL = (instance handle)
//
// Returns:      tCopKernel            = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmInitHbc (CCM_DECL_INSTANCE_HDL);

// Callbackfunction to info of heartbeat-event
typedef tCopKernel (PUBLIC ROM* tAppHbcEventCallback) (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p,
    tNmtErrCtrlEvent HbcEvent_p);

COPDLLEXPORT tCopKernel PUBLIC CcmInitHbcEx (CCM_DECL_INSTANCE_HDL_
    tAppHbcEventCallback fpHbcEventCallback_p);


// ----------------------------------------------------------------------------
//
// Function:     CcmHbcDefineProducerTab()
//
// Description:  function configs HBC with heartbeat-time and producer-node-id
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               wHearbeatTime_p        = time for consumer
//               bProducerNodeId_p      = NodeId of HB producer
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmHbcDefineProducerTab (CCM_DECL_INSTANCE_HDL_
    tHbcProdParam GENERIC* pbProducerTab_p, BYTE bNumOfProducer_p);


// ============================================================================
// functions within ccmhbp.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:     CcmConfigHbp()
//
// Description:  function configs HBP with heartbeat-time
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               wHbProducerTime_p      = time for producer
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmConfigHbp (CCM_DECL_INSTANCE_HDL_
    WORD  wHbProducerTime_p);


// ============================================================================
// functions within ccmsync.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    CcmInitSyncConsumer()
//
// Description: function initializes SYNC consumer
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmInitSyncConsumer (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CcmConfigSyncConsumer()
//
// Description: function configures SYNC cunsumer with SYNC window length
//              and SYNC id
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              dwSyncId_p              = CanId of the sync-message
//              dwSyncWindowLength_p    = max. time to sending SYNC PDOs after
//                                        receiving the sync-message
//                                        (time base 1us)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmConfigSyncConsumer (CCM_DECL_INSTANCE_HDL_
    DWORD  dwSyncId_p, DWORD  dwSyncWindowLength_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmConfigSyncProducer()
//
// Description: function configures SYNC producer
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              dwSyncId_p              = CanId of the SYNC message
//              dwSyncCycleTime_p       = time difference to send SYNC message
//                                        (time base 1us)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmConfigSyncProducer (CCM_DECL_INSTANCE_HDL_
    DWORD  dwSyncId_p, DWORD  dwSyncCycleTime_p);

// ============================================================================
// functions within ccmemcp.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:     CcmConfigEmcp()
//
// Description:  function configs EMCP with CobId
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               dwEmcpProducerCobId_p      = CobId of emergency producer
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmConfigEmcp (CCM_DECL_INSTANCE_HDL_
    DWORD  dwEmcpProducerCobId_p);


// ----------------------------------------------------------------------------
//
// Function:     CcmSendEmergency()
//
// Description:  function sends a emergency message
//
// Parameters:   MCO_DECL_INSTANCE_PTR_ = (pointer to instance)
//               pEmcpParam_p           = parameters of emergency object
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSendEmergency (CCM_DECL_INSTANCE_HDL_
    tEmcParam MEM* pEmcpParam_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmClearPreDefinedErrorField()
//
// Description: function clears the PreDefinedErrorField - EMCP
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmClearPreDefinedErrorField (CCM_DECL_INSTANCE_HDL);


// ============================================================================
// functions within ccmemcc.c / ccmemcce.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:     CcmInitEmcc()
//
// Description:  function sets the adress of callbackfunction CcmCbEmccEvent()
//               to the CANopen stack
//
// Parameters:   CCM_DECL_INSTANCE_HDL = (instance handle)
//
// Returns:      tCopKernel            = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmInitEmcc (CCM_DECL_INSTANCE_HDL);

// Callbackfunction to info of emergency-event
typedef tCopKernel (PUBLIC ROM* tAppEmccEventCallback) (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p,
    tEmcParam MEM* pEmcParam_p);

// ----------------------------------------------------------------------------
// Function:    CcmInitEmccEx()
//
// Description: initialises application callback handler for EMCC events
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pfnCallback_p           = pointer to callback handler
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmInitEmccEx (CCM_DECL_INSTANCE_HDL_
    tAppEmccEventCallback pfnCallback_p);

// ----------------------------------------------------------------------------
//
// Function:    CcmEmccDefineProducerTab()
//
// Description: defines the producer nodeid from a table for emergency consumer
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pbProducerTab_p         = start address of producer table including NodeId's
//              bNumOfProducer_p          = number of entries in table
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmEmccDefineProducerTab (CCM_DECL_INSTANCE_HDL_
    BYTE GENERIC* pbProducerTab_p, BYTE bNumOfProducer_p);


// ============================================================================
// functions within ccmsdoc.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    CcmSdocDefineClientTab()
//
// Description: defines all specified SDO clients
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pClientTab_p            = start address of client table including
//                                        a structure with all necessary datas
//              bNumOfClients_p         = number of entries in table
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSdocDefineClientTab (CCM_DECL_INSTANCE_HDL_
    tSdocParam GENERIC* pClientTab_p,
    BYTE                bNumOfClients_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmSdocStartTransfer()
//
// Description: starts a SDO transfer to a SDO server
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pSdoTransParam_p        = Pointer to structure with parameters
//                                        for SDO transfer
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSdocStartTransfer (CCM_DECL_INSTANCE_HDL_
    tSdocTransferParam MEM* pSdoTransParam_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmSdocAbort()
//
// Description: The function abort an active client and sends the Abort
//              Transfer Service with abort code  dwAbortCode_p.
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  - instance handle
//              wClientIndex_p          - index of client in OD
//              pdwAbortCode_p          - SDO abort code
//
// Returns:     tCopKernel              - error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSdocAbort (CCM_DECL_INSTANCE_HDL_
    WORD wClientIndex_p, DWORD dwAbortCode_p );


//-------------------------------------------------------------------------
//
// Function:     SdocGetState
//
// Description:  The function gets the SDO transfer state.
//
// Parameters:   MCO_DECL_INSTANCE_PTR_  - instance handle
//               wClientIndex_p          - index of client in OD
//               pSdocState_p            - Pointer to state variable in
//                                         user application. The function
//                                         writes the transfer state in
//                                         this variable.
//               pdwAbortCode_p          - Pointer to abort code variable
//                                         in user application. The function
//                                         writes the occured abort code in
//                                         this variable.
//
// Returns:      tCopKernel              - error code
//
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSdocGetState (CCM_DECL_INSTANCE_HDL_
    WORD              wClientIndex_p,
    tSdocState MEM*   pSdocState_p,
    DWORD      MEM*   pdwAbortCode_p );


// ============================================================================
// functions within ccmboot.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    CcmBootNetwork()
//
// Description: sends a NMT command "start remote node" to all slave node
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmBootNetwork (CCM_DECL_INSTANCE_HDL);


// ============================================================================
// functions within ccmwin.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    CcmEnterCriticalSectionPdoProcess()
//
// Description: locks the CcmProcessThread in order to change the pdo variables
//              by the application
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     void
//
// ----------------------------------------------------------------------------

COPDLLEXPORT void PUBLIC CcmEnterCriticalSectionPdoProcess (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CcmLeaveCriticalSectionPdoProcess()
//
// Description: unlocks the CcmProcessThread to send the pdo variables
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     void
//
// ----------------------------------------------------------------------------

COPDLLEXPORT void PUBLIC CcmLeaveCriticalSectionPdoProcess (CCM_DECL_INSTANCE_HDL);



// ============================================================================
// functions within ccmsrdo.c
// ============================================================================

//---------------------------------------------------------------------------
//
// Function:    CcmSendSrdo()
//
// Description: function sends a srdo without check of conditions
//
// Parameters:  CCM_DECL_INSTANCE_HDL_ = (instance handle)
//              wSrdoCommuIndex_p      = communication index of the SRDO
//
// Return:      tCopKernel  = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSendSrdo (CCM_DECL_INSTANCE_HDL_
    WORD wSrdoCommuIndex_p);


//---------------------------------------------------------------------------
//
// Function:    CcmCheckSrdoConfig()
//
// Description: checks all configurations of all SRDOs
//              Function has to be called at once within safety time.
//
// Parameters:  CCM_DECL_INSTANCE_HDL_ = (instance handle)
//              pwCommuIndex_p         = index of the fail SRDO for return
//
// Return:      tCopKernel  = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmCheckSrdoConfig (CCM_DECL_INSTANCE_HDL_
    WORD MEM* pwCommuIndex_p);


//---------------------------------------------------------------------------
//
// Function:    CcmSendGfc()
//
// Description: function sends a GFC message
//
// Parameters:  CCM_DECL_INSTANCE_HDL = (instance handle)
//
// Return:      tCopKernel  = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSendGfc (CCM_DECL_INSTANCE_HDL);


//---------------------------------------------------------------------------
//
// Function:    CcmGetSrdoState()
//
// Description: Read the state of a SRDO
//
// Parameters:  CCM_DECL_INSTANCE_HDL_ = (instance handle)
//              pSrdoState_p           = Pointer to Statevariable for return
//              wSrdoCommuIndex_p      = Commuindex of the used SRDO
//
// Return:      tCopKernel  = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmGetSrdoState (CCM_DECL_INSTANCE_HDL_
    BYTE MEM* pbSrdoState_p,
    WORD wSrdoCommuIndex_p);


//---------------------------------------------------------------------------
//
// Function:    CcmSetSrdoState()
//
// Description: Write the state of a SRDO
//
// Parameters:  CCM_DECL_INSTANCE_HDL_ = (instance handle)
//              bSrdoState_p           = Statevariable to write
//              wSrdoCommuIndex_p      = Commuindex of the used SRDO
//
// Return:      tCopKernel  = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmSetSrdoState (CCM_DECL_INSTANCE_HDL_
    BYTE bSrdoState_p,
    WORD wSrdoCommuIndex_p);


//---------------------------------------------------------------------------
//
// Function:    CcmGetSrdoParam()
//
// Description: function copies commu parameter of a srdo to the application
//
// Parameters:  CCM_DECL_INSTANCE_HDL_ = (instance handle)
//              wSrdoCommuIndex_p      = communication index of the SRDO
//              pSrdoCommuParam_p      = pointer to parameterstructure to fill
//
// Return:      tCopKernel  = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmGetSrdoParam (CCM_DECL_INSTANCE_HDL_
    WORD wSrdoCommuIndex_p,
    tSrdoCommuParam MEM* pSrdoCommuParam_p,
    tSrdoMappParam MEM* pSrdoMappParam_p);


//---------------------------------------------------------------------------
//
// Function:    CcmStaticDefineSrdoVarFields()
//
// Description: function defines a SRDO variable fields for static SRDO
//              mapping (Normal/Invers Data)
//
// Parameters:  CCM_DECL_INSTANCE_HDL_ = (instance handle)
//              wSrdoCommuIndex_p      = communication index of the SRDO
//              pNormalData_p          = pointer to a normal data variable field (max. 8 bytes)
//              pInversData_p          = pointer to a invers data variable field (max. 8 bytes)
//
// Return:      tCopKernel  = error code
//
//---------------------------------------------------------------------------
#if (SRDO_USE_STATIC_MAPPING != FALSE)
COPDLLEXPORT tCopKernel PUBLIC CcmStaticDefineSrdoVarFields (CCM_DECL_INSTANCE_HDL_
    WORD wCommuIndex_p,
    void MEM* pNormalData_p,
    void MEM* pInversData_p);
#endif // (SRDO_USE_STATIC_MAPPING != FALSE)


// ============================================================================
// functions within application for SRDO module
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    AppSrdoEvent()
//
// Description: The function is a callback function from SRDO module.
//              It is called when a SRDO is received/transmitted
//              successfully.
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (handle of instance)
//              wSrdoCommuIndex_p       = communication index of the SRDO
//
// Returns:     error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC AppSrdoEvent (CCM_DECL_INSTANCE_HDL_
    WORD wSrdoCommuIndex_p);


// ----------------------------------------------------------------------------
//
// Function:    AppSrdoError()
//
// Description: The function is a callback function from SRDO module.
//              It is called when a SRDO is received/transmitted
//              faulty.
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (handle of instance)
//              wSrdoCommuIndex_p       = communication index of the SRDO
//              ErrorCode_p             = errorcode from errordef.h
//
// Returns:     error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC AppSrdoError (CCM_DECL_INSTANCE_HDL_
    WORD wSrdoCommuIndex_p,
    tCopKernel ErrorCode_p);


// ----------------------------------------------------------------------------
//
// Function:    AppGfcEvent()
//
// Description: The function is a callback function from SRDO module.
//              It is called when a GFC is received
//
// Parameters:  CCM_DECL_INSTANCE_HDL  = (handle of instance)
//
// Returns:     error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC AppGfcEvent (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    AppProgMonEvent()
//
// Description: The function is a callback function from SRDO module.
//              It is called when a program step is finished in SRDO
//              module.
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (handle of instance)
//              wSrdoCommuIndex_p       = communication index of the SRDO
//
// Returns:     error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC AppProgMonEvent (CCM_DECL_INSTANCE_HDL_
    tProgMonEvent Event_p);


// ============================================================================
// functions within ccm303.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    CcmInitIndicators()
//
// Description: initializes interface for Indicator Specification DR303-3
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     void
//
// ----------------------------------------------------------------------------

COPDLLEXPORT void PUBLIC Ccm303InitIndicators (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CcmProcessIndicators()
//
// Description: processes all LEDs for CANopen reporting DR303
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pTime_p                 = pointer to timer value
//                                        (later used for operation systems - not used now)
//
// Returns:     void
//
// ----------------------------------------------------------------------------

COPDLLEXPORT void PUBLIC Ccm303ProcessIndicators (CCM_DECL_INSTANCE_HDL_
    tTime MEM* pTime_p);


// ----------------------------------------------------------------------------
//
// Function:    Ccm303SetRunState()
//
// Description: sets the state for the "CANopen RUN LED" (green)
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              State_p                 = new state (according DR303-3)
//
// Returns:     void
//
// ----------------------------------------------------------------------------

COPDLLEXPORT void PUBLIC Ccm303SetRunState (CCM_DECL_INSTANCE_HDL_
    tDr303RunState State_p);


// ----------------------------------------------------------------------------
//
// Function:    Ccm303SetErrorState()
//
// Description: sets the state for the "CANopen ERROR LED" (red)
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              State_p                 = error state (according DR303-3)
//              fErrorSet_p             = FALSE if should be deleted
//
// Returns:     void
//
// ----------------------------------------------------------------------------

COPDLLEXPORT void PUBLIC Ccm303SetErrorState (CCM_DECL_INSTANCE_HDL_
    tDr303ErrorState State_p, BYTE fErrorDelete_p);


// ============================================================================
// functions within ccmlss.c
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    CcmLssmSwitchMode()
//
// Description: sends service "Swicht Mode Global" or "Switch Mode Selective"
//              to one or more LSS slave
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              LssMode_p               = defines global mode operation/configuration (pLssAddress_p == NULL)
//                                        or mode selective (pLssAddress_p != NULL)
//              pLssAddress_p           = pointer to LSS address of LSS slave to switch with Mode Selective
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmLssmSwitchMode (CCM_DECL_INSTANCE_HDL_
    tLssMode LssMode_p, tLssAddress GENERIC* pLssAddress_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmLssmSwitchMode()
//
// Description: configures one LSS slave with a new node-ID
//              or more LSS slaves and LSS master itself with a new baudrate
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              bNodeId_p               = if != LSS_INVALID_NODEID node-ID will be configured
//              pBitTiming_p            = pointer to new baudrate parameters
//                                        if != NULL baudrate will be configured
//              wSwitchDelay_p          = switch delay until baudrate will be switched
//              bStore_p                = if != FALSE configuration will be stored in LSS slave
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmLssmConfigureSlave (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p,
    tLssmBitTiming GENERIC* pBitTiming_p,
    WORD wSwitchDelay_p,
    BYTE bStore_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmLssmInquireIdentity()
//
// Description: inquires LSS slave for one or more parameters od LSS address and/or node-ID
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              bServiceFlags_p         = defines which LSS services of Inquire Identify should be sent
//              pLssAddress_p           = pointer to structure which receives the LSS address of the slave
//              pbNodeId_p              = pointer to variable which received the node-ID of the slave
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmLssmInquireIdentity (CCM_DECL_INSTANCE_HDL_
    BYTE bServiceFlags_p,
    tLssAddress MEM* pLssAddress_p,
    BYTE MEM* pbNodeId_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmLssmIdentifySlave()
//
// Description: sends LSS service Identify Remote Slave or Identify Non-Configured Slave
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pLssIdParam_p           = if != NULL service Identify Remote Slave will be sent
//                                      = otherwise service Identify Non-Configured Slave will be sent
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CcmLssmIdentifySlave (CCM_DECL_INSTANCE_HDL_
    tLssmIdentifyParam GENERIC* pLssIdParam_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmLsssGetNodeId()
//
// Description: Funktion to read actual NodeID
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

BYTE PUBLIC CcmLsssGetNodeId (CCM_DECL_INSTANCE_HDL);


// ============================================================================
// functions within ccmmpdo.c
// ============================================================================

//---------------------------------------------------------------------------
//
// Function:    CcmPdoSendMPDO()
//
// Description: sends a multiplexed PDO
//
// Parameters:  wPdoCommuIndex_p    = communication index of the PDO
//              bNodeId_p           = node-id of the MPDO consumer (0 = broadcast)
//              pMPdoParam_p        = pointer to parameter of MPDO
//
// Return:      tCopKernel  = error code
//
//---------------------------------------------------------------------------

#if (PDO_USE_MPDO_DAM_PRODUCER != FALSE)
COPDLLEXPORT tCopKernel PUBLIC CcmPdoSendMPDO (CCM_DECL_INSTANCE_HDL_
    WORD wPdoCommuIndex_p, BYTE bNodeId_p, tMPdoParam GENERIC* pMPdoParam_p);
#endif


// ============================================================================
// functions within ccmsdom.c
// ============================================================================

// ----------------------------------------------------------------------------
// Function:        SdomInitialize()
//
// Description:     initialize the SDO Manager
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  =   (instance handle)
//                  fActivateAllScan_p = if TRUE the STATE SDOM_SCAN_ALL_SRD
//                                       will use
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

tCopKernel PUBLIC SdomInitialize (CCM_DECL_INSTANCE_HDL_
    BYTE           fActivateAllScan_p); // IN: if TRUE STATE SDOM_SCAN_ALL_SRD
                                       //     will used

// ----------------------------------------------------------------------------
// Function:        SdomInitCobList()
//
// Description:     initialize the SDO Manager
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  =   (instance handle)
//                  padwFreeCobTab_p        = pointer to an table whith free
//                                            COB-IDs for OD 0x1F02
//                  bFreeCobCount_p         = count of entries in table
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdomInitCobList (CCM_DECL_INSTANCE_HDL_
    DWORD GENERIC* padwFreeCobTab_p,  // IN: Pointer auf Tabelle mit freien
                                      //     COB-IDs für 0x1F02
    BYTE           bFreeCobCount_p);  // IN: count of entries in table
                                      //     (0 to 254)


// ----------------------------------------------------------------------------
// Function:        SdomReset()
//
// Description:     reset the SDO Manager (set State Maschine to IDLE, delete all events)
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  =   (instance handle)
//                  pwSdoClientIndex_p      =   Pointer to an Array of SDO Client Indexes
//                                              used for Scan and configuration
//                                              -> only one used intil now
//                  bSdoClientIndexCount_p   =  number of entries in pwSdoClientIndex_p
//
//
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdomReset (CCM_DECL_INSTANCE_HDL_
    WORD GENERIC*  pwSdoClientIndex_p,
    BYTE           bSdoClientIndexCount_p);


// ----------------------------------------------------------------------------
// Function:        SdomConfig()
//
// Description:     shut configure Timeout for SDO tranfer during scan for SRD Client
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  =   (instance handle)
//                  pSdomInit_p              = pointer to init structure
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdomConfig(CCM_DECL_INSTANCE_HDL_
    tSdomInit GENERIC* pSdomInit_p);


// ----------------------------------------------------------------------------
// Function:        SdomShutDown()
//
// Description:     shut the SDO Manager down
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  =   (instance handle)
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdomShutDown (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
// Function:        SdomProcess()
//
// Description:     main function of the SDO Manager
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  =   (instance handle)
//                  pTime_p =   pointer to timer value (later used
//                              for operation systems - not used now)
//
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdomProcess (CCM_DECL_INSTANCE_HDL_
    tTime* pTime_p);


//---------------------------------------------------------------------------
// Function:        SdomRegisterPossibleSrd()
//
// Description:     -   function to register node which can be
//                      SRD Client (OD 0x1F10 existing)
//                  -   have to be called after SdomInitialize()
//                  -   usefull after a global networkscan
//                  -   if pbSrdNodeIdList_p == NULL and bSrdNodeIdCount == 0
//                      --> intern list with all node ids cleared
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  = (instance handle)
//                  pabSrdNodeIdList_p      =  pointer to an Array of
//                                             Bytes with the Node IDs
//                  bSrdNodeIdCount_p       =  Count of Entries in pabSrdNodeIdList_p
//
// Returns:         tCopKernel = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdomRegisterPossibleSrd (CCM_DECL_INSTANCE_HDL_
    BYTE GENERIC*   pabSrdNodeIdList_p,
    BYTE            bSrdNodeIdCount_p);


//---------------------------------------------------------------------------
// Function:        SdomDeregisterSrd()
//
// Description:     -   function called by CANopen Manager
//                      if Node Guarding or Heartbeat of Slave fail
//                  -   if the node is an registered SRD Client
//                      or owns all default SDOs the SDO Manager
//                      State Maschine will informed
//
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  = (instance handle)
//                  bNodeId_p               = Node-Id of the "Lost" Slave
//
//
// Returns:         tCopKernel = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdomDeregisterSrd (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p);

//---------------------------------------------------------------------------
// Function:        SdomRequestDefSdos()
//
// Description:     -   function called by CANopen Manager
//                      to request default SDO Server of a node for Scan and
//                      configuration during boot up
//
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  = (instance handle)
//                  wFreeClientIndex_p      = Index of the SDO Client
//                  bServerNodeId_p         = Node-Id of the Slave Node
//
//
// Returns:            tCopKernel = error code
// ----------------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC SdomRequestDefSdos(CCM_DECL_INSTANCE_HDL_
                                    WORD wFreeClientIndex_p,
                                    BYTE bServerNodeId_p);


// ============================================================================
// functions within ccmsrd.c
// ============================================================================

// ----------------------------------------------------------------------------
// Function:        SrdInit()
//
// Description:     initialise SRD Client
//
//
// Parameters:      CCM_DECL_INSTANCE_HDL_ =   (instance handle)
//                  pSrdInit_p             =   Pointer to an tSrdInit-structur
//                                             for initialisiation of SRD CLient
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SrdInit (CCM_DECL_INSTANCE_HDL_
    tSrdInit GENERIC* pSrdInit_p);

// ----------------------------------------------------------------------------
// Function:        SrdSetMasterMode()
//
// Description:     initialise SRD Client
//
//
// Parameters:      CCM_DECL_INSTANCE_HDL_ =   (instance handle)
//                  fMasterMode_p           =   Flag, if TRUE local access
//                                             to SDO Manager if Manager is part of
//                                             Stack
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

#if((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOM) == CCM_MODULE_SDOM)
COPDLLEXPORT tCopKernel PUBLIC SrdSetMasterMode (CCM_DECL_INSTANCE_HDL_
                            BYTE faMsterMode_p);

#endif

// ----------------------------------------------------------------------------
// Function:        SrdConfig()
//
// Description:     configurate SRD Client (Retrycount and Timeout)
//
//
// Parameters:      CCM_DECL_INSTANCE_HDL_ =   (instance handle)
//                  pSrdInit_p             =   Pointer to an tSrdInit-structur
//                                             for initialisiation of SRD CLient
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SrdConfig (CCM_DECL_INSTANCE_HDL_
                        tSrdInit GENERIC* pSrdInit_p);

// ----------------------------------------------------------------------------
// Function:        SrdReset()
//
// Description:     reset SRD Client (delete Event and set State Maschine to IDLE)
//
// Parameters:      CCM_DECL_INSTANCE_HDL =   (instance handle)
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SrdReset(CCM_DECL_INSTANCE_HDL);

// ----------------------------------------------------------------------------
// Function:        SrdProcess()
//
// Description:     - main function of SRD Client
//                  - process State aschine
//
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  =   (instance handle)
//                  pTime_p =   pointer to timer value (later used
//                              for operation systems - not used now)
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SrdProcess (CCM_DECL_INSTANCE_HDL_
    tTime MEM* pTime_p);


// ----------------------------------------------------------------------------
// Function:        SrdRegisterToManager()
//
// Description:     registers SRD Client to SDO Manager
//
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  =   (instance handle)
//                  wFreeClientIndex_p      =   free SDO Client in OD
//                  fpCb_p                  =   Pointer to a Callback-function
//                                              to get information about events in
//                                              SRD Client
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SrdRegisterToManager (CCM_DECL_INSTANCE_HDL_
    WORD wFreeClientIndex_p,        // freier SDO Client Index im OD
    tSrdEventCallback fpCb_p);      // Pointer auf Callback-Funktion


// ----------------------------------------------------------------------------
// Function:        SrdRequestConnection()
//
// Description:     requestes a dynamic SDO connection
//
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  =   (instance handle)
//                  wFreeClientIndex_p      =   free SDO Client in OD
//                  bServerNodeId_p         =   NodeId of the SDO Server (Slave)
//                  fpCb_p                  =   Pointer to a Callback-function
//                                              to get information about events in
//                                              SRD Client
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SrdRequestConnection (CCM_DECL_INSTANCE_HDL_
    WORD wFreeClientIndex_p,        // freier SDO Client Index im OD
    BYTE bServerNodeId_p,           // NodeId des SDO Server
    tSrdEventCallback fpCb_p);      // Pointer auf Callback-Funktion

// ----------------------------------------------------------------------------
// Function:        SrdRequestDefConnection()
//
// Description:     requestes a dynamic SDO connection with default SDO Server of Slave
//                  by local SDO Manager
//
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  =   (instance handle)
//                  wFreeClientIndex_p      =   free SDO Client in OD
//                  bServerNodeId_p         =   NodeId of the SDO Server (Slave)
//                  fpCb_p                  =   Pointer to a Callback-function
//                                              to get information about events in
//                                              SRD Client
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

#if((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOM) != 0)
COPDLLEXPORT tCopKernel PUBLIC SrdRequestDefConnection (CCM_DECL_INSTANCE_HDL_
        WORD wFreeClientIndex_p,        // freier SDO Client Index im OD
        BYTE bServerNodeId_p,           // NodeId des SDO Server
        tSrdEventCallback fpCb_p);       // Pointer auf Callback-Funktion
#endif

// ----------------------------------------------------------------------------
// Function:        SrdReleaseConnection()
//
// Description:     release a dynamic SDO connection
//
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  =   (instance handle)
//                  wClientIndex_p          =   SDO Client in OD which has to free
//                  bServerNodeId_p         =   NodeId of the SDO Server (Slave)
//                  fpCb_p                  =   Pointer to a Callback-function
//                                              to get information about events in
//                                              SRD Client
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SrdReleaseConnection (CCM_DECL_INSTANCE_HDL_
    WORD wClientIndex_p,            // SDO Client Index im OD der
                                    // freizugeben ist
    BYTE bServerNodeId_p,           // NodeId des SDO Server
    tSrdEventCallback fpCb_p);      // Pointer auf Callback-Funktion


// ----------------------------------------------------------------------------
// Function:        SrdShutDown()
//
// Description:     - shut down SRD Client
//                  - release all connections if necessary
//
//
// Parameters:      CCM_DECL_INSTANCE_HDL  =   (instance handle)
//
//
// Returns:         tCopKernel  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SrdShutDown (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif


// restore structure alignment
#if (DEV_SYSTEM == _DEV_WIN32_)
    #pragma pack(pop)
#elif (DEV_SYSTEM == _DEV_GNU_MPC5X5_)
    #pragma pack()
#endif


#endif //_COP_H_

// Please keep an empty line at the end of this file.
