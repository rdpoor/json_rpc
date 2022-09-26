/**
 * @file sensor_task.c
 *
 * MIT License
 *
 * Copyright (c) 2022 PRO1 IAQ, INC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

// *****************************************************************************
// Includes

#include "sensor_thread.h"

#include "FreeRTOS.h"
#include "json_rpc.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

// *****************************************************************************
// Private types and definitions

#define MAX_MSG_LEN 100
#define SENSOR_THREAD_SAMPLING_RATE_IN_MSEC 1000

// *****************************************************************************
// Private (static) storage

static char s_msg_buf[MAX_MSG_LEN];

// *****************************************************************************
// Private (static, forward) declarations

// *****************************************************************************
// Public code

void SENSOR_THREAD_Initialize ( void )
{
  json_rpc_init();
}

void SENSOR_THREAD_Tasks ( void )
{
  // Block until it is time to read light level
  vTaskDelay(SENSOR_THREAD_SAMPLING_RATE_IN_MSEC/portTICK_PERIOD_MS);
  float value = json_rpc_light_sensor_read();
  const char *json_msg = json_rpc_construct_light_sensor_state_msg(s_msg_buf,
                                                                   MAX_MSG_LEN,
                                                                   value);
  json_rpc_xmit_msg(json_msg);
}

// *****************************************************************************
// Private (static) code

// *****************************************************************************
// End of file
