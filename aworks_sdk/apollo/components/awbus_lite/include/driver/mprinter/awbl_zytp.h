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
 * \brief ZYTP��ӡ��ͨ��������������UART��USB����
 *
 * ZYTP΢�ʹ�ӡ���ǹ�����������Ƭ���Ƽ����޹�˾�Ƴ��˶������΢�ʹ�ӡ����Ʒ��
 * �ò�Ʒ�������ƣ�֧����ʮ��������ESC/POSָ�
 *
 * \internal
 * \par modification history:
 * - 1.00 16-04-25  bob, first implementation
 * \endinternal
 */

/**
 * \addtogroup grp_awbl_drv_zytp
 * \copydetails awbl_zytp.h
 * @{
 */

/** @} grp_awbl_drv_zytp */

#ifndef __AWBL_ZYTP_H
#define __AWBL_ZYTP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_sem.h"
#include "aw_mprinter.h"
#include "io/aw_io_device.h"

#define AWBL_ZYTP_XX_NAME       "ZYTP"


/** \brief ��ӡ����ֽ */
#define AWBL_ZYTP_POS_PRINT             ((uint64_t)1 << 0)

/** \brief �س�*/
#define AWBL_ZYTP_POS_ENTER             ((uint64_t)1 << 1)

/** \brief ��ӡ������ǩ/�ڱ��ֽ����ӡ��ʼλ�� */
#define AWBL_ZYTP_POS_PAFSP             ((uint64_t)1 << 2)

/** \brief ����ǩ/�ڱ��ֽ����ӡ��ʼλ�� */
#define AWBL_ZYTP_POS_FSP               ((uint64_t)1 << 3)

/** \brief ��ӡ����ֽn�� */
#define AWBL_ZYTP_POS_PAFNP             ((uint64_t)1 << 4)

/** \brief ��ӡ����ֽn�� */
#define AWBL_ZYTP_POS_PABNP             ((uint64_t)1 << 5)

/** \brief ��ӡ����ֽn�� */
#define AWBL_ZYTP_POS_PAFNFL            ((uint64_t)1 << 6)

/** \brief ��ӡ����ֽn�� */
#define AWBL_ZYTP_POS_PAFBFL            ((uint64_t)1 << 7)

/** \brief ���ô�ӡλ��Ϊ��ӡ����� */
#define AWBL_ZYTP_POS_SPPTFLS           ((uint64_t)1 << 8)

/** \brief �����м��Ϊn�� */
#define AWBL_ZYTP_POS_SLSNP             ((uint64_t)1 << 9)

/** \brief �����м��ΪĬ��ֵ */
#define AWBL_ZYTP_POS_SLSDF             ((uint64_t)1 << 10)

/** \brief ������߾� */
#define AWBL_ZYTP_POS_SLFM              ((uint64_t)1 << 11)

/** \brief �����ұ߾� */
#define AWBL_ZYTP_POS_SRGM              ((uint64_t)1 << 12)

/** \brief �趨���հ���*/
#define AWBL_ZYTP_POS_SLFBLK            ((uint64_t)1 << 13)

/** \brief �����Ҳ��ַ���� */
#define AWBL_ZYTP_POS_SRGCSP            ((uint64_t)1 << 14)

/** \brief ���ô�ӡ������ */
#define AWBL_ZYTP_POS_SPSWD             ((uint64_t)1 << 15)

/** \brief ���þ��Դ�ӡλ�� */
#define AWBL_ZYTP_POS_SABSPP            ((uint64_t)1 << 16)

/** \brief ������Դ�ӡλ�� */
#define AWBL_ZYTP_POS_SRLTPP            ((uint64_t)1 << 17)

/** \brief �����ַ���ӡ��ʽ */
#define AWBL_ZYTP_POS_SCPT              ((uint64_t)1 << 18)

/** \brief �趨/����»��� */
#define AWBL_ZYTP_POS_SRUNDL            ((uint64_t)1 << 19)

/** \brief �趨/��������ӡ */
#define AWBL_ZYTP_POS_SRBFP             ((uint64_t)1 << 20)

/** \brief ���ô�ӡ���뷽ʽ*/
#define AWBL_ZYTP_POS_SPALIT            ((uint64_t)1 << 21)

/** \brief ����/����ߵ���ӡģʽ */
#define AWBL_ZYTP_POS_SRRVSMD           ((uint64_t)1 << 22)

/** \brief �趨/������״�ӡģʽ */
#define AWBL_ZYTP_POS_SRANTWMD          ((uint64_t)1 << 23)

/** \brief ���ô�ӡ�Ҷ� */
#define AWBL_ZYTP_POS_SPGYS             ((uint64_t)1 << 24)

/** \brief ���ô�ӡ�ٶ� */
#define AWBL_ZYTP_POS_SPTATE            ((uint64_t)1 << 25)

/** \brief ���ô�ӡ���� */
#define AWBL_ZYTP_POS_SPFONT            ((uint64_t)1 << 26)

/** \brief ����/���˳ʱ��90����ת */
#define AWBL_ZYTP_POS_SRCLKWSRT         ((uint64_t)1 << 27)

/** \brief �趨�ַ���С */
#define AWBL_ZYTP_POS_SFNTSZ            ((uint64_t)1 << 28)

/** \brief ѡ����ģʽ */
#define AWBL_ZYTP_POS_SCHNMD            ((uint64_t)1 << 29)

/** \brief ȡ������ģʽ */
#define AWBL_ZYTP_POS_RCHNMD            ((uint64_t)1 << 30)

/** \brief ѡ������ַ��� */
#define AWBL_ZYTP_POS_SINTLFNT          ((uint64_t)1 << 31)

/** \brief ѡ���ַ�����ҳ */
#define AWBL_ZYTP_POS_SFNTCDP           ((uint64_t)1 << 32)

/** \brief ѡ���ӡֽ��������ֹͣ��ӡ */
#define AWBL_ZYTP_POS_SSNRTSP           ((uint64_t)1 << 33)

/** \brief ����/��ֹ��尴��*/
#define AWBL_ZYTP_POS_SRBLKKY           ((uint64_t)1 << 34)

/** \brief ͼ�δ�ֱȡģ������� */
#define AWBL_ZYTP_POS_GDMVP             ((uint64_t)1 << 35)

/** \brief ͼƬˮƽȡģ���ݴ�ӡ */
#define AWBL_ZYTP_POS_GLMDP             ((uint64_t)1 << 36)

/** \brief ��ӡNVλͼ*/
#define AWBL_ZYTP_POS_PNVIMG            ((uint64_t)1 << 37)

/** \brief ����NVλͼ */
#define AWBL_ZYTP_POS_DLNVIMG           ((uint64_t)1 << 38)

/** \brief ˮƽ�Ʊ� */
#define AWBL_ZYTP_POS_LVLCTTAB          ((uint64_t)1 << 39)

/** \brief ����ˮƽ�Ʊ�λ�� */
#define AWBL_ZYTP_POS_LVLCTTABP         ((uint64_t)1 << 40)

/** \brief ��ӡ90���� */
#define AWBL_ZYTP_POS_PRGHTANG          ((uint64_t)1 << 41)

/** \brief ����һά����ɶ��ַ���HRI����ӡλ�� */
#define AWBL_ZYTP_POS_SHRIPP            ((uint64_t)1 << 42)

/** \brief ����һά����߶� */
#define AWBL_ZYTP_POS_SODCH             ((uint64_t)1 << 43)

/** \brief ����һά������*/
#define AWBL_ZYTP_POS_SODCW             ((uint64_t)1 << 44)

/** \brief ����һά��������*/
#define AWBL_ZYTP_POS_SODCF             ((uint64_t)1 << 45)

/** \brief ��ӡһά���� */
#define AWBL_ZYTP_POS_PODC              ((uint64_t)1 << 46)

/** \brief ����/��ӡ��ά��*/
#define AWBL_ZYTP_POS_SPQRC             ((uint64_t)1 << 47)

/** \brief ��ǩֽ/�ڱ�ָֽ��*/
#define AWBL_ZYTP_POS_SLBABLKP          ((uint64_t)1 << 48)

/** \brief ���õ���ֵ */
#define AWBL_ZYTP_POS_SADJVAL           ((uint64_t)1 << 49)

/** \brief ��ѯ��ӡ��״̬��ʵʱ��*/
#define AWBL_ZYTP_POS_RTQPS             ((uint64_t)1 << 50)

/** \brief ����/ȡ����ӡ��״̬�Զ�����*/
#define AWBL_ZYTP_POS_SRPSAB            ((uint64_t)1 << 51)

/** \brief ����״̬����ʵʱ�� */
#define AWBL_ZYTP_POS_DLSTAT            ((uint64_t)1 << 52)

/** \brief ��ʼ����ӡ�� */
#define AWBL_ZYTP_POS_PINIT             ((uint64_t)1 << 53)

/** \brief ���ӡ���棨ʵʱ�� */
#define AWBL_ZYTP_POS_RLCLPBUF          ((uint64_t)1 << 54)

/** \brief ��ֽ����ֽ */
#define AWBL_ZYTP_POS_FDCTPAP           ((uint64_t)1 << 55)

/** \brief ���ô���ͨѶ���� */
#define AWBL_ZYTP_POS_SCMMPAP           ((uint64_t)1 << 56)

/** \brief ѡ���ӡ����ģʽ */
#define AWBL_ZYTP_POS_SPCTLMD           ((uint64_t)1 << 57)

/** \brief ѡ���ӡͷ����ģʽ */
#define AWBL_ZYTP_POS_SPHCTLMD          ((uint64_t)1 << 58)

/** \brief ���ʹ�ӡ��ID */
#define AWBL_ZYTP_POS_DLPRTID           ((uint64_t)1 << 59)

/** \brief ����/�˳��͹���ģʽ��ʵʱ�� */
#define AWBL_ZYTP_POS_SRLOWPMD          ((uint64_t)1 << 60)

/** \brief ����/��д��ӡ������ */
#define AWBL_ZYTP_POS_SRCFG             ((uint64_t)1 << 61)


/** \brief �ͺ�AWBL_ZYTP58_XX4A  */
#define AWBL_ZYTP58_XX4A  (                        \
                           AWBL_ZYTP_POS_PRINT     | \
                           AWBL_ZYTP_POS_ENTER     | \
                           AWBL_ZYTP_POS_PAFNP     | \
                           AWBL_ZYTP_POS_PABNP     | \
                           AWBL_ZYTP_POS_PAFNFL    | \
                           AWBL_ZYTP_POS_PAFBFL    | \
                           AWBL_ZYTP_POS_SLSNP     | \
                           AWBL_ZYTP_POS_SLSDF     | \
                           AWBL_ZYTP_POS_SLFM      | \
                           AWBL_ZYTP_POS_SRGM      | \
                           AWBL_ZYTP_POS_SABSPP    | \
                           AWBL_ZYTP_POS_SPALIT    | \
                           AWBL_ZYTP_POS_SPGYS     | \
                           AWBL_ZYTP_POS_GDMVP     | \
                           AWBL_ZYTP_POS_GLMDP     | \
                           AWBL_ZYTP_POS_LVLCTTAB  | \
                           AWBL_ZYTP_POS_LVLCTTABP | \
                           AWBL_ZYTP_POS_RTQPS     | \
                           AWBL_ZYTP_POS_SRPSAB    | \
                           AWBL_ZYTP_POS_DLSTAT    | \
                           AWBL_ZYTP_POS_PINIT     | \
                           AWBL_ZYTP_POS_RLCLPBUF  | \
                           AWBL_ZYTP_POS_SCMMPAP   | \
                           AWBL_ZYTP_POS_SRLOWPMD  | \
                           AWBL_ZYTP_POS_SRCFG       \
                          )

/** \brief �ͺ�AWBL_ZYTP58_XX4B  */
#define AWBL_ZYTP58_XX4B (                         \
                          AWBL_ZYTP_POS_PRINT      | \
                          AWBL_ZYTP_POS_ENTER      | \
                          AWBL_ZYTP_POS_PAFNP      | \
                          AWBL_ZYTP_POS_PABNP      | \
                          AWBL_ZYTP_POS_PAFNFL     | \
                          AWBL_ZYTP_POS_PAFBFL     | \
                          AWBL_ZYTP_POS_SLSNP      | \
                          AWBL_ZYTP_POS_SLSDF      | \
                          AWBL_ZYTP_POS_SLFM       | \
                          AWBL_ZYTP_POS_SRGM       | \
                          AWBL_ZYTP_POS_SABSPP     | \
                          AWBL_ZYTP_POS_SCPT       | \
                          AWBL_ZYTP_POS_SPALIT     | \
                          AWBL_ZYTP_POS_SPGYS      | \
                          AWBL_ZYTP_POS_GDMVP      | \
                          AWBL_ZYTP_POS_GLMDP      | \
                          AWBL_ZYTP_POS_LVLCTTAB   | \
                          AWBL_ZYTP_POS_LVLCTTABP  | \
                          AWBL_ZYTP_POS_SHRIPP     | \
                          AWBL_ZYTP_POS_SODCH      | \
                          AWBL_ZYTP_POS_SODCW      | \
                          AWBL_ZYTP_POS_SODCF      | \
                          AWBL_ZYTP_POS_PODC       | \
                          AWBL_ZYTP_POS_RTQPS      | \
                          AWBL_ZYTP_POS_SRPSAB     | \
                          AWBL_ZYTP_POS_DLSTAT     | \
                          AWBL_ZYTP_POS_PINIT      | \
                          AWBL_ZYTP_POS_RLCLPBUF   | \
                          AWBL_ZYTP_POS_SCMMPAP    | \
                          AWBL_ZYTP_POS_SRLOWPMD   | \
                          AWBL_ZYTP_POS_SRCFG        \
                         )

/** \brief �ͺ�AWBL_ZYTP58_XX5B  */
#define AWBL_ZYTP58_XX5B  (                         \
                           AWBL_ZYTP_POS_PRINT      | \
                           AWBL_ZYTP_POS_ENTER      | \
                           AWBL_ZYTP_POS_PAFNP      | \
                           AWBL_ZYTP_POS_PABNP      | \
                           AWBL_ZYTP_POS_PAFNFL     | \
                           AWBL_ZYTP_POS_PAFBFL     | \
                           AWBL_ZYTP_POS_SLSNP      | \
                           AWBL_ZYTP_POS_SLSDF      | \
                           AWBL_ZYTP_POS_SLFM       | \
                           AWBL_ZYTP_POS_SRGM       | \
                           AWBL_ZYTP_POS_SABSPP     | \
                           AWBL_ZYTP_POS_SCPT       | \
                           AWBL_ZYTP_POS_SPALIT     | \
                           AWBL_ZYTP_POS_SPGYS      | \
                           AWBL_ZYTP_POS_SPTATE     | \
                           AWBL_ZYTP_POS_SPFONT     | \
                           AWBL_ZYTP_POS_GDMVP      | \
                           AWBL_ZYTP_POS_GLMDP      | \
                           AWBL_ZYTP_POS_LVLCTTAB   | \
                           AWBL_ZYTP_POS_LVLCTTABP  | \
                           AWBL_ZYTP_POS_SHRIPP     | \
                           AWBL_ZYTP_POS_SODCH      | \
                           AWBL_ZYTP_POS_SODCW      | \
                           AWBL_ZYTP_POS_SODCF      | \
                           AWBL_ZYTP_POS_PODC       | \
                           AWBL_ZYTP_POS_RTQPS      | \
                           AWBL_ZYTP_POS_SRPSAB     | \
                           AWBL_ZYTP_POS_DLSTAT     | \
                           AWBL_ZYTP_POS_PINIT      | \
                           AWBL_ZYTP_POS_RLCLPBUF   | \
                           AWBL_ZYTP_POS_SCMMPAP    | \
                           AWBL_ZYTP_POS_SRLOWPMD   | \
                           AWBL_ZYTP_POS_SRCFG        \
                          )

/** \brief �ͺ�AWBL_ZYTP58_XX6B  */
#define AWBL_ZYTP58_XX6B  (                        \
                           AWBL_ZYTP_POS_PRINT     | \
                           AWBL_ZYTP_POS_ENTER     | \
                           AWBL_ZYTP_POS_PAFNP     | \
                           AWBL_ZYTP_POS_PABNP     | \
                           AWBL_ZYTP_POS_PAFNFL    | \
                           AWBL_ZYTP_POS_PAFBFL    | \
                           AWBL_ZYTP_POS_SLSNP     | \
                           AWBL_ZYTP_POS_SLSDF     | \
                           AWBL_ZYTP_POS_SLFM      | \
                           AWBL_ZYTP_POS_SRGM      | \
                           AWBL_ZYTP_POS_SABSPP    | \
                           AWBL_ZYTP_POS_SCPT      | \
                           AWBL_ZYTP_POS_SPALIT    | \
                           AWBL_ZYTP_POS_SPGYS     | \
                           AWBL_ZYTP_POS_SPTATE    | \
                           AWBL_ZYTP_POS_SCHNMD    | \
                           AWBL_ZYTP_POS_RCHNMD    | \
                           AWBL_ZYTP_POS_SINTLFNT  | \
                           AWBL_ZYTP_POS_SFNTCDP   | \
                           AWBL_ZYTP_POS_GDMVP     | \
                           AWBL_ZYTP_POS_GLMDP     | \
                           AWBL_ZYTP_POS_PNVIMG    | \
                           AWBL_ZYTP_POS_DLNVIMG   | \
                           AWBL_ZYTP_POS_LVLCTTAB  | \
                           AWBL_ZYTP_POS_LVLCTTABP | \
                           AWBL_ZYTP_POS_SHRIPP    | \
                           AWBL_ZYTP_POS_SODCH     | \
                           AWBL_ZYTP_POS_SODCW     | \
                           AWBL_ZYTP_POS_SODCF     | \
                           AWBL_ZYTP_POS_PODC      | \
                           AWBL_ZYTP_POS_RTQPS     | \
                           AWBL_ZYTP_POS_SRPSAB    | \
                           AWBL_ZYTP_POS_DLSTAT    | \
                           AWBL_ZYTP_POS_PINIT     | \
                           AWBL_ZYTP_POS_RLCLPBUF  | \
                           AWBL_ZYTP_POS_SCMMPAP   | \
                           AWBL_ZYTP_POS_DLPRTID   | \
                           AWBL_ZYTP_POS_SRLOWPMD  | \
                           AWBL_ZYTP_POS_SRCFG       \
                          )

/** \brief �ͺ�AWBL_ZYTP58_XX4C  */
#define AWBL_ZYTP58_XX4C   (                        \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_PRGHTANG  | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_RLCLPBUF  | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD  | \
                            AWBL_ZYTP_POS_SRCFG       \
                           )

/** \brief �ͺ�AWBL_ZYTP58_XX4BC  */
#define AWBL_ZYTP58_XX4BC  (                        \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_RLCLPBUF  | \
                            AWBL_ZYTP_POS_FDCTPAP   | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD  | \
                            AWBL_ZYTP_POS_SRCFG       \
                           )

/** \brief �ͺ�AWBL_ZYTP80_XX4EC  */
#define AWBL_ZYTP80_XX4EC  (                        \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFSP     | \
                            AWBL_ZYTP_POS_FSP       | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_SPPTFLS   | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SLFBLK    | \
                            AWBL_ZYTP_POS_SRGCSP    | \
                            AWBL_ZYTP_POS_SPSWD     | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SRLTPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SRUNDL    | \
                            AWBL_ZYTP_POS_SRBFP     | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SRRVSMD   | \
                            AWBL_ZYTP_POS_SRANTWMD  | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_SPTATE    | \
                            AWBL_ZYTP_POS_SPFONT    | \
                            AWBL_ZYTP_POS_SRCLKWSRT | \
                            AWBL_ZYTP_POS_SFNTSZ    | \
                            AWBL_ZYTP_POS_SFNTCDP   | \
                            AWBL_ZYTP_POS_SSNRTSP   | \
                            AWBL_ZYTP_POS_SRBLKKY   | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_PNVIMG    | \
                            AWBL_ZYTP_POS_DLNVIMG   | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_SPQRC     | \
                            AWBL_ZYTP_POS_SLBABLKP  | \
                            AWBL_ZYTP_POS_SADJVAL   | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_FDCTPAP   | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SPCTLMD   | \
                            AWBL_ZYTP_POS_SPHCTLMD  | \
                            AWBL_ZYTP_POS_DLPRTID     \
                           )

/** \brief �ͺ�AWBL_ZY_TP01  */
#define AWBL_ZY_TP01      (                         \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SRLTPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_RLCLPBUF  | \
                            AWBL_ZYTP_POS_FDCTPAP   | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD    \
                           )

/** \brief �ͺ�AWBL_ZY_TP11  */
#define AWBL_ZY_TP11      (                         \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_RLCLPBUF  | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD    \
                           )

/** \brief �ͺ�AWBL_ZY_TP12  */
#define AWBL_ZY_TP12       (                        \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_RLCLPBUF  | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD    \
                           )

/** \brief �ͺ�AWBL_ZY_TP21  */
#define AWBL_ZY_TP21       (                        \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_SPTATE    | \
                            AWBL_ZYTP_POS_SCHNMD    | \
                            AWBL_ZYTP_POS_RCHNMD    | \
                            AWBL_ZYTP_POS_SINTLFNT  | \
                            AWBL_ZYTP_POS_SFNTCDP   | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_PNVIMG    | \
                            AWBL_ZYTP_POS_DLNVIMG   | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_PRGHTANG  | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_SLBABLKP  | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD    \
                           )

/**
 * \brief ״̬���ػص�����
 */
typedef int (*aw_pfn_zytp_rsp_isr_t)(void *p_arg);

/**
 * \brief ZYTP �豸��Ϣ
 */
struct awbl_zytp_devinfo {
    /** \brief ��ӡ���� */
    const char            *p_name;

    /** \brief ��ӡ���豸�ͺ� */
    uint64_t               type;
};

/**
 * \brief ZYTP �豸ʵ��
 */
struct awbl_zytp_dev {
    struct awbl_dev  dev;          /**< \brief �̳� awbl_dev �豸 */
    struct aw_iodev  io_dev;       /**< \brief io�豸 */

    /** \brief ��ѹ�ص�����               */
    aw_pfn_zytp_rsp_isr_t pfn_over_vltg;
    /** \brief ��ѹ�ص���������           */
    void                 *p_over_vltg_arg;

    /** \brief ��ѹ�ָ��ص�����               */
    aw_pfn_zytp_rsp_isr_t pfn_over_vltg_resume;
    /** \brief ��ѹ�ָ��ص���������           */
    void                 *p_over_vltg_resume_arg;

    /** \brief ѹ��ص�����               */
    aw_pfn_zytp_rsp_isr_t pfn_axo_open;
    /** \brief ѹ��ص���������           */
    void                 *p_axo_open_arg;

    /** \brief ѹ��ָ��ص�����               */
    aw_pfn_zytp_rsp_isr_t pfn_axo_close;
    /** \brief ѹ��ָ��ص���������           */
    void                 *p_axo_close_arg;

    /** \brief ���Ȼص�����               */
    aw_pfn_zytp_rsp_isr_t pfn_over_heat;
    /** \brief ���Ȼص���������           */
    void                 *p_over_heat_arg;

    /** \brief ���Ȼָ��ص�����               */
    aw_pfn_zytp_rsp_isr_t pfn_over_heat_resume;
    /** \brief ���Ȼָ��ص���������           */
    void                 *p_over_heat_resume_arg;

    /** \brief ȱֽ�ص�����               */
    aw_pfn_zytp_rsp_isr_t pfn_of_paper;
    /** \brief ȱֽ�ص���������           */
    void                 *p_of_paper_arg;

    /** \brief ȱֽ�ָ��ص�����               */
    aw_pfn_zytp_rsp_isr_t pfn_of_paper_resume;
    /** \brief ȱֽ�ָ��ص���������           */
    void                 *p_of_paper_resume_arg;


    /** \brief �е��Ƴ��ص�����               */
    aw_pfn_zytp_rsp_isr_t pfn_cutter_on;
    /** \brief �е��Ƴ��ص���������           */
    void                 *p_cutter_on_arg;

    /** \brief �е��رջָ��ص�����               */
    aw_pfn_zytp_rsp_isr_t pfn_cutter_off;
    /** \brief �е��رջָ��ص���������           */
    void                 *p_cutter_off_arg;

    /** \brief ��¼΢�ʹ�ӡ����ʵʱ״̬           */
    uint8_t               zytp_stat;

    /** \brief �Զ�״̬���ر�־��ΪTRUEʱ����ӡ��״̬���κα仯���᷵��       */
    uint8_t               auto_stat_flag;

    /** \brief ָ��΢�ʹ�ӡ�����ڴ����ָ�����           */
    aw_mprinter_params_t *p_mp_param;
};

/**
 * \brief zytp driver register
 */
void awbl_zytp_drv_register(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_ZYTP_H */

/* end of file */
