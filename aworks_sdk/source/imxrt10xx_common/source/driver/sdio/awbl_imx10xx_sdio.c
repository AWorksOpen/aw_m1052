/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx SDIO driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-13  mex, first implementation
 * \endinternal
 */


/*******************************************************************************
  includes
*******************************************************************************/
#include "aworks.h"
#include "awbl_sdiobus.h"
#include "aw_clk.h"
#include "driver/sdio/awbl_imx10xx_sdio.h"
#include "aw_spinlock.h"
#include "aw_task.h"
#include "aw_cache.h"
#include "aw_assert.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_sem.h"
#include "aw_mem.h"
#include "aw_wdt.h"
#include "aw_int.h"
#include <string.h>


/*******************************************************************************
                      register
*******************************************************************************/

typedef struct imx10xx_sdio_regs {
    volatile uint32_t    dsaddr;      /* SDMA system address register */
    volatile uint32_t    blkattr;     /* Block attributes register */
    volatile uint32_t    cmdarg;      /* Command argument register */
    volatile uint32_t    xfertyp;     /* Transfer type register */
    volatile uint32_t    cmdrsp0;     /* Command response 0 register */
    volatile uint32_t    cmdrsp1;     /* Command response 1 register */
    volatile uint32_t    cmdrsp2;     /* Command response 2 register */
    volatile uint32_t    cmdrsp3;     /* Command response 3 register */
    volatile uint32_t    datport;     /* Buffer data port register */
    volatile uint32_t    prsstat;     /* Present state register */
    volatile uint32_t    proctl;      /* Protocol control register */
    volatile uint32_t    sysctl;      /* System Control Register */
    volatile uint32_t    irqstat;     /* Interrupt status register */
    volatile uint32_t    irqstaten;   /* Interrupt status enable register */
    volatile uint32_t    irqsigen;    /* Interrupt signal enable register */
    volatile uint32_t    autoc12err;  /* Auto CMD error status register */
    volatile uint32_t    hostcapblt;  /* Host controller capabilities register */
    volatile uint32_t    wml;         /* Watermark level register */
    volatile uint32_t    mixctrl;     /* For USDHC */
    volatile char        reserved1[4];/* reserved */
    volatile uint32_t    fevt;        /* Force event register */
    volatile uint32_t    admaes;      /* ADMA error status register */
    volatile uint32_t    adsaddr;     /* ADMA system address register */
    volatile char        reserved2[4];
    volatile uint32_t    dllctrl;
    volatile uint32_t    dllstat;
    volatile uint32_t    clktunectrlstatus;
    volatile char        reserved3[84];
    volatile uint32_t    vendorspec;
    volatile uint32_t    mmcboot;
    volatile uint32_t    vendorspec2;
    volatile uint32_t    tuning_ctrl;
} imx10xx_sdio_regs_t;

#define CONFIG_DMA_BUF_SIZE_DEFAULT         16 * 1024

#if CONFIG_DMA_BUF_SIZE_DEFAULT % 256 != 0
#error "CONFIG_DMA_BUF_SIZE_DEFAULT not fit cache line size"
#endif

/*******************************************************************************
                      macro define
*******************************************************************************/
#define __SDMMC_BLOCK_LENGTH        512        /* SD卡块长度                    */
#define __SDMMC_BLOCK_LENGTH_POWER2 9          /* 块长度的指数表示法*/

#define __DATABYTES_PER_OP        64
#define __DATABYTES_PER_OP_POWER2 6

#define __IMX6UL_SDIO_HOST_TO_INFO_DECL(p_info, p_host)          \
            awbl_imx10xx_sdio_dev_info_t *p_info =                \
                (awbl_imx10xx_sdio_dev_info_t *)AWBL_DEVINFO_GET( \
                    &(((struct awbl_sdio_host *)p_host)->super.super))

#define __IMX6UL_SDIO_HOST_TO_DEV_DECL(p_dev, p_host)            \
            awbl_imx10xx_sdio_dev_t *p_dev =                      \
                AW_CONTAINER_OF(p_host, awbl_imx10xx_sdio_dev_t, host)

#define __IMX6UL_SDIO_AWDEV_TO_INFO_DECL(p_info, p_awdev)        \
            awbl_imx10xx_sdio_dev_info_t *p_info =                \
                (awbl_imx10xx_sdio_dev_info_t *)AWBL_DEVINFO_GET(p_awdev)

#define __IMX6UL_SDIO_AWDEV_TO_DEV_DECL(p_dev, p_awdev)                \
            awbl_imx10xx_sdio_dev_t *p_dev =                            \
                AW_CONTAINER_OF(                                       \
                    AW_CONTAINER_OF(p_awdev, struct awbl_sdio_host, super), \
                        awbl_imx10xx_sdio_dev_t,                        \
                        host)

#define __IMX6UL_SDIO_DMA_BUF_SIZE 64                    /* FIFO缓冲区大小 */

#define __SDMMC_SYSCTL_RSTA               (0x01000000)

#define __SDMMC_SYSCTL_CKEN               (0x00000008)
#define __SDMMC_SYSCTL_CLOCK_MASK         (0x0000fff0)
#define __SDMMC_SYSCTL_TIMEOUT_MASK       (0x000f0000)

#define __SDMMC_PROCTL                    (0x0002e028)
#define __SDMMC_PROCTL_INIT               (0x08000020)
#define __SDMMC_PROCTL_DTW_4              (0x00000002)
#define __SDMMC_PROCTL_DTW_8              (0x00000004)

#define __SDMMC_PRSSTAT_DLA               (0x00000004)
#define __SDMMC_PRSSTAT_CICHB             (0x00000002)
#define __SDMMC_PRSSTAT_CIDHB             (0x00000001)

#define __SDMMC_VENDSPEC_FRC_SDCLK_ON     (0x00000100)
#define __SDMMC_VENDORSPEC_INIT           (0x20007809)
#define __SDMMC_VENDORSPEC_VSELECT        (0x00000002) /* Use 1.8V */

#define __SDMMC_WML_RD_WML_MAX            (0x80)
#define __SDMMC_WML_WR_WML_MAX            (0x80)
#define __SDMMC_WML_RD_WML_MAX_VAL        (0x0)
#define __SDMMC_WML_WR_WML_MAX_VAL        (0x0)
#define __SDMMC_WML_RD_WML_MASK           (0x7f)
#define __SDMMC_WML_WR_WML_MASK           (0x7f0000)

#define __SDMMC_XFERTYP                   (0x0002e00c)
#define __SDMMC_XFERTYP_CMD(x)            ((x & 0x3f) << 24)
#define __SDMMC_XFERTYP_CMDTYP_NORMAL     (0x0)
#define __SDMMC_XFERTYP_CMDTYP_SUSPEND    (0x00400000)
#define __SDMMC_XFERTYP_CMDTYP_RESUME     (0x00800000)
#define __SDMMC_XFERTYP_CMDTYP_ABORT      (0x00c00000)
#define __SDMMC_XFERTYP_DPSEL             (0x00200000)
#define __SDMMC_XFERTYP_CICEN             (0x00100000)
#define __SDMMC_XFERTYP_CCCEN             (0x00080000)
#define __SDMMC_XFERTYP_RSPTYP_NONE       (0)
#define __SDMMC_XFERTYP_RSPTYP_136        (0x00010000)
#define __SDMMC_XFERTYP_RSPTYP_48         (0x00020000)
#define __SDMMC_XFERTYP_RSPTYP_48_BUSY    (0x00030000)
#define __SDMMC_XFERTYP_MSBSEL            (0x00000020)
#define __SDMMC_XFERTYP_DTDSEL            (0x00000010)
#define __SDMMC_XFERTYP_DDREN             (0x00000008)
#define __SDMMC_XFERTYP_AC12EN            (0x00000004)
#define __SDMMC_XFERTYP_BCEN              (0x00000002)
#define __SDMMC_XFERTYP_DMAEN             (0x00000001)

#define __SDMMC_IRQSTAT                    0x0002e030
#define __SDMMC_IRQSTAT_DMAE              (0x10000000)
#define __SDMMC_IRQSTAT_AC12E             (0x01000000)
#define __SDMMC_IRQSTAT_DEBE              (0x00400000)
#define __SDMMC_IRQSTAT_DCE               (0x00200000)
#define __SDMMC_IRQSTAT_DTOE              (0x00100000)
#define __SDMMC_IRQSTAT_CIE               (0x00080000)
#define __SDMMC_IRQSTAT_CEBE              (0x00040000)
#define __SDMMC_IRQSTAT_CCE               (0x00020000)
#define __SDMMC_IRQSTAT_CTOE              (0x00010000)
#define __SDMMC_IRQSTAT_CINT              (0x00000100)
#define __SDMMC_IRQSTAT_CRM               (0x00000080)
#define __SDMMC_IRQSTAT_CINS              (0x00000040)
#define __SDMMC_IRQSTAT_BRR               (0x00000020)
#define __SDMMC_IRQSTAT_BWR               (0x00000010)
#define __SDMMC_IRQSTAT_DINT              (0x00000008)
#define __SDMMC_IRQSTAT_BGE               (0x00000004)
#define __SDMMC_IRQSTAT_TC                (0x00000002)
#define __SDMMC_IRQSTAT_CC                (0x00000001)


/*
* Return type defines
*/
#ifndef E_PASS
#define E_PASS    (0x00000000u)
#endif
#ifndef E_FAIL
#define E_FAIL    (0x00000001u)
#endif
#ifndef E_TIMEOUT
#define E_TIMEOUT (0x00000002u)
#endif


aw_local void __imx10xx_sdio_hw_init (struct awbl_sdio_host *p_host);

/*******************************************************************************
                                  function
*******************************************************************************/
static int pointer_is_align(const void *p,int align)
{
    uint32_t t = (uint32_t )p;
    if( (t%align) == 0) {
        return 1;
    }
    return 0;
}

/* 清除响应寄存器 */
aw_local void __clearresponse (imx10xx_sdio_regs_t *p_reg)
{
    writel(0x00, &p_reg->cmdrsp0);
    writel(0x00, &p_reg->cmdrsp1);
    writel(0x00, &p_reg->cmdrsp2);
    writel(0x00, &p_reg->cmdrsp3);
}

/* 发送命令 */
aw_local int32_t __sendcmd (struct awbl_sdio_host *p_host,
                            awbl_sdio_cmd_t  *p_cmd,
                            uint32_t          xfertyp)
{
    __IMX6UL_SDIO_HOST_TO_INFO_DECL(p_info, p_host);
    __IMX6UL_SDIO_HOST_TO_DEV_DECL(p_dev,p_host);
    imx10xx_sdio_regs_t *p_reg = (imx10xx_sdio_regs_t *)(p_info->regbase);

    uint32_t reg_val = 0;
    aw_semb_reset(p_dev->cc_sem_id, 0);
    /* 清除响应寄存器 */
    __clearresponse(p_reg);

    /* 打开响应完成中断,响应超时中断 和响应CRC错误*/
    reg_val = readl(&p_reg->irqstat);
    reg_val  |= __SDMMC_IRQSTAT_CC   |
                __SDMMC_IRQSTAT_CTOE;// |
//              __SDMMC_IRQSTAT_CCE  |
//              __SDMMC_IRQSTAT_CEBE;
    writel(reg_val, &p_reg->irqstat);

    reg_val = readl(&p_reg->irqstaten);
    reg_val |= __SDMMC_IRQSTAT_CC   |
               __SDMMC_IRQSTAT_CTOE;//  |
//             __SDMMC_IRQSTAT_CCE  |
//             __SDMMC_IRQSTAT_CEBE;
    writel(reg_val, &p_reg->irqstaten);

    reg_val = readl(&p_reg->irqsigen);
    reg_val |= __SDMMC_IRQSTAT_CC    |
               __SDMMC_IRQSTAT_CTOE;//   |
//             __SDMMC_IRQSTAT_CCE   |
//             __SDMMC_IRQSTAT_CEBE;
    writel(reg_val, &p_reg->irqsigen);


    /* 设置参数寄存器和发送命令 */
    writel(p_cmd->arg, &p_reg->cmdarg);

    reg_val = readl(&p_reg->mixctrl);
    writel((reg_val & 0xFFFFFF80) | (xfertyp & 0x7F), &p_reg->mixctrl);
    writel((xfertyp & 0xFFFF0000), &p_reg->xfertyp);

    /*等待响应完成*/
    aw_err_t retval = aw_semb_take(p_dev->cc_sem_id, 1000);

    /*关闭中断*/
    reg_val = readl(&p_reg->irqsigen);
    reg_val &= ~(__SDMMC_IRQSTAT_CC   |
                 __SDMMC_IRQSTAT_CTOE |
                 __SDMMC_IRQSTAT_CCE  |
                 __SDMMC_IRQSTAT_CEBE);
    writel(reg_val, &p_reg->irqsigen);


    /* 到这里我们得到软件超时 */
    if (retval != AW_OK) {
        AW_INFOF(("line%d  cmd%d :software timeout\n", __LINE__, p_cmd->cmd));
        return E_TIMEOUT;
    }

    /* 判断是否响应成功完成 */
    if (p_dev->sd_status & __SDMMC_IRQSTAT_CC) {
        if (p_cmd->rsp_type == AWBL_SDIO_RSP_R2) {
            p_cmd->p_rsp[0] =  readl(&p_reg->cmdrsp0) << 8;
            p_cmd->p_rsp[1] = (readl(&p_reg->cmdrsp0) >> 24) | (readl(&p_reg->cmdrsp1) << 8);
            p_cmd->p_rsp[2] = (readl(&p_reg->cmdrsp1) >> 24) | (readl(&p_reg->cmdrsp2) << 8);
            p_cmd->p_rsp[3] = (readl(&p_reg->cmdrsp2) >> 24) | (readl(&p_reg->cmdrsp3) << 8);
        } else {
            p_cmd->p_rsp[0] = readl(&p_reg->cmdrsp0);
        }
        return E_PASS;
    } else {          /* 没有成功，返回状态寄存器的值 */
//        AW_INFOF(("line%d: status = %x, cmd=%d\n",
//                __LINE__, p_dev->sd_status, p_cmd->cmd));
        return E_FAIL;
    }

}

aw_local int __esdhc_setup_data (struct awbl_sdio_host *p_host,
                                 awbl_sdio_cmd_t  *p_cmd)
{
    __IMX6UL_SDIO_HOST_TO_INFO_DECL(p_info, p_host);
    __IMX6UL_SDIO_HOST_TO_DEV_DECL(p_dev, p_host);

    imx10xx_sdio_regs_t *p_reg = (imx10xx_sdio_regs_t *)(p_info->regbase);
    uint32_t            wml_value = 0;
    uint32_t            buf_size;
    uint32_t            reg_val = 0;

    wml_value =  1;     /*  Watermark Level */
    buf_size = p_cmd->nblock * p_cmd->blk_size;

    if (buf_size > p_dev->dma_buf_len ) {
        volatile int            *p = NULL;
        aw_kprintf("sdio::The request size %d is too long\n",buf_size);
        *p = 0;
        while(1);
    }

    if (p_cmd->opt == AWBL_SDIO_RD_DATA) {
        if (wml_value > __SDMMC_WML_RD_WML_MAX) {
            wml_value = __SDMMC_WML_RD_WML_MAX_VAL;
        }

        reg_val = readl(&p_reg->wml);
        reg_val &= ~__SDMMC_WML_RD_WML_MASK;
        reg_val |= wml_value;
        writel(reg_val, &p_reg->wml);

        if (pointer_is_align(p_cmd->p_data,AW_CACHE_LINE_SIZE) ) {
            writel((uint32_t)p_cmd->p_data, &p_reg->dsaddr);
        } else {
            writel((uint32_t)p_dev->p_dma_buf, &p_reg->dsaddr);
        }

    } else {
        if (wml_value > __SDMMC_WML_WR_WML_MAX) {
            wml_value = __SDMMC_WML_WR_WML_MAX_VAL;
        }

        reg_val = readl(&p_reg->wml);
        reg_val &= ~__SDMMC_WML_WR_WML_MASK;
        reg_val |= wml_value << 16;
        writel(reg_val, &p_reg->wml);

        if (pointer_is_align(p_cmd->p_data,AW_CACHE_LINE_SIZE) ) {
            writel((uint32_t)p_cmd->p_data, &p_reg->dsaddr);
        } else {
            writel((uint32_t)p_dev->p_dma_buf, &p_reg->dsaddr);
        }
    }

    writel((p_cmd->nblock << 16 | p_cmd->blk_size), &p_reg->blkattr);

    return 0;
}

/* Return the __SDMMC_XFERTYP flags for a given command and data packet */
static uint32_t __esdhc_xfertyp (struct awbl_sdio_host *p_host,
                                 awbl_sdio_cmd_t  *p_cmd)
{
    uint32_t xfertyp = 0;

    if (p_cmd->p_data) {
        xfertyp |= __SDMMC_XFERTYP_DPSEL;
        xfertyp |= __SDMMC_XFERTYP_DMAEN;
        if (p_cmd->nblock > 1) {
            xfertyp |= __SDMMC_XFERTYP_MSBSEL;
            xfertyp |= __SDMMC_XFERTYP_BCEN;
        }

        if (p_cmd->opt & AWBL_SDIO_RD_DATA) {
            xfertyp |= __SDMMC_XFERTYP_DTDSEL;
        }
    }

    xfertyp |= __SDMMC_XFERTYP_CCCEN;
    switch (p_cmd->rsp_type) {
    case AWBL_SDIO_RSP_NONE:
        xfertyp |= __SDMMC_XFERTYP_RSPTYP_NONE;
        break;
    case AWBL_SDIO_RSP_R1B:
        xfertyp |= __SDMMC_XFERTYP_RSPTYP_48_BUSY;
        break;
    case AWBL_SDIO_RSP_R2:
        xfertyp |= __SDMMC_XFERTYP_RSPTYP_136;
        break;
    default:
        xfertyp |= __SDMMC_XFERTYP_RSPTYP_48;
        break;
    }
    return __SDMMC_XFERTYP_CMD(p_cmd->cmd) | xfertyp;
}

/******************************************************************************/
aw_local aw_err_t __imx10xx_sdio_send_cmd (
                                          struct awbl_sdio_host *p_host,
                                          awbl_sdio_cmd_t  *p_cmd
                                         )
{
    __IMX6UL_SDIO_HOST_TO_INFO_DECL(p_info, p_host);
    __IMX6UL_SDIO_HOST_TO_DEV_DECL(p_dev, p_host);
    imx10xx_sdio_regs_t *p_reg = (imx10xx_sdio_regs_t *)(p_info->regbase);

    aw_err_t          err = AW_OK;
    int32_t           res = 0;
    uint32_t          xfertyp = 0;

    aw_tick_t   start_ticks;
    aw_tick_t   end_ticks;
    uint32_t    timeout_ms = 0;
    uint32_t    reg_val = 0;

    /* clear irq status register */
    reg_val = readl(&p_reg->irqstat);
    reg_val |= 0xFFFFFFFF;
    writel(reg_val, &p_reg->irqstat);

    p_dev->sd_status = 0;
    start_ticks = aw_sys_tick_get();

    /* Wait for the bus to be idle */
    if (p_cmd->cmd == 0     ||
        p_cmd->cmd == 12    ||
        p_cmd->cmd == 13    ||
        p_cmd->cmd == 52) {
        do {
            end_ticks  = aw_sys_tick_get();
            timeout_ms = aw_ticks_to_ms(end_ticks - start_ticks);
        } while(((readl(&p_reg->prsstat) & __SDMMC_PRSSTAT_CICHB)) &&
               (timeout_ms < 3000));
    } else {
        do {
            end_ticks  = aw_sys_tick_get();
            timeout_ms = aw_ticks_to_ms(end_ticks - start_ticks);
        } while(((readl(&p_reg->prsstat) & __SDMMC_PRSSTAT_CICHB) ||
               (readl(&p_reg->prsstat) & __SDMMC_PRSSTAT_CIDHB)) &&
               (timeout_ms < 3000));
    }

    if (timeout_ms > 3000) {
        AW_INFOF(("file %s, line %d, timeout\n", __FILE__, __LINE__));
        return AW_ERROR;
    }

    start_ticks = aw_sys_tick_get();
    do {
        end_ticks  = aw_sys_tick_get();
        timeout_ms = aw_ticks_to_ms(end_ticks - start_ticks);
    } while((readl(&p_reg->prsstat) & __SDMMC_PRSSTAT_DLA) &&
            (timeout_ms < 3000));

    if (timeout_ms > 3000) {
        AW_INFOF(("file %s, line %d, timeout\n", __FILE__, __LINE__));
        return AW_ERROR;
    }
    aw_semb_reset(p_dev->tc_sem_id, 0);
    /* Set up for a data transfer if we have one */
    if (p_cmd->p_data) {
        if (pointer_is_align(p_cmd->p_data,AW_CACHE_LINE_SIZE) ) {
            aw_cache_flush(p_cmd->p_data, p_cmd->nblock*p_cmd->blk_size);
        }
        else {
            memcpy(p_dev->p_dma_buf,
                    p_cmd->p_data,p_cmd->nblock*p_cmd->blk_size);
            aw_cache_flush(p_dev->p_dma_buf, p_cmd->nblock*p_cmd->blk_size);
        }

        reg_val = readl(&p_reg->irqstat);
        reg_val |= __SDMMC_IRQSTAT_TC   |
                   __SDMMC_IRQSTAT_DMAE |
                   __SDMMC_IRQSTAT_DEBE |
                   __SDMMC_IRQSTAT_DCE  |
                   __SDMMC_IRQSTAT_DTOE;
        writel(reg_val, &p_reg->irqstat);

        reg_val = readl(&p_reg->irqstaten);
        reg_val |= __SDMMC_IRQSTAT_TC   |
                   __SDMMC_IRQSTAT_DMAE |
                   __SDMMC_IRQSTAT_DEBE |
                   __SDMMC_IRQSTAT_DCE  |
                   __SDMMC_IRQSTAT_DTOE;
        writel(reg_val, &p_reg->irqstaten);

        reg_val = readl(&p_reg->irqsigen);
        reg_val |= __SDMMC_IRQSTAT_TC   |
                   __SDMMC_IRQSTAT_DMAE |
                   __SDMMC_IRQSTAT_DEBE |
                   __SDMMC_IRQSTAT_DCE  |
                   __SDMMC_IRQSTAT_DTOE;
        writel(reg_val, &p_reg->irqsigen);

        err = __esdhc_setup_data(p_host, p_cmd);
        if(err) {
            return err;
        }
    }

    xfertyp = __esdhc_xfertyp(p_host, p_cmd);

    res = __sendcmd(p_host, p_cmd, xfertyp);
//    if ((res != E_PASS) && (p_cmd->cmd != 52)) {
    if (res != E_PASS) {
        if (p_cmd->cmd == 52) {
            p_cmd->cmd = 52;    //test
        } else {
            __imx10xx_sdio_hw_init(p_host);
            return AW_ERROR;
        }
    }

    /* Switch voltage to 1.8V if CMD11 succeeded */
    if (p_cmd->cmd == 11) {

        reg_val = readl(&p_reg->vendorspec);
        reg_val |= __SDMMC_VENDORSPEC_VSELECT;
        writel(reg_val, &p_reg->vendorspec);
    }

    if (p_cmd->p_data) {
        res = aw_semb_take(p_dev->tc_sem_id, 10000);
        reg_val = readl(&p_reg->irqsigen);
        reg_val &= ~(__SDMMC_IRQSTAT_TC   |
                     __SDMMC_IRQSTAT_DMAE |
                     __SDMMC_IRQSTAT_DEBE |
                     __SDMMC_IRQSTAT_DCE  |
                     __SDMMC_IRQSTAT_DTOE);
        writel(reg_val, &p_reg->irqsigen);

        if (res == -AW_ETIME) {
            AW_INFOF(("line %d: write complete timeout, status = %08x\n",
                    __LINE__, p_dev->sd_status));
            return AW_ERROR;
        }
        if (p_dev->sd_status & __SDMMC_IRQSTAT_TC) {
            if (pointer_is_align(p_cmd->p_data,AW_CACHE_LINE_SIZE) ) {
                aw_cache_invalidate(p_cmd->p_data,
                        p_cmd->blk_size*p_cmd->nblock);
            }
            else {
                aw_cache_invalidate(p_dev->p_dma_buf,
                        p_cmd->blk_size*p_cmd->nblock);
                memcpy(p_cmd->p_data,
                    p_dev->p_dma_buf,p_cmd->nblock*p_cmd->blk_size);
            }

            return AW_OK;
        } else {          /* 没有成功，返回状态寄存器的值 */
             AW_INFOF(("line%d: status = %d\n", __LINE__, p_dev->sd_status));
             return E_FAIL;
         }
    }
    return AW_OK;
}




/******************************************************************************/
aw_local void __sdio_trans_task_entry (void *p_arg)
{
    awbl_imx10xx_sdio_dev_t *p_dev  = (awbl_imx10xx_sdio_dev_t *)p_arg;
    struct awbl_sdio_host   *p_host = &(p_dev->host);
    awbl_sdio_msg_t         *p_msg  = NULL;
    awbl_sdio_cmd_t         *p_cmd  = NULL;
    aw_err_t                 err;

    AW_FOREVER {
        AW_SEMB_TAKE(p_dev->sem_sync, AW_SEM_WAIT_FOREVER);

        while ((p_msg = awbl_sdio_msg_out(p_host))) {
            /* update status */
            p_msg->status = -AW_EINPROGRESS;
            p_cmd         = NULL;

            /* handle all command */
            aw_list_for_each_entry(p_cmd,
                                   &p_msg->cmd_list,
                                   awbl_sdio_cmd_t,
                                   cmd_node) {
                /* do a command */
                do {
                    err = __imx10xx_sdio_send_cmd(p_host, p_cmd);

                } while ((err != AW_OK) &&
                         (p_cmd->retries--));

                if (err != AW_OK) {

                    /* error */
                    p_msg->status = err;
                    break;
                }
            }

            /* update status */
            if (p_msg->status == -AW_EINPROGRESS) {
                p_msg->status = AW_OK;
            }
            /* complete  */
            if (p_msg->pfn_complete) {
                p_msg->pfn_complete(p_msg->p_arg);
            }
        }
    }
}

/******************************************************************************/
aw_local aw_err_t __imx10xx_sdio_xfer (struct awbl_sdio_host *p_host)
{
    __IMX6UL_SDIO_HOST_TO_DEV_DECL(p_dev, p_host);

    if (p_host == NULL) {
        return -AW_EINVAL;
    }

    AW_SEMB_GIVE(p_dev->sem_sync);
    return AW_OK;
}

/******************************************************************************/
aw_local void __imx10xx_sdio_cancel (struct awbl_sdio_host *p_host,
                                    awbl_sdio_msg_t  *p_msg)
{

}

/* 设置传输速度，单位HZ */
aw_local uint32_t __imx10xx_sdio_set_speed (
                                          struct awbl_sdio_host *p_host,
                                          uint32_t          clock)
{
    __IMX6UL_SDIO_HOST_TO_INFO_DECL(p_info, p_host);
    imx10xx_sdio_regs_t *p_reg = (imx10xx_sdio_regs_t *)(p_info->regbase);
    int                 pre_div  = 0;
    int                 div  = 0;
    int                 sdhc_clk;
    uint32_t            reg_val = 0;

    sdhc_clk = aw_clk_rate_get(p_info->ref_clk_id);
    uint32_t clk = 0;
    
    if (sdhc_clk / 16 > clock) {
        for (pre_div = 1; pre_div <= 256; pre_div *= 2) {
            if ((sdhc_clk / pre_div) <= (clock * 16)) {
                break;
            }
        }
    } else {
        pre_div = 1;
    }

    for (div = 1; div <= 16; div++)
        if ((sdhc_clk / (div * pre_div)) <= clock)
            break;

    pre_div >>= 1;
    div -= 1;

    clk = (pre_div << 8) | (div << 4);

    reg_val = readl(&p_reg->vendorspec);

    /* 一直保持CLK活跃，只有这样，才能让SDIO卡(比如WIFI)产生SDIO同步卡中断 */
    reg_val |= __SDMMC_VENDSPEC_FRC_SDCLK_ON;
    writel(reg_val, &p_reg->vendorspec);

    reg_val = readl(&p_reg->sysctl);
    reg_val &= ~__SDMMC_SYSCTL_CLOCK_MASK;
    reg_val |= clk;
    writel(reg_val, &p_reg->sysctl);

    /* wait clock stable*/
    aw_udelay(100);
    return AW_OK;

}

/* 设置传输数据线宽度,1,2,4 */
aw_local aw_err_t __imx10xx_sdio_set_bus_width (
                                              struct awbl_sdio_host *p_host,
                                              awbl_sdio_bus_width_t  width)
{
    __IMX6UL_SDIO_HOST_TO_INFO_DECL(p_info, p_host);
    imx10xx_sdio_regs_t *p_reg = (imx10xx_sdio_regs_t *)(p_info->regbase);
    uint32_t proctl = 0;

    /* Set the bus width */
    proctl = readl(&p_reg->proctl);
    proctl &= ~(0x00000006);
    writel(proctl, &p_reg->proctl);

    if (width == 1) {

        proctl = readl(&p_reg->proctl);
        proctl |= __SDMMC_PROCTL_DTW_4;
        writel(proctl, &p_reg->proctl);
    }

    else if (width == 2){
        proctl = readl(&p_reg->proctl);
        proctl |= __SDMMC_PROCTL_DTW_8;
        writel(proctl, &p_reg->proctl);
    }


    return AW_OK;
}

aw_local aw_err_t __imx10xx_sdio_dummy_clks (
                                            struct awbl_sdio_host *p_host,
                                            uint8_t           nclks)
{
    return AW_OK;
}


/******************************************************************************/
aw_local void __imx10xx_sdio_hw_init (struct awbl_sdio_host *p_host)
{
    int cnt = 0;

    __IMX6UL_SDIO_HOST_TO_INFO_DECL(p_info, p_host);
    imx10xx_sdio_regs_t *p_regs = (imx10xx_sdio_regs_t *)(p_info->regbase);
    uint32_t reg_val = 0;
    
    /* Reset the entire host controller */
    reg_val = readl(&p_regs->sysctl);
    reg_val |= __SDMMC_SYSCTL_RSTA;
    writel(reg_val, &p_regs->sysctl);

    /* Wait until the controller is available */
    while ((readl(&p_regs->sysctl) & __SDMMC_SYSCTL_RSTA) && cnt < 1000){
        aw_udelay(1000);
        cnt++;
    }

    /* RSTA doesn't reset MMC_BOOT register, so manually reset it */
    writel(0x00, &p_regs->mmcboot);

    /* Reset MIX_CTRL and CLK_TUNE_CTRL_STATUS regs to 0 */
    writel(0x00, &p_regs->mixctrl);
    writel(0x00, &p_regs->clktunectrlstatus);

    /* Put VEND_SPEC to default value */
    writel(__SDMMC_VENDORSPEC_INIT, &p_regs->vendorspec);

    /* Put the PROCTL reg back to the default */
    writel(__SDMMC_PROCTL_INIT, &p_regs->proctl);

    /* Set timout to the maximum value */
    reg_val = readl(&p_regs->sysctl);
    reg_val &= ~__SDMMC_SYSCTL_TIMEOUT_MASK;
    reg_val |= (14 << 16);

    writel(reg_val, &p_regs->sysctl);
}


/* 使能SDIO卡中断 */
aw_local void __imx10xx_sdio_card_int_enable(awbl_imx10xx_sdio_dev_t *p_dev)
{

    __IMX6UL_SDIO_AWDEV_TO_INFO_DECL(p_devinfo, p_dev);
    imx10xx_sdio_regs_t *p_reg = (imx10xx_sdio_regs_t *)(p_devinfo->regbase);

    uint32_t reg_val = 0;

    reg_val = readl(&p_reg->irqstaten);
    reg_val |= __SDMMC_IRQSTAT_CINT;
    writel(reg_val, &p_reg->irqstaten);

    reg_val = readl(&p_reg->irqsigen);
    reg_val |= __SDMMC_IRQSTAT_CINT;
    writel(reg_val, &p_reg->irqsigen);

}


aw_local void __imx10xx_sdio_card_int_unmask(awbl_imx10xx_sdio_dev_t *p_dev)
{

    __IMX6UL_SDIO_AWDEV_TO_INFO_DECL(p_devinfo, p_dev);
    imx10xx_sdio_regs_t *p_reg = (imx10xx_sdio_regs_t *)(p_devinfo->regbase);

    uint32_t reg_val = 0;

    reg_val = readl(&p_reg->irqstaten);
    reg_val |= __SDMMC_IRQSTAT_CINT;
    writel(reg_val, &p_reg->irqstaten);
}


/*
 * MMCSD中断处理函数
 */
aw_local void __mmcsd_isr (void *p_arg)
{
    awbl_imx10xx_sdio_dev_t         *p_dev = (awbl_imx10xx_sdio_dev_t *)p_arg;
    __IMX6UL_SDIO_HOST_TO_INFO_DECL(p_info, &p_dev->host);
    uint32_t                        sd_status = 0;
    uint32_t                        reg_val = 0;

    imx10xx_sdio_regs_t *p_reg = (imx10xx_sdio_regs_t *)(p_info->regbase);

    sd_status = readl(&p_reg->irqstat);
    writel(sd_status, &p_reg->irqstat);

    p_dev->sd_status |= sd_status;


    if (sd_status & __SDMMC_IRQSTAT_CINT) {   /* 卡中断 */

        if (p_info->card_int_callback != NULL) {
            p_info->card_int_callback();
        }

        reg_val = readl(&p_reg->irqstaten);
        reg_val &= ~__SDMMC_IRQSTAT_CINT;
        writel(reg_val, &p_reg->irqstaten);
    }


    if (p_dev->sd_status & (__SDMMC_IRQSTAT_TC   |
                             __SDMMC_IRQSTAT_DMAE |
                             __SDMMC_IRQSTAT_DEBE |
                             __SDMMC_IRQSTAT_DCE  |
                             __SDMMC_IRQSTAT_DTOE)) {
        aw_semb_give(p_dev->tc_sem_id);
    }


    if (p_dev->sd_status & (__SDMMC_IRQSTAT_CC   |
                             __SDMMC_IRQSTAT_CTOE |
                             __SDMMC_IRQSTAT_CCE  |
                             __SDMMC_IRQSTAT_CEBE)) {

        aw_semb_give(p_dev->cc_sem_id);
    }

}


aw_local aw_err_t __imx10xx_sdio_get_state (struct awbl_sdio_host *p_host,
                                           uint8_t               *status)
{
    aw_err_t err = 0;
    __IMX6UL_SDIO_HOST_TO_INFO_DECL(p_info, p_host);
    __IMX6UL_SDIO_HOST_TO_DEV_DECL (p_dev, p_host);

    p_dev->cd_state = p_info->pfn_cd(p_dev) ?
            AWBL_SDIO_CD_PLUG : AWBL_SDIO_CD_STUBBS;

    *status = p_dev->cd_state;

    return err;
}

/**
 * \brief 探测函数，若复位失败则返回AW_FLASE
 */
aw_local aw_bool_t __imx10xx_sdio_probe(awbl_dev_t *p_dev)
{
    __IMX6UL_SDIO_AWDEV_TO_INFO_DECL(p_info, p_dev);
    imx10xx_sdio_regs_t *p_regs = (imx10xx_sdio_regs_t *)(p_info->regbase);
    uint32_t reg_val = 0;
    int cnt = 0;

    /* Reset the entire host controller */
    reg_val = readl(&p_regs->sysctl);
    reg_val |= __SDMMC_SYSCTL_RSTA;
    writel(reg_val, &p_regs->sysctl);

    /* Wait until the controller is available */
    do {
        if (!(readl(&p_regs->sysctl) & __SDMMC_SYSCTL_RSTA)) {
            return AW_TRUE;
        }
    } while(cnt++ < 1000);

    /* is's not a available device */
    return AW_FALSE;
}

/******************************************************************************/
aw_local void __imx10xx_sdio_inst_init1 (struct awbl_dev *p_awdev)
{
    return;
}


/******************************************************************************/
aw_local void __imx10xx_sdio_inst_init2 (struct awbl_dev *p_awdev)
{
    __IMX6UL_SDIO_AWDEV_TO_INFO_DECL(p_info, p_awdev);
    __IMX6UL_SDIO_AWDEV_TO_DEV_DECL(p_dev, p_awdev);

    /* 平台初始化 */
    if (p_info->pfunc_plfm_init != NULL) {
        p_info->pfunc_plfm_init();
    }
    // 分配内存
    p_dev->p_dma_buf = aw_cache_dma_align(CONFIG_DMA_BUF_SIZE_DEFAULT,
            AW_CACHE_LINE_SIZE);


    if (NULL == p_dev->p_dma_buf) {
        return;
    }
    p_dev->dma_buf_len = CONFIG_DMA_BUF_SIZE_DEFAULT;


    /* 硬件初始化 */
    __imx10xx_sdio_hw_init(&(p_dev->host));

    p_dev->tc_sem_id = AW_SEMB_INIT(p_dev->tc_sem, 0, AW_SEM_Q_PRIORITY);
    p_dev->cc_sem_id = AW_SEMB_INIT(p_dev->cc_sem, 0, AW_SEM_Q_PRIORITY);
    aw_int_connect(p_info->inum, __mmcsd_isr, p_dev);
    aw_int_enable(p_info->inum);

    /* 初始化驱动信号量 */
    AW_SEMB_INIT(p_dev->sem_sync, 0, AW_SEM_Q_FIFO);

    /* 初始化任务ssp_trans_task */
    AW_TASK_INIT(p_dev->imx10xx_sdio_trans_task,    /* 任务实体     */
                 "sdio_trans_task",                 /* 任务名字     */
                 p_info->task_trans_prio,           /* 任务优先级   */
                 __SDIO_SPI_TRANS_TASK_STACK_SIZE,  /* 任务堆栈大小 */
                 __sdio_trans_task_entry,           /* 任务入口函数 */
                 (void *)p_dev);                    /* 任务入口参数 */

    /* 启动任务task0 */
    AW_TASK_STARTUP(p_dev->imx10xx_sdio_trans_task);

    p_dev->sdio_card_int_enable = (void (*)(void *))__imx10xx_sdio_card_int_enable;
    p_dev->sdio_card_int_unmask = (void (*)(void *))__imx10xx_sdio_card_int_unmask;

    /* 创建SDIO总线控制器 */
    awbl_sdio_host_create(&(p_dev->host));

    return;
}

/******************************************************************************/
aw_local void __imx10xx_sdio_inst_connect (struct awbl_dev *p_awdev)
{
    return ;
}

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_imx10xx_sdio_drvfuncs = {
    __imx10xx_sdio_inst_init1,
    __imx10xx_sdio_inst_init2,
    __imx10xx_sdio_inst_connect
};


/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_sdio_host_drvinfo_t __g_imx10xx_sdio_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver         */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* bus_id          */
        AWBL_IMX10XX_SDIO_NAME,                 /* p_drvname       */
        &__g_imx10xx_sdio_drvfuncs,             /* p_busfuncs      */
        NULL,                                   /* p_methods       */
        __imx10xx_sdio_probe                    /* pfunc_drv_probe */
    },
    __imx10xx_sdio_xfer,
    __imx10xx_sdio_set_speed,
    __imx10xx_sdio_set_bus_width,
    __imx10xx_sdio_dummy_clks,
    __imx10xx_sdio_hw_init,
    __imx10xx_sdio_get_state,
};

/******************************************************************************/
void awbl_imx10xx_sdio_drv_register (void)
{
    aw_err_t err;
    err = awbl_drv_register((struct awbl_drvinfo *)&__g_imx10xx_sdio_drv_registration);
    err = (aw_err_t)err;
    return;
}

/* end of file */
