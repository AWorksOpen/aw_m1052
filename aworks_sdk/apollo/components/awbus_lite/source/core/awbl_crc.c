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
 * \brief CRC(Cyclic Redundancy Check)ѭ������У��
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
 * \brief ���ʵ�ֵ�CRC32����,��������Ϊ void * ����������Ӳ������
 */
aw_err_t awbl_crc_init_soft(void *p_crc_soft,aw_crc_pattern_t *p_pattern);
aw_err_t awbl_crc_cal_soft(void *p_crc_soft,uint8_t *p_data,uint32_t nbytes);
aw_err_t awbl_crc_final_soft(void *p_crc_soft,uint32_t *p_crcvalue);

/** \brief CRC �豸��Ӧ�ṩ����3������ */
const struct awbl_crc_functions  crc32_serv_func = {
    awbl_crc_init_soft,
    awbl_crc_cal_soft,
    awbl_crc_final_soft
};

/** \brief 8λ����bit����(�ڲ�ʹ��) */
aw_local uint8_t __reflect8(uint8_t data)
{
    uint8_t value = 0;
    uint8_t i = 1;

    /* ���� bit0 �� bit7��bit1 �� bit6������ */
    for(; i < 9; i++)
    {
        if(data & 1) {
            value |= 1 << (8 - i);
        }
        data >>= 1;
    }
    return value;
}

/** \brief nλ����bit����(�ڲ�ʹ��)  */
aw_local uint32_t __reflectn(uint32_t data,uint8_t n)
{
    uint32_t value = 0;
    uint8_t i = 1;

    /* ���� bit0 �� bit7��bit1 �� bit6������ */
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
 * \brief һ��ͨ�õ�CRC table��������,void* �ɽ����������͵Ĳ���
 *        8λ��8λ���£�crc_table��ԭ��Ӧ��Ϊ��uint8_t table[256]
 *        9  ��16λ,    crc_table��ԭ��Ӧ��Ϊ��uint16_t table[256];
 *        17 ��32λ��   crc_table��ԭ��Ӧ��Ϊ��uint32_t table[256];
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
        return 1;         /* �Ƿ��Ŀ�� */
    }

    /* CRC ��ȴ���8ʱ  */
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
            /* ����32λʱ��λ���� */
            data = data & ((((1ul << (width - 1)) - 1) << 1) | 1);

            /* �����ݷ��������� */
            if (width <= 8) {
                p_table8[i] = data;
            } else if (width <= 16){
                p_table16[i] = data;
            } else if (width <= 32){
                p_table32[i] = data;
            }
        }
    } else {                               /* CRC���С��8ʱ,��Ҫ���⴦�� */

        for (i = 0; i < 256; i++) {

            temp = i;
            if (refin ) {
                temp = __reflect8(i);
            }
            data = temp << (width);        /* ���ﲻ��ȥ8 */

            for (j = 0; j < 8; j++) {

                topbit = 1ul << (8 + width -1 - j);

                if (data & topbit) {      /* ��λΪ1���������Ϊ0 */
                    data &= ~topbit;      /* ��λ�ɲ����㣬����ֻȡwidthλ���� */
                    data = data ^ (poly << (7 - j));  /* ��֤���� */
                }
            }
            if (refin ) {
                data = __reflectn(data,width);
            }
            data = data & ((1ul << width) - 1);

            if ( !refin ) {
                data <<= (8 - width);     /* ���λ���룬����Ϊ8λ ����λ��0 */
            }
            /*ΪTRUEʱ�����λ���룬Ĭ��...*/
            p_table8[i] = data;
        }
    }
    return 0;
}


/** \brief Ѱ��һ��Ӳ����CRC������������ʼ�� */
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
        return -AW_EINTR;                       /* Ѱ�ҵ�CRC������ */
    }

    /* retrieve the method supported by a timer */
    pfunc_crc = (pfunc_crc_func_get_t)awbl_dev_method_get(p_dev,
                    AWBL_METHOD_CALL(awbl_crcserv_get));/* ���÷���ID��ȡ������ */

    /*
     * pfunc_crc is invalid if this is not a crc device
     * return val AW_OK ��iterate will be continue.
     */
    if (pfunc_crc == NULL) {
        return AW_OK;
    }

    if (pfunc_crc(p_dev,&(p_info->drv_info)) == AW_OK) {    /* Ѱ�ҳɹ�  */

        /* ��ʼ��������ʼ���Ƿ�ɹ���ͬʱ������ܷ�֧�ָ�ģ�ͣ�*/
        if (p_info->drv_info.p_servfuncs->pfunc_init(p_info->drv_info.p_drv,
                                                     p_info->p_pattern) == AW_OK) {

            p_info->is_searched = AW_TRUE;

            return -AW_EINTR;     /* Ѱ�ҵ�CRC������ */
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

    if (((flags & AW_CRC_FLAG_HARDWARE) != 0) ||    /* ����ʹ��Ӳ��CRC */
        ((flags & AW_CRC_FLAG_AUTO)     != 0) ) {

        struct awbl_crc_serch_info  serch_info;

        serch_info.p_pattern   = p_pattern;
        serch_info.is_searched = AW_FALSE;

        /*
         * ���������豸��ÿ���豸�����ú���__awbl_crc_hardware_find_init��
         * ����Ϊp_crc
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

            /* ������Ӳ��ʵ�� */
            return AW_ERROR;                          /* ���ش���*/
        }
    }

    if (p_crc_table == NULL) {
        return AW_ERROR;
    }

    /* ���沿������ʹ�����ʵ�� */
    if ((flags & AW_CRC_FLAG_CREATETAB) != 0) {       /* �û����󴴽��� */
        /* ������ */
        aw_crc_table_create(p_pattern->width,
                            p_pattern->poly,
                            p_pattern->refin,
                            p_crc_table);
    }
    p_crc_client->p_table = p_crc_table;

    /* Ϊ����Ӳ��ͳһ */
    p_crc_client->p_servfuncs = (void *)&crc32_serv_func;
    p_crc_client->p_arg       = p_crc_client;           /* ���������ΪCRC�ͻ��� */

                                                        /* ���������ʼ������ */
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
      * ��ʼֵ,λ������8λʱ��Ӧ�����λ���� (ref = false )
      *                       Ӧ�����λ����(ref = true  )
      */
    if ( (!p_pattern->refin)  && p_pattern->width < 8) {

        p_crc->crcvalue = (p_pattern->initvalue << (8 - p_pattern->width)) ;

    } else {
        p_crc->crcvalue = p_pattern->initvalue;
    }

    return AW_OK;

}

/** \brief crc32 ������� */
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
     * С��8λʱ,����û�з���ʱ����������У������λ����
     * ��Ҫ��λ���������ս������λ����
     */
    if ( (!p_pattern->refin)  && p_pattern->width < 8) {
        crc >>= (8 - p_pattern->width);
    }

    /* ͨ����������ָ���� */
    if (p_pattern->refin != p_pattern->refout) {
        crc = __reflectn(crc,p_pattern->width);    /* С��8λҲ�ǰ���λ����� */
    }

    crc = crc ^ p_pattern->xorout;                 /* ������ֵ  */

    *p_crcvalue = crc;

    return AW_OK;
}

/* end of file */
