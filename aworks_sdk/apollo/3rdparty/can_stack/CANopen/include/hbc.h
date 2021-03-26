/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  header for Heartbeat Consumer module (sub module)

  -------------------------------------------------------------------------

                $RCSfile: hbc.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.15 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

  m.b.: first implementation

****************************************************************************/

#ifndef _HBC_H_
#define _HBC_H_


// ============================================================================
// types
// ============================================================================

typedef struct
{
    BYTE            m_bFirstMsgFlag;
    BYTE            m_bNodeId;
    tTime           m_LastRecvTime;     // time base is 100us
    tTime           m_CfgConsHbTime;    // time base is 100us
    tCobParam       m_CobParam;         // COB parameter for HBC
    BYTE            m_bNodeState;       // current NodeState of heartbeat producer

    // pointer to object data of object 0x1016/X
    DWORD GENERIC*  m_pdwConsHbTime;    // base time is 1ms

} tHbcInfo;

// Callbackfunction to info of heartbeat-event
typedef tCopKernel (PUBLIC ROM* tHbcEventCallback) (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p,
    tNmtErrCtrlEvent HbcEvent_p);

typedef struct
{
    #if (COP_MAX_INSTANCES > 1)

        void MEM*               m_CobInstance;
        void MEM*               m_ObdInstance;
        void MEM*               m_NmtInstance;

    #endif

    tHbcEventCallback           m_fpHbcEventCallback;
    tHbcInfo            MEM*    m_paHbcProducerTable;
    BYTE                        m_bHbcMaxProducerEntries;

} tHbcInitParam;


// ============================================================================
// public functions
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:    HbcInit()
//
// Description: function clears instance table and initializes the first instance
//              of heartbeat consumer module
//
// Parameters:  MCO_DECL_PTR_INSTANCE_PTR_  = (address to instance pointer)
//              pInitParam_p                = pointer to initialising struct
//
// Returns:     tCopKernel                  = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbcInit (MCO_DECL_PTR_INSTANCE_PTR_
    tHbcInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
//
// Function:    HbcAddInstance()
//
// Description: function adds a new instance of heartbeat consumer module
//
// Parameters:  MCO_DECL_PTR_INSTANCE_PTR_  = (address to instance pointer)
//              pInitParam_p                = pointer to initialising struct
//
// Returns:     tCopKernel                  = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbcAddInstance (MCO_DECL_PTR_INSTANCE_PTR_
    tHbcInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
//
// Function:    HbcDeleteInstance()
//
// Description: signes the instance as unused
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbcDeleteInstance (MCO_DECL_INSTANCE_PTR);


// ----------------------------------------------------------------------------
//
// Function:    HbcProcess()
//
// Description: function checks heartbeat consumer
//              It generates a heartbeat-event, when a heartbeat-error
//              occours
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              pNextCallTime_p         = relative time for next calling of this function [0.1ms]
//                                        
//              The use of parameter pNextCallTime_p can be defined by setting the 
//              HBC_PROCESS_TIME_CONTROL to TRUE in copcfg.h
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbcProcess (MCO_DECL_INSTANCE_PTR_
    tTime MEM* pNextCallTime_p);


// ----------------------------------------------------------------------------
//
// Function:    HbcNmtEvent()
//
// Description: function is called from CCM module if an NMT event did occur
//              This is important to reset heartbeat after receiving
//              NMT command "Reset Communication".
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              NmtEvent_p              = NMT event
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbcNmtEvent (MCO_DECL_INSTANCE_PTR_
    tNmtEvent NmtEvent_p);


// ----------------------------------------------------------------------------
//
// Function:    HbcSetEventCallback
//
// Description: function is called to set the pointer to CB-Function into Inst-tab
//
// Parameters:  MCO_DECL_INSTANCE_HDL_  = (handle of instance)
//              fpHbcEventCallback_p    = pointer to callback-funtion
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbcSetEventCallback (MCO_DECL_INSTANCE_PTR_
    tHbcEventCallback fpHbcEventCallback_p);


// ----------------------------------------------------------------------------
//
// Function:    HbcGetNodeState()
//
// Description: function gets node state of Heartbeat Producer which is to control
//
// Parameters:  MCO_DECL_INSTANCE_HDL_  = (handle of instance)
//              bNodeId_p               = NodeId of Heartbeat Producer
//              pNodeState_p            = pointer to receive NodeState
//
// Returns:     tCopKernel = error code
//                  kCopSuccessful
//                  kCopHbcEntryNotExist
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbcGetNodeState (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p, tNodeState MEM* pNodeState_p);


// ----------------------------------------------------------------------------
//
// Function:    HbcGetHeartbeatTime()
//
// Description: function gets configured heartbeat time from OD
//
// Parameters:  MCO_DECL_INSTANCE_HDL_  = (handle of instance)
//              bNodeId_p               = NodeId of Heartbeat Producer
//              pwHbTime_p              = pointer to receive heartbeat time
//
// Returns:     tCopKernel = error code
//                  kCopSuccessful
//                  kCopHbcEntryNotExist
//
// ----------------------------------------------------------------------------

#if (HBC_USE_ADDITIONAL_API != FALSE)
COPDLLEXPORT tCopKernel PUBLIC HbcGetHeartbeatTime (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p, WORD MEM* pwHbTime_p);
#endif


//---------------------------------------------------------------------------
//
// Function:    HbcGetNextFreeConsumer()
//
// Description: returns the next free heartbeat consumer entry from OD.
//
// Parameters:  pbHbcSubindex_p = [INOUT]   pointer to variable of subindex entry
//                                          (input:  start subindex)
//                                          (output: subindex which is found for free)
//
// Return:      tCopKernel  = error code
//
//---------------------------------------------------------------------------

#if (HBC_USE_ADDITIONAL_API != FALSE)
COPDLLEXPORT tCopKernel PUBLIC HbcGetNextFreeConsumer (MCO_DECL_INSTANCE_PTR_
    BYTE MEM* pbHbcSubindex_p);
#endif


// ==============================================================================
// it is not allowed to call this function from application
// ==============================================================================

// ----------------------------------------------------------------------------
//
// Function:    HbcCbConfigHeartbeat()
//
// Description: function will be called from OBD module, when object 0x1016 is
//              been written
//
// Parameters:  CCM_DECL_INSTANCE_HDL_  = (instance handle)
//              pParam_p                = object parameters
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbcCbConfigHeartbeat (CCM_DECL_INSTANCE_HDL_
    tObdCbParam MEM* pParam_p);


// ==============================================================================

#endif // _HBC_H_

// Please keep an empty line at the end of this file.
