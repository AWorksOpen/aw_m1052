
#ifndef  __IMX10xx_ENET_REGS_H
#define  __IMX10xx_ENET_REGS_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#pragma pack(push)
#pragma pack(1)

/**
 * \brief ENET register definitions
 */
struct ethernet_regs {
    uint32_t res0[1];            /**< \brief 0x000 */
    uint32_t ievent;             /**< \brief 0x004,Interrupt Event Register */
    uint32_t imask;              /**< \brief 0x008,Interrupt Mask Register */
    uint32_t res1[1];            /**< \brief 0x00C */
    uint32_t r_des_active;       /**< \brief 0x010,Receive Descriptor Active Register */
    uint32_t x_des_active;       /**< \brief 0x014,Transmit Descriptor Active Register */
    uint32_t res2[3];            /**< \brief 0x018-20 */
    uint32_t ecntrl;             /**< \brief 0x024,Ethernet Control Register */
    uint32_t res3[6];            /**< \brief 0x028-03C */
    uint32_t mii_data;           /**< \brief 0x040,MII Management Frame Register */
    uint32_t mii_speed;          /**< \brief 0x044,MII Speed Control Register */
    uint32_t res4[7];            /**< \brief 0x048-60 */
    uint32_t mib_control;        /**< \brief 0x064,MIB Control Register */
    uint32_t res5[7];            /**< \brief 0x068-80 */
    uint32_t r_cntrl;            /**< \brief 0x084,Receive Control Register */
    uint32_t res6[15];           /**< \brief 0x088-C0 */
    uint32_t x_cntrl;            /**< \brief 0x0C4,Transmit Control Register */
    uint32_t res7[7];            /**< \brief 0x0C8-E0 */
    uint32_t paddr1;             /**< \brief 0x0E4,Physical Address Lower Register */
    uint32_t paddr2;             /**< \brief 0x0E8,Physical Address Upper Register */
    uint32_t op_pause;           /**< \brief 0x0EC,Opcode/Pause Duration Register */
    uint32_t txic;               /**< \brief 0x0F0,Transmit Interrupt Coalescing */
    uint32_t res81[3];           /**< \brief 0x0F4-FC */
    uint32_t rxic;               /**< \brief 0x100,Receive Interrupt Coalescing */
    uint32_t res82[5];           /**< \brief 0x104-114 */
    uint32_t iaddr1;             /**< \brief 0x118,Descriptor Individual Upper Address */
    uint32_t iaddr2;             /**< \brief 0x11C,Descriptor Individual Lower Address */
    uint32_t gaddr1;             /**< \brief 0x120,Descriptor Group Upper Address */
    uint32_t gaddr2;             /**< \brief 0x124,Descriptor Group Lower Address */
    uint32_t res9[7];            /**< \brief 0x128-140 */
    uint32_t x_wmrk;             /**< \brief 0x144,Transmit FIFO Watermark Registerv */
    uint32_t res10[1];           /**< \brief 0x148 */
    uint32_t r_bound;            /**< \brief 0x14C */
    uint32_t r_fstart;           /**< \brief 0x150 */
    uint32_t res11[11];          /**< \brief 0x154-17C */
    uint32_t erdsr;              /**< \brief 0x180,Receive Descriptor Ring Start Register */
    uint32_t etdsr;              /**< \brief 0x184,Transmit Buffer Descriptor Ring Start Register */
    uint32_t emrbr;              /**< \brief 0x188,Maximum Receive Buffer Size */
    uint32_t res12[1];           /**< \brief 0x18C-18C */
    uint32_t rsfl;               /**< \brief 0x190,Receive FIFO Section Full Threshold */
    uint32_t rsem;               /**< \brief 0x194,Receive FIFO Section Empty Threshold */
    uint32_t raem;               /**< \brief 0x198,Receive FIFO Almost Empty Threshold */
    uint32_t rafl;               /**< \brief 0x19C,Receive FIFO Almost Full Threshold */
    uint32_t tsem;               /**< \brief 0x1A0,Transmit FIFO Section Empty Threshold */
    uint32_t taem;               /**< \brief 0x1A4,Transmit FIFO Almost Empty Threshold */
    uint32_t tafl;               /**< \brief 0x1A8,Transmit FIFO Almost Full Threshold */
    uint32_t tipg;               /**< \brief 0x1AC,Transmit Inter-Packet Gap */
    uint32_t ftrl;               /**< \brief 0x1B0,Frame Truncation Length */
    uint32_t res13[3];           /**< \brief 0x1B4-1BC */
    uint32_t tacc;               /**< \brief 0x1C0,Transmit Accelerator Function Configuration */
    uint32_t racc;               /**< \brief 0x1C4,Receive Accelerator Function Configuration */
} ;



/**
 * \brief ENET Receive & Transmit Buffer Descriptor definitions
 *
 * Note: The first BD must be aligned (see DB_ALIGNMENT)
 */
struct fec_bd {
    uint16_t        data_length;        /**< \brief payload's length in bytes */
    uint16_t        status;             /**< \brief BD's staus (see datasheet)*/
    uint32_t        data_pointer;       /**< \brief payload's buffer address*/
};
#pragma pack(pop)

/** @} grp_awbl_drv_imx10xx_enet_hardware_struct*/

/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_micro ENET hardware micro
 * @{
 */

/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_interrupt_events ENET interrupt events flags
 * @{
 */

/** \brief Babbling Receive Error*/
#define FEC_IEVENT_BABR         0x40000000
/** \brief Babbling Transmit Error*/
#define FEC_IEVENT_BABT         0x20000000
/** \brief Graceful Stop Complete*/
#define FEC_IEVENT_GRA          0x10000000
/** \brief Transmit Frame Interrupt*/
#define FEC_IEVENT_TXF          0x08000000
/** \brief Transmit Buffer Interrupt*/
#define FEC_IEVENT_TXB          0x04000000
/** \brief Receive Frame Interrupt*/
#define FEC_IEVENT_RXF          0x02000000
/** \brief Receive Buffer Interrupt*/
#define FEC_IEVENT_RXB          0x01000000
/** \brief MII Interrupt*/
#define FEC_IEVENT_MII          0x00800000
/** \brief Ethernet Bus Error*/
#define FEC_IEVENT_EBERR        0x00400000
/** \brief Late Collision*/
#define FEC_IEVENT_LC           0x00200000
/** \brief Collision Retry Limit*/
#define FEC_IEVENT_RL           0x00100000
/** \brief Transmit FIFO Underrun */
#define FEC_IEVENT_UN           0x00080000
/** \brief Node Wakeup Request Indication */
#define FEC_IEVENT_PLR          0x00040000
/** \brief Node Wakeup Request Indication*/
#define FEC_IEVENT_WAKEUP       0x00020000
/** \brief Transmit Timestamp Available */
#define FEC_IEVENT_TS_AVAIL     0x00010000
/** \brief Timestamp Timer*/
#define FEC_IEVENT_TS_TIMER     0x00008000

/** @} grp_awbl_drv_imx10xx_enet_hardware_interrupt_events*/

/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_interrupt_mask ENET interrupt mask flags
 * @{
 */
/** \brief BABR Interrupt Mask */
#define FEC_IMASK_BABR          0x40000000
/** \brief BABT Interrupt Mask */
#define FEC_IMASKT_BABT         0x20000000
/** \brief GRA Interrupt Mask */
#define FEC_IMASK_GRA           0x10000000
/** \brief TXF Interrupt Mask */
#define FEC_IMASKT_TXF          0x08000000
/** \brief TXB Interrupt Mask */
#define FEC_IMASK_TXB           0x04000000
/** \brief RXF Interrupt Mask */
#define FEC_IMASKT_RXF          0x02000000
/** \brief RXB Interrupt Mask */
#define FEC_IMASK_RXB           0x01000000
/** \brief MII Interrupt Mask */
#define FEC_IMASK_MII           0x00800000
/** \brief EBERR Interrupt Mask */
#define FEC_IMASK_EBERR         0x00400000
/** \brief LC Interrupt Mask */
#define FEC_IMASK_LC            0x00200000
/** \brief RL Interrupt Mask */
#define FEC_IMASKT_RL           0x00100000
/** \brief UN Interrupt Mask */
#define FEC_IMASK_UN            0x00080000
/** @} grp_awbl_drv_imx10xx_enet_hardware_interrupt_mask*/

/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_rx_control Receive control flags
 * @{
 */
/** \brief the bit offset of Maximum Frame Length */
#define FEC_RCNTRL_MAX_FL_SHIFT 16
/** \brief Internal Loopback */
#define FEC_RCNTRL_LOOP         0x00000001
/** \brief Disable Receive On Transmit */
#define FEC_RCNTRL_DRT          0x00000002
/** \brief Media Independent Interface Mode */
#define FEC_RCNTRL_MII_MODE     0x00000004
/** \brief Promiscuous Mode */
#define FEC_RCNTRL_PROM         0x00000008
/** \brief Broadcast Frame Reject */
#define FEC_RCNTRL_BC_REJ       0x00000010
/** \brief Flow Control Enable */
#define FEC_RCNTRL_FCE          0x00000020
/** \brief RMII Mode Enable */
#define FEC_RCNTRL_RMII         0x00000100
/** \brief Enables 10-Mbit/s mode of the RMII */
#define FEC_RCNTRL_RMII_10T     0x00000200

/** @} grp_awbl_drv_imx10xx_enet_hardware_rx_control*/


/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_tx_control transmit control flags
 * @{
 */
/** \brief Graceful Transmit Stop */
#define FEC_TCNTRL_GTS          0x00000001
/** \brief Full-Duplex Enable */
#define FEC_TCNTRL_FDEN         0x00000004
/** \brief Transmit Frame Control Pause */
#define FEC_TCNTRL_TFC_PAUSE    0x00000008
/** \brief Receive Frame Control Pause */
#define FEC_TCNTRL_RFC_PAUSE    0x00000010
/** @} grp_awbl_drv_imx10xx_enet_hardware_tx_control */

/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_enet_control enet control flags
 * @{
 */
/** \brief Ethernet MAC Reset */
#define FEC_ECNTRL_RESET        0x00000001
/** \brief Ethernet Enable */
#define FEC_ECNTRL_ETHER_EN     0x00000002
/** \brief Sleep Mode Enable */
#define FEC_ECNTRL_SPEED        0x00000020
/** \brief Descriptor Byte Swapping Enable */
#define FEC_ECNTRL_DBSWAP       0x00000100
/** @} grp_awbl_drv_imx10xx_enet_hardware_enet_control */


/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_tx_fifo_watermark transmit Watermark flags
 * @{
 */
/** \brief Store And Forward Enable */
#define FEC_X_WMRK_STRFWD       0x00000100
/** @} grp_awbl_drv_imx10xx_enet_hardware_tx_fifo_watermark */


/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_tx_rx_active_des transmit & receive active descriptor flags
 * @{
 */
/** \brief Transmit Descriptor Active */
#define FEC_X_DES_ACTIVE_TDAR       0x01000000
/** \brief Receive Descriptor Active */
#define FEC_R_DES_ACTIVE_RDAR       0x01000000
/** @} grp_awbl_drv_imx10xx_enet_hardware_tx_rx_active_des */

/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_mii_data MII data flags
 * @{
 */
/** \brief Start of frame delimiter */
#define FEC_MII_DATA_ST         0x40000000
/** \brief Perform a read operation */
#define FEC_MII_DATA_OP_RD      0x20000000
/** \brief Perform a write operation */
#define FEC_MII_DATA_OP_WR      0x10000000
/** \brief PHY Address field mask */
#define FEC_MII_DATA_PA_MSK     0x0f800000
/** \brief PHY Register field mask */
#define FEC_MII_DATA_RA_MSK     0x007c0000
/** \brief Turnaround */
#define FEC_MII_DATA_TA         0x00020000
/** \brief PHY data field */
#define FEC_MII_DATA_DATAMSK    0x0000ffff
/** \brief MII Register address bits */
#define FEC_MII_DATA_RA_SHIFT   18
/** \brief MII PHY address bits */
#define FEC_MII_DATA_PA_SHIFT   23
/** @} grp_awbl_drv_imx10xx_enet_hardware_mii_data */

/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_rx_des_status receive descriptor status flags
 * @{
 */
/** \brief Receive BD status: Buffer is empty */
#define FEC_RBD_EMPTY   0x8000
/** \brief Receive BD status: Last BD in ring */
#define FEC_RBD_WRAP    0x2000
/** \brief Receive BD status: Buffer is last in frame (useless here!) */
#define FEC_RBD_LAST    0x0800
/** \brief Receive BD status: Miss bit for prom mode */
#define FEC_RBD_MISS    0x0100
/** \brief Receive BD status: The received frame is broadcast frame */
#define FEC_RBD_BC  0x0080

/** \brief Receive BD status: The received frame is multicast frame */
#define FEC_RBD_MC  0x0040
/** \brief Receive BD status: Frame length violation */
#define FEC_RBD_LG  0x0020
/** \brief Receive BD status: Nonoctet align frame */
#define FEC_RBD_NO  0x0010
/** \brief Receive BD status: CRC error */
#define FEC_RBD_CR  0x0004
/** \brief Receive BD status: Receive FIFO overrun */
#define FEC_RBD_OV  0x0002
/** \brief Receive BD status: Frame is truncated  */
#define FEC_RBD_TR  0x0001
/** \brief reveive err */
#define FEC_RBD_ERR (FEC_RBD_LG | FEC_RBD_NO | FEC_RBD_CR | \
            FEC_RBD_OV | FEC_RBD_TR)

/** @} grp_awbl_drv_imx10xx_enet_hardware_rx_des_status */

/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_tx_des_status transmit descriptor status flags
 * @{
 */
/** \brief Tansmit BD status: Buffer is ready */
#define FEC_TBD_READY   0x8000
/** \brief Tansmit BD status: Mark as last BD in ring */
#define FEC_TBD_WRAP    0x2000
/** \brief Tansmit BD status: Buffer is last in frame */
#define FEC_TBD_LAST    0x0800
/** \brief Tansmit BD status: Transmit the CRC */
#define FEC_TBD_TC      0x0400
/** \brief Tansmit BD status: Append bad CRC */
#define FEC_TBD_ABC     0x0200
/** @} grp_awbl_drv_imx10xx_enet_hardware_tx_des_status */

/**
 * \addtogroup grp_awbl_drv_imx10xx_enet_hardware_others Others
 * @{
 */
/** \brief  the ethernet packet size limit in memory */
#define FEC_MAX_PKT_SIZE            1536
/** \brief enet's dma align */
#define FEC_DMA_ALIGN               64
/** \brief max recv packet size */
#define FEC_RECV_PKT_SIZE           1518

/** @} grp_awbl_drv_imx10xx_enet_hardware_others */


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX10xx_ENET_REGS_H */
