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

#include <stdio.h>
#include <stdlib.h>
#include "libmtp.h"
#include "util.h"
#include "mtp_adapter.h"


/**
 * �⽫��ӡ��stdout��Ϣ��˵���豸��IDδ֪����libmtp�İ汾��
 *
 * @param dev_number   �豸��
 * @param id_vendor    USB�豸�������Ĺ�Ӧ��ID
 * @param id_product   USB�豸�������Ĳ�ƷID
 */
void device_unknown(const int dev_number, const int id_vendor, const int id_product)
{
    /* ������Ա��֪������豸*/
    LIBMTP_ERROR("Device %d (VID=%04x and PID=%04x) is UNKNOWN in libmtp v%s.\n",
                    dev_number, id_vendor, id_product, LIBMTP_VERSION_STRING);
    LIBMTP_ERROR("Please report this VID/PID and the device model to the "
                    "libmtp development team\n");
}










