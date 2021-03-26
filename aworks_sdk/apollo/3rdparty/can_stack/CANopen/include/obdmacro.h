/****************************************************************************

  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
      www.systec-electronic.de

  Project:      CANopen V5

  Description:  Mocro definitions for building object dictionary

  -------------------------------------------------------------------------

                $RCSfile: ObdMacro.h,v $

                $Author: R.Dietzsch $

                $Revision: 1.85 $  $Date: 2006/09/19 15:32:21 $

                $State: Exp $

                Build Environment:
                KEIL uVision 2 C166 V4.26a

  -------------------------------------------------------------------------

  Revision History:

  2002/XX/XX r.d.:  Start der Implementierung
  2003/04/10 r.d.:  OBD_SUBINDEX_RAM_xSTRING() legt im RAM statt tXString eine
                    tVarEntry Struktur an und setzt das entsprechende Access-Bit

  2003/12/11 f.j.:  Aenderung der Makros OBD_SUBINDEX_RAM_VSTRING
                    Die Verwaltungstruktur von Strings wird vom eigentlichen
                    String abgekoppelt und liegt im MEM.
                    Die Verwaltungstruktur darf nicht als char Feld abgelegt werden,
                    da diese Feld z.B. vom C166 auf eine ungerade Adresse gelinkt
                    werden kann. Das fuehrt zu einer Schutzverletzung
                    Strings im RAM erhalten den zusaetzlichen Parameter fuer
                    die maximale Laenge des String

  2004/11/17 a.s.:  Änderung des Makro OBD_SUBINDEX_RAM_VSTRING
                    Die Initialisierung der Verwaltungsstruktur im MEM erfolgt 
                    durch die Funktion ObdAccessOdPartIntern und sollte nicht
                    durch den Compiler als vorbelegte Variablen erfolgen.
                    Das funktioniert nicht für jeden Compiler bzw. Compiler-
                    einstellung sicher. 

  2005/05/XX r.d.:  Macros added for SRDO module.

  2005/06/22 r.d.:  MSVC 1.5 does not support macro definitions more than 251
                    characters in one line. These macros were be cutted.

  2006/01/02 r.d.:  macros OBD_ROM_INDEX_RAM_ARRAY() and OBD_ROM_INDEX_RAM_EXTARRAY() added

  2006/01/09 r.d.:  macro OBD_ROM_INDEX_RAM_VARARRAY() added

****************************************************************************/

#if (DEV_SYSTEM == _DEV_MSVC_DOS_)
    #if (PDO_USE_BIT_MAPPING != FALSE)
        #error 'PDO Bit Mapping can not be used with MSVC 1.5 because macro definitions are to long!'
    #endif
#endif

#if defined (OBD_DEFINE_MACRO)

    //-------------------------------------------------------------------------------------------
    #if defined (OBD_CREATE_ROM_DATA)

        #define OBD_BEGIN()                                                         static CONST DWORD ROM dwObd_OBK_g = OD_BUILDER_KEY;
        #define OBD_END()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_PART_GENERIC()
        #define OBD_BEGIN_PART_MANUFACTURER()
        #define OBD_BEGIN_PART_DEVICE()
        #define OBD_END_PART()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_INDEX_ROM(ind,cnt,call)
        #define OBD_BEGIN_INDEX_RAM(ind,cnt,call)
        #define OBD_END_INDEX(ind)
        #define OBD_ROM_INDEX_RAM_ARRAY(ind,cnt,call,typ,acc,dtyp,name,def)         static CONST tObdUnsigned8 ROM xDef##ind##_0x00_g = (cnt); \
                                                                                    static CONST dtyp ROM xDef##ind##_0x01_g = (def);
        #define OBD_ROM_INDEX_RAM_EXTARRAY(ind,cnt,call,typ,acc,dtyp,name,def)      static CONST tObdUnsigned8 ROM xDef##ind##_0x00_g = (cnt); \
                                                                                    static CONST dtyp ROM xDef##ind##_0x01_g = (def);
        #define OBD_ROM_INDEX_RAM_VARARRAY(ind,cnt,call,typ,acc,dtyp,name,def)      static CONST tObdUnsigned8 ROM xDef##ind##_0x00_g = (cnt); \
                                                                                    static CONST dtyp ROM xDef##ind##_0x01_g = (def);

        //---------------------------------------------------------------------------------------
        #define OBD_SUBINDEX_ROM_VAR(ind,sub,typ,acc,dtyp,name,val)                 static CONST dtyp ROM xDef##ind##_##sub##_g        = val;
        #define OBD_SUBINDEX_RAM_VAR(ind,sub,typ,acc,dtyp,name,val)                 static CONST dtyp ROM xDef##ind##_##sub##_g        = val;
        #define OBD_SUBINDEX_RAM_VAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)     static CONST dtyp ROM xDef##ind##_##sub##_g[3]     = {val,low,high};
        #define OBD_SUBINDEX_RAM_EXTVAR(ind,sub,typ,acc,dtyp,name,val)              static CONST dtyp ROM xDef##ind##_##sub##_g        = val;
        #define OBD_SUBINDEX_RAM_EXTVAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)  static CONST dtyp ROM xDef##ind##_##sub##_g[3]     = {val,low,high};
        #define OBD_SUBINDEX_ROM_VSTRING(ind,sub,acc,name,val)                      static CONST tObdVString ROM xDef##ind##_##sub##_g = {sizeof(val), val};
        #define OBD_SUBINDEX_RAM_VAR_NOINIT(ind,sub,typ,acc,dtyp,name)
        #define OBD_SUBINDEX_RAM_EXTVAR_NOINIT(ind,sub,typ,acc,dtyp,name)
                                                                                    // 14-dec-2004 r.d.: always size+1 and sizeof(val) because '\0' termination of strings
        #define OBD_SUBINDEX_RAM_VSTRING(ind,sub,acc,name,size,val)                 static char  MEM szCur##ind##_##sub##_g[size+1]; \
                                                                                    static CONST tObdVStringDef ROM xDef##ind##_##sub##_g = {size, val, szCur##ind##_##sub##_g};
                                                                                    //static CONST tObdVStringDef ROM xDef##ind##_##sub##_g = {sizeof(val), val, szCur##ind##_##sub##_g};
        #define OBD_SUBINDEX_RAM_DOMAIN(ind,sub,acc,name)
        #define OBD_SUBINDEX_RAM_USERDEF(ind,sub,typ,acc,dtyp,name,val)             static CONST dtyp ROM xDef##ind##_##sub##_g        = val;
        #define OBD_SUBINDEX_RAM_USERDEF_RG(ind,sub,typ,acc,dtyp,name,val,low,high) static CONST dtyp ROM xDef##ind##_##sub##_g[3]     = {val,low,high};
        #define OBD_SUBINDEX_RAM_USERDEF_NOINIT(ind,sub,typ,acc,dtyp,name)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SDOS(ind,num,cobcs,cobsc,idc)                            static CONST BYTE  ROM bSdos##num##MaxEntries_g = (((ind) == 0x1200) ? 2 : 3); \
                                                                                    static CONST DWORD ROM dwSdos##num##DefCobCs_g  = cobcs; \
                                                                                    static CONST DWORD ROM dwSdos##num##DefCobSc_g  = cobsc; \
                                                                                    static CONST BYTE  ROM bSdos##num##DefIdc_g     = idc;
        #define OBD_CREATE_SDOC(ind,num,cobcs,cobsc,ids)                            static CONST BYTE  ROM bSdoc##num##MaxEntries_g = 3; \
                                                                                    static CONST DWORD ROM dwSdoc##num##DefCobCs_g  = cobcs; \
                                                                                    static CONST DWORD ROM dwSdoc##num##DefCobSc_g  = cobsc; \
                                                                                    static CONST BYTE  ROM bSdoc##num##DefIds_g     = ids;

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_RXPDO_COMMU(inx,num,cob,typ,inh,evt)                     static CONST BYTE  ROM bRxPdo##num##MaxCommuEntries_g = 5; \
                                                                                    static CONST DWORD ROM dwRxPdo##num##DefCob_g         = cob; \
                                                                                    static CONST BYTE  ROM bRxPdo##num##DefTyp_g          = typ; \
                                                                                    static CONST WORD  ROM wRxPdo##num##DefInh_g          = inh; \
                                                                                    static CONST WORD  ROM wRxPdo##num##DefEvt_g          = evt;
        #define OBD_CREATE_RXPDO_MAPP(inx,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)  static CONST BYTE  ROM bRxPdo##num##MaxMappEntries_g  = cnt; \
                                                                                    static CONST DWORD ROM adwRxPdo##num##DefMapp_g[PDO_MAX_MAPPENTRIES] = \
                                                                                        {ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8};
        #define OBD_CREATE_TXPDO_COMMU(inx,num,cob,typ,inh,evt)                     static CONST BYTE  ROM bTxPdo##num##MaxCommuEntries_g = 5; \
                                                                                    static CONST DWORD ROM dwTxPdo##num##DefCob_g         = cob; \
                                                                                    static CONST BYTE  ROM bTxPdo##num##DefTyp_g          = typ; \
                                                                                    static CONST WORD  ROM wTxPdo##num##DefInh_g          = inh; \
                                                                                    static CONST WORD  ROM wTxPdo##num##DefEvt_g          = evt;
        #define OBD_CREATE_TXPDO_MAPP(inx,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)  static CONST BYTE  ROM bTxPdo##num##MaxMappEntries_g  = cnt; \
                                                                                    static CONST DWORD ROM adwTxPdo##num##DefMapp_g[PDO_MAX_MAPPENTRIES] = \
                                                                                        {ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8};

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SRDO_COMMU(inx,num,dir,sct,srvt,cob1,cob2)               static CONST BYTE  ROM bSrdo##num##MaxCommuEntries_g = 6; \
                                                                                    static CONST BYTE  ROM bSrdo##num##DefDir_g          = dir; \
                                                                                    static CONST WORD  ROM wSrdo##num##DefSct_g          = sct; \
                                                                                    static CONST BYTE  ROM bSrdo##num##DefSrvt_g         = srvt; \
                                                                                    static CONST BYTE  ROM bSrdo##num##DefTyp_g          = 254; \
                                                                                    static CONST DWORD ROM dwSrdo##num##DefCob1_g        = cob1; \
                                                                                    static CONST DWORD ROM dwSrdo##num##DefCob2_g        = cob2;
        #define OBD_BEGIN_SRDO_MAPP(ind,num,cnt)                                    static CONST BYTE  ROM bSrdo##num##DefMaxMappEntries_g  = cnt; \
                                                                                    static CONST DWORD ROM adwSrdo##num##DefMapp_g[] = {
        #define OBD_SUBINDEX_SRDO_MAPP(ind,sub,num,name,val)                        val,
        #define OBD_END_SRDO_MAPP(ind)                                              0};

        #define OBD_CREATE_SRDO_CFG_VALID()                                         static CONST BYTE  ROM bSrdoDef0x13FEConfigValid_g = 0;
        #define OBD_CREATE_SRDO_GFC_PARAM()                                         static CONST BYTE  ROM bSrdoDef0x1300GfcParam_g = 0;

        #define OBD_BEGIN_SRDO_CRC(cnt)                                             static CONST BYTE  ROM bSrdo0x13FFDefMaxCRCEntries_g = cnt; \
                                                                                    static CONST WORD  ROM awSrdo0x13FFDefCRCTable_g[] = {
        #define OBD_SUBINDEX_SRDO_CRC(sub,name,val)                                 val,
        #define OBD_END_SRDO_CRC()                                                  0};

    //-------------------------------------------------------------------------------------------
    #elif defined (OBD_CREATE_RAM_DATA)

        #define OBD_BEGIN()           CDRV_HIGHBUFF_ONLY ( CDRV_CREATE_BUFFER_HIGH (static tCdrvMsg        MEM aCanTxMsgBufferHigh_g [CDRV_MAX_TX_BUFF_ENTRIES_HIGH])) \
                                      CDRV_HIGHBUFF_ONLY ( CDRV_CREATE_BUFFER_HIGH (static tCdrvMsg        MEM aCanRxMsgBufferHigh_g [CDRV_MAX_RX_BUFF_ENTRIES_HIGH])) \
                                                            CDRV_CREATE_BUFFER_LOW (static tCdrvMsg        MEM aCanTxMsgBufferLow_g  [CDRV_MAX_TX_BUFF_ENTRIES_LOW]) \
                                                            CDRV_CREATE_BUFFER_LOW (static tCdrvMsg        MEM aCanRxMsgBufferLow_g  [CDRV_MAX_RX_BUFF_ENTRIES_LOW]) \
                                                                                    static tCobEntry       MEM aCobRxTab1_g          [COB_MAX_RX_COB_ENTRIES]; \
                                                                                    static tCobEntry       MEM aCobTxTab1_g          [COB_MAX_TX_COB_ENTRIES]; \
                                                  CCM_NMTM_ONLY (NMTM_CREATE_TABLE (static tNmtmSlaveEntry MEM aNmtmSlaveTable_g     [NMTM_MAX_SLAVE_ENTRIES])) \
                                                    CCM_HBC_ONLY (HBC_CREATE_TABLE (static tHbcInfo        MEM aHbcProducerTable_g   [HBC_MAX_CONSUMER_IN_OBD])) \
                                                                CCM_PREDEFERR_ONLY (static DWORD MEM*      MEM apdwEmcpErrorField_g  [EMCP_MAX_ERROR_ENTRIES_IN_OBD]) \
                                                  CCM_EMCC_ONLY (EMCC_CREATE_TABLE (static tEmccInfo       MEM aEmccProducerTable_g  [EMCC_MAX_CONSUMER])) \
                                                 CCM_PDO_ONLY (PDO_CREATE_RX_TABLE (static tPdoTabEntry    MEM aPdoRxTable_g         [PDO_MAX_RXPDO_IN_OBD])) \
                                                 CCM_PDO_ONLY (PDO_CREATE_TX_TABLE (static tPdoTabEntry    MEM aPdoTxTable_g         [PDO_MAX_TXPDO_IN_OBD])) \
                                                                                    static tSdosTabEntry   MEM aSdosTable_g          [SDO_MAX_SERVER_IN_OBD]; \
                                                  CCM_SDOC_ONLY (SDOC_CREATE_TABLE (static tSdocTabEntry   MEM aSdocTable_g          [SDO_MAX_CLIENT_IN_OBD])) \
                                                  CCM_SRDO_ONLY (SRDO_CREATE_TABLE (static tSrdoTabEntry   MEM aSrdoTable_g          [SRDO_MAX_SRDO_IN_OBD]))

        #define OBD_END()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_PART_GENERIC()
        #define OBD_BEGIN_PART_MANUFACTURER()
        #define OBD_BEGIN_PART_DEVICE()
        #define OBD_END_PART()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_INDEX_ROM(ind,cnt,call)
        #define OBD_BEGIN_INDEX_RAM(ind,cnt,call)
        #define OBD_END_INDEX(ind)
        #define OBD_ROM_INDEX_RAM_ARRAY(ind,cnt,call,typ,acc,dtyp,name,def)         static dtyp         MEM axCur##ind##_g[cnt];
        #define OBD_ROM_INDEX_RAM_EXTARRAY(ind,cnt,call,typ,acc,dtyp,name,def)
        #define OBD_ROM_INDEX_RAM_VARARRAY(ind,cnt,call,typ,acc,dtyp,name,def)      static tObdVarEntry MEM aVarEntry##ind##_g[cnt];

        //---------------------------------------------------------------------------------------
        #define OBD_SUBINDEX_ROM_VAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_VAR(ind,sub,typ,acc,dtyp,name,val)                 static dtyp         MEM xCur##ind##_##sub##_g;
        #define OBD_SUBINDEX_RAM_VAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)     static dtyp         MEM xCur##ind##_##sub##_g;
        #define OBD_SUBINDEX_RAM_EXTVAR(ind,sub,typ,acc,dtyp,name,val)              /*extern dtyp         MEM name;*/
        #define OBD_SUBINDEX_RAM_EXTVAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)  /*extern dtyp         MEM name;*/
        #define OBD_SUBINDEX_ROM_VSTRING(ind,sub,acc,name,val)
        #define OBD_SUBINDEX_RAM_VSTRING(ind,sub,acc,name,size,val)                 static tObdVString  MEM xCur##ind##_##sub##_g;
        #define OBD_SUBINDEX_RAM_VAR_NOINIT(ind,sub,typ,acc,dtyp,name)              static dtyp         MEM xCur##ind##_##sub##_g;
        #define OBD_SUBINDEX_RAM_EXTVAR_NOINIT(ind,sub,typ,acc,dtyp,name)
        #define OBD_SUBINDEX_RAM_DOMAIN(ind,sub,acc,name)                           static tObdVarEntry MEM VarEntry##ind##_##sub##_g;
        #define OBD_SUBINDEX_RAM_USERDEF(ind,sub,typ,acc,dtyp,name,val)             static tObdVarEntry MEM VarEntry##ind##_##sub##_g;
        #define OBD_SUBINDEX_RAM_USERDEF_RG(ind,sub,typ,acc,dtyp,name,val,low,high) static tObdVarEntry MEM VarEntry##ind##_##sub##_g;
        #define OBD_SUBINDEX_RAM_USERDEF_NOINIT(ind,sub,typ,acc,dtyp,name)          static tObdVarEntry MEM VarEntry##ind##_##sub##_g;

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SDOS(inx,num,cobcs,cobsc,idc)
        #define OBD_CREATE_SDOC(inx,num,cobcs,cobsc,ids)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_RXPDO_COMMU(ind,num,cob,typ,inh,evt)
        #define OBD_CREATE_RXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)
        #define OBD_CREATE_TXPDO_COMMU(ind,num,cob,typ,inh,evt)
        #define OBD_CREATE_TXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SRDO_COMMU(inx,num,dir,sct,srvt,cob1,cob2)
        #define OBD_BEGIN_SRDO_MAPP(ind,num,cnt)
        #define OBD_SUBINDEX_SRDO_MAPP(ind,sub,num,name,val)
        #define OBD_END_SRDO_MAPP(ind)
        #define OBD_CREATE_SRDO_CFG_VALID()                                         static BYTE MEM bSrdoCur0x13FEConfigValid_g;
        #define OBD_CREATE_SRDO_GFC_PARAM()                                         static BYTE MEM bSrdoCur0x1300GfcParam_g;
        #define OBD_BEGIN_SRDO_CRC(cnt)                                             static BYTE MEM bSrdo0x13FFCurMaxCRCEntries_g; \
                                                                                    static WORD MEM awSrdo0x13FFCurCRCTable_g[cnt];
        #define OBD_SUBINDEX_SRDO_CRC(sub,name,val)
        #define OBD_END_SRDO_CRC()

    //-------------------------------------------------------------------------------------------
    #elif defined (OBD_CREATE_SUBINDEX_TAB)

        #define OBD_BEGIN()
        #define OBD_END()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_PART_GENERIC()
        #define OBD_BEGIN_PART_MANUFACTURER()
        #define OBD_BEGIN_PART_DEVICE()
        #define OBD_END_PART()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_INDEX_ROM(ind,cnt,call)                                   static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = {
        #define OBD_BEGIN_INDEX_RAM(ind,cnt,call)                                   static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[cnt]; \
                                                                                    static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = {
        #define OBD_END_INDEX(ind)                                                  OBD_END_SUBINDEX()};
        #define OBD_ROM_INDEX_RAM_ARRAY(ind,cnt,call,typ,acc,dtyp,name,def)         static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[OBD_MAX_ARRAY_SUBENTRIES]; \
                                                                                    static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        {0, kObdTypUInt8, kObdAccCR,          &xDef##ind##_0x00_g,   NULL}, \
                                                                                        {1, typ,          (acc)|kObdAccArray, &xDef##ind##_0x01_g,   &axCur##ind##_g[0]}, \
                                                                                        OBD_END_SUBINDEX()};
        #define OBD_ROM_INDEX_RAM_EXTARRAY(ind,cnt,call,typ,acc,dtyp,name,def)      static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[OBD_MAX_ARRAY_SUBENTRIES]; \
                                                                                    static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        {0, kObdTypUInt8, kObdAccCR,          &xDef##ind##_0x00_g,   NULL}, \
                                                                                        {1, typ,          (acc)|kObdAccArray, &xDef##ind##_0x01_g,   &name}, \
                                                                                        OBD_END_SUBINDEX()};
        #define OBD_ROM_INDEX_RAM_VARARRAY(ind,cnt,call,typ,acc,dtyp,name,def)      static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[OBD_MAX_ARRAY_SUBENTRIES]; \
                                                                                    static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        {0, kObdTypUInt8, kObdAccCR,                     &xDef##ind##_0x00_g,   NULL}, \
                                                                                        {1, typ,          (acc)|kObdAccArray|kObdAccVar, &xDef##ind##_0x01_g,   &aVarEntry##ind##_g[0]}, \
                                                                                        OBD_END_SUBINDEX()};

        //---------------------------------------------------------------------------------------
        #define OBD_SUBINDEX_ROM_VAR(ind,sub,typ,acc,dtyp,name,val)                 {sub,typ,           ((acc)&~kObdAccWrite)|kObdAccConst, &xDef##ind##_##sub##_g,   NULL},
        #define OBD_SUBINDEX_RAM_VAR(ind,sub,typ,acc,dtyp,name,val)                 {sub,typ,            (acc),                        &xDef##ind##_##sub##_g,   &xCur##ind##_##sub##_g},
        #define OBD_SUBINDEX_RAM_VAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)     {sub,typ,            (acc)|kObdAccRange,           &xDef##ind##_##sub##_g[0],&xCur##ind##_##sub##_g},
        #define OBD_SUBINDEX_RAM_EXTVAR(ind,sub,typ,acc,dtyp,name,val)              {sub,typ,            (acc),                        &xDef##ind##_##sub##_g,   &name},
        #define OBD_SUBINDEX_RAM_EXTVAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)  {sub,typ,            (acc)|kObdAccRange,           &xDef##ind##_##sub##_g[0],&name},
        #define OBD_SUBINDEX_ROM_VSTRING(ind,sub,acc,name,val)                      {sub,kObdTypVString,((acc)&~kObdAccWrite)|kObdAccConst, &xDef##ind##_##sub##_g,   NULL},
        #define OBD_SUBINDEX_RAM_VAR_NOINIT(ind,sub,typ,acc,dtyp,name)              {sub,typ,            (acc),                        NULL,   &xCur##ind##_##sub##_g},
        #define OBD_SUBINDEX_RAM_EXTVAR_NOINIT(ind,sub,typ,acc,dtyp,name)           {sub,typ,            (acc),                        NULL,   &name},
        #define OBD_SUBINDEX_RAM_VSTRING(ind,sub,acc,name,size,val)                 {sub,kObdTypVString,(acc)/*|kObdAccVar*/,         &xDef##ind##_##sub##_g,   &xCur##ind##_##sub##_g},
        #define OBD_SUBINDEX_RAM_DOMAIN(ind,sub,acc,name)                           {sub,kObdTypDomain, (acc)|kObdAccVar,             NULL,                     &VarEntry##ind##_##sub##_g},
        #define OBD_SUBINDEX_RAM_USERDEF(ind,sub,typ,acc,dtyp,name,val)             {sub,typ,           (acc)|kObdAccVar,             &xDef##ind##_##sub##_g,   &VarEntry##ind##_##sub##_g},
        #define OBD_SUBINDEX_RAM_USERDEF_RG(ind,sub,typ,acc,dtyp,name,val,low,high) {sub,typ,           (acc)|kObdAccVar|kObdAccRange,&xDef##ind##_##sub##_g[0],&VarEntry##ind##_##sub##_g},
        #define OBD_SUBINDEX_RAM_USERDEF_NOINIT(ind,sub,typ,acc,dtyp,name)          {sub,typ,           (acc)|kObdAccVar,             NULL,    &VarEntry##ind##_##sub##_g},

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SDOS(ind,num,cobcs,cobsc,idc)                            static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        {0, kObdTypUInt8,  kObdAccCR,  &bSdos##num##MaxEntries_g, NULL                              }, \
                                                                                        {1, kObdTypUInt32, kObdAccSRW, &dwSdos##num##DefCobCs_g,  &aSdosTable_g[num].m_dwRxCobID    }, \
                                                                                        {2, kObdTypUInt32, kObdAccSRW, &dwSdos##num##DefCobSc_g,  &aSdosTable_g[num].m_dwTxCobID    }, \
                                                                                        {3, kObdTypUInt8,  kObdAccSRW, &bSdos##num##DefIdc_g,     &aSdosTable_g[num].m_bClientNodeID}};
        #define OBD_CREATE_SDOC(ind,num,cobcs,cobsc,ids)                            static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        {0, kObdTypUInt8,  kObdAccCR,  &bSdoc##num##MaxEntries_g, NULL                              }, \
                                                                                        {1, kObdTypUInt32, kObdAccSRW, &dwSdoc##num##DefCobCs_g,  &aSdocTable_g[num].m_dwTxCobId    }, \
                                                                                        {2, kObdTypUInt32, kObdAccSRW, &dwSdoc##num##DefCobSc_g,  &aSdocTable_g[num].m_dwRxCobId    }, \
                                                                                        {3, kObdTypUInt8,  kObdAccSRW, &bSdoc##num##DefIds_g,     &aSdocTable_g[num].m_bServerNodeId}};

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_RXPDO_COMMU(ind,num,cob,typ,inh,evt)                     static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        {0, kObdTypUInt8,  kObdAccCR,  &bRxPdo##num##MaxCommuEntries_g, NULL                              }, \
                                                                                        {1, kObdTypUInt32, kObdAccSRW, &dwRxPdo##num##DefCob_g,        &aPdoRxTable_g[num].m_dwCanId      }, \
                                                                                        {2, kObdTypUInt8,  kObdAccSRW, &bRxPdo##num##DefTyp_g,         &aPdoRxTable_g[num].m_bTxTyp       }, \
                                                                                        {3, kObdTypUInt16, kObdAccSRW, &wRxPdo##num##DefInh_g,         &aPdoRxTable_g[num].m_wInhibitTime }, \
                                                                                        {5, kObdTypUInt16, kObdAccSRW, &wRxPdo##num##DefEvt_g,         &aPdoRxTable_g[num].m_wEventTimer  }}; \
                                                                                    CCM_SUBINDEX_RAM_ONLY (static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[5])
#if (DEV_SYSTEM == _DEV_MSVC_DOS_)
        #define OBD_CREATE_RXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)  static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        { 0, kObdTypUInt8,  kObdAccSRW, &bRxPdo##num##MaxMappEntries_g, &aPdoRxTable_g[num].m_bNoOfMappedObjects          }, \
                                                                                        { 1, kObdTypUInt32, kObdAccSRW, &adwRxPdo##num##DefMapp_g[ 0],  &aPdoRxTable_g[num].m_aMappObjects[ 0].m_dwMapping}, \
                                                                                        { 2, kObdTypUInt32, kObdAccSRW, &adwRxPdo##num##DefMapp_g[ 1],  &aPdoRxTable_g[num].m_aMappObjects[ 1].m_dwMapping}, \
                                                                                        { 3, kObdTypUInt32, kObdAccSRW, &adwRxPdo##num##DefMapp_g[ 2],  &aPdoRxTable_g[num].m_aMappObjects[ 2].m_dwMapping}, \
                                                                                        { 4, kObdTypUInt32, kObdAccSRW, &adwRxPdo##num##DefMapp_g[ 3],  &aPdoRxTable_g[num].m_aMappObjects[ 3].m_dwMapping}, \
                                                                                        { 5, kObdTypUInt32, kObdAccSRW, &adwRxPdo##num##DefMapp_g[ 4],  &aPdoRxTable_g[num].m_aMappObjects[ 4].m_dwMapping}, \
                                                                                        { 6, kObdTypUInt32, kObdAccSRW, &adwRxPdo##num##DefMapp_g[ 5],  &aPdoRxTable_g[num].m_aMappObjects[ 5].m_dwMapping}, \
                                                                                        { 7, kObdTypUInt32, kObdAccSRW, &adwRxPdo##num##DefMapp_g[ 6],  &aPdoRxTable_g[num].m_aMappObjects[ 6].m_dwMapping}, \
                                                                                        { 8, kObdTypUInt32, kObdAccSRW, &adwRxPdo##num##DefMapp_g[ 7],  &aPdoRxTable_g[num].m_aMappObjects[ 7].m_dwMapping}}; \
                                                                                    CCM_SUBINDEX_RAM_ONLY (static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[PDO_MAX_MAPPENTRIES + 1])
#else
        #define OBD_CREATE_RXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)  static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        { 0, kObdTypUInt8,  PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &bRxPdo##num##MaxMappEntries_g, \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_bNoOfMappedObjects)          }, \
                                                                                        { 1, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[ 0],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[ 0].m_dwMapping)}, \
                                                                                        { 2, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[ 1],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[ 1].m_dwMapping)}, \
                                                                                        { 3, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[ 2],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[ 2].m_dwMapping)}, \
                                                                                        { 4, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[ 3],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[ 3].m_dwMapping)}, \
                                                                                        { 5, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[ 4],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[ 4].m_dwMapping)}, \
                                                                                        { 6, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[ 5],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[ 5].m_dwMapping)}, \
                                                                                        { 7, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[ 6],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[ 6].m_dwMapping)}, \
                                                                                        { 8, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[ 7],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[ 7].m_dwMapping)}  \
                                                               PDO_SUBINDEX_BIT_MAPPING ( 9, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[ 8],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[ 8].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (10, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[ 9],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[ 9].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (11, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[10],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[10].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (12, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[11],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[11].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (13, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[12],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[12].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (14, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[13],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[13].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (15, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[14],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[14].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (16, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[15],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[15].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (17, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[16],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[16].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (18, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[17],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[17].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (19, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[18],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[18].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (20, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[19],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[19].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (21, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[20],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[20].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (22, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[21],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[21].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (23, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[22],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[22].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (24, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[23],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[23].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (25, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[24],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[24].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (26, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[25],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[25].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (27, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[26],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[26].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (28, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[27],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[27].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (29, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[28],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[28].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (30, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[29],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[29].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (31, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[30],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[30].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (32, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[31],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[31].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (33, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[32],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[32].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (34, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[33],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[33].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (35, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[34],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[34].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (36, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[35],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[35].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (37, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[36],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[36].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (38, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[37],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[37].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (39, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[38],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[38].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (40, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[39],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[39].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (41, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[40],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[40].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (42, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[41],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[41].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (43, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[42],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[42].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (44, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[43],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[43].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (45, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[44],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[44].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (46, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[45],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[45].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (47, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[46],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[46].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (48, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[47],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[47].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (49, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[48],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[48].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (50, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[49],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[49].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (51, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[50],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[50].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (52, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[51],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[51].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (53, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[52],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[52].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (54, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[53],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[53].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (55, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[54],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[54].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (56, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[55],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[55].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (57, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[56],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[56].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (58, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[57],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[57].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (59, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[58],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[58].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (60, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[59],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[59].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (61, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[60],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[60].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (62, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[61],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[61].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (63, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[62],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[62].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (64, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwRxPdo##num##DefMapp_g[63],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoRxTable_g[num].m_aMappObjects[63].m_dwMapping))}; \
                                                                                    CCM_SUBINDEX_RAM_ONLY (static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[PDO_MAX_MAPPENTRIES + 1])
#endif
        #define OBD_CREATE_TXPDO_COMMU(ind,num,cob,typ,inh,evt)                     static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        {0, kObdTypUInt8,  kObdAccCR,  &bTxPdo##num##MaxCommuEntries_g, NULL                              }, \
                                                                                        {1, kObdTypUInt32, kObdAccSRW, &dwTxPdo##num##DefCob_g,        &aPdoTxTable_g[num].m_dwCanId      }, \
                                                                                        {2, kObdTypUInt8,  kObdAccSRW, &bTxPdo##num##DefTyp_g,         &aPdoTxTable_g[num].m_bTxTyp       }, \
                                                                                        {3, kObdTypUInt16, kObdAccSRW, &wTxPdo##num##DefInh_g,         &aPdoTxTable_g[num].m_wInhibitTime }, \
                                                                                        {5, kObdTypUInt16, kObdAccSRW, &wTxPdo##num##DefEvt_g,         &aPdoTxTable_g[num].m_wEventTimer  }}; \
                                                                                    CCM_SUBINDEX_RAM_ONLY (static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[5])
#if (DEV_SYSTEM == _DEV_MSVC_DOS_)
        #define OBD_CREATE_TXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)  static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        { 0, kObdTypUInt8,  kObdAccSRW, &bTxPdo##num##MaxMappEntries_g, &aPdoTxTable_g[num].m_bNoOfMappedObjects          }, \
                                                                                        { 1, kObdTypUInt32, kObdAccSRW, &adwTxPdo##num##DefMapp_g[ 0],  &aPdoTxTable_g[num].m_aMappObjects[ 0].m_dwMapping}, \
                                                                                        { 2, kObdTypUInt32, kObdAccSRW, &adwTxPdo##num##DefMapp_g[ 1],  &aPdoTxTable_g[num].m_aMappObjects[ 1].m_dwMapping}, \
                                                                                        { 3, kObdTypUInt32, kObdAccSRW, &adwTxPdo##num##DefMapp_g[ 2],  &aPdoTxTable_g[num].m_aMappObjects[ 2].m_dwMapping}, \
                                                                                        { 4, kObdTypUInt32, kObdAccSRW, &adwTxPdo##num##DefMapp_g[ 3],  &aPdoTxTable_g[num].m_aMappObjects[ 3].m_dwMapping}, \
                                                                                        { 5, kObdTypUInt32, kObdAccSRW, &adwTxPdo##num##DefMapp_g[ 4],  &aPdoTxTable_g[num].m_aMappObjects[ 4].m_dwMapping}, \
                                                                                        { 6, kObdTypUInt32, kObdAccSRW, &adwTxPdo##num##DefMapp_g[ 5],  &aPdoTxTable_g[num].m_aMappObjects[ 5].m_dwMapping}, \
                                                                                        { 7, kObdTypUInt32, kObdAccSRW, &adwTxPdo##num##DefMapp_g[ 6],  &aPdoTxTable_g[num].m_aMappObjects[ 6].m_dwMapping}, \
                                                                                        { 8, kObdTypUInt32, kObdAccSRW, &adwTxPdo##num##DefMapp_g[ 7],  &aPdoTxTable_g[num].m_aMappObjects[ 7].m_dwMapping}}; \
                                                                                    CCM_SUBINDEX_RAM_ONLY (static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[PDO_MAX_MAPPENTRIES + 1])
#else
        #define OBD_CREATE_TXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)  static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        { 0, kObdTypUInt8,  PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &bTxPdo##num##MaxMappEntries_g, \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_bNoOfMappedObjects)          }, \
                                                                                        { 1, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[ 0],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[ 0].m_dwMapping)}, \
                                                                                        { 2, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[ 1],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[ 1].m_dwMapping)}, \
                                                                                        { 3, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[ 2],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[ 2].m_dwMapping)}, \
                                                                                        { 4, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[ 3],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[ 3].m_dwMapping)}, \
                                                                                        { 5, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[ 4],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[ 4].m_dwMapping)}, \
                                                                                        { 6, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[ 5],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[ 5].m_dwMapping)}, \
                                                                                        { 7, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[ 6],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[ 6].m_dwMapping)}, \
                                                                                        { 8, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[ 7],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[ 7].m_dwMapping)}  \
                                                               PDO_SUBINDEX_BIT_MAPPING ( 9, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[ 8],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[ 8].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (10, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[ 9],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[ 9].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (11, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[10],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[10].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (12, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[11],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[11].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (13, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[12],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[12].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (14, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[13],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[13].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (15, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[14],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[14].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (16, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[15],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[15].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (17, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[16],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[16].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (18, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[17],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[17].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (19, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[18],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[18].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (20, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[19],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[19].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (21, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[20],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[20].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (22, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[21],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[21].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (23, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[22],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[22].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (24, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[23],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[23].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (25, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[24],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[24].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (26, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[25],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[25].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (27, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[26],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[26].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (28, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[27],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[27].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (29, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[28],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[28].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (30, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[29],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[29].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (31, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[30],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[30].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (32, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[31],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[31].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (33, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[32],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[32].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (34, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[33],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[33].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (35, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[34],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[34].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (36, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[35],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[35].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (37, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[36],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[36].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (38, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[37],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[37].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (39, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[38],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[38].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (40, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[39],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[39].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (41, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[40],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[40].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (42, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[41],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[41].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (43, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[42],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[42].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (44, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[43],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[43].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (45, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[44],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[44].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (46, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[45],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[45].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (47, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[46],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[46].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (48, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[47],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[47].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (49, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[48],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[48].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (50, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[49],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[49].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (51, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[50],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[50].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (52, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[51],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[51].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (53, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[52],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[52].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (54, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[53],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[53].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (55, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[54],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[54].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (56, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[55],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[55].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (57, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[56],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[56].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (58, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[57],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[57].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (59, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[58],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[58].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (60, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[59],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[59].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (61, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[60],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[60].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (62, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[61],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[61].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (63, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[62],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[62].m_dwMapping))  \
                                                               PDO_SUBINDEX_BIT_MAPPING (64, kObdTypUInt32, PDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwTxPdo##num##DefMapp_g[63],  \
                                                                                            PDO_STATIC_MAPPING_ONEOF (NULL,&aPdoTxTable_g[num].m_aMappObjects[63].m_dwMapping))}; \
                                                                                    CCM_SUBINDEX_RAM_ONLY (static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[PDO_MAX_MAPPENTRIES + 1])
#endif

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SRDO_COMMU(ind,num,dir,sct,srvt,cob1,cob2)               static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        {0, kObdTypUInt8,  kObdAccCR,  &bSrdo##num##MaxCommuEntries_g, NULL                            }, \
                                                                                        {1, kObdTypUInt8,  kObdAccSRW, &bSrdo##num##DefDir_g         , &aSrdoTable_g[num].m_bDirection }, \
                                                                                        {2, kObdTypUInt16, kObdAccSRW, &wSrdo##num##DefSct_g         , &aSrdoTable_g[num].m_wSct       }, \
                                                                                        {3, kObdTypUInt8,  kObdAccSRW, &bSrdo##num##DefSrvt_g        , &aSrdoTable_g[num].m_bSrvt      }, \
                                                                                        {4, kObdTypUInt8,  kObdAccCR,  &bSrdo##num##DefTyp_g         , NULL                            }, \
                                                                                        {5, kObdTypUInt32, kObdAccSRW, &dwSrdo##num##DefCob1_g       , &aSrdoTable_g[num].m_dwCobId1   }, \
                                                                                        {6, kObdTypUInt32, kObdAccSRW, &dwSrdo##num##DefCob2_g       , &aSrdoTable_g[num].m_dwCobId2   }}; \
                                                                                    CCM_SUBINDEX_RAM_ONLY (static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[7])
        #define OBD_BEGIN_SRDO_MAPP(ind,num,cnt)                                    CCM_SUBINDEX_RAM_ONLY (static tObdSubEntry MEM aObdSubEntry##ind##Ram_g[cnt+1]) \
                                                                                    static CONST tObdSubEntry ROM aObdSubEntry##ind##Rom_g[] = { \
                                                                                        {0  ,  kObdTypUInt8,  SRDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &bSrdo##num##DefMaxMappEntries_g, \
                                                                                            SRDO_STATIC_MAPPING_ONEOF (NULL,&aSrdoTable_g[num].m_bNoOfMappedObjects)         },
        #define OBD_SUBINDEX_SRDO_MAPP(ind,sub,num,name,val)                            {sub,  kObdTypUInt32, SRDO_STATIC_MAPPING_ONEOF (kObdAccCR,kObdAccSRW), &adwSrdo##num##DefMapp_g[(sub)-1], \
                                                                                            SRDO_STATIC_MAPPING_ONEOF (NULL,&aSrdoTable_g[num].m_aMappObjects[(sub)-1].m_dwMapping)},
        #define OBD_END_SRDO_MAPP(ind)                                                  {0,0,0,NULL,NULL}}; 

        #define OBD_CREATE_SRDO_CFG_VALID()                                         static CONST tObdSubEntry ROM aObdSubEntry0x13FERom_g[] = { \
                                                                                        {0, kObdTypUInt8,  kObdAccRW,  &bSrdoDef0x13FEConfigValid_g, &bSrdoCur0x13FEConfigValid_g }}; \
                                                                                    CCM_SUBINDEX_RAM_ONLY (static tObdSubEntry MEM aObdSubEntry0x13FERam_g[1])
        #define OBD_CREATE_SRDO_GFC_PARAM()                                         static CONST tObdSubEntry ROM aObdSubEntry0x1300Rom_g[] = { \
                                                                                        {0, kObdTypUInt8,  kObdAccRW,  &bSrdoDef0x1300GfcParam_g, &bSrdoCur0x1300GfcParam_g }}; \
                                                                                    CCM_SUBINDEX_RAM_ONLY (static tObdSubEntry MEM aObdSubEntry0x1300Ram_g[1])
        #define OBD_BEGIN_SRDO_CRC(cnt)                                             CCM_SUBINDEX_RAM_ONLY (static tObdSubEntry MEM aObdSubEntry0x13FFRam_g[(cnt)+1]) \
                                                                                    static CONST tObdSubEntry ROM aObdSubEntry0x13FFRom_g[] = { \
                                                                                        {0,    kObdTypUInt8,  kObdAccRW, &bSrdo0x13FFDefMaxCRCEntries_g,  &bSrdo0x13FFCurMaxCRCEntries_g},
        #define OBD_SUBINDEX_SRDO_CRC(sub,name,val)                                     {sub,  kObdTypUInt16, kObdAccRW, &awSrdo0x13FFDefCRCTable_g[(sub)-1], &awSrdo0x13FFCurCRCTable_g[(sub)-1]},
        #define OBD_END_SRDO_CRC()                                                      {0,0,0,NULL,NULL}};

    //-------------------------------------------------------------------------------------------
    #elif defined (OBD_CREATE_INDEX_TAB)

        #define OBD_BEGIN()
        #define OBD_END()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_PART_GENERIC()                                            static CONST tObdEntry ROM aObdTabGeneric_g[]      = {
        #define OBD_BEGIN_PART_MANUFACTURER()                                       static CONST tObdEntry ROM aObdTabManufacturer_g[] = {
        #define OBD_BEGIN_PART_DEVICE()                                             static CONST tObdEntry ROM aObdTabDevice_g[]       = {
        #define OBD_END_PART()                                                      {OBD_TABLE_INDEX_END,(tObdSubEntryPtr)&dwObd_OBK_g,0,NULL}};

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_INDEX_ROM(ind,cnt,call)                                   {ind,(tObdSubEntryPtr)&aObdSubEntry##ind##Rom_g[0],cnt,(tObdCallback)call},
        #define OBD_BEGIN_INDEX_RAM(ind,cnt,call)                                   {ind,(tObdSubEntryPtr)&aObdSubEntry##ind##Ram_g[0],cnt,(tObdCallback)call},
        #define OBD_END_INDEX(ind)
        #define OBD_ROM_INDEX_RAM_ARRAY(ind,cnt,call,typ,acc,dtyp,name,def)         {ind,(tObdSubEntryPtr)&aObdSubEntry##ind##Ram_g[0],(cnt)+1,(tObdCallback)call},
        #define OBD_ROM_INDEX_RAM_EXTARRAY(ind,cnt,call,typ,acc,dtyp,name,def)      {ind,(tObdSubEntryPtr)&aObdSubEntry##ind##Ram_g[0],(cnt)+1,(tObdCallback)call},
        #define OBD_ROM_INDEX_RAM_VARARRAY(ind,cnt,call,typ,acc,dtyp,name,def)      {ind,(tObdSubEntryPtr)&aObdSubEntry##ind##Ram_g[0],(cnt)+1,(tObdCallback)call},

        //---------------------------------------------------------------------------------------
        #define OBD_SUBINDEX_ROM_VAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_VAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_VAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_RAM_EXTVAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_EXTVAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_ROM_VSTRING(ind,sub,acc,name,val)
        #define OBD_SUBINDEX_RAM_VSTRING(ind,sub,acc,name,size,val)
        #define OBD_SUBINDEX_RAM_VAR_NOINIT(ind,sub,typ,acc,dtyp,name)
        #define OBD_SUBINDEX_RAM_EXTVAR_NOINIT(ind,sub,typ,acc,dtyp,name)
        #define OBD_SUBINDEX_RAM_DOMAIN(ind,sub,acc,name)
        #define OBD_SUBINDEX_RAM_USERDEF(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_USERDEF_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_RAM_USERDEF_NOINIT(ind,sub,typ,acc,dtyp,name)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SDOS(ind,num,cobcs,cobsc,idc)                            {ind,(tObdSubEntryPtr)&aObdSubEntry##ind##Rom_g[0],4,(tObdCallback)SdosCbConfigServer},
        #define OBD_CREATE_SDOC(ind,num,cobcs,cobsc,ids)                            {ind,(tObdSubEntryPtr)&aObdSubEntry##ind##Rom_g[0],4,(tObdCallback)SdocCbConfigClient},

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_RXPDO_COMMU(ind,num,cob,typ,inh,evt)                     {ind,(tObdSubEntryPtr)CCM_SUBINDEX_RAM_ONEOF(&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0]),5,(tObdCallback)PdoCbConfigCommu},
        #define OBD_CREATE_RXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)  {ind,(tObdSubEntryPtr)CCM_SUBINDEX_RAM_ONEOF(&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0]),PDO_MAX_MAPPENTRIES + 1, \
                                                                                            (tObdCallback)PDO_STATIC_MAPPING_ONEOF (NULL,PdoCbConfigMapp)},
        #define OBD_CREATE_TXPDO_COMMU(ind,num,cob,typ,inh,evt)                     {ind,(tObdSubEntryPtr)CCM_SUBINDEX_RAM_ONEOF(&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0]),5,(tObdCallback)PdoCbConfigCommu},
        #define OBD_CREATE_TXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)  {ind,(tObdSubEntryPtr)CCM_SUBINDEX_RAM_ONEOF(&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0]),PDO_MAX_MAPPENTRIES + 1, \
                                                                                            (tObdCallback)PDO_STATIC_MAPPING_ONEOF (NULL,PdoCbConfigMapp)},

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SRDO_COMMU(ind,num,dir,sct,srvt,cob1,cob2)               {ind,(tObdSubEntryPtr)CCM_SUBINDEX_RAM_ONEOF(&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0]),7,       (tObdCallback)SrdoCbConfig},
        #define OBD_BEGIN_SRDO_MAPP(ind,num,cnt)                                    {ind,(tObdSubEntryPtr)CCM_SUBINDEX_RAM_ONEOF(&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0]),(cnt)+1, \
                                                                                            (tObdCallback)SRDO_STATIC_MAPPING_ONEOF (NULL,SrdoCbConfigMapp)},
        #define OBD_SUBINDEX_SRDO_MAPP(ind,sub,num,name,val)                        
        #define OBD_END_SRDO_MAPP(ind)                                              
        #define OBD_CREATE_SRDO_CFG_VALID()                                         {0x13FE,(tObdSubEntryPtr)CCM_SUBINDEX_RAM_ONEOF(&aObdSubEntry0x13FERam_g[0],&aObdSubEntry0x13FERom_g[0]),1,      (tObdCallback)SrdoCbConfig},
        #define OBD_CREATE_SRDO_GFC_PARAM()                                         {0x1300,(tObdSubEntryPtr)CCM_SUBINDEX_RAM_ONEOF(&aObdSubEntry0x1300Ram_g[0],&aObdSubEntry0x1300Rom_g[0]),1,      (tObdCallback)SrdoCbConfig},
        #define OBD_BEGIN_SRDO_CRC(cnt)                                             {0x13FF,(tObdSubEntryPtr)CCM_SUBINDEX_RAM_ONEOF(&aObdSubEntry0x13FFRam_g[0],&aObdSubEntry0x13FFRom_g[0]),(cnt)+1,(tObdCallback)SrdoCbConfig},
        #define OBD_SUBINDEX_SRDO_CRC(sub,name,val)
        #define OBD_END_SRDO_CRC()

    //-------------------------------------------------------------------------------------------
    #elif defined (OBD_CREATE_INIT_FUNCTION)

        #define OBD_BEGIN()
        #define OBD_END()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_PART_GENERIC()                                            pInitParam->m_ObdInitParam.m_pGenericPart      = (tObdEntryPtr) &aObdTabGeneric_g[0];
        #define OBD_BEGIN_PART_MANUFACTURER()                                       pInitParam->m_ObdInitParam.m_pManufacturerPart = (tObdEntryPtr) &aObdTabManufacturer_g[0];
        #define OBD_BEGIN_PART_DEVICE()                                             pInitParam->m_ObdInitParam.m_pDevicePart       = (tObdEntryPtr) &aObdTabDevice_g[0];
        #define OBD_END_PART()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_INDEX_ROM(ind,cnt,call)
        #define OBD_BEGIN_INDEX_RAM(ind,cnt,call)
        #define OBD_END_INDEX(ind)
        #define OBD_ROM_INDEX_RAM_ARRAY(ind,cnt,call,typ,acc,dtyp,name,def)
        #define OBD_ROM_INDEX_RAM_EXTARRAY(ind,cnt,call,typ,acc,dtyp,name,def)
        #define OBD_ROM_INDEX_RAM_VARARRAY(ind,cnt,call,typ,acc,dtyp,name,def)

        //---------------------------------------------------------------------------------------
        #define OBD_SUBINDEX_ROM_VAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_VAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_VAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_RAM_EXTVAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_EXTVAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_ROM_VSTRING(ind,sub,acc,name,val)
        #define OBD_SUBINDEX_RAM_VSTRING(ind,sub,acc,name,size,val)
        #define OBD_SUBINDEX_RAM_VAR_NOINIT(ind,sub,typ,acc,dtyp,name)
        #define OBD_SUBINDEX_RAM_EXTVAR_NOINIT(ind,sub,typ,acc,dtyp,name)
        #define OBD_SUBINDEX_RAM_DOMAIN(ind,sub,acc,name)
        #define OBD_SUBINDEX_RAM_USERDEF(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_USERDEF_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_RAM_USERDEF_NOINIT(ind,sub,typ,acc,dtyp,name)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SDOS(inx,num,cobcs,cobsc,idc)
        #define OBD_CREATE_SDOC(inx,num,cobcs,cobsc,ids)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_RXPDO_COMMU(ind,num,cob,typ,inh,evt)
        #define OBD_CREATE_RXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)
        #define OBD_CREATE_TXPDO_COMMU(ind,num,cob,typ,inh,evt)
        #define OBD_CREATE_TXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SRDO_COMMU(ind,num,dir,sct,srvt,cob1,cob2)
        #define OBD_BEGIN_SRDO_MAPP(ind,num,cnt)                                    SRDO_STATIC_MAPPING_ONEOF (aSrdoTable_g[num].m_pbNoOfMappedObjects = &bSrdo##num##DefMaxMappEntries_g;,) \
                                                                                    SRDO_STATIC_MAPPING_ONEOF (aSrdoTable_g[num].m_pdwMappObjects = &adwSrdo##num##DefMapp_g[0];,)
        #define OBD_SUBINDEX_SRDO_MAPP(ind,sub,num,name,val)                        
        #define OBD_END_SRDO_MAPP(ind)                                              
        #define OBD_CREATE_SRDO_CFG_VALID()
        #define OBD_CREATE_SRDO_GFC_PARAM()
        #define OBD_BEGIN_SRDO_CRC(cnt)
        #define OBD_SUBINDEX_SRDO_CRC(sub,name,val)                                 aSrdoTable_g[(sub)-1].m_pwCheckSum = &awSrdo0x13FFCurCRCTable_g[(sub)-1];
        #define OBD_END_SRDO_CRC()

    //-------------------------------------------------------------------------------------------
    #elif defined (OBD_CREATE_INIT_SUBINDEX)

        #define OBD_BEGIN()
        #define OBD_END()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_PART_GENERIC()
        #define OBD_BEGIN_PART_MANUFACTURER()
        #define OBD_BEGIN_PART_DEVICE()
        #define OBD_END_PART()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_INDEX_ROM(ind,cnt,call)
        #define OBD_BEGIN_INDEX_RAM(ind,cnt,call)                                   CCM_SUBINDEX_RAM_ONLY (COP_MEMCPY (&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0],sizeof(aObdSubEntry##ind##Ram_g)));
        #define OBD_END_INDEX(ind)
        #define OBD_ROM_INDEX_RAM_ARRAY(ind,cnt,call,typ,acc,dtyp,name,def)         COP_MEMCPY (&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0],sizeof(aObdSubEntry##ind##Ram_g));
        #define OBD_ROM_INDEX_RAM_EXTARRAY(ind,cnt,call,typ,acc,dtyp,name,def)      COP_MEMCPY (&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0],sizeof(aObdSubEntry##ind##Ram_g));
        #define OBD_ROM_INDEX_RAM_VARARRAY(ind,cnt,call,typ,acc,dtyp,name,def)      COP_MEMCPY (&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0],sizeof(aObdSubEntry##ind##Ram_g));

        //---------------------------------------------------------------------------------------
        #define OBD_SUBINDEX_ROM_VAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_VAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_VAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_RAM_EXTVAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_EXTVAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_ROM_VSTRING(ind,sub,acc,name,val)
        #define OBD_SUBINDEX_RAM_VSTRING(ind,sub,acc,name,size,val)
        #define OBD_SUBINDEX_RAM_VAR_NOINIT(ind,sub,typ,acc,dtyp,name)
        #define OBD_SUBINDEX_RAM_EXTVAR_NOINIT(ind,sub,typ,acc,dtyp,name)
        #define OBD_SUBINDEX_RAM_DOMAIN(ind,sub,acc,name)
        #define OBD_SUBINDEX_RAM_USERDEF(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_USERDEF_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_RAM_USERDEF_NOINIT(ind,sub,typ,acc,dtyp,name)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SDOS(inx,num,cobcs,cobsc,idc)
        #define OBD_CREATE_SDOC(inx,num,cobcs,cobsc,ids)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_RXPDO_COMMU(ind,num,cob,typ,inh,evt)                     CCM_SUBINDEX_RAM_ONLY (COP_MEMCPY (&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0],sizeof(aObdSubEntry##ind##Ram_g)));
        #define OBD_CREATE_RXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)  CCM_SUBINDEX_RAM_ONLY (COP_MEMCPY (&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0],sizeof(aObdSubEntry##ind##Ram_g)));
        #define OBD_CREATE_TXPDO_COMMU(ind,num,cob,typ,inh,evt)                     CCM_SUBINDEX_RAM_ONLY (COP_MEMCPY (&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0],sizeof(aObdSubEntry##ind##Ram_g)));
        #define OBD_CREATE_TXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)  CCM_SUBINDEX_RAM_ONLY (COP_MEMCPY (&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0],sizeof(aObdSubEntry##ind##Ram_g)));

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SRDO_COMMU(ind,num,dir,sct,srvt,cob1,cob2)               CCM_SUBINDEX_RAM_ONLY (COP_MEMCPY (&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0],sizeof(aObdSubEntry##ind##Ram_g)));
        #define OBD_BEGIN_SRDO_MAPP(ind,num,cnt)                                    CCM_SUBINDEX_RAM_ONLY (COP_MEMCPY (&aObdSubEntry##ind##Ram_g[0],&aObdSubEntry##ind##Rom_g[0],sizeof(aObdSubEntry##ind##Ram_g)));
        #define OBD_SUBINDEX_SRDO_MAPP(ind,sub,num,name,val)
        #define OBD_END_SRDO_MAPP(ind)                                              
        #define OBD_CREATE_SRDO_CFG_VALID()                                         CCM_SUBINDEX_RAM_ONLY (COP_MEMCPY (&aObdSubEntry0x13FERam_g[0],&aObdSubEntry0x13FERom_g[0],sizeof(aObdSubEntry0x13FERam_g)));
        #define OBD_CREATE_SRDO_GFC_PARAM()                                         CCM_SUBINDEX_RAM_ONLY (COP_MEMCPY (&aObdSubEntry0x1300Ram_g[0],&aObdSubEntry0x1300Rom_g[0],sizeof(aObdSubEntry0x1300Ram_g)));
        #define OBD_BEGIN_SRDO_CRC(cnt)
        #define OBD_SUBINDEX_SRDO_CRC(sub,name,val)                                 CCM_SUBINDEX_RAM_ONLY (COP_MEMCPY (&aObdSubEntry0x13FFRam_g[0],&aObdSubEntry0x13FFRom_g[0],sizeof(aObdSubEntry0x13FFRam_g)));
        #define OBD_END_SRDO_CRC()

    //-------------------------------------------------------------------------------------------
    #else

        #define OBD_BEGIN()
        #define OBD_END()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_PART_GENERIC()
        #define OBD_BEGIN_PART_MANUFACTURER()
        #define OBD_BEGIN_PART_DEVICE()
        #define OBD_END_PART()

        //---------------------------------------------------------------------------------------
        #define OBD_BEGIN_INDEX_ROM(ind,cnt,call)
        #define OBD_BEGIN_INDEX_RAM(ind,cnt,call)
        #define OBD_END_INDEX(ind)
        #define OBD_ROM_INDEX_RAM_ARRAY(ind,cnt,call,typ,acc,dtyp,name,def)
        #define OBD_ROM_INDEX_RAM_EXTARRAY(ind,cnt,call,typ,acc,dtyp,name,def)
        #define OBD_ROM_INDEX_RAM_VARARRAY(ind,cnt,call,typ,acc,dtyp,name,def)

        //---------------------------------------------------------------------------------------
        #define OBD_SUBINDEX_ROM_VAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_VAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_VAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_RAM_EXTVAR(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_EXTVAR_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_ROM_VSTRING(ind,sub,acc,name,val)
        #define OBD_SUBINDEX_RAM_VSTRING(ind,sub,acc,name,sizes,val)
        #define OBD_SUBINDEX_RAM_VAR_NOINIT(ind,sub,typ,acc,dtyp,name)
        #define OBD_SUBINDEX_RAM_EXTVAR_NOINIT(ind,sub,typ,acc,dtyp,name)
        #define OBD_SUBINDEX_RAM_DOMAIN(ind,sub,acc,name)
        #define OBD_SUBINDEX_RAM_USERDEF(ind,sub,typ,acc,dtyp,name,val)
        #define OBD_SUBINDEX_RAM_USERDEF_RG(ind,sub,typ,acc,dtyp,name,val,low,high)
        #define OBD_SUBINDEX_RAM_USERDEF_NOINIT(ind,sub,typ,acc,dtyp,name)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SDOS(inx,num,cobcs,cobsc,idc)
        #define OBD_CREATE_SDOC(inx,num,cobcs,cobsc,ids)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_RXPDO_COMMU(ind,num,cob,typ,inh,evt)
        #define OBD_CREATE_RXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)
        #define OBD_CREATE_TXPDO_COMMU(ind,num,cob,typ,inh,evt)
        #define OBD_CREATE_TXPDO_MAPP(ind,num,cnt,ob1,ob2,ob3,ob4,ob5,ob6,ob7,ob8)

        //---------------------------------------------------------------------------------------
        #define OBD_CREATE_SRDO_COMMU(ind,num,dir,sct,srvt,cob1,cob2)
        #define OBD_BEGIN_SRDO_MAPP(ind,num,cnt)
        #define OBD_SUBINDEX_SRDO_MAPP(ind,sub,num,name,val)                        
        #define OBD_END_SRDO_MAPP(ind)                                              
        #define OBD_CREATE_SRDO_CFG_VALID()
        #define OBD_CREATE_SRDO_GFC_PARAM()
        #define OBD_BEGIN_SRDO_CRC(cnt)
        #define OBD_SUBINDEX_SRDO_CRC(sub,name,val)
        #define OBD_END_SRDO_CRC()

    #endif

    //-------------------------------------------------------------------------------------------
#elif defined (OBD_UNDEFINE_MACRO)

    #undef OBD_BEGIN
    #undef OBD_END

    //---------------------------------------------------------------------------------------
    #undef OBD_BEGIN_PART_GENERIC
    #undef OBD_BEGIN_PART_MANUFACTURER
    #undef OBD_BEGIN_PART_DEVICE
    #undef OBD_END_PART

    //---------------------------------------------------------------------------------------
    #undef OBD_BEGIN_INDEX_ROM
    #undef OBD_BEGIN_INDEX_RAM
    #undef OBD_END_INDEX
    #undef OBD_ROM_INDEX_RAM_ARRAY
    #undef OBD_ROM_INDEX_RAM_EXTARRAY
    #undef OBD_ROM_INDEX_RAM_VARARRAY

    //---------------------------------------------------------------------------------------
    #undef OBD_SUBINDEX_ROM_VAR
    #undef OBD_SUBINDEX_RAM_VAR
    #undef OBD_SUBINDEX_RAM_VAR_RG
    #undef OBD_SUBINDEX_RAM_EXTVAR
    #undef OBD_SUBINDEX_RAM_EXTVAR_RG
    #undef OBD_SUBINDEX_ROM_VSTRING
    #undef OBD_SUBINDEX_RAM_VSTRING
    #undef OBD_SUBINDEX_RAM_VAR_NOINIT
    #undef OBD_SUBINDEX_RAM_EXTVAR_NOINIT
    #undef OBD_SUBINDEX_RAM_DOMAIN
    #undef OBD_SUBINDEX_RAM_USERDEF
    #undef OBD_SUBINDEX_RAM_USERDEF_RG
    #undef OBD_SUBINDEX_RAM_USERDEF_NOINIT

    //---------------------------------------------------------------------------------------
    #undef OBD_CREATE_SDOS
    #undef OBD_CREATE_SDOC

    //---------------------------------------------------------------------------------------
    #undef OBD_CREATE_RXPDO_COMMU
    #undef OBD_CREATE_RXPDO_MAPP
    #undef OBD_CREATE_TXPDO_COMMU
    #undef OBD_CREATE_TXPDO_MAPP

    //---------------------------------------------------------------------------------------
    #undef OBD_CREATE_SRDO_COMMU
    #undef OBD_BEGIN_SRDO_MAPP
    #undef OBD_SUBINDEX_SRDO_MAPP
    #undef OBD_END_SRDO_MAPP
    #undef OBD_CREATE_SRDO_CFG_VALID
    #undef OBD_CREATE_SRDO_GFC_PARAM
    #undef OBD_BEGIN_SRDO_CRC
    #undef OBD_SUBINDEX_SRDO_CRC
    #undef OBD_END_SRDO_CRC

#endif


// Please keep an empty line at the end of this file.

