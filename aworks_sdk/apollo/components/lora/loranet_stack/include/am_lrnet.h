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
 * \brief   LoRaNet Application Program Interface
 *
 * \internal
 * \par modification history:
 * - 2017-08-08 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_H
#define __AM_LRNET_H

#include "am_lora_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name LoRaNet Version Definition
 * @{
 */
#define AM_LRNET_VER_MAJOR                   0   /**< \brief major version */
#define AM_LRNET_VER_MINOR                   6   /**< \brief minor version */
#define AM_LRNET_VER_REVISED                 0   /**< \brief revised version */
/** @} */

/** \brief Return Code: packet is sent, but no ack packet return */
#define AM_LRNET_RC_NOT_ACK                  2000

/** \brief Return Code: timeout */
#define AM_LRNET_RC_TIMEOUT                  2001

/** @} */

/**
 * \name LoRaNet Address Definition
 * @{
 */
 
/** \brief address of center */
#define AM_LRNET_ADDR_CENTER                 0x0000  

/** \brief lowest address of router */
#define AM_LRNET_ADDR_ROUTER_LOWEST          0x0001  

/** \brief highest address of router */
#define AM_LRNET_ADDR_ROUTER_HIGHEST         0x007F  

/** \brief invalid address of router */
#define AM_LRNET_ADDR_INVALID                0x00FF  

/** \brief lowest address of terminal */
#define AM_LRNET_ADDR_TERMINAL_LOWEST        0x0100  

/** \brief highest address of terminal */
#define AM_LRNET_ADDR_TERMINAL_HIGHEST       0x06FF  

/** \brief lowest address of group cast */
#define AM_LRNET_ADDR_GROUP_CAST_LOWEST      0xFF00  

/** \brief highest address of group cast */
#define AM_LRNET_ADDR_GROUP_CAST_HIGHEST     0xFFFE  

/** \brief broadcast address */
#define AM_LRNET_ADDR_BROADCAST              0xFFFF  

#define AM_LRNET_ADDR_ROUTER_NUM                            \
            (AM_LRNET_ADDR_ROUTER_HIGHEST - AM_LRNET_ADDR_ROUTER_LOWEST + 1)

#define AM_LRNET_ADDR_TERMINAL_NUM                          \
            (AM_LRNET_ADDR_TERMINAL_HIGHEST - AM_LRNET_ADDR_TERMINAL_LOWEST + 1)

#define AM_LRNET_ADDR_GROUP_CAST(msb)                       \
            (0xFF00 | ((msb) & 0xFF))

#define AM_LRNET_ADDR_IS_ROUTER(addr)                       \
            ((addr >= AM_LRNET_ADDR_ROUTER_LOWEST) &&       \
             (addr <= AM_LRNET_ADDR_ROUTER_HIGHEST))

#define AM_LRNET_ADDR_IS_CENTER(addr)                       \
            (AM_LRNET_ADDR_CENTER == addr)

#define AM_LRNET_ADDR_IS_TERMINAL(addr)                     \
            ((addr >= AM_LRNET_ADDR_TERMINAL_LOWEST) &&     \
            (addr <= AM_LRNET_ADDR_TERMINAL_HIGHEST))

#define AM_LRNET_ADDR_IS_GROUP_CAST(addr)                   \
            ((addr >= AM_LRNET_ADDR_GROUP_CAST_LOWEST) &&   \
            (addr <= AM_LRNET_ADDR_GROUP_CAST_HIGHEST))

/** @} */

/** \brief max size of packet payload */
#define AM_LRNET_PACKET_PAYLOAD_SIZE_MAX     210

/**
 * \brief LoRaNet RF State Enumerate
 */
enum am_lrnet_rf_state {
    AM_LRNET_RF_STATE_IDLE,     /**< \brief LoRaNet RF Idle */
    AM_LRNET_RF_STATE_RX,       /**< \brief LoRaNet RF in RX mode */
    AM_LRNET_RF_STATE_TX,       /**< \brief LoRaNet RF in TX mode */
};

/**
 * \brief Common Event Callback Function Table
 */
typedef struct am_lrnet_common_evt_cb {

    /** \brief Callback while RF state changed */
    void (*pfn_rf_state_changed) (enum am_lrnet_rf_state state);
} am_lrnet_common_evt_cb_t;

/**
 * \brief Received Data Structure
 */
typedef struct {
    const uint8_t *p_data;      /**< \brief data */
    uint8_t        data_size;   /**< \brief size of received data */
    uint16_t       dev_addr;    /**< \brief sender of received data */
} am_lrnet_data_t;

/**
 * \brief Fetch Request Structure
 */
typedef struct {
    uint16_t       dev_addr;            /**< \brief sender of received data */
    const uint8_t *p_data;              /**< \brief received data */
    uint8_t       *p_respond_buffer;    /**< \brief buffer of responding data */
    uint16_t       data_size;           /**< \brief size of received data */
    uint16_t       respond_buffer_size; /**< \brief size of responding buffer */
} am_lrnet_fetch_request_t;

/**
 * \brief Upstream Request Structure
 */
typedef struct {
    uint8_t       *p_respond_buffer;    /**< \brief buffer of responding data */
    uint16_t       respond_buffer_size; /**< \brief size of responding buffer */
} am_lrnet_upstream_request_t;

/** @} */

/**
 * \name LoRaNet Configuration Definition
 * @{
 */

/**
 * \brief LoRaNet Work Mode Enumerate
 */
typedef enum {
    AM_LRNET_MODE_NORMAL,          /**< \brief always receive */
    AM_LRNET_MODE_PERIOD_WAKE,     /**< \brief period wakeup and receive */
    AM_LRNET_MODE_SLEEP,           /**< \brief always sleep */
} am_lrnet_mode_t;

/**
 * \brief LoRaNet Rate Enumerate
 */
typedef enum {
    AM_LRNET_RF_RATE_300,   /**< \brief 0.3kbps  (SF=12, BW=125K) */
    AM_LRNET_RF_RATE_600,   /**< \brief 0.6kbps  (SF=11, BW=125K) */
    AM_LRNET_RF_RATE_1200,  /**< \brief 1.2kbps  (SF=10, BW=125K) */
    AM_LRNET_RF_RATE_2400,  /**< \brief 2.4kbps  (SF=9,  BW=125K) */
    AM_LRNET_RF_RATE_4800,  /**< \brief 4.8kbps  (SF=8,  BW=125K) */
    AM_LRNET_RF_RATE_9600,  /**< \brief 9.6kbps  (SF=7,  BW=125K) */
    AM_LRNET_RF_RATE_19200, /**< \brief 19.2kbps (SF=7,  BW=250K) */
} am_lrnet_rf_rate_t;

/**
 * \brief LoRaNet Coding Rate Enumerate
 */
typedef enum {
    AM_LRNET_RF_CR_5,       /**< \brief Coding Rate 4/5 */
    AM_LRNET_RF_CR_6,       /**< \brief Coding Rate 4/6 */
    AM_LRNET_RF_CR_7,       /**< \brief Coding Rate 4/7 */
    AM_LRNET_RF_CR_8        /**< \brief Coding Rate 4/8 */
} am_lrnet_rf_cr_t;

/**
 * \brief LoRaNet RF Configuration
 */
typedef struct am_lrnet_rf_cfg {
    
    /** \brief LoRa RF Data Channel Frequency */
    uint32_t           data_channel;  
    uint32_t           ack_channel; /**< \brief LoRa RF ACK Channel Frequency */
    am_lrnet_rf_rate_t rate;        /**< \brief LoRa RF Rate */
    am_lrnet_rf_cr_t   cr;          /**< \brief LoRa RF Coding Rate */
    int8_t             tx_power;    /**< \brief LoRa RF TX Power, unit: dBm */
} am_lrnet_rf_cfg_t;

/**
 * \brief LoRaNet Common Configuration
 */
typedef struct {

    /** \brief NetId */
    uint8_t  net_id;

    /** \brief Minimum RSSI of router, unit: dBm */
    int8_t   router_rssi_min;

    /** \brief maximal process time of a LoRaNet packet, unit: ms */
    uint8_t  packet_proc_time;

    /** \brief inaccuracy of wakeup interval, unit: percent */
    uint8_t  wakeup_interval_inacc;

    /** \brief deepsleep wake-up interval, unit: ms */
    uint16_t wakeup_interval;

    /** \brief deepsleep wake-up interval while network active, unit: ms */
    uint16_t wakeup_interval_short;

    /** \brief short deppsleep wake-up interval keep time, unit: second */
    uint16_t wakeup_interval_short_keep;

    /**
     * \brief  NetworkKey
     * \details for integrity check, point to a uint8_t[16] array
     */
    const uint8_t *p_network_key;

    /** \brief RF Configuration */
    am_lrnet_rf_cfg_t rf_cfg;
} am_lrnet_net_cfg_t;

/**
 * \brief Role Common Configuration Structure
 */
typedef struct {
    uint16_t         dev_addr; /**< \brief address of device */
    uint8_t          tx_retry; /**< \brief tx packet retry times */
    am_lrnet_mode_t  mode;     /**< \brief work mode */
} am_lrnet_role_common_cfg_t;

/**
 * \brief LoRaNet Upstream Mode Configuration
 */
typedef struct {
    uint8_t timepiece_sec;      /**< \brief timepiece, unit: second */

    /**
     * \brief   Time Bitmap
     * \details Time Bitmap describes the time to do upstream operation.
     *          24 hours a day be divide to 144 sections, each section is 10
     *          minute.
     *
     *          e.g.
     *          time_bitmap[0] is 0x01 signify devices will do Upstream on
     *          00:00AM everyday.
     *          time_bitmap[1] is 0x02 signify devices will do Upstream on
     *          01:30AM everyday.
     *
     *          Please fill it with macros AM_LRNET_UPSTREAM_TIME_BITMAP_SET.
     */
    uint8_t time_bitmap[18];
} am_lrnet_upstream_cfg_t;

/**
 * \brief   Macro to fill am_lrnet_upstream_cfg_t member time_bitmap
 *          e.g. Set 13:30
 *               AM_LRNET_UPSTREAM_TIME_BITMAP_SET(cfg.time_bitmap, 13, 30);
 */
#define AM_LRNET_UPSTREAM_TIME_BITMAP_SET(time_bitmap, oclock, minute)      \
                do {                                                        \
                    int tmp = (60*oclock + minute) / 10;                    \
                    time_bitmap[tmp/8] |= AM_LORA_BIT(tmp % 8);             \
                } while (0)


/*******************************************************************************
  LoRaNet Center Function Declaration
*******************************************************************************/

/**
 * \brief LoRaNet Center Event Callback Functions Table
 */
struct am_lrnet_center_evt_cb {

    /** \brief Common Event Callback Fuctions Table */
    struct am_lrnet_common_evt_cb common_cb;

    /** \brief Callback while received a fetch request */
    int (*pfn_fetch_request) (am_lrnet_fetch_request_t *p_request);

    /** \brief Upstream Data Rx Event Callback */
    void (*pfn_upstream_rx) (am_lrnet_data_t *p_data);

    /** \brief Exception Report Rx Event Callback */
    void (*pfn_exception_rx) (am_lrnet_data_t *p_data);
};

/**
 * \brief LoRaNet Center Configuration
 */
typedef struct am_lrnet_center_cfg {

    /** \brief LoRaNet Network Configuration */
    const am_lrnet_net_cfg_t *p_net_cfg;

    /** \brief Device Role Configuration */
    am_lrnet_role_common_cfg_t role_cfg;

    /** \brief LoRaNet Center Event Callback Functions Table */
    struct am_lrnet_center_evt_cb evt_cb;
} am_lrnet_center_cfg_t;

/**
 * \brief       Initialize LoRaNet Center
 *
 * \param[in]   p_cfg       center configuration
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 * \note        DO NOT call this function repeatedly.
 */
int am_lrnet_center_init (const am_lrnet_center_cfg_t *p_cfg);

/**
 * \brief       Center get router list
 *
 * \details     This API fill router address to p_buf until p_buf is full, if
 *              the buffer is not enough to store all the data, please call this
 *              API  repeatly until export progress completed.
 *
 * \param[out]      p_buf       router list output buffer
 * \param[in]       buf_size    size of output buffer
 * \param[in, out]  p_origin    pointing to a integer for recording the export
 *                              progress. Generally initialized to 0;
 *                              initialized to NULL if it is confident that the
 *                              buffer is big enough.
 *
 * \retval          number of router address in the output buffer, or negative
 *                  error code.
 */
int am_lrnet_center_routers_get (uint8_t *p_buf,
                                 uint8_t  buf_size,
                                 int     *p_origin);

/**
 * \brief       Clear route table
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_route_clear (void);

/**
 * \brief       Add record to route table
 *
 * \param[in]   tar_addr        address of target device, could be a terminal or
 *                              router.
 * \param[in]   router_addr     the router of the target device
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_route_add (uint16_t tar_addr, uint16_t router_addr);

/**
 * \brief      Remove record from route table
 *
 * \param[in]  addr             specify which device be remove.
 *
 * \return     AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_route_remove (uint16_t addr);

/**
 * \brief       Get the sub-node address list of specified router
 *8
 * \details     This API fill device address to p_buf until p_buf is full, if
 *              the buffer is not enough to store all the data, please call this
 *              API repeatly until export progress completed.
 *
 * \param[in]       router_addr specified router address
 * \param[out]      p_buf       output buffer of address list
 * \param[in]       buf_size    size of the output buffer
 * \param[in, out]  p_origin    pointing to a integer for recording the export
 *                              progress. Generally initialized to 0;
 *                              initialized to NULL if it is confident that the
 *                              buffer is big enough.
 *
 * \retval          number of router address in the output buffer, or negative
 *                  error code on fail.
 */
int am_lrnet_center_route_sub_dev_get (uint16_t  router_addr,
                                       uint16_t *p_buf,
                                       uint16_t  buf_size,
                                       int      *p_origin);

/**
 * \brief       Get specified device's super node device address
 *
 * \param[in]   terminal_addr   specified terminal address
 *
 * \return      super node device address, AM_LRNET_ADDR_INVALID on fail.
 */
uint16_t am_lrnet_center_route_super_get (uint16_t addr);

/**
 * \brief       Center Asynchronous Detect
 * \details     Clear route table, detect routers&terminals and add to route
 *              table, function return immediately.
 *              pfn_finish() would be called by an internal ISR while operation
 *              completed.
 *
 * \param[in]   sub_router_num      expecting surrounding router number
 * \param[in]   sub_terminal_num    expecting surrounding terminal number
 * \param[in]   pfn_finish          finish callback
 * \param[in]   p_arg               parameter for callback
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_detect_asyn (int     sub_router_num,
                                 int     sub_terminal_num,
                                 void  (*pfn_finish)(void *p_arg),
                                 void   *p_arg);

/**
 * \brief       Center Detect
 *
 * \details     Clear route table, detect routers&terminals and add to route
 *              table, function blocking until detecting completed.
 *
 * \param[in]   sub_router_num      expecting surrounding router number,
 *                                  set 0 to disable routers.
 * \param[in]   sub_terminal_num    expecting surrounding terminal number
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_detect (int sub_router_num, int sub_terminal_num);

/**
 * \brief       Center Asynchronous TX
 *
 * \param[in]   p_tx_data      data to send
 * \param[in]   pfn_finish     finish callback
 * \param[in]   p_arg          parameter for callback
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_tx_asyn (am_lrnet_data_t  *p_tx_data,
                             void            (*pfn_finish) (void *p_arg),
                             void             *p_arg);

/**
 * \brief       Center TX
 *
 * \param[in]   p_tx_data      data to send
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_tx (am_lrnet_data_t *p_tx_data);

/**
 * \brief       Center Asynchronous Fetch
 *
 * \details     Send to specified device and fetch respond data.
 *              pfn_finish() would be called by an internal ISR while operation
 *              completed.
 *
 * \param[in]   p_tx_data           data to send
 * \param[in]   pfn_finish          finish callback
 * \param[in]   p_arg               parameter for callback
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_fetch_asyn (
        am_lrnet_data_t  *p_tx_data,
        void            (*pfn_finish) (void *p_arg, am_lrnet_data_t *p_rx_data),
        void             *p_arg);

/**
 * \brief       Center Fetch
 * \details     Center send a downlink packet to specified terminal or router,
 *              wait for the respond from dev_addr until timeout.
 *
 * \param[in]   p_tx_data           data to send
 * \param[out]  p_rx_buf            buffer for received data
 * \param[in]   buf_size            size of p_rx_buf buffer
 * \param[out]  p_rx_data           received data
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_fetch (am_lrnet_data_t  *p_tx_data,
                           uint8_t          *p_rx_buf,
                           uint8_t           buf_size,
                           am_lrnet_data_t  *p_rx_data);

/**
 * \brief LoRaNet Center Group Fetch Configuration Structure
 */
typedef struct {

    /** \brief data to send */
    const uint8_t  *p_send_data;

    /** \brief size of data to send, maximum 174 Bytes. */
    uint8_t         send_data_size;

    /** \brief data size of group fetch reply data, maximum 201 Bytes. */
    uint8_t         fetch_data_size;

} am_lrnet_center_gfetch_cfg_t;


/**
 * \brief       Center Asynchronous Group Fetch
 *
 * \details     Start group fetch and return immediately, group fetch all
 *              terminal in route table.
 *              pfn_finish() would be called by an internal ISR while operation
 *              completed.
 *
 * \param[in]   p_cfg           group fetch configuration
 * \param[in]   pfn_rx          inform callback for each target device
 * \param[in]   pfn_finish      finish callback
 * \param[in]   p_arg           parameter for callback
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_gfetch_asyn (
      const am_lrnet_center_gfetch_cfg_t *p_cfg,
      void                              (*pfn_rx) (void            *p_arg,
                                                   am_lrnet_data_t *p_rx_data),
      void                              (*pfn_finish) (void *p_arg),
      void                               *p_arg);

/**
 * \brief       Center Group Fetch Specific Terminals
 * \details     Start group fetching, blocking till reply received.
 *              pfn_finish() would be called by an internal ISR while operation
 *              completed.
 *
 * \param[in]   p_cfg               group fetch configuration
 * \param[in]   router_addr         target router
 * \param[in]   p_terminal_list     target terminal list
 * \param[in]   terminal_num        number of target terminal
 * \param[in]   pfn_rx              inform callback for each target device
 * \param[in]   pfn_finish          finish callback
 * \param[in]   p_arg               parameter for callback
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_gfetch_specific_asyn (
        const am_lrnet_center_gfetch_cfg_t *p_cfg,
        uint8_t                             router_addr,
        const uint16_t                     *p_terminal_list,
        uint8_t                             terminal_num,
        void                              (*pfn_rx) (void            *p_arg,
                                                     am_lrnet_data_t *p_rx_data),
        void                              (*pfn_finish) (void *p_arg),
        void                               *p_arg);

/**
 * \brief       Center Asynchronous Time Broadcast
 *
 * \param[in]   pfn_finish      finish callback
 * \param[in]   p_arg           parameter for callback
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_time_broadcast_asyn (void (*pfn_finish) (void *p_arg),
                                         void  *p_arg);

/**
 * \brief       Center Time Broadcast
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_time_broadcast (void);

/**
 * \brief       Center Asynchronous Ping
 * \details     Send a ping packet to specified device and wait for the respond
 *              packet to judge if the device still alive.
 *              pfn_finish() would be called by an internal ISR while operation
 *              completed.
 *
 * \param[in]   dev_addr            specified device address
 * \param[in]   payload_size        payload size of packet
 * \param[in]   pfn_finish          finish callback
 * \param[in]   p_arg               parameter for callback
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_ping_asyn (uint16_t  dev_addr,
                               uint16_t  payload_size,
                               void    (*pfn_finish) (void     *p_arg,
                                                      uint16_t  addr,
                                                      int32_t   ms),
                               void     *p_arg);

/**
 * \brief       LoRaNet Center Ping
 *
 * \details     Send a ping packet to specified device and wait for the respond
 *              packet to judge if the device still alive.
 *              Function blocking until operation completed
 *
 * \param[in]   dev_addr            specified device address
 * \param[in]   payload_size        payload size of packet
 *
 * \return      Time taken(unit: ms), or negative errno code on fail
 */
int am_lrnet_center_ping (uint16_t dev_addr, uint16_t payload_size);

/**
 * \brief       Center Asynchronous Upstream Broadcast
 *
 * \details     Broadcast to inform terminals do upstream operation with
 *              specified configuration. Router is ignored in this API.
 *              pfn_finish() would be called by an internal ISR while operation
 *              completed.
 *
 * \param[in]   p_cfg               upstream configuration
 * \param[out]  pfn_finish          finish callback
 * \param[out]  p_arg               parameter for callback
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_upstream_broadcast_asyn (
            am_lrnet_upstream_cfg_t *p_cfg,
            void                   (*pfn_finish) (void *p_arg),
            void                    *p_arg);


/**
 * \brief       Center Upstream Broadcast
 *
 * \details     Broadcast to inform terminals do upstream operation with
 *              specified configuration. Router is ignored in this API.
 *
 * \param[in]   p_cfg               upstream configuration
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_center_upstream_broadcast (am_lrnet_upstream_cfg_t *p_cfg);


/*******************************************************************************
  LoRaNet Router Function Declaration
*******************************************************************************/

/** \brief LoRaNet Router Event Callback Functions Table */
struct am_lrnet_router_evt_cb {

    /** \brief Common Event Callback Fuctions Table */
    struct am_lrnet_common_evt_cb common_cb;

    /** \brief Callback while received a fetch request */
    int (*pfn_fetch_request) (am_lrnet_fetch_request_t *p_request);
};

/** \brief LoRaNet Router Configuration */
typedef struct am_lrnet_router_cfg {

    /** \brief LoRaNet Network Configuration */
    const am_lrnet_net_cfg_t   *p_net_cfg;

    /** \brief Device Role Configuration */
    am_lrnet_role_common_cfg_t  role_cfg;

    /** \brief LoRaNet Router Event Callback Functions Table */
    struct am_lrnet_router_evt_cb evt_cb;
} am_lrnet_router_cfg_t;

/**
 * \brief       LoRaNet Router Initialize
 *
 * \param[in]   p_cfg       router configuration
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 * \note        DO NOT call this function repeatedly.
 */
int am_lrnet_router_init (const am_lrnet_router_cfg_t *p_cfg);


/*******************************************************************************
  LoRaNet Terminal Function Declaration
*******************************************************************************/

/** \brief LoRaNet Terminal Event Callback Functions Table */
struct am_lrnet_terminal_evt_cb {

    /** \brief Common Event Callback Fuctions Table */
    struct am_lrnet_common_evt_cb common_cb;

    /** \brief Callback while received a fetch request */
    int (*pfn_fetch_request) (am_lrnet_fetch_request_t *p_request);

    /** \brief Callback when it is time to send upstream packet */
    int (*pfn_upstream_request) (am_lrnet_upstream_request_t *p_request);

    /** \brief Callback while received a Exception ACK from the center */
    void (*pfn_exception_ack_rx) (void);

};

/** \brief LoRaNet Terminal Configuration */
typedef struct am_lrnet_terminal_cfg {

    /** \brief LoRaNet Network Configuration */
    const am_lrnet_net_cfg_t        *p_net_cfg;

    /** \brief Device Role Configuration */
    am_lrnet_role_common_cfg_t       role_cfg;

    /** \brief LoRaNet Terminal Event Callback Functions Table */
    struct am_lrnet_terminal_evt_cb  evt_cb;
} am_lrnet_terminal_cfg_t;

/**
 * \brief       LoRaNet Terminal Initialize
 * \param[in]   p_cfg       terminal configuration
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 * \note        DO NOT call this function repeatedly.
 */
int am_lrnet_terminal_init (const am_lrnet_terminal_cfg_t *p_cfg);

/**
 * \brief       LoRaNet Terminal Exception Report
 *
 * \param[in]   p_data      data to send
 * \param[in]   data_size   size of data
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_terminal_err_report (const uint8_t *p_data, uint8_t data_size);

/**
 * \brief       LoRaNet Asynchronous Terminal Exception Report
 *
 * \param[in]   p_data      data to send
 * \param[in]   data_size   size of data
 * \param[in]   pfn_finish  finish callback function
 *                          p_arg is the extra parameter, rc is the return code
 * \param[in]   p_arg       parameter for callback
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_terminal_err_report_asyn (
            const uint8_t *p_data,
            uint8_t        data_size,
            void         (*pfn_finish) (void *p_arg, int rc),
            void          *p_arg);

/**
 * \brief       LoRaNet Asynchronous Terminal Burst Report
 *
 * \param[in]   p_data      data to send
 * \param[in]   data_size   size of data
 * \param[in]   timeout     Burst Report timeout(ms)
 * \param[in]   pfn_finish  finish callback function
 *                          p_arg is the extra parameter, rc is the return code
 * \param[in]   p_arg       parameter for callback
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_terminal_burst_report_asyn (
            const uint8_t *p_data,
            uint8_t        data_size,
            uint32_t       timeout,
            void         (*pfn_finish) (void *p_arg, int rc),
            void          *p_arg);

/**
 * \brief       LoRaNet Terminal Burst Report
 *
 * \param[in]   p_data      data to send
 * \param[in]   data_size   size of data
 * \param[in]   timeout     Burst Report timeout(ms)
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_terminal_burst_report (const uint8_t *p_data,
                                    uint8_t        data_size,
                                    uint32_t       timeout);

/*******************************************************************************
  LoRaNet Raw Function Declaration
*******************************************************************************/

/**
 * \brief Raw Packet Structure
 */
typedef struct {
    const uint8_t *p_data;          /**< \brief data */
    uint16_t       data_size;       /**< \brief size of data */
    uint16_t       sender;          /**< \brief sender address */
    uint16_t       receiver;        /**< \brief receiver address */
    uint8_t        route_list_len;  /**< \brief route list length */
    uint8_t        route_list[4];   /**< \brief route list */
    int16_t        rssi;            /**< \brief RSSI of packet, unit: dBm */
    uint8_t        no_ack;          /**< \brief needn't acknowledge */
} am_lrnet_raw_packet_t;

/**
 * \brief LoRaNet Terminal Event Callback Functions Table
 */
struct am_lrnet_raw_evt_cb {

    /** \brief Common Event Callback Fuctions Table */
    struct am_lrnet_common_evt_cb common_cb;

    /** \brief Callback while packet received */
    void (*pfn_raw_rx) (am_lrnet_raw_packet_t *p_packet);
};

/**
 * \brief LoRaNet Raw Device Configuration
 */
typedef struct am_lrnet_raw_cfg {

    /** \brief LoRaNet Network Configuration */
    const am_lrnet_net_cfg_t   *p_net_cfg;

    /** \brief Device Role Configuration */
    am_lrnet_role_common_cfg_t  role_cfg;

    /** \brief LoRaNet Raw Event Callback Functions Table */
    struct am_lrnet_raw_evt_cb  evt_cb;
} am_lrnet_raw_cfg_t;

/**
 * \brief       LoRaNet Raw Initialize
 *
 * \param[in]   p_cfg       raw configuration
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 * \note        DO NOT call this function repeatedly.
 */
int am_lrnet_raw_init (const am_lrnet_raw_cfg_t *p_cfg);

/**
 * \brief       LoRaNet Raw TX
 *
 * \param[in]   p_packet    raw packet
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_raw_tx (const am_lrnet_raw_packet_t *p_packet);

/**
 * \brief      LoRaNet Raw Tx Asynchronously
 * \details    Send a raw packet asynchronously, function return directly,
 *             callback pfn_finish() function while the operation completed.
 *
 * \param[in]  p_packet     raw packet
 * \param[in]  pfn_finish   finish callback function
 *                          p_arg is the extra parameter, rc is the return code
 * \param[in]  p_arg        callback function extra parameter
 *
 * \return     AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_raw_tx_asyn (
              const am_lrnet_raw_packet_t *p_packet,
              void                       (*pfn_finish) (void *p_arg, int rc),
              void                        *p_arg);


/*******************************************************************************
  LoRaNet Common Function Declaration
*******************************************************************************/

/**
 * \brief       LoRaNet Set RF Configuration
 *
 * \param[in]   p_rf_cfg    RF configuration
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_rf_cfg_set (const am_lrnet_rf_cfg_t *p_rf_cfg);

/**
 * \brief       LoRaNet Set Device Address
 *
 * \param[in]   dev_addr    device address
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_dev_addr_set (uint16_t dev_addr);

/**
 * \brief       LoRaNet Get Device Address
 */
uint16_t am_lrnet_dev_addr_get (void);

/**
 * \brief       LoRaNet Set Work Mode
 *
 * \param[in]   mode        work mode
 *
 * \return      AM_LORA_RET_OK on success, negative errno code on fail.
 */
int am_lrnet_mode_set (am_lrnet_mode_t mode);

/**
 * \brief       LoRaNet Set Work Mode
 * \return      Current work mode of LoRaNet
 */
am_lrnet_mode_t am_lrnet_mode_get (void);

/**
 * \brief    LoRaNet get system tick
 * \note     Tick frequency is 1000Hz
 * \return   Current system 32bit tick count
 */
uint32_t am_lrnet_systick_get (void);

/**
 * \brief  Get the version string of LoRaNet
 */
const char* am_lrnet_version_get (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AM_LRNET_H */

/* end of file */
