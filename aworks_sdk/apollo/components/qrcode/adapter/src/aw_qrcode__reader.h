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

/**
 * \file   aw_qrcode__reader.h
 *
 * \internal
 * \par modification history:
 * - 1.00 15-11-10  dcf, first implementation.
 * \endinternal
 */
 
#ifndef __AW_QRCODE_READER_H_
#define __AW_QRCODE_READER_H_


#include <zxing/LuminanceSource.h>

class ImageReaderSource : public zxing::LuminanceSource {

public:
  static zxing::Ref<LuminanceSource> create(unsigned char *data, int width, int height);

  ImageReaderSource(unsigned char *image_, int width, int height, int comps_);

  zxing::ArrayRef<char> getRow(int y, zxing::ArrayRef<char> row) const;

  zxing::ArrayRef<char> getMatrix() const;


private:
    typedef LuminanceSource Super;
    unsigned char* image;
    int comps;
};

#endif /* __IMAGE_READER_SOURCE_H_ */
