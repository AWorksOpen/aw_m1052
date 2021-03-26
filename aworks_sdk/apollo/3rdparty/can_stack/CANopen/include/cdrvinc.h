/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5 / CAN driver

  Description:  additional includes for CAN driver

  -------------------------------------------------------------------------

                $RCSfile: CdrvInc.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.22 $  $Date: 2007/01/08 13:56:54 $
                
                $State: Exp $
                
                Build Environment:
                    all

  -------------------------------------------------------------------------

  Revision History:

  2002/XX/XX r.d.:  start of implementation
  2004/08/31 f.j.: cdrvid.h nur einbinden wenn #if (CDRV_USE_IDVALID != FALSE)

****************************************************************************/

#ifndef _CDRVINC_H_
#define _CDRVINC_H_


// =========================================================================
// additional includes for CAN driver
// =========================================================================

#ifdef WIN32

    #ifndef __BORLANDC__
        // MSVC needs to include windows.h at first
        // the following defines ar necceray for function prototypes for waitable timers
        #define _WIN32_WINDOWS 0x0401
        #define _WIN32_WINNT   0x0400
    #endif
    
    #include <windows.h>

#endif

#ifdef PXROS

    // PXROS include for MPC5x5 only
    #include "pxdef.h"

#endif

#ifdef _C166 // Tasking C166_ST10 compiler

    // tasking wishes to include this headers at first
    #include <stdio.h>
    #include <assert.h>

#endif

#include "global_cop.h"     // global definitions

// additional defines for Paradigm C compiler for Beck-Chip 1x3
#if (DEV_SYSTEM == _DEV_PAR_BECK1X3_)

    // Paradigm Beck IPC Edition compiler defines the __ADDR24__ symbol
    // when the -Y command line option ("Extended address space") is used.
    #ifdef __ADDR24__
         #ifndef SC123
            #define  SC123
        #endif
    #endif
#endif

#if (DEV_SYSTEM == _DEV_WIN32_)
    #pragma pack(push,1)    // structure with 1 byte alignment
#elif (DEV_SYSTEM == _DEV_GNU_MPC5X5_)
    #pragma pack(4)         // structure with 4 Byte Alignment
#endif                      

// definitions for DLL export
#if ((DEV_SYSTEM == _DEV_WIN32_) || (DEV_SYSTEM == _DEV_WIN_CE_)) && defined (COP_LIB)

    #define CDRVDLLEXPORT   __declspec (dllexport)

#else

    #define CDRVDLLEXPORT

#endif

#if defined(AW_COM_CANOPEN_MST)
#include "cop_mst/copcfg.h"     // configuration of CAN driver
#elif defined(AW_COM_CANOPEN_SLV)
#include "cop_slv/copcfg.h"     // configuration of CAN driver
#else
#error "must define AW_COM_CANOPEN_MST or AW_COM_CANOPEN_SLV at project"
#endif
//#include "copcfg.h"     // configuration of CAN driver
#include "instdef.h"    // definition of macros for instances

#include "errordef.h"   // error codes for API funtions

// needed, because Paradigm C used own target.h for Beck-Chip 1x3
#if (DEV_SYSTEM == _DEV_PAR_BECK1X3_)
    #include "../../../../include/target.h"   // target specific functions and definitions
#else
    #include "target_cop.h"     // target specific functions and definitions  
#endif

#include "debug.h"      // debug definitions
#include "cdrvdef.h"
//#include "cdrvtgt.h"    // target specific include file for CAN driver
#include "cdrv.h"       // function interface for CAN driver

#if (CDRV_USE_IDVALID != FALSE) //==TRUE // 06-dec-2004 r.d.: bitte immer auf != FALSE prüfen (siehe style guid)
    #include "cdrvid.h"     // filter interface for CAN messages
#else
    #define CDRV_USE_IDINFO     FALSE
#endif

#include "ami.h"        // interface for advanced memory interface

#if (DEV_SYSTEM == _DEV_WIN32_) || (DEV_SYSTEM == _DEV_WIN_CE_)
    #include "tgtthrd.h"
#endif

#if ((CDRV_USE_HPT & CDRV_HPT_TYPEMASK) != CDRV_HPT_OFF)
    #include "Hpt.h"
#endif

// restore structure alignment
#if (DEV_SYSTEM == _DEV_WIN32_)
    #pragma pack(pop)
#elif (DEV_SYSTEM == _DEV_GNU_MPC5X5_)
    #pragma pack()
#endif


#endif // _CDRVINC_H_

// Please keep an empty line at the end of this file.
