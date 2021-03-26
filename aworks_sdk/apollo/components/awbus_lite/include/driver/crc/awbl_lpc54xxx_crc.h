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

#ifndef AWBL_LPC54XXX_CRC_H_
#define AWBL_LPC54XXX_CRC_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/**
 * \file
 * \brief lpc54xxx  CRC ���� 
 *
 * ������ʵ���� lpc54xxx Ƭ��CRC ѭ������У�������    
 *
 * \par 1.������Ϣ
 *
 *  - ������:  "lpc54xxx_crc"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_lpc54xxx_crc_param
 *
 * \par 2.�����豸
 *
 *  - NXP LPC54XXX ϵ��MCU
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_drv_lpc54xxx_crc_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_crc
 *
 * \internal
 * \par modification history
 * - 1.00 14-10-28  tee, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "awbl_crc.h"

/** \brief lpc54xxx crc ������ */
#define AWBL_LPC54XXX_CRC_NAME   "lpc54xxx_crc"


/** \brief ģʽ���üĴ���λ���ܶ��� */
/** \brief ploy = 0x04C11DB7 */
#define CRC_MODE_POLY_CRC_32      (3)      
/** \brief poly = 0x8005     */   
#define CRC_MODE_POLY_CRC_16      (1)   
/** \brief poly = 0x1021     */      
#define CRC_MODE_POLY_CRC_CCITT   (0)         

/** \brief Bit order reverse for CRC_WR_DATA (per byte) */
#define CRC_MODE_BIT_RVS_WR       (1 << 2)    
/**
 * \brief Data 1's complement for CRC_WR_DATA 
 *        ���� = ԭ�룬���� = ����
 *        2's complment 2�Ĳ��� 
 *        ���� = ԭ�룬���� = ���� + 1
 */
#define CRC_MODE_CMPL_WR          (1 << 3)   
                                              
#define CRC_MODE_BIT_RVS_SUM      (1 << 4)    /**< \brief CRC sum bit order*/
#define CRC_MODE_CMPL_SUM         (1 << 5)    /**< \brief CRC sum complement*/

 
/** lpc54xxx �Ĵ����б� */
struct awbl_lpc54xxx_crc_reg {
    
    uint32_t mode;                /**< \brief CRC mode register     */
    uint32_t seed;                /**< \brief CRC seed register     */
    
    union {
        uint32_t sum;             /**< \brief CRC checksum register */
        uint32_t wr_data;         /**< \brief CRC data register     */
    }u;
};

 
/**
 * \brief lpc54xxx CRC �豸ʵ��
 */
struct awbl_lpc54xxx_crc_dev {
    
    /** \brief always go first */   
    awbl_dev_t  dev;                             /**< \brief AWBus device data */
    
    /** \brief CRC �������غ��� */
    const struct awbl_crc_functions *p_servfuncs;
    
    struct awbl_lpc54xxx_crc_reg  *reg;          /**< \brief register base */
    
    aw_bool_t   isallocated;                       
    
    /** \brief CRC info */
    aw_bool_t   isneedxorout;
    uint32_t    xorout;
    
};

struct awbl_lpc54xxx_crc_devinfo {
    
    uint32_t regbase;
    
    void (*pfunc_plfm_init) (void);     /**< \brief platform initializing   */
    
};

/**
 * \brief lpc54xxx CRC driver register
 */
void awbl_lpc54xxx_crc_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_lpc54xxx_RTC_H_ */
