/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  interface if LSS master module

  -------------------------------------------------------------------------

                $RCSfile: Lssm.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.5 $
                
                $State: Exp $
                
                Build Environment:
                    all

  -------------------------------------------------------------------------

  Revision History:

    2005/07/05 r.d.:    start of implementation

****************************************************************************/

#ifndef _LSSM_H_
#define _LSSM_H_


//---------------------------------------------------------------------------
// constants
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// types
//---------------------------------------------------------------------------

typedef tCopKernel (PUBLIC ROM* tLssmEventCallback) (CCM_DECL_INSTANCE_HDL_
    tLssmEvent LssmEvent_p, void MEM* m_pArg);

// init parameters
typedef struct
{
    #if (COP_MAX_INSTANCES > 1)

        void MEM*       m_CobInstance;

    #endif

    tTime               m_ConfirmTimeout;
    tTime               m_ProcessDelay;
    tLssmEventCallback  m_fpLssEventCallback;

} tLssmInitParam;


//---------------------------------------------------------------------------
// PUBLIC functions
//---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//
// Function:    LssmInit()
//
// Description: function clears instance table and initializes the first instance
//              of LSS slave slave module
//
// Parameters:  MCO_DECL_PTR_INSTANCE_PTR_  = (address to instance pointer)
//              pInitParam_p                = pointer to initialising struct
//
// Returns:     tCopKernel                  = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmInit (MCO_DECL_PTR_INSTANCE_PTR_
    tLssmInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
//
// Function:    LssmAddInstance()
//
// Description: function adds a new instance of LSS slave module
//
// Parameters:  MCO_DECL_PTR_INSTANCE_PTR_  = (address to instance pointer)
//              pInitParam_p                = pointer to initialising struct
//
// Returns:     tCopKernel                  = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmAddInstance (MCO_DECL_PTR_INSTANCE_PTR_
    tLssmInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
//
// Function:    LssmDeleteInstance()
//
// Description: deletes all used COB objects and signes the instance as unused
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmDeleteInstance (MCO_DECL_INSTANCE_PTR);


// ----------------------------------------------------------------------------
//
// Function:    LssmProcess()
//
// Description: function has to be called cyclic in process of whole CANopen.
//              It is important for changing baudrate with configurated switch
//              delay.
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              pNextCallTime_p         = address to time variable for next calling LssmProcess()
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmProcess (MCO_DECL_INSTANCE_PTR_
    tTime MEM* pNextCallTime_p);


//---------------------------------------------------------------------------
//
// Function:    LssmSwitchModeGlobal()
//
// Description: function switches all LSS slaves into LSS mode configuration or
//              operation
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              LssMode_p               = new LSS mode (see defines kLssMode...)
//
// Returns:     tCopKernel              = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmSwitchModeGlobal (MCO_DECL_INSTANCE_PTR_
    tLssMode LssMode_p);


//---------------------------------------------------------------------------
//
// Function:    LssmSwitchModeSelective()
//
// Description: function switches a LSS slaves which matches to the LSS address
//              into LSS mode configuration
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              pLssAddress_p           = pointer to LSS address
//
// Returns:     tCopKernel              = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmSwitchModeSelective (MCO_DECL_INSTANCE_PTR_
    tLssAddress GENERIC* pLssAddress_p);


//---------------------------------------------------------------------------
//
// Function:    LssmConfigureNodeId()
//
// Description: function configures the node-ID of a LSS slave
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              bNodeId_p               = new node-ID of the slave
//
// Returns:     tCopKernel              = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmConfigureNodeId (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p);


//---------------------------------------------------------------------------
//
// Function:    LssmConfigureBitTiming()
//
// Description: function configures the baudrate of all LSS slaves (and it self)
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              bTableSelector_p        = table selector of new baudrate
//                                        (0 = defined by CiA DS-305)
//              bTableIndex_p           = tabe index of new baudrate
//
// Returns:     tCopKernel              = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmConfigureBitTiming (MCO_DECL_INSTANCE_PTR_
    tLssmBitTiming GENERIC* pBitTiming_p);


//---------------------------------------------------------------------------
//
// Function:    LssmActivateBitTiming()
//
// Description: function activates the new baudrate of all LSS slaves and itself
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              wSwitchDelay_p          = delay time in ms for waiting befor switching new baudrate
//
// Returns:     tCopKernel              = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmActivateBitTiming (MCO_DECL_INSTANCE_PTR_
    WORD wSwitchDelay_p);


//---------------------------------------------------------------------------
//
// Function:    LssmStoreConfiguration()
//
// Description: function lets store configuration of LSS slaves
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//
// Returns:     tCopKernel              = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmStoreConfiguration (MCO_DECL_INSTANCE_PTR);


//---------------------------------------------------------------------------
//
// Function:    LssmInquireIdentity()
//
// Description: function inquires to identify LSS slave
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              bServiceCmd_p           = command of services which should be sent
//
// Returns:     tCopKernel              = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmInquireIdentity (MCO_DECL_INSTANCE_PTR_
    BYTE bServiceCmd_p);


//---------------------------------------------------------------------------
//
// Function:    LssmIdentifyRemoteSlave()
//
// Description: function lets identify LSS slave which matches to identification parameters
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              pLssIdParam_p           = pointer to structure with identification parameters
//
// Returns:     tCopKernel              = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmIdentifyRemoteSlave (MCO_DECL_INSTANCE_PTR_
    tLssmIdentifyParam GENERIC* pLssIdParam_p);


//---------------------------------------------------------------------------
//
// Function:    LssmIdentifyNonCfgSlave()
//
// Description: function lets identify LSS slave which is not configured with a valid Node-ID
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//
// Returns:     tCopKernel              = error code
//
//---------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LssmIdentifyNonCfgSlave (MCO_DECL_INSTANCE_PTR);


#endif // _LSSM_H_

// Please keep an empty line at the end of this file.

