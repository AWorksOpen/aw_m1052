/**
 * \file device-flags.h
 * Special device flags to deal with bugs in specific devices.
 *
 * Copyright (C) 2005-2007 Richard A. Low <richard@wentnet.com>
 * Copyright (C) 2005-2012 Linus Walleij <triad@df.lth.se>
 * Copyright (C) 2006-2007 Marcus Meissner
 * Copyright (C) 2007 Ted Bullock
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
 *
 * This file is supposed to be included by both libmtp and libgphoto2.
 */

/**
 * ��Щ�������ָʾĳЩ�������豸�Ƿ���Ҫ���⴦����ЩӦ�ÿ���ʹ���߼����ν�����������
 * ��Ϊÿ������ʹ��һ��λ���������ǲ���Ҫ����32λ��
 */
#define DEVICE_FLAG_NONE 0x00000000
/**
 * ����ζ�ţ������������0xFFFFFFFF�Ķ�����������ʱ���������3ͬ������Ϊ0xFFFFFFFF����
 * PTP_OC_MTP_GetObjPropList���᷵���豸�������ļ��ĸ�ʽ��ȷ��Ԫ���ݡ���
 * DEVICE_FLAG_BROKEN_MTPGETOBJECTPROPLIST��ȣ�
 * DEVICE_FLAG_BROKEN_MTPGETOBJECTPROPLISTֻ��ʾ���ڻ�ȡ���������Ԫ����ʱ���ƻ���
 * ʵ�ֿ��ܱ��ƻ���һ�����ͷ�ʽ�ǣ��������޷����ض������ȷ������������ʱ��(������ZENs��)���ʹ��
 * ��������������ֻ�Ǵ��б��ж�ʧ����ʱ��������������ڸǴ����еĴ���(���紦���СΪ-1(0xFFFFFFFFU))��
 * ������������������£���ɾ���������ڴ����ǹ̶��ġ���ʱʹ�������������Ϊ��ȡ���ж���̫�������ʹ��
 * ������USB���񽫳�ʱ��
 */
#define DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST_ALL 0x00000001
/**
 * ����ζ����Linux�£���һ���ں�ģ���������ʹ������豸��USB�ӿڣ���������������������Ҫ
 * ������롣ͨ��������˫ģ�豸�ϣ����豸��ͬʱ��ʾ����MTP�ӿں��豸��������һ��USB�������洢
 * �ӿڡ����USB�������洢�ӿ���ʹ���У�����apps(���������û��ռ��libmtpͨ��libusb����·��)
 * ����ʹ�������������ǿ����Ƴ����Ϸ���Ӧ�á�ͨ������ζ������ܵ���root�û������������
 */
#define DEVICE_FLAG_UNLOAD_DRIVER 0x00000002
/**
 * ����ζ��PTP_OC_MTP_GetObjPropList(9805)��ĳ�̶ֳ��ϱ��ƻ��ˣ�����
 * ������������(����android�豸)�������������3����Ϊ0xFFFFFFFFU����������
 * ��ȷ�ط������ж������ԡ�
 */
#define DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST 0x00000004
/**
 * ����ζ�ŵ�����߽���64�ֽڵı���(USB 1.1 �˵��С)ʱ���豸���ᷢ��0���ݰ���ָʾ���������
 * �෴�������С��USB 1.1 �˵��С(64�ֽ�)�������������ڴ������ʱֻ����һ�������ֽڡ�
 *
 * ������Ϊ�ܿ�����һ�ֽ����������Ϊ�豸���Ǹ���Ӳ��USB�ӻ��������������ܴ���0д����������USB
 * 1.1 �˵��С��ʹ������Ϊ�豸��USB 1.1�������ϡ����١�����Ϊ64�ֽ���512�ֽڵı�����������
 * ��USB1.1 ��USB2.0 �Ϲ�����
 */
#define DEVICE_FLAG_NO_ZERO_READS 0x00000008
/**
 * �����־��ʾ�豸��������OGG�����ļ����ͣ����libmtp����鿴�ļ���չ������ȷ���ļ�ʵ��
 * ����OGG������һ������ĵ�ǰ�̼��Ĵ�����Ȼ�̼�����Ӧ���ڹ̼����޸��������Ƿǳ�ϲ��OGG��
 * ����������������־��֧�������������ֻ����iriver�豸�ϲ��ܿ������������־��������
 * Ӱ�죬ֻҪ�ԡ�δ֪���ļ������ļ���չ����鼴�ɡ����������������֪��(����)��֧��ogg����ʹ
 * ��֧�֣���ʹ����ǿ���OGG_IS_UNKNOWN��־������ǿ��֧���κ���չ��Ϊ.ogg�Ķ����ϵ�ogg��
 */
#define DEVICE_FLAG_IRIVER_OGG_ALZHEIMER 0x00000010
/**
 * �����־ָ�����豸���Խ��ܵ��ļ���������-���Ǳ�����7λ(�����ַ�<=127/0x7F)��
 * Philips Shoqbox�ϵ�һ�η���������⣬PTP��׼Ҫ���κ�unicode�ַ�����������
 * �ļ������Ҳ����������豸������ʹ��7λ�ļ�ϵͳ���µġ�
 */
#define DEVICE_FLAG_ONLY_7BIT_FILENAMES 0x00000020
/**
 * �����־��ʾ����ڹر��豸ʱ���Ի�ȡ�˵��״̬��/���ͷŽӿڣ����豸�����������ر�����
 * SanDisk Sansa�豸�����⡣�������Windows�Ӳ��ͷŽӿڵ���Ϊ�ĸ����á�
 */
#define DEVICE_FLAG_NO_RELEASE_INTERFACE 0x00000040
/**
 * �����־������ ZEN 8GB�ĳ��ֶ�����ġ��豸��ʱ�᷵��һ������PTPͷ������������
 * < 1502 0000 0200 01d1 02d1 01d2 >��6���ֽ�(��ʾ�����롱�͡�����ID��)����
 * ��������Υ����PTP/MTP�淶������Ȼʹ����Windows����������ΪWindowsû�м����Щ
 * �ֽ��Ƿ���Ч��Ҫ���������豸���в�����������Ҫ�����־��ģ��Windows������Aricent
 * MTPЭ��ջ��Ҳ�����˻���ͷ��
 */
#define DEVICE_FLAG_IGNORE_HEADER_ERRORS 0x00000080
/**
 * Ħ������RAZR2 V8(������)�ѶϿ����ö��������б�����Ԫ��������ʧ�ܡ�(��
 * �ö������������õ������Ե������ڸ��豸����Ч����Ԫ������׷���ϻᱻ���ԣ����ܿ�����
 * �ò����б�����)
 */
#define DEVICE_FLAG_BROKEN_SET_OBJECT_PROPLIST 0x00000100
/**
 * Samsung YP-T10��ΪOgg�ļ�Ӧ���ԡ�unknown��(PTP_OFC_Undefined)�ļ�
 * ���ͷ��ͣ�������һ�������ã���iRiver Ogg Alzheimer����(�������ļ���չ����
 * ʶ��Ogg�ļ�)����Ҫ����Ogg֧��(�豸��������ȷ����֧����)�Լ��ڴ洢Ogg�ļ�ʱ��
 * Ҫ���ļ������豸Ϊδ֪����ʹ����ʵ���ϲ�����δ֪������iRivers�ƺ���Ҫ�����־����
 * Ϊ���ǲ�����֧��Ogg����ʹ�������֧��Ogg��ͨ�����豸��USB�������洢ģʽ��֧��Ogg��
 * ��ô�̼�ֻ��û����ȷ����Ԫ����֧��Ogg��
 */
#define DEVICE_FLAG_OGG_IS_UNKNOWN 0x00000200
/**
 * Creative Zen��libmtp�����൱���ȶ��ģ������ڸ��ߵĹ̼��汾���ƺ�����ʵ�֡�
 * ���ǣ���������������ߴ�ʱ������Ȼ�����������˱�־��ֹ���óߴ�(���ƺ���ͼ�ε���ʾ��ʽ�޹�)��
 */
#define DEVICE_FLAG_BROKEN_SET_SAMPLE_DIMENSIONS 0x00000400
/**
 * һЩ�豸���ر���SanDisk Sansas����Ҫʼ�ռ���䡰OS��������������������
 */
#define DEVICE_FLAG_ALWAYS_PROBE_DESCRIPTOR 0x00000800
/**
 * Samsung�Ѿ�ʵ�����Լ��Ĳ����б��ʽ����Ϊ�洢����ͨ�ļ�ϵͳ�е�.spl�ļ�����������ȷ��
 * mtp�����б�.spl��ʽ�ж���汾�����ļ������汾 X.XX
 * �汾1.00ֻ��һ���򵥵Ĳ����б�
 */
#define DEVICE_FLAG_PLAYLIST_SPL_V1 0x00001000
/**
 * Samsung�Ѿ�ʵ�����Լ��Ĳ����б��ʽ����Ϊ�洢����ͨ�ļ�ϵͳ�е�.spl�ļ�����������ȷ��
 * mtp�����б�.spl��ʽ�ж���汾�����ļ������汾 X.XX
 * �汾2.00��һ�������б�������ÿ�������б�洢DNSe��������
 */
#define DEVICE_FLAG_PLAYLIST_SPL_V2 0x00002000
/**
 * Sansa E250��֪����������⣬ʵ�������豸��������PTP_OPC_DateModified�ǿɶ�/��д
 * ����Ȼ�޷���������ֻ���ڵ�һ�η����ļ�ʱ������ȷ���á�
 */
#define DEVICE_FLAG_CANNOT_HANDLE_DATEMODIFIED 0x00004000
/**
 * �������Ա���ʹ�÷��Ͷ��������б����ڴ����¶���(�������Ǹ���)ʱʹ
 * �ã�DEVICE_FLAG_BROKEN_SET_OBJECT_PROPLIST�����
 * �ģ���ֻ�漰���Ͷ��������б����������ж���������
 * ����Toshiba Gigabeat MEU202����������⡣
 */
#define DEVICE_FLAG_BROKEN_SEND_OBJECT_PROPLIST 0x00008000
/**
 * ���ò�֧�ֶ�����
 */
#define DEVICE_FLAG_BROKEN_BATTERY_LEVEL 0x00010000
/**
 * ��Ҫֱ��PTPƥ��(libgphoto2)
 */
#define DEVICE_FLAG_OLYMPUS_XML_WRAPPED     0x00800000
/**
 * �����־��DEVICE_FLAG_OGG_IS_UNKNOWN���ƣ�������FLAC�ļ�������
 * FLAC�ļ�ʹ��δ֪�ļ�����
 */
#define DEVICE_FLAG_FLAC_IS_UNKNOWN     0x01000000
/**
 * �豸��Ҫ��һ�޶����ļ�����û�������ļ�����������ͬ���ַ�����
 */
#define DEVICE_FLAG_UNIQUE_FILENAMES        0x02000000
/**
 * �����־��BlackBerry�豸��ִ��һЩmagic����USB�������洢�л���������Ϊ��MTPģʽ
 */
#define DEVICE_FLAG_SWITCH_MODE_BLACKBERRY  0x04000000
/**
 * �����־ָ�����豸��һЩ��������Ҫһ������ĳ�ʱ�䳬ʱ
 */
#define DEVICE_FLAG_LONG_TIMEOUT        0x08000000
/**
 * �����־��ʾ�豸��ÿ�����Ӻ���Ҫ��ʾ��USB��λ��ĳЩ�豸��ϲ�����������
 * Ĭ������²���������
 */
#define DEVICE_FLAG_FORCE_RESET_ON_CLOSE    0x10000000
/**
 * �ƺ���Щ�豸��ʹ��GetObjectInfo����ʱ�����˴�������ݡ���������������£�����
 * ��ϲ����MTP property list����PTP-compatible object infos��
 *
 * ����һЩSamsung Galaxy S�豸����һ��MTPЭ��ջ����Э����64λ������32λ��
 * ʾObjectInfo
 */
#define DEVICE_FLAG_PROPLIST_OVERRIDES_OI   0x40000000
/**
 * Samsung Galaxy�豸��MTPЭ��ջ����GetPartialObject��һ�����صĴ��󡣵�GetPartialObject��
 * ��������ȡ�ļ�������ֽڲ���Ҫ��ȡ��������ʹ��Ӧ���з��͵����һ��USB���ݰ���USB2.0���ݰ���С��ȫƥ��
 * ʱ��Samsung Galaxy�豸�����𣬴Ӷ����³�ʱ����
 */
#define DEVICE_FLAG_SAMSUNG_OFFSET_BUG      0x80000000
/**
 * ������Щbug��־��Ҫ��SONY NWZ Walkman�����ã��ͻ��ڼ�⵽�Զ�����չ��
 * ����"sony.net"��δ֪�豸�ϱ��Զ����
 */
#define DEVICE_FLAGS_SONY_NWZ_BUGS \
  (DEVICE_FLAG_UNLOAD_DRIVER | \
   DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST | \
   DEVICE_FLAG_UNIQUE_FILENAMES | \
   DEVICE_FLAG_FORCE_RESET_ON_CLOSE)
/**
 * ������Щbug��־����Ҫ��android�豸�����ã���������֧�ֵ�ʵ���������޷������
 * MTP�������ر���9805(��ȡ���������б�)�������Զ���������豸��չ�������б�
 * �桰android.com�����豸��
 */
#define DEVICE_FLAGS_ANDROID_BUGS \
  (DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST | \
   DEVICE_FLAG_BROKEN_SET_OBJECT_PROPLIST | \
   DEVICE_FLAG_BROKEN_SEND_OBJECT_PROPLIST | \
   DEVICE_FLAG_UNLOAD_DRIVER | \
   DEVICE_FLAG_LONG_TIMEOUT | \
   DEVICE_FLAG_FORCE_RESET_ON_CLOSE)
/**
 * ������Щbug ��־��������һЩSonyEricsson�豸�ϣ�����û��ʹ��Android 4.0+ MTP
 * Э��ջ��Android�豸���߶Ȼ�����Щbug����Aricent��MTP��ʵ�֣����������ΪAricent bug
 * ��־���������Э��ջ��˵����������Ҫ���Ե�ͷ�����ǵ��Ͳ���
 *
 * ����һЩ�²�֮�������Զ�����Щ�����־��������"microsoft.com/WPDNA"��"sonyericsson.com/SE"��
 * ������"android.com"���������豸*/
#define DEVICE_FLAGS_ARICENT_BUGS \
  (DEVICE_FLAG_IGNORE_HEADER_ERRORS | \
   DEVICE_FLAG_BROKEN_SEND_OBJECT_PROPLIST | \
   DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST)
