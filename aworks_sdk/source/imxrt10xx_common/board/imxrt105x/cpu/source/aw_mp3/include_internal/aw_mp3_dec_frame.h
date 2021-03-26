/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Stock Co.,  Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 
 *
 * \internal
 * \par Modification history
 * - 1.00 20-10-13  win, first implementation.
 * \endinternal
 */

#ifndef ___AW_MP3_DEC_FRAME_H
#define ___AW_MP3_DEC_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_mp3_internal.h"

/**
 * \brief MPEG ID
 */
enum aw_mp3_dec_frame_id {
    AW_MP3_DEC_FRAME_ID_2_5     = 0,    /**< \brief MPEG 2.5 */
    AW_MP3_DEC_FRAME_ID_RESERVE = 1,    /**< \brief 保留 */
    AW_MP3_DEC_FRAME_ID_2       = 2,    /**< \brief MPEG 2 */
    AW_MP3_DEC_FRAME_ID_1       = 3,    /**< \brief MPEG 1 */
};

/**
 * \brief MPEG Layer
 */
enum aw_mp3_dec_frame_layer {
    AW_MP3_DEC_FRAME_LAYER_I   = 1, /**< \brief Layer I */
    AW_MP3_DEC_FRAME_LAYER_II  = 2, /**< \brief Layer II */
    AW_MP3_DEC_FRAME_LAYER_III = 3, /**< \brief Layer III */
};

/**
 * \brief MPEG通道模式
 */
enum aw_mp3_dec_frame_mode {

    /** \brief single channel */
    AW_MP3_DEC_FRAME_MODE_SINGLE_CHANNEL = 0,

    /** \brief dual channel */
    AW_MP3_DEC_FRAME_MODE_DUAL_CHANNEL   = 1,

    /** \brief joint (MS/intensity) stereo */
    AW_MP3_DEC_FRAME_MODE_JOINT_STEREO   = 2,

    /** \brief normal LR stereo */
    AW_MP3_DEC_FRAME_MODE_STEREO         = 3,

};

/**
 * \brief 强调模式
 */
enum aw_mp3_dec_frame_emphasis {

    /** \brief no emphasis */
    AW_MP3_DEC_FRAME_EMPHASIS_NONE       = 0,

    /** \brief 50/15 microseconds emphasis */
    AW_MP3_DEC_FRAME_EMPHASIS_50_15_US   = 1,

    /** \brief unknown emphasis */
    AW_MP3_DEC_FRAME_EMPHASIS_RESERVED   = 2,

    /** \brief CCITT J.17 emphasis */
    AW_MP3_DEC_FRAME_EMPHASIS_CCITT_J_17 = 3,
};

/**
 * \brief MPEG帧头部
 * \note 32位（4字节）头部信息
 * 表1
 * -----------------+----------------+------------------------------------------
 * name             | bit length     | description
 * -----------------+----------------+------------------------------------------
 * syncword         | 11(31-21)      | 所有位均为1
 * -----------------+----------------+------------------------------------------
 * ID               | 2(20,19)       | MPEG 音频版本ID
 *                  |                | 00 - MPEG 2.5
 *                  |                | 01 - 保留
 *                  |                | 10 - MPEG 2(ISO/IEC 13818-3)
 *                  |                | 11 - MPEG 1(ISO/IEC 11172-3)
 *                  |                | 注：MPEG 2.5不是官方标准。帧头第20个比特用来表示2.5版本。
 *                  |                |   不支持该版本的应用程序一般认为该比特为帧同步位，也就是说帧同步
 *                  |                | (syncword)的长度为12而不是这里的规定的11，这样ID就变成了
 *                  |                | 1位。推荐使用该表的方法，因为你可以区分3个版本获得最高兼容性。
 * -----------------+----------------+------------------------------------------
 * layer            | 2(18,17)       | Layer描述:
 *                  |                | 00 - 保留
 *                  |                | 01 - Layer III
 *                  |                | 10 - Layer II
 *                  |                | 11 - Layer I
 * -----------------+----------------+------------------------------------------
 * protection_bit   | 1(16)          | 校验位
 *                  |                | 0 - 紧跟帧头后有16位用作CRC校验
 *                  |                | 1 - 无校验
 * -----------------+----------------+------------------------------------------
 * bitrate_index    | 4(15-12)       | 位率索引，如表2所示
 * -----------------+----------------+------------------------------------------
 * sample_frequency | 2(11,10)       | 采样频率（单位：Hz），如表3所示
 * -----------------+----------------+------------------------------------------
 *  padding_bit     | 1(9)           | 填充位
 *                  |                | 0 - 无填充
 *                  |                | 1 - 填充
 *                  |                | 填充用来达到正确的比特率，例如：128K 44.1kHz LayerII
 *                  |                | 使用了很多418bit或417bit长的帧来达到正确的128k比特率，
 *                  |                | LayerI的空位有32bit长，LayerII和LayerIII的空位有
 *                  |                | 8bit长。
 * -----------------+----------------+------------------------------------------
 *  private_bit     | 1(8)           | 私有bit，可以用来做特殊应用。例如可以用来触发应用程序的特殊事件
 * -----------------+----------------+------------------------------------------
 *  mode            | 2(7,6)         | 声道
 *                  |                | 00 - 立体声
 *                  |                | 01 - 联合立体声（立体声）
 *                  |                | 10 - 双声道
 *                  |                | 11 - 单声道
 *                  |                | 注：双声道文件由二个独立的单声道组成，每一个声道使用整个文件一半的
 *                  |                | 位率。大多数的解码器把它当作立体声来输出。两个声道的信息是完全相同
 *                  |                | 的，与立体声不同。
 * -----------------+----------------+------------------------------------------
 *  mode_extension  | 2(5,4)         | 扩展模式（仅在联合立体声时有效）
 *                  |                | 扩展模式用来连接对立体声效果无用的信息，来减少所需要的资源。 这两个
 *                  |                | 位在联合立体声模式下有编码器动态指定
 *                  |                | 完整的MPEG文件的频率序列分成有32个子带。在LayerI和
 *                  |                | LayerII中这两个位确定强度立体声应用的频带。
 *                  |                | LayerIII中这两个位确定应用了哪一种联合立体声（M/S
 *                  |                | stereo或者Intensity stereo)频带由解压算法决定。
 *                  |                | 如表4所示。
 * -----------------+----------------+------------------------------------------
 *  copyright       | 1(3)           | 版权
 *                  |                | 0 - 无版权
 *                  |                | 1 - 有版权
 * -----------------+----------------+------------------------------------------
 *  original/copy   | 1(2)           | 原创
 *                  |                | 0 - 拷贝
 *                  |                | 1 - 原创
 * -----------------+----------------+------------------------------------------
 *  emphasis        | 2(1,0)         | 强调
 *                  |                | 00 - 无
 *                  |                | 01 - 50/15 ms
 *                  |                | 10 - 保留
 *                  |                | 11 - CCIT J.17
 * -----------------+----------------+------------------------------------------
 *
 * 表2
 * ---------------+--------------------------------+--------------------------
 *  bitrate_index | MPEG 1                         | MPEG 2,2.5(LSF)
 *                +---------+----------+-----------+---------+----------------
 *                | Layer I | Layer II | Layer III | Layer I | Layer II & III
 * ---------------+---------+----------+-----------+---------+----------------
 *  0000          | Free
 * ---------------+---------+----------+-----------+---------+----------------
 *  0001          | 32      | 32       | 32        | 32      | 8
 * ---------------+---------+----------+-----------+---------+----------------
 *  0010          | 64      | 48       | 40        | 48      | 16
 * ---------------+---------+----------+-----------+---------+----------------
 *  0011          | 96      | 56       | 48        | 56      | 24
 * ---------------+---------+----------+-----------+---------+----------------
 *  0100          | 128     | 64       | 56        | 64      | 32
 * ---------------+---------+----------+-----------+---------+----------------
 *  0101          | 160     | 80       | 64        | 80      | 40
 * ---------------+---------+----------+-----------+---------+----------------
 *  0110          | 192     | 96       | 80        | 96      | 48
 * ---------------+---------+----------+-----------+---------+----------------
 *  0111          | 224     | 112      | 96        | 112     | 56
 * ---------------+---------+----------+-----------+---------+----------------
 *  1000          | 256     | 128      | 112       | 128     | 64
 * ---------------+---------+----------+-----------+---------+----------------
 *  1001          | 288     | 160      | 128       | 144     | 80
 * ---------------+---------+----------+-----------+---------+----------------
 *  1010          | 320     | 192      | 160       | 160     | 96
 * ---------------+---------+----------+-----------+---------+----------------
 *  1011          | 352     | 224      | 192       | 176     | 112
 * ---------------+---------+----------+-----------+---------+----------------
 *  1100          | 384     | 256      | 224       | 192     | 128
 * ---------------+---------+----------+-----------+---------+----------------
 *  1101          | 416     | 320      | 224       | 192     | 128
 * ---------------+---------+----------+-----------+---------+----------------
 *  1110          | 448     | 384      | 320       | 256     | 160
 * ---------------+---------+----------+-----------+---------+----------------
 *  1111          | Forbidden
 * ---------------+-----------------------------------------------------------
 * 注：单位kbps，1kbit=1000bit
 *
 * Free表示空闲，如果固定比特率（这种文件不能变换比特率）和上表定义的不同，应该由应用程序决定。这种情况的实现应该只用于内部目的
 * ，因为第三方应用程序是无法找出正确的比特率。Forbidden表示该值无效。
 * MPEG文件可以有VBR，表示文件的比特率可以变化。
 * 比特率变换（bitrate switching）: 每一帧都创建成不同的比特率。可以应用在任何层。
 * Layer III 解码器必须支持该方法。Layer I和Layer II可以选择支持。
 *
 * 比特池(bit reservoir)：比特率可以使用从前吗的帧中借来的（受限），以便于腾出空间来容纳输入信号部分，然而这样就导致各帧
 * 之间不再相互独立，意味着不能随便分割文件。这种方法只有LayerIII支持。
 *
 * LayerII中有一些不被允许比特率组合模式。下表是允许的组合
 * 表2.1
 * ---------+-------------------------------------------
 *  bitrate | allowed modes
 * ---------+-------------------------------------------
 *  free    | all
 * ---------+-------------------------------------------
 *  32      | single channel
 * ---------+-------------------------------------------
 *  48      | single channel
 * ---------+-------------------------------------------
 *  56      | single channel
 * ---------+-------------------------------------------
 *  64      | all
 * ---------+-------------------------------------------
 *  80      | single channel
 * ---------+-------------------------------------------
 *  90      | all
 * ---------+-------------------------------------------
 *  112     | all
 * ---------+-------------------------------------------
 *  128     | all
 * ---------+-------------------------------------------
 *  160     | all
 * ---------+-------------------------------------------
 *  192     | all
 * ---------+-------------------------------------------
 *  224     | stereo, intensity stereo, dual channel
 * ---------+-------------------------------------------
 *  256     | stereo, intensity stereo, dual channel
 * ---------+-------------------------------------------
 *  320     | stereo, intensity stereo, dual channel
 * ---------+-------------------------------------------
 *  384     | stereo, intensity stereo, dual channel
 * ---------+-------------------------------------------
 *
 * 采样频率（单位：Hz）
 * 表3
 * ---------+---------+----------+----------
 *  bits    | MPEG1   |  MPEG2   | MPEG2.5
 * ---------+---------+----------+----------
 *  00      | 44100   | 22050    | 11025
 * ---------+---------+----------+----------
 *  01      | 48000   | 24000    | 12000
 * ---------+---------+----------+----------
 *  10      | 32000   | 16000    | 8000
 * ---------+---------+----------+----------
 *  11      | reserved
 * ---------+-------------------------------
 *
 * 扩展模式（仅在联合立体声时有效）
 * 表4
 * ------+----------------+-------------------------------
 *  bits | Layer I & II   | Layer III
 *       |                +------------+------------------
 *       |                | M/S stereo | Intensity stereo
 * ------+----------------+------------+------------------
 *  00   | bands 4 to 31  | off        | off
 * ------+----------------+------------+------------------
 *  01   | bands 8 to 31  | off        | on
 * ------+----------------+------------+------------------
 *  10   | bands 12 to 31 | on         | off
 * ------+----------------+------------+------------------
 *  11   | bands 16 to 31 | on         | on
 * ------+----------------+------------+------------------
 *
 * 帧采样数
 * 表5
 * ------------+-------------+-------------+---------------
 *             | MPEG1       | MPEG2(LSF)  | MPEG2.5(LSF)
 * ------------+-------------+-------------+---------------
 *  Layer I    | 384         | 384         | 384
 * ------------+-------------+-------------+---------------
 *  Lyaer II   | 1152        | 1152        | 1152
 * ------------+-------------+-------------+---------------
 *  Lyaer III  | 1152        | 576         | 576
 * ------------+-------------+-------------+---------------
 *
 * LayerI:
 * 帧长度（字节）=（每帧采样数  / 8 * 比特率） / 采样频率 + 填充 * 4
 * LayerII&LayerIII
 * 帧长度（字节）=（每帧采样数 / 8 * 比特率） / 采样频率 + 填充
 *
 * 每帧持续时间（毫秒）=每帧采样数 / 采样频率 * 1000
 */
struct aw_mp3_dec_frame_header {

    /** \brief audio layer (1, 2, or 3) */
    enum aw_mp3_dec_frame_layer    layer;

    /** \brief audio layer (1, 2, or 3) */
    enum aw_mp3_dec_frame_mode     mode;

    /** \brief additional mode info */
    int                            mode_extension;

    /** \brief de-emphasis to use (see above) */
    enum aw_mp3_dec_frame_emphasis emphasis;

    /** \brief bitrate bps */
    uint32_t                       bit_rate;

    /** \brief sample frequency */
    uint32_t                       sample_rate;

    /** \brief frame CRC accumulator */
    uint16_t                       crc_check;

    /** \brief final target CRC checksum */
    uint16_t                       crc_target;

    /** \brief flags (see below) */
    int                            flags;

    /** \brief private bits (see below) */
    int                            private_bits;

    /** \brief audio playing time of frame */
    struct aw_mp3_time             duration;
};

/**
 * \brief MPEG 帧
 */
struct aw_mp3_dec_frame {

    /** \brief MPEG audio header */
    struct aw_mp3_dec_frame_header header;

    /** \brief decoding options (from stream) */
    int                            options;

    /** \brief synthesis subband filter samples */
    int32_t                        sbsample[2][36][32];

    /** \brief Layer III block overlap data */
    int32_t                        overlap[2][32][18];

};

#define AW_MP3_DEC_FRAME_NCHANNELS(p_header)   ((p_header)->mode ? 2 : 1)

#define AW_MP3_DEC_FRAME_NSBSAMPLES(p_header)  \
    ((p_header)->layer == AW_MP3_DEC_FRAME_LAYER_I ? 12 :  \
     (((p_header)->layer == AW_MP3_DEC_FRAME_LAYER_III &&  \
       ((p_header)->flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT)) ? 18 : 36))

/**
 * \brief frame flag
 */
enum aw_mp3_dec_frame_flag {

    /** \brief number of Layer III private bits */
    AW_MP3_DEC_FRAME_FLAG_NPRIVATE_III = 0x0007,

    /** \brief header but not data is decoded */
    AW_MP3_DEC_FRAME_FLAG_INCOMPLETE   = 0x0008,

    /** \brief frame has CRC protection */
    AW_MP3_DEC_FRAME_FLAG_PROTECTION   = 0x0010,

    /** \brief frame is copyright */
    AW_MP3_DEC_FRAME_FLAG_COPYRIGHT    = 0x0020,

    /** \brief frame is original (else copy) */
    AW_MP3_DEC_FRAME_FLAG_ORIGINAL     = 0x0040,

    /** \brief frame has additional slot */
    AW_MP3_DEC_FRAME_FLAG_PADDING      = 0x0080,

    /** \brief uses intensity joint stereo */
    AW_MP3_DEC_FRAME_FLAG_I_STEREO     = 0x0100,

    /** \brief uses middle/side joint stereo */
    AW_MP3_DEC_FRAME_FLAG_MS_STEREO    = 0x0200,

    /** \brief uses free format bitrate */
    AW_MP3_DEC_FRAME_FLAG_FREEFORMAT   = 0x0400,

    /** \brief lower sampling freq. extension */
    AW_MP3_DEC_FRAME_FLAG_LSF_EXT      = 0x1000,

    /** \brief multichannel audio extension */
    AW_MP3_DEC_FRAME_FLAG_MC_EXT       = 0x2000,

    /** \brief MPEG 2.5 (unofficial) extension */
    AW_MP3_DEC_FRAME_FLAG_MPEG_2_5_EXT = 0x4000
};

/**
 * \brief private
 */
enum aw_mp3_dec_frame_private {

    /** \brief header private bit */
    AW_MP3_DEC_FRAME_PRIVATE_HEADER  = 0x0100,

    /** \brief Layer III private bits (up to 5) */
    AW_MP3_DEC_FRAME_PRIVATE_III     = 0x001f
};

/**
 * \brief 初始化帧头部
 * \param[in] p_header : 帧头部结构体
 * \return 无
 */
void aw_mp3_dec_frame_header_init (struct aw_mp3_dec_frame_header *p_header);

/**
 * \brief 帧头部解码
 * \param[in] p_header : 帧头部结构体
 * \return AW_OK表示成功，其他值表示错误
 */
int aw_mp3_dec_frame_header_decode (struct aw_mp3_dec_frame_header *p_header,
                                    struct aw_mp3_dec_stream       *p_stream);

/**
 * \brief MPEG 帧初始化
 * \param[in] p_frame : 帧结构体
 * \return 无
 */
void aw_mp3_dec_frame_init (struct aw_mp3_dec_frame *p_frame);

/**
 * \brief 帧解码
 *
 * \param[in] p_frame  : 帧结构体
 * \param[in] p_stream : 流结构体
 *
 * \return 无
 */
aw_err_t aw_mp3_dec_frame_decode (struct aw_mp3_dec_frame  *p_frame,
                                  struct aw_mp3_dec_stream *p_stream);

/**
 * \brief 禁音当前帧
 * \param[in] p_frame : 帧结构体
 * \return 无
 */
void aw_mp3_dec_frame_mute (struct aw_mp3_dec_frame *p_frame);

/**
 * \brief 获取通道数量
 * \param[in] p_frame : 帧结构体
 * \return 通道数量
 */
uint8_t
aw_mp3_dec_frame_channel_get (const struct aw_mp3_dec_frame_header *p_header);

/**
 * \brief synthesis subband filter samples
 */
uint8_t
aw_mp3_dec_frame_sbsamples_get (const struct aw_mp3_dec_frame_header *p_header);

#ifdef __cplusplus
}
#endif

#endif /* ___AW_MP3_DEC_FRAME_H */

/* end of file */
