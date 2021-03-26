/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  interface if LSS slave module

  -------------------------------------------------------------------------

                $RCSfile: Lsss.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.11 $
                
                $State: Exp $
                
                Build Environment:
                    all

  -------------------------------------------------------------------------

  Revision History:


****************************************************************************/

#ifndef _LSSS_H_
#define _LSSS_H_


//---------------------------------------------------------------------------
// constants
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// types
//---------------------------------------------------------------------------

typedef tCopKernel (PUBLIC ROM* tLssEventCallback) (CCM_DECL_INSTANCE_HDL_
    tLssCbParam MEM* pLssCbParam_p);

// init parameters
typedef struct
{
    #if (COP_MAX_INSTANCES > 1)

        void MEM*       m_CobInstance;
        void MEM*       m_ObdInstance;

    #endif

    BYTE                m_bNodeId;
    BYTE                m_bBtrTableSelector;
    BYTE                m_bBtrIndex;

    tLssEventCallback   m_fpLssEventCallback;

} tLsssInitParam;


//---------------------------------------------------------------------------
// PUBLIC functions
//---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function:    LsssInit()
//
// Description: function clears instance table and initializes the first instance
//              of LSS slave slave module
//
// Parameters:  MCO_DECL_PTR_INSTANCE_PTR_  = (address to instance pointer)
//              pInitParam_p                = pointer to initialising struct
//
// Returns:     tCopKernel                  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LsssInit (MCO_DECL_PTR_INSTANCE_PTR_
    tLsssInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
// Function:    LsssAddInstance()
//
// Description: function adds a new instance of LSS slave module
//
// Parameters:  MCO_DECL_PTR_INSTANCE_PTR_  = (address to instance pointer)
//              pInitParam_p                = pointer to initialising struct
//
// Returns:     tCopKernel                  = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LsssAddInstance (MCO_DECL_PTR_INSTANCE_PTR_
    tLsssInitParam MEM* pInitParam_p);


// ----------------------------------------------------------------------------
// Function:    LsssDeleteInstance()
//
// Description: deletes all used COB objects and signes the instance as unused
//
// Parameters:  MCO_DECL_INSTANCE_PTR   = (pointer to instance)
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LsssDeleteInstance (MCO_DECL_INSTANCE_PTR);


// ----------------------------------------------------------------------------
// Function:    LsssProcess()
//
// Description: function has to be called cyclic in process of whole CANopen.
//              It is important for changing baudrate with configurated switch
//              delay.
//
// Parameters:  MCO_DECL_INSTANCE_PTR_  = (pointer to instance)
//              pNextCallTime_p         = address to time variable for next calling LsssProcess()
//
// Returns:     tCopKernel              = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC LsssProcess (MCO_DECL_INSTANCE_PTR_
    tTime MEM* pNextCallTime_p);


#endif // _LSSS_H_

// Please keep an empty line at the end of this file.

