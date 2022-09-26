/**
 * @file json_rpc.c
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

#include "json_rpc.h"

#include "button_thread.h"
#include "definitions.h"
#include "jsmn.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// *****************************************************************************
// Private types and definitions

#define JSMN_TOKEN_COUNT 10 // max # of json tokens expected in a message

/**
 * @brief Low order 32 bits of the processor id.
 */
#define PROCESSOR_ID3 (0x00806018)

// *****************************************************************************
// Private (static) storage

static jsmntok_t s_jsmn_tokens[JSMN_TOKEN_COUNT];
static jsmn_parser s_jsmn_parser;
static DRV_HANDLE s_usart_handle;
bool static s_is_initialized = false;

// *****************************************************************************
// Private (static, forward) declarations

static void light_sensor_state(float value);
static void button_state(bool value);
static float lerp(float x, float x0, float x1, float y0, float y1);
static float clamp(float x, float xlo, float xhi);
static uint32_t processor_id(void);

// *****************************************************************************
// Public code

void json_rpc_init(void) {
  if (s_is_initialized == false) {

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
    s_is_initialized = true;
  }
}

const char *json_rpc_get_msg(char *buf, size_t buflen) {
  // Perform a byte at a time read, blocking if needed, returning when a line
  // terminator seen or buf is full.  Always returns a null-terminated string.
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

    } else if (*p == '\r') {
      // silently ignore carriage return

    } else if (*p == '\n') {
      // found end of line.
      break;

    } else {
      p += 1;
      available -= 1;
    }
  }

  *p = '\0'; // always null terminate
  printf("\n%08lx << `%s`", processor_id(), buf);
  return buf;
}

void json_rpc_parse_and_call(const char *json_rpc) {
  int count;
  jsmntok_t *fn_token, *arg_token;
  const char *fn_str, *arg_str, *match_str;
  char *terminator;

  jsmn_init(&s_jsmn_parser);

  count = jsmn_parse(&s_jsmn_parser,
                     json_rpc,
                     strlen(json_rpc),
                     s_jsmn_tokens,
                     JSMN_TOKEN_COUNT);

  // perhaps the most fragile, sloppy parser this side of the Mississippi
  if (count != 6) {
    // wrong # of args
    return;
  }

  fn_token = &s_jsmn_tokens[2];
  fn_str = &json_rpc[fn_token->start];
  arg_token = &s_jsmn_tokens[5];
  arg_str = &json_rpc[arg_token->start];

  match_str = "light_sensor_state";
  if (strncmp(fn_str, match_str, strlen(match_str)) == 0) {
    float value = strtof(arg_str, &terminator);
    if (*terminator != ']') {
      // unrecognized argument
      return;
    }
    // call the fn
    light_sensor_state(value);
    return;
  }

  match_str = "button_state";
  if (strncmp(fn_str, match_str, strlen(match_str)) == 0) {
    if (strncmp(arg_str, "true]", strlen("true]")) == 0) {
      button_state(true);
    } else if (strncmp(arg_str, "false]", strlen("false]")) == 0) {
      button_state(false);
    } else {
      // unrecognized argument
      return;
    }

  } else {
    // fn name not recognized.
    return;
  }
}

bool json_rpc_button_read(void) { return !BUTTON_Get(); }

const char *json_rpc_construct_button_state_msg(char *buf,
                                                size_t buflen,
                                                bool pressed) {
  snprintf(buf,
           buflen,
           "{\"fn\":\"button_state\", \"args\":[%s]}\r\n",
           pressed ? "true" : "false");
  return buf;
}

float json_rpc_light_sensor_read(void) {
  ADC1_ConversionStart();
  while (!ADC1_ConversionStatusGet()) {
    // ADC must be polled.  Let other tasks have a chance...
    vTaskDelay(1);
  }
  uint16_t raw_value = ADC1_ConversionResultGet();
  float result = lerp(raw_value, 3700, 100, 0.0, 1.0);
  result = clamp(result, 0.0, 1.0);
  printf("\nadc raw_value = %d, mapped = %f", raw_value, result);
  return result;
}

const char *json_rpc_construct_light_sensor_state_msg(char *buf,
                                                      size_t buflen,
                                                      float level) {
  snprintf(
      buf, buflen, "{\"fn\":\"light_sensor_state\", \"args\":[%f]}\r\n", level);
  return buf;
}

// NOTE: json_msg MUST be null terminated
void json_rpc_xmit_msg(const char *json_msg) {
  printf("\n%08lx >> `%s`", processor_id(), json_msg); // debugging
  DRV_USART_WriteBuffer(s_usart_handle, (void *)json_msg, strlen(json_msg));
}

void json_rpc_pwm_led_set(float level) {
  uint16_t period = level * (1<<16);
  printf("\nset pwm level = %f, period=0x%04x", level, period);
  TC4_Compare16bitMatch0Set(period);
}

void json_rpc_led_set(bool on) {
  // NB: Low level means "on"
  printf("\nset LED %s", on ? "on" : "off");
  if (on) {
    LED_Clear();
  } else {
    LED_Set();
  }
}

// *****************************************************************************
// Private (static) code

static void light_sensor_state(float value) { json_rpc_pwm_led_set(value); }

static void button_state(bool value) { json_rpc_led_set(value); }

static float lerp(float x, float x0, float x1, float y0, float y1) {
  return y0 + (x-x0)*(y1-y0)/(x1-x0);
}

static float clamp(float x, float xlo, float xhi) {
  if (x < xlo) {
    return xlo;
  } else if (x > xhi) {
    return xhi;
  } else {
    return x;
  }
}

static uint32_t processor_id(void) {
  return *(uint32_t *)PROCESSOR_ID3;
}

// *****************************************************************************
// End of file
