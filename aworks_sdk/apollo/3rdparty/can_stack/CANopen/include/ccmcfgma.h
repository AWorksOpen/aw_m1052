/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  Header file for CANopen API for Configuration Manager for Concise DCF

  -------------------------------------------------------------------------

                $RCSfile: CcmCfgMa.h,v $

                $Author: K.Trenkel $

                $Revision: 1.6 $  $Date: 2006/10/24 07:31:28 $

                $State: Exp $

                Build Environment:
                GNU

  -------------------------------------------------------------------------

  Revision History:
  2004/10/04 f.j.:   Start of implementation
  14-dec-2004 r.d.:  better description in funcion headers
  2006/01/18  k.t.:  chances for concise format

****************************************************************************/

#ifndef _CCMCFGMA_H_
#define _CCMCFGMA_H_
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_CFGMA) != 0)
#include "cop.h" // for COPDLLEXPORT needed

//---------------------------------------------------------------------------
// const defines
//---------------------------------------------------------------------------

//define max number of timeouts for configuration of 1 device
#ifndef CFGMA_MAX_TIMEOUT
    #define CFGMA_MAX_TIMEOUT       3
#endif

//bitcodoiert die Antwort ueber den Stand des letzen SDO geben
//in Variable m_bSdocState
// also used to singal start of the State Maschine
#define CFGMA_SDOC_BUSY             0       // SDOC aktiv, es muss gewartet werden
#define CFGMA_SDOC_READY            1       // SDOC abgeschlossen
#define CFGMA_SDOC_TIMEOUT          2       // SDOC Timeout
#define CFGMA_SDOC_ABORT_RECEIVED   4       // SDOC Abort, see Abortcode
#define CFGMA_START                 8       // START State Mschine


//---------------------------------------------------------------------------
// typedef
//---------------------------------------------------------------------------

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


//---------------------------------------------------------------------------
// function prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Function:    CfgMaInit()
//
// Description: Function creates a instance of CANopen Configuration Manager
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//                  kCopSuccessful      - no error
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CfgMaInit (CCM_DECL_INSTANCE_HDL);

//---------------------------------------------------------------------------
// Function:    CfgMaStartConfigurationNode()
//
// Description: Function started the configuration process
//              with the  consice DCF from 0x1F22 Subindex == bNodeId_p
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              wSdocIndex_p            = object index of used SDO client
//              bNodeId_p               = NodeId of the CANopen node to configure
//              fpCfgMaCb_p             = pointer to callback function (should not be NULL)
//
// Returns:     tCopKernel              = error code
//---------------------------------------------------------------------------

tCopKernel PUBLIC CfgMaStartConfigurationNode (CCM_DECL_INSTANCE_HDL_    //IN: Instanz
                                    WORD              wSdocIndex_p,  //IN: SDOC der verwendet werden soll
                                    BYTE              bNodeId_p,     //IN: Adresse des Geraetes, das Konfiguriert werden soll
                                    tfpCfgMaCb        fpCfgMaCb_p    //IN: Zeiger auf die Callback die bei Abschluss gerufen wird.
                                );

//---------------------------------------------------------------------------
// Function:    CfgMaStartConfiguration()
//
// Description: Function started the configuration process
//              initialization the statemachine for Cfg- process
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              wSdocIndex_p            = object index of used SDO client
//              bNodeId_p               = NodeId of the CANopen node to configure
//              pbConcise_p             = pointer to concise DCF
//              fpCfgMaCb_p             = pointer to callback function (should not be NULL)
//              pSizeOfConcise_p        = size of concise DCF in BYTE -> for future use
//
// Returns:     tCopKernel              = error code
//---------------------------------------------------------------------------

tCopKernel PUBLIC CfgMaStartConfiguration (CCM_DECL_INSTANCE_HDL_    //IN: Instanz
                                    WORD              wSdocIndex_p,  //IN: SDOC der verwendet werden soll
                                    BYTE              bNodeId_p,     //IN: Adresse des Geraetes, das Konfiguriert werden soll
                                    BYTE FAR *        pbConcise_p,   //IN: start of concise format
                                    tfpCfgMaCb        fpCfgMaCb_p,    //IN: Zeiger auf die Callback die bei Abschluss gerufen wird.
                                    tObdSize          pSizeOfConcise_p); // IN: größe des concise DCF in Byte


//---------------------------------------------------------------------------
// Function:    CfgMaProcess()
//
// Description: process-function for configuration manager
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CfgMaProcess (CCM_DECL_INSTANCE_HDL);

#endif
#endif

// Please keep an empty line at the end of this file.
