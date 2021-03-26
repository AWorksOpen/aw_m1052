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
 * \brief Ethernet AX88796 Driver.
 *
 * \internal
 * \par modification history:
 * - 1.10 18-04-13, phd, fix transmit and recive driver interface.
 * - 1.01 16-10-24, zoe, fix transmit and recive driver interface.
 * - 1.00 15-12-01, zoe, first implementation.
 * \endinternal
 */

#ifndef __AWBL_EMAC_AX88796_H
#define __AWBL_EMAC_AX88796_H

#include "aw_assert.h"
#include "aw_msgq.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "aw_task.h"
#include "awbl_miibus.h"

#include "aw_netif.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#define AWBL_AX88796_ENABLE             0

#if AWBL_AX88796_ENABLE
#define AWBL_AX88796_NAME               "awbl_emac_ax88796"
#define AWBL_AX88796_RX_THREAD_PRIO     3
#define AWBL_AX88796_RX_THREAD_STK_SIZE 2048

/* Support internal phy */
#define __AX88796_PHY_SUPPORT
/*****************************************************************************/
/*
 * The NE2000 register space is broken up into pages. There are
 * three available pages, selectable via two bits in the command register,
 * which is always mapped in the same place in all three pages.
 * Page 0 contains the main runtime registers. Page 1 contains the
 * physical address and multicast hash table registers. Page 2
 * contains diagnostic registers which aren't needed in normal operation.
 * Page 3 is invalid and should never be modified.
 */

/* Page 0 registers, read */

#define __ENE_CR    0x00    /**< \brief Command register */
#if 0 
#define __ENE_CLDA0    0x01    /**< \brief Current local DMA address 0 */
#define __ENE_CLDA1    0x02    /**< \brief Current local DMA address 1 */
#endif 
#define __ENE_BNRY   0x03    /**< \brief Boundary pointer */
#define __ENE_TSR    0x04    /**< \brief TX status register */
#define __ENE_NCR    0x05    /**< \brief Collision count */
#if 0 
#define __ENE_FIFO    0x06    /**< \brief FIFO */
#endif 
#define __ENE_ISR    0x07    /**< \brief Interrupt status register */
#define __ENE_CRDA0  0x08    /**< \brief Current remote DMA address 0 */
#define __ENE_CRDA1  0x09    /**< \brief Current remode DMA address 1 */
#define __ENE_RSVD0  0x0A
#define __ENE_RSVD1  0x0B
#define __ENE_RSR    0x0C    /**< \brief RX status register */
#define __ENE_CNTR0  0x0D    /**< \brief Frame alignment error counter */
#define __ENE_CNTR1  0x0E    /**< \brief CRC error counter */
#define __ENE_CNTR2  0x0F    /**< \brief Missed packet counter */

/**< \brief Page 0 registers, write */

#define __ENE_PSTART    0x01    /**< \brief Page start */
#define __ENE_PSTOP     0x02    /**< \brief Page stop */
#if 0 
#define __ENE_BNRY    0x03       Boundary pointer
#endif 
#define __ENE_TPSR      0x04    /**< \brief TX page start address */
#define __ENE_TBCR0     0x05    /**< \brief TX byte count 0 */
#define __ENE_TBCR1     0x06    /**< \brief TX byte count 1 */
#if 0 
#define __ENE_ISR    0x07       Interrupt status register */
#endif 
#define __ENE_RSAR0     0x08    /**< \brief Remote start address 0 */
#define __ENE_RSAR1     0x09    /**< \brief Remote start address 1 */
#define __ENE_RBCR0     0x0A    /**< \brief Remote byte count 0 */
#define __ENE_RBCR1     0x0B    /**< \brief Remote byte count 1 */
#define __ENE_RCR       0x0C    /**< \brief RX configuration register */
#define __ENE_TCR       0x0D    /**< \brief TX configuration register */
#define __ENE_DCR       0x0E    /**< \brief data configuration register */
#define __ENE_IMR       0x0F    /**< \brief Interrupt mask register */

/**< \brief AX88796 Private register PAGE0 */
#define __AX88796_MII_EEPROM    0x14
#define __ENE_BTCR          0x15  /**< \brief Buffer Type Configure Register */
#define __ENE_SR            0x17  /**< \brief AX88796B Status Register */
#define __ENE_FLOW          0x1A  /**< \brief AX88796B Flow control register */
#define __ENE_MCR           0x1B  /**< \brief Mac configure register */
#define __ENE_CTEPR         0x1C  /**< \brief Current TX End Page */
#define __ENE_VID0          0x1C  /**< \brief VLAN ID 0 */
#define __ENE_VID1          0x1D  /**< \brief VLAN ID 1 */

 /** \brief Issue a read to reset, a write to clear. */
#define __ENE_RESET         0x1F 

/**< \brief Command register */

#define __ENE_CR_STOP       0x01    /**< \brief Software reset */
#define __ENE_CR_START      0x02    /**< \brief Start ST-NIC */
#define __ENE_CR_TXP        0x04    /**< \brief Initiate packet transmission */
#define __ENE_CR_RDMA_CMD   0x38    /**< \brief RDMA command */
#define __ENE_CR_PAGESEL    0xC0    /**< \brief Page select */

#define __ENE_RDMA_READ     0x08    /**< \brief Remote read */
#define __ENE_RDMA_WRITE    0x10    /**< \brief Remote write */
#define __ENE_RDMA_SEND     0x18    /**< \brief Send packet */
#define __ENE_RDMA_ABORT    0x20    /**< \brief Abort/complete RDMA */

#define __ENE_PAGESEL_0     0x00    /**< \brief Select page 0 */
#define __ENE_PAGESEL_1     0x40    /**< \brief Select page 1 */
#define __ENE_PAGESEL_2     0x80    /**< \brief Select page 2 */
#define __ENE_PAGESEL_3     0xC0    /**< \brief reserved */


/**< \brief ISR register bit */

#define __ENE_ISR_PRX    0x01    /**< \brief Packet received */
#define __ENE_ISR_PTX    0x02    /**< \brief Packet sent */
#define __ENE_ISR_RXE    0x04    /**< \brief Receive error */
#define __ENE_ISR_TXE    0x08    /**< \brief Transmit error */
#define __ENE_ISR_OVW    0x10    /**< \brief RX overrun */
#define __ENE_ISR_CNT    0x20    /**< \brief Counter overflow */
#define __ENE_ISR_RDC    0x40    /**< \brief Remote DMA complete */
#define __ENE_ISR_RST    0x80    /**< \brief Reset complete */

/**< \brief Data configuration register */

/** \brief 0 = byte transfers, 1 = word transfers */
#define __ENE_DCR_WTS    0x01    

/** \brief 0 = little endian, 1 = big endian */
#define __ENE_DCR_BOS    0x02    

/** \brief 0 = dual 16 bit DMA, 1 = single 32 bit DMA*/
#define __ENE_DCR_LAS    0x04    
#define __ENE_DCR_LS     0x08    /**< \brief 0 = loopback, 1 = normal */
#define __ENE_DCR_ARM    0x10    /**< \brief autoinit remote DMA */
#define __ENE_DCR_FT     0x60    /**< \brief FIFO threshold select */

#define __ENE_FIFOTHR_1WORD     0x00
#define __ENE_FIFOTHR_2WORDS    0x20
#define __ENE_FIFOTHR_4WORDS    0x40
#define __ENE_FIFOTHR_6WORDS    0x60


/**< \brief TX configuration register */

#define __ENE_TCR_CRC   0x01    /**< \brief 1 = inhibit CRC generation */
#define __ENE_TCR_LB    0x06    /**< \brief loopback control */

/** \brief Auto transmit disable (flow control) */
#define __ENE_TCR_ATD   0x08    
#define __ENE_TCR_OFST  0x10    /**< \brief Collision offset enable */

#define __ENE_LOOP_OFF    0x00    /**< \brief Normal operation, loopback off */
#define __ENE_LOOP_NIC    0x02    /**< \brief NIC/MAC loopback */
#define __ENE_LOOP_ENDEC  0x04    /**< \brief ENDEC loopback */
#define __ENE_LOOP_EXT    0x06    /**< \brief External loopback */


/**< \brief TX status register */

#define __ENE_TSR_PTX    0x01    /**< \brief Frame transmitted */
#define __ENE_TSR_COL    0x04    /**< \brief Collision detected */
#define __ENE_TSR_ABT    0x08    /**< \brief Abort due to excess collisions */
#define __ENE_TSR_CRS    0x10    /**< \brief Carrier sense lost */
#define __ENE_TSR_FU     0x20    /**< \brief FIFO underrun */
#define __ENE_TSR_CDH    0x40    /**< \brief CD heartbeat failure */
#define __ENE_TSR_OWC    0x80    /**< \brief Out of window (late) collision */


/**< \brief RX configuration register */

#define __ENE_RCR_SEP    0x01    /**< \brief Save bad frames */
#define __ENE_RCR_AR     0x02    /**< \brief Accept runt frames */
#define __ENE_RCR_AB     0x04    /**< \brief Accept broadcast frames */
#define __ENE_RCR_AM     0x08    /**< \brief Accept multicast frames */
#define __ENE_RCR_PRO    0x10    /**< \brief Accept all unicasts */
#define __ENE_RCR_MON    0x20    /**< \brief Monitor mode */


/**< \brief RX status register */

#define __ENE_RSR_PRX    0x01    /**< \brief Frame received */
#define __ENE_RSR_CRC    0x02    /**< \brief CRC error */
#define __ENE_RSR_FAE    0x04    /**< \brief Frame alignment error */
#define __ENE_RSR_FO     0x08    /**< \brief FIFO overrun */
#define __ENE_RSR_MPA    0x10    /**< \brief Missed frame */
#define __ENE_RSR_PHY    0x20    /**< \brief 0 = unicast, 1 = multicast */

/** \brief Receiver disabled (in monitor mode) */
#define __ENE_RSR_DIS    0x40    
#define __ENE_RSR_DFR    0x80    /**< \brief Deferring, jabber */


/**< \brief Page 1 registers, read/write */

#define __ENE_PAR0    0x01    /**< \brief Station address 0 */
#define __ENE_PAR1    0x02    /**< \brief Station address 1 */
#define __ENE_PAR2    0x03    /**< \brief Station address 2 */
#define __ENE_PAR3    0x04    /**< \brief Station address 3 */
#define __ENE_PAR4    0x05    /**< \brief Station address 4 */
#define __ENE_PAR5    0x06    /**< \brief Station address 5 */
#define __ENE_CURR    0x07    /**< \brief Current page */
#define __ENE_MAR0    0x08    /**< \brief Multicast hash table 0 */
#define __ENE_MAR1    0x09    /**< \brief Multicast hash table 1 */
#define __ENE_MAR2    0x0A    /**< \brief Multicast hash table 2 */
#define __ENE_MAR3    0x0B    /**< \brief Multicast hash table 3 */
#define __ENE_MAR4    0x0C    /**< \brief Multicast hash table 4 */
#define __ENE_MAR5    0x0D    /**< \brief Multicast hash table 5 */
#define __ENE_MAR6    0x0E    /**< \brief Multicast hash table 6 */
#define __ENE_MAR7    0x0F    /**< \brief Multicast hash table 7 */

/**< \brief Bits in device status register, EN0_SR */
#define __ENSR_DMA_DONE     0x40        /**< \brief Remote DMA completed */
#define __ENSR_DMA_READY    0x20        /**< \brief Remote DMA ready */
#define __ENSR_DEV_READY    0x10        /**< \brief Device ready */
#define __ENSR_SPEED_100    0x04        /**< \brief PHY link at 100 Mb/s */
#define __ENSR_DUPLEX_DULL  0x02        /**< \brief PHY link at full duplex */
#define __ENSR_LINK         0x01        /**< \brief PHY link up */

/**< \brief AX88796 MDIO */
#define __MDIO_SHIFT_CLK    0x01
#define __MDIO_DATA_WRITE0  0x00
#define __MDIO_DATA_WRITE1  0x08
#define __MDIO_DATA_READ    0x04
#define __MDIO_MASK         0x0f
#define __MDIO_ENB_IN       0x02

#define __ENE_MISC  0x0D

/**< \brief NE2000 data port offset */
#define __ENE_IOPORT    0x10

/**< \brief NE2000 reset port */
#define __ENE_RESET         0x1F

/** \brief __ENE_RCR: broadcasts, no multicast,errors */
#define __ENE_RXCONFIG      0x4C    
#define __ENE_RXOFF         0x20  /**< \brief __ENE_RCR: Accept no packets */
#define __ENE_TXCONFIG      0x80  /**< \brief __ENE_TCR: Normal transmit mode */
#define __ENE_TXOFF         0x02  /**< \brief __ENE_TCR: Transmitter off */

/**< \brief AX88796 MAC Configure Regiseter Bit */
#define __ENE_TQC_ENABLE 0x20   /**< \brief Back-To-Back Transmission Control */
/******************************************************************************
  寄存器读写宏定义
******************************************************************************/
#define __ENE_SHIFT(offset)  ((offset) << 1)

#define __ENEREG_BYTE_READ(base, offset) \
        (*((volatile uint8_t *)((base) + __ENE_SHIFT(offset))))

#define __ENEREG_SHORT_READ(base, offset)  \
        (*((volatile uint16_t *)((base) + __ENE_SHIFT(offset))))

#define __ENEREG_BYTE_WRITE(base, offset, data)  \
        (*((volatile uint8_t *)((base) + __ENE_SHIFT(offset))))  = \
            (uint8_t)(data)

#define __ENEREG_SHORT_WRITE(base, offset, data)  \
        (*((volatile uint16_t *)((base) + __ENE_SHIFT(offset)))) = \
            (uint16_t)(data)

/*****************************************************************************/
struct __ene_rxpkt_hdr {
    uint8_t  status;       /**< \brief rx packet status */
    uint8_t  next_page;    /**< \brief page next pkt starts at */
    uint16_t len;          /**< \brief frame length */
};

/**
 * Helper struct to hold private data used to operate the ethernet interface.
 */
struct awbl_ne2000_emacif {
    aw_netif_t ethif;               /**< \brief Ethernet struct  */
    AW_MUTEX_DECL(ene_out_mutex);   /**< \brief output mutex  */
};

/**
 * \brief NE2000 设备信息
 */
struct awbl_ne2000_devinfo {
	char  *p_nic_name;
	aw_netif_info_get_entry_t *get_info_entry;

    uint32_t  regbase;                  /**< \brief register base  */

    uint32_t  eint_gpio;                /**< \brief EINT_GPIO   */

    aw_bool_t    byte_access;           /**< \brief Byte Acess  */

    int       create_mii_bus_id;        /**< \brief Create MII Bus ID */
    int       phy_use_mii_bus_id;       /**< \brief Use MII Bus ID */

    uint32_t *reg_offset;               /**< \brief NE2000 register offset */

    void (*pfunc_plfm_init) (void);     /**< \brief platform init */
};

/**
 * \brief NE2000 设备实例
 */
struct awbl_ne2000_dev {
    struct awbl_dev            ene_dev; /**< \brief ne2000 dev super */
    struct awbl_ne2000_emacif  emac_if; /**< \brief ne2000 ethernet interface */

    struct awbl_mii_master      mii_master;     /**< \brief  MII master  */
    struct awbl_dev            *p_ene_mii_dev;  /**< \brief  Get MII device */

    
    awbl_miibus_read_t      mii_phy_read;   /**< \brief  PHY read function */
    awbl_miibus_write_t     mii_phy_write;  /**< \brief  PHY write function */
    AW_SEMB_DECL(           mii_dev_semb);  /**< \brief  PHY device lock */
    uint8_t                 tx_full;        /**< \brief  Tx Buffer full stat */
    uint8_t                 tx_curr_page;   /**< \brief  current tx page */
    uint8_t                 tx_prev_ctepr;  /**< \brief  tx prev ctepr  */
    uint8_t                 tx_curr_ctepr;  /**< \brief  tx current ctepr  */
    uint8_t                 tx_start_page;  /**< \brief  tx start page  */
    uint8_t                 tx_stop_page;   /**< \brief  tx stop page  */
    uint8_t                 rx_start_page;  /**< \brief  rx_start_page  */
    uint8_t                 rx_curr_page;   /**< \brief  rx_curr_page  */
    uint8_t                 stop_page;      /**< \brief  stop_page  */
    uint8_t                 txing;          /**< \brief  is txing  */

    
    /** \brief  current rx handling, AW_TRUE : handling   */
    aw_bool_t                      rx_handling; 

    /** \brief rx overrun handling, AW_TRUE : handling   */
    aw_bool_t                      rx_overrun;     
    
    /**< \brief save current interrupt mask   */
    uint8_t                     int_mask;       

    aw_spinlock_isr_t           lock;           /**< \brief device lock */

    uint32_t                    phy_mode;       /**< \brief PHY mode */
    uint32_t                    phy_stat;       /**< \brief PHY stat */

    /** \brief NE2000 register offset */
    uint32_t                   *reg_offset;     

    AW_SEMB_DECL(               rx_semb);
    AW_TASK_DECL(               rx_task, AWBL_AX88796_RX_THREAD_STK_SIZE);
};

/**
 * \brief AX88796 driver register
 */
void awbl_emac_ax88796_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */
#endif
#endif  /* __AWBL_EMAC_AX88796_H */

/* end of file */

