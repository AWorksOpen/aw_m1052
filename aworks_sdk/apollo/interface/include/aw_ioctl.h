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
 * \brief Generic ioctl commands
 *
 * \internal
 * \par modification history
 * - 1.10 14-01-17  zen, move ioctl function code to platform
 * - 1.00 12-10-25  orz, simple implementation from VxWorks and linux
 * \endinternal
 */

#ifndef __AW_IOCTL_H
#define __AW_IOCTL_H

/*
 * \name generic ioctl function codes
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


#include "aw_ioctl_generic.h"

/******************************************************************************/

/*
 * \name ioctl types
 *
 * apollo platform use type number 0~99, the rest is for user use,
 * max type number is 255
 * @{
 */
#define AW_IOC_TYPE_FIO     0   /**< \brief File ioctl command type */
#define AW_IOC_TYPE_TIO     1   /**< \brief TTY ioctl command type */
#define AW_IOC_TYPE_SIO     2   /**< \brief serial device ioctl command type */
#define AW_IOC_TYPE_MPIO    3   /**< \brief MP device ioctl command type */

#define AW_IOC_TYPE_USER    100

/** @} */

/*
 * \name file stream ioctl function codes
 * @{
 */

#define _AW_FIO(nr)            __AW_IO(AW_IOC_TYPE_FIO, nr)
#define _AW_FIOR(nr, size)     __AW_IOR(AW_IOC_TYPE_FIO, nr, size)
#define _AW_FIOW(nr, size)     __AW_IOW(AW_IOC_TYPE_FIO, nr, size)
#define _AW_FIOWR(nr, size)    __AW_IOWR(AW_IOC_TYPE_FIO, nr, size)

#define AW_FIONREAD    _AW_FIOR(1, int) /**< get num chars available to read */
#define AW_FIOFLUSH    _AW_FIO (2)      /**< flush any chars in buffers */
#define AW_FIOOPTIONS  _AW_FIOW(3, int) /**< set options (FIOSETOPTIONS) */
#define AW_FIOBAUDRATE _AW_FIOW(4, int) /**< set serial baud rate */

#define AW_FIODISKFORMAT   _AW_FIO(5)   /**< format disk */
#define AW_FIODISKINIT     _AW_FIO(6)   /**< format disk */

#define AW_FIOSEEK     _AW_FIOW(7, int) /**< set current file char position */
#define AW_FIOWHERE    _AW_FIOR(8, int) /**< get current file char position */
#define AW_FIODIRENTRY _AW_FIOR(9, int) /**< return a directory entry */
#define AW_FIORENAME   _AW_FIOW(10,int) /**< rename a directory entry */

#define AW_FIONWRITE   _AW_FIOR(12,int) /**< get num chars still to be written */

#define AW_FIOCANCEL   _AW_FIO(14) /**< cancel read or write on the device */

#define AW_FIONBIO     _AW_FIO(16) /**< set non-blocking I/O; SOCKETS ONLY!*/

#define AW_FIONMSGS    _AW_FIOR(17, int) /**< return num msgs in pipe */

#define AW_FIOISATTY   _AW_FIOR(20, int) /**< is a tty */
#define AW_FIOSYNC     _AW_FIO (21)      /**< sync to disk */

#define AW_FIORBUFSET  _AW_FIOW(24, int) /**< alter the size of read buffer  */
#define AW_FIOWBUFSET  _AW_FIOW(25, int) /**< alter the size of write buffer */
#define AW_FIORFLUSH   _AW_FIO (26)      /**< flush any chars in read buffers */
#define AW_FIOWFLUSH   _AW_FIO (27)      /**< flush any chars in write buffers */
#define AW_FIOSELECT   _AW_FIO (28)     /**< wake up process in select on I/O */
#define AW_FIOUNSELECT _AW_FIO (29)     /**< wake up process in select on I/O */
#define AW_FIONFREE    _AW_FIOR(30, int)/**< get free byte count on device */
#define AW_FIOMKDIR    _AW_FIOW(31, int)/**< create a directory */
#define AW_FIORMDIR    _AW_FIOW(32, int)/**< remove a directory */

#define AW_FIOREADDIR  _AW_FIOR(37, int)/**< read a directory entry (POSIX) */

#define AW_FIOTRUNC    _AW_FIOW(42, int)/**< truncate file to specified length(POSIX) */
#define AW_FIOUTIME    _AW_FIOW(43, int)/**< set file access and modification times(POSIX) */
#define AW_FIOCHMOD    _AW_FIOW(44, int)/**< change mode of a file(POSIX) */

#define AW_FIOFSTATFSGET   _AW_FIOR(46, int) /**< get FS status info */

#define AW_FIODATASYNC     _AW_FIO(57)      /* sync to I/O data integrity */

#define AW_FIOLINK         _AW_FIO(58)      /* create a link */
#define AW_FIOUNLINK       _AW_FIO(59)      /* remove a link */

#define AW_FIOSTATGET      _AW_FIOR(64, int) /**< get stat info (POSIX) */
#define AW_FIOFSTATGET     _AW_FIOR(66, int) /**< get fstat info (POSIX) */

#define _AW_MPIOW(nr, size) __AW_IOW(AW_IOC_TYPE_MPIO, nr, size)
#define _AW_MPIOR(nr, size) __AW_IOR(AW_IOC_TYPE_MPIO, nr, size)

/** @} */

/******************************************************************************/

/**
 * \name serial device ioctl function codes
 * @{
 */
#define _AW_TIO(nr)        __AW_IO(AW_IOC_TYPE_TIO, nr)
#define _AW_TIOR(nr, size) __AW_IOR(AW_IOC_TYPE_TIO, nr, size)
#define _AW_TIOW(nr, size) __AW_IOW(AW_IOC_TYPE_TIO, nr, size)

#define AW_TIOPROTOHOOK    _AW_TIOW(1, int)   /**< specify protocol hook routine */
#define AW_TIOPROTOARG     _AW_TIOW(2, int)   /**< specify protocol argument */
#define AW_TIOGETOPTIONS   _AW_TIOR(3, int)   /**< get serial device options */
#define AW_TIOSETOPTIONS   _AW_TIOW(4, int)   /**< set serial device options */
#define AW_TIOISATTY       _AW_TIOR(5, int)   /**< is a tty */

#define AW_TIOCBAUDRATE    _AW_TIOW(6, int)   /**< \brief set serial baud rate */
#define AW_TIOCGRS485      _AW_TIOR(7, int)   /**< \brief Get serial RS-485 options */

/** \brief ����,�ñ�־������Ч ,485 ʹ�ܷ��ڴ��������ļ���,���485������ָ�벻ΪNULL��485ʹ��,������� */
#define AW_TIOCSRS485      _AW_TIOW(8, int)   /**< \brief Set serial RS-485 options */

#define AW_TIOCRDTIMEOUT   _AW_TIOW(9, int)   /**< \brief set serial read timeout */
#define AW_TIOCGSTATUS	   _AW_TIOW(10, int)  /**< \brief get serial status */


/**
 * \name options to AW_TIOCSRS485 , bitwise or'ed together
 * @{
 */
#define RS485_ENABLE          0x1     /* enable rs485 mode */
#define RS485_DISABLE         0x0     /* disable rs485 mode */

/**
 * \name status return by AW_TIOCGSTATUS
 * @{
 */
#define SERIAL_IDLE          0x0     /* serial device idle */
#define SERIAL_BUSY          0x1     /* serial device busy */


/**
 * \name mp device function codes
 * @{
 */

/** \brief ��ӡ����ֽ */
#define AW_MPCPRINT             _AW_MPIOW(1, int)

/** \brief �س�*/
#define AW_MPCENTER             _AW_MPIOR(2, int)

/** \brief ��ӡ������ǩ/�ڱ��ֽ����ӡ��ʼλ�� */
#define AW_MPCPAFSP             _AW_MPIOW(3, int)

/** \brief ����ǩ/�ڱ��ֽ����ӡ��ʼλ�� */
#define AW_MPCFSP               _AW_MPIOW(4, int)

/** \brief ��ӡ����ֽn�� */
#define AW_MPCPAFNP             _AW_MPIOW(5, int)

/** \brief ��ӡ����ֽn�� */
#define AW_MPCPABNP             _AW_MPIOW(6, int)

/** \brief ��ӡ����ֽn�� */
#define AW_MPCPAFNFL            _AW_MPIOW(7, int)

/** \brief ��ӡ����ֽn�� */
#define AW_MPCPAFBFL            _AW_MPIOW(8, int)

/** \brief ���ô�ӡλ��Ϊ��ӡ����� */
#define AW_MPCSPPTFLS           _AW_MPIOW(9, int)

/** \brief �����м��Ϊn�� */
#define AW_MPCSLSNP             _AW_MPIOW(10, int)

/** \brief �����м��ΪĬ��ֵ */
#define AW_MPCSLSDF             _AW_MPIOW(11, int)

/** \brief �趨���հ���*/
#define AW_MPCSLFBLK            _AW_MPIOW(12, int)

/** \brief �����Ҳ��ַ���� */
#define AW_MPCSRGCSP            _AW_MPIOW(13, int)

/** \brief ���ô�ӡ������ */
#define AW_MPCSPSWD             _AW_MPIOW(14, int)

/** \brief ���þ��Դ�ӡλ�� */
#define AW_MPCSABSPP            _AW_MPIOW(15, int)

/** \brief ������Դ�ӡλ�� */
#define AW_MPCSRLTPP            _AW_MPIOW(16, int)

/** \brief �����ַ���ӡ��ʽ */
#define AW_MPCSCPT              _AW_MPIOW(17, int)

/** \brief �趨/����»��� */
#define AW_MPCSRUNDL            _AW_MPIOW(18, int)

/** \brief �趨/��������ӡ */
#define AW_MPCSRBFP             _AW_MPIOW(19, int)

/** \brief ���ô�ӡ���뷽ʽ*/
#define AW_MPCSPALIT            _AW_MPIOW(20, int)

/** \brief ����/����ߵ���ӡģʽ */
#define AW_MPCSRRVSMD           _AW_MPIOW(21, int)

/** \brief �趨/������״�ӡģʽ */
#define AW_MPCSRANTWMD          _AW_MPIOW(22, int)

/** \brief ���ô�ӡ���� */
#define AW_MPCSPFONT            _AW_MPIOW(23, int)

/** \brief ����/���˳ʱ��90����ת */
#define AW_MPCSRCLKWSRT         _AW_MPIOW(24, int)

/** \brief �趨�ַ���С */
#define AW_MPCSFNTSZ            _AW_MPIOW(25, int)

/** \brief ѡ����ģʽ */
#define AW_MPCSCHNMD            _AW_MPIOW(26, int)

/** \brief ȡ������ģʽ */
#define AW_MPCRCHNMD            _AW_MPIOW(27, int)

/** \brief ѡ������ַ��� */
#define AW_MPCSINTLFNT          _AW_MPIOW(28, int)

/** \brief ѡ���ַ�����ҳ */
#define AW_MPCSFNTCDP           _AW_MPIOW(29, int)

/** \brief ѡ���ӡֽ��������ֹͣ��ӡ */
#define AW_MPCSSNRTSP           _AW_MPIOW(30, int)

/** \brief ����/��ֹ��尴��*/
#define AW_MPCSRBLKKY           _AW_MPIOW(31, int)

/** \brief ͼ�δ�ֱȡģ������� */
#define AW_MPCGDMVP             _AW_MPIOW(32, int)

/** \brief ͼƬˮƽȡģ���ݴ�ӡ */
#define AW_MPCGLMDP             _AW_MPIOW(33, int)

/** \brief ��ӡNVλͼ*/
#define AW_MPCPNVIMG            _AW_MPIOW(34, int)

/** \brief ����NVλͼ */
#define AW_MPCDLNVIMG           _AW_MPIOW(35, int)

/** \brief ˮƽ�Ʊ� */
#define AW_MPCLVLCTTAB          _AW_MPIOW(36, int)

/** \brief ����ˮƽ�Ʊ�λ�� */
#define AW_MPCLVLCTTABP         _AW_MPIOW(37, int)

/** \brief ����һά����ɶ��ַ���HRI����ӡλ�� */
#define AW_MPCSHRIPP            _AW_MPIOW(38, int)

/** \brief ����һά����߶� */
#define AW_MPCSODCH             _AW_MPIOW(39, int)

/** \brief ����һά������*/
#define AW_MPCSODCW             _AW_MPIOW(40, int)

/** \brief ����һά��������*/
#define AW_MPCSODCF             _AW_MPIOW(41, int)

/** \brief ��ӡһά���� */
#define AW_MPCPODC              _AW_MPIOW(42, int)

/** \brief ����/��ӡ��ά��*/
#define AW_MPCSPQRC             _AW_MPIOW(43, int)

/** \brief ��ǩֽ/�ڱ�ָֽ��*/
#define AW_MPCSLBABLKP          _AW_MPIOW(44, int)

/** \brief ��ѯ��ӡ��״̬��ʵʱ��*/
#define AW_MPCRTQPS             _AW_MPIOW(45, int)

/** \brief ����/ȡ����ӡ��״̬�Զ�����*/
#define AW_MPCSRPSAB            _AW_MPIOW(46, int)

/** \brief ����״̬����ʵʱ�� */
#define AW_MPCDLSTAT            _AW_MPIOW(47, int)

/** \brief ��ʼ����ӡ�� */
#define AW_MPCPINIT             _AW_MPIOW(48, int)

/** \brief ���ӡ���棨ʵʱ�� */
#define AW_MPCRLCLPBUF          _AW_MPIOW(49, int)

/** \brief ��ֽ����ֽ */
#define AW_MPCFDCTPAP           _AW_MPIOW(50, int)

/** \brief ���ô���ͨѶ���� */
#define AW_MPCSCMMPAP           _AW_MPIOW(51, int)

/** \brief ѡ���ӡ����ģʽ */
#define AW_MPCSPCTLMD           _AW_MPIOW(52, int)

/** \brief ���ʹ�ӡ��ID */
#define AW_MPCDLPRTID           _AW_MPIOW(53, int)

/** \brief ����/�˳��͹���ģʽ��ʵʱ�� */
#define AW_MPCSRLOWPMD          _AW_MPIOW(54, int)

/** \brief ESC % Select/cancel user-defined character set */
#define AW_MPSRUDCHDEFS         _AW_MPIOW(55, int)

/** \brief ESC & Define user-defined characters */
#define AW_MPSUDCHDEF           _AW_MPIOW(56, int)

/** \brief ESC ? Cancel user-defined characters */
#define AW_MPRUDCHDEF           _AW_MPIOW(57, int)

/** \brief ESC G Turn double-strike mode on/off */
#define AW_MPSRDBLSTKMD         _AW_MPIOW(58, int)

/** \brief GS ( N Select character effects */
#define AW_MPSCHEFCT            _AW_MPIOW(59, int)

/** \brief GS b Turn smoothing mode on/off */
#define AW_MPSRSMTHMD           _AW_MPIOW(61, int)

/** \brief ESC c 3 Select paper sensor(s) to output paper-end signals */
#define AW_MPSPSNONDSGN         _AW_MPIOW(62, int)

/** \brief ESC T Select print direction in page mode */
#define AW_SMPPGMDIR            _AW_MPIOW(63, int)

/** \brief ESC W Set print area in page mode */
#define AW_MPSPGMDPAR           _AW_MPIOW(64, int)

/** \brief GS $ Set absolute vertical print position in page mode */
#define AW_MPSPGMDABSVPP        _AW_MPIOW(65, int)

/** \brief GS \ Set relative vertical print position in page mode */
#define AW_MPSPGMDRESVPP        _AW_MPIOW(66, int)

/** \brief GS * Define downloaded bit image */
#define AW_MPDLDEFBMG           _AW_MPIOW(67, int)

/** \brief GS / Print downloaded bit image */
#define AW_MPPTDEFBMG           _AW_MPIOW(68, int)

/** \brief ESC u Transmit peripheral device status */
#define AW_MPTNPRPHDSTAT        _AW_MPIOW(69, int)

/** \brief ESC v Transmit paper sensor status */
#define AW_MPTNPSNSTAT          _AW_MPIOW(70, int)

/** \brief GS : Start/end macro definition */
#define AW_MPSRMCDEF            _AW_MPIOW(71, int)

/** \brief GS ^ Execute macro */
#define AW_MPEMCDEF             _AW_MPIOW(72, int)

/** \brief ESC i Partial cut (one point left uncut) */
#define AW_MPCONENPNT           _AW_MPIOW(73, int)

/** \brief ESC m Partial cut (three points left uncut) */
#define AW_MPCTHRNPNT           _AW_MPIOW(74, int)

/** \brief DLE DC4 (fn = 1) Generate pulse in real-time */
#define AW_MPGENTLTPLS          _AW_MPIOW(75, int)

/** \brief ESC = Select peripheral device */
#define AW_MPSPRPHDEV           _AW_MPIOW(76, int)

/** \brief ESC L Select page mode */
#define AW_SPGMD                _AW_MPIOW(77, int)

/** \brief ESC S Select standard mode */
#define AW_SSTDMD               _AW_MPIOW(78, int)

/** \brief ESC p Generate pulse */
#define AW_MPGENPLS             _AW_MPIOW(79, int)

/** \brief GS ( A Execute test print */
#define AW_MPEXETESTP           _AW_MPIOW(80, int)

/** \brief GS ( D Enable/disable real-time command */
#define AW_MPSRRLTCMD           _AW_MPIOW(81, int)

/** \brief GS ( H Request transmission of response or status */
#define AW_MPREQRSPOSTAT        _AW_MPIOW(82, int)

/** \brief GS ( K Select print control method(s) */
#define AW_MPSPCTLMD            _AW_MPIOW(83, int)

/** \brief GS P Set horizontal and vertical motion units */
#define AW_SHAVMTNT             _AW_MPIOW(84, int)

/** \brief GS g 0 Initialize maintenance counter */
#define AW_MPINITMAINCNT        _AW_MPIOW(85, int)

/** \brief GS g 2 Transmit maintenance counter */
#define AW_MPTNMAINCNT          _AW_MPIOW(86, int)

/** \brief FS �C Turn underline mode on/off for Kanji characters */
#define AW_MPSRUNDERFKCH        _AW_MPIOW(87, int)

/** \brief FS g 1 Write to NV user memory */
#define AW_MPWRNVUSRMEM         _AW_MPIOW(88, int)

/** \brief FS g 2 Read from NV user memory */
#define AW_MPRDNVUSRMEM         _AW_MPIOW(89, int)

/** \brief DLE ENQ Send real-time request to printer */
#define AW_MPSDRLTREQTP         _AW_MPIOW(90, int)

/** \brief ESC FF Print data in page mode */
#define AW_MPPGMDPDAT           _AW_MPIOW(91, int)

/** \brief FS ! Select print mode(s) for Kanji characters */
#define AW_MPSRPMDKCH           _AW_MPIOW(92, int)

/** \brief FF (in page mode) Print and return to standard mode (in page mode) */
#define AW_MPPGSTDMD            _AW_MPIOW(93, int)

/** \brief CAN Cancel print data in page mode */
#define AW_MPPGCANDAT           _AW_MPIOW(94, int)

/** \brief ���ûص����� */
#define AW_MPCALLBACKSET        _AW_MPIOW(100, int)

/** \brief ��չָ������ */
#define AW_MPEXPANDTYPE         _AW_MPIOW(101, int)

/** \brief �ļ��Ƿ�ѹ�� */
#define AW_FILEGZIP             _AW_MPIOW(102, int)
/** @} */


/******************************************************************************/
/** \brief structure for rename and move */
struct rename_struct {
    const char *old_name;
    const char *new_name;
};

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __AW_IOCTL_H */

/* end of file */
