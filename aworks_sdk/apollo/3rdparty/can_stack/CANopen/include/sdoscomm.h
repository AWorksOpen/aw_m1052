/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  interface for SDO-Server functions

  -------------------------------------------------------------------------

                $RCSfile: SdosComm.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.43 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

  a.s.: first implementation

****************************************************************************/

#ifndef _SDOSCOMM_H_
#define _SDOSCOMM_H_


//---------------------------------------------
// define control flags for state machine
#define STABIL   TRUE
#define INSTABIL FALSE

// -------------------------------------------------------------------------
// typedefs

//---------------------------------------------
// define sdo server state for state machine
typedef tFastByte           tSdosState;
#define kSdosInit           0
#define kSdosIdle           1
#define kSdosSegDown        2
#define kSdosSegUp          3
#define kSdosBlockDown      4
#define kSdosBlockDownEnd   5
#define kSdosBlockUpInit    6
#define kSdosBlockUp        7
#define kSdosBlockUpEnd     8
#define kSdosAbort          9
#define kSdosFinish         10


// -------------------------------------------------------------------------
// Type of Server tab

typedef struct
{
    tSdosState      m_bSDOState;      // activ state of SDOS-state machine
    tTime           m_dwTimeOut;      // configured timeout

    // parameter from OBD (RX and TX-CODID are stored in m_RxCobParam or m_TxCobParam) 
    WORD            m_wIndex;         // Index of server (area of 0x1200-0x127F)
    BYTE            m_bClientNodeID;  // NodeID of client
    DWORD           m_dwRxCobID;      // Cob-ID Client->Server
    DWORD           m_dwTxCobID;      // Cob-ID Server->Client

    // objects for COB modul access
    tCobHdl         m_RxCobHdl;       // handle for selection of RxCOB
    BYTE  MEM*      m_pbRxCobData;    // pointer to RxCOB data (this is one CAN message)
    tCobHdl         m_TxCobHdl;       // handle for selection of TxCOB
    BYTE  MEM*      m_pbTxCobData;    // pointer to TxCOB data (this is one CAN message)

    // interface to OBD modul
    tObdSubEntryPtr m_pSubEntry;
    tObdCallback    m_fpCallback;   // function is called back if object access
    tObdCbParam     m_CbParam;       // data struct for callback function

    // parameters that used during transmission
    BYTE GENERIC*   m_pbObjData;      // pointer to object (will be changed at segmented or block transfer)
    tObdSize        m_ObjLen;         // SDO-transfer data length
    BYTE            m_bObjType;       // object type (BYTE, WORD, DWORD, ...)
    BYTE            m_bSegInfo;       // protocoll info for segmented transmission
    tTime           m_dwStartTime;    // start time for timeout checking
    DWORD           m_adwRxNumBuf[2]; // temporarly buffer for numerical values

#if (SDO_BLOCKTRANSFER != FALSE)     // only for block transmission
    BYTE GENERIC*   m_pbBlockStart;   // pointer to first data address of activ block
    tObdSize        m_ObjRestLen;     // size of data bytes at start block transmission
    WORD            m_wCRC;           // result of CRC-calc
    BYTE            m_bBlockSize;     // number of segments per block
    BYTE            m_bLastSeqNo;     // number of segment
#endif

} tSdosTabEntry;

// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
// Struct for global vars of one CANopen-instance

//--------------------------------------------------------------------------
// struct to intitialize the SDOS server part of instance table
typedef struct
{
#if (COP_MAX_INSTANCES > 1)

    void MEM*       m_ObdInstance;       // pointer to instance table of OBD-modul
    void MEM*       m_CobInstance;       // pointer to instance table of COB-modul

#endif


    tTime          m_dwTimeOut;
    void MEM*      m_pSdosTable;        // pointer to the SDOS-server table
    BYTE           m_bSdosTabEntries;   // according to the number of server in OBD-modul

} tSdosInitParam;
    

// -------------------------------------------------------------------------
// function prototypes

// -------------------------------------------------------------------------
// functions to init. and process SDO-servers
// -------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdosInit (MCO_DECL_PTR_INSTANCE_PTR_ 
    tSdosInitParam MEM* pSdosInitParam_p);


// -------------------------------------------------------------------------
// function to define a new instance
// -------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdosAddInstance (MCO_DECL_PTR_INSTANCE_PTR_ 
    tSdosInitParam MEM* pSdosInitParam_p);


// -------------------------------------------------------------------------
// function to process an NMT event
// -------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdosNmtEvent (MCO_DECL_INSTANCE_PTR_
    tNmtEvent NmtEvent_p);


// -------------------------------------------------------------------------
// function to delete a instance
// -------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdosDeleteInstance (MCO_DECL_INSTANCE_PTR);


// -------------------------------------------------------------------------
//
// Function:     SdosGetFreeSdoServer()
//
// Description:  Function returns a free SDO server.
//
// Parameters:   MCO_DECL_INSTANCE_PTR_  = instance handle
//               pwServerIndex_p         = pointer to variable to receive free server index
//
// Returns:      tCopKernel              = error code
//
// -------------------------------------------------------------------------
#if (SDOS_USE_ADDITIONAL_API != FALSE)
COPDLLEXPORT tCopKernel PUBLIC SdosGetFreeSdoServer (MCO_DECL_INSTANCE_PTR_
    WORD MEM* pwServerIndex_p);
#endif

// -------------------------------------------------------------------------
// functions to define an server
// -------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdosDefineServer (MCO_DECL_INSTANCE_PTR_
    tSdosParam MEM* pSdosServerParam_p);


// -------------------------------------------------------------------------
// functions to define an server
// -------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdosUndefineServer (MCO_DECL_INSTANCE_PTR_
    WORD  wServerIndex_p,
    DWORD dwAbortCode_p);


// -------------------------------------------------------------------------
// function to change server parameter as callback function
// -------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdosCbConfigServer (CCM_DECL_INSTANCE_HDL_ 
    tObdCbParam MEM* pParam_p);


// -------------------------------------------------------------------------
// function to process SDO server states
// -------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdosProcess (MCO_DECL_INSTANCE_PTR_
    tTime MEM* pTime_p);


// -------------------------------------------------------------------------
// function to abort an running SDO transfer
// -------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdosAbort (MCO_DECL_INSTANCE_PTR_  
    WORD wServerIndex_p, 
    DWORD dwAbortCode_p);


#endif

// Please keep an empty line at the end of this file.
