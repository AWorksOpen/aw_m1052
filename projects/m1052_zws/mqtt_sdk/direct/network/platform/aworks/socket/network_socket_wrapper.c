/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>
#include <timer_platform.h>
#include <network_interface.h>
//#include <net/if.h>

#include "zlg_iot_error.h"
#include "zlg_iot_log.h"
#include "network_interface.h"
#include "network_socket_platform.h"

#include "aworks_platform_common.h"

#if !LWIP_POSIX_SOCKETS_IO_NAMES
#define close(s)              lwip_close(s)
#define fcntl(a,b,c)          lwip_fcntl(a,b,c)
#endif /* LWIP_POSIX_SOCKETS_IO_NAMES */

/* This defines the value of the debug buffer that gets allocated.
 * The value can be altered based on memory constraints
 */

static int socket_connect (const char *host, int port, const char *interface_name) {
  int sock;
  int connected = 0;
  struct sockaddr_in s_in;
  memset(&s_in, 0, sizeof(s_in));

  s_in.sin_family = AF_INET;
  struct hostent *h = gethostbyname(host);
  if(h == NULL) {
    printf("gethostbyname failed: %s" ENDL, host);
    return -1;
  }

  memcpy(&s_in.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  s_in.sin_port = htons(port);

  printf("connecting... " AW_PRINT_IP4ADDR_FMT ":%u" ENDL, AW_PRINT_IP4ADDR_VAL(s_in.sin_addr.s_addr), ntohs(s_in.sin_port));
  if((sock = (int) socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    printf("socket error" ENDL);
    return -1;
  }

//  if (interface_name && strlen(interface_name) > 0) {
//    struct ifreq ifr;
//    int ret;
//    memset(&ifr, 0x00, sizeof(ifr));
//    snprintf(ifr.ifr_name, IFNAMSIZ, interface_name);
//    ret = setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));
//    printf("ret=%d" ENDL, ret);
//  }

  fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
  connected = connect(sock, (struct sockaddr *) &s_in, sizeof(struct sockaddr_in));
  if(connected != 0) {
    if(errno != AW_EINPROGRESS) {
      printf("connect errno :%d != %d" ENDL, errno, AW_EINPROGRESS);
      goto error_exit;
    } else {
      struct timeval tm = {5, 0};
      fd_set wset, rset;
      FD_ZERO(&wset);
      FD_ZERO(&rset);
      FD_SET(sock, &wset);
      FD_SET(sock, &rset);
      int res = select(sock + 1, &rset, &wset, NULL, &tm);
      if(res < 0) {
        printf("network error in connect" ENDL);
        goto error_exit;
      } else if(res == 0) {
        printf("connect time out" ENDL);
        goto error_exit;
      } else if(1 == res) {
        if(FD_ISSET(sock, &wset)) {
          printf("connect succeed." ENDL);
          fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) & ~O_NONBLOCK);
        } else {
          printf("other error when select: errno=%d" ENDL, errno);
          goto error_exit;
        }
      }
    }
  }

  return sock;

error_exit:
  close(sock);
  return -1;
}

/*
 * This is a function to do further verification if needed on the cert received
 */

static void init_param (Network *pNetwork, char *pDestinationURL, uint16_t destinationPort, char *pIFRNName, uint32_t timeout_ms) {
  pNetwork->tlsConnectParams.DestinationPort = destinationPort;
  pNetwork->tlsConnectParams.pDestinationURL = pDestinationURL;
  pNetwork->tlsConnectParams.timeout_ms = timeout_ms;
  pNetwork->tlsConnectParams.pIFRNName = pIFRNName;
}

static IoT_Error_t iot_socket_is_connected (Network *pNetwork) {
  (void) pNetwork;
  /* Use this to add implementation which can check for physical layer disconnect */
  return NETWORK_PHYSICAL_LAYER_CONNECTED;
  /* todo get link status */
#if 0
  extern bool_t wifi_is_connected (void);
  return (FALSE == wifi_is_connected()) ? NETWORK_PHYSICAL_LAYER_DISCONNECTED : NETWORK_PHYSICAL_LAYER_CONNECTED;
#endif
}

static IoT_Error_t iot_socket_connect (Network *pNetwork, TLSConnectParams *params) {
  uint16_t port = 0;
  const char *host = NULL;
  struct timeval timeout;
  SocketDataParams *s = (SocketDataParams *) pNetwork->tlsDataParams;

  if(NULL != params) {
    init_param(pNetwork, params->pDestinationURL, params->DestinationPort, params->pIFRNName, params->timeout_ms);
  }

  host = pNetwork->tlsConnectParams.pDestinationURL;
  port = pNetwork->tlsConnectParams.DestinationPort;
  timeout.tv_sec = pNetwork->tlsConnectParams.timeout_ms / 1000;
  timeout.tv_usec = (pNetwork->tlsConnectParams.timeout_ms % 1000) * 1000;

  s->sock = socket_connect(host, port, pNetwork->tlsConnectParams.pIFRNName);

  if(s->sock >= 0) {
    //struct timeval timeout = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
    //int ret = setsockopt(s->sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    int ret = setsockopt(s->sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    printf("ret=%d" ENDL, ret);

    /*if (pNetwork->tlsConnectParams.pIFRNName) {
      struct ifreq ifr;
      memset(&ifr, 0x00, sizeof(ifr));
      snprintf(ifr.ifr_name, IFNAMSIZ, pNetwork->tlsConnectParams.pIFRNName);
      ret = setsockopt(s->sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));
      printf("ret=%d" ENDL, ret);
    }*/
  }

  return s->sock >= 0 ? SUCCESS : TCP_CONNECTION_ERROR;
}

static IoT_Error_t iot_socket_write(Network *pNetwork, unsigned char *pMsg, size_t len, Timer *timer, size_t *written_len) {
  size_t written_so_far;
  bool isErrorFlag = false;
  int frags = 0, ret = 0;
  SocketDataParams *s = (SocketDataParams *) pNetwork->tlsDataParams;

  for(written_so_far = 0, frags = 0; written_so_far < len && !has_timer_expired(timer); written_so_far += ret, frags++) {
    while(!has_timer_expired(timer) && (ret = send(s->sock, pMsg + written_so_far, len - written_so_far, 0)) <= 0) {
      if(ret < 0) {
        isErrorFlag = true;
        aw_mdelay(100);
      }
    }
    if(isErrorFlag) {
      break;
    }
  }

  *written_len = written_so_far;

  if(isErrorFlag) {
    return NETWORK_SSL_WRITE_ERROR;
  } else if(has_timer_expired(timer) && written_so_far != len) {
    return NETWORK_SSL_WRITE_TIMEOUT_ERROR;
  }

  return SUCCESS;
}

static IoT_Error_t iot_socket_read_any(Network *pNetwork, unsigned char *pMsg, size_t len, size_t *read_len) {
  int ret = 0;
  size_t rxLen = 0;
  SocketDataParams *s = (SocketDataParams *)pNetwork->tlsDataParams;

  while(ret <= 0) {
    ret = recv(s->sock, pMsg, len, 0);
    if(ret > 0) {
      rxLen += ret;
      pMsg += ret;
      len -= ret;
      break;
    } else if(errno != EAGAIN) {
      return NETWORK_SSL_READ_ERROR;
    } else {
      printf("iot_socket_read:%d\n", ret);
      continue; 
    }
  }

  *read_len = ret;

  return ret >= 0 ? SUCCESS : NETWORK_SSL_READ_ERROR;
}

static IoT_Error_t iot_socket_read_remaining_len(Network *pNetwork, unsigned char *pMsg, size_t len, size_t *read_len) {
  int ret = 0;
  size_t rxLen = 0;
  uint32_t timeout_ms = 3000;

  Timer timer;
  init_timer(&timer);
  countdown_ms(&timer, timeout_ms);
  SocketDataParams *s = (SocketDataParams *)pNetwork->tlsDataParams;

  while(len > 0 && !has_timer_expired(&timer)) {
    ret = recv(s->sock, pMsg, len, 0);
    if(ret > 0) {
      rxLen += ret;
      pMsg += ret;
      len -= ret;
    } else {
      if(errno == EAGAIN) {
        continue;
      }
      break;
    }
  }

  *read_len = rxLen;

  return len > 0 ? NETWORK_SSL_READ_ERROR : SUCCESS;
}

static IoT_Error_t iot_socket_read(Network *pNetwork, unsigned char *pMsg, size_t len, Timer *timer, size_t *read_len) {
  int ret;
  int timeout = 0;
  size_t rxLen = 0;
  SocketDataParams *s = (SocketDataParams *) pNetwork->tlsDataParams;

  while(len > 0) {
    ret = recv(s->sock, pMsg, len, 0);
    if(ret > 0) {
      rxLen += ret;
      pMsg += ret;
      len -= ret;
    } else if(ret == 0 || (errno != EINTR && errno != EAGAIN)) {
      return NETWORK_SSL_READ_ERROR;
    }

    if(has_timer_expired(timer)) {
      timeout = 1;
      break;
    }
  }

  /** if read uncomplete, try to read remaining length in 3s*/
  if(rxLen > 0 && len > 0 && timeout == 1) {
    size_t rem_len = 0;
    iot_socket_read_remaining_len(pNetwork, pMsg, len, &rem_len);
    rxLen += rem_len;
    len -= rem_len;
  }

  *read_len = rxLen;
  if(len == 0) {
    return SUCCESS;
  }

  if(rxLen == 0) {
    return NETWORK_SSL_NOTHING_TO_READ;
  } else {
    return NETWORK_SSL_READ_TIMEOUT_ERROR;
  }
}

static IoT_Error_t iot_socket_disconnect (Network *pNetwork) {
  SocketDataParams *s = (SocketDataParams *) pNetwork->tlsDataParams;

  if (s->sock >= 0) {
    close(s->sock);
    s->sock = -1;
  }

  return SUCCESS;
}

static IoT_Error_t iot_socket_destroy (Network *pNetwork) {
  SocketDataParams *s = (SocketDataParams *) pNetwork->tlsDataParams;

  if(s->sock >= 0) {
    close(s->sock);
    s->sock = -1;
  }

  return SUCCESS;
}

static int iot_socket_get_fd (Network *pNetwork) {
  SocketDataParams *s = (SocketDataParams *) pNetwork->tlsDataParams;

  return s->sock;
}

IoT_Error_t iot_socket_init (Network *pNetwork, const char *host, uint16_t port, const char *interface_name, uint32_t timeout_ms) {
  SocketDataParams *tlsDataParams = (SocketDataParams *) calloc(1, sizeof(SocketDataParams));
  init_param(pNetwork, (char *) host, port, interface_name, timeout_ms);

  pNetwork->getSocket = iot_socket_get_fd;
  pNetwork->connect = iot_socket_connect;
  pNetwork->read = iot_socket_read;
  pNetwork->readAny = iot_socket_read_any;
  pNetwork->write = iot_socket_write;
  pNetwork->disconnect = iot_socket_disconnect;
  pNetwork->isConnected = iot_socket_is_connected;
  pNetwork->destroy = iot_socket_destroy;

  tlsDataParams->sock = -1;
  pNetwork->tlsDataParams = tlsDataParams;

  return SUCCESS;
}

#ifdef __cplusplus
}
#endif
