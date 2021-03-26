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
 * \brief CRC(Cyclic Redundancy Check)ѭ������У������׼ͷ�ļ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_crc.h
 *
 * \par ʹ��ʾ��
 * \code
 *  #include "aw_crc.h"
 *
 *  int main() 
 *  {
 *
 *      uint8_t  ret;
 *      uint8_t  ndata[4] = {0x30,0x31,0x32,0x33};
 *      uint32_t crc;
 *
 *      //������һ��ģ�ͣ���ģ�;��ǳ��õ�CRC-16ģ�ͣ���WinRAR�о���ʹ�õĸ�ģ��
 *      AW_CRC_DECL(crc16,         // crcģ�� crc16
 *                  16,            // crc��� 16
 *                  0x1021,        // ���ɶ���ʽ
 *                  0x0000,        // ��ʼֵ
 *                  AW_TRUE,          // refin
 *                  AW_TRUE,          // refout
 *                  0x0000);       // xorout
 *
 *
 *       ret = AW_CRC_INIT(crc16,               // crcģ�ͣ���AW_CRC_DECL()�����
 *                         crctable16_1021_ref, // crc��ϵͳ�ģ�����Ҫ����
 *                         AW_CRC_FLAG_AUTO)    // �Զ�ģʽ���Զ�ѡ��Ӳ�������ʵ��
 *      if (ret != AW_OK) {
 *          //��ʼ��ʧ��
 *      } else {
 *          AW_CRC_CAL(crc16,    // crcģ�ͣ���AW_CRC_DECL()�����
 *                     ndata,   // ������CRC���������
 *                     4);   // �˴μ��������
 *
 *          //�м���Լ�������
 *          // ...
 *
 *
 *          //��������ȡ�����ս��
 *          crc = AW_CRC_FINAL(crc16);  // crcģ�ͣ���AW_CRC_DECL()�����
 *
 *          aw_kprintf("������Ϊ:%x\n",crc);
 *
 *          //�������������㣬����Ҫ���µ���AW_CRC_INIT()��ʼ�����ټ��㡣
 *      }
 *      return 0;
 *  }
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 14-10-27  tee, first implementation
 * \endinternal
 */

#ifndef __AW_CRC_H
#define __AW_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_crc 
 * \copydoc aw_crc.h
 * @{
 */

#include "aworks.h"
#include "stdint.h"

/**
 * \name 8λ��8λ����crc table
 *
 * ��ǰ�Ѿ�֧�ֵ�ģ�ͣ�����Ѿ������ã��������½�����ֱ��ʹ�ü��ɡ�
 * �����������crctableX_Y(_ref)[256] X��Y����һ�����֡�
 *   - ����crctable������X��ʾcrc��ȣ���crc5���Ӧcrctable5
 *   - �����»��ߺ������Y��ʾpoly,���ɶ���ʽ��
 *      - 8λ��8λ���£�polyʹ��8λ��ʾ��  ��09,��ʡ��0
 *      - 9��16λ     ��polyʹ��16λ��ʾ����8005,��ʡ��0
 *      - 17 ��32λ   ��polyʹ��32λ��ʾ����04C11DB7,��ʡ��0.
 *   - ��׺��ref�ı�ʾ�ñ�Ϊ�������crcģ����refin == TRUEʱ��
 *     Ӧ�ô����׺��ref�ı�
 *
 * ��ˣ�����ʹ�õ�crcģ�ͣ�����֪����ʹ���ĸ�����crcģ��Ϊ��
 *     width = 5��poly = 0x09,refin = AW_TRUE.
 * ��ֱ��ʹ�� crctable5_09_ref ���ɡ�
 *
 * \note �����������ͬ���ʺ�������λ����CRC table��
 * @{
 */

/** \brief crc4  poly = 0x03, refin = AW_TRUE  */
aw_import aw_const uint8_t crctable4_03_ref[256];

/** \brief crc5  poly = 0x09, refin = AW_FALSE */
aw_import aw_const uint8_t crctable5_09[256];

/** \brief crc5  poly = 0x15, refin = AW_TRUE */
aw_import aw_const uint8_t crctable5_15_ref[256];

/** \brief crc5  poly = 0x05, refin = AW_TRUE */
aw_import aw_const uint8_t crctable5_05_ref[256];

/** \brief crc6  poly = 0x03, refin = AW_TRUE */
aw_import aw_const uint8_t crctable6_03_ref[256];

/** \brief crc7  poly = 0x09, refin = AW_FALSE */
aw_import aw_const uint8_t crctable7_09[256];

/** \brief crc8  poly = 0x07, refin = AW_FALSE */
aw_import aw_const uint8_t crctable8_07[256];

/** \brief crc8  poly = 0x07, refin = AW_TRUE */
aw_import aw_const uint8_t crctable8_07_ref[256];

/** \brief crc8  poly = 0x31, refin = AW_TRUE */
aw_import aw_const uint8_t crctable8_31_ref[256];

/** @} */

/**
 * \name 9λ��16λcrc table
 * @{
 */

/** \brief crc16  poly = 0x8005, refin = AW_TRUE */
aw_import aw_const uint16_t crctable16_8005_ref[256];

/** \brief crc16  poly = 0x1021, refin = AW_TRUE */
aw_import aw_const uint16_t crctable16_1021_ref[256];

/** \brief crc16  poly = 0x1021, refin = AW_FALSE */
aw_import aw_const uint16_t crctable16_1021[256];

/** \brief crc16  poly = 0x3D65, refin = AW_TRUE */
aw_import aw_const uint16_t crctable16_3d65_ref[256];

/** @} */

/**
 * \name 17λ��32λcrc table
 * @{
 */

/** \brief crc32  poly = 0x04c11db7, refin = AW_TRUE */
aw_import aw_const uint32_t crctable32_04c11db7_ref[256];

/** \brief crc32  poly = 0x04c11db7, refin = AW_FALSE */
aw_import aw_const uint32_t crctable32_04c11db7[256];

/** @} */

/**
 * \name CRC�����־������AW_CRC_INIT�������flags����
 * @{
 */
/** \brief �Զ�ģʽ���ɳ���ѡ��Ӳ���������ʵ�� */
#define AW_CRC_FLAG_AUTO           1     

/** \brief ָ����Ӳ��ʵ�� */
#define AW_CRC_FLAG_HARDWARE       2    

/** \brief ָ�������ʵ�� */ 
#define AW_CRC_FLAG_SOFTWARE       4 

/** \brief ��Ҫ����table�� */
#define AW_CRC_FLAG_CREATETAB      8     
/** @} */

/**
 * \brief CRCģ�ͽṹ�嶨��
 */
typedef struct aw_crc_pattern {
    uint8_t    width;           /**< \brief CRC��� */
    uint32_t   poly;            /**< \brief CRC���ɶ���ʽ */
    uint32_t   initvalue;       /**< \brief CRC��ʼֵ */
    aw_bool_t  refin;           /**< \brief �����ֽ�bit���� */
    aw_bool_t  refout;          /**< \brief ���CRC��bit���� */
    uint32_t   xorout;          /**< \brief ������ֵ */
} aw_crc_pattern_t;


/**
 * \brief CRC�ͻ��˽ṹ�嶨�壬�û���Ӧʹ��,��Ӧ��ֱ��ʹ��AW_CRC_DECL()����crcģ�͵Ķ���
 */
typedef struct aw_crc_client {
    aw_crc_pattern_t pattern;     /**< \brief CRCģ��            */
    uint32_t         crcvalue;    /**< \brief CRC����ֵ          */
    uint32_t        *p_table;     /**< \brief CRC table��        */
    aw_bool_t        isinit;      /**< \brief �Ƿ���ȷ��ʼ����־ */
    aw_bool_t        ishardware;  /**< \brief �Ƿ���Ӳ������     */
    void            *p_servfuncs; /**< \brief ������           */
    void            *p_arg;       /**< \brief ����������       */
} aw_crc_client_t;


/**
 * \brief CRC��ʼ���������û�����ֱ��ʹ�øú�����Ӧ��ʹ�ú�AW_CRC_INIT()��ʼ��
 *
 * \param[in] p_crc_client  ָ��CRC�ͻ��˽ṹ���ָ��
 * \param[in] crc_table     ָ��CRC���ָ��
 * \param[in] flags         CRC��־
 *
 * \retval   AW_OK       ��ʼ���ɹ�
 * \retval   AW_ERROR    ��ʼ��ʧ�ܣ�ͨ���������ڱ������������ʧ�ܣ�����
 *                       ����Ƿ�����ȷ,���ʹ����Ӳ��ģʽ��������Ϊû��������
 *                       ��ӦCRC�����Ӳ���豸��
 */
aw_err_t aw_crc_init(struct aw_crc_client *p_crc_client,
                     void                 *crc_table,
                     uint32_t              flags);

/**
 * \brief CRC���㣬 �û�����ֱ��ʹ�øú�����Ӧ��ʹ�ú�AW_CRC_CAL()���м���
 *
 * \param[in] p_crc_client  CRCģ��ʵ�壬�� AW_CRC_DECL() �� AW_CRC_DECL_STATIC()
 *                          �����
 * \param[in] p_data        ָ�������CRC���������
 * \param[in] nbytes        ���μ���ĸ���
 *
 * \return �޷���ֵ
 *
 * \note �ú겻�᷵��ֵ��crc�������ʱ������AW_CRC_FINAL()��ȡ���ս��ֵ
 */
void aw_crc_cal(struct aw_crc_client *p_crc_client,
                uint8_t              *p_data,
                uint32_t              nbytes);

/**
 * \brief ��ȡCRC�������� �û�����ֱ��ʹ�øú�����Ӧ��ʹ�ú�AW_CRC_FINAL()�õ�
 *        ���յ�CRC������
 *
 * \param[in] p_crc_client  CRCģ��ʵ�壬�� AW_CRC_DECL() �� AW_CRC_DECL_STATIC() 
 *                          �����
 *
 * \return crcֵ
 */
uint32_t aw_crc_final(struct aw_crc_client *p_crc_client);


/**
 * \brief ����һ��CRCģ��
 *
 * ���궨��һ��CRC���󣬶���֮����Ҫ����AW_CRC_INIT()��ʼ��CRC����
 * ��ʼ����ɺ󣬿���ʹ��AW_CRC_CAL()����CRCֵ��
 *
 * \param[in] crc        CRC�����Ǻ�����Ϊ�� AW_CRC_INIT()�� AW_CRC_CAL()�ĵ�
 *                       һ������
 * \param[in] width      CRC�Ŀ�ȣ��� crc16 �� width Ϊ 16.��ǰ֧�ֵ������Ϊ32.
 * \param[in] poly       CRC���ɶ���ʽ
 * \param[in] initvalue  CRC����ĳ�ʼֵ
 * \param[in] refin      �����ֽ�bit���򣨾���μ�CRCģ�ͣ�
 * \param[in] refout     ���CRC��bit���򣨾��庬��μ�CRCģ�ͣ�
 * \param[in] xorout     ������ֵ(���庬��μ�CRCģ��)
 *
 * CRCģ�Ϳ��Բμ���<http://reveng.sourceforge.net/crc-catalogue/>
 */
#define AW_CRC_DECL(crc,width,poly,initvalue,refin,refout,xorout)   \
                                struct aw_crc_client crc = \
                          {{width,poly,initvalue,refin,refout,xorout}}

/**
 * \brief ����CRCģ��(��̬)
 *
 * \param[in] crc        CRC�����Ǻ�����Ϊ�� AW_CRC_INIT()�� AW_CRC_CAL()�ĵ�
 *                       һ������
 * \param[in] width      CRC�Ŀ�ȣ��� crc16 �� width Ϊ 16.��ǰ֧�ֵ������Ϊ32.
 * \param[in] poly       CRC���ɶ���ʽ
 * \param[in] initvalue  CRC����ĳ�ʼֵ
 * \param[in] refin      �����ֽ�bit���򣨾���μ�CRCģ�ͣ�
 * \param[in] refout     ���CRC��bit���򣨾��庬��μ�CRCģ�ͣ�
 * \param[in] xorout     ������ֵ(���庬��μ�CRCģ��)
 *
 * ������ AW_CRC_DECL() �Ĺ�����ͬ���������ڣ�AW_CRC_DECL_STATIC() �ڶ���
 * ���������ڴ�ʱ��ʹ�ùؼ��� \b static �����һ��������Խ�����ʵ���������������
 * ģ����(�ļ���)�Ӷ�����ģ��֮�������������ͻ;�������ں�����ʹ�ñ��궨������
 */
#define AW_CRC_DECL_STATIC(crc,width,poly,initvalue,refin,refout,xorout)   \
                               static struct aw_crc_client crc = \
                          {{width,poly,initvalue,refin,refout,xorout}}

/**
 * \brief ��ʼ��CRCģ��
 *
 * \param[in] crc        CRCģ��ʵ�壬�� AW_CRC_DECL() �� AW_CRC_DECL_STATIC() 
 *                       �����
 * \param[in] crctable   crc�������õ�table����ǰϵͳ֧�ֵ�CRCģ�;��ṩ��
 *                       table���û�ֱ��ʹ�ö�Ӧ�ı��ɡ���Ҫע����ǣ���
 *                       ��table��ʱ�����ģ����refinΪTRUE,����Ҫ�����׺��
 *                       ref�ı�
 * \param[in] flags      CRC��־
                         - AW_CRC_FLAG_AUTO       �Զ�ģʽ���ɳ���ѡ��Ӳ������
 *                                                ���ʵ��
 *                       - AW_CRC_FLAG_HARDWARE   ָ����Ӳ������,��table����ΪNULL
 *                       - AW_CRC_FLAG_SOFTWARE   ָ�������ʵ��
 *                       - AW_CRC_FLAG_CREATETAB  ָ����Ҫ����crc����ϵͳ��֧
 *                                                ��ָ��CRCģ��ʱ�� ����˱�־��
 *                                                �����Զ���crctable�����ȷ��ֵ��
 * \retval   AW_OK       ��ʼ���ɹ�
 * \retval   AW_ERROR    ��ʼ��ʧ�ܣ�ͨ���������ڱ������������ʧ�ܣ�����
 *                       ����Ƿ�����ȷ���ʹ����Ӳ��ģʽ��������Ϊû��������
 *                       ��ӦCRC�����Ӳ���豸��
 *
 * \note �ر�أ������ǰϵͳ��֧���û���ʹ�õ�ģ�ͣ�
 *       ������Զ���һ�������飬��СΪ256������Ԫ�ؿ����CRC����
 *       ���(��CRC�����ݿ�ȣ���CRC5������Ҫһ��8λ�����������棬��ֵ��Ϊ8)
 *       һ�¡���:
 *        - crc5,  ����  һ�� uint8_t  table[256]���͵�����.
 *        - crc16, ����  һ�� uint16_t table[256]���͵�����,
 *        - crc32, ����  һ�� uint32_t table[256]���͵����顣
 *       �ں�����flags��־�У� �����־AW_CRC_FLAG_CREATETAB���ɡ�
 */
#define AW_CRC_INIT(crc,crctable,flags)   \
                    aw_crc_init(&crc,(void *)crctable,flags)


/**
 * \brief  CRC����
 *
 * \param[in] crc     CRCģ��ʵ�壬�� AW_CRC_DECL() �� AW_CRC_DECL_STATIC() �����
 * \param[in] pdata   ָ�������CRC���������
 * \param[in] nbytes  ���μ���ĸ���
 *
 * \return �޷���ֵ
 *
 * \note �ú겻�᷵��ֵ��crc�������ʱ������AW_CRC_FINAL()��ȡ���ս��ֵ
 *
 */
#define AW_CRC_CAL(crc,pdata,nbytes)    aw_crc_cal(&crc,pdata,nbytes)

/**
 * \brief  ��ȡCRC������
 *
 * \param[in] crc  CRCģ��ʵ�壬�� AW_CRC_DECL() �� AW_CRC_DECL_STATIC() �����
 *
 * \return crcֵ
 */
#define AW_CRC_FINAL(crc)     aw_crc_final(&crc)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AW_CRC_H */

/* end of file */

