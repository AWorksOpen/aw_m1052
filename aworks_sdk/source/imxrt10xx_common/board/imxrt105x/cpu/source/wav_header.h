#ifndef     __WAV_HEADER_H__
#define     __WAV_HEADER_H__

/* one chunk of wav file */
struct __wav_chunk {
    char                id[4];   /* string for chuck id             */
    uint32_t            size;    /* The data length of the chunk    */
    const uint8_t      *p_data;  /* The data of the chunk           */
    struct __wav_chunk *p_next;  /* pointer to the next chunk       */
};

#pragma pack(push)
#pragma pack(2) 

struct __wav_format {
    uint16_t fmt_tag;           /* �����ʽ������WAVE_FORMAT_PCM��WAVEFORMAT_ADPCM�� */
    uint16_t channels;          /* ������  */
    uint32_t samples_per_sec;   /* ����Ƶ�� */
    uint32_t bytes_per_sec;     /* ÿ��������� */
    uint16_t blockalign;        /* �����               */
    uint16_t bits_per_sample;   /* ����λ��           */
};

struct RIFFChunk                    /* ����*.wav RIFF��*/
{
    char     chunkid[4];            /* RIFF���ʶ*/
    uint32_t uiChunkSize;
    char     waveID[4];             /* *.wav�ļ���ʶ*/
};

struct FmtChunk                     /* define FmtChunk*/
{
    char                chunkid[4];
    uint32_t            uChunkSize;
    struct __wav_format wf;
};

struct DataChunkHeader
{
    char        chunkid[4];
    uint32_t    uiChuankSize;
};


#pragma pack(pop)


#endif
