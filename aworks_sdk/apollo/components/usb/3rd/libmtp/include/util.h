/**
 * \file util.c
 *
 * This file contains generic utility functions such as can be
 * used for debugging for example.
 *
 * Copyright (C) 2005-2007 Linus Walleij <triad@df.lth.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __MTP__UTIL__H
#define __MTP__UTIL__H
#include "libmtp.h"
#include "mtp_adapter.h"



void device_unknown(const int dev_number, const int id_vendor, const int id_product);

#ifndef LIBMTP_INFO
/**
 * ��Ϣ��
 */
#define LIBMTP_INFO(format, args...) \
  do { \
    if (LIBMTP_debug != 0) \
      fprintf(stdout, "LIBMTP %s[%d]: " format, __FUNCTION__, __LINE__, ##args); \
    else \
      fprintf(stdout, format, ##args); \
  } while (0)
#endif

#ifndef LIBMTP_ERROR
/**
 * �����
 */
#define LIBMTP_ERROR(format, args...) \
  do { \
    if (LIBMTP_debug != 0) \
      fprintf(stderr, "LIBMTP %s[%d]: " format, __FUNCTION__, __LINE__, ##args); \
    else \
      fprintf(stderr, format, ##args); \
  } while (0)

#endif

#endif //__MTP__UTIL__H

