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

#ifndef __AW_UVC_H
#define __AW_UVC_H
#include "host/class/uvc/videodev2.h"

/* UVC閺嶇厧绱￠柊宥囩枂缂佹挻鐎担锟�*/
typedef struct aw_uvc_format_cfg {
    uint32_t type;                 /* 閺嶇厧绱＄猾璇茬��*/
    struct v4aw_pix_format   pix;  /* 閸嶅繒绀岄悙瑙勭壐瀵繒绮ㄩ弸鍕秼*/

}aw_uvc_format_cfg_t;








#endif
