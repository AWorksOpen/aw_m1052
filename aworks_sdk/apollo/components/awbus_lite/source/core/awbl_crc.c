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
 * \brief CRC(Cyclic Redundancy Check)循环冗余校验
 *
 * \internal
 * \par modification history:
 * - 1.00 14-10-14  tee, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_crc.h"
#include "aw_vdebug.h"

/*******************************************************************************
  import
*******************************************************************************/

AWBL_METHOD_IMPORT(awbl_crcserv_get);

/*******************************************************************************
  locals
*******************************************************************************/

/**
 * \brief 软件实现的CRC32计算,参数设置为 void * 可以做到与硬件兼容
 */
aw_err_t awbl_crc_init_soft(void *p_crc_soft,aw_crc_pattern_t *p_pattern);
aw_err_t awbl_crc_cal_soft(void *p_crc_soft,uint8_t *p_data,uint32_t nbytes);
aw_err_t awbl_crc_final_soft(void *p_crc_soft,uint32_t *p_crcvalue);

/** \brief CRC 设备均应提供如下3个函数 */
const struct awbl_crc_functions  crc32_serv_func = {
    awbl_crc_init_soft,
    awbl_crc_cal_soft,
    awbl_crc_final_soft
};

/** \brief 8位数字bit反序(内部使用) */
aw_local uint8_t __reflect8(uint8_t data)
{
    uint8_t value = 0;
    uint8_t i = 1;

    /* 交换 bit0 和 bit7，bit1 和 bit6，类推 */
    for(; i < 9; i++)
    {
        if(data & 1) {
            value |= 1 << (8 - i);
        }
        data >>= 1;
    }
    return value;
}

/** \brief n位数字bit反序(内部使用)  */
aw_local uint32_t __reflectn(uint32_t data,uint8_t n)
{
    uint32_t value = 0;
    uint8_t i = 1;

    /* 交换 bit0 和 bit7，bit1 和 bit6，类推 */
    for(; i < (n + 1); i++)
    {
        if(data & 1) {
            value |= 1 << (n - i);
        }
        data >>= 1;
    }
    return value;
}

/**
 * \brief 一个通用的CRC table创建函数,void* 可接受任意类型的参数
 *        8位及8位以下，crc_table的原型应该为：uint8_t table[256]
 *        9  至16位,    crc_table的原型应该为：uint16_t table[256];
 *        17 至32位，   crc_table的原型应该为：uint32_t table[256];
 */
uint8_t aw_crc_table_create(uint8_t    width,
                            uint32_t   poly,
                            aw_bool_t  refin,
                            void      *crc_table)
{
    uint32_t temp,i,j;
    uint32_t data;

    uint32_t topbit =  (1ul << (width - 1));

    uint8_t  *p_table8   = ( uint8_t *)crc_table;
    uint16_t *p_table16  = (uint16_t *)crc_table;
    uint32_t *p_table32  = (uint32_t *)crc_table;

    if (width > 32 || width == 0) {
        return 1;         /* 非法的宽度 */
    }

    /* CRC 宽度大于8时  */
    if (width >= 8) {

        for (i = 0; i < 256; i++) {

            temp = i;
            if (refin ) {
                temp = __reflect8(i);
            }

            data = temp << (width - 8);

            for (j = 0; j < 8; j++) {

                if (data & topbit) {
                    data = (data << 1) ^ poly;
                } else {
                    data <<= 1;
                }
            }
            if (refin ) {
                data = __reflectn(data,width);
            }
            /* 避免32位时移位出界 */
            data = data & ((((1ul << (width - 1)) - 1) << 1) | 1);

            /* 将数据放入数组中 */
            if (width <= 8) {
                p_table8[i] = data;
            } else if (width <= 16){
                p_table16[i] = data;
            } else if (width <= 32){
                p_table32[i] = data;
            }
        }
    } else {                               /* CRC宽度小于8时,需要特殊处理 */

        for (i = 0; i < 256; i++) {

            temp = i;
            if (refin ) {
                temp = __reflect8(i);
            }
            data = temp << (width);        /* 这里不减去8 */

            for (j = 0; j < 8; j++) {

                topbit = 1ul << (8 + width -1 - j);

                if (data & topbit) {      /* 此位为1，将其异或为0 */
                    data &= ~topbit;      /* 该位可不清零，后面只取width位即可 */
                    data = data ^ (poly << (7 - j));  /* 保证对齐 */
                }
            }
            if (refin ) {
                data = __reflectn(data,width);
            }
            data = data & ((1ul << width) - 1);

            if ( !refin ) {
                data <<= (8 - width);     /* 向高位对齐，补齐为8位 ，低位补0 */
            }
            /*为TRUE时，向低位补齐，默认...*/
            p_table8[i] = data;
        }
    }
    return 0;
}


/** \brief 寻找一个硬件的CRC计算器，并初始化 */
aw_local aw_err_t __awbl_crc_hardware_find_init (
    struct awbl_dev  *p_dev, struct awbl_crc_serch_info  *p_info)
{
    /* to hold the Method retrieved from the AWBus device */
    pfunc_crc_func_get_t  pfunc_crc;

    /* check for the validity of the function parameters */
    if ((p_dev == NULL) || (p_info == NULL)) {
        return (AW_ERROR);
    }

    if (p_info->is_searched ) {
        return -AW_EINTR;                       /* 寻找到CRC，结束 */
    }

    /* retrieve the method supported by a timer */
    pfunc_crc = (pfunc_crc_func_get_t)awbl_dev_method_get(p_dev,
                    AWBL_METHOD_CALL(awbl_crcserv_get));/* 利用方法ID获取服务函数 */

    /*
     * pfunc_crc is invalid if this is not a crc device
     * return val AW_OK ，iterate will be continue.
     */
    if (pfunc_crc == NULL) {
        return AW_OK;
    }

    if (pfunc_crc(p_dev,&(p_info->drv_info)) == AW_OK) {    /* 寻找成功  */

        /* 初始化，检测初始化是否成功（同时检测了能否支持该模型）*/
        if (p_info->drv_info.p_servfuncs->pfunc_init(p_info->drv_info.p_drv,
                                                     p_info->p_pattern) == AW_OK) {

            p_info->is_searched = AW_TRUE;

            return -AW_EINTR;     /* 寻找到CRC，结束 */
        }
    }

    return AW_OK;
}

aw_err_t aw_crc_init(struct aw_crc_client *p_crc_client,
                     void                 *p_crc_table,
                     uint32_t              flags)
{
    aw_crc_pattern_t *p_pattern = &(p_crc_client->pattern);

    p_crc_client->isinit     = AW_FALSE;
    p_crc_client->ishardware = AW_FALSE;

    if (((flags & AW_CRC_FLAG_HARDWARE) != 0) ||    /* 优先使用硬件CRC */
        ((flags & AW_CRC_FLAG_AUTO)     != 0) ) {

        struct awbl_crc_serch_info  serch_info;

        serch_info.p_pattern   = p_pattern;
        serch_info.is_searched = AW_FALSE;

        /*
         * 遍历所有设备，每个设备均调用函数__awbl_crc_hardware_find_init，
         * 参数为p_crc
         */
        awbl_dev_iterate((awbl_iterate_func_t)__awbl_crc_hardware_find_init,
                         &serch_info,
                         AWBL_ITERATE_INSTANCES | AWBL_ITERATE_INTRABLE);

        if (serch_info.is_searched ) {

            p_crc_client->ishardware  = AW_TRUE;
            p_crc_client->isinit      = AW_TRUE;
            p_crc_client->p_servfuncs = serch_info.drv_info.p_servfuncs;
            p_crc_client->p_arg       = serch_info.drv_info.p_drv;

            return AW_OK;
        } else if ((flags & AW_CRC_FLAG_HARDWARE) != 0) {

            /* 必须是硬件实现 */
            return AW_ERROR;                          /* 返回错误*/
        }
    }

    if (p_crc_table == NULL) {
        return AW_ERROR;
    }

    /* 下面部分则是使用软件实现 */
    if ((flags & AW_CRC_FLAG_CREATETAB) != 0) {       /* 用户请求创建表 */
        /* 创建表 */
        aw_crc_table_create(p_pattern->width,
                            p_pattern->poly,
                            p_pattern->refin,
                            p_crc_table);
    }
    p_crc_client->p_table = p_crc_table;

    /* 为了与硬件统一 */
    p_crc_client->p_servfuncs = (void *)&crc32_serv_func;
    p_crc_client->p_arg       = p_crc_client;           /* 软件参数就为CRC客户端 */

                                                        /* 调用软件初始化函数 */
    if (crc32_serv_func.pfunc_init(p_crc_client,p_pattern) == AW_OK) {

        p_crc_client->isinit      = AW_TRUE;
        p_crc_client->ishardware  = AW_FALSE;

        return AW_OK;
    }

    return AW_ERROR;
}

void aw_crc_cal(struct aw_crc_client *p_crc,uint8_t *p_data,uint32_t nbytes)
{
    struct awbl_crc_functions * p_func = \
                      (struct awbl_crc_functions *)(p_crc->p_servfuncs);

    if ( (!p_crc->isinit) ){
        return;
    }

    p_func->pfunc_cal(p_crc->p_arg,p_data,nbytes);

}

uint32_t aw_crc_final(struct aw_crc_client *p_crc)
{

    struct awbl_crc_functions * p_func = \
                   (struct awbl_crc_functions *)(p_crc->p_servfuncs);

    if ( (!p_crc->isinit) ){
        return 0;
    }

    p_func->pfunc_final(p_crc->p_arg,&(p_crc->crcvalue));

    return p_crc->crcvalue;
}

aw_err_t awbl_crc_init_soft(void *p_crc_soft, aw_crc_pattern_t *p_pattern)
{

    struct aw_crc_client *p_crc = (struct aw_crc_client  *)p_crc_soft;

     /*
      * 初始值,位数不足8位时，应该向高位对齐 (ref = false )
      *                       应该向低位对齐(ref = true  )
      */
    if ( (!p_pattern->refin)  && p_pattern->width < 8) {

        p_crc->crcvalue = (p_pattern->initvalue << (8 - p_pattern->width)) ;

    } else {
        p_crc->crcvalue = p_pattern->initvalue;
    }

    return AW_OK;

}

/** \brief crc32 软件计算 */
aw_err_t awbl_crc_cal_soft (void *p_crc_soft,uint8_t *p_data,uint32_t nbytes)
{

    struct aw_crc_client *p_client  = (struct aw_crc_client  *)p_crc_soft;
    aw_crc_pattern_t     *p_pattern = &(p_client->pattern);

    uint32_t crc     = p_client->crcvalue;
    uint8_t width    = p_pattern->width;
    uint8_t refin    = p_pattern->refin;
    uint8_t *p_table = (uint8_t *) p_client->p_table;

    uint32_t bit_mask = 0;

    if ( (!p_client->isinit) ){
        return AW_ERROR;
    }

    if (width <= 8) {
        while(nbytes) {
            crc = ((uint8_t *)p_table)[crc ^ *p_data++];
            nbytes--;
        }
    } else if (width <= 16) {
        if (refin ) {
            while(nbytes) {
                crc = (crc >> 8) ^((uint16_t *)p_table) \
                                     [((crc) ^ (*p_data++)) & 0xff];
                nbytes--;
            }
        } else {
            bit_mask = ((1ul << width)- 1);
            while(nbytes) {
                crc = (crc << 8) ^((uint16_t *)p_table) \
                                    [(crc >> (width - 8)) ^ (*p_data++)];
                crc &= bit_mask;
                nbytes--;
            }
        }
    } else if (width <= 32) {

        if (refin ) {
            while(nbytes) {
                crc = (crc >> 8) ^((uint32_t *)p_table) \
                                   [((crc) ^ (*p_data++)) & 0xff];
                nbytes--;
            }
        } else {
            bit_mask =((((1ul << (width - 1)) - 1) << 1) | 1);
            while(nbytes) {
                crc = (crc << 8) ^((uint32_t *)p_table) \
                                  [(crc >> (width-8)) ^ (*p_data++)];
                crc &= bit_mask;
                nbytes--;
            }
        }
    }

    p_client->crcvalue = crc;

    return AW_OK;
}

aw_err_t awbl_crc_final_soft(void *p_crc_soft,uint32_t *p_crcvalue)
{
    struct aw_crc_client *p_client  = (struct aw_crc_client *)p_crc_soft;
    aw_crc_pattern_t     *p_pattern = &(p_client->pattern);

    uint32_t crc = p_client->crcvalue;

    if ( (!p_client->isinit) ){
        return AW_ERROR;
    }

    /*
     * 小于8位时,并且没有反向时，计算过程中，其向高位对齐
     * 需要移位，保持最终结果按低位对齐
     */
    if ( (!p_pattern->refin)  && p_pattern->width < 8) {
        crc >>= (8 - p_pattern->width);
    }

    /* 通常，不会出现该情况 */
    if (p_pattern->refin != p_pattern->refout) {
        crc = __reflectn(crc,p_pattern->width);    /* 小于8位也是按低位对齐的 */
    }

    crc = crc ^ p_pattern->xorout;                 /* 异或输出值  */

    *p_crcvalue = crc;

    return AW_OK;
}

/* end of file */
