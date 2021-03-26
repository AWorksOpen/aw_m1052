/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen

  Description:  include file for CANopen archiv functions

  -------------------------------------------------------------------------

                $RCSfile: TgtCav.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.3 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                	X

  -------------------------------------------------------------------------

  Revision History:

  2004/10/19 -as:   start of the implementation

  2004/11/15 -as:   fuction TgtCavCheckValid added

****************************************************************************/

#ifndef _TGTCAV_H_
#define _TGTCAV_H_


//---------------------------------------------------------------------------
// const defines
//---------------------------------------------------------------------------

#define CAV_OPEN_READ               0x01
#define CAV_OPEN_WRITE              0x80

// storage read access parameters for objects 0x1010 and 0x1011 (see DS301)
#define CAV_ATTRIB_UNSUPPORTED      0x00000000L
#define CAV_ATTRIB_ON_COMMAND       0x00000001L
#define CAV_ATTRIB_AUTONOMOUSLY     0x00000002L
#define CAV_ATTRIB_DEVICE_STATE     0x00000004L   // not included in standard DS301


//---------------------------------------------------------------------------
// typedef
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// function prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// Function:     TgtCavInit()
//
// Description:  initializes functionality for STORE, RESTORE and LOAD
//               for OD values in other memory mediums, initializes
//               interface to non-volatile memory if it not done by
//               operating systems
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//
// Returns:      tCopKernel             = error code
//
//---------------------------------------------------------------------------

tCopKernel PUBLIC TgtCavInit (CCM_DECL_INSTANCE_HDL);


//---------------------------------------------------------------------------
//
// Function:     TgtCavShutDown()
//
// Description:  disable interface to non-volatile memory
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//
// Returns:      tCopKernel             = error code
//
//---------------------------------------------------------------------------

tCopKernel PUBLIC TgtCavShutDown (CCM_DECL_INSTANCE_HDL);


//---------------------------------------------------------------------------
//
// Function:     TgtCavCreate()
//
// Description:  Function creates an archiv for the selected OD part. In
//               existence archiv is set invalid.
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//               bCurrentOdPart_p       = selected OD-part
//
// Returns:      tCopKernel             = error code
//
//---------------------------------------------------------------------------

tCopKernel PUBLIC TgtCavCreate (CCM_DECL_INSTANCE_HDL_
    tObdPart bCurrentOdPart_p);


//---------------------------------------------------------------------------
//
// Function:     TgtCavDelete()
//
// Description:  Function sets an archiv invalid.
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//               bCurrentOdPart_p       = selected OD-part
//
// Returns:      tCopKernel             = error code
//
//---------------------------------------------------------------------------

tCopKernel PUBLIC TgtCavDelete (CCM_DECL_INSTANCE_HDL_
    tObdPart bCurrentOdPart_p);


//---------------------------------------------------------------------------
//
// Function:     TgtCavOpen()
//
// Description:  Function opens an in existence archiv.
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//               bCurrentOdPart_p       = selected OD-part
//               bAttrib_p              = attribute: read, write
//
// Returns:      tCopKernel             = error code
//
//---------------------------------------------------------------------------


tCopKernel PUBLIC TgtCavOpen (CCM_DECL_INSTANCE_HDL_
    tObdPart bCurrentOdPart_p, BYTE bAttrib_p);


//---------------------------------------------------------------------------
//
// Function:     TgtCavClose()
//
// Description:  Function closes an archiv for the selected OD part by
//               setting a valid signature.
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//               bCurrentOdPart_p       = selected OD-part
//
// Returns:      tCopKernel             = error code
//
//---------------------------------------------------------------------------

tCopKernel PUBLIC TgtCavClose (CCM_DECL_INSTANCE_HDL_
    tObdPart bCurrentOdPart_p);


//---------------------------------------------------------------------------
//
// Function:     TgtCavStore()
//
// Description:  Function saves the parameter of the select part into
//               memory.
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//               bCurrentOdPart_p       = selected OD-part
//               pbData                 = pointer to source data
//               dwSize_p               = number of bytes
//
// Returns:      tCopKernel             = error code
//
//---------------------------------------------------------------------------

tCopKernel PUBLIC TgtCavStore (CCM_DECL_INSTANCE_HDL_
    tObdPart bCurrentOdPart_p, BYTE GENERIC *pbData, DWORD dwSize_p);


//---------------------------------------------------------------------------
//
// Function:     TgtCavRestore()
//
// Description:  Function reads the parameter from memory and stores the
//               value into the selected OD part.
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//               bCurrentOdPart_p       = selected OD-part
//               pbData                 = pointer to destination data
//               dwSize_p               = number of bytes
//
// Returns:      tCopKernel             = error code
//
//---------------------------------------------------------------------------

tCopKernel PUBLIC TgtCavRestore (CCM_DECL_INSTANCE_HDL_
    tObdPart bCurrentOdPart_p, BYTE GENERIC *pbData, DWORD dwSize_p);


//---------------------------------------------------------------------------
//
// Function:     TgtCavGetAttrib()
//
// Description:  The function gets the attrib of accessed OD part for storing/
//               restoring.
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//               wIndex_p               = identifies command (0x1010 = save, 0x1011 = load)
//               bSubIndex_p            = identifies OD part
//
// Returns:      attrib
//               CAV_ATTRIB_UNSUPPORTED  = 0x00000000 - Device does not save parameters.
//               CAV_ATTRIB_ON_COMMAND   = 0x00000001 - Device saves parameters on command.
//               CAV_ATTRIB_AUTONOMOUSLY = 0x00000002 - Device saves parameters autonomously.
//               CAV_ATTRIB_DEVICE_STATE = 0x00000004 - Device is not in rigth device state.
//                                        (0x00000004 is not included in DS301)
//
//---------------------------------------------------------------------------

DWORD PUBLIC TgtCavGetAttrib (CCM_DECL_INSTANCE_HDL_
    WORD wIndex_p, BYTE bSubIndex_p);


//---------------------------------------------------------------------------
//
// Function:     TgtCavCheckValid()
//
// Description:  Function checks up if signature of selected OD
//               part is valid.
//
// Parameters:   CCM_DECL_INSTANCE_HDL  = (instance handle)
//               bCurrentOdPart_p       = selected OD-part
//
// Returns:      FALSE  -> signature is invalid
//               TRUE   -> signature is valid
//
//---------------------------------------------------------------------------

BYTE PUBLIC TgtCavCheckValid (CCM_DECL_INSTANCE_HDL_
    tObdPart bCurrentOdPart_p);


#endif  // #ifndef _TGTCAV_H_


// Please keep an empty line at the end of this file.
