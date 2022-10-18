/**
 * @file button_thread.c
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

#include "button_thread.h"

#include "demo_bsp.h"
#include "demo_impl.h"   // TODO: refactor for transport layer
#include "definitions.h"
#include "user_button.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

// *****************************************************************************
// Private types and definitions

// *****************************************************************************
// Private (static) storage

extern TaskHandle_t xBUTTON_THREAD_Tasks;  // defined in tasks.c

// *****************************************************************************
// Private (static, forward) declarations

// *****************************************************************************
// Public code

void BUTTON_THREAD_Initialize ( void ) {
    // Nothing yet.
}

void BUTTON_THREAD_Tasks ( void ) {
  user_button_t user_button;

  // block here until ISR wakes us, then encode and transmit button state.
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  user_button.is_pressed = demo_bsp_button_is_pressed();
  user_button.timestamp = demo_bsp_timestamp();

  demo_bsp_xmt_reset();
  user_button_encode(&user_button, demo_jems_instance());
  demo_bsp_xmt();
}

void BUTTON_THREAD_Isr(uintptr_t ctx) {
  (void)ctx;
  // Arrive here when button changes state: wake the BUTTON_THREAD
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(xBUTTON_THREAD_Tasks, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// *****************************************************************************
// Private (static) code

// *****************************************************************************
// End of file
