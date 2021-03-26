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
 * \brief AWBus I2C bus type implementation (lite)
 *
 * \internal
 * \par modification history:
 *
 * - 3.00 18-11-08  sls  update driver frame
 *
 * - 2.00 14-03-07  zen, change: specify"devhcf_list"at initialization time
 *                  (for multicore-support reason )
 * - 1.00 12-10-10  zen, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include <string.h>
#include "aw_list.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "aw_int.h"
#include "aw_delay.h"

/*******************************************************************************
  import
*******************************************************************************/

/*******************************************************************************
  defines
*******************************************************************************/

#define __I2C_MST_DEVINFO_GET(p_dev) \
        ((struct awbl_i2c_master_devinfo *)AWBL_DEVINFO_GET(p_dev))

#define __I2C_MST_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_i2c_master_devinfo *p_devinfo = \
        (struct awbl_i2c_master_devinfo *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
  type defines
*******************************************************************************/

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_bool_t awbl_i2cbus_devmatch(const struct awbl_drvinfo *p_drv,
                               struct awbl_dev           *p_dev);

/*******************************************************************************
  globals
*******************************************************************************/

/*******************************************************************************
  locals
*******************************************************************************/

/** \brief bus type registration (must defined as aw_const)*/
aw_local aw_const struct awbl_bustype_info __g_i2c_bustype = {
    AWBL_BUSID_I2C,         /* bus_id */
    NULL,                   /* pfunc_bustype_init1 */
    NULL,                   /* pfunc_bustype_init2 */
    NULL,                   /* pfunc_bustype_connect */
    NULL,                   /* pfunc_bustype_newdev_present */
    awbl_i2cbus_devmatch    /* pfunc_bustype_devmatch */
};

/** \brief header of i2c masters list */
aw_local struct awbl_i2c_master *__gp_i2c_master_head;

/** \brief �첽��������  */
AW_TASK_DECL_STATIC(i2c_async_task_decl, AWBL_I2C_ASYNC_TASK_STACK_SIZE);

/** \brief �첽���������ź���  */
AW_SEMB_DECL_STATIC(i2c_sem_async);

/** \brief ��ֹ���������lock, ���ļ������з�ֹ����ϵĲ������ô˻�����  */
AW_MUTEX_DECL_STATIC(i2cbus_lock);

/* �첽��Ϣ���� */
aw_local struct aw_list_head __async_msg_list_head;

/* ��Ҫ���ݽ������д���ı�־λ  */
#define  __DR_USE_FLAGS  (AW_I2C_M_10BIT          |\
                          AW_I2C_M_REV_DIR_ADDR   |\
                          AW_I2C_M_IGNORE_NAK     |\
                          AW_I2C_M_NO_RD_ACK)

/** \brief �����Ժϲ���trans����  */
#define __MAX_TRANS_GROUP              10


/*******************************************************************************
  implementation
*******************************************************************************/

aw_local aw_err_t __i2c_sync (struct awbl_i2c_master *p_master,
                              struct aw_i2c_message  *p_msg);

aw_local aw_err_t __i2c_rw (struct awbl_i2c_master *p_master,
                            uint16_t                flags,
                            uint16_t                chip_addr,
                            uint32_t                subaddr,
                            void                   *p_buf,
                            size_t                  nbytes,
                            uint8_t                 is_read);

aw_local aw_err_t __raw_rw_with_subaddr (struct awbl_i2c_master  *p_master,
                                         uint16_t                 addr,
                                         uint16_t                 flags,
                                         const void              *p_txbuf,
                                         size_t                   n_tx,
                                         void                    *p_buf,
                                         size_t                   nbytes,
                                         uint8_t                  is_read,
                                         uint8_t                  read_send_stop);

/**
 * \brief  asynchronous message insert
 */
aw_local void __awbl_i2c_msg_in (struct aw_i2c_message  *p_msg)
{
    aw_list_add_tail((struct aw_list_head *)(&p_msg->msg),
                     &__async_msg_list_head);
}


/**
 * \brief asynchronous message out
 */
aw_local struct aw_i2c_message * __awbl_i2c_msg_out (void)
{
    if (aw_list_empty_careful(&__async_msg_list_head)) {
        return NULL;
    } else {
        struct aw_list_head *p_node = __async_msg_list_head.next;
        aw_list_del(p_node);
        return aw_list_entry(p_node, struct aw_i2c_message, msg);
    }
}


/**
 * \brief add an i2c master to master list
 */
aw_local void __i2c_master_insert (struct awbl_i2c_master **pp_head,
                                   struct awbl_i2c_master  *p_master)
{
    while (*pp_head != NULL) {
        pp_head = &(*pp_head)->p_next;
    }

    *pp_head = p_master;

     p_master->p_next = NULL;
}


aw_local void __i2c_async_task_entry(void)
{
    struct awbl_i2c_master *p_master  = NULL;
    struct aw_i2c_message  *p_cur_msg = NULL;

    AW_FOREVER {
        AW_SEMB_TAKE(i2c_sem_async, AW_SEM_WAIT_FOREVER);

        /* ����Message */
        AW_FOREVER {

            AW_MUTEX_LOCK(i2cbus_lock, AW_SEM_WAIT_FOREVER);
            p_cur_msg = __awbl_i2c_msg_out();
            AW_MUTEX_UNLOCK(i2cbus_lock);

            if (p_cur_msg == NULL) {

                /* ���û����Ϣ��������ѭ��  */
                break;
            } else {

                /* ��Ϣ���ڱ����� */
                p_cur_msg->status = -AW_EINPROGRESS;

                /* ��ȡ��ǰmessage��master */
                p_master = p_cur_msg->p_master;

                /*
                 * �������У��첽Message�����п��ܲ���ͬ���ӿڴ�����__i2c_sync
                 */
                __i2c_sync(p_master, p_cur_msg);
            }
        }
    }
}


aw_local aw_err_t __i2c_check_feature(struct awbl_i2c_master *p_master,
                                      uint32_t flags)
{
    /** \brief ����ӳ�� */
    struct feature_map {
        uint16_t set;     /** < \brief �û����õ�����  */
        uint16_t support; /** < \brief ������֧�ֵ����� */
    };

    int i = 0;
    const static struct feature_map feture_match_tab[] = {
            {AW_I2C_ADDR_7BIT ,  AWBL_FEATURE_I2C_7BIT },
            {AW_I2C_ADDR_10BIT,  AWBL_FEATURE_I2C_10BIT},
            {AW_I2C_IGNORE_NAK,  AWBL_FEATURE_I2C_IGNORE_NAK},
            {AW_I2C_M_NO_RD_ACK, AWBL_FEATURE_I2C_NO_RD_ACK},
    };

    for (i = 0; i < sizeof(feture_match_tab) / sizeof(struct feature_map); i++) {
        if ((flags & feture_match_tab[i].set) == feture_match_tab[i].set) {
            /* ������10bit�����������в�֧�֣����ش���  */
            if ( !(p_master->p_devinfo2->features & feture_match_tab[i].support)) {

                /* ֻҪ��һ����֧�ֵľͷ��ش���  */
                return -AW_ENOTSUP;
            }
        }
    }

    return AW_OK;
}


aw_local aw_err_t __i2c_check_trans(struct awbl_i2c_master *p_master,
                                    struct aw_i2c_message  *p_msg)
{
    int i;
    int ret = AW_OK;

    /* check if each transfer is valid */
    for (i = 0; i < p_msg->trans_num; i++) {
        /* check feature support */
        ret = __i2c_check_feature(p_master, p_msg->p_transfers[i].flags);

        if (ret != AW_OK) {
            return ret;
        }

        /* 0 data length is not allowed , check if data buffer is valid*/
        if ((p_msg->p_transfers[i].nbytes == 0) ||
            (p_msg->p_transfers[i].p_buf == NULL)) {
            return -AW_EINVAL;
        }
    }

    return AW_OK;
}


/**
 * \brief asynchronous I2C transfer
 */
aw_local aw_err_t __i2c_async (struct awbl_i2c_master *p_master,
                               struct aw_i2c_message  *p_msg)
{
    int   ret = AW_OK;

    AW_MUTEX_LOCK(i2cbus_lock, AW_SEM_WAIT_FOREVER);

    /* �������û�г�ʼ�����ʼ������ */
    if (!AW_TASK_VALID (i2c_async_task_decl)) {
        aw_task_id_t task = NULL;
        AW_INIT_LIST_HEAD(&__async_msg_list_head);
        AW_SEMB_INIT(i2c_sem_async, 0, AW_SEM_Q_FIFO);

        /* ȷ���ź����ȳ�ʼ��,���ᱻ������������  */
        aw_barrier();

        /* ��ʼ������spi_trans_task */
        task = AW_TASK_INIT( i2c_async_task_decl,              /* ����ʵ�� */
                             "i2c_trans_task",                 /* �������� */
                             AWBL_I2C_ASYNC_TASK_PRIO,         /* �������ȼ� */
                             AWBL_I2C_ASYNC_TASK_STACK_SIZE,   /* �����ջ��С */
                             __i2c_async_task_entry,           /* ������ں��� */
                             NULL);

        if (task == AW_TASK_ID_INVALID) {
            while(1);
        }

        /* ��������spi_trans_task,startup�Ǹ����������ύ�� */
        AW_TASK_STARTUP(i2c_async_task_decl);

        /* �ȴ�������Ч  */
        while (!AW_TASK_VALID (i2c_async_task_decl)) {
           /* ���������Ч�����˳� */
           aw_mdelay(1);
        }
    }

    AW_MUTEX_UNLOCK(i2cbus_lock);

    ret = __i2c_check_trans(p_master, p_msg);

    if (ret) {
        p_msg->status = ret;
        return ret;
    }

    p_msg->status = -AW_ENOTCONN;

    /* add message to contoller's message list */
    AW_MUTEX_LOCK(i2cbus_lock, AW_SEM_WAIT_FOREVER);

    /* message is queueing */
    p_msg->status = -AW_EISCONN;
    p_msg->p_master = p_master;
    __awbl_i2c_msg_in(p_msg);
    AW_MUTEX_UNLOCK(i2cbus_lock);

    /* �ͷ��ź���  */
    AW_SEMB_GIVE(i2c_sem_async);

    return AW_OK;
}


aw_local aw_err_t __deal_write_trans(struct aw_i2c_message  *p_msg,
                                     struct aw_i2c_transfer *p_transfers,
                                     struct awbl_trans_buf  *p_trans_buf,
                                     size_t                  trans_size,
                                     size_t                 *p_merge_num,
                                     uint8_t                *p_is_read_start,
                                     uint8_t                *p_is_write_stop)
{
    int  i = 0;

    /* ��ʼtrans�����Ǵ���ʼ�źŲ�����д�Ž��кϲ�,�����˳����ϲ�trans  */
    if (!(p_transfers[i].flags & (AW_I2C_M_NOSTART | AW_I2C_M_RD))) {

        /* ���  */
        memset(p_trans_buf, 0, trans_size);

        p_trans_buf[i].p_buf      = p_transfers[i].p_buf;
        p_trans_buf[i].buf_size   = p_transfers[i].nbytes;
        i++;
    } else {

        *p_merge_num = 0;

        /* ����Ƕ���ֱ�ӷ���, �����û����ʼ�źŵ�д,�򷵻���Ч����  */
        if (p_transfers[i].flags & AW_I2C_M_RD) {

            /* ����ϴ�д������ֹͣ�ź�, ���´ζ��͸���ʼ�źţ�������ظ���ʼ�ź� */
            if (*p_is_write_stop == AWBL_I2C_WRITE_STOP) {
                *p_is_read_start = AWBL_I2C_READ_START;
            } else {
                *p_is_read_start = AWBL_I2C_READ_RESTART;
            }

            return AW_OK;
        } else {
            return -AW_EINVAL;
        }
    }

    while (i < (__MAX_TRANS_GROUP + 1)) {

        /* �Ѿ������һ��buffer��  */
        if (&p_msg->p_transfers[p_msg->trans_num - 1] == &p_transfers[i - 1]) {
            *p_merge_num = i;
            *p_is_write_stop = AWBL_I2C_WRITE_STOP;
            return AW_OK;
        }

        /* û����ʼ�źŵ�д������֧�ֺϲ�trans */
        if ((p_transfers[i].flags & AW_I2C_M_NOSTART) &&
            (!(p_transfers[i].flags & AW_I2C_M_RD))) {

            /* ���ݽ������ı�־λҪ��ͬ  */
            if ((p_transfers[i].flags & __DR_USE_FLAGS) ==
                (p_transfers[i - 1].flags & __DR_USE_FLAGS)) {
                p_trans_buf[i].p_buf      = p_transfers[i].p_buf;
                p_trans_buf[i].buf_size = p_transfers[i].nbytes;
                i++;
            } else {
                return -AW_ENOTSUP;
            }
        } else {

            /* û��AW_I2C_M_NOSTART ��־ ��д��������AW_I2C_M_RD ��־���˳�  */

            *p_merge_num = i;

            /* ����Ƕ���־, �򲻺ϲ�,���´���һ���ظ���ʼ�ź�  */
            if (p_transfers[i].flags & AW_I2C_M_RD) {
                *p_is_write_stop = AWBL_I2C_WRITE_NO_STOP;
            } else {
                /*
                 * û��AW_I2C_M_NOSTART, ���ܺϲ�,
                 * ��ǰ�������һ�����䲻�Ƕ�, Ҫ��ֹͣ�ź�  
                 */
                *p_is_write_stop = AWBL_I2C_WRITE_STOP;
            }

            return AW_OK;
        }
    }

    /* ����10����֧�� */
    return -AW_ENOTSUP;
}


/**
 * \brief synchronous I2C transfer
 */
aw_local aw_err_t __i2c_sync (struct awbl_i2c_master *p_master,
                              struct aw_i2c_message  *p_msg)
{
    int ret = AW_OK;
    int i = 0;
    uint8_t      send_stop  = AWBL_I2C_WRITE_STOP;
    uint8_t      send_start = AWBL_I2C_READ_START;
    uint16_t     dr_flags   = 0;
    struct awbl_trans_buf trans_buf[__MAX_TRANS_GROUP];
    size_t        merge_num = 0;

    ret = __i2c_check_trans(p_master, p_msg);

    if (ret) {
        p_msg->status = ret;
        return ret;
    }

    p_msg->status    = -AW_EINPROGRESS;
    p_msg->done_num  = 0;

    AW_MUTEX_LOCK(p_master->dev_mux, AW_SEM_WAIT_FOREVER);

    for (i = 0; i < p_msg->trans_num;) {

        ret = __deal_write_trans(p_msg,
                                 &p_msg->p_transfers[i],
                                 trans_buf,
                                 sizeof(trans_buf),
                                 &merge_num,
                                 &send_start,
                                 &send_stop);

        if (ret) {
            goto __end;
        }

        /* mask out the flags used by the current layer */
        dr_flags =  p_msg->p_transfers[i].flags & __DR_USE_FLAGS;

        /* �кϲ���Ϊд  */
        if (merge_num) {

            ret = p_master->p_devinfo2->pfunc_i2c_write(p_master,
                                            p_msg->p_transfers[i].addr,
                                            trans_buf,
                                            merge_num,
                                            send_stop,
                                            dr_flags);

            if (ret) {
                p_msg->status = ret;
                goto __end;
            }

            p_msg->done_num += merge_num;
            i += merge_num;

        } else {
            uint8_t buf_index = i + merge_num;

            /* 
             * �����һ������û�и�ֹͣ�źţ������һ���ظ���ʼ�źţ�
             * �����һ����ʼ�źţ���������׸����䣬�����ʼ�ź�
             */
            struct awbl_trans_buf trans_buf = {p_msg->p_transfers[buf_index].p_buf,
                                               p_msg->p_transfers[buf_index].nbytes};

            /* have START and STOP condition */
            ret = p_master->p_devinfo2->pfunc_i2c_read(p_master,
                                            p_msg->p_transfers[buf_index].addr,
                                            &trans_buf,
                                            send_start,
                                            dr_flags);

            if (ret) {
                p_msg->status = ret;
                goto __end;
            }

            i++;
            p_msg->done_num ++;
        }
    }

    p_msg->status = AW_OK;

__end:

    /* the message done */
    if (p_msg->pfunc_complete) {
        p_msg->pfunc_complete(p_msg->p_arg);
    }
    AW_MUTEX_UNLOCK(p_master->dev_mux);
    return ret;
}


/**
 * \brief find out controller by master id
 */
aw_local struct awbl_i2c_master *__i2c_master_find_by_id (int masterid)
{
    struct awbl_i2c_master *p_master_cur = __gp_i2c_master_head;

    /* todo: if only one controller, just return it directly */

    while (p_master_cur != NULL) {

        if (masterid ==
           ((struct awbl_i2c_master_devinfo *)
             AWBL_DEVINFO_GET(p_master_cur))->bus_index) {

            return p_master_cur;
        }

        p_master_cur = p_master_cur->p_next;
    }

    return NULL;
}


/******************************************************************************/
void awbl_i2cbus_init (void)
{
    AW_MUTEX_INIT(i2cbus_lock, AW_SEM_Q_PRIORITY);
    
    /* register I2C as a valid bus type */
    awbl_bustype_register(&__g_i2c_bustype);
}


/******************************************************************************/
aw_bool_t awbl_i2cbus_devmatch (const struct awbl_drvinfo *p_drv,
                             struct awbl_dev           *p_dev)
{
    /* check the bus type */

    if (p_dev->p_devhcf->bus_type != AWBL_BUSID_I2C) {
        return AW_FALSE;
    }

    /* check name */

    if (strcmp(p_dev->p_devhcf->p_name, p_drv->p_drvname) != 0) {
        return AW_FALSE;
    }

    return AW_TRUE;
}


/******************************************************************************/
aw_err_t awbl_i2cbus_devenum (struct awbl_i2c_master *p_master)
{
    int i;
    struct awbl_dev    *p_dev  = NULL;
    const struct awbl_devhcf *p_devhcf_list = awbl_devhcf_list_get();
    size_t              devhcf_list_count = awbl_devhcf_list_count_get();
    __I2C_MST_DEVINFO_DECL(p_mdinfo, p_master);

    /* search for the devices on I2C bus from HCF list */

    for (i = 0; i < devhcf_list_count; i++) {

        /* the device is on this I2C bus */

        if ((p_devhcf_list[i].bus_type  == AWBL_BUSID_I2C) &&
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
aw_err_t awbl_i2cbus_create (struct awbl_i2c_master *p_master)
{
    /* check parameters */
    if (p_master == NULL) {
        return -AW_EFAULT;
    }

    /* announce as bus */
    awbl_bus_announce((struct awbl_busctlr *)p_master, AWBL_BUSID_I2C);

    /* add this master to list */
    __i2c_master_insert(&__gp_i2c_master_head, p_master);

    /* enumerate the i2c device */
    awbl_i2cbus_devenum(p_master);

    AW_MUTEX_INIT(p_master->dev_mux, AW_SEM_Q_PRIORITY);

    return AW_OK;
}


/******************************************************************************/
aw_err_t awbl_i2c_async (struct awbl_i2c_device *p_dev,
                         struct aw_i2c_message  *p_msg)
{
    struct awbl_i2c_master *p_master = NULL;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if ((p_dev              == NULL) ||
        (p_msg              == NULL) ||
        (p_msg->p_transfers == NULL) ||
        (p_msg->trans_num   == 0)) {

        return -AW_EINVAL;
    }

    p_master = AWBL_I2C_PARENT_GET(p_dev);

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    return __i2c_async(p_master, p_msg);
}


/******************************************************************************/
aw_err_t awbl_i2c_sync (struct awbl_i2c_device *p_dev,
                        struct aw_i2c_message  *p_msg)
{
    struct awbl_i2c_master *p_master = NULL;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if ((p_dev              == NULL) ||
        (p_msg              == NULL) ||
        (p_msg->p_transfers == NULL) ||
        (p_msg->trans_num   == 0)) {

        return -AW_EINVAL;
    }

    p_master = AWBL_I2C_PARENT_GET(p_dev);

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    return __i2c_sync(p_master, p_msg);
}


/******************************************************************************/
aw_err_t awbl_i2c_read (struct awbl_i2c_device *p_dev,
                        uint16_t                flags,
                        uint16_t                addr,
                        uint32_t                subaddr,
                        void                   *p_buf,
                        size_t                  nbytes)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __i2c_rw (AWBL_I2C_PARENT_GET(p_dev),
                     flags,
                     addr,
                     subaddr,
                     p_buf,
                     nbytes,
                     AW_TRUE);
}


/******************************************************************************/
aw_err_t awbl_i2c_write (struct awbl_i2c_device *p_dev,
                         uint16_t                flags,
                         uint16_t                addr,
                         uint32_t                subaddr,
                         const void             *p_buf,
                         size_t                  nbytes)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __i2c_rw (AWBL_I2C_PARENT_GET(p_dev),
                     flags,
                     addr,
                     subaddr,
                     (void*)p_buf,
                     nbytes,
                     AW_FALSE);

}


/******************************************************************************/
aw_err_t aw_i2c_async (uint8_t           busid,
                       aw_i2c_message_t *p_msg)
{
    struct awbl_i2c_master *p_master = __i2c_master_find_by_id(busid);

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    if ((p_msg              == NULL) ||
        (p_msg->p_transfers == NULL) ||
        (p_msg->trans_num   == 0)) {

        return -AW_EINVAL;
    }

    return __i2c_async(p_master, p_msg);
}


/******************************************************************************/
aw_err_t aw_i2c_sync (uint8_t           busid,
                      aw_i2c_message_t *p_msg)
{
    struct awbl_i2c_master *p_master = __i2c_master_find_by_id(busid);

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    if ((p_msg              == NULL) ||
        (p_msg->p_transfers == NULL) ||
        (p_msg->trans_num   == 0)) {

        return -AW_EINVAL;
    }

    return __i2c_sync(p_master, p_msg);
}


aw_local aw_err_t __i2c_rw_no_subaddr(struct awbl_i2c_master *p_master,
                                      uint16_t                chip_addr,
                                      uint16_t                flags,
                                      void                   *p_buf,
                                      size_t                  nbytes,
                                      uint8_t                 is_read)
{
    int       ret = AW_OK;
    struct awbl_trans_buf trans_buf = {p_buf, nbytes};

    AW_MUTEX_LOCK(p_master->dev_mux, AW_SEM_WAIT_FOREVER);

    if (is_read) {
        /* �п�ʼλ��ֹͣλ  */
        ret = p_master->p_devinfo2->pfunc_i2c_read(p_master,
                                                   chip_addr,
                                                   &trans_buf,
                                                   AWBL_I2C_READ_START,
                                                   flags);
    } else {


        ret = p_master->p_devinfo2->pfunc_i2c_write(p_master,
                                        chip_addr,
                                        &trans_buf,
                                        1,
                                        AWBL_I2C_WRITE_STOP,
                                        flags);
    }

    AW_MUTEX_UNLOCK(p_master->dev_mux);
    return ret;
}


aw_local aw_err_t __i2c_rw (struct awbl_i2c_master *p_master,
                            uint16_t                flags,
                            uint16_t                chip_addr,
                            uint32_t                subaddr,
                            void                   *p_buf,
                            size_t                  nbytes,
                            uint8_t                 is_read)
{
    uint16_t  subaddr_len = AW_I2C_SUBADDR_LEN_GET(flags);
    uint8_t   tmp_subaddr[2];
    int       ret = AW_OK;

    /* �������㴫�ݵ�flags */
    uint16_t  dr_flags = flags;

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    ret = __i2c_check_feature(p_master, flags);

    if (ret != AW_OK) {
        return ret;
    }

    if ((p_buf == NULL) || (nbytes == 0)) {
        return -AW_EINVAL;
    }

    /* ���ε��м��ʹ�õı�־λ */
    dr_flags &= __DR_USE_FLAGS;
    if (subaddr_len == 0) {
        return __i2c_rw_no_subaddr(p_master,
                                   chip_addr,
                                   dr_flags,
                                   p_buf,
                                   nbytes,
                                   is_read);
    }else if (subaddr_len == 1) {
        tmp_subaddr[0] = subaddr;
    } else if (subaddr_len == 2) {

        if (flags & AW_I2C_SUBADDR_LSB_FIRST) {
            tmp_subaddr[0] = (uint8_t)(subaddr & 0xFF);
            tmp_subaddr[1] = (uint8_t)(subaddr >> 8);
        } else {
            tmp_subaddr[0] = (uint8_t)(subaddr >> 8);
            tmp_subaddr[1] = (uint8_t)(subaddr & 0xFF);
        }
    } else {
        return -AW_ENOTSUP;
    }

    return __raw_rw_with_subaddr(p_master,
                                 chip_addr,
                                 dr_flags,
                                 tmp_subaddr,
                                 subaddr_len,
                                 p_buf,
                                 nbytes,
                                 is_read,
                                 AW_FALSE);
}


/******************************************************************************/
aw_err_t aw_i2c_read (aw_i2c_device_t  *p_dev,
                      uint32_t          subaddr,
                      uint8_t          *p_buf,
                      size_t            nbytes)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __i2c_rw(__i2c_master_find_by_id(p_dev->busid),
                    p_dev->flags,
                    p_dev->addr,
                    subaddr,
                    p_buf,
                    nbytes,
                    AW_TRUE);
}


/******************************************************************************/
aw_err_t aw_i2c_write (aw_i2c_device_t  *p_dev,
                       uint32_t          subaddr,
                       const void       *p_buf,
                       size_t            nbytes)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __i2c_rw(__i2c_master_find_by_id(p_dev->busid),
                    p_dev->flags,
                    p_dev->addr,
                    subaddr,
                    (void*)p_buf,
                    nbytes,
                    AW_FALSE);
}


aw_local aw_err_t __i2c_raw_rw (struct awbl_i2c_master *p_master,
                                uint16_t                addr,
                                uint16_t                flags,
                                void                   *p_buf,
                                size_t                  nbytes,
                                uint8_t                 is_read)
{
    int                   ret       = AW_OK;
    uint16_t              dr_flags  = 0;
    struct awbl_trans_buf trans_buf = {p_buf, nbytes};

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    ret = __i2c_check_feature(p_master, flags);

    if (ret != AW_OK) {
        return ret;
    }

    if ((p_buf == NULL) || (nbytes == 0)) {
        return -AW_EINVAL;
    }

    dr_flags = flags & __DR_USE_FLAGS;
    AW_MUTEX_LOCK(p_master->dev_mux, AW_SEM_WAIT_FOREVER);

    if (is_read) {
        ret = p_master->p_devinfo2->pfunc_i2c_read(p_master,
                                                   addr,
                                                   &trans_buf,
                                                   AWBL_I2C_READ_START,
                                                   dr_flags);
    } else {
        ret = p_master->p_devinfo2->pfunc_i2c_write(p_master,
                                        addr,
                                        &trans_buf,
                                        1,
                                        AWBL_I2C_WRITE_STOP,
                                        dr_flags);
    }

    AW_MUTEX_UNLOCK(p_master->dev_mux);

    return ret;
}


aw_err_t aw_i2c_raw_write (aw_i2c_device_t  *p_dev,
                           const void       *p_buf,
                           size_t            nbytes)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __i2c_raw_rw ( __i2c_master_find_by_id(p_dev->busid),
                          p_dev->addr,
                          p_dev->flags,
                          (void*)p_buf,
                          nbytes,
                          AW_FALSE);
}


aw_err_t awbl_i2c_raw_write (struct awbl_i2c_device  *p_dev,
                             uint16_t                 addr,
                             uint16_t                 flags,
                             const void              *p_buf,
                             size_t                   nbytes)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __i2c_raw_rw ( AWBL_I2C_PARENT_GET(p_dev),
                          addr,
                          flags,
                          (void*)p_buf,
                          nbytes,
                          AW_FALSE);
}


aw_err_t aw_i2c_raw_read ( aw_i2c_device_t   *p_dev,
                           void              *p_buf,
                           size_t             nbytes)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __i2c_raw_rw ( __i2c_master_find_by_id(p_dev->busid),
                          p_dev->addr,
                          p_dev->flags,
                          p_buf,
                          nbytes,
                          AW_TRUE);
}


aw_err_t awbl_i2c_raw_read (struct awbl_i2c_device   *p_dev,
                            uint16_t                  addr,
                            uint16_t                  flags,
                            void                     *p_buf,
                            size_t                    nbytes)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __i2c_raw_rw ( AWBL_I2C_PARENT_GET(p_dev),
                          addr,
                          flags,
                          p_buf,
                          nbytes,
                          AW_TRUE);
}


aw_err_t aw_i2c_set_cfg (uint8_t busid, struct aw_i2c_config *p_cfg )
{
    struct awbl_i2c_master *p_master  = __i2c_master_find_by_id(busid);
    int ret = AW_OK;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    if ((p_cfg == NULL) || (p_cfg->speed == 0)) {
        return -AW_EINVAL;
    }


    AW_MUTEX_LOCK(p_master->dev_mux, AW_SEM_WAIT_FOREVER);
    ret = p_master->p_devinfo2->pfunc_i2c_set_cfg(p_master, p_cfg);
    AW_MUTEX_UNLOCK(p_master->dev_mux);

    return ret;
}


aw_err_t aw_i2c_get_cfg (uint8_t busid, struct aw_i2c_config *p_cfg)
{
    struct awbl_i2c_master *p_master  = __i2c_master_find_by_id(busid);
    int ret = AW_OK;

    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    if ((p_cfg == NULL)) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_master->dev_mux, AW_SEM_WAIT_FOREVER);
    ret = p_master->p_devinfo2->pfunc_i2c_get_cfg(p_master, p_cfg);
    AW_MUTEX_UNLOCK(p_master->dev_mux);

    return ret;
}


aw_local aw_err_t __raw_rw_with_subaddr (struct awbl_i2c_master  *p_master,
                                         uint16_t                 addr,
                                         uint16_t                 flags,
                                         const void              *p_txbuf,
                                         size_t                   n_tx,
                                         void                    *p_buf,
                                         size_t                   nbytes,
                                         uint8_t                  is_read,
                                         uint8_t                  read_send_stop)
{
    int      ret      = AW_OK;
    struct awbl_trans_buf trans_buf[2] = {{(uint8_t*)p_txbuf, n_tx},
                                          {(uint8_t*)p_buf, nbytes}};

    AW_MUTEX_LOCK(p_master->dev_mux, AW_SEM_WAIT_FOREVER);

    if (is_read) {
        ret = p_master->p_devinfo2->pfunc_i2c_write(p_master,
                                        addr,
                                        &trans_buf[0],
                                        1,
                                        (read_send_stop ? AWBL_I2C_WRITE_STOP :
                                                        AWBL_I2C_WRITE_NO_STOP),
                                        flags);
        if (ret) {
            goto __end;
        }

        ret = p_master->p_devinfo2->pfunc_i2c_read(p_master,
                                                   addr,
                                                   &trans_buf[1],
                                                   AWBL_I2C_READ_RESTART,
                                                   flags);

    } else {

        /* ��������д  */
        ret = p_master->p_devinfo2->pfunc_i2c_write(p_master,
                                        addr,
                                        trans_buf,
                                        2,
                                        AWBL_I2C_WRITE_STOP,
                                        flags);
    }

__end:
    AW_MUTEX_UNLOCK(p_master->dev_mux);
    return ret;

}

aw_local aw_err_t __rw_with_subaddr(struct awbl_i2c_master  *p_master,
                                    uint16_t                 addr,
                                    uint16_t                 flags,
                                    const void              *p_txbuf,
                                    size_t                   n_tx,
                                    void                    *p_buf,
                                    size_t                   nbytes,
                                    uint8_t                  is_read,
                                    uint8_t                  read_send_stop)
{
    int      ret      = AW_OK;
    uint16_t dr_flags = 0;

    if (p_master == NULL) {
        return -AW_ENXIO;
    }

    ret = __i2c_check_feature(p_master, flags);

    if (ret != AW_OK) {
        return ret;
    }

    if ((p_buf == NULL) || (p_txbuf == NULL) ||
          (nbytes == 0) || (n_tx == 0)) {
        return -AW_EINVAL;;
    }

    dr_flags = flags & __DR_USE_FLAGS;

    return __raw_rw_with_subaddr(p_master,
                                 addr,
                                 dr_flags,
                                 p_txbuf,
                                 n_tx,
                                 p_buf,
                                 nbytes,
                                 is_read,
                                 read_send_stop);
}

aw_err_t aw_i2c_write_then_write (aw_i2c_device_t  *p_dev,
                                  const void       *p_buf0,
                                  size_t            n_tx0,
                                  const void       *p_buf1,
                                  size_t            n_tx1)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __rw_with_subaddr(__i2c_master_find_by_id(p_dev->busid),
                             p_dev->addr,
                             p_dev->flags,
                             p_buf0,
                             n_tx0,
                             (void*)p_buf1,
                             n_tx1,
                             AW_FALSE,
                             AW_FALSE);
}


aw_err_t awbl_i2c_write_then_write (struct awbl_i2c_device   *p_dev,
                                    uint16_t                  addr,
                                    uint16_t                  flags,
                                    const void               *p_buf0,
                                    size_t                    n_tx0,
                                    const void               *p_buf1,
                                    size_t                    n_tx1)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __rw_with_subaddr(AWBL_I2C_PARENT_GET(p_dev),
                             addr,
                             flags,
                             p_buf0,
                             n_tx0,
                             (void*)p_buf1,
                             n_tx1,
                             AW_FALSE,
                             AW_FALSE);
}


aw_err_t aw_i2c_write_then_read (aw_i2c_device_t  *p_dev,
                                 const void       *p_txbuf,
                                 size_t            n_tx,
                                 void             *p_rxbuf,
                                 size_t            n_rx,
                                 uint8_t           is_send_stop)
{
    if (AW_INT_CONTEXT ()) {
     return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
     return -AW_EINVAL;
    }

    return __rw_with_subaddr(__i2c_master_find_by_id(p_dev->busid),
                             p_dev->addr,
                             p_dev->flags,
                             p_txbuf,
                             n_tx,
                             p_rxbuf,
                             n_rx,
                             AW_TRUE,
                             is_send_stop);
}


aw_err_t awbl_i2c_write_then_read (struct awbl_i2c_device  *p_dev,
                                   uint16_t                 addr,
                                   uint16_t                 flags,
                                   const void              *p_txbuf,
                                   size_t                   n_tx,
                                   void                    *p_rxbuf,
                                   size_t                   n_rx,
                                   uint8_t                  is_send_stop)
{
    if (AW_INT_CONTEXT ()) {
        return -AW_ENOTSUP;
    }

    if (p_dev == NULL) {
     return -AW_EINVAL;
    }

    return __rw_with_subaddr(AWBL_I2C_PARENT_GET(p_dev),
                             addr,
                             flags,
                             p_txbuf,
                             n_tx,
                             p_rxbuf,
                             n_rx,
                             AW_TRUE,
                             is_send_stop);
}

/* end of file */


