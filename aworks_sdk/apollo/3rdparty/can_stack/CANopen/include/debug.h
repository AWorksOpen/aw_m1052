/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      X

  Description:  Debug interface

  -------------------------------------------------------------------------

                $RCSfile: Debug.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.2 $  $Date: 2007/01/08 13:49:45 $

                $State: Exp $

                Build Environment:
                    ...

  -------------------------------------------------------------------------

  Revision History:

****************************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "global_cop.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*          G L O B A L   D E F I N I T I O N S                            */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

//---------------------------------------------------------------------------
// global const defines
//---------------------------------------------------------------------------

// These definitions are important for level-debug traces.
// A macro DEBUG_GLB_LVL() defines the current debug-level using following bis.
// If the corresponding bit is set then trace message will be printed out
// (only if NDEBUG is not defined). The upper debug-levels are reserved for
// the debug-levels ALWAYS, ERROR and ASSERT.
#define DEBUG_LVL_01                    0x00000001
#define DEBUG_LVL_02                    0x00000002
#define DEBUG_LVL_03                    0x00000004
#define DEBUG_LVL_04                    0x00000008
#define DEBUG_LVL_05                    0x00000010
#define DEBUG_LVL_06                    0x00000020
#define DEBUG_LVL_07                    0x00000040
#define DEBUG_LVL_08                    0x00000080
#define DEBUG_LVL_09                    0x00000100
#define DEBUG_LVL_10                    0x00000200
#define DEBUG_LVL_11                    0x00000400
#define DEBUG_LVL_12                    0x00000800
#define DEBUG_LVL_13                    0x00001000
#define DEBUG_LVL_14                    0x00002000
#define DEBUG_LVL_15                    0x00004000
#define DEBUG_LVL_16                    0x00008000
#define DEBUG_LVL_17                    0x00010000
#define DEBUG_LVL_18                    0x00020000
#define DEBUG_LVL_19                    0x00040000
#define DEBUG_LVL_20                    0x00080000
#define DEBUG_LVL_21                    0x00100000
#define DEBUG_LVL_22                    0x00200000
#define DEBUG_LVL_23                    0x00400000
#define DEBUG_LVL_24                    0x00800000
#define DEBUG_LVL_25                    0x01000000
#define DEBUG_LVL_26                    0x02000000
#define DEBUG_LVL_27                    0x04000000
#define DEBUG_LVL_28                    0x08000000
#define DEBUG_LVL_KERNEL                0x10000000
#define DEBUG_LVL_ASSERT                0x20000000
#define DEBUG_LVL_ERROR                 0x40000000
#define DEBUG_LVL_ALWAYS                0x80000000


//---------------------------------------------------------------------------
// global types
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// global vars
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// global function prototypes
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// global macros
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// this macro defines a version string
#define MAKE_VERSION_STRING(product,appname,verstr,author) \
    "�prd?" product ",�app?" appname ",�ver?" verstr ",�dat?" __DATE__ ",�aut?" author


//---------------------------------------------------------------------------
// this macro defines a build info string (e.g. for using in printf())
#define DEBUG_MAKE_BUILD_INFO(prefix,product,prodid,descr,verstr,author) "\n" \
    prefix "***************************************************\n" \
    prefix "Project:   " product ", " prodid                  "\n" \
    prefix "Descript.: " descr                                "\n" \
    prefix "Author:    " author                               "\n" \
    prefix "Date:      " __DATE__                             "\n" \
    prefix "Version:   " verstr                               "\n" \
    prefix "***************************************************\n\n"


//---------------------------------------------------------------------------
// The default debug-level is: ERROR and ALWAYS.
// You can define an other debug-level in project settings.
#ifndef DEF_DEBUG_LVL
    #define DEF_DEBUG_LVL                   (DEBUG_LVL_ALWAYS | DEBUG_LVL_ERROR)
#endif
#ifndef DEBUG_GLB_LVL
    #define DEBUG_GLB_LVL()                 DEF_DEBUG_LVL
#endif


//---------------------------------------------------------------------------
#if (DEV_SYSTEM == _DEV_WIN32_) && defined (TRACE_MSG)

    // For WIN32 the macro DEBUG_TRACE0 can be defined as function call TraceLvl()
    // or as macro TRACE().
    //
    // Here the parameter 'lvl' can be used with more than one
    // debug-level (using OR).
    //
    // Example: DEBUG_TRACE1(DEBUG_LVL_30 | DEBUG_LVL_02, "Hello %d", bCount);

    #define DEBUG_TRACE0(lvl,str)               TraceLvl((lvl),str)
    #define DEBUG_TRACE1(lvl,str,p1)            TraceLvl((lvl),str,p1)
    #define DEBUG_TRACE2(lvl,str,p1,p2)         TraceLvl((lvl),str,p1,p2)
    #define DEBUG_TRACE3(lvl,str,p1,p2,p3)      TraceLvl((lvl),str,p1,p2,p3)
    #define DEBUG_TRACE4(lvl,str,p1,p2,p3,p4)   TraceLvl((lvl),str,p1,p2,p3,p4)
    #define DEBUG_GLB_LVL()                     dwDebugLevel_g

#else

    // At microcontrollers we do reduce the memory usage by deleting DEBUG_TRACE-lines
    // (compiler does delete the lines).
    //
    // Here the parameter 'lvl' can only be used with one debug-level.
    //
    // Example: DEBUG_TRACE1(DEBUG_LVL_ERROR, "error code %d", dwRet);

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_ALWAYS)
        #define DEBUG_LVL_ALWAYS_TRACE0(str)                TRACE0(str)
        #define DEBUG_LVL_ALWAYS_TRACE1(str,p1)             TRACE1(str,p1)
        #define DEBUG_LVL_ALWAYS_TRACE2(str,p1,p2)          TRACE2(str,p1,p2)
        #define DEBUG_LVL_ALWAYS_TRACE3(str,p1,p2,p3)       TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_ALWAYS_TRACE4(str,p1,p2,p3,p4)    TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_ALWAYS_TRACE0(str)
        #define DEBUG_LVL_ALWAYS_TRACE1(str,p1)
        #define DEBUG_LVL_ALWAYS_TRACE2(str,p1,p2)
        #define DEBUG_LVL_ALWAYS_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_ALWAYS_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_ERROR)
        #define DEBUG_LVL_ERROR_TRACE0(str)                 TRACE0(str)
        #define DEBUG_LVL_ERROR_TRACE1(str,p1)              TRACE1(str,p1)
        #define DEBUG_LVL_ERROR_TRACE2(str,p1,p2)           TRACE2(str,p1,p2)
        #define DEBUG_LVL_ERROR_TRACE3(str,p1,p2,p3)        TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_ERROR_TRACE4(str,p1,p2,p3,p4)     TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_ERROR_TRACE0(str)
        #define DEBUG_LVL_ERROR_TRACE1(str,p1)
        #define DEBUG_LVL_ERROR_TRACE2(str,p1,p2)
        #define DEBUG_LVL_ERROR_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_ERROR_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_ASSERT)
        #define DEBUG_LVL_ASSERT_TRACE0(str)                TRACE0(str)
        #define DEBUG_LVL_ASSERT_TRACE1(str,p1)             TRACE1(str,p1)
        #define DEBUG_LVL_ASSERT_TRACE2(str,p1,p2)          TRACE2(str,p1,p2)
        #define DEBUG_LVL_ASSERT_TRACE3(str,p1,p2,p3)       TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_ASSERT_TRACE4(str,p1,p2,p3,p4)    TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_ASSERT_TRACE0(str)
        #define DEBUG_LVL_ASSERT_TRACE1(str,p1)
        #define DEBUG_LVL_ASSERT_TRACE2(str,p1,p2)
        #define DEBUG_LVL_ASSERT_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_ASSERT_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_KERNEL)
        #define DEBUG_LVL_KERNEL_TRACE0(str)                TRACE0(str)
        #define DEBUG_LVL_KERNEL_TRACE1(str,p1)             TRACE1(str,p1)
        #define DEBUG_LVL_KERNEL_TRACE2(str,p1,p2)          TRACE2(str,p1,p2)
        #define DEBUG_LVL_KERNEL_TRACE3(str,p1,p2,p3)       TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_KERNEL_TRACE4(str,p1,p2,p3,p4)    TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_KERNEL_TRACE0(str)
        #define DEBUG_LVL_KERNEL_TRACE1(str,p1)
        #define DEBUG_LVL_KERNEL_TRACE2(str,p1,p2)
        #define DEBUG_LVL_KERNEL_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_KERNEL_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_28)
        #define DEBUG_LVL_28_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_28_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_28_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_28_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_28_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_28_TRACE0(str)
        #define DEBUG_LVL_28_TRACE1(str,p1)
        #define DEBUG_LVL_28_TRACE2(str,p1,p2)
        #define DEBUG_LVL_28_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_28_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_27)
        #define DEBUG_LVL_27_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_27_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_27_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_27_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_27_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_27_TRACE0(str)
        #define DEBUG_LVL_27_TRACE1(str,p1)
        #define DEBUG_LVL_27_TRACE2(str,p1,p2)
        #define DEBUG_LVL_27_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_27_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_26)
        #define DEBUG_LVL_26_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_26_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_26_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_26_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_26_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_26_TRACE0(str)
        #define DEBUG_LVL_26_TRACE1(str,p1)
        #define DEBUG_LVL_26_TRACE2(str,p1,p2)
        #define DEBUG_LVL_26_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_26_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_25)
        #define DEBUG_LVL_25_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_25_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_25_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_25_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_25_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_25_TRACE0(str)
        #define DEBUG_LVL_25_TRACE1(str,p1)
        #define DEBUG_LVL_25_TRACE2(str,p1,p2)
        #define DEBUG_LVL_25_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_25_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_24)
        #define DEBUG_LVL_24_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_24_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_24_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_24_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_24_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_24_TRACE0(str)
        #define DEBUG_LVL_24_TRACE1(str,p1)
        #define DEBUG_LVL_24_TRACE2(str,p1,p2)
        #define DEBUG_LVL_24_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_24_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_23)
        #define DEBUG_LVL_23_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_23_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_23_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_23_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_23_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_23_TRACE0(str)
        #define DEBUG_LVL_23_TRACE1(str,p1)
        #define DEBUG_LVL_23_TRACE2(str,p1,p2)
        #define DEBUG_LVL_23_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_23_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_22)
        #define DEBUG_LVL_22_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_22_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_22_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_22_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_22_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_22_TRACE0(str)
        #define DEBUG_LVL_22_TRACE1(str,p1)
        #define DEBUG_LVL_22_TRACE2(str,p1,p2)
        #define DEBUG_LVL_22_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_22_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_21)
        #define DEBUG_LVL_21_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_21_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_21_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_21_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_21_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_21_TRACE0(str)
        #define DEBUG_LVL_21_TRACE1(str,p1)
        #define DEBUG_LVL_21_TRACE2(str,p1,p2)
        #define DEBUG_LVL_21_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_21_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_20)
        #define DEBUG_LVL_20_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_20_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_20_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_20_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_20_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_20_TRACE0(str)
        #define DEBUG_LVL_20_TRACE1(str,p1)
        #define DEBUG_LVL_20_TRACE2(str,p1,p2)
        #define DEBUG_LVL_20_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_20_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_19)
        #define DEBUG_LVL_19_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_19_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_19_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_19_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_19_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_19_TRACE0(str)
        #define DEBUG_LVL_19_TRACE1(str,p1)
        #define DEBUG_LVL_19_TRACE2(str,p1,p2)
        #define DEBUG_LVL_19_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_19_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_18)
        #define DEBUG_LVL_18_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_18_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_18_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_18_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_18_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_18_TRACE0(str)
        #define DEBUG_LVL_18_TRACE1(str,p1)
        #define DEBUG_LVL_18_TRACE2(str,p1,p2)
        #define DEBUG_LVL_18_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_18_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_17)
        #define DEBUG_LVL_17_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_17_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_17_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_17_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_17_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_17_TRACE0(str)
        #define DEBUG_LVL_17_TRACE1(str,p1)
        #define DEBUG_LVL_17_TRACE2(str,p1,p2)
        #define DEBUG_LVL_17_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_17_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_16)
        #define DEBUG_LVL_16_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_16_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_16_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_16_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_16_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_16_TRACE0(str)
        #define DEBUG_LVL_16_TRACE1(str,p1)
        #define DEBUG_LVL_16_TRACE2(str,p1,p2)
        #define DEBUG_LVL_16_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_16_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_15)
        #define DEBUG_LVL_15_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_15_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_15_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_15_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_15_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_15_TRACE0(str)
        #define DEBUG_LVL_15_TRACE1(str,p1)
        #define DEBUG_LVL_15_TRACE2(str,p1,p2)
        #define DEBUG_LVL_15_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_15_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_14)
        #define DEBUG_LVL_14_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_14_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_14_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_14_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_14_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_14_TRACE0(str)
        #define DEBUG_LVL_14_TRACE1(str,p1)
        #define DEBUG_LVL_14_TRACE2(str,p1,p2)
        #define DEBUG_LVL_14_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_14_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_13)
        #define DEBUG_LVL_13_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_13_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_13_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_13_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_13_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_13_TRACE0(str)
        #define DEBUG_LVL_13_TRACE1(str,p1)
        #define DEBUG_LVL_13_TRACE2(str,p1,p2)
        #define DEBUG_LVL_13_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_13_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_12)
        #define DEBUG_LVL_12_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_12_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_12_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_12_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_12_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_12_TRACE0(str)
        #define DEBUG_LVL_12_TRACE1(str,p1)
        #define DEBUG_LVL_12_TRACE2(str,p1,p2)
        #define DEBUG_LVL_12_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_12_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_11)
        #define DEBUG_LVL_11_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_11_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_11_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_11_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_11_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_11_TRACE0(str)
        #define DEBUG_LVL_11_TRACE1(str,p1)
        #define DEBUG_LVL_11_TRACE2(str,p1,p2)
        #define DEBUG_LVL_11_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_11_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_10)
        #define DEBUG_LVL_10_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_10_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_10_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_10_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_10_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_10_TRACE0(str)
        #define DEBUG_LVL_10_TRACE1(str,p1)
        #define DEBUG_LVL_10_TRACE2(str,p1,p2)
        #define DEBUG_LVL_10_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_10_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_09)
        #define DEBUG_LVL_09_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_09_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_09_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_09_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_09_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_09_TRACE0(str)
        #define DEBUG_LVL_09_TRACE1(str,p1)
        #define DEBUG_LVL_09_TRACE2(str,p1,p2)
        #define DEBUG_LVL_09_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_09_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_08)
        #define DEBUG_LVL_08_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_08_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_08_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_08_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_08_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_08_TRACE0(str)
        #define DEBUG_LVL_08_TRACE1(str,p1)
        #define DEBUG_LVL_08_TRACE2(str,p1,p2)
        #define DEBUG_LVL_08_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_08_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_07)
        #define DEBUG_LVL_07_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_07_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_07_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_07_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_07_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_07_TRACE0(str)
        #define DEBUG_LVL_07_TRACE1(str,p1)
        #define DEBUG_LVL_07_TRACE2(str,p1,p2)
        #define DEBUG_LVL_07_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_07_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_06)
        #define DEBUG_LVL_06_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_06_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_06_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_06_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_06_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_06_TRACE0(str)
        #define DEBUG_LVL_06_TRACE1(str,p1)
        #define DEBUG_LVL_06_TRACE2(str,p1,p2)
        #define DEBUG_LVL_06_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_06_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_05)
        #define DEBUG_LVL_05_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_05_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_05_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_05_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_05_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_05_TRACE0(str)
        #define DEBUG_LVL_05_TRACE1(str,p1)
        #define DEBUG_LVL_05_TRACE2(str,p1,p2)
        #define DEBUG_LVL_05_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_05_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_04)
        #define DEBUG_LVL_04_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_04_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_04_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_04_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_04_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_04_TRACE0(str)
        #define DEBUG_LVL_04_TRACE1(str,p1)
        #define DEBUG_LVL_04_TRACE2(str,p1,p2)
        #define DEBUG_LVL_04_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_04_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_03)
        #define DEBUG_LVL_03_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_03_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_03_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_03_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_03_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_03_TRACE0(str)
        #define DEBUG_LVL_03_TRACE1(str,p1)
        #define DEBUG_LVL_03_TRACE2(str,p1,p2)
        #define DEBUG_LVL_03_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_03_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_02)
        #define DEBUG_LVL_02_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_02_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_02_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_02_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_02_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_02_TRACE0(str)
        #define DEBUG_LVL_02_TRACE1(str,p1)
        #define DEBUG_LVL_02_TRACE2(str,p1,p2)
        #define DEBUG_LVL_02_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_02_TRACE4(str,p1,p2,p3,p4)
    #endif

    #if (DEBUG_GLB_LVL() & DEBUG_LVL_01)
        #define DEBUG_LVL_01_TRACE0(str)                    TRACE0(str)
        #define DEBUG_LVL_01_TRACE1(str,p1)                 TRACE1(str,p1)
        #define DEBUG_LVL_01_TRACE2(str,p1,p2)              TRACE2(str,p1,p2)
        #define DEBUG_LVL_01_TRACE3(str,p1,p2,p3)           TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_01_TRACE4(str,p1,p2,p3,p4)        TRACE4(str,p1,p2,p3,p4)
    #else
        #define DEBUG_LVL_01_TRACE0(str)
        #define DEBUG_LVL_01_TRACE1(str,p1)
        #define DEBUG_LVL_01_TRACE2(str,p1,p2)
        #define DEBUG_LVL_01_TRACE3(str,p1,p2,p3)
        #define DEBUG_LVL_01_TRACE4(str,p1,p2,p3,p4)
    #endif

    #define DEBUG_TRACE0(lvl,str)                           lvl##_TRACE0(str)
    #define DEBUG_TRACE1(lvl,str,p1)                        lvl##_TRACE1(str,p1)
    #define DEBUG_TRACE2(lvl,str,p1,p2)                     lvl##_TRACE2(str,p1,p2)
    #define DEBUG_TRACE3(lvl,str,p1,p2,p3)                  lvl##_TRACE3(str,p1,p2,p3)
    #define DEBUG_TRACE4(lvl,str,p1,p2,p3,p4)               lvl##_TRACE4(str,p1,p2,p3,p4)

#endif


//---------------------------------------------------------------------------
// The macro DEBUG_DUMP_DATA() can be used with the same debug-levels to dump
// out data bytes. Function DumpData() has to be included.
// NOTE: DUMP_DATA has to be defined in project settings.
#if (!defined (NDEBUG) && defined (DUMP_DATA)) || (DEV_SYSTEM == _DEV_WIN32_)

    #ifdef __cplusplus
    extern "C"
    {
    #endif

        void DumpData (char* szStr_p, BYTE MEM* pbData_p, WORD wSize_p);

    #ifdef __cplusplus
    } // von extern "C"
    #endif

    #define DEBUG_DUMP_DATA(lvl,str,ptr,siz)    if ((DEBUG_GLB_LVL() & (lvl))==(lvl)) \
                                                    DumpData (str, (BYTE MEM*) (ptr), (WORD) (siz));

#else

    #define DEBUG_DUMP_DATA(lvl,str,ptr,siz)

#endif


//---------------------------------------------------------------------------
// The macro DEBUG_ASSERT() can be used to print out an error string if the
// parametered expresion does not result TRUE.
// NOTE: If DEBUG_KEEP_ASSERT is defined, then DEBUG_ASSERT-line will not be
//       deleted from compiler (in release version too).
#if !defined (NDEBUG) || defined (DEBUG_KEEP_ASSERT)

    #if (DEV_SYSTEM == _DEV_WIN32_)

        // For WIN32 process will be killed after closing message box.

        #define DEBUG_ASSERT0(expr,str)         if (!(expr ) && ((DEBUG_GLB_LVL() & DEBUG_LVL_ASSERT)!=0)) { \
                                                    MessageBox (NULL, \
                                                        "Assertion failed: line " __LINE__ " file " __FILE__ \
                                                        "\n    -> " str "\n"); \
                                                    ExitProcess (-1); }

        #define DEBUG_ASSERT1(expr,str,p1)      if (!(expr ) && ((DEBUG_GLB_LVL() & DEBUG_LVL_ASSERT)!=0)) { \
                                                    MessageBox (NULL, \
                                                        "Assertion failed: line " __LINE__ " file " __FILE__ \
                                                        "\n    -> " str "\n"); \
                                                    ExitProcess (-1); }

    #else

        // For microcontrollers process will be stopped using endless loop.

        #define DEBUG_ASSERT0(expr,str)         if (!(expr )) { \
                                                    DEBUG_LVL_ASSERT_TRACE3 ( \
                                                        "Assertion failed: line %d file '%s'\n" \
                                                        "    -> '%s'\n", __LINE__, __FILE__, str); \
                                                    while (1); }

        #define DEBUG_ASSERT1(expr,str,p1)      if (!(expr )) { \
                                                    DEBUG_LVL_ASSERT_TRACE4 ( \
                                                        "Assertion failed: line %d file '%s'\n" \
                                                        "    -> '%s'\n" \
                                                        "    -> 0x%08lX\n", __LINE__, __FILE__, str, (DWORD) p1); \
                                                    while (1); }

    #endif

#else

    #define DEBUG_ASSERT0(expr,str)
    #define DEBUG_ASSERT1(expr,str,p1)

#endif


//---------------------------------------------------------------------------
// The macro DEBUG_ONLY() implements code, if NDEBUG is not defined.
#if !defined (DEBUG_ONLY)
    #if !defined (NDEBUG)

        #define DEBUG_ONLY(expr)    expr

    #else

        #define DEBUG_ONLY(expr)

    #endif
#endif


#endif // _DEBUG_H_
