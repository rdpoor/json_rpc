/**
 * @file jrpc.c
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

// *****************************************************************************
// Includes

#include "jrpc.h"
#include "jsmn.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// *****************************************************************************
// Private types and definitions

// *****************************************************************************
// Private (static) storage

static jrpc_t s_jrpc;

// *****************************************************************************
// Private (static, forward) declarations

static jsmntok_t *token_ref(jrpc_t *jrpc, int position);

static jsmntok_t *token_ref_type(jrpc_t *jrpc, int position, jsmntype_t expected);

static bool string_match(jrpc_t *jrpc, jsmntok_t *token, const char *expected);

// *****************************************************************************
// Public code

jrpc_t *jrpc_instance(void) { return &s_jrpc; }

jrpc_t *jrpc_parse(jrpc_t *jrpc, const char *json, size_t json_len) {
    jsmn_init(&jrpc->parser);
    jrpc->token_count = jsmn_parse(&jrpc->parser, json, json_len,
                                   jrpc->tokens, JSMN_TOKEN_COUNT);
    jrpc->json = json;
    return jrpc;
}

bool jrpc_is_rpc(jrpc_t *jrpc) {
    if (jrpc->token_count < 5) {
        return false;
    } else if (!jrpc_type_matches(jrpc, 0, JSMN_OBJECT)) {
        return false;
    } else if (!jrpc_string_matches(jrpc, 1, "fn")) {
        return false;
    } else if (!jrpc_string_matches(jrpc, 3, "args")) {
        return false;
    } else if (!jrpc_type_matches(jrpc, 4, JSMN_OBJECT)) {
        return false;
    } else {
        return true;
    }
}

int jrpc_token_count(jrpc_t *jrpc) { return jrpc->token_count; }

bool jrpc_type_matches(jrpc_t *jrpc, int token_pos, jsmntype_t expected) {
    return token_ref_type(jrpc, token_pos, expected) ? true : false;
}

bool jrpc_string_matches(jrpc_t *jrpc, int token_pos, const char *expected) {
    jsmntok_t *token = token_ref_type(jrpc, token_pos, JSMN_STRING);
    return string_match(jrpc, token, expected);
}

bool jrpc_parse_integer(jrpc_t *jrpc, int token_pos, int64_t *result) {
    char *endptr;

    jsmntok_t *token = token_ref_type(jrpc, token_pos, JSMN_PRIMITIVE);
    if (token == NULL) {
        return false;
    } else {
        const char *str = &jrpc->json[token->start];
        *result = strtol(str, &endptr, 10);
        // TODO: we could check that endptr is whitespace, comma, ']' or '}'
        return str != endptr;
    }
}

bool jrpc_parse_double(jrpc_t *jrpc, int token_pos, double *result) {
    char *endptr;

    jsmntok_t *token = token_ref_type(jrpc, token_pos, JSMN_PRIMITIVE);
    if (token == NULL) {
        return false;
    } else {
        const char *str = &jrpc->json[token->start];
        *result = strtod(str, &endptr);
        // TODO: we could check that endptr is whitespace, comma, ']' or '}'
        return str != endptr;
    }
}

bool jrpc_parse_bool(jrpc_t *jrpc, int token_pos, bool *result) {
    jsmntok_t *token = token_ref_type(jrpc, token_pos, JSMN_PRIMITIVE);
    if (string_match(jrpc, token, "true")) {
        // Matched true
        *result = true;
        return true;
    } else if (string_match(jrpc, token, "false")) {
        // Matched false
        *result = false;
        return true;
    } else {
        // Did not match
        return false;
    }
}

// *****************************************************************************
// Private (static) code

static jsmntok_t *token_ref(jrpc_t *jrpc, int position) {
    if (position >= jrpc->token_count) {
        return NULL;
    } else {
        return &jrpc->tokens[position];
    }
}

static jsmntok_t *token_ref_type(jrpc_t *jrpc, int position, jsmntype_t expected) {
    jsmntok_t *token = token_ref(jrpc, position);
    if (token == NULL) {
        return NULL;
    } else if (token->type != expected) {
        return NULL;
    } else {
        return token;
    }
}

static bool string_match(jrpc_t *jrpc, jsmntok_t *token, const char *expected) {
    if (token == NULL) {
        return false;
    } else {
        return strncmp(&jrpc->json[token->start], expected, strlen(expected)) ==
               0;
    }
}

// *****************************************************************************
// End of file
