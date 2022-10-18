// gcc -Wall -o jsmn_test jsmn_test.c jsmn.c && ./jsmn_test && rm ./jsmn_test

#include "jsmn.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define JSMN_TOKEN_COUNT 10 // max # of json tokens expected in a message

const char *str1 = "{\"fn\":\"light_sensor\", \"args\":{\"timestamp\": 987654321, \"value\":1.23456}}\n";

static jsmntok_t s_jsmn_tokens[JSMN_TOKEN_COUNT];
static jsmn_parser s_jsmn_parser;

void explain_token(int i, jsmntok_t *token, const char *str) {
  printf("%d: type=%d \'%.*s\'\n",
         i,
         token->type,
         token->end - token->start,
         &str[token->start]);
}

void explain_tokens(jsmntok_t *tokens, int count, const char *str) {
  if (count < 0) {
    printf("jsmn_parse error %d\n", count);
  } else {
    for (int i = 0; i < count; i++) {
      explain_token(i, &tokens[i], str);
    }
  }
}

int main(void) {
  jsmn_init(&s_jsmn_parser);
  int count = jsmn_parse(
      &s_jsmn_parser, str1, strlen(str1), s_jsmn_tokens, JSMN_TOKEN_COUNT);
  explain_tokens(s_jsmn_tokens, count, str1);
}
