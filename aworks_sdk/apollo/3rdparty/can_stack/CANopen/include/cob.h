/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.com

  Project:      CANopen V5

  Description:  Header Communication Object Module (COB)

  -------------------------------------------------------------------------

                $RCSfile: Cob.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.37 $  $Date: 2007/01/08 13:55:20 $
                
                $State: Exp $
                
                Build Environment:
                    KEIL uVision 2
                    MSVC 6.0
                    Tasking C166

  -------------------------------------------------------------------------

  Revision History:

****************************************************************************/


#ifndef _COB_H_
#define _COB_H_


//---------------------------------------------------------------------------
// constants
//---------------------------------------------------------------------------

#define COB_SEARCHALGO_LIST         0
#define COB_SEARCHALGO_INDEXARRAY   1

// specific values as reasons for calling callbackfunction
typedef tFastByte tCobEvent;

#define kCobEvtReceived             0   // communication object received
#define kCobEvtSent                 1   // communication object sent because received RTR frame
// ... further values


// Type tCobHdl depends on define COB_MORE_THAN_64_ENTIES. If more than
// 64 entries are used for each COB table it has to be set to TRUE. So
// data memory can be kept back for other using.
// The enum eCobHdl includes masks for type tCobHdl. Variables
// should never be type of eCobHdl because KEIL compiler C51 is only
// able to provide 8-bit enums.
#if (COB_MORE_THAN_128_ENTRIES != FALSE)

    typedef WORD tCobHdl;

    #define kCobHdlRxTable      0x0000
    #define kCobHdlTxTable      0x8000
    #define kCobHdlIndexMask    0x7FFF
    #define kCobHdlTableMask    0x8000
    #define kCobHdlInvalid      0xFFFF

#else

    typedef BYTE tCobHdl;

    #define kCobHdlRxTable      0x00
    #define kCobHdlTxTable      0x80
    #define kCobHdlIndexMask    0x7F
    #define kCobHdlTableMask    0x80
    #define kCobHdlInvalid      0xFF

#endif

#if (COB_MORE_THAN_256_ENTRIES != FALSE)

    #if (COB_MORE_THAN_128_ENTRIES == FALSE)

        #error 'ERROR: configurartion wrong COB_MORE_THAN_128_ENTRIES <-> COB_MORE_THAN_256_ENTRIES!'

    #endif

    typedef WORD tCobIdx;

    #define COB_INVALID_INDEX   0xFFFF

#else

    typedef BYTE tCobIdx;

    #define COB_INVALID_INDEX   0xFF

#endif


// These constants define types of communication objects. Bits 3 and 4
// are reserved because kMtbHighPrio and kMtbExtend from enum tMsgTypBit
// (cdrv.h) has to be set in m_bCobType of tCobParam if necessary.
typedef BYTE tCobType;

#define kCobTypSend         (kMsgTypSend)                   // for standard CAN messages to send
#define kCobTypRecv         (kMsgTypRecv)                   // for standard CAN messages to receive
#define kCobTypRmtSend      (kMsgTypSendRtrReq)             // for receiving data as answer of RTR frame
#define kCobTypRmtRecv      (kMsgTypRecvRtrReq)             // for sending data as answer of RTR frame
#define kCobTypForceSend    (kMsgTypSend | kMtbForce)       // for standard CAN messages to send and forced
                                                            // by received RTR frame
#define kCobTypForceRmtSend (kMsgTypSendRtrReq | kMtbForce) // for compatibility reason
#define kCobTypForceRmtRecv (kMsgTypRecvRtrReq | kMtbForce) // for sending data as answer of RTR frame and with
                                                            // CobSend(immediately=TRUE)

// filter for COB type
#define kCobTypFilter       (kMtbFilter | kMtbForce)

#define kCobTypRtrInvalid   (0x10)

// reserved bits which are not allowed to be changed
#define kCobReserved1       (kMtbForce)
#define kCobReserved2       (kMtbFirstMsg)
#define kCobReserved3       (kMtbHighPrio)
#define kCobReserved4       (kMtbExtend)


//---------------------------------------------------------------------------
// security check
//---------------------------------------------------------------------------

#ifdef COB_MORE_THAN_64_ENTRIES
    #error 'ERROR: old define is used! check CopCfg.h!!!'
    // you have to define COB_MORE_THAN_128_ENTRIES instead of COB_MORE_THAN_64_ENTRIES !!!
#endif

#if ((COB_SEARCHALGO == COB_SEARCHALGO_INDEXARRAY) && (CDRV_CAN_SPEC == CAN20B))
    #error 'ERROR: search algorithm "Index Array" can not be used for 29 bit identifiers!!!'
#endif


//---------------------------------------------------------------------------
// types
//---------------------------------------------------------------------------

// Note:
// Callback functions should not have much parameters because 8-bit
// compilers must fit these parameters only in registers R0 till R7.
typedef tCopKernel (PUBLIC ROM *tCobCallback) (CCM_DECL_INSTANCE_HDL_
    //tCobHdl CobHdl_p,
    void MEM* pArg_p,
    tCobEvent Event_p);

// COB tables internaly consists of this structure.
typedef struct
{
    BYTE         m_bUsed;       // entry is used or unused
    BYTE         m_bCobType;    // values from tCobType
    tCdrvMsg     m_CanMsg;      // CAN message for CAN driver
    tCobCallback m_fpCallback;  // Callback function for this object
    void MEM*    m_pArg;        // arument for callback function

} tCobEntry;

// Caller of functions of COB module only use this structure.
typedef struct
{
    // Input
    tCanId       m_CobId;       // CAN-ID der CAN Nachricht
    BYTE         m_bCobType;    // values from tCobType
    BYTE         m_bSize;       // Länge des CAN Frames
    tCobCallback m_fpCallback;  // Callbackfunktion für den COB Eintrag
    void MEM*    m_pArg;        // arument for callback function

    // Output
    tCobHdl      m_CobHdl;      // handle for COB (input for CobGetParam() and CobSetParam())
    BYTE    MEM* m_pbData;
    BYTE    MEM* m_pbRecvdSize; // address to count of received bytes

} tCobParam;

// A little stucture to initialze COB table.
typedef struct
{
    tCobEntry MEM* m_pBase;         // base address of COB table
    tCobEntryIndex m_MaxEntries;    // count of entries in COB table

} tCobInfo;

// Parameters for initializing COB module.
typedef struct
{

    #if (COP_MAX_INSTANCES > 1)

        void MEM*       m_CdrvInstance;

    #endif

    #if (COP_USE_CDRV_FUNCTION_POINTER != FALSE)

        tCobCdrvFct     m_CdrvFct;

    #endif

    tCobInfo            m_RxCobInfo;        // Info für die Tabelle aRxCobEntryx
    tCobInfo            m_TxCobInfo;        // Info für die Tabelle aTxCobEntryx

    tCcmErrorCallback   m_fpErrorCallback;

} tCobInitParam;


//---------------------------------------------------------------------------
// PUBLIC functions
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CobInit (MCO_DECL_PTR_INSTANCE_PTR_
    tCobInitParam MEM* pInitParam_p);

COPDLLEXPORT tCopKernel PUBLIC CobAddInstance (MCO_DECL_PTR_INSTANCE_PTR_
    tCobInitParam MEM* pInitParam_p);

COPDLLEXPORT tCopKernel PUBLIC CobDeleteInstance (MCO_DECL_INSTANCE_PTR);

COPDLLEXPORT tCopKernel PUBLIC CobCheck (MCO_DECL_INSTANCE_PTR_  
    DWORD dwCANID_p, 
//  tCobType CobType_p);
    tCobParam MEM* pCobParam_p);

COPDLLEXPORT tCopKernel PUBLIC CobDefine (MCO_DECL_INSTANCE_PTR_
    tCobParam MEM*  pCobParam_p);

COPDLLEXPORT tCopKernel PUBLIC CobClearRtr (MCO_DECL_INSTANCE_PTR_
    tCobHdl CobHdl_p);

COPDLLEXPORT tCopKernel PUBLIC CobUndefine (MCO_DECL_INSTANCE_PTR_
    tCobHdl CobHdl_p);

COPDLLEXPORT tCopKernel PUBLIC CobGetParam (MCO_DECL_INSTANCE_PTR_
    tCobParam MEM* pCobParam_p);

COPDLLEXPORT tCopKernel PUBLIC CobSetParam (MCO_DECL_INSTANCE_PTR_
    tCobParam MEM* pCobParam_p);

COPDLLEXPORT void PUBLIC CobResetParam (tCobParam MEM* pCobParam_p);

COPDLLEXPORT tCopKernel PUBLIC CobSend (MCO_DECL_INSTANCE_PTR_
    tCobHdl CobHdl_p, BYTE fSendRtrDataImmediately_p);

COPDLLEXPORT tCopKernel PUBLIC CobProcessRecvQueue (MCO_DECL_INSTANCE_PTR_
    BYTE LowBuffProcessCount_p);


//---------------------------------------------------------------------------
//
// Function:    CobProcessMsg()
//
// Description: processes a received CAN message from receive buffer
//
// Parameters:  pCanMsg_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CobProcessMsg (MCO_DECL_INSTANCE_PTR_
    tCdrvMsg MEM* pCanMsg_p);


//---------------------------------------------------------------------------
//
// Function:    CobProcessCANState()
//
// Description: reads the CAN state and calls CcmCbError() if state has been changed
//
// Parameters:  void
//
// Return:      void
//
//---------------------------------------------------------------------------

COPDLLEXPORT void PUBLIC CobProcessCANState (MCO_DECL_INSTANCE_PTR);


#endif //_COB_H_

// Please keep an empty line at the end of this file.
