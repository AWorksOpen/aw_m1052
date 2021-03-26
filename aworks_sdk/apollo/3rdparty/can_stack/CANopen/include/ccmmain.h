/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  Header for all files of CCM module
                it contains typedef of struct for instance table

  -------------------------------------------------------------------------

                $RCSfile: CcmMain.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.75 $  $Date: 2007/01/08 14:00:56 $

                $State: Exp $

                Build Environment:
                    KEIL uVision

  -------------------------------------------------------------------------

  Revision History:

    2005/12/16  k.t.:   - Add Membervariables for SDO Manager and SRD Client
    2005/12/21  k.t.:   - Add new marcos from r.d. for linking Var to OD entry
    2005/12/22  k.t.:   - Add some structures for SDO Manager and SRD Client
                          from cop.h

****************************************************************************/


#ifndef _CCMMAIN_H_
#define _CCMMAIN_H_

#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_COPMGR) != 0)
    #include "copmgr.h"
#endif

#if !defined (CFGMA_MAX_SDO_CLIENTS)
    #if defined (NET_MAX_SDO_CLIENTS)
        #define CFGMA_MAX_SDO_CLIENTS       NET_MAX_SDO_CLIENTS
    #else
        #define CFGMA_MAX_SDO_CLIENTS       4
    #endif
#endif

// 16.07.2004 r.d.:  defines set for Windows DLL
#if (defined (COP_WINDOWS_DLL)) || ((CCM_MODULE_INTEGRATION & CCM_MODULE_COPMGR) != 0)
    #define HBC_MAX_EMCY_VALUES         126
    #define PDO_MAX_EMCY_VALUES         512
#else
    #define HBC_MAX_EMCY_VALUES         10
    #define PDO_MAX_EMCY_VALUES         3
#endif

// 21.12.2005 k.t.: add new macros from r.d.
//-----------------------------------------------------------------------
// new macros for declarating varaiables in OD

//-----------------------------------------------------------------------
// new macros for declarating varaiables in OD

// deklaration of variables which will be linked with OD
#define CCM_DECL_OD_VARIABLE(type,name)     type name
#define CCM_DECL_OD_ARRAY(type,name,count)  type name[count]

// linking of instance variables with OD
#define CCM_LINK_OD_VARIABLE(inst,name)     aCcmInstanceTable_g[inst].name
#define CCM_LINK_OD_ARRAY(inst,name,idx)    aCcmInstanceTable_g[inst].name[idx]

// macros to refer instance variables which are linked with OD
#define CCM_REFER_OD_VARIABLE(name)         CCM_INST_ENTRY.name
#define CCM_REFER_OD_ARRAY(name,idx)        CCM_INST_ENTRY.name[idx]
//-----------------------------------------------------------------------



// =========================================================================
// types
// =========================================================================

#if (defined (OBD_USE_DYNAMIC_OD) && (OBD_USE_DYNAMIC_OD != FALSE))

    typedef union
    {
        tObdVarEntry    m_ObdVarEntry;
        tObdVString     m_ObdVString;

    } tDynVarEntry;

    typedef struct
    {
        tObdEntryPtr            m_pIndexTab;
        tObdSubEntryPtr         m_pSubindexTab;
        BYTE            MEM*    m_pbCurDataTab;
        tDynVarEntry    MEM*    m_pVarEntryTab;
        tPdoTabEntry    MEM*    m_pRxPdoTab;
        tPdoTabEntry    MEM*    m_pTxPdoTab;

    } tDynOdMemInfo;

    typedef struct
    {
        BYTE  FAR*  m_pbIndexSeg;
        BYTE  FAR*  m_pbAddressSeg;
        BYTE  FAR*  m_pbExtendedSeg;
        BYTE  FAR*  m_pbPIParamSeg;

        WORD        m_wIndexCount;
        DWORD       m_dwSubindexCount;
        DWORD       m_dwCurDataCount;
        WORD        m_wVarEntryCount;
        WORD        m_wRxPdoCount;
        WORD        m_wTxPdoCount;

    } tDynSegContInfo;

#endif

//configuration Manager for extended systec format
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_CFGMASYS) != 0)
/*
    //define max number of timeouts for configuration of 1 device
    #define CFGMA_MAX_TIMEOUT           3

    //callbackfunction, called if configuration is finished
    typedef void (PUBLIC ROM* tfpCfgMaCb) (CCM_DECL_INSTANCE_HDL_
        BYTE bNodeId_p, tCopKernel Errorstate_p);

    //State for configuartion manager Statemachine
    typedef enum
    {
        // general states
        kCfgMaIdle                  = 0x0000,     // Konfigurationsprozess ist in Ruhe, und kann verwendet werden
        kCfgMaStartConfiguration    = 0x0001,     // start configuration process
        kCfgMaWaitForSdocEvent      = 0x0002,     // wait until the last SDOC is finisched
        kCfgMaWaitReadingCobIdMap   = 0x0004,     // wait until the last SDOC is finisched for reading PDO COB-Id for Config Mapping
        kCfgMaWaitReadingCobIdPdo   = 0x0008,     // wait until the last SDOC is finisched for reading PDO COB-Id for Config Pdo
        kCfgMaWaitReadingCobIdEmer  = 0x0010,     // wait until the last SDOC is finisched for reading Emergency COB-Id for Config
        kCfgMaWaitReadingCobIdSdo1  = 0x0020,     // wait until the last SDOC is finisched for reading 1. SDO COB-Id (Slave or Master) for Config
        kCfgMaSkipMappingSub0       = 0x0040,     // write Sub0 of mapping parameter with 0
        kCfgMaWaitReadingCobIdSdo2  = 0x0080,     // wait until the last SDOC is finisched for reading 2. SDO COB-Id (Slave or Master) for Config
        kCfgMaWaitWriteCobIdSdo2    = 0x0100,     // wait until write of the 2. SDO COB ID is ready
        kCfgMaFinished              = 0x0200      // configuartion is finisched

        //...

    } tCfgState;

    typedef enum
    {
        kCfgMaCommon                = 0,        // all other index
        kCfgMaPdoComm               = 1,        // communication index
        kCfgMaPdoMapp               = 2,        // mapping index
        kCfgMaPdoCommAfterMapp      = 3,        // write PDO Cob-Id after mapping subindex 0(set PDO valid)
        kCfgMaEmerCobId             = 4,        // index of the emergency cob id
        kCfgMaSdo                   = 5         // SDO Server or SDO Client has to setup

    } tCfgMaIndexType;


    //bitcodoiert die Antwort ueber den Stand des letzen SDO geben
    //in Variable m_bSdocState
    // also used to singal start of the State Maschine
    #define CFGMA_SDOC_BUSY             0       // SDOC aktiv, es muss gewartet werden
    #define CFGMA_SDOC_READY            1       // SDOC abgeschlossen
    #define CFGMA_SDOC_TIMEOUT          2       // SDOC Timeout
    #define CFGMA_SDOC_ABORT_RECEIVED   4       // SDOC Abort, see Abortcode
    #define CFGMA_START                 8       // START State Mschine


    //internal structure (instancetable for modul configuration manager)
    typedef struct
    {
        DWORD           m_dwLastAbortCode;
        WORD            m_wSdocIndex;           // index from the SDOC with used
        WORD            m_wLastIndex;           // last index of configuration, to compair with actual index
        BYTE FAR *      m_pbSegment;            // Ptr to segmentstructure with DCF
        BYTE FAR *      m_pbActualIndex;        // Ptr to actual index in the DCF segment
        BYTE FAR *      m_pbExtendedSeg;        // Ptr to extented segment with Access rights
        tfpCfgMaCb      m_pfnCfgMaCb;           // Ptr to CfgMa Callback, is call if configuration finished
        tCfgState       m_CfgState;             // configuration state for statemachine
        tCopKernel      m_CopKernelError;       // Fehlercode
        DWORD           m_dwNumValueCopy;       // numeric values are copied in this variable
        DWORD           m_dwPdoCobId;           // Zwischenspeicher fuer den letzten PDO COB
                                                // weil ein PDO immer invalid sein muss, bevor
                                                // der TxType geschrieben werden kann
        DWORD           m_dwEmerCobId;          // buffer emergency cob id, first set old invalid before
                                                // write new one
        DWORD           m_dwSdoCobId;           // 1. COB ID of a SDO Server or Client for config
        BYTE            m_bNrOfMappedObject;    // number of mapped objects
        BYTE            m_bNodeId;              // Cop node addresse
        BYTE            m_bSdocState;           // bitcodiert der Erfolgt des SDO
        BYTE            m_bLastSubIndex;        // last subindex of configuration
        BYTE            m_bEventFlag;           // for Eventsignaling to the State MAschine
        DWORD           m_dwCntObjectInDcf;     // number of Objects in DCF
        tCfgMaIndexType m_SkipCfg;              // TRUE wenn ein zusätzlicher Konfigurationsvorgang
                                                // eingefügt wurde z.B. bei Mapping-Parametern
        WORD            m_wTimeOutCnt;          // Timeout Counter, break configuration is
                                                // m_wTimeOutCnt == CFGMA_MAX_TIMEOUT

    } tCfgMaNode;
*/
#endif //configuration Manager for extended systec format

    // the following part is for the configuration manager
    // which works with concise format
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_CFGMA) != 0)

    #include "ccmcfgma.h"
/*
    //define max number of timeouts for configuration of 1 device
    #define CFGMA_MAX_TIMEOUT   3

    //callbackfunction, called if configuration is finished
    typedef void (PUBLIC ROM* tfpCfgMaCb) (CCM_DECL_INSTANCE_HDL_
        BYTE bNodeId_p, tCopKernel Errorstate_p);

    //State for configuartion manager Statemachine
    typedef enum
    {
        // general states
        kCfgMaIdle                  = 0x0000,     // Konfigurationsprozess
                                                  // ist in Ruhe, und kann
                                                  // verwendet werden
        kCfgMaWaitForSdocEvent      = 0x0001,     // wait until the last
                                                  // SDOC is finisched
        kCfgMaWaitReadingCobIdMap   = 0x0002,     // wait until the last
                                                  // SDOC is finisched for
                                                  // reading PDO COB-Id for
                                                  // Config Mapping
        kCfgMaWaitReadingCobIdPdo   = 0x0004,     // wait until the last
                                                  // SDOC is finisched for
                                                  // reading PDO COB-Id for
                                                  // Config Pdo
        kCfgMaWaitReadingCobIdEmer  = 0x0008,     // wait until the last
                                                  // SDOC is finisched for
                                                  // reading Emergency COB-
                                                  // Id for Config
        kCfgMaWaitReadingCobIdSdo1  = 0x0010,     // wait until the last
                                                  // SDOC is finisched for
                                                  // reading 1. SDO COB-Id
                                                  // (Slave or Master) for
                                                  // Config
        kCfgMaSkipMappingSub0       = 0x0020,     // write Sub0 of mapping
                                                  // parameter with 0
        kCfgMaWaitReadingCobIdSdo2  = 0x0040,     // wait until the last
                                                  // SDOC is finisched for
                                                  // reading 2. SDO COB-Id
                                                  // (Slave or Master) for
                                                  // Config
        kCfgMaWaitWriteCobIdSdo2    = 0x0080,     // wait until write of
                                                  // the 2. SDO COB ID is
                                                  // ready
        kCfgMaFinished              = 0x0100      // configuartion is
                                                  //
    } tCfgState;


    typedef enum
    {
        kCfgMaCommon                = 0,        // all other index
        kCfgMaPdoComm               = 1,        // communication index
        kCfgMaPdoMapp               = 2,        // mapping index
        kCfgMaPdoCommAfterMapp      = 3,        // write PDO Cob-Id after mapping subindex 0(set PDO valid)
        kCfgMaEmerCobId             = 4,        // index of the emergency cob id
        kCfgMaSdo                   = 5         // SDO Server or SDO Client has to setup

    } tCfgMaIndexType;


    //bitcodoiert die Antwort ueber den Stand des letzen SDO geben
    //in Variable m_bSdocState
    // also used to singal start of the State Maschine
    #define CFGMA_SDOC_BUSY             0       // SDOC aktiv, es muss gewartet werden
    #define CFGMA_SDOC_READY            1       // SDOC abgeschlossen
    #define CFGMA_SDOC_TIMEOUT          2       // SDOC Timeout
    #define CFGMA_SDOC_ABORT_RECEIVED   4       // SDOC Abort, see Abortcode
    #define CFGMA_START                 8       // START State Mschine


    //internal structure (instancetable for modul configuration manager)
    typedef struct
    {
        DWORD           m_dwLastAbortCode;
        WORD            m_wSdocIndex;           // index from the SDOC with used
        WORD            m_wLastIndex;           // last index of configuration, to compair with actual index
        BYTE FAR *      m_pbConcise;            // Ptr to concise DCF
        BYTE FAR *      m_pbActualIndex;        // Ptr to actual index in the DCF segment
        tfpCfgMaCb      m_pfnCfgMaCb;           // Ptr to CfgMa Callback, is call if configuration finished
        tCfgState       m_CfgState;             // configuration state for statemachine
        tCopKernel      m_CopKernelError;       // Fehlercode
        DWORD           m_dwNumValueCopy;       // numeric values are copied in this variable
        DWORD           m_dwPdoCobId;           // Zwischenspeicher fuer den letzten PDO COB
                                                // weil ein PDO immer invalid sein muss, bevor
                                                // der TxType geschrieben werden kann
        DWORD           m_dwEmerCobId;          // buffer emergency cob id, first set old invalid befor
                                                // write new one
        DWORD           m_dwSdoCobId;           // 1. COB ID of a SDO Server or Client for config
        BYTE            m_bNrOfMappedObject;    // number of mapped objects
        BYTE            m_bNodeId;              // Cop node addresse
        BYTE            m_bSdocState;           // bitcodiert der Erfolgt des SDO
        BYTE            m_bLastSubIndex;        // last subindex of configuration
        BYTE            m_fOneTranferOk;        // atleased one transfer was successful
        BYTE            m_bEventFlag;           // for Eventsignaling to the State MAschine
        DWORD           m_dwCntObjectInDcf;     // number of Objects in DCF
        tCfgMaIndexType m_SkipCfg;              // TRUE wenn ein zusätzlicher Konfigurationsvorgang
                                                // eingefügt wurde z.B. bei Mapping-Parametern
        WORD            m_wTimeOutCnt;          // Timeout Counter, break configuration is
                                                // m_wTimeOutCnt == CFGMA_MAX_TIMEOUT

    } tCfgMaNode;
*/
#endif //configuration Manager for concise format

// ============================================================================
// typedef for SDO Manager Module
// ============================================================================

#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOM) != 0)


    // define Errors to inform SRD Client about
    typedef enum
    {
        kSdomErrorNoError           =   0x00,   // no error for SRD Client
        kSdomErrorNoFreeCob         =   0x01,   // no free COB-Ids
        kSdomErrorNoDefaultSdos     =   0x02,   // not all default SDOs free
        kSdomErrorSdosFail          =   0x03,   // SDO Server Life-Guarding
                                                // or Heartbeat failed
                                                // if a SRD Client want do know
                                                // -> inform it

                                                // not used jet !!!

        kSdomErrorSrdSdoFail        =   0x04,   // error during access
                                                // to SRD Client
        kSdomErrorNoFreeSdosSdom    =   0x05,   // no free SDO Server an SDO Manager
        kSdomErrorNoFreeSdosSlave   =   0x06,   // no free SDO Server an Slave Node
        kSdomErrorNoConnectSlave    =   0x07,   // no connect to Slave Node
        kSdomErrorNoFreeListSdom    =   0x08,   // no free space for registration of the
                                                // connection in SDO Manager
        kSdomErrorSrdFail           =   0x09    // Life-Guarding or Heartbeat Error of
                                                // SRD Client

    } tSdomSrdError;

    // Status which kind of connection has to establish
    typedef enum
    {
        kSdomStatusNoStatus         =   0x00,   // no status
        kSdomStatusNewSrd           =   0x01,   // new SRD Client has to register
        kSdomStatusAllDefaultSdos   =   0x02,   // SRD Client want all default SDOs
        kSdomStatusNewConnect       =   0x03,   // registered Client want new Connection
        kSdomStatusReleaseConnect   =   0x04    // registered Client want to release one
                                                // or all connections

    } tSdomStatus;

    // define Substatus to manage SDO Transfer
    typedef enum
    {
        kSdomSubStatusNoSdo         =   0x00,   // no SDO transfer
        kSdomSubStatusSdo1          =   0x01,   // first SDO Transfer in this
                                                // state runs
        kSdomSubStatusSdo1Ready     =   0x02,   // first SDO Transfer ready
        kSdomSubStatusSdo2          =   0x03,   // second SDO Transfer in this
                                                // state runs
        kSdomSubStatusSdo2Ready     =   0x04,   // second SDO Transfer Ready
        kSdomSubStatusSdo3          =   0x05,   // third SDO Transfer in this
                                                // state runs
        kSdomSubStatusSdo3Ready     =   0x06,   // third SDO Transfer Ready
        kSdomSubStatusSdo4          =   0x07,   // fourth SDO Transfer in this
                                                // state runs
        kSdomSubStatusSdo4Ready     =   0x08,   // fourth SDO Transfer Ready
        kSdomSubStatusSdo5          =   0x09,   // fifth SDO Transfer in this
                                                // state runs
        kSdomSubStatusSdo5Ready     =   0x0A,   // 6. SDO Transfer Ready
        kSdomSubStatusSdo6          =   0x0B,   // 6. SDO Transfer in this
                                                // state runs
        kSdomSubStatusSdo6Ready     =   0x0C,   // 7. SDO Transfer Ready
        kSdomSubStatusSdo7          =   0x0D,   // 7. SDO Transfer in this
                                                // state runs
        kSdomSubStatusSdo7Ready     =   0x0E,   // fifth SDO Transfer Ready
        kSdomSubStateCheckReady     =   0x0F,   // signals that SdomCheckFreeSdosSlave()
                                                // is ready
        kSdomSubStatusCheckSdo1     =   0x10,   // signals that SdomCheckFreeSdosSlave()
                                                // is working
        kSdomSubStatusCheckSdo2     =   0x11,   // signals that SdomCheckFreeSdosSlave()
                                                // is working
        kSdomSubStatusCheckSdo3     =   0x12,   // signals that SdomCheckFreeSdosSlave()
                                                // is working
        kSdomSubStatusSearching     =   0x13    // signals that the search for connection in
                                                // CCM_INST_ENTRY.m_SdomConStatus is in process
    } tSdomSubStatus;

    // defines an entry for m_aSrdInfoTable in tSdomParam
    typedef struct
    {
        BYTE    m_bCobSrdToSdosOffset; // Subindex in 0x1F02 für COB-ID SRD ->SDOS (SDOM)
        BYTE    m_bCobSdosToSrdOffset; // Subindex in 0x1F02 für COB-ID SDOS(SDOM)->SRD
    } tSrdInfoEntry;
    // defines the state of the SDO Manager state maschine
    typedef enum
    {
          kSdomIdle           =   0x00,    // SDO Manager State Maschine
                                           // in State SDOM_IDLE
          kSdomScanSrd        =   0x01,    // SDO Manager State Maschine
                                           // in State SDOM_SCAN_SRD
          kSdomWriteSrdSdo    =   0x02,    // SDO Manager State Maschine
                                           // in State SDOM_WRITE_SRD_SDO
          kSdomWriteSrdRes    =   0x03,    // SDO Manager State Maschine
                                           // in State SDOM_WRITE_SRD_RES
          kSdomWriteSdosSdo   =   0x04,    // SDO Manager State Maschine
                                           // in State SDOM_WRITE_SDOS_SDO
          kSdomScanAllSrd    =    0x05     // SDO Manager State Maschine
                                           // in State SDOM_SCAN_ALL_SRD
    } tSdomState;
    // central structur of the SDO Manager
    typedef struct
    {
        tSdomState      m_State;            // Status of the State Machine
        BYTE            m_fAllDefaultSdos;  // TRUE if one SRD Client
                                            // used all Default SDO Server

        // table for connections to SRD Clients and SRD Clients to Slaves
        tSrdInfoEntry   m_aSrdInfoTable[SDOM_CONNECT_LIST_COUNT];

    } tSdomParam;


    // contains infos for a new connection
    // the exact meaning of some membervariables depends on m_SdoStatus
    typedef struct
    {
        BYTE            m_bSrdNodeId;       // SRD Client Node-ID
        BYTE            m_bSdosNodeId;      // Node-Id of the SDO Server
                                            // can be 0 if not used
        BYTE            m_fSdoTransReady;   // TRUE if SDO is finished
                                            // a possible Error is shown in
                                            // m_SdomSrdError
        WORD            m_wSrdOdIndex;      // OD Index in SRD Client for new connection
                                            // if m_SdoStatus == kSdomStatusNewSrd
                                            // -> for connection SDOM - SRD
                                            // if m_SdoStatus == kSdomStatusNewConnect
                                            // -> for connection SRD - SDOS
                                            // can be 0 if not used
        WORD            m_wSdosOdIndex;     // OD Index in SDOS for new connection
                                            // will be set during process of
                                            // connecting SRD Client to SDO Server
                                            // or connecting SRD Client to SDO Manager
                                            // can be 0 if not used
        DWORD           m_adwFreeCobIds[2]; // Free Cob Ids for connection
                                            // m_adwFreeCobIds[0] -> COB-ID Client -> Server
                                            // m_adwFreeCobIds[1] -> COB-ID Server -> Client
        BYTE            m_abFreeCobIdsOffset[2];    // Offset in 0x1f02 of the COB-Ids in
                                                    // m_adwFreeCobIds
                                                    // necessary for fast release of COb-Ids
        tSdomStatus     m_SdomStatus;       // Status which kind of connection
                                            // has to establish
        tSdomSubStatus  m_SdomSubStatus;    // Substatus to manage SDO Transfers
        tSdomSrdError   m_SdomSrdError;     // Error -> inform SRD Client about

        BYTE            m_fForceDefSdos;    // saves if the default SDO Server should force
                                            // needed for CANopen Mgr during boot up and configuration

    } tSdomConStatus;

    // structure for errorcallback function
    // used if a Cop error occured
    typedef struct
    {
        tSdomConStatus*     SdomConStatus;  // pointer to SdomConStatus struct in SDO Manager
                                            // were error occured
        tCopKernel          CopError;       // Cop Error which occured

    } tSdomCopErrorConStatus;


#endif // SDO Manager Modul

// ============================================================================
// typedef for SRD Client Module
// ============================================================================
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRD) != 0)

    // States of the SRD Client State Maschine
    typedef enum
    {
        kSrdIdle            =   0x00,  // State SRD_IDLE
        kSrdWaitRegister    =   0x01,  // State SRD_WAIT_REGISTER
        kSrdWaitRequest     =   0x02,  // State SRD_WAIT_REQUEST
        kSrdWaitRelease     =   0x04,  // State SRD_WAIT_RELEASE
        kSrdWaitDefRequest  =   0x05,  // State SRD_WAIT_DEF_REQUEST
        kSrdWaitUndefSdoc   =   0x06   // State SRD_WAIT_UNDEF_SDOC

    } tSrdState;

// central structur of SRD Client
typedef struct
{
    BYTE        m_bState;           // State of the State Maschine
    BYTE        m_fSdomRegistered;  // TRUE if SRD Client is registered by the SDO Manager
    WORD        m_wSdocToManager;   // SDO Client for SRD Manager -> SDO Manager
                                    // 0 if all default SDO Server are asigned to this Node

    // Event Callback Function for interface function
    tSrdEventCallback   m_fpCallback;

} tSrdParam;

#endif // SRD Client Modul

#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_COPMGR) != 0)

    // internal type definitions, not used by application

    // State for CANopen Manager Statemachine
    typedef enum
    {
        // general states
        kCopMgrInit                 = 0x00,     // CopMgr inited
        kCopMgrBootup               = 0x01,     // CopMgr boots up
        kCopMgrConfig               = 0x02,     // CopMgr configures slaves
        kCopMgrSlave                = 0x03,     // CopMgr is a slave itself
        kCopMgrHalt                 = 0x04,     // CopMgr is halted (not all mandatory slaves are found)
        kCopMgrReady                = 0x05,     // CopMgr is operational and ready
        kCopMgrWaitApp              = 0x06      // CopMgr waits for application to enter state Ready / Operational
        //...

    } tCopMgrState;

    typedef union
    {
        DWORD               m_dwSdoData;
        tCopMgrErrorStatus  m_ErrorStatus;
    } tCopMgrNodeData;

    // internal node list type
    typedef struct
    {
        #if ((COPMGR_FEATURES & COPMGR_FEAT_INCL_ADD_INFO) != 0)
        // pointer to additional information from the application
        void GENERIC*   m_pAddInfo;
        #endif // ((COPMGR_FEATURES & COPMGR_FEAT_INCL_ADD_INFO) != 0)

        // internal information
        tCopMgrNodeData m_Data;             // data for SDO transfer and error status
        tTime           m_dwStartTime;      // TickCount bei der Bootphase begann
        WORD            m_wSdoClientIndex;  // SDO client index used for this slave node
        WORD            m_wInternalEvent;   // internal event
        BYTE            m_bInternalState;   // internal state (boot phase)

    } tCopMgrNodeInfo;

    // size of internal structures
    #define COPMGR_SIZE_QUEUE    127 // size of internal node priority queue

    #if ((COPMGR_FEATURES & COPMGR_FEAT_CONF_HEARTBEAT) != 0)

        #ifndef CMAUTOHB_MAX_ENTRIES
        #define CMAUTOHB_MAX_ENTRIES  5
        #endif

        typedef union
        {
            WORD            m_wValue;
            BYTE            m_abValue[sizeof (WORD) / sizeof (BYTE)];

        } tSplitWord;

        // internal node list type
        typedef struct
        {
            BYTE        m_bNodeId;          // node id
            BYTE        m_bInternalState;   // internal state
            BYTE        m_bInternalEvent;   // internal event
            tSplitWord  m_wSdoData;         // data for SDO transfer
            WORD        m_wSdoClientIndex;  // SDO client index used for this slave node

        } tCmAutoHbNodeInfo;

        typedef void (PUBLIC ROM* tCmAutoHbCbEvent) (CCM_DECL_INSTANCE_HDL_
            BYTE                    bNodeId_p,      // IN: node id
            tCopMgrErrorStatus      ErrorStatus_p); // IN: error status

    #endif // ((COPMGR_FEATURES & COPMGR_FEAT_CONF_HEARTBEAT) != 0)

#endif // CCM_MODULE_COPMGR


// The struct tCopInstanceInfo is used by different files in CCM module.
// Therfore instance variables has to be defined in an header file.
// But so it is necessary that it is a structure (not separate variables).

typedef struct
{

    // -----------------------------------------------------------------------------------------------------
    // variables for more instances than one
    #if (COP_MAX_INSTANCES > 1)

        tInstState          m_InstState;            // state of this instance (used or not)
        BYTE                m_InstNr;               // index of instance for using as handle for callback functions

        void MEM*           m_CdrvInstance;         // instance pointer to lower modules of CANopen
        void MEM*           m_CobInstance;          // these modules always exist
        void MEM*           m_ObdInstance;
        void MEM*           m_NmtInstance;
        void MEM*           m_SdosInstance;
        void MEM*           m_EmcpInstance;


        // instance pointer to additional modules

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSS) != 0)

            void MEM*       m_LsssInstance;         // instance pointer to LSS slave

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSS) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSM) != 0)

            void MEM*       m_LssmInstance;         // instance pointer to LSS master

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSM) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTS) != 0)

            void MEM*       m_NmtsInstance;         // instance pointer to NMT slave

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTS) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0)

            void MEM*       m_NmtmInstance;         // instance pointer to NMT master

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBP) != 0)

            void MEM*       m_HbpInstance;          // instance pointer to heartbeat producer

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBP) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBC) != 0)

            void MEM*       m_HbcInstance;          // instance pointer to heartbeat consumer

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBC) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_EMCC) != 0)

            void MEM*       m_EmccInstance;

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_EMCC) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_PDO) != 0)

            void MEM*       m_PdoInstance;          // instance pointer to PDO module

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_PDO) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRDO) != 0)

            void MEM*       m_SrdoInstance;          // instance pointer to SRDO module

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_PDO) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOC) != 0)

            void MEM*       m_SdocInstance;          // instance pointer to SDO client module

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOC) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOM) != 0)

            void MEM*       m_SdomInstance;          // instance pointer to SDO Manager module

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOM) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRD) != 0)

            void MEM*       m_SrdInstance;          // instance pointer to SRD Client module

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRD) != 0)

    #endif // (COP_MAX_INSTANCES > 1)

    // -----------------------------------------------------------------------------------------------------
    // variables which has to be defined in all projects

    tCcmInitParam           m_CcmInitParam;         // struct for initialisation of whole CANopen
    BYTE MEM*               m_pbErrorRegister;

    #if !defined (COP_302)
        // r.d.: this is still important for CcmWinXxx.c (should be replaced to CcmInitCANopen)
        tObdRamParam        m_ObdRamParam;          // Struct which contains parameters configured by OD
    #endif

    // -----------------------------------------------------------------------------------------------------
    // variables for time contolling

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSS) != 0)

        #if (LSSS_PROCESS_TIME_CONTROL != FALSE)

            tTime           m_LsssAbsLastCallTime;   // time of last calling of LsssProcess()
            tTime           m_LsssRelNextCallTime;   // relative time of next calling of LsssProcess()

        #endif // (LSSS_PROCESS_TIME_CONTROL != FALSE)

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSS) != 0)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSM) != 0)

        #if (LSSM_PROCESS_TIME_CONTROL != FALSE)

            tTime           m_LssmAbsLastCallTime;   // time of last calling of LssmProcess()
            tTime           m_LssmRelNextCallTime;   // relative time of next calling of LssmProcess()

        #endif // (LSSM_PROCESS_TIME_CONTROL != FALSE)

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSS) != 0)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTS) != 0)

        #if (NMTS_PROCESS_TIME_CONTROL != FALSE)

            tTime           m_NmtsAbsLastCallTime;   // time of last calling of NmtsProcess()
            tTime           m_NmtsRelNextCallTime;   // relative time of next calling of NmtsProcess()

        #endif // (NMTS_PROCESS_TIME_CONTROL != FALSE)

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTS) != 0)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0)

        #if (NMTM_PROCESS_TIME_CONTROL != FALSE)

            tTime           m_NmtmAbsLastCallTime;   // time of last calling of NmtmProcess()
            tTime           m_NmtmRelNextCallTime;   // relative time of next calling of NmtmProcess()

        #endif // (NMTM_PROCESS_TIME_CONTROL != FALSE)

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBP) != 0)

        #if (HBP_PROCESS_TIME_CONTROL != FALSE)

            tTime           m_HbpAbsLastCallTime;   // time of last calling of HbpProcess()
            tTime           m_HbpRelNextCallTime;   // relative time of next calling of HbpProcess()

        #endif // (HBP_PROCESS_TIME_CONTROL != FALSE)

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBP) != 0)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBC) != 0)

        #if (HBC_PROCESS_TIME_CONTROL != FALSE)

            tTime           m_HbcAbsLastCallTime;   // time of last calling of HbcProcess()
            tTime           m_HbcRelNextCallTime;   // relative time of next calling of HbcProcess()

        #endif // (HBC_PROCESS_TIME_CONTROL != FALSE)

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBC) != 0)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_PDO) != 0)

        #if (PDO_PROCESS_TIME_CONTROL == FUNCTION_CALL_TIME)

            tTime           m_PdoAbsLastCallTime;   // time of last calling of PdoProcessAsync()
            tTime           m_PdoRelNextCallTime;   // relative time of next calling of PdoProcessAsync()

            #if (PDO_USE_SYNC_PRODUCER != FALSE)

                tTime       m_SyncAbsLastCallTime;  // time of last calling of PdoProcessSync()
                tTime       m_SyncRelNextCallTime;  // relative time of next calling of PdoProcessSync()

            #endif // (PDO_USE_SYNC_PRODUCER != FALSE)

        #endif // (PDO_PROCESS_TIME_CONTROL == FUNCTION_CALL_TIME)

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_PDO) != 0)

    #if (SDOS_PROCESS_TIME_CONTROL == FUNCTION_CALL_TIME)

        tTime               m_SdosAbsLastCallTime;  // time of last calling of SdosProcessAsync()
        tTime               m_SdosRelNextCallTime;  // relative time of next calling of SdosProcessAsync()

    #endif // (SDOS_PROCESS_TIME_CONTROL == FUNCTION_CALL_TIME)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOC) != 0)

        #if (SDOC_PROCESS_TIME_CONTROL == FUNCTION_CALL_TIME)

            tTime           m_SdocAbsLastCallTime;  // time of last calling of SdocProcess()
            tTime           m_SdocRelNextCallTime;  // relative time of next calling of SdocProcess()

        #endif // (SDOC_PROCESS_TIME_CONTROL == FUNCTION_CALL_TIME)

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOC) != 0)

    // -----------------------------------------------------------------------------------------------------
    // variables for emergency producer

    // $$$ d.k.: include the following variables only, if CopMgr is not enabled,
    //           because the appropriate CCM modules (not the COP modules)
    //           are not needed and omitted
    //#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_COPMGR) == 0)
    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTS) != 0)

        #if (NMTS_USE_LIFEGUARDING != FALSE)

            BYTE            m_bNmtsEmergencySendFlag;

        #endif // (NMTS_USE_LIFEGUARDING != FALSE)

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTS) != 0)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBC) != 0)

        BYTE                m_ErrorEmcyHbc[HBC_MAX_EMCY_VALUES];    // example of error handling
        BYTE                m_EmcyHbcErrorCount;                    // it shoult be the same value as HBC_MAX_CONSUMER_IN_OBD in obdcfg.h

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBC) != 0)
    //#endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_COPMGR) == 0)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_PDO) != 0)

        #if (PDO_USE_ERROR_CALLBACK != FALSE)

            BYTE            m_fErrorEmcyPdo[PDO_MAX_EMCY_VALUES];   // example of error handling
            WORD            m_bEmcyPdoErrorCount;                   // it shoult be the same value as PDO_MAX_RXPDO_IN_OBD in obdcfg.h
            // 16.07.2004 r.d.: set to WORD

        #endif // (PDO_USE_ERROR_CALLBACK != FALSE)

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_PDO) != 0)

    #if (((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOM) != 0) || \
         ((CCM_MODULE_INTEGRATION & CCM_MODULE_CFGMA) != 0))
        tCcmErrorCallback   m_fpCcmErrorCallback;
    #endif

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_COPMGR) != 0)

        tCopMgrInitParam    m_CopMgrInitParam;
        // common parameter for application callback function m_CopMgrInitParam.m_fpCbEvent
        tCopMgrEventArg     m_CopMgrEventArg;
        // 0 = undefined; 2 = Slave; 1 = Master
        BYTE                m_bCopMgrMode;
        tCopMgrState        m_CopMgrState;          // current state
        WORD                m_wCopMgrEvent;         // arrived event
        // runtime configuration parameters (timeouts, retry counts, ...)
        tCopMgrConfigParam  m_CopMgrConfigParam;
//        BYTE                m_bCopMgrSdoClientsUsedMgr; // number of SDO client indexes used by CopMgr itself
//        BYTE                m_bCopMgrSdoClientsUsedApp; // number of SDO client indexes used by application
//        BYTE                m_bCopMgrSdoClientsFree;    // number of free SDO client indexes
        // first reserved SDO client index (for SDO manager for example)
        WORD                m_wCopMgrSdoClientIndexReserved;

        // flages for CAN-Driverstate
        // -> needed to indicate BusOff only once
        BYTE                m_bCopMgrDrvState;  // bitfild to indicate
                                                // bit0 = 1 first bus contact flag set
                                                // bit1 = 1 Reset from Busoff

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRD) != 0)
        BYTE                m_fCopMgrSrdBusy;
        // "global" SDO transfer parameters to be used after SrdRequestConnection() has finished
        tSdocTransferParam  m_CopMgrSdocTransferParam;
        // "global" node id for SDO transfer
        BYTE                m_bCopMgrSdocTransferNodeId;
        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRD) != 0)

        #if (SDO_BLOCKTRANSFER != FALSE)
        // "global" SDO transfer parameters for automatic block transfer detection
        tSdocTransferParam  m_CopMgrSdocBlockTransferParam;
        // bit field which marks which node isn't capable of SDO block transfer
        // d.k.: currently not implemented
        //BYTE                m_abCopMgrSdocSegNodeList[16];
        #endif // (SDO_BLOCKTRANSFER != FALSE)

        // object 0x1000 "Device Type"
        CCM_DECL_OD_VARIABLE (DWORD, dwDeviceType);

        // object 0x1018 "Identify Object"
        CCM_DECL_OD_VARIABLE (DWORD, dwVendorId);
        CCM_DECL_OD_VARIABLE (DWORD, dwProductId);
        CCM_DECL_OD_VARIABLE (DWORD, dwRevisionNr);
        CCM_DECL_OD_VARIABLE (DWORD, dwSerialNr);

        #if (((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0) || \
             ((COPMGR_FEATURES & COPMGR_FEAT_INCL_EXT_SLAVE) != 0) )
            // 0x1F80 NMTStartup (configure Master / Slave)
            // d.k.: is also needed to configure a pure slave, which enters operational mode automatically
            //
            CCM_DECL_OD_VARIABLE (DWORD, dwNMTStartup);
        #endif // (((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0) || ((COPMGR_FEATURES & COPMGR_FEAT_INCL_EXT_SLAVE) != 0) )

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0)

            // d.k.: node lists are not needed if NMTM module is present
            // solely to perform marginal master functionality
            #if ((COPMGR_FEATURES & COPMGR_FEAT_INCL_SLAVEASSIGN) != 0)
                // 0x1F81 SlaveAssignment
                CCM_DECL_OD_ARRAY (DWORD, adwSlaveAssignment, 127);

                // internal node list
                tCopMgrNodeInfo m_aCopMgrNodeInfo[127];
                BYTE    m_abCopMgrBootSlaveQueue[COPMGR_SIZE_QUEUE];  // node priority queue,
                    // die gerade gebootet werden
                    // ist 1 größer, als es maximal Knoten gibt, weil wenn die
                    // Schreibposition genau 1 kleiner als die Leseposition ist,
                    // dann ist der Puffer voll; wenn die Schreibposition der Leseposition
                    // gleicht, dann ist der Puffer leer
//                BYTE    m_bCopMgrBootSlaveReadPos;     // Leseposition im Ringpuffer
                // current write position in node priority queue (equals the current size)
                BYTE    m_bCopMgrBootSlaveWritePos;

//                BYTE    m_bCopMgrNextBootSlaveEntry;     // priority queue entry of NMT slave which shall be booted
                // number of mandatory NMT slaves to boot
                BYTE    m_bCopMgrMandatorySlaveCount;
                // number of NMT slaves, which shall be signaled for completion
                BYTE    m_bCopMgrSignalSlaveCount;

                #if ((COPMGR_FEATURES & COPMGR_FEAT_COMP_DEVICETYPE) != 0)
                    // 0x1F84 DeviceTypeIdentification
                    CCM_DECL_OD_ARRAY (tObdUnsigned32, adwDeviceTypeId, 127);
                #endif // ((COPMGR_FEATURES & COPMGR_FEAT_COMP_DEVICETYPE) != 0)

                #if ((COPMGR_FEATURES & COPMGR_FEAT_COMP_PRODUCT) != 0)
                    // 0x1F85 VendorIdentification
                    CCM_DECL_OD_ARRAY (tObdUnsigned32, adwVendorId, 127);
                    // 0x1F86 ProductCode
                    CCM_DECL_OD_ARRAY (tObdUnsigned32, adwProductCode, 127);
                    // 0x1F87 RevisionNumber
                    CCM_DECL_OD_ARRAY (tObdUnsigned32, adwRevisionNumber, 127);
                #endif // ((COPMGR_FEATURES & COPMGR_FEAT_COMP_PRODUCT) != 0)

                #if ((COPMGR_FEATURES & COPMGR_FEAT_COMP_SERIALNUMBER) != 0)
                    // 0x1F88 SerialNumber
                    CCM_DECL_OD_ARRAY (tObdUnsigned32, adwSerialNumber, 127);
                #endif // ((COPMGR_FEATURES & COPMGR_FEAT_COMP_SERIALNUMBER) != 0)

                #if ((COPMGR_FEATURES & COPMGR_FEAT_INCL_BOOTTIME) != 0)
                    // start boot time
                    tTime   m_dwCopMgrStartBootTime;
                    // 0x1F89 BootTime
                    CCM_DECL_OD_VARIABLE (tObdUnsigned32, dwBootTime);
                #endif // ((COPMGR_FEATURES & COPMGR_FEAT_INCL_BOOTTIME) != 0)

            #endif // ((COPMGR_FEATURES & COPMGR_FEAT_INCL_SLAVEASSIGN) != 0)

            // $$$ Flying Master
            #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_FLYMA) != 0)
                // $$$ r.d.: 0x1F90 & 0x1F91
            #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_FLYMA) != 0)

            #if ((COPMGR_FEATURES & COPMGR_FEAT_CONF_HEARTBEAT) != 0)

                tCmAutoHbNodeInfo   m_aCmAutoHbNodeInfo[CMAUTOHB_MAX_ENTRIES];
                // bit field which marks on which node id lifeguarding was configured
                BYTE                m_abCmAutoHbNodeLifeGuardList[16];
                // next node guarding shall be started / stopped
                BYTE                m_bCmAutoHbStartStopNode;
                // number of nodes which are processed currently
                BYTE                m_bCmAutoHbCurNodeCount;
                // event callback function
                tCmAutoHbCbEvent    m_fpCmAutoHbCbEvent;

            #endif // ((COPMGR_FEATURES & COPMGR_FEAT_CONF_HEARTBEAT) != 0)

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0)

        #if ((COPMGR_FEATURES & COPMGR_FEAT_INCL_REQUESTNMT) != 0)
            // 0x1F82 RequestNMT (also used to save NMT states of other nodes in NMT slave mode)
            CCM_DECL_OD_ARRAY (tObdUnsigned8, abRequestNmt, 128);
            // 0x1F83 RequestGuarding
            //CCM_DECL_OD_ARRAY (tObdUnsigned8, abRequestGuarding, 128);
        #endif // ((COPMGR_FEATURES & COPMGR_FEAT_INCL_REQUESTNMT) != 0)

        #if ((COPMGR_FEATURES & COPMGR_FEAT_INCL_COB) != 0)
            #ifndef CMCOB_MAX_ENTRIES
            #define CMCOB_MAX_ENTRIES    10
            #endif

            tCobParam   m_aCmCobParam[CMCOB_MAX_ENTRIES];
        #endif // ((COPMGR_FEATURES & COPMGR_FEAT_INCL_COB) != 0)

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_COPMGR) != 0)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOM) != 0)

        // var for OD entries 0x1F02 and 0x1F03
        //-----------------------------------------------------------------------
        CCM_DECL_OD_ARRAY (DWORD, m_adwSdomCobList, SDOM_COB_ID_LIST_COUNT);
        CCM_DECL_OD_ARRAY (DWORD, m_adwSdomConnectionList, SDOM_CONNECT_LIST_COUNT);
        //-----------------------------------------------------------------------

        // events for Sdom State Maschine
        BYTE                m_bSdomEvents;
        // status of the SDO Manager
        tSdomParam          m_SdomParam;
        // status for an new connection
        tSdomConStatus      m_SdomConStatus;
        // Cob Handler for Cob 0x6E0
        tCobHdl             m_SdomCobHdl;
        //SDO transfer timeout for SRD Client scan
        tTime               m_SdomSdoTransferTimeout;
        // buffer for data in SRD scan
        DWORD               m_dwSdomSrdData;
        // buffer for Data in SDO transfer
        DWORD               m_dwSdomSdoData;
        // count for Release of more then one Connection
        // used in SdomWriteSdosSdoReleaseCon()
        DWORD               m_dwSdomConCount;
        // SDO Client Index for Scan and config
        WORD                m_SdomSdocIndex;
        // bit field to register possible SRD Client Node Ids
        BYTE                m_SdomSrdList[16];
        // bit field to save which Node is allready registered to the SDO Manager
        BYTE                m_SdomRegSrdList[16];
        // BYTE buffer for Data in SDO transfer
        BYTE                m_bSdomSdoData;
        // count for list entries in srd scan
        // in SdomScanAllSrd used as NodeId
        BYTE                m_bSdomListCount;
        // saves if default SDO Server on Slave Node is free
        // for function SdomCheckFreeSdosSlave()
        BYTE                m_fSdomDefaultSdosFree;
        // used in function SdomReleaseConSdos()
        BYTE                m_fSdomSdosFree;
        // saves highest used SDO Server Offset of Slave Node
        // for function SdomCheckFreeSdosSlave()
        BYTE                m_bSdomHighestSdosOffset;
        // used in SdomScanAllSrd
        BYTE                m_bSdomOffset;
        // Flag to signal the use of the STATE SDOM_SCAN_ALL_SRD or not
        // if TRUE State is used
        BYTE                m_fSdomActivateAllScan;
        // Flag for SRD Client scan to store if a nde is registered
        BYTE                m_fSdomSrdRegistered;
        // // bit field to save which node(s) are to deregister
        BYTE                m_bSdomDeregisterlist[16];
        // Flag to signal if more to do or not
        BYTE                m_fSdomWorkToDo;
        // size of transfered bytes for sdo transfer
        BYTE                m_bSdomSdoSize;
        // data pointer for sdo
        void*                m_pSdomSdoData;


    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOM) != 0)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRD) != 0)

        // var for OD entry 0x1F10
        //-----------------------------------------------------------------------
        CCM_DECL_OD_VARIABLE (DWORD, m_dwSrdOdEntry0x1F10);
        //-----------------------------------------------------------------------

        // central administration structur
        tSrdParam           m_SrdParam;
        // save Cob-Handler
        tCobHdl             m_SrdCobHdl;
        // saves events
        WORD                m_wSrdInternEvent;
        // save Time for timeout in functions
        tTime               m_SrdTime;
        // save Timeout-Count and Retry-Count from SrdInit
        tSrdInit            m_SrdInit;
        // buffer for data written by SDO Manager to OD of SDO Client
        DWORD               m_dwSrdOdData;
        // buffer for Client Index in OD
        WORD                m_wSrdClientIndex;
        // counts Retries
        BYTE                m_bSrdRetryCounter;
        // buffer for Server Node ID
        BYTE                m_bSrdServerNodeId;
        /// Flag to signal if more to do or not
        BYTE                m_fSrdWorkToDo;
        // if SDO Manager on local Node,
        // to decide if access to SDO Manager is local (we work as Master)
        // or over CAN bus (wie work as Slave)
        #if((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOM) == CCM_MODULE_SDOM)
        BYTE                m_fSrdMasterMode;
        #endif


    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRD) != 0)

    // -----------------------------------------------------------------------------------------------------
    // variables for dynamic OD

    #if (OBD_USE_DYNAMIC_OD != FALSE)

        tDynSegContInfo     m_DynSegContInfo;
        tDynOdMemInfo       m_DynMemoryInfo;        // memory info structure for dynamic OD
      //BYTE FAR*           m_pbSegCont;            // pointer to segment container

    #endif // (OBD_USE_DYNAMIC_OD != FALSE)

    // -----------------------------------------------------------------------------------------------------
    // variables for Indicator Specification (DR303-3)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_DR303_3) != 0)

        tTime               m_Dr303ProcessTime;
        BYTE                m_bGreenLedState;
        BYTE                m_bRedLedState;
        BYTE                m_bGreenLedTabIdx;
        BYTE                m_bRedLedTabIdx;
        WORD                m_wCurrErrorFlags;

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_DR303_3) != 0)

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSM) != 0)

        tLssmEvent          m_LssmWaitEvent;
        DWORD               m_dwLssmInqureData;
        BYTE                m_bLssmConfirmError;
        BYTE                m_bLssmConfirmSpecError;

    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSM) != 0)

    // -----------------------------------------------------------------------------------------------------
    // variables for configuration manager

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_CFGMA) != 0)
        //variable for use in configuration manager
        tCfgMaNode          m_CfgMaNode[CFGMA_MAX_SDO_CLIENTS];
//        tCcmErrorCallback   m_fpCfgMaErrorCallback;
    #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_CFGMA) != 0)

    // -----------------------------------------------------------------------------------------------------
    // variables for operation system PxROS

    #ifdef PXROS

        tTgtPxParam             m_ProcessTask;          // task parameters for process task for PxROS
        tTgtPxCriticalSection   m_PdoCriticalSection;
        tTgtPxCriticalSection   m_ObjCriticalSection;

    #endif // PXROS

    // -----------------------------------------------------------------------------------------------------
    // variables for operation system LINUX
    #ifdef LINUX

        pthread_mutex_t m_MutexCcmCanopenThreads;
        pthread_t       m_CcmProcessRxThread;
        pthread_cond_t  m_RxCond;

        pthread_t       m_CcmThread;
        BYTE            m_ValidNodeIdFlag;
        BYTE            m_NmtStartedFlag;

        pthread_t       m_CcmProcessSdosThread;
        pthread_cond_t  m_SdosCond;

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSS) != 0)

            pthread_t       m_CcmProcessLsssThread;
            pthread_cond_t  m_LsssCond;

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSS) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTS) != 0)

            pthread_t       m_CcmProcessNmtsThread;
            pthread_cond_t  m_NmtsCond;

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTS) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0)

            pthread_t       m_CcmProcessNmtmThread;
            pthread_cond_t  m_NmtmCond;

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBP) != 0)

            pthread_t       m_CcmProcessHbpThread;
            pthread_cond_t  m_HbpCond;

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBP) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBC) != 0)

            pthread_t       m_CcmProcessHbcThread;
            pthread_cond_t  m_HbcCond;

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBC) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_PDO) != 0)

            pthread_t       m_CcmProcessPdoThread;
            pthread_cond_t  m_PdoCond;
            pthread_t       m_CcmProcessPdoCheckVarThread;
            pthread_cond_t  m_PdoCheckVarCond;

            #if (PDO_USE_SYNC_PRODUCER != FALSE)

                pthread_t       m_CcmProcessPdoSyncThread;
                pthread_cond_t  m_PdoSyncCond;

            #endif // (PDO_USE_SYNC_PRODUCER != FALSE)

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_PDO) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSM) != 0)

            pthread_t       m_CcmProcessLssmThread; // later
            pthread_cond_t  m_LssmCond; // later

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSM) != 0)

        #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOC) != 0)

            pthread_t       m_CcmProcessSdocThread; // later
            pthread_cond_t  m_SdocCond; // later

        #endif // ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOC) != 0)

    #endif // LINUX

    // -----------------------------------------------------------------------------------------------------
    // variables for operation system WINDOWS
    #ifdef WIN32

        #ifdef WINCE
            MMRESULT        m_Timer_Handle[23];
        #endif

        tTgtThrdParam       m_ThreadParam;                  // thread structur
        HANDLE              m_ahCcmEventHandle[23];         // event handle array
        //20.07.04 r.d.: nicht benötigt -> HANDLE              m_hCcmProcessThread;            // thread handle of CcmProcessThread
        //20.07.04 r.d.: nicht benötigt -> BYTE                m_bThrdPriority;                // thread priority
        CRITICAL_SECTION    m_PdoProcessCriticalSection;    // critical section for PDO process
        CRITICAL_SECTION    m_SdocProcessCriticalSection;   // critical section for SDOC process

    #endif //WIN32

    tCcmErrorCallback   m_fpAppErrorCallback;

} tCcmInstanceInfo;


// =========================================================================
// variables
// =========================================================================

// This macro replace the unspecific pointer to an instance through
// the modul specific type for the local instance table. This macro
// must defined in each modul.
#define tCopPtrInstance tCcmInstanceInfo MEM*

// instance table for CCM module
// "extern" because other files of CCM module needs access to this structure
extern tCcmInstanceInfo MEM aCcmInstanceTable_g[];


// ============================================================================
// function prototypes
// ============================================================================

// converts instance handle to instance pointer of CCM module
// "extern" because other files of CCM module have to call this function
extern tCcmInstanceInfo MEM* CcmGetInstancePtr (tCopInstanceHdl InstanceHandle);


// ============================================================================
// events for CANopen processing in operation systems
// ============================================================================

#ifdef PXROS

    #define PX_EVENT_CAN_RECEIVE    0x00000001L     // CAN message was received
//                                                  // note: use CAN_RX_EVENT in future
    #define PX_EVENT_LSSS_TIMEOUT   0x00000002L     // timer for LSS slave runs over
    #define PX_EVENT_LSSM_TIMEOUT   0x00000004L     // timer for LSS master runs over
    #define PX_EVENT_NMTM_TIMEOUT   0x00000008L     // timer for NMT master runs over
    #define PX_EVENT_NMTM_APPCFG    0x00000010L     // application has configured lifeguard master
    #define PX_EVENT_NMTS_TIMEOUT   0x00000020L     // timer for NMT slave runs over
    #define PX_EVENT_NMTS_APPCFG    0x00000040L     // application has configured lifeguard slave
    #define PX_EVENT_HBP_TIMEOUT    0x00000080L     // timer for heartbeat producer runs over
    #define PX_EVENT_HBP_APPCFG     0x00000100L     // application has configured heartbeat producer
    #define PX_EVENT_HBC_TIMEOUT    0x00000200L     // timer for heartbeat consumer runs over
    #define PX_EVENT_HBC_APPCFG     0x00000400L     // application has configured heartbeat consumer
    #define PX_EVENT_SDOS_TIMEOUT   0x00000800L     // timer for SDO server runs over
    #define PX_EVENT_SDOC_TIMEOUT   0x00001000L     // timer for SDO client runs over
    #define PX_EVENT_SDOC_APPEVENT  0x00002000L     // application has requested a SDO
    #define PX_EVENT_PDO_TIMEOUT    0x00004000L     // timer for PDO runs over
    #define PX_EVENT_PDO_SIGNALVAR  0x00008000L     // application has changed a variable for a PDO
    #define PX_EVENT_PDO_CHECKVAR   0x00010000L     // application has changed some variables for any PDO's
    #define PX_EVENT_PDO_APPCFG     0x00020000L     // application has configured PDO
    #define PX_EVENT_SYNC_TIMEOUT   0x00040000L     // timer for SYNC producer runs over
    #define PX_EVENT_SYNC_APPCFG    0x00080000L     // application has configured SYNC producer
    #define PX_EVENT_NMT_EVENT      0x00100000L     // NMT event occured
    #define PX_EVENT_LSSM_APPEVENT  0x00200000L     // application has started a LSS master service
    #define PX_EVENT_CRITCAL_SECT   0x20000000L     // event for critical sections
//                                  0x40000000L     // is reserved by CAN driver
    #define PX_EVENT_DO_EXIT_TASK   0x80000000L     // task should be closed

#endif // PXROS


#ifdef WIN32

    #define WIN_EVENT_CAN_RECEIVE    0x00000001L     // CAN message was received
    #define WIN_EVENT_LSSS_TIMEOUT   0x00000002L     // timer for LSS slave runs over
    #define WIN_EVENT_LSSM_TIMEOUT   0x00000004L     // timer for LSS master runs over
    #define WIN_EVENT_NMTM_TIMEOUT   0x00000008L     // timer for NMT master runs over
    #define WIN_EVENT_NMTM_APPCFG    0x00000010L     // application has configured lifeguard master
    #define WIN_EVENT_NMTS_TIMEOUT   0x00000020L     // timer for NMT slave runs over
    #define WIN_EVENT_NMTS_APPCFG    0x00000040L     // application has configured lifeguard slave
    #define WIN_EVENT_HBP_TIMEOUT    0x00000080L     // timer for heartbeat producer runs over
    #define WIN_EVENT_HBP_APPCFG     0x00000100L     // application has configured heartbeat producer
    #define WIN_EVENT_HBC_TIMEOUT    0x00000200L     // timer for heartbeat consumer runs over
    #define WIN_EVENT_HBC_APPCFG     0x00000400L     // application has configured heartbeat consumer
    #define WIN_EVENT_SDOS_TIMEOUT   0x00000800L     // timer for SDO server runs over
    #define WIN_EVENT_SDOC_TIMEOUT   0x00001000L     // timer for SDO client runs over
    #define WIN_EVENT_SDOC_APPEVENT  0x00002000L     // application has requested a SDO
    #define WIN_EVENT_PDO_TIMEOUT    0x00004000L     // timer for PDO runs over
    #define WIN_EVENT_PDO_SIGNALVAR  0x00008000L     // application has changed a variable for a PDO
    #define WIN_EVENT_PDO_CHECKVAR   0x00010000L     // application has changed some variables for any PDO's
    #define WIN_EVENT_PDO_APPCFG     0x00020000L     // application has configured PDO
    #define WIN_EVENT_SYNC_TIMEOUT   0x00040000L     // timer for SYNC producer runs over
    #define WIN_EVENT_SYNC_APPCFG    0x00080000L     // application has configured SYNC producer
    #define WIN_EVENT_NMT_EVENT      0x00100000L     // NMT event occured
    #define WIN_EVENT_LSSM_APPEVENT  0x00200000L     // application has started a LSS master service
    #define WIN_EVENT_DO_EXIT_TASK   0x80000000L     // task should be closed

    #define SDOS_MODUL_TIMEOUT       WAIT_OBJECT_0
    #define SDOC_MODUL_TIMEOUT       WAIT_OBJECT_0 + 1
    #define LSSS_MODUL_TIMEOUT       WAIT_OBJECT_0 + 2
    #define LSSM_MODUL_TIMEOUT       WAIT_OBJECT_0 + 3
    #define NMTM_MODUL_TIMEOUT       WAIT_OBJECT_0 + 4
    #define NMTM_APP_CFG             WAIT_OBJECT_0 + 5
    #define NMTS_MODUL_TIMEOUT       WAIT_OBJECT_0 + 6
    #define NMTS_APP_CFG             WAIT_OBJECT_0 + 7
    #define HBC_MODUL_TIMEOUT        WAIT_OBJECT_0 + 8
    #define HBC_APP_CFG              WAIT_OBJECT_0 + 9
    #define HBP_MODUL_TIMEOUT        WAIT_OBJECT_0 + 10
    #define HBP_APP_CFG              WAIT_OBJECT_0 + 11
    #define PDO_MODUL_TIMEOUT        WAIT_OBJECT_0 + 12
    #define PDO_CHECK_VAR            WAIT_OBJECT_0 + 13
    #define PDO_SIGNAL_VAR           WAIT_OBJECT_0 + 14
    #define PDO_SYNC_PRODUCER        WAIT_OBJECT_0 + 15
    #define PDO_APP_CFG              WAIT_OBJECT_0 + 16
    #define DO_EXIT_CCM_THREAD       WAIT_OBJECT_0 + 17
    #define SDOC_APP_EVENT           WAIT_OBJECT_0 + 18
    #define NMT_EVENT                WAIT_OBJECT_0 + 19
    #define SYNC_APP_CFG             WAIT_OBJECT_0 + 20
    #define LSSM_APP_EVENT           WAIT_OBJECT_0 + 21
    #define CAN_MSG_RECEIVED         WAIT_OBJECT_0 + 22

#endif

#ifdef LINUX

    #define LINUX_EVENT_NMTM_CFG       0x00000001L     // application has configured lifeguard master
    #define LINUX_EVENT_NMTS_CFG       0x00000002L     // application has configured lifeguard slave
    #define LINUX_EVENT_HBP_CFG        0x00000004L     // application has configured heartbeat producer
    #define LINUX_EVENT_HBC_CFG        0x00000008L     // application has configured heartbeat consumer
    #define LINUX_EVENT_SDOC_CFG       0x00000010L     // application has requested a SDO
    #define LINUX_EVENT_PDO_SIGNALVAR  0x00000020L     // application has changed a variable for a PDO
    #define LINUX_EVENT_PDO_CHECKVAR   0x00000040L     // application has changed some variables for any PDO's
    #define LINUX_EVENT_PDO_CFG        0x00000080L     // application has configured PDO
    #define LINUX_EVENT_SYNC_CFG       0x00000100L     // application has configured SYNC producer
    #define LINUX_EVENT_NMT_EVENT      0x00000200L     // NMT event occured

#endif // LINUX


// -------------------------------------------------------------------------
// macros for CAN driver functions
// -------------------------------------------------------------------------

#if (COP_USE_CDRV_FUNCTION_POINTER != FALSE)

    #define CCM_CDRV_FUNCPTR                            CCM_INST_ENTRY.m_CcmInitParam.m_pCdrvFct

    #define CCM_CALL_CDRV_INIT(inst,par)                CCM_CDRV_FUNCPTR->m_fpInit           (CCM_INSTANCE_PARAM_ (inst) (par));
    #define CCM_CALL_CDRV_ADD_INSTANCE(inst,par)        CCM_CDRV_FUNCPTR->m_fpAddInstance    (CCM_INSTANCE_PARAM_ (inst) (par));
    #define CCM_CALL_CDRV_DELETE_INSTANCE(inst)         CCM_CDRV_FUNCPTR->m_fpDeleteInstance (CCM_INSTANCE_PARAM  (inst)      );
    #define CCM_CALL_CDRV_RESET_STATUS(inst,par)        CCM_CDRV_FUNCPTR->m_fpResetStatus    (CCM_INSTANCE_PARAM_ (inst) (par));
    #define CCM_CALL_CDRV_RESET(inst,par)               CCM_CDRV_FUNCPTR->m_fpReset          (CCM_INSTANCE_PARAM_ (inst) (par));
    #define CCM_CALL_CDRV_SET_BAUDRATE(inst,p1,p2,p3)   CCM_CDRV_FUNCPTR->m_fpSetBaudrate    (CCM_INSTANCE_PARAM_ (inst) (p1),(p2),(p3));

#else

    #define CCM_CALL_CDRV_INIT(inst,par)                CdrvInit            (CCM_INSTANCE_PARAM_ (inst) (par));
    #define CCM_CALL_CDRV_ADD_INSTANCE(inst,par)        CdrvAddInstance     (CCM_INSTANCE_PARAM_ (inst) (par));
    #define CCM_CALL_CDRV_DELETE_INSTANCE(inst)         CdrvDeleteInstance  (CCM_INSTANCE_PARAM  (inst)      );
    #define CCM_CALL_CDRV_RESET_STATUS(inst,par)        CdrvResetStatus     (CCM_INSTANCE_PARAM_ (inst) (par));
    #define CCM_CALL_CDRV_RESET(inst,par)               CdrvReset           (CCM_INSTANCE_PARAM_ (inst) (par));
    #define CCM_CALL_CDRV_SET_BAUDRATE(inst,p1,p2,p3)   CdrvSetBaudrate     (CCM_INSTANCE_PARAM_ (inst) (p1),(p2),(p3));

#endif


// -------------------------------------------------------------------------
// function prototypes
// -------------------------------------------------------------------------

#ifdef WIN32

// ----------------------------------------------------------------------------
//
// Function:    CcmEnterCriticalSectionSdocProcess()
//
// Description: locks the CcmProcessThread for SDOC process
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     void
//
// ----------------------------------------------------------------------------

COPDLLEXPORT void PUBLIC CcmEnterCriticalSectionSdocProcess (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CcmLeaveCriticalSectionSdocProcess()
//
// Description: unlocks the CcmProcessThread for the SDOC process
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     void
//
// ----------------------------------------------------------------------------

COPDLLEXPORT void PUBLIC CcmLeaveCriticalSectionSdocProcess (CCM_DECL_INSTANCE_HDL);

#endif // WIN32


// ----------------------------------------------------------------------------
//
// Function:    CcmCbNmtEvent()
//
// Description: Callback handler for NMT state machine in CCM module
//              it have to call all lower CANopen modules
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              NmtEvent_p              = called NMT event
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CcmCbNmtEvent (CCM_DECL_INSTANCE_HDL_
    tNmtEvent NmtEvent_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmCbLsssEvent()
//
// Description: Callback handler for LSS slave module
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pLssCbParam_p           = pointer to callback parameters
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CcmCbLsssEvent (CCM_DECL_INSTANCE_HDL_
    tLssCbParam MEM* pLssCbParam_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmCbError()
//
// Description: Callback handler for all error events
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pArg_p                  = pointer to additional error parameters (depends on ErrorCode_p)
//              ErrorCode_p             = error code of type tCopKernel (see enum in errordef.h)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CcmCbError (CCM_DECL_INSTANCE_HDL_
    void MEM* pArg_p,
    tCopKernel ErrorCode_p);


// ----------------------------------------------------------------------------
//
// Function:     CcmCbLgsEvent()
//
// Description:  callback function, called by NMTS module, that notifies of
//               a life guard event
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               ErrCtrlEvent_p         = notified event
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CcmCbLgsEvent (CCM_DECL_INSTANCE_HDL_
    tNmtErrCtrlEvent ErrCtrlEvent_p);


// ----------------------------------------------------------------------------
// Function:    CcmCbNmtmEvent()
//
// Description: callback function, called by NMTM module, that notifies
//              an event of NMT master
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              bNodeId_p               = NodeId which generated the event
//              ErrCtrlEvent_p          = notified event
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CcmCbNmtmEvent (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p,
    tNmtErrCtrlEvent ErrCtrlEvent_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmCbStoreLoadObject()
//
// Description: callback function, called by OBD module, that notifies of
//              commands STORE or LOAD
//              The function is called with the selected OD part:
//              ODPart = kObdPartGen: 0x1000-0x1FFF
//                     = kObdPartMan: 0x2000-0x5FFF
//                     = kObdPartDev: 0x6000-0x9FFF
//              For each part the function creates an archiv an saves
//              the parameters. Following command sequence is used:
//              1. kObdCommOpenWrite(Read)  --> create archiv (set last archiv invalid)
//              2. kObdCommWrite(Read)Obj   --> write data to archiv
//              3. kObdCommCloseWrite(Read) --> close archiv (set archiv valid)
//
// Parameters:  CCM_DECL_INSTANCE_HDL_ = (instance handle)
//              pData_p                = address to object data in OD
//              ObjSize_p              = size of this object
//              Direction_p            = direction (STORE to medium or LOAD from medium)
//
// Returns:     tCopKernel             = error code
//
// State:       not tested -as
//              06-dec-2004 r.d.: tested for Infineon C16x
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CcmCbStoreLoadObject (CCM_DECL_INSTANCE_HDL_
    tObdCbStoreParam MEM* pCbStoreParam_p);


// ----------------------------------------------------------------------------
// Function:        CopMgrCbObdAccess()
//
// Description:     Callback function to inform CopMgr about
//                  read / write access to OD 0x1F8x and other application
//                  specific entries
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  = (instance handle)
//                  pParam_p                =  aditional infos for reasons of
//                                             function call
//
// Returns:	        tCopKernel = error code
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CopMgrCbObdAccess(CCM_DECL_INSTANCE_HDL_ tObdCbParam MEM* pParam_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmCbEmcpEvent()
//
// Description: sample of callback function of emergency producer
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              bEmcpEvent_p            = event
//              papdwErrorField_p       = pointer to an array of pointers
//                                        which points to the errors in PreDefinedErrorField (objects 0x1003/>0)
//                                        (NULL if bEmcpEvent_p == EMCP_EVENT_ERROR_DELETEALL)
//              bErrorCount_p           = number of errors in PreDefinedErrorField (object 0x1003/0)
//                                        (0 if bEmcpEvent_p == EMCP_EVENT_ERROR_DELETEALL)
//
// Returns:     void
//
// ----------------------------------------------------------------------------

void PUBLIC CcmCbEmcpEvent (CCM_DECL_INSTANCE_HDL_
    BYTE bEmcpEvent_p, DWORD MEM* MEM* papdwErrorField_p, BYTE bErrorCount_p);


// ----------------------------------------------------------------------------
//
// Function:     CcmCbEmccEvent()
//
// Description:  callback function, called by EMCC module, that notifies of
//               an emergency event
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               bNodeId                = NodeId from failed Node
//               pEmcParam_p            = parameter od emergency message
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CcmCbEmccEvent (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p,
    tEmcParam MEM* pEmcParam_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmCbHbcEvent()
//
// Description: callback function, called by HBC module, that notifies of
//              an heartbeat event
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              bNodeId                 = NodeId from failed Node
//              ErrCtrlEvent_p          = notified event
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CcmCbHbcEvent (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p,
    tNmtErrCtrlEvent ErrCtrlEvent_p);


// ----------------------------------------------------------------------------
//
// Function:    CcmCbLssmEvent()
//
// Description: Callback handler for LSS master module
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              LssmEvent_p             = LSS master event
//              m_pArg                  = pointer to additional parameter (depends on LSS master event)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CcmCbLssmEvent (CCM_DECL_INSTANCE_HDL_
    tLssmEvent LssmEvent_p, void MEM* m_pArg);


// ----------------------------------------------------------------------------
//
// Function:    CcmCbSyncReceived()
//
// Description: callback function, called by PDO module, that notifies of
//              a receive a sync message
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CcmCbSyncReceived (CCM_DECL_INSTANCE_HDL);


// ============================================================================
// functions within ccmsdom.c
// ============================================================================

//---------------------------------------------------------------------------
//
// Function:    SdomCbReceive()
//
// Description: callback function for COB events
//              It will be called if COB module receives messages
//              with COB-ID 0x6E0.
//              Second user is the SRD module residing on the
//              same instance. Hence this function needs to be public.
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pArg_p                  = additional argument define in tCobParam
//
// Returns:     tCopKernel = error code
//                  kCopSuccessful
//                  from other functions to configure CANopen modules
//
//
//---------------------------------------------------------------------------

tCopKernel PUBLIC SdomCbReceive ( CCM_DECL_INSTANCE_HDL_
    void GENERIC* pArg_p);


// ----------------------------------------------------------------------------
// Function:        SdomCbWriteProtect0x1F02()
//
// Description:     - Callback- function for OD 0x1F02
//                  - if there are dynamic SDO Connections
//                    -> protect 0x1F02 against write access
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  = (instance handle)
//                  pParam_p                =  aditional infos for reasons of
//                                             function call
//
// Returns:         tCopKernel = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdomCbWriteProtect0x1F02 (CCM_DECL_INSTANCE_HDL_
    tObdCbParam MEM* pParam_p);


// ----------------------------------------------------------------------------
// Function:        SdomCbWriteOd()
//
// Description:     Callback- function to inform SDO Manager about
//                  write access to OD 0x1F00 or 0x1F01
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  = (instance handle)
//                  pParam_p                =  aditional infos for reasons of
//                                             function call
//
// Returns:         tCopKernel = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdomCbWriteOd (CCM_DECL_INSTANCE_HDL_
    tObdCbParam MEM* pParam_p);


// ============================================================================
// functions within ccmsrd.c
// ============================================================================

// ----------------------------------------------------------------------------
// Function:        SrdCbWriteOd()
//
// Description:     Callback- function to inform SRD Client about
//                  write access to OD 0x1F10 or 0x1F11
//
// Parameters:      CCM_DECL_INSTANCE_HDL_  = (instance handle)
//                  pParam_p                =  aditional infos for reasons of
//                                             function call
//
// Returns:         tCopKernel = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SrdCbWriteOd (CCM_DECL_INSTANCE_HDL_
    tObdCbParam MEM* pParam_p);


// ----------------------------------------------------------------------------

/* function for aworks adapter*/
tCopKernel PUBLIC CcmNmtmGetSlaveInfo (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p, tNmtmSlaveInfo MEM* pSlaveInfo_p);
COPDLLEXPORT tCopKernel PUBLIC CcmNmtsExecCommand (CCM_DECL_INSTANCE_HDL_
    tNmtCommand NmtCommand_p);
tCopKernel PUBLIC CcmHbcGetNodeState (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p, tNodeState MEM* pNodeState_p);
tCopKernel PUBLIC CcmTspSend (CCM_DECL_INSTANCE_HDL_
    DWORD  dwTimeStampId, tTimeOfDay MEM* pTimeOfDay_p);

tCopKernel PUBLIC CcmCobGetParam (CCM_DECL_INSTANCE_HDL_
    tCobParam MEM* pCobParam_p);
BYTE PUBLIC CcmObdGetNodeState (CCM_DECL_INSTANCE_HDL);

#endif // _CCMMAIN_H_

// Please keep an empty line at the end of this file.

