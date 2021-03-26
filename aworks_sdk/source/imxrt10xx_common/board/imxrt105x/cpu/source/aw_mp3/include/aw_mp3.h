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
 * - 1.00 20-10-12  win, first implementation.
 * \endinternal
 */

#ifndef __AW_MP3_DECODE_H
#define __AW_MP3_DECODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_types.h"
#include "aw_common.h"

/**
 * \brief MP3解码器实例
 */
typedef void *aw_mp3_decode_t;

/**
 * \brief MP3数据流信息
 */
struct aw_mp3_stream_info {

    const char  *p_title;      /**< \brief 歌曲名字  */
    const char  *p_artist;     /**< \brief 艺术家名字 */
    const char  *p_album;      /**< \brief 专辑 */
    int          year;         /**< \brief 歌曲年份 */
    const char  *p_category;   /**< \brief 音乐类型 */
    int          track_m;      /**< \brief 音轨M，表示专辑中歌曲数量 */
    int          track_n;      /**< \brief 音轨N，表示专辑中的第几首歌 */
    const char  *p_comment;    /**< \brief 备注 */

    uint8_t      quality;      /**< \brief 音频质量（0~100），数值越高越好 */

    uint32_t     stream_size;  /**< \brief 音频数据大小（单位：字节） */
    uint32_t     frame_num;    /**< \brief 音频帧数 */

    uint32_t     total_ms;     /**< \brief 播放总时间（单位毫秒）*/

    uint32_t     bit_rate;     /**< \brief 比特率（单位:bps） */
    uint32_t     sample_rate;  /**< \brief 采样率 （单位：Hz）*/

    uint8_t      sample_bit;   /**< \brief 采样位数 */

    /**
     * \brief 输出采样数(单位：字节)
     * \note 如果为双通道，则采样存储规则为 左声道 + 右声道 + 左声道 + 右声道 +... + 左声道 + 右声道。
     *       应用需提供out_samples字节的空间用于存放解码后的数据。
     * \code
     *  aw_mp3_decode_t           decode;
     *  struct aw_mp3_stream_info info;
     *
     *  extern const uint8_t g_mp3_data[];
     *  extern const uint8_t g_mp3_data_len;
     *
     *  uint8_t *p_buffer;
     *
     *  decode = aw_mp3_decode_open();
     *  if (decode == NULL) {
     *      // 打开失败
     *      return;
     *  }
     *
     *  aw_mp3_decode_stream_mem_load(decode, g_mp3_data, g_mp3_data_len);
     *  aw_mp3_decode_stream_info_get(decode, &info);
     *
     *  p_buffer = aw_mem_align(info.out_samples, AW_CACHE_LINE_SIZE);
     *  if (p_buffer == NULL) {
     *      // 分配失败
     *      return;
     *  }
     *
     *  aw_mp3_decode_stream_frame(decode, p_buffer, info.out_samples, NULL);
     * \endcode
     */
    uint32_t     out_samples;

    /**
     * \brief 通道数
     * \note 如果为1，则表示单声道；如果为2，则为双声道
     */
    uint8_t      channels;

    /**
     * \brief 每帧采样数
     * \attention 采样数不等于解码数据的字节数
     */
    uint32_t     samples_per_frame;
};

/** \brief 精度 */
#define AW_MP3_TIME_RESOLUTION    (352800000UL)

/**
 * \brief 时间单位
 */
struct aw_mp3_time {
    int seconds;    /** \brief 秒 */
    int fraction;   /** \brief 1/AW_MP3_TIME_RESOLUTION 秒 */
};

/**
 * \brief 打开MP3解码器
 * \note 每次调用该接口，将分配一个新的解码器实例。为了保证内存不泄漏，需要调用aw_mp3_decode_close()关闭解码器。
 * \return 如果成功返回解码器实例，失败则返回NULL
 */
aw_mp3_decode_t aw_mp3_decode_open (void);

/**
 * \brief 设置解码器位深
 * \attention 当前仅支持8位，16位，24位与32位。默认情况下为16位。
 *
 * \param[in] decode : 解码器实例
 * \param[in] bits   : 位深
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 参数错误
 */
aw_err_t aw_mp3_decode_stream_bits_set (aw_mp3_decode_t decode,
                                        uint8_t         bits);

/**
 * \brief 加载数据流到解码器中
 *
 * \param[in] decode : 解码器实例
 * \param[in] p_data : 数据
 * \param[in] len    : 数据长度
 *
 * \retval AW_OK      : 成功
 * \retval -AW_ENOMEM : 内存空间不足
 * \retval -AW_EINVAL : 参数错误
 */
aw_err_t aw_mp3_decode_stream_mem_load (aw_mp3_decode_t  decode,
                                        const uint8_t   *p_data,
                                        uint32_t         len);

/**
 * \brief 加载文件流到解码器中
 *
 * \param[in] decode     : 解码器实例
 * \param[in] p_filename : 文件名称
 *
 * \retval AW_OK      : 成功
 * \retval -AW_ENOMEM : 内存空间不足
 * \retval -AW_EBADF  : 文件打开失败
 * \retval -AW_EINVAL : 参数错误
 */
aw_err_t aw_mp3_decode_stream_file_load (aw_mp3_decode_t  decode,
                                         const char      *p_filename);

/**
 * \brief 卸载解码器中（文件/数据）流
 * \param[in] decode : 解码器实例
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 参数错误
 */
aw_err_t aw_mp3_decode_stream_unload (aw_mp3_decode_t decode);

/**
 * \brief 获取解码器中（文件/数据）流信息
 *
 * \param[in]  decode : 解码器实例
 * \param[out] p_info : 输出解码器流信息
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 参数错误
 */
aw_err_t aw_mp3_decode_stream_info_get (aw_mp3_decode_t            decode,
                                        struct aw_mp3_stream_info *p_info);

/**
 * \brief 从解码器流中获取一帧数据
 *
 * \param[in]  decode      : 解码器实例
 * \param[out] p_buffer    : 存放解码后的帧数据
 * \param[in]  buffer_size : 缓冲区大小，一般等于流信息中的 out_samples
 * \param[out] p_frame_cnt : 流累积解码时间，可用于音频播放时间
 *
 * \return > 0        : 解码后数据帧长度
 * \retval AW_ERROR   : 解码错误
 * \retval -AW_EINVAL : 参数错误
 */
ssize_t aw_mp3_decode_stream_frame (aw_mp3_decode_t     decode,
                                    void               *p_buffer,
                                    uint16_t            buffer_size,
                                    struct aw_mp3_time *p_time);

/**
 * \brief 移动解码器中流解码偏移时间
 * \note 通过该功能实现MP3解码的快进和后退
 * \attention
 *  因偏移单位为毫秒，计算过程中存在误差，实际偏移与设定的值将存在误差。
 *  当偏移值小于一帧的播放时间时，解码器将不会做任何偏移。
 *  一帧的播放时间(毫秒)=每帧采样数（samples_per_frame） / 采样频率（sample_rate） * 1000
 *
 * \param[in] decode   : 解码器
 * \param[in] ms       : 偏移值(单位：毫秒)
 * \param[in] whence   : SEEK_SET, 参数ms即为新的解码位置
 *                       SEEK_CUR, 以目前的读写位置往后增加ms个位移量
 *                       SEEK_END, 将读写位置指向文件尾后再增加ms个位移量
 *                       当whence值为SEEK_CUR或SEEK_END时，参数offset允许负值的出现
 *
 * \return 当前解码器解码距流开头的偏移值
 * \retval -1         : 偏移错误
 * \retval -AW_EINVAL : 参数错误
 */
aw_err_t aw_mp3_decode_stream_time (aw_mp3_decode_t decode,
                                    int             ms,
                                    int             whence);

/**
 * \brief 关闭MP3解码器
 *
 * \param[in] decode : 解码器
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 参数错误
 */
aw_err_t aw_mp3_decode_close (aw_mp3_decode_t decode);

/*******************************************************************************
 common
*******************************************************************************/

/**
 * \brief 获取MP3组件版本号
 * \return 版本号字符串
 */
const char *aw_mp3_version_get (void);

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_DECODE_H */

/* end of file */
