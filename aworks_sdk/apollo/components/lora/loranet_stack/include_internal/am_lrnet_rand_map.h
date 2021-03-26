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
 * \file
 * \brief   LoRaNet Pool Module
 *
 * \internal
 * \par modification history:
 * - 18-01-31 may, first implementation.
 * \endinternal
 */


#ifndef __AM_LRNET_RAND_MAP_H
#define __AM_LRNET_RAND_MAP_H

/*
 * demo
 * #define  CNT  213
 *  struct am_lrnet_rand_map  rand_map;
 *  uint32_t  map[AM_LRNET_RAND_MAP_SIZE(CNT)];
 *  int  i = 0;
 *  uint32_t  data;
 *  uint32_t tick = 0;
 *
 *  am_lrnet_rand_map_init(&rand_map, CNT, &map, am_lrnet_random, (void *)32);
 *
 *  tick = am_lrnet_systick_get();
 *  for (i = 0; i < CNT; i++) {
 *      am_lrnet_rand_map_data_get(&rand_map, &data);
 *      AM_DBG_INFO("rand: %d\n", data);
 *  }
 *  AM_DBG_INFO("\n\ntick: %d\n\n", am_lrnet_systick_get() - tick);
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef  uint32_t (*pfunc_am_rand_t)(void *p_arg);

struct am_lrnet_rand_map {
    uint32_t         cnt;
    uint32_t         i_num;
    uint32_t        *p_rand_map;
    pfunc_am_rand_t  rand_func;
    void            *p_rand_arg;
};

#define  AM_LRNET_RAND_MAP_SIZE(cnt)   ((AM_ROUND_UP(cnt, 32))/32)

int  am_lrnet_rand_map_init(struct am_lrnet_rand_map *p_rand_map,
                            uint32_t                  cnt,
                            uint32_t                 *p_bit_map,
                            pfunc_am_rand_t           func,
                            void                     *p_rand_arg);

int  am_lrnet_rand_map_data_get(struct am_lrnet_rand_map *p_rand_map,
                                uint32_t                 *p_data);

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_RAND_MAP_H */

/* end of file */
