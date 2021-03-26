/* ptp-pack.c
 *
 * Copyright (C) 2001-2004 Mariusz Woloszyn <emsi@ipartners.pl>
 * Copyright (C) 2003-2017 Marcus Meissner <marcus@jet.franken.de>
 * Copyright (C) 2006-2008 Linus Walleij <triad@df.lth.se>
 * Copyright (C) 2007 Tero Saarni <tero.saarni@gmail.com>
 * Copyright (C) 2009 Axel Waggershauser <awagger@web.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include "libmtp.h"
#include "ptp.h"
#include "gphoto2-endian.h"
#include "string.h"
#include "mtp_adapter.h"
#include "time.h"
#include "pathutils.h"

#ifndef UINT_MAX
#define UINT_MAX 0xFFFFFFFF
#endif
#if defined(HAVE_ICONV) && defined(HAVE_LANGINFO_H)
#include <iconv.h>
#endif

//static inline u16 htod16p (PTPParams *params, u16 var){
//    return ((params->byteorder == PTP_DL_LE)?htole16(var):htobe16(var));
//}
//
//static inline u32 htod32p (PTPParams *params, u32 var){
//    return ((params->byteorder == PTP_DL_LE)?htole32(var):htobe32(var));
//}

static inline void htod16ap (PTPParams *params, unsigned char *a, u16 val){
    if (params->byteorder == PTP_DL_LE)
        htole16a(a,val);
    else
        htobe16a(a,val);
}

static inline void htod32ap (PTPParams *params, unsigned char *a, u32 val){
    if (params->byteorder == PTP_DL_LE)
        htole32a(a,val);
    else
        htobe32a(a,val);
}

static inline void htod64ap (PTPParams *params, unsigned char *a, u64 val){
    if (params->byteorder == PTP_DL_LE)
        htole64a(a,val);
    else
        htobe64a(a,val);
}

//static inline u16 dtoh16p (PTPParams *params, u16 var){
//    return ((params->byteorder == PTP_DL_LE)?le16toh(var):be16toh(var));
//}
//
//static inline u32 dtoh32p (PTPParams *params, u32 var){
//    return ((params->byteorder == PTP_DL_LE)?le32toh(var):be32toh(var));
//}
//
//static inline u64 dtoh64p (PTPParams *params, u64 var){
//    return ((params->byteorder == PTP_DL_LE)?le64toh(var):be64toh(var));
//}

static inline u16 dtoh16ap (PTPParams *params, const unsigned char *a){
    return ((params->byteorder == PTP_DL_LE)?le16atoh(a):be16atoh(a));
}

static inline u32 dtoh32ap (PTPParams *params, const unsigned char *a){
    return ((params->byteorder == PTP_DL_LE)?le32atoh(a):be32atoh(a));
}

static inline u64 dtoh64ap (PTPParams *params, const unsigned char *a){
    return ((params->byteorder == PTP_DL_LE)?le64atoh(a):be64atoh(a));
}

#define htod8a(a,x)     *(u8*)(a) = x
#define htod16a(a,x)    htod16ap(params,a,x)
#define htod32a(a,x)    htod32ap(params,a,x)
#define htod64a(a,x)    htod64ap(params,a,x)
#define htod16(x)       htod16p(params,x)
#define htod32(x)       htod32p(params,x)
#define htod64(x)       htod64p(params,x)

#define dtoh8a(x)       (*(uint8_t*)(x))
#define dtoh16a(a)      dtoh16ap(params,a)
#define dtoh32a(a)      dtoh32ap(params,a)
#define dtoh64a(a)      dtoh64ap(params,a)
#define dtoh16(x)       dtoh16p(params,x)
#define dtoh32(x)       dtoh32p(params,x)
#define dtoh64(x)       dtoh64p(params,x)

/**
 * PTP����ַ���
 *
 * @param params PTP����
 * @param data ���ݻ���
 * @param offset ����ƫ��
 * @param total �����ܳ���
 * @param len �������ַ�������
 * @param retstr Ҫ���صĽ����ַ�������
 * @return �ɹ�����1
 */
static inline int
ptp_unpack_string(PTPParams *params, unsigned char* data, u16 offset, u32 total, u8 *len, char **retstr)
{
    u8 length;
    u16 string[PTP_MAXSTRLEN + 1];
    /* ���� UTF-8: ÿ��UCS-2�ַ����3���ֽڣ��������յĿ�ֵ */
    char loclstr[PTP_MAXSTRLEN*3 + 1];
    size_t nconv;
    char *dest;
#if defined(HAVE_ICONV) && defined(HAVE_LANGINFO_H)
    char *src;
    size_t srclen, destlen;
#endif
    *len = 0;
    *retstr = NULL;
    /* ��������ܳ��Ƿ�Ϸ�*/
    if (offset + 1 > total)
        return 0;

    /* PTP_MAXSTRLEN == 255, 8λ���� */
    /* ��ȡ�ַ����ĳ���*/
    length = dtoh8a(&data[offset]);
    if (length == 0) {
        *len = 0;
        return 1;
    }
    /* ����ַ����ĳ����Ƿ�Ϸ�*/
    if (offset + 1 + length*sizeof(string[0]) > total)
        return 0;
    /* ��ֵ�ַ����ĳ���*/
    *len = length;

    /* �ó����� */
    memcpy(string, &data[offset+1], length * sizeof(string[0]));
    /* ������ֹ��*/
    string[length] = 0x0000U;
    loclstr[0] = '\0';

    /* convert from camera UCS-2 to our locale */
    dest = loclstr;
    nconv = (size_t) - 1;
#if defined(HAVE_ICONV) && defined(HAVE_LANGINFO_H)
    src = (char *)string;
    srclen = length * sizeof(string[0]);
    destlen = sizeof(loclstr) - 1;
    if (params->cd_ucs2_to_locale != (iconv_t)-1)
        nconv = iconv(params->cd_ucs2_to_locale, &src, &srclen, &dest, &destlen);
#endif
    if (nconv == (size_t) - 1) { /* do it the hard way */
        int i;
        /* try the old way, in case iconv is broken */
        for (i = 0;i < length; i++) {
            if (dtoh16a(&data[offset + 1 + 2*i]) > 127)
                loclstr[i] = '?';
            else
                loclstr[i] = dtoh16a(&data[offset + 1 + 2*i]);
        }
        dest = loclstr + length;
    }
    *dest = '\0';
    loclstr[sizeof(loclstr)-1] = '\0';   /* be safe? */
    /* �ѽ����������ַ�����������*/
    *retstr = mtp_strdup(loclstr);
    return 1;
}

/**
 * ����unicode�ַ����ĳ���
 *
 * @param unicstr unicode�ַ���
 *
 * @return �����ַ�������(��������ֹ��)
 **/
static inline int
ucs2strlen(u16 const * const unicstr)
{
    int length = 0;

    /* Unicode�ַ�����ֹ��2 * 0x00*/
    for(length = 0; unicstr[length] != 0x0000U; length ++);
    return length;
}

/**
 * PTP����ַ���
 *
 * @param params PTP����
 * @param string Ҫ������ַ���
 * @param data   ���صĴ���������
 * @param offset ƫ��
 * @param len    �����ַ����ĳ���
 **/
static inline void
ptp_pack_string(PTPParams *params, char *string, unsigned char* data, u16 offset, u8 *len)
{
    int packedlen = 0;
    u16 ucs2str[PTP_MAXSTRLEN + 1];
    char *ucs2strp = (char *) ucs2str;
    size_t convlen = strlen(string);

    /* ���ܳ���255(PTP_MAXSTRLEN)����Ϊ����һ���ֽ�*/
    memset(ucs2strp, 0, sizeof(ucs2str));  /* XXX: necessary? */
#if defined(HAVE_ICONV) && defined(HAVE_LANGINFO_H)
    if (params->cd_locale_to_ucs2 != (iconv_t)-1) {
        size_t nconv;
        size_t convmax = PTP_MAXSTRLEN * 2; /* Includes the terminator */
        char *stringp = string;

        nconv = iconv(params->cd_locale_to_ucs2, &stringp, &convlen,
            &ucs2strp, &convmax);
        if (nconv == (size_t) -1)
            ucs2str[0] = 0x0000U;
    } else
#endif
    {
        unsigned int i;

        for (i = 0;i < convlen;i++) {
            ucs2str[i] = string[i];
        }
        ucs2str[convlen] = 0;
    }
    /*
     * XXX: isn't packedlen just ( (uint16_t *)ucs2strp - ucs2str )?
     *      why do we need ucs2strlen()?
     */
    /* ��ȡunicode�ַ����ĳ���*/
    packedlen = ucs2strlen(ucs2str);
    /* ��鳤���Ƿ�Ϸ�*/
    if (packedlen > PTP_MAXSTRLEN - 1) {
        *len=0;
        return;
    }

    /* �ַ���������������ֹ��(PTP ��׼ȷ��) */
    htod8a(&data[offset], packedlen + 1);
    memcpy(&data[offset + 1], &ucs2str[0], packedlen * sizeof(ucs2str[0]));
    htod16a(&data[offset + packedlen * 2 + 1], 0x0000);  /* ��ֹ�� */

    /* ���س������ַ�������*/
    *len = (u8) packedlen + 1;
}

/**
 * PTP��ȡ������ַ�������
 *
 * @param params      PTP����
 * @param string      Ҫ������ַ���
 * @param packed_size ���ش������ַ����ĳ���
 *
 * @return �ɹ����ش������ַ���
 */
static inline unsigned char *
ptp_get_packed_stringcopy(PTPParams *params, char *string, u32 *packed_size)
{
    u8 packed[PTP_MAXSTRLEN * 2 + 3], len;
    size_t plen;
    unsigned char *retcopy = NULL;

    if (string == NULL)
        ptp_pack_string(params, "", (unsigned char*) packed, 0, &len);
    else
        ptp_pack_string(params, string, (unsigned char*) packed, 0, &len);

    /* �����ַ����ĳ��ȣ�����һ���ֽڴ洢�ַ�������*/
    plen = len * 2 + 1;

    retcopy = mtp_malloc(plen);
    if (!retcopy) {
        *packed_size = 0;
        return NULL;
    }
    memcpy(retcopy, packed, plen);
    *packed_size = plen;
    return (retcopy);
}

/**
 * PTP��ȡ32λ����
 *
 * @param params PTP����
 * @param data ���ݻ���
 * @param offset ����ƫ��
 * @param total �����ܳ���
 * @param array ���������ݻ���ĵ�ַ
 * @return �ɹ����ػ�ȡ�������ݳ���
 */
static inline u32
ptp_unpack_uint32_t_array(PTPParams *params, unsigned char* data, unsigned int offset, unsigned int datalen, u32 **array)
{
    u32 n, i=0;

    if (!data)
        return 0;

    /* ƫ�ƴ������ݳ���*/
    if (offset >= datalen)
        return 0;

    /* ƫ�Ƽ���4�ֽڴ������ݳ���*/
    if (offset + sizeof(u32) > datalen)
        return 0;

    *array = NULL;
    /* ��ȡ32λ���鳤��*/
    n = dtoh32a(&data[offset]);
    /* ������鳤���Ƿ�������ĳ���*/
    if (n >= UINT_MAX/sizeof(u32))
        return 0;
    if (!n)
        return 0;
    /* ���ƫ�Ƽ��������ܳ����Ƿ�������ݳ���*/
    if (offset + sizeof(u32)*(n + 1) > datalen) {
        ptp_debug (params ,"array runs over datalen bufferend (%d vs %d)", offset + sizeof(uint32_t) * (n + 1) , datalen);
        return 0;
    }
    /* �����ڴ�*/
    *array = mtp_malloc (n * sizeof(u32));
    if (!*array)
        return 0;
    /* ��ȡ����*/
    for (i = 0; i < n; i++)
        (*array)[i] = dtoh32a(&data[offset + (sizeof(u32) * (i + 1))]);
    return n;
}

/**
 * PTP���32λ����
 *
 * @param params   PTP����
 * @param array    Ҫ�����32λ����
 * @param arraylen 32λ���鳤��
 * @param data     ���������ݻ���
 *
 * @return �ɹ����ش��������ݵĳ���
 */
static inline u32 ptp_pack_uint32_t_array(PTPParams *params, u32 *array, u32 arraylen, unsigned char **data )
{
    u32 i = 0;

    *data = mtp_malloc ((arraylen + 1)*sizeof(u32));
    if (!*data)
        return 0;
    htod32a(&(*data)[0],arraylen);
    for (i = 0;i < arraylen;i++)
        htod32a(&(*data)[sizeof(u32)*(i + 1)], array[i]);
    return (arraylen + 1)*sizeof(u32);
}

/**
 * PTP��ȡ16λ����
 *
 * @param params PTP����
 * @param data ���ݻ���
 * @param offset ����ƫ��
 * @param total �����ܳ���
 * @param array ���������ݻ���ĵ�ַ
 * @return �ɹ����ػ�ȡ�������ݳ���
 */
static inline u32
ptp_unpack_uint16_t_array(PTPParams *params, unsigned char* data, unsigned int offset, unsigned int datalen, u16 **array)
{
    u32 n, i=0;

    if (!data)
        return 0;
    *array = NULL;

    /* ƫ�ƺ�4�ֽڴ�����ݳ���*/
    if (datalen - offset < sizeof(u32))
        return 0;
    /* ��ȡ���ݳ���*/
    n = dtoh32a(&data[offset]);
    /* ������ݳ����Ƿ�Ϸ�*/
    if (n >= (UINT_MAX - offset - sizeof(u32))/sizeof(u16))
        return 0;
    /* ���ݳ���Ϊ0*/
    if (!n)
        return 0;

    if (offset + sizeof(u32) > datalen)
        return 0;

    /* ��������Ƿ�Ϸ�*/
    if (offset + sizeof(u32) + sizeof(u16)*n > datalen) {
        ptp_debug (params ,"array runs over datalen bufferend (%d vs %d)", offset + sizeof(u32) + n*sizeof(u16) , datalen);
        return 0;
    }
    /* ��������*/
    *array = mtp_malloc (n * sizeof(u16));
    if (!*array)
        return 0;
    /* ת���ֽ���*/
    for (i = 0;i < n;i++)
        (*array)[i] = dtoh16a(&data[offset + (sizeof(u16) * (i + 2))]);
    return n;
}


#define PTP_di_StandardVersion        0
#define PTP_di_VendorExtensionID      2
#define PTP_di_VendorExtensionVersion 6
#define PTP_di_VendorExtensionDesc    8
#define PTP_di_FunctionalMode         8
#define PTP_di_OperationsSupported    10

/**
 *  PTP�����豸��Ϣ
 *
 * @param params PTP����
 * @param data Ҫ���������ݻ���
 * @param di �洢����������ݵĽṹ��
 * @param datalen Ҫ���������ݵĳ���
 * @return �ɹ�����1
 */
static inline
int ptp_unpack_DI (PTPParams *params, unsigned char* data, PTPDeviceInfo *di, unsigned int datalen)
{
    u8 len;
    unsigned int totallen;

    if (!data) return 0;
    if (datalen < 12) return 0;
    memset (di, 0, sizeof(*di));
    /* �豸��Ϣ��׼�汾*/
    di->StandardVersion = dtoh16a(&data[PTP_di_StandardVersion]);
    /* �Զ�����չID*/
    di->VendorExtensionID = dtoh32a(&data[PTP_di_VendorExtensionID]);
    /* �Զ�����չ�汾*/
    di->VendorExtensionVersion = dtoh16a(&data[PTP_di_VendorExtensionVersion]);
    /* ��ȡ�Զ�����չ����*/
    if (!ptp_unpack_string(params, data,
        PTP_di_VendorExtensionDesc,
        datalen,
        &len,
        &di->VendorExtensionDesc)){
        return 0;
    }
    /* ������ݳ���*/
    totallen = len * 2 + 1;
    if (datalen <= totallen + PTP_di_FunctionalMode + sizeof(u16)) {
        ptp_debug (params, "datalen %d <= totallen + PTP_di_FunctionalMode + sizeof(uint16_t) %d", datalen, totallen + PTP_di_FunctionalMode + sizeof(uint16_t));
        return 0;
    }
    /* ��ȡ����ģʽ*/
    di->FunctionalMode = dtoh16a(&data[PTP_di_FunctionalMode + totallen]);
    /* ֧�ֵĲ���*/
    di->OperationsSupported_len = ptp_unpack_uint16_t_array(params, data,
                                                  PTP_di_OperationsSupported + totallen,
                                                  datalen,
                                                  &di->OperationsSupported);
    totallen = totallen + di->OperationsSupported_len * sizeof(u16) + sizeof(u32);
    if (datalen <= totallen + PTP_di_OperationsSupported) {
        ptp_debug (params, "datalen %d <= totallen + PTP_di_OperationsSupported %d 1", datalen, totallen + PTP_di_OperationsSupported);
        return 0;
    }
    /* �¼�֧��*/
    di->EventsSupported_len = ptp_unpack_uint16_t_array(params, data,
                                                PTP_di_OperationsSupported + totallen,
                                                datalen,
                                                &di->EventsSupported);
    totallen = totallen + di->EventsSupported_len * sizeof(u16) + sizeof(u32);
    if (datalen <= totallen+PTP_di_OperationsSupported) {
        ptp_debug (params, "datalen %d <= totallen + PTP_di_OperationsSupported %d 2", datalen, totallen+PTP_di_OperationsSupported);
        return 0;
    }
    /* �豸����*/
    di->DevicePropertiesSupported_len = ptp_unpack_uint16_t_array(params, data,
                                            PTP_di_OperationsSupported + totallen,
                                            datalen,
                                            &di->DevicePropertiesSupported);
    totallen = totallen + di->DevicePropertiesSupported_len * sizeof(u16) + sizeof(u32);
    if (datalen <= totallen + PTP_di_OperationsSupported) {
        ptp_debug (params, "datalen %d <= totallen + PTP_di_OperationsSupported %d 3", datalen, totallen+PTP_di_OperationsSupported);
        return 0;
    }
    /* �����ʽ*/
    di->CaptureFormats_len = ptp_unpack_uint16_t_array(params, data,
                                        PTP_di_OperationsSupported + totallen,
                                        datalen,
                                        &di->CaptureFormats);
    totallen = totallen + di->CaptureFormats_len * sizeof(u16) + sizeof(u32);
    if (datalen <= totallen + PTP_di_OperationsSupported) {
        ptp_debug (params, "datalen %d <= totallen + PTP_di_OperationsSupported %d 4", datalen, totallen+PTP_di_OperationsSupported);
        return 0;
    }
    /* ֧�ֵ�ͼ���ʽ*/
    di->ImageFormats_len = ptp_unpack_uint16_t_array(params, data,
                                            PTP_di_OperationsSupported + totallen,
                                            datalen,
                                            &di->ImageFormats);
    totallen = totallen+di->ImageFormats_len * sizeof(u16) + sizeof(u32);
    if (datalen <= totallen+PTP_di_OperationsSupported) {
        ptp_debug (params, "datalen %d <= totallen + PTP_di_OperationsSupported %d 5", datalen, totallen+PTP_di_OperationsSupported);
        return 0;
    }
    /* ����*/
    if (!ptp_unpack_string(params, data,
        PTP_di_OperationsSupported + totallen,
        datalen,
        &len,
        &di->Manufacturer)){
        return 0;
    }
    totallen += len * 2 + 1;
    /* ��Щ��Ϣ������Ҳû��ϵ */
    if (datalen <= totallen + PTP_di_OperationsSupported) {
        ptp_debug (params, "datalen %d <= totallen + PTP_di_OperationsSupported %d 6", datalen, totallen+PTP_di_OperationsSupported);
        return 1;
    }
    if (!ptp_unpack_string(params, data,
        PTP_di_OperationsSupported + totallen,
        datalen,
        &len,
        &di->Model)){
        return 1;
    }
    totallen += len * 2 + 1;
    /* ��Щ��Ϣ������Ҳû��ϵ */
    if (datalen <= totallen + PTP_di_OperationsSupported) {
        ptp_debug (params, "datalen %d <= totallen + PTP_di_OperationsSupported %d 7", datalen, totallen+PTP_di_OperationsSupported);
        return 1;
    }
    /* �豸�汾*/
    if (!ptp_unpack_string(params, data,
        PTP_di_OperationsSupported + totallen,
        datalen,
        &len,
        &di->DeviceVersion)){
        return 1;
    }
    totallen += len * 2 + 1;
    /* ��Щ��Ϣ������Ҳû��ϵ */
    if (datalen <= totallen + PTP_di_OperationsSupported) {
        ptp_debug (params, "datalen %d <= totallen + PTP_di_OperationsSupported %d 8", datalen, totallen+PTP_di_OperationsSupported);
        return 1;
    }
    /* ���к�*/
    if (!ptp_unpack_string(params, data,
        PTP_di_OperationsSupported + totallen,
        datalen,
        &len,
        &di->SerialNumber)){
        return 1;
    }
    return 1;
}

/**
 *  PTP�ͷ��豸��Ϣ
 *
 * @param di �豸��Ϣ�ṹ��
 */
inline static void ptp_free_DI (PTPDeviceInfo *di) {
    mtp_free (di->SerialNumber);
    mtp_free (di->DeviceVersion);
    mtp_free (di->Model);
    mtp_free (di->Manufacturer);
    mtp_free (di->ImageFormats);
    mtp_free (di->CaptureFormats);
    mtp_free (di->VendorExtensionDesc);
    mtp_free (di->OperationsSupported);
    mtp_free (di->EventsSupported);
    mtp_free (di->DevicePropertiesSupported);
    memset(di, 0, sizeof(*di));
}


/* ���������� ���/��� */
#define PTP_oh               0

static inline void
ptp_unpack_OH (PTPParams *params, unsigned char* data, PTPObjectHandles *oh, unsigned int len)
{
    if (len) {
        oh->n = ptp_unpack_uint32_t_array(params, data, PTP_oh, len, &oh->Handler);
    } else {
        oh->n = 0;
        oh->Handler = NULL;
    }
}

/* �洢ID������/��� */
#define PTP_sids             0

static inline void
ptp_unpack_SIDs (PTPParams *params, unsigned char* data, PTPStorageIDs *sids, unsigned int len)
{
    sids->n = 0;
    sids->Storage = NULL;

    if (!data || !len)
        return;
    /* ��ȡ32λ������*/
    sids->n = ptp_unpack_uint32_t_array(params, data, PTP_sids, len, &sids->Storage);
}

/* �洢��Ϣ���/��� */

#define PTP_si_StorageType       0
#define PTP_si_FilesystemType        2
#define PTP_si_AccessCapability      4
#define PTP_si_MaxCapability         6
#define PTP_si_FreeSpaceInBytes     14
#define PTP_si_FreeSpaceInImages    22
#define PTP_si_StorageDescription   26

static inline int
ptp_unpack_SI (PTPParams *params, unsigned char* data, PTPStorageInfo *si, unsigned int len)
{
    u8 storagedescriptionlen;

    if (len < 26)
        return 0;
    si->StorageType = dtoh16a(&data[PTP_si_StorageType]);
    si->FilesystemType = dtoh16a(&data[PTP_si_FilesystemType]);
    si->AccessCapability = dtoh16a(&data[PTP_si_AccessCapability]);
    si->MaxCapability = dtoh64a(&data[PTP_si_MaxCapability]);
    si->FreeSpaceInBytes = dtoh64a(&data[PTP_si_FreeSpaceInBytes]);
    si->FreeSpaceInImages = dtoh32a(&data[PTP_si_FreeSpaceInImages]);

    /* FIXME: check more lengths here */
    if (!ptp_unpack_string(params, data,
        PTP_si_StorageDescription,
        len,
        &storagedescriptionlen,
        &si->StorageDescription))
        return 0;

    if (!ptp_unpack_string(params, data,
        PTP_si_StorageDescription + storagedescriptionlen * 2 + 1,
        len,
        &storagedescriptionlen,
        &si->VolumeLabel)) {
        ptp_debug(params, "could not unpack storage description");
        return 0;
    }
    return 1;
}

/* ������Ϣ ���/��� */

#define PTP_oi_StorageID         0
#define PTP_oi_ObjectFormat      4
#define PTP_oi_ProtectionStatus      6
#define PTP_oi_ObjectCompressedSize  8
#define PTP_oi_ThumbFormat      12
#define PTP_oi_ThumbCompressedSize  14
#define PTP_oi_ThumbPixWidth        18
#define PTP_oi_ThumbPixHeight       22
#define PTP_oi_ImagePixWidth        26
#define PTP_oi_ImagePixHeight       30
#define PTP_oi_ImageBitDepth        34
#define PTP_oi_ParentObject     38
#define PTP_oi_AssociationType      42
#define PTP_oi_AssociationDesc      44
#define PTP_oi_SequenceNumber       48
#define PTP_oi_filenamelen      52
#define PTP_oi_Filename         53

/* ����0�������ݵ���󳤶ȣ�������Ҫ3���ֽ�*/
#define PTP_oi_MaxLen PTP_oi_Filename + (PTP_MAXSTRLEN + 1) * 2 + 3

static inline u32
ptp_pack_OI (PTPParams *params, PTPObjectInfo *oi, unsigned char** oidataptr)
{
    unsigned char* oidata;
    u8 filenamelen;
    u8 capturedatelen=0;

    /* �ȷ����ڴ棬����0��������*/
    oidata = mtp_malloc(PTP_oi_MaxLen + params->ocs64 * 4);
    *oidataptr = oidata;
    /* ������Ӧ�����ͷ���ʹ��*/
#if 0
    char *capture_date="20020101T010101"; /* XXX Fake date */
#endif
    memset (oidata, 0, PTP_oi_MaxLen + params->ocs64 * 4);
    htod32a(&oidata[PTP_oi_StorageID], oi->StorageID);
    htod16a(&oidata[PTP_oi_ObjectFormat], oi->ObjectFormat);
    htod16a(&oidata[PTP_oi_ProtectionStatus], oi->ProtectionStatus);
    htod32a(&oidata[PTP_oi_ObjectCompressedSize], oi->ObjectCompressedSize);
    if (params->ocs64)
        oidata += 4;
    htod16a(&oidata[PTP_oi_ThumbFormat], oi->ThumbFormat);
    htod32a(&oidata[PTP_oi_ThumbCompressedSize], oi->ThumbCompressedSize);
    htod32a(&oidata[PTP_oi_ThumbPixWidth], oi->ThumbPixWidth);
    htod32a(&oidata[PTP_oi_ThumbPixHeight], oi->ThumbPixHeight);
    htod32a(&oidata[PTP_oi_ImagePixWidth], oi->ImagePixWidth);
    htod32a(&oidata[PTP_oi_ImagePixHeight], oi->ImagePixHeight);
    htod32a(&oidata[PTP_oi_ImageBitDepth], oi->ImageBitDepth);
    htod32a(&oidata[PTP_oi_ParentObject], oi->ParentObject);
    htod16a(&oidata[PTP_oi_AssociationType], oi->AssociationType);
    htod32a(&oidata[PTP_oi_AssociationDesc], oi->AssociationDesc);
    htod32a(&oidata[PTP_oi_SequenceNumber], oi->SequenceNumber);

    ptp_pack_string(params, oi->Filename, oidata, PTP_oi_filenamelen, &filenamelen);
/*
    filenamelen=(uint8_t)strlen(oi->Filename);
    htod8a(&req->data[PTP_oi_filenamelen],filenamelen+1);
    for (i=0;i<filenamelen && i< PTP_MAXSTRLEN; i++) {
        req->data[PTP_oi_Filename+i*2]=oi->Filename[i];
    }
*/
    /*
     *XXX Fake date.
     * for example Kodak sets Capture date on the basis of EXIF data.
     * Spec says that this field is from perspective of Initiator.
     */
#if 0   /* seems now we don't need any data packed in OI dataset... for now ;)*/
    capturedatelen=strlen(capture_date);
    htod8a(&data[PTP_oi_Filename+(filenamelen+1)*2],
        capturedatelen+1);
    for (i=0;i<capturedatelen && i< PTP_MAXSTRLEN; i++) {
        data[PTP_oi_Filename+(i+filenamelen+1)*2+1]=capture_date[i];
    }
    htod8a(&data[PTP_oi_Filename+(filenamelen+capturedatelen+2)*2+1],
        capturedatelen+1);
    for (i=0;i<capturedatelen && i< PTP_MAXSTRLEN; i++) {
        data[PTP_oi_Filename+(i+filenamelen+capturedatelen+2)*2+2]=
          capture_date[i];
    }
#endif
    /* XXX this function should return dataset length */
    return (PTP_oi_Filename+filenamelen * 2 + (capturedatelen + 1) * 3) + params->ocs64 * 4;
}

/**
 * PTP���ʱ��
 *
 * @param str Ҫ�������ַ���
 **/
static time_t ptp_unpack_PTPTIME (const char *str) {
    char ptpdate[40];
    char tmp[5];
    size_t  ptpdatelen;
    struct tm tm;

    if (!str)
        return 0;
    ptpdatelen = strlen(str);
    /* ����ַ�������*/
    if (ptpdatelen >= sizeof (ptpdate)) {
        /*ptp_debug (params ,"datelen is larger then size of buffer", ptpdatelen, (int)sizeof(ptpdate));*/
        return 0;
    }
    if (ptpdatelen < 15) {
        /*ptp_debug (params ,"datelen is less than 15 (%d)", ptpdatelen);*/
        return 0;
    }
    strncpy (ptpdate, str, sizeof(ptpdate));
    ptpdate[sizeof(ptpdate) - 1] = '\0';

    memset(&tm,0,sizeof(tm));
    /* ��*/
    strncpy (tmp, ptpdate, 4);
    tmp[4] = 0;
    tm.tm_year = atoi (tmp) - 1900;
    /* ��*/
    strncpy (tmp, ptpdate + 4, 2);
    tmp[2] = 0;
    tm.tm_mon = atoi (tmp) - 1;
    /* ��*/
    strncpy (tmp, ptpdate + 6, 2);
    tmp[2] = 0;
    tm.tm_mday = atoi (tmp);
    /* ʱ*/
    strncpy (tmp, ptpdate + 9, 2);
    tmp[2] = 0;
    tm.tm_hour = atoi (tmp);
    /* ��*/
    strncpy (tmp, ptpdate + 11, 2);
    tmp[2] = 0;
    tm.tm_min = atoi (tmp);
    /* ��*/
    strncpy (tmp, ptpdate + 13, 2);
    tmp[2] = 0;
    tm.tm_sec = atoi (tmp);
    tm.tm_isdst = -1;
    /* ת��Ϊ����*/
    return mktime (&tm);
}

/**
 * PTP���������Ϣ
 *
 * @param params   PTP����
 * @param data     ԭʼ���ݻ���
 * @param oi       Ҫ���صĶ�����Ϣ�ṹ��
 * @param len      ԭʼ���ݳ���
 **/
static inline void
ptp_unpack_OI (PTPParams *params, unsigned char* data, PTPObjectInfo *oi, unsigned int len)
{
    u8 filenamelen;
    u8 capturedatelen;
    char *capture_date;

    /* ������ݳ���*/
    if (len < PTP_oi_SequenceNumber)
        return;

    oi->Filename = oi->Keywords = NULL;

    /* FIXME: ����������ַ����ĳ��� */
    oi->StorageID = dtoh32a(&data[PTP_oi_StorageID]);
    oi->ObjectFormat = dtoh16a(&data[PTP_oi_ObjectFormat]);
    oi->ProtectionStatus = dtoh16a(&data[PTP_oi_ProtectionStatus]);
    oi->ObjectCompressedSize = dtoh32a(&data[PTP_oi_ObjectCompressedSize]);

    /* Stupid Samsung Galaxy developers emit a 64bit objectcompressedsize */
    if ((data[PTP_oi_filenamelen] == 0) && (data[PTP_oi_filenamelen + 4] != 0)) {
        ptp_debug (params, "objectsize 64bit detected!");
        params->ocs64 = 1;
        data += 4;
        len -= 4;
    }
    oi->ThumbFormat = dtoh16a(&data[PTP_oi_ThumbFormat]);
    oi->ThumbCompressedSize = dtoh32a(&data[PTP_oi_ThumbCompressedSize]);
    oi->ThumbPixWidth = dtoh32a(&data[PTP_oi_ThumbPixWidth]);
    oi->ThumbPixHeight = dtoh32a(&data[PTP_oi_ThumbPixHeight]);
    oi->ImagePixWidth = dtoh32a(&data[PTP_oi_ImagePixWidth]);
    oi->ImagePixHeight = dtoh32a(&data[PTP_oi_ImagePixHeight]);
    oi->ImageBitDepth = dtoh32a(&data[PTP_oi_ImageBitDepth]);
    oi->ParentObject = dtoh32a(&data[PTP_oi_ParentObject]);
    oi->AssociationType = dtoh16a(&data[PTP_oi_AssociationType]);
    oi->AssociationDesc = dtoh32a(&data[PTP_oi_AssociationDesc]);
    oi->SequenceNumber = dtoh32a(&data[PTP_oi_SequenceNumber]);
    /* ��ȡ�ļ�����*/
    ptp_unpack_string(params, data, PTP_oi_filenamelen, len, &filenamelen, &oi->Filename);
    ptp_unpack_string(params, data, PTP_oi_filenamelen + filenamelen*2 + 1, len, &capturedatelen, &capture_date);
    /* ISO 8601���Ӽ�, ����ʮ��֮һ���ʱ��*/
    oi->CaptureDate = ptp_unpack_PTPTIME(capture_date);
    mtp_free(capture_date);

    /* �����޸�����*/
    ptp_unpack_string(params, data,
        PTP_oi_filenamelen + filenamelen * 2
        + capturedatelen * 2 + 2, len, &capturedatelen, &capture_date);
    /* ��ȡʱ��*/
    oi->ModificationDate = ptp_unpack_PTPTIME(capture_date);
    mtp_free(capture_date);
}

/**
 * ��ȡ���ݺ���
 * @param target Ҫ��ȡ������
 * @param func   ������
 **/
#define CTVAL(target,func) {            \
    if (total - *offset < sizeof(target))   \
        return 0;           \
    target = func(&data[*offset]);      \
    *offset += sizeof(target);      \
}

/**
 * ��ȡ���ݺ���
 * @param val    MTP���Խṹ��
 * @param member ����ֵ�ĳ�Ա
 * @param func   ������
 **/
#define RARR(val,member,func)   {           \
    unsigned int n,j;               \
    if (total - *offset < sizeof(u32))     \
        return 0;               \
    n = dtoh32a (&data[*offset]);           \
    *offset += sizeof(u32);            \
                            \
    if (n >= UINT_MAX/sizeof(val->a.v[0]))      \
        return 0;               \
    if (n > (total - (*offset))/sizeof(val->a.v[0]))\
        return 0;               \
    val->a.count = n;               \
    val->a.v = mtp_malloc(sizeof(val->a.v[0])*n);   \
    if (!val->a.v) return 0;            \
    for (j = 0;j < n;j++)               \
        CTVAL(val->a.v[j].member, func);    \
}

/**
 * PTP����豸����ֵ
 *
 * @param params   PTP����
 * @param data     ԭʼ���ݻ���
 * @param offset   ƫ��
 * @param total    ԭʼ���ݳ���
 * @param value    ����ֵ
 * @param datatype ��������
 * @return �ɹ�����1��
 **/
static inline unsigned int
ptp_unpack_DPV (PTPParams *params, unsigned char* data, unsigned int *offset, unsigned int total,
    PTPPropertyValue* value, u16 datatype) {

    /* ƫ�ƴ��������ܳ���*/
    if (*offset >= total)
        return 0;
    /* �����������ͻ�ȡ����*/
    switch (datatype) {
    case PTP_DTC_INT8:
        CTVAL(value->i8,dtoh8a);
        break;
    case PTP_DTC_UINT8:
        CTVAL(value->u8,dtoh8a);
        break;
    case PTP_DTC_INT16:
        CTVAL(value->i16,dtoh16a);
        break;
    case PTP_DTC_UINT16:
        CTVAL(value->u16,dtoh16a);
        break;
    case PTP_DTC_INT32:
        CTVAL(value->i32,dtoh32a);
        break;
    case PTP_DTC_UINT32:
        CTVAL(value->u32,dtoh32a);
        break;
    case PTP_DTC_INT64:
        CTVAL(value->i64,dtoh64a);
        break;
    case PTP_DTC_UINT64:
        CTVAL(value->u64,dtoh64a);
        break;

    case PTP_DTC_UINT128:
        *offset += 16;
        /*fprintf(stderr,"unhandled unpack of uint128n");*/
        break;
    case PTP_DTC_INT128:
        *offset += 16;
        /*fprintf(stderr,"unhandled unpack of int128n");*/
        break;

    case PTP_DTC_AINT8:
        RARR(value,i8,dtoh8a);
        break;
    case PTP_DTC_AUINT8:
        RARR(value,u8,dtoh8a);
        break;
    case PTP_DTC_AUINT16:
        RARR(value,u16,dtoh16a);
        break;
    case PTP_DTC_AINT16:
        RARR(value,i16,dtoh16a);
        break;
    case PTP_DTC_AUINT32:
        RARR(value,u32,dtoh32a);
        break;
    case PTP_DTC_AINT32:
        RARR(value,i32,dtoh32a);
        break;
    case PTP_DTC_AUINT64:
        RARR(value,u64,dtoh64a);
        break;
    case PTP_DTC_AINT64:
        RARR(value,i64,dtoh64a);
        break;
    /* XXX: ������int�����ͻ�û��ʵ�� */
    /* XXX: ������int�����黹û��ʵ�� */
    case PTP_DTC_STR: {
        uint8_t len;
        /* XXX: ����С */
        if (*offset >= total + 1)
            return 0;
        /* ����ַ���*/
        if (!ptp_unpack_string(params,data,*offset,total,&len,&value->str))
            return 0;
        *offset += len * 2 + 1;
        break;
    }
    default:
        return 0;
    }
    return 1;
}

/* �豸���� ���/��� */
#define PTP_dpd_DevicePropertyCode  0
#define PTP_dpd_DataType        2
#define PTP_dpd_GetSet          4
#define PTP_dpd_FactoryDefaultValue 5

/**
 * PTP����豸����������
 * params
 */
static inline int
ptp_unpack_DPD (PTPParams *params, unsigned char* data, PTPDevicePropDesc *dpd, unsigned int dpdlen)
{
    unsigned int offset = 0, ret;

    memset (dpd, 0, sizeof(*dpd));
    if (dpdlen <= 5)
        return 0;
    /* ��ȡ�豸���Դ���*/
    dpd->DevicePropertyCode = dtoh16a(&data[PTP_dpd_DevicePropertyCode]);
    /* ��ȡ��������*/
    dpd->DataType = dtoh16a(&data[PTP_dpd_DataType]);
    dpd->GetSet = dtoh8a(&data[PTP_dpd_GetSet]);
    dpd->FormFlag = PTP_DPFF_None;

    offset = PTP_dpd_FactoryDefaultValue;
    /* ����Ĭ�����Բ���*/
    ret = ptp_unpack_DPV (params, data, &offset, dpdlen, &dpd->FactoryDefaultValue, dpd->DataType);
    if (!ret)
        goto outofmemory;
    if ((dpd->DataType == PTP_DTC_STR) && (offset == dpdlen))
        return 1;
    ret = ptp_unpack_DPV (params, data, &offset, dpdlen, &dpd->CurrentValue, dpd->DataType);
    if (!ret)
        goto outofmemory;
    /* ���offset==0����˴��벻֧���������͸�ʽ�����������������ַ���(�������ַ���Ϊֵ)��
     * ���������Form Flag��Ҫ����Ϊ0x00����FORM������*/
    if (offset + sizeof(u8) > dpdlen)
        return 1;

    dpd->FormFlag = dtoh8a(&data[offset]);
    offset += sizeof(u8);

    switch (dpd->FormFlag) {
    case PTP_DPFF_Range:
        ret = ptp_unpack_DPV (params, data, &offset, dpdlen, &dpd->FORM.Range.MinimumValue, dpd->DataType);
        if (!ret)
            goto outofmemory;
        ret = ptp_unpack_DPV (params, data, &offset, dpdlen, &dpd->FORM.Range.MaximumValue, dpd->DataType);
        if (!ret)
            goto outofmemory;
        ret = ptp_unpack_DPV (params, data, &offset, dpdlen, &dpd->FORM.Range.StepSize, dpd->DataType);
        if (!ret)
            goto outofmemory;
        break;
    case PTP_DPFF_Enumeration: {
        int i;
#define N   dpd->FORM.Enum.NumberOfValues

        if (offset + sizeof(u16) > dpdlen)
            goto outofmemory;

        N = dtoh16a(&data[offset]);
        offset += sizeof(u16);
        dpd->FORM.Enum.SupportedValue = mtp_malloc(N*sizeof(dpd->FORM.Enum.SupportedValue[0]));
        if (!dpd->FORM.Enum.SupportedValue)
            goto outofmemory;

        memset (dpd->FORM.Enum.SupportedValue,0 , N*sizeof(dpd->FORM.Enum.SupportedValue[0]));
        for (i  =0;i < N;i++) {
            ret = ptp_unpack_DPV (params, data, &offset, dpdlen, &dpd->FORM.Enum.SupportedValue[i], dpd->DataType);

            /* ����Ĵ������в�ͬ��HP PhotoSmart 120ָ����N�ڴ���˵�ö����00 01������01 00��
              * �������ǽ�enum���������ݰ��Ľ�β��
              */
            if (!ret) {
                if (!i)
                    goto outofmemory;
                dpd->FORM.Enum.NumberOfValues = i;
                break;
            }
        }
        }
    }
#undef N
    return 1;
outofmemory:
    ptp_free_devicepropdesc(dpd);
    return 0;
}

#define PTP_opd_ObjectPropertyCode  0
#define PTP_opd_DataType            2
#define PTP_opd_GetSet              4
#define PTP_opd_FactoryDefaultValue 5

static inline int
ptp_unpack_OPD (PTPParams *params, unsigned char* data, PTPObjectPropDesc *opd, unsigned int opdlen)
{
    unsigned int offset = 0, ret;

    memset (opd, 0, sizeof(*opd));

    if (opdlen < 5)
        return 0;

    opd->ObjectPropertyCode = dtoh16a(&data[PTP_opd_ObjectPropertyCode]);
    opd->DataType = dtoh16a(&data[PTP_opd_DataType]);
    opd->GetSet = dtoh8a(&data[PTP_opd_GetSet]);

    offset = PTP_opd_FactoryDefaultValue;
    ret = ptp_unpack_DPV (params, data, &offset, opdlen, &opd->FactoryDefaultValue, opd->DataType);
    if (!ret)
        goto outofmemory;

    if (offset + sizeof(uint32_t) > opdlen)
        goto outofmemory;
    opd->GroupCode = dtoh32a(&data[offset]);
    offset += sizeof(u32);

    if (offset + sizeof(u8) > opdlen)
        goto outofmemory;
    opd->FormFlag = dtoh8a(&data[offset]);
    offset += sizeof(u8);

    switch (opd->FormFlag) {
    case PTP_OPFF_Range:
        ret = ptp_unpack_DPV (params, data, &offset, opdlen, &opd->FORM.Range.MinimumValue, opd->DataType);
        if (!ret)
            goto outofmemory;
        ret = ptp_unpack_DPV (params, data, &offset, opdlen, &opd->FORM.Range.MaximumValue, opd->DataType);
        if (!ret)
            goto outofmemory;
        ret = ptp_unpack_DPV (params, data, &offset, opdlen, &opd->FORM.Range.StepSize, opd->DataType);
        if (!ret)
            goto outofmemory;
        break;
    case PTP_OPFF_Enumeration: {
        unsigned int i;
#define N   opd->FORM.Enum.NumberOfValues

        if (offset + sizeof(u16) > opdlen)
            goto outofmemory;
        N = dtoh16a(&data[offset]);
        offset += sizeof(u16);

        opd->FORM.Enum.SupportedValue = mtp_malloc(N * sizeof(opd->FORM.Enum.SupportedValue[0]));
        if (!opd->FORM.Enum.SupportedValue)
            goto outofmemory;

        memset (opd->FORM.Enum.SupportedValue,0 , N*sizeof(opd->FORM.Enum.SupportedValue[0]));
        for (i = 0;i < N;i++) {
            ret = ptp_unpack_DPV (params, data, &offset, opdlen, &opd->FORM.Enum.SupportedValue[i], opd->DataType);

            /* ����Ĵ������в�ͬ��HP PhotoSmart 120ָ����N�ڴ���˵�ö����00 01������01 00��
             * �������ǽ�enum���������ݰ��Ľ�β��
             */
            if (!ret) {
                if (!i)
                    goto outofmemory;
                opd->FORM.Enum.NumberOfValues = i;
                break;
            }
        }
#undef N
        }
    }
    return 1;
outofmemory:
    ptp_free_objectpropdesc(opd);
    return 0;
}

static inline u32
ptp_pack_DPV (PTPParams *params, PTPPropertyValue* value, unsigned char** dpvptr, u16 datatype)
{
    unsigned char* dpv = NULL;
    u32 size = 0;
    unsigned int i;

    switch (datatype) {
    case PTP_DTC_INT8:
        size = sizeof(s8);
        dpv = mtp_malloc(size);
        htod8a(dpv, value->i8);
        break;
    case PTP_DTC_UINT8:
        size = sizeof(u8);
        dpv = mtp_malloc(size);
        htod8a(dpv, value->u8);
        break;
    case PTP_DTC_INT16:
        size = sizeof(s16);
        dpv = mtp_malloc(size);
        htod16a(dpv, value->i16);
        break;
    case PTP_DTC_UINT16:
        size = sizeof(u16);
        dpv = mtp_malloc(size);
        htod16a(dpv,value->u16);
        break;
    case PTP_DTC_INT32:
        size = sizeof(s32);
        dpv = mtp_malloc(size);
        htod32a(dpv, value->i32);
        break;
    case PTP_DTC_UINT32:
        size = sizeof(u32);
        dpv = mtp_malloc(size);
        htod32a(dpv,value->u32);
        break;
    case PTP_DTC_INT64:
        size = sizeof(s64);
        dpv = mtp_malloc(size);
        htod64a(dpv, value->i64);
        break;
    case PTP_DTC_UINT64:
        size = sizeof(u64);
        dpv = mtp_malloc(size);
        htod64a(dpv, value->u64);
        break;
    case PTP_DTC_AUINT8:
        size = sizeof(u32) + value->a.count * sizeof(u8);
        dpv = mtp_malloc(size);
        htod32a(dpv, value->a.count);
        for (i = 0;i < value->a.count;i++)
            htod8a(&dpv[sizeof(u32) + i * sizeof(u8)], value->a.v[i].u8);
        break;
    case PTP_DTC_AINT8:
        size = sizeof(u32) + value->a.count * sizeof(s8);
        dpv = mtp_malloc(size);
        htod32a(dpv, value->a.count);
        for (i = 0;i < value->a.count;i++)
            htod8a(&dpv[sizeof(u32) + i * sizeof(s8)], value->a.v[i].i8);
        break;
    case PTP_DTC_AUINT16:
        size = sizeof(u32) + value->a.count * sizeof(u16);
        dpv = mtp_malloc(size);
        htod32a(dpv, value->a.count);
        for (i = 0;i < value->a.count;i++)
            htod16a(&dpv[sizeof(u32) + i * sizeof(u16)], value->a.v[i].u16);
        break;
    case PTP_DTC_AINT16:
        size = sizeof(u32) + value->a.count * sizeof(s16);
        dpv = mtp_malloc(size);
        htod32a(dpv, value->a.count);
        for (i = 0;i < value->a.count;i++)
            htod16a(&dpv[sizeof(u32) + i * sizeof(s16)], value->a.v[i].i16);
        break;
    case PTP_DTC_AUINT32:
        size = sizeof(u32) + value->a.count * sizeof(u32);
        dpv = mtp_malloc(size);
        htod32a(dpv, value->a.count);
        for (i = 0;i < value->a.count;i++)
            htod32a(&dpv[sizeof(u32) + i * sizeof(u32)], value->a.v[i].u32);
        break;
    case PTP_DTC_AINT32:
        size = sizeof(u32) + value->a.count * sizeof(s32);
        dpv = mtp_malloc(size);
        htod32a(dpv, value->a.count);
        for (i = 0;i < value->a.count;i++)
            htod32a(&dpv[sizeof(u32) + i * sizeof(s32)], value->a.v[i].i32);
        break;
    case PTP_DTC_AUINT64:
        size = sizeof(u32) + value->a.count * sizeof(u64);
        dpv = mtp_malloc(size);
        htod32a(dpv, value->a.count);
        for (i = 0;i < value->a.count;i++)
            htod64a(&dpv[sizeof(u32) + i * sizeof(u64)], value->a.v[i].u64);
        break;
    case PTP_DTC_AINT64:
        size = sizeof(u32) + value->a.count * sizeof(s64);
        dpv = mtp_malloc(size);
        htod32a(dpv, value->a.count);
        for (i = 0;i < value->a.count;i++)
            htod64a(&dpv[sizeof(u32) + i * sizeof(s64)], value->a.v[i].i64);
        break;
    /* XXX: ����int����û��ʵ�� */
    case PTP_DTC_STR: {
        dpv = ptp_get_packed_stringcopy(params, value->str, &size);
        break;
    }
    }
    *dpvptr=dpv;
    return size;
}

/* �������������*/
#define MAX_MTP_PROPS 127
static inline u32
ptp_pack_OPL (PTPParams *params, MTPProperties *props, int nrofprops, unsigned char** opldataptr)
{
    unsigned char* opldata;
    MTPProperties *propitr;
    unsigned char *packedprops[MAX_MTP_PROPS];
    u32 packedpropslens[MAX_MTP_PROPS];
    u32 packedobjecthandles[MAX_MTP_PROPS];
    u16 packedpropsids[MAX_MTP_PROPS];
    u16 packedpropstypes[MAX_MTP_PROPS];
    u32 totalsize = 0;
    u32 bufp = 0;
    u32 noitems = 0;
    u32 i;

    /* 4�ֽڴ洢Ԫ�ص�����*/
    totalsize = sizeof(u32);
    propitr = props;
    while (nrofprops-- && noitems < MAX_MTP_PROPS) {
        /* ������ */
        packedobjecthandles[noitems] = propitr->ObjectHandle;
        totalsize += sizeof(u32); /* ����ID */
        /* Ԫ�������� */
        packedpropsids[noitems] = propitr->property;
        totalsize += sizeof(u16);
        /* �������� */
        packedpropstypes[noitems] = propitr->datatype;
        totalsize += sizeof(u16);
        /* ���ÿһ��Ҫ���͵�����*/
        packedpropslens[noitems] = ptp_pack_DPV (params, &propitr->propval, &packedprops[noitems], propitr->datatype);
        totalsize += packedpropslens[noitems];
        noitems++;
        propitr++;
    }

    /* Ϊ���������������ռ�*/
    opldata = mtp_malloc(totalsize);

    htod32a(&opldata[bufp], noitems);
    bufp += 4;

    /* ������һ���������*/
    for (i = 0; i < noitems; i++) {
        /* ����ID */
        htod32a(&opldata[bufp], packedobjecthandles[i]);
        bufp += sizeof(u32);
        htod16a(&opldata[bufp], packedpropsids[i]);
        bufp += sizeof(u16);
        htod16a(&opldata[bufp], packedpropstypes[i]);
        bufp += sizeof(u16);
        /* �������� */
        memcpy(&opldata[bufp], packedprops[i], packedpropslens[i]);
        bufp += packedpropslens[i];
        mtp_free(packedprops[i]);
    }
    *opldataptr = opldata;
    return totalsize;
}

/**
 * �ȽϺ���
 *
 * @param x Ҫ�Ƚϵ����ݵ�ַ
 * @param y Ҫ�Ƚϵ����ݵ�ַ
 * @return ���С��0����x��y��ߣ��������0����˳��ȷ�����������0����x��y���ұ�
 **/
static int
_compare_func(const void* x, const void *y) {
    const MTPProperties *px = x;
    const MTPProperties *py = y;

    return px->ObjectHandle - py->ObjectHandle;
}

#define  INT_MAX  0x7FFFFFFF

/**
 * PTP���������������
 *
 * @param params PTP����
 * @param data   Ҫ�����ԭʼ����
 * @param pprops ����󷵻ص�MTP��������
 * @param len    ԭʼ���ݳ���
 * @return �ɹ��������Ե���Ŀ
 **/
static inline int
ptp_unpack_OPL (PTPParams *params, unsigned char* data, MTPProperties **pprops, unsigned int len)
{
    u32 prop_count;
    MTPProperties *props = NULL;
    unsigned int offset = 0, i;

    /* ���ԭʼ���ݳ����Ƿ�Ϸ�*/
    if (len < sizeof(u32)) {
        ptp_debug (params ,"must have at least 4 bytes data, not %d", len);
        return 0;
    }
    /* ��ȡ���Ե�����*/
    prop_count = dtoh32a(data);
    *pprops = NULL;
    /* ���������ԣ�����*/
    if (prop_count == 0)
        return 0;
    /* ���Ե����������������*/
    if (prop_count >= INT_MAX/sizeof(MTPProperties)) {
        ptp_debug (params ,"prop_count %d is too large", prop_count);
        return 0;
    }
    ptp_debug (params ,"Unpacking MTP OPL, size %d (prop_count %d)", len, prop_count);

    data += sizeof(u32);
    len -= sizeof(u32);
    /* ���������ڴ�*/
    props = mtp_malloc(prop_count * sizeof(MTPProperties));
    if (!props)
        return 0;
    for (i = 0; i < prop_count; i++) {
        /* ���ʣ�೤�Ȳ���һ�����ԵĴ�С*/
        if (len <= (sizeof(u32) + sizeof(u16) + sizeof(u16))) {
            ptp_debug (params ,"short MTP Object Property List at property %d (of %d)", i, prop_count);
            ptp_debug (params ,"device probably needs DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST_ALL");
            ptp_debug (params ,"or even DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST", i);
            /* ����MTP����*/
            qsort (props, i, sizeof(MTPProperties), _compare_func);
            *pprops = props;
            return i;
        }
        /* ��ȡ������*/
        props[i].ObjectHandle = dtoh32a(data);
        data += sizeof(u32);
        len -= sizeof(u32);
        /* ��ȡ��������*/
        props[i].property = dtoh16a(data);
        data += sizeof(u16);
        len -= sizeof(u16);
        /* ��ȡ����������������*/
        props[i].datatype = dtoh16a(data);
        data += sizeof(u16);
        len -= sizeof(u16);

        offset = 0;
        /* ����豸����ֵ��������ƫ����*/
        if (!ptp_unpack_DPV(params, data, &offset, len, &props[i].propval, props[i].datatype)) {
            ptp_debug (params ,"unpacking DPV of property %d encountered insufficient buffer. attack?", i);
            qsort (props, i, sizeof(MTPProperties),_compare_func);
            *pprops = props;
            return i;
        }
        data += offset;
        len -= offset;
    }
    /* ���ж�������*/
    qsort (props, prop_count, sizeof(MTPProperties),_compare_func);
    *pprops = props;
    return prop_count;
}

/* PTP �����ļ�����ڽ�� Copyright (c) 2003 Nikolai Kopanygin*/
#define PTP_cfe_ObjectHandle        0
#define PTP_cfe_ObjectFormatCode    4
#define PTP_cfe_Flags           6
#define PTP_cfe_ObjectSize      7
#define PTP_cfe_Time            11
#define PTP_cfe_Filename        15

static inline void
ptp_unpack_Canon_FE (PTPParams *params, unsigned char* data, PTPCANONFolderEntry *fe)
{
    int i;
    if (data==NULL)
        return;
    fe->ObjectHandle = dtoh32a(&data[PTP_cfe_ObjectHandle]);
    fe->ObjectFormatCode = dtoh16a(&data[PTP_cfe_ObjectFormatCode]);
    fe->Flags = dtoh8a(&data[PTP_cfe_Flags]);
    fe->ObjectSize = dtoh32a((unsigned char*)&data[PTP_cfe_ObjectSize]);
    fe->Time = (time_t)dtoh32a(&data[PTP_cfe_Time]);
    for (i = 0; i < PTP_CANON_FilenameBufferLen; i++)
        fe->Filename[i] = (char)dtoh8a(&data[PTP_cfe_Filename + i]);
}

