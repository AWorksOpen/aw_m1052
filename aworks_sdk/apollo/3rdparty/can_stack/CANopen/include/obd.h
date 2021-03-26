/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  definitions for CANopen OBD module (object dictionary)

  -------------------------------------------------------------------------

                $RCSfile: Obd.h,v $
                
                $Author: R.Dietzsch $
                
                $Revision: 1.69 $  $Date: 2006/09/19 15:32:21 $
                
                $State: Exp $
                
                Build Environment:
                KEIL uVision 2

  -------------------------------------------------------------------------

  Revision History:

  07-dec-2004 r.d.: Defines kObdPart... replaced from obd.h to cop.h.
  09-dec-2004 r.d.: Defines kObdEv... replaced from obd.h to cop.h.

****************************************************************************/

#ifndef _OBD_H_
#define _OBD_H_


// ============================================================================
// defines
// ============================================================================

#define OBD_TABLE_INDEX_END     0xFFFF


// ============================================================================
// enums
// ============================================================================

// directions for access to object dictionary
typedef BYTE tObdDir;

#define kObdDirInit         0x00    // initialising after power on
#define kObdDirStore        0x01    // store all object values to non volatile memory
#define kObdDirLoad         0x02    // load all object values from non volatile memory
#define kObdDirRestore      0x03    // deletes non volatile memory (restore)
#define kObdDirOBKCheck     0xFF    // reserved


// commands for callback function CcmCbStoreLoadObject()
typedef BYTE tObdCommand;

#define kObdCommNothing     0x00
#define kObdCommOpenWrite   0x01
#define kObdCommWriteObj    0x02
#define kObdCommCloseWrite  0x03
#define kObdCommOpenRead    0x04
#define kObdCommReadObj     0x05
#define kObdCommCloseRead   0x06
#define kObdCommClear       0x07
#define kObdCommUnknown     0xFF


//-----------------------------------------------------------------------------------------------------------
// access types for objects
typedef BYTE tObdAccess;

#define kObdAccRead         0x01    // object can be read
#define kObdAccWrite        0x02    // object can be written
#define kObdAccConst        0x04    // object contains a constant value
#define kObdAccPdo          0x08    // object can be mapped in a PDO
#define kObdAccArray        0x10    // object contains an array of numerical values
#define kObdAccRange        0x20    // object contains lower and upper limit
#define kObdAccVar          0x40    // object data is placed in application
#define kObdAccStore        0x80    // object data can be stored to non volatile memory

// combinations (not all combinations are required)
#define kObdAccR            (0            | 0          | 0            | 0          | 0            | 0            | kObdAccRead)
#define kObdAccW            (0            | 0          | 0            | 0          | 0            | kObdAccWrite | 0          )
#define kObdAccRW           (0            | 0          | 0            | 0          | 0            | kObdAccWrite | kObdAccRead)
#define kObdAccCR           (0            | 0          | 0            | 0          | kObdAccConst | 0            | kObdAccRead)
#define kObdAccGR           (0            | 0          | kObdAccRange | 0          | 0            | 0            | kObdAccRead)
#define kObdAccGW           (0            | 0          | kObdAccRange | 0          | 0            | kObdAccWrite | 0          )
#define kObdAccGRW          (0            | 0          | kObdAccRange | 0          | 0            | kObdAccWrite | kObdAccRead)
#define kObdAccVR           (0            | kObdAccVar | 0            | 0          | 0            | 0            | kObdAccRead)
#define kObdAccVW           (0            | kObdAccVar | 0            | 0          | 0            | kObdAccWrite | 0          )
#define kObdAccVRW          (0            | kObdAccVar | 0            | 0          | 0            | kObdAccWrite | kObdAccRead)
#define kObdAccVPR          (0            | kObdAccVar | 0            | kObdAccPdo | 0            | 0            | kObdAccRead)
#define kObdAccVPW          (0            | kObdAccVar | 0            | kObdAccPdo | 0            | kObdAccWrite | 0          )
#define kObdAccVPRW         (0            | kObdAccVar | 0            | kObdAccPdo | 0            | kObdAccWrite | kObdAccRead)
#define kObdAccVGR          (0            | kObdAccVar | kObdAccRange | 0          | 0            | 0            | kObdAccRead)
#define kObdAccVGW          (0            | kObdAccVar | kObdAccRange | 0          | 0            | kObdAccWrite | 0          )
#define kObdAccVGRW         (0            | kObdAccVar | kObdAccRange | 0          | 0            | kObdAccWrite | kObdAccRead)
#define kObdAccVGPR         (0            | kObdAccVar | kObdAccRange | kObdAccPdo | 0            | 0            | kObdAccRead)
#define kObdAccVGPW         (0            | kObdAccVar | kObdAccRange | kObdAccPdo | 0            | kObdAccWrite | 0          )
#define kObdAccVGPRW        (0            | kObdAccVar | kObdAccRange | kObdAccPdo | 0            | kObdAccWrite | kObdAccRead)
#define kObdAccSR           (kObdAccStore | 0          | 0            | 0          | 0            | 0            | kObdAccRead)
#define kObdAccSW           (kObdAccStore | 0          | 0            | 0          | 0            | kObdAccWrite | 0          )
#define kObdAccSRW          (kObdAccStore | 0          | 0            | 0          | 0            | kObdAccWrite | kObdAccRead)
#define kObdAccSCR          (kObdAccStore | 0          | 0            | 0          | kObdAccConst | 0            | kObdAccRead)
#define kObdAccSGR          (kObdAccStore | 0          | kObdAccRange | 0          | 0            | 0            | kObdAccRead)
#define kObdAccSGW          (kObdAccStore | 0          | kObdAccRange | 0          | 0            | kObdAccWrite | 0          )
#define kObdAccSGRW         (kObdAccStore | 0          | kObdAccRange | 0          | 0            | kObdAccWrite | kObdAccRead)
#define kObdAccSVR          (kObdAccStore | kObdAccVar | 0            | 0          | 0            | 0            | kObdAccRead)
#define kObdAccSVW          (kObdAccStore | kObdAccVar | 0            | 0          | 0            | kObdAccWrite | 0          )
#define kObdAccSVRW         (kObdAccStore | kObdAccVar | 0            | 0          | 0            | kObdAccWrite | kObdAccRead)
#define kObdAccSVPR         (kObdAccStore | kObdAccVar | 0            | kObdAccPdo | 0            | 0            | kObdAccRead)
#define kObdAccSVPW         (kObdAccStore | kObdAccVar | 0            | kObdAccPdo | 0            | kObdAccWrite | 0          )
#define kObdAccSVPRW        (kObdAccStore | kObdAccVar | 0            | kObdAccPdo | 0            | kObdAccWrite | kObdAccRead)
#define kObdAccSVGR         (kObdAccStore | kObdAccVar | kObdAccRange | 0          | 0            | 0            | kObdAccRead)
#define kObdAccSVGW         (kObdAccStore | kObdAccVar | kObdAccRange | 0          | 0            | kObdAccWrite | 0          )
#define kObdAccSVGRW        (kObdAccStore | kObdAccVar | kObdAccRange | 0          | 0            | kObdAccWrite | kObdAccRead)
#define kObdAccSVGPR        (kObdAccStore | kObdAccVar | kObdAccRange | kObdAccPdo | 0            | 0            | kObdAccRead)
#define kObdAccSVGPW        (kObdAccStore | kObdAccVar | kObdAccRange | kObdAccPdo | 0            | kObdAccWrite | 0          )
#define kObdAccSVGPRW       (kObdAccStore | kObdAccVar | kObdAccRange | kObdAccPdo | 0            | kObdAccWrite | kObdAccRead)
//#define kObdAccSVGPCRW    (kObdAccStore | kObdAccVar | kObdAccRange | kObdAccPdo | kObdAccConst | kObdAccWrite | kObdAccRead)


//-----------------------------------------------------------------------------------------------------------
// states of variables
typedef BYTE tVarState;

#define kVarStateIdle       0x00
#define kVarStateSend       0x01


//-----------------------------------------------------------------------------------------------------------
// defines for NMT node states (defined in DS-301)
typedef BYTE tNodeState;

#define kNodeStateInitialisation    0x00
#define kNodeStateStopped           0x04
#define kNodeStateOperational       0x05
#define kNodeStatePreOperational    0x7F


// ============================================================================
// types
// ============================================================================

// -------------------------------------------------------------------------
// subindexstruct
// -------------------------------------------------------------------------

// Change not the order for this struct!!!

typedef struct
{
    BYTE            m_bSubIndex;
    BYTE            m_bType;        // values from tObdType
    BYTE            m_bAccess;      // values from tObdAccess
    CONST void ROM* m_pDefault;     // points always to ROM
    void  MEM*      m_pCurrent;     // points always to RAM

} tObdSubEntry;

// r.d.: has always to be GENERIC because new OBD-Macros for arrays
typedef tObdSubEntry GENERIC* tObdSubEntryPtr;
/*
// If variable subindex tables are used, then all pointer to a subindex entry has to be a generic pointer!
#if (defined (OBD_USE_VARIABLE_SUBINDEX_TAB) && (OBD_USE_VARIABLE_SUBINDEX_TAB != FALSE))

    typedef tObdSubEntry GENERIC* tObdSubEntryPtr;

#else

    typedef CONST tObdSubEntry ROM* tObdSubEntryPtr;

#endif
*/

// -------------------------------------------------------------------------
// callback function for objdictionary modul
// -------------------------------------------------------------------------

// parameters for callback function
typedef struct
{
    BYTE            m_ObdEvent;
    WORD            m_wIndex;
    BYTE            m_bSubIndex;
    void GENERIC*   m_pArg;
    DWORD           m_dwAbortCode;

} tObdCbParam;


// define type for callback function: pParam_p points to tObdCbParam
typedef tCopKernel (PUBLIC ROM* tObdCallback) (CCM_DECL_INSTANCE_HDL_
    tObdCbParam MEM* pParam_p);

// do not change the order for this struct!!!

typedef struct
{
    WORD            m_wIndex;
    tObdSubEntryPtr m_pSubIndex;
    BYTE            m_bCount;
    tObdCallback    m_fpCallback;   // function is called back if object access

} tObdEntry;

// If user OD is used, then all pointer to an index entry has to be a generic pointer!
#if (defined (OBD_USER_OD) && (OBD_USER_OD != FALSE))

    typedef tObdEntry GENERIC* tObdEntryPtr;

#else

    typedef CONST tObdEntry ROM* tObdEntryPtr;

#endif


// -------------------------------------------------------------------------
//structur  to save the internal mapping parameter in the ROM
// -------------------------------------------------------------------------
typedef struct
{
    BYTE            m_bNumberOfMappedObjects;
    DWORD           m_aMapObjects[8];

} tObdMappParam;


// -------------------------------------------------------------------------
// structur to save the internal communication parameter in the ROM
// -------------------------------------------------------------------------
typedef struct
{
    DWORD           m_dwCANId;
    BYTE            m_bTxTyp;
    WORD            m_wInhibitTime;
    WORD            m_wEventTimer;

} tObdCommuParam;


// -------------------------------------------------------------------------
// structur to initialize OBD module
// -------------------------------------------------------------------------

typedef struct
{
    tObdEntryPtr        m_pGenericPart;
    tObdEntryPtr        m_pManufacturerPart;
    tObdEntryPtr        m_pDevicePart;

    #if (defined (OBD_USER_OD) && (OBD_USER_OD != FALSE))

          tObdEntryPtr  m_pUserPart;

    #endif

} tObdInitParam;


// -------------------------------------------------------------------------
// structur for parameters of STORE RESTORE command
// -------------------------------------------------------------------------

typedef struct
{
    BYTE            m_bCommand;
    BYTE            m_bCurrentOdPart;
    void MEM*       m_pData;
    tObdSize        m_ObjSize;

} tObdCbStoreParam;


typedef tCopKernel (PUBLIC ROM* tInitTabEntryCallback) (
    void MEM* pTabEntry_p,
    WORD wObjIndex_p);

typedef tCopKernel (PUBLIC ROM* tObdStoreLoadObjCallback) (CCM_DECL_INSTANCE_HDL_
    tObdCbStoreParam MEM* pCbStoreParam_p);
/*
    tObdPart        CurrentOdPart_p,
    void GENERIC*   pData_p,
    tObdSize        ObjSize_p,
    tObdCommand     Command_p);
*/

// -------------------------------------------------------------------------
// this stucture is used for parameters for function ObdInitModuleTab()
// -------------------------------------------------------------------------
typedef struct
{
    WORD                    m_wLowerObjIndex;   // lower limit of ObjIndex
    WORD                    m_wUpperObjIndex;   // upper limit of ObjIndex
    tInitTabEntryCallback   m_fpInitTabEntry;   // will be called if ObjIndex was found
    void MEM*               m_pTabBase;         // base address of table
    WORD                    m_bEntrySize;       // size of table entry      // 25-feb-2005 r.d.: expansion from BYTE to WORD necessary for PDO bit mapping
    WORD                    m_wMaxEntries;      // max. tabel entries

} tObdModulTabParam;


// ============================================================================
// global variables
// ============================================================================

extern BYTE MEM abObdTrashObject_g[8];


// ============================================================================
// public functions
// ============================================================================

//COPDLLEXPORT tCopKernel PUBLIC ObdInitRam (void MEM* pInitParam_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdInit (MCO_DECL_PTR_INSTANCE_PTR_
    tObdInitParam MEM*      pInitParam_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdAddInstance (MCO_DECL_PTR_INSTANCE_PTR_
    tObdInitParam MEM*      pInitParam_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdDeleteInstance (MCO_DECL_INSTANCE_PTR);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdGetEntry (MCO_DECL_INSTANCE_PTR_
    WORD                    wIndex_p,
    BYTE                    bSubindex_p,
    tObdEntryPtr*           ppObdEntry_p,
    tObdSubEntryPtr*        ppObdSubEntry_p);

// ---------------------------------------------------------------------
/*
COPDLLEXPORT tCopKernel PUBLIC ObdWriteEntry (MCO_DECL_INSTANCE_PTR_
    WORD                    wIndex_p,
    BYTE                    bSubIndex_p,
    void GENERIC*           pSrcData_p);
*/
COPDLLEXPORT tCopKernel PUBLIC ObdWriteEntry (MCO_DECL_INSTANCE_PTR_
    WORD          wIndex_p,
    BYTE          bSubIndex_p,
    void GENERIC* pSrcData_p,
    tObdSize      Size_p);

// ---------------------------------------------------------------------
/*
COPDLLEXPORT tCopKernel PUBLIC ObdReadEntry (MCO_DECL_INSTANCE_PTR_
    WORD                    wIndex_p,
    BYTE                    bSubIndex_p,
    void MEM*               pDstData_p,
    tObdSize                Size_p);
*/
COPDLLEXPORT tCopKernel PUBLIC ObdReadEntry (MCO_DECL_INSTANCE_PTR_
    WORD              wIndex_p,
    BYTE              bSubIndex_p,
    void GENERIC*     pDstData_p,
    tObdSize GENERIC* pSize_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdSetStoreLoadObjCallback (MCO_DECL_INSTANCE_PTR_
    tObdStoreLoadObjCallback fpCallback_p);

// ---------------------------------------------------------------------
//COPDLLEXPORT tCopKernel PUBLIC ObdRestorePart (MCO_DECL_INSTANCE_PTR_
//    tObdPart                ObdPart_p);
COPDLLEXPORT tCopKernel PUBLIC ObdAccessOdPart (MCO_DECL_INSTANCE_PTR_
    tObdPart ObdPart_p,
    tObdDir Direction_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdSearchVarEntry (MCO_DECL_INSTANCE_PTR_
    WORD                    wIndex_p,
    BYTE                    bSubindex_p,
    tObdVarEntry MEM**      ppVarEntry_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdGetVarEntry (
    tObdSubEntryPtr         pSubindexEntry_p,
    tObdVarEntry MEM**      ppVarEntry_p);

//---------------------------------------------------------------------------
//
// Function:    ObdGetVarEntryArray()
//
// Description: 
//
// Parameters:  pSubindexEntry_p
//              bSubindex_p
//
// Return:      tObdVarEntry MEM*
//
//---------------------------------------------------------------------------

COPDLLEXPORT tObdVarEntry MEM* PUBLIC ObdGetVarEntryArray (
    tObdSubEntryPtr    pSubindexEntry_p,
    BYTE               bSubindex_p);

//---------------------------------------------------------------------------
//
// Function:    ObdGetVarEntryMapp()
//
// Description: 
//
// Parameters:  pSubindexEntry_p
//              wMappLowWord_p
//
// Return:      COPDLLEXPORT tObdVarEntry MEM*
//
//---------------------------------------------------------------------------

COPDLLEXPORT tObdVarEntry MEM* PUBLIC ObdGetVarEntryMapp (
    tObdSubEntryPtr    pSubindexEntry_p,
    WORD               wMappLowWord_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdDefineVar (MCO_DECL_INSTANCE_PTR_
    tVarParam MEM*          pVarParam_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdSetObjectData (
    void MEM*               pDst_p,
    void GENERIC*           pSrc_p,
    BYTE                    bSize_p,
    tObdType                ObdType_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdSetNumObjectData (
    void MEM*               pDst_p,
    void GENERIC*           pSrc_p,
    tObdType                ObdType_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdGetObjectData (
    void MEM*               pDst_p,
    void GENERIC*           pSrc_p,
    BYTE                    bSize_p,
    tObdType                ObdType_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdGetNumObjectData (
    void MEM*               pDst_p,
    void GENERIC*           pSrc_p,
    tObdType                ObdType_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdCallObjectCallback (MCO_DECL_INSTANCE_PTR_
    tObdCallback            fpCallback_p,
    tObdCbParam MEM*        pCbParam_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdCheckObjectRange (
    tObdSubEntryPtr         pSubindexEntry_p,
    void GENERIC*           pData_p);

// ---------------------------------------------------------------------
COPDLLEXPORT void GENERIC* PUBLIC ObdGetObjectDataPtr (tObdSubEntryPtr pSubindexEntry_p);

// ---------------------------------------------------------------------
COPDLLEXPORT BYTE PUBLIC ObdGetObjectClass (tObdType bType_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tObdSize PUBLIC ObdGetObjectSize (tObdSubEntryPtr pSubindexEntry_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tObdSize PUBLIC ObdGetDataSize (tObdSubEntryPtr pSubIndexEntry_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tObdSize PUBLIC  ObdGetStrLen (void GENERIC* pObjData_p, tObdSize ObjLen_p, BYTE bObjType_p);

// ---------------------------------------------------------------------
COPDLLEXPORT BYTE PUBLIC ObdGetNodeState (MCO_DECL_INSTANCE_PTR);

// ---------------------------------------------------------------------
COPDLLEXPORT void PUBLIC ObdSetNodeState (MCO_DECL_INSTANCE_PTR_
    BYTE                    bNodeState_p);

// ---------------------------------------------------------------------
COPDLLEXPORT BYTE PUBLIC ObdGetNodeId (MCO_DECL_INSTANCE_PTR);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdSetNodeId (MCO_DECL_INSTANCE_PTR_
    BYTE bNodeId_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdInitModuleTab (MCO_DECL_INSTANCE_PTR_
    tObdModulTabParam MEM*  pTabParam_p);

// ---------------------------------------------------------------------
COPDLLEXPORT tCopKernel PUBLIC ObdRegisterUserOd (MCO_DECL_INSTANCE_PTR_
    tObdEntryPtr pUserOd_p);

// ---------------------------------------------------------------------
COPDLLEXPORT void PUBLIC ObdInitVarEntry (tObdVarEntry MEM* pVarEntry_p,
     BYTE bType_p, tObdSize ObdSize_p);

// ---------------------------------------------------------------------
#if (OBD_CHECK_FLOAT_VALID != FALSE)
COPDLLEXPORT tCopKernel PUBLIC ObdCheckFloatVar (
    tObdSubEntryPtr         pSubindexEntry_p,
    tObdReal32 GENERIC*     pflVar_p);
#endif // (OBD_CHECK_FLOAT_VALID != FALSE)


#endif //_OBD_H_

// Please keep an empty line at the end of this file.

