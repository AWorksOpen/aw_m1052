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
 * \brief   AMetal Low Power Interface
 *
 * \internal
 * \par modification history:
 * - 2017-07-14 sky, ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LOWPOWER_H
#define __AM_LOWPOWER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup am_if_lora_lowpower
 * @copydoc am_lora_lowpower.h
 * @{
 */

/**
 * \brief entry of low power mode
 * \details System keep low power until wake up by lptmr, rtc, ex_pin and so on.
 * \note CANNOT used in a intterupt callback
 */
void am_lowpower_entry (void);

/**
 * \brief enum of lowpower wakeup pin trigger way
 */
typedef enum am_lowpower_wakeup_pin_trig {
    AM_LOWPOWER_WAKEUP_PIN_TRIGGER_DISABLE = 0, /**< \brief trigger disable */
    AM_LOWPOWER_WAKEUP_PIN_TRIGGER_RISE,        /**< \brief trigger by rising */
    AM_LOWPOWER_WAKEUP_PIN_TRIGGER_FALL,       /**< \brief trigger by falling */
} am_lowpower_wakeup_pin_trig_t;

/**
 * \brief config wakeup pin of low power mode
 * \details this pin can wake up system by rising or falling way.
 *
 * \param[in] pin  : which pin to config. now only supply PIOB_0, PIOC_1, 
 *                   PIOC_3, PIOC_4,PIOC_5, PIOC_6, PIOD_4, PIOD_6. and PIOC_5, 
 *                   PIOC_6 are reserved.
 * \param[in] trig : which way of trigger.
 *
 * \return 0 if config succeed, other means failed.
 */
int am_lowpower_wakeup_pin_config (int pin, am_lowpower_wakeup_pin_trig_t trig);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LOWPOWER_H */

/* end of file */
