/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  header for Heartbeat Producer module (sub module)

  -------------------------------------------------------------------------

                $RCSfile: hbp.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.9 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

  m.b.: first implementation

****************************************************************************/

#ifndef _HBP_H_
#define _HBP_H_


// ============================================================================
// types
// ============================================================================

typedef struct
{

    #if (COP_MAX_INSTANCES > 1)

        void MEM*       m_CobInstance;
        void MEM*       m_ObdInstance;
        void MEM*       m_NmtInstance;

    #endif

    BYTE                m_bNodeId;      // dummy variable

} tHbpInitParam;


// ============================================================================
// public functions
// ============================================================================

// ----------------------------------------------------------------------------
// Function:     HbpInit()
//
// Description:  function clears instance table and initializes the first instance
//               of heartbeat producer module
//
// Parameters:   MCO_DECL_PTR_INSTANCE_PTR_ = (address to instance pointer)
//               pInitParam_p               = pointer to initialising struct
//
// Returns:      tCopKernel                 = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbpInit (MCO_DECL_PTR_INSTANCE_PTR_  
    tHbpInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
// Function:     HbpAddInstance()
//
// Description:  function adds a new instance of heartbeat producer module
//
// Parameters:   MCO_DECL_PTR_INSTANCE_PTR_ = (address to instance pointer)
//               pInitParam_p               = pointer to initialising struct
//
// Returns:      tCopKernel                 = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbpAddInstance (MCO_DECL_PTR_INSTANCE_PTR_  
    tHbpInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
// Function:    HbpDeleteInstance()
//
// Description: signes the instance as unused
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbpDeleteInstance (MCO_DECL_INSTANCE_PTR);


// ----------------------------------------------------------------------------
// Function:    HbpProcess()
//
// Description: function checks heartbeat producer time
//              heartbeat message will be sent if time is over
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              pNextCallTime_p         = address to time variable for next calling HbpProcess()
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbpProcess (MCO_DECL_INSTANCE_PTR_
    tTime MEM* pNextCallTime_p);


// ----------------------------------------------------------------------------
// Function:     HbpNmtEvent()
//
// Description:  function is called from CCM module if an NMT event did occur
//               This is important to reset heartbeat after receiving
//               NMT command "Reset Communication".
//
// Parameters:   MCO_DECL_INSTANCE_PTR_ = (pointer to instance)
//               NmtEvent_p             = NMT event
//
// Returns:      tCopKernel             = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbpNmtEvent (MCO_DECL_INSTANCE_PTR_
    tNmtEvent NmtEvent_p);


// ==============================================================================
// ==============================================================================
// it is not allowed to call this function from application

// ----------------------------------------------------------------------------
// Function:     HbpCbConfigHeartbeat()
//
// Description:  function will be called from OBD module, when object 0x1017 is
//               been written
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               pParam_p               = object parameters
//
// Returns:      tCopKernel             = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC HbpCbConfigHeartbeat (CCM_DECL_INSTANCE_HDL_  
    tObdCbParam MEM* pParam_p);

// ==============================================================================
// ==============================================================================


#endif // _HBP_H_

// Please keep an empty line at the end of this file.

