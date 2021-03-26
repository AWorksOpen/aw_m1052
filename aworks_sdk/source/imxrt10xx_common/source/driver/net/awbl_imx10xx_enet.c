/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite iMX RT10xx ENET 驱动
 *
 * \internal
 * \par modification history:
 * - 1.10 18-04-17, phd, migrate to aw_netif
 * - 1.00 2017-11-07, mex, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "aw_assert.h"
#include "aw_cache.h"
#include "aw_clk.h"
#include "aw_delay.h"
#include "aw_gpio.h"
#include "aw_int.h"
#include "aw_mem.h"
#include "aw_nvram.h"
#include "aw_system.h"
#include "aw_task.h"

#include "awbus_lite.h"
#include "awbl_plb.h"

#include "aw_net.h"

#include "driver/net/awbl_imx10xx_enet.h"

#include <string.h>
#include <stdlib.h>


/*******************************************************************************
  local defines
*******************************************************************************/

/** \brief the MTU of ENET */
#define __ENET_MTU     1500

/** \brief 由awbl设备实例获取enet设备信息实例 */
#define __ENET_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_enet_dev *p_this = \
        (struct awbl_imx10xx_enet_dev *) (p_dev)

/** \brief 由awbl设备实例获取enet设备信息实例 */
#define __ENET_DEVINFO_DECL(p_info, p_dev) \
    struct awbl_imx10xx_enet_devinfo *p_info = \
        (struct awbl_imx10xx_enet_devinfo *) AWBL_DEVINFO_GET(p_dev)

/** \brief n除以d结果向上取整 */
#define DIV_ROUND_UP(n,d)       (((n) + (d) - 1) / (d))

/** \brief 将x按y的整数倍向上取整 */
#define roundup(x, y)           ((((x) + ((y) - 1)) / (y)) * (y))

/*******************************************************************************
  functions
*******************************************************************************/

/**
 * ENET硬件函数大部分都以fec开头，主要实现一些硬件特定功能，用于在驱动函数中调用
 */

/**
 * \brief 设置enet的硬件mac地址
 *
 * \param   fec  enet驱动设备对象
 * \param   p    指向一个6字节的MAC地址，如果为NULL，则使用默认MAC地址
 * \returns
 */
aw_local void fec_set_mac_address (awbl_imx10xx_enet_dev_t *fec, uint8_t *p_hwaddr)
{
    writel(0, &fec->eth->iaddr1);
    writel(0, &fec->eth->iaddr2);
    writel(0, &fec->eth->gaddr1);
    writel(0, &fec->eth->gaddr2);
    /*
     * Set physical address
     */
    writel((p_hwaddr[0] << 24) + (p_hwaddr[1] << 16) + (p_hwaddr[2] << 8) + p_hwaddr[3], &fec->eth->paddr1);
    writel((p_hwaddr[4] << 24) + (p_hwaddr[5] << 16) + 0x8808, &fec->eth->paddr2);
}

/**
 * \brief 设置enet的硬件mdio的速度（目标位2.5MHz上下）
 *
 * \param   eth    enet寄存器基地址
 * \returns
 */
aw_local void fec_mii_setspeed (awbl_imx10xx_enet_dev_t  *efc)
{
    __ENET_DEV_DECL(p_this, efc);
    __ENET_DEVINFO_DECL(p_info, efc);

    /*
     * Set MII_SPEED = (1/(mii_speed * 2)) * System Clock
     * and do not drop the Preamble.
     */
    uint32_t speed;
    uint32_t holdtime;
    uint32_t ipg_clk_freq;

    ipg_clk_freq = aw_clk_rate_get(p_info->clk_id);

    speed = DIV_ROUND_UP(ipg_clk_freq, 5000000);
    holdtime = DIV_ROUND_UP(ipg_clk_freq, 100000000) - 1;
    speed--;

    speed <<= 1;
    holdtime <<= 8;
    writel(speed | holdtime, &p_this->eth->mii_speed);
}

/**
 * \brief 分配enet驱动中所要使用的发送接收描述符和缓冲区
 *
 * \param   fec   enet驱动设备对象
 * \param
 * \returns AW_OK表明操作成功，否则返回负的错误码
 */
aw_local int fec_alloc_descs (awbl_imx10xx_enet_dev_t *fec)
{
    __ENET_DEVINFO_DECL(p_info, fec);

    unsigned int size;
    int i;
    uint8_t *data;
    int ret = -AW_ENOMEM;

    /* Allocate TX descriptors. */
    size = roundup(p_info->fec_tbd_num * sizeof(struct fec_bd), FEC_DMA_ALIGN);
    fec->tbd_base = aw_cache_dma_align(size, FEC_DMA_ALIGN);
    if (NULL == fec->tbd_base) {
        AW_ERRF(("err aw_cache_dma_align tx" ENDL));
        goto cleanup;
    }
    memset(fec->tbd_base, 0, size);

    /* Allocate RX descriptors. */
    size = roundup(p_info->fec_rbd_num * sizeof(struct fec_bd), FEC_DMA_ALIGN);
    fec->rbd_base = aw_cache_dma_align(size, FEC_DMA_ALIGN);
    if (NULL == fec->rbd_base) {
        AW_ERRF(("err aw_cache_dma_align rx" ENDL));
        goto cleanup;
    }

    memset(fec->rbd_base, 0, size);

    /* Allocate RX buffers. */
    /* Maximum RX buffer size. */
    size = roundup(FEC_MAX_PKT_SIZE, FEC_DMA_ALIGN);
    for (i = 0; i < p_info->fec_rbd_num; i++) {
        data = aw_mem_align(size, FEC_DMA_ALIGN);
        if (NULL == data) {
            AW_ERRF(("err aw_cache_dma_align RX" ENDL));
            goto cleanup;
        }

        fec->rbd_base[i].data_pointer = (uint32_t) data;
        fec->rbd_base[i].status = FEC_RBD_EMPTY;
        fec->rbd_base[i].data_length = 0;
    }

    /* Allocate TX buffers. */
    /* Maximum TX buffer size. */
    size = roundup(FEC_MAX_PKT_SIZE, FEC_DMA_ALIGN);
    for (i = 0; i < p_info->fec_tbd_num; i++) {
        data = aw_mem_align(size, FEC_DMA_ALIGN);
        if (NULL == data) {
            AW_ERRF(("err aw_cache_dma_align TX" ENDL));
            goto cleanup;
        }

        fec->tbd_base[i].data_pointer = (uint32_t) data;
        fec->tbd_base[i].status = 0;
        fec->tbd_base[i].data_length = 0;
    }

    /* Mark the last TBD to close the ring. */
    fec->tbd_base[i - 1].status = FEC_TBD_WRAP;

    fec->rbd_index = 0;
    fec->tbd_head = fec->tbd_tail = 0;
    ret = 0;
cleanup:

    return ret;
}

/**
 * \brief 复位enet硬件
 *
 * \param   fec  enet驱动设备对象
 * \returns
 */
aw_local void fec_reset (awbl_imx10xx_enet_dev_t *fec)
{
     /* Whack a reset.  We should wait for this.
     * For i.MX6SX SOC, enet use AXI bus, we use disable MAC
     * instead of reset MAC itself.
     */
    /* Reset chip. */
    writel(readl(&fec->eth->ecntrl) | FEC_ECNTRL_RESET, &fec->eth->ecntrl);

    while (readl(&fec->eth->ecntrl) & FEC_ECNTRL_RESET) {
        aw_udelay(10);
    }
}

/**
 * \brief 将enet驱动中用到的发送描述符设置为初始状态
 *
 * 即将所有发送描述符设置为无数据的状态
 * \param   fec  enet驱动设备对象
 * \returns
 */
aw_local void fec_tbd_init (awbl_imx10xx_enet_dev_t *fec)
{
    __ENET_DEVINFO_DECL(p_info, fec);
    int i;

    for (i = 0; i < p_info->fec_tbd_num; i++) {
        fec->tbd_base[i].status = 0;
        fec->tbd_base[i].data_length = 0;
    }

    fec->tbd_base[0].status = 0;
    fec->tbd_base[i - 1].status = FEC_TBD_WRAP;
    fec->tbd_head = fec->tbd_tail = 0;
}

/**
 * \brief 将enet驱动中用到的接收描述符设置为初始状态
 *
 * 即将所有接收描述符设置为无数据的状态
 * \param   fec   enet驱动设备对象
 * \returns
 */
aw_local void fec_rbd_init (awbl_imx10xx_enet_dev_t *fec, int count, int dsize)
{
    int i = 0;
    /*
     * Reload the RX descriptors with default values and wipe
     * the RX buffers.
     */
    for (i = 0; i < count; i++) {
        fec->rbd_base[i].status = FEC_RBD_EMPTY;
        fec->rbd_base[i].data_length = 0;
    }

    /* Mark the last RBD to close the ring. */
    fec->rbd_base[i - 1].status = FEC_RBD_WRAP | FEC_RBD_EMPTY;
    fec->rbd_index = 0;
}

/**
 * \brief 启动enet硬件的接收任务
 *
 * \param   fec  enet驱动设备对象
 * \returns
 */
aw_local void fec_rx_task_enable (awbl_imx10xx_enet_dev_t *fec)
{
    writel(FEC_R_DES_ACTIVE_RDAR, &fec->eth->r_des_active);
}

/**
 * \brief 启动enet硬件的发送任务
 *
 * \param   fec  enet驱动设备对象
 * \returns
 */
aw_local int fec_tx_task_enable (awbl_imx10xx_enet_dev_t *fec)
{
    writel(FEC_X_DES_ACTIVE_TDAR, &fec->eth->x_des_active);
    return 0;
}

/**
 * \brief 使能enet硬件产生驱动所需要的中断
 *
 * \param   fec  enet驱动设备对象
 * \returns
 */
aw_local void fec_enable_interrupt (awbl_imx10xx_enet_dev_t *fec)
{
    struct ethernet_regs *eth = fec->eth;
    uint32_t val = 0;

    val = FEC_IMASKT_RXF | FEC_IMASK_MII;

    writel(val, &eth->imask);
}

/**
 * \brief 禁能enet硬件产生驱动所需要的中断
 *
 * \param   fec  enet驱动设备对象
 * \returns
 */
aw_local void fec_disable_interrupt (awbl_imx10xx_enet_dev_t *fec)
{
    struct ethernet_regs *eth = fec->eth;
    writel(0, &eth->imask);
}

aw_local uint32_t fec_multicast_group_crc (uint8_t *address)
{
    uint32_t crc = 0xFFFFFFFFU;
    uint32_t count1 = 0;
    uint32_t count2 = 0;

    /* Calculates the CRC-32 polynomial on the multicast group address. */
    for (count1 = 0; count1 < 6; count1++) {
        uint8_t c = address[count1];
        for (count2 = 0; count2 < 0x08U; count2++) {
            if ((c ^ crc) & 1U) {
                crc >>= 1U;
                c >>= 1U;
                crc ^= 0xEDB88320U;
            } else {
                crc >>= 1U;
                c >>= 1U;
            }
        }
    }

    return crc;
}

aw_local void fec_join_multicast_group (awbl_imx10xx_enet_dev_t *fec, uint8_t *address)
{
    uint32_t crc = fec_multicast_group_crc(address);

    /* Enable a multicast group address. */
    if (!((crc >> 0x1FU) & 1U)) {
        writel(readl(&fec->eth->gaddr2) | 1U << ((crc >> 0x1AU) & 0x1FU), &fec->eth->gaddr2);
    } else {
        writel(readl(&fec->eth->gaddr1) | 1U << ((crc >> 0x1AU) & 0x1FU), &fec->eth->gaddr1);
    }
}

aw_local void fec_leave_multicast_group (awbl_imx10xx_enet_dev_t *fec, uint8_t *address)
{
    uint32_t crc = fec_multicast_group_crc(address);

    /* Set the hash table. */
    if (!((crc >> 0x1FU) & 1U)) {
        writel(readl(&fec->eth->gaddr2) & ~(1U << ((crc >> 0x1AU) & 0x1FU)), &fec->eth->gaddr2);
    } else {
        writel(readl(&fec->eth->gaddr1) & ~(1U << ((crc >> 0x1AU) & 0x1FU)), &fec->eth->gaddr1);
    }
}

/**
 * \brief 根据驱动中的配置初始化enet的寄存器
 *
 * 此函数完成后，enet硬件已经开始接收任务并产生需要的中断了
 * \param   fec  enet驱动设备对象
 * \returns
 */
aw_local void fec_reg_setup (awbl_imx10xx_enet_dev_t *fec)
{
    uint32_t ecr = 0;
    uint32_t rcr = 0;
    uint32_t tcr = 0;

    /* Set interrupt mask register */
    writel(0x00000000, &fec->eth->imask);
    /*  Clear FEC-Lite interrupt event register(IEVENT) */
    writel(0xffffffff, &fec->eth->ievent);
    /* Set Opcode/Pause Duration Register */
    writel(0x1FFF0, &fec->eth->op_pause);
    writel(0x2, &fec->eth->x_wmrk);
    /* Set multicast address filter */
    writel(0x00000000, &fec->eth->gaddr1);
    writel(0x00000000, &fec->eth->gaddr2);
    /* size and address of each buffer */
    writel(FEC_MAX_PKT_SIZE, &fec->eth->emrbr);
    writel((uint32_t) fec->tbd_base, &fec->eth->etdsr);
    writel((uint32_t) fec->rbd_base, &fec->eth->erdsr);

    /* Enable ENET store and forward mode */
    writel(readl(&fec->eth->x_wmrk) | FEC_X_WMRK_STRFWD, &fec->eth->x_wmrk);

    fec_mii_setspeed(fec);

    /*
     * Set FEC-Lite receive control register(R_CNTRL):
     */
    /* Start with frame length = 1518, common for all modes. */
    rcr = FEC_RECV_PKT_SIZE << FEC_RCNTRL_MAX_FL_SHIFT;

    /* the phy interface */
    rcr |= (FEC_RCNTRL_MII_MODE | FEC_RCNTRL_FCE | FEC_RCNTRL_PROM);
    if (fec->phy_interface == PHY_INTERFACE_MODE_RMII) {
        rcr |= FEC_RCNTRL_RMII;
    } else {
        rcr &= ~FEC_RCNTRL_RMII;
    }

    /* set duplex */
    if (fec->phy_mode & AWBL_IFM_HDX) {
        /* Half duplex */
        rcr |= FEC_RCNTRL_DRT;
    } else {
        rcr &= (~FEC_RCNTRL_DRT);
        tcr |= (FEC_TCNTRL_FDEN);
    }

    if (AWBL_IFM_SUBTYPE(fec->phy_mode) == AWBL_IFM_10_T) {
        rcr |= FEC_RCNTRL_RMII_10T;
    }

    /* Enable ENET HW endian SWAP */
    ecr |= FEC_ECNTRL_DBSWAP;

    /* Enable FEC-Lite controller */
    ecr |= FEC_ECNTRL_ETHER_EN;

    writel((2 << 30) | (8 << 20) | (78), &fec->eth->rxic);
    writel(0, &fec->eth->tacc);
    writel(0, &fec->eth->racc);

    fec_set_mac_address(fec, fec->ethif.hwaddr);
    writel(0, &fec->eth->mib_control);
    writel(tcr, &fec->eth->x_cntrl);
    writel(rcr, &fec->eth->r_cntrl);
    writel(ecr, &fec->eth->ecntrl);

    /* Enable SmartDMA receive task */
    fec_rx_task_enable(fec);
    fec_enable_interrupt(fec);
}

/**
 * \brief 清空一个接收描述符中的数据
 *
 * \param   last   是否为接收描述符组中的最后一个描述符
 * \param   pRbd   指向要清空的描述符
 * \returns
 */
aw_local void fec_rbd_clean (int last, struct fec_bd *p_rbd)
{
    unsigned short flags = FEC_RBD_EMPTY;

    if (last) {
        flags |= FEC_RBD_WRAP;
    }
    writew(flags, &p_rbd->status);
    writew(0, &p_rbd->data_length);
}

/**
 * \brief 根据驱动中的配置将enet硬件启动
 *
 * 此函数完成后，enet硬件已经开始接收任务并产生需要的中断了
 * \param   fec   enet驱动设备对象
 * \returns
 */
aw_local void fec_start (awbl_imx10xx_enet_dev_t *fec)
{
    __ENET_DEVINFO_DECL(p_info, fec);

    /* Setup transmit descriptors, there are two in total. */
    fec_tbd_init(fec);

    /* Setup receive descriptors */
    fec_rbd_init(fec, p_info->fec_rbd_num, FEC_MAX_PKT_SIZE);

    fec_reg_setup(fec);
}

/**
 * \brief 将enet硬件停止
 *
 * \param   fec  enet驱动设备对象
 * \returns
 */
aw_local void fec_stop (awbl_imx10xx_enet_dev_t *fec)
{
    uint32_t ecr = 0;
    ecr = readl(&fec->eth->ecntrl);
    ecr &= FEC_ECNTRL_ETHER_EN;
    writel(ecr, &fec->eth->ecntrl);
}

/**
 * \brief 在驱动初始化阶段完成enet的硬件相关的初始化工作
 *
 * \param   fec  enet驱动设备对象
 * \returns
 */
aw_local void fec_enet_init (awbl_imx10xx_enet_dev_t *fec)
{
    fec_alloc_descs(fec);
    fec_reset(fec);
}

/**
 * \brief 在驱动的运行过程中根据需要完成enet硬件的重启
 *
 * \param   fec   enet驱动设备对象
 * \returns
 */
aw_local void fec_restart (awbl_imx10xx_enet_dev_t *fec)
{
    /* first, wo stop the interrupt */
    aw_int_disable(fec->inum);

    /* get all the lock */
    AW_MUTEX_LOCK(fec->tx_lock, AW_WAIT_FOREVER);
    AW_MUTEX_LOCK(fec->rx_lock, AW_WAIT_FOREVER);

    fec_stop(fec);
    fec_reset(fec);
    fec_start(fec);

    AW_MUTEX_UNLOCK(fec->rx_lock);
    AW_MUTEX_UNLOCK(fec->tx_lock);

    aw_int_enable(fec->inum);
}

/**
 * \brief 改变enet模式
 *
 * 在驱动的运行过程中因为PHY模式的改变导致ENET也必须要改变模式
 * \param   fec         enet驱动设备对象
 * \param   old_mode    旧的模式
 * \returns
 */
aw_local void fec_mode_change (awbl_imx10xx_enet_dev_t *fec, uint32_t old_mode)
{
    /* 检查双工模式是否改变，如果改变了，则enet需要复位才可以使用 */
    if ((fec->phy_mode & AWBL_IFM_FDX) != (old_mode & AWBL_IFM_FDX)) {
        /* 重启ENET */
        fec_restart(fec);
    } else {
        uint32_t rcntl;
        struct ethernet_regs *eth;

        eth = fec->eth;
        rcntl = readl(&eth->r_cntrl);

        if ( AWBL_IFM_SUBTYPE(fec->phy_mode) == AWBL_IFM_100_TX) {
            rcntl &= ~(FEC_RCNTRL_RMII_10T);
        } else {
            rcntl |= (FEC_RCNTRL_RMII_10T);
        }
        writel(rcntl, &eth->r_cntrl);
    }
}


/** \brief MII操作超时时间，以ms计算 */
#define __FEC_MII_TIMEOUT_MS        50

/**
 * \brief 通过enet读取PHY的寄存器
 *
 * \param   p_dev       enet驱动设备对象
 * \param   phy_addr    PHY的地址
 * \param   phy_reg     要读取的寄存器编号
 * \param   p_reg_val   用于返回寄存器的值
 * \returns AW_OK表明操作成功，否则返回负的错误码
 */
aw_local aw_err_t enet_mii_phy_read (struct awbl_mii_master  *p_master,
                                     int                      phy_addr,
                                     int                      phy_reg,
                                     uint16_t                *p_reg_val)
{
    aw_err_t err = AW_OK;
    struct ethernet_regs *eth = NULL;
    uint32_t val = 0;
    uint32_t reg = 0; /* convenient holder for the PHY register */
    uint32_t phy = 0; /* convenient holder for the PHY */

    awbl_imx10xx_enet_dev_t *p_dev = AW_CONTAINER_OF(p_master,
                                                     struct awbl_imx10xx_enet_dev,
                                                     mii_master);

    eth = p_dev->eth;

    AW_SEMB_TAKE(p_dev->mii_sem, AW_SEM_NO_WAIT);

    /* start a read op */
    reg = phy_reg << FEC_MII_DATA_RA_SHIFT;
    phy = phy_addr << FEC_MII_DATA_PA_SHIFT;

    writel(FEC_MII_DATA_ST | FEC_MII_DATA_OP_RD | FEC_MII_DATA_TA | phy | reg,
           &eth->mii_data);

    err = AW_SEMB_TAKE(p_dev->mii_sem, __FEC_MII_TIMEOUT_MS);

    val = readl(&eth->mii_data);
    *p_reg_val = (uint16_t) val;

    return err;
}

/**
 * \brief 通过enet设置PHY的寄存器
 *
 * \param   p_dev       enet驱动设备对象
 * \param   phy_addr    PHY的地址
 * \param   phy_reg     要写入的寄存器编号
 * \param   reg_val     要写入的值
 * \returns AW_OK表明操作成功，否则返回负的错误码
 */
aw_local aw_err_t enet_mii_phy_write (struct awbl_mii_master  *p_master,
                                      int                      phy_addr,
                                      int                      phy_reg,
                                      uint16_t                 reg_val)
{
    aw_err_t err = AW_OK;
    struct ethernet_regs *eth = NULL;
    uint32_t reg = 0; /* convenient holder for the PHY register */
    uint32_t phy = 0; /* convenient holder for the PHY */

    awbl_imx10xx_enet_dev_t *p_dev = AW_CONTAINER_OF(p_master,
                                                     struct awbl_imx10xx_enet_dev,
                                                     mii_master);

    eth = p_dev->eth;

    AW_SEMB_TAKE(p_dev->mii_sem, AW_SEM_NO_WAIT);

    /* start a write op */
    reg = phy_reg << FEC_MII_DATA_RA_SHIFT;
    phy = phy_addr << FEC_MII_DATA_PA_SHIFT;
    writel(FEC_MII_DATA_ST | FEC_MII_DATA_OP_WR | FEC_MII_DATA_TA | phy | reg | reg_val, &eth->mii_data);

    err = AW_SEMB_TAKE(p_dev->mii_sem, __FEC_MII_TIMEOUT_MS);

    return err;
}

/**
 * \brief 根据PHY连接状态的改变更新enet驱动的状态
 *
 * 当连接状态改变时MII总线通过这个函数通知enet驱动
 * \param   p_dev       enet驱动设备对象
 * \param   phy_mode    phy模式
 * \param   phy_stat    phy状态
 * \returns AW_OK表明操作成功，否则返回负的错误码
 */
aw_local aw_err_t enet_mii_update_link (void                    *p_dev,
                                        uint32_t                 phy_mode,
                                        uint32_t                 phy_stat)
{
    __ENET_DEV_DECL(p_this, p_dev);

    uint32_t old_stat = 0;
    uint32_t old_mode = 0;

    if (p_this->emac_init_flg == 0) {
        return AW_ERROR;
    }

    old_stat = p_this->phy_stat;
    old_mode = p_this->phy_mode;

    p_this->phy_stat = phy_stat;
    p_this->phy_mode = phy_mode;

    /* If status went from down to up, announce link up. */
    if ((!(old_stat & AWBL_IFM_ACTIVE)) && (p_this->phy_stat & AWBL_IFM_ACTIVE)) {
        aw_netif_set_link_up(&p_this->ethif);
        AW_INFOF(("enet : set link up." ENDL));
        fec_mode_change(p_this, old_mode);
    } else if ((old_stat & AWBL_IFM_ACTIVE) && (!(p_this->phy_stat & AWBL_IFM_ACTIVE))) {
        /* If status went from up to down, announce link down. */
        aw_netif_set_link_down(&p_this->ethif);
        p_this->phy_mode = old_mode;
        AW_INFOF(("enet : set link down." ENDL));
    } else if (p_this->phy_mode != old_mode) {
        fec_mode_change(p_this, old_stat);
    }

    return AW_OK;
}


/** @} grp_awbl_drv_imx10xx_enet_drv_mii_func */

/**
 * \ 以太网适配器接口函数
 * 以太网适配器接口函数大部分都以enet_ethif开头，供以太网接口适配层（核心LWIP）调用
 * 以发送和接收以太网包
 * @{
 */
/** \brief 由ethif地址addr，找到对应的enet驱动对象的地址 */
#define __EMAC_EIF_TO_DEV(c) \
    AW_CONTAINER_OF(c, awbl_imx10xx_enet_dev_t, ethif)

#define __ETHIF_TO_DEV(s,i_f,addr)  (s *)(((char *)(addr))-AW_OFFSET(s,i_f))

/**
 * \brief 以太网适配层初始化连接
 *
 * \param   p_ethif     以太网接口对象
 * \returns AW_OK表明操作成功，否则返回负的错误码
 */
aw_local aw_err_t enet_link_init (aw_netif_t *p_ethif)
{
    awbl_imx10xx_enet_dev_t *fec = __ETHIF_TO_DEV(awbl_imx10xx_enet_dev_t, ethif, p_ethif);
    fec_set_mac_address(fec, p_ethif->hwaddr);
    return AW_OK;
}

aw_local aw_err_t enet_up (aw_netif_t *p_ethif)
{
    return AW_OK;
}

aw_local aw_err_t enet_down (aw_netif_t *p_ethif)
{
    return AW_OK;
}

/**
 * \brief 将数据包发送出去
 *
 * \param   p_ethif     以太网接口对象
 * \param   p_buf       要发送的数据包
 * \returns AW_OK表明操作成功，否则返回负的错误码
 */
aw_local aw_err_t enet_pkt_output (aw_netif_t *p_ethif, aw_net_buf_t *p_buf)
{
    awbl_imx10xx_enet_dev_t *fec = __ETHIF_TO_DEV(awbl_imx10xx_enet_dev_t, ethif, p_ethif);
    __ENET_DEVINFO_DECL(p_info, fec);

    int len;
    uint16_t status;
    uint8_t *data;

    if (FEC_MAX_PKT_SIZE < aw_net_buf_chain_len(p_buf)) {
        AW_ERRF(("enet : drop big frame." ENDL));
        return -AW_EMSGSIZE;
    }

    AW_MUTEX_LOCK(fec->tx_lock, AW_WAIT_FOREVER);
recheck:

    /* 检查发送队列，回收已经发送完成的描述符 */
    if (fec->tbd_head != fec->tbd_tail) {
        status = readw(&fec->tbd_base[fec->tbd_tail].status);
        while (!(FEC_TBD_READY & status)) {
            fec->tbd_tail = (fec->tbd_tail + 1) % p_info->fec_tbd_num;
            status = readw(&fec->tbd_base[fec->tbd_tail].status);
            if (fec->tbd_tail == fec->tbd_head) {
                break;
            }
        }
    }

    if ((fec->tbd_head + 1) % p_info->fec_tbd_num == fec->tbd_tail) {
        aw_task_delay(1);
        goto recheck;
    }

    data = (uint8_t *) fec->tbd_base[fec->tbd_head].data_pointer;
    len = aw_net_buf_copy_partial(p_buf, data, -1, 0);
    aw_cache_flush(data, len);
    writew(len, &fec->tbd_base[fec->tbd_head].data_length);

    /*
     * update BD's status now
     * This block:
     * - is always the last in a chain (means no chain)
     * - should transmitt the CRC
     * - might be the last BD in the list, so the address counter should
     *   wrap (-> keep the WRAP flag)
     */
    status = readw(&fec->tbd_base[fec->tbd_head].status) & FEC_TBD_WRAP;
    status |= FEC_TBD_LAST | FEC_TBD_TC | FEC_TBD_READY;
    writew(status, &fec->tbd_base[fec->tbd_head].status);

    readw((uint32_t *) (&fec->tbd_base[p_info->fec_tbd_num - 1].status));

    /*
     * Enable SmartDMA transmit task
     */
    fec_tx_task_enable(fec);

    fec->tbd_head = (fec->tbd_head + 1) % p_info->fec_tbd_num;

    AW_MUTEX_UNLOCK(fec->tx_lock);

    return AW_OK;
}


/**
 * \brief 处理数据包的接收
 *
 * 当接收中断来临时，以太网适配层调用此函数来处理接收
 * \param   p_ethif     以太网接口对象
 * \returns AW_OK表明操作成功，否则返回负的错误码
 */
aw_local void enet_ethif_pkt_rx_hdl (awbl_imx10xx_enet_dev_t *fec)
{
    __ENET_DEVINFO_DECL(p_info, fec);

    int len = 0;
    struct fec_bd *rbd;
    uint16_t bd_status;
    int frame_length;
    uint8_t *frame;
    int i;
    uint32_t events;
    struct ethernet_regs *eth = fec->eth;

    events = AW_REG_READ32(&eth->ievent);
    AW_REG_WRITE32(&eth->ievent, events);

    if (events & FEC_IEVENT_RXF) {
        AW_MUTEX_LOCK(fec->rx_lock, AW_WAIT_FOREVER);
        rbd = &fec->rbd_base[fec->rbd_index];
        bd_status = readw(&rbd->status);

        while (!(bd_status & FEC_RBD_EMPTY)) {
            if ((bd_status & FEC_RBD_LAST) && !(bd_status & FEC_RBD_ERR)
                    && ((readw(&rbd->data_length) - 4) > 14)) {
                /*
                 * Get buffer address and size
                 */
                frame = (uint8_t *) readl(&rbd->data_pointer);
                frame_length = readw(&rbd->data_length) - 4;
                len = frame_length;
                /*
                 *  Fill the buffer and pass it to upper layers
                 */
                aw_cache_invalidate(frame, len);
                if (AW_OK != aw_netif_input(&fec->ethif, frame, len)) {
                    AW_INFOF(("aw_netif_input: IP input err" ENDL));
                }
            } else {
                  ;
//                if (bd_status & FEC_RBD_ERR)
//                    aw_kprintf("error frame: 0x%08lx 0x%08x\n",
//                            (uint64_t) rbd->data_pointer, bd_status);
            }

            /*
             * Free the current buffer, restart the engine and move forward
             * to the next buffer. Here we check if the whole cacheline of
             * descriptors was already processed and if so, we mark it free
             * as whole.
             */

            i = fec->rbd_index;
            fec_rbd_clean(i == (p_info->fec_rbd_num - 1), &fec->rbd_base[i]);

            fec_rx_task_enable(fec);
            fec->rbd_index = (fec->rbd_index + 1) % p_info->fec_rbd_num;
            rbd = &fec->rbd_base[fec->rbd_index];
            bd_status = readw(&rbd->status);
        }

        AW_MUTEX_UNLOCK(fec->rx_lock);
    }
    if (events & FEC_IEVENT_MII) {
        AW_SEMB_GIVE(fec->mii_sem);
    }
    fec_enable_interrupt(fec);
}

aw_local aw_err_t enet_nic_ioctl (aw_netif_t *p_netif,
                                  int         ops,
                                  int         name,
                                  void       *p_arg)
{
    aw_err_t ret = AW_OK;

    if (NULL == p_netif) {
        return -AW_ENODEV;
    }
    /**
     * todo: 增加私有处理
     */
    //switch (ops) {
    //case AW_NETIF_OPT_SET:
    //    switch (name) {
    //
    //    case AW_NETIF_OPT_MCAST_JOIN: {
    //        awbl_imx10xx_enet_dev_t *fec;
    //        fec = __ETHIF_TO_DEV(awbl_imx10xx_enet_dev_t, ethif, p_netif);
    //        fec_join_multicast_group(fec, (uint8_t *) p_arg);
    //        ret = AW_OK;
    //    }
    //        break;
    //
    //    case AW_NETIF_OPT_MCAST_LEAVE: {
    //        awbl_imx10xx_enet_dev_t *fec;
    //        fec = __ETHIF_TO_DEV(awbl_imx10xx_enet_dev_t, ethif, p_netif);
    //        fec_leave_multicast_group(fec, (uint8_t *) p_arg);
    //        ret = AW_OK;
    //    }
    //        break;
    //
    //    default :
    //        break;
    //    }
    //    break;
    //default :
    //    break;
    //}

    return ret;
}

aw_local void input_thread (void *p_arg)
{
    __ENET_DEV_DECL(p_this, p_arg);
    AW_FOREVER {
        AW_SEMB_TAKE(p_this->rx_semb, AW_SEM_WAIT_FOREVER);
        enet_ethif_pkt_rx_hdl(p_this);
    }
}

/** \brief 导出以太网适配层操作函数 */
aw_local const aw_netif_ops_t __g_ethif_ops = {
    enet_link_init,
    enet_up,
    enet_down,
    enet_pkt_output,
    enet_nic_ioctl,
    NULL
};

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __enet_inst_init1 (awbl_dev_t *p_dev)
{
    __ENET_DEV_DECL(p_this, p_dev);
    p_this->emac_init_flg = 0; /* not init */
}

aw_local aw_err_t  iterate_phy_dev_attach(void *p_dev);

/**
 * \brief second level initialization routine
 *
 * 主要创建了mii bus,初始化了硬件设设备对象成员
 * \param[in] p_dev     device instance
 */
aw_local void __enet_inst_init2 (awbl_dev_t *p_dev)
{
    __ENET_DEVINFO_DECL(p_info, p_dev);
    __ENET_DEV_DECL(p_this, p_dev);

    static const awbl_mii_master_ops_t mii_ops = {
            enet_mii_phy_write,
            enet_mii_phy_read,
    };

    /* platform initialization */
    if (p_info->pfn_plfm_init) {
        p_info->pfn_plfm_init();
    }

    p_this->eth = (struct ethernet_regs *) p_info->emac_regbase;
    p_this->phy_interface = p_info->interface_mode;
    p_this->inum = p_info->inum;
    p_this->rbd_base = NULL;
    p_this->rbd_index = 0;
    p_this->tbd_base = NULL;

    p_this->phy_mode = AWBL_IFM_ETHER | AWBL_IFM_FDX | AWBL_IFM_100_TX;
    p_this->phy_stat = AWBL_IFM_AVALID;

    AW_MUTEX_INIT(p_this->tx_lock, AW_SEM_Q_PRIORITY);
    AW_MUTEX_INIT(p_this->rx_lock, AW_SEM_Q_PRIORITY);
    AW_SEMB_INIT(p_this->mii_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    fec_enet_init(p_this);

    /* create MII bus */
    memset(&p_this->mii_master, 0, sizeof(p_this->mii_master));
    awbl_miibus_create(p_dev, &p_this->mii_master, p_info->create_mii_bus_id, &mii_ops);
    iterate_phy_dev_attach(p_dev);
}

/**
 * \brief the isr of enet
 *
 * enet的中断处理
 * \param[in] p_arg     device instance
 */
aw_local void __enet_isr (void *p_arg)
{
    __ENET_DEV_DECL(p_this, p_arg);

    AW_SEMB_GIVE(p_this->rx_semb);
    fec_disable_interrupt(p_this);
}


aw_local aw_err_t enet_fec_phy_attach (struct awbl_dev *p_phy_dev, void  *p_arg)
{

    __ENET_DEVINFO_DECL(p_info, p_arg);

    struct awbl_phy_device *p_phy = (struct awbl_phy_device *)p_phy_dev;

    if (AWBL_DEVHCF_GET(p_phy_dev)->bus_type != AWBL_BUSID_MII) {
        return -AW_EINVAL;
    }

    if (!strcmp( p_phy->p_ethif_name, p_info->p_nic_name)) {
        p_phy->phy_attach_dev.pfn_update_link = enet_mii_update_link;
        p_phy->phy_attach_dev.p_enet_dev = p_arg;

        return AW_OK;
    }

    return -AW_EINVAL;
}


aw_local aw_err_t  iterate_phy_dev_attach(void *p_dev)
{
   awbl_dev_iterate(enet_fec_phy_attach,
                    p_dev,
                    AWBL_ITERATE_INSTANCES);
   return AW_OK;
}



/**
 * \brief third level initialization routine
 *
 * 负责将驱动注册到以太网接口适配层，以及使能全局中断
 * \param[in] p_dev     device instance
 */
aw_local void __enet_inst_connect (awbl_dev_t *p_dev)
{
    __ENET_DEVINFO_DECL(p_info, p_dev);
    __ENET_DEV_DECL(p_this, p_dev);
    uint8_t hwaddr[NETIF_MAX_HWADDR_LEN];


    if (AW_OK == aw_int_connect(p_info->inum, __enet_isr, p_this)) {
        if (p_info->get_info_entry->get_hwaddr) {
            p_info->get_info_entry->get_hwaddr(hwaddr, NETIF_MAX_HWADDR_LEN);
        }

        AW_SEMB_INIT(p_this->rx_semb, AW_SEM_EMPTY, AW_SEM_Q_FIFO);
        AW_TASK_INIT(p_this->rx_task, AWBL_IMX10XX_ENET_NAME, p_info->rx_task_prio,
                AWBL_IMX10XX_ENET_RX_THREAD_STK_SIZE, input_thread, p_this);

        AW_TASK_STARTUP(p_this->rx_task);
        aw_int_enable(p_info->inum);
        fec_start(p_this);

        aw_netif_add(&p_this->ethif, p_info->p_nic_name, &__g_ethif_ops,
                p_info->get_info_entry, AW_NETIF_TYPE_ETHERNET, hwaddr,
                __ENET_MTU);

        p_this->emac_init_flg = 1;
    }
}

/** \brief 导出AWbuslite操作函数 */
aw_local aw_const struct awbl_drvfuncs __g_enet_drvfuncs = {
    __enet_inst_init1,
    __enet_inst_init2,
    __enet_inst_connect
};

/** \brief enet驱动注册信息 */
aw_local aw_const struct awbl_drvinfo __g_enet_drv_registration = {
    AWBL_VER_1,                      /* awb_ver */
    AWBL_BUSID_PLB,                  /* bus_id */
    AWBL_IMX10XX_ENET_NAME,          /* p_drvname */
    &__g_enet_drvfuncs,              /* p_busfuncs */
    NULL,                            /* p_methods */
    NULL                             /* pfunc_drv_probe */
};

/******************************************************************************/

/** \brief enet驱动注册 */
void awbl_imx10xx_enet_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *) &__g_enet_drv_registration);
}

/* end of file */
