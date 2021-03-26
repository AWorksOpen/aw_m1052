/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen Manager

  Description:  Header file for CANopen Manager Extended API

  -------------------------------------------------------------------------

                $RCSfile: CopMgrEx.h,v $
                
                $Author: D.Krueger $
                
                $Revision: 1.2 $  $Date: 2006/08/15 09:52:44 $
                
                $State: Exp $
                
                Build Environment:
                    all

  -------------------------------------------------------------------------

  Revision History:

  2006/02/14 d.k.:  Start of Implementation

****************************************************************************/


//=========================================================================//
//                                                                         //
//          P U B L I C   F U N C T I O N S                                //
//                                                                         //
//=========================================================================//

// ----------------------------------------------------------------------------
//
// Function:    CopMgrExSendSync()
//
// Description: function sends the SYNC message without checking cycle time.
//              It sends only if NMT state is unequal to STOPPED and SYNC
//              producer is activated.
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//
// State:       
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrExSendSync (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CopMgrExForceAsynPdo()
//
// Description: function force all async TxPDO's independent of
//              Inhibittime or Eventtimer
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     tCopKernel              = error code
//                  kCopSuccessful      = returns always successful
//
// State:       
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrExForceAsynPdo (CCM_DECL_INSTANCE_HDL);


// ----------------------------------------------------------------------------
//
// Function:    CopMgrExGetNodeId()
//
// Description: returns the current local node id
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//
// Returns:     BYTE                    = local node id
//
// State:       
//
// ----------------------------------------------------------------------------

COPDLLEXPORT BYTE PUBLIC CopMgrExGetNodeId (CCM_DECL_INSTANCE_HDL);

//----------------------------------------------------------------------------
// Function:    CopMgrExDefineObject()
//
//
// Description: Function defines global vars for one specified object
//
//
// Parameters:  CCM_DECL_INSTANCE_HDL   = (instance handle)
//              wObjIndex_p             = Function defines vars for this object
//              pVar_p                  = Pointer to data memory area for the specified object
//              pbVarEntries_p          = Pointer to number of entries to defined
//                                        if 0 all entries of this object will be defined
//              pEntrySize_p            = Pointer to size of one entry
//                                        muss der groessten Ausbaustufe entsprechen
//                                        es wird ueber diesen Parameter die tatsaechliche
//                                        Anzahl von BYTE's zurueckgegeben (f.j.)
//              bFirstSubindex_p        = This is the first subindex to be mapped.
//              bIndexWholeArray_p      = At read access to this subindex
//                                        the whole array is returned
//                                        if 0 no special subindex exists
//
//
// Returns:     tCopKernel              = error code
//
// State:
//----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrExDefineObject(CCM_DECL_INSTANCE_HDL_
                                    WORD            wObjIndex_p,
                                    void MEM*       pVar_p,
                                    BYTE MEM*       pbVarEntries_p,
                                    tObdSize MEM*   pEntrySize_p,
                                    BYTE            bFirstSubindex_p,
                                    BYTE            bIndexWholeArray_p );

// ----------------------------------------------------------------------------
// Function:    CopMgrExSearchObdVarEntry()
//
// Description: Function gets the variable structure from OD
//
// Parameters:   InstanceHandle         = instance handle
//               wIndex_p               = index of the object
//               bSubindex_p            = subindex of the object
//               ppVarEntry_p           = pointer to pointer to var structure
//                                        (filled by function ObdSearchVarEntry)
//
// Returns:      tCopKernel             = error code
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC CopMgrExSearchObdVarEntry(CCM_DECL_INSTANCE_HDL_
                                        WORD               wIndex_p,
                                        BYTE               bSubindex_p,
                                        tObdVarEntry MEM** ppVarEntry_p);
