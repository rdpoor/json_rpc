/**
 * @file user_button.c
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

#include "user_button.h"
#include "user_button_process.h"
#include "jems.h"
#include "jrpc.h"
#include <stdbool.h>
#include <stdint.h>

// *****************************************************************************
// Private types and definitions

// *****************************************************************************
// Private (static) storage

// *****************************************************************************
// Private (static, forward) declarations

// *****************************************************************************
// Public code

void user_button_encode(user_button_t *state, jems_t *jems) {
  jems_reset(jems);
  jems_object_open(jems);
  jems_string(jems, "fn");
  jems_string(jems, "user_button");
  jems_string(jems, "args");
  jems_object_open(jems);
  jems_string(jems, "timestamp");
  jems_integer(jems, state->timestamp);
  jems_string(jems, "is_pressed");
  jems_bool(jems, state->is_pressed);
  jems_object_close(jems);
  jems_object_close(jems);
}

bool user_button_decode(jrpc_t *jrpc) {
  user_button_t state;
  if (jrpc_token_count(jrpc) != 9) {   // (4 args * 2) + 5 = 9
    return false;
  } else if (!jrpc_string_matches(jrpc, 2, "user_button")) {
    return false;
  } else if (!jrpc_string_matches(jrpc, 5, "timestamp")) {
    return false;
  } else if (!jrpc_parse_integer(jrpc, 6, &state.timestamp)) {
    return false;
  } else if (!jrpc_string_matches(jrpc, 7, "is_pressed")) {
    return false;
  } else if (!jrpc_parse_bool(jrpc, 8, &state.is_pressed)) {
    return false;
  } else {
    user_button_process(&state);
    return true;
  }
}

// *****************************************************************************
// Private (static) code

// *****************************************************************************
// End of file
