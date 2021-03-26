/************************************************
 * create by CYX at 3/18-2020
 * Human Interface Device Driver
 ************************************************/
#ifndef __AWBL_HID_CORE_H
#define __AWBL_HID_CORE_H


/*
 * HID��������
 */
#define HID_INPUT_REPORT    0
#define HID_OUTPUT_REPORT   1
#define HID_FEATURE_REPORT  2

#define HID_REPORT_TYPES    3

struct awbl_hid_ll_driver {
    /* �˷���ֻ����һ���Է����豸����*/
    int (*parse)(struct awbl_hid_device *hdev);
    /* ����ԭʼ���������豸*/
    int (*raw_request) (struct awbl_hid_device *hdev, uint8_t reportnum,
            uint8_t *buf, uint32_t len, uint8_t rtype, int reqtype);
};

/* HID�豸����������*/
struct awbl_hid_device {
    /* �豸�ײ�����������*/
    struct awbl_hid_ll_driver *ll_driver;
    /* ��Ӧ��ID*/
    uint32_t vendor;
    /* ����ID*/
    uint32_t product;
    /* �豸����*/
    char name[128];
};


/* ����һ��HID�豸*/
struct awbl_hid_device *awbl_hid_allocate_device(void);



#endif
