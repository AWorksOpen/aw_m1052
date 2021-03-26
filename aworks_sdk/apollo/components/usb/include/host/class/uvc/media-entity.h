/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#ifndef _MEDIA_ENTITY_H
#define _MEDIA_ENTITY_H


#define MEDIA_PAD_FL_SINK       (1 << 0)
#define MEDIA_PAD_FL_SOURCE     (1 << 1)
#define MEDIA_PAD_FL_MUST_CONNECT   (1 << 2)

/* 媒体PAD结构体*/
struct media_pad {
    uint16_t index;                 /* 实体PAD数组的PAD索引*/
    unsigned long flags;            /* PAD索引 (MEDIA_PAD_FL_*) */
};







#endif
