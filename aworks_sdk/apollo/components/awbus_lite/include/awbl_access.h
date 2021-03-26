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
 * \brief AWBus �Ĵ������ʷ���
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbus_lite.h"
 * #include "awbl_access.h"
 * \endcode
 *
 * ��ģ��ΪAWBus���豸�����ṩͳһ�ļĴ������ʽӿڣ�ʹ�豸�����ﵽ���̶ȵĸ���
 *
 * ����ʹ�ñ��ӿڱ�д��NS16550�����ȿ�����MCU�ֲ������ϵ�NS16550�������裬
 * Ҳ����������I2C��SPI�������ϵ�NS16550����������
 *
 * \attention ��ʹ�����߿������������豸�����ṩ�ļĴ������ʽӿ�ǰ��
 *            Ҫ���豸�������߿������Ѿ�ʵ������
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
 * \brief �Ĵ������ʲ����ӿ�
 *
 * \param [in]     p_hdl     �豸�Ĵ������ʾ��
 * \param [in]     p_reg     ���߻��豸��صļĴ�����ַ
 * \param [in,out] p_buf     ���ݻ�����ָ��
 * \param [in]     n         ���ݸ���
 *
 * \note ���ṹ�ݲ�֧�ֶ�д��ȴ���32λ�ļĴ���
 */
struct awbl_access_opt {
    /**
     * \name 8λ�����ӿ�
     * @{
     */

    /** \brief 8λ�Ĵ������ӿ� */
    uint8_t (*pfunc_read8)(void *p_hdl, void *p_reg);

    /** \brief 8λ�Ĵ���д�ӿ� */
    void (*pfunc_write8)(void *p_hdl, void *p_reg, uint8_t val);

    /** \brief 8λ�Ĵ����������ӿ� */
    void (*pfunc_read8_rep)(void *p_hdl, void *p_reg, uint8_t *p_buf, int n);

    /** \brief 8λ�Ĵ�������д�ӿ� */
    void (*pfunc_write8_rep)(void          *p_hdl,
                             void          *p_reg,
                             const uint8_t *p_buf,
                             int            n);
    /** @} */

    /**
     * \name 16λ�����ӿ�
     * @{
     */

    /** \brief 16λ�Ĵ������ӿ� */
    uint16_t (*pfunc_read16)(void *p_hdl, void *p_reg);

    /** \brief 16λ�Ĵ���д�ӿ� */
    void (*pfunc_write16)(void *p_hdl, void *p_reg, uint16_t val);

    /** \brief 16λ�Ĵ����������ӿ� */
    void (*pfunc_read16_rep)(void *p_hdl, void *p_reg, uint16_t *p_buf, int n);

    /** \brief 16λ�Ĵ�������д�ӿ� */
    void (*pfunc_write16_rep)(void           *p_hdl,
                              void           *p_reg,
                              const uint16_t *p_buf,
                              int             n);
    /** @} */

    /**
     * \name 32λ�����ӿ�
     * @{
     */

    /** \brief 32λ�Ĵ������ӿ� */
    uint32_t (*pfunc_read32)(void *p_hdl, void *p_reg);

    /** \brief 32λ�Ĵ���д�ӿ� */
    void (*pfunc_write32)(void *p_hdl, void *p_reg, uint32_t val);

    /** \brief 32λ�Ĵ����������ӿ� */
    void (*pfunc_read32_rep)(void     *p_hdl,
                             void     *p_reg,
                             uint32_t *p_buf,
                             int       n);

    /** \brief 32λ�Ĵ�������д�ӿ� */
    void (*pfunc_write32_rep)(void           *p_hdl,
                              void           *p_reg,
                              const uint32_t *p_buf,
                              int             n);
    /** @} */
};

/**
 * \biref AWBus�豸�Ĵ������ʽṹ
 *
 * �ýṹ�����豸���������߿������ṩ�������豸�Ĵ������ʡ�
 */
struct awbl_reg_access {
    void                         *p_hdl;    /**< \brief ���ʼĴ���ʹ�õľ�� */
    const struct awbl_access_opt *p_opt;    /**< \brief ���ʲ����ӿ�ָ�� */
};

/**
 * \brief �豸�Ĵ���ӳ��ӿ�����
 *
 * \param p_dev Ҫӳ��Ĵ������豸
 * \param p_reg Ҫӳ��ļĴ�����ַ
 *
 * \return ӳ��õ��豸�Ĵ������ʽṹָ��
 */
typedef struct awbl_reg_access *(*awbl_dev_reg_map_t)(struct awbl_dev *p_dev,
                                                      void            *p_reg);

/*******************************************************************************
    methods
*******************************************************************************/

/**
 * \brief ��ȡ�Ĵ������ʽṹ�ķ�����
 *
 * Ϊ�˼�㣬ͨ��awbl_dev_method_get()��õķ���ֵ����һ��
 * struct awbl_reg_accessָ�룬���ָ�뽫��Ϊ�豸�Ĵ������ʵľ����
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
