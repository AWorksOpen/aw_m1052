/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief ���������
 *
 * \internal
 * \par modification history:
 * - 1.00 20-09-02  vih, first implementation
 * \endinternal
 */


#include "aworks.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "assert.h"

#include "aw_netif.h"
#include "aw_mem.h"
#include "aw_shell.h"

#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/prot/ethernet.h"
#include "lwip/ext/vnetif.h"
#include "lwip/ext/vlan.h"

#if BRIDGEIF_SUPPORT_VLAN

/******************************************************************************/
#define PFX "VLAN"

#define __LOG_FCOLOR_RED        "\x1B[31m"

#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {\
         __log(color, PFX, __LINE__, fmt, ##__VA_ARGS__);\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%4d] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)

#define __CHECK(mask, cond, exe, fmt, ...) \
    if (!(cond)) {\
        __LOG_BASE(mask, __LOG_FCOLOR_RED, \
                   "[ CHECK Failed ][%s:%d] " #cond "\n- " fmt " \n",\
                   __func__, __LINE__, ##__VA_ARGS__); \
        exe; \
    }

#define __CHECK_D(mask, a, cond, b, exe)  \
        __CHECK(mask, (a cond b), exe, "%d %d", a, b)

/******************************************************************************/
#define __MASK_ERR               1

/******************************************************************************/
uint8_t vlanid_netif_client_id = 0xFF;
/******************************************************************************/
static void __log (void         *p_color,
                   const char   *func_name,
                   int           line,
                   const char   *fmt, ...)
{
    char        log_buf[512 + 1];
    uint16_t    len;
    va_list     ap;

    va_start( ap, fmt );
    len = aw_vsnprintf((char *)log_buf, sizeof(log_buf), fmt, ap);
    va_end( ap );
    if (len >= sizeof(log_buf)) {
        aw_assert(len < sizeof(log_buf) - 1);
    }

#ifdef __VIH_DEBUG
    extern int aw_shell_printf_with_color (aw_shell_user_t *p_user,
                                           const char *color,
                                           const char *func_name,
                                           const char *fmt, ...);
    aw_shell_printf_with_color(AW_DBG_SHELL_IO, p_color, func_name, log_buf);
#else
    AW_INFOF(("%s\n", log_buf));
#endif
}

/******************************************************************************/

typedef struct {
    vlan_entry_t     *p_entry;
    uint16_t          entry_num;
    uint8_t           netif_client_id;
} vlan_ctx_t;

static vlan_ctx_t __g_vlan_ctx;

/**
 * \brief
 *
 * \param   p_entyr         vlan ��Ŀ�����ַ,���ڴ治�����ͷ�
 * \param   entry_num       ���鳤��
 *
 * \retval  AW_OK                   �����ɹ�
 * \retval  -AW_EINVAL              ��������
 */
aw_err_t vlan_init (struct netif *bridgeif,
                    vlan_entry_t *p_entry,
                    uint16_t entry_num)
{
    /* ���ȳ�ʼ�����Ź��� */
    __CHECK_D(1, bridgeif, !=, NULL, return -AW_EACCES);
    __CHECK_D(1, LWIP_NUM_NETIF_CLIENT_DATA, >, 2, return -AW_EPERM);
    if (vlanid_netif_client_id == 0xFF) {
        vlanid_netif_client_id = netif_alloc_client_data_id();
    }

    /* Ϊ NULL ��δʹ�û��� mac �� vlan */
    __g_vlan_ctx.p_entry   = p_entry;
    __g_vlan_ctx.entry_num = entry_num;

    for (int i = 0; i < entry_num; i++) {
        __CHECK_D(1, p_entry->mode, == , VLAN_ACCESS_LINK_MAC, return -AW_EINVAL);
        p_entry++;
    }

    return AW_OK;
}

aw_err_t vlan_port_add (struct netif *portif, vlan_entry_t *p_entry)
{
    __CHECK_D(1, p_entry, != , NULL, return -AW_EINVAL);
    __CHECK_D(1, portif, != , NULL, return -AW_EINVAL);
    __CHECK_D(1, p_entry->mode, == , VLAN_ACCESS_LINK_PORT, return -AW_ENOTSUP);

    netif_set_client_data(portif, __g_vlan_ctx.netif_client_id, p_entry);
    return AW_OK;
}

aw_err_t vlan_port_del (struct netif *portif)
{
    __CHECK_D(1, portif, != , NULL, return -AW_EINVAL);
    netif_set_client_data(portif, __g_vlan_ctx.netif_client_id, NULL);
    return AW_OK;
}

/**
 * \brief
 *
 * \param   p_entyr         vlan ����,���ڴ治�����ͷ�
 * \param   entry_num       ���鳤��
 *
 * \retval  AW_OK                   �����ɹ�
 * \retval  -AW_EINVAL              ��������
 */
vlan_entry_t * vlan_port_entry_get (struct netif *portif)
{
    __CHECK_D(1, portif, != , NULL, return NULL);
    return netif_get_client_data(portif, __g_vlan_ctx.netif_client_id);
}

aw_bool_t vlan_port_forward_allow (struct netif *portif, struct pbuf *p)
{
    struct netif *in_portif;

    vlan_entry_t *p_entry_in;
    vlan_entry_t *p_entry_out;
    int           src_vlan;
    int           dst_vlan;

    struct eth_addr *src, *dst;
    struct eth_hdr  *eth_hdr;

    eth_hdr = (struct eth_hdr *)p->payload;
    dst = &eth_hdr->dest;
    src = &eth_hdr->src;

    /* ���ȼ�� vlan_mac ��,���� src �˿ڵĻ��� mac �� vlan */
    p_entry_in = NULL;
    if (__g_vlan_ctx.entry_num) {
        for (int i = 0; i < __g_vlan_ctx.entry_num; i++) {
            p_entry_in = &__g_vlan_ctx.p_entry[i];
            if (memcmp(&p_entry_in->eth, src, sizeof(struct eth_addr)) == 0) {
                break;
            }
            if (i == __g_vlan_ctx.entry_num - 1) {
                p_entry_in = NULL;
            }
        }
    }

    /* ��� src �˿��޻��� mac �� vlan, ������� port �� vlan */
    if (!p_entry_in) {
        in_portif = netif_get_by_index(p->if_idx);

        if (in_portif) {
            p_entry_in = netif_get_client_data(in_portif,
                                               __g_vlan_ctx.netif_client_id);
        } else {
            /* in_portif Ϊ NULL, ��Ϊ br ����ֱ�ӷ�����,������ת��,ֱ�ӷ���, br ���� vlan �� */
            return AW_TRUE;
        }
    }
    if (p_entry_in) {
        src_vlan = p_entry_in->vid;
    } else {
        src_vlan = -1;
    }

    /* ���ȼ�� vlan_mac ��,���� dst �˿ڵĻ��� mac �� vlan */
    p_entry_out = NULL;
    if (__g_vlan_ctx.entry_num) {
        for (int i = 0; i < __g_vlan_ctx.entry_num; i++) {
            p_entry_out = &__g_vlan_ctx.p_entry[i];
            if (memcmp(&p_entry_out->eth, dst, sizeof(struct eth_addr)) == 0) {
                break;
            }
            if (i == __g_vlan_ctx.entry_num - 1) {
                p_entry_out = NULL;
            }
        }
    }

    /* ��� dst �˿��޻��� mac �� vlan, ������� port �� vlan */
    if (!p_entry_out) {
        p_entry_out = netif_get_client_data(portif,
                                            __g_vlan_ctx.netif_client_id);
    }

    if (p_entry_out) {
        dst_vlan = p_entry_out->vid;
    } else {
        dst_vlan = -1;
    }

    /* �� vlan �Ķ˿ڣ�ֻ������ vlan �ڶ˿�ͨ�� */
    if (dst_vlan == src_vlan) {
        return AW_TRUE;
    }

    return AW_FALSE;
}


#endif /* BRIDGEIF_SUPPORT_VLAN */





