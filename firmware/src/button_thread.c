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

#include "FreeRTOS.h"
#include "json_rpc.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

// *****************************************************************************
// Private types and definitions

#define MAX_MSG_LEN 100

// *****************************************************************************
// Private (static) storage

static char s_msg_buf[MAX_MSG_LEN];

extern TaskHandle_t xBUTTON_THREAD_Tasks;  // defined in tasks.c

// *****************************************************************************
// Private (static, forward) declarations

// *****************************************************************************
// Public code

void BUTTON_THREAD_Initialize ( void )
{
  json_rpc_init();
}

void BUTTON_THREAD_Tasks ( void )
{
  // block here until ISR wakes us, then read button state and generate a
  // JSON RPC message.
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  bool button_state = json_rpc_button_read();
  const char *json_msg = json_rpc_construct_button_state_msg(s_msg_buf,
                                                             MAX_MSG_LEN,
                                                             button_state);
  json_rpc_xmit_msg(json_msg);
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
