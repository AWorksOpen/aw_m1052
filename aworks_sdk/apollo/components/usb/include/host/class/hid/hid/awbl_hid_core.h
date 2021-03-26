/************************************************
 * create by CYX at 3/18-2020
 * Human Interface Device Driver
 ************************************************/
#ifndef __AWBL_HID_CORE_H
#define __AWBL_HID_CORE_H


/*
 * HID报告类型
 */
#define HID_INPUT_REPORT    0
#define HID_OUTPUT_REPORT   1
#define HID_FEATURE_REPORT  2

#define HID_REPORT_TYPES    3

struct awbl_hid_ll_driver {
    /* 此方法只调用一次以分析设备数据*/
    int (*parse)(struct awbl_hid_device *hdev);
    /* 发送原始报告请求到设备*/
    int (*raw_request) (struct awbl_hid_device *hdev, uint8_t reportnum,
            uint8_t *buf, uint32_t len, uint8_t rtype, int reqtype);
};

/* HID设备报告描述符*/
struct awbl_hid_device {
    /* 设备底层驱动函数集*/
    struct awbl_hid_ll_driver *ll_driver;
    /* 供应商ID*/
    uint32_t vendor;
    /* 产商ID*/
    uint32_t product;
    /* 设备名字*/
    char name[128];
};


/* 分配一个HID设备*/
struct awbl_hid_device *awbl_hid_allocate_device(void);



#endif
