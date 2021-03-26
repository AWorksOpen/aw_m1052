/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ͨ��MTP�豸ͨѶ��ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨��USBH���USBHЭ��ջ
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. �ڵװ��USB0�˿��в���U���豸
 *   4. �ڵװ��USB1�˿��в���MTP�豸
 *   5. ��demoע���������mtp_test����������á�help����ѯ֧�ֲ���������
 *   6. �����ʹ�÷�����ʹ�á�--help����ѯ
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *   2. ʹ�á�mtp_test�����������
 *   3. �����ֻ���Ҫͬ��usb���ӷ�ʽ���д����ļ���
 *
 */
#include <string.h>
#include "Aworks.h"
#include "aw_delay.h"
#include "aw_serial.h"
#include "aw_vdebug.h"
#include "aw_shell.h"
#include "libmtp.h"
#include "mtp_adapter.h"
#include "pathutils.h"

#define USB_LOCAL
#ifdef USB_LOCAL
#define USB_BLK_NAME  "/dev/h0-d1-0"
#endif

LIBMTP_raw_device_t    *rawdevices;
int                     numrawdevices;

LIBMTP_mtpdevice_t     *device;
int                     MTP_DEVICE_OPEN[256];

static int prompt (void)
{
    int  len = 0;
    char buff[2];

    aw_kprintf("> \r\n");
    while (1) {
        len = aw_serial_read(COM0, buff, sizeof(buff));
        if((len > 0) && (len == 1)){
            if (buff[0] == 'y') {
                return 0;
            } else if (buff[0] == 'n') {
                return 1;
            }
            return -1;
        }
//        fprintf(stdout, "> ");
//        if ( fgets(buff, sizeof(buff), stdin) == NULL ) {
//            if (ferror(stdin)) {
//                fprintf(stderr, "File error on stdin\n");
//            } else {
//                fprintf(stderr, "EOF on stdin\n");
//            }
//            return 1;
//        }
//        if (buff[0] == 'y') {
//            return 0;
//        } else if (buff[0] == 'n') {
//            return 1;
//        }
    }
}

/**
 * �ҵ�·���е���һ����/����ƫ��
 *
 * @param path��·��
 *
 * @return �ҵ�����ƫ������û�ҵ�����-1��
 */
static int my_dirname (char *path)
{
    int i, len;
    int find = -1;

    /* ��鳤���Ƿ�Ϸ� */
    len = strlen(path);
    if (len == 0) {
        return -2;
    }

    /* ��'/'*/
    for (i = 0;i < len; i++) {
        if (path[i] == '/') {
            find = i;
            break;
        }
    }
    return find;
}

static int __find_id_from_name (char               *path,
                                int                 storageid,
                                u32                 parentid,
                                LIBMTP_filetype_t   filetype)
{
    LIBMTP_file_t *files_list;
    LIBMTP_file_t *file_temp;
    LIBMTP_file_t *oldfile;
    int offset;
    int find_id = -1;

    /* ����Ƿ�Ϊ�� */
    if (path == NULL) {
        aw_kprintf("path is NULL\r\n");
        return -1;
    }

    /* ���·���Ƿ�Ϸ� */
    if (path[0] != '/') {
        aw_kprintf("path is illegal\r\n");
        return -1;
    }
    path = path + 1;

    /* ���ļ��� */
    if (strcmp(path, "/") == 0)
        return 0;

    offset = my_dirname(path);
    if ((offset == 0) || (offset == -2)) {
        aw_kprintf("path is illegal\r\n");
        return -1;
    }

    /* ��ȡ�ļ����� */
    files_list = LIBMTP_Get_Files_And_Folders(device, storageid, parentid);
    if (files_list == NULL) {
        LIBMTP_Dump_Errorstack(device);
        LIBMTP_Clear_Errorstack(device);
    } else {
        file_temp = files_list;
        while (file_temp != NULL) {
            if (offset > 0) {
                if (strncmp(file_temp->filename, path, offset) == 0) {
                    if (file_temp->filetype == LIBMTP_FILETYPE_FOLDER) {
                        find_id = __find_id_from_name(path + offset, storageid, file_temp->item_id, filetype);
                    } else{
                        aw_kprintf("target is not a folder\r\n");
                    }
                }
            } else if (offset == -1) {
                if (strcmp(file_temp->filename, path) == 0) {
                    if (filetype == LIBMTP_FILETYPE_UNKNOWN) {
                        find_id = file_temp->item_id;
                    } else{
                        if (file_temp->filetype == filetype) {
                            find_id = file_temp->item_id;
                        }
                    }
                }
            }
            oldfile = file_temp;
            file_temp = file_temp->next;

            /* �����ļ� */
            LIBMTP_destroy_file_t(oldfile);
        }
    }
    return find_id;
}

/**
 * �ݹ��ļ���
 *
 * @param device  ��ص�MTP�豸
 * @param storage Ҫ�ݹ��ļ����Ĵ洢�豸
 * @param leaf    ��Ŀ¼
 * @param depth   ���
 */
static void recursive_file_tree (LIBMTP_mtpdevice_t     *device,
                                 LIBMTP_devicestorage_t *storage,
                                 u32                     leaf,
                                 int                     depth)
{
    LIBMTP_file_t *files_list;
    LIBMTP_file_t *file_temp;

    /* ��ȡ�ļ����ļ��� */
    files_list = LIBMTP_Get_Files_And_Folders(device, storage->id, leaf);
    if (files_list == NULL) {
        return;
    }

    /* �����ļ����� */
    file_temp = files_list;
    while (file_temp != NULL) {
        int i;
        LIBMTP_file_t *oldfile;

        /* Indent */
        for (i = 0; i < depth; i++) {
            aw_kprintf(" ");
        }
        aw_kprintf("%u %s\r\n", file_temp->item_id, file_temp->filename);

        /* ���ļ��У������ݹ�������ļ����ļ��� */
        if (file_temp->filetype == LIBMTP_FILETYPE_FOLDER) {
            recursive_file_tree(device, storage, file_temp->item_id, depth + 2);
        }

        oldfile = file_temp;
        file_temp = file_temp->next;

        /* �����ļ� */
        LIBMTP_destroy_file_t(oldfile);
    }
}


/**
 * �ݹ��ļ���
 *
 * @param device  ��ص�MTP�豸
 * @param storage Ҫ�ݹ��ļ����Ĵ洢�豸
 * @param leaf    ��Ŀ¼
 * @param path    Ŀ¼·��
 * @param find    �Ƿ��ҵ�Ŀ¼
 */
static void recursive_folder_tree(LIBMTP_mtpdevice_t       *device,
                                  LIBMTP_devicestorage_t   *storage,
                                  uint32_t                  leaf,
                                  char                     *path,
                                  int                       find)
{
    LIBMTP_file_t *file_list;
    LIBMTP_file_t *file_temp;
    LIBMTP_file_t *oldfile;
    int            i = 0, offset;

    /* ��ȡ��Ŀ¼���ļ����ļ��� */
    file_list = LIBMTP_Get_Files_And_Folders(device, storage->id, leaf);
    if (file_list == NULL) {
        return;
    }
    file_temp = file_list;

    /* Ҫ�������Ǹ�Ŀ¼��ֱ�Ӵ�ӡ */
    if (find) {
        while (file_temp != NULL) {
            if (i == 8) {
                i = 0;
                aw_kprintf("\r\n");
            }
            aw_kprintf("%s  ", file_temp->filename);
            i++;
            oldfile = file_temp;
            file_temp = file_temp->next;

            /* �����ļ� */
            LIBMTP_destroy_file_t(oldfile);
        }
        return;
    }
    if (path[0] == '/') {
        path = path + 1;
    }

    /* �ҵ���һ����/�� */
    offset = my_dirname(path);
    if ((offset == 0) || (offset == -2)) {
        aw_kprintf("path is illegal\r\n");
        return;
    }

    /* �����ļ����� */
    while (file_temp != NULL) {
        if (offset > 0) {
            if (strncmp(file_temp->filename, path, offset) == 0) {
                if (file_temp->filetype == LIBMTP_FILETYPE_FOLDER) {
                    recursive_folder_tree(device, storage, file_temp->item_id, path + offset, 0);
                } else{
                    aw_kprintf("target is not a folder\r\n");
                }
            }
        } else if (offset == -1) {
            if (strcmp(file_temp->filename, path) == 0) {
                if (file_temp->filetype == LIBMTP_FILETYPE_FOLDER) {
                    recursive_folder_tree(device, storage, file_temp->item_id, path, 1);
                } else{
                    aw_kprintf("target is not a folder\r\n");
                }
            }
        }
        oldfile = file_temp;
        file_temp = file_temp->next;

        /* �����ļ� */
        LIBMTP_destroy_file_t(oldfile);
    }
}

/* MTP�������� */
static int __mtp_devdetect (int argc, char **argv, struct aw_shell_user *p_user)
{
    LIBMTP_error_number_t err;

    /* ������ */
    if (argc > 0) {
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:mtp-detect\r\n\r\n");
            aw_kprintf("Description:detect MTP device\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        } else {
            aw_kprintf("Try \"mtp-detect --help\" for more information.\r\n");
        }
        return 0;
    }

    /* ���ͷ�ԭ�����豸�ڴ� */
    if (rawdevices != NULL) {
        mtp_free(rawdevices);
    }
    aw_kprintf("Listing raw device(s):\r\n");

    /* ���MTP�豸 */
    err = LIBMTP_Detect_Raw_Devices(&rawdevices, &numrawdevices);
    switch(err) {
        case LIBMTP_ERROR_NO_DEVICE_ATTACHED:
            aw_kprintf("   No raw devices found.\r\n");
            return 0;

        case LIBMTP_ERROR_CONNECTING:
            aw_kprintf("Detect: There has been an error connecting. Exiting\r\n");
            return 0;

        case LIBMTP_ERROR_MEMORY_ALLOCATION:
            aw_kprintf("Detect: Encountered a Memory Allocation Error. Exiting\r\n");
            return 0;

        case LIBMTP_ERROR_NONE:
        {
            int i;
            aw_kprintf("   Found %d device(s):\r\n", numrawdevices);
            for (i = 0; i < numrawdevices; i++) {
                if (rawdevices[i].device_entry.vendor != NULL ||
                        rawdevices[i].device_entry.product != NULL) {
                    aw_kprintf("   %s: %s (%04x:%04x) @ bus %d, dev %d\r\n",
                            rawdevices[i].device_entry.vendor,
                            rawdevices[i].device_entry.product,
                            rawdevices[i].device_entry.vendor_id,
                            rawdevices[i].device_entry.product_id,
                            rawdevices[i].bus_location,
                            rawdevices[i].devnum);
                } else {
                    aw_kprintf("   %04x:%04x @ bus %d, dev %d\r\n",
                            rawdevices[i].device_entry.vendor_id,
                            rawdevices[i].device_entry.product_id,
                            rawdevices[i].bus_location,
                            rawdevices[i].devnum);
                }
            }
        }
        break;

        case LIBMTP_ERROR_GENERAL:
        default:
            aw_kprintf("Unknown connection error.\r\n");
            return 0;
    }

    return 0;
}

/* ���豸���� */
static int __mtp_devopen (int argc, char **argv, struct aw_shell_user *p_user)
{
    int     num, i, ret;
    char   *endptr;
    char   *friendlyname;
    char   *syncpartner;
    u8      maxbattlevel;
    u8      currbattlevel;
    u16    *filetypes;
    u16     filetypes_len;

    if (rawdevices == NULL) {
        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
        return 0;
    }
    if (device != NULL) {
        aw_kprintf("Please run \"mtp-close\" cmd first.\r\n");
        return 0;
    }
    if ((argc > 1) || (argc == 0)) {
        aw_kprintf("Try \"mtp-open --help\" for more information.\r\n");
        return 0;
    }
    if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
        aw_kprintf("Command:mtp-open <devnum>\r\n\r\n");
        aw_kprintf("Description:open a MTP device\r\n\r\n");
        aw_kprintf("parameter:\r\n");
        aw_kprintf("    1.devnum :devnum of MTP device want to open\r\n");
        return 0;
    }

    /* ��ȡ��������޷��ų����� */
    num = strtoul(argv[0], &endptr, 10);
    if (num == 0) {
        aw_kprintf("Try \"mtp-open --help\" for more information.\r\n");
        return 0;
    }

    /* Ѱ�Ҷ�Ӧ�豸�ŵ�MTP�豸 */
    for (i = 0;i < numrawdevices; i++) {
        if (rawdevices[i].devnum == num) {
            if (MTP_DEVICE_OPEN[num] == 1) {
                aw_kprintf("MTP device already open\r\n");
                return 0;
            }
            break;
        }
    }
    if (i == numrawdevices) {
        aw_kprintf("Unable to find device devnum is %d\r\n", num);
        return 0;
    }

    /* ��MTP�豸�����Ҳ����������Ϣ */
    device = LIBMTP_Open_Raw_Device_Uncached(&rawdevices[i]);
    if (device == NULL) {
        aw_kprintf("Unable to open raw device\r\n");
        return 0;
    }

    /* �������� */
    LIBMTP_Dump_Errorstack(device);
    LIBMTP_Clear_Errorstack(device);
    aw_kprintf("MTP-specific device properties:\n");

    /* ��ȡfriendly name */
    friendlyname = LIBMTP_Get_Friendlyname(device);
    if (friendlyname == NULL) {
        aw_kprintf("   Friendly name: (NULL)\r\n");
    } else {
        aw_kprintf("   Friendly name: %s\r\n", friendlyname);
        mtp_free(friendlyname);
    }

    /* ��ȡsync partner */
    syncpartner = LIBMTP_Get_Syncpartner(device);
    if (syncpartner == NULL) {
        aw_kprintf("   Synchronization partner: (NULL)\r\n");
    } else {
        aw_kprintf("   Synchronization partner: %s\r\n", syncpartner);
        mtp_free(syncpartner);
    }

    /* �����Ϣ */
    ret = LIBMTP_Get_Batterylevel(device, &maxbattlevel, &currbattlevel);
    if (ret == 0) {
        mtp_printf("   Battery level %d of %d (%d%%)\r\n",currbattlevel, maxbattlevel,
                (int) ((float) currbattlevel/ (float) maxbattlevel * 100.0));
    } else {

        /* ���ԡ�һЩ�豸��֧�ֻ�ȡ���� */
        LIBMTP_Clear_Errorstack(device);
    }

    /* ��ȡ֧�ֵ��ļ����� */
    ret = LIBMTP_Get_Supported_Filetypes(device, &filetypes, &filetypes_len);
    if (ret == 0) {
        uint16_t i;

        mtp_printf("Libmtp supported (playable) filetypes:\r\n");
        for (i = 0; i < filetypes_len; i++) {
            mtp_printf("   %s\r\n", LIBMTP_Get_Filetype_Description(filetypes[i]));
        }
        if (filetypes != NULL) {
            mtp_free(filetypes);
        }
    } else {
        LIBMTP_Dump_Errorstack(device);
        LIBMTP_Clear_Errorstack(device);
    }
    MTP_DEVICE_OPEN[num] = 1;
    return 0;
}

/* �ر��豸���� */
static int __mtp_devclose (int argc, char **argv, struct aw_shell_user *p_user) {
    int num, i;
    char *endptr;

    /* û�м���豸 */
    if (rawdevices == NULL) {
        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
        return 0;
    }
    if ((argc > 1) || (argc == 0)) {
        aw_kprintf("Try \"mtp-close --help\" for more information.\r\n");
        return 0;
    }
    if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
        aw_kprintf("Command:mtp-close <devnum>\r\n\r\n");
        aw_kprintf("Description:close a MTP device\r\n\r\n");
        aw_kprintf("parameter:\r\n");
        aw_kprintf("    1.devnum :devnum of MTP device want to close\r\n");
        return 0;
    }

    /* ��ȡ��������޷��ų����� */
    num = strtoul(argv[0], &endptr, 10);
    if (num == 0) {
        aw_kprintf("Try \"mtp-close --help\" for more information.\r\n");
        return 0;
    }

    /* Ѱ�Ҷ�Ӧ�豸�ŵ�MTP�豸 */
    for (i = 0;i < numrawdevices; i++) {
        if (rawdevices[i].devnum == num) {
            if (MTP_DEVICE_OPEN[num] == 0) {
                aw_kprintf("MTP device %d is not open\r\n",num);
                return 0;
            }
            break;
        }
    }
    if (i == numrawdevices) {
        aw_kprintf("Unable to find device devnum is %d\r\n", num);
        return 0;
    }

    /* �ͷ��豸 */
    if (device) {
        LIBMTP_Release_Device(device);
    }

    MTP_DEVICE_OPEN[num] = 0;
    device = NULL;
    aw_kprintf("MTP device devnum %d is closed\r\n", num);
    return 0;
}

/* ��λMTP�豸 */
static int __mtp_resetdev (int argc, char **argv, struct aw_shell_user *p_user)
{
    int     num, i, ret;
    char   *endptr;

    /* û�м���豸 */
    if (rawdevices == NULL) {
        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
        return 0;
    }
    if (device == NULL) {
        aw_kprintf("Please run \"mtp-open\" cmd open a device first.\r\n");
        return 0;
    }
    if ((argc > 1) || (argc == 0)) {
        aw_kprintf("Try \"mtp-reset --help\" for more information.\r\n");
        return 0;
    }
    if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
        aw_kprintf("Command:mtp-reset <devnum>\r\n\r\n");
        aw_kprintf("Description:reset a MTP device\r\n\r\n");
        aw_kprintf("parameter:\r\n");
        aw_kprintf("    1.devnum    :devnum of MTP device want to reset\r\n");
        return 0;
    }

    /* ��ȡ��������޷��ų����� */
    num = strtoul(argv[0], &endptr, 10);
    if (num == 0) {
        aw_kprintf("Try \"mtp-reset --help\" for more information.\r\n");
        return 0;
    }

    /* Ѱ�Ҷ�Ӧ�豸�ŵ�MTP�豸 */
    for (i = 0;i < numrawdevices; i++) {
        if (rawdevices[i].devnum == num) {
            if (MTP_DEVICE_OPEN[num] == 0) {
                aw_kprintf("MTP device %d is not open\r\n",num);
                return 0;
            }
            break;
        }
    }
    if (i == numrawdevices) {
        aw_kprintf("Unable to find device devnum is %d\r\n", num);
        return 0;
    }
    aw_kprintf("I will now reset device %d. This means that\r\n", num);
    aw_kprintf("the device may go inactive immediately and may report errors.\r\n");
    aw_kprintf("Continue? (y/n)\n");
    if (prompt() == 0) {
        ret = LIBMTP_Reset_Device(device);
        if (ret == 0) {
            aw_kprintf("mtp device reset successful\r\n");
        } else {
            if(ret == -1){
                aw_kprintf("device does not support resetting\r\n");
                ret = 0;
            }
            else{
                aw_kprintf("mtp device reset failed\r\n");
                ret = 0;
            }
        }
    } else {
       aw_kprintf("Aborted.\r\n");
       ret = 0;
    }
    return ret;
}

/* ��ʽ��MTP�豸 */
static int __mtp_storageformat (int argc, char **argv, struct aw_shell_user *p_user)
{
    LIBMTP_devicestorage_t *temp;
    int                     num, i, ret, storagenum;
    char                   *endptr;

    /* û�м���豸 */
    if (rawdevices == NULL) {
        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
        return 0;
    }
    if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
        aw_kprintf("Command:mtp-format <devnum>  <storageid>\r\n\r\n");
        aw_kprintf("Description:format a MTP device\r\n\r\n");
        aw_kprintf("parameter:\r\n");
        aw_kprintf("    1.devnum    :devnum of MTP device want to format\r\n");
        aw_kprintf("    2.storageid :storage id\r\n");
        return 0;
    }
    if (argc != 2) {
        aw_kprintf("Try \"mtp-format --help\" for more information.\r\n");
        return 0;
    }

    /* ��ȡ��������޷��ų����� */
    num = strtoul(argv[0], &endptr, 10);
    if (num == 0) {
        aw_kprintf("Try \"mtp-format --help\" for more information.\r\n");
        return 0;
    }

    /* Ѱ�Ҷ�Ӧ�豸�ŵ�MTP�豸 */
    for (i = 0;i < numrawdevices; i++) {
        if (rawdevices[i].devnum == num) {
            if (MTP_DEVICE_OPEN[num] == 0) {
                aw_kprintf("MTP device %d is not open\r\n",num);
                return 0;
            }
            break;
        }
    }
    if (i == numrawdevices) {
        aw_kprintf("Unable to find device devnum is %d\r\n", num);
        return 0;
    }
    temp = device->storage;

    /* ���洢�豸ID */
    storagenum = strtoul(argv[1], &endptr, 10);
    if (storagenum == 0) {
        aw_kprintf("Try \"mtp-format --help\" for more information.\r\n");
        return 0;
    }
    while (temp != NULL) {
        if (temp->id == storagenum) {
            break;
        }
        temp = temp->next;
    }
    /* δ�ҵ��洢�豸���˳�*/
    if(temp == NULL){
        aw_kprintf("illegal storageid %s\r\n", argv[1]);
        return 0;
    }

    aw_kprintf("I will now format device %d storage %d. This means that\r\n", num, storagenum);
    aw_kprintf("all content (and licenses) will be lost forever.\r\n");
    aw_kprintf("you will not be able to undo this operation.\r\n");
    aw_kprintf("Continue? (y/n)\r\n");
    if (prompt() == 0) {
        /* ��ʽ��MTP�豸�ĵ�һ���洢�豸 */
        ret = LIBMTP_Format_Storage(device, temp);
        if (ret == 0) {
            aw_kprintf("storage format successful\r\n");
        } else {
            if(ret == -1){
                aw_kprintf("device does not support formatting storage\r\n");
                return 0;
            }
            else{
                aw_kprintf("storage format failed\r\n");
                return 0;
            }
        }
    } else {
        aw_kprintf("Aborted.\r\n");
        ret = 0;
    }
    return ret;
}

/* ��ȡMTP�豸�еĴ洢�豸���� */
static int __mtp_getstorage (int argc, char **argv, struct aw_shell_user *p_user)
{
    LIBMTP_devicestorage_t *temp;

    /* û�м���豸 */
    if (rawdevices == NULL) {
        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
        return 0;
    }

    /* û�д��豸 */
    if (device == NULL) {
        aw_kprintf("Please run \"mtp-open\" cmd open a device first.\r\n");
        return 0;
    }

    /* ������ */
    if (argc > 0) {
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:mtp-storage\r\n\r\n");
            aw_kprintf("Description:get MTP device storage info\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        } else {
            aw_kprintf("Try \"mtp-storage --help\" for more information.\r\n");
        }
        return 0;
    }
    temp = device->storage;

    while (temp != NULL) {
        aw_kprintf("storage id:%d\r\n",temp->id);
        aw_kprintf("   storage description:%s\r\n", temp->StorageDescription);

        if (temp->StorageType == 0) {
            aw_kprintf("   storage type: undefined\r\n");
        } else if (temp->StorageType == 1) {
            aw_kprintf("   storage type: FixedROM\r\n");
        } else if (temp->StorageType == 2) {
            aw_kprintf("   storage type: RemovableROM\r\n");
        } else if (temp->StorageType == 3) {
            aw_kprintf("   storage type: FixedRAM\r\n");
        } else if (temp->StorageType == 4) {
            aw_kprintf("   storage type: RemovableRAM\r\n");
        }

        if (temp->AccessCapability == 0) {
            aw_kprintf("   storage access capability: Read and Write\r\n");
        } else if (temp->AccessCapability == 1) {
            aw_kprintf("   storage access capability: Read Only\r\n");
        } else if (temp->AccessCapability == 2) {
            aw_kprintf("   storage access capability: Read and Deletion\r\n");
        }
        aw_kprintf("   storage max capacity: %d(Bytes)\r\n", temp->MaxCapacity);
        aw_kprintf("   storage free space: %d(Bytes)\r\n", temp->FreeSpaceInBytes);
        temp = temp->next;
    }
    return 0;
}

/* չʾMTP �豸�ļ��� */
int __mtp_getfiletree (int argc, char **argv, struct aw_shell_user *p_user)
{
    u32                     storageid;
    char                   *endptr;
    LIBMTP_devicestorage_t *temp;

    if (rawdevices == NULL) {
        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
        return 0;
    }
    if (device == NULL) {
        aw_kprintf("Please run \"mtp-open\" cmd open a device first.\r\n");
        return 0;
    }

    if (argc == 1) {
        if (strcmp(argv[0], "--help") == 0) {
            aw_kprintf("Command:mtp-filetree <storageid>\r\n\r\n");
            aw_kprintf("Description:show MTP device file tree\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.storageid  :storage id(0 indicate the first storage)\r\n");
            return 0;
        } else {

            /* ��ȡ�洢�豸ID */
            storageid = strtoul(argv[0], &endptr, 10);
            if (*endptr != 0) {
                aw_kprintf("illegal value %s\r\n", argv[0]);
                return 0;
            }
        }
    } else {
        aw_kprintf("Try \"mtp-filetree --help\" for more information.\r\n");
        return 0;
    }

    /* ʹ��һ���洢�豸 */
    if (storageid == 0) {
        recursive_file_tree(device, device->storage, LIBMTP_FILES_AND_FOLDERS_ROOT, 0);
        return 0;
    }

    temp = device->storage;

    while (temp != NULL) {
        if (temp->id == storageid) {
            break;
        }
        temp = temp->next;
    }
    if (temp == NULL) {
        aw_kprintf("can not find storage\r\n");
        return 0;
    }
    recursive_file_tree(device, temp, LIBMTP_FILES_AND_FOLDERS_ROOT, 0);

    return 0;
}

/* չʾMTP �豸�ļ��� */
static int __mtp_getfoldertree (int argc, char **argv, struct aw_shell_user *p_user)
{
    u32                     storageid;
    char                   *endptr;
    int                     find = 0;
    LIBMTP_devicestorage_t *temp;

    if (rawdevices == NULL) {
        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
        return 0;
    }
    if (device == NULL) {
        aw_kprintf("Please run \"mtp-open\" cmd open a device first.\r\n");
        return 0;
    }

    if (argc != 2) {
        if (argc == 1) {
            if (strcmp(argv[0], "--help") == 0) {
                aw_kprintf("Command:mtp-ls <storageid> <parentpath>\r\n\r\n");
                aw_kprintf("Description:show MTP device specifc folder's file tree\r\n\r\n");
                aw_kprintf("parameter:\r\n");
                aw_kprintf("    1.storageid  :storage id(0 indicate the first storage)\r\n");
                aw_kprintf("    2.parentpath :path of the parent folder\r\n");
                return 0;
            } else {
                aw_kprintf("Try \"mtp-ls --help\" for more information.\r\n");
                return 0;
            }
        } else {
            aw_kprintf("Try \"mtp-ls --help\" for more information.\r\n");
            return 0;
        }
    }

    /* ��ȡ�洢�豸ID */
    storageid = strtoul(argv[0], &endptr, 10);
    if (*endptr != 0) {
        aw_kprintf("illegal value %s\r\n", argv[0]);
        return 0;
    }

    /* ��Ŀ¼ */
    if (strcmp(argv[1], "/") == 0) {
        find = 1;
    }

    /* ʹ��һ���洢�豸 */
    if (storageid == 0) {
        recursive_folder_tree(device, device->storage, LIBMTP_FILES_AND_FOLDERS_ROOT, argv[1], find);
        return 0;
    }

    /* Ѱ�Ҷ�Ӧ�Ĵ洢�豸 */
    temp = device->storage;
    while (temp != NULL) {
        if (temp->id == storageid) {
            break;
        }
        temp = temp->next;
    }
    if (temp == NULL) {
        aw_kprintf("can not find storage\r\n");
        return 0;
    }
    recursive_folder_tree(device, device->storage, LIBMTP_FILES_AND_FOLDERS_ROOT, argv[1], find);

    return 0;
}

/* �����ļ������� */
static int __mtp_newfolder (int argc, char **argv, struct aw_shell_user *p_user)
{
    uint32_t newid;

    if (rawdevices == NULL) {
        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
        return 0;
    }
    if (device == NULL) {
        aw_kprintf("Please run \"mtp-open\" cmd open a device first.\r\n");
        return 0;
    }
    if (argc != 3) {
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:mtp-newfolder <foldername> <parent_id> <storge_id>\r\n\r\n");
            aw_kprintf("Description:create a new folder\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.foldername:new folder name\r\n");
            aw_kprintf("    2.parent_id :parent folder ID, 0 to create the new folder in the root dir\r\n");
            aw_kprintf("    3.storge_id :storge ID, 0 to create the new folder on the primary storage\r\n");
        } else {
            aw_kprintf("Try \"mtp-newfolder --help\" for more information.\r\n");
        }
        return 0;
    }

    /* �����µ��ļ��� */
    newid = LIBMTP_Create_Folder(device, argv[0], atol(argv[1]), atol(argv[2]));
    if (newid == 0) {
        aw_kprintf("\r\nFolder creation failed.\r\n");
        return 0;
    } else {
        aw_kprintf("\r\nNew folder created with ID: %d\r\n", newid);
    }
    return 0;
}

/* ɾ������ */
static int __mtp_obdelete (int argc, char **argv, struct aw_shell_user *p_user)
{
    int FILENAME    = 1;
    int ITEMID      = 2;
    int field_type  = 0;
    int i, ret      = 0;

    /* �����û���豸 */
    if (device == NULL) {
        aw_kprintf("Please run \"mtp-open\" cmd open a device first.\r\n");
        return 0;
    }

    /* ������ */
    if (argc != 2) {
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:mtp-delete [-n] <fileid/trackid> | [-f] <filename>\r\n\r\n");
            aw_kprintf("Description:delete a object\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1-1.-n             :indicate next parameter is a object ID\r\n");
            aw_kprintf("    2-1.fileid/trackid :object ID\r\n");
            aw_kprintf("    1-2.-f             :incidate next parameter is a object name\r\n");
            aw_kprintf("    2-2.filename       :object name\r\n");
        } else {
            aw_kprintf("Try \"mtp-delete --help\" for more information.\r\n");
        }
        return 0;
    }

    /* ������ */
    if (strncmp(argv[0], "-f", 2) == 0) {
        field_type = FILENAME;
        strcpy(argv[0],"");
    } else if (strncmp(argv[0], "-n", 2) == 0) {
        field_type = ITEMID;
        strcpy(argv[0],"0");
    } else {
        aw_kprintf("Try \"mtp-delete --help\" for more information.\r\n");
        return 0;
    }

    for (i = 1; i < argc; i++) {
        u32   id;
        char *endptr;

        if (field_type == ITEMID) {

            /* ��ȡ����ID */
            id = strtoul(argv[i], &endptr, 10);
            if ( *endptr != 0 ) {
                aw_kprintf("illegal value %s .. skipping\r\n", argv[i]);
                id = 0;
            }
        } else {
            if (strlen(argv[i]) > 0) {

                /* ����·�������ҵ���ӦID */
                ret = __find_id_from_name(argv[i], device->storage->id, LIBMTP_FILES_AND_FOLDERS_ROOT,
                                          LIBMTP_FILETYPE_UNKNOWN);
                if (ret <= 0) {
                    aw_kprintf("Failed to delete file:%s\r\n",argv[i]);
                    return 0;
                } else {
                    id = ret;
                }
            } else {
                id = 0;
            }
        }
        if (id > 0 ) {
            aw_kprintf("Deleting %s\r\n", argv[i]);
            ret = LIBMTP_Delete_Object(device, id);
            ret = 0;
        }
        if ( ret != 0 ) {
            aw_kprintf("Failed to delete file:%s\r\n",argv[i]);
            LIBMTP_Dump_Errorstack(device);
            LIBMTP_Clear_Errorstack(device);
            ret = 0;
        }
    }

    return ret;
}

/* ���ö������� */
static int __mtp_obsetname (int argc, char **argv, struct aw_shell_user *p_user)
{
    u32     storageid;
    int     fileid;
    char   *endptr;
    int     ret;
    LIBMTP_devicestorage_t *temp = NULL;

    if (rawdevices == NULL) {
        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
        return 0;
    }
    if (device == NULL) {
        aw_kprintf("Please run \"mtp-open\" cmd open a device first.\r\n");
        return 0;
    }

    if (argc != 3) {
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:mtp-setname <storageid> <file/folder> <new name>\r\n\r\n");
            aw_kprintf("Description:set a file or a folder new name\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.storageid   :storage ID\r\n");
            aw_kprintf("    2.file/folder :file or folder ID or path\r\n");
            aw_kprintf("    3.new name    :new name\r\n");
        } else {
            aw_kprintf("Try \"mtp-setname --help\" for more information.\r\n");
        }
        return 0;
    }

    /* ��ȡ�洢�豸ID */
    storageid = strtoul(argv[0], &endptr, 10);
    if (*endptr != 0) {
        aw_kprintf("illegal value %s\r\n", argv[0]);
        return 0;
    }
    /* ��ȡ�洢�豸*/
    temp = device->storage;
    while (temp != NULL) {
        if (temp->id == storageid) {
            break;
        }
        temp = temp->next;
    }
    /* δ�ҵ��洢�豸���˳�*/
    if(temp == NULL){
        aw_kprintf("illegal storageid %s\r\n", argv[0]);
        return 0;
    }

    /* ��ȡ�ļ����ļ���ID */
    fileid = strtoul(argv[1], &endptr, 10);
    if ((fileid == 0) && (*endptr != 0)) {
        fileid = __find_id_from_name(argv[1], storageid, LIBMTP_FILES_AND_FOLDERS_ROOT,
                                     LIBMTP_FILETYPE_UNKNOWN);
        if (fileid < 0) {
            mtp_printf("Parent folder could not be found, skipping\n");
            return 0;
        }
    } else if ((fileid > 0) && (*endptr != 0)) {
        aw_kprintf("bad file/folder id %u\r\n", fileid);
        return 0;
    }

    ret = LIBMTP_Set_Object_Filename(device, fileid, argv[2]);
    if (ret != 0) {
        aw_kprintf("set name failed\r\n");
    } else {
        aw_kprintf("set name success\r\n");
    }

    return 0;
}

/* ��ȡ�ļ� */
int __mtp_getfile (int argc, char **argv, struct aw_shell_user *p_user)
{
    int     ret = 0;
    int     id;
    char   *endptr;
    char    *file;

    if (rawdevices == NULL) {
        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
        return 0;
    }
    if (device == NULL) {
        aw_kprintf("Please run \"mtp-open\" cmd open a device first.\r\n");
        return 0;
    }

    if (argc != 2) {
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:mtp-getfile <fileid/filepath> <filename>\r\n\r\n");
            aw_kprintf("Description:get a file from MTP device\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.fileid/filepath:ID of the file or path of the file which want to get\r\n");
            aw_kprintf("    2.filename       :name of the file which want to save data \r\n");
        } else {
            aw_kprintf("Try \"mtp-getfile --help\" for more information.\r\n");
        }
        return 0;
    }

    /* ��ȡ��������޷��ų����� */
    id = strtoul(argv[0], &endptr, 10);
    if ((id == 0) && (*endptr != 0)) {
        id = __find_id_from_name(argv[0], device->storage->id, LIBMTP_FILES_AND_FOLDERS_ROOT,
                                 LIBMTP_FILETYPE_UNKNOWN);
        if (id < 0) {
            mtp_printf("Parent folder could not be found, skipping\n");
            return 0;
        }
    } else if ((id > 0) && (*endptr != 0)) {
        aw_kprintf("bad file/track id %u\r\n", id);
        return 0;
    }

    /* �ļ���������"foo.mp3" */
    file = argv[1];
    aw_kprintf("Getting file/track %d to local file %s\r\n", id, file);

    /* ��MTP�豸���ļ����������ص��û��ļ��� */
    if (LIBMTP_Get_File_To_File(device, id, file, progress, NULL) != 0 ) {
        aw_kprintf("\r\nError getting file from MTP device.\r\n");
        ret = 0;
    }

    /* ��ֹ������ */
    aw_kprintf("\r\n");

    return ret;
}

/* �����ļ�*/
int __mtp_sendfile (int argc, char **argv, struct aw_shell_user *p_user)
{
    int             ret = 0;
    char           *filename;
    char           *endptr;
    u64             filesize;
    LIBMTP_file_t  *genfile;
    int             parent_id = 0;

    struct aw_stat  sb;

    if (rawdevices == NULL) {
        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
        return 0;
    }
    if (device == NULL) {
        aw_kprintf("Please run \"mtp-open\" cmd open a device first.\r\n");
        return 0;
    }

    if (argc != 2) {
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:mtp-sendfile <fromfile> <tofolder>\r\n\r\n");
            aw_kprintf("Description:send a file to MTP device\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.fromfile :file path that want to send\r\n");
            aw_kprintf("    2.tofolder :folder path or folder id that want to get\r\n");
        } else {
            aw_kprintf("Try \"mtp-sendfile --help\" for more information.\r\n");
        }
        return 0;
    }
    memset(&sb, 0, sizeof(struct aw_stat));
    aw_kprintf("Sending %s to %s\r\n", argv[0], argv[1]);
    if (aw_stat(argv[0], &sb) != 0) {
        aw_kprintf("%s get state failed\r\n", argv[0]);
        return 0;
    }

    filesize = sb.st_size;
    filename = mtp_basename(argv[0]);
    if(filename == NULL){
        aw_kprintf("%s path is wrong\r\n", argv[0]);
        return 0;
    }

    /* ��ȡMTP�豸��·����Ŀ¼ID */
//    parent_id = parse_path (argv[1], files, folders);
//    if (parent_id == -1) {
//        mtp_printf("Parent folder could not be found, skipping\n");
//        return 0;
//    }

    /* ��ȡ����ID */
    parent_id = strtoul(argv[1], &endptr, 10);
    if ((parent_id == 0) && (*endptr != 0)) {
        parent_id = __find_id_from_name(argv[1], device->storage->id, LIBMTP_FILES_AND_FOLDERS_ROOT,
                                        LIBMTP_FILETYPE_FOLDER);
        if (parent_id < 0) {
            mtp_printf("Parent folder could not be found, skipping\n");
            return 0;
        }
    } else if ((parent_id > 0) && (*endptr != 0)) {
        mtp_printf("Parent folder could not be found, skipping\n");
        return 0;
    }

    /* ����һ�����ļ� */
    genfile = LIBMTP_new_file_t();
    genfile->filesize = filesize;
    genfile->filename = mtp_strdup(filename);
    genfile->filetype = find_filetype (filename);
    genfile->parent_id = parent_id;
    genfile->storage_id = 0;

    mtp_printf("Sending file...\r\n");
    ret = LIBMTP_Send_File_From_File(device, argv[0], genfile, progress, NULL);
    mtp_printf("\n");
    if (ret != 0) {
        mtp_printf("Error sending file.\r\n");
        LIBMTP_Dump_Errorstack(device);
        LIBMTP_Clear_Errorstack(device);
        ret = 0;
    } else {
        mtp_printf("New file ID: %d\r\n", genfile->item_id);
    }

    LIBMTP_destroy_file_t(genfile);

    return ret;
}

///* �ƶ����� */
//int __mtp_obmove (int argc, char **argv, struct aw_shell_user *p_user)
//{
//    u32   oid, stroageid, parentid;
//    char *endptr;
//    int   ret;
//
//    if (rawdevices == NULL) {
//        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
//        return 0;
//    }
//    if (device == NULL) {
//        aw_kprintf("Please run \"mtp-open\" cmd open a device first.\r\n");
//        return 0;
//    }
//
//    if (argc != 3) {
//        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
//            aw_kprintf("Command:mtp-move <fileid> <stroageid> <parentid>\r\n\r\n");
//            aw_kprintf("Description:move a file/folder to another location\r\n\r\n");
//            aw_kprintf("parameter:\r\n");
//            aw_kprintf("    1.fileid    :ID of the file which want to move\r\n");
//            aw_kprintf("    2.stroageid :the destination stroage ID\r\n");
//            aw_kprintf("    3.parentid  :the destination parent ID\r\n");
//        } else {
//            aw_kprintf("Try \"mtp-move --help\" for more information.\r\n");
//        }
//        return 0;
//    }
//
//    /* ��ȡ����ID */
//    oid = strtoul(argv[0], &endptr, 10);
//    if (*endptr != 0) {
//        aw_kprintf("illegal value %s\r\n", argv[0]);
//        return 1;
//    } else if ( ! oid ) {
//        aw_kprintf("bad file/track id %u\r\n", oid);
//        return 1;
//    }
//
//    /* ��ȡ�洢�豸ID */
//    stroageid = strtoul(argv[1], &endptr, 10);
//    if (*endptr != 0) {
//        aw_kprintf("illegal value %s\r\n", argv[1]);
//        return 1;
//    }
//
//    /* ��ȡ��Ŀ¼ID */
//    parentid = strtoul(argv[2], &endptr, 10);
//    if (*endptr != 0) {
//        aw_kprintf("illegal value %s\r\n", argv[2]);
//        return 1;
//    }
//
//    /* �ƶ����� */
//    ret = LIBMTP_Move_Object(device, oid, stroageid, parentid);
//    return ret;
//}
//
///* ���ƶ��� */
//static int __mtp_obcopy (int argc, char **argv, struct aw_shell_user *p_user)
//{
//    u32   oid, stroageid, parentid;
//    char *endptr;
//    int   ret;
//
//    if (rawdevices == NULL) {
//        aw_kprintf("Please run \"mtp-detect\" cmd first.\r\n");
//        return 0;
//    }
//    if (device == NULL) {
//        aw_kprintf("Please run \"mtp-open\" cmd open a device first.\r\n");
//        return 0;
//    }
//
//    if (argc != 3) {
//        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
//            aw_kprintf("Command:mtp-copy <fileid> <stroageid> <parentid>\r\n\r\n");
//            aw_kprintf("Description:copy a file/folder to another location\r\n\r\n");
//            aw_kprintf("parameter:\r\n");
//            aw_kprintf("    1.fileid    :ID of the file which want to move\r\n");
//            aw_kprintf("    2.stroageid :the destination stroage ID\r\n");
//            aw_kprintf("    3.parentid  :the destination parent ID\r\n");
//        } else {
//            aw_kprintf("Try \"mtp-copy --help\" for more information.\r\n");
//        }
//        return 0;
//    }
//
//    /* ��ȡ����ID */
//    oid = strtoul(argv[0], &endptr, 10);
//    if (*endptr != 0) {
//        aw_kprintf("illegal value %s\r\n", argv[0]);
//        return 1;
//    } else if ( ! oid ) {
//        aw_kprintf("bad file/track id %u\r\n", oid);
//        return 1;
//    }
//
//    /* ��ȡ�洢�豸ID */
//    stroageid = strtoul(argv[1], &endptr, 10);
//    if (*endptr != 0) {
//        aw_kprintf("illegal value %s\r\n", argv[1]);
//        return 1;
//    }
//
//    /* ��ȡ��Ŀ¼ID */
//    parentid = strtoul(argv[2], &endptr, 10);
//    if (*endptr != 0) {
//        aw_kprintf("illegal value %s\r\n", argv[2]);
//        return 1;
//    }
//
//    /* �ƶ����� */
//    ret = LIBMTP_Copy_Object(device, oid, stroageid, parentid);
//    return ret;
//}

/* ��� */
aw_local const struct aw_shell_cmd __g_mtp_cmdset_cmds[] = {

        /* ��⵱ǰ���ڵ�MTP�豸 */
        {__mtp_devdetect,     "mtp-detect",      "detect MTP device"},

        /* ��MTP�豸 */
        {__mtp_devopen,       "mtp-open",        "open a MTP device"},

        /* ��ȡMTP�豸�еĴ洢�豸��Ϣ */
        {__mtp_getstorage,    "mtp-getstorage",  "get a MTP device storage info"},

        /* �ر�MTP�豸 */
        {__mtp_devclose,      "mtp-close",       "close a MTP device"},

        /* ��ӡ��MTP�豸��ָ���洢�豸��ָ��Ŀ¼���ļ��� */
        {__mtp_getfoldertree, "mtp-ls",          "show the folder's file tree"},

        /* ��ӡ��MTP�豸��ָ���洢�豸���ļ��� */
        {__mtp_getfiletree,   "mtp-filetree",    "show the file tree"},

        /* ����һ���µ��ļ��� */
        {__mtp_newfolder,     "mtp-newfolder",   "create a new folder"},

        /* ��λMTP�豸(����ʹ��:�ֻ��ϵĲ������ݻᱻ���) */
        {__mtp_resetdev,      "mtp-reset",       "reset a mtp device"},

        /* ɾ��һ���ļ����ļ��� */
        {__mtp_obdelete,      "mtp-delete",      "delete a file or folder"},

        /* ����һ���ļ����ļ��е����� */
        {__mtp_obsetname,     "mtp-setname",   "set a file or a folder name"},

//        /* �ƶ�һ���ļ����ļ��� */
//        {__mtp_obmove,        "mtp-move",        "move a file or a folder"},

//        /* ����һ���ļ����ļ��� */
//        {__mtp_obcopy,        "mtp-copy",        "copy a file or a folder"},

        /* ��ȡһ���ļ�(��MTP�豸�����ļ���U��) */
        {__mtp_getfile,       "mtp-getfile",     "get a file"},

        /* ����һ���ļ�(��U�̷����ļ���MTP�豸) */
        {__mtp_sendfile,      "mtp-sendfile",    "send a file"},

        /* ��ʽ����Ӧ�Ĵ洢�豸(����ʹ��:�ֻ��ϵĲ������ݻᱻ����������ֻ���֧�ָ�ʽ������) */
        {__mtp_storageformat, "mtp-format",      "format a storage"}
};

/* ������� */
aw_local struct aw_shell_cmdset __g_mtp_cmdset = {
    .p_enter_name   = "mtp_test",                  /* �����������*/
    .p_exit_name    = "mtp_exit",                  /* ����˳� */
    .p_man          = "This cmdset is use to test MTP",
    .pfn_always_run = NULL,
    .pfn_entry_init = NULL,
};

/* ������ */
int demo_mtp (void)
{
    int                     ret;
    LIBMTP_error_number_t   err;
    numrawdevices = 0;
    rawdevices = NULL;

    aw_kprintf("\r\n");

    /* MTP���ʼ�� */
    LIBMTP_Init();
    aw_kprintf("libmtp version: %s\r\n", LIBMTP_VERSION_STRING);

#ifdef USB_LOCAL
    /* �ļ�ϵͳ���ص� "/u" ��� */
    do{
        ret = aw_mount("/u", USB_BLK_NAME, "vfat", 0);
        if(ret != 0){
            aw_kprintf("/u mount FATFS Fail: %d!\n", ret);
            aw_mdelay(1000);
        }
    }while(ret != 0);
    aw_kprintf("/u mount success\r\n");
#endif

    aw_kprintf("Listing raw device(s):\r\n");

    /* ���MTP�豸 */
    err = LIBMTP_Detect_Raw_Devices(&rawdevices, &numrawdevices);
    switch(err) {
        case LIBMTP_ERROR_NO_DEVICE_ATTACHED:
            aw_kprintf("   No raw devices found.\r\n");
            break;

        case LIBMTP_ERROR_CONNECTING:
            aw_kprintf("Detect: There has been an error connecting. Exiting\r\n");
            break;

        case LIBMTP_ERROR_MEMORY_ALLOCATION:
            aw_kprintf("Detect: Encountered a Memory Allocation Error. Exiting\r\n");
            break;

        case LIBMTP_ERROR_NONE:
        {
            int i;
            aw_kprintf("   Found %d device(s):\r\n", numrawdevices);
            for (i = 0; i < numrawdevices; i++) {
                if (rawdevices[i].device_entry.vendor != NULL ||
                        rawdevices[i].device_entry.product != NULL) {
                    aw_kprintf("   %s: %s (%04x:%04x) @ bus %d, dev %d\r\n",
                            rawdevices[i].device_entry.vendor,
                            rawdevices[i].device_entry.product,
                            rawdevices[i].device_entry.vendor_id,
                            rawdevices[i].device_entry.product_id,
                            rawdevices[i].bus_location,
                            rawdevices[i].devnum);
                } else {
                    aw_kprintf("   %04x:%04x @ bus %d, dev %d\r\n",
                            rawdevices[i].device_entry.vendor_id,
                            rawdevices[i].device_entry.product_id,
                            rawdevices[i].bus_location,
                            rawdevices[i].devnum);
                }
            }
        }
        break;

        case LIBMTP_ERROR_GENERAL:
        default:
            aw_kprintf("Unknown connection error.\r\n");
            break;
    }

    /* ע��һ�������Ӽ� */
    AW_SHELL_REGISTER_CMDSET(&__g_mtp_cmdset, __g_mtp_cmdset_cmds);
    aw_kprintf("register mtp cmdset:\"mtp_test\"\r\n");

    return 0;
}

/* end of file */
