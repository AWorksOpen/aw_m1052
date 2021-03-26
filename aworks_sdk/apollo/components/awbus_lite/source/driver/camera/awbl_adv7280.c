/*
 * adv7280.c
 *
 *  Created on: 2020年6月9日
 *      Author: weike
 */


#include "Aworks.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "aw_mem.h"
#include "awbl_plb.h"
#include "aw_cache.h"
#include "aw_camera.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "driver/camera/awbl_adv7280.h"

#define __ADV7280_DEV_DECL(p_this, p_dev) \
            awbl_adv7280_dev_t *(p_this) = \
                (awbl_adv7280_dev_t *)(p_dev)

#define __ADV7280_DEVINFO_DECL(p_info, p_dev) \
            awbl_adv7280_devinfo_t *(p_info) = \
                (awbl_adv7280_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

#define VIDEO_SIZE(r)       (VIDEO_EXTRACT_WIDTH(r) * \
                             VIDEO_EXTRACT_HEIGHT(r))
#define PHOTO_SIZE(r)       (VIDEO_EXTRACT_WIDTH(r) * \
                             VIDEO_EXTRACT_WIDTH(r))
#define PHOTO_MEM_SIZE(p, n, bn) \
                            (((p)*(n)*(bn)+AW_CACHE_LINE_SIZE-1) \
                             / AW_CACHE_LINE_SIZE * AW_CACHE_LINE_SIZE)

#define __PHOTO_MODE_STATE      0x04000000
#define __VIDEO_MODE_STATE      0x08000000

#define __PHOTO_SEAL_STATE      0x10000000
#define __PHOTO_RUN_STATE       0x20000000
#define __VIDEO_RUN_STATE       0x40000000

#define __STANDBY_STATE         0x80000000

#define __STATUS_SET(x, v)      ((x) |= (v))
#define __STATUS_CLR(x, v)      ((x) &= (~(v)))
#define __STATUS_IS(x, v)       ((x) & (v))

#define __USER1_MAP     1
#define __USER2_MAP     2
#define __VDP_MAP       3
#define __VPP_MAP       4
#define __CSI_MAP       5

#define __POWER_DOWN_MASK       (0x01U<<5)

#define __POWER_ON      0
#define __POWER_OFF     1

#define __MANUAL_SWITCH 0

/******************************I2C TRANSLATE**********************************/

aw_local void __i2c_init(void *p_cookie, uint8_t sub_addr);
aw_local aw_err_t __i2c_reg_write(void *p_cookie, uint8_t reg, uint8_t data);
aw_local aw_err_t __i2c_reg_read (void *p_cookie, uint8_t reg, uint8_t *data);
aw_local aw_err_t __i2c_reg_set  (void *p_cookie, uint8_t reg, uint8_t data, uint8_t mask);

inline void __i2c_init(void *p_cookie, uint8_t sub_addr)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    __ADV7280_DEVINFO_DECL(p_devinfo, p_cookie);

    aw_i2c_mkdev(&p_this->i2c_device,
                 p_devinfo->i2c_master_busid,
                 (uint16_t)sub_addr,
                 AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);
}

inline aw_err_t __i2c_reg_write(void *p_cookie, uint8_t reg, uint8_t data)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);

    return aw_i2c_write(&p_this->i2c_device, reg, &data, 1);
}

inline aw_err_t __i2c_reg_read(void *p_cookie, uint8_t reg, uint8_t *data)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);

    return aw_i2c_read(&p_this->i2c_device, reg, data, 1);
}

aw_local aw_err_t __i2c_reg_set(void *p_cookie, uint8_t reg, uint8_t data, uint8_t mask)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    uint8_t  tmp;

    __i2c_reg_read(p_this, reg, &tmp);

    tmp &= (~mask);
    tmp |= (mask & data);

    return __i2c_reg_write(p_this, reg, tmp);
}

/*****************************************************************************/


/***************************** ADV7280 SETTING *******************************/

aw_local aw_err_t __adv7280_iic_map(void *p_cookie, uint8_t map)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    __ADV7280_DEVINFO_DECL(p_devinfo, p_cookie);
    aw_err_t ret = AW_OK;

    __i2c_init(p_this, p_devinfo->i2c_user_addr);

    switch (map) {
    case __USER1_MAP:
        ret = __i2c_reg_write(p_this, ADI_CONTROL, 0x00);
        break;
    case __USER2_MAP:
        ret = __i2c_reg_write(p_this, ADI_CONTROL, 0x40);
        break;
    case __VDP_MAP:
        ret = __i2c_reg_write(p_this, ADI_CONTROL, 0x20);
        break;
    case __VPP_MAP:
        ret = __i2c_reg_write(p_this, ADI_CONTROL, 0x00);
        ret = __i2c_reg_write(p_this, VPP_SLAVE_ADDRESS, 0x84);
        __i2c_init(p_this, ADV7280_IIC_VPP_ADDR);
        ret = __i2c_reg_write(p_this, 0xA3, 0x00);
        break;
    case __CSI_MAP:
        ret = __i2c_reg_write(p_this, ADI_CONTROL, 0x00);
        ret = __i2c_reg_write(p_this, CSI_TX_SLAVE_ADDRESS, 0x88);
        __i2c_init(p_this, ADV7280_IIC_CSI_ADDR);
        break;
    default:
        ret = -AW_EINVAL;
        break;
    }

    return ret;
}

aw_local aw_err_t __adv7280_power(void *p_cookie, uint8_t on_off)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    return __i2c_reg_write(p_this, POWER_MANAGEMENT, on_off<<5);
}

/* USER MAP 1 */
aw_local aw_err_t __adv7280_input_select(void *p_cookie, uint8_t pin)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    int ret;

#if __MANUAL_SWITCH
    ret = __i2c_reg_write(p_this, ADC_SWITCH2, 0x80);
    ret = __i2c_reg_write(p_this, ADC_SWITCH1, pin+1);
    ret = __i2c_reg_write(p_this, 0x0E, 0x80);
    ret = __i2c_reg_write(p_this, 0x9C, 0x00);
    ret = __i2c_reg_write(p_this, 0x9C, 0xFF);
#else
    ret = __i2c_reg_write(p_this, INPUT_CONTROL, pin);
    ret = __i2c_reg_write(p_this, 0x0E, 0x80);
    ret = __i2c_reg_write(p_this, 0x9C, 0x00);
    ret = __i2c_reg_write(p_this, 0x9C, 0xFF);
#endif

    return ret;
}


/*****************************************************************************/


/*********************************INIT SETTING********************************/

/**
 * \brief find out a service who accept the pin
 */
aw_local struct awbl_camera_controller_service * __camera_controller_to_serv (void *p_cookie)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    __ADV7280_DEVINFO_DECL(p_devinfo, p_cookie);
    struct awbl_camera_controller_service *p_serv_cur = p_this->p_controller_serv;

    /* todo: if only one service, just return it directly */

    while ((p_serv_cur != NULL)) {

        if (p_devinfo->camera_data_bus_id == p_serv_cur->id) {

            return p_serv_cur;
        }
        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}


/**  relative configuration of initialize interface
 *  connect CSI,allocate memory,configure IO...
 */

AWBL_METHOD_IMPORT(awbl_cameracontrollerserv_get);

aw_local aw_err_t __camera_controller_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t      pfunc_camera_controller_serv = NULL;
    struct awbl_camera_service  *p_camera_serv     = NULL;

    /** find handler */

    pfunc_camera_controller_serv = awbl_dev_method_get(p_dev,
                                          AWBL_METHOD_CALL(awbl_cameracontrollerserv_get));

    /** call method handler to allocate camera service */

    if (pfunc_camera_controller_serv != NULL) {

        pfunc_camera_controller_serv(p_dev, &p_camera_serv);

        /** found a camera service, insert it to the service list */

        if (p_camera_serv != NULL) {
            struct awbl_camera_service **pp_serv_cur = (struct awbl_camera_service **)p_arg;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_camera_serv;
             p_camera_serv->p_next = NULL;
        }
    }

    return AW_OK;
}

/**
 * \brief 获取adv7280对应csi的子方法
 */
aw_local void __adv7280_sub_method_init(void *p_cookie)
{
    /* 初始化CSI */
    __ADV7280_DEV_DECL(p_this, p_cookie);

    p_this->p_controller_serv = NULL;

    /* 在打开中枚举相应的服务  */
    awbl_dev_iterate(__camera_controller_serv_alloc_helper,
                     &p_this->p_controller_serv,
                     AWBL_ITERATE_INSTANCES);

}



aw_local aw_err_t __adv7280_video_mem_reset(void *p_cookie, uint32_t video_resolution)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    __ADV7280_DEVINFO_DECL(p_devinfo, p_cookie);
    uint32_t mem_size;

    /* 如果修改的分辨率与原分辨率一致，则返回 */
    if (video_resolution == p_this->video_resolution) {
        return AW_OK;
    }

    p_this->video_resolution = video_resolution;

    if (p_this->cfg.p_video_buf_addr != NULL) {
        aw_mem_free(p_this->cfg.p_video_buf_addr);
    }

    mem_size = PHOTO_MEM_SIZE(p_this->cfg.bytes_per_pix,
                              VIDEO_SIZE(p_this->video_resolution),
                              p_devinfo->buf_num);

    if (mem_size == 0) {
        p_this->cfg.p_video_buf_addr = NULL;
        __STATUS_CLR(p_this->status, __VIDEO_MODE_STATE);
    } else {
        p_this->cfg.p_video_buf_addr =
                        (uint32_t *)aw_mem_align(mem_size, AW_CACHE_LINE_SIZE);
        if (p_this->cfg.p_video_buf_addr == NULL) {
            return -AW_ENOMEM;
        }
        __STATUS_SET(p_this->status, __VIDEO_MODE_STATE);
    }

    return AW_OK;
}


aw_local aw_err_t __adv7280_photo_mem_reset(void *p_cookie, uint32_t photo_resolution)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    uint32_t mem_size;

    /* 如果修改的分辨率与原分辨率一致，则返回 */
    if (photo_resolution == p_this->photo_resolution) {
        return AW_OK;
    }

    p_this->photo_resolution = photo_resolution;

    if (p_this->cfg.p_photo_buf_addr != NULL) {
        aw_mem_free(p_this->cfg.p_photo_buf_addr);
    }

    mem_size = PHOTO_MEM_SIZE(p_this->cfg.bytes_per_pix,
                              PHOTO_SIZE(p_this->photo_resolution),
                              2);

    if (mem_size == 0) {
        p_this->cfg.p_photo_buf_addr = NULL;
        __STATUS_CLR(p_this->status, __PHOTO_MODE_STATE);
    } else {
        p_this->cfg.p_photo_buf_addr =
                        (uint32_t *)aw_mem_align(mem_size, AW_CACHE_LINE_SIZE);
        if (p_this->cfg.p_photo_buf_addr == NULL) {
            return -AW_ENOMEM;
        }
        __STATUS_SET(p_this->status, __PHOTO_MODE_STATE);
    }

    return AW_OK;
}

aw_local aw_err_t __adv7280_restart(void *p_cookie,
                                    uint32_t video_resolution,
                                    uint32_t photo_resolution)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;

    __adv7280_iic_map(p_this, __USER1_MAP);

    if (!__STATUS_IS(p_this->status, __STANDBY_STATE)) {
        p_this->camera_serv.p_servfuncs->\
            pfunc_camera_close(p_this->p_controller_serv->p_cookie);
    }

    ret = __adv7280_video_mem_reset(p_this, video_resolution);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __adv7280_photo_mem_reset(p_this, photo_resolution);
    if (ret != AW_OK) {
        return ret;
    }

    if (!__STATUS_IS(p_this->status, __STANDBY_STATE)) {
        if (__STATUS_IS(p_this->status, __VIDEO_RUN_STATE)) {
            p_this->cfg.resolution   = p_this->video_resolution;
            p_this->cfg.current_mode = VIDEO;
            ret = p_this->p_controller_serv->p_servfuncs->open\
                    (p_this->p_controller_serv->p_cookie, &p_this->cfg);
        } else if (__STATUS_IS(p_this->status, __PHOTO_RUN_STATE)) {
            p_this->cfg.resolution   = p_this->photo_resolution;
            p_this->cfg.current_mode = PHOTO;
            ret = p_this->p_controller_serv->p_servfuncs->open\
                    (p_this->p_controller_serv->p_cookie, &p_this->cfg);
        }
    }

    return ret;
}

aw_local aw_err_t __adv7280_init(void *p_cookie, uint8_t pin)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    __ADV7280_DEVINFO_DECL(p_devinfo, p_cookie);
    uint8_t tmp = 0;
    int ret;

    aw_mdelay(10);

    if (p_devinfo->control_flags & AW_CAM_DATA_LATCH_ON_RISING_EDGE) {
        tmp |= 0x01;
    }
    if (p_devinfo->control_flags & AW_CAM_HREF_ACTIVE_LOW) {
        tmp |= 0x80;
    }
    if (p_devinfo->control_flags & AW_CAM_VSYNC_ACTIVE_LOW) {
        tmp |= 0x20;
    }


    ret = __i2c_reg_write(p_this, POWER_MANAGEMENT, 0xA0);
    aw_mdelay(5);

    ret = __adv7280_power(p_this, __POWER_ON);

    ret = __adv7280_input_select(p_this, pin);

    //Enter User Sub Map
    ret = __i2c_reg_write(p_this, ADI_CONTROL, 0x00);
    //Enable Pixel & Sync output drivers
    ret = __i2c_reg_write(p_this, OUTPUT_CONTROL, 0x0C);
    //Power-up INTRQ, HS & VS pads
    ret = __i2c_reg_write(p_this, EXTENDED_OUTPUT_CONTROL, 0x07);

    //Enable INTRQ output driver
    ret = __i2c_reg_write(p_this, STATUS3, 0x00);
    //Enable SH1
    ret = __i2c_reg_write(p_this, SHAPING_FILTER_CONTROL1, 0x41);
    //Enable LLC output driver
    ret = __i2c_reg_write(p_this, ADI_CONTROL2, 0x40);

    //HSYNC 输出HS有效信号
    ret = __i2c_reg_write(p_this, OUTPUT_SYNC_SELECT1, 0x00);
    //VSYNC 输出VS有效信号
    ret = __i2c_reg_write(p_this, OUTPUT_SYNC_SELECT2, 0x01);
    //Polarity setting
    ret = __i2c_reg_write(p_this, POLARITY, tmp);

    // ADI Required Write
    ret = __i2c_reg_write(p_this, 0x52, 0xCD);
    ret = __i2c_reg_write(p_this, 0x80, 0x51);
    ret = __i2c_reg_write(p_this, 0x81, 0x51);
    ret = __i2c_reg_write(p_this, 0x82, 0x68);

    if (p_devinfo->fast_enbale) {
        ret = __i2c_reg_write(p_this, 0x0E, 0x80);
        ret = __i2c_reg_write(p_this, 0xD9, 0x44);
        ret = __i2c_reg_write(p_this, 0x0E, 0x00);

        //Select fast Switching Mode.
        __adv7280_iic_map(p_this, __USER2_MAP);
        ret = __i2c_reg_write(p_this, FL_CONTROL, 0x01);
    }
    if (p_devinfo->ccir_mode == CCIR_PROGRESSIVE_MODE) {
        //Enable interlaced-to-progressive
        __adv7280_iic_map(p_this, __VPP_MAP);
        ret = __i2c_reg_write(p_this, ADV_TIMING_MODE_EN, 0x00);
        ret = __i2c_reg_write(p_this, I2C_DEINT_ENABLE, 0x80);
    }

    ret = __adv7280_iic_map(p_this, __USER1_MAP);

    return ret;
}

aw_local aw_err_t __mem_init(void *p_dev)
{
    __ADV7280_DEV_DECL(p_this, p_dev);
    __ADV7280_DEVINFO_DECL(p_devinfo, p_dev);

    p_this->cfg.p_photo_buf_addr = NULL;
    p_this->cfg.p_photo_buf_addr = NULL;
    p_this->cfg.buf_num = p_devinfo->buf_num;

    return __adv7280_restart(p_this, p_devinfo->video_resolution, p_devinfo->photo_resolution);
}


aw_local void __adv7280_inst_init1(awbl_dev_t *p_dev) {}

aw_local void __adv7280_inst_init2(awbl_dev_t *p_dev)
{
    __ADV7280_DEVINFO_DECL(p_devinfo, p_dev);
    p_devinfo->pfunc_plfm_init();
}

aw_local void __adv7280_inst_connect(awbl_dev_t *p_dev)
{
    __ADV7280_DEV_DECL(p_this, p_dev);
    __ADV7280_DEVINFO_DECL(p_devinfo, p_dev);
    aw_err_t ret;

    AW_MUTEX_INIT(p_this->dev_lock, AW_PSP_SEM_Q_FIFO);

    /* 当前摄像头没有被打开  */
    p_this->status = 0;

    p_this->photo_resolution = 0;
    p_this->video_resolution = 0;

    p_this->cfg.bytes_per_pix = 2;
    p_this->cfg.control_flags = p_devinfo->control_flags;
    p_this->cfg.resolution = 0;

    __STATUS_SET(p_this->status, __STANDBY_STATE);
    ret = __mem_init(p_this);
    if (ret != AW_OK) {
        aw_kprintf("adv7280 memory init failed: %d\n\r", ret);
        return;
    }

    ret = __adv7280_init(p_this, p_devinfo->ain_channel);
    if (ret != AW_OK) {
        aw_kprintf("adv7280 init failed: %d\n\r", ret);
        return;
    }

    __adv7280_sub_method_init(p_this);

}

/*****************************************************************************/


/***************************APPLICATION INTERFACE*****************************/

aw_local aw_err_t __camera_open(void *p_cookie)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    p_this->p_controller_serv = __camera_controller_to_serv(p_cookie);
    if (p_this->p_controller_serv == NULL) {
        return -AW_ENXIO;
    }

    if (ret !=  AW_OK) {
        AW_MUTEX_UNLOCK(p_this->dev_lock);
        return ret;
    }

    __STATUS_CLR(p_this->status, __STANDBY_STATE);
    __STATUS_CLR(p_this->status, __VIDEO_RUN_STATE);

    AW_MUTEX_UNLOCK(p_this->dev_lock);

    return AW_OK;
}

aw_local aw_err_t __camera_close(void *p_cookie)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    aw_err_t ret = 0;

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    if (__STATUS_IS(p_this->status, __PHOTO_RUN_STATE|__VIDEO_RUN_STATE)) {
        ret = p_this->p_controller_serv->p_servfuncs->close(p_this->p_controller_serv->p_cookie);
    }

    __STATUS_SET(p_this->status, __STANDBY_STATE);

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return ret;
}

aw_local aw_err_t __camera_get_photo_buf(void *p_cookie, uint32_t **p_buf)
{
    aw_err_t ret = 0;

    __ADV7280_DEV_DECL(p_this, p_cookie);

    if (!__STATUS_IS(p_this->status, __PHOTO_MODE_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);


    if (!__STATUS_IS(p_this->status, __PHOTO_RUN_STATE)) {
        /* 打开默认是预览的分辨率 */
        p_this->cfg.resolution = p_this->photo_resolution;
        p_this->cfg.current_mode = PHOTO;
        __STATUS_CLR(p_this->status, __VIDEO_RUN_STATE);
        __STATUS_SET(p_this->status, __PHOTO_RUN_STATE);

        /* 以拍照模式打开摄像头 */
        ret = p_this->p_controller_serv->p_servfuncs->open(p_this->p_controller_serv->p_cookie, &p_this->cfg);
        if (ret != AW_OK) {
            AW_MUTEX_UNLOCK(p_this->dev_lock);
            return ret;
        }
    }

    ret = p_this->p_controller_serv->p_servfuncs->get_photo_buf(p_this->p_controller_serv->p_cookie, p_buf);

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return ret;
}

aw_local aw_err_t __camera_release_photo_buf(void *p_cookie, uint32_t addr)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;

    if (!__STATUS_IS(p_this->status, __PHOTO_MODE_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);
    ret = p_this->p_controller_serv->p_servfuncs->release_photo_buf(p_this->p_controller_serv->p_cookie, addr);
    AW_MUTEX_UNLOCK(p_this->dev_lock);

    return ret;
}

aw_local aw_err_t __camera_get_video_buf(void *p_cookie, uint32_t **p_buf)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    aw_err_t ret = AW_OK;

    if (!__STATUS_IS(p_this->status, __VIDEO_MODE_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);


    if (!__STATUS_IS(p_this->status, __VIDEO_RUN_STATE)) {

        /* 打开默认是预览的分辨率 */
        p_this->cfg.resolution = p_this->video_resolution;
        p_this->cfg.current_mode = VIDEO;
        __STATUS_SET(p_this->status, __VIDEO_RUN_STATE);
        __STATUS_CLR(p_this->status, __PHOTO_RUN_STATE);

        /* 以拍照模式打开摄像头 */
        ret = p_this->p_controller_serv->p_servfuncs->open(p_this->p_controller_serv->p_cookie, &p_this->cfg);
        if (ret != AW_OK) {
            AW_MUTEX_UNLOCK(p_this->dev_lock);
            return ret;
        }
    }

    ret = p_this->p_controller_serv->p_servfuncs->get_video_buf(p_this->p_controller_serv->p_cookie, p_buf);


    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return ret;
}

aw_local aw_err_t __camera_release_video_buf(void *p_cookie, uint32_t  addr)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;

    if (!__STATUS_IS(p_this->status, __VIDEO_MODE_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);
    ret = p_this->p_controller_serv->p_servfuncs->release_video_buf(p_this->p_controller_serv->p_cookie, addr);
    AW_MUTEX_UNLOCK(p_this->dev_lock);

    return ret;
}

aw_local aw_err_t __camera_config_set(void *p_cookie,
                                      aw_camera_cfg_t *p_cfg,
                                      uint32_t *option)
{
    __ADV7280_DEV_DECL(p_this, p_cookie);
    __ADV7280_DEVINFO_DECL(p_devinfo, p_cookie);
    int ret;
    uint8_t tmp;

    if (*option & AW_CAMERA_PRIVATE) {
        if (((int)p_cfg->private_data < 0) && ((int)p_cfg->private_data > 3)) {
            *option &= (~AW_CAMERA_PRIVATE);
        }

        __camera_close(p_this);
#if __MANUAL_SWITCH
        /* 关闭模拟通道与数字通道 */
        ret = __i2c_reg_set(p_this, ANALOG_CLAMP_CONTROL, 0x00, 0x10);
        ret = __i2c_reg_set(p_this, DIGITAL_CLAMP_CONTROL1, 0x10, 0x10);
#endif

        tmp = (uint8_t)p_cfg->private_data & 0x03;
        __adv7280_iic_map(p_this, __USER1_MAP);
        ret = __adv7280_input_select(p_this, tmp);
        if (ret != AW_OK) {
            *option &= (~AW_CAMERA_PRIVATE);
        }

        // ADI Required Write
        ret = __i2c_reg_write(p_this, 0x52, 0xCD);
        ret = __i2c_reg_write(p_this, 0x80, 0x51);
        ret = __i2c_reg_write(p_this, 0x81, 0x51);
        ret = __i2c_reg_write(p_this, 0x82, 0x68);

        if (p_devinfo->fast_enbale) {
            ret = __i2c_reg_write(p_this, 0x0E, 0x80);
            ret = __i2c_reg_write(p_this, 0xD9, 0x44);
            ret = __i2c_reg_write(p_this, 0x0E, 0x00);
            aw_mdelay(100);
        } else {
            aw_mdelay(300);
        }
#if __MANUAL_SWITCH
        /* 启动模拟通道与数字通道 */
        ret = __i2c_reg_set(p_this, ANALOG_CLAMP_CONTROL, 0x10, 0x10);
        ret = __i2c_reg_set(p_this, DIGITAL_CLAMP_CONTROL1, 0x00, 0x10);
#endif
        __camera_open(p_this);
    }

    return AW_OK;
}

aw_local aw_err_t __camera_config_get(void *p_cookie,
                                      aw_camera_cfg_t *p_cfg,
                                      uint32_t *option)
{
    return -AW_EPERM;
}

/*****************************************************************************/


/*****************************************************************************/

aw_local aw_const struct awbl_camera_servfuncs __g_camera_servfuncs = {

    __camera_open,

    __camera_close,

    __camera_get_photo_buf,

    __camera_get_video_buf,

    __camera_release_photo_buf,

    __camera_release_video_buf,

    __camera_config_set,

    __camera_config_get,

};

/**
 * \brief 获取OV75640服务的函数列表
 *
 * awbl_adv7280serv_get的运行时实现
 * \param   p_dev   AWbus传下来表征当前设备
 * \param   p_arg   用于输出ADV7280服务函数列表的指针
 * \return  无
 */
aw_local aw_err_t __adv7280_cameraserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __ADV7280_DEV_DECL(p_this, p_dev);

    struct awbl_camera_service *p_serv = &p_this->camera_serv;

    /* initialize the adv7280 service instance */
    p_serv->p_next      = NULL;
    p_serv->p_name      = AWBL_ADV7280_NAME;
    p_serv->p_servfuncs = &__g_camera_servfuncs;
    p_serv->p_cookie    = (void *)p_this;
    p_serv->flag        = AW_CAMERA_FLAG_ALIVE;
    p_serv->used_count  = 1;

    /* send back the service instance */
    *(struct awbl_camera_service **)p_arg = p_serv;

    return AW_OK;
}

/**
 * \brief ADV7280驱动AWbus驱动函数
 */
aw_local aw_const struct awbl_drvfuncs __g_adv7280_drvfuncs = {
    __adv7280_inst_init1,
    __adv7280_inst_init2,
    __adv7280_inst_connect
};

/******************************************************************************/

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_cameraserv_get);


/**
 * \brief ADV7280驱动设备专有函数
 */
aw_local aw_const struct awbl_dev_method __g_adv7280_dev_methods[] = {
    AWBL_METHOD(awbl_cameraserv_get, __adv7280_cameraserv_get),
    AWBL_METHOD_END
};


/**
 * \brief ADV7280驱动注册信息
 */
aw_local aw_const awbl_plb_drvinfo_t __g_adv7280_drv_registration = {
    {
        AWBL_VER_1,                 /* awb_ver */
        AWBL_BUSID_PLB,             /* bus_id */
        AWBL_ADV7280_NAME,           /* p_drvname */
        &__g_adv7280_drvfuncs,       /* p_busfuncs */
        &__g_adv7280_dev_methods[0], /* p_methods */
        NULL                        /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_adv7280_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_adv7280_drv_registration);
}

