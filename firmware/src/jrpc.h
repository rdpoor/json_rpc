/**
 * @file jrpc_runtime.h
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
  * @brief Runtime support for the JSON RPC framework
  */

#ifndef _JRPC_H_
#define _JRPC_H_

// *****************************************************************************
// Includes

#include "jsmn.h"
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

#define JSMN_TOKEN_COUNT 20 // max # of json tokens expected in a message

typedef struct {
    jsmn_parser parser;
    jsmntok_t tokens[JSMN_TOKEN_COUNT];
    const char *json;
    int token_count;
} jrpc_t;

// *****************************************************************************
// Public declarations

/**
 * @brief Return a pointer to a singleton jrpc.
 *
 * NOTE: If there's ever a need for multiple instances, replace this with a
 * call to jrpc_init(jrpc_t *jrpc).
 */
jrpc_t *jrpc_instance(void);

/**
 * @brief Parse the given JSON string.
 */
jrpc_t *jrpc_parse(jrpc_t *jrpc, const char *json, size_t json_len);

/**
 * @brief Return true if the parsed JSON matches the RPC format.
 */
bool jrpc_is_rpc(jrpc_t *jrpc);

/**
 * @brief Return the number of parsed JSON tokens.
 */
int jrpc_token_count(jrpc_t *jrpc);

/**
 * @brief Return true iff the Nth token matches the expected type.
 */
bool jrpc_type_matches(jrpc_t *jrpc, int token_pos, jsmntype_t expected);

/**
 * @brief Return true iff the Nth token is a string and matches the expected.
 */
bool jrpc_string_matches(jrpc_t *jrpc, int token_pos, const char *expected);

/**
 * @brief Parse the Nth token as an integer.  Return true on success.
 *
 * @param jrpc The jrpc runtime object.
 * @param token_pos The token for parsing.
 * @param expected The string to be matched.
 * @param Pointer to the uint64_t to receive the parsed value.
 * @return true if the Nth token is a number and if it parses properly.
 */
bool jrpc_parse_integer(jrpc_t *jrpc, int token_pos, int64_t *result);

/**
 * @brief Parse the Nth token as a double float.  Return true on success.
 *
 * @param jrpc The jrpc runtime object.
 * @param token_pos The token for parsing.
 * @param expected The string to be matched.
 * @param Pointer to the double to receive the parsed value.
 * @return true if the Nth token is a number and if it parses properly.
 */
bool jrpc_parse_double(jrpc_t *jrpc, int token_pos, double *result);

/**
 * @brief Parse the Nth token as a boolean.  Return true on success.
 *
 * @param jrpc The jrpc runtime object.
 * @param token_pos The token for parsing.
 * @param expected The string to be matched.
 * @param Pointer to the bool to receive the parsed value.
 * @return true if the Nth token is exactly "true" or "false".
 */
bool jrpc_parse_bool(jrpc_t *jrpc, int token_pos, bool *result);


// *****************************************************************************
// End of file

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _JRPC_H_ */
