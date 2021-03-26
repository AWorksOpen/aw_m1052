/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen

  Description:  defines objdict dictionary

  -------------------------------------------------------------------------

                $RCSfile: ObjDef.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.35 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                KEIL uVision 2

  -------------------------------------------------------------------------

  Revision History:

****************************************************************************/

#ifndef _OBJDEF_H_
#define _OBJDEF_H_


//---------------------------------------------------------------------------
// security checks
//---------------------------------------------------------------------------

#if (NMTM_MAX_SLAVE_ENTRIES > 127)

    #error 'ERROR: maximum of supported slave entries (NMTM_MAX_SLAVE_ENTRIES) is 127!'

#endif

#if (HBC_MAX_CONSUMER_IN_OBD > 127)

    #error 'ERROR: maximum of supported HBC entries (HBC_MAX_CONSUMER_IN_OBD) is 127!'

#endif

#if (EMCP_MAX_ERROR_ENTRIES_IN_OBD > 254)

    #error 'ERROR: maximum of supported error field entries (EMCP_MAX_ERROR_ENTRIES_IN_OBD) is 254!'

#endif

#if (SDO_MAX_SERVER_IN_OBD < 1)

    #error 'ERROR: SDO_MAX_SERVER_IN_OBD should never be less than 1!'

#endif

#if (SDO_MAX_SERVER_IN_OBD > 128)

    #error 'ERROR: maximum of supported SDO server entries (SDO_MAX_SERVER_IN_OBD) is 128!'

#endif

#if (SDO_MAX_CLIENT_IN_OBD > 128)

    #error 'ERROR: maximum of supported SDO clients entries (SDO_MAX_CLIENT_IN_OBD) is 128!'

#endif

#if (PDO_MAX_RXPDO_IN_OBD > 512)

    #error 'ERROR: maximum of supported RX PDOs (PDO_MAX_RXPDO_IN_OBD) is 512!'

#endif

#if (PDO_MAX_TXPDO_IN_OBD > 512)

    #error 'ERROR: maximum of supported TX PDOs (PDO_MAX_TXPDO_IN_OBD) is 512!'

#endif

#if (defined (SRDO_MAX_SRDO_IN_OBD) && (SRDO_MAX_SRDO_IN_OBD > 64))

    #error 'ERROR: maximum of supported SRDOs (SRDO_MAX_SRDO_IN_OBD) is 64!'

#endif


//---------------------------------------------------------------------------
// macros to help building OD
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// this macro desides if high priority CAN message buffer will be implemented or not
#if (CDRV_USE_HIGHBUFF != FALSE)

    #define CDRV_HIGHBUFF_ONLY(a)   a

#else

    #define CDRV_HIGHBUFF_ONLY(a)

#endif

#if (CDRV_MAX_TX_BUFF_ENTRIES_HIGH == 0)

    #define CDRV_CREATE_BUFFER_HIGH(a)

#else

    #define CDRV_CREATE_BUFFER_HIGH(a)  a;

#endif

#if (CDRV_MAX_TX_BUFF_ENTRIES_LOW == 0)

    #define CDRV_CREATE_BUFFER_LOW(a)

#else

    #define CDRV_CREATE_BUFFER_LOW(a)   a;

#endif



//---------------------------------------------------------------------------
// this macro desides if slave table will be implemented for NMT master or not
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_NMTM) != 0)

    #define CCM_NMTM_ONLY(a)        a

#else

    #define CCM_NMTM_ONLY(a)

#endif

#if (NMTM_MAX_SLAVE_ENTRIES == 0)

    #define NMTM_CREATE_TABLE(a)

#else

    #define NMTM_CREATE_TABLE(a)    a;

#endif


//---------------------------------------------------------------------------
// this macro desides if heartbeat consumer table will be implemented for HBC module or not
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_HBC) != 0)

    #define CCM_HBC_ONLY(a)         a

#else

    #define CCM_HBC_ONLY(a)

#endif

#if (HBC_MAX_CONSUMER_IN_OBD == 0)

    #define HBC_CREATE_TABLE(a)

#else

    #define HBC_CREATE_TABLE(a)     a;

#endif


//---------------------------------------------------------------------------
// this macro desides if emergency consumer table will be implemented for EMCC module or not
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_EMCC) != 0)

    #define CCM_EMCC_ONLY(a)        a

#else

    #define CCM_EMCC_ONLY(a)

#endif

#if (EMCC_MAX_CONSUMER == 0)

    #define EMCC_CREATE_TABLE(a)

#else

    #define EMCC_CREATE_TABLE(a)    a;

#endif


//---------------------------------------------------------------------------
// this macro desides if PDO tables will be implemented for PDO module or not
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_PDO) != 0)

    #define CCM_PDO_ONLY(a)         a

#else

    #define CCM_PDO_ONLY(a)

#endif

#if (PDO_USE_STATIC_MAPPING != FALSE)

    #define PDO_STATIC_MAPPING_ONEOF(a,b) a

#else

    #define PDO_STATIC_MAPPING_ONEOF(a,b) b

#endif

#if (PDO_MAX_RXPDO_IN_OBD == 0)

    #define PDO_CREATE_RX_TABLE(a)

#else

    #define PDO_CREATE_RX_TABLE(a)  a;

#endif

#if (PDO_MAX_TXPDO_IN_OBD == 0)

    #define PDO_CREATE_TX_TABLE(a)

#else

    #define PDO_CREATE_TX_TABLE(a)  a;

#endif


//---------------------------------------------------------------------------
// this macro desides if SDOC tables will be implemented for SDOC module or not
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SDOC) != 0)

    #define CCM_SDOC_ONLY(a)        a

#else

    #define CCM_SDOC_ONLY(a)

#endif

#if (SDO_MAX_CLIENT_IN_OBD == 0)

    #define SDOC_CREATE_TABLE(a)

#else

    #define SDOC_CREATE_TABLE(a)    a;

#endif


//---------------------------------------------------------------------------
// this macro desides if predefined error field will be implemented for EMCP module or not
#if (EMCP_USE_PREDEF_ERROR_FIELD != FALSE)

    #define CCM_PREDEFERR_ONLY(a)   a;

#else

    #define CCM_PREDEFERR_ONLY(a)

#endif


//---------------------------------------------------------------------------
// this macro desides if SRDO table will be implemented for SRDO module or not
#if ((CCM_MODULE_INTEGRATION & CCM_MODULE_SRDO) != 0)

    #define CCM_SRDO_ONLY(a)         a

#else

    #define CCM_SRDO_ONLY(a)

#endif


#if (SRDO_USE_STATIC_MAPPING != FALSE)

    #define SRDO_STATIC_MAPPING_ONEOF(a,b) a

#else

    #define SRDO_STATIC_MAPPING_ONEOF(a,b) b

#endif

#if (!defined (SRDO_MAX_SRDO_IN_OBD) || (SRDO_MAX_SRDO_IN_OBD == 0))

    #define SRDO_CREATE_TABLE(a)

#else

    #define SRDO_CREATE_TABLE(a)  a;

#endif


//---------------------------------------------------------------------------
#if (defined (OBD_USE_VARIABLE_SUBINDEX_TAB) && (OBD_USE_VARIABLE_SUBINDEX_TAB != FALSE))

    #define CCM_SUBINDEX_RAM_ONLY(a)    a;
    #define CCM_SUBINDEX_RAM_ONEOF(a,b) a

#else

    #define CCM_SUBINDEX_RAM_ONLY(a)
    #define CCM_SUBINDEX_RAM_ONEOF(a,b) b

#endif

//---------------------------------------------------------------------------
#if ((PDO_GRANULARITY != 8) && (PDO_USE_BIT_MAPPING != FALSE))

    #define PDO_SUBINDEX_BIT_MAPPING(sub,typ,acc,def,cur) \
        ,{sub, typ, acc, def, cur}

#else

    #define PDO_SUBINDEX_BIT_MAPPING(sub,typ,acc,def,cur)

#endif


//---------------------------------------------------------------------------
// To prevent unused memory in subindex tables we need this macro.
// But not all compilers support to preset the last struct value followed by a comma.
// Compilers which does not support a comma after last struct value has to place in a dummy subindex.
#if ((DEV_SYSTEM & _DEV_COMMA_EXT_) != 0)

    #define OBD_END_SUBINDEX()
    #define OBD_MAX_ARRAY_SUBENTRIES    2

#else

    #define OBD_END_SUBINDEX()          {0,0,0,NULL,NULL}
    #define OBD_MAX_ARRAY_SUBENTRIES    3

#endif


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// globale vars
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// -------------------------------------------------------------------------
// creation of data in ROM memory
// -------------------------------------------------------------------------

#define OBD_CREATE_ROM_DATA
#if defined(AW_COM_CANOPEN_MST)
#include "objdicts_mst/objdict.h"     // configuration of CAN driver
#elif defined(AW_COM_CANOPEN_SLV)
#include "objdicts_slv/objdict.h"     // configuration of CAN driver
#else
#error "must define AW_COM_CANOPEN_MST or AW_COM_CANOPEN_SLV at project"
#endif
//    #include "objdict.h"
#undef OBD_CREATE_ROM_DATA


// -------------------------------------------------------------------------
// creation of data in RAM memory
// -------------------------------------------------------------------------

#define OBD_CREATE_RAM_DATA
#if defined(AW_COM_CANOPEN_MST)
#include "objdicts_mst/objdict.h"     // configuration of CAN driver
#elif defined(AW_COM_CANOPEN_SLV)
#include "objdicts_slv/objdict.h"     // configuration of CAN driver
#else
#error "must define AW_COM_CANOPEN_MST or AW_COM_CANOPEN_SLV at project"
#endif
//    #include "objdict.h"
#undef OBD_CREATE_RAM_DATA


// -------------------------------------------------------------------------
// creation of subindex tables in ROM and RAM
// -------------------------------------------------------------------------

#define OBD_CREATE_SUBINDEX_TAB
#if defined(AW_COM_CANOPEN_MST)
#include "objdicts_mst/objdict.h"     // configuration of CAN driver
#elif defined(AW_COM_CANOPEN_SLV)
#include "objdicts_slv/objdict.h"     // configuration of CAN driver
#else
#error "must define AW_COM_CANOPEN_MST or AW_COM_CANOPEN_SLV at project"
#endif
//    #include "objdict.h"
#undef OBD_CREATE_SUBINDEX_TAB


// -------------------------------------------------------------------------
// creation of index tables for generic, manufacturer and device part
// -------------------------------------------------------------------------

#define OBD_CREATE_INDEX_TAB
#if defined(AW_COM_CANOPEN_MST)
#include "objdicts_mst/objdict.h"     // configuration of CAN driver
#elif defined(AW_COM_CANOPEN_SLV)
#include "objdicts_slv/objdict.h"     // configuration of CAN driver
#else
#error "must define AW_COM_CANOPEN_MST or AW_COM_CANOPEN_SLV at project"
#endif
//    #include "objdict.h"
#undef OBD_CREATE_INDEX_TAB


//=========================================================================//
//                                                                         //
//          P U B L I C   F U N C T I O N S                                //
//                                                                         //
//=========================================================================//

// ----------------------------------------------------------------------------
//
// Function:    OBD_INIT_RAM_NAME()
//
// Description: function to initialize object dictionary
//
// Parameters:  pInitParam_p    = pointer to init param struct of CANopen
//
// Returns:     tCopKernel      = error code
//
// State:
//
// ----------------------------------------------------------------------------

COPDLLEXPORT tCopKernel PUBLIC OBD_INIT_RAM_NAME (void MEM* pInitParam_p)
{

tObdRamParam MEM* pInitParam = (tObdRamParam MEM*) pInitParam_p;

    // check if pointer to parameter structure is valid
    // if not then only copy subindex tables below
    if (pInitParam != NULL)
    {
        // at first delete all parameters (all pointers will be set zu NULL)
        COP_MEMSET (pInitParam, 0, sizeof (tObdRamParam));

        // CAN high priority buffer
        #if (CDRV_MAX_TX_BUFF_ENTRIES_HIGH > 0)
        {
            CDRV_HIGHBUFF_ONLY (pInitParam->m_CdrvInitParam.m_TxBuffHigh.m_pBufferBase = &aCanTxMsgBufferHigh_g[0]);
            CDRV_HIGHBUFF_ONLY (pInitParam->m_CdrvInitParam.m_TxBuffHigh.m_MaxEntries  = (tCdrvBuffIndex) CDRV_MAX_TX_BUFF_ENTRIES_HIGH);
        }
        #else
        {
            CDRV_HIGHBUFF_ONLY (pInitParam->m_CdrvInitParam.m_TxBuffHigh.m_pBufferBase = NULL);
            CDRV_HIGHBUFF_ONLY (pInitParam->m_CdrvInitParam.m_TxBuffHigh.m_MaxEntries  = 0);
        }
        #endif

        #if (CDRV_MAX_RX_BUFF_ENTRIES_HIGH > 0)
        {
            CDRV_HIGHBUFF_ONLY (pInitParam->m_CdrvInitParam.m_RxBuffHigh.m_pBufferBase = &aCanRxMsgBufferHigh_g[0]);
            CDRV_HIGHBUFF_ONLY (pInitParam->m_CdrvInitParam.m_RxBuffHigh.m_MaxEntries  = (tCdrvBuffIndex) CDRV_MAX_RX_BUFF_ENTRIES_HIGH);
        }
        #else
        {
            CDRV_HIGHBUFF_ONLY (pInitParam->m_CdrvInitParam.m_RxBuffHigh.m_pBufferBase = NULL);
            CDRV_HIGHBUFF_ONLY (pInitParam->m_CdrvInitParam.m_RxBuffHigh.m_MaxEntries  = 0);
        }
        #endif

        // CAN low priority buffer
        #if (CDRV_MAX_TX_BUFF_ENTRIES_LOW > 0)
        {
            pInitParam->m_CdrvInitParam.m_TxBuffLow.m_pBufferBase = &aCanTxMsgBufferLow_g[0];
            pInitParam->m_CdrvInitParam.m_TxBuffLow.m_MaxEntries  = (tCdrvBuffIndex) CDRV_MAX_TX_BUFF_ENTRIES_LOW;
        }
        #else
        {
            pInitParam->m_CdrvInitParam.m_TxBuffLow.m_pBufferBase = NULL;
            pInitParam->m_CdrvInitParam.m_TxBuffLow.m_MaxEntries  = 0;
        }
        #endif

        #if (CDRV_MAX_RX_BUFF_ENTRIES_LOW > 0)
        {
            pInitParam->m_CdrvInitParam.m_RxBuffLow.m_pBufferBase = &aCanRxMsgBufferLow_g[0];
            pInitParam->m_CdrvInitParam.m_RxBuffLow.m_MaxEntries  = (tCdrvBuffIndex) CDRV_MAX_RX_BUFF_ENTRIES_LOW;
        }
        #else
        {
            pInitParam->m_CdrvInitParam.m_RxBuffLow.m_pBufferBase = NULL;
            pInitParam->m_CdrvInitParam.m_RxBuffLow.m_MaxEntries  = 0;
        }
        #endif

        // receive COB object table
        pInitParam->m_CobInitParam.m_RxCobInfo.m_pBase      = &aCobRxTab1_g[0];
        pInitParam->m_CobInitParam.m_RxCobInfo.m_MaxEntries = (tCobEntryIndex) COB_MAX_RX_COB_ENTRIES;

        // send COB object table
        pInitParam->m_CobInitParam.m_TxCobInfo.m_pBase      = &aCobTxTab1_g[0];
        pInitParam->m_CobInitParam.m_TxCobInfo.m_MaxEntries = (tCobEntryIndex) COB_MAX_TX_COB_ENTRIES;

        // NMT slave table for NMT master
        #if (NMTM_MAX_SLAVE_ENTRIES > 0)
        {
            CCM_NMTM_ONLY (pInitParam->m_NmtmInitParam.m_paNmtmSlaveEntry     = &aNmtmSlaveTable_g[0]);
            CCM_NMTM_ONLY (pInitParam->m_NmtmInitParam.m_bNmtmMaxSlaveEntries = (BYTE) NMTM_MAX_SLAVE_ENTRIES);
        }
        #else
        {
            CCM_NMTM_ONLY (pInitParam->m_NmtmInitParam.m_paNmtmSlaveEntry     = NULL);
            CCM_NMTM_ONLY (pInitParam->m_NmtmInitParam.m_bNmtmMaxSlaveEntries = 0);
        }
        #endif

        // heartbeat producer table for heartbeat consumer
        #if (HBC_MAX_CONSUMER_IN_OBD > 0)
        {
            CCM_HBC_ONLY (pInitParam->m_HbcInitParam.m_paHbcProducerTable     = &aHbcProducerTable_g[0]);
            CCM_HBC_ONLY (pInitParam->m_HbcInitParam.m_bHbcMaxProducerEntries = (BYTE) HBC_MAX_CONSUMER_IN_OBD);
        }
        #else
        {
            CCM_HBC_ONLY (pInitParam->m_HbcInitParam.m_paHbcProducerTable     = NULL);
            CCM_HBC_ONLY (pInitParam->m_HbcInitParam.m_bHbcMaxProducerEntries = 0);
        }
        #endif

        // predefined error field
        CCM_PREDEFERR_ONLY (pInitParam->m_EmcpInitParam.m_papdwPreDefErrorField           = &apdwEmcpErrorField_g[0]);
        CCM_PREDEFERR_ONLY (pInitParam->m_EmcpInitParam.m_bEmcpMaxPreDefErrorFieldEntries = (BYTE) EMCP_MAX_ERROR_ENTRIES_IN_OBD);
        pInitParam->m_EmcpInitParam.m_fpErrorCallback = NULL;

        // emergency producer table for emergency consumer
        #if (EMCC_MAX_CONSUMER > 0)
        {
            CCM_EMCC_ONLY (pInitParam->m_EmccInitParam.m_paEmccProducerTable     = &aEmccProducerTable_g[0]);
            CCM_EMCC_ONLY (pInitParam->m_EmccInitParam.m_bEmccMaxProducerEntries = (BYTE) EMCC_MAX_CONSUMER);
        }
        #else
        {
            CCM_EMCC_ONLY (pInitParam->m_EmccInitParam.m_paEmccProducerTable     = NULL);
            CCM_EMCC_ONLY (pInitParam->m_EmccInitParam.m_bEmccMaxProducerEntries = 0);
        }
        #endif

        // RX and TX PDO table
        #if (PDO_MAX_RXPDO_IN_OBD > 0)
        {
            CCM_PDO_ONLY (pInitParam->m_PdoInitParam.m_PdoTabParam.m_pRxPdoTable = &aPdoRxTable_g[0]);
            CCM_PDO_ONLY (pInitParam->m_PdoInitParam.m_PdoTabParam.m_RxPdoCount  = (tPdoIndex) PDO_MAX_RXPDO_IN_OBD);
        }
        #else
        {
            // 2006/08/11 d.k.: because of change in PdoRegisterPdoTabIntern() m_pRxPdoTable needs to be not
            //                  NULL to disable RPDOs. m_RxPdoCount will remain unaffected.
            CCM_PDO_ONLY (pInitParam->m_PdoInitParam.m_PdoTabParam.m_pRxPdoTable = (tPdoTabEntry MEM*) 1);
            CCM_PDO_ONLY (pInitParam->m_PdoInitParam.m_PdoTabParam.m_RxPdoCount  = 0);
        }
        #endif

        #if (PDO_MAX_TXPDO_IN_OBD > 0)
        {
            CCM_PDO_ONLY (pInitParam->m_PdoInitParam.m_PdoTabParam.m_pTxPdoTable = &aPdoTxTable_g[0]);
            CCM_PDO_ONLY (pInitParam->m_PdoInitParam.m_PdoTabParam.m_TxPdoCount  = (tPdoIndex) PDO_MAX_TXPDO_IN_OBD);
        }
        #else
        {
            // 2006/08/11 d.k.: because of change in PdoRegisterPdoTabIntern() m_pTxPdoTable needs to be not
            //                  NULL to disable TPDOs. m_TxPdoCount will remain unaffected.
            CCM_PDO_ONLY (pInitParam->m_PdoInitParam.m_PdoTabParam.m_pTxPdoTable = (tPdoTabEntry MEM*) 1);
            CCM_PDO_ONLY (pInitParam->m_PdoInitParam.m_PdoTabParam.m_TxPdoCount  = 0);
        }
        #endif

        // SDO server table
        pInitParam->m_SdosInitParam.m_pSdosTable      = &aSdosTable_g[0];
        pInitParam->m_SdosInitParam.m_bSdosTabEntries = (BYTE) SDO_MAX_SERVER_IN_OBD;

        // SDO client table
        #if (SDO_MAX_CLIENT_IN_OBD > 0)
        {
            CCM_SDOC_ONLY (pInitParam->m_SdocInitParam.m_pSdocTable      = &aSdocTable_g[0]);
            CCM_SDOC_ONLY (pInitParam->m_SdocInitParam.m_bSdocTabEntries = (BYTE) SDO_MAX_CLIENT_IN_OBD);
        }
        #else
        {
            CCM_SDOC_ONLY (pInitParam->m_SdocInitParam.m_pSdocTable      = NULL);
            CCM_SDOC_ONLY (pInitParam->m_SdocInitParam.m_bSdocTabEntries = 0);
        }
        #endif

        // SRDO-Table
        #if (defined (SRDO_MAX_SRDO_IN_OBD) && (SRDO_MAX_SRDO_IN_OBD > 0))
        {
            CCM_SRDO_ONLY (pInitParam->m_SrdoInitParam.m_SrdoTabParam.m_pSrdoTable = &aSrdoTable_g[0]);
            CCM_SRDO_ONLY (pInitParam->m_SrdoInitParam.m_SrdoTabParam.m_SrdoCount  = (tSrdoIndex) SRDO_MAX_SRDO_IN_OBD);
        }
        #else
        {
            CCM_SRDO_ONLY (pInitParam->m_SrdoInitParam.m_SrdoTabParam.m_pSrdoTable = NULL);
            CCM_SRDO_ONLY (pInitParam->m_SrdoInitParam.m_SrdoTabParam.m_SrdoCount  = 0);
        }
        #endif

        CCM_SRDO_ONLY (pInitParam->m_SrdoInitParam.m_pbSrdoConfigValid = &bSrdoCur0x13FEConfigValid_g);

        #define OBD_CREATE_INIT_FUNCTION
        {
            // inserts code to init pointer to index tables
#if defined(AW_COM_CANOPEN_MST)
#include "objdicts_mst/objdict.h"     // configuration of CAN driver
#elif defined(AW_COM_CANOPEN_SLV)
#include "objdicts_slv/objdict.h"     // configuration of CAN driver
#else
#error "must define AW_COM_CANOPEN_MST or AW_COM_CANOPEN_SLV at project"
#endif
//            #include "objdict.h"
        }
        #undef OBD_CREATE_INIT_FUNCTION

        #if (defined (OBD_USER_OD) && (OBD_USER_OD != FALSE))
        {
            // to begin no user OD is defined
            pInitParam->m_ObdInitParam.m_pUserPart = NULL;
        }
        #endif
    }

    #define OBD_CREATE_INIT_SUBINDEX
    {
        // inserts code to copy subindex tables
#if defined(AW_COM_CANOPEN_MST)
#include "objdicts_mst/objdict.h"     // configuration of CAN driver
#elif defined(AW_COM_CANOPEN_SLV)
#include "objdicts_slv/objdict.h"     // configuration of CAN driver
#else
#error "must define AW_COM_CANOPEN_MST or AW_COM_CANOPEN_SLV at project"
#endif
//        #include "objdict.h"
    }
    #undef OBD_CREATE_INIT_SUBINDEX

    return kCopSuccessful;

}


#endif // _OBJDEF_H_

// Please keep an empty line at the end of this file.

