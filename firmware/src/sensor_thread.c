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

#include "demo_bsp.h"
#include "demo_impl.h"   // TODO: refactor for transport layer
#include "light_sensor.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// *****************************************************************************
// Private types and definitions

#define MAX_MSG_LEN 100
#define SENSOR_THREAD_SAMPLING_RATE_IN_MSEC 1000

// *****************************************************************************
// Private (static) storage

// *****************************************************************************
// Private (static, forward) declarations

// *****************************************************************************
// Public code

void SENSOR_THREAD_Initialize ( void ) {
  // Nothing at present
}

void SENSOR_THREAD_Tasks ( void )
{
    light_sensor_t light_sensor;
    
  // Block until it is time to read light level
  vTaskDelay(SENSOR_THREAD_SAMPLING_RATE_IN_MSEC/portTICK_PERIOD_MS);
  light_sensor.intensity = demo_bsp_light_level();
  light_sensor.timestamp = demo_bsp_timestamp();

  demo_bsp_xmt_reset();
  light_sensor_encode(&light_sensor, demo_jems_instance());
  demo_bsp_xmt();
}

// *****************************************************************************
// Private (static) code

// *****************************************************************************
// End of file
