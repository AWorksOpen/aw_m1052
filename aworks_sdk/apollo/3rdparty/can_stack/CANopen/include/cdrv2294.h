/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen

  Description:  interface for CAN driver for Philips LPC 21xx and 22xx

  -------------------------------------------------------------------------

                $RCSfile: Cdrv2294.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.3 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                KEIL ARM / uVision 3

  -------------------------------------------------------------------------

  Revision History:

  2005/01/18 m.b.:   start of implementation

****************************************************************************/
#ifndef _Cdrv2294_H_
#define _Cdrv2294_H_


// =========================================================================
// includes
// =========================================================================

// =========================================================================
// CAN driver specific defines have to be redefined with gerneric defines
// =========================================================================
#define CDRV_AUTO_RTR         FALSE

// check the count of instances
#if (CDRV_MAX_INSTANCES > COP_MAX_INSTANCES)
    #error 'ERROR: to many instances in CAN driver'
#endif

#if (CDRV_MAX_INSTANCES > 4)
    #error 'ERROR: use only 4 instances with CAN driver !'
#endif

#if (CDRV_2294_USED_CAN_CONTROLLER == 0)
    #error 'ERROR: please set define for used CAN-controller !'
#endif

#if (CDRV_MAX_INSTANCES == 1)
    #if ((CDRV_2294_USED_CAN_CONTROLLER != 1) && \
         (CDRV_2294_USED_CAN_CONTROLLER != 2) && \
         (CDRV_2294_USED_CAN_CONTROLLER != 4) && \
         (CDRV_2294_USED_CAN_CONTROLLER != 8))
        #error 'ERROR: please define only one CAN-controller !'
    #endif
#endif

#if (CDRV_MAX_INSTANCES > 1)
    #if ((CDRV_2294_USED_CAN_CONTROLLER == 1) || \
         (CDRV_2294_USED_CAN_CONTROLLER == 2) || \
         (CDRV_2294_USED_CAN_CONTROLLER == 4) || \
         (CDRV_2294_USED_CAN_CONTROLLER == 8))
        #error 'ERROR: please define more then one CAN-controller !'
    #endif
#endif

#if (CDRV_USE_BASIC_CAN != TRUE )
    #error 'ERROR: The FullCAN-Feature is not implemented yet !'
#endif


// =========================================================================
// function names have to be renamed if number of instances of CAN driver
// is not equal to number of instances of CAN open
// =========================================================================

#if (COP_USE_CDRV_FUNCTION_POINTER != FALSE)

    #define CdrvInit                Cdrv2294Init
    #define CdrvAddInstance         Cdrv2294AddInstance
    #define CdrvDeleteInstance      Cdrv2294DeleteInstance
    #define CdrvSetBaudrate         Cdrv2294SetBaudrate
    #define CdrvReset               Cdrv2294Reset
    #define CdrvClearRtr            Cdrv2294ClearRtr
    #define CdrvDefineCanId         Cdrv2294DefineCanId
    #define CdrvUndefineCanId       Cdrv2294UndefineCanId
    #define CdrvWriteMsg            Cdrv2294WriteMsg
    #define CdrvReadMsg             Cdrv2294ReadMsg
    #define CdrvReleaseMsg          Cdrv2294ReleaseMsg
    #define CdrvUpdateMsg           Cdrv2294UpdateMsg
    #define CdrvInterruptHandler    Cdrv2294InterruptHandler
    #define CdrvGetStatus           Cdrv2294GetStatus
    #define CdrvResetStatus         Cdrv2294ResetStatus
    #define CdrvGetDiagnostic       Cdrv2294GetDiagnostic

#endif


// =========================================================================
// this bits are defined for registers of the SJA1000
// =========================================================================

// register adresses ------------------------------------------
// in normal mode
#define CAN_MR          0x00
#define CAN_CMR         0x04
#define CAN_GSR          0x08
//#define CAN_REC         0x0A
//#define CAN_TEC         0x0B
#define CAN_IR          0x0C
//#define CAN_ECC         0x0E
//#define CAN_ALR         0x0F
#define CAN_IER         0x10
#define CAN_BTR         0x14
//#define CAN_BTR0        0x14
//#define CAN_BTR1        0x16
#define CAN_EWL         0x18
#define CAN_SR          0x0C
#define CAN_RFS         0x20
#define CAN_RID         0x24
#define CAN_RDA         0x28
#define CAN_RDB         0x2C
#define CAN_TFS1        0x30
#define CAN_TID1        0x34
#define CAN_TDA1        0x38
#define CAN_TDB1        0x3C
#define CAN_TFS2        0x40
#define CAN_TID2        0x44
#define CAN_TDA2        0x48
#define CAN_TDB2        0x4C
#define CAN_TFS3        0x50
#define CAN_TID3        0x54
#define CAN_TDA3        0x58
#define CAN_TDB3        0x5C


#define CAN_AFMR          0x00

// Base adresses of CAN-controller
#define CAN_ACCEPTANCE_RAM    0xE0008000
#define CAN_ACCEPTANCE_REG    0xE003C000
#define CAN_CENTRAL_REG       0xE0040000
#define CAN_1_REG             0xE0044000
#define CAN_2_REG             0xE0048000
#define CAN_3_REG             0xE004C000
#define CAN_4_REG             0xE0050000



// register bits ------------------------------------------
// CAN_MR
#define CAN_MR_RM       0x00000001 // reset mode
#define CAN_MR_LOM      0x00000002 // listen only mode
#define CAN_MR_STM      0x00000004 // self test mode
#define CAN_MR_TPM      0x00000008 // transmit buffer priority mode
#define CAN_MR_SM       0x00000010 // sleep mode
#define CAN_MR_RPM      0x00000020 // reverse polaritiy mode
//                      0x00000040 reserved
#define CAN_MR_TM       0x00000080 // test mode

// CAN_CMR
#define CAN_CMR_TR      0x00000001 // transmition request
#define CAN_CMR_AT      0x00000002 // abort transmition
#define CAN_CMR_RRB     0x00000004 // release recieve buffer
#define CAN_CMR_CDO     0x00000008 // clear data overrun
#define CAN_CMR_SRR     0x00000010 // self reception request
#define CAN_CMR_STB1    0x00000020 // select transmit buffer 1
#define CAN_CMR_STB2    0x00000040 // select transmit buffer 2
#define CAN_CMR_STB3    0x00000080 // select transmit buffer 3

// CAN_SR
#define CAN_GSR_RBS      0x00000001 // recieve buffer status
#define CAN_GSR_DOS      0x00000002 // data overrun status
#define CAN_GSR_TBS      0x00000004 // transmition buffer status
#define CAN_GSR_TCS      0x00000008 // transmition cmplete status
#define CAN_GSR_RS       0x00000010 // recieve status
#define CAN_GSR_TS       0x00000020 // transmit status
#define CAN_GSR_ES       0x00000040 // error status
#define CAN_GSR_BS       0x00000080 // bus status
#define CAN_GSR_REC_MASK 0x00FF0000 // receive error counter mask
#define CAN_GSR_TEC_MASK 0xFF000000 // transmit error counter mask

// CAN_IR
#define CAN_IR_RI         0x00000001 // resieve interrupt
#define CAN_IR_TI1        0x00000002 // transmit interrupt 1
#define CAN_IR_EI         0x00000004 // error warning interrupt
#define CAN_IR_DOI        0x00000008 // data overrun interrupt
#define CAN_IR_WUI        0x00000010 // wake-up interrupt
#define CAN_IR_EPI        0x00000020 // error passive interrupt
#define CAN_IR_ALI        0x00000040 // arbitration lost interrupt
#define CAN_IR_BEI        0x00000080 // bus error interrupt
#define CAN_IR_IDI        0x00000100 // ID ready interrupt
#define CAN_IR_TI2        0x00000200 // transmit interrupt 2
#define CAN_IR_TI3        0x00000400 // transmit interrupt 3
//                        0x00000800...0x00008000 reserved
#define CAN_IR_ERRBIT0    0x00010000 // error code capture bit 0
#define CAN_IR_ERRBIT1    0x00020000 // error code capture bit 1
#define CAN_IR_ERRBIT2    0x00040000 // error code capture bit 2
#define CAN_IR_ERRBIT3    0x00080000 // error code capture bit 3
#define CAN_IR_ERRBIT4    0x00100000 // error code capture bit 4
#define CAN_IR_ERRDIR     0x00200000 // error direction
#define CAN_IR_ERRC0      0x00400000 // error captured bit 0
#define CAN_IR_ERRC1      0x00800000 // error captured bit 1
#define CAN_IR_ERRC_MASK  0x00C00000 // error captured mask
#define CAN_IR_ALCBIT0    0x01000000 // arbitration lost bit 0
#define CAN_IR_ALCBIT1    0x02000000 // arbitration lost bit 1
#define CAN_IR_ALCBIT2    0x04000000 // arbitration lost bit 2
#define CAN_IR_ALCBIT3    0x08000000 // arbitration lost bit 3
#define CAN_IR_ALCBIT4    0x10000000 // arbitration lost bit 4


// CAN_IER
#define CAN_IER_RIE     0x00000001 // resieve interrupt enable
#define CAN_IER_TIE1    0x00000002 // transmit interrupt 1 enable
#define CAN_IER_EIE     0x00000004 // error warning interrupt enable
#define CAN_IER_DOIE    0x00000008 // data overrun interrupt enable
#define CAN_IER_WUIE    0x00000010 // wake-up interrupt enable
#define CAN_IER_EPIE    0x00000020 // error passive interrupt enable
#define CAN_IER_ALIE    0x00000040 // arbitration lost interrupt enable
#define CAN_IER_BEIE    0x00000080 // bus error interrupt enable
#define CAN_IER_IDIE    0x00000100 // ID ready interrupt enable
#define CAN_IER_TIE2    0x00000200 // transmit interrupt 2 enable
#define CAN_IER_TIE3    0x00000400 // transmit interrupt 3 enable

// CAN_BTR
#define CAN_BTR0_BRP0   0x00000001 // baud rate prescaler 0
#define CAN_BTR0_BRP1   0x00000002 // baud rate prescaler 1
#define CAN_BTR0_BRP2   0x00000004 // baud rate prescaler 2
#define CAN_BTR0_BRP3   0x00000008 // baud rate prescaler 3
#define CAN_BTR0_BRP4   0x00000010 // baud rate prescaler 4
#define CAN_BTR0_BRP5   0x00000020 // baud rate prescaler 5
#define CAN_BTR0_BRP6   0x00000040 // baud rate prescaler 6
#define CAN_BTR0_BRP7   0x00000080 // baud rate prescaler 7
#define CAN_BTR0_BRP8   0x00000100 // baud rate prescaler 8
#define CAN_BTR0_BRP9   0x00000200 // baud rate prescaler 9
#define CAN_BTR0_SJW0   0x00004000 // synchronisation jump width 0
#define CAN_BTR0_SJW1   0x00008000 // synchronisation jump width 1

#define CAN_BTR1_TSEG10 0x00010000 // time segment 1.0
#define CAN_BTR1_TSEG11 0x00020000 // time segment 1.1
#define CAN_BTR1_TSEG12 0x00040000 // time segment 1.2
#define CAN_BTR1_TSEG13 0x00080000 // time segment 1.3
#define CAN_BTR1_TSEG20 0x00100000 // time segment 2.0
#define CAN_BTR1_TSEG21 0x00200000 // time segment 2.1
#define CAN_BTR1_TSEG22 0x00400000 // time segment 2.2
#define CAN_BTR1_SAM    0x00800000 // sampling





// =========================================================================
// additional includes
// =========================================================================

#include "cdrv.h"


// =========================================================================
// macros for access to CAN controller
// =========================================================================

// internal "SJA1000" and LPC 2294
#if (((TARGET_HARDWARE & TGT_ADD_IMASK_) == TGT_ADD_I1000) && ((TARGET_HARDWARE & TGT_CPU_MASK_) == TGT_CPU_ARM7))

    // common access
    #define CCI_WRITE_DW_ACC_RAM(off,val)         (*((volatile DWORD *) ((CAN_ACCEPTANCE_RAM) + (off))) = (DWORD) (val))
    #define CCI_WRITE_DW_ACC_REG(off,val)         (*((volatile DWORD *) ((CAN_ACCEPTANCE_REG) + (off))) = (DWORD) (val))
    #define CCI_WRITE_DW_CENTRAL_REG(off,val)     (*((volatile DWORD *) ((CAN_CENTRAL_REG)    + (off))) = (DWORD) (val))

    

    #if (COP_MAX_INSTANCES > 1) // more than one instance of CANopen
        #define CCI_WRITE_DW_CAN_REG(off,val)      (*((volatile DWORD *) ((MCO_GLB_VAR(m_Intern.m_dwCanReg)) + (off))) = (DWORD) (val))
        #define CCI_READ_DW_CAN_REG(off)           (*((volatile DWORD *) ((MCO_GLB_VAR(m_Intern.m_dwCanReg)) + (off))))
    #else // (COP_MAX_INSTANCES > 1) // more than one instance of CANopen
        #if (CDRV_2294_USED_CAN_CONTROLLER == 1)
            #define CCI_WRITE_DW_CAN_REG(off,val)  (*((volatile DWORD *) ((CAN_1_REG) + (off))) = (DWORD) (val))
            #define CCI_READ_DW_CAN_REG(off)       (*((volatile DWORD *) ((CAN_1_REG) + (off))))
        #elif (CDRV_2294_USED_CAN_CONTROLLER == 2)
            #define CCI_WRITE_DW_CAN_REG(off,val)  (*((volatile DWORD *) ((CAN_2_REG) + (off))) = (DWORD) (val))
            #define CCI_READ_DW_CAN_REG(off)       (*((volatile DWORD *) ((CAN_2_REG) + (off))))
        #elif (CDRV_2294_USED_CAN_CONTROLLER == 4)
            #define CCI_WRITE_DW_CAN_REG(off,val)  (*((volatile DWORD *) ((CAN_3_REG) + (off))) = (DWORD) (val))
            #define CCI_READ_DW_CAN_REG(off)       (*((volatile DWORD *) ((CAN_3_REG) + (off))))
        #elif (CDRV_2294_USED_CAN_CONTROLLER == 8)
            #define CCI_WRITE_DW_CAN_REG(off,val)  (*((volatile DWORD *) ((CAN_4_REG) + (off))) = (DWORD) (val))
            #define CCI_READ_DW_CAN_REG(off)       (*((volatile DWORD *) ((CAN_4_REG) + (off))))
        #endif
    #endif // (COP_MAX_INSTANCES > 1) // more than one instance of CANopen

// ..more to implement ???
#else

    #error 'ERROR: Target was never implemented!'

#endif

#endif //_Cdrv2294_H_

// Please keep an empty line at the end of this file.
