/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      ...

  Description:  ...

  -------------------------------------------------------------------------

                $RCSfile: Hpt.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.1 $  $Date: 2006/07/18 07:41:19 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

****************************************************************************/

#ifndef _TGTHPT_H_
#define _TGTHPT_H_


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*          G L O B A L   D E F I N I T I O N S                            */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

//---------------------------------------------------------------------------
// default settings
//---------------------------------------------------------------------------

#ifndef HPT_CB_DIRECT_CALL
    #define HPT_CB_DIRECT_CALL          TRUE
#endif

#ifndef HPT_TIMER_CORRECTION
    #define HPT_TIMER_CORRECTION        3
#endif


//---------------------------------------------------------------------------
// const defines
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// modul global types
//---------------------------------------------------------------------------

typedef tCopKernel (PUBLIC ROM* tHptEventCallback) (CCM_DECL_INSTANCE_HDL);


//---------------------------------------------------------------------------
// local vars
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// local function prototypes
//---------------------------------------------------------------------------



//=========================================================================//
//                                                                         //
//          P U B L I C   F U N C T I O N S                                //
//                                                                         //
//=========================================================================//


//---------------------------------------------------------------------------
//
// Function:    TgtHptInit()
//
// Description: initializes taget specific registers for high presition time stamp
//
// Parameters:  void
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC TgtHptInit (void);


//---------------------------------------------------------------------------
//
// Function:    TgtHptGetCurrentTime()
//
// Description: returns the current gigh presition time
//
// Parameters:  void
//
// Return:      DWORD
//
//---------------------------------------------------------------------------

DWORD TgtHptGetCurrentTime (void);


//---------------------------------------------------------------------------
//
// Function:    TgtHptSaveTime()
//
// Description: savee the cuurent time stamp and having regard to the timer correction
//
// Parameters:  void
//
// Return:      void
//
//---------------------------------------------------------------------------

void TgtHptSaveTime (void);


//---------------------------------------------------------------------------
//
// Function:    TgtHptGetSavedTime()
//
// Description: returns the previously saved time stamp
//
// Parameters:  void
//
// Return:      DWORD
//
//---------------------------------------------------------------------------

DWORD TgtHptGetSavedTime (void);


//---------------------------------------------------------------------------
//
// Function:    TgtHptSetNewTime()
//
// Description: sets the new timer value
//
// Parameters:  dwPdoVal_p
//
// Return:      void
//
//---------------------------------------------------------------------------

void TgtHptSetNewTime (DWORD dwPdoVal_p);


//---------------------------------------------------------------------------
//
// Function:    TgtHptStartMeasure()
//
// Description: start of meassurment
//
// Parameters:  void
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC TgtHptStartMeasure (void);


//---------------------------------------------------------------------------
//
// Function:    TgtHptStoppMeasure()
//
// Description: stopp of meassurment
//
// Parameters:  void
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC TgtHptStoppMeasure (void);


//---------------------------------------------------------------------------
//
// Function:    HptInit()
//
// Description: initializes the High Presition Time Stamp module
//
// Parameters:  pfnCallback_p
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC HptInit (CCM_DECL_INSTANCE_HDL_
    tHptEventCallback     pfnCallback_p);


//---------------------------------------------------------------------------
//
// Function:    HptRegisterSyncCobId()
//
// Description: registers a new SYNC OB-ID
//
// Parameters:  dwSyncCob_p
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC HptRegisterSyncCobId (CCM_DECL_INSTANCE_HDL_
    DWORD   dwSyncCob_p);


//---------------------------------------------------------------------------
//
// Function:    HptpCheckCanMsg()
//
// Description: checks a received or sent CAN message
//
// Parameters:  pCanMsg_p
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC HptCheckCanMsg (CCM_DECL_INSTANCE_HDL_
    tCdrvMsg MEM* pCanMsg_p);


//---------------------------------------------------------------------------
//
// Function:    AppCbHptEvent()
//
// Description: callback handler in application
//
// Parameters:  void
//
// Return:      void
//
//---------------------------------------------------------------------------

void PUBLIC AppCbHptEvent (CCM_DECL_INSTANCE_HDL);


#endif // _TGTHPT_H_

// Please keep an empty line at the end of this file.

