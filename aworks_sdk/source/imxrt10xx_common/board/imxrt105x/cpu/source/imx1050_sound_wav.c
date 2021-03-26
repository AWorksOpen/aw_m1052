/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief The IMX6UL play wav format music.
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-13, tee, first implementation.
 * \endinternal
 */
#include "aworks.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_cache.h"

#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_vdebug.h"
#include "aw_assert.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_sem.h"
#include "string.h"
#include "imx1050_sound_wav.h"
#include "aw_clk.h"
#include "driver/sai/awbl_imx1050_sai.h"
#include "driver/mqs/awbl_imx1050_mqs.h"
#include "stream.h"
#include "wav_header.h"


/*******************************************************************************
  local defines
*******************************************************************************/

/* for the wav, the chunk max num is 4 */
#define __WAV_CHUNK_MAX_NUM     4

#define WAV_PLAY_BUFFER_LEN     40*1024

typedef struct play_buffer
{
    struct play_buffer     *next;
    uint8_t                 p_buffer[WAV_PLAY_BUFFER_LEN];
}play_buffer_t;

struct wav_play {
    AW_MUTEX_DECL(mutex);
    AW_SEMB_DECL(semb_wav_process);
    AW_SEMB_DECL(semb_wav_process_complete);
    AW_SEMC_DECL(semc_buf_played);
    AW_TASK_DECL(task_play_wav,40*1024);

    play_buffer_t               buffer1;
    play_buffer_t               buffer2;
    struct __wav_format         fmt;
    play_buffer_t              *p_played_hdr;
    int                         i_filled_buf;
    stream_t                    stream;
    int                         stream_inited;
    int                         data_start;

    aw_pfuncvoid_t              pfn_complete;
    void                       *p_arg;
    aw_bool_t                   busy;
};

aw_local void sai_buffer_finish_call_back(void *p_arg1,void *p_arg2);
aw_local void play_wav_proc(struct wav_play *p_wav_play)
{
    int                         ret,buf_len;
    play_buffer_t              *p_buf;
    awbl_imx1050_sai_buffer_t    sai_buf;

    while(1) {
        AW_SEMC_TAKE(p_wav_play->semc_buf_played,AW_WAIT_FOREVER);
        AW_MUTEX_LOCK(p_wav_play->mutex,AW_WAIT_FOREVER);
        p_buf = p_wav_play->p_played_hdr;
        if (NULL != p_buf) {
            p_wav_play->p_played_hdr  = p_buf->next;
        }
        AW_MUTEX_UNLOCK(p_wav_play->mutex);

        buf_len = stream_read(&p_wav_play->stream,
                p_buf->p_buffer,sizeof(p_buf->p_buffer) );
        if (buf_len <= 0) {
            AW_MUTEX_LOCK(p_wav_play->mutex,AW_WAIT_FOREVER);
            p_wav_play->i_filled_buf --;
            if ( 0 == p_wav_play->i_filled_buf ) {
                AW_MUTEX_UNLOCK(p_wav_play->mutex);
                goto cleanup;
            }
            AW_MUTEX_UNLOCK(p_wav_play->mutex);
            continue;
        }

        sai_buf.p_buf = p_buf->p_buffer;
        sai_buf.len = buf_len;
        sai_buf.fn_complete = sai_buffer_finish_call_back;
        sai_buf.p_arg1 = p_wav_play;
        sai_buf.p_arg2 = p_buf;
        ret = aw_imx1050_sai_tx_append_buffer(2,
                &sai_buf);
        if (ret != AW_OK) {
            goto cleanup;
        }
    }
cleanup:
    return;
}

aw_local void task_play_wav_fn(void *param)
{
    struct wav_play        *p_wav_play = (struct wav_play *)param;

    while (1) {
        AW_SEMB_TAKE(p_wav_play->semb_wav_process,AW_WAIT_FOREVER);
        play_wav_proc(p_wav_play);

        AW_MUTEX_LOCK(p_wav_play->mutex,AW_WAIT_FOREVER);
        /* disable the MQS */
        aw_imx1050_mqs_disable();
        p_wav_play->busy = AW_FALSE;
        AW_MUTEX_UNLOCK(p_wav_play->mutex);

        if (p_wav_play->pfn_complete) {
            p_wav_play->pfn_complete(p_wav_play->p_arg);
        }

        AW_SEMB_GIVE(p_wav_play->semb_wav_process_complete);
    }
}


aw_local void init_wav_play(struct wav_play *p_wav_play)
{

    memset(p_wav_play,0,sizeof(*p_wav_play) );

    AW_MUTEX_INIT(p_wav_play->mutex,AW_SEM_Q_FIFO);
    AW_SEMB_INIT(p_wav_play->semb_wav_process,AW_SEM_EMPTY,AW_SEM_Q_FIFO);
    AW_SEMB_INIT(p_wav_play->semb_wav_process_complete,
            AW_SEM_EMPTY,AW_SEM_Q_FIFO);
    AW_SEMC_INIT(p_wav_play->semc_buf_played,0,AW_SEM_Q_FIFO);

    AW_TASK_INIT(
            p_wav_play->task_play_wav,
            "task_play_wav",
            2,
            40*1024,
            task_play_wav_fn,
            p_wav_play);
    AW_TASK_STARTUP(p_wav_play->task_play_wav);
}

aw_local volatile int       __g_wav_play_inited = 0;;
aw_local struct wav_play    __g_wav_play ;


aw_local int wav_play_open_wav(struct wav_play *p_wav_play)
{
    int                 ret = -EINVAL;
    stream_t           *p_stream = NULL;
    struct RIFFChunk    riffchunk;
    struct FmtChunk     fmtchunk;
    size_t              siRet;
    uint32_t            iTempLen=0;
    long                lWavLen;
    char                cpTempChar[4];
    off_t               fmt_off;

    AW_MUTEX_LOCK(p_wav_play->mutex,AW_WAIT_FOREVER);
    p_stream = &p_wav_play->stream;

    /* 处理riff块*/
    siRet=stream_read(p_stream,&riffchunk,sizeof(riffchunk));

    if (sizeof(riffchunk) != siRet)
    {
        aw_kprintf( "%s@%d::Err read!\n",__func__,__LINE__);
        goto cleanup;
    }
    if(    riffchunk.chunkid[0]!='R'   \
        || riffchunk.chunkid[1]!='I'   \
        || riffchunk.chunkid[2]!='F'   \
        || riffchunk.chunkid[3]!='F')
    {
        aw_kprintf( "%s@%d::Err riffchunk.chunkid!\n",__func__,__LINE__);
        goto cleanup;
    }
    if(    riffchunk.waveID[0]!='W'   \
        || riffchunk.waveID[1]!='A'   \
        || riffchunk.waveID[2]!='V'   \
        || riffchunk.waveID[3]!='E')
    {
        aw_kprintf( "%s@%d::Err riffchunk.waveID!\n",__func__,__LINE__);
        goto cleanup;
    }

    //在文件流中查找fmt块
    fmtchunk.uChunkSize=0;
    do
    {
        //根据wav文件块的规范，跳到下一个块的开头
        fmt_off = stream_lseek(p_stream,(off_t)fmtchunk.uChunkSize,SEEK_CUR);
        if (fmt_off< 0 )
        {
            aw_kprintf( "%s@%d::Err fseek!\n",__func__,__LINE__);
            goto cleanup;
        }
        //读取块的ID和大小
        siRet=stream_read(p_stream,&fmtchunk,8);
        if (8!= siRet)
        {
            aw_kprintf( "%s@%d::Err fread!\n",__func__,__LINE__);
            goto cleanup;
        }

        //直到块的id为"fmt "
    } while(  (fmtchunk.chunkid[0]!='f'   \
        || fmtchunk.chunkid[1]!='m'   \
        || fmtchunk.chunkid[2]!='t'   \
        || fmtchunk.chunkid[3]!=' ')  \
        );
    //已经查找到fmt块，并且文件的当前位置为存储wav文件格式的结构
    //尝试读取次wav文件的格式信息
    siRet = stream_read(p_stream,
            &p_wav_play->fmt,sizeof(p_wav_play->fmt));
    if (sizeof(p_wav_play->fmt) != siRet )
    {
        aw_kprintf( "%s@%d::Err fread!\n",__func__,__LINE__);
        goto cleanup;
    }

    if (p_wav_play->fmt.fmt_tag != 1)
    {
        aw_kprintf( "%s@%d::Err _wf.wFormatTag=%d!\n",
                __func__,__LINE__,p_wav_play->fmt.fmt_tag);
        goto cleanup;
    }

    //定位到fmt块的末尾

    if (stream_lseek(p_stream,fmt_off+fmtchunk.uChunkSize+8,SEEK_SET) <0 )
    {
        aw_kprintf( "%s@%d::Err seek!\n",__func__,__LINE__);
        goto cleanup;
    }
    //查找data块
    iTempLen = 0;
    do
    {
        //根据wav文件块的规范，调到下一个块的开头
        if (0 > stream_lseek(p_stream,iTempLen,SEEK_CUR) )
        {
            aw_kprintf( "%s@%d::Err seek!\n",__func__,__LINE__);
            goto cleanup;
        }
        siRet = stream_read(p_stream,cpTempChar,4);
        if (4 != siRet)
        {
            aw_kprintf( "%s@%d::Err read!\n",__func__,__LINE__);
            goto cleanup;
        }
        siRet = stream_read(p_stream,&iTempLen,sizeof(uint32_t));
        if (sizeof(uint32_t) != siRet)
        {
            aw_kprintf( "%s@%d::Err read!\n",__func__,__LINE__);
            goto cleanup;
        }
        //指定块的id为"data"
    }while (  (cpTempChar[0]!='d'  \
        || cpTempChar[1]!='a'  \
        || cpTempChar[2]!='t'  \
        || cpTempChar[3]!='a' )\
        );
    //检查音频数据的长度是否合法
    if (iTempLen == 0)
    {
        aw_kprintf( "%s@%d::Err iTempLen==0\n",__func__,__LINE__);
        goto cleanup;
    }

    p_wav_play->data_start = stream_lseek(p_stream,0,SEEK_CUR);
    lWavLen=stream_lseek(p_stream,0,SEEK_END);
    if (0 >lWavLen )
    {
        aw_kprintf( "%s@%d::Err seek!\n",__func__,__LINE__);
        goto cleanup;
    }
    if (0 >stream_lseek(p_stream,p_wav_play->data_start,SEEK_SET) )
    {
        aw_kprintf( "%s@%d::Err seek!\n",__func__,__LINE__);
        goto cleanup;
    }


    //检查音频数据的长度是否合法
    //因为data块是wav文件的最后一个块，所以在这里通过文件大小和
    //data块的大小来检验wav文件的合法性
    if (iTempLen != (lWavLen - p_wav_play->data_start))
    {
        aw_kprintf( "%s@%d::Err invalid data length!\n",__func__,__LINE__);
        goto cleanup;
    }
    ret = 0;
cleanup:
    if (ret) {
        stream_close(p_stream);
    }
    AW_MUTEX_UNLOCK(p_wav_play->mutex);
    return ret;
}

//aw_local int wav_play_open_wav_mem(
int wav_play_open_wav_mem(
        struct wav_play *p_wav_play,
        const uint8_t *p_mem,
        int len)
{
    int                 ret = 0;
    AW_MUTEX_LOCK(p_wav_play->mutex,AW_WAIT_FOREVER);
    if (p_wav_play->busy) {
        ret = -EBUSY;
        goto cleanup;
    }

    if (p_wav_play->stream_inited ) {
        stream_close(&p_wav_play->stream);
        p_wav_play->stream_inited = 0;
    }

    ret = stream_init_mem(&p_wav_play->stream,p_mem,len);
    if (ret) {
        goto cleanup;
    }
    p_wav_play->stream_inited = 1;
    ret = wav_play_open_wav(p_wav_play);
    if (ret) {
        goto cleanup;
    }
    //p_wav_play->busy = AW_TRUE;
cleanup:
    if (0 != ret) {
        if (p_wav_play->stream_inited ) {
            stream_close(&p_wav_play->stream);
            p_wav_play->stream_inited = 0;
        }
    }
    AW_MUTEX_UNLOCK(p_wav_play->mutex);
    return ret;

}

aw_local int wav_play_open_wav_file(
        struct wav_play *p_wav_play,
        const char *filename)
{
    int                 ret = 0;
    AW_MUTEX_LOCK(p_wav_play->mutex,AW_WAIT_FOREVER);
    if (p_wav_play->busy) {
        ret = -EBUSY;
        goto cleanup;
    }

    if (p_wav_play->stream_inited ) {
        stream_close(&p_wav_play->stream);
        p_wav_play->stream_inited = 0;
    }

    ret = stream_init_file(&p_wav_play->stream,filename);
    if (ret) {
        goto cleanup;
    }
    p_wav_play->stream_inited = 1;
    ret = wav_play_open_wav(p_wav_play);
    if (ret) {
        goto cleanup;
    }
    //p_wav_play->busy = AW_TRUE;
cleanup:
    if (0 != ret) {
        if (p_wav_play->stream_inited ) {
            stream_close(&p_wav_play->stream);
            p_wav_play->stream_inited = 0;
        }
    }
    AW_MUTEX_UNLOCK(p_wav_play->mutex);
    return ret;

}

aw_local void sai_buffer_finish_call_back(void *p_arg1,void *p_arg2)
{
    struct wav_play            *p_wav_play;
    play_buffer_t              *p_buffer,*p;

    p_wav_play = p_arg1;
    p_buffer = p_arg2;
    AW_MUTEX_LOCK(p_wav_play->mutex,AW_WAIT_FOREVER);
    p_buffer->next = NULL;
    if (NULL  == p_wav_play->p_played_hdr) {
        p_wav_play->p_played_hdr = p_buffer;
    }
    else {
        p = p_wav_play->p_played_hdr;
        while ( p->next != NULL) {
            p = p->next;
        }
        p ->next = p_buffer;
    }
    AW_MUTEX_UNLOCK(p_wav_play->mutex);

    AW_SEMC_GIVE(p_wav_play->semc_buf_played);
}


aw_local int wav_play_begin(
        struct wav_play *p_wav_play,
        aw_pfuncvoid_t pfn_complete,
        void *p_arg)
{
    int                         ret = 0;
    int                         buf1_len = 0,buf2_len = 0;
    int                         mqs_enabled = 0;
    awbl_imx1050_sai_config_t   config = {0};
    awbl_imx1050_sai_buffer_t   sai_buf = {0};

    AW_MUTEX_LOCK(p_wav_play->mutex,AW_WAIT_FOREVER);
    if (p_wav_play->busy) {
        ret = -EBUSY;
        goto cleanup;
    }
    if ( !p_wav_play->stream_inited )  {
        ret = -ENOENT;
        goto cleanup;
    }


    p_wav_play->p_played_hdr = NULL;
    p_wav_play->i_filled_buf = 0;
    AW_SEMC_RESET(p_wav_play->semc_buf_played,0);

    // 开始填充缓冲区
    memset(&p_wav_play->buffer1,0,sizeof(p_wav_play->buffer1) );
    memset(&p_wav_play->buffer2,0,sizeof(p_wav_play->buffer2) );

    stream_lseek(&p_wav_play->stream,
            p_wav_play->data_start,SEEK_SET);
    buf1_len = stream_read(
            &p_wav_play->stream,
            p_wav_play->buffer1.p_buffer,
            sizeof(p_wav_play->buffer1.p_buffer) );
    if ( buf1_len > 0) {
        buf2_len = stream_read(
            &p_wav_play->stream,
            p_wav_play->buffer2.p_buffer,
            sizeof(p_wav_play->buffer2.p_buffer) );
    }

    if (buf1_len <=0 && buf2_len <= 0) {
        ret = -ENOBUFS;
        goto cleanup;
    }

    aw_imx1050_mqs_reset();

    // config the sai
    config.flags = 0;
    config.width = p_wav_play->fmt.bits_per_sample;
    config.channels = p_wav_play->fmt.channels;
    config.sample_rate = p_wav_play->fmt.samples_per_sec;
    ret = aw_imx1050_sai_tx_config(2,&config);
    if (ret != AW_OK) {
        goto cleanup;
    }

    /* enable the MQS */
    ret = aw_imx1050_mqs_enable(p_wav_play->fmt.samples_per_sec,
                               p_wav_play->fmt.bits_per_sample,
                               p_wav_play->fmt.channels);
    if (ret != AW_OK) {
        goto cleanup;
    }

    mqs_enabled = 1;
    if(buf1_len > 0) {
        sai_buf.p_buf = p_wav_play->buffer1.p_buffer;
        sai_buf.len = buf1_len;
        sai_buf.fn_complete = sai_buffer_finish_call_back;
        sai_buf.p_arg1 = p_wav_play;
        sai_buf.p_arg2 = &p_wav_play->buffer1;
        ret = aw_imx1050_sai_tx_append_buffer(2,
                &sai_buf);
        if (ret != AW_OK) {
            goto cleanup;
        }
        p_wav_play->i_filled_buf ++;
    }

    if(buf2_len > 0) {
        sai_buf.p_buf = p_wav_play->buffer2.p_buffer;
        sai_buf.len = buf2_len;
        sai_buf.fn_complete = sai_buffer_finish_call_back;
        sai_buf.p_arg1 = p_wav_play;
        sai_buf.p_arg2 = &p_wav_play->buffer2;
        ret = aw_imx1050_sai_tx_append_buffer(2,
                &sai_buf);
        if (ret != AW_OK) {
            goto cleanup;
        }
        p_wav_play->i_filled_buf ++;
    }

    p_wav_play->pfn_complete = pfn_complete;
    p_wav_play->p_arg = p_arg;
cleanup:
    if (0 == p_wav_play->i_filled_buf ) {
        if (mqs_enabled) {
            aw_imx1050_mqs_disable();
        }
        ret = -1;
    }
    else {
        p_wav_play->busy = AW_TRUE;
    }
    AW_MUTEX_UNLOCK(p_wav_play->mutex);
    if (0 == ret) {
        AW_SEMB_GIVE(p_wav_play->semb_wav_process);
    }

    return ret;
}





/******************************************************************************/
/* synchronous transfer complete */
aw_local void __imx1050_mqs_trans_sync_complete (void *p_arg)
{
    if (p_arg) {
        aw_semb_give(p_arg);
    }
}



/*******************************************************************************
  public functions
*******************************************************************************/
aw_err_t  aw_imx1050_sound_wav_play_async (const uint8_t   *p_wav_buf,
                                          size_t           len,
                                          aw_pfuncvoid_t   pfn_complete,
                                          void            *p_arg)
{
    aw_err_t        ret = AW_OK;

    if (!__g_wav_play_inited) {
        __g_wav_play_inited = 1;
        init_wav_play(&__g_wav_play);
    }

    ret = wav_play_open_wav_mem(
            &__g_wav_play,
            p_wav_buf,
            len);
    if (ret) {
        goto cleanup;
    }
    ret = wav_play_begin(&__g_wav_play,pfn_complete,p_arg);

cleanup:
    return ret;
}

aw_err_t  aw_imx1050_sound_wav_file_play_async (const char *filename,
                                          aw_pfuncvoid_t   pfn_complete,
                                          void            *p_arg)
{
    aw_err_t        ret = AW_OK;

    if (!__g_wav_play_inited) {
        __g_wav_play_inited = 1;
        init_wav_play(&__g_wav_play);
    }

    ret = wav_play_open_wav_file(
            &__g_wav_play,
            filename);
    if (ret) {
        goto cleanup;
    }
    ret = wav_play_begin(&__g_wav_play,pfn_complete,p_arg);

cleanup:
    return ret;
}

/******************************************************************************/

aw_err_t  aw_imx1050_sound_wav_play_sync (const uint8_t *p_wav_buf, size_t len)
{
    AW_SEMB_DECL(sem_sync);

    aw_semb_id_t   sem_id;
    aw_err_t       ret;

    sem_id = AW_SEMB_INIT(sem_sync, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    ret = aw_imx1050_sound_wav_play_async(p_wav_buf,
                                         len,
                                         __imx1050_mqs_trans_sync_complete,
                                        (void *)sem_id);

    if (ret != AW_OK) {
        return ret;
    }

    AW_SEMB_TAKE(sem_sync, AW_WAIT_FOREVER);

    /* Just in case that give the signal in reset procedure */
    AW_SEMB_TERMINATE(sem_sync);

    return AW_OK;
}

aw_err_t  aw_imx1050_sound_wav_file_play_sync (const char *filename)
{
    AW_SEMB_DECL(sem_sync);

    aw_semb_id_t   sem_id;
    aw_err_t       ret;

    sem_id = AW_SEMB_INIT(sem_sync, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    ret = aw_imx1050_sound_wav_file_play_async(filename,
                                         __imx1050_mqs_trans_sync_complete,
                                        (void *)sem_id);

    if (ret != AW_OK) {
        return ret;
    }

    AW_SEMB_TAKE(sem_sync, AW_WAIT_FOREVER);

    /* Just in case that give the signal in reset procedure */
    AW_SEMB_TERMINATE(sem_sync);

    return AW_OK;
}


/******************************************************************************/


/* end of file */

