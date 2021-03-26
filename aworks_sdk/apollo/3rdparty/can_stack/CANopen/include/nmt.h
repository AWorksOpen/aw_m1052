/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  header of NMT main modul

  -------------------------------------------------------------------------

                $RCSfile: Nmt.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.17 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

  r.d.: first implementation

****************************************************************************/

#ifndef _NMT_H_
#define _NMT_H_


// ============================================================================
// enums
// ============================================================================

typedef enum
{
    kNmtErrNo           = 0x00,
    kNmtErrBootup       = 0x01,
    kNmtErrNodeGuard    = 0x02,
    kNmtErrLifeGuard    = 0x03,
    kNmtErrHeartbeat    = 0x04

} tNmtErrProtocoll;

// These constant values are used for function NmtResetErrorControlCob()
// to register callback-function of COB object of NMT master and Heartbeat
// Consumer which have the same CAN-IDs for same nodes to control.
typedef enum
{
    kNmtCbNmtm          = 0x00,     // callback function for NMT master module
    kNmtCbHbc           = 0x01      // callback function for Heartbeat Consumer module

} tNmtCbModule;


// ============================================================================
// types
// ============================================================================

typedef tCopKernel (PUBLIC ROM* tNmtEventCallback) (CCM_DECL_INSTANCE_HDL_
    tNmtEvent NmtEvent_p);

typedef tCopKernel (PUBLIC ROM* tNmtResetNodeGuard) (CCM_DECL_INSTANCE_HDL);

typedef BYTE (PUBLIC ROM* tNmtDoDeleteCob) (CCM_DECL_INSTANCE_HDL_
    tCobParam MEM* pCobParam_p);

typedef struct
{
    #if (COP_MAX_INSTANCES > 1)

        void MEM*       m_CobInstance;
        void MEM*       m_ObdInstance;

    #endif

    BYTE                m_bNodeId;
    tNmtEventCallback   m_fpNmtEventCallback;
    tCcmErrorCallback   m_fpErrorCallback;

} tNmtInitParam;

typedef struct
{
    BYTE    m_bNmtState;
    BYTE    m_bNmtEvent;
    BYTE    m_bAdditionalRet;

} tNmtStateError;


// ============================================================================
// function prototypes
// ============================================================================

COPDLLEXPORT tCopKernel PUBLIC NmtInit              (MCO_DECL_PTR_INSTANCE_PTR_ tNmtInitParam MEM* pInitParam_p);
COPDLLEXPORT tCopKernel PUBLIC NmtAddInstance       (MCO_DECL_PTR_INSTANCE_PTR_ tNmtInitParam MEM* pInitParam_p);
COPDLLEXPORT tCopKernel PUBLIC NmtDeleteInstance    (MCO_DECL_INSTANCE_PTR);
COPDLLEXPORT tCopKernel PUBLIC NmtExecCommand       (MCO_DECL_INSTANCE_PTR_     tNmtCommand NmtCommand_p);
COPDLLEXPORT tCopKernel PUBLIC NmtUpdateErrCtrlData (MCO_DECL_INSTANCE_PTR_     BYTE bToggleFlag_p);
COPDLLEXPORT tCopKernel PUBLIC NmtTriggerBootup     (MCO_DECL_INSTANCE_PTR);

// ----------------------------------------------------------------------------
// Function:    NmtSetResetNodeGuardCallback()
//
// Description: sets new callback function to reset nodegauarding afer
//              switching of heartbeat
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance entry)
//              fpCallback_p            = new callback function
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtSetResetNodeGuardCallback (MCO_DECL_INSTANCE_PTR_
    tNmtResetNodeGuard  fpCallback_p);


COPDLLEXPORT tNmtErrProtocoll PUBLIC NmtGetActiveErrorProtocoll (MCO_DECL_INSTANCE_PTR);


// ----------------------------------------------------------------------------
// Function:    NmtSetActiveErrorProtocoll()
//
// Description: sets new error controlling protocoll and redefines its COB object
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance entry)
//              NewErrProtocoll_p       = new error controlling protocoll
//
// Returns:     tCopKernel              = error code from CobDefine()
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtSetActiveErrorProtocoll (MCO_DECL_INSTANCE_PTR_
    tNmtErrProtocoll NewErrProtocoll_p);


// ----------------------------------------------------------------------------
// Function:    NmtSetErrorProtocollCallback()
//
// Description: sets new callback function for error controlling protocoll
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance entry)
//              fpCobCallback_p         = callback function for COB object (can be NULL)
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtSetErrorProtocollCallback (MCO_DECL_INSTANCE_PTR_
    tCobCallback     fpCobCallback_p);


// ----------------------------------------------------------------------------
//
// Function:    NmtResetErrorControlCob()
//
// Description: sets new callback function for error controlling protocoll
//              for NMT master and Hertbeat Consumer and defines COB object.
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance entry)
//              CbModule_p              = defines which module wishes to set
//                                        callback funtion
//              pCobParam_p             = pointer to COB parameters
//                                        (NULL means: module is switched off)
//              fDeleteCob_p            = should COB object be deleted? (TRUE/FALSE)
//              fpCbDoDelete_p          = callback function which asks for deleting
//                                        COB object in correspond other module (HBC or NMTM)
//
// Returns:     tCopKernel = error code of CobDefine()
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtResetErrorControlCob (MCO_DECL_INSTANCE_PTR_
    tNmtCbModule CbModule_p, tCobParam MEM* pCobParam_p,
    BYTE fDeleteCob_p, tNmtDoDeleteCob fpCbDoDelete_p);


#endif // _NMT_H_

// Please keep an empty line at the end of this file.

