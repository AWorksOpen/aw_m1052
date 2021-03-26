/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen Manager

  Description:  Header file for auto configuration of heartbeat / lifeguarding

  -------------------------------------------------------------------------

                $RCSfile: CmAutoHb.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.1 $  $Date: 2006/05/05 10:08:54 $

                $State: Exp $

                Build Environment:
                    all

  -------------------------------------------------------------------------

  Revision History:

  2006/02/08 d.k.:  first implementation

****************************************************************************/

#ifndef _CMAUTOHB_H_
#define _CMAUTOHB_H_

//---------------------------------------------------------------------------
// global function prototypes
//---------------------------------------------------------------------------


// ----------------------------------------------------------------------------
//
// Function:    CmAutoHbProcess()
//
// Description: configures heartbeat / lifeguarding automatically
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CmAutoHbProcess (CCM_DECL_INSTANCE_HDL);

// ----------------------------------------------------------------------------
//
// Function:    CmAutoHbReset()
//
// Description: resets all internal variables
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CmAutoHbReset (CCM_DECL_INSTANCE_HDL_
                                 tCmAutoHbCbEvent fpCmAutoHbCbEvent_p);

// ----------------------------------------------------------------------------
//
// Function:    CmAutoHbConfigureNode()
//
// Description: configure automatically heartbeat / lifeguarding for the specified node
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = node id
//              wClientIndex_p          = preconfigured SDO client index
//              fOnlyHbc_p              = configure only heartbeat consumer on local node
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CmAutoHbConfigureNode (CCM_DECL_INSTANCE_HDL_
                                         BYTE bNodeId_p,
                                         WORD wClientIndex_p,
                                         BYTE fOnlyHbc_p);

// ----------------------------------------------------------------------------
//
// Function:    CmAutoHbRequestGuarding()
//
// Description: sets up or cancels the lifeguarding on the specified node
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = node id
//              wClientIndex_p          = preconfigured SDO client index
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

tCopKernel PUBLIC CmAutoHbRequestGuarding (CCM_DECL_INSTANCE_HDL_
                                         BYTE bNodeId_p,
                                         WORD wClientIndex_p,
                                         BYTE fSetupCancel_p);

// ----------------------------------------------------------------------------
//
// Function:    CmAutoHbLgConfigured()
//
// Description: returns if lifeguarding was configured automatically for the specified node
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              bNodeId_p               = node id
//
// Returns:     BYTE                    = TRUE / FALSE
//
// ----------------------------------------------------------------------------

BYTE PUBLIC CmAutoHbLgConfigured (CCM_DECL_INSTANCE_HDL_
                                                     BYTE bNodeId_p);

#endif // _CMAUTOHB_H_
