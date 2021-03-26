/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5 / CAN driver

  Description:  gereric interface for all CAN driver

  -------------------------------------------------------------------------

                $RCSfile: Cdrv.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.66 $  $Date: 2007/01/08 13:59:43 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

****************************************************************************/


#ifndef _CDRV_H_
#define _CDRV_H_


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

// Event definitions (used by PXROS, LINUX etc.)
#define CAN_RX_EVENT                0x00000001L
#define CAN_TX_EVENT                0x01000000L
#define CAN_ERR_EVENT               0x40000000L

#define CDRV_HPT_OFF                0
#define CDRV_HPT_PRODUCER           1
#define CDRV_HPT_CONSUMER           2
#define CDRV_HPT_TYPEMASK           (CDRV_HPT_OFF | CDRV_HPT_PRODUCER | CDRV_HPT_CONSUMER)
#define CDRV_HPT_MEASURE            16
#define CDRV_HPT_PRODUCER_MEAS      (CDRV_HPT_PRODUCER | CDRV_HPT_MEASURE)
#define CDRV_HPT_CONSUMER_MEAS      (CDRV_HPT_CONSUMER | CDRV_HPT_MEASURE)


//---------------------------------------------------------------------------
// default configuration
//---------------------------------------------------------------------------

// CDRV_MAX_INSTANCES defines count of instances of CAN driver.
// It should never be greater than COP_MAX_INSTANCES. If COP_MAX_INSTANCES
// (yes COP_MAX_INSTANCES) is greater than 1 the first parameter of
// functions of CAN driver is the instance number tCdrvInstance.
#ifndef CDRV_MAX_INSTANCES
    #define CDRV_MAX_INSTANCES          1
#endif

// Here you can define, which CAN-Controller should be used. It is only
// neccessary to define, if you use only one instance of the CAN-driver.
// otherwise the define is ignored. 0 for CAN1 and 1 for CAN2. (Fujitsu only !!!)
#ifndef CDRV_USED_CAN_CONTROLLER
    #define CDRV_USED_CAN_CONTROLLER    0
#endif

// This define is only used for FUULCAN-controller !!!
// If CDRV_USE_BASIC_CAN != FALSE, then one BASIC-CAN-Channel
// for receive is used
#ifndef CDRV_USE_BASIC_CAN
    #define CDRV_USE_BASIC_CAN          TRUE
#endif

// Here you can define which CAN specification is used (CiA specification).
// Should 29 bit CAN identifiers be supported then use CAN20B. Otherwise
// use CAN20A. With CAN20A the CAN identifier is typed as 16 bit WORD. So
// you can keep back some data memory for future use.
#ifndef CDRV_CAN_SPEC
    #define CDRV_CAN_SPEC               CAN20A
#endif

// If CDRV_USE_HIGHBUFF != FALSE high priority buffer will be implemented.
// With high priority buffer can important CAN messages be received or
// sent faster than other CAN messages. Important CAN messages should
// have kMtbHighPrio in m_MsgType of tCdrvMsg or in m_CobType of tCobParam.
// An application with few code and/or data memory can set this define
// to zero to get some memory for other things.
#ifndef CDRV_USE_HIGHBUFF
    #define CDRV_USE_HIGHBUFF           TRUE
#endif

// If CDRV_USE_NO_TXBUFF != FALSE an OS will not implement a Tx Buffer.
// CAN messages will be sent directly to the CAN driver.
#ifndef CDRV_USE_NO_TXBUFF
    #define CDRV_USE_NO_TXBUFF          FALSE
#endif

// If CDRV_USE_NO_RXBUFF != FALSE it will not implement a Rx Buffer.
// CAN messages will be received directly from the CAN controller.
#ifndef CDRV_USE_NO_RXBUFF
    #define CDRV_USE_NO_RXBUFF          FALSE
#endif


// If CDRV_USE_IDVALID != FALSE driver will check CAN-ID before copying
// CAN message in buffer. If CAN-ID is not known for this application
// CAN message will not be copied.
#ifndef CDRV_USE_IDVALID
    #define CDRV_USE_IDVALID            TRUE
#endif

// CDRV_IDINFO_ALGO specifies IdInfo algorithm. CAN driver filters
// CAN messages by CAN-ID. For that there are some diferent algorithm
// Some of them are data memory intensive but faster and some of them
// use less data memory but are slower. You can use:
//   CDRV_IDINFO_ALGO_BITFIELD:  bit field algorithm (very fast)
//   CDRV_IDINFO_ALGO_FULLCAN:   messages will be filtered by channels of CAN controller
//   CDRV_IDINFO_ALGO_IDLISTEXT: ID list with extended info (29 bit CAN-IDs)
#ifndef CDRV_IDINFO_ALGO
    #define CDRV_IDINFO_ALGO            CDRV_IDINFO_ALGO_BITFIELD
#endif

// CDRV_TIMESTAMP specifies if a timestamp is set in the message-struct of
// received CAN-messages. The timestamp has the format DWORD and is scaled
// in ms. The timestamp is relative.
#ifndef CDRV_TIMESTAMP
    #define CDRV_TIMESTAMP              FALSE
#endif

// For IdInfo algorithm == CDRV_IDINFO_ALGO_IDLISTEXT
// this defines the max count of list entries. For other algorithm this define
// is disregarded.
#ifndef CDRV_IDINFO_ENTRIES
    #define CDRV_IDINFO_ENTRIES         10
#endif

// Error state of CAN controller can be read in two different ways.
// If CDRV_USE_ERROR_ISR is set to FALSE Funktion CdrvGetStatus() reads
// the error state of CAN controller and returns it in tCdrvStatus.
// If CDRV_USE_ERROR_ISR is set to TRUE CAN Interrupt Service Routine reads
// the error state of CAN controller put it in an global variable and
// function CdrvGetStatus() returns it in tCdrvStatus.
// NOTE: Using CDRV_USE_ERROR_ISR with TRUE may occur much interrupts if
//      no CAN bus is connectid.
#ifndef CDRV_USE_ERROR_ISR
    #define CDRV_USE_ERROR_ISR          FALSE
#endif

// For CANopen for Windows this define means the count RX messages which
// will be polled from CDRV wrapper.
#ifndef CDRV_MAX_RXPOLLING
    #define CDRV_MAX_RXPOLLING          10
#endif

#ifndef CDRV_USE_DELETEINST_FUNC
    #define CDRV_USE_DELETEINST_FUNC    TRUE
#endif

#ifndef CDRV_USE_HPT
    #define CDRV_USE_HPT                CDRV_HPT_OFF
#endif

#ifndef CDRV_IMPLEMENT_RTR 
    #define CDRV_IMPLEMENT_RTR          TRUE
#endif

#ifndef CDRVWIN_USE_EVENT_CALLBACK
    #if (CDRV_MAX_INSTANCES > 1)
        #define CDRVWIN_USE_EVENT_CALLBACK  FALSE
    #else
        #define CDRVWIN_USE_EVENT_CALLBACK  TRUE
    #endif
#endif


//---------------------------------------------------------------------------
// macros
//---------------------------------------------------------------------------

#define CDRV_SELECT_CHANNEL(a)          (((WORD) 1)<<(a))
#define CDRV_SELECT_CHANNEL32(a)        (((DWORD)1)<<(a))


#if ((CDRV_USE_HPT & CDRV_HPT_MEASURE) == 0)
    #define CDRV_START_MEASUREMENT()
    #define CDRV_STOPP_MEASUREMENT()
#else
    #define CDRV_START_MEASUREMENT()    TgtHptStartMeasure()
    #define CDRV_STOPP_MEASUREMENT()    TgtHptStoppMeasure()
#endif

#if ((CDRV_USE_HPT & CDRV_HPT_TYPEMASK) == CDRV_HPT_PRODUCER)
    #define CDRV_CHECK_TX_MSG(a)        if ((a) != NULL) { \
                                            HptCheckCanMsg (MCO_INSTANCE_PARAM_IDX_() (a)); \
                                            CDRV_SAVE_LAST_TX (a, NULL); }
    #define CDRV_DEFINE_LAST_TX(a)      a;
    #define CDRV_SAVE_LAST_TX(a,b)      a = (b)
    #define CDRV_COPY_LAST_TX(a,b)      COP_MEMCPY (a,b,sizeof(tCdrvMsg)); b = a
#else
    #define CDRV_CHECK_TX_MSG(a)
    #define CDRV_DEFINE_LAST_TX(a)
    #define CDRV_SAVE_LAST_TX(a,b)
    #define CDRV_COPY_LAST_TX(a,b)
#endif

#if ((CDRV_USE_HPT & CDRV_HPT_TYPEMASK) == CDRV_HPT_CONSUMER)
    #define CDRV_CHECK_RX_MSG(a)        HptCheckCanMsg (MCO_INSTANCE_PARAM_IDX_() (a))
#else
    #define CDRV_CHECK_RX_MSG(a)
#endif


//---------------------------------------------------------------------------
// modul global types
//---------------------------------------------------------------------------

// -------------------------------------------------------------------------
typedef enum
{
    kCdrvRxIsr          = 0,
    kCdrvTxIsr          = 1,
    kCdrvErrorIsr       = 2
} tCdrvIsrType;

// -------------------------------------------------------------------------
// constants for baud rate (CiA DS-305)
typedef enum
{
    kBdi1MBaud      = 0, // 1 MBit/sec
    kBdi800kBaud    = 1, // 800 kBit/sec
    kBdi500kBaud    = 2, // 500 kBit/sec
    kBdi250kBaud    = 3, // 250 kBit/sec
    kBdi125kBaud    = 4, // 125 kBit/sec
    kBdi100kBaud    = 5, // 100 kBit/sec
    kBdi50kBaud     = 6, // 50 kBit/sec
    kBdi20kBaud     = 7, // 20 kBit/sec
    kBdi10kBaud     = 8  // 10 kBit/sec

} tCdrvBaudIndex;

// -------------------------------------------------------------------------
// Bits for comparision of message types (refer to tMsgTyp).
typedef BYTE tMsgTypBit;

// message specific bits
#define kMtbRecv        0x01
#define kMtbRtr         0x02

// filter values
#define kMtbFilter      0x03

// additional bits
#define kMtbForce       0x04
#define kMtbFirstMsg    0x20
#define kMtbHighPrio    0x40
#define kMtbExtend      0x80


// -------------------------------------------------------------------------
// These constants define type of CAN message. They are made of constants
// from enum tMsgTypBit. So CAN driver can use these constants directly
// for comparision of message types.
typedef BYTE tMsgTyp;

#define kMsgTypSend         (0                 )
#define kMsgTypRecv         (kMtbRecv          )
#define kMsgTypSendRtrReq   (           kMtbRtr)
#define kMsgTypRecvRtrReq   (kMtbRecv | kMtbRtr)


// -------------------------------------------------------------------------
// constants for reseting buffer and/or status variable with function CdrvReset()
typedef BYTE tCdrvResetCode;

#define kResetTxBuffHigh    0x01
#define kResetTxBuffLow     0x02
#define kResetTxBuffer      (kResetTxBuffHigh | kResetTxBuffLow)    // both tx buffers
#define kResetRxBuffHigh    0x04
#define kResetRxBuffLow     0x08
#define kResetRxBuffer      (kResetRxBuffHigh | kResetRxBuffLow)    // both rx buffers
#define kResetAllBuffer     (kResetTxBuffer   | kResetRxBuffer)     // all rx and tx buffers
#define kResetCanCtrl       0x10
#define kResetAll           (kResetAllBuffer  | kResetCanCtrl)      // reset CAN controller and buffers


// -------------------------------------------------------------------------
// One entry of CAN message buffer consisits of following members:
typedef struct
{
    tCanId   m_CanId;
    tMsgTyp  m_MsgType;
    BYTE     m_bSize;
    BYTE     m_bChannel_p;  // channel for message (assigned from CAN driver)
    BYTE     m_bData[8];

    #if CDRV_TIMESTAMP != FALSE

        tTime    m_Time;        // consists receiving time of message

    #endif // CDRV_TIMESTAMP != FALSE

} tCdrvMsg;

// -------------------------------------------------------------------------
// hardware parameter for PC
typedef struct
{
    WORD m_wPortAddr;   // PC Portadresse
    BYTE m_bIrq;        // PC IRQ

} tPcIoParam;

// -------------------------------------------------------------------------
// hardware parameter for PC and USB
typedef struct
{
    BYTE m_bDeviceNr;

} tPcUsbParam;

// -------------------------------------------------------------------------
// hardware parameter for Linux and Peak CAN hardware
#if ((TARGET_SYSTEM == _LINUX_) && ((TARGET_HARDWARE & TGT_ADD_EMASK_) == TGT_ADD_PEAK))
//#ifdef LINUX

    typedef struct
    {
        BYTE m_bDevNumber;
        pthread_mutex_t*    m_pCcmMutex;
        pthread_cond_t*     m_pRxCond;
    
    } tPCANLinParam;

#endif

// -------------------------------------------------------------------------
// hardware parameter for Windows and Peak CAN hardware
#ifdef WIN32

    #ifdef WINCDRVSER
        typedef struct
        {
            LPCTSTR       m_szComName;
            BYTE          m_bDeviceNo;
            BYTE          m_bThrdPriority;
            HANDLE        m_hCanMsgReceive;
            DWORD         m_dwBufferTimeout; // [ms]
            DWORD         m_dwRepeatTimeout; // [ms]
            BYTE          m_bNoOfRepeats;

        } tWinCdrvSerParam;
    #elif defined (WINCE)
        typedef struct
        {
            BYTE          m_bDeviceNo;
            BYTE          m_bCdrvThrdPriority;
            BYTE          m_bCcmThrdPriority;
            HANDLE        m_hCanMsgReceive;

        } tWindowsCEParam;
    #else
        typedef struct
        {
            tVxDType        m_VxDType;
            BYTE            m_bThrdPriority;
            BYTE            m_bDeviceNr;
            WORD            m_wIOBase;
            BYTE            m_bIRQ;
            HANDLE          m_hCanMsgReceive;
            DWORD           m_dwIpAddress;
            WORD            m_wIPPort;
            DWORD           m_dwReconnectTimeout;
            DWORD           m_dwConnectTimeout;
            DWORD           m_dwDisconnectTimeout;
            BYTE            m_bIpProtocol;

        } tWindowsParam;
    #endif

#endif

// -------------------------------------------------------------------------
// hardware parameter for MC
typedef struct
{
    BYTE HWACC*     m_pbBaseAddr;   // base address of CAN controller

    #if (TARGET_SYSTEM == _PXROS_)
        DWORD    m_bTaskId;      // task-ID of application task
    #endif

} tMcIoParam;
/*
// -------------------------------------------------------------------------
// hardware parameter for MC Fujitsu 543? m.b.
typedef struct
{
    BYTE m_bCanNo; //number of can-controller

} tMcFujitsuIoParam;
/ *
// -------------------------------------------------------------------------
// hardware parameter for Mitsubishi M16C m.b.
typedef struct
{
    BYTE m_bCanNo; //number of can-controller

} tMcM16cIoParam;
*/
/*
// -------------------------------------------------------------------------
// hardware parameter for MC with CAN-Number (e.g. Infineon XC16x)
typedef struct
{
    BYTE m_bCanNo; //number of can-controller

    #ifdef PXROS
        PxTask_t    m_bTaskId; //number of can-controller
    #endif

} tMcXc16xIoParam;
*/
// -------------------------------------------------------------------------
// hardware parameter for Philips LPC 2294
//typedef struct
//{
//    BYTE m_bCanNo; //number of can-controller

//} tMcLpc2294IoParam;


/*
// -------------------------------------------------------------------------
// hardware parameter for Atmel T89C51CC03
typedef struct
{
    BYTE m_bCanNo; //number of can-controller

} tMcCC03IoParam;
* /
// -------------------------------------------------------------------------
// hardware parameter for Motorola Coldfire MCF52xx/MCF54xx (using FlexCAN)
typedef struct
{
    BYTE            m_bCanNo;   // number of can-controller

} tMcMCF5xParam;
* /
// -------------------------------------------------------------------------
// parameter for HighTec operation system PXROS
#ifdef PXROS
typedef struct
{
    BYTE HWACC* m_pbBaseAddr;
    PxTask_t    m_bTaskId;      // task-ID of application task

} tPXROSParam;
#endif
/ *
// -------------------------------------------------------------------------
// parameter for Beck-Chip 1X3 RTOS CAN API
typedef struct
{
    BYTE m_bCanNo;          //number of can-controller
                            // 0 or 1
    WORD m_wRxBufferSize;   // number of entries in the RX buffer
                            // 0 to 500 possible
    WORD m_wTxBufferSize;   // number of entries in the TX Buffer  
                            // 0 to 500 possible      

} tRTOSBeck1X3Param;
*/

typedef struct
{
    BYTE            m_bCanNo;
    
    #if (TARGET_SYSTEM == _PXROS_)
        PxTask_t    m_bTaskId;  // task-ID of application task
    #endif

} tNonZeroBasedCanNo;

typedef struct
{
    BYTE            m_bCanNo;

    #if (TARGET_SYSTEM == _PXROS_)
        PxTask_t    m_bTaskId;  // task-ID of application task
    #endif

    #if (DEV_SYSTEM == _DEV_PAR_BECK1X3_)
        WORD m_wRxBufferSize;   // number of entries in the RX buffer of CAN device driver
        WORD m_wTxBufferSize;   // number of entries in the TX Buffer of CAN device driver 
                                // (range 0 to 500)
    #endif

} tZeroBasedCanNo;

#define tPXROSParam         tMcIoParam
#define tMcCC03IoParam      tZeroBasedCanNo
#define tMcXc16xIoParam     tZeroBasedCanNo
#define tMcM16cIoParam      tZeroBasedCanNo
#define tMcMCF5xParam       tZeroBasedCanNo
#define tRTOSBeck1X3Param   tZeroBasedCanNo
#define tMcLpc2294IoParam   tNonZeroBasedCanNo
#define tMcFujitsuIoParam   tNonZeroBasedCanNo


// -------------------------------------------------------------------------
// union with hardware parameters
typedef union
{
    #if (TARGET_SYSTEM == _WIN16_) || (TARGET_SYSTEM == _WIN32_) || (TARGET_SYSTEM == _LINUX_) || (TARGET_SYSTEM == _DOS_)
        tPcIoParam              m_PcIoParam;        // PC I/O Parameter (Dongle o. pcNetCAN)
    #endif
    
    #if (TARGET_SYSTEM == _WIN32_) || (TARGET_SYSTEM == _LINUX_)
        tPcUsbParam             m_PcUsbParam;       // PC USB Parameter (USB-CANmodul)
    #endif

    #if ((TARGET_SYSTEM == _LINUX_) && ((TARGET_HARDWARE & TGT_ADD_EMASK_) == TGT_ADD_PEAK))
        tPCANLinParam           m_PCANLinIoParam;   // PC Linux and Peak CAN hardware
    #endif

    #if (TARGET_SYSTEM == _WIN32_)
        #if defined (WINCDRVSER)
            tWinCdrvSerParam    m_WinCdrvSerParam;  // serial communication
        #elif defined (WINCE)
            tWindowsCEParam     m_WindowsCEIoParam; // NETDCU and WinCE
        #else
            tWindowsParam       m_WindowsIoParam;   // PC Windows and Wrapper.dll
        #endif
    #endif

    tMcIoParam          m_McIoParam;                // MC I/O Parameter (all microcontrollers)
    tMcIoParam          m_PXROSParam;               // MC I/O Parameter (all microcontrollers using PXROS)
    tNonZeroBasedCanNo  m_McIoFujitsuParam;         // MC I/O Parameter (Fujitsu Microcontroller using CAN-Nr.)
    tNonZeroBasedCanNo  m_McIoM16cParam;            // MC I/O Parameter (XC16x Microcontroller using CAN-Nr.)
    tNonZeroBasedCanNo  m_McIoLpc2294Param;         // MC I/O Parameter (Philips LPC 2294 using CAN-Nr.)
    tZeroBasedCanNo     m_McIoXc16xParam;           // MC I/O Parameter (XC16x Microcontroller using CAN-Nr.)
    tZeroBasedCanNo     m_McIoCC03Param;            // MC I/O Parameter (Atmel T89C51CC01 using CAN-Nr.)
    tZeroBasedCanNo     m_McMCF5xParam;             // MC I/O Parameter (MCF52xx/MCF54xx using CAN-Nr.)
    tZeroBasedCanNo     m_RTOSBeck1X3Param;         // Paramter for Beck-Chip 1X3 with RTOS using CAN-Nr.
    
    tNonZeroBasedCanNo  m_NonZeroBasedCanNo;        // generic member for all CAN drivers which needs a non zero based CAN-Nr.
    tZeroBasedCanNo     m_ZeroBasedCanNo;           // generic member for all CAN drivers which needs a zero based CAN-Nr.
    
} tCdrvHwParam;

// -------------------------------------------------------------------------
typedef DWORD tCdrvLinFlags;
#define kCdrvLinBlockingMode    0x00000001L // open CAN device in blocking mode
#define kCdrvLinInodeInVarTree  0x00000002L // inode for CAN device in /var tree
#define kCdrvLinRWMsgViaIoctrl  0x80000000L // use <ioctrl> for ReadMsg/WriteMsg

// -------------------------------------------------------------------------
// Struct tCdrvBuffInit defines parameters for initializing buffers.
typedef struct
{
    tCdrvMsg MEM*   m_pBufferBase;  // base address of buffer
    tCdrvBuffIndex  m_MaxEntries;   // count of enties in this buffer

} tCdrvBuffInit;

// -------------------------------------------------------------------------
// This structure consits all internaly using informations for
// a buffer.
typedef struct
{
    tCdrvMsg MEM*   m_pReadPtr;
    tCdrvMsg MEM*   m_pWritePtr;
    tCdrvBuffIndex  m_ReadIndex;
    tCdrvBuffIndex  m_WriteIndex;
    tCdrvBuffIndex  m_Count;

} tCdrvBuff;

// -------------------------------------------------------------------------
typedef void (PUBLIC ROM *tCdrvEnableInt) (BYTE bEnable_p);

// -------------------------------------------------------------------------
// struct for initialisation of CAN driver instance
typedef struct
{
    tCdrvBaudIndex  m_BaudIndex;    // Baudrate als Index
// -rs: changed for BDI tables with variable sizes
//  CONST WORD ROM* m_pBdiTable;    // address of Bdi-table
    CONST void ROM* m_pBdiTable;    // address of Bdi-table
    WORD            m_wSizeOfBdiTab;
    DWORD           m_dwAmr;        // Akzeptanzfilterung
    DWORD           m_dwAcr;
    tCdrvHwParam    m_HwParam;      // treiber- und hardwarespezifische
                                    // Parmeter
#ifdef LINUX_SYSTEM
    BYTE            m_bDevNumber;   // device number for selecting CAN controller
    char*           m_pszDevName;   // optional: device name (e.g. "/dev/can0")
    tCdrvLinFlags   m_LinDevFlags;  // Linux specific device flags
#endif

#if (CDRV_USE_HIGHBUFF != FALSE)

    tCdrvBuffInit   m_RxBuffHigh;   // Info für hochpr. Rx Puffer
    tCdrvBuffInit   m_TxBuffHigh;   // Info für hochpr. Tx Puffer

#endif

    tCdrvBuffInit   m_RxBuffLow;    // Info für niederpr. Rx Puffer
    tCdrvBuffInit   m_TxBuffLow;    // Info für niederpr. Tx Puffer

    tCdrvEnableInt  m_fpEnableInterrupt;

} tCdrvInitParam;

// -------------------------------------------------------------------------
// constants for CAN driver status
typedef WORD tCdrvStatus;

#define kCdrvNoError            0x0000
#define kCdrvWarningLimitSet    0x0001
#define kCdrvWarningLimitReset  0x0002
#define kCdrvErrorPassiveSet    0x0004
#define kCdrvErrorPassiveReset  0x0008
#define kCdrvBusOff             0x0010
#define kCdrvOverrun            0x0020
#define kCdrvStuffError         0x0040
#define kCdrvFormError          0x0080
#define kCdrvAckError           0x0100
#define kCdrvCrcError           0x0200
#define kCdrvRxBuffHighOverrun  0x0400
//#define kCdrvTxBuffHighOverrun  0x0800  // obsolete
#define kCdrvRxBuffLowOverrun   0x1000
//#define kCdrvTxBuffLowOverrun   0x2000  // obsolete
#define kCdrvBusOffReset        0x4000
#define kCdrvFirstBusContact    0x8000

// -------------------------------------------------------------------------
// struct for diagnostic informations of CAN driver
// constants for reseting buffer and/or status variable with function CdrvReset()
typedef BYTE tCdrvDumpFlags;
#define kCdrvDumpAll            0x0000  // dump all CAN Controller Registers
#define kCdrvDumpIgnoreRdOnce   0x0001  // ignore all read-once registers

typedef struct
{
    WORD            m_wSize;            // IN:  sizeof(tCdrvDiagnostic)

#if (CDRV_USE_HIGHBUFF != FALSE)

    tCdrvBuff       m_RxBuffHigh;       // OUT: Rx buffer high priority
    tCdrvBuff       m_TxBuffHigh;       // OUT: Tx buffer high priority

#endif

    tCdrvBuff       m_RxBuffLow;        // OUT: Rx buffer low priority
    tCdrvBuff       m_TxBuffLow;        // OUT: Tx buffer low priority

    void MEM*       m_pDumpBuff;        // IN:  buffer addr for CAN Controller dump
    WORD            m_wDumpStart;       // IN:  start offset for dump within CAN controller
    WORD            m_wDumpSize;        // IN:  # of bytes to write in dump buffer
    tCdrvDumpFlags  m_DumpFlags;        // IN:  flags for dump control
    BYTE HWACC*     m_pbBaseAddr;       // OUT: base address of CAN controller

    // !!! newer entries always has to be set after the last entry !!!

} tCdrvDiagnostic;

// -------------------------------------------------------------------------
// This struct is an infostruct wich is used to get CAN frame information before
// reading the whole CAN message. So CAN driver can check if CAN message should be
// took in buffer before copying it.
typedef struct
{
    BYTE            m_bFrameFormat; // flags RTR and EXTENDED CAN frame
    tCanId          m_CanId;

} tCanFrameInfo;

// -------------------------------------------------------------------------
typedef struct
{
    BYTE            m_bFrameFormat; // flags RTR and EXTENDED CAN frame
    tCanId          m_CanId;
    BYTE            m_bChannel;

} tFullCanFrameInfo;

// -------------------------------------------------------------------------
typedef struct
{
    BYTE            m_bFrameFormat; // flags RTR and EXTENDED CAN frame
    tCanId          m_CanId;
    WORD            m_wChannel;

} tFullCan32FrameInfo;


typedef tCopKernel (PUBLIC ROM *tCdrvInit)             (MCO_DECL_PTR_INSTANCE_PTR_ tCdrvInitParam MEM* pInitParam_p);
typedef tCopKernel (PUBLIC ROM *tCdrvAddInstance)      (MCO_DECL_PTR_INSTANCE_PTR_ tCdrvInitParam MEM* pInitParam_p);
typedef tCopKernel (PUBLIC ROM *tCdrvDeleteInstance)   (MCO_DECL_INSTANCE_PTR);
typedef tCopKernel (PUBLIC ROM *tCdrvSetBaudrate)      (MCO_DECL_INSTANCE_PTR_ CONST void ROM* pBdiTable_p, WORD wSizeOfBdiTab_p, BYTE bBdiIndex_p);
//typedef tCopKernel (PUBLIC ROM *tCdrvSetBaudrate)      (MCO_DECL_INSTANCE_PTR_ CONST WORD ROM* pBdiTable_p, BYTE bBdiIndex_p);
typedef tCopKernel (PUBLIC ROM *tCdrvReset)            (MCO_DECL_INSTANCE_PTR_ tCdrvResetCode ResetCode_p);
typedef tCopKernel (PUBLIC ROM *tCdrvClearRtr)         (MCO_DECL_INSTANCE_PTR_ BYTE bChannel_p);
typedef tCopKernel (PUBLIC ROM *tCdrvDefineCanId)      (MCO_DECL_INSTANCE_PTR_ tCdrvMsg MEM* pCanMsg_p);
typedef tCopKernel (PUBLIC ROM *tCdrvUndefineCanId)    (MCO_DECL_INSTANCE_PTR_ tCdrvMsg MEM* pCanMsg_p);
typedef tCopKernel (PUBLIC ROM *tCdrvWriteMsg)         (MCO_DECL_INSTANCE_PTR_ tCdrvMsg MEM* pCanMsg_p);
typedef tCopKernel (PUBLIC ROM *tCdrvReadMsg)          (MCO_DECL_INSTANCE_PTR_ tCdrvMsg MEM* MEM* ppCanMsg_p);
typedef tCopKernel (PUBLIC ROM *tCdrvReleaseMsg)       (MCO_DECL_INSTANCE_PTR_ tCdrvMsg MEM* pCanMsg_p);
typedef tCopKernel (PUBLIC ROM *tCdrvUpdateMsg)        (MCO_DECL_INSTANCE_PTR_ tCdrvMsg MEM* pCanMsg_p);
typedef tCopKernel (PUBLIC ROM *tCdrvInterruptHandler) (MCO_DECL_INSTANCE_PTR_ tCdrvIsrType IsrType_p);
typedef tCopKernel (PUBLIC ROM *tCdrvGetStatus)        (MCO_DECL_INSTANCE_PTR_ tCdrvStatus MEM* pStatus_p);
typedef tCopKernel (PUBLIC ROM *tCdrvResetStatus)      (MCO_DECL_INSTANCE_PTR_ tCdrvStatus ResetStatus_p);
typedef tCopKernel (PUBLIC ROM *tCdrvGetDiagnostic)    (MCO_DECL_INSTANCE_PTR_ tCdrvDiagnostic MEM* pDiagnostic_p);


//---------------------------------------------------------------------------
// global vars
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// global function prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// Function:    CdrvInit()
//
// Description: 
//
// Parameters:  pInitParam_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvInit (MCO_DECL_PTR_INSTANCE_PTR_
    tCdrvInitParam MEM* pInitParam_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvAddInstance()
//
// Description: 
//
// Parameters:  pInitParam_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvAddInstance (MCO_DECL_PTR_INSTANCE_PTR_
    tCdrvInitParam MEM* pInitParam_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvDeleteInstance()
//
// Description: 
//
// Parameters:  MCO_DECL_INSTANCE_PTR
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvDeleteInstance (MCO_DECL_INSTANCE_PTR);


//---------------------------------------------------------------------------
//
// Function:    CdrvSetBaudrate()
//
// Description: 
//
// Parameters:  pBdiTable_p
//              wSizeOfBdiTab_p
//              bBdiIndex_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvSetBaudrate (MCO_DECL_INSTANCE_PTR_
    CONST void ROM* pBdiTable_p, WORD wSizeOfBdiTab_p, BYTE bBdiIndex_p);
//  CONST WORD ROM* pBdiTable_p, BYTE bBdiIndex_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvReset()
//
// Description: 
//
// Parameters:  ResetCode_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvReset (MCO_DECL_INSTANCE_PTR_
    tCdrvResetCode ResetCode_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvClearRtr()
//
// Description: 
//
// Parameters:  bChannel_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvClearRtr (MCO_DECL_INSTANCE_PTR_
    BYTE bChannel_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvDefineCanId()
//
// Description: 
//
// Parameters:  pCanMsg_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvDefineCanId (MCO_DECL_INSTANCE_PTR_
    tCdrvMsg MEM* pCanMsg_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvUndefineCanId()
//
// Description: 
//
// Parameters:  pCanMsg_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvUndefineCanId (MCO_DECL_INSTANCE_PTR_
    tCdrvMsg MEM* pCanMsg_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvWriteMsg()
//
// Description: 
//
// Parameters:  pCanMsg_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvWriteMsg (MCO_DECL_INSTANCE_PTR_
    tCdrvMsg MEM* pCanMsg_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvReadMsg()
//
// Description: 
//
// Parameters:  ppCanMsg_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvReadMsg (MCO_DECL_INSTANCE_PTR_
    tCdrvMsg MEM* MEM* ppCanMsg_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvReleaseMsg()
//
// Description: 
//
// Parameters:  pCanMsg_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvReleaseMsg (MCO_DECL_INSTANCE_PTR_
    tCdrvMsg MEM* pCanMsg_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvUpdateMsg()
//
// Description: 
//
// Parameters:  pCanMsg_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvUpdateMsg (MCO_DECL_INSTANCE_PTR_
    tCdrvMsg MEM* pCanMsg_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvInterruptHandler()
//
// Description: 
//
// Parameters:  IsrType_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvInterruptHandler (MCO_DECL_INSTANCE_PTR_
    tCdrvIsrType IsrType_p) CDRV_REENTRANT;


//---------------------------------------------------------------------------
//
// Function:    CdrvGetStatus()
//
// Description: 
//
// Parameters:  pStatus_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvGetStatus (MCO_DECL_INSTANCE_PTR_
    tCdrvStatus MEM* pStatus_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvResetStatus()
//
// Description: 
//
// Parameters:  ResetStatus_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvResetStatus (MCO_DECL_INSTANCE_PTR_
    tCdrvStatus ResetStatus_p);


//---------------------------------------------------------------------------
//
// Function:    CdrvGetDiagnostic()
//
// Description: 
//
// Parameters:  pDiagnostic_p
//
// Return:      tCopKernel
//
//---------------------------------------------------------------------------

CDRVDLLEXPORT tCopKernel PUBLIC CdrvGetDiagnostic (MCO_DECL_INSTANCE_PTR_
    tCdrvDiagnostic MEM* pDiagnostic_p);


// ============================================================================
// common debug macros
// ============================================================================

#define CDRV_DBGLVL_CDRV                DEBUG_LVL_01
#define CDRV_DBGLVL_CDRV_TRACE0         DEBUG_LVL_01_TRACE0
#define CDRV_DBGLVL_CDRV_TRACE1         DEBUG_LVL_01_TRACE1
#define CDRV_DBGLVL_CDRV_TRACE2         DEBUG_LVL_01_TRACE2
#define CDRV_DBGLVL_CDRV_TRACE3         DEBUG_LVL_01_TRACE3
#define CDRV_DBGLVL_CDRV_TRACE4         DEBUG_LVL_01_TRACE4

#define CDRV_DBGLVL_KERNEL              DEBUG_LVL_KERNEL        // 0x10000000L
#define CDRV_DBGLVL_KERNEL_TRACE0       DEBUG_LVL_KERNEL_TRACE0
#define CDRV_DBGLVL_KERNEL_TRACE1       DEBUG_LVL_KERNEL_TRACE1
#define CDRV_DBGLVL_KERNEL_TRACE2       DEBUG_LVL_KERNEL_TRACE2
#define CDRV_DBGLVL_KERNEL_TRACE3       DEBUG_LVL_KERNEL_TRACE3
#define CDRV_DBGLVL_KERNEL_TRACE4       DEBUG_LVL_KERNEL_TRACE4

#define CDRV_DBGLVL_ASSERT              DEBUG_LVL_ASSERT
#define CDRV_DBGLVL_ASSERT_TRACE0       DEBUG_LVL_ASSERT_TRACE0
#define CDRV_DBGLVL_ASSERT_TRACE1       DEBUG_LVL_ASSERT_TRACE1
#define CDRV_DBGLVL_ASSERT_TRACE2       DEBUG_LVL_ASSERT_TRACE2
#define CDRV_DBGLVL_ASSERT_TRACE3       DEBUG_LVL_ASSERT_TRACE3
#define CDRV_DBGLVL_ASSERT_TRACE4       DEBUG_LVL_ASSERT_TRACE4

#define CDRV_DBGLVL_ERROR               DEBUG_LVL_ERROR
#define CDRV_DBGLVL_ERROR_TRACE0        DEBUG_LVL_ERROR_TRACE0
#define CDRV_DBGLVL_ERROR_TRACE1        DEBUG_LVL_ERROR_TRACE1
#define CDRV_DBGLVL_ERROR_TRACE2        DEBUG_LVL_ERROR_TRACE2
#define CDRV_DBGLVL_ERROR_TRACE3        DEBUG_LVL_ERROR_TRACE3
#define CDRV_DBGLVL_ERROR_TRACE4        DEBUG_LVL_ERROR_TRACE4

#define CDRV_DBGLVL_ALWAYS              DEBUG_LVL_ALWAYS
#define CDRV_DBGLVL_ALWAYS_TRACE0       DEBUG_LVL_ALWAYS_TRACE0
#define CDRV_DBGLVL_ALWAYS_TRACE1       DEBUG_LVL_ALWAYS_TRACE1
#define CDRV_DBGLVL_ALWAYS_TRACE2       DEBUG_LVL_ALWAYS_TRACE2
#define CDRV_DBGLVL_ALWAYS_TRACE3       DEBUG_LVL_ALWAYS_TRACE3
#define CDRV_DBGLVL_ALWAYS_TRACE4       DEBUG_LVL_ALWAYS_TRACE4


// ============================================================================
// spetial debug macros (combinations)
// ============================================================================

// CAN driver errors:
#define CDRV_DBGLVL_CDRVERROR           (DEBUG_LVL_ERROR | CDRV_DBGLVL_CDRV)
#if ((DEBUG_GLB_LVL() & CDRV_DBGLVL_CDRVERROR) == CDRV_DBGLVL_CDRVERROR)
    #define CDRV_DBGLVL_CDRVERROR_TRACE0(str)               TRACE0(str)
    #define CDRV_DBGLVL_CDRVERROR_TRACE1(str,p1)            TRACE1(str,p1)
    #define CDRV_DBGLVL_CDRVERROR_TRACE2(str,p1,p2)         TRACE2(str,p1,p2)
    #define CDRV_DBGLVL_CDRVERROR_TRACE3(str,p1,p2,p3)      TRACE3(str,p1,p2,p3)
    #define CDRV_DBGLVL_CDRVERROR_TRACE4(str,p1,p2,p3,p4)   TRACE4(str,p1,p2,p3,p4)
#else
    #define CDRV_DBGLVL_CDRVERROR_TRACE0(str)
    #define CDRV_DBGLVL_CDRVERROR_TRACE1(str,p1)
    #define CDRV_DBGLVL_CDRVERROR_TRACE2(str,p1,p2)
    #define CDRV_DBGLVL_CDRVERROR_TRACE3(str,p1,p2,p3)
    #define CDRV_DBGLVL_CDRVERROR_TRACE4(str,p1,p2,p3,p4)
#endif

// kernel mode CAN driver:
#define CDRV_DBGLVL_KERNCDRV            (DEBUG_LVL_KERNEL | CDRV_DBGLVL_CDRV)
#if ((DEBUG_GLB_LVL() & CDRV_DBGLVL_KERNCDRV) == CDRV_DBGLVL_KERNCDRV)
    #define CDRV_DBGLVL_KERNCDRV_TRACE0(str)                TRACE0(str)
    #define CDRV_DBGLVL_KERNCDRV_TRACE1(str,p1)             TRACE1(str,p1)
    #define CDRV_DBGLVL_KERNCDRV_TRACE2(str,p1,p2)          TRACE2(str,p1,p2)
    #define CDRV_DBGLVL_KERNCDRV_TRACE3(str,p1,p2,p3)       TRACE3(str,p1,p2,p3)
    #define CDRV_DBGLVL_KERNCDRV_TRACE4(str,p1,p2,p3,p4)    TRACE4(str,p1,p2,p3,p4)
#else
    #define CDRV_DBGLVL_KERNCDRV_TRACE0(str)
    #define CDRV_DBGLVL_KERNCDRV_TRACE1(str,p1)
    #define CDRV_DBGLVL_KERNCDRV_TRACE2(str,p1,p2)
    #define CDRV_DBGLVL_KERNCDRV_TRACE3(str,p1,p2,p3)
    #define CDRV_DBGLVL_KERNCDRV_TRACE4(str,p1,p2,p3,p4)
#endif

// kernel mode CAN driver errors:
#define CDRV_DBGLVL_KERNCDRVERR           (DEBUG_LVL_KERNEL | DEBUG_LVL_ERROR /* | CDRV_DBGLVL_CDRV */ )
#if ((DEBUG_GLB_LVL() & CDRV_DBGLVL_KERNCDRVERR) == CDRV_DBGLVL_KERNCDRVERR)
    #define CDRV_DBGLVL_KERNCDRVERR_TRACE0(str)             TRACE0(str)
    #define CDRV_DBGLVL_KERNCDRVERR_TRACE1(str,p1)          TRACE1(str,p1)
    #define CDRV_DBGLVL_KERNCDRVERR_TRACE2(str,p1,p2)       TRACE2(str,p1,p2)
    #define CDRV_DBGLVL_KERNCDRVERR_TRACE3(str,p1,p2,p3)    TRACE3(str,p1,p2,p3)
    #define CDRV_DBGLVL_KERNCDRVERR_TRACE4(str,p1,p2,p3,p4) TRACE4(str,p1,p2,p3,p4)
#else
    #define CDRV_DBGLVL_KERNCDRVERR_TRACE0(str)
    #define CDRV_DBGLVL_KERNCDRVERR_TRACE1(str,p1)
    #define CDRV_DBGLVL_KERNCDRVERR_TRACE2(str,p1,p2)
    #define CDRV_DBGLVL_KERNCDRVERR_TRACE3(str,p1,p2,p3)
    #define CDRV_DBGLVL_KERNCDRVERR_TRACE4(str,p1,p2,p3,p4)
#endif


#endif //_CDRV_H_

// Please keep an empty line at the end of this file.
