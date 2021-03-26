/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.com

  Project:      CANopen V5 / CAN driver

  Description:  definitions for all CANopen return codes

  -------------------------------------------------------------------------

                $RCSfile: errordef.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.63 $  $Date: 2007/01/08 13:47:35 $
                
                $State: Exp $
                
                Build Environment:
                    all

  -------------------------------------------------------------------------

  Revision History:

  2002/XX/XX r.d.:  start of implementation
  2004/08/30 f.j.:  neuen Eintrag im COB Modul definiert
  2004/10/27 f.j.:  new error code for configuration manager modul
  2004/10/28 a.s.:  new error code for ccmstore-modul

****************************************************************************/

#ifndef _ERRORDEF_H_
#define _ERRORDEF_H_


//---------------------------------------------------------------------------
// return codes
//---------------------------------------------------------------------------

typedef enum
{
    // area for generic errors 0x0000 - 0x000F
    kCopSuccessful              = 0x0000,       // no error/successful run
    kCopIllegalInstance         = 0x0001,       // the called Instanz are not exist
    kCopInvalidInstanceParam    = 0x0002,       // 
    kCopNoFreeInstance          = 0x0003,       // XxxAddInstance was called but no free instance is available
    kCopWrongSignature          = 0x0004,       // wrong signature while writing to object 0x1010 or 0x1011
    kCopUnsupportedBtrTable     = 0x0005,       // unsupported baudrate table was configured with LSS service
    kCopUnsupportedBtrIndex     = 0x0006,       // unsupported baudrate index was configured with LSS service
    kCopInvalidNodeId           = 0x0007,       // CANopen was initialized with an invalid NodeId
    kCopNoResource              = 0x0008,       // resource could not be created (Windows, PxROS, ...)
    kCopInvalidParam            = 0x0009,       // generic error code for an invalid param

    // area for CDRV module 0x0010 - 0x001F
    kCopCdrvNoMsg               = 0x0010,       // no CAN message was received
    kCopCdrvMsgHigh             = 0x0011,       // CAN message with high priority was received
    kCopCdrvMsgLow              = 0x0012,       // CAN message with low priority was received
    kCopCdrvInitError           = 0x0013,       // 
    kCopCdrvNoFreeChannel       = 0x0014,       // CAN controller has not a free channel
    kCopCdrvTxBuffHighOverrun   = 0x0015,       // buffer for high priority CAN transmit messages has overrun
    kCopCdrvTxBuffLowOverrun    = 0x0016,       // buffer for low priority CAN transmit messages has overrun
    kCopCdrvIllegalBdi          = 0x0017,       // unsupported baudrate within baudrate table
    kCopCdrvBusy                = 0x0018,       // remote frame can not be updated because no bus contact or CAN
                                                // transmission is activ
    kCopCdrvInvalidDriverType   = 0x0019,       // (PC: Windows or Linux) invalid driver type
    kCopCdrvDriverNotFound      = 0x001A,       // (PC: Windows or Linux) driver (DLL) could not be found
    kCopCdrvInvalidBaseAddress  = 0x001B,       // (PC: Windows or Linux) driver could not found the CAN controller
    kCopCdrvInvalidParam        = 0x001C,       // invalid param in function call
    kCopCdrvInvalidDevNumber    = 0x001D,       // (PC: Windows or Linux) the given device number is invalid
    kCopCdrvDevAlreadyInUse     = 0x001E,       // (PC: Windows or Linux) the device with the given number is already in use
    kCopCdrvHostOsError         = 0x001F,       // (PC: Windows or Linux) OS specific internal error (e.g. malloc, copy_from_user, ...)

    // area for COB module 0x0020 - 0x002F
    kCopCobNoFreeEntry          = 0x0020,       // no free entry in RX- or TX-COB table
    kCopCobAlreadyExist         = 0x0021,       // COB-ID already exists in RX- resp. TX-COB table
    kCopCobIllegalHdl           = 0x0022,       // illegal handle for a COB was assigned
    kCopCobIllegalCanId         = 0x0023,       // COB-ID is not allowed (like 0x000 is reserved for NMT, ...)
    kCopCobInvalidCanId         = 0x0024,       // COB-ID is switched off 
    kCopCobCdrvStateSet         = 0x0025,       // at least one bit of CAN driver state is set
    kCopCobNoFreeEntryHighBuf   = 0x0026,       // no free entry in high priotity RX- or TX-COB table
    kCopCobOwnId                = 0x0027,       // COB-ID already exists in own module which calls CobDefine() or CobCheck()
    kCopCobWrongCobType         = 0x0028,       // e.g. a COB which was definded as Rx-Message should be sent

    // area for OBD module 0x0030 - 0x003F
    kCopObdIllegalPart          = 0x0030,       // unknown OD part
    kCopObdIndexNotExist        = 0x0031,       // object index does not exist in OD
    kCopObdSubindexNotExist     = 0x0032,       // subindex does not exist in object index
    kCopObdReadViolation        = 0x0033,       // read access to a write-only object
    kCopObdWriteViolation       = 0x0034,       // write access to a read-only object
    kCopObdAccessViolation      = 0x0035,       // access not allowed
    kCopObdUnknownObjectType    = 0x0036,       // object type not defined/known
    kCopObdVarEntryNotExist     = 0x0037,       // object does not contain VarEntry structure
    kCopObdValueToLow           = 0x0038,       // value to write to an object is to low
    kCopObdValueToHigh          = 0x0039,       // value to write to an object is to high
    kCopObdValueLengthError     = 0x003A,       // value to write is to long or to short
    kCopObdIllegalFloat         = 0x003B,       // illegal float variable
    kCopObdWrongOdBuilderKey    = 0x003F,       // OD was generated with demo version of tool ODBuilder

    // area for NMT module 0x0040 - 0x004F
    kCopNmtUnknownCommand       = 0x0040,       // unknown NMT command
    kCopNmtLifeguardVsHeartbeat = 0x0041,       // one of life guarding or heartbeat was configured but
                                                // the other one is already configured
    kCopNmtBootupNotPossible    = 0x0042,       // bootup message can not be sent in this NMT state
    kCopNmtHbcWrongCfg          = 0x0043,       // configuration of Heartbeat Consumer was wrong
    kCopNmtmSlaveNodeExist      = 0x0044,       // NMT slave node can not be added because it does already exist
    kCopNmtmNoFreeSlaveEntry    = 0x0045,       // no free entry to add a new NMT slave node
    kCopNmtmSlaveEntryNotExist  = 0x0046,       // NMT slave node was never added
    kCopNmtStateError           = 0x0047,       // error has occured during NMT state machine
    kCopNmtmInvalidSlaveNodeId  = 0x0048,

    // area for SDO client module 0x0050 - 0x005F
    kCopSdocInitError           = 0x0050,       // initial. or changing parameter not successful
    kCopSdocInvalidParam        = 0x0051,       // one of the function parameter is invalid
    kCopSdocClientNotExist      = 0x0052,       // selected SDO client does not exist
    kCopSdocBusy                = 0x0053,       // SDO client is already busy
    kCopSdocTransferError       = 0x0054,       // an error occured during SDO transfer    
    kCopSdocWrongMultiplexor    = 0x0055,       // an SDO response was received with wron multiplexor
    kCopSdocEntryAlreadyDefined = 0x0056,       // client entry is already defined
    kCopSdocNoFreeEntry         = 0x0057,       // no free client entry found in SDO server table
    kCopSdocConfigOrder         = 0x0058,

    // area for SDO server module 0x0060 - 0x006F
    kCopSdosInitError           = 0x0060,       // initial. or changing parameter not successful
    kCopSdosServerNotExist      = 0x0061,       // selected SDO server does not exist
    kCopSdosBusy                = 0x0062,       // SDO server is already busy
    kCopSdosSizeMismatch        = 0x0063,       // 
    kCopSdosNoFreeEntry         = 0x0064,       // no free server entry found in SDO server table
    kCopSdosEntryAlreadyDefined = 0x0065,       // server entry is already defined
    kCopSdosConfigOrder         = 0x0066,
    kCopSdosWrongDlc            = 0x0067,
    kCopSdosWrongCcs            = 0x0068,

    // area for PDO module 0x0070 - 0x007F
    kCopPdoNotExist             = 0x0070,       // selected PDO does not exist
    kCopPdoLengthExceeded       = 0x0071,       // length of PDO mapping exceedes 64 bis
    kCopPdoGranularityMismatch  = 0x0072,       // configured PDO granularity is not equal to supported granularity
    kCopPdoInitError            = 0x0073,       // error during initialisation of PDO module
    kCopPdoErrorPdoEncode       = 0x0074,       // error during encoding a PDO
    kCopPdoErrorPdoDecode       = 0x0075,       // error during decoding a PDO
    kCopPdoErrorSend            = 0x0076,       // error during sending a PDO
    kCopPdoErrorSyncWin         = 0x0077,       // the SYNC window runs out during sending SYNC-PDOs
    kCopPdoErrorMapp            = 0x0078,       // invalid PDO mapping
    kCopPdoVarNotFound          = 0x0079,       // variable was not found in function PdoSignalVar()
    kCopPdoErrorEmcyPdoLen      = 0x007A,       // the length of a received PDO is unequal to the expected value
    kCopPdoWriteConstObject     = 0x007B,       // constant object can not be written
                                                // (only TxType, Inhibit-, Event Time for CANopen Kit)
    kCopPdoErrorRxEventTimer    = 0x007C,       // If an RPDO is configured with Event Timer, but PDO
                                                // is not received after this time CcmCbError() will be called
                                                // with this error code.

    // area for LSS slave module 0x0080 - 0x008F
    kCopLsssResetNode           = 0x0080,       // NMT command "reset node" has to be processed after LSS configuration
                                                // new of NodeId
    kCopLsssInvalidNodeId       = 0x0081,       // no valid NodeId is configured -> wait until it is configured with
                                                // LSS service before calling CcmConnectToNet()

    // area for emergency consumer module 0x0090 - 0x009F
    kCopEmccNoFreeProducerEntry = 0x0090,       // no free entry to add a Emergency Producer
    kCopEmccNodeIdNotExist      = 0x0091,       // selected NodeId was never added 
    kCopEmccNodeIdInvalid       = 0x0092,       // selected NodeId is outside of range (0x01 until 0x7F)
    kCopEmccNodeIdExist         = 0x0093,       // selected NodeId already exist

    // area for dynamic OD 0x00A0 - 0x00AF
    kCopDynNoMemory             = 0x00A0,       // no memory available
    kCopDynInvalidConfig        = 0x00A1,       // invalid configuration in segment container

    // area for hertbeat consumer module 0x00B0 - 0x00BF
    kCopHbcEntryNotExist        = 0x00B0,       // Heartbeat Producer node not configured
    kCopHbcEntryAlreadyExist    = 0x00B1,       // NodeId was already defined in heartbeat consumer table (object 0x1016)
    kCopHbcNoFreeEntry          = 0x00B2,

    // Configuration manager
    kCopCfgMaConfigError        = 0x00C0,       // error in configuration manager
    kCopCfgMaSdocTimeOut        = 0x00C1,       // error in configuration manager, Sdoc timeout 
    kCopCfgMaNoErrorCb          = 0x00C2,       // no error callback function is used
    kCopCfgMaNoFreeConfig       = 0x00C3,       // no free entry in internal list for configuratio
    kCopCfgMaNoConfigData       = 0x00C4,       // no data for configuration found in 0x1F22
    kCopCfgMaInvaildSdoc        = 0x00C5,       // the SDO Client Index is not in Rang 0x1280 util 0x1300
                                                // but configurtration data for this node exists in 0x1F22
    kCopCfgMaConfigwithErrors   = 0x00C6,       // configuration of node finished, but with errors

    // area for LSS master module 0x00D0 - 0x00DF
    kCopLssmIllegalMode         = 0x00D0,       // illegal LSS mode (operation / configuration)
    kCopLssmIllegalState        = 0x00D1,       // function was called in illegal state of LSS master
    kCopLssmBusy                = 0x00D2,       // LSS process is busy with an previous service
    kCopLssmIllegalCmd          = 0x00D3,       // illegal command code was set for function LssmInquireIdentity()
    kCopLssmTimeout             = 0x00D4,       // LSS slave did not answer a LSS service
    kCopLssmErrorInConfirm      = 0x00D5,       // LSS slave replied an error code for a LSS service

    // area for CCM modules 0x00E0 - 0xEF
    kCopCcmStoreUnvalidState    = 0x00E0,       // memory device not available due device state
    kCopCcmStoreHwError         = 0x00E1,       // hw error due device access

    // area for SRDO module 0x0100 - 0x011F
    kCopSrdoNotExist            = 0x0100,       // selected SRDO does not exist
    kCopSrdoGranularityMismatch = 0x0101,       // configured SRDO granularity is not equal to supported granularity
    kCopSrdoCfgTimingError      = 0x0102,       // configuration is not ok (Timing)
    kCopSrdoCfgIdError          = 0x0103,       // configuration is not ok (CobIds)
    kCopSrdoCfgCrcError         = 0x0104,       // configuration is not ok (CRC)
    kCopSrdoNmtError            = 0x0105,       // an action was tried in a wrong NMT state
    kCopSrdoInvalidCfg          = 0x0106,       // an action was tried with an invald SRDO configuration
    kCopSrdoInvalid             = 0x0107,       // an action was tried with an invald SRDO
    kCopSrdoRxTxConflict        = 0x0108,       // an transmission was tried with an receive SRDO (or the other way)
    kCopSrdoIllegalCanId        = 0x0109,       // the CanId is invalid
    kCopSrdoCanIdAlreadyInUse   = 0x010A,       // the CanId is already in use
    kCopSrdoNotInOrder          = 0x010B,       // the two messages of a SRDO are not in order
    kCopSrdoSctTimeout          = 0x010C,       // timeout of SCT
    kCopSrdoSrvtTimeout         = 0x010D,       // timeout of SRVT
    kCopSrdoCanIdNotValid       = 0x010E,       // one of received CAN-IDs are not equal to configured one
    kCopSrdoDlcNotValid         = 0x010F,       // one of received CAN-DLC are not equal to configured one
    kCopSrdoErrorMapp           = 0x0110,       // wrong values in mapping found
    kCopSrdoDataError           = 0x0111,       // data of CAN messages are not invers
    kCopSrdoLengthExceeded      = 0x0112,       // length of SRDO mapping exceedes 64 bit per CAN-message
    kCopSrdoNotHandledInApp     = 0x0113,       // the SRDO error was not handled in AppSrdoError()
    kCopSrdoOverrun             = 0x0114,       // a RxSRDO was received but the pevious one was not else processed

    // area for SDO Manager  0x0120 - 0x012F
    //kCopSdomListFull            = 0x0120,       // List for registration of possible SRD Clients is full
    kCopSdomBusy                = 0x0121,       // SDO Manager works    
    kCopSdomInternStructFull    = 0x0122,       // an internal structure is full
    kCopSdomErrorCbInternError  = 0x0123,       // for Errorcallback function to indicate an internal error
    kCopSdomErrorCbCopError     = 0x0124,       // for Errorcallback function to indicate a Cop Error
    
    // area for SRD Client   
    kCopSrdBusy                 = 0x0130,       // SRD Client is not in State Idle
    kCopSrdNotRegistered        = 0x0131,       // SRD Client not registered to SDO Manager
    kCopSrdNoAnswerSdom         = 0x0132,       // SRD Client got no answer from SDO Manager
    kCopSrdNotMasterMode        = 0x0133,       // SRD Client MasterMode not TRUE
    kCopSrdNotDynConnection     = 0x0134,       // the connection to release is no dynamic connection
                                                // (dyn-flag in COB-IDs not set (bit 30))
                                                // to save predefine connection, connection could not be released
    kCopSrdSdomNotUndefSdoc     = 0x0135,       // SDO Manager did not undefine SDO Client after
                                                // release connection was successfully requested


    // area for CANopen Manager (COPMGR)
    kCopMgrTaskDeferred         = 0x0140,       // CopMgr performs task in background and informs the application, when it is finished
    kCopMgrInvalidOperation     = 0x0141,       // calling CopMgrEnterOperational() or any other function call is not allowed in this situation
    kCopMgrInvalidParam         = 0x0142,       // passed invalid parameters to a function (e.g. invalid node id)
    //kCopMgrSdomError            = 0x0143,       // Error in SDO manager while in boot slave process
    kCopMgrNoFreeEntry          = 0x0144,       // no free entry in node list
    kCopMgrNodeBusy             = 0x0145,       // CopMgr performs SDO transfers to the request slave node by itself; no other SDO transfer possible at the moment
    kCopMgrSdocTypeAutoRunning  = 0x0146,        // a SDO transfer with type kCopMgrSdocTypeAuto is already running

    // area untill 0x07FF is reserved
    // area for user application from 0x0800 to 0x7FFF
    kCopUserSDOAbort            = 0x0800

} tCopKernel;


#endif
//EOF

// Please keep an empty line at the end of this file.

