/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      ...

  Description:  optional file of CCM module
                functions to convert float number's for Tasking CC166 Compiler

  -------------------------------------------------------------------------

                $RCSfile: ccmfloat.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.4 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                    Tasking CC166

  -------------------------------------------------------------------------

  Revision History:

  r.d.: first implementation

****************************************************************************/


#ifndef _CCMFLOAT_H_
#define _CCMFLOAT_H_

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*          G L O B A L   D E F I N I T I O N S                            */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

//---------------------------------------------------------------------------
// const defines
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// modul global types
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// local vars
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// local function prototypes
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//
// Function:    CcmConvertFloat
//
// Description: function converts a float number from TASKING definition to 
//              CANopen definition
//              all float Value with used in CANopen OD have to convert before
//              write into the OD
//
//
// Parameters:  Float_p  = float value in TASKING version
//
// 
// Returns:     float    = float in CANopen version
//
//---------------------------------------------------------------------------

float CcmConvertFloat(float Float_p);



#endif // _CCMFLOAT_H_

// Please keep an empty line at the end of this file.

