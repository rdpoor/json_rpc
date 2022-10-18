/**
 * @file listen_thread.c
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

#include "listen_thread.h"

#include "demo_bsp.h"
#include "demo_impl.h"
#include <string.h>

// *****************************************************************************
// Private types and definitions

#define MAX_MSG_LEN 100

// *****************************************************************************
// Private (static) storage

static char s_msg_buf[MAX_MSG_LEN];

// *****************************************************************************
// Private (static, forward) declarations

// *****************************************************************************
// Public code

void LISTEN_THREAD_Initialize(void) {
    // nothing yet.
}

void LISTEN_THREAD_Tasks(void) {
  // block until a JSON RPC message is available.
  const char *msg = demo_bsp_gets(s_msg_buf, MAX_MSG_LEN);
  // decode the message and dispatch
  if (msg) {
      demo_decode(s_msg_buf, strlen(s_msg_buf));
  } else {
    // parser error...
    asm("nop");
  }
}

// *****************************************************************************
// Private (static) code

// *****************************************************************************
// End of file
