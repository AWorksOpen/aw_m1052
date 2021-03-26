/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief IMX10xx FlexIO UART Driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-02  pea, first implementation
 * \endinternal
 */

/*******************************************************************************
  ͷ�ļ�����
*******************************************************************************/
#include "aworks.h"
#include "aw_int.h"
#include "aw_serial.h"
#include "aw_assert.h"
#include "aw_bitops.h"
#include "driver/flexio/imx10xx_flexio_regs.h"
#include "driver/flexio/awbl_imx10xx_flexio_uart.h"

/*******************************************************************************
  �궨��
*******************************************************************************/
#define __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev)   \
          struct awbl_imx10xx_flexio_uart_devinfo *p_devinfo = \
          (struct awbl_imx10xx_flexio_uart_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __DEV_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_dev)   \
          struct awbl_imx10xx_flexio_uart_chan *p_chan = \
          &(((struct awbl_imx10xx_flexio_uart_dev *)p_dev)->chan)

#define __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan) \
          struct awbl_dev *p_dev =                     \
          &AW_CONTAINER_OF(p_chan, struct awbl_imx10xx_flexio_uart_dev, chan)->dev;

#define __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan) \
          struct awbl_imx10xx_flexio_uart_chan *p_chan =         \
          AW_CONTAINER_OF(p_sio_chan, struct awbl_imx10xx_flexio_uart_chan, sio_chan);

#define __IMX10xx_CONSOLE_BAUD_RATE        115200

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/** \brief ������ڵ㺯�� */
aw_local void __flexio_uart_inst_init1 (struct awbl_dev *p_dev);
aw_local void __flexio_uart_inst_init2 (struct awbl_dev *p_dev);
aw_local void __flexio_uart_inst_connect (struct awbl_dev *p_dev);

/** \brief �豸���ƺ��� */
aw_local aw_err_t __flexio_uart_ioctl (struct aw_sio_chan *p_sio_chan,
                                       int                 cmd,
                                       void               *p_arg);

/** \brief ���÷����ж� */
aw_local aw_err_t __flexio_uart_tx_startup (struct aw_sio_chan *p_chan);

/** \brief �����ж��е��õĽ��ա������ַ��ص����� */
aw_local aw_err_t __flexio_uart_callback_install (
    struct aw_sio_chan *p_sio_chan,
    int                 callback_type,
    aw_err_t          (*pfn_callback)(void *),
    void               *p_callback_arg);

/** \brief ������ѯ���� */
aw_local aw_err_t __flexio_uart_poll_input (struct aw_sio_chan *p_sio_chan,
                                            char               *p_inchar);

/** \brief ������ѯ���� */
aw_local aw_err_t __flexio_uart_poll_output (struct aw_sio_chan *p_sio_chan,
                                             char                outchar);

/** \brief FlexIO UART �жϷ����� */
aw_local void __flexio_uart_isr (struct awbl_imx10xx_flexio_uart_chan *p_chan);

/** \brief FlexIO UART ���������� */
aw_local aw_err_t __flexio_uart_baudrate_set (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    uint32_t                              baudtare);

/** \brief FlexIO UART ģʽ���� */
aw_local aw_err_t __flexio_uart_mode_set (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    int                                   new_mode);

/** \brief FlexIO UART ѡ������ */
aw_local aw_err_t __flexio_uart_opt_set (
    struct awbl_imx10xx_flexio_uart_chan  *p_chan,
    int                                    options);

/** \brief METHOD: ��ȡָ�� p_chan �ṹ���ָ�� */
void imx10xx_flexio_uart_sio_chan_get (awbl_dev_t *p_dev, void *p_arg);

/** \brief METHOD: ����ָ��ͨ���� SIO ͨ�� */
void imx10xx_flexio_uart_sio_chan_connect (awbl_dev_t *p_dev, void *p_arg);

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

/** \brief ������ڵ� */
aw_const struct awbl_drvfuncs g_flexio_uart_awbl_drvfuncs = {
    __flexio_uart_inst_init1,       /**< \brief ��һ�׶γ�ʼ�� */
    __flexio_uart_inst_init2,       /**< \brief �ڶ��׶γ�ʼ�� */
    __flexio_uart_inst_connect      /**< \brief �����׶γ�ʼ�� */
};

/** \brief �豸���� */
aw_local aw_const struct awbl_dev_method g_flexio_uart_plb_awbl_methods[] = {
    AWBL_METHOD(aw_sio_chan_get,        imx10xx_flexio_uart_sio_chan_get),
    AWBL_METHOD(aw_sio_chan_connect,    imx10xx_flexio_uart_sio_chan_connect),
    AWBL_METHOD_END
};

/** \brief ����ע����Ϣ */
aw_local aw_const struct awbl_drvinfo __g_flexio_uart_drv_registration = {
    AWBL_VER_1,                         /**< \brief AWBus �汾�� */
    AWBL_BUSID_PLB,                     /**< \brief ����ID */
    AWBL_IMX10XX_FLEXIO_UART_DRV_NAME,  /**< \brief ������ */
   &g_flexio_uart_awbl_drvfuncs,        /**< \brief ������ڵ� */
    g_flexio_uart_plb_awbl_methods,     /**< \brief �����ṩ�ķ��� */
    NULL                                /**< \brief ����̽�⺯�� */
};

/** \brief SIO �������� */
aw_local aw_const struct aw_sio_drv_funcs __g_imx10xx_flexio_uart_sio_drv_funcs = {
    __flexio_uart_ioctl,
    __flexio_uart_tx_startup,
    __flexio_uart_callback_install,
    __flexio_uart_poll_input,
    __flexio_uart_poll_output
};

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/** \brief FlexIO UART ���лص����� */
aw_local aw_err_t __flexio_uart_dummy_callback (void)
{
    return AW_ERROR;
}

/** \brief FlexIO UART ʧ�� */
aw_local void __flexio_uart_disable (
    struct awbl_imx10xx_flexio_uart_devinfo *p_devinfo)
{
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;

    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    AW_REG_BITS_SET32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], 0, 2, 0);
    while (0 != AW_REG_BITS_GET32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], 0, 2));

    AW_REG_BITS_SET32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], 0, 2, 0);
    while (0 != AW_REG_BITS_GET32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], 0, 2));
}

/** \brief FlexIO UART ʹ�� */
aw_local void __flexio_uart_enable (
    struct awbl_imx10xx_flexio_uart_devinfo *p_devinfo)
{
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;

    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    AW_REG_BITS_SET32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], 0, 2, 1);
    while (1 != AW_REG_BITS_GET32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], 0, 2));

    AW_REG_BITS_SET32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], 0, 2, 1);
    while (1 != AW_REG_BITS_GET32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], 0, 2));
}

/** \brief FlexIO UART �����ж�ʹ�� */
aw_local void __flexio_uart_tx_interrupt_enable (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    int                                   enable)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    p_chan->is_tx_int_en = enable;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (enable) {

        /* ʹ�ܷ����ж� */
        AW_REG_BIT_SET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
    }
    else {

        /* ʧ�ܷ����ж� */
        AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
    }

    aw_spinlock_isr_give(&p_chan->dev_lock);
}

/**
 * \brief FlexIO UART Ӳ��ȥ��ʼ��
 *
 * \param[in] p_chan ָ���豸ͨ����Ϣ�ṹ��ָ��
 */
aw_local void __flexio_uart_hardware_deinit (
    struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    aw_spinlock_isr_take(&p_chan->dev_lock);

    AW_REG_WRITE32(&p_reg->SHIFTCFG[p_devinfo->shifter_index[0]], 0);
    AW_REG_WRITE32(&p_reg->SHIFTCTL[p_devinfo->shifter_index[0]], 0);
    AW_REG_WRITE32(&p_reg->SHIFTCFG[p_devinfo->shifter_index[1]], 0);
    AW_REG_WRITE32(&p_reg->SHIFTCTL[p_devinfo->shifter_index[1]], 0);
    AW_REG_WRITE32(&p_reg->TIMCFG[p_devinfo->timer_index[0]], 0);
    AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[0]], 0);
    AW_REG_WRITE32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], 0);
    AW_REG_WRITE32(&p_reg->TIMCFG[p_devinfo->timer_index[1]], 0);
    AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[1]], 0);
    AW_REG_WRITE32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], 0);

    /* ��� Shifter ��־λ(д 1 ��0) */
    AW_REG_BIT_SET32(&p_reg->SHIFTSTAT, p_devinfo->shifter_index[0]);
    AW_REG_BIT_SET32(&p_reg->SHIFTSTAT, p_devinfo->shifter_index[1]);

    /* ��� Timer ��־λ(д 1 ��0) */
    AW_REG_BIT_SET32(&p_reg->TIMSTAT, p_devinfo->timer_index[0]);
    AW_REG_BIT_SET32(&p_reg->TIMSTAT, p_devinfo->timer_index[1]);

    /* ʧ�� Shifter DMA */
    AW_REG_BIT_CLR32(&p_reg->SHIFTSDEN, p_devinfo->shifter_index[0]);
    AW_REG_BIT_CLR32(&p_reg->SHIFTSDEN, p_devinfo->shifter_index[1]);

    /* ʧ�� Timer �ж� */
    AW_REG_BIT_CLR32(&p_reg->TIMIEN, p_devinfo->timer_index[0]);
    AW_REG_BIT_CLR32(&p_reg->TIMIEN, p_devinfo->timer_index[1]);

    /* ʧ�� Shifter �����ж� */
    AW_REG_BIT_CLR32(&p_reg->SHIFTEIEN, p_devinfo->shifter_index[0]);
    AW_REG_BIT_CLR32(&p_reg->SHIFTEIEN, p_devinfo->shifter_index[1]);

    /* ʧ�� Shifter ״̬�ж� */
    AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
    AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);

    /* ��� Shifter �����־λ(д 1 ��0) */
    AW_REG_BIT_SET32(&p_reg->SHIFTERR, p_devinfo->shifter_index[0]);
    AW_REG_BIT_SET32(&p_reg->SHIFTERR, p_devinfo->shifter_index[1]);

    aw_spinlock_isr_give(&p_chan->dev_lock);
}

/**
 * \brief FlexIO UART Ӳ����ʼ��
 *
 * \param[in] p_chan ָ���豸ͨ����Ϣ�ṹ��ָ��
 */
aw_local void __flexio_uart_hardware_init (
    struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    uint32_t               reg_temp;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    /* ���� CTRL �Ĵ��� */
    reg_temp = AW_REG_READ32(&p_reg->CTRL);
    AW_BIT_SET(reg_temp, 0);
    AW_BIT_MODIFY(reg_temp, 2, p_devinfo->is_fast_access_en);
    AW_BIT_MODIFY(reg_temp, 30, p_devinfo->is_debug_en);
    AW_BIT_MODIFY(reg_temp, 31, !p_devinfo->is_doze_en);
    AW_REG_WRITE32(&p_reg->CTRL, reg_temp);

    /* ��λ��� Shifter �� Timer */
    __flexio_uart_hardware_deinit(p_chan);

    /* ���� UART ������ */
    reg_temp = (0x02 << 0) |   /* ��ʼλΪ�͵�ƽ */
               (0x03 << 4);    /* ֹͣλΪ�ߵ�ƽ */
    AW_REG_WRITE32(&p_reg->SHIFTCFG[p_devinfo->shifter_index[0]], reg_temp);

    reg_temp = (0x02 << 0)                       |   /* ����ģʽ */
               (p_devinfo->tx_pin_index << 8)    |   /* ����ѡ�� */
               (0x03 << 16)                      |   /* ��������Ϊ��� */
               (p_devinfo->timer_index[0] << 24);    /* ѡ��ʱ�� */
    AW_REG_WRITE32(&p_reg->SHIFTCTL[p_devinfo->shifter_index[0]], reg_temp);

    reg_temp = (0x01 << 1) |  /* ��ʱ������ʱ�Զ�������ʼλ */
               (0x02 << 4) |  /* ��ʱ��ֹͣʱ�Զ�����ֹͣλ */
               (0x02 << 8) |  /* ������ƽΪ��ʱ��ʱ������ */
               (0x02 << 12);  /* ��ʱ��ƥ��ʱ�Զ�ֹͣ */
    AW_REG_WRITE32(&p_reg->TIMCFG[p_devinfo->timer_index[0]], reg_temp);

    reg_temp = (0x01 << 0)                                         | /* �����ʷ���ģʽ */
               (p_devinfo->tx_pin_index << 8)                      | /* ����ѡ�� */
               (0x01 << 22)                                        | /* ѡ���ڲ�����Դ */
               (0x01 << 23)                                        | /* �͵�ƽ���� */
               (((p_devinfo->shifter_index[0] << 2) | 0x01) << 24);  /* ����Դѡ�� */
    AW_REG_WRITE32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], reg_temp);

    /* ���ʹ�ܽ��� */
    if (p_chan->options & CREAD) {

        /* ���� UART ������ */
        reg_temp = (0x02 << 0) |  /* ��ʼλΪ�͵�ƽ */
                   (0x03 << 4);   /* ֹͣλΪ�ߵ�ƽ */
        AW_REG_WRITE32(&p_reg->SHIFTCFG[p_devinfo->shifter_index[1]], reg_temp);

        reg_temp = (0x01 << 0)                       |  /* ����ģʽ */
                   (p_devinfo->rx_pin_index << 8)    |  /* ����ѡ�� */
                   (0x01 << 23)                      |  /* ��ʱ���½�����λ */
                   (p_devinfo->timer_index[1] << 24);   /* ѡ��ʱ�� */
        AW_REG_WRITE32(&p_reg->SHIFTCTL[p_devinfo->shifter_index[1]], reg_temp);

        reg_temp = (0x01 << 1)  |  /* ��ʱ������ʱ�Զ�������ʼλ */
                   (0x02 << 4)  |  /* ��ʱ��ֹͣʱ�Զ�����ֹͣλ */
                   (0x04 << 8)  |  /* ����������ʱ��ʱ������ */
                   (0x02 << 12) |  /* ��ʱ��ƥ��ʱ�Զ�ֹͣ */
                   (0x04 << 16) |  /* ����������ʱ��ʱ����λ(�Ͱ�λ) */
                   (0x02 << 24);   /* ��ʱ����λʱ����߼��ߵ�ƽ */
        AW_REG_WRITE32(&p_reg->TIMCFG[p_devinfo->timer_index[1]], reg_temp);

        reg_temp = (0x01 << 0)                            |  /* �����ʷ���ģʽ */
                   (0x01 << 7)                            |  /* ���ŵ͵�ƽ��Ч */
                   (p_devinfo->rx_pin_index << 8)         |  /* ����ѡ�� */
                   ((p_devinfo->rx_pin_index << 1) << 24);   /* ����Դѡ�� */
        AW_REG_WRITE32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], reg_temp);
    }

    /* ���� UART ������ */
    __flexio_uart_baudrate_set(p_chan, p_chan->baudrate);

    /* UART ѡ������ */
    __flexio_uart_opt_set(p_chan, p_chan->options);
}

/**
 * \brief FlexIO UART ����������
 *
 * \param[in] p_chan   ָ���豸ͨ����Ϣ�ṹ��ָ��
 * \param[in] baudtare ������
 *
 * \retval  AW_OK     ���
 * \retval -AW_EINVAL ��֧�ֵĲ�����
 */
aw_local aw_err_t __flexio_uart_baudrate_set (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    uint32_t                              baudtare)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    uint32_t               reg_temp;
    uint32_t               f_clk;
    uint16_t               timer_div;
    aw_err_t               result = AW_OK;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    do {
        f_clk = aw_clk_rate_get(p_devinfo->async_parent_clk_id);

        timer_div = f_clk / baudtare;
        timer_div = timer_div / 2 - 1;

        if (timer_div > 0xFFu) {
            result = -AW_EINVAL;
            break;
        }

        reg_temp = AW_REG_READ32(&p_reg->TIMCMP[p_devinfo->timer_index[0]]) & 0xFF00;
        reg_temp |= timer_div & 0x00FF;

        aw_spinlock_isr_take(&p_chan->dev_lock);
        __flexio_uart_disable(p_devinfo);

        AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[0]], reg_temp);

        /* ���ʹ�ܽ��� */
        if (p_chan->options & CREAD) {
            AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[1]], reg_temp);
        }

        __flexio_uart_enable(p_devinfo);
        aw_spinlock_isr_give(&p_chan->dev_lock);

        p_chan->baudrate = baudtare;
    } while(0);

    return result;
}

/**
 * \brief FlexIO UART ����λ������
 *
 * \param[in] p_chan   ָ���豸ͨ����Ϣ�ṹ��ָ��
 * \param[in] baudtare ����λ��
 *
 * \retval  AW_OK     ���
 * \retval -AW_EINVAL ��֧�ֵ�����λ��
 */
aw_local aw_err_t __flexio_uart_data_bits_set (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    uint32_t                              data_bits)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    uint32_t               reg_temp;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    if ((data_bits < 7) || (data_bits > 9)) {
        return -AW_EINVAL;
    }

    reg_temp = AW_REG_READ32(&p_reg->TIMCMP[p_devinfo->timer_index[0]]) & 0xFF;
    reg_temp |= ((uint32_t)(data_bits * 2 - 1)) << 8u;

    __flexio_uart_disable(p_devinfo);

    AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[0]], reg_temp);

    /* ���ʹ�ܽ��� */
    if (p_chan->options & CREAD) {
        AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[1]], reg_temp);
    }

    __flexio_uart_enable(p_devinfo);

    return AW_OK;
}

/**
 * \brief FlexIO UART ģʽ����
 *
 * ����Ϊ�ж�ģʽ������ѯģʽ
 *
 * \param[in] p_chan   ָ���豸ͨ����Ϣ�ṹ��ָ��
 * \param[in] new_mode SIO_MODE_INT ���� SIO_MODE_POLL
 *
 * \retval  AW_OK     ���
 * \retval -AW_EINVAL ��Ч����
 */
aw_local aw_err_t __flexio_uart_mode_set (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    int                                   new_mode)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    if ((new_mode != SIO_MODE_POLL) && new_mode != SIO_MODE_INT) {
        return -AW_EINVAL;
    }

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (new_mode == SIO_MODE_INT) {

        if (p_chan->options & CREAD) {
            AW_REG_BIT_SET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);
        } else {
            AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);
        }

        if (p_chan->is_tx_int_en) {
            AW_REG_BIT_SET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
        } else {
            AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
        }

    } else {
        AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);
        AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
    }

    p_chan->channel_mode = new_mode;

    aw_spinlock_isr_give(&p_chan->dev_lock);

    return AW_OK;
}

/**
 * \brief FlexIO UART ѡ������
 *
 * ����ͨ��ѡ��ֱ�Ϊ CLOCAL, HUPCL, CREAD, CSIZE, PARENB �� PARODD��
 * �� HUPCL ѡ��ʹ��ʱ��һ�����ӽ������һ�ε��� close() ʱ�رգ�����ÿ�ε��� open()
 * ����򿪡�
 *
 * \param[in] p_chan  ָ���豸ͨ����Ϣ�ṹ��ָ��
 * \param[in] options ѡ��
 *
 * \retval  AW_OK      ���
 * \retval -AW_ENOTSUP ��֧��
 */
aw_local aw_err_t __flexio_uart_opt_set (
    struct awbl_imx10xx_flexio_uart_chan  *p_chan,
    int                                    options)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    aw_err_t               state = AW_OK;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    aw_spinlock_isr_take(&p_chan->dev_lock);

    switch (options & CSIZE) {

    case CS5:
    case CS6:
        state = -AW_ENOTSUP;
        break;

    case CS7:
        __flexio_uart_data_bits_set(p_chan, 7);
        break;

    case CS8:
        __flexio_uart_data_bits_set(p_chan, 8);
        break;

    default:
        break;
    }

    /* ���ʹ�ܽ�����Ϊ�ж�ģʽ */
    if ((options & CREAD) && (SIO_MODE_INT == p_chan->channel_mode)) {

        /* ʹ�ܽ����ж� */
        AW_REG_BIT_SET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);
    } else {

        /* ʧ�ܽ����ж� */
        AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);
    }

    /* ��֧����żУ�� */
    if (options & (PARENB | PARODD)) {
        state = -AW_ENOTSUP;
    }

    /* ��֧�� 2 λֹͣ�ź� */
    if (options & STOPB) {
        state = -AW_ENOTSUP;
    }

    /* ��֧��Ӳ������ */
    if (!(options & CLOCAL)) {
        state = -AW_ENOTSUP;
    }

    p_chan->options = options;

    aw_spinlock_isr_give(&p_chan->dev_lock);

    return state;
}

/** \brief ���� UART, ���� RTS Ϊ��Ч��������͡����ܻ����� */
aw_local aw_err_t __flexio_uart_hup (
    struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    return -AW_ENOTSUP;
}

/** \brief ���� RTS Ϊ��Ч��������͡����ܻ�����  */
aw_local aw_err_t __flexio_uart_open (
    struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    return -AW_ENOTSUP;
}

/**
 * \brief ����ָ��ͨ�����ж�
 *
 * \param[in] p_dev  ָ���豸ʵ��ָ��
 * \param[in] p_chan ָ���豸ͨ����Ϣ�ṹ��ָ��
 */
aw_local void __flexio_uart_chan_int_connect (
    awbl_dev_t                           *p_dev,
    struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);

    aw_assert(p_dev != NULL);
    aw_assert(p_chan != NULL);
    aw_assert(p_devinfo != NULL);

    /* ������ʹ���ж� */
    aw_int_connect(p_devinfo->inum, (void ( *)(void *))__flexio_uart_isr, p_chan);
    aw_int_enable(p_devinfo->inum);
}

/** \brief ��ȡ���ڽ��յ������ݲ����ûص����� */
aw_local void __flexio_uart_int_rd (struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    char                   inchar;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (AW_REG_BIT_ISSET32(&p_reg->SHIFTSTAT, p_devinfo->shifter_index[1])) {
        inchar = (char)readl(&p_reg->SHIFTBUFBYS[p_devinfo->shifter_index[1]]);
        (*p_chan->pfn_rxchar_put)(p_chan->rxput_arg, inchar);
    }

    aw_spinlock_isr_give(&p_chan->dev_lock);
}

/** \brief ��ȡ��Ҫ���͵����ݲ�ͨ�����ڷ��� */
aw_local int __flexio_uart_int_wr (struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    char                   outchar;
    int                    count;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (p_chan->pfn_txchar_get(p_chan->txget_arg, &outchar) == AW_OK) {
        writel(outchar, &p_reg->SHIFTBUF[p_devinfo->shifter_index[0]]);
        count = 1;
    } else {
        count = 0;
    }
    aw_spinlock_isr_give(&p_chan->dev_lock);

    return count;
}

/** \brief FlexIO UART �жϷ����� */
aw_local void __flexio_uart_isr (struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    register uint32_t      sr;
    int                    send_count = 0;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    sr = readl(&p_reg->SHIFTSTAT);

    if (AW_REG_BIT_ISSET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]) &&
        (sr & (1 << p_devinfo->shifter_index[1]))) {
        __flexio_uart_int_rd(p_chan);
    }

    if (AW_REG_BIT_ISSET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]) &&
        (sr & (1 << p_devinfo->shifter_index[0]))) {
        send_count = __flexio_uart_int_wr(p_chan);
        if (0 == send_count) {
            __flexio_uart_tx_interrupt_enable(p_chan, 0);
        }
    }

    aw_spinlock_isr_give(&p_chan->dev_lock);
}

/*******************************************************************************
  SIO ��������
*******************************************************************************/

/**
 * \brief �豸���ƺ���
 *
 * \param[in] p_sio_chan ָ�� aw_sio_chan �ṹ��ָ��
 * \param[in] cmd        ���������ʻ�ȡ/���á�ģʽ���á�Ӳ�������Լ���������
 * \param[in] p_arg      ���ݲ�ͬ������벻ͬ�Ĳ���
 *
 * \note ���������һ�� close() ��������ʱ��SIO_HUP ������ ttydrv ���ͣ�
 *       ͬ����һ�ε��� open() ����ʱ��SIO_OPEN ��� ttydrv ���͡�
 */
aw_local aw_err_t __flexio_uart_ioctl (struct aw_sio_chan *p_sio_chan,
                                       int                 cmd,
                                       void               *p_arg)
{
    __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan);
    aw_err_t status = AW_OK;

    aw_assert(p_sio_chan != NULL);
    aw_assert(p_chan != NULL);

    switch (cmd) {

    case SIO_BAUD_SET:
        status = __flexio_uart_baudrate_set(p_chan, (uint32_t)p_arg);
        break;

    case SIO_BAUD_GET:
        *(int *)p_arg = p_chan->baudrate;
        break;

    case SIO_MODE_SET:
        status = (__flexio_uart_mode_set(p_chan, (int)p_arg) == AW_OK)
                 ? AW_OK : -AW_EIO;
        break;

    case SIO_MODE_GET:
        *(int *)p_arg = p_chan->channel_mode;
        break;

    case SIO_AVAIL_MODES_GET:
        *(int *)p_arg = SIO_MODE_INT | SIO_MODE_POLL;
        break;

    case SIO_HW_OPTS_SET:
        status = (__flexio_uart_opt_set(p_chan, (int)p_arg) == AW_OK) ? AW_OK : AW_EIO;
        break;

    case SIO_HW_OPTS_GET:
        *(int *)p_arg = p_chan->options;
        break;

    case SIO_HUP:
        if (p_chan->options & HUPCL) {
            status = __flexio_uart_hup(p_chan);
        }
        break;

    case SIO_OPEN:
        if (p_chan->options & HUPCL) {
            status = __flexio_uart_open(p_chan);
        }
        break;

    default:
        status = -AW_ENOSYS;
        break;
    }

    return status;
}

/**
 * \brief ���÷����ж�
 *
 * \details �����жϼ�����ַ����ͳ�����������ж�ģʽ����Ӳ�����أ��������ж�
 *          �������Ӳ�����أ����� CTS ����Ϊ TRUE�������� TX �жϡ�
 */
aw_local aw_err_t __flexio_uart_tx_startup (struct aw_sio_chan *p_sio_chan)
{
    __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan);

    aw_assert(p_sio_chan != NULL);
    aw_assert(p_chan != NULL);

    if (p_chan->channel_mode == SIO_MODE_INT) {
         if (__flexio_uart_int_wr(p_chan) > 0 ) {
             __flexio_uart_tx_interrupt_enable(p_chan, 1);
         }
         return AW_OK;
     }

     return -AW_ENOSYS;
}

/**
 * \brief �����ж��е��õĽ��ա������ַ��ص�����
 *
 * \param p_sio_chan     ָ�� aw_sio_chan �ṹ��ָ��
 * \param callback_type  �ص���������(���ͻ��߽���)
 * \param pfn_callback   ָ��ص�������ָ��
 * \param p_callback_arg ����ص������Ĳ���
 *
 * \retval  AW_OK  ���
 * \retval -AW_ENOSYS ��֧�ֵĻص���������
 */
aw_local aw_err_t __flexio_uart_callback_install (
    struct aw_sio_chan *p_sio_chan,
    int                 callback_type,
    aw_err_t          (*pfn_callback)(void *),
    void               *p_callback_arg)
{
    __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan);

    aw_assert(p_sio_chan != NULL);
    aw_assert(p_chan != NULL);

    switch (callback_type) {

    case AW_SIO_CALLBACK_GET_TX_CHAR:
        p_chan->pfn_txchar_get = (pfunc_sio_txchar_get_t)pfn_callback;
        p_chan->txget_arg      = p_callback_arg;
        return AW_OK;

    case AW_SIO_CALLBACK_PUT_RCV_CHAR:
        p_chan->pfn_rxchar_put = (pfunc_sio_rxchar_put_t)pfn_callback;
        p_chan->rxput_arg      = p_callback_arg;
        return AW_OK;

    default:
        return -AW_ENOSYS;
    }
}

/** \brief ������ѯ���� */
aw_local aw_err_t __flexio_uart_poll_input (struct aw_sio_chan *p_sio_chan,
                                            char               *p_inchar)
{
    __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan);
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    aw_err_t               err = AW_OK;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (!AW_REG_BIT_ISSET32(&p_reg->SHIFTSTAT, p_devinfo->shifter_index[1])) {
        err = -AW_EAGAIN;
        goto cleanup;
    }

    *p_inchar = (char)readl(&p_reg->SHIFTBUFBYS[p_devinfo->shifter_index[1]]);

cleanup:
    aw_spinlock_isr_give(&p_chan->dev_lock);
    return err;
}

/** \brief ������ѯ���� */
aw_local aw_err_t __flexio_uart_poll_output (struct aw_sio_chan *p_sio_chan,
                                             char                outchar)
{
    __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan);
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    aw_err_t               err = AW_OK;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (!AW_REG_BIT_ISSET32(&p_reg->SHIFTSTAT, p_devinfo->shifter_index[0])) {
        err =  -AW_EAGAIN;
        goto cleanup;
    }

    writel(outchar, &p_reg->SHIFTBUF[p_devinfo->shifter_index[0]]);

cleanup:
    aw_spinlock_isr_give(&p_chan->dev_lock);
    return err;
}

/*******************************************************************************
  METHOD
*******************************************************************************/

/**
 * \brief METHOD: ��ȡָ�� p_chan �ṹ���ָ��
 *
 * \param[in] p_dev ָ���豸ʵ���ṹ��ָ��
 * \param[in] p_arg ����
 */
void imx10xx_flexio_uart_sio_chan_get (awbl_dev_t *p_dev, void *p_arg)
{
    __DEV_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_dev);
    struct aw_sio_chan_info *p_info = (struct aw_sio_chan_info *)p_arg;

    while (p_chan != NULL ) {

        /* ����������У� channel_no ���� dev unit */
        if (p_chan->channel_no == p_info->chan_no) {
            p_info->p_chan = p_chan;
            return;
        }
        p_chan = p_chan->p_next;
    }
}

/** \brief METHOD: ����ָ��ͨ���� SIO ͨ�� */
void imx10xx_flexio_uart_sio_chan_connect (awbl_dev_t *p_dev, void *p_arg)
{
    __DEV_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_dev);
    struct aw_sio_chan_info *p_info = (struct aw_sio_chan_info *)p_arg;

    if (p_info->chan_no == -1) {
        /* ��������ͨ�� */

        while (NULL != p_chan) {

            /* ���Ӳ�ʹ���ж� */
            __flexio_uart_chan_int_connect(p_dev, p_chan);
            p_chan = p_chan->p_next;
        }

    } else {
        /* ����ָ��ͨ�� */

        if (p_info->p_chan != NULL) {
            return;
        }

        /* ����ͨ�� */
        while (p_chan && (p_chan->channel_no != p_info->chan_no)) {
            p_chan = p_chan->p_next;
        }

        if (p_chan == NULL) {
            return;
        }

        if ((p_info->chan_no != -1)
            && (p_chan->channel_no != p_info->chan_no)) {
            return;
        }

        /* ���Ӳ�ʹ���ж� */
        __flexio_uart_chan_int_connect(p_dev, p_chan);

        p_info->p_chan = p_chan;
    }
}

/** \brief ��һ�׶γ�ʼ�� */
aw_local void __flexio_uart_inst_init1 (struct awbl_dev *p_dev)
{
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    __DEV_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_dev);

    /* ����ƽ̨��ʼ������ */
    if (NULL != p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }

    /* ��ʼ�������� */
    aw_spinlock_isr_init(&p_chan->dev_lock, 0);

    /* ��ʼ���豸ͨ���ṹ */
    p_chan->channel_no = p_dev->p_devhcf->unit;

    p_chan->sio_chan.p_drv_funcs = (struct aw_sio_drv_funcs *)&__g_imx10xx_flexio_uart_sio_drv_funcs;
    p_chan->channel_mode = SIO_MODE_INT; /* �ж�ģʽ */
    p_chan->options      = (CLOCAL |     /* �����豸(û�е��ƽ����) */
                            CREAD  |     /* ʹ�ܽ��� */
                            CS8);        /* ���ݳ���Ϊ 8 λ */

    p_chan->is_tx_int_en = 0;
    p_chan->baudrate = __IMX10xx_CONSOLE_BAUD_RATE;

    p_chan->pfn_txchar_get = (int ( *)(void *, char *))__flexio_uart_dummy_callback;
    p_chan->pfn_rxchar_put = (int ( *)(void *, char ))__flexio_uart_dummy_callback;

    p_chan->p_next = NULL;

    /* ��FlexIO����ΪUART */
    __flexio_uart_hardware_init(p_chan);

}

/** \brief �ڶ��׶γ�ʼ�� */
aw_local void __flexio_uart_inst_init2 (struct awbl_dev *p_dev)
{

}

/** \brief �����׶γ�ʼ�� */
aw_local void __flexio_uart_inst_connect (struct awbl_dev *p_dev)
{

}

/*******************************************************************************
  �ⲿ��������
*******************************************************************************/

/**
 * \brief �� IMX10xx FlexIO UART ����ע�ᵽ AWBus ��ϵͳ��
 *
 * \note ������Ӧ�� aw_prj_early_init() �г�ʼ�����������н�ֹ���ò���ϵͳ��غ���
 */
void awbl_imx10xx_flexio_uart_drv_register (void)
{
    awbl_drv_register(&__g_flexio_uart_drv_registration);
}

/* end of file */
