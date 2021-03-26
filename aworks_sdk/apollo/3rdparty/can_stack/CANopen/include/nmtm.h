/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  NMT master module (sub module)

  -------------------------------------------------------------------------

                $RCSfile: Nmtm.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.15 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

  r.d.: first implementation

****************************************************************************/

#ifndef _NMTM_H_
#define _NMTM_H_


// ============================================================================
// constants
// ============================================================================

typedef BYTE tNmtmSlaveState;

#define kNmtmStateReset         0x00
#define kNmtmStateUsed          0x01    // slave entry is used
#define kNmtmBootupReceived     0x02    // bootup is received
#define kNmtmStateLgActive      0x04    // life guarding is activated
#define kNmtmLgReceived         0x08
#define kNmtmStateNgActive      0x10    // node guarding is activated
#define kNmtmUpdate             0x40    // node-/life guarding message is updated
#define kNmtmStateToggleBit     0x80    // current toggle bit for node-/life guarding



// ============================================================================
// types
// ============================================================================

typedef struct
{
    BYTE        m_bSlaveState;          // values from tNmtmSlaveState
    BYTE        m_bLostMsgCount;
    BYTE        m_bSlaveNodeState;

} tNmtmSlaveInfo;

typedef struct
{
    BYTE            m_bNodeId;
    tNmtmSlaveParam m_SlaveParam;           // defined parameters from user
    tNmtmSlaveInfo  m_SlaveInfo;            // current informations for stack and user

    // ..................................   // current informations only for stack
    tTime           m_ConfigLifeguardTime;  // configurated lifeguard time (base time is 100 us)
    tTime           m_LastLifeguardTime;    // time of last sending the lifeguarding RTR frame (base time is 100 us)
    tCobParam       m_GuardCob;

} tNmtmSlaveEntry;

typedef tCopKernel (PUBLIC ROM* tNmtmEventCallback) (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p,
    tNmtErrCtrlEvent NmtmEvent_p);

typedef struct
{
    #if (COP_MAX_INSTANCES > 1)

        void MEM*               m_CobInstance;
        void MEM*               m_NmtInstance;
        void MEM*               m_ObdInstance;

    #endif

    tNmtmEventCallback          m_fpNmtmEventCallback;
    tNmtmSlaveEntry     MEM*    m_paNmtmSlaveEntry;
    BYTE                        m_bNmtmMaxSlaveEntries;

} tNmtmInitParam;


// ----------------------------------------------------------------------------
// Function:    NmtmInit()
//
// Description: function clears instance table and initializes the first instance
//              of NMT master module
//
// Parameters:  MCO_DECL_PTR_INSTANCE_PTR_  = (address to instance pointer)
//              pInitParam_p                = pointer to initialising struct
//
// Returns:     tCopKernel                  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtmInit (MCO_DECL_PTR_INSTANCE_PTR_
    tNmtmInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
// Function:    NmtmAddInstance()
//
// Description: function adds a new instance of NMTM module
//
// Parameters:  MCO_DECL_PTR_INSTANCE_PTR_  = (address to instance pointer)
//              pInitParam_p                = pointer to initialising struct
//
// Returns:     tCopKernel                  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtmAddInstance (MCO_DECL_PTR_INSTANCE_PTR_
    tNmtmInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
// Function:    NmtmDeleteInstance()
//
// Description: deletes all used COB objects and signes the instance as unused
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtmDeleteInstance (MCO_DECL_INSTANCE_PTR);


// ----------------------------------------------------------------------------
// Function:    NmtmSetEventCallback
//
// Description: function sets the address to callback function for NMT master events
//
// Parameters:  MCO_DECL_INSTANCE_HDL_  = (handle of instance)
//              fpNmtmEventCallback_p   = address to callback function
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtmSetEventCallback (MCO_DECL_INSTANCE_PTR_
    tNmtmEventCallback fpNmtmEventCallback_p);


// ----------------------------------------------------------------------------
// Function:    NmtmAddSlaveNode()
//
// Description: adds a new slave node to watching lifeguarding
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//              bNodeId_p               = NodeId of slave node
//                                        (if 0x00 then NodeId will be taken from slave param)
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtmAddSlaveNode (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p);


// ----------------------------------------------------------------------------
// Function:    NmtmDeleteSlaveNode()
//
// Description: deletes a slave node from slave table
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//              bNodeId_p               = NodeId of slave which has to be deleted
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtmDeleteSlaveNode (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p);


// ----------------------------------------------------------------------------
// Function:    NmtmConfigLgm()
//
// Description: configures a slave node entry and starts lifeguarding as master
//              If connection was lost this function can be used to reactivate
//              lifeguarding for this slave node.
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//              bNodeId_p               = NodeId of slave node
//              pSlaveParam_p           = pointer to parameters for watching slave
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtmConfigLgm (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p, tNmtmSlaveParam GENERIC* pSlaveParam_p);


// ----------------------------------------------------------------------------
// Function:    NmtmTriggerNodeGuard()
//
// Description: sends a RTR frame to the specified slave node to get the current
//              node state. function does not return the node state. slave node
//              answers the RTR frame after any time. if the master receives the
//              answer, then callbackfunction will be called.
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//              bNodeId_p               = NodeId of slave
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtmTriggerNodeGuard (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p);


// ----------------------------------------------------------------------------
// Function:    NmtmGetSlaveInfo()
//
// Description: gets slave information to application
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//              bNodeId_p               = NodeId of slave
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtmGetSlaveInfo (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p, tNmtmSlaveInfo MEM* pSlaveInfo_p);


// ----------------------------------------------------------------------------
// Function:    NmtmSendCommand()
//
// Description: send a NMT command via CAN message and/or executes this NMT
//              command for the own node
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//              bNodeId_p               = NodeId of CANopen node which state
//                                        has to be changed
//              NmtCommand_p            = NMT command to send or execute
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtmSendCommand (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p, tNmtCommand NmtCommand_p);


// ----------------------------------------------------------------------------
// Function:    NmtmProcess()
//
// Description: runs the NMT master process for lifeguarding
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//              pNextCallTime_p         = address to time variable for next calling NmtmProcess()
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtmProcess (MCO_DECL_INSTANCE_PTR_
    tTime MEM* pNextCallTime_p);


#endif // _NMTM_H_

// Please keep an empty line at the end of this file.

