/* Copyright JS Foundation and other contributors, http://js.foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "aworks.h"
#include "aw_time.h"
#include "aw_vdebug.h"

#include "jerryscript-port.h"
#include "jerryscript-port-default.h"

/**
 * Dummy function to get the time zone adjustment.
 *
 * @return 0
 */
double
jerry_port_get_local_time_zone_adjustment (double unix_ms, bool is_utc)
{
  /* We live in UTC. */
  return 0;
} /* jerry_port_get_local_time_zone_adjustment */


/**
 * Implementation of jerry_port_get_current_time.
 *
 * @return current timer's counter value in milliseconds
 */
double
jerry_port_get_current_time (void)
{
    int64_t ms;

    aw_timespec_t timespec;
    aw_timespec_get(&timespec);
    ms = timespec.tv_nsec / 1000000;
    AW_INFOF((" %d ms \n", ms));       // ¥Ú”°∫¡√Î

    return (double) ms;
} /* jerry_port_get_current_time */

