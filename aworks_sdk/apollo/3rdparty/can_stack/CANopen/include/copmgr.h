/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen Manager according to CiA WD-302

  Description:  Header file for CANopen Manager API (WD-302)

  -------------------------------------------------------------------------

                $RCSfile: CopMgr.h,v $

                $Author: K.Trenkel $

                $Revision: 1.6 $  $Date: 2006/11/16 13:06:12 $

                $State: Exp $

                Build Environment:
                    all

  -------------------------------------------------------------------------

  Revision History:

  2005/12/22 r.d.:  first implementation

****************************************************************************/

#ifndef _COPMGR_H_
#define _COPMGR_H_

#include "cop.h"        // CANopen Standard API (CCM)

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*          G L O B A L   D E F I N I T I O N S                            */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

//---------------------------------------------------------------------------
// const defines
//---------------------------------------------------------------------------

// general settings
#ifndef COPMGR_MAX_ODSTRING_SIZE
#define COPMGR_MAX_ODSTRING_SIZE        32          // is used for objects 0x1008/0x1009/0x100A
#endif

// These constants define optional features of COPMGR.
// Use these constants for define COPMGR_FEATURES in file CopCfg.h.
#define COPMGR_FEAT_INCL_SLAVEASSIGN  0x00000001L // SlaveAssignment (include object 0x1F81)
#define COPMGR_FEAT_INCL_REQUESTNMT   0x00000002L // RequestNMT (include objects 0x1F82, 0x1F83)
#define COPMGR_FEAT_COMP_DEVICETYPE   0x00000004L // compare DeviceType (include object 0x1F84)
#define COPMGR_FEAT_COMP_PRODUCT      0x00000008L // compare VendorID, ProductCode, RevisionNumber (include objects 0x1F85, 0x1F86, 0x1F87)
#define COPMGR_FEAT_COMP_SERIALNUMBER 0x00000010L // compare SerialNumber (include object 0x1F88)
#define COPMGR_FEAT_INCL_BOOTTIME     0x00000020L // maximum BootTime (include object 0x1F89)
#define COPMGR_FEAT_INCL_EXT_SLAVE    0x00000040L // extended slave (include object 0x1F80 to configure extended slave features like automatically enter operational without NMTM module)
#define COPMGR_FEAT_INCL_ADD_INFO     0x00000080L // a pointer to additional info can be set for each node by the application
#define COPMGR_FEAT_INCL_COB          0x00000100L // includes functions for (un-)defining, receiving and sending application specific COBs

// non-standard features
#define COPMGR_FEAT_CONF_HEARTBEAT 0x80000000L // configure heartbeat/lifeguarding automatically


// defines for object 0x1F80 "NMTStartup" see CiA DS-302 (only bits)
#define COPMGR_NMTST_MASTER             0x00000001L // NMT Master
#define COPMGR_NMTST_START_ALLSLV       0x00000002L // send NMT Start All Nodes after boot-up
#define COPMGR_NMTST_NO_AUTOSTART       0x00000004L // do not enter operational automatically
#define COPMGR_NMTST_NO_STARTSLV        0x00000008L // do not allow NMT Start All Nodes
#define COPMGR_NMTST_RESETALL_MANDSLV   0x00000010L // on Error Control Event reset all nodes
#define COPMGR_NMTST_FLYING_MASTER      0x00000020L // Flying Master (not supported yet)
#define COPMGR_NMTST_STOPPALL_MANDSLV   0x00000040L // on Error Control Event stop all nodes

// defines for object 0x1F80 "NMTStartup" see CiA DS-302 (predefined combinations)
#define COPMGR_NMTSTEX_MST_NOFLYMA      (COPMGR_NMTST_MASTER)
#define COPMGR_NMTSTEX_MST_FLYMA        (COPMGR_NMTST_MASTER | COPMGR_NMTST_FLYING_MASTER)
#define COPMGR_NMTSTEX_MST_BOOT_NOFLYMA (COPMGR_NMTST_MASTER | COPMGR_NMTST_START_ALLSLV)
#define COPMGR_NMTSTEX_MST_BOOT_FLYMA   (COPMGR_NMTST_MASTER | COPMGR_NMTST_START_ALLSLV | COPMGR_NMTST_FLYING_MASTER)
#define COPMGR_NMTSTEX_SLV_NOFLYMA      (COPMGR_NMTST_NO_STARTSLV)  // but enter operational mode automatically
#define COPMGR_NMTSTEX_SLV_FLYMA        (COPMGR_NMTST_NO_STARTSLV | COPMGR_NMTST_FLYING_MASTER)
#define COPMGR_NMTSTEX_SLV_BOOT_NOFLYMA (COPMGR_NMTST_START_ALLSLV)
#define COPMGR_NMTSTEX_SLV_BOOT_FLYMA   (COPMGR_NMTST_START_ALLSLV | COPMGR_NMTST_FLYING_MASTER)
#define COPMGR_NMTSTEX_SLV_PURE         (COPMGR_NMTST_NO_STARTSLV | COPMGR_NMTST_NO_AUTOSTART)  // pure slave

// bit combinations of object 0x1F80 and what they cause
//   bit
// 3 2 1 0 | define COPMGR_NMTST*
//-------------------------------------------------------------------------------------
// NMT master mode:
// 0 0 0 1 | MST_NOFLYMA      | enter OPERATIONAL automatically
//         |                  | and start all assigned slaves individually
//         |                  |
// 0 0 1 1 | MST_BOOT_NOFLYMA | enter OPERATIONAL automatically
//         |                  | but perform NMT service START all REMOTE NODEs before
//         |                  |
// 1 0 x 1 | MST_NOFLYMA |    | enter OPERATIONAL automatically
//         | NO_STARTSLV      | do not perform NMT service START REMOTE NODE automatically
//         |                  |
// 0 1 0 1 | MST_NOFLYMA      | only enter OPERATIONAL if requested by application
//         | NO_AUTOSTART     | do not perform NMT service START REMOTE NODE automatically
//         |                  |
// 0 1 1 1 | MST_BOOT_NOFLYMA | only enter OPERATIONAL if requested by application
//         | NO_AUTOSTART     | but perform NMT service START all REMOTE NODEs before
//         |                  |
// 1 1 x 1 | MST_NOFLYMA |    | only enter OPERATIONAL if requested by application
//         | NO_STARTSLV      | do not perform NMT service START REMOTE NODE automatically
//         | NO_AUTOSTART     |
//-------------------------------------------------------------------------------------
// NMT slave mode:
// 0 0 1 0 | SLV_BOOT_NOFLYMA | enter OPERATIONAL automatically
//         |                  | but perform NMT service START all REMOTE NODEs before
//         |                  |
// 1 0 x 0 | NO_STARTSLV      | enter OPERATIONAL automatically
//         |                  | and ignore any received NMT service START all REMOTE NODE
//         |                  |
// 0 0 0 0 |                  | only enter OPERATIONAL if requested by remote NMT master
//         |                  |
// 0 1 0 0 | NO_AUTOSTART     | only enter OPERATIONAL if requested by remote NMT master
//         |                  |


// defines for object 0x1F81 "SlaveAssignment"
#define COPMGR_SLVASSIGN_SLAVE          0x00000001L // node is slave
#define COPMGR_SLVASSIGN_ERR_STARTSLV   0x00000004L // start slave on error control event
#define COPMGR_SLVASSIGN_MANDSLV        0x00000008L // slave is mandatory
#define COPMGR_SLVASSIGN_KEEPALIVE      0x00000010L // keepalive slave

// defines for object 0x1F82 "RequestNMT"
#define COPMGR_REQUESTNMT_UNKNOWN       0x00 // NMT state unknown
#define COPMGR_REQUESTNMT_MISSING       0x01 // CANopen device missing
#define COPMGR_REQUESTNMT_STOPPED       0x04 // NMT state STOPPED
#define COPMGR_REQUESTNMT_OPERATIONAL   0x05 // NMT state OPERATIONAL
#define COPMGR_REQUESTNMT_RESETNODE     0x06 // NMT service RESET NODE
#define COPMGR_REQUESTNMT_RESETCOMM     0x07 // NMT service RESET COMMUNICATION
#define COPMGR_REQUESTNMT_PREOPERATIONAL 0x7F // NMT state PREOPERATIONAL

// defines for object 0x1F83 "RequestNodeGuarding"
#define COPMGR_REQUESTGUARD_STOPPED     0x00 // node guarding stopped
#define COPMGR_REQUESTGUARD_STARTED     0x01 // node guarding started

// callback events                        // pEventArg_p union is:
//-------------------------------------------------------------------
typedef enum
{
    kCopMgrEventLssEvent =        0x01,    // m_LssEvent
    kCopMgrEventNmtEvent =        0x10,    // m_bNmtEvent
    kCopMgrEventRequestNmt =      0x11,    // m_bNmtCmd
    // d.k.: according to CiA WD-302 V3.4 RequestNMT 0x1F82 on own node shall be ignored
    kCopMgrEventCopError =        0x12,    // m_CopError
    // d.k.: Error in CAN driver -> there exists a CopError
    kCopMgrEventNodeCtrl =        0x13,    // m_NodeCtrl
    kCopMgrEventNodeEmcy =        0x14,    // m_NodeEmcy
    kCopMgrEventLgEvent =         0x15,    // m_LgEvent
    kCopMgrEventNmtCommand =      0x16,    // m_NmtCommand
    kCopMgrEventCobReceived =     0x40,    // m_Cob
    kCopMgrEventStartSlave =      0x60,    // ---
    kCopMgrEventStartMaster =     0x61,    // ---
    kCopMgrEventNodeSdo =         0x62,    // m_NodeSdo
    kCopMgrEventNodeBooted =      0x63,    // m_bNodeId
    kCopMgrEventNodeFail =        0x64,    // m_NodeConfig
    kCopMgrEventNodeWarn =        0x65,    // m_NodeConfig
    kCopMgrEventBootupOk =        0x66,    // ---
    kCopMgrEventBootupFail =      0x67,    // ---
    kCopMgrEventMissModule =      0x68,    // m_MissModule
    kCopMgrEventObdAccess =       0x69,    // m_ObdCbParam
    kCopMgrEventSrdCopError =     0x6A,    // m_SrdError (m_CopErrorCode): nur bei reinen Cop stack errors oder in NMT slave mode bei kCopSrdNoAnswerSdom oder kCopSrdSdomNotUndefSdoc
    kCopMgrEventSdomError =       0x6B,    // m_SrdError (m_SdomError)
    kCopMgrEventSdocReleased =    0x6C,    // m_bNodeId
    kCopMgrEventSrdRegCopError =  0x6D,    // m_SrdError (m_CopErrorCode): nur bei reinen Cop stack errors oder in NMT slave mode bei kCopSrdNoAnswerSdom oder kCopSrdSdomNotUndefSdoc
    kCopMgrEventSdomRegError =    0x6E,    // m_SrdError (m_SdomError)
    kCopMgrEventNodeConfigured =  0x6F,    // m_NodeConfig
    kCopMgrEventNodeGuardStart =  0x70,    // m_NodeConfig
    kCopMgrEventNodeGuardStop =   0x71,    // m_NodeConfig
    kCopMgrEventBootupWarn =      0x72    // ---

} tCopMgrEventType;
// d.k.: if there isn't any additional argument, shall we pass a NULL pointer?
//       no

#define COPMGR_ENABLE_SRD_SDOM          0x01 // enable SRD client and SDO Manager at runtime
#define COPMGR_ENABLE_CONF_HEARTBEAT    0x02 // enable auto configuration of heartbeat / life guarding at runtime
#define COPMGR_DISABLE_NODESCAN_RETRY   0x04 // disable retry of node scan if it fails with error status B and local node is in state OPERATIONAL
#define COPMGR_ENABLE_RESET_NOT_FOUND   0x08 // enable sending of NMT service RESET NODE if node fails with error status B
#define COPMGR_ENABLE_REQUESTNMT_LOCAL  0x10 // enable processing of RequestNMT commands (object 0x1F82) also on local node otherwise the local node is discarded

#define COPMGR_PROCESS_CHECKASYNCPDO    0x0001 // check all asynchronous PDOs in CopMgrProcess()

//---------------------------------------------------------------------------
// global types
//---------------------------------------------------------------------------

typedef enum
{
    kCopMgrSdocTypeAuto         = 0x00, // automatically detect SDO transfer type, that means first try block transfer if size is larger than COPMGR_MIN_SDOBLOCKTRANSFER_SIZE
    kCopMgrSdocTypeSegment      = 0x01, // use SDO segmented transfer
    kCopMgrSdocTypeBlock        = 0x02  // use SDO block transfer

} tCopMgrSdocType;

typedef enum
{
    kCopMgrError0_Ok                = 0x00, // node booted successfully
    kCopMgrErrorA_NodeNotListed     = 0x01, // node is not listed in 0x1F81
    kCopMgrErrorB_NoResponse        = 0x02, // no response received
    kCopMgrErrorC_WrongDeviceType   = 0x03, // 0x1000 from slave node is different to 0x1F84
    kCopMgrErrorD_WrongVendorId     = 0x04, // 0x1018/01 from slave node is different to 0x1F85
    kCopMgrErrorE_NoHeartbeat       = 0x05, // no heartbeat received from slave node
    kCopMgrErrorF_NoNodeGuardConf   = 0x06, // no confirmation for node guarding request received
    kCopMgrErrorG_NoExpSwVersion    = 0x07, // object 0x1F53 / 0x1F54 not configured
    kCopMgrErrorH_NoAutoSwUpdate    = 0x08, // 0x1F81 bit 6 not set, but 0x1F53 and 0x1F54 are configured but different to values from slave node
    kCopMgrErrorI_SwDownloadErr     = 0x09, // software download failed
    kCopMgrErrorJ_ConfDownloadErr   = 0x0A, // configuration download failed
    kCopMgrErrorK_NoFirstHeartbeat  = 0x0B, // no heartbeat during start error control service
    kCopMgrErrorL_NodeOperational   = 0x0C, // keepalive node was initially OPERATIONAL
    kCopMgrErrorM_WrongProductCode  = 0x0D, // 0x1018/02 from slave node is different to 0x1F86
    kCopMgrErrorN_WrongRevisionNo   = 0x0E, // 0x1018/03 from slave node is different to 0x1F87
    kCopMgrErrorO_WrongSerialNo     = 0x0F, // 0x1018/04 from slave node is different to 0x1F88
    kCopMgrErrorV_ConfigurationErr  = 0x16, // booting of node failed, CopMgr was not properly configured, look for kCopMgrEventCopError events
    kCopMgrErrorW_ManualMode        = 0x17, // NMT slave is in manual mode, because of RequestNMT
    kCopMgrErrorX_SdoTransferFailed = 0x18, // SDO transfer failed
    kCopMgrErrorY_SdoClientFailed   = 0x19, // SDO client index could not be defined
    kCopMgrErrorZ_SrdFailed         = 0x1A, // SRD client failed
    kCopMgrWarning                  = 0xD0, // general warning
    kCopMgrWarnT_ConfigWithErrors   = 0xF9, // configuration was not completly successfully
    kCopMgrWarnU_HbConsumerTooLow   = 0xFA, // heartbeat consumer time too low
    kCopMgrWarnV_NoGuarding         = 0xFB, // no guarding available at all at slave node, this is not standard compliant (CiA 301)
    kCopMgrWarnW_NoFreeHbConsumer   = 0xFC, // no free heartbeat consumer found
    kCopMgrWarnX_NoGuardTime        = 0xFD, // no GuardTime specified in 0x1F81
    kCopMgrWarnY_NoLifeTimeFactor   = 0xFE, // no LifeTimeFactor specified in 0x1F81
    kCopMgrWarnZ_GuardTimeTooLow    = 0xFF  // GuardTime on slave too low

} tCopMgrErrorStatus;

typedef struct
{
    tCopKernel m_ErrorCode;         // Error in CANopen stack
    void MEM*  m_pArg;              // pointer to additional information

} tCopMgrCopError;

typedef struct
{
    BYTE    m_bLssEvent;            // LSS Ereignis (in CANopen definiert)
    BYTE    m_bNodeId;              // neue Knotennummer
    BYTE    m_bBtrTableSelector;    // neuer Baudratenselektor
    BYTE    m_bBtrIndex;            // neuer Baudratenindex

} tCopMgrCfgLssEvent;

typedef struct
{
    BYTE    m_bNodeId;    // node id of the guarded node
    BYTE    m_bCtrlEvent; // event (e.g.:
                          // 0x20 = Bootup received
                          // 0x30 (Life Guard Master) ...
                          // 0x40 (Heartbeat Consumer) ...
                          // -> as defined in CANopen stack.
    BYTE    m_bNodeState; // NMT state of the node (according to DS-301)
    BYTE    m_bRequestNmtState; // NMT state of the node (according to 0x1F82 in CiA 302)

} tCopMgrNodeCtrl;

typedef struct
{
    BYTE    m_bNodeId;        // Kontenadresse des Emergency Producers
    WORD    m_wEmcErrorCode;  // Emergency Error Code nach DS-301, ...
    BYTE    m_bErrorReg;      // Error Register (Index 0x1001 des Slave)
    BYTE    m_abUserCode[5];  // Manufacturer Error Code

} tCopMgrNodeEmcy;

/*typedef struct
{
    tCopKernel  m_PdoErrorCode;   // eigenlicher Fehlercode (Std. CANopen)
    WORD        m_wPdoCommuIndex; // identifiziert welches PDO fehlerhaft
    WORD        m_wVarIndex;      // Index der Mappingvariable
    BYTE        m_bVarSubindex;   // Subindex der Mappingvariable
    BYTE        m_bAdditionalRet; // zusätzlicher Fehlercode 
                                  // (z.B. Fehlercode aus COB-Modul, der
                                  // diesen Fehler auslöste)
} tCopMgrPdoError;
*/

/*    typedef struct
{
    BYTE    m_bNodeId;        // Kontenadresse des konfigurierten Knoten
    WORD    m_wIndex;         // Index bei dem es fehl schlug
    BYTE    m_bSubindex;      // Subindex bei dem es fehl schlug
    DWORD   m_dwAbortCode;    // SDO Abort Code
    BYTE    m_bCallerId;      // Wer hat Fehler festgestellt?
                              // gleich 0   -> eigener Knoten
                              // ungleich 0 -> zu konfigurierender Knoten
} tCopMgrNodeConfig;
*/

typedef struct
{
    BYTE    m_bNodeId;        // node id
    DWORD   m_dwAbortCode;    // SDO Abort Code (0 wenn SUCCESSFUL)
    DWORD   m_dwTransmittedBytes;   // transmitted bytes

    // d.k.: the following elements are unknown when SDO tranfer is finished
    // d.k.: maybe pass value of SdocGetTransmittedBytes()
/*    WORD    m_wIndex;         // Index bei dem es fehl schlug
    BYTE    m_bSubindex;      // Subindex bei dem es fehl schlug
    DWORD   m_dwReadSize;     // Anzahl der gelesenen Bytes*/

} tCopMgrSdoFinish;

typedef struct
{
    BYTE    m_bNodeId;        // node id of the configured node
    tCopMgrErrorStatus m_ErrorStatus;     // Error status code according to CiA WD-302 (A-O)
                              // 0 = Successful
                              // 1 = A: slave no longer exists in node list
                              // ...
    BYTE    m_fMandatory;     // node is marked as mandatory in 0x1F81 (only set on event kCopMgrEventNodeFail)

} tCopMgrNodeConfig;

typedef enum
{
    kCopMgrMissNmtm = 0x01,
    kCopMgrMissNmts = 0x02

} tCopMgrMissModule;

typedef struct
{
    BYTE            m_ObdEvent;
    WORD            m_wIndex;
    BYTE            m_bSubIndex;
    void GENERIC*   m_pArg;
    DWORD           m_dwAbortCode;

} tCopMgrObdCbParam;

typedef struct
{
    tCanId          m_CobId;
    BYTE *          m_pbData;
    BYTE            m_bSize;

} tCopMgrCob;

typedef union
{
    tCopMgrCfgLssEvent  m_LssEvent;
    BYTE                m_bNmtEvent;
    BYTE                m_bNmtCmd;
    tCopMgrMissModule   m_MissModule;  // fehlender Dienst

    tCopMgrCopError     m_CopError;
    tCopMgrNodeCtrl     m_NodeCtrl;
    tCopMgrNodeEmcy     m_NodeEmcy;
    tNmtErrCtrlEvent    m_LgEvent;
    tNmtCommand         m_NmtCommand;
    //tCopMgrPdoError     m_PdoError;

    BYTE                m_bNodeId;
    tCopMgrSdoFinish    m_NodeSdo;   // d.k.: SDO -> Sdo
    //tCopMgrNodeScan    m_NodeScan;
    //tCopMgrNodeConfig  m_NodeConfig;
    tCopMgrNodeConfig   m_NodeConfig;
    tCopMgrObdCbParam   m_ObdCbParam;
    tSrdEventErrorArg   m_SrdError;
    tCopMgrCob          m_Cob;

} tCopMgrEventArg;

// d.k.: convert bEventType_p to enum
typedef tCopKernel (PUBLIC ROM* tCopMgrCbEvent) (CCM_DECL_INSTANCE_HDL_
    tCopMgrEventType        EventType_p,   // IN: event type (enum)
    tCopMgrEventArg MEM*    pEventArg_p,   // IN: event argument (union)
    void GENERIC*           pUserArg_p);

typedef struct
{
    WORD            m_wSizeOfStruct;    // used in future to recognize new parameters
    BYTE            m_bNodeID;          // local node ID

    // OD parameters
    tObdInitRam     m_fpOdbInitRam;
    DWORD           m_dwDeviceType;     // Device Profile   (0x1000/0 local OD)
    DWORD           m_dwVendorId;       // Vendor ID        (0x1018/1 local OD)
    DWORD           m_dwProductCode;    // Produkt code     (0x1018/2 local OD)
    DWORD           m_dwRevision;       // Revisions number (0x1018/3 local OD)
    DWORD           m_dwSerNum;         // Serial number    (0x1018/4 local OD)
    char*           m_pszDevName;       // Device name      (0x1008/0 local OD)
    char*           m_pszHwVersion;     // Hardware version (0x1009/0 local OD)
    char*           m_pszSwVersion;     // Software version (0x100A/0 local OD)

    // callback parameters
    tCopMgrCbEvent  m_fpCbEvent;        // callback function
    void GENERIC*   m_pUserArg;         // argument for callback function

    #if ((COP_USE_CDRV_FUNCTION_POINTER != FALSE) && !defined (COP_WINDOWS_DLL))

        tCobCdrvFct GENERIC* m_pCdrvFct;

    #endif

} tCopMgrInitParam;

typedef struct
{
    WORD    m_wSizeOfStruct;  // used in future to recognize new parameters
    tTime   m_wBootSlaveInitialWait; // initial wait time before start
                                     // boot slave (after Reset Comm.) in 100 us
    tTime   m_wBootSlaveWaitTime;  // wait time before (re-)trying to
                                   // boot slave in 100 us
                                   // default: 1 s (DSP-302 V3.3.0 Figure 3)
    tTime   m_wBootSlaveWaitTime2;  // wait time before (re-)trying to
                                   // boot slave, when in state Ready (OPERATIONAL) in 100 us
                                   // default: 1 s (DSP-302 V3.3.0 Figure 3)
    tTime   m_wResetWaitTime;       // wait time between two consecutive
                                    // NMT Reset Communication to individual slave nodes
                                    // at beginning of state kCopMgrConfig in 100 us
    tTime   m_wNodeGuardConfTimeOut; // Node guard confirmation timeout in 100 us
                                     // default: 100 ms (DSP-302 V3.3.0 Figure 6)
    WORD    m_wToggleFeatures; // enable/disable several features at runtime

// SRD client
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRD) != 0)
    BYTE    m_bSrdRetryCount; // Retry count for connections to SDO Manager
    tTime   m_wSrdTimeOut;    // Timeout for connections to SDO Manager in 100 us
#endif

// SDO manager
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOM) != 0)
    tTime   m_wSdomSdocTimeOut; // timeout for SDO transfer during SRD Client scan
#endif

// CmAutoHb - auto configuration of heartbeat / lifeguarding
#if ((COPMGR_FEATURES & COPMGR_FEAT_CONF_HEARTBEAT) != 0)
    WORD    m_wHbSafetyFactor;      // factor for heartbeat time (hb consumer time = hb producer time + m_wHbSafetyFactor * hb producer time / 100)
    tTime   m_wHbProducerTime;      // default heartbeat producer time in 100 us
    WORD    m_wLgSafetyFactor;      // factor for guard time (0x100C = guard time in 0x1F81 + m_wLgSafetyFactor * guard time in 0x1F81 / 100)
    tTime   m_wLgGuardTime;         // default guard time in 100 us
    BYTE    m_bLgRetryFactor;       // default retry factor for life/node guarding
#endif

} tCopMgrConfigParam; // all parameters are input values


//---------------------------------------------------------------------------
// global vars
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// global function prototypes
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrInitialize (CCM_DECL_PTR_INSTANCE_HDL_
    tCopMgrInitParam GENERIC* pMgrInitParam_p,
    tCdrvInitParam GENERIC*   pCanInitParam_p);

// konfiguriert verschiedene Timeouts und Wiederholraten
COPDLLEXPORT tCopKernel PUBLIC CopMgrConfig (CCM_DECL_INSTANCE_HDL_
    tCopMgrConfigParam GENERIC* pMgrConfigParam_p); // IN: Config Strukt

// ----------------------------------------------------------------------------
//
// Function:    CopMgrProcessLssInitState()
//
// Description: runs LSS process for 'invalid NodeId'
//              if NodeId is valid or has been configurated then function returns
//              with kCopSuccessful instatt of kCopLsssInvalidNodeId
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pNextCallTime_p         = pointer to next call time
//              LowBuffProcessCount_p   = number of processed CAN messages in
//                                          low priority buffer
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSS) != 0)
COPDLLEXPORT tCopKernel PUBLIC CopMgrProcessLssInitState (CCM_DECL_INSTANCE_HDL_
    tTime MEM* pNextCallTime_p, BYTE LowBuffProcessCount_p);
#endif



//---------------------------------------------------------------------------
//
// Function:    CopMgrStartBootUp()
//
// Description: sets NMT state machine to PRE-OPERATIONAL
//              sets the CANopen Manager to the state Bootup.
//              Has to be called also if CANopen Manager is configured as NMT slave!
//
// Parameters:  CCM_DECL_INSTANCE_HDL
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrStartBootUp (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CcmMgrProcess()
//
// Description: runs through all process functions of CANopen
//              and process the own global state machine and the state machines
//              of each slave node in queue.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              pNextCallTime_p         = pointer to next call time
//              bLowBuffProcessCount_p  = number of processed CAN messages in
//                                          low priority buffer
//              wFlags_p                = flags (e.g. check async PDOs)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrProcess (CCM_DECL_INSTANCE_HDL_
    tTime MEM* pNextCallTime_p, BYTE bLowBuffProcessCount_p, WORD wFlags_p);


// ----------------------------------------------------------------------------
//
// Function:    CopMgrEnterOperational()
//
// Description: sends NMT Start All Remote Nodes if bit 1 = 1 and bit 3 = 0 (of 0x1F80)
//              and local node enters NMT state Operational.
//              If the node is already in NMT state Operational, it silently ignores
//              the command and does nothing.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              fForce_p                = IN: if FALSE, only if all mandatory
//                                        slaves are booted the state will be changed.
//                                        It has to be TRUE also if the node is in NMT slave mode
//                                        and the application wants to enter state Operational
//                                        manually.
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrEnterOperational (CCM_DECL_INSTANCE_HDL_
    BYTE fForce_p);  // IN: Wenn TRUE, dann wird ohne Beachtung von 0x1F80
                     //     Bit 2 in Operational gewechselt.


#if ((COPMGR_FEATURES & COPMGR_FEAT_INCL_ADD_INFO) != 0)

// ----------------------------------------------------------------------------
//
// Function:    CopMgrSetAdditionalInfo()
//
// Description: saves additional information from the application in the internal node list
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//              pAddInfo_p              = IN: pointer to something
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrSetAdditionalInfo (CCM_DECL_INSTANCE_HDL_
    BYTE          bNodeId_p,    // IN: Knotennummer
    void GENERIC* pAddInfo_p);  // IN: Pointer auf zus. Informationen


// ----------------------------------------------------------------------------
//
// Function:    CopMgrGetAdditionalInfo()
//
// Description: returns saved additional information for a specified node
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//              pAddInfo_p              = OUT: pointer to something
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrGetAdditionalInfo (CCM_DECL_INSTANCE_HDL_
    BYTE          bNodeId_p,    // IN: Knotennummer
    void GENERIC* pAddInfo_p);  // IN: Pointer auf zus. Informationen

#endif // ((COPMGR_FEATURES & COPMGR_FEAT_INCL_ADD_INFO) != 0)

// ----------------------------------------------------------------------------
//
// Function:    CopMgrGetSlaveCfgState()
//
// Description: returns the configuration state of the specified node
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//              pdwConfigState_p        = OUT: pointer to bitmask
//                                             like sub-indexes of 0x1F81
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrGetSlaveCfgState (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p,          // IN:  Knotennummer
    DWORD MEM* pdwConfigState_p);  // OUT: Pointer auf Daten


// ----------------------------------------------------------------------------
//
// Function:    CopMgrSetSlaveCfgState()
//
// Description: sets the configuration state of the specified node
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//              dwConfigState_p         = IN: bitmask of SlaveAssignment
//                                             like sub-indexes of 0x1F81
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrSetSlaveCfgState (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p,          // IN:  Knotennummer
    DWORD      dwConfigState_p);  // IN: SlaveAssignment


// ----------------------------------------------------------------------------
//
// Function:    CopMgrGetSlaveNmtState()
//
// Description: returns the NMT state of the specified node
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//              pbNodeState_p           = OUT: pointer to node state
//                                             like sub-indexes of 0x1F82
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrGetSlaveNmtState (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p,       // IN:  Knotennummer
    BYTE MEM* pbNodeState_p);   // OUT: Pointer aud Daten


// ----------------------------------------------------------------------------
//
// Function:    CopMgrSetSlaveNmtState()
//
// Description: sets the NMT state of the specified node, which means to send
//              the appropriate NMT command
//              only allowed if it is currently master
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//              bNodeState_p            = INT: new node state
//                                             like sub-indexes of 0x1F82
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrSetSlaveNmtState (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p,       // IN:  Knotennummer
    BYTE       bNodeState_p);   // IN:  Knotenstatus


// ----------------------------------------------------------------------------
//
// Function:    CopMgrSaveSlaveNmtState()
//
// Description: saves the NMT state of the specified node in the internal
//              RequestNMT array (0x1F82).
//              This function is useful for application specific nodeguarding
//              protocols. The application could save the NMT state of a node
//              via this function and get the NMT state via CopMgrGetSlaveNmtState().
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//              bNodeState_p            = IN: node state
//                                             like sub-indexes of 0x1F82
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrSaveSlaveNmtState (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p,
    BYTE       bNodeState_p);


// ----------------------------------------------------------------------------
//
// Function:    CopMgrRequestNodeGuarding()
//
// Description: Starts resp. stops node guarding of the specified slave node.
//              If auto configuration of heartbeat / life guarding is enabled,
//              additionally it sets up resp. cancels life guarding on the
//              specified slave node if necessary.
//              CopMgrRequestNodeGuarding() can be called only if CopMgr is in
//              state READY, otherwise it returns kCopMgrInvalidOperation.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//                                            0x80 = all nodes
//              bStartStop_p            = IN: if COPMGR_REQUESTGUARD_STOPPED stop guarding
//                                             else start guarding
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0)
COPDLLEXPORT tCopKernel PUBLIC CopMgrRequestNodeGuarding (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p,
    BYTE       bStartStop_p);
#endif

// ----------------------------------------------------------------------------
//
// Function:    CopMgrGetNodeGuarding()
//
// Description: returns the node guarding state of the specified
//              node is active or not.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//              pbNodeGuard_p           = OUT: pointer to node guarding state
//                                             like sub-indexes of 0x1F83
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrGetNodeGuarding (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p,
    BYTE MEM* pbNodeGuard_p);

// ----------------------------------------------------------------------------
//
// Function:    CopMgrRequestConfiguration()
//
// Description: Requests the configurion of the specified slave node.
//              If the Configuration Manager is included the concise DCF stored in
//              object 0x1F22 is used.
//              If auto configuration of heartbeat / life guarding is enabled
//              and no concise DCF is present, heartbeat resp. life guarding will
//              be configured for and on this node.
//              CopMgrRequestConfiguration() can be called only if CopMgr is in
//              state READY, otherwise it returns kCopMgrInvalidOperation.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//                                            0x80 = all nodes
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if (((COPMGR_FEATURES & COPMGR_FEAT_INCL_SLAVEASSIGN) != 0) \
     && (((CCM_MODULE_INTEGRATION & CCM_MODULE_CFGMA) != 0) \
         || ((COPMGR_FEATURES & COPMGR_FEAT_CONF_HEARTBEAT) != 0)))
COPDLLEXPORT tCopKernel PUBLIC CopMgrRequestConfiguration (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p);
#endif


// ----------------------------------------------------------------------------
//
// Function:    CopMgrReadObject()
//
// Description: reads the specified entry from the OD of the specified node.
//              If this node is a remote node, it performs a SDO transfer, which
//              means this function returns kCopMgrTaskDeferred and the application
//              is informed via the callback function when the task is completed.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID (0 = itself)
//              wIndex_p                = IN: index of object in OD
//              bSubindex_p             = IN: sub-index of object in OD
//              pDstData_p              = OUT: pointer to data
//              pSize_p                 = INOUT: pointer to size of data
//              SdocType_p              = IN: type of SDO transfer
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrReadObject (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p,
    WORD       wIndex_p,
    BYTE       bSubindex_p,
    void MEM*  pDstData_p,
    tObdSize MEM* pSize_p,
    tCopMgrSdocType SdocType_p);


// ----------------------------------------------------------------------------
//
// Function:    CopMgrWriteObject()
//
// Description: writes the specified entry to the OD of the specified node.
//              If this node is a remote node, it performs a SDO transfer, which
//              means this function returns kCopMgrTaskDeferred and the application
//              is informed via the callback function when the task is completed.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID (0 = itself)
//              wIndex_p                = IN: index of object in OD
//              bSubindex_p             = IN: sub-index of object in OD
//              pSrcData_p              = IN: pointer to data
//              Size_p                  = IN: size of data in bytes
//              SdocType_p              = IN: type of SDO transfer
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrWriteObject (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p,
    WORD       wIndex_p,
    BYTE       bSubindex_p,
    void MEM*  pSrcData_p,
    tObdSize   Size_p,
    tCopMgrSdocType SdocType_p);


// ----------------------------------------------------------------------------
//
// Function:    CopMgrReadLocalObject()
//
// Description: reads the specified entry from the local OD.
//
// Parameters:  wIndex_p                = IN: index of object in OD
//              bSubindex_p             = IN: sub-index of object in OD
//              pDstData_p              = OUT: pointer to data in platform byte order
//              pSize_p                 = INOUT: pointer to size of data
//
// Return:      tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrReadLocalObject(CCM_DECL_INSTANCE_HDL_
        WORD              wIndex_p,
        BYTE              bSubindex_p,
        void*             pDstData_p,
        tObdSize*         pSize_p);


// ----------------------------------------------------------------------------
//
// Function:    CopMgrWriteLocalObject()
//
// Description: writes the specified entry to the local OD.
//
// Parameters:  wIndex_p                = IN: index of object in OD
//              bSubindex_p             = IN: sub-index of object in OD
//              pSrcData_p              = IN: pointer to data in platform byte order
//              Size_p                  = IN: size of data in bytes
//
// Return:      tEplKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrWriteLocalObject(CCM_DECL_INSTANCE_HDL_
            WORD              wIndex_p,
            BYTE              bSubindex_p,
            void*             pSrcData_p,
            tObdSize          Size_p);


// ----------------------------------------------------------------------------
//
// Function:    CopMgrFreeSdoChannel()
//
// Description: release SDO client index, which is reserved for the specified
//              slave node.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//
// Returns:     tCopKernel              = error code
//              kCopSdocBusy            = SDO transfer is still running
//                                        call CopMgrAbortSdoTransfer() first
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrFreeSdoChannel (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p);   // IN:    Knotenadresse


// ----------------------------------------------------------------------------
//
// Function:    CopMgrAbortSdoTransfer()
//
// Description: aborts the currently running SDO transfer to the specified
//              slave node.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = IN: node ID
//              dwAbortCode_p           = IN: SDO abort code
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrAbortSdoTransfer (CCM_DECL_INSTANCE_HDL_
    BYTE       bNodeId_p,   // IN:    Knotenadresse
    DWORD  dwAbortCode_p);  // IN:    SDO abort code


// ----------------------------------------------------------------------------
//
// Function:    CopMgrShutDownCANopen()
//
// Description: deletes an instance of CANopen
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// State:
//
// ----------------------------------------------------------------------------
#if (COP_USE_DELETEINST_FUNC != FALSE)
COPDLLEXPORT tCopKernel PUBLIC CopMgrShutDownCANopen (CCM_DECL_INSTANCE_HDL);
#endif

// ----------------------------------------------------------------------------
// Function:     CopMgrDefineVarTab()
//
// Description:  defines variables for application from a table
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               pVarTab_p              = start address of table
//               wNumOfVars_p           = number of variables to define
//
// Returns:      tCopKernel             = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrDefineVarTab (CCM_DECL_INSTANCE_HDL_
    tVarParam GENERIC* pVarTab_p,
    WORD               wNumOfVars_p);

#endif //_COPMGR_H_

// Please keep an empty line at the end of this file.
