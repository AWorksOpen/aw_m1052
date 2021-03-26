/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  This file includes all files which are used in CANopen internaly

  -------------------------------------------------------------------------

                $RCSfile: CopInc.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.42 $  $Date: 2006/09/19 15:32:21 $
                
                $State: Exp $
                
                Build Environment:
                KEIL uVision 2

  -------------------------------------------------------------------------

  Revision History:

  2004/08/31 f.j.: cdrvid.h nur einbinden wenn #if (CDRV_USE_IDVALID != FALSE)

****************************************************************************/

#ifndef _COPINC_H_
#define _COPINC_H_


// ============================================================================
// includes
// ============================================================================

#include "cop.h"            // defines and includes for application

#if (DEV_SYSTEM == _DEV_WIN32_)
    #pragma pack(push,1)    // structure with 1 byte alignment
#elif (DEV_SYSTEM == _DEV_GNU_MPC5X5_)
    #pragma pack(4)         // structure with 4 byte alignment
#endif                      

#include "cdrvinc.h"        // defines and includes for CAN driver

#if (CDRV_USE_IDVALID != FALSE) //== TRUE) // 06-dec-2004 r.d.: bitte immer auf != FALSE testen (siehe style guid)
    #include "cdrvid.h"
#endif

#include "ami.h"
#include "cob.h"
#include "obd.h"
#include "lss.h"
#include "lsss.h"
#include "lssm.h"
#include "nmt.h"
#include "nmts.h"
#include "nmtm.h"
#include "hbp.h"
#include "hbc.h"
#include "pdo.h"
#include "sdoscomm.h"
#include "sdoc.h"
#include "emcp.h"
#include "emcc.h"
#include "emcyerrc.h"

#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRDO) != 0)
    #include "srdo.h"
#endif

#ifdef WIN32

    #include "tgtthrd.h"

#endif // WIN32


// -------------------------------------------------------------------------
// macros
// -------------------------------------------------------------------------

#define COP_STACK_VERSION(ver,rev,rel)      ((((DWORD)(ver)) & 0xFF)|((((DWORD)(rev))&0xFF)<<8)|(((DWORD)(rel))<<16))
#define COP_OBJ1018_VERSION(ver,rev,rel)    ((((DWORD)(ver))<<16) |(((DWORD)(rev))&0xFFFF))
#define COP_STRING_VERSION(ver,rev,rel)     "V" #ver "." #rev " r" #rel

#include "version.h"


// -------------------------------------------------------------------------
// this stucture will be filled out by ObdInitRam()
// -------------------------------------------------------------------------
// $$$ r.d.: andere Datei dafür finden !!!
typedef struct
{
    // modules which are always awailable

    tCdrvInitParam      m_CdrvInitParam;
    tCobInitParam       m_CobInitParam;
    tObdInitParam       m_ObdInitParam;
    tNmtInitParam       m_NmtInitParam;
    tEmcpInitParam      m_EmcpInitParam;
    tSdosInitParam      m_SdosInitParam;

    // modules which are not always awailable

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSS) != 0)

        tLsssInitParam  m_LsssInitParam;

    #endif

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_LSSM) != 0)

        tLssmInitParam  m_LssmInitParam;

    #endif

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTS) != 0)

        tNmtsInitParam  m_NmtsInitParam;

    #endif

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0)

        tNmtmInitParam  m_NmtmInitParam;

    #endif

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBP) != 0)

        tHbpInitParam   m_HbpInitParam;

    #endif

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBC) != 0)

        tHbcInitParam   m_HbcInitParam;

    #endif

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_EMCC) != 0)

        tEmccInitParam  m_EmccInitParam;

    #endif

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_PDO) != 0)

        tPdoInitParam   m_PdoInitParam;

    #endif

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOC) != 0)

        tSdocInitParam  m_SdocInitParam;

    #endif

    #if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRDO) != 0)

        tSrdoInitParam   m_SrdoInitParam;

    #endif

} tObdRamParam;


// restore structure alignment
#if (DEV_SYSTEM == _DEV_WIN32_)
    #pragma pack(pop)
#elif (DEV_SYSTEM == _DEV_GNU_MPC5X5_)
    #pragma pack()
#endif                      


#endif // _COPINC_H_

// Please keep an empty line at the end of this file.

