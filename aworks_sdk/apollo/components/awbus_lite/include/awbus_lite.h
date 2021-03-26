/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus �����ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbus_lite.h"
 * \endcode
 * ��ģ��Ϊ AWBus �ľ����ʵ�֣�������С��Դƽ̨�϶�������Ӳ���豸���й���
 * ʵ��Ӳ���豸���Զ����ֺ�ʵ������
 * ƽ̨�����е�(Ӳ��)�豸ʵ������֯��һ�����νṹ�ϣ��ϲ��豸ʹ�á�method��
 * �ӿ�����ȡ��Щ�豸ʵ���ṩ�Ľӿڡ�
 *
 * �������ݴ���ӡ�����
 *
 * \attention ֻ����ϵͳ��ʼ����ʱ����ע��������������ʵ��������֧�ֶ�̬ע��
 *            ������
 *
 * \internal
 * \par modification history:
 * - 1.01 12-11-02  zen, add function awbl_dev_parent()
 * - 1.00 12-09-21  zen, first implementation
 * \endinternal
 */

#ifndef __AWBUS_LITE_H
#define __AWBUS_LITE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/*******************************************************************************
  defines
*******************************************************************************/

/**
 * \name AWBus-lite �汾��
 * @{
 * �����ʵ���У��汾����8-bit��ʾ��bit[7:0] ���汾�ţ��޸��汾�ź�΢���汾�š�
 */
#define AWBL_VER_1              1       /**< \current version 1.0.0  */

/** @} */

#define AWBL_MAX_DRV_NAME_LEN   31      /* 31-byte device names */

/**
 * \name ��������ID
 * @{
 * ��8-bit��
 * bit[6:0] �������ͣ�Ϊ���涨���ֵ����Χ(0~127)��
 * bit[7] �豸���ͣ�1-���豸Ϊ���߿�������0-���豸Ϊ��ͨ�豸
 */
#define AWBL_DEVID_GET(a)         ((a) & 0x80)
#define AWBL_BUSID_GET(a)         ((a) & 0x7F)

#define AWBL_DEVID_DEVICE         0x00 /**< \brief ��ͨ�豸 */
#define AWBL_DEVID_BUSCTRL        0x80 /**< \brief ���߿����� */

#define AWBL_BUSID_PLB            1  /**< \brief �������������� */

#define AWBL_BUSID_VME            2  /**< \brief VME */
#define AWBL_BUSID_PCI            3  /**< \brief PCI */
#define AWBL_BUSID_PCIX           4  /**< \brief PCI w/ PCI-X ext */
#define AWBL_BUSID_PCIEXPRESS     5  /**< \brief PCI w/ PCI-Express ext */
#define AWBL_BUSID_HYPERTRANSPORT 6  /**< \brief PCI w/ HyperTransport ext */
#define AWBL_BUSID_RAPIDIO        7  /**< \brief RapidIO */
#define AWBL_BUSID_MII            8  /**< \brief MII ���� */
#define AWBL_BUSID_VIRTUAL        9  /**< \brief �������� */

#define AWBL_BUSID_MF             10  /**< \brief �๦���豸���� */
#define AWBL_BUSID_USB_HOST_EHCI  11  /**< \brief EHCI �������� */
#define AWBL_BUSID_USB_HOST_OHCI  12  /**< \brief OHCI �������� */
#define AWBL_BUSID_USB_HOST_UHCI  13  /**< \brief UHCI �������� */
#define AWBL_BUSID_USB_HUB        14  /**< \brief hub �������� */

#define AWBL_BUSID_I2C            20  /**< \brief I2C �������� */
#define AWBL_BUSID_SPI            21  /**< \brief SPI �������� */
#define AWBL_BUSID_SDIO           22  /**< \brief SDIO �������� */
#define AWBL_BUSID_NAND           23  /**< \brief NAND �������� */
#define AWBL_BUSID_USBH           24  /**< \brief USB �������� */
#define AWBL_BUSID_SATA           25  /**< \brief SATA �������� */
#define AWBL_BUSID_EIM            26  /**< \brief EIM �������� */
/** @} */

/**
 * \name ����(method)��صĺ궨��
 * @{
 */

/** \brief �������� */
#define AWBL_METHOD(name, handler)  \
    {(awbl_method_id_t)(&awbl_##name##_desc[0]), (awbl_method_handler_t)handler}

/** \brief �����б������־ */
#define AWBL_METHOD_END     {0, NULL}

/** \brief ����һ������ */
#define AWBL_METHOD_IMPORT(name)    aw_import char aw_const awbl_##name##_desc[]

/** \brief ����һ������ */
#define AWBL_METHOD_DEF(method, string) char aw_const awbl_##method##_desc[]=""

/** \brief �õ�������ID */
#define AWBL_METHOD_CALL(method)    (awbl_method_id_t)(&awbl_##method##_desc[0])

/** @} */

/**
 * \name ��׼����
 * @{
 */

/* �Ƴ��豸�豸ʱ�����ô˷��� , ����AW_OK��ʾ�ɹ�����������ֵ��ʾ�Ƴ�ʧ�� */
AWBL_METHOD_IMPORT(awbl_drv_unlink);

/** @} */

/** \brief �õ��豸ʵ�����豸��Ϣ(����������) */
#define AWBL_DEVINFO_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf->p_devinfo)

/** \brief �õ��豸ʵ�� HCF ��Ϣ(AWBUS������, ���� name, unit, bus_type ����Ϣ) */
#define AWBL_DEVHCF_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf)

/** \brief �õ��豸ʵ�������� */
#define AWBL_DEV_NAME_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf->p_name)

/** \brief �õ��豸ʵ���ĵ�Ԫ�� */
#define AWBL_DEV_UNIT_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf->unit)

/** \brief �õ��豸ʵ������������ */
#define AWBL_DEV_BUS_TYPE_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf->bus_type)

/** \brief �õ��豸ʵ�������߱�� */
#define AWBL_DEV_BUS_INDEX_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf->bus_index)

/*******************************************************************************
 type defines
*******************************************************************************/
typedef struct awbl_dev         awbl_dev_t;
typedef struct awbl_busctlr     awbl_busctlr_t;
typedef struct awbl_drvfuncs    awbl_drvfuncs_t;
typedef struct awbl_drvinfo     awbl_drvinfo_t;
typedef struct awbl_dev_method  awbl_dev_method_t;
typedef struct awbl_buspresent  awbl_buspresent_t;

/** \biref ���ݸ� awbl_dev_iterate() �Ĵ��������� */
typedef aw_err_t (*awbl_iterate_func_t)(struct awbl_dev *p_dev, void *p_arg);

/** \biref ����ID���� */
typedef int *awbl_method_id_t;

/** \biref ������ں������� */
typedef aw_err_t (*awbl_method_handler_t)(struct awbl_dev *p_dev, void *p_arg);

/**
 * \biref �豸����
 *
 * �ýṹ�屻�����豸���ڸ��ϲ�ģ���ṩ�������磬��Դ����
 */
struct awbl_dev_method {
    awbl_method_id_t       dev_method_id;   /**< \brief ������ID */
    awbl_method_handler_t  pfunc_handler;   /**< \brief ��������ڵ� */
};

/**
 * \brief ����������Ϣ
 *
 * ���ṹ���ṩ����������ϵͳ��ʼ�������ʱ���õ�����ڵ㡣
 */
struct awbl_bustype_info {

    /**
     * \brief ��������
     *
     * �������ͣ����磬 AWBL_BUSID_PLB
     */
    uint8_t  bus_id;

    /** \brief ��ʼ���׶�1��ʼ������ */
    int (*pfunc_bustype_init1)(void);

    /** \brief ��ʼ���׶�2��ʼ������ */
    int (*pfunc_bustype_init2)(void);

    /** \brief ��ʼ���׶�3���Ӻ��� */
    int (*pfunc_bustype_connect)(void);

    /** \brief ��������ϵ����豸 */
    int (*pfunc_bustype_newdev_present)(void);

    /** \brief ����豸�������Ƿ�ƥ�� */
    aw_bool_t (*pfunc_bustype_devmatch)(const struct awbl_drvinfo *p_drv,
                                        struct awbl_dev           *p_dev);
};

/**
 * \brief AWBus �豸ʵ���ṹ��
 *
 * ���ṹ�������ߺ��豸������ʱʹ�á����������Ѿ�ע�ᵽ������ϵͳ�С���Щ��Ϣ��
 * ����̽���ö�ٽ׶α���ʹ�á�
 */
struct awbl_dev {

    /** \brief ָ����һ��ʵ�� */
    struct awbl_dev  *p_next;

    /** \brief ָ������ */
    struct awbl_buspresent *p_parentbus;

    /**
     * \brief ָ��������
     *
     * ���豸Ϊ���߿�����ʱ��ָ���������Ƶ��¼����ߣ�
     * Ϊ��ͨ�豸ʱ�����豸�Լ�������;��
     */
    struct awbl_buspresent *p_subbus;

    /** \brief �豸ʵ������Ӧ������ */
    const struct awbl_drvinfo *p_driver;

    /** \brief �豸ʵ������Ӧ��Ӳ��������Ϣ */
    const struct awbl_devhcf *p_devhcf;

    /** \brief AWBus �ڲ�ʹ�� */
    uint32_t    flags;
};

/*
 * The following flags are defined for use with the flags field in
 * the AWBus device instance structure above.
 */
#define AWBL_INST_INIT1_DONE    0x00000010
#define AWBL_INST_INIT2_DONE    0x00000020
#define AWBL_INST_CONNECT_DONE  0x00000040

#define AWBL_REMOVAL_MASK       0x0000f000
#define AWBL_REMOVAL_INSTANCE   0x00001000
#define AWBL_REMOVAL_ORPHAN     0x00002000

/**
 * \biref bus instance
 *
 * Keep track of busses which are known to be present on the system.
 * This is only used internally.
 */
struct awbl_buspresent {
    struct awbl_buspresent         *p_next;    /**< \brief next bus instance */
    const struct awbl_bustype_info *p_bustype; /**< \brief bus type */
    struct awbl_dev     *p_ctlr;         /**< \brief pointer to bus controller*/
    struct awbl_dev     *p_inst_list;    /**< \brief pointer to instance list */
    struct awbl_dev     *p_orphan_list;  /**< \brief pointer to orphan list */
};

/**
 * \brief AWBus ���߿������豸ʵ���ṹ��
 */
struct awbl_busctlr {
    struct awbl_dev        super;
    struct awbl_buspresent buspresent;  /**< \brief ���߿�������Ӧ������ʵ�� */
};

/**
 * \biref ������ڵ�ṹ��
 *
 * ���ṹ���ṩ����������ڵ㣬������ϵͳ�ڳ�ʼ���Ͳ�����ʱ���ʹ����Щ��ڵ㡣
 * �����ݽṹ��������ϵͳ��ʶ����������������ע�ᵽ��ϵͳ�С�
 * ÿ�����������߿��������� aw_drv_register() ������һ�������������ı��ṹ�塣
 */
struct awbl_drvfuncs {

    /*
     * \brief ��ʼ���������豸(��1�׶�)
     *
     * ��������ϵͳ����ʱ����ö�ٺ�����ʱ�����á�������ͨ�����˳�Ա����Ϊ�պ���
     * [nulDrv()]
     * \param[in,out] p_dev   �豸ʵ��
     */
    void (*pfunc_dev_init1) (struct awbl_dev *p_dev);

    /*
     * \brief ��ʼ���������豸(��2�׶�)
     *
     * ��������ϵͳ����ʱ����ö�ٺ�����ʱ�����á�
     * �������� awbl_dev_init2() �б�ֱ�ӵ��á��������߿����������˳�Ա����Ϊ
     * �պ���[nulDrv()]
     *
     *  \param[in,out] p_dev   �豸ʵ��
     */
    void (*pfunc_dev_init2) (struct awbl_dev *p_dev);

    /*
     * \brief ��ʼ���������豸(��3�׶�)
     *
     * ��������ϵͳ����ʱ����ö�ٺ�����ʱ�����á�
     * �������� awbl_dev_connect() �б�ֱ�ӵ��á��������߿����������˳�Ա����Ϊ
     * �պ���[nulDrv()]��
     *
     *  \param[in,out] p_dev   �豸��Ϣ����δ�γ�ʵ��
     */
    void (*pfunc_dev_connect) (struct awbl_dev *p_dev);
};

/**
 * \biref ����ע����Ϣ�ṹ��
 *
 * ���ṹ����������ϵͳ��ʶ�����������ڽ�����ע�ᵽ��ϵͳ�С�
 * ÿ�����������߿��������� awbl_drv_register()
 * ������һ�������������ı��ṹ�塣
 *
 * ���ṹ�岻Ӧ�ñ�ֱ��ʹ�á�ȡ����֮���ǣ�ÿ���������Ͷ�����һ�����Լ��Ľṹ�壬
 * ���ѱ��ṹ����Ϊ���һ����Ա(�Ǳ���������������Ϊһ��ָ��)�����߶���Ľṹ��
 * ���԰����������Ϣ�����磬PCI bus ������� DEV_ID �� VEND_ID��Ϊ�˵õ���Щ��
 * ����Ķ��壬����� awbl_{bus type}.h�����磬��awbl_plb.h����awbl_i2c.h����
 *
 * \attention lite ����������˽ṹ��ʱ������ʹ�� aw_const �ؼ���
 *
 * \par ʾ��
 *
 *  // GPIO ������ڵ�(�豸ʵ����ʼ������)
 *  aw_local aw_const struct awbl_drvfuncs lpc11xx_gpio_drvfuncs = {
 *      lpc11xx_gpio_inst_init1,
 *      lpc11xx_gpio_inst_init2,
 *      lpc11xx_gpio_inst_connect
 *  };
 *
 *  // ��������
 *  awb_local aw_const struct awb_dev_method lpc11xx_gpio_drv_methods = {
 *      AWB_DEVMETHOD(awbGpioFunc, lpc11xx_gpio_func_get),
 *      AWB_DEVMETHOD_END
 *  }
 *
 *  // ����ע����Ϣ
 *  aw_local aw_const awbl_drvinfo_t lpc11xx_gpio_drv_registration = {
 *      AWBL_VER_1,
 *      AWBL_BUSID_PLB,
 *      "lpc11xx_gpio",
 *      &lpc11xx_gpio_drv_funcs,
 *      &lpc11xx_gpio_drv_methods
 *  }
 */
struct awbl_drvinfo {

    /**
     * \brief ֧�ֵ� AWBus �汾��
     *
     * ���磬AWBL_VER_1
     */
    uint8_t     awb_ver;

    /**
     * \brief ����ID
     *
     * ���磬I2C�����豸:AWBL_BUSID_I2C ��
     *                   AWBL_BUSID_I2C | AWBL_DEVID_DEVICE;
     *       I2C���߿�����:AWBL_BUSID_I2C | AWBL_DEVID_DEVICE;
     */
    uint8_t     bus_id;

    /**
     * \brief ������/�豸��
     *
     * ���������֣��������豸��ϱ��ʵ���󣬿���Ϊ�豸������
     */
    char       *p_drvname;

    /**
     * \brief ������ڵ�
     *
     * ϵͳ������ʱ�������Щ��ڵ����ʵ���ĳ�ʼ��
     */
    const struct awbl_drvfuncs *p_busfuncs;

    /**
     * \breif �����ṩ�ķ���
     *
     * �����ṩ�ķ���
     */
    const struct awbl_dev_method *p_methods;

    /**
     * \brief ����̽�⺯��
     *
     * \retval true     ̽��ɹ�
     * \retval false    ̽��ʧ��
     */
    aw_bool_t (* pfunc_drv_probe) (awbl_dev_t *p_dev);

};

/**
 * \brief Ӳ���豸��Ϣ
 *
 * ��Ӳ�������ļ� awbus_lite_hwconf_usrcfg.c ��ʹ�ñ��ṹ��������Ŀ��ϵͳ�ϵ�Ӳ��
 * �豸
 */
struct awbl_devhcf {
    const char          *p_name;    /**< \brief �豸�� */
    uint8_t              unit;      /**< \brief �豸��Ԫ�� */
    uint8_t              bus_type;  /**< \brief �豸�������ߵ����� */
    uint8_t              bus_index; /**< \brief �豸�������ߵı�� */
    struct awbl_dev     *p_dev;     /**< \brief ָ���豸ʵ���ڴ� */
    const void          *p_devinfo; /**< \brief ָ���豸��Ϣ(����) */
};


/**
 * \brief ��ȡӲ���豸�б�
 *
 * \attention ��������Ӧ���ṩ,����awbl_devhcf_list_count_get() ��ȡ�б��Ա����
 *
 * \return Ӳ���豸�б�
 *
 * \sa awbl_devhcf_list_count_get()
 */
aw_import aw_const struct awbl_devhcf *awbl_devhcf_list_get(void);

/**
 * \brief ��ȡӲ���豸�б��Ա����
 *
 * \attention ��������Ӧ���ṩ,����awbl_devhcf_list_get() ��ȡ�б�
 *
 * \return Ӳ���豸�б��Ա����
 *
 * \sa awbl_devhcf_list_get();
 */
aw_import size_t awbl_devhcf_list_count_get(void);


/**
 * \brief ��ȡӲ���豸�б��Ա��
 */
void awbus_lite_init (void);

/**
 * \brief AWBus �豸��ʼ����1�׶�
 *
 * ������ִ���豸��ʼ���ĵ�1�׶Ρ�������ִ��ǰ��Ӧ���ѽ���������� C ���л�����
 * ����ϵͳ������δ�ṩ���������ڱ��׶�ִ�������жϡ������ź����Ȳ�����
 *
 * \retval  AW_OK   �ɹ�
 */
aw_err_t awbl_dev_init1 (void);

/**
 * \brief AWBus �豸��ʼ����2�׶�
 *
 * ������ִ���豸��ʼ���ĵ�2�׶Ρ�������ִ��ǰ������ϵͳ����Ӧ���Ѿ��ܹ�ʹ�ã�
 * ���׶�ִ���豸����Ҫ��ʼ����
 *
 * \retval  AW_OK   �ɹ�
 */
aw_err_t awbl_dev_init2 (void);

/**
 * \brief AWBus �豸��ʼ����3�׶�
 *
 * ������ִ���豸��ʼ���ĵ�3�׶Ρ�������ִ��ǰ��ϵͳ����Ҫ��ʼ���Ѿ���ɣ�����
 * ʹ�ø�����Դ�����ƽ̨֧������(�߳�)����˺�������һ�������������б�ִ�С�
 * �豸��ʱ���ĳ�ʼ������Ӧ�����ڴ˽׶��С�
 *
 * \retval  AW_OK   �ɹ�
 */
aw_err_t awbl_dev_connect (void);

/**
 * \brief ע��һ���豸����
 *
 * ������ע��һ���豸������ AWBus ϵͳ
 *
 * \attention   ֻ���� awbl_dev_init1() ����������֮ǰע������
 *
 * \param[in]   p_drv       ����ע����Ϣ
 *
 * \retval      AW_OK       ע��ɹ�
 * \retval      -EPERM      ������Ĳ��� (�� awbl_dev_init1() ֮��ע������)
 * \retval      -ENOTSUP    ��ǰ AWBus ��֧�ִ˰汾������
 * \retval      -ENOSPC     �ռ䲻��
 */
aw_err_t awbl_drv_register(const struct awbl_drvinfo *p_drvinfo);

/**
 * \brief ע��һ����������
 *
 * ������ע��һ���������͵� AWBus ϵͳ
 *
 * \param[in]   p_bustype   ������Ϣ
 *
 * \retval      AW_OK       ע��ɹ�
 * \retval      -ENOSPC     �ռ䲻��
 */
aw_err_t awbl_bustype_register(const struct awbl_bustype_info *p_bustype);

/**
 * \brief ��������ϵͳ����һ���豸
 *
 * ������������ע����������ͺ��豸���������Ը��ѷ��ֵ��豸ƥ��һ�����ʵ�������
 * ����ҵ���������Ȼ��Ϳ��Դ����豸ʵ������ʵ������ӵ��豸�����������ϡ�
 * ���û���ҵ�ƥ������������豸����ӵ��������ߵĹ¶��б��С�
 *
 * �ڵ��ñ�����֮ǰ��p_dev->p_parentbus ���뱻��ȷ��ֵ�����򣬸��豸����������
 *
 * \param[in,out]   p_dev   δ��ɵ��豸��Ϣ
 *
 * \retval          AW_OK   �ɹ�
 * \retval         -ENOENT  �豸�޸�����
 */
aw_err_t awbl_dev_announce (struct awbl_dev *p_dev);


/**
 * \brief ��������ϵͳ���Ƴ�һ���豸
 *
 * ���������豸���丸������ɾ���������������ṩ��unlink���������豸Ϊ���߿���
 * �������ݹ�ɾ�����������ϵ������豸��
 *
 * \param[in,out]   p_dev   δ��ɵ��豸��Ϣ
 *
 * \retval          AW_OK    �ɹ�
 * \retval          AW_ERROR ʧ��
 */
aw_err_t awbl_dev_remove_announce (struct awbl_dev *p_dev);

/**
 * \brief ��������ϵͳ����һ������ʵ��
 *
 * �����߿���������һ�������ߵ�ʱ�򣬵��ñ���������ʵ������
 *
 * �ڵ��ñ�����֮ǰ��p_dev->p_parentbus ���뱻��ȷ��ֵ�����򣬸����߽���������
 *
 * \param[in,out]   p_busdev        ���߿�����
 * \param[in]       bus_id          Ҫʵ��������������
 *
 * \retval          AW_OK       �ɹ�
 * \retval         -EINVA       ��������p_busctlr Ϊ NULL
 * \retval         -ENODEV      ���߿�������δ������
 * \retval         -ENOENT      δ�ҵ��������� \a bus_id ��δ�ҵ�������
 */
aw_err_t awbl_bus_announce (struct awbl_busctlr *p_busctlr, uint8_t bus_id);

/**
 * \brief ��ÿ���豸��ִ��ָ���Ķ���
 *
 * \a pfunc_action �ķ���ֵ���������Ƿ������
 *    - AW_OK                   ��������
 *    - -EINTR��������AW_OK��ֵ ������ֹ
 *
 * \attention   ��\a flags �������� AWBL_ITERATE_INTRABLE ��־��
 *              ��\a pfunc_action �ķ���ֵ����ֹ��������û�����ã�
 *              �� awbl_dev_iterate() ���Ƿ��� AW_OK��
 *
 * \param[in]   pfunc_action    ��ÿ���豸Ҫִ�еĶ���
 * \param[in]   p_arg           ���ݸ� \a pfunc_action �Ĳ���
 * \param[in]   flags           ������������Ϊ��Ϊ����ֵ�Ļ������
 *                              - AWBL_ITERATE_INSTANCES
 *                              - AWBL_ITERATE_ORPHANS
 *                              - AWBL_ITERATE_INTRABLE
 * \retval      AW_OK           �ɹ�
 * \retval      -EFAULT         \a pfunc_action Ϊ NULL
 * \retval      -EINTR          �������жϣ�δ�ܳɹ����������豸
 */
aw_err_t awbl_dev_iterate(awbl_iterate_func_t  pfunc_action,
                          void                *p_arg,
                          int                  flags);

/* awb_dev_iterate() flags �������� */

#define AWBL_ITERATE_INSTANCES   1 /* ��������ʵ�� */
#define AWBL_ITERATE_ORPHANS     2 /* �������й¶��豸 */
#define AWBL_ITERATE_INTRABLE    4 /* pfunc_action �ķ���ֵ����ֹ���� */

/**
 * \brief �����豸�ķ���
 *
 * ���������豸�в���һ��ָ����������ڵ�
 *
 * \param[in]   p_dev   �豸
 * \param[in]   method  ָ���ķ���
 *
 * \retval      NULL    δ�ҵ��������������
 * \retval      ��NULL  ��������ڵ�(����ָ��)
 */
awbl_method_handler_t awbl_dev_method_get(struct awbl_dev  *p_dev,
                                          awbl_method_id_t  method);

/**
 * \brief �������豸������ָ���ķ���
 *
 * �������������豸(ʵ��)������ָ���ķ����������ݲ���\a p_arg ��
 *
 * \param[in]       method  Ҫ���еķ���
 * \param[in,out]   p_arg   ���з���ʱ����Ĳ���
 *
 * \retval          AW_OK   �ɹ�
 */
aw_err_t awbl_alldevs_method_run(awbl_method_id_t method, void *p_arg);

/**
 * \brief ���������κβ��������Ƿ��� -ENOTSUP
 *
 * \retval  -ENOTSUP    ��֧�ֵĲ���
 */
aw_err_t awbl_rtn_notsup(void);

/**
 * \brief ����������һ��ָ���豸�ĸ���
 *
 * \param p_dev ָ�����豸
 *
 * \return ָ�����豸��ָ�룬��NULL
 */
struct awbl_dev *awbl_dev_parent(struct awbl_dev *p_dev);

/**
 * \brief ����ָ�����ƺ͵�Ԫ�ŵ��豸
 *
 * \param name �豸����
 * \param unit ��Ԫ��
 *
 * \return ָ�����豸��ָ�룬��NULL
 */
awbl_dev_t *awbl_dev_find_by_name(const char *name, int unit);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBUS_LITE_H */

/* end of file */
