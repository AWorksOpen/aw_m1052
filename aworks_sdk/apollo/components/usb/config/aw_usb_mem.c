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


#include "apollo.h"
#include "aw_assert.h"
#include "aw_cache.h"
#include "aw_memheap.h"
#include "aw_assert.h"
#include "aw_usb_mem.h"

#define __USB_MEM_DMA    0
#define __USB_MEM_CNT_EN 1

#include "aw_shell.h"
#include "aw_vdebug.h"

/******************************************************************************/

aw_local uint32_t                   __g_usb_mem_alloc_cnt = 0;
aw_local uint32_t                   __g_usb_mem_map_cnt = 0;

int awbl_usb_mem_sta (void)
{
    AW_INFOF(("allocate: %d \n", __g_usb_mem_alloc_cnt));
    AW_INFOF(("map: %d \n", __g_usb_mem_map_cnt));

    return AW_OK;
}




aw_local struct aw_memheap __g_usb_heap;

/****************************************************************************/
void aw_usb_mem_lib_init (void *p_mem, size_t size)
{
#if (!__USB_MEM_DMA)
    aw_err_t  ret;
    ret = aw_memheap_init(&__g_usb_heap,
                          "usb-heap",
                          p_mem,
                          size);
    aw_assert(ret == AW_OK);
#endif
}




/****************************************************************************/
void *aw_usb_mem_align_alloc (size_t size, size_t align)
{
    void   *p_ret = NULL;
#if __USB_MEM_DMA
    p_ret = aw_cache_dma_align(size, align);
#else
    void   *p_mem;

    align = AW_ROUND_UP(align, AW_CACHE_LINE_SIZE);
    size  = AW_ROUND_UP(size, AW_CACHE_LINE_SIZE);
    size  = size + align;

    p_mem = aw_memheap_alloc(&__g_usb_heap, size);
    if (p_mem != NULL) {
        if (AW_ALIGNED((uint32_t)p_mem, align)) {
            p_ret = (void *)((uint32_t)p_mem + align);
        } else {
            p_ret = (void *)(AW_ROUND_UP((uint32_t)p_mem, align));
        }
        *((uint32_t *)((uint32_t)p_ret - sizeof(void *))) = (uint32_t)p_mem;
    }
#endif

#if __USB_MEM_CNT_EN
    if (p_ret != NULL) {
        __g_usb_mem_alloc_cnt++;
    }
#endif
    return p_ret;

}




/****************************************************************************/
void *aw_usb_mem_alloc (size_t size)
{
#if __USB_MEM_DMA
    return aw_cache_dma_malloc(size);
#else
    return aw_usb_mem_align_alloc(size, AW_CACHE_LINE_SIZE);
#endif
}




/****************************************************************************/
void aw_usb_mem_free (void *ptr)
{
#if __USB_MEM_DMA
    aw_cache_dma_free(ptr);
#else
    void *p_real = NULL;

    if (ptr != NULL) {
        p_real = (void *)*(uint32_t *)((uint32_t)ptr - sizeof(void *));
#if __USB_MEM_CNT_EN
        __g_usb_mem_alloc_cnt--;
#endif
    }
    aw_memheap_free(p_real);
#endif
}




/****************************************************************************/
void *aw_usb_dma_map (void *p_mem, size_t size, uint8_t dir)
{
#if (!__USB_MEM_DMA)
    //todo check p_men align to AW_CACHE_LINE_SIZE

    switch (dir) {
        case AW_USB_DMA_TO_DEVICE:
            aw_cache_flush(p_mem, size);
            break;

        case AW_USB_DMA_FROM_DEVICE:
//            aw_cache_flush(p_mem, size);//some CPU use flush will error
            aw_cache_invalidate(p_mem, size);
            break;

        default:
            return NULL;
    }
#endif
#if __USB_MEM_CNT_EN
        __g_usb_mem_map_cnt++;
#endif

    return p_mem;
}




/****************************************************************************/
void *aw_usb_dma_unmap (void *p_dma, size_t size, uint8_t dir)
{
#if (!__USB_MEM_DMA)
    switch (dir) {
        case AW_USB_DMA_TO_DEVICE:
            //aw_cache_flush(p_dma, size);
            break;

        case AW_USB_DMA_FROM_DEVICE:
            aw_cache_invalidate(p_dma, size);
            break;

        default:
            return NULL;
    }
#endif

#if __USB_MEM_CNT_EN
        __g_usb_mem_map_cnt--;
#endif
    return p_dma;
}
