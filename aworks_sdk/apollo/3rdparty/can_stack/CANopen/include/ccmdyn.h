/****************************************************************************


  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  Header file for CANopen API for dynamic OD

  -------------------------------------------------------------------------

                $RCSfile: CcmDyn.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.7 $  $Date: 2006/09/21 09:37:12 $
                
                $State: Exp $
                
                Build Environment:
                    KEIL uVision 2

  -------------------------------------------------------------------------

  Revision History:
  2004/02/27 -as:   Erstellung

****************************************************************************/

#ifndef _CCMDYN_H_
#define _CCMDYN_H_


//---------------------------------------------------------------------------
// typedef
//---------------------------------------------------------------------------

typedef struct
{
    // Address and size of process image
    BYTE FAR* m_pbProcessImage;
    DWORD     m_dwProcessImageSize;

    // Offset and size of inputs
    DWORD     m_dwDynPIOffsetIn;
    DWORD     m_dwDynPISizeIn;

    // Offset and size of outputs
    DWORD     m_dwDynPIOffsetOut;
    DWORD     m_dwDynPISizeOut;

} tProcessImageDscrpt;


//---------------------------------------------------------------------------
// function prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Function:    DynBuildOd()
//
// Description: Function creates the dynamic OD and resets the PDO modul.
//
// Parameters:  pbSegCont_p           = pointer to segment container
//              pProcessImageDscrpt_p = pointer to process image descriptor
//
// Returns:     tCopKernel             = error code
//                  kCopSuccessful          = no error
//                  kCopDynNoMemory         = not enogth memory
//                  kCopCobIllegalCanId     = illegal CAN-ID for SYNC or PDO
//                  kCopAllreadyExist       = COB already exists fro SYNC or PDO
//                  kCopNoFreeEntry         = no free entry in COB-Table for SYNC or PDO
//                  kCopObdIndexNotExist    = PDO mapping index does not exist
//                  kCopObdSubindexNotExist = PDO mapping subindex does not exist
//                  kCopPdoErrorMapp        = size of mapped object does not match to real size
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC DynBuildOd (CCM_DECL_INSTANCE_HDL_
    BYTE FAR*            pbSegCont_p,
    tProcessImageDscrpt* pProcessImageDscrpt_p);


//---------------------------------------------------------------------------
// Function:    DynDestroyOd()
//
// Description: Function destroys the dynamic OD and resets the PDO Module.
//
// Parameters:  void
//
// Returns:     tCopKernel          = error code
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC DynDestroyOd (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
// Function:     DynNmtEvent()
//
// Description:  function is called, when an NMT event is occured
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               NmtEvent_p = nmt-event
//
// Returns:      tCopKernel = error code
// ----------------------------------------------------------------------------

tCopKernel PUBLIC DynNmtEvent (CCM_DECL_INSTANCE_HDL_
    tNmtEvent NmtEvent_p);


#endif

// Please keep an empty line at the end of this file.
