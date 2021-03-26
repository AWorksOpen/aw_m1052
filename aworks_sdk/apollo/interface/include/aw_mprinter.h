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
 * \brief ΢�ʹ�ӡ��ͨ�ñ�׼�ӿڣ��ο�EPSON�ֲ��΢�ʹ�ӡ�������������з�װ��
 *        �û��ɸ���Ӧ����չ�����ָ��
 * \internal
 * \par Modification History
 * - 1.00 16-04-25  bob, first implementation.
 * \endinternal
 */

#ifndef __AW_MPRINTER_H
#define __AW_MPRINTER_H

#ifdef __cplpl
extern "C" {
#endif

#include "apollo.h"
#include "aw_sem.h"


/**
 * \addtogroup am_if_mprinter
 * \copydoc aw_mprinter.h
 * @{
 */

/**
 * \name �ַ���ӡ��ʽ��aw_mprinter_font_type_set�������룬�ɶ�ѡ��
 * \anchor grp_aw_mprinter_font_type
 * @{
 */

/** \brief ��ͨ           */
#define AW_MPRINTER_FONT_TYPE_NORMAL         0x01

/** \brief б��           */
#define AW_MPRINTER_FONT_TYPE_ITALIC         0x02

/** \brief �߿�           */
#define AW_MPRINTER_FONT_TYPE_BORDER         0x04

/** \brief �Ӵ�           */
#define AW_MPRINTER_FONT_TYPE_EMPRINTHASIZE  0x08

/** \brief ����           */
#define AW_MPRINTER_FONT_TYPE_DOUBLE_HEIGHT  0x10

/** \brief ����           */
#define AW_MPRINTER_FONT_TYPE_DOUBLE_WIDTH   0x20

/** \brief ����           */
#define AW_MPRINTER_FONT_TYPE_REVERSE_VEDIO  0x40

/** \brief �»���         */
#define AW_MPRINTER_FONT_TYPE_UNDERLINE      0x80

/** @} */

/**
 * \name ��ӡ��״̬��־λ��aw_mprinter_printer_state_rt_get������ȡ��
 * \anchor grp_aw_mprinter_stat_flag
 * @{
 */

/** \brief ��ѹ                    */
#define AW_MPRINTER_STAT_FLAG_OVERVOLATGE    0x01

/** \brief ���                  */
#define AW_MPRINTER_STAT_FLAG_AXOOPEN        0x02

/** \brief ȱֽ����ӡ��û�д�ӡֽ  */
#define AW_MPRINTER_STAT_FLAG_PAPER_NONE     0x04

/** \brief ����                    */
#define AW_MPRINTER_STAT_FLAG_OVERHEAT       0x08

/** \brief ��ֽ                    */
#define AW_MPRINTER_STAT_FLAG_PAPER_STUCK    0x10

/** \brief �е��Ƴ�                */
#define AW_MPRINTER_STAT_FLAG_CUTTERON       0x20

/** \brief ֽ���þ�                */
#define AW_MPRINTER_STAT_FLAG_PAPER_LACK     0x40

/** \brief �û�δȡֽ              */
#define AW_MPRINTER_STAT_FLAG_PAPER_OUT      0x80

/** @} */

/**
 * \name ��ӡ������ID��aw_mprinter_id_get������ȡ,��
 * \anchor grp_aw_mprinter_type_id
 * @{
 */

/** \brief ֧��˫�ֽڱ���                 */
#define AW_MPRINTER_TYPE_ID_DOUBLE_CODE      0x01

/** \brief ֧����ֽ����                   */
#define AW_MPRINTER_TYPE_ID_CUT_PAPER        0x02

/** \brief ����BM������                   */
#define AW_MPRINTER_TYPE_ID_BM_SENSOR        0x04

/** @} */

/**
 * \name kanji char mode��aw_mprinter_kanjichar_pmode_select�������룬�ɶ�ѡ��
 * \anchor grp_aw_mprinter_kanjichar_mode
 * @{
 */

/** \brief ����           */
#define AW_MPRINTER_KANJICHAR_MODE_DBL_WDT         0x04

/** \brief ����           */
#define AW_MPRINTER_KANJICHAR_MODE_DBL_HGT         0x08

/** \brief �»���           */
#define AW_MPRINTER_KANJICHAR_MODE_DBL_UNDERLINE   0x80

/** @} */

/**
 * \name ��ӡ��ֽ��״̬��aw_mprinter_paper_stat_get������ȡ��
 * \anchor grp_aw_mprinter_paper_stat
 * @{
 */

/** \brief ֽ�Ž�����               */
#define AW_MPRINTER_PAPER_STAT_LACK       0x03

/** \brief ȱֽ����ӡ��û��ֽ�ţ�   */
#define AW_MPRINTER_PAPER_STAT_NONE       0x0c


/** @} */


/**
 * \brief ��ӡ���ص���������
 */
typedef enum aw_mprinter_cbk_type {

    /** \brief ��ѹ               */
    AW_MPRINTER_CBK_OVER_VOLTAGE = 0,

    /** \brief ��ѹ�ָ�           */
    AW_MPRINTER_CBK_OVER_VOLTAGE_RESUME,

    /** \brief ѹ��               */
    AW_MPRINTER_CBK_AXO_OPEN,

    /** \brief ѹ��ָ�           */
    AW_MPRINTER_CBK_AXO_CLOSE,

    /** \brief ����               */
    AW_MPRINTER_CBK_OVER_HEAT,

    /** \brief ���Ȼָ�           */
    AW_MPRINTER_CBK_OVER_HEAT_RESUME,

    /** \brief ȱֽ               */
    AW_MPRINTER_CBK_OF_PAPER,

    /** \brief ȱֽ�ָ�           */
    AW_MPRINTER_CBK_OF_PAPER_RESUME,

    /** \brief �е�  �Ƴ�         */
    AW_MPRINTER_CBK_CUTTER_ON,

    /** \brief �е��ر�           */
    AW_MPRINTER_CBK_CUTTER_OFF,
} aw_mprinter_cbk_type_t;

/**
 * \brief ��ӡ���뷽ʽö��
 */
typedef enum aw_mprinter_align {

    AW_MPRINTER_ALIGN_LEFT  = 48, /**< \brief ����    */
    AW_MPRINTER_ALIGN_CENTER,     /**< \brief ����    */
    AW_MPRINTER_ALIGN_RIGHT,      /**< \brief ����    */

} aw_mprinter_align_t;

/**
 * \brief ��ӡ����ģʽ
 */
typedef enum aw_mprinter_ctrl_md {

    AW_MPRINTER_PRINT_CTRL_MODE  = 48, /**< \brief ��ӡ��ģʽ  */
    AW_MPRINTER_PRINT_CTRL_DENSITY,    /**< \brief ��ӡ���ܶ�  */
    AW_MPRINTER_PRINT_CTRL_SPEED,      /**< \brief ��ӡ�ٶ�    */

} aw_mprinter_ctrl_md_t;

/**
 * \brief ѡ������ַ���
 */
typedef enum aw_mprinter_inter_char {

    AW_MPRINTER_INTER_CHAR_USA = 0, /**< \brief ����                 */
    AW_MPRINTER_INTER_CHAR_FRA,     /**< \brief ����                 */
    AW_MPRINTER_INTER_CHAR_GM,      /**< \brief �¹�                 */
    AW_MPRINTER_INTER_CHAR_UK,      /**< \brief Ӣ��                 */
    AW_MPRINTER_INTER_CHAR_DK1,     /**< \brief �����               */
    AW_MPRINTER_INTER_CHAR_SE,      /**< \brief ���                 */
    AW_MPRINTER_INTER_CHAR_IT,      /**< \brief �����               */
    AW_MPRINTER_INTER_CHAR_ES1,     /**< \brief ������               */
    AW_MPRINTER_INTER_CHAR_JP,      /**< \brief �ձ�                 */
    AW_MPRINTER_INTER_CHAR_NO,      /**< \brief Ų��                 */
    AW_MPRINTER_INTER_CHAR_DK2,     /**< \brief �����               */
    AW_MPRINTER_INTER_CHAR_ES2,     /**< \brief ��������             */
    AW_MPRINTER_INTER_CHAR_LA,      /**< \brief ��������             */
    AW_MPRINTER_INTER_CHAR_KP,      /**< \brief ����                 */
    AW_MPRINTER_INTER_CHAR_SVN,     /**< \brief ˹��������/���޵���  */
    AW_MPRINTER_INTER_CHAR_CHI,     /**< \brief �й�                 */

} aw_mprinter_inter_char_t;

/**
 * \brief �ַ�����ҳ
 */
typedef enum aw_mprinter_code_page {

    AW_MPRINTER_CODE_PAGE_PC437    = 0, /**< \brief ������ŷ�ޱ�׼    */
    AW_MPRINTER_CODE_PAGE_KATAKANA,     /**< \brief Ƭ����            */
    AW_MPRINTER_CODE_PAGE_PC850,        /**< \brief �������          */
    AW_MPRINTER_CODE_PAGE_PC860,        /**< \brief ��������          */
    AW_MPRINTER_CODE_PAGE_PC863,        /**< \brief ���ô�-����       */
    AW_MPRINTER_CODE_PAGE_PC865,        /**< \brief ��ŷ              */
    AW_MPRINTER_CHAR_PAGE_WPC1252 = 16, /**< \brief WPC1252           */
    AW_MPRINTER_CODE_PAGE_PC866,        /**< \brief ��˹����2         */
    AW_MPRINTER_CODE_PAGE_PC852,        /**< \brief ������2           */
    AW_MPRINTER_CODE_PAGE_PC858,        /**< \brief ŷ�ޱ�׼          */
    AW_MPRINTER_CODE_PAGE_SPACE = 255,  /**< \brief �հ�              */

} aw_mprinter_char_page_t;


/**
 * \brief ͼ�δ�ֱ����ͼ��ʽ
 */
typedef enum aw_mprinter_dot_type {

    /** \brief 8�㵥����    */
    AW_MPRINTER_DOT_8_SINGLE_DENSITY  = 0,

    /** \brief 8��˫����    */
    AW_MPRINTER_DOT_8_DOUBLE_DENSITY,

    /** \brief 24�㵥����   */
    AW_MPRINTER_DOT_24_SINGLE_DENSITY = 32,

    /** \brief 24��˫����   */
    AW_MPRINTER_DOT_24_DOUBLE_DENSITY,

} aw_mprinter_dot_type_t;

/**
 * \brief ͼ��ˮƽ��ӡ��ͼ��ʽ
 */
typedef enum aw_mprinter_picture_type {

    /** \brief ����ģʽ    */
    AW_MPRINTER_PICTURE_TYPE_NORMAL  = 48,

    /** \brief ����        */
    AW_MPRINTER_PICTURE_TYPE_DOUBLE_WIDTH,

    /** \brief ����        */
    AW_MPRINTER_PICTURE_TYPE_DOUBLE_HEIGHT,

    /** \brief ������    */
    AW_MPRINTER_PICTURE_TYPE_QUADRUPLE,

} aw_mprinter_picture_type_t;


/**
 * \brief ��ӡ��ͨ�Ų�����������
 */
typedef enum aw_mprinter_cmm_cfg_type {

    /** \brief ����������       */
    AW_MPRINTER_CMM_CFG_BAUDRATE_SET  = 1,

    /** \brief У��λ����   */
    AW_MPRINTER_CMM_CFG_PARITY_SET,

    /** \brief ����������   */
    AW_MPRINTER_CMM_CFG_FLOWCTRL_SET,

    /** \brief ���ݳ�������   */
    AW_MPRINTER_CMM_CFG_DATLEN_SET
} aw_mprinter_cmm_cfg_type_t;

/**
 * \brief ��ӡ������������
 */
enum aw_mprinter_flowctrl_type {

    /** \brief Ӳ��������   */
    AW_MPRINTER_FLOWCTRL_HARDWARE = 48,

    /** \brief ���������   */
    AW_MPRINTER_FLOWCTRL_SOFTWARE,
};


/**
 * \brief ��ӡ������У��λ����
 */
enum aw_mprinter_parity_type {

    /** \brief ��У��       */
    AW_MPRINTER_PARITY_NONE  = 48,

    /** \brief ��У��   */
    AW_MPRINTER_PARITY_ODD,

    /** \brief żУ��   */
    AW_MPRINTER_PARITY_EVEN,
};

/**
 * \brief ����λ
 */
enum aw_mprinter_datlen_type {

    /** \brief 7λ       */
    AW_MPRINTER_DATLEN_NONE_7BIT = 55,

    /** \brief 8λ   */
    AW_MPRINTER_DATLEN_NONE_8BIT,
};

/**
 * \brief ����״̬���ͣ���ʵʱ��
 */
typedef enum aw_mprinter_trans_stat_type {

    /** \brief ֽ�Ŵ�����״̬ */
    AW_MPRINTER_TRANS_STAT_PAPEER_SENSOR  = 1,

    /** \brief   */
    AW_MPRINTER_TRANS_STAT_DRAW_KICKOUT,

    /** \brief ��ī״̬ */
    AW_MPRINTER_TRANS_STAT_INK = 4,
} aw_mprinter_trans_stat_type_t;

/**
 * \brief ��ӡ��ID����
 */
typedef enum aw_mprinter_id {

    /** \brief ��ӡ�ͺ�ID  */
    AW_MPRINTER_ID_PRINT_MODE  = 49,

    /** \brief ����ID      */
    AW_MPRINTER_ID_TYPE,

    /** \brief �̼��汾ID  */
    AW_MPRINTER_ID_FIRMWARE_VER,
} aw_mprinter_id_t;

/**
 * \brief �»����ߴ�
 */
typedef enum aw_mprinter_underline_crude {

    /** \brief �»���һ��� */
    AW_MPRINTER_UNDERLINE_CRUDE_1  = 49,

    /** \brief �»�������� */
    AW_MPRINTER_UNDERLINE_CRUDE_2,

} aw_mprinter_underline_crude_t;

/**
 * \brief ��ӡ��������
 */
typedef enum aw_mprinter_print_font_type {

    /** \brief ����:24��24������:12��24 */
    AW_MPRINTER_PRINT_FONT_TYPE_1  = 0,

    /** \brief ����:16��16������:8��16  */
    AW_MPRINTER_PRINT_FONT_TYPE_2,

    /** \brief ����:12��12������:6��12  */
    AW_MPRINTER_PRINT_FONT_TYPE_3,

} aw_mprinter_print_font_type_t;

/**
 * \brief ��ֽ��ʽ
 */
typedef enum aw_mprinter_cut_paper_type {

    /** \brief ���� */
    AW_MPRINTER_CUT_PAPER_TYPE_HALF  = 0,

    /** \brief ȫ�� */
    AW_MPRINTER_CUT_PAPER_TYPE_ALL,

} aw_mprinter_cut_paper_type_t;

/**
 * \brief ��ӡ������ģʽ
 */
typedef enum aw_mprinter_ctrl_mode {

    /** \brief ��ӡģʽ����   */
    AW_MPRINTER_CTRL_MODE_PRINT  = 0,

    /** \brief  ���ô�ӡ�ܶ�  */
    AW_MPRINTER_CTRL_MODE_PRINT_DENSITY,

} aw_mprinter_ctrl_mode_t;

/**
 * \brief ��ӡ������ģʽ����ӡ�ٶȣ�
 */
typedef enum aw_mprinter_print_head_ctrl_mode {

    /** \brief ����ٶ�150mm/s  */
    AW_MPRINTER_PRINT_HEAD_CTRL_MODE_150  = 0,

    /** \brief ����ٶ�100mm/s  */
    AW_MPRINTER_PRINT_HEAD_CTRL_MODE_100 = 8,

    /** \brief ����ٶ�50mm/s   */
    AW_MPRINTER_PRINT_HEAD_CTRL_MODE_50 = 16,

} aw_mprinter_print_head_ctrl_mode_t;


/**
 * \brief �»���ģʽ
 */
typedef enum aw_mprinter_underline_mode {

    /** \brief ����»���ģʽ  */
    AW_MPRINTER_UNDERLINE_OFF,

    /** \brief �趨�»���ģʽ��1��֣�  */
    AW_MPRINTER_UNDERLINE_1_DOT,

    /** \brief �趨�»���ģʽ��2��֣�  */
    AW_MPRINTER_UNDERLINE_2_DOT,

} aw_mprinter_underline_mode_t;

/**
 * \brief ��ӡ����
 */
typedef enum aw_mprinter_char_font {

    /** \brief ��ӡ����A  */
    AW_MPRINTER_CHAR_FONT_A,

    /** \brief ��ӡ����B  */
    AW_MPRINTER_CHAR_FONT_B,

    /** \brief ��ӡ����C  */
    AW_MPRINTER_CHAR_FONT_C,

    /** \brief Extended font  */
    AW_MPRINTER_CHAR_FONT_EXTENDED = 97,
} aw_mprinter_char_font_t;

/**
 * \brief ���������С
 */
typedef enum aw_mprinter_char_size {

    /** \brief ��ͨģʽ  */
    AW_MPRINTER_CHAR_SIZE_NORMAL = 0,

    /** \brief ����  */
    AW_MPRINTER_CHAR_SIZE_DBL_HGT,

    /** \brief ����  */
    AW_MPRINTER_CHAR_SIZE_DBL_WDT = 16,

    /** \brief ���߱���  */
    AW_MPRINTER_CHAR_SIZE_DBL_HGT_DBL_WDT,
} aw_mprinter_char_size_t;


/**
 * \brief λͼ��ӡģʽ
 */
typedef enum aw_mprinter_bmg_ptype {

    /** \brief ��ͨģʽ  */
    AW_MPRINTER_BMG_PTYPE_NORMAL = 0x01,

    /** \brief ����  */
    AW_MPRINTER_BMG_PTYPE_DBL_WDT,

    /** \brief ����  */
    AW_MPRINTER_BMG_PTYPE_DBL_HGT,

    /** \brief ���߱���  */
    AW_MPRINTER_BMG_PTYPE_DBL_HGT_DBL_WDT,
} aw_mprinter_bmg_ptype_t;

/**
 * \brief ��ӡλ������Ϊ��ӡ�����
 */
typedef enum aw_mprinter_ppos_beg {

    /** \brief ɾ����ӡ�������е��������ݺ����ô�ӡλ��Ϊ��ӡ��ʼ��  */
    AW_MPRINTER_PPOS_BEG_CANCEL,

    /** \brief ����ӡ�������е��������ݴ�ӡ�����ô�ӡλ��Ϊ��ӡ����ʼ��  */
    AW_MPRINTER_PPOS_BEG_PRINT,

} aw_mprinter_ppos_beg_t;

/**
 * \brief �����ַ�Ч��
 */
typedef enum aw_mprinter_effect_type {

    /** \brief �����ַ���ɫ  */
    AW_MPRINTER_EFFECT_CHAR_COLOR,

    /** \brief ���ñ���ɫ  */
    AW_MPRINTER_EFFECT_BACKGROUND_COLOR,

    /** \brief ��Ӱģʽ��  */
    AW_MPRINTER_EFFECT_SHADING_ON,

    /** \brief ��Ӱģʽ��  */
    AW_MPRINTER_EFFECT_SHADING_OFF,

} aw_mprinter_char_effect_type_t;

/**
 * \brief ҳģʽ�´�ӡ��������
 */
typedef enum aw_mprinter_pdir_type {

    /** \brief �����Ҵ�ӡ */
    AW_MPRINTER_PDIR_LEFT_TO_RIGHT,

    /** \brief �������ϴ�ӡ  */
    AW_MPRINTER_PDIR_BOTTOM_TO_TOP,

    /** \brief ���ҵ����ӡ  */
    AW_MPRINTER_PDIR_RIGHT_TO_LEFT,

    /** \brief �������´�ӡ  */
    AW_MPRINTER_PDIR_TOP_TO_BOTTOM,

} aw_mprinter_pdir_type_t;

/**
 * \brief ��ֽ����
 */
typedef enum aw_mprinter_cut_pape_type {

    /** \brief Executes a full cut (cuts the paper comprintletely). */
    AW_MPRINTER_CUT_PAPE_A,

    /** \brief Executes a partial cut (one point left uncut). */
    AW_MPRINTER_CUT_PAPE_A_HALF,

    /** \brief Feeds paper to and executes a full cut.*/
    AW_MPRINTER_CUT_PAPE_B = 65,

    /** \brief Feeds paper to and executes a partial cut. */
    AW_MPRINTER_CUT_PAPE_B_HALF,

    /** \brief Specifies a paper cutting range to and executes a full cut. */
    AW_MPRINTER_CUT_PAPE_C = 97,

    /** \brief Specifies a paper cutting range to and executes a partial cut. */
    AW_MPRINTER_CUT_PAPE_C_HALF,

    /** \brief Feeds paper to and executes a full cut, then feeds paper to the
     *         print start position. */
    AW_MPRINTER_CUT_PAPE_D = 103,

    /** \brief Feeds paper to and executes a partial cut, then feeds paper to
     *         the print start position. */
    AW_MPRINTER_CUT_PAPE_D_HALF,

} aw_mprinter_cut_pape_type_t;

/**
 * \brief ��ӡ������ģʽ,��������ɶ��ַ���HRI����ӡλ��
 */
typedef enum aw_mprinter_hri_site {

    /** \brief ����ӡ       */
    AW_MPRINTER_HRI_SITE_NONE  = 48,

    /** \brief �Ϸ�         */
    AW_MPRINTER_HRI_SITE_TOP,

    /** \brief �·�         */
    AW_MPRINTER_HRI_SITE_BELOW ,

    /** \brief �Ϸ����·�   */
    AW_MPRINTER_HRI_SITE_TOP_BELOW,

} aw_mprinter_hri_site_t;

/**
 * \brief һά��������
 */
typedef enum aw_mprinter_hri_font_type {

    /** \brief 12 �� 24  */
    AW_MPRINTER_HRI_FONT_TYPE_12_24  = 0,

    /** \brief 8 �� 16   */
    AW_MPRINTER_HRI_FONT_TYPE_8_16,

    /** \brief 6 �� 12   */
    AW_MPRINTER_HRI_FONT_TYPE_6_12 ,

} aw_mprinter_hri_font_type_t;


/**
 * \brief һά������뷽ʽ
 */
typedef enum aw_mprinter_barcode_sys {

    /** \brief ����ϵͳΪUPC-A                    */
    AW_MPRINTER_BARCODE_SYS_UPCA = 65,

    /** \brief ����ϵͳΪUPC-E                    */
    AW_MPRINTER_BARCODE_SYS_UPCE,

    /** \brief ����ϵͳΪJAN13(EAN13)             */
    AW_MPRINTER_BARCODE_SYS_EAN13,

    /** \brief ����ϵͳΪJAN8(EAN8)               */
    AW_MPRINTER_BARCODE_SYS_EAN8,

    /** \brief ����ϵͳΪCODE39                   */
    AW_MPRINTER_BARCODE_SYS_CODE39,

    /** \brief ����ϵͳΪITF (Interleaved 2 of 5) */
    AW_MPRINTER_BARCODE_SYS_ITF25,

    /** \brief ����ϵͳΪCODABAR (NW-7)           */
    AW_MPRINTER_BARCODE_SYS_CODABAR,

    /** \brief ����ϵͳΪCODE93                   */
    AW_MPRINTER_BARCODE_SYS_CODE93,

    /** \brief ����ϵͳΪCODE128                  */
    AW_MPRINTER_BARCODE_SYS_CODE128,

    /** \brief ����ϵͳΪC/EAN128                 */
    AW_MPRINTER_BARCODE_SYS_EAN128,

} aw_mprinter_barcode_sys_t;


/**
 * \brief ��ά����뷽ʽ
 */
typedef enum aw_mprinter_symbol_type {

    /** \brief PDF417����  */
    AW_MPRINTER_SYMBOL_TYPE_PDF417  = 48,

    /** \brief QR����   */
    AW_MPRINTER_SYMBOL_TYPE_QR_CODE,

} aw_mprinter_symbol_type_t;

/**
 * \brief ״̬���ػص�����
 */
typedef int (*aw_pfn_printer_stat_isr_t)(void *p_arg);

/**
 * \brief ״̬�ص�����
 */
typedef struct aw_mprinter_stat_cbk {

    /** \brief �ص��������� */
    aw_mprinter_cbk_type_t     stat_cbk_type;

    /** \brief �ص�����    */
    aw_pfn_printer_stat_isr_t  pfn_callback;

    /** \brief �ص���������    */
    void                      *p_callback_arg;
} aw_mprinter_stat_cbk_t;


/**
 * \brief ��ӡ�����ݲ�������
 */
typedef struct aw_mprinter_params {

    /** \brief ����buf1, �������ӹ������������ */
    uint8_t     *p_buf1;

    /** \brief ����buf1���� */
    size_t       buf1_len;

    /** \brief ����buf2�����������ݣ�����ͼ������ */
    uint8_t     *p_buf2;

    /** \brief ����buf2���� */
    size_t       buf2_len;


    /** \brief ��Ӧ���ݻ�����������ָ��Ҫ��΢���Ӧ����*/
    uint8_t     *p_rsp_buf;

    /** \brief �����Ӧ���� */
    size_t       rsp_len;

    /** \brief �յ���Ӧ���� */
    size_t       ret_len;

    /** \brief ��Ӧ��ʱʱ�� */
    int          timeout;

    /** \brief �ź���ID */
    aw_semb_id_t sem_id;
} aw_mprinter_params_t;


/**
 * \brief ��ӡ����ֽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note ����ӡ����������ݴ�ӡ��֮����ݵ�ǰ���м�����ý�ֽһ�У���������ӡλ
 *       ������һ�е���ʼλ��(�ɲο�EPSON΢��ָ��LF Print and line feed)
 */
aw_err_t aw_mprinter_print_and_feed (int fd);

/**
 * \brief ҳģʽ�ع��׼ģʽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note (�ɲο�EPSON΢��ָ��FF (in page mode) Print and return to standard mode
 *       (in page mode))
 */
aw_err_t aw_mprinter_page_stdmd_enter (int fd);

/**
 * \brief �س�����ӡλ�õ�����������ʼλ�ã�����ֽ��
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��CR Print and carriage return
 */
aw_err_t aw_mprinter_print_and_enter (int fd);

/**
 * \brief ҳģʽ�´�ӡ����
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note    ESC FF Print data in page mode
 */
aw_err_t aw_mprinter_page_print (int fd);

/**
 * \brief ��ӡ����ֽn�㣨����ӡ����������ݴ�ӡ����ֽn�㣩
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] ndotlines      : ��ֽndotlines�㣨0~255��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC J Print and feed paper
 */
aw_err_t aw_mprinter_print_and_ndotlines_feed (int fd, uint8_t ndotlines);

/**
 * \brief ��ӡ����ֽn�㣨����ӡ��������ݴ�ӡ����ֽn�㣩
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] ndotlines      : ��ֽndotlines�㣨0~255��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC K Print and reverse feed
 */
aw_err_t aw_mprinter_print_and_ndotlines_back (int fd, uint8_t ndotlines);

/**
 * \brief ��ӡ����ֽn�У�����ӡ��������ݴ�ӡ����ֽn�У�
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] nfontlines     : ��ֽnfontlines�У�0~255��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC d Print and feed n lines
 */
aw_err_t aw_mprinter_print_and_nfontlines_feed (int fd, uint8_t nfontlines);

/**
 * \brief ��ӡ����ֽn��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] nfontlines     : ��ֽnfontlines�У�0~255��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC e Print and reverse feed n lines
 */
aw_err_t aw_mprinter_print_and_nfontlines_back (int fd, uint8_t nfontlines);

/**
 * \brief �����м��ΪĬ��ֵ��33�㣩
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC 2 Select default line spacing
 */
aw_err_t aw_mprinter_print_line_spaceing_default_set (int fd);

/**
 * \brief �����м��Ϊn��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] ndotlines      : �����м��ndotlines�㣨0~255��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC 3 Set line spacing
 */
aw_err_t aw_mprinter_print_line_spaceing_set (int fd, uint8_t ndotlines);

/**
 * \brief ҳģʽ��ɾ����ӡ����
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��CAN Cancel print data in page mode
 *
 */
aw_err_t aw_mprinter_page_data_cancel (int fd);

/**
 * \brief �����Ҳ��ַ����
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] space          : �ַ����
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC SP Set right-side character spacing
 */
aw_err_t aw_mprinter_right_spacing_set (int fd, uint8_t space);

/**
 * \brief �����ַ���ӡ��ʽ
 *
 * \param[in] fd          : ָ��zytp58_xx6b�豸�ṹ���ָ��
 * \param[in] font_type      : �ַ���ӡ��ʽ��AW_MPRINTER58_XX6B_FONT_TYPE_*��
 *                             �ɶ�������ʹ�ã��μ�
 *                             \ref grp_aw_mprinter_font_type
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC ! Select print mode(s)
 */
aw_err_t aw_mprinter_font_type_set (int fd, uint8_t font_type);

/**
 * \brief ѡ���û��Զ����ַ�
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC % Select/cancel user-defined character set
 *
 */
aw_err_t aw_mprinter_user_defch_select (int fd);

/**
 * \brief ȡ���û��Զ����ַ�
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note
 */
aw_err_t aw_mprinter_user_defch_cancel (int fd);

/**
 * \brief �����û��Զ����ַ�
 *
 * \param[in] fd                        : ΢���豸������
 * \param[in] ver_bytes                 : ��ֱ�����ֽ���
 * \param[in] char_code_beg             : �ַ���ʼ���
 * \param[in] char_code_end             : �ַ��������
 * \param[in] p_code_buf                : �Զ����ַ�����ʱ
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC & Define user-defined characters
 */
aw_err_t aw_mprinter_user_char_def (int      fd,
                                    uint8_t  ver_bytes,
                                    uint8_t  char_code_beg,
                                    uint8_t  char_code_end,
                                    uint8_t *p_code_buf);

/**
 * \brief �»���ģʽ��
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC �C Turn underline mode on/off
 */
aw_err_t aw_mprinter_underline_on (int fd);

/**
 * \brief  �»���ģʽ��
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC �C Turn underline mode on/off
 */
aw_err_t aw_mprinter_underline_off (int fd);

/**
 * \brief ȡ���û��Զ����ַ�
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] char_code      : �ַ����
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC ? Cancel user-defined characters
 */
aw_err_t aw_mprinter_user_char_canceldef (int fd, uint8_t char_code) ;

/**
 * \brief ���������ӡ�Ŀ�
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC E Turn emprinthasized mode on/off
 */
aw_err_t aw_mprinter_emprinthasized_on (int fd);

/**
 * \brief ���������ӡ�Ĺ�
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC E Turn emprinthasized mode on/off
 */
aw_err_t aw_mprinter_emprinthasized_off (int fd);

/**
 * \brief ˫�ش�ӡģʽ�Ŀ�
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC G Turn double-strike mode on/off
 */
aw_err_t aw_mprinter_dbl_strike_on (int fd);

/**
 * \brief ˫�ش�ӡģʽ�Ĺ�
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC G Turn double-strike mode on/off
 */
aw_err_t aw_mprinter_dbl_strike_off (int fd);

/**
 * \brief ѡ���ӡ����
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] font           : ��������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC M Select character font
 */
aw_err_t aw_mprinter_character_font_set (int fd, aw_mprinter_char_font_t font);

/**
 * \brief ѡ������ַ���
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] char_type      : �����ַ�
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC R Select an international character set
 */
aw_err_t aw_mprinter_gb_char_type_set (int                      fd,
                                       aw_mprinter_inter_char_t char_type);

/**
 * \brief ����/���˳ʱ��90����ת
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC V Turn 90�� clockwise rotation mode on/off
 */
aw_err_t aw_mprinter_clockwise90_rotat_set (int fd);

/**
 * \brief ѡ���ַ�����ҳ
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] code_page      : �ַ�����ҳ
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC t Select character code table
 */
aw_err_t aw_mprinter_char_page_set (int fd, aw_mprinter_char_page_t code_page);

/**
 * \brief ���õߵ���ӡģʽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC { Turn upside-down print mode on/off
 */
aw_err_t aw_mprinter_upside_down_on (int fd);

/**
 * \brief ����ߵ���ӡģʽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note  �ɲο�EPSON΢��ָ��ESC { Turn upside-down print mode on/off
 */
aw_err_t aw_mprinter_upside_down_off (int fd);

/**
 * \brief ѡ���ַ�Ч��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] type           : �ַ�Ч������
 * \param[in] color          : ��ɫ����
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( N Select character effects
 */
aw_err_t aw_mprinter_char_effects_select (int                            fd,
                                          aw_mprinter_char_effect_type_t type,
                                          uint8_t                        color);

/**
 * \brief  ѡ���ַ���С
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] size           : �ַ���С
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note  �ɲο�EPSON΢��ָ��GS ! Select character size
 */
aw_err_t aw_mprinter_char_size_set (int fd, aw_mprinter_char_size_t size);

/**
 * \brief �趨���״�ӡģʽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS B Turn white/black reverse print mode on/off
 */
aw_err_t aw_mprinter_whi_blk_reverse_on (int fd);

/**
 * \brief ������״�ӡģʽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS B Turn white/black reverse print mode on/off
 */
aw_err_t aw_mprinter_whi_blk_reverse_off (int fd);

/**
 * \brief �趨ƽ��ģʽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS b Turn smoothing mode on/off
 */
aw_err_t aw_mprinter_smoothing_mode_on (int fd);

/**
 * \brief ���ƽ��ģʽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS b Turn smoothing mode on/off
 */
aw_err_t aw_mprinter_smoothing_mode_off (int fd);

/**
 * \brief ʹ����尴ť
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC c 5 Enable/disable panel buttons
 */
aw_err_t aw_mprinter_panel_buttons_enable (int fd);

/**
 * \brief ������尴ť
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC c 5 Enable/disable panel buttons
 */
aw_err_t aw_mprinter_panel_buttons_disable (int fd);

/**
 * \brief ��ֽ�Ŵ�����ֹͣ��ӡ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC c 4 Select paper sensor(s) to stop printing
 */
aw_err_t aw_mprinter_paper_sensors_on (int fd);

/**
 * \brief ��ֽ�Ŵ�����ֹͣ��ӡ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC c 4 Select paper sensor(s) to stop printing
 */
aw_err_t aw_mprinter_paper_sensors_off (int fd);

/**
 * \brief ˮƽ�Ʊ�
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������

 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��HT Horizontal tab
 */
aw_err_t aw_mprinter_horizontal_tab (int fd);

/**
 * \brief ���þ��Դ�ӡλ�ã���ָ��ֻ�Ա�����Ч��
 *        ���к��ӡλ�ø�λΪ��ӡ��ʼλ�ã�
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] xnum           : ��ӡλ�ã��Ե�Ϊ��λ����Χ0~65535��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������

 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC $ Set absolute print position
 */
aw_err_t aw_mprinter_absolute_print_set (int fd, uint16_t xnum);

/**
 * \brief ����ˮƽ�Ʊ�λ��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] p_buf          : ˮƽ�Ʊ�λ�����ݣ���8�㵥λ
 * \param[in] data_num       : ���ݸ�����0~16��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC D Set horizontal tab positions
 */
aw_err_t aw_mprinter_horizontaltab_set (int            fd,
                                        uint8_t       *p_buf,
                                        size_t         data_num);

/**
 * \brief ҳ��ģʽ��ѡ���ӡ����
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] type           : ����
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC T Select print direction in page mode
 */
aw_err_t aw_mprinter_page_print_dir_set (int fd, aw_mprinter_pdir_type_t type);

/**
 * \brief ҳ��ģʽ��ѡ���ӡ����
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] origin_x       : ��ӡ��ԭ��ˮƽ����
 * \param[in] origin_y       : ��ӡ��ԭ�㴹ֱ����
 * \param[in] dx             : ��ӡ��ˮƽ���
 * \param[in] dy             : ��ӡ����ֱ�߶�
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC W Set print area in page mode
 */
aw_err_t aw_mprinter_page_print_area_set (int       fd,
                                          uint16_t  origin_x,
                                          uint16_t  origin_y,
                                          uint16_t  dx,
                                          uint16_t  dy);

/**
 * \brief ������Դ�ӡλ��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] post           : ���λ��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note  �ɲο�EPSON΢��ָ��ESC \ Set relative print position
 */
aw_err_t aw_mprinter_relative_ppos_set (int fd, uint16_t post);

/**
 * \brief ���ô�ӡ���뷽ʽ
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] align          : ��ӡ���뷽ʽ
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������

 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC a Select justification
 */
aw_err_t aw_mprinter_print_align_type_set (int fd, aw_mprinter_align_t align);

/**
 * \brief ҳģʽ���������������λ��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] post           : ����λ��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS $ Set absolute vertical 
 *       print position in page mode
 */
aw_err_t aw_mprinter_page_absver_ppost_set (int fd, uint8_t post);

/**
 * \brief �趨���հ���
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] space          : �հ���
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS L Set left margin
 */
aw_err_t aw_mprinter_left_spacing_set (int fd, uint16_t space);

/**
 * \brief ���ô�ӡλ��Ϊ��ӡ�����
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] type           : ��������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS T Set print position to the 
 *       beginning of print line
 */
aw_err_t aw_mprinter_ppos_begp_set (int fd, aw_mprinter_ppos_beg_t type);

/**
 * \brief ���ô�ӡ������
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] width          : �����Ϣ
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS W Set print area width
 */
aw_err_t aw_mprinter_area_width_set (int fd, uint16_t width);

/**
 * \brief ҳģʽ�����ô�ֱ���λ��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] post           : ���λ��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS \ Set relative vertical print 
 *       position in page mode
 */
aw_err_t aw_mprinter_page_vrtcl_post_set (int fd, uint16_t post);

/**
 * \brief ͼ�δ�ֱȡģ�������
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] type           : ��ͼ��ʽ
 * \param[in] hsize          : ˮƽ���������1 �� HSize �� 384��
 * \param[in] p_dataBuf      : ͼƬ���ݻ�����
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC * Select bit-image mode
 */
aw_err_t aw_mprinter_picture_printv (int                     fd,
                                     aw_mprinter_dot_type_t  type,
                                     uint16_t                hsize,
                                     uint8_t                *p_dataBuf);

/**
 * \brief ��ӡNVλͼ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \param[in] picture_id     : ָ��λͼ
 * \param[in] picture_type   : λͼ��ʽ
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��FS p Print NV bit image
 */
aw_err_t aw_mprinter_nvpicture_print (int                        fd,
                                      uint8_t                    picture_id,
                                      aw_mprinter_picture_type_t picture_type);

/**
 * \brief ����NVλͼ
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] picture_num    : ���ص�ͼƬ����
 * \param[in] p_picture_buf  : ����ͼƬ�Ļ�����
 * \param[in] len            : ͼƬ���ݳ���
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��FS q Define NV bit image
 */
aw_err_t aw_mprinter_nvpicture_download (int              fd,
                                         uint8_t          picture_num,
                                         uint8_t         *p_picture_buf,
                                         size_t           len);

/**
 * \brief ͼƬˮƽȡģ���ݴ�ӡ
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] type           : λͼ��ʽ������Ϊ��������ֵ
 * \param[in] xsize          : ˮƽ�����ֽ�����1 �� xsize �� 48��
 * \param[in] ysize          : ��ֱ���������0 �� ysize �� 65535��
 * \param[in] p_data_buf     : ͼƬ���ݻ�����ַ
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS v 0 Print raster bit image
 */
aw_err_t aw_mprinter_picture_printh (int                         fd,
                                     aw_mprinter_picture_type_t  type,
                                     uint16_t                    xsize,
                                     uint16_t                    ysize,
                                     uint8_t                    *p_data_buf);

/**
 * \brief ��������λͼ
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] x_bytes        : ˮƽ�����ֽ���
 * \param[in] y_bytes        : ��ֱ�����ֽ���
 * \param[in] p_image_buf    : λͼ��Ϣ
 *
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS * Define downloaded bit image
 */
aw_err_t aw_mprinter_download_bmg_def (int        fd,
                                       uint8_t    x_bytes,
                                       uint8_t    y_bytes,
                                       uint8_t   *p_image_buf);

/**
 * \brief ��ӡ����λͼ
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] type           : λͼ����
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��Print downloaded bit image
 */
aw_err_t aw_mprinter_download_bmg_print (int fd, aw_mprinter_bmg_ptype_t type);

/**
 * \brief ��ѯ��ӡ��״̬��ʵʱ��
 *
 * \param[in]  fd            : ΢���豸������
 * \param[out] p_state       : ��ӡ������״ֵ̬����������λ�������ɱ�ʾ���״̬
 *                             �μ�\ref grp_aw_mprinter_stat_flag
 * \param[in]  ms_time_out   : Ӧ��ʱ��msΪ��λ��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ETIME         : ���䳬ʱ
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��DLE EOT Transmit real-time status
 */
aw_err_t aw_mprinter_printer_state_rt_get (int             fd,
                                           uint8_t        *p_state,
                                           uint16_t        ms_time_out);

/**
 * \brief ������Χ�豸״̬
 *
 * \param[in]  fd            : ΢���豸������
 * \param[out] p_stat        : ״̬
 * \param[in] time_out       : ��ʱʱ��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC u Transmit peripheral device status
 */
aw_err_t aw_mprinter_prph_dev_stat_trans (int          fd,
                                          uint8_t     *p_stat,
                                          uint16_t     time_out);

/**
 * \brief ����ֽ�Ŵ�����״̬
 *
 * \param[in]  fd            : ΢���豸������
 * \param[out] p_stat        : ״̬
 * \param[out] time_out      : ��ʱʱ��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC v Transmit paper sensor status
 */
aw_err_t aw_mprinter_paper_sensor_stat_trans (int         fd,
                                              uint8_t    *p_stat,
                                              uint16_t    time_out);

/**
 * \brief ����/ȡ����ӡ��״̬�Զ�����
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] key            : ѡ��أ����ѡ��������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS a Enable/disable Automatic Status Back
 */
aw_err_t aw_mprinter_printer_state_auto_set (int fd, uint8_t key);

/**
 * \brief ����״̬����ʵʱ��
 *
 * \param[in]  fd            : ΢���豸������
 * \param[in] type          : ����״̬����
 * \param[out] p_state       : ״̬��־
 * \param[in]  time_out      : ��ʱ����λΪms
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ETIME         : ���䳬ʱ
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS r Transmit status
 */
aw_err_t aw_mprinter_paper_stat_get (int                           fd,
                                     aw_mprinter_trans_stat_type_t type,
                                     uint8_t                      *p_state,
                                     uint16_t                      time_out);

/**
 * \brief ����һά����ɶ��ַ���HRI����ӡλ��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] hri_pos        : ����HRI��ʾλ��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS H Select print position of HRI characters
 */
aw_err_t aw_mprinter_barcode_hri_pos_set (int                    fd,
                                          aw_mprinter_hri_site_t hri_pos);

/**
 * \brief ����һά��������
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] hri_font       : ������������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS f Select font for HRI characters
 */
aw_err_t aw_mprinter_barcode_hri_font_set (int                         fd,
                                           aw_mprinter_hri_font_type_t hri_font);

/**
 * \brief ����һά����߶�
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] height         : ����߶ȣ��Ե�Ϊ��λ��0~255��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS h Set bar code height
 */
aw_err_t aw_mprinter_barcode_height_set (int fd, uint8_t height);

/**
 * \brief ��ӡһά����
 *
 * \param[in] fd             : ΢���豸������
 *
 * \param[in] barcode_sys    : ���뷽ʽ
 * \param[in] p_code_buf     : ���������׵�ַ
 * \param[in] code_len       : �������ݳ���
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS k Print bar code
 */
aw_err_t aw_mprinter_barcode_print (int                       fd,
                                    aw_mprinter_barcode_sys_t barcode_sys,
                                    uint8_t                  *p_code_buf,
                                    size_t                    code_len);

/**
 * \brief ����һά������
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] width          : ����ڰ�����ȣ��Ե�Ϊ��λ����Χ��1~6��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS w Set bar code width
 */
aw_err_t aw_mprinter_barcode_width_set (int fd, uint8_t width);

/**
 * \brief ��ʼ�궨��
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS : Start/end macro definition
 */
aw_err_t aw_mprinter_macro_def_start (int fd);

/**
 * \brief �����и����֮�⣩
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC m Partial cut (three points left uncut)
 */
aw_err_t aw_mprinter_threepoint_cut (int fd);

/**
 * \brief ѡ���и�ģʽ���и�ֽ��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] type           : ��ֽ����
 * \param[in] n              : ��ֽ��ֽn��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS V Select cut mode and cut paper
 */
aw_err_t aw_mprinter_print_and_cut (int                         fd,
                                    aw_mprinter_cut_pape_type_t type,
                                    uint8_t                     n);

/**
 * \brief ����ʵʱ�������ӡ��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] type           : ��������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��DLE ENQ Send real-time request to printer
 */
aw_err_t aw_mprinter_real_time_req_send (int fd, uint8_t type);

/**
 * \brief ����͹���ģʽ��ʵʱ��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] ms_time_out    : ��ʱʱ�䣨��λΪ���룩
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ETIME         : ��ʱ
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��DLE DC4 (fn = 2) Execute power-off sequence
 */
aw_err_t aw_mprinter_printer_low_power_mode_enter (int fd, uint16_t ms_time_out);

/**
 * \brief �˳��͹���ģʽ��ʵʱ��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] ms_time_out    : ��ʱʱ�䣨��λΪ���룩
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������

 * \retval -AW_ETIME         : ��ʱ
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note  �ɲο�EPSON΢��ָ��DLE DC4 (fn = 2) Execute power-off sequence
 */
aw_err_t aw_mprinter_printer_low_power_mode_exit (int fd, uint16_t ms_time_out);

/**
 * \brief ���ӡ���棨ʵʱ��
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��DLE DC4 (fn = 8) Clear buffer (s)
 */
aw_err_t aw_mprinter_printer_buffer_clear (int fd);

/**
 * \brief ʹ����Χ�豸
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC = Select peripheral device
 */
aw_err_t aw_mprinter_prph_dev_enable (int fd);

/**
 * \brief ������Χ�豸
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC = Select peripheral device
 */
aw_err_t aw_mprinter_prph_dev_disable (int fd);

/**
 * \brief ��ʼ����ӡ��
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC @ Initialize printer
 */
aw_err_t aw_mprinter_init (int fd);

/**
 * \brief ѡ��ҳģʽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC L Select page mode
 */
aw_err_t aw_mprinter_page_mode_select (int fd);

/**
 * \brief ѡ���׼ģʽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��ESC S Select standard mode
 */
aw_err_t aw_mprinter_stand_mode_select (int fd);

/**
 * \brief ����ʵʱ���ʹ��ָ��ʵʱ��DLE DC4������
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] cmd_id         : ָ��ID(DLE DC4ʵʱָ����)
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( D Enable/disable real-time command
 */
aw_err_t aw_mprinter_real_time_cmd_enable (int fd, uint8_t cmd_id);
/**
 * \brief ����ʵʱ����
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] cmd_id         : ָ��ID
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( D Enable/disable real-time command
 */
aw_err_t aw_mprinter_real_time_cmd_disable (int fd, uint8_t cmd_id);

/**
 * \brief ѡ���ӡ����ģʽ
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] md             : ����ģʽ
 * \param[in] dat            : ��������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( K Select print control method(s)
 */
aw_err_t aw_mprinter_print_ctrl_meth_select (int                   fd,
                                             aw_mprinter_ctrl_md_t md,
                                             uint8_t               dat);

/**
 * \brief ���ʹ�ӡ��ID
 *
 * \param[in]  fd            : ΢���豸������
 * \param[in]  id_type       : id����
 * \param[out] p_id_val      : idֵ��id����ΪAW_MPRINTER_ID_TYPEʱid_val���
 *                             �μ�\ref grp_aw_mprinter_type_id
 * \param[in]  ms_time_out   : ��ʱʱ�䣨��λΪ���룩
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ETIME         : ���䳬ʱ
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS I Transmit printer ID
 */
aw_err_t aw_mprinter_id_get (int              fd,
                             aw_mprinter_id_t id_type,
                             uint8_t         *p_id_val,
                             uint16_t         ms_time_out);

/**
 * \brief ����ˮƽ��ֱ�˶���Ԫ
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] x              : ˮƽ��Ԫ���ٶȣ�
 * \param[in] y              : ��ֱ��Ԫ���ٶȣ�
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS P Set horizontal and vertical motion units
 */
aw_err_t aw_mprinter_hv_motion_unit_set (int fd, uint8_t  x, uint8_t  y);

/**
 * \brief ѡ���ִ�ӡģʽ
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] type           : ��ӡģʽ�� �μ�
 *                             \ref grp_aw_mprinter_kanjichar_mode
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��FS ! Select print mode(s) for Kanji characters
 */
aw_err_t aw_mprinter_kanjichar_pmode_select (int fd, uint8_t type);

/**
 * \brief ѡ����ģʽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��FS & Select Kanji character mode
 */
aw_err_t aw_mprinter_chinese_mode_enter (int fd);

/**
 * \brief �趨����ģʽ�»���
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] type           : �»���ģʽ
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��FS �C Turn underline mode on/off for Kanji characters
 */
aw_err_t aw_mprinter_kanjichar_underline_on (int                           fd,
                                             aw_mprinter_underline_crude_t type);

/**
 * \brief �������ģʽ�»���
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��FS �C Turn underline mode on/off for Kanji characters
 */
aw_err_t aw_mprinter_kanjichar_underline_off (int fd);

/**
 * \brief ȡ������ģʽ
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��FS . Cancel Kanji character mode
 */
aw_err_t aw_mprinter_chinese_mode_exit (int fd);

/**
 * \brief ����PDF417��ά��������������
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] vrows          : ������������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_PDF417_areavrow_set (int fd, uint8_t vrows);

/**
 * \brief ����PDF417��ά������
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] hrows          : ����
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_PDF417_areahrow_set (int fd, uint8_t hrows);

/**
 * \brief ���ö�ά�뵥Ԫ��ȴ�С
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] symbol         : ���뷽ʽ
 * \param[in] wdt            : ��ȴ�С
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_symbol_width_set (int                       fd,
                                       aw_mprinter_symbol_type_t symbol,
                                       uint8_t                   wdt);

/**
 * \brief ����PDF417��ά���и�
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] high           : �и�
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_PDF417_rowhigh_set (int fd, uint8_t high);

/**
 * \brief ���ö�ά������ȼ�
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] symbol         : ���뷽ʽ
 * \param[in] lvl            : ����ȼ�
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_symbol_errcorlvl_set (int                       fd,
                                           aw_mprinter_symbol_type_t symbol,
                                           uint8_t                   lvl);

/**
 * \brief ����PDF417��ά��Ϊ��׼ģʽ����ά�룩
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_PDF417_stdmd_set (int fd);

/**
 * \brief ����PDF417��ά��Ϊ�ض�ģʽ����ά�룩
 *
 * \param[in] fd             : ΢���豸������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_PDF417_tncatemd_set (int fd);

/**
 * \brief �������ݵ����棨��ά�룩
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] symbol         : ���뷽ʽ
 * \param[in] p_code         : ��ά������
 * \param[in] code_len       : ��ά�����ݳ���
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_symbol_data_write (int                       fd,
                                      aw_mprinter_symbol_type_t   symbol,
                                      uint8_t                    *p_code,
                                      size_t                      code_len);

/**
 * \brief ��ӡ���뻺���еĶ�ά����
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] symbol         : ���뷽ʽ
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_symbol_data_print (int                       fd,
                                        aw_mprinter_symbol_type_t symbol);

/**
 * \brief ���û�NV�ڴ�д��
 *
 * \param[in] fd               : ΢���豸������
 * \param[in] addr             : ΢���nv�ڴ�ĵ�ַ
 * \param[in] p_nv_buf         : ������
 * \param[in] len              : ���ݳ���
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��FS g 1 Write to NV user memory
 */
aw_err_t aw_mprinter_nv_usr_mem_write (int       fd,
                                     uint32_t    addr,
                                     uint8_t    *p_nv_buf,
                                     size_t      len);

/**
 * \brief ���û�NV�ڴ��ȡ
 *
 * \param[in] fd              : ΢���豸������
 * \param[in] addr            : ΢���nv�ڴ�ĵ�ַ
 * \param[in] p_nv_buf        : ������
 * \param[in] len             : ���ݳ���
 * \param[in] time_out        : ��ʱʱ��
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��FS g 2 Read from NV user memory
 */
aw_err_t aw_mprinter_nv_usr_mem_nv_read (int       fd,
                                       uint32_t    addr,
                                       uint8_t    *p_nv_buf,
                                       size_t      len,
                                       uint16_t    time_out);

/**
 * \brief �����û���������
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] cfg_type       : ��������
 * \param[in] param          : ��������
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��GS ( E Set user setup commands
 */
aw_err_t aw_mprinter_cmm_cfg_set (int                          fd,
                                aw_mprinter_cmm_cfg_type_t     cfg_type,
                                uint32_t                       param);

/**
 * \brief ��ǩֽ/�ڱ�ָֽ��
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] p_code         : �ӹ����������
 * \param[in] code_len       : ���ݳ���
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧�ִ˹���
 *
 * \note �ɲο�EPSON΢��ָ��FS ( L Select label and black mark control 
 *       function(s)
 */
aw_err_t aw_mprinter_abel_and_black_mark_set (int       fd,
                                              uint8_t  *p_code,
                                              size_t    code_len);

/**
 * \brief ���ô�ӡ��״̬�ص�����
 *
 * \param[in] fd              : ΢���豸������
 *
 * \param[in] callback_type   : �ص���������
 * \param[in] pfn_callback    : �ص�����ָ��
 * \param[in] p_callback_arg  : �ص���������,�ص�����������ʱ���ݸò���
 *
 * \retval AW_OK              : �ɹ�
 * \retval -AW_EBADF          : �豸��������
 * \retval -AW_EINVAL         : ��Ч����
 */
aw_err_t aw_mprinter_stat_callback_set (int                       fd,
                                        aw_mprinter_cbk_type_t    callback_type,
                                        aw_pfn_printer_stat_isr_t pfn_callback,
                                        void                     *p_callback_arg);

/**
 * \brief ����΢�����
 *
 * \param[in] p_cmd           : ΢�����������ӡ�����ݣ�
 * \param[in] p_buf1          : ָ������1
 * \param[in] buf1_len        : ָ������1����
 * \param[in] p_buf2          : ָ������2
 * \param[in] buf2_len        : ָ������2����
 * \param[in] p_rspbuf        : ��Ӧ���ݻ���
 * \param[in] rspbuf_len      : ��Ҫ��Ӧ��������
 * \param[in] time_out        : ��Ӧ��ʱ
 *
 * \retval ��
 *
 * \note �����յ�p_cmd��������Ȼ�����������Ч�ԣ������豸�Ƿ�֧�ָ�ָ�
 *       ͨ��У�������ν�buf1��buf2���������д��΢���豸���������������Ҫ΢
 *       ���Ӧ����ʱp_rspbuf���û�Ӧ���ݵĻ�������rspbuf_lenΪ��Ӧ���ݵĳ��ȣ�
 *       ��time_outʱ����û���յ�rspbuf_len���ȵĻ�Ӧ���ݻ������ʱ����ͨ��
 *       p_cmd->ret_len�ж��յ���Ӧ��������������������Ҫ��Ӧ������p_rspbuf
 *       ��Ϊ�ա�֮����ָ�����ݷ�Ϊbuf1��buf2������Ϊ�ӹ��������ӡ���ݲ���һ��
 *       �����Ŀռ䣬�ֿ�����������������ݰ��˲�����
 */
aw_local aw_inline
void aw_mprinter_cfg_cmd (aw_mprinter_params_t     *p_cmd,
                        uint8_t                    *p_buf1,
                        size_t                      buf1_len,
                        uint8_t                    *p_buf2,
                        size_t                      buf2_len,
                        uint8_t                    *p_rspbuf,
                        size_t                      rspbuf_len,
                        int                         time_out)
{
    p_cmd->p_buf1    = p_buf1;
    p_cmd->buf1_len  = buf1_len;
    p_cmd->p_buf2    = p_buf2;
    p_cmd->buf2_len  = buf2_len;
    p_cmd->p_rsp_buf = p_rspbuf;
    p_cmd->rsp_len   = rspbuf_len;
    p_cmd->timeout   = time_out;
}

/**
 * \brief ��չָ��ӿ�
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] p_cmd          : ��չ����
 *
 * \retval AW_OK             : �ɹ�
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧������ָ��
 */
aw_err_t aw_mprinter_features_expand (int fd, aw_mprinter_params_t *p_cmd);

/**
 * \brief ΢���豸д
 *
 * \param[in] fd             : ΢���豸������
 * \param[in] p_buf          : ���ݻ�����
 * \param[in] nbytes         : ���ݳ���
 *
 * \retval ����0             : д������ݳ���
 * \retval -AW_EINVAL        : ��Ч����
 * \retval -AW_EBADF         : �豸��������
 * \retval -AW_ENOSYS        : ������΢�ʹ�ӡ����֧������ָ��
 */
ssize_t aw_mprinter_write(int fd, const void *p_buf, size_t nbytes);

/**
 * \brief ��΢���豸
 *
 * \param[in] p_name         : ��ӡ�������ɲ鿴�����ļ����豸��
 *
 * \retval ����0             : �豸������
 * \retval С��0             : ����
 */
int aw_mprinter_dev_open (const char *p_name);

/**
 * \brief �ر�΢���豸
 *
 * \param[in] fd             : ΢���豸������
 *
 * \return ��
 */
void aw_mprinter_dev_close (int fd);

/** @} */

#ifdef __cplplus
}
#endif

#endif /* __AW_MPRINTER_H */

/* end of file */
