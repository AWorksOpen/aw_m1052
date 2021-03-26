/*********************************************Copyright (c)***********************************************
**                                Guangzhou ZLG MCU Technology Co., Ltd.
**
**                                        http://www.zlgmcu.com
**
**      广州周立功单片机科技有限公司所提供的所有服务内容旨在协助客户加速产品的研发进度，在服务过程中所提供
**  的任何程序、文档、测试结果、方案、支持等资料和信息，都仅供参考，客户有权不使用或自行参考修改，本公司不
**  提供任何的完整性、可靠性等保证，若在客户使用过程中因任何原因造成的特别的、偶然的或间接的损失，本公司不
**  承担任何责任。
**                                                                        ——广州周立功单片机科技有限公司
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               httpd_structs.h
** Last modified date:      2015-05-12
** Last version:            V1.0
** Description:             const strings for http server application.
**
**--------------------------------------------------------------------------------------------------------
** Create by:               chenminghao
** Create date:             2015-05-12
** Version:                 V1.0
** Description:
**--------------------------------------------------------------------------------------------------------
** Modifie by:
** Modifie date:
** Version:
** Description:
*********************************************************************************************************/
/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *         Simon Goldschmidt
 *
 */

#ifndef __HTTPD_STRUCTS_H__
#define __HTTPD_STRUCTS_H__

#include "httpd.h"

/** This string is passed in the HTTP header as "Server: " */
#ifndef HTTPD_SERVER_AGENT
#define HTTPD_SERVER_AGENT "lwIP/1.4.1 (enhanced by cmheia https://blog.dabria.net)"
#endif

/** Set this to 1 if you want to include code that creates HTTP headers
 * at runtime. Default is off: HTTP headers are then created statically
 * by the makefsdata tool. Static headers mean smaller code size, but
 * the (readonly) fsdata will grow a bit as every file includes the HTTP
 * header. */
#ifndef LWIP_HTTPD_DYNAMIC_HEADERS
#define LWIP_HTTPD_DYNAMIC_HEADERS 0
#endif


#if LWIP_HTTPD_DYNAMIC_HEADERS
/** This struct is used for a list of HTTP header strings for various
 * filename extensions. */
typedef struct
{
  const char *extension;
  int headerIndex;
} tHTTPHeader;

/** A list of strings used in HTTP headers */
static const char * const g_psHTTPHeaderStrings[] =
{
 "Content-Encoding: gzip\r\nContent-type: text/html\r\n\r\n",
 "Content-type: text/html\r\nExpires: Fri, 10 Apr 2015 14:00:00 GMT\r\nPragma: no-cache\r\n\r\n",
 "Content-type: image/gif\r\n\r\n",
 "Content-type: image/png\r\n\r\n",
 "Content-type: image/jpeg\r\n\r\n",
 "Content-type: image/bmp\r\n\r\n",
 "Content-type: image/x-icon\r\n\r\n",
 "Content-type: application/octet-stream\r\n\r\n",
 "Content-Encoding: gzip\r\nContent-type: application/x-javascript\r\n\r\n",
 "Content-type: application/x-javascript\r\n\r\n",
 "Content-Encoding: gzip\r\nContent-type: text/css\r\n\r\n",
 "Content-type: image/svg+xml\r\n\r\n",
 "Content-type: application/x-shockwave-flash\r\n\r\n",
 "Content-Encoding: gzip\r\nContent-type: text/xml\r\n\r\n",
 "Content-type: text/plain\r\n\r\n",
 "HTTP/1.0 200 OK\r\n",
#if LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC
 "HTTP/1.0 401 Unauthorized\r\n",
#endif /* LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC */
 "HTTP/1.0 404 File not found\r\n",
 "HTTP/1.0 400 Bad Request\r\n",
 "HTTP/1.0 501 Not Implemented\r\n",
 "HTTP/1.1 200 OK\r\n",
 "HTTP/1.1 404 File not found\r\n",
 "HTTP/1.1 400 Bad Request\r\n",
 "HTTP/1.1 501 Not Implemented\r\n",
#if LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC
 "Authorization: ",
#endif /* LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC */
 "Content-Length: ",
 "Connection: Close\r\n",
 "Connection: keep-alive\r\n",
 "Server: "HTTPD_SERVER_AGENT"\r\n",
 "\r\n<html><body><h2>404: The requested file cannot be found.</h2></body></html>\r\n",
#if LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC
 "\r\n<html><head><title>Error</title></head><body><h1>401 Unauthorized</h1></body></html>",
#endif /* LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC */
};

/* Indexes into the g_psHTTPHeaderStrings array */
enum {
  HTTP_HDR_HTML                   , /* text/html */
  HTTP_HDR_SSI                    , /* text/html Expires... */
  HTTP_HDR_GIF                    , /* image/gif */
  HTTP_HDR_PNG                    , /* image/png */
  HTTP_HDR_JPG                    , /* image/jpeg */
  HTTP_HDR_BMP                    , /* image/bmp */
  HTTP_HDR_ICO                    , /* image/x-icon */
  HTTP_HDR_APP                    , /* application/octet-stream */
  HTTP_HDR_JS                     , /* application/x-javascript */
  HTTP_HDR_RA                     , /* application/x-javascript */
  HTTP_HDR_CSS                    , /* text/css */
  HTTP_HDR_SVG                    , /* image/svg+xml */
  HTTP_HDR_SWF                    , /* application/x-shockwave-flash */
  HTTP_HDR_XML                    , /* text/xml */
  HTTP_HDR_DEFAULT_TYPE           , /* text/plain */
  HTTP_HDR_OK                     , /* 200 OK */
#if LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC
  HTTP_HDR_UNAUTHORIZED           , /* 401 Unauthorized */
#endif /* LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC */
  HTTP_HDR_NOT_FOUND              , /* 404 File not found */
  HTTP_HDR_BAD_REQUEST            , /* 400 Bad request */
  HTTP_HDR_NOT_IMPL               , /* 501 Not Implemented */
  HTTP_HDR_OK_11                  , /* 200 OK */
  HTTP_HDR_NOT_FOUND_11           , /* 404 File not found */
  HTTP_HDR_BAD_REQUEST_11         , /* 400 Bad request */
  HTTP_HDR_NOT_IMPL_11            , /* 501 Not Implemented */
#if LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC
  HTTP_HDR_AUTHORIZATION          , /* Authenticate needed */
#endif /* LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC */
  HTTP_HDR_CONTENT_LENGTH         , /* Content-Length: (HTTP 1.1)*/
  HTTP_HDR_CONN_CLOSE             , /* Connection: Close (HTTP 1.1) */
  HTTP_HDR_CONN_KEEPALIVE         , /* Connection: keep-alive (HTTP 1.1) */
  HTTP_HDR_SERVER                 , /* Server: HTTPD_SERVER_AGENT */
  DEFAULT_404_HTML                , /* default 404 body */
#if LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC
  DEFAULT_401_HTML                , /* default 401 body */
#endif /* LWIP_HTTPD_SUPPORT_AUTHENTICATION_BASIC */
} HTTP_HDR_CODE;

/** A list of extension-to-HTTP header strings */
const static tHTTPHeader g_psHTTPHeaders[] =
{
 { "html"   ,  HTTP_HDR_HTML      },
 { "htm"    ,  HTTP_HDR_HTML      },
 { "shtml"  ,  HTTP_HDR_SSI       },
 { "shtm"   ,  HTTP_HDR_SSI       },
 { "ssi"    ,  HTTP_HDR_SSI       },
 { "gif"    ,  HTTP_HDR_GIF       },
 { "png"    ,  HTTP_HDR_PNG       },
 { "jpg"    ,  HTTP_HDR_JPG       },
 { "bmp"    ,  HTTP_HDR_BMP       },
 { "ico"    ,  HTTP_HDR_ICO       },
 { "class"  ,  HTTP_HDR_APP       },
 { "cls"    ,  HTTP_HDR_APP       },
 { "js"     ,  HTTP_HDR_JS        },
 { "ram"    ,  HTTP_HDR_RA        },
 { "css"    ,  HTTP_HDR_CSS       },
 { "svg"    ,  HTTP_HDR_SVG       },
 { "swf"    ,  HTTP_HDR_SWF       },
 { "xml"    ,  HTTP_HDR_XML       },
 { "xsl"    ,  HTTP_HDR_XML       }
};

#define NUM_HTTP_HEADERS (sizeof(g_psHTTPHeaders) / sizeof(tHTTPHeader))

#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */

#if LWIP_HTTPD_SSI
static const char * const g_pcSSIExtensions[] = {
  ".shtml", ".shtm", ".ssi", ".xml"
};
#define NUM_SHTML_EXTENSIONS (sizeof(g_pcSSIExtensions) / sizeof(const char *))
#endif /* LWIP_HTTPD_SSI */

#endif /* __HTTPD_STRUCTS_H__ */
