/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.com

  Project:      CANopen V5

  Description:  Header file for SDO client module

  -------------------------------------------------------------------------

                $RCSfile: Sdoc.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.31 $  $Date: 2007/01/08 13:45:04 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

  r.slf.: first implementation

  2006/01/30 r.d.:  Code optimisation:
                    - Functions SdocGetTransmittedBytes(), SdocGetFreeSdoClient()
                      SdocGetNextFreeClient() and SdocGetClientNumber() are
                      only available if define SDOC_USE_ADDITIONAL_API is set
                      to TRUE

  2006/08/14 r.d.:  - new SDO state kSdocSegUpWaitLen for checking illegal
                      SDO sequences on CAN bus

****************************************************************************/

#ifndef _SDOCCOMM_H_
#define _SDOCCOMM_H_


//=========================================================================
// definition of types, enums and structures
//=========================================================================

//-------------------------------------------------------------------------
// define sdo client states for state machine
//-------------------------------------------------------------------------

typedef tFastByte               tSdocInternalState;
#define kSdocInit               0
#define kSdocIdle               1
#define kSdocSegDownInit        2
#define kSdocSegDownExpedited   3
#define kSdocSegDown            4
#define kSdocSegUpInit          5
#define kSdocSegUpExpedited     6
#define kSdocSegUpWaitLen       7+128       // 14.08.2006 r.d.: new SDO state for checking illegal SDO sequences on CAN bus
#define kSdocSegUp              7
#define kSdocBlockDownInit      8
#define kSdocBlockDownInitWait  9
#define kSdocBlockDown          10
#define kSdocBlockDownEndBlock  11
#define kSdocBlockDownEnd       12
#define kSdocBlockDownEndWait   13
#define kSdocBlockUpInit        14
#define kSdocBlockUpInitWait    15
#define kSdocBlockUp            16
#define kSdocBlockUpEnd         17
#define kSdocAbort              18
#define kSdocFinish             19


//-------------------------------------------------------------------------
// Type of client table
//-------------------------------------------------------------------------

typedef struct
{
    tSdocState          m_SdocTransferState;// state of SDO transfer (for API functions)
    tSdocInternalState  m_SdocInternalState;// internal state of SDOC state machine
    DWORD               m_dwAbortCode;      // occured received/sended sdo abort code
    tTime               m_TimeOut;          // configured timeout
    
    // parameter from OBD (RX and TX-CODID are stored in m_RxCobParam or m_TxCobParam) 
    WORD                m_wClientIndex;     // Index of client (area of 0x1280-0x12FF)
    BYTE                m_bServerNodeId;    // Node-ID of server
    DWORD               m_dwRxCobId;        // Cob-ID Client<-Server
    DWORD               m_dwTxCobId;        // Cob-ID Client->Server

    // objects for COB modul access
    tCobHdl             m_RxCobHdl;         // handle for selection of RxCOB
    BYTE  MEM*          m_pbRxCobData;      // pointer to RxCOB data (this is one CAN message)
    tCobHdl             m_TxCobHdl;         // handle for selection of TxCOB
    BYTE  MEM*          m_pbTxCobData;      // pointer to TxCOB data (this is one CAN message)

    // parameters that used during transmission
    BYTE  GENERIC*      m_pbData;           // pointer to object
    tObdSize            m_DataLen;          // SDO-transfer data length
    tObdSize            m_DataRestLen;      // size of data bytes still to transfer
    tTime               m_StartTime;        // start time for timeout checking
    BYTE                m_bSegInfo;         // protocol info for segmented transmission
    tSdocType           m_SdocType;         // transfer type (down- or upload)

    // pointer to and parameter for user callback function
    tSdocCbFinished     m_pfnCbFinished;    // callback function is call after transfer end
                                            // (error or success)
    tSdocCbFinishParam  m_CbParam;          // parameter structure for callback function

    // Index and subindex of object that transmitted from server 
    // This parameters are only use to send abort messages because this messages
    // includes the abort code and index and subindex of the transmitted object
    WORD                m_wObjIndex;        // object index
    BYTE                m_bObjSubIndex;     // object subindex

#if (SDO_BLOCKTRANSFER != FALSE)      

    // variables for block transmission only
    BYTE  GENERIC*      m_pbBlockStart;     // pointer to first data address of activ block
    WORD                m_wCRC;             // result of CRC calculation
    BYTE                m_bBlockSize;       // number of sequences per block
    BYTE                m_bLastSeqNo;       // number of last valid sequence 
    BYTE                m_bInvalidBytes;    // number of invalid bytes in last segment of last block

#endif

} tSdocTabEntry;



//-------------------------------------------------------------------------
// Struct for global vars of one CANopen-instance
//-------------------------------------------------------------------------
// Struct to intitialize the SDO client part of instance table
//-------------------------------------------------------------------------

typedef struct
{
#if (COP_MAX_INSTANCES > 1)

    void MEM*       m_ObdInstance;       // pointer to instance table of OBD-modul
    void MEM*       m_CobInstance;       // pointer to instance table of COB-modul

#endif

    void MEM*      m_pSdocTable;        // pointer to the SDO client table
    BYTE           m_bSdocTabEntries;   // according to the number of client in OBD-modul
    tTime          m_TimeOut;

} tSdocInitParam;


//=========================================================================
// public function prototypes
//=========================================================================

//-------------------------------------------------------------------------
// function to initialize SDO clients
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdocInit (MCO_DECL_PTR_INSTANCE_PTR_
    tSdocInitParam MEM* pSdocInitParam_p);


//-------------------------------------------------------------------------
// function to define a new instance
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdocAddInstance (MCO_DECL_PTR_INSTANCE_PTR_
    tSdocInitParam MEM* pSdocInitParam_p);


//-------------------------------------------------------------------------
// function to delete an instance
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdocDeleteInstance (MCO_DECL_INSTANCE_PTR);


//-------------------------------------------------------------------------
// function to process a NMT event
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdocNmtEvent (MCO_DECL_INSTANCE_PTR_
    tNmtEvent        NmtEvent_p);


//-------------------------------------------------------------------------
// function to start a SDO transfer
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdocInitTransfer (MCO_DECL_INSTANCE_PTR_
    tSdocTransferParam MEM* pSdocTransferParam_p);


//-------------------------------------------------------------------------
// function to get the SDO transfer state and as additional information
// the abort code if an error is occured
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdocGetState (MCO_DECL_INSTANCE_PTR_
    WORD             wClientIndex_p,
    tSdocState MEM*  pSdocState_p,
    DWORD      MEM*  pdwAbortCode_p);


//-------------------------------------------------------------------------
// function to get the number of transmitted data bytes
//-------------------------------------------------------------------------
#if (SDOC_USE_ADDITIONAL_API != FALSE)
COPDLLEXPORT tCopKernel PUBLIC SdocGetTransmittedBytes (MCO_DECL_INSTANCE_PTR_
    WORD          wClientIndex_p,
    tObdSize MEM* pDataSize_p);
#endif


//-------------------------------------------------------------------------
// function to get the index of the next free client
//-------------------------------------------------------------------------
#if (SDOC_USE_ADDITIONAL_API != FALSE)
COPDLLEXPORT tCopKernel PUBLIC SdocGetNextFreeClient (MCO_DECL_INSTANCE_PTR_
    BYTE MEM* pbClientIndex_p);
#endif


//-------------------------------------------------------------------------
// Function:     SdocGetNextFreeClient
//
// Description:  The function gets the index of the next free client
//
// Parameters:   MCO_DECL_INSTANCE_PTR_  - instance handle
//               pbClientIndex_p         - Pointer to variable in
//                                         user application. The function
//                                         writes the index of the next
//                                         free client into this variable.
//
// Returns:      tCopKernel              - error code
//-------------------------------------------------------------------------
#if (SDOC_USE_ADDITIONAL_API != FALSE)
COPDLLEXPORT tCopKernel PUBLIC SdocGetFreeSdoClient (MCO_DECL_INSTANCE_PTR_
    WORD MEM* pwClientIndex_p);
#endif


//-------------------------------------------------------------------------
//
// Function:    SdocConvertClientIndex
//
// Description: The function converts server node-id to client index or reverse
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = instance handle
//              pbServerNodeId_p        = [IN/OUT] pointer to server node-id
//                                          if INPUT is != 0 then pwClientIndex_p is OUTPUT
//                                          otherwise pbServerNodeId_p is OUTPUT and pwClientIndex_p is INPUT
//              pwClientIndex_p         = [OUT/IN] pointer to client index in OD
//
// Returns:     tCopKernel              = error code
//
//-------------------------------------------------------------------------
#if (SDOC_USE_ADDITIONAL_API != FALSE)
COPDLLEXPORT tCopKernel PUBLIC SdocConvertClientIndex (MCO_DECL_INSTANCE_PTR_
    BYTE MEM* pbServerNodeId_p,
    WORD MEM* pwClientIndex_p);
#endif


//-------------------------------------------------------------------------
// function to get the number of SDO clients
//-------------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC SdocGetClientNumber (MCO_DECL_INSTANCE_PTR_
    BYTE MEM* pbClientNumber_p);

//-------------------------------------------------------------------------
// functions to define a client
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdocDefineClient (MCO_DECL_INSTANCE_PTR_
    tSdocParam GENERIC* pSdocClientParam_p);


//-------------------------------------------------------------------------
// functions to undefine a client
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdocUndefineClient (MCO_DECL_INSTANCE_PTR_
    WORD             wClientIndex_p);


//-------------------------------------------------------------------------
// function to change client parameter as callback function
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdocCbConfigClient (CCM_DECL_INSTANCE_HDL_
    tObdCbParam MEM* pParam_p);


//-------------------------------------------------------------------------
// function to process SDO client transmissions
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdocProcess (MCO_DECL_INSTANCE_PTR_
    tTime MEM*       pNextCallTime_p);


//-------------------------------------------------------------------------
// function to abort a running SDO transmission
//-------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC SdocAbort (MCO_DECL_INSTANCE_PTR_
    WORD            wClientIndex_p,
    DWORD           dwAbortCode_p);

#endif //#ifndef _SDOCCOMM_H_

// Please keep an empty line at the end of this file.
