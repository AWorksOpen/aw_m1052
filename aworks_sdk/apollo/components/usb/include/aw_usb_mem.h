/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/


#ifndef __AW_USB_MEM_H
#define __AW_USB_MEM_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_usb_mem
 * \copydoc aw_usb_mem.h
 * @{
 */

/** \brief USB DMA方向 */
#define AW_USB_DMA_TO_DEVICE    0x01
#define AW_USB_DMA_FROM_DEVICE  0x02


/**
 * \brief USB内存初始化接口
 *
 * \param[in] p_mem 需初始化内存地址
 * \param[in] size  内存字节大小
 *
 * \return 无
 */
void aw_usb_mem_lib_init (void *p_mem, size_t size);

/**
 * \brief 分配USB使用的对齐的内存
 *
 * \param[in] size  分配字节长度
 * \param[in] align 对齐字节长度
 *
 * \return 分配的内存起始地址 或 NULL
 */
void *aw_usb_mem_align_alloc (size_t size, size_t align);

/**
 * \brief 分配USB使用的内存
 *
 * \param[in] size  分配字节长度
 *
 * \return 分配的内存起始地址 或 NULL
 */
void *aw_usb_mem_alloc (size_t size);

/**
 * \brief 释放USB使用的内存
 *
 * \param[in] ptr 内存起始地址
 *
 * \return 无
 */
void aw_usb_mem_free (void *ptr);


/**
 * \brief 数据收发前map操作
 *
 * \param[in] p_mem 内存起始地址
 * \param[in] size  内存大小
 * \param[in] dir   数据传输方向
 *
 * \return 内存起始地址
 */
void *aw_usb_dma_map (void *p_mem, size_t size, uint8_t dir);


/**
 * \brief 数据收发完成后unmap操作
 *
 * \param[in] p_mem 内存起始地址
 * \param[in] size  内存大小
 * \param[in] dir   数据传输方向
 *
 * \return 内存起始地址
 */
void *aw_usb_dma_unmap (void *p_dma, size_t size, uint8_t dir);

/** @} grp_aw_if_usb_mem */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_USB_MEM_H */

/* end of file */
