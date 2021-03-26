/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.com

  Project:      CANopen V5

  Description:  Header file for CANopen API

  -------------------------------------------------------------------------

                $RCSfile: CcmSdoGwy.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.1 $  $Date: 2006/07/03 09:09:00 $

                $State: Exp $

                Build Environment:
                KEIL uVision 2

  -------------------------------------------------------------------------

  Revision History:

  2003/03/25 -mb:   start of implementation
  
****************************************************************************/

#ifndef _CCMSDOGW_H_
#define _CCMSDOGW_H_



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

// Default setting for SDO Gateway. If there are no settings in CopCfg.h
// then these settings are active.

#ifndef SDOGWY_INSTANCE_MAINNET
    #define SDOGWY_INSTANCE_MAINNET         0x00    // instance index for main CANopen network
#endif

#ifndef SDOGWY_INSTANCE_SUBNET
    #define SDOGWY_INSTANCE_SUBNET          0x01    // instance index for CANopen subnetwork
#endif

#ifndef SDOGWY_COBID_BASE_RX_MAINNET
    #define SDOGWY_COBID_BASE_RX_MAINNET    0x665   // base of Rx-CobId for SDO gateway on main CANopen network
#endif

#ifndef SDOGWY_COBID_BASE_TX_MAINNET
    #define SDOGWY_COBID_BASE_TX_MAINNET    0x5E5   // base of Tx-CobId for SDO gateway on main CANopen network
#endif

#ifndef SDOGWY_COBID_BASE_RX_SUBNET
    #define SDOGWY_COBID_BASE_RX_SUBNET     0x5C0   // base of Rx-CobId for SDO gateway on CANopen subnetwork
#endif

#ifndef SDOGWY_COBID_BASE_TX_SUBNET
    #define SDOGWY_COBID_BASE_TX_SUBNET     0x640   // base of Tx-CobId for SDO gateway on CANopen subnetwork
#endif

#ifndef SDOGWY_CONFIG_INDEX
    #define SDOGWY_CONFIG_INDEX             0x3000  // index for configuration of SDO gateway in OD
#endif

#ifndef SDOGWY_CONFIG_SUBINDEX_RX
    #define SDOGWY_CONFIG_SUBINDEX_RX       0x01    // subindex of Rx-CobId of SDO gateway under config-index
#endif

#ifndef SDOGWY_CONFIG_SUBINDEX_TX
    #define SDOGWY_CONFIG_SUBINDEX_TX       0x02    // subindex of Tx-CobId of SDO gateway under config-index
#endif

#ifndef SDOGWY_CONFIG_SUBINDEX_SUBDEV
    #define SDOGWY_CONFIG_SUBINDEX_SUBDEV   0x03    // subindex of Subnetwork-NodeId under config-index
#endif

#ifndef SDOGWY_MIN_NODEID_MAINDEV
    #define SDOGWY_MIN_NODEID_MAINDEV       0x01    // lowest supported node ID of SDO gateway (main network)
#endif

#ifndef SDOGWY_MAX_NODEID_MAINDEV
    #define SDOGWY_MAX_NODEID_MAINDEV       0x18    // largest supported node ID of SDO gateway (main network)
#endif

#ifndef SDOGWY_MAX_VALID_NODEID_SUBDEV
    #define SDOGWY_MAX_VALID_NODEID_SUBDEV  0x30    // highest valid node ID for subnetwork device is 48
#endif


//---------------------------------------------------------------------------
// modul global types
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// local vars
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// local function prototypes
//---------------------------------------------------------------------------

// NOTE:
// These functions are not API functions for the application.
// They are internaly used by the CCM module.
// Therefore COPDLLEXPORT is not set before these functions.


//---------------------------------------------------------------------------
//
// Function:     CcmSdogwyInit
//
// Description:  init a instance of SDO gateway
//
// Parameters:   CCM_DECL_INSTANCE_HDL = handle of instance
//
// Returns:      tCopKernel = ErrorCode
//
//---------------------------------------------------------------------------

tCopKernel  PUBLIC  CcmSdogwyInit (CCM_DECL_INSTANCE_HDL);


//---------------------------------------------------------------------------
//
// Function:     CcmSdogwyShutDown
//
// Description:  shuts a instance of SDO gateway down
//               The existing COBs of the SDO gateway
//               will be undefined.
//
// Parameters:   CCM_DECL_INSTANCE_HDL = handle of instance
//
// Returns:      tCopKernel = ErrorCode
//
//---------------------------------------------------------------------------

tCopKernel  PUBLIC  CcmSdogwyShutDown (CCM_DECL_INSTANCE_HDL);


//---------------------------------------------------------------------------
//
// Function:     CcmSdogwyNmtEvent
//
// Description:  function is called from CCM module if an NMT event did occur
//               so further initialisation can be done
//               At event kNmtEvEnterInitialising global variables will be defined.
//               At event kNmtEvResetNode the real CobIds of the PC side Communication
//               will be written into the OD.
//               At event kNmtEvPreResetCommunication the existing COBs of the SDO
//               gateway will be undefined.
//               At event kNmtEvResetCommunication the COBs of the DSO gateway
//               will be defined, if a LM-NodeId greater then 0 is configurated in OD.
//
// Parameters:   CCM_DECL_INSTANCE_HDL_ = handle of instance
//               NmtEvent_p = NMT event
//
// Returns:      tCopKernel = ErrorCode
//
//---------------------------------------------------------------------------

tCopKernel  PUBLIC  CcmSdogwyNmtEvent (CCM_DECL_INSTANCE_HDL_
    tNmtEvent NmtEvent_p);


#endif  //_CCMSDOGW_H_

