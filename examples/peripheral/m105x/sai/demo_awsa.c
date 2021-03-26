/*******************************************************************************
 *                                  AWorks
 *                       ----------------------------
 *                       innovating embedded platform
 *
 * Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
 * All rights reserved.
 *
 * Contact information:
 * web site:    http://www.zlg.cn
 * e-mail:      support@zlg.cn
 *******************************************************************************/

/**
 * \file
 * \brief 声卡播放例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_DEV_XXX_SAI3
 *      - AW_COM_AWSA
 *      - AW_COM_CONSOLE
 *      - AW_COM_SHELL_SERIAL
 *   2. 将sai文件夹拖入工程中
 *   3. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 输入shell命令："playback "，播放内存流；
 *   2. 输入shell命令："playback xxx.wav"，播放文件流；
 *   3. 输入shell命令："playback start"，恢复播放；
 *   4. 输入shell命令："playback pause"，暂停音频；
 *   5. 输入shell命令："playback stop"，停止播放。
 *
 * \par 源代码
 * \snippet demo_awsa.c src_awsa
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-24  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_std_awsa 声卡播放
 * \copydoc demo_awsa.c
 */

/** [src_awsa] */
#include "apollo.h"
#include "aw_mem.h"
#include "aw_vdebug.h"
#include "aw_shell.h"
#include "aw_time.h"
#include "awsa_stream.h"
#include "awsa_mixer.h"
#include "awsa_pcm.h"
#include "aw_task.h"
#include "aw_msgq.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"


/* 音频数据 */
extern const unsigned char g_sound_wav_data[];
extern const unsigned int  g_sound_wav_data_len;

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

#define CARD_ID           0   /* 声卡ID */

#define __WAV_OPEN        1
#define __WAV_STOP        2
#define __WAV_START       3
#define __WAV_PAUSE       4
#define __FILE_NAME_SIZE  32

struct wav_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t riff_fmt;
    uint32_t fmt_id;
    uint32_t fmt_sz;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_sz;
};

typedef struct play_cmd {
    uint8_t cmd;
    char file_name[__FILE_NAME_SIZE];
} play_cmd_t;


#define WAV_TASK_PRIO     5
#define WAV_TASK_SIZE    (1024 * 4)
AW_TASK_DECL(wav_play_task, WAV_TASK_SIZE);           /* 定义WAV播放任务 */
AW_MSGQ_DECL_STATIC(wav_msgq, 5, sizeof(play_cmd_t)); /* 定义消息队列 */

static void wav_play_task_entry (void *p_param)
{
    aw_err_t            ret = AW_OK;
    awsa_stream_t       stream;
    struct wav_header   header;
    awsa_pcm_config_t   config;
    awsa_pcm_t          pcm;
    char               *p_buffer;
    uint8_t             pause_flag = 0;
    size_t              size;
    int                 num_read = 0;
    awsa_pcm_uframes_t  frames;
    play_cmd_t          play_cmd = {0};
    char               *p_name = NULL;

    while (1) {

        /* 接收消息 */
        AW_MSGQ_RECEIVE(wav_msgq,
                        &play_cmd,
                        sizeof(play_cmd_t),
                        AW_MSGQ_WAIT_FOREVER);
play_restart:
        p_name = play_cmd.file_name;
        if (play_cmd.cmd == __WAV_OPEN) {
            if (strlen(p_name) == 0) {
                ret = awsa_stream_mem_init(&stream,
                                           g_sound_wav_data,
                                           g_sound_wav_data_len);
            } else {
                ret = awsa_stream_file_init(&stream, p_name, O_RDONLY, 0777);
            }

            if (ret != AW_OK) {
                continue;
            }
        }

        awsa_stream_read(&stream, &header, sizeof(struct wav_header));

        if ((header.riff_id      != ID_RIFF)    ||
            (header.riff_fmt     != ID_WAVE)    ||
            (header.fmt_id       != ID_FMT)     ||
            (header.audio_format != FORMAT_PCM) ||
            (header.fmt_sz       != 16)) {

            AW_INFOF(("Error: '%s' is not a riff/wave file\n", p_name));
            awsa_stream_close(&stream);
            continue;
        }

        /* 开始播放音乐 */
        config.channels     =  header.num_channels;
        config.rate         =  header.sample_rate;
        config.period_size  =  4096;
        config.period_count =  8;
        if (header.bits_per_sample == 32) {
            config.format = AWSA_PCM_FORMAT_S32_LE;
        } else if (header.bits_per_sample == 16) {
            config.format = AWSA_PCM_FORMAT_S16_LE;
        }
        config.start_threshold   = 0;
        config.stop_threshold    = 0;
        config.silence_threshold = 0;

        ret = awsa_pcm_open(&pcm, 0, CARD_ID, AWSA_PCM_STREAM_PLAYBACK, &config);
        if (ret != AW_OK) {
            AW_INFOF(("Unable to open sound device %u\n", 0));
            continue;
        }

        frames = awsa_pcm_buffer_size_get(&pcm);
        size   = awsa_pcm_frames_to_bytes(&pcm, frames);
        p_buffer = aw_mem_alloc(size);
        if (p_buffer == NULL) {
            AW_INFOF(("Unable to allocate %d bytes\n", size));
            awsa_pcm_close(&pcm);
            continue;
        }

        AW_INFOF(("Playing : %u ch, %u hz, %u bit\n",
                header.num_channels, header.sample_rate, header.bits_per_sample));

        do {
            memset(&play_cmd, 0x00, sizeof(play_cmd_t));
            AW_MSGQ_RECEIVE(wav_msgq,
                            &play_cmd,
                            sizeof(play_cmd_t),
                            AW_MSGQ_NO_WAIT);

            if (play_cmd.cmd == __WAV_PAUSE) {          /* 暂停播放 */
                pause_flag = 1;
                awsa_pcm_pause(&pcm, 1);
            } else if (play_cmd.cmd == __WAV_START) {   /* 恢复播放 */
                pause_flag = 0;
                awsa_pcm_pause(&pcm, 0);
            } else if (play_cmd.cmd == __WAV_STOP) {    /* 退出播放 */
                pause_flag = 0;
                break;
            } else if (play_cmd.cmd == __WAV_OPEN) {    /* 播放新的一首歌曲 */
                pause_flag = 0;
                awsa_pcm_drain(&pcm);
                awsa_pcm_close(&pcm);
                aw_mem_free(p_buffer);
                awsa_stream_close(&stream);
                goto play_restart;
            }
            if (pause_flag == 1) {  /* 暂停后就不能再继续写入数据 */
                aw_mdelay(5);
                continue;
            }
            num_read = awsa_stream_read(&stream, p_buffer, size);
            if (num_read > 0) {
                awsa_pcm_prepare(&pcm);
                if (awsa_pcm_write(&pcm,
                                   (const uint8_t *)p_buffer,
                                   num_read) < 0) {
                    AW_INFOF(("Error playing sample\n"));
                    break;
                }
            }
        } while (num_read > 0);

        awsa_pcm_drain(&pcm);
        awsa_pcm_close(&pcm);
        aw_mem_free(p_buffer);
        awsa_stream_close(&stream);
    }
}


/* 播放 */
static aw_err_t __wav_playback (int                   argc,
                                char                 *argv[],
                                struct aw_shell_user *sh)
{
    play_cmd_t   play_cmd = {0};

    /* 只创建一次任务 */
    if (AW_TASK_VALID(wav_play_task) == 0) {
        AW_TASK_INIT(wav_play_task,
                    "wav_play_task",
                     WAV_TASK_PRIO,
                     WAV_TASK_SIZE,
                     wav_play_task_entry,
                     NULL);

        AW_MSGQ_INIT(wav_msgq, 5, sizeof(play_cmd_t), AW_MSGQ_Q_FIFO);
        AW_TASK_STARTUP(wav_play_task);

    }

    if (argc >= 1) {
        if (!strcmp(argv[0], "pause")) {
            play_cmd.cmd = __WAV_PAUSE;
        } else if (!strcmp(argv[0], "start")) {
            play_cmd.cmd = __WAV_START;
        } else if (!strcmp(argv[0], "stop")) {
            play_cmd.cmd = __WAV_STOP;
        } else {
            play_cmd.cmd = __WAV_OPEN;
            aw_snprintf(play_cmd.file_name, __FILE_NAME_SIZE, "%s", (const char *)argv[0]);
        }
    } else {
        play_cmd.cmd = __WAV_OPEN;
        aw_snprintf(play_cmd.file_name, __FILE_NAME_SIZE, "%s", "");
    }

    /* 发送消息 */
    AW_MSGQ_SEND(wav_msgq,
                 &play_cmd,
                 sizeof(play_cmd_t),
                 AW_MSGQ_WAIT_FOREVER,
                 AW_MSGQ_PRI_NORMAL);

    return AW_OK;
}


#define __WAV_PLAY_DESC  "<path>  -- Play the WAV file of the specified path\r\n \
<pause> -- Pause        \r\n <start> -- Start playing\r\n<stop>  -- Stop playing"

/* playback shell 命令 */
static const struct aw_shell_cmd __g_awsa_test_cmds[] = {
    {__wav_playback, "playback", __WAV_PLAY_DESC},
};

/* AWSA 测试例程 */
void demo_awsa_entry (void)
{
    static struct aw_shell_cmd_list cl;
    (void)AW_SHELL_REGISTER_CMDS(&cl, __g_awsa_test_cmds);
}

/** [src_awsa] */

/* end of file */
