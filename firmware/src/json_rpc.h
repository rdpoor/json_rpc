/**
 * @file json_rpc.h
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
 */

/**
 * @brief A collection of functions for the json_rpc demo.
 */

#ifndef _JSON_RPC_H_
#define _JSON_RPC_H_

// *****************************************************************************
// Includes

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// *****************************************************************************
// C++ compatibility

#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
// Public types and definitions

// *****************************************************************************
// Public declarations

/**
 * @brief One-time initialization of the json_rpc module.
 *
 * NOTE: this may be called repeatedly: it only takes effect on the first call.
 */
void json_rpc_init(void);

/**
 * @brief: Read a line from a serial port.
 *
 * Reads byte at a time read, blocking if needed, returning when a line
 * terminator seen or buf is full.  Always returns a null-terminated string.
 */
const char *json_rpc_get_msg(char *buf, size_t buflen);

/**
 * @brief Parse a JSON RPC formatted msg, then call the local function.
 */
void json_rpc_parse_and_call(const char *json_rpc);

/**
 * @brief Read the local state of the user button, return true if pressed.
 */
bool json_rpc_button_read(void);

/**
 * @brief Construct a JSON RPC message advertising the button state.
 */
const char *json_rpc_construct_button_state_msg(char *buf,
                                                size_t buflen,
                                                bool pressed);

/**
 * @brief Read the light sensor, returning a value between 0.0 and 1.0.
 */
float json_rpc_light_sensor_read(void);

/**
 * @brief Construct a JSON RPC message advertising the light sensor state.
 */
const char *json_rpc_construct_light_sensor_state_msg(char *buf,
                                                      size_t buflen,
                                                      float level);

/**
 * @brief Send a JSON RPC message via the serial port.
 */
void json_rpc_xmit_msg(const char *json_msg);

/**
 * @brief Set the PWM LED to a brightness between 0.0 and 1.0
 */
void json_rpc_pwm_led_set(float level);

/**
 * @brief Set the regular LED on or off
 */
void json_rpc_led_set(bool on);

// *****************************************************************************
// End of file

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _JSON_RPC_H_ */
