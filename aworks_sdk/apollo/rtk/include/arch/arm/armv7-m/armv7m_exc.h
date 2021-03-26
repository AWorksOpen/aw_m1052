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
 * \brief ARMv7-M 异常处理
 *
 * 本模块为Cortex-M3(ARMv7-M架构)的异常模块实现。
 *
 * \par 特性
 *
 * - 实现AnyWhere标准异常接口，参考 \ref grp_aw_serv_exc
 * - 实现了对NMI、HardFalt、SVCall、PendSV、SysTick和EINT的异常管理，
 *      关于这些异常的定义和描述请参考 \ref grp_aw_plfm_arm_exc_excnums
 * - 本模块没有提供对Reset异常的管理
 * - 本模块将所有外部中断异常(IRQ,16号之后的异常)归纳为EINT异常，这些异常将由
 *   中断模块管理，请参考 \ref grp_aw_serv_int
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-04  zen, first implementation
 * \endinternal
 */

#ifndef __ARMV7M_EXC_H
#define __ARMV7M_EXC_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/**
 * \addtogroup grp_aw_plfm_arm_exc_excnums
 * @{
 */

/**
 * \name ARMv7-M 异常号定义
 * @{
 */

/**
 * \brief Reset 异常
 *
 * 在上电(power up)或热重启(warm reset)时将产生复位(Reset)。异常模型将复位看作
 * 一种特殊形式的异常。当复位生效时，处理停止运作，这可能发生在一条指令中的任何
 * 点。当复位失效时，将从向量表中复位入口(reset entry)提供的地址处重新开始执行。
 * 执行将在特权模式下重新开始。
 */
#define EXCNUM_RESET        1

/**
 * \brief NMI 异常
 *
 * NMI 异常可以由外设或软件触发。这是除复位之外的最高优先级异常。它被永久地使能
 * 并且拥有固定的优先级-2。NMI 不可以：
 *  - 被其它任何异常屏蔽或阻止
 *  - 被除复位外的其它任何异常抢占
 *
 * \attention 在部分CPU上，未实现本异常
 */
#define EXCNUM_NMI          2

/**
 * \brief HardFalt 异常
 *
 * HardFault 是在普通或异常处理中产生错误所导致的异常。HardFaults 拥有固定的
 * 优先级 -1，意味着它比其它任何优先级可配置的异常有着更高个优先级。
 */
#define EXCNUM_HARDFALT     3

/**
 * \brief MemManage 异常
 *
 * MemManage 错误是由内存保护相关错误所触发的异常。对于指令和数据内存传输，MPU
 * 或固定内存保护限制决定此异常。该错误总是被用来终止对永不执行(Execute Never)
 * 内存段的指令访问。
 */
#define EXCNUM_MEMMANAGE    4

/**
 * \brief BusFault 异常
 *
 * BusFault 是由指令或数据内存传输所导致的内存相关错误所触发的。该异常可能来自于
 * 内存系统所检查到的某条总线上的错误。
 */
#define EXCNUM_BUSFAULT     5

/**
 * \brief UsageFault 异常
 *
 * UsageFault 是由与指令执行相关的错误所导致的异常。这些错误包括：
 *  - 一条未定义的指令
 *  - 一次非法的不对其访问
 *  - 异常返回时产生的错误
 *
 * 下面的条件可能会导致 UsageFault 异常，如果内核被配置为报告这些错误的话：
 *  - 在一个未对齐的地址上进行字和半字内存访问
 *  - 被 0 除
 */
#define EXCNUM_USAGEFAULT   6

/**
 * \brief SvCall 异常
 *
 * 超级用户调用(SVC) 是由 SVC 指令触发的异常。在OS环境中，应用程序可以使用SVC
 * 指令来访问OS内核函数以及设备驱动。
 */
#define EXCNUM_SVCALL       11

/**
 * \brief PendSV 异常
 *
 * PendSV 是由中断驱动的系统级服务请求。在OS环境中，当没有其它异常活跃时，使用
 * PendSV 来进行上下文切换。
 */
#define EXCNUM_PENDSV       14

/**
 * \brief SysTick 异常
 *
 * SysTick 异常是由系统定时器计数值为0时所触发的异常。软件同样也可以产生 SysTick
 * 异常。在OS环境中，处理器使用该异常作为系统节拍。

 */
#define EXCNUM_SYSTICK      15

/**
 * \brief EINT 异常
 *
 * 16以上异常都归为 EINT 中断 (或称作IRQ中断)，该异常由外设或软件所触发。所有
 * 中断的指令执行都是异步的。在系统中，外设使用中断来与处理器通信。
 */
#define EXCNUM_EINT         16


/** @} */

/** @} grp_armv7m_exc_excnums */

#define EXCNUM_MAX          EXCNUM_EINT /**< 最大的异常号数值 */

/**
 * \addtogroup grp_aw_plfm_arm_exc_usrcfg
 * @{
 */

/**
 * \name 异常处理函数
 * @{
 *
 * 下列异常处理函数的入口地址需要被填写到对应的异常向量中
 * \attention 上层用户无需关心此操作
 */

/**
 * \brief NMI 异常处理函数
 */
void armv7m_exc_nmi_handler(void);

/**
 * \brief HardFalt 异常处理函数
 */
void armv7m_exc_hardfault_handler(void);

/**
 * \brief MemManage 异常处理函数
 */
void armv7m_exc_memmanage_handler(void);

/**
 * \brief BusFault 异常处理函数
 */
void armv7m_exc_busfault_handler(void);

/**
 * \brief UsageFault 异常处理函数
 */
void armv7m_exc_usagefault_handler(void);

/**
 * \brief SVCall 异常处理函数
 */
void armv7m_exc_svcall_handler(void);

/**
 * \brief pendSV 异常处理函数
 */
void armv7m_exc_pendsv_handler(void);

/**
 * \brief SysTick 异常处理函数
 */
void armv7m_exc_systick_handler(void);

/**
 * \brief EINT 异常处理函数
 */
void armv7m_exc_eint_handler(void);

/** @} */

/** @} grp_armv7m_exc_usrcfg */

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __ARMV7M_EXC_H */

/* end of file */
