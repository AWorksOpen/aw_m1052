/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen Manager

  Description:  Header file for CANopen Manager Extended API for COBs
                (Communication Objects)

  -------------------------------------------------------------------------

                $RCSfile: CmCob.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.1 $  $Date: 2006/05/05 10:08:54 $
                
                $State: Exp $
                
                Build Environment:
                    all

  -------------------------------------------------------------------------

  Revision History:

  2006/02/23 d.k.:  Start of Implementation

****************************************************************************/

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

// must be the same as defined in Cob.h
#define CMCOB_TYPE_SEND     0x00    // for standard CAN messages to send
#define CMCOB_TYPE_RECV     0x01    // for standard CAN messages to receive
#define CMCOB_TYPE_RTR      0x02    // RTR frame
#define CMCOB_TYPE_EXT      0x80    // extended CAN message


//=========================================================================//
//                                                                         //
//          P U B L I C   F U N C T I O N S                                //
//                                                                         //
//=========================================================================//

// ----------------------------------------------------------------------------
//
// Function:    CmCobDefine()
//
// Description: defines a new COB with the specified COB-ID and message type.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              CobId_p                 = CAN ID
//              bCobType_p              = CAN message type (CMCOB_TYPE_*)
//              bSize_p                 = message size
//
// Returns:     tCopKernel              = error code
//
// State:       
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CmCobDefine (CCM_DECL_INSTANCE_HDL_
                                            tCanId CobId_p, BYTE bCobType_p, BYTE bSize_p);

// ----------------------------------------------------------------------------
//
// Function:    CmCobUndefine()
//
// Description: undefines the COB with the specified COB-ID and message type.
//              If CobId_p equals 0, all COBs of the specified message type
//              will be undefined.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              CobId_p                 = CAN ID
//              bCobType_p              = CAN message type (CMCOB_TYPE_*)
//
// Returns:     tCopKernel              = error code
//
// State:       
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CmCobUndefine (CCM_DECL_INSTANCE_HDL_
                                              tCanId    CobId_p,
                                              BYTE      bCobType_p);

// ----------------------------------------------------------------------------
//
// Function:    CmCobSend()
//
// Description: sends a CAN message with the specified COB-ID.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              CobId_p                 = CAN ID
//              pbData_p                = message data
//              bSize_p                 = message size
//
// Returns:     tCopKernel              = error code
//
// State:       
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CmCobSend (CCM_DECL_INSTANCE_HDL_
                                          tCanId CobId_p, BYTE * pbData_p, BYTE bSize_p);
