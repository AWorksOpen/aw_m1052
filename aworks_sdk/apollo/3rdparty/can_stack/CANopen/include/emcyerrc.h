/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  header for Emergency Consumer module

  -------------------------------------------------------------------------

                $RCSfile: emcyerrc.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.6 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                several
  
  -------------------------------------------------------------------------

  Revision History:
  
  2002/??/?? m.b.: start of implementation

  2004/11/12 f.j.:  new error code EMCYERRC_BUSOFF_OCCURED only for
                    CANopen master or manager, this error code signals
                    a local error BUSOFF
                    
  **************************************************************************/

#ifndef _EMCYERRC_H_
#define _EMCYERRC_H_


// ============================================================================
// emergency error codes
// ============================================================================
#define  EMCYERRC_NO_ERROR                               0x0000
//#define  EMCYERRC_GENERIC_ERROR                          0x10xx
//#define  EMCYERRC_CURRENT                                0x20xx
//#define  EMCYERRC_CURRENT_DEVICE_INPUT_SIDE              0x21xx
//#define  EMCYERRC_CURRENT_INSIDE_THE_DEVICE              0x22xx
//#define  EMCYERRC_CURRENT_DEVICE_OUTPUT_SIDE             0x23xx
//#define  EMCYERRC_VOLTAGE                                0x30xx
//#define  EMCYERRC_MAINS_VOLTAGE                          0x31xx
//#define  EMCYERRC_VOLTAGE_INSIDE_THE_DEVICE              0x32xx
//#define  EMCYERRC_OUTPUT_VOLTAGE                         0x33xx
//#define  EMCYERRC_TEMPERATURE                            0x40xx
//#define  EMCYERRC_AMBIENT_TEMPERATURE                    0x41xx
//#define  EMCYERRC_DEVICE_TEMPERATURE                     0x42xx
//#define  EMCYERRC_DEVICE_HARDWARE                        0x50xx
//#define  EMCYERRC_DEVICE_SOFTWARE                        0x60xx
//#define  EMCYERRC_INTERNAL_SOFTWARE                      0x61xx
//#define  EMCYERRC_USER_SOFTWARE                          0x62xx
//#define  EMCYERRC_DATA_SET                               0x63xx
//#define  EMCYERRC_ADDITIONAL_MODULES                     0x70xx
//#define  EMCYERRC_MONITORING                             0x80xx
//#define  EMCYERRC_COMMUNICATION                          0x81xx
#define  EMCYERRC_CAN_OVERRUN                            0x8110
#define  EMCYERRC_CAN_IN_ERROR_PASSIVE_MODE              0x8120
#define  EMCYERRC_LIFEGUARD_ERROR_OR_HEARTBEAT_ERROR     0x8130
#define  EMCYERRC_RECOVERED_FROM_BUSOFF                  0x8140
#define  EMCYERRC_BUSOFF_OCCURED                         0x8141
#define  EMCYERRC_TRANSMIT_COBID_COLLISION               0x8150
//#define  EMCYERRC_PROTOCOLL_ERROR                        0x82xx
#define  EMCYERRC_PDO_NOT_PROCESSED_DUE_TO_LENGTH_ERROR  0x8210
#define  EMCYERRC_PDO_LENGTH_EXCEEDED                    0x8220
//#define  EMCYERRC_EXTERNAL_ERROR                         0x90xx
//#define  EMCYERRC_ADDITIONAL_FUNCTIONS                   0xf0xx
//#define  EMCYERRC_DEVICE_SPECIFIC                        0xffxx


#endif // _EMCYERRC_H_

// Please keep an empty line at the end of this file.

