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

#include <stdlib.h>
#include "aworks.h"
#include "aw_assert.h"

#include "jerryscript-port.h"
#include "jerryscript-port-default.h"

/**
 * Provide fatal message implementation for the engine.
 */
void jerry_port_fatal (jerry_fatal_code_t code)
{
    jerry_port_log (JERRY_LOG_LEVEL_ERROR, "Jerry Fatal Error!\n");
    aw_assert(AW_FALSE);
} /* jerry_port_fatal */
