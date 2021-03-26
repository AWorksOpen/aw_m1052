/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  include all public function prototypes and defines

  -------------------------------------------------------------------------

                $RCSfile: Pdo.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.59 $  $Date: 2006/12/20 15:04:57 $
                
                $State: Exp $
                
                Build Environment:
                    all

  -------------------------------------------------------------------------

  Revision History:

****************************************************************************/

#ifndef _PDO_H_
#define _PDO_H_


// ============================================================================
// defines
// ============================================================================


// ============================================================================
// types
// ============================================================================

// Callbackfunction to info of receive a sync
typedef tCopKernel (PUBLIC ROM* tPdoSyncCallback) (CCM_DECL_INSTANCE_HDL);

#if (PDO_USE_STATIC_MAPPING == FALSE)
typedef struct 
{
    DWORD           m_dwMapping;    // mapping entry from object dictionary
    tObdSubEntryPtr m_pSubEntry;    // (tObdVarEntry -> obd.h) includes Index,Subindex,old data,address to data and Callback function

} tMappInfo;
#endif

// Die PDO-Tabellen bestehen aus folgender Struktur
typedef struct
{
    // internal values
    // ------------------------------------------------------------------------
    WORD                    m_wPdoCommuIndex;               // index of communication parameters in OD
    BYTE                    m_bState;                       // state of PDO
    BYTE                    m_bTxMode;                      // transmission mode of PDO
    tTime                   m_LastTime;                     // This variable always contains the last sending time
                                                            // of sending this PDO
                                                            // for RPDOs it contains the last receive time
    tCobParam               m_CobParam;                     // parameters of COB object of this PDO
    BYTE                    m_abCmpData[8];                 // buffer to compare all variables of PDO
                                                            // until the next transmission of the PDO
    tPdoCallback            m_fpCallback;                   // callback function for this PDO

    #if (PDO_USE_SYNC_PDOS != FALSE)
        BYTE                m_bSyncCount;                   // counter for the received SYNC objects 
    #endif // (PDO_USE_SYNC_PDOS != FALSE

    #if (PDO_USE_EVENT_TIMER != FALSE)
        tTime               m_CfgEventTimer;                // configured event timer (time base is 100us)
    #endif // (PDO_USE_EVENT_TIMER != FALSE)

    // configured values in object dictionary for communication parameters
    // ------------------------------------------------------------------------
    DWORD                   m_dwCanId;
    WORD                    m_wInhibitTime;                 // (time base 100us)
    WORD                    m_wEventTimer;                  // (time base 1ms)
    BYTE                    m_bTxTyp;

    // configured values in object dictionary for mapping parameters
    // ------------------------------------------------------------------------
    #if (PDO_USE_STATIC_MAPPING == FALSE)
        BYTE                m_bNoOfMappedObjects;
        tMappInfo           m_aMappObjects[PDO_MAX_MAPPENTRIES]; // field of mapping entries
    #else
        void MEM*           m_pVarData;
    #endif

    #if (PDO_USE_MPDO_DAM_PRODUCER != FALSE)
        BYTE                m_bNodeId;
        tMPdoParam GENERIC* m_pMPdoParam;
    #endif

} tPdoTabEntry;

typedef struct
{
    tPdoTabEntry MEM*   m_pRxPdoTable;
    tPdoIndex           m_RxPdoCount;

    tPdoTabEntry MEM*   m_pTxPdoTable;
    tPdoIndex           m_TxPdoCount;

} tPdoTabParam;

typedef struct
{

#if (COP_MAX_INSTANCES > 1)

    void MEM*           m_ObdInstance;
    void MEM*           m_CobInstance;

#endif

    tPdoTabParam        m_PdoTabParam;
    tCcmErrorCallback   m_fpErrorCallback;
    BYTE                m_bGranularity;

} tPdoInitParam;


typedef struct
{
    WORD    m_wPdoCommuIndex;
    WORD    m_wVarIndex;
    BYTE    m_bVarSubindex;
    BYTE    m_bAdditionalRet;

} tPdoError;



// ============================================================================
// functionsprototypes
// ============================================================================


// ----------------------------------------------------------------------------
//
// Function:    PdoInit
//
// Description: function clears instance table and initializes the first instance
//              of PDO module
//
// Parameters:  MCO_DECL_PTR_INSTANCE_PTR_  = (address to instance pointer)
//              pInitParam_p                = pointer to initialising struct
//
// Returns:     tCopKernel                  = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC PdoInit (MCO_DECL_PTR_INSTANCE_PTR_
    tPdoInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
//
// Function:    PdoAddInstance
//
// Description: function adds a new instance of PDO module
//
// Parameters:  MCO_DECL_PTR_INSTANCE_PTR_  = (address to instance pointer)
//              pInitParam_p                = pointer to initialising struct
//
// Returns:     tCopKernel                  = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC PdoAddInstance (MCO_DECL_PTR_INSTANCE_PTR_
    tPdoInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
//
// Function:    PdoDeleteInstance
//
// Description: function deletes an instance of PDO module
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC PdoDeleteInstance (MCO_DECL_INSTANCE_PTR);


// ----------------------------------------------------------------------------
//
// Function:    PdoRegisterPdoTab
//
// Description: function registers all PDO tables (RX and TX)
//
// Parameters:  MCO_DECL_PTR_INSTANCE_PTR_ = (address to instance pointer)
//              pPdoTabParam_p             = pointer to PDO table parameter
//
// Returns:     tCopKernel = error code of ObdInitModuleTab()
//
// ----------------------------------------------------------------------------
#if (PDO_USE_ADDITIONAL_API != FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoRegisterPdoTab (MCO_DECL_INSTANCE_PTR_
    tPdoTabParam GENERIC* pPdoTabParam_p);
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoNmtEvent
//
// Description: function is called, when an nmt-event is occured
//              The following NMT-events are interesting for PDO module:
//
//              kNmtEvPreResetCommunication
//              kNmtEvResetCommunication: 
//              kNmtEvEnterOperational:
// 
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              NmtEvent_p              = nmt-event
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC PdoNmtEvent (MCO_DECL_INSTANCE_PTR_
    tNmtEvent NmtEvent_p);


// ----------------------------------------------------------------------------
//
// Function:    PdoDefineCallback
//
// Description: function writes the pointer to the callbackfunction into the 
//              corresponding entry of the pdo-tab
// 
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              wCommuIndex_p           = communication index
//              fpPdoCallback_p         = address to callback function
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC PdoDefineCallback (MCO_DECL_INSTANCE_PTR_
    WORD wCommuIndex_p,
    tPdoCallback fpPdoCallback_p);


//---------------------------------------------------------------------------
//
// Function:    PdoStaticDefineVarField()
//
// Description: function defines a PDO variable field for static PDO
//              mapping
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              wPdoCommuIndex_p        = communication index of the PDO
//              pData_p                 = pointer to a variable field (max. 8 bytes)
//              bSize_p                 = number of mapped bytes within PDO
//
// Returns:     error code
//                  kCopSuccessful
//                  kCopPdoNotExist
//
//---------------------------------------------------------------------------

#if (PDO_USE_STATIC_MAPPING != FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoStaticDefineVarField (MCO_DECL_INSTANCE_PTR_
    WORD wCommuIndex_p, void MEM* pData_p, BYTE bSize_p);
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoSend
//
// Description: function sends a pdo without check of conditions
// 
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              wPdoCommuIndex_p        = communication index of the PDO
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC PdoSend (MCO_DECL_INSTANCE_PTR_
    WORD wPdoCommuIndex_p);


//---------------------------------------------------------------------------
//
// Function:    PdoSignalStaticPdo()
//
// Description: function marks a PDO for send while next call of PdoProcessAsync()
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              wPdoCommuIndex_p        = communication index of the PDO
// 
// Returns:     tCopKernel = error code
//                  kCopSuccessful
//                  kCopPdoNotExist
//
//---------------------------------------------------------------------------

#if (PDO_USE_STATIC_MAPPING != FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoSignalStaticPdo (MCO_DECL_INSTANCE_PTR_
    WORD wCommuIndex_p);
#endif


//---------------------------------------------------------------------------
//
// Function:    PdoSignalDynPdo()
//
// Description: function signals a PDO for sending while next calling
//              PdoProcessAsync()
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              wPdoCommuIndex_p        = communication index of the PDO
//
// Returns:     error code
//                  kCopSuccessful
//                  kCopPdoNotExist
//
//---------------------------------------------------------------------------

#if (PDO_USE_STATIC_MAPPING == FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoSignalDynPdo (MCO_DECL_INSTANCE_PTR_
    WORD wPdoCommuIndex_p);
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoSignalVar
//
// Description: function marks a variable for sending PDO in which it is mapped
//              (only dynamic mapping)
//              (will be send if function PdoProcessAsync() is is called)
// 
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              pVar_p                  = pointer to the variable
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if (PDO_USE_STATIC_MAPPING == FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoSignalVar (MCO_DECL_INSTANCE_PTR_
    void MEM* pVar_p);
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoSignalPdo
//
// Description: function marks a PDO for sending (only static mapping)
//              (will be send if function PdoProcessAsync() is is called)
// 
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              wPdoCommuIndex_p        = communication index of the PDO
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if (PDO_USE_STATIC_MAPPING != FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoSignalPdo (MCO_DECL_INSTANCE_PTR_
    WORD wCommuIndex_p);
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoProcessCheckVar
//
// Description: function checks the pdos with TxType 0, 253 and 254 for 
//              changing and marks this pdos for send when they are changed
// 
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              ProcessTime_p           = max. time for return from this function [0.1ms]
//              pfPdoIsToSend_p         = this pointer addresses a flag
//                                        if a variable is changed this flag will be set to TRUE
//                                        in this case PdoProcessAsync() has to be called
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC PdoProcessCheckVar (MCO_DECL_INSTANCE_PTR_
    tTime ProcessTime_p,
    BYTE MEM* fPdoIsToSend_p);


// ----------------------------------------------------------------------------
//
// Function:    PdoProcessAsync
//
// Description: function checks the pdos with TxType 254 and 255 for 
//              sending and sends they
// 
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              pTime_p                 = this parameter has different meanings:
//
//              1) no meaning if FUNCTION_NO_CONTROL is set
//              2) max. time until return from this function [0.1ms] if FUNCTION_BREAK_TIME is set
//              3) relative time for next calling of this function [0.1ms] if FUNCTION_CALL_TIME is set
//
//              The meaning is configured by #define PDO_PROCESS_TIME_CONTROL in copcfg.h
//              It can be set to FUNCTION_NO_CONTROL (1), FUNCTION_BREAK_TIME (2) or FUNCTION_CALL_TIME (3).
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC PdoProcessAsync (MCO_DECL_INSTANCE_PTR_
    tTime MEM* pTime_p);


// ----------------------------------------------------------------------------
//
// Function:    PdoProcessMux
//
// Description: function handles multiplexed pdos
//              It is to be defined!!!
// 
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              ProcessTime_p           = max. time for return from this function
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if (PDO_USE_MUX_PDOS != FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoProcessMux (MCO_DECL_INSTANCE_PTR_
    tTime ProcessTime_p);
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoProcessSync()
//
// Description: function checks communication cycle period (only for SYNC Producer)
//              sync message will be sent if time is over
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              pNextCallTime_p         = address to time variable for next calling PdoProcessSync()
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if (PDO_USE_SYNC_PRODUCER != FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoProcessSync (MCO_DECL_INSTANCE_PTR_
    tTime MEM* pNextCallTime_p);
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoSendSync()
//
// Description: function sends the SYNC message without checking cycle time
//              It sends only if NMT state is unequal to STOPPED and SYNC
//              producer is activated. (only for SYNC Producer)
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if (PDO_USE_SYNC_PRODUCER != FALSE)
    COPDLLEXPORT tCopKernel PUBLIC PdoSendSync (MCO_DECL_INSTANCE_PTR);
#else
    // this prevents linker error when PDO_USE_SYNC_PRODUCER...==FALSE but object 0x1006 is defined
    #define PdoSendSync         NULL
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoCbCfgSyncId
//
// Description: function is called from SDO-Module when the entry for sync-id 
//              in OD will be changed (0x1005)
// 
// Parameters:  MCO_DECL_INSTANCE_HDL_  = (handle of instance)
//              pParam_p                = pointer to callback parameter struct
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if ((PDO_USE_SYNC_PRODUCER != FALSE) || (PDO_USE_SYNC_PDOS != FALSE))
    COPDLLEXPORT tCopKernel PUBLIC PdoCbCfgSyncId (CCM_DECL_INSTANCE_HDL_
        tObdCbParam MEM* pParam_p);
#else
    // this prevents linker error when PDO_USE_SYNC_...==FALSE but object 0x1005 is defined
    #define PdoCbCfgSyncId      NULL
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoCbCfgSyncWindow
//
// Description: function will be called from SDO or OBD module, when object
//              0x1007 for SYNC window length is accessed
// 
// Parameters:  MCO_DECL_INSTANCE_HDL_  = (handle of instance)
//              pParam_p                = pointer to callback parameter struct
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if (PDO_USE_SYNC_PDOS != FALSE)
    COPDLLEXPORT tCopKernel PUBLIC PdoCbCfgSyncWindow (CCM_DECL_INSTANCE_HDL_
        tObdCbParam MEM* pParam_p);
#else
    // this prevents linker error when PDO_USE_SYNC_PDOS...==FALSE but object 0x1005 is defined
    #define PdoCbCfgSyncWindow  NULL
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoCbCfgSyncCycle
//
// Description: function will be called from SDO or OBD module, when object
//              0x1006 for SYNC cycle period is accessed
// 
// Parameters:  MCO_DECL_INSTANCE_HDL_  = (handle of instance)
//              pParam_p                = pointer to callback parameter struct
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if (PDO_USE_SYNC_PRODUCER != FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoCbCfgSyncCycle (CCM_DECL_INSTANCE_HDL_
    tObdCbParam MEM* pParam_p);
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoCbConfigCommu
//
// Description: function is called from SDO-Module when an entry for Rx- or Tx-
//              communication parameter in OD will be changed 
// 
// Parameters:  MCO_DECL_INSTANCE_HDL_  = (handle of instance)
//              pParam_p                = pointer to callback parameter struct
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC PdoCbConfigCommu (CCM_DECL_INSTANCE_HDL_
    tObdCbParam MEM* pParam_p);


// ----------------------------------------------------------------------------
//
// Function:    PdoCbConfigMapp
//
// Description: function is called from SDO-Module when an entry for Rx- or Tx-
//              mapping parameter in OD will be changed 
// 
// Parameters:  MCO_DECL_INSTANCE_HDL_  = (handle of instance)
//              pParam_p                = pointer to callback parameter struct
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if (PDO_USE_STATIC_MAPPING == FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoCbConfigMapp (CCM_DECL_INSTANCE_HDL_
    tObdCbParam MEM* pParam_p);
#endif


// ----------------------------------------------------------------------------
//
// Function:    PdoSetSyncCallback
//
// Description: function is called to set the pointer to CB-Function into Inst-tab
//              (only for SYNC Consumer)
// 
// Parameters:  MCO_DECL_INSTANCE_HDL_  = (handle of instance)
//              fpSyncCallback_p        = pointer to callback-funtion
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

#if (PDO_USE_SYNC_PDOS != FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoSetSyncCallback (MCO_DECL_INSTANCE_PTR_
    tPdoSyncCallback fpSyncCallback_p);
#endif

//---------------------------------------------------------------------------
//
// Function:    PdoForceAsynPdo()
//
// Description: function force all async TxPDO's independent of
//              Inhibittime or Eventtimer
//
// Parameters:  MCO_DECL_INSTANCE_HDL  = (handle of instance)
//
// Returns:     tPdoTxMode  = transmission mode
//
//---------------------------------------------------------------------------

void PUBLIC PdoForceAsynPdo (MCO_DECL_INSTANCE_PTR);


//---------------------------------------------------------------------------
//
// Function:    PdoSendMPDO()
//
// Description: sends a multiplexed PDO
//
// Parameters:  wPdoCommuIndex_p    = communication index of the PDO
//              bNodeId_p           = node-id of consumer
//              pMPdoParam_p        = pointer to parameter struct
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

#if (PDO_USE_MPDO_DAM_PRODUCER != FALSE)
COPDLLEXPORT tCopKernel PUBLIC PdoSendMPDO (MCO_DECL_INSTANCE_PTR_
    WORD wPdoCommuIndex_p, BYTE bNodeId_p, tMPdoParam GENERIC* pMPdoParam_p);
#endif

#endif //_PDO_H_

// Please keep an empty line at the end of this file.

