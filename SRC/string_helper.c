#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "string_helper.h"

#define CHAR_COUNT_CONSTANT 100

char *str_append(char *original, unsigned argcount, ...) {
    va_list args;
    va_start(args, argcount);

    unsigned length = strlen(original);
    unsigned size = (length / CHAR_COUNT_CONSTANT + 1) * CHAR_COUNT_CONSTANT;
    char *str = malloc(size);
    memcpy(str, original, length);

    while(argcount-- > 0) {
      char *arg = va_arg(args, char *);

      while(*arg) {
        if(length == size) str = realloc(str, size += CHAR_COUNT_CONSTANT);

        str[length++] = *arg++;
      }
    }

    va_end(args);

    str = realloc(str, length + 1);
    str[length] = 0;

    return str;
}

char *str_decimal(unsigned value) {
    unsigned length = snprintf(0x0, 0, "%u", value);
    char *str = malloc(length + 1);
    sprintf(str, "%u", value);
    str[length] = 0;

    return str;
}

void str_shuffle(char *original, unsigned long long seed) {
  unsigned length = strlen(original);

  struct LLItem { char value; struct LLItem *next; };
  struct LinkedList { unsigned length; struct LLItem *first; } list;
  list.length = length;
  list.first = malloc(sizeof(struct LLItem));

  struct LLItem *item = list.first;
  for(unsigned i = 0; i < length; ++i) {
    item->value = original[i];
    item->next = malloc(sizeof(struct LLItem));
    item = item->next;
  }

  free(item);

  srand(seed);

  unsigned cindex = 0;

  struct LLItem *last;

  while(list.length-- > 0) {
    unsigned index = rand() % (!list.length ? 1 : list.length);
    
    last = 0x0;
    item = list.first;
    for(; index > 0; --index) { last = item; item = item->next; }

    original[cindex++] = item->value;
    if(last) last->next = item->next;
    else list.first = item->next;

    free(item);
  }
}

int str_equal(char *a, char *b) {
  while(*a && *b) if(*a++ != *b++) return 0;

  return (*a | *b) == 0;
}

int str_nequal(char *a, char *b, int n) {
  while(n-- > 0) if(*a++ != *b++) return 0;

  return 1;
}