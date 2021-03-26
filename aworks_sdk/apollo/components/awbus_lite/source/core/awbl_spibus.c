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
 * \brief AWBus SPI bus type implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 3.00 18-10-09  sls, update spi arch
 * - 2.00 14-03-07  zen, change: specify"devhcf_list"at initialization time
 *                  (for multicore-support reason )
 * - 1.00 12-11-20  zen, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "aw_list.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "awbus_lite.h"
#include "awbl_spibus.h"
#include "aw_int.h"
#include "aw_gpio.h"
#include "aw_psp_delay.h"
#include "aw_delay.h"

/*******************************************************************************
  import
*******************************************************************************/

/*******************************************************************************
 defines
*******************************************************************************/

#define __SPI_MST_DEVINFO_GET(p_dev) \
        ((struct awbl_spi_master_devinfo *)AWBL_DEVINFO_GET(p_dev))

#define __SPI_MST_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_spi_master_devinfo *p_devinfo = \
        (struct awbl_spi_master_devinfo *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
  type defines
*******************************************************************************/

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_bool_t awbl_spibus_devmatch(const struct awbl_drvinfo *p_drv,
                               struct awbl_dev           *p_dev);

/*******************************************************************************
  globals
*******************************************************************************/

/*******************************************************************************
  locals
*******************************************************************************/

/* bus type registration (must defined as aw_const)*/
aw_local aw_const struct awbl_bustype_info __g_spi_bustype = {
    AWBL_BUSID_SPI,         /**< \brief bus_id */
    NULL,                   /**< \brief pfunc_bustype_init1 */
    NULL,                   /**< \brief pfunc_bustype_init2 */
    NULL,                   /**< \brief pfunc_bustype_connect */
    NULL,                   /**< \brief pfunc_bustype_newdev_present */
    awbl_spibus_devmatch    /**< \brief pfunc_bustype_devmatch */
};


/*******************************************************************************
 * 注: 如果不调用异步接口，则与异步相关的全局变量将被忽略，从而节约RAM和ROM
 *
 ******************************************************************************/

/* head of spi masters list */
aw_local struct awbl_spi_master *__gp_spi_master_head;

/** \brief 异步传输任务  */
AW_TASK_DECL_STATIC(spi_async_task_decl, AWBL_SPI_ASYNC_TASK_STACK_SIZE);

/** \brief 异步传输所用信号量  */
AW_SEMB_DECL_STATIC(spi_sem_async);

/** \brief 防止操作被打断spibus_lock, 本文件中所有防止被打断的操作都用此互斥量 */
AW_MUTEX_DECL_STATIC(spibus_lock);

/* 异步消息链表 */
aw_local struct aw_list_head __async_msg_list_head;

aw_local aw_err_t __spi_sync (struct awbl_spi_master *p_master,
                              struct aw_spi_message  *p_msg);


aw_local aw_err_t __spi_cs_on( struct awbl_spi_master  *p_master,
                               aw_spi_device_t         *p_dev);

aw_local void __spi_cs_off( struct awbl_spi_master  *p_master,
                            aw_spi_device_t         *p_dev);

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief 异步消息插入
 */
aw_local void __awbl_spi_msg_in (struct aw_spi_message  *p_msg)
{
    aw_list_add_tail((struct aw_list_head *)(&p_msg->msg),
                     &__async_msg_list_head);
}


/**
 * \brief 异步消息取出
 */
aw_local struct aw_spi_message * __awbl_spi_msg_out (void)
{
    if (aw_list_empty_careful(&__async_msg_list_head)) {
        return NULL;
    } else {
        struct aw_list_head *p_node = __async_msg_list_head.next;
        aw_list_del(p_node);
        return aw_list_entry(p_node, struct aw_spi_message, msg);
    }
}


/**
 * \brief add an spi master to master list
 */
aw_local void __spi_master_insert (struct awbl_spi_master **pp_head,
                                   struct awbl_spi_master  *p_master)
{
    while (*pp_head != NULL) {
        pp_head = &(*pp_head)->p_next;
    }

    *pp_head = p_master;

     p_master->p_next = NULL;
}


/**
 * \brief 异步任务入口函数
 *
 * \note 异步实现是开启一个任务调用同步方式来实现的。
 */
aw_local void __spi_async_task_entry(void)
{

    struct awbl_spi_master *p_master  = NULL;
    struct aw_spi_message  *p_cur_msg = NULL;

    AW_FOREVER {
        AW_SEMB_TAKE(spi_sem_async, AW_SEM_WAIT_FOREVER);

        /* 发送Message */
        AW_FOREVER {

            AW_MUTEX_LOCK(spibus_lock, AW_SEM_WAIT_FOREVER);
            p_cur_msg = __awbl_spi_msg_out();
            AW_MUTEX_UNLOCK(spibus_lock);

            if (p_cur_msg == NULL) {

                /* 如果没有消息则跳出此循环  */
                break;
            } else {

                /* 获取当前message的master */
                p_master = p_cur_msg->p_master;

                /*
                 * 多任务中，异步Message队列中可能插入同步接口触发的__spi_sync
                 */
                __spi_sync(p_master, p_cur_msg);

                /* 异步模式不管成功与否都要执行回调函数   */
                if (p_cur_msg->pfunc_complete != NULL) {
                    p_cur_msg->pfunc_complete(p_cur_msg->p_arg);
                }
            }
        }
    }
}


/*
 * \brief trans参数有效性检测，用于在异步接口中检测Message中的trans 。
 */
aw_local aw_err_t __spi_check_trans (struct awbl_spi_master *p_master,
                                     struct aw_spi_message  *p_msg)
{
    struct aw_spi_transfer *p_cur_trans = NULL;
    struct awbl_spi_config tmp_config;
    uint32_t trans_cur_speed = 0;
    uint32_t trans_cur_bpw   = 0;
    int      ret             = 0;

    /* 获取message中的第1个trans  */
    p_cur_trans   = aw_list_first_entry(&p_msg->transfers,
                                        aw_spi_transfer_t,
                                        trans_node);

    while (1) {

        /* 如果发buffer和接收buffer都为空则返回-AW_EINVAL */
        if ((p_cur_trans->p_rxbuf == NULL) && (p_cur_trans->p_txbuf == NULL)) {
            return -AW_EINVAL;
        }

        /* p_cur_trans->speed_hz 和  p_cur_trans->bits_per_word 可能是0
         *
         * 如果是0则以message中的dev配置为准
         */
        if ((trans_cur_speed != p_cur_trans->speed_hz) ||
            (trans_cur_bpw != p_cur_trans->bits_per_word)) {

            AW_MUTEX_LOCK(p_master->dev_mutex, AW_SEM_WAIT_FOREVER);

            /* 每次配置都是取的最新的p_master_cur_config  */
            tmp_config.bpw = p_cur_trans->bits_per_word ?
                             p_cur_trans->bits_per_word :
                             p_master->cur_config.bpw;

            tmp_config.speed_hz = p_cur_trans->speed_hz ?
                                  p_cur_trans->speed_hz :
                                  p_master->cur_config.speed_hz;
            tmp_config.mode     = p_master->cur_config.mode;

            /* 配置  */
            ret = p_master->p_devinfo2->config(p_master, &tmp_config);

            if (ret) {
                AW_MUTEX_UNLOCK(p_master->dev_mutex);
                return ret;
            }

            p_master->cur_config = tmp_config;

            AW_MUTEX_UNLOCK(p_master->dev_mutex);

            /* 检测bpw, 和nbytes  */
            if (p_cur_trans->nbytes != 0) {
                   if (tmp_config.bpw > 17) {
                      if ( p_cur_trans->nbytes % 4 ) {
                          return  -AW_EINVAL;
                      }
                   } else if (tmp_config.bpw > 9) {
                       if ( p_cur_trans->nbytes % 2 ) {
                           return  -AW_EINVAL;
                       }
                   }
            } else {
               return  -AW_EINVAL;
            }
        }

        if (aw_list_is_last(&p_cur_trans->trans_node,
                                   &p_msg->transfers)) {
           return AW_OK;

        } else {

            /* 取出下一个 trans  */
            p_cur_trans =  aw_list_entry(p_cur_trans->trans_node.next,
                           aw_spi_transfer_t,
                           trans_node);

        }
    }
}


/**
 * \brief asynchronous SPI transfer
 */
aw_local aw_err_t __spi_async (struct awbl_spi_master *p_master,
                               struct aw_spi_message  *p_msg)
{
    int    ret = AW_OK;

    AW_MUTEX_LOCK(spibus_lock, AW_SEM_WAIT_FOREVER);

    if (!AW_TASK_VALID(spi_async_task_decl)) {
        aw_task_id_t task = NULL;
        AW_INIT_LIST_HEAD(&__async_msg_list_head);
        AW_SEMB_INIT(spi_sem_async, 0, AW_SEM_Q_FIFO);

        /* 确保信号量先初始化,不会被编译器给乱序  */
        aw_barrier();

        /* 初始化任务spi_trans_task */
        task = AW_TASK_INIT( spi_async_task_decl,              /* 任务实体 */
                             "spi_trans_task",                 /* 任务名字 */
                             AWBL_SPI_ASYNC_TASK_PRIO,         /* 任务优先级 */
                             AWBL_SPI_ASYNC_TASK_STACK_SIZE,        /* 任务堆栈大小 */
                             __spi_async_task_entry,           /* 任务入口函数 */
                             NULL);                            /* 任务入口参数 */

        if (task == NULL) {
            while(1);
        }

        /* 启动任务spi_trans_task,startup是个函数，不会交换 */
        AW_TASK_STARTUP(spi_async_task_decl);


        /* 等待任务有效  */
        while (!AW_TASK_VALID (spi_async_task_decl)) {
           /* 如果任务有效，则退出 */
           aw_mdelay(1);
        }
    }

    AW_MUTEX_UNLOCK(spibus_lock);

    /* message is not queueing */
    p_msg->status = -AW_ENOTCONN;
    p_msg->actual_length = 0;

#if 0

    /*
     * Half-duplex links include original MicroWire, and ones with
     * only one data pin like SPI_3WIRE (switches direction) or where
     * either MOSI or MISO is missing.  They can also be caused by
     * software limitations.
     */

     /* todo: */

#endif

    /* 异步时检测trans，同步不用检测  */
    ret = __spi_check_trans(p_master, p_msg);

    if (ret) {
        return ret;
    }

    AW_MUTEX_LOCK(spibus_lock, AW_SEM_WAIT_FOREVER);

    /* message is queueing */
    p_msg->status = -AW_ENOTCONN;
    p_msg->p_master = p_master;
    __awbl_spi_msg_in(p_msg);
    AW_MUTEX_UNLOCK(spibus_lock);

    /* 释放信号量  */
    AW_SEMB_GIVE(spi_sem_async);

    return AW_OK;
}


aw_local void __spi_cs_pin_set(int cs_pin, uint16_t mode, int state)
{
    /* judge chip select */
    if (mode & (AW_SPI_NO_CS | AW_SPI_AUTO_CS)) {
        return;
    }

    if (mode & AW_SPI_CS_HIGH) {
        aw_gpio_set(cs_pin, state);
    } else {
        aw_gpio_set(cs_pin, !state);
    }
}


/**
 * \brief synchronous SPI transfer
 */
aw_local aw_err_t __spi_sync (struct awbl_spi_master *p_master,
                              struct aw_spi_message  *p_msg)
{
    struct aw_spi_message  *p_cur_msg       = p_msg;
    struct aw_spi_transfer *p_cur_trans     = NULL;
    uint32_t                trans_cur_speed = 0;      /**< /brief 控制器当前速度 */
    uint8_t                 trans_cur_bpw   = 0;      /**< /brief 控制器当前帧大小 */
    int                     ret             = AW_OK;

    p_cur_msg->actual_length     = 0;
    p_cur_msg->status            = -AW_EINPROGRESS;

    /* 获取message中的第1个trans  */
    p_cur_trans   = aw_list_first_entry(&p_cur_msg->transfers,
                                        aw_spi_transfer_t,
                                        trans_node);

    if ((p_cur_trans->p_rxbuf == NULL) && (p_cur_trans->p_txbuf == NULL)) {
        return -AW_EINVAL;
    }

    /* 如果是同步任务的竟争则互斥信号阻塞 ,整个Message都应该被锁住 */
    AW_MUTEX_LOCK(p_master->dev_mutex, AW_SEM_WAIT_FOREVER);

    if ((p_master->cur_config.bpw != p_msg->p_spi_dev->bits_per_word) ||
        (p_master->cur_config.mode != p_msg->p_spi_dev->mode)         ||
        (p_master->cur_config.speed_hz != p_msg->p_spi_dev->max_speed_hz)) {

        struct awbl_spi_config   tem_config;
        tem_config.bpw = p_msg->p_spi_dev->bits_per_word;
        tem_config.mode = p_msg->p_spi_dev->mode;
        tem_config.speed_hz = p_msg->p_spi_dev->max_speed_hz;

        ret = p_master->p_devinfo2->config(p_master, &tem_config);

        if (ret) {
            goto __err;
        }

        p_master->cur_config = tem_config;
    }

    while (1) {

        /* p_cur_trans->speed_hz 和  p_cur_trans->bits_per_word 可能是0
         *
         * 如果是0则以message中的dev配置为准
         */
        if ((trans_cur_speed != p_cur_trans->speed_hz) ||
            (trans_cur_bpw != p_cur_trans->bits_per_word)) {

            struct awbl_spi_config   tem_config;

            /* 如果trans与Message的配置相同，则不用配置  */
            if ((p_cur_trans->speed_hz == p_master->cur_config.speed_hz) &&
                (p_cur_trans->bits_per_word == p_master->cur_config.bpw)) {

                goto __no_trans_cfg;
            }

            trans_cur_speed = p_cur_trans->speed_hz;
            trans_cur_bpw   = p_cur_trans->bits_per_word;

            tem_config.bpw      = p_cur_trans->bits_per_word ?
                                  p_cur_trans->bits_per_word :
                                  p_master->cur_config.bpw;
            tem_config.speed_hz = p_cur_trans->speed_hz ?
                                  p_cur_trans->speed_hz :
                                  p_master->cur_config.speed_hz;
            tem_config.mode     = p_master->cur_config.mode;

            ret = p_master->p_devinfo2->config(p_master, &tem_config);

            if (ret) {
                /* 配制失败  */
                goto __err;
            }

            /* 配置成功，更新cur_config  */
            p_master->cur_config = tem_config;
        }

__no_trans_cfg:
        /* 判断nbytes的有效性  */
        if (p_cur_trans->nbytes != 0) {
            if (p_master->cur_config.bpw > 17) {
               if ( p_cur_trans->nbytes % 4 ) {
                   ret = -AW_EINVAL;
                   goto __err;
               }
            } else if (p_master->cur_config.bpw > 9) {
                if ( p_cur_trans->nbytes % 2 ) {
                    ret = -AW_EINVAL;
                    goto __err;
                }
            }
        } else {
            ret = -AW_EINVAL;
            goto __err;
        }

        ret = __spi_cs_on(p_master, p_cur_msg->p_spi_dev);

        if (ret) {
            goto __err;
        }

        p_master->cs_toggle = 0;

        if (!(p_msg->p_spi_dev->mode & AW_SPI_AUTO_CS)) {
            ret = p_master->p_devinfo2->write_and_read(p_master,
                                                       p_cur_trans->p_txbuf,
                                                       p_cur_trans->p_rxbuf,
                                                       p_cur_trans->nbytes);
        } else {

            uint8_t end_of_cs_date = AWBL_SPI_END_OF_HARDCS_VALID;
            uint8_t cs_polarity    = ((p_msg->p_spi_dev->mode & AW_SPI_CS_HIGH) 
                                      ? 1 : 0);

            if (p_cur_trans->cs_change) {

                if (!aw_list_is_last(&p_cur_trans->trans_node,
                                    &p_cur_msg->transfers)) {

                    /* 消息结束后继续效片选信号  */
                    end_of_cs_date = AWBL_SPI_END_OF_HARDCS_INVALID;
                }
            } else {
                if (aw_list_is_last(&p_cur_trans->trans_node,
                                                    &p_cur_msg->transfers)) {
                    end_of_cs_date = AWBL_SPI_END_OF_HARDCS_INVALID;
                }
            }

            ret = p_master->p_devinfo2->write_and_read_hard_cs(p_master,
                                                       p_cur_trans->p_txbuf,
                                                       p_cur_trans->p_rxbuf,
                                                       p_cur_trans->nbytes,
                                                       p_msg->p_spi_dev->cs_pin,
                                                       cs_polarity,
                                                       end_of_cs_date);
        }

        if (ret) {
            goto __err;
        }

        /* trans 与 trans 之间的延迟  */
        if (p_cur_trans->delay_usecs) {
            aw_udelay(p_cur_trans->delay_usecs);
        }

        p_cur_msg->actual_length += p_cur_trans->nbytes;

        if (aw_list_is_last(&p_cur_trans->trans_node,
                            &p_cur_msg->transfers)) {

            /* cs_change 为真的情况  */
            if (p_cur_trans->cs_change) {
                p_master->cs_toggle = 1;
            }

            p_cur_trans = NULL;

            /* deal with cs toggole */
            if (!p_master->cs_toggle || (p_cur_msg->status != -AW_EINPROGRESS)) {
                __spi_cs_off(p_master, p_cur_msg->p_spi_dev);
            } else {
                if (p_cur_msg->p_spi_dev->pfunc_cs) {
                    p_master->tgl_pfunc_cs = p_cur_msg->p_spi_dev->pfunc_cs;

                    /* 外部提供片选函数  */
                    p_master->cs_tgl_pin = -1;
                } else {
                    p_master->cs_tgl_mode = p_cur_msg->p_spi_dev->mode;
                    p_master->cs_tgl_pin  = p_cur_msg->p_spi_dev->cs_pin;
                }
            }

            /* update message's status */
            if (p_cur_msg->status == -AW_EINPROGRESS) {
                p_cur_msg->status = AW_OK;
            }

            /* Message 发送完闭  */
            break;
        } else {

            /* cs_change 为真的情况  */
            if (p_cur_trans->cs_change) {

                __spi_cs_off(p_master, p_cur_msg->p_spi_dev);
            }

            /* 取出下一个 trans  */
            p_cur_trans =
            aw_list_entry(p_cur_trans->trans_node.next,
                          aw_spi_transfer_t,
                          trans_node);

            if ((p_cur_trans->p_rxbuf == NULL) && (p_cur_trans->p_txbuf == NULL)) {
                ret = -AW_EINVAL;
                goto __err;
            }
        }
    }

__err:
    AW_MUTEX_UNLOCK(p_master->dev_mutex);
    return ret;
}


/**
 * \brief write then read
 */
aw_local aw_err_t __spi_write_then_read (struct awbl_spi_master *p_master,
                                         aw_spi_device_t        *p_dev,
                                         const uint8_t          *p_txbuf,
                                         size_t                  n_tx,
                                         uint8_t                *p_rxbuf,
                                         size_t                  n_rx)
{
    int ret = AW_OK;
    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    if ((p_txbuf == NULL) || (p_rxbuf == NULL)) {
        return -AW_EINVAL;
    }

    if ((n_tx == 0) || (n_rx == 0) ) {
        return -AW_EINVAL;
    }

    /* 如果是等于0的话，证明已经被设置过  */
    if ((uint32_t)p_dev + p_dev->setuped) {

        ret = aw_spi_setup(p_dev);

        if (ret) {
            return ret;
        }
    }

    AW_MUTEX_LOCK(p_master->dev_mutex, AW_SEM_WAIT_FOREVER);

    if ((p_master->cur_config.bpw != p_dev->bits_per_word) ||
        (p_master->cur_config.mode != p_dev->mode)         ||
        (p_master->cur_config.speed_hz != p_dev->max_speed_hz)) {

        struct awbl_spi_config tmp_config;
        tmp_config.bpw      = p_dev->bits_per_word;
        tmp_config.mode     = p_dev->mode;
        tmp_config.speed_hz = p_dev->max_speed_hz;

        /* 会不会一个config都没有执行但是，上面的却是相等的 */
        ret = p_master->p_devinfo2->config(p_master, &tmp_config);

        if (ret) {
            goto __err;
        }

        p_master->cur_config = tmp_config;
    }

    ret = __spi_cs_on(p_master, p_dev);

    if (ret) {
        goto __err;
    }

    if (!(p_dev->mode & AW_SPI_AUTO_CS)) {

        /* 有没有必要判断返回值待定  */
        ret = p_master->p_devinfo2->write_and_read(p_master,
                                                   p_txbuf,
                                                   NULL,
                                                   n_tx);
        if (ret) {
            goto __err;
        }

        ret = p_master->p_devinfo2->write_and_read(p_master,
                                                   NULL,
                                                   p_rxbuf,
                                                   n_rx);
        if (ret) {
            goto __err;
        }
    } else {

        uint8_t cs_polarity    = ((p_dev->mode & AW_SPI_CS_HIGH) ?
                                   AWBL_SPI_HARDCS_HIGH_VALID    :
                                   AWBL_SPI_HARDCS_LOW_VALID);
        /* 有没有必要判断返回值待定  */
        ret = p_master->p_devinfo2->write_and_read_hard_cs(
                                        p_master,
                                        p_txbuf,
                                        NULL,
                                        n_tx,
                                        p_dev->cs_pin,
                                        cs_polarity,
                                        AWBL_SPI_END_OF_HARDCS_VALID);
        if (ret) {
            goto __err;
        }

        ret = p_master->p_devinfo2->write_and_read_hard_cs(
                                        p_master,
                                        NULL,
                                        p_rxbuf,
                                        n_rx,
                                        p_dev->cs_pin,
                                        cs_polarity,
                                        AWBL_SPI_END_OF_HARDCS_INVALID);

        if (ret) {
            goto __err;
        }
    }

    __spi_cs_off(p_master, p_dev);

__err:
    AW_MUTEX_UNLOCK(p_master->dev_mutex);

    return ret;
}


/**
 * \brief write then write
 */
aw_local aw_err_t __spi_write_then_write (struct awbl_spi_master *p_master,
                                          aw_spi_device_t        *p_dev,
                                          const uint8_t          *p_txbuf0,
                                          size_t                  n_tx0,
                                          const uint8_t          *p_txbuf1,
                                          size_t                  n_tx1)
{
    int ret = AW_OK;
    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    if ((p_txbuf0 == NULL) || (p_txbuf1 == NULL)) {
        return -AW_EINVAL;
    }

    if ((n_tx0 == 0) || (n_tx1 == 0) ) {
        return -AW_EINVAL;
    }

    /* 如果是等于0的话，证明已经被设置过  */
    if ((uint32_t)p_dev + p_dev->setuped) {

        ret = aw_spi_setup(p_dev);

        if (ret) {
            return ret;
        }
    }

    AW_MUTEX_LOCK(p_master->dev_mutex, AW_SEM_WAIT_FOREVER);

    if ((p_master->cur_config.bpw != p_dev->bits_per_word) ||
        (p_master->cur_config.mode != p_dev->mode)         ||
        (p_master->cur_config.speed_hz != p_dev->max_speed_hz)) {

        struct awbl_spi_config tmp_config;
        tmp_config.bpw      = p_dev->bits_per_word;
        tmp_config.mode     = p_dev->mode;
        tmp_config.speed_hz = p_dev->max_speed_hz;

        /* 会不会一个config都没有执行但是，上面的却是相等的 */
        ret = p_master->p_devinfo2->config(p_master, &tmp_config);

        if (ret) {
            goto __err;
        }

        /* 配置成功，更新cur_config  */
        p_master->cur_config = tmp_config;
    }

    ret = __spi_cs_on(p_master, p_dev);

    if (ret) {
        goto __err;
    }

    if (!(p_dev->mode & AW_SPI_AUTO_CS)) {

        /* 有没有必要判断返回值待定  */
        ret = p_master->p_devinfo2->write_and_read(p_master,
                                                   p_txbuf0,
                                                   NULL,
                                                   n_tx0);

        if (ret) {
            goto __err;
        }

        ret = p_master->p_devinfo2->write_and_read(p_master,
                                                   p_txbuf1,
                                                   NULL,
                                                   n_tx1);

        if (ret) {
            goto __err;
        }
    } else {

        uint8_t cs_polarity    = ((p_dev->mode & AW_SPI_CS_HIGH) ?
                                   AWBL_SPI_HARDCS_HIGH_VALID    :
                                   AWBL_SPI_HARDCS_LOW_VALID);
        /* 有没有必要判断返回值待定  */
        ret = p_master->p_devinfo2->write_and_read_hard_cs(
                                        p_master,
                                        p_txbuf0,
                                        NULL,
                                        n_tx0,
                                        p_dev->cs_pin,
                                        cs_polarity,
                                        AWBL_SPI_END_OF_HARDCS_VALID);
        if (ret) {
            goto __err;
        }

        ret = p_master->p_devinfo2->write_and_read_hard_cs(
                                        p_master,
                                        p_txbuf1,
                                        NULL,
                                        n_tx1,
                                        p_dev->cs_pin,
                                        cs_polarity,
                                        AWBL_SPI_END_OF_HARDCS_INVALID);
        if (ret) {
            goto __err;
        }
    }

    __spi_cs_off(p_master, p_dev);

__err:
    AW_MUTEX_UNLOCK(p_master->dev_mutex);
    return ret;
}


aw_local aw_err_t __spi_check_feature(struct awbl_spi_master *p_master,
                                      uint32_t flags)
{
    /** \brief 属性映射 */
    struct feature_map {
        uint16_t set;     /** < \brief 用户设置的属性  */
        uint16_t support; /** < \brief 驱动中支持的属性 */
    };

    int i = 0;
    const static struct feature_map feture_match_tab[] = {
            {AW_SPI_MODE_0 , AWBL_FEATURE_SPI_CPOL_L | AWBL_FEATURE_SPI_CPHA_U},
            {AW_SPI_MODE_1,  AWBL_FEATURE_SPI_CPOL_L | AWBL_FEATURE_SPI_CPHA_D},
            {AW_SPI_MODE_2,  AWBL_FEATURE_SPI_CPOL_H | AWBL_FEATURE_SPI_CPHA_U},
            {AW_SPI_MODE_3,  AWBL_FEATURE_SPI_CPOL_H | AWBL_FEATURE_SPI_CPHA_D},
            {AW_SPI_LSB_FIRST, AWBL_FEATURE_SPI_LSB_FIRST                     },
            {AW_SPI_LOOP,      AWBL_FEATURE_SPI_LOOP},
            {AW_SPI_3WIRE,     AWBL_FEATURE_SPI_3WIRE},
            {AW_SPI_READ_MOSI_HIGH, AWBL_FEATURE_SPI_READ_MOSI_HIGH},
            {AW_SPI_AUTO_CS, AWBL_FEATURE_SPI_AUTO_CS},
    };

    for (i = 0; i < sizeof(feture_match_tab) / sizeof(struct feature_map); 
         i++) {
        if ((flags & feture_match_tab[i].set) == feture_match_tab[i].set) {
            if ( !(p_master->p_devinfo2->features & 
                 feture_match_tab[i].support)) {

                /* 只要有一个不支持的就返回错误  */
                return -AW_ENOTSUP;
            }
        }
    }

    return AW_OK;
}


/**
 * \brief setup spi device
 *
 * \note setup不能同时在多个任务中调用（在片选相同的情况下）
 */
aw_local aw_err_t __spi_setup (struct awbl_spi_master *p_master,
                               aw_spi_device_t        *p_dev)
{
    int       ret = AW_OK;

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    /*
     *  help drivers fail *cleanly* when they need options
     *  that aren't supported with their current master
     */
    ret = __spi_check_feature(p_master, p_dev->mode);

    if (ret != AW_OK) {
        return ret;
    }

    /* defualt bits_per_word set */
    if (!p_dev->bits_per_word) {
        p_dev->bits_per_word = 8;
    }

    /* no cs 和 auto cs 同时存在反回错误  */
    if ((p_dev->mode & (AW_SPI_NO_CS | AW_SPI_AUTO_CS))
                == (AW_SPI_NO_CS | AW_SPI_AUTO_CS)) {
        /* 不支持同时设置NO CS 和 硬件CS */
        return -AW_ENOTSUP;

    /* have chip select control */
    }

    /* no chip select control */
    if (p_dev->mode & (AW_SPI_NO_CS | AW_SPI_AUTO_CS)) {

       p_dev->pfunc_cs = NULL;

    /* have chip select control */
    } else {

       /* device didn't provide cs function, use default */
       if (p_dev->pfunc_cs == NULL) {

           /* GPIO初始状态设置  */
           if (p_dev->mode & AW_SPI_CS_HIGH) {

               /* set chip select pin function to GPIO OUTPUT */
               aw_gpio_pin_cfg(p_dev->cs_pin, AW_GPIO_OUTPUT_INIT_LOW);
           } else {
               aw_gpio_pin_cfg(p_dev->cs_pin, AW_GPIO_OUTPUT_INIT_HIGH);
           }
       }
    }

    AW_MUTEX_LOCK(p_master->dev_mutex, AW_SEM_WAIT_FOREVER);
    ret =  p_master->p_devinfo2->pfunc_setup(p_master, p_dev);
    AW_MUTEX_UNLOCK(p_master->dev_mutex);

    if (ret) {
        return ret;
    }

    /* 设置成功  */
    p_dev->setuped = -(uint32_t)p_dev;

    return AW_OK;
}


/**
 * \brief find out controller by master id
 */
aw_local struct awbl_spi_master *__spi_master_find_by_id (int masterid)
{
    struct awbl_spi_master *p_master_cur = __gp_spi_master_head;

    while (p_master_cur != NULL) {

        if (masterid ==
           ((struct awbl_spi_master_devinfo *)
             AWBL_DEVINFO_GET(p_master_cur))->bus_index) {

            return p_master_cur;
        }

        p_master_cur = p_master_cur->p_next;
    }

    return NULL;
}


/******************************************************************************/
void awbl_spibus_init (void)
{
    AW_MUTEX_INIT(spibus_lock, AW_SEM_Q_PRIORITY);

    /* register SPI as a valid bus type */
    awbl_bustype_register(&__g_spi_bustype);
}


/******************************************************************************/
aw_bool_t awbl_spibus_devmatch (const struct awbl_drvinfo *p_drv,
                                struct awbl_dev           *p_dev)
{
    /* check the bus type */

    if (p_dev->p_devhcf->bus_type != AWBL_BUSID_SPI) {
        return AW_FALSE;
    }

    /* check name */

    if (strcmp(p_dev->p_devhcf->p_name, p_drv->p_drvname) != 0) {
        return AW_FALSE;
    }

    return AW_TRUE;
}


/******************************************************************************/
aw_err_t awbl_spibus_devenum (struct awbl_spi_master *p_master)
{
    int                 i;
    struct awbl_dev    *p_dev               = NULL;
    const struct awbl_devhcf *p_devhcf_list = awbl_devhcf_list_get();
    size_t              devhcf_list_count   = awbl_devhcf_list_count_get();

    __SPI_MST_DEVINFO_DECL(p_mdinfo, p_master);


    /* search for the devices on SPI bus from HCF list */

    for (i = 0; i < devhcf_list_count; i++) {

        /* the device is on this SPI bus */

        if ((p_devhcf_list[i].bus_type  == AWBL_BUSID_SPI) &&
            (p_devhcf_list[i].bus_index == p_mdinfo->bus_index)) {

            p_dev = p_devhcf_list[i].p_dev;
            if (p_dev == NULL) {
                return -AW_ENOMEM;     /* the device has no instance memeory */
            }

            /* dev bus handle is controller's subordinate bus */
            p_dev->p_parentbus = ((struct awbl_dev *)p_master)->p_subbus;

            /* save device's HCF information */
            p_dev->p_devhcf = &p_devhcf_list[i];

            /* we now have the device, so let the bus subsystem know */
            awbl_dev_announce(p_dev);
        }
    }

    return AW_OK;
}


/******************************************************************************/
aw_err_t awbl_spibus_create (struct awbl_spi_master *p_master)
{
    int ret = AW_OK;

    /* check parameters */
    if (p_master == NULL) {
        return -AW_EFAULT;
    }

    /* announce as bus */
    awbl_bus_announce((struct awbl_busctlr *)p_master, AWBL_BUSID_SPI);

    /* add this master to list */
    __spi_master_insert(&__gp_spi_master_head, p_master);

    /* enumerate the spi device */
    awbl_spibus_devenum(p_master);

    /* 初始化SPI控制器   */
    p_master->tgl_pfunc_cs  = NULL;
    p_master->cs_toggle     = 0;
    p_master->bus_lock_flag = 0;

    AW_MUTEX_INIT(p_master->dev_mutex, AW_SEM_Q_PRIORITY);

    /* master 默认配置   */
    ret = p_master->p_devinfo2->get_default_config(&p_master->cur_config);

    if (ret) {
        return ret;
    }

    p_master->p_devinfo2->config(p_master, &p_master->cur_config);

    return AW_OK;
}


/******************************************************************************/
aw_err_t awbl_spi_setup (struct awbl_spi_device *p_dev)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __spi_setup(AWBL_SPI_PARENT_GET(p_dev), &p_dev->spi_dev);
}


/******************************************************************************/
aw_err_t awbl_spi_async (struct awbl_spi_device *p_dev,
                         struct aw_spi_message  *p_msg)
{
    struct awbl_spi_master *p_master = NULL;
    int    ret = AW_OK;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if ((p_dev == NULL) || (p_msg == NULL)) {

        return -AW_EINVAL;
    }

    p_master = AWBL_SPI_PARENT_GET(p_dev);

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    /* connect spi device to message */
    p_msg->p_spi_dev = &p_dev->spi_dev;

    /* 如果是等于0的话，证明已经被设置过  */
    if (!((uint32_t)p_dev + p_msg->p_spi_dev->setuped)) {

        ret = awbl_spi_setup(p_dev);

        if (ret) {
            return ret;
        }
    }

    /* check if spi master is busy */
    AW_MUTEX_LOCK(p_master->dev_mutex, AW_SEM_WAIT_FOREVER);
    if (p_master->bus_lock_flag ) {
        ret = __spi_sync(p_master, p_msg);
        AW_MUTEX_UNLOCK(p_master->dev_mutex);
        return ret;
    }

    AW_MUTEX_UNLOCK(p_master->dev_mutex);

    return __spi_async(p_master, p_msg);
}


/******************************************************************************/
aw_err_t awbl_spi_sync (struct awbl_spi_device *p_dev,
                        struct aw_spi_message  *p_msg)
{
    int ret = AW_OK;
    struct awbl_spi_master *p_master = NULL;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if ((p_dev == NULL) || (p_msg == NULL)) {

        return -AW_EINVAL;
    }

    p_master = AWBL_SPI_PARENT_GET(p_dev);

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    /* connect spi device to message */
    p_msg->p_spi_dev = &p_dev->spi_dev;

    /* 如果是等于0的话，证明已经被设置过  */
    if (!((uint32_t)p_dev + p_msg->p_spi_dev->setuped)) {

        ret = awbl_spi_setup(p_dev);

        if (ret) {
            return ret;
        }
    }

    return __spi_sync(p_master, p_msg);
}


/******************************************************************************/
aw_err_t awbl_spi_bus_lock (struct awbl_spi_device *p_dev)
{
    struct awbl_spi_master *p_master = NULL;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    p_master = AWBL_SPI_PARENT_GET(p_dev);

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    AW_MUTEX_LOCK(p_master->dev_mutex, AW_SEM_WAIT_FOREVER);

    p_master->bus_lock_flag = AW_TRUE;

    return AW_OK;
}


/******************************************************************************/
aw_err_t awbl_spi_bus_unlock (struct awbl_spi_device *p_dev)
{
    struct awbl_spi_master *p_master = NULL;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    p_master = AWBL_SPI_PARENT_GET(p_dev);

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    p_master->bus_lock_flag = AW_FALSE;
    AW_MUTEX_UNLOCK(p_master->dev_mutex);

    return AW_OK;
}


/******************************************************************************/
aw_err_t awbl_spi_write_then_read (struct awbl_spi_device *p_dev,
                                   const uint8_t          *p_txbuf,
                                   size_t                  n_tx,
                                   uint8_t                *p_rxbuf,
                                   size_t                  n_rx)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __spi_write_then_read(AWBL_SPI_PARENT_GET(p_dev),
                                 &p_dev->spi_dev,
                                  p_txbuf,
                                  n_tx,
                                  p_rxbuf,
                                  n_rx);
}


/******************************************************************************/
aw_err_t awbl_spi_write_then_write (struct awbl_spi_device *p_dev,
                                    const uint8_t          *p_txbuf0,
                                    size_t                  n_tx0,
                                    const uint8_t          *p_txbuf1,
                                    size_t                  n_tx1)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __spi_write_then_write(AWBL_SPI_PARENT_GET(p_dev),
                                  &p_dev->spi_dev,
                                   p_txbuf0,
                                   n_tx0,
                                   p_txbuf1,
                                   n_tx1);
}


/******************************************************************************/
aw_err_t aw_spi_setup (struct aw_spi_device *p_dev)
{
    /* 判断是否在中断上下文中调用接口 */
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __spi_setup(__spi_master_find_by_id(p_dev->busid), p_dev);
}


/******************************************************************************/
aw_err_t aw_spi_async (struct aw_spi_device  *p_dev,
                       struct aw_spi_message *p_msg)
{
    struct awbl_spi_master *p_master = NULL;
    int    ret = 0;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if ((p_dev == NULL) ||
        (p_msg == NULL)) {

        return -AW_EINVAL;
    }

    p_master = __spi_master_find_by_id(p_dev->busid);
    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    /* 如果是等于0的话，证明已经被设置过  */
    if ((uint32_t)p_dev + p_dev->setuped) {

        ret = aw_spi_setup(p_dev);

        if (ret) {
            return ret;
        }
    }

    /* connect spi device to message */
    p_msg->p_spi_dev = p_dev;

    /* if the spi master was locked . we will use the sync insted of asyn. */
    AW_MUTEX_LOCK(p_master->dev_mutex, AW_SEM_WAIT_FOREVER);
    if (p_master->bus_lock_flag ) {
        ret =  __spi_sync(p_master, p_msg);
        AW_MUTEX_UNLOCK(p_master->dev_mutex);
        return ret;
    }
    AW_MUTEX_UNLOCK(p_master->dev_mutex);

    return __spi_async(p_master, p_msg);
}


/******************************************************************************/
aw_err_t aw_spi_sync (struct aw_spi_device  *p_dev,
                      struct aw_spi_message *p_msg)
{
    int    ret                       = AW_OK;
    struct awbl_spi_master *p_master = NULL;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if ((p_dev == NULL) || (p_msg == NULL)) {
        return -AW_EINVAL;
    }

    p_master = __spi_master_find_by_id(p_dev->busid);
    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    /* 如果是等于0的话，证明已经被设置过  */
    if ((uint32_t)p_dev + p_dev->setuped) {

        ret = aw_spi_setup(p_dev);

        if (ret) {
            return ret;
        }
    }

    /* connect spi device to message */
    p_msg->p_spi_dev = p_dev;

    return __spi_sync(p_master, p_msg);
}


/******************************************************************************/
aw_err_t aw_spi_bus_lock (struct aw_spi_device *p_dev)
{
    struct awbl_spi_master *p_master = NULL;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    p_master = __spi_master_find_by_id(p_dev->busid);
    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    AW_MUTEX_LOCK(p_master->dev_mutex, AW_SEM_WAIT_FOREVER);

    p_master->bus_lock_flag = AW_TRUE;

    return AW_OK;
}


/******************************************************************************/
aw_err_t aw_spi_bus_unlock (struct aw_spi_device *p_dev)
{
    struct awbl_spi_master *p_master = NULL;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    p_master = __spi_master_find_by_id(p_dev->busid);
    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    p_master->bus_lock_flag = AW_FALSE;
    AW_MUTEX_UNLOCK(p_master->dev_mutex);

    return AW_OK;
}


/******************************************************************************/
aw_err_t aw_spi_write_then_read (struct aw_spi_device *p_dev,
                                 const uint8_t        *p_txbuf,
                                 size_t                n_tx,
                                 uint8_t              *p_rxbuf,
                                 size_t                n_rx)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __spi_write_then_read(__spi_master_find_by_id(p_dev->busid),
                                 p_dev,
                                 p_txbuf,
                                 n_tx,
                                 p_rxbuf,
                                 n_rx);
}


/******************************************************************************/
aw_err_t aw_spi_write_then_write (struct aw_spi_device *p_dev,
                                  const uint8_t        *p_txbuf0,
                                  size_t                n_tx0,
                                  const uint8_t        *p_txbuf1,
                                  size_t                n_tx1)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __spi_write_then_write(__spi_master_find_by_id(p_dev->busid),
                                  p_dev,
                                  p_txbuf0,
                                  n_tx0,
                                  p_txbuf1,
                                  n_tx1);
}


aw_local aw_err_t __spi_cs_on( struct awbl_spi_master  *p_master,
                           aw_spi_device_t         *p_dev)
{
    /* if last device toggled after message */

    if (p_master->cs_toggle) {
        if ((p_master->cs_tgl_pin != -1) && (p_master->cs_tgl_pin != p_dev->cs_pin)) {

            /*
             * 上个Message的cschange为true，但是下个Message的片选引脚变化了，
             * 这种形为是不允许的
             */
            return -AW_ENOTSUP;
        }
    }

    if (p_dev->pfunc_cs == NULL) {
        __spi_cs_pin_set(p_dev->cs_pin, p_dev->mode, 1);
    } else {
        p_dev->pfunc_cs(1);
    }

    return AW_OK;
}


aw_local void __spi_cs_off( struct awbl_spi_master  *p_master,
                            aw_spi_device_t         *p_dev)
{

    if (p_master->cs_tgl_pin  == p_dev->cs_pin) {
        p_master->cs_tgl_pin   = -1;
        p_master->tgl_pfunc_cs = NULL;
        p_master->cs_tgl_mode  = 0;
    }

    if (p_dev->pfunc_cs == NULL) {
        __spi_cs_pin_set(p_dev->cs_pin, p_dev->mode, 0);
    } else {
        p_dev->pfunc_cs(0);
    }
}


aw_err_t awbl_spi_write_and_read(struct awbl_spi_device *p_dev,
                                 const void             *p_txbuf,
                                 void                   *p_rxbuf,
                                 size_t                  nbytes)
{

    int ret = AW_OK;
    struct awbl_spi_master *p_master = NULL;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
       return -AW_EINVAL;
    }

    p_master = AWBL_SPI_PARENT_GET(p_dev);

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    if ((p_txbuf == NULL) && (p_rxbuf == NULL)) {
        return -AW_EINVAL;
    }

    /* 如果是等于0的话，证明已经被设置过  */
    if ((uint32_t)&p_dev->spi_dev + p_dev->spi_dev.setuped) {

       ret = awbl_spi_setup(p_dev);

       if (ret) {
           return ret;
       }
    }

    AW_MUTEX_LOCK(p_master->dev_mutex, AW_SEM_WAIT_FOREVER);

    if ((p_master->cur_config.bpw != p_dev->spi_dev.bits_per_word) ||
        (p_master->cur_config.mode != p_dev->spi_dev.mode)         ||
        (p_master->cur_config.speed_hz != p_dev->spi_dev.max_speed_hz)) {

        struct awbl_spi_config tmp_config;
        tmp_config.bpw = p_dev->spi_dev.bits_per_word;
        tmp_config.mode = p_dev->spi_dev.mode;
        tmp_config.speed_hz = p_dev->spi_dev.max_speed_hz;

        ret = p_master->p_devinfo2->config(p_master, &tmp_config);

        if (ret) {
            goto __err;
        }

        /* 配置成功，更新cur_config  */
        p_master->cur_config = tmp_config;
    }

    /* 判断nbytes的合法性  */
    if (nbytes != 0) {
        if (p_master->cur_config.bpw > 17) {
           if ( nbytes % 4 ) {
               ret = -AW_EINVAL;
               goto __err;
           }
        } else if (p_master->cur_config.bpw > 9) {
            if ( nbytes % 2 ) {
                ret = -AW_EINVAL;
                goto __err;
            }
        }
    } else {
        ret = -AW_EINVAL;
        goto __err;
    }

    ret = __spi_cs_on(p_master, &p_dev->spi_dev);

    if (ret) {
        goto __err;
    }

    if (!(p_dev->spi_dev.mode & AW_SPI_AUTO_CS)) {

        /* 有没有必要判断返回值待定  */
        ret = p_master->p_devinfo2->write_and_read(p_master,
                                                   p_txbuf,
                                                   p_rxbuf,
                                                   nbytes);
    } else {

        uint8_t cs_polarity    = ((p_dev->spi_dev.mode & AW_SPI_CS_HIGH) ?
                                   AWBL_SPI_HARDCS_HIGH_VALID            :
                                   AWBL_SPI_HARDCS_LOW_VALID);
        /* 有没有必要判断返回值待定  */
        ret = p_master->p_devinfo2->write_and_read_hard_cs( 
                                        p_master,
                                        p_txbuf,
                                        p_rxbuf,
                                        nbytes,
                                        p_dev->spi_dev.cs_pin,
                                        cs_polarity,
                                        AWBL_SPI_END_OF_HARDCS_INVALID);
    }

    if (ret) {
        goto __err;
    }
    __spi_cs_off(p_master, &p_dev->spi_dev);

__err:
    AW_MUTEX_UNLOCK(p_master->dev_mutex);
    return ret;

}


aw_err_t aw_spi_write_and_read (struct aw_spi_device   *p_dev,
                                const void             *p_txbuf,
                                void                   *p_rxbuf,
                                size_t                  nbytes)
{
    int ret = AW_OK;
    struct awbl_spi_master *p_master = NULL;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    p_master = __spi_master_find_by_id(p_dev->busid);

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    if ((p_txbuf == NULL) && (p_rxbuf == NULL)) {
        return -AW_EINVAL;
    }

    /* 如果是等于0的话，证明已经被设置过  */
    if ((uint32_t)p_dev + p_dev->setuped) {

        ret = aw_spi_setup(p_dev);

        if (ret) {
            return ret;
        }
    }

    AW_MUTEX_LOCK(p_master->dev_mutex, AW_SEM_WAIT_FOREVER);

    if ((p_master->cur_config.bpw != p_dev->bits_per_word) ||
        (p_master->cur_config.mode != p_dev->mode)         ||
        (p_master->cur_config.speed_hz != p_dev->max_speed_hz)) {

        struct awbl_spi_config tmp_config;
        tmp_config.bpw = p_dev->bits_per_word;
        tmp_config.mode = p_dev->mode;
        tmp_config.speed_hz = p_dev->max_speed_hz;

        ret = p_master->p_devinfo2->config(p_master, &tmp_config);

        if (ret) {
            goto __err;
        }

        /* 配置成功，更新cur_config  */
        p_master->cur_config = tmp_config;
    }

    /* 判断nbytes 的合法性 */
    if (nbytes != 0) {
        if (p_master->cur_config.bpw > 17) {
           if ( nbytes % 4 ) {
               ret = -AW_EINVAL;
               goto __err;
           }
        } else if (p_master->cur_config.bpw > 9) {
            if ( nbytes % 2 ) {
                ret = -AW_EINVAL;
                goto __err;
            }
        }
    } else {
        ret = -AW_EINVAL;
        goto __err;
    }

    ret = __spi_cs_on(p_master, p_dev);

    if (ret) {
        goto __err;
    }

    if (!(p_dev->mode & AW_SPI_AUTO_CS)) {

        /* 有没有必要判断返回值待定  */
        ret = p_master->p_devinfo2->write_and_read(p_master,
                                                   p_txbuf,
                                                   p_rxbuf,
                                                   nbytes);
    } else {

        uint8_t cs_polarity    = ((p_dev->mode & AW_SPI_CS_HIGH) ?
                                   AWBL_SPI_HARDCS_HIGH_VALID             :
                                   AWBL_SPI_HARDCS_LOW_VALID);
        /* 有没有必要判断返回值待定  */
        ret = p_master->p_devinfo2->write_and_read_hard_cs(
                                        p_master,
                                        p_txbuf,
                                        p_rxbuf,
                                        nbytes,
                                        p_dev->cs_pin,
                                        cs_polarity,
                                        AWBL_SPI_END_OF_HARDCS_INVALID);
    }

    if (ret) {
        goto __err;
    }
    __spi_cs_off(p_master, p_dev);

__err:
    AW_MUTEX_UNLOCK(p_master->dev_mutex);
    return ret;
}

/* end of file */
