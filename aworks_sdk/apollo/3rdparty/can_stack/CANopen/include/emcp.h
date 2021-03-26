/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  header for Emergency Producer module

  -------------------------------------------------------------------------

                $RCSfile: emcp.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.12 $  $Date: 2006/12/20 15:05:41 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

  m.b.: first implementation

  2005/05/03 r.d.:  - Definitions for Callback-Function of Emergency Producer

****************************************************************************/

#ifndef _EMCP_H_
#define _EMCP_H_


// ============================================================================
// const defines
// ============================================================================

#define EMCP_EVENT_ERROR_LOG        1
#define EMCP_EVENT_ERROR_DELETEALL  2


// ============================================================================
// types
// ============================================================================


typedef void (PUBLIC ROM* tEmcpCallback) (CCM_DECL_INSTANCE_HDL_
    BYTE bEmcpEvent_p, DWORD MEM* MEM* papdwErrorField_p, BYTE bErrorCount_p);

typedef struct
{

    #if (COP_MAX_INSTANCES > 1)

        void MEM*       m_CobInstance;
        void MEM*       m_ObdInstance;

    #endif

    #if (EMCP_USE_PREDEF_ERROR_FIELD != FALSE)

        DWORD MEM* MEM* m_papdwPreDefErrorField;
        BYTE            m_bEmcpMaxPreDefErrorFieldEntries;

    #endif // (EMCP_USE_PREDEF_ERROR_FIELD != FALSE)

    tEmcpCallback       m_fpErrorCallback;

} tEmcpInitParam;


// ============================================================================
// public functions
// ============================================================================

// ----------------------------------------------------------------------------
//
// Function:     EmcpInit()
//
// Description:  function clears instance table and initializes the first instance
//               of emergency producer module
//
// Parameters:   MCO_DECL_PTR_INSTANCE_PTR_ = (address to instance pointer)
//               pInitParam_p               = pointer to initialising struct
//
// Returns:      tCopKernel                 = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC EmcpInit (MCO_DECL_PTR_INSTANCE_PTR_
    tEmcpInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
//
// Function:     EmcpAddInstance()
//
// Description:  function adds a new instance of emergency producer module
//
// Parameters:   MCO_DECL_PTR_INSTANCE_PTR_ = (address to instance pointer)
//               pInitParam_p               = pointer to initialising struct
//
// Returns:      tCopKernel                 = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC EmcpAddInstance (MCO_DECL_PTR_INSTANCE_PTR_
    tEmcpInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
//
// Function:    EmcpDeleteInstance()
//
// Description: signes the instance as unused
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//
// Returns:     tCopKernel              = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC EmcpDeleteInstance (MCO_DECL_INSTANCE_PTR);


// ----------------------------------------------------------------------------
//
// Function:     EmcpNmtEvent()
//
// Description:  function is called from CCM module if an NMT event did occur
//
// Parameters:   MCO_DECL_INSTANCE_PTR_ = (pointer to instance)
//               NmtEvent_p             = NMT event
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC EmcpNmtEvent (MCO_DECL_INSTANCE_PTR_
    tNmtEvent NmtEvent_p);


// ----------------------------------------------------------------------------
//
// Function:     EmcpSendEmergency()
//
// Description:  function sends a emergency message
//
// Parameters:   MCO_DECL_INSTANCE_PTR_ = (pointer to instance)
//               pEmcpParam_p           = parameters of emergency object
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC EmcpSendEmergency (MCO_DECL_INSTANCE_PTR_
    tEmcParam MEM* pEmcpParam_p);


// ==============================================================================
// ==============================================================================

// ----------------------------------------------------------------------------
//
// Function:     EmcpCbConfigEmergencyId()
//
// Description:  function will be called from OBD module, when object 0x1014 is been
//               written
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               pParam_p               = object parameters
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC EmcpCbConfigEmergencyId (CCM_DECL_INSTANCE_HDL_
    tObdCbParam MEM* pParam_p);


// ----------------------------------------------------------------------------
//
// Function:     EmcpCbPreDefinedErrorField()
//
// Description:  function will be called from OBD module, when object 0x1003 is been
//               written
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = (instance handle)
//               pParam_p               = object parameters
//
// Returns:      tCopKernel             = error code
//
// ----------------------------------------------------------------------------

#if (EMCP_USE_PREDEF_ERROR_FIELD != FALSE)
    COPDLLEXPORT tCopKernel PUBLIC EmcpCbPreDefinedErrorField (CCM_DECL_INSTANCE_HDL_
        tObdCbParam MEM* pParam_p);
#else
    // this prevents linker error when EMCP_USE_PREDEF_ERROR_FIELD==FALSE but object 0x1003 is defined
    #define EmcpCbPreDefinedErrorField      NULL
#endif

// ==============================================================================
// ==============================================================================

#endif // _EMCP_H_

// Please keep an empty line at the end of this file.

