/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  LSS types and konstants

  -------------------------------------------------------------------------

                $RCSfile: Lss.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.7 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

  r.d.: first implementation

****************************************************************************/

#ifndef _LSS_H_
#define _LSS_H_


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

#define kLssSwitchModeGlobal                4    // m->s all
#define kLssConfigureNodeId                 17   // m->s cfg
#define kLssConfigureNodeId_Confirm         17   // m<-s cfg
#define kLssConfigureBitTiming              19   // m->s cfg
#define kLssConfigureBitTiming_Confirm      19   // m<-s cfg
#define kLssActivateBitTiming               21   // m->s cfg
#define kLssStoreConfiguration              23   // m->s cfg
#define kLssStoreConfiguration_Confirm      23   // m<-s cfg
#define kLssSwitchModeSel_VendorId          64   // m->s all
#define kLssSwitchModeSel_ProductId         65   // m->s all
#define kLssSwitchModeSel_RevisionNr        66   // m->s all
#define kLssSwitchModeSel_SerialNr          67   // m->s all
#define kLssSwitchModeSel_Confirm           68   // m<-s all
#define kLssIdentifyRemSlv_VendorId         70   // m->s cfg
#define kLssIdentifyRemSlv_ProductId        71   // m->s cfg
#define kLssIdentifyRemSlv_RevisionNrL      72   // m->s cfg
#define kLssIdentifyRemSlv_RevisionNrH      73   // m->s cfg
#define kLssIdentifyRemSlv_SerialNrL        74   // m->s cfg
#define kLssIdentifyRemSlv_SerialNrH        75   // m->s cfg
#define kLssIdentifyNonConfigRemSlv         76   // m->s cfg
#define kLssIdentifySlave                   79   // m<-s cfg answer to 70-75 if LSS address is in range
#define kLssIdentifyNonConfigSlave          80   // m<-s cfg answer to 76 if invalid node-ID (0xFF)
#define kLssInquireVendorId                 90   // m->s cfg
#define kLssInquireVendorId_Confirm         90   // m<-s cfg
#define kLssInquireProductId                91   // m->s cfg
#define kLssInquireProductId_Confirm        91   // m<-s cfg
#define kLssInquireRevisionNr               92   // m->s cfg
#define kLssInquireRevisionNr_Confirm       92   // m<-s cfg
#define kLssInquireSerialNr                 93   // m->s cfg
#define kLssInquireSerialNr_Confirm         93   // m<-s cfg
#define kLssInquireNodeId                   94   // m->s cfg
#define kLssInquireNodeId_Confirm           94   // m<-s cfg


//---------------------------------------------------------------------------
// modul global types
//---------------------------------------------------------------------------

typedef tFastByte tLssCommand;


//---------------------------------------------------------------------------
// local vars
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// local function prototypes
//---------------------------------------------------------------------------



#endif // _LSS_H_

// Please keep an empty line at the end of this file.

