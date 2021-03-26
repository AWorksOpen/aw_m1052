/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  NXP Semiconductors USA, Inc.  whose
registered  office  is  situated  at 411 E. Plumeria Drive, San  Jose,
CA 95134, USA  solely for  the  purposes  of  creating  libraries  for
NXPs M0, M3/M4 and  ARM7/9 processor-based  devices,  sublicensed  and
distributed under the terms and conditions of the NXP End User License
Agreement.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIConf.c
Purpose     : Display controller initialization
---------------------------END-OF-HEADER------------------------------
*/

#include "aworks.h"
#include "aw_mem.h"
#include "aw_assert.h"
#include "aw_fb.h"

#include "GUI.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Define the available number of bytes available for the GUI
// emwin 内存空间大小
//
#define GUI_NUMBYTES  (1024 * 1024 * 4)  // 4 MB
//
// Define the average block size
//
#define GUI_BLOCKSIZE 0x80

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

//
// 32 bit aligned memory area
//
#ifdef __ICCARM__
  #pragma location="GUI_RAM"
  static __no_init U32 _aMemory[GUI_NUMBYTES / 4];
#endif
#ifdef __CC_ARM
  U32 static _aMemory[GUI_NUMBYTES / 4] __attribute__ ((section ("GUI_RAM"), zero_init));
#endif
#ifdef __GNUC__
//  U32 static _aMemory[GUI_NUMBYTES / 4] __attribute__ ((section(".GUI_RAM"))) = { 0 };
#endif
#ifdef _WINDOWS
  static U32 _aMemory[GUI_NUMBYTES / 4];
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/
void GUI_X_Config(void) {
    uint8_t *p_data;
  //
  // Assign memory to emWin
  //
    p_data = aw_mem_alloc(GUI_NUMBYTES);
    aw_assert(p_data);

    GUI_ALLOC_AssignMemory(p_data, GUI_NUMBYTES);
    GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);

}

/*************************** End of file ****************************/
