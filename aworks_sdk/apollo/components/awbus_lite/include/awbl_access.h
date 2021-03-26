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
 * \brief AWBus 寄存器访问方法
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbus_lite.h"
 * #include "awbl_access.h"
 * \endcode
 *
 * 本模块为AWBus的设备驱动提供统一的寄存器访问接口，使设备驱动达到最大程度的复用
 *
 * 比如使用本接口编写的NS16550驱动既可驱动MCU局部总线上的NS16550兼容外设，
 * 也可驱动挂在I2C或SPI等总线上的NS16550兼容器件。
 *
 * \attention 在使用总线控制器驱动或设备驱动提供的寄存器访问接口前，
 *            要求设备所在总线控制器已经实例化。
 *
 * \internal
 * \par modification history:
 * - 1.01 12-11-28  orz, add readsXX & writesXX access interfaces
 * - 1.00 12-11-06  orz, first implementation
 * \endinternal
 */

#ifndef __AWBL_ACCESS_H
#define __AWBL_ACCESS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/*******************************************************************************
    includes
*******************************************************************************/
#include "apollo.h"
#include "awbus_lite.h"

/*******************************************************************************
  types
*******************************************************************************/

/**
 * \brief 寄存器访问操作接口
 *
 * \param [in]     p_hdl     设备寄存器访问句柄
 * \param [in]     p_reg     总线或设备相关的寄存器地址
 * \param [in,out] p_buf     数据缓冲区指针
 * \param [in]     n         数据个数
 *
 * \note 本结构暂不支持读写宽度大于32位的寄存器
 */
struct awbl_access_opt {
    /**
     * \name 8位操作接口
     * @{
     */

    /** \brief 8位寄存器读接口 */
    uint8_t (*pfunc_read8)(void *p_hdl, void *p_reg);

    /** \brief 8位寄存器写接口 */
    void (*pfunc_write8)(void *p_hdl, void *p_reg, uint8_t val);

    /** \brief 8位寄存器连续读接口 */
    void (*pfunc_read8_rep)(void *p_hdl, void *p_reg, uint8_t *p_buf, int n);

    /** \brief 8位寄存器连续写接口 */
    void (*pfunc_write8_rep)(void          *p_hdl,
                             void          *p_reg,
                             const uint8_t *p_buf,
                             int            n);
    /** @} */

    /**
     * \name 16位操作接口
     * @{
     */

    /** \brief 16位寄存器读接口 */
    uint16_t (*pfunc_read16)(void *p_hdl, void *p_reg);

    /** \brief 16位寄存器写接口 */
    void (*pfunc_write16)(void *p_hdl, void *p_reg, uint16_t val);

    /** \brief 16位寄存器连续读接口 */
    void (*pfunc_read16_rep)(void *p_hdl, void *p_reg, uint16_t *p_buf, int n);

    /** \brief 16位寄存器连续写接口 */
    void (*pfunc_write16_rep)(void           *p_hdl,
                              void           *p_reg,
                              const uint16_t *p_buf,
                              int             n);
    /** @} */

    /**
     * \name 32位操作接口
     * @{
     */

    /** \brief 32位寄存器读接口 */
    uint32_t (*pfunc_read32)(void *p_hdl, void *p_reg);

    /** \brief 32位寄存器写接口 */
    void (*pfunc_write32)(void *p_hdl, void *p_reg, uint32_t val);

    /** \brief 32位寄存器连续读接口 */
    void (*pfunc_read32_rep)(void     *p_hdl,
                             void     *p_reg,
                             uint32_t *p_buf,
                             int       n);

    /** \brief 32位寄存器连续写接口 */
    void (*pfunc_write32_rep)(void           *p_hdl,
                              void           *p_reg,
                              const uint32_t *p_buf,
                              int             n);
    /** @} */
};

/**
 * \biref AWBus设备寄存器访问结构
 *
 * 该结构体由设备驱动或总线控制器提供，用于设备寄存器访问。
 */
struct awbl_reg_access {
    void                         *p_hdl;    /**< \brief 访问寄存器使用的句柄 */
    const struct awbl_access_opt *p_opt;    /**< \brief 访问操作接口指针 */
};

/**
 * \brief 设备寄存器映射接口类型
 *
 * \param p_dev 要映射寄存器的设备
 * \param p_reg 要映射的寄存器地址
 *
 * \return 映射好的设备寄存器访问结构指针
 */
typedef struct awbl_reg_access *(*awbl_dev_reg_map_t)(struct awbl_dev *p_dev,
                                                      void            *p_reg);

/*******************************************************************************
    methods
*******************************************************************************/

/**
 * \brief 获取寄存器访问结构的方法。
 *
 * 为了简便，通过awbl_dev_method_get()获得的返回值就是一个
 * struct awbl_reg_access指针，这个指针将作为设备寄存器访问的句柄。
 */
AWBL_METHOD_IMPORT(awbl_dev_reg_map);

/*******************************************************************************
    APIs
*******************************************************************************/
aw_err_t awbl_reg_map (struct awbl_dev *p_dev, void *p_reg, void **pp_hdl);

uint8_t  awbl_read8  (void *p_hdl, void *p_reg);
uint16_t awbl_read16 (void *p_hdl, void *p_reg);
uint32_t awbl_read32 (void *p_hdl, void *p_reg);

void awbl_write8  (void *p_hdl, void *p_reg, uint8_t  val);
void awbl_write16 (void *p_hdl, void *p_reg, uint16_t val);
void awbl_write32 (void *p_hdl, void *p_reg, uint32_t val);

void awbl_read8_rep  (void *p_hdl, void *p_reg, uint8_t  *p_buf, int n);
void awbl_read16_rep (void *p_hdl, void *p_reg, uint16_t *p_buf, int n);
void awbl_read32_rep (void *p_hdl, void *p_reg, uint32_t *p_buf, int n);

void awbl_write8_rep (void *p_hdl, void *p_reg, const uint8_t *p_buf, int n);

void awbl_write16_rep(void           *p_hdl,
                      void           *p_reg,
                      const uint16_t *p_buf,
                      int             n);

void awbl_write32_rep(void           *p_hdl,
                      void           *p_reg,
                      const uint32_t *p_buf,
                      int             n);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_ACCESS_H */

/* end of file */
