/*******************************************************************************
 *                                 AWorks
 *                       ----------------------------
 *                       innovating embedded platform
 *
 * Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
 * All rights reserved.
 *
 * Contact information:
 * web site:    http://www.zlg.cn/
 *******************************************************************************/

/**
 * \file  barrier.h
 * \brief rtk 关于 barrier 的定义和函数
 *
 * \internal
 * \par modification history:
 * \endinternal
 */

#ifndef         __BARRIER_H__
#define         __BARRIER_H__


#if defined ( __GNUC__ )
#include "barrier_gcc.h"
#else
#error "not supported compiler!!"
#endif

#endif

/* end of file */