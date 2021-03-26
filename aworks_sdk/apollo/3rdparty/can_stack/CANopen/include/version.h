/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  This file defines the CANopen version for the stack, as string
                and for object 0x1018 within object dictionary.

  -------------------------------------------------------------------------

                $RCSfile: Version.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.32 $  $Date: 2007/01/08 13:42:38 $
                
                $State: Exp $
                
                Build Environment:
                    all

  -------------------------------------------------------------------------

  Revision History:

****************************************************************************/

#ifndef _VERSION_H_
#define _VERSION_H_


// NOTE:
// All version macros should contain the same version number. But does not use
// defines instead of the numbers. Because the macro COP_STRING_VERSION() can not
// convert a define to a string.
//
// Format: maj.min.build
//         maj            = major version
//             min        = minor version (will be set to 0 if major version will be incremented)
//                 build  = current build (will be set to 0 if minor version will be incremented)
//
#define DEFINED_STACK_VERSION       COP_STACK_VERSION   (5, 38, 0)
#define DEFINED_OBJ1018_VERSION     COP_OBJ1018_VERSION (5, 38, 0)
#define DEFINED_STRING_VERSION      COP_STRING_VERSION  (5, 38, 0)


// -----------------------------------------------------------------------------
#define COP_PRODUCT_NAME            "CANopen V5"
#define COP_PRODUCT_VERSION         DEFINED_STRING_VERSION
#define COP_PRODUCT_MANUFACTURER    "SYS TEC electronic GmbH"

#if defined (COP_1131)
    #define COP_PRODUCT_KEY         "SO-874"
    #define COP_PRODUCT_DESCRIPTION "CANopen for IEC61131"
#elif defined (COP_302)
    #define COP_PRODUCT_KEY         "SO-1063"
    #define COP_PRODUCT_DESCRIPTION "CANopen Manager DS-302"
#elif defined (COP_KIT)
    #define COP_PRODUCT_KEY         "KIT-151"
    #define COP_PRODUCT_DESCRIPTION "CANopen Kit Library DS-301"
#elif defined (COP_LIB)
    #define COP_PRODUCT_KEY         "SO-877"
    #define COP_PRODUCT_DESCRIPTION "CANopen Stack Library DS-301"
#else
    #define COP_PRODUCT_KEY         "SO-877"
    #define COP_PRODUCT_DESCRIPTION "CANopen Stack in Source DS-301"
#endif

#endif // _VERSION_H_

// Please keep an empty line at the end of this file.

