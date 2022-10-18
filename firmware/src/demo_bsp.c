/**
 * @file demo_bsp.c
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

#include "demo_bsp.h"

#include "demo_utils.h"
#include "definitions.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// *****************************************************************************
// Private types and definitions

/**
 * @brief Low order 32 bits of the processor id.
 */
#define PROCESSOR_ID3 (0x00806018)

#define MAX_XMT_LEN 100

// *****************************************************************************
// Private (static) storage

static DRV_HANDLE s_usart_handle;

static char s_xmt_buf[MAX_XMT_LEN];

static int s_xmt_idx;

// *****************************************************************************
// Private (static, forward) declarations

// *****************************************************************************
// Public code

void demo_bsp_init(void) {
  // Call BUTTON_THREAD_Isr when the user button changes state.
  EIC_InterruptEnable(EIC_PIN_15);
  EIC_CallbackRegister(EIC_PIN_15, BUTTON_THREAD_Isr, (uintptr_t)0);

  // Enable ADC for light level sensor
  ADC1_Enable();

  // Enable Timer / Counter for PWM control of the LED
  TC4_Compare16bitMatch0Set(0);
  TC4_CompareStart();

  // Open serial channel for JSON RPC messages
  s_usart_handle = DRV_USART_Open(DRV_USART_INDEX_0, 0);
}

uint32_t demo_bsp_timestamp(void) {
  return 0;  // TODO: find a timer.
}

bool demo_bsp_button_is_pressed(void) {
  return !BUTTON_Get();
}

float demo_bsp_light_level(void) {
  ADC1_ConversionStart();
  while (!ADC1_ConversionStatusGet()) {
    // ADC must be polled.  Let other tasks have a chance...
    vTaskDelay(1);
  }
  uint16_t raw_value = ADC1_ConversionResultGet();
  float result = demo_utils_lerp(raw_value, 3700, 100, 0.0, 1.0);
  result = demo_utils_clamp(result, 0.0, 1.0);
  printf("\nadc raw_value = %d, mapped = %f", raw_value, result);
  return result;
}

uint32_t demo_bsp_processor_id(void) {
    return *(uint32_t *)PROCESSOR_ID3;
}

void demo_bsp_pwm_led_set(float level) {
  uint16_t period = level * (1<<16);
  printf("\nset pwm level = %f, period=0x%04x", level, period);
  TC4_Compare16bitMatch0Set(period);
}

void demo_bsp_led_set(bool on) {
  printf("\nset LED %s", on ? "on" : "off");
  // NB: active low
  if (on) {
    LED_Clear();
  } else {
    LED_Set();
  }
}

const char *demo_bsp_gets(char *buf, size_t buflen) {
  // Perform a byte at a time read, BLOCKING IF NEEDED, returning when a null is
  // seen or buf is full.  Always returns a null-terminated string.
  if ((buf == NULL) || (buflen == 0)) {
    return NULL;
  }

  size_t available = buflen - 1; // always leave room for null terminator
  char *p = buf;                 // where next char gets written

  while (true) {
    if (available == 0) {
      // buffer is full
      break;

    } else if (DRV_USART_ReadBuffer(s_usart_handle, p, 1) == false) {
      // read failed: quit now.
      return NULL;

    } else if (*p == '\0') {
      // found null terminator.  Quit now.
      break;

    } else {
      p += 1;
      available -= 1;
    }
  }

  *p = '\0'; // always null terminate
  printf("\n%08lx << `%s`", demo_bsp_processor_id(), buf);
  return buf;
}

void demo_bsp_xmt_reset(void) {
  s_xmt_idx = 0;
}

void demo_bsp_putc(char ch) {
  if (s_xmt_idx < MAX_XMT_LEN-2) {  // always leave room for '\n\0'
    s_xmt_buf[s_xmt_idx++] = ch;
  }
}

void demo_bsp_xmt(void) {
  // Terminate xmt buffer with newline and a null before sending via serial
  s_xmt_buf[s_xmt_idx++] = '\n';
  s_xmt_buf[s_xmt_idx++] = '\0';
  printf("\n%08lx >> `%s`", demo_bsp_processor_id(), s_xmt_buf); // debugging
  DRV_USART_WriteBuffer(s_usart_handle, s_xmt_buf, s_xmt_idx);
  s_xmt_idx = 0;
}

// *****************************************************************************
// Private (static) code

// *****************************************************************************
// End of file
