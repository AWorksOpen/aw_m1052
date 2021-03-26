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
 * - 1.00 20-10-19  win, first implementation.
 * \endinternal
 */

#include "aw_mp3_internal.h"

/******************************************************************************/

/**
 * \brief ID3V1 ��ǩ��Ϣ
 * \note
 * ��1
 * -------+---------+-----------------------
 *  IDX   | length  | ����
 * -------+---------+-----------------------
 *  0     | 3       | ��ǩ��־���������"TAG"
 * -------+---------+-----------------------
 *  3     | 30      | ����
 * -------+---------+-----------------------
 *  33    | 30      | ������
 * -------+---------+-----------------------
 *  63    | 30      | ��Ƭ��
 * -------+---------+-----------------------
 *  93    | 4       | ���
 * -------+---------+-----------------------
 *  97    | 30      | ע��
 * -------+---------+-----------------------
 *  127   | 1       | ���ɣ����2��ʾ
 * -------+---------+-----------------------
 * �ù��Ҫ�����пռ�����Կ��ַ�(NUL)��䣬���ǲ��������е�Ӧ�ó�����ѭ�ù��򣬱���winamp���ÿո�(' ')���档
 * �����֪������ʱд����ַ���
 *
 * ��2
 * ����
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  0   | Blues        | 20   | Alternative  | 40   | AlternRock        | 60  | Top 40
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  1   | Classic Rock | 21   | Ska          | 41   | Bass              | 61  | Christian Rap
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  2   | Country      | 22   | Death Metal  | 42   | Soul              | 62  | Pop/Funk
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  3   | Dance        | 23   | Pranks       | 43   | Punk              | 63  | Jungle
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  4   | Disco        | 24   | Soundtrack   | 44   | Space             | 64  | Native American
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  5   | Funk         | 25   | Euro-Techno  | 45   | Meditative        | 65  | Cabaret
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  6   | Grunge       | 26   | Ambient      | 46   | Instrumental Pop  | 66  | New Wave
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  7   | Hip-Hop      | 27   | Trip-Hop     | 47   | Instrumental Rock | 67  | Psychadelic
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  8   | Jazz         | 28   | Vocal        | 48   | Ethnic            | 68  | Rave
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  9   | Metal        | 29   | Jazz+Funk    | 49   | Gothic            | 69  | Showtunes
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  10  | New Age      | 30   | Fusion       | 50   | Darkwave          | 70  | Trailer
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  11  | Oldies       | 31   | Trance       | 51   | Techno-Industrial | 71  | Lo-Fi
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  12  | Other        | 32   | Classical    | 52   | Electronic        | 72  | Tribal
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  13  | Pop          | 33   | Instrumental | 53   | Pop-Folk          | 73  | Acid Punk
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  14  | R&B          | 34   | Acid         | 54   | Eurodance         | 74  | Acid Jazz
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  15  | Rap          | 35   | House        | 55   | Dream             | 75  | Polka
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  16  | Reggae       | 36   | Game         | 56   | Sounthern Rock    | 76  | Retro
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  17  | Rock         | 37   | Sound Clip   | 57   | Comedy            | 77  | Musical
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  18  | Techno       | 38   | Gospel       | 58   | Cult              | 78  | Rock & Roll
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *  19  | Industrial   | 39   | Noise        | 59   | Gangsta           | 79  | Hard Rock
 * -----+--------------+------+--------------+------+-------------------+-----+-----------------
 *
 * Winamp�����˱�2
 * ��3
 * -----+---------------+------+------------------+-------+---------------+-------+--------------
 *  80  | Folk          | 92   | Progressive Rock | 104   | Chamber Music | 116   | Ballad
 * -----+---------------+------+------------------+-------+---------------+-------+--------------
 *  81  | Folk-Rock     | 93   | Psychedelic Rock | 105   | Sonata        | 117   | Poweer Ballad
 * -----+---------------+------+------------------+-------+---------------+-------+---------------
 *  82  | National Folk | 94   | Symphonic Rock   | 106   | Symphony      | 118   | Rhytmic Soul
 * -----+---------------+------+------------------+-------+---------------+-------+---------------
 *  83  | Swing         | 95   | Slow Rock        | 107   | Booty Brass   | 119   | Freestyle
 * -----+---------------+------+------------------+-------+---------------+-------+---------------
 *  84  | Fast Fusion   | 96   | Big Band         | 108   | Primus        | 120   | Duet
 * -----+---------------+------+------------------+-------+---------------+-------+---------------
 *  85  | Bebob         | 97   | Chorus           | 109   | Porn Groove   | 121   | Punk Rock
 * -----+---------------+------+------------------+-------+---------------+-------+---------------
 *  86  | Latin         | 98   | Easy Listening   | 110   | Satire        | 122   | Drum Solo
 * -----+---------------+------+------------------+-------+---------------+-------+---------------
 *  87  | Revival       | 99   | Acoustic         | 111   | Slow Jam      | 123   | A Capela
 * -----+---------------+------+------------------+-------+---------------+-------+---------------
 *  88  | Celtic        | 100  | Humour           | 112   | Club          | 124   | Euro-House
 * -----+---------------+------+------------------+-------+---------------+-------+---------------
 *  89  | Bluegrass     | 101  | Speech           | 113   | Tango         | 125   | Dance Hall
 * -----+---------------+------+------------------+-------+---------------+-------+---------------
 *  90  | Avantgarde    | 102  | Chanson          | 114   | Samba         |       |
 * -----+---------------+------+------------------+-------+---------------+-------+---------------
 *  91  | Gothic Rock   | 103  | Opera            | 115   | Folklore      |       |
 * -----+---------------+------+------------------+-------+---------------+-------+---------------
 * ��������
 * -----+---------------+------+---------------------+-------+-----------------------+-------+---------------
 *  126 | Goa           | 127  | BritPop             | 138   | BlackMetal            | 144   | TrashMetal
 * -----+---------------+------+---------------------+-------+-----------------------+-------+---------------
 *  127 | Drum&Bass     | 133  | Negerpunk           | 139   | Crossover             | 145   | Anime
 * -----+---------------+------+---------------------+-------+-----------------------+-------+---------------
 *  128 | Club-House    | 134  | PoiskPunk           | 140   | ContemporaryChristian | 146   | Jpop
 * -----+---------------+------+---------------------+-------+-----------------------+-------+---------------
 *  129 | Hardcore      | 135  | Beat                | 141   | ChristianRock         | 147   | Synthpop
 * -----+---------------+------+---------------------+-------+-----------------------+-------+---------------
 *  130 | Terror        | 136  | ChristianGangstaRap | 142   | Merengue              |       |
 * -----+---------------+------+---------------------+-------+-----------------------+-------+---------------
 *  131 | Indie         | 137  | HeavyMetal          | 143   | Salsa                 |       |
 * -----+---------------+------+---------------------+-------+-----------------------+-------+---------------
 * �����κε���ֵ����Ϊ��"unknown"
 */
struct __mp3_id3v1_tag {
    uint8_t  id[3];          /**< \brief ��š�TAG���ַ� */
    uint8_t  title[30];      /**< \brief �������� */
    uint8_t  artist[30];     /**< \brief �������� */
    uint8_t  album[30];      /**< \brief ר������ */
    uint8_t  year[4];        /**< \brief ��� */
    uint8_t  comment[30];    /**< \brief ��ע */
    uint8_t  category;       /**< \brief ������𣬹�147�� */
} aw_mp3_var_aligned(1);

/**
 * \brief �������ɣ�����147��
 */
static const char *__g_mp3_id3v1_category[] = {
    "Blues",
    "Classic Rock",
    "Country",
    "Dance",
    "Disco",
    "Funk",
    "Grunge",
    "Hip-Hop",
    "Jazz",
    "Metal",
    "New Age",
    "Oldies",
    "Other",
    "Pop",
    "R&B",
    "Rap",
    "Reggae",
    "Rock",
    "Techno",
    "Industrial",

    "Alternative",
    "Ska",
    "Death Metal",
    "Pranks",
    "Soundtrack",
    "Euro-Techno",
    "Ambient",
    "Trip-Hop",
    "Vocal",
    "Jazz+Funk",
    "Fusion",
    "Trance",
    "Classical",
    "Instrumental",
    "Acid",
    "House",
    "Game",
    "Sound Clip",
    "Gospel",
    "Noise",

    "AlternRock",
    "Bass",
    "Soul",
    "Punk",
    "Space",
    "Meditative",
    "Instrumental Pop",
    "Instrumental Rock",
    "Ethnic",
    "Gothic",
    "Darkwave",
    "Techno-Industrial",
    "Electronic",
    "Pop-Folk",
    "Eurodance",
    "Dream",
    "Sounthern Rock",
    "Comedy",
    "Cult",
    "Gangsta",

    "Top 40",
    "Christian Rap",
    "Pop/Funk",
    "Jungle",
    "Native American",
    "Cabaret",
    "New Wave",
    "Psychadelic",
    "Rave",
    "Showtunes",
    "Trailer",
    "Lo-Fi",
    "Tribal",
    "Acid Punk",
    "Acid Jazz",
    "Polka",
    "Retro",
    "Musical",
    "Rock & Roll",
    "Hard Rock",

    "Folk",
    "Folk-Rock",
    "National Folk",
    "Swing",
    "Fast Fusion",
    "Bebob",
    "Latin",
    "Revival",
    "Celtic",
    "Bluegrass",
    "Avantgarde",
    "Gothic Rock",

    "Progressive Rock",
    "Psychedelic Rock",
    "Symphonic Rock",
    "Slow Rock",
    "Big Band",
    "Chorus",
    "Easy Listening",
    "Acoustic",
    "Humour",
    "Speech",
    "Chanson",
    "Opera",

    "Chamber Music",
    "Sonata",
    "Symphony",
    "Booty Brass",
    "Primus",
    "Porn Groove",
    "Satire",
    "Slow Jam",
    "Club",
    "Tango",
    "Samba",
    "Folklore",

    "Ballad",
    "Poweer Ballad",
    "Rhytmic Soul",
    "Freestyle",
    "Duet",
    "Punk Rock",
    "Drum Solo",
    "A Capela",
    "Euro-House",
    "Dance Hall",

    "Goa",
    "Drum&Bass",
    "Club-House",
    "Hardcore",
    "Terror",
    "Indie",

    "BritPop",
    "Negerpunk",
    "PoiskPunk",
    "Beat",
    "ChristianGangstaRap",
    "HeavyMetal",

    "BlackMetal",
    "Crossover",
    "ContemporaryChristian",
    "ChristianRock",
    "Merengue",
    "Salsa",

    "TrashMetal",
    "Anime",
    "Jpop",
    "Synthpop",
};

/**
 * \brief ID3V2.3 ��ǩͷ
 * \note ĳЩMP3�ļ��ı�ǩ��С�ǲ�������ǩͷ��Ϊ�˼�����Ҫ���������ֱ���ҵ���֡
 * 1.flags
 * ��־�ֽڣ�һ��Ϊ0���������£�
 * �����ƣ�abc00000
 * a - ��ʾ�Ƿ�ʹ��Unsynchronisation
 * b - ��ʾ�Ƿ�����չͷ��
 * c - ��ʾ�Ƿ�Ϊ���Ա�ǩ
 * 2.��ǩ��С
 * һ��4�ֽڣ�ÿ���ֽ�ֻ��7λ�����λ����0����ʽ���£�
 * 0xxxxxxx 0xxxxxxx 0xxxxxxx 0xxxxxxx
 * ���㹫ʽ���£�
 * total_size = (size[0] & 0x7F) * 0x200000 +
 *              (size[1] & 0x7F) * 0x4000   +
 *              (size[2] & 0x7F) * 0x80     +
 *              (size[3] & 0x7F)
 */
struct __mp3_id3v2_3_tag_header {
    uint8_t  id[3];          /**< \brief ID �������ǡ�ID3�� */
    uint8_t  mversion;       /**< \brief ���汾�� */
    uint8_t  sversion;       /**< \brief �Ӱ汾�� */
    uint8_t  flags;          /**< \brief ��ǩͷ��־ */
    uint8_t  size[4];        /**< \brief ��ǩ��Ϣ��С */
} aw_mp3_var_aligned(1);

/**
 * \brief ID3V2.3 ��ǩ֡
 * \note
 * 1.֡��ʶ
 * ���ĸ��ַ���ʶһ��֡�����õĶ������£�
 * TIT2=����
 * TALB=ר��
 * TRCK=���� ��ʽ��M/N������Nר���еĵ�N�ף�MΪר���и���������N��MΪASCII���ʾ������
 * TYER=�����ASCII��
 * TCON=���ͣ��ַ�����ʾ
 * COMM=��ע ��ʽ:"eng\0��ע����"������eng��ʾ��ע��ʹ�õ���Ȼ����
 * 2.��С
 * ÿ���ֽ�8λȫ��
 * fsize = size[0] * 0x1000000 +
 *         size[2] * 0x10000   +
 *         size[2] * 0x100     +
 *         size[3];
 * 3.��־
 * ֻ������6λ������λΪ0����ʽ���£�
 * abc00000 def00000
 * a - ��ǩ������־������ʱ��Ϊ��֡����
 * b - �ļ�������־������ʱ��Ϊ��֡����
 * c - ֻ����־������ʱ�����֡�����޸�
 * d - ѹ����־������ʱһ���ֽڴ������BCD���ʾ����
 * e - ���ܱ�־
 * f - ���־������ʱ˵����֡��������ĳ֡��һ��
 */
struct __mp3_id3v2_3_tag_frame {
    uint8_t   id[4];         /**< \brief ֡ID���ĸ��ַ���־һ��֡ */
    uint8_t   size[4];       /**< \brief ֡���ݴ�С��������֡ͷ */
    uint16_t  flags;         /**< \brief ֡��־ */
} aw_mp3_var_aligned(1);

/**
 * \brief VBR֡
 * \note
 * ---------+-----------+------------------------------------------
 *  byte    | length    | ˵��
 * ---------+-----------+------------------------------------------
 *  0       | 4         | VBRͷ��ǣ�4���ֽ�ASCII�ַ�������Ϊ"Xing" ����"Info"
 * ---------+-----------+------------------------------------------
 *  4       | 4         | ָʾVBRͷ�������ݵı�ǣ���Ϸ�ʽΪ�߼���������ǿ�Ƶġ�
 *          |           | 0x0001 - ��֡���洢��������Ϊ���ڣ���������һ֡
 *          |           | 0x0002 - �ļ����ȴ洢��������Ϊ���ڣ���������ǩ
 *          |           | 0x0004 - TOC�����洢��������Ϊ����
 *          |           | 0x0008 - ����ָʾ�洢��������Ϊ����
 * ---------+-----------+------------------------------------------
 *  8       | 4         | �洢��֡����Big-Endianֵ
 * ---------+-----------+------------------------------------------
 *  8 or 12 | 4         | �洢�ļ�����Big-Endianֵ����λΪ�ֽ�
 * ---------+-----------+------------------------------------------
 *  8,      | 100       | 100�ֽڵ�TOC���������ڿ��ٶ�λ
 *  12,     |           | �����������Ĵ洢���ݣ���1���ֽ������������ļ��ǲ�������׼ȷ��λ��
 *  16,     |           |
 *          |           | TOC�����ļ��㷽ʽ���£�
 *          |           | (TOC[i]/256)*�ļ�����
 *          |           | �����ļ�����240�룬��Ҫ��ת��60�룬�ļ�����Ϊ5000000�ֽ�
 *          |           | ��������
 *          |           | TOC[(60/240)*100]=TOC[25]
 *          |           | Ȼ��������ļ��е�λ�ô�Լ����
 *          |           | (TOC[25]/256)*5000000
 * ---------+-----------+------------------------------------------
 *  8, 12,  | 4         | ����ָʾ����Ϊ0����ã�-100������Big-Endianֵ
 *  16,     |           |
 *  108,    |           |
 *  112,    |           |
 *  116,    |           |
 * ---------+-----------+-------------------------------------------
 */
struct __mp3_frame_vbr {
    uint8_t  id[4];     /**< \brief VBRͷ��� */
    uint32_t flags;     /**< \brief ��ű�־ */
    //uint32_t frames;    /**< \brief �洢��֡�� */
    //uint32_t fsize;     /**< \brief �ļ��ܴ�С */
    //uint8_t  toc[100];  /**< \brief TOC���� */
    //uint32_t quality;   /**< \brief ����ָʾ�� */
} aw_mp3_var_aligned(1);

/**
 * \brief MP3 VBRI֡��Ϣ
 * \note
 * -----------+----------+------------------------------------------------------
 *  position  | length   | Meaning
 * -----------+----------+------------------------------------------------------
 *  0         | 4        | VBR header ID in 4 ASCII chars, always 'VBRI', not
 *            |          | NULL-terminated
 * -----------+----------+------------------------------------------------------
 *  4         | 2        | Version ID as Big-Endian WORD
 * -----------+----------+------------------------------------------------------
 *  6         | 2        | Delay as Big-Endian float
 * -----------+----------+------------------------------------------------------
 *  8         | 2        | Quality indicator
 * -----------+----------+------------------------------------------------------
 *  10        | 4        | Number of Bytes as Big-Endian DWORD
 * -----------+----------+------------------------------------------------------
 *  14        | 4        | Number of Frames as Big-Endian DWORD
 * -----------+----------+------------------------------------------------------
 *  18        | 2        | Number of entries within TOC table as Big-Endian WORD
 * -----------+----------+------------------------------------------------------
 *  20        | 2        | Scale factor of TOC table entries as Big-Endian DWORD
 * -----------+----------+------------------------------------------------------
 *  22        | 2        | Size per table entry in bytes (max 4) as Big-Endian
 *            |          | WORD
 * -----------+----------+------------------------------------------------------
 *  24        | 2        | Frames per table entry as Big-Endian WORD
 * -----------+----------+------------------------------------------------------
 *  26        |          | TOC entries for seeking as Big-Endian integral. From
 *            |          | size per table entry and number of entries, you can
 *            |          | calculate the length of this field
 * -----------+----------+------------------------------------------------------
 */
struct __mp3_frame_vbri {
    uint8_t  id[4];              /**< \brief ֡ID��Ϊ"VBRI" */
    uint8_t  version[2];         /**< \brief �汾ID, Big-Endian */
    uint8_t  delay[2];           /**< \brief �ӳ�, Big-Endian */
    uint8_t  quality[8];         /**< \brief ����ָʾ��, Big-Endian */
    uint8_t  fsize[4];           /**< \brief �ֽ���, Big-Endian */
    uint8_t  frames[4];          /**< \brief ֡����, Big-Endian */
    uint8_t  toc_num[2];         /**< \brief TOC����Ŀ��Big-Endian */
    uint8_t  scale_factor[2];    /**< \brief �������ӣ�Big-Endian  */
    uint8_t  toc_per_size[2];    /**< \brief TOC��ÿ���ֽڴ�С */
    uint8_t  toc_per_frames[2];  /**< \brief TOC��ÿ��֡��С */
} aw_mp3_var_aligned(1);

/**
 * \brief MP3����ṹ��
 */
struct __mp3_decode {
    struct aw_mp3_dec_stream   stream;
    struct aw_mp3_dec_frame    frame;
    struct aw_mp3_dec_synth    synth;
    struct aw_mp3_stream_info  info;

    uint32_t                   data_start;

    aw_mp3_buf_t               buf;

    uint16_t                   toc_num;
    uint16_t                   toc_entry_size   /**< \brief TOCÿһ��Ĵ洢�����ֽ��� */;
    uint16_t                   toc_entry_frames;
    uint8_t                   *p_toc;

    uint8_t                    bits;
    struct aw_mp3_time         time;
    uint32_t                   frame_cnt;

    AW_MUTEX_DECL(lock);
};

/**
 * \brief ����ID3V2��ǩ��Ϣ
 */
static aw_err_t __mp3_decode_id3v2_info (struct __mp3_decode *p_dec)
{
    struct __mp3_id3v2_3_tag_header  id3v2_header;
    struct __mp3_id3v2_3_tag_frame   id3v2_frame;

    uint32_t                         tag_size = 0;
    uint32_t                         offset   = 0;

    aw_mp3_buf_read(&p_dec->buf,
                    &id3v2_header,
                    sizeof(struct __mp3_id3v2_3_tag_header));

    /* ����ID3V2��ǩ*/
    if (strncmp("ID3", (const char *)id3v2_header.id, 3) == 0) {

        tag_size = ((uint32_t)(id3v2_header.size[0] & 0x7F) << 21) |
                   ((uint32_t)(id3v2_header.size[1] & 0x7F) << 14) |
                   ((uint32_t)(id3v2_header.size[2] & 0x7F) << 7)  |
                   ((uint32_t)(id3v2_header.size[3] & 0x7F));

        /* ������ǩ֡ */
        while (offset < tag_size) {

            uint32_t  frame_size = 0;
            uint8_t  *p_info     = NULL;

            aw_mp3_buf_read(&p_dec->buf,
                            &id3v2_frame,
                            sizeof(struct __mp3_id3v2_3_tag_frame));

            frame_size = ((uint32_t)id3v2_frame.size[0] << 24)|
                         ((uint32_t)id3v2_frame.size[1] << 16)|
                         ((uint32_t)id3v2_frame.size[2] << 8 )|
                         ((uint32_t)id3v2_frame.size[3]);

            /* ����֡ */
            if (strncmp("TT2",  (const char *)id3v2_frame.id, 3) == 0 ||
                strncmp("TIT2", (const char *)id3v2_frame.id, 4) == 0) {

                if (p_dec->info.p_title != NULL) {
                    aw_mem_free((void *)p_dec->info.p_title);
                }

                p_info = aw_mem_align(frame_size, AW_CACHE_LINE_SIZE);
                if (p_info == NULL) {
                    return -AW_ENOMEM;
                }
                memset(p_info, 0, frame_size);

                aw_mp3_buf_read(&p_dec->buf, p_info, frame_size - 1);

                p_dec->info.p_title = (char *)p_info;
            }

            /* ������֡ */
            if (strncmp("TP1",  (const char *)id3v2_frame.id, 3) == 0 ||
                strncmp("TPE1", (const char *)id3v2_frame.id, 4) == 0) {

                if (p_dec->info.p_artist != NULL) {
                    aw_mem_free((void *)p_dec->info.p_artist);
                }

                p_info = aw_mem_align(frame_size, AW_CACHE_LINE_SIZE);
                if (p_info == NULL) {
                    return -AW_ENOMEM;
                }
                memset(p_info, 0, frame_size);

                aw_mp3_buf_read(&p_dec->buf, p_info, frame_size - 1);

                p_dec->info.p_artist = (char *)p_info;
            }

            /* ר�� */
            if (strncmp("TALB", (const char *)id3v2_frame.id, 4) == 0) {

                if (p_dec->info.p_album != NULL) {
                    aw_mem_free((void *)p_dec->info.p_album);
                }

                p_info = aw_mem_align(frame_size, AW_CACHE_LINE_SIZE);
                if (p_info == NULL) {
                    return -AW_ENOMEM;
                }
                memset(p_info, 0, frame_size);

                aw_mp3_buf_read(&p_dec->buf, p_info, frame_size - 1);

                p_dec->info.p_album = (char *)p_info;
            }

            /* ���� */
            if (strncmp("TRCK", (const char *)id3v2_frame.id, 4) == 0) {

                p_info = aw_mem_align(frame_size, AW_CACHE_LINE_SIZE);
                if (p_info == NULL) {
                    return -AW_ENOMEM;
                }
                memset(p_info, 0, frame_size);

                aw_mp3_buf_read(&p_dec->buf, p_info, frame_size - 1);

                sscanf((char *)p_info,
                       "%d/%d",
                       &p_dec->info.track_m,
                       &p_dec->info.track_n);

                aw_mem_free(p_info);
            }

            /* ��� */
            if (strncmp("TYER", (const char *)id3v2_frame.id, 4) == 0) {

                p_info = aw_mem_align(frame_size, AW_CACHE_LINE_SIZE);
                if (p_info == NULL) {
                    return -AW_ENOMEM;
                }
                memset(p_info, 0, frame_size);

                aw_mp3_buf_read(&p_dec->buf, p_info, frame_size - 1);

                sscanf((char *)p_info, "%d", &p_dec->info.year);

                aw_mem_free(p_info);
            }

            /* ���� */
            if (strncmp("TCON", (const char *)id3v2_frame.id, 4) == 0) {

                if (p_dec->info.p_category != NULL) {
                    aw_mem_free((void *)p_dec->info.p_category);
                }

                p_info = aw_mem_align(frame_size, AW_CACHE_LINE_SIZE);
                if (p_info == NULL) {
                    return -AW_ENOMEM;
                }
                memset(p_info, 0, frame_size);

                aw_mp3_buf_read(&p_dec->buf, p_info, frame_size - 1);

                p_dec->info.p_category = (char *)p_info;
            }

            /* ��ע */
            if (strncmp("COMM", (const char *)id3v2_frame.id, 4) == 0) {

                if (p_dec->info.p_comment != NULL) {
                    aw_mem_free((void *)p_dec->info.p_comment);
                }

                p_info = aw_mem_align(frame_size, AW_CACHE_LINE_SIZE);
                if (p_info == NULL) {
                    return -AW_ENOMEM;
                }
                memset(p_info, 0, frame_size);

                aw_mp3_buf_read(&p_dec->buf, p_info, frame_size - 1);

                p_dec->info.p_comment = (char *)p_info;
            }

            offset += sizeof(struct __mp3_id3v2_3_tag_frame) + frame_size - 1;
        }

        return (sizeof(struct __mp3_id3v2_3_tag_header) + tag_size);
    }

    return 0;
}

/**
 * \brief ����ID3V2��ǩ��Ϣ
 */
static void __mp3_decode_id3v1_info (struct __mp3_decode *p_dec)
{
    struct __mp3_id3v1_tag id3v1_tag;

    aw_mp3_buf_lseek(&p_dec->buf, -128, SEEK_END);

    memset(&id3v1_tag, 0, sizeof(struct __mp3_id3v1_tag));

    /* ID�ǡ�TAG����˵����ID3V1��ǩ֡*/
    aw_mp3_buf_read(&p_dec->buf,
                    &id3v1_tag,
                    sizeof(struct __mp3_id3v1_tag));

    /* ID�ǡ�TAG����˵����ID3V1��ǩ֡*/
    if (strncmp("TAG", (char*)id3v1_tag.id, 3) == 0) {

        /* ��ȡ������  */
        if (id3v1_tag.title[0] != '\0') {
            p_dec->info.p_title = aw_mem_align(sizeof(id3v1_tag.title),
                                               AW_CACHE_LINE_SIZE);
            strncpy((char *)p_dec->info.p_title,
                    (char *)id3v1_tag.title,
                    sizeof(id3v1_tag.title));

        }

        /* ��ȡ������ */
        if (id3v1_tag.artist[0] != '\0') {

            p_dec->info.p_artist = aw_mem_align(sizeof(id3v1_tag.artist),
                                                AW_CACHE_LINE_SIZE);

            strncpy((char *)p_dec->info.p_artist,
                    (char *)id3v1_tag.artist,
                    sizeof(id3v1_tag.artist));

        }

        /* ��ȡר����  */
        if (id3v1_tag.album[0] != '\0') {

            p_dec->info.p_album = aw_mem_align(sizeof(id3v1_tag.album),
                                               AW_CACHE_LINE_SIZE);

            strncpy((char *)p_dec->info.p_album,
                    (char *)id3v1_tag.album,
                    sizeof(id3v1_tag.album));
        }

        /* ��ȡ���  */
        if (id3v1_tag.year[0] != '\0') {
            sscanf((char *)id3v1_tag.year, "%d", &p_dec->info.year);
        }

        /* ��ȡ��ע */
        if (id3v1_tag.comment[0] != '\0') {

            p_dec->info.p_comment = aw_mem_align(sizeof(id3v1_tag.comment),
                                                 AW_CACHE_LINE_SIZE);

            strncpy((char *)p_dec->info.p_album,
                    (char *)id3v1_tag.album,
                    sizeof(id3v1_tag.album));
        }

        if (id3v1_tag.category < 148) {
            p_dec->info.p_category = __g_mp3_id3v1_category[id3v1_tag.category];
        }
    }
}

#if 0
/**
 * \brief ���δ֪�ı�ǩ��Ϣ
 */
static void __mp3_decode_info_unknown (struct __mp3_decode *p_dec)
{
    const char *p_unknown = "unknown";

    if (p_dec->info.p_title == NULL) {
        p_dec->info.p_title = p_unknown;
    }

    if (p_dec->info.p_artist == NULL) {
        p_dec->info.p_artist = p_unknown;
    }

    if (p_dec->info.p_album == NULL) {
        p_dec->info.p_album = p_unknown;
    }

    if (p_dec->info.p_cagegory == NULL) {
        p_dec->info.p_cagegory = p_unknown;
    }

    if (p_dec->info.p_comment == NULL) {
        p_dec->info.p_comment = p_unknown;
    }
}
#endif

/**
 * \brief �����ļ���Ϣ
 */
static aw_err_t __mp3_decode_info (struct __mp3_decode *p_dec)
{
    aw_err_t  tag_size          = 0;
    uint32_t  offset            = 0;
    uint32_t  fsize             = 0;
    uint32_t  samples_per_frame = 0;

    struct __mp3_frame_vbri vbri;
    struct __mp3_frame_vbr  vbr;

    tag_size = __mp3_decode_id3v2_info(p_dec);
    if (tag_size < 0) {
        return tag_size;
    }

    if (tag_size == 0) {
        __mp3_decode_id3v1_info(p_dec);
    }

    fsize                    = aw_mp3_buf_lseek(&p_dec->buf, 0, SEEK_END);
    fsize                   -= tag_size;
    p_dec->info.stream_size  = fsize;
    p_dec->data_start        = tag_size;

    aw_mp3_buf_lseek(&p_dec->buf, tag_size, SEEK_SET);

    /* ���ý���������ػ�����  */
    aw_mp3_dec_stream_buffer_set(&p_dec->stream,
                                 &p_dec->buf,
                                 tag_size,
                                 fsize);

    /* ��ȡ��һ��֡ͷ���� */
    if (aw_mp3_dec_frame_header_decode(&p_dec->frame.header,
                                       &p_dec->stream) != AW_OK) {
        return AW_ERROR;
    }

    /* CBRģʽ�������������VBRģʽ����Ḳ�Ǹ�ֵ��*/
    {
        uint64_t tmp = (uint64_t)fsize * (uint64_t)8 * (uint64_t)1000;
        p_dec->info.total_ms = tmp / (uint64_t)p_dec->frame.header.bit_rate;
    }

    p_dec->info.bit_rate    = p_dec->frame.header.bit_rate;
    p_dec->info.sample_rate = p_dec->frame.header.sample_rate;
    p_dec->info.sample_bit  = p_dec->bits;
    p_dec->info.channels    =
        aw_mp3_dec_frame_channel_get(&p_dec->frame.header);

    /* ֡������ */
    samples_per_frame = 32 * AW_MP3_DEC_FRAME_NSBSAMPLES(&p_dec->frame.header);

    p_dec->info.samples_per_frame = samples_per_frame;

    /* CBRģʽ�������������VBRģʽ����Ḳ�Ǹ�ֵ��*/
    {
        uint64_t tmp = (uint64_t)p_dec->info.total_ms *
                       (uint64_t)p_dec->info.sample_rate;
        uint32_t div = p_dec->info.samples_per_frame * 1000;
        p_dec->info.frame_num = tmp / (uint64_t)div;
    }

    p_dec->info.stream_size = fsize;

    {
        uint8_t bytes;

        switch (p_dec->info.sample_bit) {

        case 8:
        case 16:
        case 32:
            bytes = p_dec->info.sample_bit / 8;
            break;

        case 24:
            bytes = 4;
            break;

        default:
            aw_assert(0);
            break;
        }

        p_dec->info.out_samples = samples_per_frame * p_dec->info.channels *
                                  bytes;
    }

    /* ��ȡ��ǰƫ��ֵ */
    offset = aw_mp3_buf_lseek(&p_dec->buf, 0, SEEK_CUR);

    aw_mp3_buf_read(&p_dec->buf, &vbri, sizeof(struct __mp3_frame_vbri));

    if (strncmp("VBRI", (char *)vbri.id, 4) == 0) {
        p_dec->info.frame_num = ((uint32_t)vbri.frames[0] << 24) |
                                ((uint32_t)vbri.frames[1] << 16) |
                                ((uint32_t)vbri.frames[2] << 8)  |
                                ((uint32_t)vbri.frames[3]);
        p_dec->info.stream_size = ((uint32_t)vbri.fsize[0] << 24) |
                                  ((uint32_t)vbri.fsize[1] << 16) |
                                  ((uint32_t)vbri.fsize[2] << 8)  |
                                  ((uint32_t)vbri.fsize[3]);
        p_dec->toc_num      = ((uint16_t)vbri.toc_num[0] << 8) |
                              ((uint16_t)vbri.toc_num[1]);
        p_dec->toc_entry_size = ((uint16_t)vbri.toc_per_size[0] << 8) |
                                ((uint16_t)vbri.toc_per_size[1]);
        p_dec->toc_entry_frames = ((uint16_t)vbri.toc_per_frames[0] << 8) |
                                  ((uint16_t)vbri.toc_per_frames[1]);

        {
            uint64_t tmp = (uint64_t)1000 *
                           (uint64_t)p_dec->info.frame_num *
                           (uint64_t)samples_per_frame;
            p_dec->info.total_ms = tmp /
                                   (uint64_t)p_dec->frame.header.sample_rate;
        }

        p_dec->p_toc = aw_mem_align(p_dec->toc_num *
                                    p_dec->toc_entry_size,
                                    AW_CACHE_LINE_SIZE);
        if (p_dec->p_toc == NULL) {
            return -AW_ENOMEM;
        }

        aw_mp3_buf_read(&p_dec->buf,
                        p_dec->p_toc,
                        p_dec->toc_num * p_dec->toc_entry_size);
    } else {

        /* ��ȡVBRƫ�� */
        if (!(p_dec->frame.header.flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT)) {
            if (p_dec->frame.header.mode !=
                AW_MP3_DEC_FRAME_MODE_SINGLE_CHANNEL) {
                offset += 32;   /* 36 */
            } else {
                offset += 17;   /* 21 */
            }
        } else {
            if (p_dec->frame.header.mode !=
                AW_MP3_DEC_FRAME_MODE_SINGLE_CHANNEL) {
                offset += 13;   /* 17 */
            } else {
                offset += 5;    /* 9 */
            }
        }

        aw_mp3_buf_lseek(&p_dec->buf, offset, SEEK_SET);
        aw_mp3_buf_read(&p_dec->buf, &vbr, sizeof(struct __mp3_frame_vbr));

        if (strncmp("Xing", (char *)vbr.id, 4) == 0 ||
            strncmp("Info", (char *)vbr.id, 4) == 0) {

            /* ��֡������ */
            if (vbr.flags & 0x1) {
                uint8_t frames[4];

                aw_mp3_buf_read(&p_dec->buf, frames, 4);
                p_dec->info.frame_num = ((uint32_t)frames[0] << 24)|
                                        ((uint32_t)frames[1] << 16)|
                                        ((uint32_t)frames[2] << 8 )|
                                        ((uint32_t)frames[3]);

                {
                    uint64_t tmp = (uint64_t)1000 *
                                   (uint64_t)p_dec->info.frame_num *
                                   (uint64_t)samples_per_frame;
                    p_dec->info.total_ms =
                        tmp / (uint64_t)p_dec->frame.header.sample_rate;
                }
            }

            /* �ļ����ȴ��� */
            if (vbr.flags & 0x02) {
                uint8_t fszie[4];

                aw_mp3_buf_read(&p_dec->buf, fszie, 4);
                p_dec->info.stream_size = ((uint32_t)fszie[0] << 24)|
                                          ((uint32_t)fszie[1] << 16)|
                                          ((uint32_t)fszie[2] << 8 )|
                                          ((uint32_t)fszie[3]);
            }

            /* TOC���� */
            if (vbr.flags & 0x04) {
                p_dec->toc_num        = 100;
                p_dec->toc_entry_size = 1;

                p_dec->p_toc = aw_mem_align(p_dec->toc_num *
                                            p_dec->toc_entry_size,
                                            AW_CACHE_LINE_SIZE);
                if (p_dec->p_toc == NULL) {
                    return -AW_ENOMEM;
                }

                aw_mp3_buf_read(&p_dec->buf,
                                p_dec->p_toc,
                                p_dec->toc_num * p_dec->toc_entry_size);
            }

            /* ����ָʾ������ */
            if (vbr.flags & 0x08) {
                uint8_t quality[4];

                aw_mp3_buf_read(&p_dec->buf, quality, 4);

                p_dec->info.quality = ((uint32_t)quality[0] << 24)|
                                      ((uint32_t)quality[1] << 16)|
                                      ((uint32_t)quality[2] << 8 )|
                                      ((uint32_t)quality[3]);
            }
        }
    }

    return AW_OK;
}

/**
 * \brief Converts a sample from fixed point number format to bits.
 */
static uint32_t __mp3_dec_int32_to_bits (int32_t sample, uint8_t bits)
{
    if (bits > 28) {
        return sample;
    }

    /* round */
    sample += (1L << (AW_MP3_F_FRACBITS - bits));

    /* clip */
    if (sample >= AW_MP3_F_ONE) {
        sample = AW_MP3_F_ONE - 1;
    } else if (sample < -AW_MP3_F_ONE) {
        sample = -AW_MP3_F_ONE;
    }

    /* quantize */
    return sample >> (AW_MP3_F_FRACBITS + 1 - bits);
}

/******************************************************************************/

/**
 * \brief ��MP3������
 */
aw_mp3_decode_t aw_mp3_decode_open (void)
{
    struct __mp3_decode *p_dec = NULL;

    p_dec = aw_mem_align(sizeof(struct __mp3_decode), AW_CACHE_LINE_SIZE);
    if (p_dec == NULL) {
        return NULL;
    }

    memset(p_dec, 0, sizeof(struct __mp3_decode));

    aw_mp3_dec_stream_init(&p_dec->stream);
    aw_mp3_dec_frame_init(&p_dec->frame);
    aw_mp3_dec_synth_init(&p_dec->synth);

    p_dec->bits = 16;

    aw_mp3_time_init(&p_dec->time);

    p_dec->frame_cnt = 0;

    AW_MUTEX_INIT(p_dec->lock, AW_SEM_Q_PRIORITY);
    return p_dec;
}

/**
 * \brief ���ý�����λ��
 * \attention ��ǰ��֧��8λ��16λ��24λ��32λ��Ĭ�������Ϊ16λ��
 *
 * \param[in] decode : ������ʵ��
 * \param[in] bits   : λ��
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ��������
 */
aw_err_t aw_mp3_decode_stream_bits_set (aw_mp3_decode_t decode,
                                        uint8_t         bits)
{
    struct __mp3_decode *p_dec = decode;

    if (p_dec == NULL || ((bits % 8) != 0)) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dec->lock, AW_SEM_WAIT_FOREVER);
    p_dec->bits = bits;
    AW_MUTEX_UNLOCK(p_dec->lock);
    return AW_OK;
}

/**
 * \brief ��������������������
 */
aw_err_t aw_mp3_decode_stream_mem_load (aw_mp3_decode_t  decode,
                                        const uint8_t   *p_data,
                                        uint32_t         len)
{
    struct __mp3_decode *p_dec = decode;
    aw_err_t             ret;


    if (decode == NULL || p_data == NULL || len == 0) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dec->lock, AW_SEM_WAIT_FOREVER);

    /* ж��֮ǰ��MP3��Ϣ */
    aw_mp3_decode_stream_unload(decode);

    /* ��ʼ�������� */
    aw_mp3_buf_mem_init(&p_dec->buf, p_data, len);

    /* ��������Ϣ */
    ret = __mp3_decode_info(p_dec);
    if (ret != AW_OK) {
        aw_mp3_decode_stream_unload(decode);
    }

    AW_MUTEX_UNLOCK(p_dec->lock);
    return ret;
}

/**
 * \brief �����ļ�������������
 */
aw_err_t aw_mp3_decode_stream_file_load (aw_mp3_decode_t  decode,
                                         const char      *p_filename)
{
    struct __mp3_decode *p_dec = decode;
    aw_err_t             ret;


    if (decode == NULL || p_filename == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dec->lock, AW_SEM_WAIT_FOREVER);

    /* ж��֮ǰ��MP3��Ϣ */
    aw_mp3_decode_stream_unload(decode);

    /* ��ʼ���ļ��� */
    if (aw_mp3_buf_file_init(&p_dec->buf, p_filename, O_RDONLY, 0777) < 0) {
        AW_MUTEX_UNLOCK(p_dec->lock);
        return -AW_EBADF;
    }

    /* ��������Ϣ */
    ret = __mp3_decode_info(p_dec);
    if (ret != AW_OK) {
        aw_mp3_decode_stream_unload(decode);
    }

    AW_MUTEX_UNLOCK(p_dec->lock);
    return ret;
}

/**
 * \brief ж�ؽ������У��ļ�/���ݣ���
 */
aw_err_t aw_mp3_decode_stream_unload (aw_mp3_decode_t decode)
{
    struct __mp3_decode *p_dec = decode;

    if (p_dec == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dec->lock, AW_SEM_WAIT_FOREVER);

    if (p_dec->info.p_title != NULL) {
        aw_mem_free((void *)p_dec->info.p_title);
    }

    if (p_dec->info.p_artist != NULL) {
        aw_mem_free((void *)p_dec->info.p_artist);
    }

    if (p_dec->info.p_album != NULL) {
        aw_mem_free((void *)p_dec->info.p_album);
    }

    p_dec->info.year = 0;

    if (p_dec->info.p_category != NULL) {
        int i;

        for (i = 0; i < 148; i++) {
            if (p_dec->info.p_category == __g_mp3_id3v1_category[i]) {
                break;
            }
        }

        if (i > 148) {
            aw_mem_free((void *)p_dec->info.p_category);
        }
    }

    p_dec->info.track_m = p_dec->info.track_n = 0;

    if (p_dec->info.p_comment != NULL) {
        aw_mem_free((void *)p_dec->info.p_comment);
    }

    p_dec->info.quality     = 0;
    p_dec->info.stream_size = 0;
    p_dec->info.frame_num   = 0;
    p_dec->info.total_ms    = 0;
    p_dec->info.bit_rate    = 0;
    p_dec->info.sample_rate = 0;
    p_dec->info.out_samples = 0;

    aw_mp3_buf_close(&p_dec->buf);

    p_dec->toc_num          = 0;
    p_dec->toc_entry_frames = 0;
    p_dec->toc_entry_size   = 0;

    if (p_dec->p_toc != NULL) {
        aw_mem_free((void *)p_dec);
    }

    AW_MUTEX_UNLOCK(p_dec->lock);
    return AW_OK;
}

/**
 * \brief ��ȡ�������У��ļ�/���ݣ�����Ϣ
 */
aw_err_t aw_mp3_decode_stream_info_get (aw_mp3_decode_t            decode,
                                        struct aw_mp3_stream_info *p_info)
{
    struct __mp3_decode *p_dec = decode;

    if (p_dec == NULL || p_info == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dec->lock, AW_SEM_WAIT_FOREVER);
    *p_info = p_dec->info;
    AW_MUTEX_UNLOCK(p_dec->lock);
    return AW_OK;
}

/**
 * \brief �ӽ��������л�ȡһ֡����
 */
ssize_t aw_mp3_decode_stream_frame (aw_mp3_decode_t     decode,
                                    void               *p_buffer,
                                    uint16_t            buffer_size,
                                    struct aw_mp3_time *p_time)
{
    struct __mp3_decode *p_dec = decode;
    aw_err_t             ret;
    int                  i;
    uint8_t             *p_tmp = (uint8_t *)p_buffer;
    int                  len;
    uint8_t              bytes;

    if (p_dec == NULL || p_buffer == NULL || buffer_size == 0) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dec->lock, AW_SEM_WAIT_FOREVER);

    ret = aw_mp3_dec_frame_decode(&p_dec->frame, &p_dec->stream);

    if (ret != AW_OK) {
        switch (p_dec->stream.error) {

        case AW_MP3_DEC_ERR_LOSTSYNC:
            AW_DBGF(("lost synchronization \r\n"));
            break;

        case AW_MP3_DEC_ERR_BUFLEN:
            AW_DBGF(("input buffer too small \r\n"));
            ret = AW_OK;
            break;

        case AW_MP3_DEC_ERR_BUFPTR:
            AW_DBGF(("invalid (null) buffer pointer \r\n"));
            break;

        case AW_MP3_DEC_ERR_NOMEM:
            AW_DBGF(("not enough memory \r\n"));
            break;

        case AW_MP3_DEC_ERR_BADCRC:
            AW_DBGF(("CRC check failed\r\n"));
            break;

        case AW_MP3_DEC_ERR_NONE:
            break;

        default :
            AW_DBGF(("error number is :  %X \r\n", p_dec->stream.error));
            break;
        }

        AW_MUTEX_UNLOCK(p_dec->lock);
        return ret;
    }

    aw_mp3_time_add(&p_dec->time, &p_dec->frame.header.duration);
    if (p_time != NULL) {
        *p_time = p_dec->time;
    }
    p_dec->frame_cnt++;

    aw_mp3_dec_synth_frame(&p_dec->synth, &p_dec->frame);

    switch (p_dec->info.sample_bit) {

    case 8:
    case 16:
    case 32:
        bytes = p_dec->info.sample_bit / 8;
        break;

    case 24:
        bytes = 4;
        break;

    default:
        aw_assert(0);
        break;
    }

    bytes = p_dec->info.channels * bytes;

    len = p_dec->synth.pcm.length > (buffer_size / bytes) ?
          (buffer_size / bytes) : p_dec->synth.pcm.length;

    for (i = 0; i < len; i++) {

        uint32_t sample;

        sample = __mp3_dec_int32_to_bits(p_dec->synth.pcm.samples[0][i],
                                         p_dec->info.sample_bit);

        switch (p_dec->info.sample_bit) {

        case 8:
            *(p_tmp++) = sample & 0xFF;
            break;

        case 16:
            *(p_tmp++) = sample        & 0xFF;
            *(p_tmp++) = (sample >> 8) & 0xFF;
            break;

        case 24:
            *(p_tmp++) = sample         & 0xFF;
            *(p_tmp++) = (sample >> 8)  & 0xFF;
            *(p_tmp++) = (sample >> 16) & 0xFF;
            *(p_tmp++) = 0;
            break;

        case 32:
            *(p_tmp++) = sample         & 0xFF;
            *(p_tmp++) = (sample >> 8)  & 0xFF;
            *(p_tmp++) = (sample >> 16) & 0xFF;
            *(p_tmp++) = (sample >> 24) & 0xFF;
            break;

        default:
            break;
        }

        if (p_dec->info.channels == 2) {
            sample = __mp3_dec_int32_to_bits(p_dec->synth.pcm.samples[1][i],
                                             p_dec->info.sample_bit);

            switch (p_dec->info.sample_bit) {

            case 8:
                *(p_tmp++) = sample & 0xFF;
                break;

            case 16:
                *(p_tmp++) = sample        & 0xFF;
                *(p_tmp++) = (sample >> 8) & 0xFF;
                break;

            case 24:
                *(p_tmp++) = sample         & 0xFF;
                *(p_tmp++) = (sample >> 8)  & 0xFF;
                *(p_tmp++) = (sample >> 16) & 0xFF;
                *(p_tmp++) = 0x00;
                break;

            case 32:
                *(p_tmp++) = sample         & 0xFF;
                *(p_tmp++) = (sample >> 8)  & 0xFF;
                *(p_tmp++) = (sample >> 16) & 0xFF;
                *(p_tmp++) = (sample >> 24) & 0xFF;
                break;

            default:
                break;
            }
        }
    }

    AW_MUTEX_UNLOCK(p_dec->lock);
    return (len * bytes);
}

/**
 * \brief �ƶ���������������ƫ��ʱ��
 * \note ͨ���ù���ʵ��MP3����Ŀ���ͺ���
 * \attention
 *  ��ƫ�Ƶ�λΪ���룬��������д�����ʵ��ƫ�����趨��ֵ��������
 *  ��ƫ��ֵС��һ֡�Ĳ���ʱ��ʱ�����������������κ�ƫ�ơ�
 *  һ֡�Ĳ���ʱ��(����)=ÿ֡��������samples_per_frame�� / ����Ƶ�ʣ�sample_rate�� * 1000
 */
aw_err_t aw_mp3_decode_stream_time (aw_mp3_decode_t decode,
                                    int             ms,
                                    int             whence)
{
    struct __mp3_decode *p_dec = decode;
    aw_err_t             ret;
    uint32_t             frames;
    uint32_t             frame_cnt;
    uint32_t             time;
    off_t                pos;
    aw_bool_t            minus = AW_FALSE;
    uint32_t             tmp_ms;

    if (p_dec == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dec->lock, AW_SEM_WAIT_FOREVER);

    /* ���� */
    if (ms < 0) {
        minus  = AW_TRUE;
        tmp_ms = -ms;
    } else {
        tmp_ms = ms;
    }

    {
        uint64_t total = (uint64_t)tmp_ms * (uint64_t)p_dec->info.sample_rate;
        frames = total / (uint64_t)(1000 * p_dec->info.samples_per_frame);
    }

    switch (whence) {

    case SEEK_SET:
        if (minus) {
            frame_cnt = 0;
            time      = 0;
        } else {
            frame_cnt = frames;
            time      = tmp_ms;
        }
        break;

    case SEEK_CUR: {
        time      = p_dec->time.seconds * 1000 + p_dec->time.fraction / 352800;
        frame_cnt = p_dec->frame_cnt;

        if (minus) {
            frame_cnt -= frames;
            time      -= tmp_ms;
        } else {
            frame_cnt += frames;
            time      += tmp_ms;
        }
        break;
    }

    case SEEK_END:
        if (minus) {
            frame_cnt = p_dec->info.frame_num - frames;
            time      = p_dec->info.total_ms  - tmp_ms;
        } else {
            frame_cnt = p_dec->info.frame_num;
            time      = p_dec->info.total_ms;
        }
        break;

    default:
        ret = -1;
        goto __fail;
    }

    if (frame_cnt > p_dec->info.frame_num) {
        frame_cnt = p_dec->info.frame_num;
    }

    if (frame_cnt == p_dec->frame_cnt) {
        goto __exit;
    }

    p_dec->frame_cnt = frame_cnt;

    /* VBRģʽ */
    if (p_dec->p_toc != NULL) {

        if (time > p_dec->info.total_ms) {
            time = p_dec->info.total_ms;
        }

        {
            uint64_t tmp = (uint64_t)p_dec->p_toc[time /
                                     p_dec->info.total_ms * p_dec->toc_num] *
                           (uint64_t)p_dec->info.stream_size;

            pos = tmp / (uint64_t)(1 << (p_dec->toc_entry_size * 8)) +
                  (uint64_t)p_dec->data_start;
        }
    } else {
        uint64_t tmp = (uint64_t)p_dec->info.stream_size *
                       (uint64_t)p_dec->frame_cnt;
        pos = tmp / (uint64_t)p_dec->info.frame_num + p_dec->data_start;
    }

    aw_mp3_dec_stream_init(&p_dec->stream);
    aw_mp3_dec_frame_init(&p_dec->frame);
    aw_mp3_dec_synth_init(&p_dec->synth);

    aw_mp3_time_reset(&p_dec->time);

    /* ���ý���������ػ�����  */
    aw_mp3_dec_stream_buffer_set(&p_dec->stream,
                                 &p_dec->buf,
                                 pos,
                                 p_dec->info.stream_size - pos);

    p_dec->stream.sync = 0;

    if (aw_mp3_dec_frame_header_decode(&p_dec->frame.header,
                                       &p_dec->stream) != AW_OK) {
        ret = -1;
        goto __fail;
    }

    if (p_dec->stream.pos_this_frame > pos) {
        p_dec->frame_cnt++;
    }

    do {
        ret = aw_mp3_dec_frame_decode(&p_dec->frame, &p_dec->stream);
        p_dec->frame_cnt++;
    } while (ret != AW_OK);

    {
        uint64_t tmp = (uint64_t)p_dec->frame_cnt *
                       (uint64_t)p_dec->info.samples_per_frame;
        aw_mp3_time_set(&p_dec->time,
                        tmp / (uint64_t)p_dec->info.sample_rate,
                        tmp % (uint64_t)p_dec->info.sample_rate,
                        p_dec->info.sample_rate);
    }

__exit:
    ret = p_dec->time.seconds * 1000 + p_dec->time.fraction / 352800;

__fail:
    AW_MUTEX_UNLOCK(p_dec->lock);
    return ret;
}

/**
 * \brief �ر�MP3������
 */
aw_err_t aw_mp3_decode_close (aw_mp3_decode_t decode)
{
    if (decode == NULL) {
        return -AW_EINVAL;
    }

    aw_mp3_decode_stream_unload(decode);

    AW_MUTEX_TERMINATE(((struct __mp3_decode *)decode)->lock);
    aw_mem_free(decode);
    return AW_OK;
}

/*******************************************************************************
 common
*******************************************************************************/

/**
 * \name MP3 Version Definition
 * @{
 */
#define AW_MP3_VER_MAJOR          0   /**< \brief major version */
#define AW_MP3_VER_MINOR          1   /**< \brief minor version */
#define AW_MP3_VER_REVISED        0   /**< \brief revised version */

/** @} */

const char *aw_mp3_version_get (void)
{
    static char  ver[24];
    char        *p = ver;

    *(p++) = 'v';

#if (AW_MP3_VER_MAJOR < 10)
    *(p++) = AW_MP3_SVAL(AW_MP3_VER_MAJOR)[0];
#elif (AW_MP3_VER_MAJOR < 100)
    *(p++) = AW_MP3_SVAL(AW_MP3_VER_MAJOR)[0];
    *(p++) = AW_MP3_SVAL(AW_MP3_VER_MAJOR)[1];
#elif (AW_MP3_VER_MAJOR >= 100)
#error
#endif

    *(p++) = '.';

#if (AW_MP3_VER_MINOR < 10)
    *(p++) = AW_MP3_SVAL(AW_MP3_VER_MINOR)[0];
#elif (AW_MP3_VER_MINOR < 100)
    *(p++) = AW_MP3_SVAL(AW_MP3_VER_MINOR)[0];
    *(p++) = AW_MP3_SVAL(AW_MP3_VER_MINOR)[1];
#elif (AW_MP3_VER_MINOR >= 100)
#error
#endif

    *(p++) = '.';

#if (AW_MP3_VER_REVISED < 10)
    *(p++) = AW_MP3_SVAL(AW_MP3_VER_REVISED)[0];
#elif (AW_MP3_VER_REVISED < 100)
    *(p++) = AW_MP3_SVAL(AW_MP3_VER_REVISED)[0];
    *(p++) = AW_MP3_SVAL(AW_MP3_VER_REVISED)[1];
#elif (AW_MP3_VER_REVISED >= 100)
#error
#endif

    *(p++) = '+';

    *(p++) = BUILD_YEAR_CH2;
    *(p++) = BUILD_YEAR_CH3;
    *(p++) = BUILD_MONTH_CH0;
    *(p++) = BUILD_MONTH_CH1;
    *(p++) = BUILD_DAY_CH0;
    *(p++) = BUILD_DAY_CH1;
    *(p++) = BUILD_HOUR_CH0;
    *(p++) = BUILD_HOUR_CH1;
    *(p++) = BUILD_MIN_CH0;
    *(p++) = BUILD_MIN_CH1;

    *(p++) = '\0';

    return ver;
}

/* end of file */
