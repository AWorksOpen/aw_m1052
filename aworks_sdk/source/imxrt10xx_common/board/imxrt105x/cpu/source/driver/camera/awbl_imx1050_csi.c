/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#include "Aworks.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "awbl_plb.h"
#include "aw_int.h"
#include "aw_list.h"
#include "aw_cache.h"
#include "aw_errno.h"
#include "driver/camera/awbl_imx1050_csi.h"


#define __REG_MODIFY32_SET(addr, value) \
        do { \
        uint32_t val = readl((volatile void __iomem *)(addr)); \
        val |= value; \
        writel(val, (volatile void __iomem *)(addr)); \
    } while (0)

#define __REG_MODIFY32_CLR(addr, value) \
        do { \
        uint32_t val = readl((volatile void __iomem *)(addr)); \
        val &= value; \
        writel(val, (volatile void __iomem *)(addr)); \
    } while (0)


/* declare csi device instance */
#define __CSI_DEV_DECL(p_this, p_dev) \
    struct awbl_imx1050_csi_dev *p_this = \
        (struct awbl_imx1050_csi_dev *)(p_dev)

/* declare csi device infomation */
#define __CSI_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx1050_csi_devinfo *p_devinfo = \
        (struct awbl_imx1050_csi_devinfo *)AWBL_DEVINFO_GET(p_dev)


/* declare cap hardware information */
#define __CSI_HW_DECL(p_hw_csi, p_devinfo)    \
            awbl_imx1050_reg_t *p_hw_csi =            \
            (awbl_imx1050_reg_t *)(p_devinfo->regbase)


/** \brief CSI work mode.
 *
 *
 * The CCIR656 interlace mode is not supported currently.
 */
typedef enum _csi_work_mode
{
    CSI_GATED_ClOCK_MODE = CSI_CSICR1_GCLK_MODE(1U),       /**< \brief HSYNC, VSYNC, and PIXCLK signals are used. */
    CSI_NON_GATED_CLOCK_MODE = 0U,                          /**< \brief VSYNC, and PIXCLK signals are used. */
    CSI_CCIR656_PROG_RESSIVE_Mode = CSI_CSICR1_CCIR_EN(1U), /**< \brief CCIR656 progressive mode. */
} csi_work_mode_t;


/** CSI signal polarity. */
enum __csi_polarity_flags
{
    CSI_HSYNC_ACTIVE_LOW = 0U,                           /**< \brief HSYNC is active low. */
    CSI_HSYNC_ACTIVE_HIGH = CSI_CSICR1_HSYNC_POL_MASK,   /**< \brief HSYNC is active high. */
    CSI_DATA_LATCH_ON_RISING_EDGE = CSI_CSICR1_REDGE_MASK, /**< \brief Pixel data latched at rising edge of pixel clock. */
    CSI_Data_LATCH_ON_FALLING_EDGE = 0U,                   /**< \brief Pixel data latched at falling edge of pixel clock. */
    CSI_VSYNC_ACTIVE_HIGH = 0U,                       /**< \brief VSYNC is active high. */
    CSI_VSYNC_ACTIVE_LOW = CSI_CSICR1_SOF_POL_MASK,   /**< \brief VSYNC is active low. */
};

/** The CSI FIFO, used for FIFO operation. */
typedef enum __csi_fifo
{
    CSI_RXFIFO = (1U << 0U),   /**< \brief RXFIFO. */
    CSI_STATFIFO = (1U << 1U), /**< \brief STAT FIFO. */
    CSI_ALLFIFO = 0x01 | 0x02, /**< \brief Both RXFIFO and STAT FIFO. */
} csi_fifo_t;

typedef enum __csi_bool
{
    DISABLE,
    ENABLE,
}csi_bool_t;

#define __CSI_F1_FLAG  0x01
#define __CSI_F2_FLAG  0x02

aw_local void __buf_cache_flush(void* p_dev, uint32_t addr);
aw_local void __buf_cache_invalidate(void* p_dev, uint32_t addr);

aw_local int  __csi_fb_status(void *p_cookie, uint32_t csisr, uint32_t fb);

aw_local void __csi_restart(void *p_cookie, csi_fifo_t fifo);
aw_local void __csi_start(void *p_cookie, csi_fifo_t fifo);
aw_local void __csi_stop(void *p_cookie);

aw_local void __csi_fifo_refrash(void *p_cookie);
aw_local void __csi_reset(void *p_cookie);

aw_local void __csi_rxfifo_fill(void *p_cookie, uint8_t fb, void *p_buf);
aw_local void* __csi_rxfifo_get(void *p_cookie, uint8_t fb);
aw_local void __csi_statfifo_fill(void *p_cookie, void *p_buf);
aw_local void* __csi_statfifo_get(void *p_cookie);

aw_local aw_err_t __csi_init(void *p_cookie, awbl_camera_controller_cfg_info_t *p_cfg);
aw_local aw_err_t __csi_rxfifo_transfer(void *p_cookie);
aw_local aw_err_t __csi_statfifo_transfer(void *p_cookie);

aw_local aw_err_t __csi_get_buf(void *p_cookie, uint8_t **p_buf);
aw_local aw_err_t __csi_give_photo(void *p_cookie, void *p_buf);
aw_local aw_err_t __csi_get_photo(void *p_cookie, uint8_t **p_buf, aw_tick_t timeout);
aw_local aw_err_t __csi_release_buf(void *p_cookie, void *p_buf);


/******************************************************************************/

/**< \brief  初始化消息队列  */
aw_local aw_err_t __csi_msg_init(__csi_msg_t *msg)
{
    int i;

    if (msg == NULL) {
        return -AW_EINVAL;
    }

    AW_SEMC_INIT(msg->sem, 0, AW_SEM_Q_PRIORITY);
    AW_MUTEX_INIT(msg->lock, AW_SEM_Q_PRIORITY);

    for(i=0; i<__IMX1050_CSI_MSGQ_SIZE; i++) {
        msg->array[i].element = NULL;
        msg->array[i].next = &msg->array[i];
        msg->array[i].prev = &msg->array[i];
    }

    msg->list.next = &msg->list;
    msg->list.prev = &msg->list;
    msg->list.element = NULL;

    msg->count = 0;

    return AW_OK;
}

/**< \brief  注销消息队列  */
aw_local aw_err_t __csi_msg_deinit(__csi_msg_t *msg)
{
    int i;
    AW_MUTEX_LOCK(msg->lock, AW_SEM_WAIT_FOREVER);

    for(i=0; i<__IMX1050_CSI_MSGQ_SIZE; i++) {
        msg->array[i].element = NULL;
        msg->array[i].next = &msg->array[i];
        msg->array[i].prev = &msg->array[i];
    }

    msg->list.next = &msg->list;
    msg->list.prev = &msg->list;
    msg->list.element = NULL;

    AW_SEMC_TERMINATE(msg->sem);

    AW_MUTEX_UNLOCK(msg->lock);
    AW_MUTEX_TERMINATE(msg->lock);
    return AW_OK;
}

/**< \brief  获取一帧消息 */
aw_local aw_err_t __csi_msg_get(__csi_msg_t *msg, uint8_t** element, aw_tick_t tick)
{
    aw_err_t ret;

    if ((element) && (*element)) {
        *element = NULL;
    }

    ret = AW_SEMC_TAKE(msg->sem, tick);
    if (ret != AW_OK) {
        return ret;
    }

    ret = AW_MUTEX_LOCK(msg->lock, AW_SEM_NO_WAIT);
    if (ret != AW_OK) {
        return ret;
    }

    if (msg->list.next == &msg->list) {
        AW_MUTEX_UNLOCK(msg->lock);
        return -AW_ENOENT;
    }

    *element = msg->list.prev->element;

    msg->list.prev->element = NULL;

    msg->list.prev->prev->next = &msg->list;
    msg->list.prev = msg->list.prev->prev;

    msg->count--;

    AW_MUTEX_UNLOCK(msg->lock);


    return ret;
}

/**< \brief  发布一帧消息  */
aw_local aw_err_t __csi_msg_give(__csi_msg_t *msg, void* element)
{
    aw_err_t ret;
    int i;

    if (element == NULL) {
        return -AW_EINVAL;
    }

    ret = AW_SEMC_GIVE(msg->sem);
    if (ret != AW_OK) {
        return ret;
    }

    ret = AW_MUTEX_LOCK(msg->lock, AW_SEM_NO_WAIT);
    if (ret != AW_OK) {
        return ret;
    }

    for (i=0; i<__IMX1050_CSI_MSGQ_SIZE; i++) {
        if (msg->array[i].element == NULL){
            break;
        }
    }

    if (i == __IMX1050_CSI_MSGQ_SIZE) {
        AW_MUTEX_UNLOCK(msg->lock);
        return -AW_ENOMEM;
    }

    msg->array[i].element = element;
    msg->array[i].next = msg->list.next;
    msg->array[i].prev = &msg->list;

    msg->list.next->prev = &msg->array[i];
    msg->list.next = &msg->array[i];

    msg->count++;

    AW_MUTEX_UNLOCK(msg->lock);

    return ret;
}

/******************************************************************************/

/**< \brief   buffer的cahce刷新操作  */
aw_local void __buf_cache_flush(void* p_dev, uint32_t addr)
{
    __CSI_DEV_DECL(p_this, p_dev);
    uint32_t space =  VIDEO_EXTRACT_WIDTH(p_this->resolution)
                    * VIDEO_EXTRACT_HEIGHT(p_this->resolution)
                    * p_this->bytes_per_pix;

    aw_cache_flush((void*)addr, space);
}


/**< \brief   buffer的cahce无效操作  */
aw_local void __buf_cache_invalidate(void* p_dev, uint32_t addr)
{
    __CSI_DEV_DECL(p_this, p_dev);
    uint32_t space =  VIDEO_EXTRACT_WIDTH(p_this->resolution)
                    * VIDEO_EXTRACT_HEIGHT(p_this->resolution)
                    * p_this->bytes_per_pix;

    aw_cache_invalidate((void*)addr, space);
}
/******************************************************************************/

/**< \brief CSI中断回调  */
aw_local void __csi_isr (void* p_dev)
{
    __CSI_DEV_DECL(p_this, p_dev);
    __CSI_DEVINFO_DECL(p_devinfo, p_dev);

    aw_int_disable(p_devinfo->inum);
    AW_SEMB_GIVE(p_this->csi_sem);
}

aw_inline aw_local int __csi_fb_status(void *p_cookie, uint32_t csisr, uint32_t fb)
{
    if ((fb == 1) && (csisr & CSI_CSISR_DMA_TSF_DONE_FB2_MASK)) {
        return -1;
    } else if ((fb == 2) && (csisr & CSI_CSISR_DMA_TSF_DONE_FB1_MASK)) {
        return -1;
    }
    return 0;
}

/**< \brief  CCIR656的interlace模式中断处理函数  */
aw_local void __csi_ccir656_interlace_deal(void *p_cookie,
                                           uint8_t** p_next_buf,
                                           uint8_t **p_current_buf)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    __CSI_HW_DECL(p_hw_csi, p_devinfo);

    uint32_t dma_flag, fb_flag;
    uint32_t csisr = 0;
    uint32_t buf = 0;
    uint8_t *p_buf = NULL;

    csisr = readl(&p_hw_csi->CSISR);
    writel(csisr, &p_hw_csi->CSISR);

    /* CCIR656 interlace中断处理流程：
     *     启用SOF，COF中断使能，就能在Field启动接收的时候
     *     由F1_INT(Field 0)/F2_INT(Field 1)判断是哪个场（启动）
     *     然后下一次DONE_FB中断到达的时候，就可以知道哪个FB接收完成（结束）
     *     如果是Field 0启动，然后下一个中断是DONE_FB1(DMA)，则可判断Field 0对应FB1，此时认为数据有效
     *     (因为__csi_rxfifo_transfer的初始设置需要Field 0对应FB1)
     */
    fb_flag = csisr & (CSI_CSISR_F1_INT_MASK |\
                       CSI_CSISR_F2_INT_MASK);

    if (fb_flag == CSI_CSISR_F1_INT_MASK) {
        /* 当前为field 1数据 */
        p_this->field_record = __CSI_F1_FLAG;
    } else if ((fb_flag == CSI_CSISR_F2_INT_MASK) &&\
        (p_this->field_record = __CSI_F1_FLAG)) {
        /* 当前为field 2数据，Field 2 必须在Field 1之后接收 */
        p_this->field_record = __CSI_F2_FLAG;
    }

    dma_flag = csisr & (CSI_CSISR_DMA_TSF_DONE_FB2_MASK |\
                        CSI_CSISR_DMA_TSF_DONE_FB1_MASK );

    /* Field1 需要对应FB1 */
    if (p_this->current_mode == VIDEO) {
        if (dma_flag == CSI_CSISR_DMA_TSF_DONE_FB1_MASK) {
            if (p_this->field_record == __CSI_F1_FLAG) {
                //FB1的图像属于Field0，更替一个新的缓冲区（下一帧数据将存入新缓存的偶数行）
                __csi_get_buf(p_this, p_next_buf);
                if (*p_next_buf == NULL) {
                    return;
                }
                p_buf = *p_next_buf;
            }  else {
                /* 当前图像不属于Field0,重启传输  */
                __csi_restart(p_this, CSI_RXFIFO);
                p_this->field_record = 0;
                return;
            }

            //更换新的缓存
            buf = readl(&p_hw_csi->CSIDMASA_FB1);
            csisr = readl(&p_hw_csi->CSISR);
            if (__csi_fb_status(p_this, csisr, 1) == -1) {
                __csi_release_buf(p_this, p_buf);
                __csi_restart(p_this, CSI_RXFIFO);
                p_this->field_record = 0;
                return;
            }
            __csi_rxfifo_fill(p_this, 1, p_buf);

            //对于Field1，则记录下缓存的地址（等Field2获取完毕之后再发布完整的图像）
            *p_current_buf = (void*)buf;

        } else if (dma_flag == CSI_CSISR_DMA_TSF_DONE_FB2_MASK) {
            if (p_this->field_record == __CSI_F2_FLAG) {
                if ((*p_next_buf == NULL) && (*p_current_buf == NULL)) {
                    return;
                }
                //如果FB的图像属于Field1，则FB的起始地址将更替为新缓冲的第一行（下一帧数据将存入新缓存的奇数行）
                p_buf = *p_next_buf;
                p_buf += VIDEO_EXTRACT_WIDTH(p_this->resolution) * p_this->bytes_per_pix;
            } else {
                /* 重启传输  */
                __csi_restart(p_this, CSI_RXFIFO);
                p_this->field_record = 0;
                return;
            }

            //更换新的缓存
            buf = readl(&p_hw_csi->CSIDMASA_FB2);
            csisr = readl(&p_hw_csi->CSISR);
            if (__csi_fb_status(p_this, csisr, 2) == -1) {
                __csi_release_buf(p_this, p_buf);
                __csi_restart(p_this, CSI_RXFIFO);
                p_this->field_record = 0;
                return;
            }
            __csi_rxfifo_fill(p_this, 2, p_buf);

            //把完整的一帧缓存发布
            __csi_give_photo(p_this, *p_current_buf);
            *p_current_buf = NULL;

        } else if (dma_flag == (CSI_CSISR_DMA_TSF_DONE_FB1_MASK|\
                                CSI_CSISR_DMA_TSF_DONE_FB2_MASK)){
            /* 重启传输  */
            __csi_restart(p_this, CSI_RXFIFO);
            p_this->field_record = 0;
        }

    } else if (p_this->current_mode == PHOTO) {
        return;
    }
    return;
}

/**< \brief  常规中断处理函数  */
aw_local void __csi_normal_deal(void *p_cookie)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    __CSI_HW_DECL(p_hw_csi, p_devinfo);

    uint32_t flag;
    uint32_t csisr = 0;
    void    *buf = NULL;
    uint8_t *p_buf = NULL;

    uint8_t fb, r_fb;

    csisr = readl(&p_hw_csi->CSISR);
    writel(csisr, &p_hw_csi->CSISR);

    flag = csisr & (CSI_CSISR_DMA_TSF_DONE_FB2_MASK |\
                    CSI_CSISR_DMA_TSF_DONE_FB1_MASK);

    if (flag == (CSI_CSISR_DMA_TSF_DONE_FB2_MASK|\
                 CSI_CSISR_DMA_TSF_DONE_FB1_MASK)){
        /* 启动重传  */
        __csi_restart(p_this, CSI_RXFIFO);
        return ;
    } else if (flag == CSI_CSISR_DMA_TSF_DONE_FB1_MASK) {
        fb = 1; r_fb = 2;
    } else if (flag == CSI_CSISR_DMA_TSF_DONE_FB2_MASK){
        fb = 2; r_fb = 1;
    } else {
        return;
    }

    if (p_this->current_mode == VIDEO) {
        //获取新的缓冲区
        __csi_get_buf(p_this, &p_buf);
        if (p_buf == NULL) {
            return;
        }

        //获取FB的缓存地址
        buf = __csi_rxfifo_get(p_this, fb);

        //如果在运行中另一个FB已经完成一帧图像，则本FB的缓存可能已经“脏”了,重启传输
        csisr = readl(&p_hw_csi->CSISR);
        if (__csi_fb_status(p_this, csisr, fb) == -1) {
            __csi_release_buf(p_this, p_buf);
            __csi_restart(p_this, CSI_RXFIFO);
            return;
        }

        //将新缓冲区填入本FB，发布新的图像缓存
        __csi_rxfifo_fill(p_this, fb, p_buf);
        __csi_give_photo(p_this, buf);

    } else if (p_this->current_mode == PHOTO) {
        //获取FB的缓存地址并停下CSI传输
        buf = __csi_rxfifo_get(p_this, fb);
        __csi_stop(p_this);

        //如果在运行中另一个FB已经完成一帧图像，则本FB的缓存可能已经“脏”了,重启传输
        csisr = readl(&p_hw_csi->CSISR);
        if (__csi_fb_status(p_this, csisr, fb) == -1) {
            __csi_restart(p_this, CSI_RXFIFO);
            return;
        }

        //将图像缓存发出去，并清空FB
        __csi_give_photo(p_this, (void*)buf);
        __csi_rxfifo_fill(p_this, fb, NULL);

        //回收另一个FB的缓冲区
        buf = __csi_rxfifo_get(p_this, r_fb);
        __csi_release_buf(p_this, (void*)buf);
        __csi_rxfifo_fill(p_this, r_fb, NULL);

    }
    return;
}

/**< \brief CSI中断任务处理函数 */
aw_local void __csi_task_entry (void* p_dev)
{
    __CSI_DEV_DECL(p_this, p_dev);
    __CSI_DEVINFO_DECL(p_devinfo, p_dev);
    uint8_t *p_buf = NULL, *p_buf1 = NULL;

    AW_FOREVER {

        AW_SEMB_TAKE(p_this->csi_sem, AW_SEM_WAIT_FOREVER);

        AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

        if ((p_devinfo->transfer_mode == CCIR656) &&\
            (p_devinfo->ccir_mode == INTERLACE_MODE)) {
            /* CCIR656 interlace处理模式 */
            __csi_ccir656_interlace_deal(p_dev, &p_buf, &p_buf1);
        } else {
            /* 常规双缓存中断处理 */
            __csi_normal_deal(p_dev);
        }

        AW_MUTEX_UNLOCK(p_this->dev_lock);
        aw_int_enable(p_devinfo->inum);
    }
}

/******************************************************************************/

/**< \brief  重启传输  */
aw_local void __csi_restart(void *p_cookie, csi_fifo_t fifo)
{
    __csi_stop(p_cookie);
    __csi_fifo_refrash(p_cookie);
    __csi_start(p_cookie, fifo);
}

/**< \brief 启动CSI外设,fifo传输以及相关中断  */
aw_local void __csi_start(void *p_cookie, csi_fifo_t fifo)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    __CSI_HW_DECL(p_hw_csi, p_devinfo);
    uint32_t cr3, cr1 = 0;

    p_this->field_record  = 0;

    /* 使能中断 */
    if ((p_devinfo->transfer_mode == CCIR656) &&\
        (p_devinfo->ccir_mode == INTERLACE_MODE)) {
        cr1 |=  (CSI_CSICR1_SOF_INTEN_MASK|CSI_CSICR1_COF_INT_EN_MASK);
    }

    /* 使能FIFO接收 */
    cr3  = readl(&p_hw_csi->CSICR3);
    cr3 &= (~(CSI_CSICR3_DMA_REQ_EN_RFF_MASK|CSI_CSICR3_RxFF_LEVEL_MASK |\
              CSI_CSICR3_DMA_REQ_EN_SFF_MASK|CSI_CSICR3_STATFF_LEVEL_MASK));

    /* 设置RxFIFO及STATFIFO 的FULL level,并使能FIFO接收 */
    if (fifo & CSI_RXFIFO) {
        cr3 |= CSI_CSICR3_DMA_REQ_EN_RFF_MASK;
        cr3 |= p_this->full_level << CSI_CSICR3_RxFF_LEVEL_SHIFT;
        cr1 |= (CSI_CSICR1_FB2_DMA_DONE_INTEN_MASK| CSI_CSICR1_FB1_DMA_DONE_INTEN_MASK);
    }
    if (fifo & CSI_STATFIFO) {
        cr3 |= CSI_CSICR3_DMA_REQ_EN_SFF_MASK;
        cr3 |= p_this->full_level << CSI_CSICR3_STATFF_LEVEL_SHIFT;
        cr1 |= CSI_CSICR1_SFF_DMA_DONE_INTEN_MASK;
    }

    /* 配置CR3 */
    writel(cr3, &p_hw_csi->CSICR3);

    /* 使能中断 */
    __REG_MODIFY32_SET(&p_hw_csi->CSICR1, cr1);

    /* 使能CSI */
    __REG_MODIFY32_SET(&p_hw_csi->CSICR18, CSI_CSICR18_CSI_ENABLE_MASK);
}

/**< \brief 停止CSI外设功能 */
aw_local void __csi_stop(void *p_cookie)
{
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    __CSI_HW_DECL(p_hw_csi, p_devinfo);

    /* 禁能CSI */
    __REG_MODIFY32_CLR(&p_hw_csi->CSICR18, ~CSI_CSICR18_CSI_ENABLE_MASK);

    /* 禁能RxFIFO及STATFIFO */
    __REG_MODIFY32_CLR(&p_hw_csi->CSICR3,
                       ~(CSI_CSICR3_DMA_REQ_EN_RFF_MASK|CSI_CSICR3_DMA_REQ_EN_SFF_MASK));

    /* 禁能中断请求 */
    __REG_MODIFY32_CLR(&p_hw_csi->CSICR1, ~(CSI_CSICR1_FB2_DMA_DONE_INTEN_MASK|\
                                            CSI_CSICR1_FB1_DMA_DONE_INTEN_MASK|\
                                            CSI_CSICR1_SFF_DMA_DONE_INTEN_MASK));

    if ((p_devinfo->transfer_mode == CCIR656) &&\
        (p_devinfo->ccir_mode == INTERLACE_MODE)) {
        __REG_MODIFY32_CLR(&p_hw_csi->CSICR1, ~(CSI_CSICR1_SOF_INTEN_MASK|\
                                                CSI_CSICR1_COF_INT_EN_MASK));
    }
}

/**< \brief 清空fifo */
aw_local void __csi_fifo_refrash(void *p_cookie)
{
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    __CSI_HW_DECL(p_hw_csi, p_devinfo);
    uint32_t cr1 = 0;
    uint32_t cr1_mask = CSI_CSICR1_CLR_RXFIFO_MASK | CSI_CSICR1_CLR_STATFIFO_MASK;
    uint32_t cr3_mask = CSI_CSICR3_DMA_REFLASH_RFF_MASK | CSI_CSICR3_DMA_REFLASH_SFF_MASK;

    cr1 = readl(&p_hw_csi->CSICR1);

    /* 只有CSICR1[FCC] = 0才可以清空FIFO,清空完成后需置位 */
    cr1 &= ~CSI_CSICR1_FCC_MASK;
    writel(cr1, &p_hw_csi->CSICR1);

    /* 当FIFO清空完成，CLR位将被清空 */
    cr1 &= ~(CSI_CSICR1_CLR_RXFIFO_MASK|CSI_CSICR1_CLR_STATFIFO_MASK);
    writel((cr1|cr1_mask), &p_hw_csi->CSICR1);
    while (readl(&p_hw_csi->CSICR1) & cr1_mask);

    cr1 |= CSI_CSICR1_FCC_MASK;
    writel(cr1, &p_hw_csi->CSICR1);

    /* 当FIFO刷新完成，REFLASH位将被清空 */
    __REG_MODIFY32_SET(&p_hw_csi->CSICR3, cr3_mask);
    while (readl(&p_hw_csi->CSICR3) & cr3_mask);
}

/**< \brief 刷新（清空）fifo，重置相关寄存器 */
aw_local void __csi_reset(void *p_cookie)
{
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    __CSI_HW_DECL(p_hw_csi, p_devinfo);

    uint32_t csisr;

    /* Disable transfer first. */
    __csi_stop(p_cookie);

    /* Reset the fame count. */
    __REG_MODIFY32_SET(&p_hw_csi->CSICR3, CSI_CSICR3_FRMCNT_RST_MASK);
    while (readl(&p_hw_csi->CSICR3) & CSI_CSICR3_FRMCNT_RST_MASK);

    /* 刷新fifo */
    __csi_fifo_refrash(p_cookie);

    csisr = readl(&p_hw_csi->CSISR);
    writel(csisr, &p_hw_csi->CSISR);

    writel(0u, &p_hw_csi->CSICR2);
    writel(0u, &p_hw_csi->CSICR3);
    writel(0u, &p_hw_csi->CSIFBUF_PARA);
    writel(0u, &p_hw_csi->CSIIMAG_PARA);

    writel(0xFFu, &p_hw_csi->CSICR19);
    writel(CSI_CSICR18_AHB_HPROT(0x0DU) | 0x00020000, &p_hw_csi->CSICR18);
}

/**< \brief RxFIFO缓冲区填充  */
aw_local void __csi_rxfifo_fill(void *p_cookie, uint8_t fb, void *p_buf)
{
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    __CSI_HW_DECL(p_hw_csi, p_devinfo);

    if (fb == 1) {
        writel((uint32_t)p_buf, &p_hw_csi->CSIDMASA_FB1);
    } else if (fb == 2) {
        writel((uint32_t)p_buf, &p_hw_csi->CSIDMASA_FB2);
    }
}

/**< \brief RxFIFO缓冲区填充  */
aw_local void* __csi_rxfifo_get(void *p_cookie, uint8_t fb)
{
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    __CSI_HW_DECL(p_hw_csi, p_devinfo);
    uint32_t buf;

    if (fb == 1) {
        buf = readl(&p_hw_csi->CSIDMASA_FB1);
    } else if (fb == 2) {
        buf = readl(&p_hw_csi->CSIDMASA_FB2);
    }
    return (void*)buf;
}

/**< \brief STATFIFO缓冲区填充  */
aw_local void __csi_statfifo_fill(void *p_cookie, void *p_buf)
{
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    __CSI_HW_DECL(p_hw_csi, p_devinfo);

    writel((uint32_t)p_buf, &p_hw_csi->CSIDMASA_STATFIFO);
}

/**< \brief STATFIFO缓冲区填充  */
aw_local void* __csi_statfifo_get(void *p_cookie)
{
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    __CSI_HW_DECL(p_hw_csi, p_devinfo);

    return (void*)readl(&p_hw_csi->CSIDMASA_STATFIFO);
}

/**< \brief 适配csi相关参数  */
aw_local aw_err_t __csi_init(void *p_cookie, awbl_camera_controller_cfg_info_t *p_cfg)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    __CSI_HW_DECL(p_hw_csi, p_devinfo);
    uint32_t width_bytes = 0;
    uint32_t width,height;
    uint32_t tmp,reg;

    width  = VIDEO_EXTRACT_WIDTH(p_cfg->resolution);
    height = VIDEO_EXTRACT_HEIGHT(p_cfg->resolution);

    p_this->photo_flag    = 0;
    p_this->resolution    = p_cfg->resolution;
    p_this->current_mode  = p_cfg->current_mode;
    p_this->bytes_per_pix = p_cfg->bytes_per_pix;

    /* 行数据量需被8整除 */
    width_bytes = width * p_cfg->bytes_per_pix;
    if (width_bytes & 0x07) {
        return -AW_EINVAL;
    }

    /* 配置传输模式 */
    if (p_devinfo->transfer_mode == GATED_CLOCK) {
        reg = CSI_GATED_ClOCK_MODE;
        reg |= CSI_CSICR1_EXT_VSYNC_MASK;
    } else if (p_devinfo->transfer_mode == NONGATED_CLOCK) {
        reg = CSI_NON_GATED_CLOCK_MODE;
        reg |= CSI_CSICR1_EXT_VSYNC_MASK;
    } else if (p_devinfo->transfer_mode == CCIR656) {
        reg = CSI_CSICR1_CCIR_EN(1);
        reg |= CSI_CSICR1_EXT_VSYNC(p_devinfo->vsync);
        reg |= CSI_CSICR1_CCIR_MODE(p_devinfo->ccir_mode);
    }

    /* 配置时钟极性 */
    if (AW_CAM_HREF_ACTIVE_HIGH ==
            (p_cfg->control_flags & AW_CAM_HREF_ACTIVE_HIGH)) {
        reg |= CSI_HSYNC_ACTIVE_HIGH;
    }
    if (AW_CAM_DATA_LATCH_ON_RISING_EDGE
            == (p_cfg->control_flags & AW_CAM_DATA_LATCH_ON_RISING_EDGE)) {
        reg |= CSI_DATA_LATCH_ON_RISING_EDGE;
    }
    if (AW_CAM_VSYNC_ACTIVE_HIGH
            != (p_cfg->control_flags & AW_CAM_VSYNC_ACTIVE_HIGH)) {
        reg |= CSI_VSYNC_ACTIVE_LOW;
    }

    /* 使能FCC,并将寄存器的CSICR1配置值写入 */
    reg |= CSI_CSICR1_FCC_MASK;
    writel(reg, &p_hw_csi->CSICR1);

    /* DMA步进设置 */
    if ((p_devinfo->transfer_mode == CCIR656) &&\
        (p_devinfo->ccir_mode == INTERLACE_MODE)) {
        /* 接收的数据隔行放置 */
        writel((width_bytes/8)<<16, &p_hw_csi->CSIFBUF_PARA);
        height /= 2;
    } else {
        writel(0x00, &p_hw_csi->CSIFBUF_PARA);
    }

    /* 分辨率设置 */
    tmp = width_bytes;
    if (4U == p_cfg->bytes_per_pix)
    {
        /* Enable zero pack. */
        __REG_MODIFY32_SET(&p_hw_csi->CSICR3, CSI_CSICR3_ZERO_PACK_EN_MASK);
        tmp = width;
    }
    reg = ((tmp    << CSI_CSIIMAG_PARA_IMAGE_WIDTH_SHIFT) |\
           (height << CSI_CSIIMAG_PARA_IMAGE_HEIGHT_SHIFT));
    writel(reg, &p_hw_csi->CSIIMAG_PARA);
    reg = tmp * height / 4;
    writel(reg, &p_hw_csi->CSIDMATS_STATFIFO);

    /* DMA的FIFO相关配置 */
    if (!(width_bytes % (8 * 16))){
        /* 如果行宽的字节数是 8*16的整数倍，则DMA接收突发设置为 INC16 */
        /* 接收FIFO 的FULL level 设置为 16 double words */
        tmp = 3U;
    } else if (!(width_bytes % (8 * 8))) {
        /* 如果行宽的字节数是 8*8的整数倍，则DMA接收突发设置为 INC8 */
        /* 接收FIFO 的FULL level 设置为 8 double words */
        tmp = 2U;
    } else if (!(width_bytes % (8 * 4))) {
        /* 如果行宽的字节数是 8*4的整数倍，则DMA接收突发设置为 INC4 */
        /* 接收FIFO 的FULL level 设置为 4 double words */
        tmp = 1U;
    }

    /* DMA接收突发设置 */
    writel(CSI_CSICR2_DMA_BURST_TYPE_RFF(tmp)|\
           CSI_CSICR2_DMA_BURST_TYPE_SFF(tmp), &p_hw_csi->CSICR2);
    p_this->full_level = tmp-1;

    /* Enable auto ECC. */
    __REG_MODIFY32_SET(&p_hw_csi->CSICR3, CSI_CSICR3_ECC_AUTO_EN_MASK);

    if ((p_devinfo->transfer_mode == CCIR656) &&\
        (p_devinfo->ccir_mode == INTERLACE_MODE)) {
        /* 从第二个完整帧开始将数据写入内存. */
        writel((readl(&p_hw_csi->CSICR18)
                & ~CSI_CSICR18_MASK_OPTION_MASK)
                | CSI_CSICR18_MASK_OPTION(2)
                , &p_hw_csi->CSICR18);
    } else {
        /* 从第一个完整帧开始将数据写入内存. */
        writel((readl(&p_hw_csi->CSICR18)
                & ~CSI_CSICR18_MASK_OPTION_MASK)
                | CSI_CSICR18_MASK_OPTION(0)
                , &p_hw_csi->CSICR18);
    }

    return AW_OK;
}

/**< \brief csi传输启动（RxFIFO模式） */
aw_local aw_err_t __csi_rxfifo_transfer(void *p_cookie)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);

    uint8_t *addr1,*addr2;

    /* 设置双缓存的地址 */
    if ((p_devinfo->transfer_mode == CCIR656) &&\
        (p_devinfo->ccir_mode == INTERLACE_MODE)) {
        __csi_get_buf(p_this, &addr1);
        if (addr1 == NULL) {
            return -AW_ENOMEM;
        }
        addr2 = addr1 + VIDEO_EXTRACT_WIDTH(p_this->resolution) * p_this->bytes_per_pix;
    } else {
        __csi_get_buf(p_this, &addr1);
        __csi_get_buf(p_this, &addr2);
        if ((addr1 == NULL) || (addr2 == NULL)) {
            return -AW_ENOMEM;
        }
    }

    __csi_rxfifo_fill(p_this, 1, addr1);
    __csi_rxfifo_fill(p_this, 2, addr2);
    __csi_fifo_refrash(p_cookie);

    __csi_start(p_this, CSI_RXFIFO);
    aw_int_enable(p_devinfo->inum);

    return AW_OK;
}

/**< \brief csi传输启动（STATFIFO模式） */
aw_local aw_err_t __csi_statfifo_transfer(void *p_cookie)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    uint8_t *addr;
    int ret;

    ret = __csi_get_buf(p_this, &addr);
    if (ret!= AW_OK) {
        return -AW_ENOMEM;
    }
    __csi_statfifo_fill(p_this, addr);

    __csi_start(p_this, CSI_STATFIFO);
    aw_int_enable(p_devinfo->inum);

    return AW_OK;
}

/******************************************************************************/

/**< \brief  初始化CSI的缓冲队列  */
aw_local aw_err_t __csi_msgq_buf_set(void *p_cookie, awbl_camera_controller_cfg_info_t *p_cfg)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    int      i;
    uint8_t *p_buf;
    uint32_t width,height;

    width  = VIDEO_EXTRACT_WIDTH(p_cfg->resolution) * p_cfg->bytes_per_pix;
    height = VIDEO_EXTRACT_HEIGHT(p_cfg->resolution);

    /* 填充数据缓冲区 */
    if (p_cfg->current_mode == VIDEO) {
        if (p_cfg->p_video_buf_addr == NULL) {
            return -AW_EINVAL;
        }
        for(i=0; i<p_cfg->buf_num; i++) {
            p_buf = (uint8_t *)p_cfg->p_video_buf_addr + i * height * width;
            __csi_msg_give(&p_this->idle_msg, p_buf);
        }

    } else if (p_cfg->current_mode == PHOTO) {
        if (p_cfg->p_photo_buf_addr == NULL) {
            return -AW_EINVAL;
        }
        for(i=0; i<2; i++) {
            p_buf = (uint8_t *)p_cfg->p_photo_buf_addr + i * height * width;
            __csi_msg_give(&p_this->idle_msg, p_buf);
        }
    }

    __csi_rxfifo_fill(p_this, 1, NULL);
    __csi_rxfifo_fill(p_this, 2, NULL);
    __csi_statfifo_fill(p_this, NULL);

    return AW_OK;
}

/**< \brief  将CSI对象中的有效缓存转为空闲缓存  */
aw_local void __csi_msg_reset(void *p_cookie)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    int ret;
    void* addr;

    do{
        ret = __csi_msg_get(&p_this->photo_msg, (uint8_t**)&addr, AW_SEM_NO_WAIT);
        if (ret != AW_OK) {
            break;
        }
        __csi_msg_give(&p_this->idle_msg, addr);
    } while(1) ;

}

/**\brief   获取一帧缓存
 * \note    优先获取空闲（无效）的缓存，如果没有，则从旧的缓存取出一帧
 */
aw_local aw_err_t __csi_get_buf(void *p_cookie, uint8_t **p_buf)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    int ret;

    if ((p_buf==NULL) && (p_buf == NULL)){
        return -AW_EINVAL;
    }

    ret = __csi_msg_get(&p_this->idle_msg, p_buf, AW_MSGQ_NO_WAIT);
    if (ret == AW_OK) {
        return ret;
    }

    ret = __csi_msg_get(&p_this->photo_msg, p_buf, AW_MSGQ_NO_WAIT);
    if (ret == AW_OK) {
        return ret;
    }

    *p_buf = NULL;
    return -AW_ENOMEM;
}

/**< \brief  发布有效的图像缓存  */
aw_local aw_err_t __csi_give_photo(void *p_cookie, void *p_buf)
{
    __CSI_DEV_DECL(p_this, p_cookie);

    if (p_buf == NULL) {
        return -AW_ENOMEM;
    }

    /* buffer中的数据更新到cache中  */
    __buf_cache_invalidate(p_this, (uint32_t)p_buf);

    return __csi_msg_give(&p_this->photo_msg, p_buf);
}

/**< \brief  获取一帧图像缓存  */
aw_local aw_err_t __csi_get_photo(void *p_cookie, uint8_t **p_buf, aw_tick_t timeout)
{
    __CSI_DEV_DECL(p_this, p_cookie);

    return __csi_msg_get(&p_this->photo_msg, p_buf, aw_ms_to_ticks(3000));
}

/**< \brief  释放一帧无效的缓存至空闲缓存队列中  */
aw_local aw_err_t __csi_release_buf(void *p_cookie, void *p_buf)
{
    __CSI_DEV_DECL(p_this, p_cookie);

    return __csi_msg_give(&p_this->idle_msg, p_buf);
}

/******************************************************************************/

/******************************************************************************/
/**< \brief CSI驱动初始化步骤1 */
aw_local void __csi_inst_init1 (awbl_dev_t *p_dev)
{

}

/**< \brief CSI驱动初始化阶段2 */
aw_local void __csi_inst_init2(awbl_dev_t *p_dev)
{

}

/**< \brief CSI驱动初始化阶段3 */
aw_local void __csi_inst_connect(awbl_dev_t *p_dev)
{
    __CSI_DEV_DECL(p_this, p_dev);
    __CSI_DEVINFO_DECL(p_devinfo, p_dev);

    p_devinfo->pfunc_plfm_init();

    /* connect and enable isr */
    aw_int_connect(p_devinfo->inum,
                  (aw_pfuncvoid_t)__csi_isr,
                  (void *)p_dev);
    aw_int_enable(p_devinfo->inum);

    /* 初始化信号量及锁 */
    AW_MUTEX_INIT(p_this->dev_lock,  AW_PSP_SEM_Q_FIFO);
    AW_SEMB_INIT (p_this->csi_sem,   AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    /* 初始化任务sim_trans_task */
    AW_TASK_INIT(p_this->csi_task_decl,           /* 任务实体 */
                 "csi_task",                      /* 任务名字 */
                 p_devinfo->task_prio,            /* 任务优先级 */
                 __IMX1050_CSI_TASK_STACK_SIZE,    /* 任务堆栈大小 */
                 __csi_task_entry,                /* 任务入口函数 */
                 (void *)p_this);                 /* 任务入口参数 */

    /* 启动任务sim_trans_task */
    AW_TASK_STARTUP(p_this->csi_task_decl);
}

aw_local aw_err_t __camera_controller_open(void *p_cookie, awbl_camera_controller_cfg_info_t *p_cfg)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    int ret;

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    __csi_msg_init(&p_this->idle_msg);
    __csi_msg_init(&p_this->photo_msg);
    __csi_msgq_buf_set(p_this, p_cfg);

    __csi_reset(p_this);

    ret = __csi_init(p_this, p_cfg);
    if (ret != AW_OK) {
        AW_MUTEX_UNLOCK(p_this->dev_lock);
        return ret;
    }

    if (p_this->current_mode == VIDEO) {
        ret = __csi_rxfifo_transfer(p_this);
        if (ret != AW_OK) {
            AW_MUTEX_UNLOCK(p_this->dev_lock);
            return ret;
        }

    }

    AW_MUTEX_UNLOCK(p_this->dev_lock);

    return ret;
}

aw_local aw_err_t __camera_controller_close(void *p_cookie)
{
    __CSI_DEV_DECL(p_this, p_cookie);

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    __csi_stop(p_cookie);

    AW_SEMB_RESET(p_this->csi_sem, AW_SEM_EMPTY);

    __csi_msg_deinit(&p_this->idle_msg);
    __csi_msg_deinit(&p_this->photo_msg);

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return AW_OK;
}

aw_local aw_err_t __camera_controller_get_photo_addr(void *p_cookie, uint32_t **p_buf)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    __CSI_DEVINFO_DECL(p_devinfo, p_cookie);
    int ret;

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);
    if (p_this->photo_flag == 1) {
        AW_MUTEX_UNLOCK(p_this->dev_lock);
        return -AW_EBUSY;
    }
    p_this->photo_flag = 1;
    __csi_stop(p_this);

    if ((p_devinfo->transfer_mode == CCIR656) &&\
        (p_devinfo->ccir_mode == INTERLACE_MODE)) {
        AW_MUTEX_UNLOCK(p_this->dev_lock);
        return -AW_EPERM;
    }

    if (p_this->current_mode == PHOTO) {
        __csi_msg_reset(p_this);
        ret = __csi_rxfifo_transfer(p_this);
        if (ret != AW_OK) {
            AW_MUTEX_UNLOCK(p_this->dev_lock);
            return ret;
        }
    }

    AW_MUTEX_UNLOCK(p_this->dev_lock);

    return __csi_get_photo(p_this, (uint8_t**)p_buf, 3000);
}

aw_local aw_err_t __camera_controller_relase_photo_addr(void *p_cookie, uint32_t addr)
{
    /* 将cache中的数据刷新到内存中，防止dma在操作内存时，cpu又去操作内存，遭成cache不一致的问题 */
    __buf_cache_flush(p_cookie, addr);

    return __csi_release_buf(p_cookie, (void*)addr);
}

aw_local aw_err_t __camera_controller_get_video_addr(void *p_cookie, uint32_t **p_buf)
{
    int ret;
    ret = __csi_get_photo(p_cookie, (uint8_t**)p_buf, AW_SEM_WAIT_FOREVER);
    return ret;
}

aw_local aw_err_t __camera_controller_relase_video_addr(void *p_cookie, uint32_t addr)
{
    __CSI_DEV_DECL(p_this, p_cookie);
    int ret;
    /* 将cache中的数据刷新到内存中，防止dma在操作内存时，cpu又去操作内存，遭成cache不一致的问题 */
    __buf_cache_flush(p_cookie, addr);

    ret = __csi_release_buf(p_cookie, (void*)addr);
    p_this->photo_flag = 0;
    return ret;
}


/**
 * \brief CSI服务函数表
 */
aw_local aw_const struct awbl_camera_controller_servfuncs __g_camera_controller_servfuncs = {
        __camera_controller_open,
        __camera_controller_close,
        __camera_controller_get_photo_addr,
        __camera_controller_get_video_addr,
        __camera_controller_relase_photo_addr,
        __camera_controller_relase_video_addr,
};

/**
 * \brief 获取CSI服务的函数列表
 *
 * awbl_csiserv_get的运行时实现
 * \param   p_dev   AWbus传下来表征当前设备
 * \param   p_arg   用于输出CSI服务函数列表的指针
 * \return  无
 */
aw_local aw_err_t __csi_camera_controller_serv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __CSI_DEV_DECL(p_this, p_dev);
    __CSI_DEVINFO_DECL(p_devinfo, p_dev);

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);
    struct awbl_camera_controller_service   *p_serv = &p_this->camera_controller_serv;

    /* initialize the csi service instance */
    p_serv->p_next      = NULL;
    p_serv->id          = p_devinfo->camera_bus_id;
    p_serv->p_servfuncs = &__g_camera_controller_servfuncs;
    p_serv->p_cookie    = (void *)p_this;

    /* send back the service instance */
    *(struct awbl_camera_controller_service **)p_arg = p_serv;

    AW_MUTEX_UNLOCK(p_this->dev_lock);

    return AW_OK;
}


/**
 * \brief CSI驱动AWbus驱动函数
 */
aw_local aw_const struct awbl_drvfuncs __g_csi_drvfuncs = {
    __csi_inst_init1,
    __csi_inst_init2,
    __csi_inst_connect
};

/******************************************************************************/

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_cameracontrollerserv_get);


/**
 * \brief CSI驱动设备专有函数
 */
aw_local aw_const struct awbl_dev_method __g_csi_dev_methods[] = {
    AWBL_METHOD(awbl_cameracontrollerserv_get, __csi_camera_controller_serv_get),
    AWBL_METHOD_END
};


/**
 * \brief CSI驱动注册信息
 */
aw_local aw_const awbl_plb_drvinfo_t __g_csi_drv_registration = {
    {
        AWBL_VER_1,                 /* awb_ver */
        AWBL_BUSID_PLB,             /* bus_id */
        AWBL_IMX1050_CSI_NAME,      /* p_drvname */
        &__g_csi_drvfuncs,          /* p_busfuncs */
        &__g_csi_dev_methods[0],    /* p_methods */
        NULL                        /* pfunc_drv_probe */
    }
};


/******************************************************************************/
void awbl_imx1050_csi_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_csi_drv_registration);
}
/* end of file */
