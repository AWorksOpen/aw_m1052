/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  header for Emergency Consumer module

  -------------------------------------------------------------------------

                $RCSfile: emcc.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.7 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

  m.b.: first implementation

****************************************************************************/

#ifndef _EMCC_H_
#define _EMCC_H_


// ============================================================================
// types
// ============================================================================

typedef struct
{
    BYTE            m_bProducerNodeId;
    tCobParam       m_CobParam;        // COB-Parameter for Emcc

} tEmccInfo;

// Callbackfunction to info of emergency-event
typedef tCopKernel (PUBLIC ROM* tEmccEventCallback) (CCM_DECL_INSTANCE_HDL_
    BYTE bNodeId_p,
    tEmcParam MEM* pEmcParam_p);

typedef struct
{
    #if (COP_MAX_INSTANCES > 1)

        void MEM*               m_CobInstance;
        void MEM*               m_ObdInstance;

    #endif

    tEmccEventCallback           m_fpEmccEventCallback;
    tEmccInfo            MEM*    m_paEmccProducerTable;
    BYTE                         m_bEmccMaxProducerEntries;

} tEmccInitParam;


// ============================================================================
// public functions
// ============================================================================

// ----------------------------------------------------------------------------
// Function:     EmccInit()
//
// Description:  function clears instance table and initializes the first instance
//               of emergency consumer module
//
// Parameters:   MCO_DECL_PTR_INSTANCE_PTR_ = (address to instance pointer)
//               pInitParam_p               = pointer to initialising struct
//
// Returns:      tCopKernel                 = error code
// ----------------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC EmccInit (MCO_DECL_PTR_INSTANCE_PTR_  tEmccInitParam MEM* pInitParam_p);

// ----------------------------------------------------------------------------
// Function:     EmccAddInstance()
//
// Description:  function adds a new instance of heartbeat consumer module
//
// Parameters:   MCO_DECL_PTR_INSTANCE_PTR_ = (address to instance pointer)
//               pInitParam_p               = pointer to initialising struct
//
// Returns:      tCopKernel                 = error code
// ----------------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC EmccAddInstance (MCO_DECL_PTR_INSTANCE_PTR_  tEmccInitParam MEM* pInitParam_p);

// ----------------------------------------------------------------------------
// Function:    EmccDeleteInstance()
//
// Description: signes the instance as unused
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC EmccDeleteInstance (MCO_DECL_INSTANCE_PTR);

// ----------------------------------------------------------------------------
// Function:     EmccNmtEvent()
//
// Description:  function is called from CCM module if an NMT event did occur
//
// Parameters:   MCO_DECL_INSTANCE_PTR_ = (pointer to instance)
//               NmtEvent_p             = NMT event
//
// Returns:      tCopKernel             = error code
// ----------------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC EmccNmtEvent (MCO_DECL_INSTANCE_PTR_  tNmtEvent NmtEvent_p);

// ----------------------------------------------------------------------------
// Function:     EmccSetEventCallback
//
// Description:  function is called to set the pointer to CB-Function into Inst-tab
//
// Parameters:   MCO_DECL_INSTANCE_HDL_ = (handle of instance)
//               fpEmccEventCallback_p = pointer to callback-funtion
//
// Returns:      tCopKernel = error code
// ----------------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC EmccSetEventCallback (MCO_DECL_INSTANCE_PTR_
    tEmccEventCallback fpEmccEventCallback_p);

// ----------------------------------------------------------------------------
// Function:    EmccAddProducerNode()
//
// Description: adds a new producer node 
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//              bNodeId_p               = NodeId of producer node
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC EmccAddProducerNode (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p);

// ----------------------------------------------------------------------------
// Function:    EmccDeleteProducerNode()
//
// Description: deletes an existing producer node 
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//              bNodeId_p               = NodeId of producer node
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC EmccDeleteProducerNode (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p);






// ==============================================================================

#endif // _EMCC_H_

// Please keep an empty line at the end of this file.

