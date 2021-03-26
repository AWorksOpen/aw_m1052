/*日志函数的简单定义*/

#ifndef _ZLG_LOG_H
#define _ZLG_LOG_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_INFO(...) \
{\
  printf(__VA_ARGS__); \
  printf("\n"); \
}

#define LOG_WARN(...) \
{\
  printf("WARN:  %s L#%d ", __func__, __LINE__); \
  printf(__VA_ARGS__); \
  printf("\n"); \
}

#define LOG_ERROR(...) \
{\
  printf("ERROR:  %s L#%d ", __func__, __LINE__); \
  printf(__VA_ARGS__); \
  printf("\n"); \
}

#define LOG_FATAL(...) \
{\
  printf("FATAL:  %s L#%d ", __func__, __LINE__); \
  printf(__VA_ARGS__); \
  printf("\n"); \
  exit(1); \
}

#ifdef __cplusplus
}
#endif

#endif //_ZLG_LOG_H