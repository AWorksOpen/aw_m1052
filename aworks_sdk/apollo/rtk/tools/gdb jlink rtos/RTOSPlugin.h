/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (C) 2004-2009    SEGGER Microcontroller Systeme GmbH        *
*                                                                    *
*      Internet: www.segger.com    Support:  support@segger.com      *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : RTOSPlugin.h
Purpose     : RTOS plugin header
---------------------------END-OF-HEADER------------------------------
*/

#ifndef RTOSPLUGIN_H              // Guard against multiple inclusion
#define RTOSPLUGIN_H

#include <string.h>

#include "JLINKARM_Const.h"
#include "TYPES.h"

#if defined(__cplusplus)    // Allow usage of this module from C++ files (disable name mangling)
  extern "C" {
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

//
// RTOS symbols
//
typedef struct {
  const char *name;
  int optional;
  U32 address;
} RTOS_SYMBOLS;

//
// GDB server functions that can be called by the plugin
//
typedef struct {
  void  (*pfFree)        (void* p);
  void* (*pfAlloc)       (unsigned Size);
  void* (*pfRealloc)     (void* p, unsigned Size);

  void  (*pfLogOutf)     (const char* sFormat, ...);
  void  (*pfDebugOutf)   (const char* sFormat, ...);
  void  (*pfWarnOutf)    (const char* sFormat, ...);
  void  (*pfErrorOutf)   (const char* sFormat, ...);

  int   (*pfReadMem)     (U32 Addr, char* pData, unsigned int NumBytes);
  char  (*pfReadU8)      (U32 Addr, U8* pData);
  char  (*pfReadU16)     (U32 Addr, U16* pData);
  char  (*pfReadU32)     (U32 Addr, U32* pData);

  int   (*pfWriteMem)    (U32 Addr, const char* pData, unsigned NumBytes);
  void  (*pfWriteU8)     (U32 Addr, U8 Data);
  void  (*pfWriteU16)    (U32 Addr, U16 Data);
  void  (*pfWriteU32)    (U32 Addr, U32 Data);

  U32   (*pfLoad16TE)    (const U8* p);
  U32   (*pfLoad24TE)    (const U8* p);
  U32   (*pfLoad32TE)    (const U8* p);
} GDB_API;

#if defined(__cplusplus)    // Allow usage of this module from C++ files (disable name mangling)
  }
#endif

#endif                      // Avoid multiple inclusion

/*************************** End of file ****************************/
