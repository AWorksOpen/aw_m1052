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

#ifndef __SYS_TIME_H
#define __SYS_TIME_H


#include <time.h>

//#define CLOCKS_PER_SEC  1000

//typedef long long clock_t;
typedef long long mytime;

extern mytime get_time(void);
extern double elapsed(mytime t1, mytime t0);

#endif
