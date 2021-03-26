/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen

  Description:  header of NMT slave module (sub module)

  -------------------------------------------------------------------------

                $RCSfile: Nmts.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.17 $  $Date: 2006/12/20 15:08:52 $

                $State: Exp $

                Build Environment:
                    X

  -------------------------------------------------------------------------

  Revision History:

    03-jan-2005 r.d.:   - in tNmtsInitParam callback function for NMT command
                          included - so application is able to ignore a NMT
                          command

****************************************************************************/

#ifndef _NMTS_H_
#define _NMTS_H_


//---------------------------------------------------------------------------
// const defines
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// types
//---------------------------------------------------------------------------

typedef tCopKernel (PUBLIC ROM* tNmtsLgCallback) (CCM_DECL_INSTANCE_HDL_
    tNmtErrCtrlEvent LgEvent_p);

typedef tCopKernel (PUBLIC ROM* tNmtsCmdCallback) (CCM_DECL_INSTANCE_HDL_
    tNmtCommand NmtCommand_p);

typedef struct
{

    #if (COP_MAX_INSTANCES > 1)

        void MEM*       m_CobInstance;
        void MEM*       m_ObdInstance;
        void MEM*       m_NmtInstance;

    #endif

    tNmtsLgCallback     m_fpNmtsLgCallback;
    tNmtsCmdCallback    m_fpNmtsCmdCallback;

} tNmtsInitParam;


//---------------------------------------------------------------------------
// function prototypes
//---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function:     NmtsInit()
//
// Description:  function clears instance table and initializes the first instance
//               of NMT slave module
//
// Parameters:   MCO_DECL_PTR_INSTANCE_PTR_ = (address to instance pointer)
//               pInitParam_p               = pointer to initialising struct
//
// Returns:      tCopKernel                 = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtsInit (MCO_DECL_PTR_INSTANCE_PTR_
    tNmtsInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
// Function:     NmtsAddInstance()
//
// Description:  function adds a new instance of NMTS module
//
// Parameters:   MCO_DECL_PTR_INSTANCE_PTR_ = (address to instance pointer)
//               pInitParam_p               = pointer to initialising struct
//
// Returns:      tCopKernel                 = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtsAddInstance (MCO_DECL_PTR_INSTANCE_PTR_
    tNmtsInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
// Function:    NmtsDeleteInstance()
//
// Description: deletes all used COB objects and signes the instance as unused
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtsDeleteInstance (MCO_DECL_INSTANCE_PTR);


// ----------------------------------------------------------------------------
// Function:     NmtsSendBootup()
//
// Description:  function sends bootup message and sets node/life guarding as
//               active error controlling protocoll
//
// Parameters:   MCO_DECL_INSTANCE_PTR  = (pointer to instance)
//
// Returns:      tCopKernel             = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtsSendBootup (MCO_DECL_INSTANCE_PTR);


// ----------------------------------------------------------------------------
// Function:    NmtsProcess()
//
// Description: function checks life guarding
//              If this node don't receive RTR request from NMT master in
//              configured time, it has to genrate an event for application.
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              pNextCallTime_p         = address to time variable for next calling NmtsProcess()
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtsProcess (MCO_DECL_INSTANCE_PTR_
    tTime MEM* pNextCallTime_p);


// ----------------------------------------------------------------------------
// Function:     NmtsNmtEvent()
//
// Description:  function is called from CCM module if an NMT event did occur
//               This is important to reset node/life guarding after receiving
//               NMT command "Reset Communication".
//               This function additionally has to set the new node state inside
//               the CAN message object (COB) of error controlling protocoll.
//
// Parameters:   MCO_DECL_INSTANCE_PTR_ = (pointer to instance)
//               NmtEvent_p             = NMT event
//
// Returns:      tCopKernel             = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtsNmtEvent (MCO_DECL_INSTANCE_PTR_
      tNmtEvent NmtEvent_p);


// ----------------------------------------------------------------------------
// Function:     NmtsSetLgCallback()
//
// Description:  function sets new address of callback function for life guarding
//               events
//
// Parameters:   MCO_DECL_INSTANCE_PTR_ = (pointer to instance)
//               fpLgsEventCallback_p   = address to callback function
//
// Returns:      tCopKernel             = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC NmtsSetLgCallback (MCO_DECL_INSTANCE_PTR_
    tNmtsLgCallback fpLgsEventCallback_p);


// ----------------------------------------------------------------------------
// Function:     NmtsCbModuleControll()
//
// Description:  function will be called from COB module, when NMT message has
//               been received
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               pArg_p                 = additional argument (defined by CobDefine())
//               Event_p                = which event is happened
//
// Returns:      tCopKernel             = error code
// ----------------------------------------------------------------------------

#if (NMTS_USE_LIFEGUARDING != FALSE)

    // it is not allowed to call this function from application
    COPDLLEXPORT tCopKernel PUBLIC NmtsCbConfigLifeGuarding (CCM_DECL_INSTANCE_HDL_
        tObdCbParam MEM* pParam_p);

#else

    // this prevents linker error when NMTS_USE_LIFEGUARDING==FALSE but object 0x100C/0x100D is defined
    #define NmtsCbConfigLifeGuarding        NULL

#endif

#endif // _NMTS_H_

// Please keep an empty line at the end of this file.

